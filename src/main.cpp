//============================================================================
// Name        : main.cpp
// Author      : Jani
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++
//============================================================================

#include <iostream>

#include "local_gpio.h"
#include "transmitter.h"
#include "frame.h"
using namespace std;
#define RX 17
#define TX 27

//
// Print a greeting message on standard output and exit.
//
// On embedded platforms this might require semi-hosting or similar.
//
// For example, for toolchains derived from GNU Tools for Embedded,
// to enable semi-hosting, the following was added to the linker:
//
// `--specs=rdimon.specs -Wl,--start-group -lgcc -lc -lc -lm -lrdimon -Wl,--end-group`
//
// Adjust it for other toolchains.
//
// If functionality is not required, to only pass the build, use
// `--specs=nosys.specs`.
//TODO add gpio to transmitter and how send whole packet

int
main()
{
	char device[] = "/dev/gpiochip0";
	char file[] = "test.txt";
	char destination[] = "222.222.222.222";
	char source[] = "111.111.111.111";
	char * desptr;
	char* souptr;
	char *fileptr;
	desptr = destination;
	souptr = source;
	fileptr = file;
	char* devptr = device;
	//init devece
	gpio create(devptr, RX,TX);
	//init frame
	transmitter sender;
	dataFrame local;
	dataFrame *frame = &local;
	//frame->data.dataptr = sender.reserveMem(BLOCKSIZE);
	sender.initFrame(souptr,desptr,fileptr,frame);
	sender.sendPacket(frame);
	//sender.printFrame(frame);
	for (int i = 3; i > -1; i--)
	{
	std::cout<<int(local.head.datalen[i]);
	}
/*	create.createDatalines();
	create.writeData(0xAA);
	create.readData();
 */ cout << "sadf" << endl;

  return 0;
}
