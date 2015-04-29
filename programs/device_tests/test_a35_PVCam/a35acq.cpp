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
//  Ca35acq.cpp - Implementation
//
//////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdio.h>
#include <memory.h>
#include "a35acq.h"

#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvStreamRaw.h>


PvDeviceInfo* lDeviceInfo;


// Overload the FinderWindow Class
class MyFinder : public PvDeviceFinderWnd
{
public:
    explicit MyFinder (): PvDeviceFinderWnd() {}
    virtual bool OnFound(PvDeviceInfo *aDI);
};

bool MyFinder::OnFound(PvDeviceInfo *aDI)
{
    // Put the device info into a global class
    lDeviceInfo = aDI;
    return(true);
}


// Create a GEV Device finder dialog
//PvDeviceFinderWnd lDeviceFinderWnd;
MyFinder lDeviceFinderWnd;

// device parameters need to control streaming
PvGenParameterArray *lDeviceParams;
PvGenInteger *lTLLocked;
PvGenInteger *lPayloadSize;
PvGenCommand *lStart;
PvGenCommand *lStop;

// stream parameters/stats
PvGenParameterArray *lStreamParams;
PvGenInteger *lCount;
PvGenFloat *lFrameRate;
PvGenFloat *lBandwidth;

PvPipeline *lPipeline;
PvDevice lDevice;

// Create the PvStream object
PvStream lStream;

char lDoodle[8];
int lDoodleIndex=0;
PvInt64 lImageCountVal;
double lFrameRateVal;
double lBandwidthVal;
PvInt64 lSize;


//////////////////////////////////////////////////////////////////////
// Ca35acq constructor
Ca35acq::Ca35acq()
{
}

//////////////////////////////////////////////////////////////////////

Ca35acq::~Ca35acq()
{
    delete(lPipeline);
}

//////////////////////////////////////////////////////////////////////

int Ca35acq::InitCamera(unsigned int *width, unsigned int *height)
{
    // MUST return width and height.
    *width = 320;
    *height=240;
    // -----------------------------

    // Select a GEV Device
    lDeviceFinderWnd.ShowModeless();

    // Wait to find device.
    while( lDeviceInfo == NULL ) {
        ;

    lDeviceFinderWnd.Close();

    lDevice.SetIPConfiguration( lDeviceInfo->GetIPAddress(),
                                                        lDeviceInfo->GetMACAddress(),
                                                        lDeviceInfo->GetSubnetMask(),
                                                        lDeviceInfo->GetDefaultGateway());

    // Connect to the GEV Device
    if ( !lDevice.Connect( lDeviceInfo ).IsOK() ) {
        return false;	// Error
    }

    printf( "Successfully connected to %s\n", lDeviceInfo->GetMACAddress().GetAscii() );



    PvGenParameterArray* lParameters = lDevice.GetGenParameters();

    // ************** WIDTH ****************
    // Get width parameter - mandatory GigE Vision parameter, it should be there
    PvGenParameter *lParameter1 = lParameters->Get( "Width" );

    // Converter generic parameter to width using dynamic cast. If the
    // type is right, the conversion will work otherwise lWidth will be NULL
    PvGenInteger *lWidthParameter = dynamic_cast<PvGenInteger *>( lParameter1 );

    // Change width value
    PvInt64 lNewWidth = *width;		// 320 pixels
    if ( !lWidthParameter->SetValue( lNewWidth ).IsOK() )
    {
        return false;	// Error
    }


    // ************** HEIGHT ****************
    // Set height parameter - mandatory GigE Vision parameter, it should be there
    PvGenParameter *lParameter2 = lParameters->Get( "Height" );

    // Converter generic parameter to height using dynamic cast. If the
    // type is right, the conversion will work otherwise lHeight will be NULL
    PvGenInteger *lHeightParameter = dynamic_cast<PvGenInteger *>( lParameter2 );

    // Change width value
    PvInt64 lNewHeight = *height;	// 240 pixels
    if ( !lHeightParameter->SetValue( lNewHeight ).IsOK() )
    {
        return false;	// Error
    }
#define FRAMESKIP
#ifdef FRAMESKIP
    // Skip every other frame
    PvGenParameter *lParameter0 = lParameters->Get( "GrbCh0TrigCfgFrameToSkip" );

    // Converter generic parameter to height using dynamic cast. If the
    // type is right, the conversion will work otherwise lSkipParameter will be NULL
    PvGenInteger *lSkipParameter = dynamic_cast<PvGenInteger *>( lParameter0 );

    PvInt64 lNewSkip = 1;	// Skip 1 frame
    if ( !lSkipParameter->SetValue( lNewSkip ).IsOK() ) {
        return false;	// Error
    }
#endif


    // ************** PIXEl FORMAT ****************
    // Set pixel format parameter - mandatory GigE Vision parameter, it should be there
    PvGenParameter *lParameter3 = lParameters->Get( "PixelFormat" );

    // Converter generic parameter to width using dynamic cast. If the
    // type is right, the conversion will work otherwise lPixelFormat will be NULL
    PvGenEnum *lPixelFormatParameter = dynamic_cast<PvGenEnum *>( lParameter3 );

    // Change width value
    PvString lNewPixelFormat = "Mono14";
    if ( !lPixelFormatParameter->SetValue( lNewPixelFormat ).IsOK() )
    {
        return false;	// Error
    }


    // ************** TEST PATTERN ****************
    // Set pixel format parameter - mandatory GigE Vision parameter, it should be there
    PvGenParameter *lParameter4 = lParameters->Get( "TestImageSelector" );

    // Converter generic parameter to width using dynamic cast. If the
    // type is right, the conversion will work otherwise lPixelFormat will be NULL
    PvGenEnum *lTsetPatternParameter = dynamic_cast<PvGenEnum *>( lParameter4 );

    // Change width value
    PvString lNewTestPattern = "Off";
    if ( !lTsetPatternParameter->SetValue( lNewTestPattern ).IsOK() )
    {
        return false;	// Error
    }





    // Get device parameters need to control streaming
    lDeviceParams = lDevice.GetGenParameters();
    lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
    lPayloadSize = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "PayloadSize" ) );
    lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
    lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

    // Get stream parameters/stats
    lStreamParams = lStream.GetParameters();
    lCount = dynamic_cast<PvGenInteger *>( lStreamParams->Get( "ImagesCount" ) );
    lFrameRate = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "AcquisitionRateAverage" ) );
    lBandwidth = dynamic_cast<PvGenFloat *>( lStreamParams->Get( "BandwidthAverage" ) );


    // Negotiate streaming packet size
    lDevice.NegotiatePacketSize();


    // Open stream - have the PvDevice do it for us
    printf( "Opening stream to device\n" );
    lStream.Open( lDeviceInfo->GetIPAddress() );

    // Create the PvPipeline object
    lPipeline = new PvPipeline( &lStream );

    // Reading payload size from device
    lSize = 0;
    lPayloadSize->GetValue( lSize );

    // Set the Buffer size and the Buffer count
    lPipeline->SetBufferSize( static_cast<PvUInt32>( lSize ) );
    lPipeline->SetBufferCount( 16 ); // Increase for high frame rate without missing block IDs

    // Have to set the Device IP destination to the Stream
    lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );


    return(true);
}

//////////////////////////////////////////////////////////////////////

unsigned long Ca35acq::GetCameraFrame(unsigned short *bufAddr)
{
    unsigned short *MyBuf;

    char lDoodle[] = "|\\-|-/";
    //lDoodleIndex = 0;
    lImageCountVal = 0;
    lFrameRateVal = 0.0;
    lBandwidthVal = 0.0;

    // Acquire image
    // Retrieve next buffer
    PvBuffer *lBuffer = NULL;
    PvResult  lOperationResult;
    PvResult lResult = lPipeline->RetrieveNextBuffer( &lBuffer, 1000, &lOperationResult );

    if ( lResult.IsOK() )
    {
        if ( lOperationResult.IsOK() )
        {
            lCount->GetValue( lImageCountVal );
            lFrameRate->GetValue( lFrameRateVal );
            lBandwidth->GetValue( lBandwidthVal );

            printf( "%c Timestamp: %016llX BlockID: %04X %.01f FPS %.01f Mb/s\r",
                    lDoodle[ lDoodleIndex ],
                    lBuffer->GetTimestamp(),
                    lBuffer->GetBlockID(),
                    lFrameRateVal,
                    lBandwidthVal / 1024 / 1024 );
        }
        // We have an image - do some processing (...) and VERY IMPORTANT,

        MyBuf = (unsigned short*)lBuffer->GetDataPointer();

        memcpy(bufAddr, MyBuf, 320*240*sizeof(unsigned short));
        /*
            for(long j=0 ; j<240 ; j++) {
            for(long i=0 ; i<0 ; i++) {
                bufAddr[j*320+i] = MyBuf[j*320+i];
            }
            for(long i=0 ; i<319 ; i++) {
                bufAddr[j*320+i] = 0;
            }
            }
*/
        //memset(bufAddr, 0, 320*240*sizeof(unsigned int) );
        /*			for(long j=0 ; j<240 ; j++) {
            for(long i=0 ; i<5 ; i++) {
                bufAddr[j*320+i] = MyBuf[j*320+i];
            }
            }
*/
        /*			for(long i=0 ; i<5 ; i++) {
                bufAddr[i] = MyBuf[i];
            }
*/

        // release the buffer back to the pipeline
        lPipeline->ReleaseBuffer( lBuffer );
    }
    else
    {
        // Timeout
        printf( "%c Timeout\r", lDoodle[ lDoodleIndex ] );
    }

    ++lDoodleIndex %= 6;

    return(0);
}


//////////////////////////////////////////////////////////////////////

void Ca35acq::StartCamera()
{
    // IMPORTANT: the pipeline needs to be "armed", or started before
    // we instruct the device to send us images
    //printf( "Starting pipeline\n" );
    lPipeline->Start();


    // TLParamsLocked is optional but when present, it MUST be set to 1
    // before sending the AcquisitionStart command
    if ( lTLLocked != NULL )
    {
        //printf( "Setting TLParamsLocked to 1\n" );
        lTLLocked->SetValue( 1 );
    }

    //printf( "Resetting timestamp counter...\n" );
    PvGenCommand *lResetTimestamp = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "GevTimestampControlReset" ) );
    lResetTimestamp->Execute();

    // The pipeline is already "armed", we just have to tell the device
    // to start sending us images
    //printf( "Sending StartAcquisition command to device\n" );
    PvResult lResult = lStart->Execute();
}

//////////////////////////////////////////////////////////////////////

int Ca35acq::StopCamera()
{
    //_getch(); // Flush key buffer for next stop
    //printf( "\n\n" );

    // Tell the device to stop sending images
    //printf( "Sending AcquisitionStop command to the device\n" );
    lStop->Execute();

    // If present reset TLParamsLocked to 0. Must be done AFTER the
    // streaming has been stopped
    if ( lTLLocked != NULL )
    {
        //printf( "Resetting TLParamsLocked to 0\n" );
        lTLLocked->SetValue( 0 );
    }

    // We stop the pipeline - letting the object lapse out of
    // scope would have had the destructor do the same, but we do it anyway
    //printf( "Stop pipeline\n" );
    lPipeline->Stop();

    // Now close the stream. Also optionnal but nice to have
    //printf( "Closing stream\n" );
    lStream.Close();

    // Finally disconnect the device. Optional, still nice to have
    //printf( "Disconnecting device\n" );
    lDevice.Disconnect();
    return(0);
}


//////////////////////////////////////////////////////////////////////

bool Ca35acq::ResetCamera()
{
    return(true);
}


//////////////////////////////////////////////////////////////////////

bool Ca35acq::AutoFFC()
{
    return(true);
}

//////////////////////////////////////////////////////////////////////

bool Ca35acq::ManualFFC()
{
    return(true);
}

//////////////////////////////////////////////////////////////////////

bool Ca35acq::DoFFC()
{
    return(true);
}
