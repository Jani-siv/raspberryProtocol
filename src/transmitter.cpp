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
this->amountOfPackets++;
std::cout<<"amounts packets: "<<this->amountOfPackets<<std::endl;
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
if (this->lastPacketSize > 2)
{
	this->lastPacketSize -= 2;
}
convertLongToChar(this->amountOfPackets,this->frame.head.totalpacks,4);

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
currentSize = BLOCKSIZE;
}
if (this->position == this->amountOfPackets)
{
	currentSize = this->lastPacketSize;
}
std::cout<<"current size in create CRC: "<<currentSize<<std::endl;
	ptr =makeCrc(frame->data.dataptr,currentSize);
	std::cout<<frame->data.dataptr<<std::endl;
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

int transmitter::sendPacket(dataFrame* frame, gpio *transfer, int type, int useExternalCrc)
{
	if (type == 0)
	{

		convertLongToChar(this->amountOfPackets,frame->head.totalpacks,4);
		std::cout<<"total Amount of packets:"<<this->amountOfPackets<<std::endl;
this->nextData(frame);
	}
this->createChunkAndSend(frame,transfer,useExternalCrc);
//only for transmitter
int breakloop = 0;
if (type == 0)
{
	while (this->position < this->amountOfPackets)
	{
		while (this->waitAnswer(frame,transfer,type) < 0)
		{
			this->createChunkAndSend(frame,transfer,type);
			breakloop++;
			if (breakloop >= 5)
			{
				std::cerr<<"Connection time out no answer.."<<std::endl;
				return -1;
			}
		}
		this->nextData(frame);
		this->createChunkAndSend(frame,transfer,type);

		breakloop = 0;
	}
	this->waitAnswer(frame,transfer,type);

}
return 0;
}

int transmitter::waitAnswer(dataFrame* frame, gpio *transfer, int useExternalCrc)
{
	char *addr;
	addr = this->address;
	receiver res(addr,nullptr,this->dataline);
	res.timeouttime= 7;
	res.waitingAnswer = true;
	int next = 0;
	std::cout<<"start waiting answer"<<std::endl;
	int breakWaiting = 0;
	while (next != 1)
	{
		if (breakWaiting >= 2) {
			std::cout<<"connection timeout sending package again"<<std::endl;
			return -1;
		}
		std::cout<<"waiting answer"<<std::endl;
		next = res.transmission();
		res.timeout = false;

		if (int(res.frame.data.data[0]) == -1)
		{
			std::cout<<"answer data: "<<int(res.frame.data.data[0])<<std::endl;
			std::cout<<"data error in package.. re-sending packets"<<std::endl;
			return -1;
		}
		breakWaiting++;


	if (next == 1)
	{
		std::cout<<"Packet delivered successfully"<<std::endl;
		return 0;
	}
	}

	std::cout<<"exiting from sendPacket"<<std::endl;

return 0;
	}




void transmitter::updateFrame(dataFrame* frame)
{

}

void transmitter::createChunkAndSend(dataFrame* frame, gpio *transfer, int useExternalCrc)
{
	if (useExternalCrc != 1)
	{
		createCRC(frame);
		convertLongToChar(this->amountOfPackets,frame->head.totalpacks,4);

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
	std::cout<<frame->head.totalpacks[i];
transfer->writeData(frame->head.totalpacks[i]);
	}
std::cout<<std::endl;
//only used in answer
if (useExternalCrc == 1)
{
	this->lastPacketSize = 1;
	this->position = this->amountOfPackets;
}

//std::cout<<"datalen"<<std::endl;
if (this->position >= this->amountOfPackets)
{
	convertLongToChar(this->lastPacketSize,frame->head.datalen,4);
}
for (int i = 0; i < 4; i++)
	{
transfer->writeData(frame->head.datalen[i]);
	}
long amountBytes = 0;
if (this->position < this->amountOfPackets)
{
	amountBytes = BLOCKSIZE;
}
else {
	amountBytes = this->lastPacketSize;
}
std::cout<<"bytes to send: "<<amountBytes<<" data len to send: "<<converCharToLong(4,frame->head.datalen)<<std::endl;
//std::cout<<"data: "<<frame->data.dataptr<<std::endl;
for (int i = 0; i < amountBytes; i++)
	{
transfer->writeData(frame->data.dataptr[i]);
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

//copy packets in frame
if (this->position < this->amountOfPackets)
{
	this->file.seekg(this->position*BLOCKSIZE);
	this->file.read(buffer,BLOCKSIZE);
	for (int i=0; i < BLOCKSIZE; i++)
	{
frame->data.dataptr[i] =buffer[i];
	}
	convertLongToChar(BLOCKSIZE,frame->head.datalen,4);
}
else
{
	this->file.seekg(this->position*BLOCKSIZE);
		this->file.read(buffer,this->lastPacketSize);
	for (int i=0; i < this->lastPacketSize; i++)
	{
frame->data.dataptr[i] =buffer[i];
	}
	convertLongToChar(this->lastPacketSize,frame->head.datalen,4);

}
//set packet id
this->convertLongToChar(this->position,frame->dataId,4);
this->position++;
std::cout<<"exiting next data"<<std::endl;
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
