/*
 * transmitter.cpp
 *
 *  Created on: Sep 18, 2021
 *      Author: Gyy
 */

#include "transmitter.h"

transmitter::transmitter(gpio* dataline)
{
	this->dataline = dataline;
}
void transmitter::initTransmission(char* filename)
{
	//open file in binarymode
this->file.open(filename, std::ifstream::binary);
if (!this->file.is_open())
	{
			std::cerr<<"File not open"<<std::endl;
	}
std::cout<<"file open normally"<<std::endl;
this->size = this->getFileSize();
std::cout<<"size:"<<this->size<<std::endl;
this->calculateAmountOfPackets();
std::cout<<"amounts of full packets: "<<this->amountOfPackets<<std::endl;
std::cout<<"last packet size: "<<this->lastPacketSize<<std::endl;
//set packets id and size in map
for (long i=0; i <= this->amountOfPackets; i++)
{
	if (i == this->amountOfPackets)
	{
		this->datalen[i] = this->lastPacketSize;
	}
	else
	{
		this->datalen[i] = this->size;
	}
}
}

void transmitter::calculateAmountOfPackets()
{
if (this->size % BLOCKSIZE != 0)
{
long carry = this->size%BLOCKSIZE;
this->amountOfPackets= this->size / BLOCKSIZE;
this->lastPacketSize = carry;
this->lastNotFullPack=true;
}
else {
	this->amountOfPackets = this->size / BLOCKSIZE;
}
}

void transmitter::getPosition()
{
	if(this->file.is_open())
	{
		int length = this->file.tellg();
		this->position = length;
	}
	else
		{
			std::cerr<<"File not open"<<std::endl;
		}
}
long transmitter::getFileSize()
{
	int length = 0;
	if(this->file.is_open())
	{
		this->file.seekg(0,this->file.end);
		length = this->file.tellg();
		this->file.seekg(0,this->file.beg);
	}
	else
	{
		std::cerr<<"File not open"<<std::endl;
		return -1;
	}
	return length;
}

void transmitter::clearData(char *data, int len)
{
	memset(data,'0',len);
}

char* transmitter::reserveMem(int len)
{
	char *ptr;
	ptr =(char*)malloc(len*sizeof(char));
	return ptr;
}

void transmitter::setPTP(char* dest, char* sour,dataFrame *frame)
{
	frame->head.destination=dest;
	frame->head.source=sour;
}

void transmitter::setSYN(dataFrame *frame)
{
		frame->head.asyn[0] = 0xAA;  //data is 10101010
}

void transmitter::setStartBits(dataFrame *frame)
{
	frame->head.dataStart[0]=0xFF;
	frame->head.endOfTransmission[0]=0x00;
}

void transmitter::setDataLen(dataFrame * frame, long len)
{
char * lenptr = nullptr;
lenptr = frame->head.datalen;
convertLongToChar(len,lenptr,4);
}

void transmitter::createCRC(dataFrame * frame)
{

char *ptr = nullptr;
long currentSize;
auto it = this->datalen.find(this->position-1);
if (it == this->datalen.end())
{
	currentSize = this->lastPacketSize;
}
else
{
currentSize = it->second;
}
std::cout<<"current size in create CRC: "<<currentSize<<std::endl;
	ptr =makeCrc(frame->data.dataptr,currentSize);
	frame->CRC[0] = ptr[0];
	frame->CRC[1] = ptr[1];
	ptr = nullptr;
}

void transmitter::endOfData(dataFrame* frame)
{
	frame->head.endOfTransmission[0]=0xFF;
}

void transmitter::printFrame(dataFrame *f)
{
	std::cout<<f->dataId<<std::endl;
}
void transmitter::addDataId(dataFrame* frame,int i)
{
	this->convertLongToChar(i,frame->dataId,4);
}

//overflow possibility

void transmitter::sendPacket(dataFrame* frame, gpio *transfer, int type, int useExternalCrc)
{
	if (type == 0)
	{
this->nextData(frame);
	}
this->createChunkAndSend(frame,transfer,useExternalCrc);
//only for transmitter
if (type == 0)
{
	std::chrono::duration<double> diff;
	auto start = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();
	//wait answer
	char *addr;
	addr = this->address;
receiver res(addr,nullptr,this->dataline);
res.timeouttime= 3;
res.waitingAnswer = true;
int next = 0;
std::cout<<"start waiting answer"<<std::endl;
int breakWaiting = 0;
while (next != 1)
{ if (breakWaiting > 5)
{break;}
	std::cout<<"waiting answer"<<std::endl;
	next = res.transmission();
	if (next == 1)
	{
		std::cout<<"message type from answer: "<<int(res.frame.head.messageType[0])<<std::endl;
		std::cout<<"answering data from answer: "<<int(res.frame.data.data[0])<<std::endl;
	}
	if (res.frame.head.messageType[0] == 0x00)
	{
		std::cout<<"answer data: "<<int(res.frame.data.data[1])<<std::endl;
		std::cout<<"exit from waiting"<<std::endl;
		next = 1;
		break;
	}
	breakWaiting++;
if (next == -1)
{
	std::cout<<"sending packet again"<<std::endl;
	createChunkAndSend(frame,transfer,useExternalCrc);
	if (res.frame.data.data[0] == 0xFF)
	{
		next = -1;
	}
}
if (next == 0)
{
	std::cerr<<"error skipping vality check"<<std::endl;
}

}
std::cout<<"exiting from sendPacket"<<std::endl;
this->position++;

}
}
void transmitter::createChunkAndSend(dataFrame* frame, gpio *transfer, int useExternalCrc)
{
	if (useExternalCrc != 1)
	{
		createCRC(frame);
	}
	//syn
//std::cout<<"syn"<<std::endl;
transfer->writeData(frame->head.asyn[0]);

//std::cout<<"data start"<<std::endl;
transfer->writeData(frame->head.dataStart[0]);
//std::cout<<"data id"<<std::endl;
for (int i = 0; i < 4; i++)
	{
transfer->writeData(frame->dataId[i]);
	}
//std::cout<<"source"<<frame->head.source<<std::endl;
for (int i = 0; i < 15; i++)
	{
transfer->writeData(frame->head.source[i]);
	}
//std::cout<<"destination"<<frame->head.destination<<std::endl;
for (int i = 0; i < 15; i++)
	{
transfer->writeData(frame->head.destination[i]);
	}
//std::cout<<"message type"<<std::endl;
for (int i = 0; i < 4; i++)
	{
transfer->writeData(frame->head.messageType[i]);
	}
//std::cout<<"total packs"<<std::endl;
for (int i = 0; i < 4; i++)
	{
transfer->writeData(frame->head.totalpacks[i]);
	}
//std::cout<<"datalen"<<std::endl;
for (int i = 0; i < 4; i++)
	{
transfer->writeData(frame->head.datalen[i]);
	}
long amountBytes = 0;
if (this->position < this->amountOfPackets)
{
	amountBytes = this->size;
}
else {
	amountBytes = this->lastPacketSize;
}
//std::cout<<"data: "<<frame->data.dataptr<<std::endl;
for (int i = 0; i < amountBytes; i++)
	{
transfer->writeData(frame->data.dataptr[i]);
	}
if (useExternalCrc == 1)
{
	for (int i = 0; i < 1; i++)
		{
	transfer->writeData(frame->data.dataptr[i]);
		}
}
std::cout<<"CRC"<<std::endl;
for (int i= 0; i < 2; i++)
{
	transfer->writeData(frame->CRC[i]);
}

//std::cout<<"end byte"<<std::endl;
transfer->writeData(frame->head.endOfTransmission[0]);

}


void transmitter::dataType(dataFrame* frame,unsigned char* dataType)
{
	// 8 bit 0x00 transmit 0xFF to answer
	frame->head.messageType[0] = dataType[0];
}

void transmitter::nextData(dataFrame* frame)
{
	char buffer[BLOCKSIZE];
this->file.read(buffer,BLOCKSIZE*(this->position));
//copy packets in frame
if (this->position < this->amountOfPackets)
{
	for (int i=0; i < BLOCKSIZE; i++)
	{
frame->data.dataptr[i] =buffer[i];
	}
}
else
{
	for (int i=0; i < this->lastPacketSize; i++)
	{
frame->data.dataptr[i] =buffer[i];
	}

}
//set packet id
this->convertLongToChar(this->position,frame->dataId,4);
this->position++;

}

void transmitter::initFrame(char* source, char* destination, char* filename, dataFrame* frame)
{
	if (filename != nullptr)
	{
	this->initTransmission(filename);
	for (int i=0; i < 15; i++)
	{
		this->address[i] = source[i];
	}
	}
	//add packed id
	this->addDataId(frame,this->position);
	//data lenght
	long currentSize;
	auto it = this->datalen.find(this->position-1);
	if (it != this->datalen.end())
	{
	currentSize = it->second;
	}
	else
	{
		currentSize = BLOCKSIZE;
	}
	std::cout<<"transmitter currentSize"<<currentSize<<std::endl;
	this->setDataLen(frame, currentSize);
	//asyn
	this->setSYN(frame);
	//start and end bit to continue
	this->setStartBits(frame);
	//source & destination to header
	this->setPTP(destination,source,frame);
	//set total amount of packets to header
	this->convertLongToChar(this->amountOfPackets,frame->head.totalpacks,4);
	//set data type
	unsigned char datatype[1];
	datatype[0] = 0xFF;
	this->dataType(frame,datatype);

}
