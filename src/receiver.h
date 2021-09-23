/*
 * receiver.h
 *
 *  Created on: Sep 16, 2021
 *      Author: Gyy
 */

#ifndef RECEIVER_H_
#define RECEIVER_H_
#define DATASTART 0
#define DATAID 0
#define SOURCE 5
#define DESTINATION 20
#define MESSAGETYPE 35
#define TOTALPACKS 39
#define DATALEN 43
#define DATA 47
#define ENDTRANS 97
#include "frame.h"
#include <iostream>
#include <string>
#include <stdbool.h>
#include <cstring>
#include <map>
#include <fstream>
#include "bitoperation.h"
#include "local_gpio.h"
#include "frame.h"


class receiver {
private:
	char answerAddress[15] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	char address[15] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	bool dataNeedProcess = false;
	char* dataptr = nullptr;
	bool dataPackedNum = 0;
	std::map<int,char*> dataStorage;
	void setAllToFrame(char* ptr);
	void storeDataToFile();
	char* fileptr = nullptr;
	std::ofstream file;
	void createFile();
public:
	receiver(char* address, char *filename);
	virtual ~receiver(){};
	void transmission(gpio receiv);
	int calclen(const char* len);
	void copyDataToMemory(int len, char* data, int packedNum);
	int calcNum(char* num);

	dataFrame frame;
};

#endif /* RECEIVER_H_ */
