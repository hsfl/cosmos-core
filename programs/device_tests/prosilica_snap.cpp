/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

#include "support/configCosmos.h"
////#include <sys/types.h>
//#include <stdint.h>
//#include <stdio.h>
//// #include <unistd.h>
//#include <cstring>
#include "device/general/gige_lib.h"
#include "support/timelib.h"

uint8_t image[20000000];

int main(int argc, char *argv[])
{
int32_t iretn;
uint16_t xsize=2448;
uint16_t  ysize=2050;
uint16_t  xoffset=0;
uint16_t  yoffset=0;
uint16_t xbin=1;
uint16_t ybin=1;
uint32_t et=50000;
gige_handle *handle;
FILE *op;
char fname[50];
std::vector<gige_acknowledge_ack> gige_list;

switch (argc)
{
case 9:
	ybin = atol(argv[8]);
case 8:
	xbin = atol(argv[7]);
case 7:
	yoffset = atol(argv[6]);
case 6:
	xoffset = atol(argv[5]);
case 5:
	ysize = atol(argv[4]);
case 4:
	xsize = atol(argv[3]);
case 3:
	et = atol(argv[2]);
case 2:
	strcpy(fname,argv[1]);
	break;
}


printf("%.15g discover\n",currentmjd(0.));
gige_list = gige_discover();
if (!gige_list.size())
{
	printf("Couldn't find any cameras\n");
	exit(1);
}

printf("%.15g open\n",currentmjd(0.));
handle = gige_open(gige_value_to_address(gige_list[0].address),0x02,10000,15000,5000000);
if (handle == NULL)
{
	printf("Couldn't open camera\n");
	exit(1);
}

printf("%.15g config\n",currentmjd(0.));
iretn = prosilica_config(handle, PROSILICA_PixelFormat_BayerRG12, xbin, ybin, xsize, ysize, xoffset, yoffset);

printf("%.15g image\n",currentmjd(0.));
if (et)
	iretn = prosilica_image(handle, PROSILICA_ExposureMode_AutoOff, et, 0, image, handle->bestsize);
else
	iretn = prosilica_image(handle, PROSILICA_ExposureMode_Auto, et, 0, image, handle->bestsize);
	
printf("%.15g fwrite\n",currentmjd(0.));
if (iretn >= 0)
	{
	op = fopen(fname,"w");
	fwrite((void *)image, iretn, 1, op);
	fclose(op);
	}
else
	{
	printf("no image acquired\n");
	}

printf("%.15g close\n",currentmjd(0.));
gige_close(handle);
}
