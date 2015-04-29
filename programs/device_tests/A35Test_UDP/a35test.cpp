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

#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define IMAGEWIDTH 256L
#define IMAGEHEIGHT 336L
#define IMAGESIZE (IMAGEWIDTH*IMAGEHEIGHT)
#define SIZE 1024
#define MAXPACKETSIZE 4096


#ifdef WIN32
SOCKET sdCommand, sdVideo, sdCommand2;                     /* The socket descriptor */
#else
static int sdCommand, sdVideo, sdCommand2;                     /* The socket descriptor */
#endif

struct sockaddr_in CamSdAddr3956;   /* Information about the server */
struct sockaddr_in CamSdAddr4;      /* Information about the server */
struct sockaddr_in CamSdAddr20202;  /* Information about the server */
int server_length;                  /* Length of server struct */

int a1, a2, a3, a4, a4Svr;          /* Server address components in xxx.xxx.xxx.xxx form */

unsigned char SequenceNum = 1;



long Ping(void)
{
    char send_buffer[SIZE];
    char packet[298];
    long bytes_recieved;

    // Ping
    send_buffer[0] = 0x42;
    send_buffer[1] = 0x01;
    send_buffer[2] = 0x00;
    send_buffer[3] = 0x02;
    send_buffer[4] = 0x00;
    send_buffer[5] = 0x00;
    send_buffer[6] = 0x50;
    send_buffer[7] = 0x02;

    /* Transmit message */
    if (sendto(sdCommand, send_buffer, 8, 0, (struct sockaddr *)&CamSdAddr3956, server_length) == -1) {
        return(-1);
    }

#ifdef WIN32
    bytes_recieved = recvfrom(sdCommand, packet, 298, 0, (struct sockaddr *)&CamSdAddr3956, &server_length);
#else
    bytes_recieved = recvfrom(sdCommand, packet, 298, 0, (struct sockaddr *)&CamSdAddr3956, (socklen_t*)&server_length);
#endif
    return(bytes_recieved);
}




long ReadReg(unsigned long RegNum, unsigned long *Value)
{
    char send_buffer[SIZE];
    char packet[60];
    long bytes_recieved;

    send_buffer[0] = 0x42;
    send_buffer[1] = 0x01;
    send_buffer[2] = 0x00;
    send_buffer[3] = 0x80;
    send_buffer[4] = 0x00;
    send_buffer[5] = 0x04;
    send_buffer[6] = SequenceNum>>8 & 0xFF;
    send_buffer[7] = SequenceNum & 0xFF;
    SequenceNum++;

    send_buffer[8] = RegNum>>24 & 0xFF;
    send_buffer[9] = RegNum>>16 & 0xFF;
    send_buffer[10] = RegNum>>8 & 0xFF;
    send_buffer[11] = RegNum&0xFF;


    /* Transmit message */
    if (sendto(sdCommand, send_buffer, 12, 0, (struct sockaddr *)&CamSdAddr3956, server_length) == -1) {
        return(-1);
    }

#ifdef WIN32
    bytes_recieved = recvfrom(sdCommand, packet, 60, 0, (struct sockaddr *)&CamSdAddr3956, &server_length);
#else
    bytes_recieved = recvfrom(sdCommand, packet, 60, 0, (struct sockaddr *)&CamSdAddr3956, (socklen_t*)&server_length);
#endif
    *Value = ((packet[8]&0xFF)<<24) + ((packet[9]&0xFF)<<16) + ((packet[10]&0xFF)<<8) + (packet[11]&0xFF);
    return(bytes_recieved);
}

void ReadReg2(unsigned long RegNum)
{
    char send_buffer[SIZE];
    //char packet[60];
    //long bytes_recieved;

    send_buffer[0] = 0x42;
    send_buffer[1] = 0x00;
    send_buffer[2] = 0x00;
    send_buffer[3] = 0x80;
    send_buffer[4] = 0x00;
    send_buffer[5] = 0x04;
    send_buffer[6] = 0;
    send_buffer[7] = 1;

    send_buffer[8] = RegNum>>24 & 0xFF;
    send_buffer[9] = RegNum>>16 & 0xFF;
    send_buffer[10] = RegNum>>8 & 0xFF;
    send_buffer[11] = RegNum&0xFF;


    /* Transmit message */
    if (sendto(sdCommand, send_buffer, 12, 0, (struct sockaddr *)&CamSdAddr3956, server_length) == -1) {
        return;
    }
}

//
void Request(unsigned long RegNum)
{
    char send_buffer[SIZE];
    //char packet[60];
    //long bytes_recieved;

    send_buffer[0] = 0x42;
    send_buffer[1] = 0x00;
    send_buffer[2] = 0x00;
    send_buffer[3] = 0x02;
    send_buffer[4] = 0x00;
    send_buffer[5] = 0x04;
    send_buffer[6] = 0xC0;
    send_buffer[7] = 0;

    send_buffer[8] = RegNum>>24 & 0xFF;
    send_buffer[9] = RegNum>>16 & 0xFF;
    send_buffer[10] = RegNum>>8 & 0xFF;
    send_buffer[11] = RegNum&0xFF;


    /* Transmit message */
    if (sendto(sdCommand, send_buffer, 12, 0, (struct sockaddr *)&CamSdAddr3956, server_length) == -1) {
        return;
    }
}

void Request2(unsigned long RegNum)
{
    char send_buffer[SIZE];
    //char packet[60];
    //long bytes_recieved;

    send_buffer[0] = 0x42;
    send_buffer[1] = 0x01;
    send_buffer[2] = 0x00;
    send_buffer[3] = 0x80;
    send_buffer[4] = 0x00;
    send_buffer[5] = 0x04;
    send_buffer[6] = 0;
    send_buffer[7] = 1;

    send_buffer[8] = RegNum>>24 & 0xFF;
    send_buffer[9] = RegNum>>16 & 0xFF;
    send_buffer[10] = RegNum>>8 & 0xFF;
    send_buffer[11] = RegNum&0xFF;

    for(int i=12 ; i<590 ; i++) {
        send_buffer[i] = 0;
    }

    /* Transmit message */
    if (sendto(sdVideo, send_buffer, 590, 0, (struct sockaddr *)&CamSdAddr20202, server_length) == -1) {
        return;
    }
}

int WriteReg(unsigned long RegNum, unsigned long Value)
{
    char send_buffer[SIZE];
    char packet[60];
    long bytes_recieved;

    send_buffer[0] = 0x42;
    send_buffer[1] = 0x01;
    send_buffer[2] = 0x00;
    send_buffer[3] = 0x82;
    send_buffer[4] = 0x00;
    send_buffer[5] = 0x08;
    send_buffer[6] = SequenceNum>>8 & 0xFF;
    send_buffer[7] = SequenceNum & 0xFF;
    SequenceNum++;

    send_buffer[8] = RegNum>>24 & 0xFF;
    send_buffer[9] = RegNum>>16 & 0xFF;
    send_buffer[10] = RegNum>>8 & 0xFF;
    send_buffer[11] = RegNum&0xFF;

    send_buffer[12] = Value>>24 & 0xFF;
    send_buffer[13] = Value>>16 & 0xFF;
    send_buffer[14] = Value>>8 & 0xFF;
    send_buffer[15] = Value & 0xFF;

    /* Transmit message */
    if (sendto(sdCommand, send_buffer, 16, 0, (struct sockaddr *)&CamSdAddr3956, server_length) == -1) {
        return(-1);
    }
#ifdef WIN32
    bytes_recieved = recvfrom(sdCommand, packet, 60, 0, (struct sockaddr *)&CamSdAddr3956, &server_length);
#else
    bytes_recieved = recvfrom(sdCommand, packet, 60, 0, (struct sockaddr *)&CamSdAddr3956, (socklen_t*)&server_length);
#endif
    return(bytes_recieved);
}

long ReadMem(unsigned long Address, unsigned long Size)
{
    char send_buffer[SIZE];
    char packet[MAXPACKETSIZE];
    long bytes_recieved;

    send_buffer[0] = 0x42;
    send_buffer[1] = 0x01;
    send_buffer[2] = 0x00;
    send_buffer[3] = 0x84;
    send_buffer[4] = 0x00;
    send_buffer[5] = 0x08;
    send_buffer[6] = SequenceNum>>8 & 0xFF;
    send_buffer[7] = SequenceNum & 0xFF;
    SequenceNum++;

    send_buffer[8] = Address>>24 & 0xFF;
    send_buffer[9] = Address>>16 & 0xFF;
    send_buffer[10] = Address>>8 & 0xFF;
    send_buffer[11] = Address & 0xFF;

    send_buffer[12] = Size>>24 & 0xFF;
    send_buffer[13] = Size>>16 & 0xFF;
    send_buffer[14] = Size>>8 & 0xFF;
    send_buffer[15] = Size & 0xFF;

    /* Transmit message */
    if (sendto(sdCommand, send_buffer, 16, 0, (struct sockaddr *)&CamSdAddr3956, server_length) == -1) {
        return(-1);
    }

#ifdef WIN32
    bytes_recieved = recvfrom(sdCommand, packet, Size+53, 0, (struct sockaddr *)&CamSdAddr3956, &server_length);
#else
    bytes_recieved = recvfrom(sdCommand, packet, Size+53, 0, (struct sockaddr *)&CamSdAddr3956, (socklen_t*)&server_length);
#endif
    return(bytes_recieved);
}

unsigned long GetCameraFrame(unsigned short *bufAddr)
{
    int i;
    //unsigned short *MyBuf;
    int bytes_recieved;
    //fd_set set;
    //struct timeval tv;
    //tv.tv_sec = 1;
    //tv.tv_usec = 0;
    char packet[1454];
    int FrameNum;
    static int FrameCount=0;
    //long FrameSequence;
    long PacketNum=0;
    long LastPacketNum;
    char *TempBuf;

    FrameCount++;
    if( (FrameCount%60) == 0 ) {
        WriteReg(0xA00, 2);
    }

    LastPacketNum = -1;

#ifdef WIN32
    bytes_recieved = recvfrom(sdVideo, packet, 590, 0, (struct sockaddr *)&CamSdAddr20202, &server_length);
    bytes_recieved = recvfrom(sdVideo, packet, 44, 0, (struct sockaddr *)&CamSdAddr20202, &server_length);
#else
    bytes_recieved = recvfrom(sdVideo, packet, 590, 0, (struct sockaddr *)&CamSdAddr20202, (socklen_t*)&server_length);
    bytes_recieved = recvfrom(sdVideo, packet, 44, 0, (struct sockaddr *)&CamSdAddr20202, (socklen_t*)&server_length);
#endif
    if(bytes_recieved!=44) {
        FrameNum = PacketNum;
    }

    PacketNum = packet[7];  //( (packet[48]&0xFF) << 8) + (packet[49]&0xFF);
    if(PacketNum!=LastPacketNum+1) {
        FrameNum = PacketNum;
    }
    LastPacketNum = PacketNum;

    printf("Pack=%ld\n", PacketNum);


    TempBuf = (char*)bufAddr;
    for(i=0 ; i<123 ; i++) {

        /* Receive 1/16th of the frame */
#ifdef WIN32
        bytes_recieved = recvfrom(sdVideo, packet, 1416, 0, (struct sockaddr *)&CamSdAddr20202, &server_length);
#else
        bytes_recieved = recvfrom(sdVideo, packet, 1416, 0, (struct sockaddr *)&CamSdAddr20202, (socklen_t*)&server_length);
#endif

        if(bytes_recieved==264) {
            for(int j=0 ; j<bytes_recieved-8 ; j++) {
                *TempBuf = packet[8+j];
                TempBuf++;
            }
            break;
        }

        for(int j=0 ; j<1408 ; j++) {
            *TempBuf = packet[8+j];
            TempBuf++;
        }

        PacketNum = packet[7]; //( (packet[48]&0xFF) << 8) + (packet[49]&0xFF);
        if(PacketNum!=LastPacketNum+1) {
            FrameNum = PacketNum;
        }
        LastPacketNum = PacketNum;

        printf("Pack=%ld\n", PacketNum);
    }



#ifdef WIN32
    bytes_recieved = recvfrom(sdVideo, packet, 16, 0, (struct sockaddr *)&CamSdAddr20202, &server_length);
#else
    bytes_recieved = recvfrom(sdVideo, packet, 16, 0, (struct sockaddr *)&CamSdAddr20202, (socklen_t*)&server_length);
#endif

    if(bytes_recieved!=136) {
        FrameNum = PacketNum;
    }



    PacketNum = packet[7]; //( (packet[48]&0xFF) << 8) + (packet[49]&0xFF);
    if(PacketNum!=LastPacketNum+1) {
        FrameNum = PacketNum;
    }
    LastPacketNum = PacketNum;

    printf("Pack=%ld\n", PacketNum);

    return(0);
}



void SetupSockAddr(sockaddr_in *SdAddr, int a1, int a2, int a3, int a4, int PortNum)
{
    char addr_string[16];
    sprintf( addr_string, "%d.%d.%d.%d", a1, a2, a3, a4);

    /* Setup server */
    memset((void *)SdAddr, '\0', sizeof(struct sockaddr_in));
    SdAddr->sin_family = AF_INET;
    SdAddr->sin_port = htons(PortNum);
#ifdef WIN32
    /* Set server address */
    SdAddr->sin_addr.S_un.S_un_b.s_b1 = (unsigned char)a1;
    SdAddr->sin_addr.S_un.S_un_b.s_b2 = (unsigned char)a2;
    SdAddr->sin_addr.S_un.S_un_b.s_b3 = (unsigned char)a3;
    SdAddr->sin_addr.S_un.S_un_b.s_b4 = (unsigned char)a4;
#else
    inet_pton(AF_INET, addr_string, &SdAddr->sin_addr);
#endif
}


// ***********************************************************
// ***********************************************************
// ***********************************************************
// ***********************************************************
// ***********************************************************
// ***********************************************************
int InitCamera(void)
{
#ifdef WIN32
    WSADATA w;                                  /* Used to open Windows connection */
#endif
    char host_name[256];                        /* Host name of this computer */
    struct hostent *hp;                         /* Information about the server */
    unsigned long Value, PacketSize;

    unsigned MyCmndPortNum;
    unsigned MyRcvVidPort;
    unsigned CamCmndPortNum = 3956;
    unsigned CamVidPortNum20202 = 20202;
    unsigned MyIpAddress;
    server_length = sizeof(struct sockaddr_in);

    printf("\n\nInit\n-------\n");



    // Open command port (for sending FFC commands
/*    MyIpAddress = 0xC0A80103;
    a1 = 192; a2 = 168; a3 = 1; a4 = 3;
    a1 = 192; a2 = 168; a3 = 1; a4Svr = 12;
*/
    // Open command port (for sending FFC commands
    MyIpAddress = 0xC0A80103;
    a1 = 192; a2 = 168; a3 = 1; a4 = 3;
    a1 = 192; a2 = 168; a3 = 1; a4Svr = 12;

    //sprintf( my_ip_string, "%d.%d.%d.%d", a1, a2, a3, a4);
    //sprintf( cam_ip_string, "%d.%d.%d.%d", a1, a2, a3, a4Svr);

#ifdef WIN32
    /* Open windows connection */
    if (WSAStartup(0x0101, &w) != 0)
    {
        fprintf(stderr, "Could not open Windows connection.\n");
        exit(0);
    }

    /* Get host name of this computer */
    gethostname(host_name, sizeof(host_name));
    hp = gethostbyname(host_name);

    /* Check for NULL pointer */
    if (hp == NULL) {
        //closesocket(sdCommand);
        WSACleanup();
        return(FALSE);
    }
#endif


    /* Setup Camera server */
    SetupSockAddr(&CamSdAddr3956, a1, a2, a3, a4Svr, CamCmndPortNum);
    SetupSockAddr(&CamSdAddr20202, a1, a2, a3, a4Svr, CamVidPortNum20202);



    /* Open command socket */
    sdCommand = socket(AF_INET, SOCK_DGRAM, 0);
    if (sdCommand < 0) {    // Fix for Windows
        fprintf(stderr, "Could not create command socket.\n");
#ifdef WIN32
        WSACleanup();
#endif
        return(false);
    }

    /* Open video socket */
    sdVideo = socket(AF_INET, SOCK_DGRAM, 0);
    if (sdVideo < 0) {    // Fix for Windows
        fprintf(stderr, "Could not create video socket.\n");
#ifdef WIN32
        WSACleanup();
#endif
        return(false);
    }




    ReadReg(0xA00, &Value);	// Get packet size
    WriteReg(0xA00, 2);
    WriteReg(0x938, 0x1388);

    ReadReg(0x8, &Value);
    ReadReg(0xC, &Value);
    ReadReg(0x0, &Value);
    ReadReg(0x4, &Value);
    ReadReg(0x934, &Value);
    ReadReg(0x904, &Value);
    ReadReg(0x900, &Value);
    ReadReg(0x930, &Value);
    ReadReg(0x92C, &Value);
    ReadReg(0xB1C, &Value);
    ReadReg(0xD1C, &Value);
    ReadReg(0x940, &Value);
    ReadReg(0x93C, &Value);
    ReadReg(0xA00, &Value);

    ReadMem(0x200, 512);
    ReadMem(0x400, 512);

    long MemLocation = 0x3FFF8000;

    for( int i=0 ; i<78 ; i++ ) {
        ReadMem(MemLocation, 536);
        MemLocation+=536;
    }
    ReadMem(MemLocation, 388);

    ReadReg(0x10, &Value);
    ReadReg(0xB8F0, &Value);
    ReadReg(0xA05C, &Value);
    ReadReg(0, &Value);
    ReadReg(0x930, &Value);
    ReadReg(0x92C, &Value);
    ReadReg(0xB8FC, &Value);
    ReadReg(0xA070, &Value);
    ReadReg(0xB9C4, &Value);
    ReadReg(0xD1AC, &Value);
    ReadReg(0xA06C, &Value);
    ReadReg(0xE968, &Value);
    ReadReg(0xE9A8, &Value);
    ReadReg(0x900, &Value);

    WriteReg(0xB14, 0x190);	//400
    ReadReg2(0xA00);
    WriteReg(0xB18, 0x3);
    WriteReg(0xB10, MyIpAddress);
    ReadReg(0xB00, &Value);
    WriteReg(0xB00, 0xE82D);	//59437

    ReadReg(0xD04, &PacketSize);	// Get packet size
    WriteReg(0xD04, 0x40001FE4);	//

    Request(0xA00);

    WriteReg(0xD18, MyIpAddress);
    ReadReg(0xD00, &Value);
    //WriteReg(0xD00, MyRcvPortNum2);

    ReadReg(0xD04, &Value);
    WriteReg(0xD04, 0x40001FE4);
    WriteReg(0xD04, 0x40001FE4);
    WriteReg(0xD04, 0xC0001FE4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0x40001FE4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0x40000FE4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0x40000FE4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0xC0000FE4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0x400007E4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0x400007E4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0xC00007E4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0x400003E4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0x400003E4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0xC00003E4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0x400005A4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0x400005A4);
    ReadReg(0xD04, &Value); WriteReg(0xD04, 0xC00005A4);

    ReadReg(0xD00, &Value);
    WriteReg(0xD00, 0);
    WriteReg(0xD18, 0); // Stop Camera?

    WriteReg(0xE984, 3);            // Set to 14 bit mode
    WriteReg(0xD308, 0x01100025);   // Set pixels to 14 bit


    // Set width & height of camera image

    // Set shutter to manual

    // Set frame rate 30 Hz

    // Take a shutter (wait a sec)

    // Set XPMode to CMOS

    // Set LVDSMode to disabled

    // (Get Device Reset settings and write settings commands)
    // (Get Device Reset commands)

    // Close / Open shutter

    // *****************************************************************************
    // *****************************************************************************
    // **********************  Second Port for Data      ***************************
    // *****************************************************************************
    // *****************************************************************************

    //Ping();
    Ping();
    Request2(0xA00);

    int iretn;
    socklen_t namelen = sizeof(sockaddr_in);
    if ((iretn = getsockname(sdVideo, (sockaddr*) &CamSdAddr20202, &namelen)) == -1) {
        return (-1);
    }

    MyRcvVidPort = ntohs(CamSdAddr20202.sin_port);
    printf("Port=%d\n" , MyRcvVidPort);


    WriteReg(0xD18, MyIpAddress);
    ReadReg(0xD00, &Value);
    WriteReg(0xD00, MyRcvVidPort);
    ReadReg(0x310, &Value);
    ReadReg(0xB8FC, &Value);
    ReadReg(0xB8FC, &Value);    
    ReadReg(0xA00, &Value);


    // Probably don't need all these
/*    ReadReg(0xA00, &Value);
    ReadReg(0xA00, &Value);
    ReadReg2(0xA00);
    ReadReg(0xA00, &Value);
    ReadReg(0xA00, &Value);
    Request2(0xA00);
    ReadReg(0xA00, &Value);
    ReadReg2(0xA00);*/


    // Start Grabbing Frames
    WriteReg(0xD314, 0x1);

    return(true);
}



int main(int argc, char *argv[])
{
    FILE *FP;
    unsigned short bufAddr[IMAGESIZE*2];
    printf("usage: a35grab NumFrames Camera_IP My_IP filename\n\n");

#ifdef WIN32
    char FilePath[512]={"C:/source/TestImage.bin"};
#else
    //char FilePath[512]={"/home/flight/source/TestImage.bin"};
    char FilePath[512]={"TestImage.bin"};
#endif


    printf("Start!\n------\n");

    InitCamera();
    GetCameraFrame(bufAddr);
    // Stop Camera();


    if ((FP = fopen( FilePath, "wb" )) == NULL ) {
        printf( "fopen failed!");
    } else {
        fwrite(bufAddr, 2, IMAGESIZE, FP);
        fclose(FP);
    }
}
