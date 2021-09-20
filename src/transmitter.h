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
#include "frame.h"
#include "bitoperation.h"
#include "local_gpio.h"
#define BLOCKSIZE 50
class transmitter : public bitoperation{
private:
	void clearData(char *data, int len);

	void setPTP(char* dest, char* sour,dataFrame *frame);
	void setSYN(dataFrame *frame);
	void setStartBits(dataFrame *frame);
	void setDataLen(dataFrame * frame, int len);
	void endOfData(dataFrame* frame);
	void nextData(dataFrame* frame);

	void addDataId(dataFrame* frame,int i);

	void dataType(dataFrame* frame,unsigned char* dataType);
	int position=0;
	int size=0;
	long amountOfPackets=0;
	long lastPacketSize;
	bool lastNotFullPack = false;

	void getPosition();
	void getPacketCount();
	long getFileSize();
	void calculateAmountOfPackets();
	void createChunkAndSend(dataFrame* frame, gpio transfer);
	std::ifstream file;
public:
	char* reserveMem(int len);
	void initFrame(char* source, char* destination, char* filename, dataFrame* frame);
	void sendPacket(dataFrame* frame, gpio transfer);
	void initTransmission(char* filename);
	void printFrame(dataFrame *f);
	dataFrame frame;
};

#endif /* TRANSMITTER_H_ */
