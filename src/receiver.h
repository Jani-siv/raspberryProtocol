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
#include "bitoperation.h"
#include "transmitter.h"
#include "crc.h"


class receiver : public bitoperation {
private:
	char answerAddress[15] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	char address[15] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	bool dataNeedProcess = false;
	char* dataptr = nullptr;
	bool dataPackedNum = 0;
	long totalPackets =1;
	long currentPacket = 0;
	std::map<int,char*> dataStorage;
	int setAllToFrame(char* ptr);
	void storeDataToFile();
	char* fileptr = nullptr;
	std::ofstream file;
	void createFile();
	int addPacketToMap();
	void testPacketsInMap();
	long addTotalAmount();
	void updateCurrentPacket();
	void sendAnswer(uint8_t status, uint8_t message);
	gpio *datalines;

public:
	receiver(char* address, char *filename, gpio * datalines);
	virtual ~receiver(){};
	int transmission();
	int calclen(const char* len);
	void copyDataToMemory(int len, char* data, int packedNum);
	int calcNum(char* num);
	int timeouttime =20;
	dataFrame frame;
	bool timeout = false;
	bool waitingAnswer = false;
};

#endif /* RECEIVER_H_ */
