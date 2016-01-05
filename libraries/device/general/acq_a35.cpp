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

//////////////////////////////////////////////////////////////////////
//
//  AcqDummy.cpp - Implementation
//
//////////////////////////////////////////////////////////////////////

#include "agentlib.h"
#include "gige_lib.h"
#include "acq_a35.h"
#include "elapsedtime.h"
#ifndef COSMOS_WIN_BUILD_MSVC
#include <sys/time.h>
#endif

#define IMAGESIZEMAX 86016

extern gige_handle *handle;
extern uint16_t ImageFrameBuffer[IMAGESIZEMAX];
extern int32_t ImageSize;
extern uint16_t Width, Height;

unsigned int PacketSize;
uint32_t PreviousImageNum=0;
//////////////////////////////////////////////////////////////////////

//! \addtogroup acq_a35_functions
//! @{
bool InitCamera(uint16_t width, uint16_t height, gige_handle* handle)
{
    uint16_t i;
    int16_t a35Index;
//    uint32_t myip;
    std::vector<gige_acknowledge_ack> gige_list;
//    struct timeval StartTime;
//    struct timeval PresentTime;
//    int64_t Duration, seconds, useconds;

    // Get time for timeout
//    gettimeofday(&StartTime, NULL);
	ElapsedTime et;
	et.start();

    do
	{
        gige_list = gige_discover();
        if(!gige_list.empty())
		{
            break;
        }

        // 2000ms timeout
//        gettimeofday(&PresentTime, NULL);
//        seconds  = PresentTime.tv_sec  - StartTime.tv_sec;
//        useconds = PresentTime.tv_usec - StartTime.tv_usec;
//        Duration = seconds*1000000.0 + useconds;
//        if(Duration > 2000000) break;   // Timeout
		if (et.lap() > 2.) break;
    } while(true);


    if(gige_list.empty())
	{
        printf("No Cameras found.");
        exit(-10);
    }


    // Determine which item in the list is the A35.
    a35Index = -1;
    for(i=0 ; i<gige_list.size() ; i++)
	{
        if( strncmp(gige_list[i].model, "FLIR AX5", 8) == 0 )
		{
            a35Index = i;
        }
    }
    if(a35Index == -1)
	{
        printf("No A35 camera found.\n");
        exit(-11);
    }


    handle = gige_open(gige_value_to_address(gige_list[a35Index].address),0x02,10000,15000,1000000);


    printf("Found %s @ %s\n" , gige_list[a35Index].model, handle->stream.address);

    if (handle == NULL)
	{
        printf("Couldn't open camera\n");
        exit(-12);
    }

	handle->maxwidth = 350;
	handle->maxheight = 256;

	// Set width & height of camera image
	if (width > handle->maxwidth)
	{
		handle->width = handle->maxwidth;
	}
	else
	{
		handle->width = width;
	}
	gige_writereg(handle, A35_WIDTH, handle->width); // Set Desired Width

	if (height > handle->maxheight)
	{
		handle->height = handle->maxheight;
	}
	else
	{
		handle->height = height;
	}
	gige_writereg(handle, A35_HEIGHT, handle->height); // Set Desired height

	handle->bestsize = 4 * handle->width + 28 +8;
	gige_writereg(handle, GIGE_REG_SCPS, handle->bestsize);

    gige_writereg(handle,0xE984, 3);            // Set to 14 bit mode
    gige_writereg(handle,0xD308, 0x01100025);   // Set pixels to 14 bit

    // Set shutter to manual
    gige_writereg(handle, A35_FFCMODE, 0); // Set FFC to manual
    gige_writereg(handle,0xE938,0); // Do Shutter

    // Set digital output to 14 bit
    gige_writereg(handle, A35_PIXELFORMAT, A35_PIXELFORMAT_14BIT);

    //if(FrameRate==60)
//	{
    //    gige_writereg(handle,A35_SENSORVIDEOSTANDARD,A35_SENSORVIDEOSTANDARD_60HZ);
    //} else
//	{

    // Set frame rate 30 Hz
    gige_writereg(handle,A35_SENSORVIDEOSTANDARD,A35_SENSORVIDEOSTANDARD_30HZ);
    //}

    // Set the number of frames to skip (i.e. A35_SKIP1 means skip every other frame).
	gige_writereg(handle, A35_FRAMESTOSKIP, A35_SKIP0); // Set Desired Width


    // Set LVDSMode to disabled
    //gige_writereg(handle,A35_LVDSMODE,0); // Disable LVDS (shouldn't be neccessary)
    // Set XPMode to CMOS
    //gige_writereg(handle,A35_XPMODE,A35_XPMODE_CMOS); // Set XPMODE to CMOS (shouldn't be neccessary)


    // *****************************************************************************
    // *****************************************************************************
    // **********************  Second Port for Data      ***************************
    // *****************************************************************************
    // *****************************************************************************
    printf("Done Init\n");
    return(true);
}

//////////////////////////////////////////////////////////////////////

bool GetCameraFrame(gige_handle* handle, uint16_t *A35FrameBuffer, uint32_t ImageSize)
{
//    int16_t iretn;
    uint8_t bufferin[GIGE_MAX_PACKET];
    uint8_t *TempBuf;
    //uint8_t NumSteps;
    //uint16_t ImageNum;
    //uint16_t RemainingBytes;
    int16_t bytes_recieved=0;
    uint32_t PacketNum=0;
    uint32_t MaxPacketNum;
    uint32_t FrameNum;
    static uint32_t FrameCount=0;
//    static int32_t LastPacketCount=-1;
    static uint32_t PacketCount;
    uint32_t bsize=GIGE_MAX_PACKET;
    uint8_t PacketType;
    uint32_t PayloadSize, FramePtr;
//    struct timeval StartTime;
//    struct timeval PresentTime;
//    int64_t Duration, seconds, useconds;


    PacketCount = 0;
    PayloadSize = PacketSize-28-8;
    MaxPacketNum = ImageSize*sizeof(uint16_t)/PayloadSize;
    TempBuf = (uint8_t*)A35FrameBuffer;

    //printf("Entering data acquisition routine.\n");

    // Handshake with camera every 1.5 seconds (30 fps)
    FrameCount++;
    if( (FrameCount%15) == 0 )
	{
        gige_writereg(handle,GIGE_REG_CCP, 2);
    }

    // Get time for timeout
//    gettimeofday(&StartTime, NULL);
	ElapsedTime et;
	et.start();



    // Loop untill we get the tail packet (then we return)
    do
	{
        // Loop untill we get a packet
        bytes_recieved=0;

        while(bytes_recieved <= 0)
		{
			bytes_recieved=recvfrom(handle->stream.cudp,(char *)bufferin,bsize,0,(struct sockaddr *)NULL,(socklen_t *)NULL);
//			printf("br=%d " , bytes_recieved);

            if(bytes_recieved==-1)
			{
				COSMOS_USLEEP(5000);

                // 500ms timeout
//                gettimeofday(&PresentTime, NULL);
//                seconds  = PresentTime.tv_sec  - StartTime.tv_sec;
//                useconds = PresentTime.tv_usec - StartTime.tv_usec;
//                Duration = seconds*1000000.0 + useconds;
//                if(Duration > 1000000)
				if (et.lap() > 1.)
				{
                    printf("Timeout2\n");
                    return(false);   // Timeout
                }
            }
        }

        FrameNum = (bufferin[2]<<8) + bufferin[3]; // Get image number
        PacketType = bufferin[4];
        PacketNum = (bufferin[5]<<16) + (bufferin[6]<<8) + bufferin[7]; // Get image number
//        printf("\n>>PCount=%u, PNum=%u , Type=%u<<  ", PacketCount , PacketNum , PacketType);

        // If lost packets. (The present packet is more than the PacketCount)
        if (PacketNum > PacketCount)
		{
            // If we only missed the header packet, just set the PacketCount
            // Oterwise fill lost data with zeros

            printf("Lost Packet. PNum=%u, PCount=%u\n" , PacketNum, PacketCount);
            if(PacketNum>1)
			{
                if(PacketCount==0)
				{
                    // Since packet 0 is header, we need to handle differently.
                    TempBuf = (uint8_t*)A35FrameBuffer;
                    memset(TempBuf, 0, PayloadSize * (PacketNum-1));
                    TempBuf+=(PayloadSize * (PacketNum-1));
                }
				else
				{
                    FramePtr = (PacketCount-1)*PayloadSize;
                    TempBuf = (uint8_t*)A35FrameBuffer + FramePtr;
                    memset(TempBuf, 0, PayloadSize * (PacketNum-PacketCount));
                    TempBuf+=PayloadSize;
                }
            }
            PacketCount = PacketNum;
        }

        // If packetCount is greater than the recieved packet number, we missed the tail packet (and are recieving the next frame).
        // Zero out lost packets and exit (for now lose the data from the new packet)
        if(PacketNum < PacketCount)
		{
            printf("Lost Tail Packet. PNum=%u, PCount=%u\n" , PacketNum, PacketCount);
            FramePtr = (PacketCount-1)*PayloadSize;
            TempBuf = (uint8_t*)A35FrameBuffer + FramePtr;
            memset(TempBuf, 0, PayloadSize * (MaxPacketNum-PacketCount));
            return(true);
        }

        switch(PacketType)
		{
        case 1: // Start packet
//            printf("Start packet. FrameNum=%u -> " , FrameNum);
            break;
        case 2: // Tail packet
            // Exit if tail packet
//            printf("Tail packet FrameNum=%u, PackCnt=%d\n" , FrameNum, PacketCount);
            return(true);
        case 3: // Data packet
//			printf("Data: FrameNum=%u, FrmCnt=%u, PkNum=%u, BytesRcvd=%d\n" , FrameNum, FrameCount, PacketNum, bytes_recieved);
            // get packet data.
            if(bytes_recieved<8)
			{
                printf("Error recieving frame\n");
                return(true);
            }

            // Copy camera image into A35FrameBuffer
			memcpy(TempBuf, &bufferin[8], bytes_recieved-8);
			TempBuf += bytes_recieved-8;
			/*
            for(i=0 ; i<bytes_recieved-8 ; i++)
			{
                *TempBuf = bufferin[8+i];
                TempBuf++;
            }
			*/
            break;
        default:
            printf("\n\n>>>>> Unknown PackType=%u, PackNum=%u, FrameNum=%u, FrmCnt=%u, \n" , PacketType, PacketNum, FrameNum, FrameCount);
            return(false);
        }

//        LastPacketCount = PacketCount;
        PacketCount++;

    } while(true);

    return(true);
}


//////////////////////////////////////////////////////////////////////

void StartCamera(gige_handle* handle)
{
    // Start Grabbing Frames
	gige_writereg(handle,GIGE_REG_SCDA, gige_address_to_value(handle->stream.address));
	gige_writereg(handle,GIGE_REG_STREAM_CHANNEL_PORT, handle->stream.cport);

	gige_writereg(handle, A35_ACQUISITIONSTART, 0x1);
}

//////////////////////////////////////////////////////////////////////

int ShutdownCamera(gige_handle* handle)
{
    // Stop Grabbing Frames
    gige_writereg(handle, A35_ACQUISITIONSTOP, 0x1);
	gige_writereg(handle,GIGE_REG_SCP,0);

    gige_close(handle);
    return(0);
}


//////////////////////////////////////////////////////////////////////

bool ResetCamera(gige_handle* handle)
{
    gige_writereg(handle, A35_CAMERAHEADRESET, 0); // Set FFC to Auto
    return(true);
}


//////////////////////////////////////////////////////////////////////

// This will set AutoDUC to 5 minutes (0x12C seconds)
bool AutoFFC(gige_handle* handle)
{
    gige_writereg(handle, A35_FFCMODE, A35_FFCMODE_AUTO); // Set FFC to Auto
    return(true);
}

//////////////////////////////////////////////////////////////////////

// This will set AutoDUC to 20 minutes (0x4B0 seconds)
bool ManualFFC(gige_handle* handle)
{
    gige_writereg(handle, A35_FFCMODE, A35_FFCMODE_MANUAL); // Set FFC to manual
    return(true);
}

//////////////////////////////////////////////////////////////////////

bool DoFFC(gige_handle* handle)
{
    gige_writereg(handle,A35_COMMAND_DOFFC,0); // Do Shutter
    return(true);
}

bool GetTemperature(gige_handle* handle)
{
    gige_writereg(handle,A35_TEMPERATUREFPA,0); // Get temperature
    return(true);
}

bool SetFramesToSkip(int FramesToSkip, gige_handle* handle)
{
    FramesToSkip = (FramesToSkip << 16) & 0xFFF0000;
    gige_writereg(handle, A35_FRAMESTOSKIP, FramesToSkip);
    return(true);
}

bool SetPacketDelay(int PacketDelay, gige_handle* handle)
{
    gige_writereg(handle, A35_PACKETDELAY, PacketDelay); // Get temperature
    return(true);
}

//! @}
