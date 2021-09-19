/*
 * transmitter.cpp
 *
 *  Created on: Sep 18, 2021
 *      Author: Gyy
 */

#include "transmitter.h"


void transmitter::initTransmission(char* filename)
{
	//open file in binarymode
this->file.open(filename, std::ifstream::binary);
if (!this->file.is_open())
	{
			std::cerr<<"File not open"<<std::endl;
	}
this->size = this->getFileSize();
this->calculateAmountOfPackets();
}

void transmitter::calculateAmountOfPackets()
{
if (this->size % BLOCKSIZE != 0)
{
long carry = this->size/BLOCKSIZE;
this->amountOfPackets= (this->size-carry) / BLOCKSIZE + 1;
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
	long size=0;
	if(this->file.is_open())
	{
		this->file.seekg(0,this->file.end);
		int length = this->file.tellg();
		this->size = length;
		this->file.seekg(0,this->file.beg);
	}
	else
	{
		std::cerr<<"File not open"<<std::endl;
		return -1;
	}
	return size;
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

void transmitter::sendPacket(dataFrame* frame)
{
this->nextData(frame);
}

void transmitter::dataType(dataFrame* frame,unsigned char* dataType)
{
	// 8 bit 0x00 transmit 0xFF to answer
	frame->head.messageType[0] = dataType[0];
}

void transmitter::nextData(dataFrame* frame)
{
	char buffer[BLOCKSIZE];
this->file.read(buffer,BLOCKSIZE*(this->position+1));
//copy packets in frame
if (this->position != this->amountOfPackets)
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

std::cout<<frame->data.dataptr<<std::endl;
//set packet id
this->convertLongToChar(this->position,frame->dataId,4);
this->position++;

}

void transmitter::initFrame(char* source, char* destination, char* filename, dataFrame* frame)
{

	this->initTransmission(filename);
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
