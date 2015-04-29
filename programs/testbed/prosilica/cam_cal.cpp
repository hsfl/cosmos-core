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

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <PvApi.h>
#include <string.h>

typedef struct
	{
	uint32_t UID;
	tPvHandle Handle;
	tPvFrame Frame;
	} tCamera;

tCamera camera;
char fbuf[10036800], cbuf[100];
unsigned long fsize = 2000000;

int main(int argc, char *argv[])
{
FILE *op;
tPvUint32 count, connected, imsize;
tPvCameraInfo clist;
int iretn, ET, X, Y, width, height, r, c;
double Red, Green, Blue;
unsigned long lret;
char fname[50];
uint16_t *array;

array=(uint16_t*)fbuf;


PvInitialize();
while(!PvCameraCount())
	{
	printf(".");
	fflush(stdout);
	sleep(1);
	}
count = PvCameraList(&clist,1,&connected);

iretn = PvCameraOpen(111870,ePvAccessMaster,&(camera.Handle));
if (iretn)
	_exit(1);

camera.Frame.Context[0] = &camera;
camera.Frame.ImageBuffer = fbuf;
camera.Frame.ImageBufferSize = fsize;

iretn = PvCaptureStart(camera.Handle);
//iretn = PcCaptureAdjustPacketSize(camera.Handle,8228);
width=1000;
height=1000;
X=524;
Y=724;
iretn = PvAttrEnumSet(camera.Handle,"AcquisitionMode","SingleFrame");
iretn = PvAttrEnumSet(camera.Handle,"ExposureMode","Manual");
iretn = PvAttrUint32Set(camera.Handle,"Width",width);
iretn = PvAttrUint32Set(camera.Handle,"Height",height);
iretn = PvAttrUint32Set(camera.Handle,"RegionX",X);
iretn = PvAttrUint32Set(camera.Handle,"RegionY",Y);
strcpy(cbuf,"Bayer16");
iretn = PvAttrEnumSet(camera.Handle,"PixelFormat",cbuf);

iretn = PvAttrEnumGet(camera.Handle,"PixelFormat",cbuf,100,&lret);
iretn = PvAttrUint32Get(camera.Handle,"TotalBytesPerFrame",&imsize);
printf("\n%u\n%s %u\n",(uint32_t)count,cbuf,(uint32_t)imsize);

ET=250;
do
{

iretn = PvAttrUint32Set(camera.Handle,"ExposureValue",ET);
iretn = PvCaptureQueueFrame(camera.Handle,&(camera.Frame),NULL);
iretn = PvCommandRun(camera.Handle,"AcquisitionStart");
iretn = PvCaptureWaitForFrameDone(camera.Handle,&(camera.Frame),15000);
Red=0;
Blue=0;
Green=0;

if (iretn == ePvErrSuccess)
	{
	sprintf(fname,"ap_cal_%08d.img",ET);
	op = fopen(fname,"w");
	fwrite((void *)fbuf,1,fsize,op);
	fclose(op);
iretn = PvCommandRun(camera.Handle,"AcquisitionStop");

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
printf("%9d %9.2f %9.2f %9.2f\n", ET, Red/250000, Green/500000, Blue/250000);
	}
else
	{
	printf("no image acquired\n");
	}
ET=ET*2;
} while(ET<25000000);

iretn = PvCaptureEnd(camera.Handle);
}
