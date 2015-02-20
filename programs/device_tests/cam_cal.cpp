#include "configCosmos.h"
//#include <sys/types.h>
//#include <stdint.h>
//#include <stdio.h>
//// #include <unistd.h>
//#include <cstring>
#include "gige_lib.h"

char cbuf[100];
unsigned long fsize = 2000000;
uint8_t image[2000000];

int main(int argc, char *argv[])
{
gige_handle *handle;
FILE *op;
int32_t iretn, et, r, c;
double Red, Green, Blue;
char fname[50];
uint16_t *array;
vector<gige_acknowledge_ack> gige_list;


gige_list = gige_discover();
if (!gige_list.size())
{
	printf("Couldn't find any cameras\n");
	exit(1);
}

handle = gige_open(gige_value_to_address(gige_list[0].address),0x02,10000,5000,1000000);
if (handle == NULL)
{
	printf("Couldn't open camera\n");
	exit(1);
}

iretn = prosilica_config(handle, PROSILICA_PixelFormat_BayerRG12, 1, 1, 1000, 1000, 524, 724);

uint32_t gain;
if (argc == 2)
	gain = atol(argv[1]);
else
	gain = 0;

et = 25;
array = (uint16_t *)image;
do
{

iretn = prosilica_image(handle, PROSILICA_ExposureMode_AutoOff, et, gain, image, handle->bestsize);
Red=0;
Blue=0;
Green=0;

if (iretn >= 0)
	{
	sprintf(fname,"ap_cal_%08d.img",et);
	op = fopen(fname,"w");
	fwrite((void *)image,1,fsize,op);
	fclose(op);

for (r=0; r<=999; r++)
	{
	for (c=0; c<=999; c++)
		{
		if (r % 2 == 0)
			{
			if (c % 2 == 0)
				{
				Red=Red+array[r*1000+c];
				}
			else
				{
				Green=Green+array[r*1000+c];
				}
			}
		else
			{
			if (c % 2 == 0)
				{
				Green=Green+array[r*1000+c];
				}
			else
				{
				Blue=Blue+array[r*1000+c];
				}
			}
		}	
	}
printf("%9d %9.2f %9.2f %9.2f\n", et, Red/250000, Green/500000, Blue/250000);
	}
else
	{
	printf("no image acquired\n");
	}
et=et*2;
} while(et<25000);

gige_close(handle);
}
