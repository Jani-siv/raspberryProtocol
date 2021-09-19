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
class receiver {
private:
	std::string deviceIP = "222.222.222.222";
	bool dataNeedProcess = false;
	char* dataptr = nullptr;
	bool dataPackedNum = 0;
	std::map<int,char*> dataStorage;
public:
	receiver(){};
	virtual ~receiver(){};
	void transmission(dataFrame* frame);
	int calclen(const char* len);
	void copyDataToMemory(int len, char* data, int packedNum);
	int calcNum(char* num);
};

#endif /* RECEIVER_H_ */
