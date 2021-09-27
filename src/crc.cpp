/*
 * crc.cpp
 *
 *  Created on: Sep 25, 2021
 *      Author: Gyy
 */

#include "crc.h"
#include <iostream>
char* makeCrc(char * data, unsigned int size)
{
	static char* ptr = nullptr;
	static char returnValue[SIZE];
	int value=0;
	ptr = (char*)malloc(size);
	for (int i = 0; i < size; i++)
	{
		value += data[i] & 0xFF;
	}
	std::cout<<"data CRC value: "<<value<<std::endl;
	if (value >= DIVIDER)
	{
value %=DIVIDER;
value = DIVIDER - value;
	}
		else
		{
		value = DIVIDER -value;
		std::cout<<"smaller :"<<value<<std::endl;
		}


	std::cout<<"value before shift: "<<value<<std::endl;
	returnValue[0] = value & 0xFF;
	value = (value >> 8);
	returnValue[1] = value & 0xFF;

	std::cout<<"return value 0: "<<int(returnValue[0])<<" return value 1: "<<int(returnValue[1])<<std::endl;

ptr = returnValue;
return ptr;

}

int checkCrc(char * data, unsigned int size, char* crcCarry)
{
	int carry = 0;
	int dataValue = 0;
	//data size
	for (int i=0; i < size; i++)
	{
		dataValue += data[i] & 0xFF;
	}

	//carry
std::cout<<"crc data size: "<<dataValue<<std::endl;

		carry += crcCarry[1] & 0xFF;
		std::cout<<"carry: "<<carry<<" crc carry"<<crcCarry[1]<<std::endl;
		carry = (carry << 8);
		std::cout<<"carry: "<<carry<<" crc carry"<<crcCarry[1]<<std::endl;
		carry += crcCarry[0] & 0xFF;
		std::cout<<"carry: "<<carry<<" crc carry"<<crcCarry[0]<<std::endl;
	std::cout<<"carry in check"<<carry<<std::endl;

	dataValue += carry;
	std::cout<<"datavalue + carry in check: "<<dataValue<<std::endl;
	carry = dataValue%DIVIDER;
	return carry;
}
