/*
 * frame.h
 *
 *  Created on: Sep 16, 2021
 *      Author: Gyy
 */

#ifndef FRAME_H_
#define FRAME_H_

typedef struct dataPack{
int datalen = 50;
char *dataptr = data;
char data[50];
}dataPack;
/*change asyn to 1 byte
 *add total amount of packets
 *add data packet size in bytes
 */
typedef struct header{
	char asyn[1];				//sync bits	10101010									1 bytes
	char dataStart[1];			//start bit 11111111									1 bytes
	char* source = nullptr;				//where to send	let's use 	ip 123.123.123.123 	15 bytes
	char* destination = nullptr;		//where it coming from		ip 124.124.124.124 	15 bytes
	char messageType[4];				//long answer / transmission					4 bytes
	unsigned char totalpacks[4];			//total amount of packets	(long)						4 bytes
	char datalen[4];			//length of data			(long)						4 bytes
	char endOfTransmission[1];	//last packed 0xFF is end								1 bytes
}header;

/*
 * dataframe total 100 bytes
 * 50 bytes to header + dataframe
 * 50 bytes to data
 */
typedef struct dataFrame{
	unsigned char* dataId;	//(long)
	char CRC[2];
	header head;
	dataPack data;
}dataFrame;


#endif /* FRAME_H_ */
