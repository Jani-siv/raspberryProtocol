/*
 * gpio.cpp
 *
 *  Created on: Sep 17, 2021
 *      Author: Gyy
 */

#include "local_gpio.h"


gpio::gpio(char* dev, __u32 rx, __u32 tx)
{
this->dev.dev = dev;
this->dev.rx =rx;
this->dev.tx = tx;
}
void gpio::writeData(uint8_t data)
{
	int ret;
	uint8_t temp = 0x00;

	//flip bits
	for(int i=0; i < 8; i++)
	{
		temp = data &(0x80);
		if (temp > 0)
		{
		this->outputdata.values[0] =255;
		std::cout<<"1";//<<int(outputdata.values[0]);
		}
		else
		{
			this->outputdata.values[0] = 0;
			std::cout<<"0";//<<int(outputdata.values[0]);
		}
		ret = ioctl(this->output.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &this->outputdata);
		data = data << 1;
		/*TODO create real sleep function to set speed*/
		usleep(5000);
	}
std::cout<<std::endl;
}
char* gpio::readData(int bytes, int timeout)
{
	/*timing
	 * read 8 bit and store those in memory
	 * because this is for protocol we can reserve memory before reading;
	 */
	char * ptr = nullptr;
	ptr = (char*)malloc(bytes*sizeof(char));
	if (this->busSpeed.inputspeed == 0)
	{
this->getTiming(timeout);
	}
if (this->timeout == true)
{
	//this wont stop program timeout
	std::cout<<"timeout in local_gpio.cpp"<<std::endl;
	return nullptr;
}
bytes--; //used first 8 bits to calculate and return halfway of bit back
int j= 0;
	while (bytes > 0)
	{
		uint8_t datablock = 0x00;
		for (int i = 0; i < 8; i++)
		{
			ioctl(this->input.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &this->inputdata);
			if (inputdata.values[0] > 0)
			{
				datablock = datablock + 0x01;
				if (i < 7)
					{
						datablock = datablock << 1;

					}
				std::cout<<"1";
			}
			else
			{
				std::cout<<"0";
				if (i < 7)
					{

						datablock = datablock << 1;
					}
			}
			usleep(this->busSpeed.inputspeed);
		}
		ptr[j] = datablock;
		if (ptr[0] != -1)
		{
			std::cout<<"ptr stop:"<<int(ptr[0])<<std::endl;
			char error = 0x00;
			char *ptrerror;
			ptrerror = &error;
			return ptrerror;
		}
		std::cout<<" "<<ptr[j]<<std::endl;
	//	std::cout<<"data block: "<<j<<" is stored in memory"<<std::endl;
		j++;
		bytes--;
	}

return ptr;
}

void gpio::getTiming(int timeout)
{
	std::chrono::duration<double> diff, timeout1;


	auto end = std::chrono::high_resolution_clock::now();
	auto timeoutEnd = std::chrono::high_resolution_clock::now();
	bool startTiming = false;

	while (startTiming == false)
	{
		ioctl(this->input.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &this->inputdata);
		//std::cout<<int(inputdata.values[0])<<std::endl;
		if (inputdata.values[0] > 0)
		{
			auto start =std::chrono::high_resolution_clock::now();
		while (inputdata.values[0] > 0)
		{
		//	std::cout<<int(inputdata.values[0])<<std::endl;
			ioctl(this->input.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &this->inputdata);
			end = std::chrono::high_resolution_clock::now();
		}
		diff = end - start;
		startTiming = true;
		}
		if (startTiming == true)
		{
			this->busSpeed.inputspeed = diff.count()*1000000;

			std::cout<<"reading speed is: "<<this->busSpeed.inputspeed<<std::endl;
			std::cout<<"reading speed is: "<<diff.count()<<std::endl;
			//skip rest of timing
			usleep(7*this->busSpeed.inputspeed+(this->busSpeed.inputspeed*0.4));
			this->busSpeed.inputspeed -= this->busSpeed.inputspeed %5000; //350 ;)
			break;
		}
		if (startTiming == false)
		{
		timeoutEnd = std::chrono::high_resolution_clock::now();
		timeout1 = timeoutEnd - end;
		double temp = timeout1.count();
		//std::cout<<"temp: "<<temp<<std::endl;
		if (temp > timeout)
		{
			this->timeout = true;
			startTiming = true;
		}}
	//	std::cout<<"timeout count"<<timeout.count()<<std::endl;
	}
}
int gpio::createDatalines()
{

	int fd, ret;

	//strcpy(chrdev_name, "/dev/gpiochip0");

	/*  Open device: gpiochip0 for GPIO bank A */
	fd = open(this->dev.dev, O_RDONLY);
	std::cout<<"opening file:"<<this->dev.dev<<std::endl;
	if (fd == -1) {
		ret = -errno;
		fprintf(stderr, "Failed to open %s\n", this->dev.dev);
		return -1;
	}

	/* request GPIO line: GPIO_A_14 */
	this->output.lineoffsets[0] = this->dev.tx; //transmit
	this->output.flags = GPIOHANDLE_REQUEST_OUTPUT;
	memcpy(this->output.default_values, &this->outputdata, sizeof(this->output.default_values));
	strcpy(this->output.consumer_label, "tx pin");
	this->output.lines  = 1;

	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &this->output);
	std::cout<<"output value is: "<<int(outputdata.values[0])<<std::endl;
	if (ret == -1) {
		ret = -errno;
		fprintf(stderr, "Failed to issue GET LINEHANDLE IOCTL (%d)\n",
			ret);
		return -1;
	}


	this->input.lineoffsets[0] = this->dev.rx; //transmit
	this->input.flags = GPIOHANDLE_REQUEST_INPUT;
	memcpy(this->input.default_values, &this->inputdata, sizeof(this->input.default_values));
	strcpy(this->input.consumer_label, "rx pin");
	this->input.lines  = 1;
	ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &this->input);
	std::cout<<"input value is: "<<int(inputdata.values[0])<<std::endl;
		if (ret == -1) {
			ret = -errno;
			fprintf(stderr, "Failed to issue GET LINEHANDLE IOCTL (%d)\n",
				ret);
			return -1;
		}


	return 0;
}
/*
 * if (close(fd) == -1)
	{
		perror("Failed to close GPIO character device file");
		return -1;
	}
	*/




