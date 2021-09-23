/*
 * receiver.cpp
 *
 *  Created on: Sep 16, 2021
 *      Author: Gyy
 */

#include "receiver.h"

receiver::receiver(char* address, char* filename)
{
	for (int i=0; i < 15; i++)
	{
this->address[i] = address[i];
	}
	this->fileptr = filename;
}
void receiver::setAllToFrame(char* ptr)
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

	this->frame.head.endOfTransmission[0] = ptr[ENDTRANS];

	for (int i =0; i < 50; i++)
	{
		this->frame.data.data[i] = ptr[DATA+i];
	}
	for(int i=0; i < 15; i++)
	{
	std::cout<<this->frame.head.destination[i];
	}
	std::cout<<std::endl;
	char* data;
	data = this->frame.data.data;
	std::cout<<data<<std::endl;
std::cout<<"end storing"<<std::endl;
}
void receiver::transmission(gpio receiv)
{

	char *ptr = nullptr;
	ptr = receiv.readData(100);
	bool store = true;
	for (int i= 0; i < 15; i++)
	{
		if (ptr[DESTINATION+i] != this->address[i])
		{
			store = false;
		}
	}
	if (store == true)
	{
	this->setAllToFrame(ptr);
	this->createFile();
	this->file << this->frame.data.data;
	this->file.close();
	}
	//check data crc and respond to sender
	if (store == false)
	{
		std::cout<<"wrong destination address! discarding packets"<<std::endl;
	}
	std::cout<<"data ready"<<std::endl;
	std::cout<<"from receiver"<<int(ptr)<<std::endl;


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
