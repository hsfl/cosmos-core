#include "kpc9612p_lib.h"
#include "timelib.h"

kpc9612p_handle handle;
char device[15]="/dev/ttyUSB0";
char source[15], destination[15];
uint8_t message[300];

int main(int argc, char *argv[])
{
	int32_t iretn;
//	double lmjd;
	
	if (argc == 2) strcpy(device,argv[1]);

	if ((iretn=kpc9612p_connect(device, &handle, 0x00)) < 0)
	{
		printf("Failed to open KPC9612P on %s, error %d\n",device,iretn);
		exit (-1);
	}

//	lmjd = currentmjd(0.);
	handle.frame.size = 255;
	for (uint16_t i=0; i<2000; ++i)
	{

		for (uint16_t j=1; j<=handle.frame.size; ++j)
		{
			message[j] = i%256;
		}
		message[0] = 0x10;
		if ((iretn=cssl_putslip(handle.serial, message, handle.frame.size+1)) < 0)
		{
			printf("Failed to send frame, error %d\r",iretn);
		}
		else
		{
			printf("[%u] Sent %d bytes\n",i,handle.frame.size);
		}
		COSMOS_USLEEP(2000000);
//		lmjd = currentmjd(0.);
	}
}
