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
#include "PvApi.h"
#include <string.h>
#include <pthread.h>

typedef struct
	{
	uint32_t UID;
	tPvHandle Handle;
	tPvFrame Frame;
	} tCamera;

tCamera camera;
char fbuf[10036800], cbuf[100];
unsigned long fsize = 10036800;

int main(int argc, char *argv[])
{
FILE *op;
tPvUint32 count, connected, imsize;
tPvCameraInfo clist;
int iretn;
unsigned long lret;


if ((iretn=PvInitialize()))
	_exit (iretn);
while(!PvCameraCount())
	{
	printf(".");
	fflush(stdout);
	sleep(1);
	}
count = PvCameraList(&clist,1,&connected);

iretn = PvCameraOpen(clist.UniqueId,ePvAccessMaster,&(camera.Handle));
if (iretn)
	_exit(1);

camera.Frame.Context[0] = &camera;
camera.Frame.ImageBuffer = fbuf;
camera.Frame.ImageBufferSize = fsize;

iretn = PvCaptureStart(camera.Handle);
//iretn = PcCaptureAdjustPacketSize(camera.Handle,8228);

/*
width=1000;
height=1000;
X=524;
Y=724;
*/
iretn = PvAttrEnumSet(camera.Handle,"AcquisitionMode","SingleFrame");
iretn = PvAttrEnumSet(camera.Handle,"ExposureMode","Manual");
iretn = PvAttrUint32Set(camera.Handle,"BinningX",1);
iretn = PvAttrUint32Set(camera.Handle,"BinningY",1);
iretn = PvAttrUint32Set(camera.Handle,"RegionX",0);
iretn = PvAttrUint32Set(camera.Handle,"RegionY",0);
iretn = PvAttrUint32Set(camera.Handle,"Width",2448);
iretn = PvAttrUint32Set(camera.Handle,"Height",2050);
/*
iretn = PvAttrUint32Set(camera.Handle,"RegionX",X);
iretn = PvAttrUint32Set(camera.Handle,"RegionY",Y);
*/
//iretn = PvAttrEnumSet(camera.Handle,"AcquisitionMode","Continuous");
iretn = PvAttrUint32Set(camera.Handle,"ExposureValue",5000);

strcpy(cbuf,"Bayer16");
iretn = PvAttrEnumSet(camera.Handle,"PixelFormat",cbuf);

iretn = PvAttrEnumGet(camera.Handle,"PixelFormat",cbuf,100,&lret);
iretn = PvAttrUint32Get(camera.Handle,"TotalBytesPerFrame",&imsize);
printf("\n%u\n%s %u\n",(uint32_t)count,cbuf,(uint32_t)imsize);

iretn = PvCaptureQueueFrame(camera.Handle,&(camera.Frame),NULL);
iretn = PvCommandRun(camera.Handle,"AcquisitionStart");
iretn = PvCaptureWaitForFrameDone(camera.Handle,&(camera.Frame),15000);

if (iretn == ePvErrSuccess)
	{
	printf("Successful!\n");
	op = fopen("test.img","w");
	fwrite((void *)fbuf,1,fsize,op);
	fclose(op);
	}

iretn = PvCommandRun(camera.Handle,"AcquisitionStop");
iretn = PvCaptureEnd(camera.Handle);
}
