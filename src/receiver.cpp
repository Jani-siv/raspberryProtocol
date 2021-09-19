/*
 * receiver.cpp
 *
 *  Created on: Sep 16, 2021
 *      Author: Gyy
 */

#include "receiver.h"

void receiver::transmission(dataFrame* frame)
{
	//test if address is correct
for (int i = 0; i < 15; i++)
{
	char temp = frame->head.destination[i];
	char temp1 = this->deviceIP[i];
	if (temp != temp1)
	{
		std::cout<<"error: "<<temp<<" != "<<temp1<<std::endl;
		this->dataNeedProcess = false;
		break;
	}
	else
	{
		this->dataNeedProcess = true;
	}
}
if (this->dataNeedProcess == true)
{
//get data len
char datalen[3];
char packedNum[5];
//find cleaner style copy char
datalen[0] =frame->head.datalen[0];
datalen[1] =frame->head.datalen[1];
datalen[2] =frame->head.datalen[2];
int len = calclen(datalen);
int num = calcNum(packedNum);
copyDataToMemory(len,frame->data.dataptr, num);
}
}

int receiver::calclen(const char* len)
{
	int length=0;
	length = len[0];
	length += len[1]*10;
	length += len[2]*100;
return length;
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
