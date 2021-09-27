/*
 * receiver.cpp
 *
 *  Created on: Sep 16, 2021
 *      Author: Gyy
 */

#include "receiver.h"

receiver::receiver(char* address, char* filename, gpio* datalines)
{
	this->datalines = datalines;
	for (int i=0; i < 15; i++)
	{
this->address[i] = address[i];
	}
	this->fileptr = filename;
	this->datalines->writeData(0x00);
}
int receiver::setAllToFrame(char* ptr)
{
	//allocate memory for pointers

	for (int i = 0; i < 15; i++)
	{
		this->answerAddress[i] = ptr[SOURCE+i];
	//	this->frame.head.destination[i] = ptr[DESTINATION+i]; check if it's valid
		//std::cout<<this->frame.head.source[i];
	}
	//change address source to destination for answering purpose
	this->frame.head.destination = this->answerAddress;

	for (int i = 0; i < 4; i++)
	{
		this->frame.dataId[i] = ptr[DATAID+i];
		this->frame.head.messageType[i] = ptr[MESSAGETYPE+i];
		this->frame.head.totalpacks[i] = ptr[TOTALPACKS+i];
		this->frame.head.datalen[i] =	ptr[DATALEN+i];
	}


	//get data len
	char* datalenptr;
	datalenptr = this->frame.head.datalen;
	long datalen = converCharToLong(4,datalenptr);
	std::cout<<"data len: "<<datalen<<std::endl;
	this->frame.CRC[0] = ptr[(DATA+datalen)] & 0xFF;
	this->frame.CRC[1] = ptr[(DATA+datalen+1)] & 0xFF;
	this->frame.head.endOfTransmission[0] = ptr[(DATA+datalen+2)];


std::cout<<"crc 0"<<int(this->frame.CRC[0])<<" crc 1 "<<int(this->frame.CRC[1])<<"end transmiss... : "<<int(this->frame.head.endOfTransmission[0])<<std::endl;
	for (int i =0; i < datalen; i++)
	{
		this->frame.data.data[i] = ptr[DATA+i];
		std::cout<<ptr[DATA+i];
	}
	std::cout<<std::endl;
	for(int i=0; i < 15; i++)
	{
	std::cout<<this->frame.head.destination[i];
	}
	std::cout<<std::endl;
	//CRC CHECK
	char* dataptr = nullptr;
	dataptr = this->frame.data.data;
	char *crcptr = nullptr;
	crcptr = this->frame.CRC;
	long crcValue = checkCrc(dataptr,datalen,crcptr);
	if (crcValue == 0)
	{
		std::cout<<"data is valid"<<std::endl;
	}
	else {
		std::cerr<<"data is corrupted: "<<crcValue<<std::endl;
		return -1;
	}
	//store data in map

return 0;
}
int receiver::transmission()
{
	//loop for receiving packet's
while(this->currentPacket < this->totalPackets)
{
	char *temp;
	temp = this->address;
	std::cout<<"address of this device:"<<temp<<std::endl;

	char *ptr = nullptr;
	this->datalines->busSpeed.inputspeed = 0;
	ptr = this->datalines->readData(100,this->timeouttime);
	this->datalines->busSpeed.inputspeed = 0;
	//timeout
	if (ptr == nullptr)
	{
	return -1;
	}
	//datastart bit found
	if (ptr[0]== -1)
	{
		std::cout<<"startbit founded"<<std::endl;
	bool correctAddress = true;

	for (int i= 0; i < 15; i++)
	{
		if (ptr[DESTINATION+i] != this->address[i])
		{
			correctAddress = false;
		}
	}
	if (correctAddress == true)
	{
		uint8_t datatype = ptr[MESSAGETYPE];
		int dataCRC = this->setAllToFrame(ptr);
		std::cout<<"DATA: "<<this->frame.data.dataptr<<std::endl;
		if (this->waitingAnswer == true)
		{
			//break answer waiting loop
			return 1;
		}
	if (dataCRC < 0)
	{
		//don't save message data is corrupted
		datatype = 0x00;
		std::cout<<"Data is corrupted"<<std::endl;
	}
	//here check CRC
	//add packet to map
	//check message type if data 0xff store to file
	//if 0x00 read first byte of data section

	std::cout<<"data type: "<<int(datatype)<<std::endl;
	if (datatype == 0xFF && this->waitingAnswer != true)
	{
		std::cout<<"data type was correct 0xff starting save data"<<std::endl;
		//this section only for saving data
		long tot =0;
		this->addTotalAmount();
		std::cout<<"total packs: "<<this->totalPackets<<std::endl;

		this->updateCurrentPacket();
		int packet = this->addPacketToMap();
		if (packet == 0)
			{
			this->createFile();
			//copy only length of data

			this->file << this->frame.data.data;

			this->file.close();
			memset(this->frame.data.data,'0',sizeof(this->frame.data.data));
			this->currentPacket++;
			//answer to sender
			//all OK
			}
		this->sendAnswer(0x00,0xAA);
	}
	//CRC ERROR
	if (datatype == 0)
	{
		if (this->waitingAnswer == false)
		{
			this->sendAnswer(0x00,-1);
		}
		else {
			//answering mechanism
			return -1;
		}
	}
	}
	//check data crc and respond to sender
	if (correctAddress == false)
	{
		std::cout<<"wrong destination address! discarding packets"<<std::endl;
		for (int i = 0; i < 15; i++)
		{
			std::cout<<ptr[DESTINATION+i];
		}
		std::cout<<std::endl;
	}
	}
}
	std::cout<<"data ready"<<std::endl;
return 0;

}

void receiver::sendAnswer(uint8_t status, uint8_t message)
{
	std::cout<<"sending answer"<<std::endl;
	usleep(200000);
transmitter answer(this->datalines);
dataFrame * frameptr;
frameptr = &this->frame;
frameptr->head.source = this->address;
frameptr->head.messageType[0] = status;
convertLongToChar(1,frameptr->head.totalpacks,4);
memset(frameptr->data.data,'0',sizeof(frame.data.data));
frameptr->data.data[0] = message & 0xFF;

std::cout<<"init frame"<<std::endl;
answer.initFrame(this->frame.head.source, this->frame.head.destination, nullptr,frameptr);
answer.setDataLen(frameptr,1);




//frameptr->data.data[0] = 0xFF;


//CREATE CRC
std::cout<<"message: "<<int(message)<<" status: "<<int(status)<<std::endl;
char* crcptr = nullptr;
char* dataptr = nullptr;
dataptr = frameptr->data.data;
crcptr = makeCrc(dataptr,1);
frameptr->CRC[0] = crcptr[0];
frameptr->CRC[1] = crcptr[1];

std::cout<<"answer crc 0: "<<frameptr->CRC[0]<<" answer crc 1: "<<frameptr->CRC[1]<<std::endl;

std::cout<<"start sending frame"<<std::endl;
answer.sendPacket(frameptr,this->datalines,1,1);
std::cout<<"done"<<std::endl;
}

void receiver::updateCurrentPacket()
{
	this->currentPacket = converCharToLong(4,this->frame.dataId);
	std::cout<<"current packet in function: "<<converCharToLong(4,this->frame.dataId)<<std::endl;
}

long receiver::addTotalAmount()
{
this->totalPackets = converCharToLong(4,this->frame.head.totalpacks);
return converCharToLong(4,this->frame.head.totalpacks);
}

int receiver::addPacketToMap()
{
	long dataId = this->converCharToLong(4,this->frame.dataId);
	auto iterator = dataStorage.find(dataId);
	if (iterator == dataStorage.end())
	{
	this->dataStorage[dataId] = this->frame.data.dataptr;
	return 0;
	}
	else
	{
		std::cerr<<"data id is already in storage"<<std::endl;
		return -1;
	}
	//if data end of transmission is set
	//check all packets are arrived;

}
void receiver::testPacketsInMap()
{

	auto iterator = this->dataStorage.begin();
	long testNum =0;
	while (iterator != this->dataStorage.end())
	{
		if (iterator->first != testNum)
		{
			std::cerr<<"missing packet"<<iterator->second<<std::endl;
		}
		testNum++;
		iterator++;
	}
	if (testNum-1 != this->totalPackets)
	{
		std::cerr<<"packets count doesn't match!!"<<std::endl;
	}
}
void receiver::createFile()
{
//create or append file
this->file.open(this->fileptr,std::ofstream::out | std::ofstream::app);
if (!this->file.is_open())
{
	std::cerr<<"error open file!!"<<std::endl;
}
}

void receiver::copyDataToMemory(int len, char* data,int packedNum)
{
	//test id packed is already in container
	auto it = this->dataStorage.find(packedNum);
	if (it == this->dataStorage.end())
	{

	//reserve memory
	this->dataptr =(char*)malloc(len*sizeof(char));
	//copy to memory
	memcpy(this->dataptr,data,len);
	std::cout<<this->dataptr<<std::endl;
	//add to container
	this->dataStorage[packedNum] = this->dataptr;
	this->dataptr = nullptr;
	for (auto it = this->dataStorage.begin(); it != this->dataStorage.end(); it++)
	{
		std::cout<<it->second<<std::endl;
	}
	this->dataNeedProcess=false;
	}
}

int receiver::calcNum(char* num)
{
	int id;
	id = num[0];
	id = num[1] *10;
	id = num[2] *100;
	id = num[3] *1000;
	id = num[4] *10000;
	return id;
}
