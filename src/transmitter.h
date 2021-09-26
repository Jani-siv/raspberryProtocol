/*
 * transmitter.h
 *
 *  Created on: Sep 18, 2021
 *      Author: Gyy
 */

#ifndef TRANSMITTER_H_
#define TRANSMITTER_H_
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <stdio.h>
#include <fstream>
#include <chrono>
#include "frame.h"
#include "bitoperation.h"
#include "local_gpio.h"
#include "receiver.h"
#include "crc.h"
#include <map>
#define BLOCKSIZE 50
class transmitter : public bitoperation{
private:

	void clearData(char *data, int len);

	void setPTP(char* dest, char* sour,dataFrame *frame);
	void setSYN(dataFrame *frame);
	void setStartBits(dataFrame *frame);

	void endOfData(dataFrame* frame);
	void nextData(dataFrame* frame);
	void createCRC(dataFrame * frame);
	void addDataId(dataFrame* frame,int i);
	std::map<int,int> datalen;
	void dataType(dataFrame* frame,unsigned char* dataType);
	int position=1;
	int size=0;
	char address[15] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	long amountOfPackets=0;
	long lastPacketSize=0;
	bool lastNotFullPack = false;
	void getPosition();
	void getPacketCount();
	long getFileSize();
	void calculateAmountOfPackets();
	void createChunkAndSend(dataFrame* frame, gpio *transfer);
	gpio * dataline;
	std::ifstream file;
public:
	transmitter(gpio* dataline);
	virtual ~transmitter(){};
	void setDataLen(dataFrame * frame, long len);
	char* reserveMem(int len);
	void initFrame(char* source, char* destination, char* filename, dataFrame* frame);
	void sendPacket(dataFrame* frame, gpio *transfer, int type);
	void initTransmission(char* filename);
	void printFrame(dataFrame *f);
	dataFrame frame;
};

#endif /* TRANSMITTER_H_ */
