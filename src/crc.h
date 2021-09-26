/*
 * crc.h
 *
 *  Created on: Sep 25, 2021
 *      Author: Gyy
 */

#ifndef CRC_H_
#define CRC_H_
#include <stdlib.h>
#include <stdio.h>
#define DIVIDER 1024
#define SIZE 2
char* makeCrc(char * data, unsigned int size);
int checkCrc(char * data, unsigned int size, char* crcCarry);




#endif /* CRC_H_ */
