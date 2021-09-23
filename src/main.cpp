//============================================================================
// Name        : main.cpp
// Author      : Jani
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++
//============================================================================

#include <iostream>

#include "transmitter.h"
#include "frame.h"
#include "receiver.h"
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

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
	int answer = atoi(argv[1]);
	std::cout<<"selected: "<<answer<<std::endl;

	char dev1[] = "111.111.111.111";
	char dev2[] = "222.222.222.222";
	char device[] = "/dev/gpiochip0";
	char file[] = "test.txt";
	char * desptr =nullptr;
	char* souptr = nullptr;
	char *fileptr;
	fileptr = file;
	char* devptr = device;
	//init devece
	gpio create(devptr, RX,TX);
	//init frame
	transmitter* sender = new transmitter ;
	dataFrame* local = new dataFrame;

	switch(answer)
	{
		case 0:
			std::cout<<"selected transmitter"<<std::endl;
			desptr = dev1;
			souptr = dev2;
			sender->initFrame(souptr,desptr,fileptr,local);
			break;

		case 1:
			std::cout<<"selected receiver"<<std::endl;
			souptr = dev1;
			desptr = dev2;
			break;

		default:
			desptr = dev1;
			souptr = dev2;
			break;
	}

	create.createDatalines();
	//transmitter part
	receiver *res = new receiver(souptr, fileptr);
	if(answer == 0)
	{
	sender->sendPacket(local, create);
	}
	//receiver part
	else
	{
		std::cout<<"ready to read"<<std::endl;
		res->transmission(create);
		std::cout<<"ready to take packets"<<std::endl;
	}

	delete(sender);
	delete(local);

	std::cout << "Happy end!!!" << endl;
	}
	else
	{
		std::cout<<"to use set 0 transmitter and 1 receiver in commandline"<<std::endl;
	}

  return 0;
}
