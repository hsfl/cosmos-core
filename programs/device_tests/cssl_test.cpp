#include "cssl_lib.h"
#include "timelib.h"

cssl_t *serial_in, *serial_out;
float stride;
void send_loop();
//int talk=0;

int main(int argc, char *argv[])
{
	int16_t ch;
	uint16_t ecount, lcount1, lcount2, tcount;
	double lmjd;

	cssl_start();

	switch (argc)
	{
	case 3:
		{
			if ((serial_in = cssl_open((char *)argv[1], 115200, 8, 0, 1)) == NULL)
			{
				printf("Unable to open %s for input\n",(char *)argv[1]);
				exit (1);
			}
			if ((serial_out = cssl_open((char *)argv[2], 115200, 8, 0, 1)) == NULL)
			{
				printf("Unable to open %s for output\n",(char *)argv[2]);
				exit (1);
			}
			break;
		}
	case 2:
		{
			if ((serial_in = cssl_open((char *)argv[1], 115200, 8, 0, 1)) == NULL)
			{
				printf("Unable to open %s for input\n",(char *)argv[1]);
				exit (1);
			}
			serial_out = serial_in;
			break;
		}
	default:
		{
			printf("Usage: cssl_test serial_in [serial_out]\n");
			exit (1);
			break;
		}
	}

//	thread send_thread(send_loop);

	tcount = 0;
	for (double to=0.; to<.2; to+=.1)
	{
		cssl_settimeout(serial_out, 0, to);
		for (stride=0.; stride<.005; stride+=.001)
		{
			while ((ch=cssl_getchar(serial_in)) >= 0);
			ecount = lcount1 = lcount2 = 0;
			lmjd = currentmjd(0.);
			for (uint16_t i=1; i<=1000; ++i)
			{
				cssl_putchar(serial_out, (uint8_t)i);
				COSMOS_USLEEP((uint32_t)(stride*1000000));
				if ((ch = cssl_getchar(serial_in)) < 0)
				{
					ecount++;
				}
				else
				{
					lcount1++;
					tcount++;
				}
			}
			while ((ch=cssl_getchar(serial_in)) >= 0)
			{
				lcount2++;
				tcount++;
			}
			printf("%f: %d %d %d %d %f\n",stride,ecount,lcount2,lcount1,tcount,86400.*(currentmjd(0.)-lmjd)/1000);
		}
	}

	cssl_close(serial_in);
	if (serial_in != serial_out)
	{
		cssl_close(serial_out);
	}
}

/*
void send_loop()
{
uint8_t byte;

	while (1)
	{
		if (talk)
		{
			for (byte=1; byte<=100; ++byte)
				{
				cssl_putchar(serial_out, (uint8_t)byte);
				COSMOS_USLEEP((uint32_t)(stride*1000000+1000));
				}
		talk = 0;
		}
	}
printf("\nExiting send_loop\n");
}
*/
