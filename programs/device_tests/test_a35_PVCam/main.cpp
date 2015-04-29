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

#include <stdlib.h>
#include <stdio.h>
#include "a35acq.h"

#define IMAGEWIDTH 320
#define IMAGEHEIGHT 256
#define IMAGESIZE (IMAGEWIDTH*IMAGEHEIGHT)

typedef char Int8;
typedef short Int16;
typedef int Int32;
typedef unsigned int uInt32;

Ca35acq Camera;
unsigned short m_nPhotonFrameBuffer[IMAGESIZE];

float FramesPerSec;

static unsigned int        AcqWinWidth = IMAGEHEIGHT;
static unsigned int        AcqWinHeight = IMAGEWIDTH;

double t1 = 0, t2;
Int32 nbFrame;
uInt32 currBufNum;


int main(int argc, char *argv[])
{
    int 	        i, j, error;
    unsigned int	bitsPerPixel;
    int WaterfallBand;
    unsigned short *bufAddr = NULL;
    int MinVal, MaxVal;
    int AutoMinVal, AutoMaxVal;
    char intfName[64];
    static short *DisplayBuffPtr;

    printf("Testing A35 camera...\n\n");


    // Clear error flag
    error=0;

    // Initialize Camera
    if( Camera.InitCamera(&AcqWinWidth, &AcqWinHeight) == false) {
        exit(-73);
    }

    Camera.StartCamera();


    nbFrame = 0;
    //t1 = timeGetTime();

    long LastBuf = 0;
    bool Start = true;

    for(;;) {

        currBufNum = Camera.GetCameraFrame(m_nPhotonFrameBuffer);

        //*** Calculate the number of frames per seconds every 300 frames
        nbFrame++;
        if (nbFrame>=300) {
            //                t2 = timeGetTime();
            FramesPerSec = 1000.0 * (double)nbFrame / (double)(t2-t1);
            t1 = t2;
            nbFrame=0;
        }
    }
}
