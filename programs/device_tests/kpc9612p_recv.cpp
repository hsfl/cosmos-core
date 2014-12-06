#include "kpc9612p_lib.h"
#include "timelib.h"

kpc9612p_handle handle;
char device[15]="/dev/ttyUSB3";
char source[15], destination[15];

int main(int argc, char *argv[])
{
	int32_t iretn, count;
	uint8_t buf[500];
	double lmjd, mean;
	bool waited;
	
	if (argc == 2) strcpy(device,argv[1]);

	if ((iretn=kpc9612p_connect(device, &handle, 0x00)) < 0)
	{
		printf("Failed to open KPC9612P on %s, error %d\n",device,iretn);
		exit (-1);
	}

	lmjd = currentmjd(0.);
	count = 0;
	for (uint16_t i=0; i<2000; ++i)
	{
		waited = false;
		while ((iretn=kpc9612p_recvframe(&handle)) < 0)
		{
			printf("Waiting: %f\r",86400.*(currentmjd(0.)-lmjd));
			fflush(stdout);
			waited = true;
		}
		if (waited) printf("\n");

		if ((iretn=kpc9612p_unloadframe(&handle, buf, 500)) >= 0)
		{
			++count;
			mean = 0.;
			for (uint16_t j=0; j<handle.frame.size-17; ++j)
			{
				mean += handle.frame.part.payload[j];
			}
			mean /= handle.frame.size-17;
			printf("[%f] Received: %u bytes [%f] #%d\n",86400.*(currentmjd(0.)-lmjd),handle.frame.size,mean,count);
			fflush(stdout);
		}

		lmjd = currentmjd(0.);
	}
}
