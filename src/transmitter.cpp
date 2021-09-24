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

void transmitter::setDataLen(dataFrame * frame, int len)
{
if (len < 999)
{
		frame->head.datalen[0] = char(len%10);
		len -= frame->head.datalen[0];
		len /=10;

		frame->head.datalen[1] = char(len%10);
		len -= frame->head.datalen[1];
		len /=10;

		frame->head.datalen[2] = char(len%10);
}
else
{
	std::cout<<"too long length"<<std::endl;
}
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

void transmitter::sendPacket(dataFrame* frame, gpio *transfer, int type)
{
	if (type == 0)
	{
this->nextData(frame);
	}
this->createChunkAndSend(frame,transfer);

if (type == 0)
{
	std::chrono::duration<double> diff;
	auto start = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();
	//wait answer
	char *addr;
	addr = this->address;
receiver res(addr,nullptr,this->dataline);
int next = 0;
std::cout<<"start waiting answer"<<std::endl;
while (next != 1)
{
	std::cout<<"waiting answer"<<std::endl;
	next = res.transmission();
if (next == -1)
{
	std::cout<<"sending packet again"<<std::endl;
	createChunkAndSend(frame,transfer);
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
void transmitter::createChunkAndSend(dataFrame* frame, gpio *transfer)
{
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
	}
	//add packed id
	this->addDataId(frame,this->position);
	//data lenght
	this->setDataLen(frame, BLOCKSIZE);
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
