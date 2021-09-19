/*
 * bitoperation.cpp
 *
 *  Created on: Sep 19, 2021
 *      Author: Gyy
 */

#include "bitoperation.h"

bitoperation::bitoperation() {
	// TODO Auto-generated constructor stub

}

bitoperation::~bitoperation() {
	// TODO Auto-generated destructor stub
}

long bitoperation::converCharToLong(int bytes,unsigned char* data)
{
long num=0;
//overflow warning
for (int i=bytes-1; i > -1; i--)
{
	num += data[i];
	//skip last bit shift
	if (i != 0)
	{
	num = (num << 8);
	}
}
return num;
}

void bitoperation::convertLongToChar(long num, unsigned char* data, int size)
{
//for (int i = 0; i < size; i++)
int i =0;
while (num > 0)
	{
	data[i] = num & 0xFF;
	num = (num >> 8);
	i++;
}
}
