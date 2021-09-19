/*
 * gpio.h
 *
 *  Created on: Sep 17, 2021
 *      Author: Gyy
 */

#ifndef LOCAL_GPIO_H_
#define LOCAL_GPIO_H_
#include <iostream>
#include <fstream>
#include <cstring>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <iostream>
#include <linux/gpio.h>
#include <iostream>
#include <string.h>
#include <chrono>

class gpio {
public:
int createDatalines();
gpio(char* dev, __u32 rx, __u32 tx);
typedef struct timing
{
	long inputspeed;
	double outputspeed;
}timing;
typedef struct devcfg
{
char* dev;
__u32 rx;
__u32 tx;
}devcfg;
void writeData(uint8_t data);
void readData();
void getTiming();
private:
timing busSpeed;
devcfg dev;
struct gpiohandle_data outputdata, inputdata;
struct gpiohandle_request input, output;
};

#endif /* LOCAL_GPIO_H_ */
