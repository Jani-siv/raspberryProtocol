/*
 * bitoperation.h
 *
 *  Created on: Sep 19, 2021
 *      Author: Gyy
 */

#ifndef BITOPERATION_H_
#define BITOPERATION_H_


class bitoperation{
public:
	bitoperation();
	virtual ~bitoperation();
	long converCharToLong(int bytes, unsigned char* data);
	void convertLongToChar(long num, unsigned char* data, int size);
};

#endif /* BITOPERATION_H_ */
