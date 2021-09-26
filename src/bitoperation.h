/*
 * bitoperation.h
 *
 *  Created on: Sep 19, 2021
 *      Author: Gyy
 */

#ifndef BITOPERATION_H_
#define BITOPERATION_H_

#include <iostream>
class bitoperation{
public:
	bitoperation();
	virtual ~bitoperation();
	long converCharToLong(int bytes, unsigned char* data);
	void convertLongToChar(long num, unsigned char* data, int size);
	void convertLongToChar(long num, char* data, int size);
	long converCharToLong(int bytes, char* data);
};

#endif /* BITOPERATION_H_ */
