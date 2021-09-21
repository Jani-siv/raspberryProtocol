/*
 * receiver.h
 *
 *  Created on: Sep 16, 2021
 *      Author: Gyy
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_
#include "frame.h"
#include <iostream>
#include <string>
#include <stdbool.h>
#include <cstring>
#include <map>
#include "bitoperation.h"
#include "local_gpio.h"
#include "frame.h"
class receiver {
private:
	char address[15];
	bool dataNeedProcess = false;
	char* dataptr = nullptr;
	bool dataPackedNum = 0;
	std::map<int,char*> dataStorage;
public:
	receiver(char* address);
	virtual ~receiver(){};
	void transmission(gpio receiv);
	int calclen(const char* len);
	void copyDataToMemory(int len, char* data, int packedNum);
	int calcNum(char* num);
	dataFrame frame;
};

#endif /* RECEIVER_H_ */
