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

#include "gige_lib.h"
#include "agentlib.h"
#include <cstring>

uint8_t image[6000000];

int main(int argc, char *argv[])
{
	gige_handle *handle;
	int nbytes, count, tbytes;
	int32_t iretn;
	uint32_t width, height;
	uint32_t pnum, bsize, i;
	uint8_t bufferin[60000];
	FILE *fp;
	double fmjd, smjd;
	vector<gige_acknowledge_ack> gige_list;

	gige_list = gige_discover();
	if (!gige_list.size())
	{
		printf("Couldn't find any cameras\n");
		exit(1);
	}

	for (i=0; i<gige_list.size(); ++i)
	{
		if (gige_list[i].model[0] == 'G');
			break;
	}
//	if (strncmp(gige_list[0].model, "FLIR AX5", 8) == 0 )
//		camtype = 1;
//	else
//		camtype = 0;

	handle = gige_open(gige_value_to_address(gige_list[i].address),0x02,15000,15000,7000000);
	if (handle == NULL)
	{
		printf("Couldn't open camera\n");
		exit(1);
	}

	iretn = gige_readreg(handle,GIGE_REG_CCP);
	printf("Read CCP: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_DEVICE_MAC_HIGH);
	printf("Read DEVICE_MAC_HIGH: %0x\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_DEVICE_MAC_LOW);
	printf("Read DEVICE_MAC_LOW: %0x\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_CCP);
	printf("Read CCP: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_VERSION);
	printf("Read VERSION: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_DEVICE_MODE);
	printf("Read DEVICE_MODE: %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_GVCP_HEARTBEAT_TIMEOUT);
	printf("Read GVCP_HEARTBEAT_TIMEOUT %u\n",iretn);

	iretn = gige_readreg(handle,GIGE_REG_PRIMARY_APPLICATION_IP_ADDRESS);
	printf("Read PRIMARY_APPLICATION_IP_ADDRESS %x\n",iretn);
/*
	iretn = gige_readmem(handle,GIGE_REG_FIRST_URL,512);
	ptr = (char *)handle->cack_mem.data;
	do
	{
		ptr++;
	} while (*ptr != ':');
	ptr1 = ++ptr;

	do
	{
		ptr++;
	} while (*ptr != ';');
	strncpy(fname,ptr1,ptr-ptr1);
	fname[ptr-ptr1] = 0;
	sscanf((char *)ptr,";%x;%x",&faddress,&fsize);
	printf("Read GIGE_REG_FIRST_URL %u %s;%u;%u\n",iretn,fname,faddress,fsize);

	iretn = gige_readmem(handle,GIGE_REG_SECOND_URL,512);
	printf("Read GIGE_REG_FIRST_SECOND %u %s\n",iretn,handle->cack_mem.data);

	fp = fopen(fname,"w");
	count=0;
	for (uint32_t i=0; i<fsize/536; ++i)
	{
		iretn = gige_readmem(handle,faddress+count,536);
		fwrite((void *)handle->cack_mem.data,iretn,1,fp);
		count += iretn;
		printf("Read GIGE_REG_XML %u %d\r",iretn,count);
	}
	iretn = gige_readmem(handle,faddress+count,fsize-count);
	fwrite((void *)handle->cack_mem.data,iretn,1,fp);
	count += iretn;
	printf("Read GIGE_REG_XML %u %d\n",iretn,count);
	fclose(fp);
*/
	iretn = gige_readreg(handle,PROSILICA_BinningXValue);
	printf("Read PROSILICA_BinningXValue %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_BinningYValue);
	printf("Read PROSILICA_BinningYValue %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_RegionX);
	printf("Read PROSILICA_RegionX %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_RegionY);
	printf("Read PROSILICA_RegionY %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_PixelFormat);
	printf("Read PROSILICA_PixelFormat %d\n",iretn);

	width = gige_readreg(handle,PROSILICA_MaxWidth);
	printf("Read PROSILICA_MaxWidth %d\n",width);

	height = gige_readreg(handle,PROSILICA_MaxHeight);
	printf("Read PROSILICA_MaxHeight %d\n",height);

	iretn = gige_readreg(handle,PROSILICA_SensorWidth);
	printf("Read PROSILICA_SensorWidth %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_Width);
	printf("Read PROSILICA_Width %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_SensorHeight);
	printf("Read PROSILICA_SensorHeight %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_Height);
	printf("Read PROSILICA_Height %d\n",iretn);

	width = gige_readreg(handle,PROSILICA_SensorWidth);
	iretn = gige_readreg(handle,PROSILICA_Width);
	printf("Read PROSILICA_Width %d\n",iretn);

	height = gige_readreg(handle,PROSILICA_SensorHeight);
	iretn = gige_readreg(handle,PROSILICA_Height);
	printf("Read PROSILICA_Height %d\n",iretn);

	iretn = prosilica_config(handle, PROSILICA_PixelFormat_BayerRG12, 1, 1, 2448, 2050, 0, 0);
	/*
	iretn = gige_writereg(handle,PROSILICA_BinningXValue,0);
	iretn = gige_writereg(handle,PROSILICA_BinningYValue,0);
	iretn = gige_writereg(handle,PROSILICA_RegionX,0);
	iretn = gige_writereg(handle,PROSILICA_RegionY,0);
	iretn = gige_writereg(handle,PROSILICA_PixelFormat,PROSILICA_PixelFormat_BayerRG12);
	iretn = gige_writereg(handle,PROSILICA_Width,2448);
	iretn = gige_writereg(handle,PROSILICA_Height,2050);
	iretn = gige_writereg(handle,PROSILICA_StreamBytesPerSec,handle->streambps);
	*/
	iretn = gige_readreg(handle,PROSILICA_StreamBytesPerSec);
	printf("Read PROSILICA_StreamBytesPerSec %d\n",iretn);

	iretn = gige_writereg(handle,GIGE_REG_SCP,handle->stream.cport);
	for (bsize=512; bsize<GIGE_MAX_PACKET; bsize+=100)
	{
		iretn = gige_writereg(handle,GIGE_REG_SCPS,0xc0000000+bsize);
		if (iretn)
		{
			printf("Error %d writing STREAM_CHANNEL_PACKET_SIZE\r",iretn);
		}
		iretn = gige_readreg(handle,GIGE_REG_SCPS)%65536;
		nbytes=recvfrom(handle->stream.cudp, (char *)bufferin,GIGE_MAX_PACKET,0,(struct sockaddr *)NULL,(socklen_t *)NULL);
		if (nbytes < 0) break;
		printf("Read GIGE_REG_STREAM_CHANNEL_PACKET_SIZE %d %d %d\r",bsize,iretn,nbytes);
	}
	printf("\n");
	bsize = bsize-100;

	iretn = gige_writereg(handle,GIGE_REG_SCPS,bsize);
	if (iretn)
	{
		printf("Error %d writing STREAM_CHANNEL_PACKET_SIZE\n",iretn);
	}
	iretn = gige_readreg(handle,GIGE_REG_SCPS);
	printf("Read GIGE_REG_STREAM_CHANNEL_PACKET_SIZE %d\n",iretn);

	iretn = gige_writereg(handle,PROSILICA_ExposureValue,10000);
	iretn = gige_readreg(handle,PROSILICA_ExposureValue);
	printf("Read PROSILICA_ExposureValue %d\n",iretn);

	iretn = gige_writereg(handle,PROSILICA_AcquisitionMode,PROSILICA_AcquisitionMode_SingleFrame);
	iretn = gige_readreg(handle,PROSILICA_AcquisitionMode);
	printf("Read PROSILICA_AcquisitionMode %d\n",iretn);

	iretn = gige_readreg(handle,PROSILICA_PayloadSize);
	printf("Read PROSILICA_PayloadSize %d\n",iretn);
/*
	iretn = gige_writereg(handle,GIGE_REG_SCDA,0xc0a89616);
	if (iretn)
	{
		printf("Error %d writing STREAM_CHANNEL_DESTINATION_ADDRESS\n",iretn);
	}
*/
	iretn = gige_readreg(handle,GIGE_REG_SCDA);
	printf("Read GIGE_STREAM_CHANNEL_DESTINATION_ADDRESS %x\n",iretn);
	iretn = gige_writereg(handle,PROSILICA_AcquisitionCommand,PROSILICA_AcquisitionCommand_Start);
	fmjd = currentmjd(0.);

	fp = fopen("testimage","wb");
	count = tbytes = 0;
	pnum = 0;
	smjd = currentmjd(0.);
	while ((nbytes=recvfrom(handle->stream.cudp, (char *)bufferin, bsize, 0, (struct sockaddr *)NULL,(socklen_t *)NULL)) > 0)
	{
		smjd = currentmjd(0.);
		++count;
//		iretn = gige_readreg(handle,GIGE_REG_CCP);
		tbytes += nbytes;
		if ((uint32_t)(65536*bufferin[5]+256*bufferin[6]+bufferin[7]) != pnum+1) printf("\n");
		pnum = 65536*bufferin[5]+256*bufferin[6]+bufferin[7];
		switch (bufferin[4])
		{
		case 1:
			printf("#%d,%d [%u %u %u %4u] Read: %4d, total: %d (%f)\n",count,pnum,256*bufferin[1]+bufferin[0],256*bufferin[2]+bufferin[3],bufferin[4],pnum,nbytes,tbytes,86400.*(smjd-fmjd));
			break;
		case 2:
			printf("\n#%d,%d [%u %u %u %4u] Read: %4d, total: %d (%f)\n",count,pnum,256*bufferin[1]+bufferin[0],256*bufferin[2]+bufferin[3],bufferin[4],pnum,nbytes,tbytes,86400.*(smjd-fmjd));
			break;
		case 3:
//			fwrite((void *)&bufferin[8],nbytes-8,1,fp);
//			printf("#%d,%d [%u %u %u %4u] Read: %4d, total: %d (%f)\r",count,pnum,256*bufferin[1]+bufferin[0],256*bufferin[2]+bufferin[3],bufferin[4],pnum,nbytes,tbytes,86400.*(smjd-fmjd));
			break;
		}
		fmjd = smjd;
	}
	printf("\n");
	printf("\n#%d,%d [%u %u %u %4u] Read: %4d, total: %d (%f)\n",count,pnum,256*bufferin[1]+bufferin[0],256*bufferin[2]+bufferin[3],bufferin[4],pnum,nbytes,tbytes,86400.*(smjd-fmjd));
	fclose(fp);

	iretn = gige_writereg(handle,PROSILICA_AcquisitionCommand,PROSILICA_AcquisitionCommand_Stop);
	iretn = gige_writereg(handle,GIGE_REG_SCP,0);

	gige_close(handle);

	exit(0);
}
