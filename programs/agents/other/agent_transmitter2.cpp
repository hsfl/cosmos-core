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

// code from testbed/blank_agent.cpp

#include "support/configCosmos.h"

//#ifdef COSMOS_MAC_OS
//#include <sys/param.h>
//#include <sys/mount.h>
//#else
//#include <sys/vfs.h>
//#endif // COSMOS_MAC_OS

#include <stdio.h>

#include "agent/agentclass.h"
#include "physics/physicslib.h" // long term we may move this away
#include "support/jsonlib.h"
//#include "support/stringlib.h"
//#include "support/timelib.h"

// Added 20130223JC: Libraries
#include <cstring>
#include "device/general/kisslib.h"
//#include "rs232_lib.h"
#include "device/general/cssl_lib.h"
#define SERIAL_USB0 16
#define SERIAL_USB1 17

int agent_transmitter2();

char agentname[COSMOS_MAX_NAME+1] = "gs_tx2";
char node[50] = "otb";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int32_t transmit_kiss(char *request, char* response, Agent *); // extra request

Agent *agent; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffe for agents

#define REQUEST_RUN_PROGRAM 0 // mst check

// Here are some variables we will map to JSON names
int32_t diskfree;
int32_t stateflag;
int32_t myport;
cssl_t *handle;

int main(int argc, char *argv[])
{
	int32_t iretn;

	// Check for other instance of this agent
	if (argc == 2)
		strcpy(node,argv[1]);

	// check if we are already running the agent
    if ((iretn=agent->get_server((char *)node,agentname,waitsec,(beatstruc *)nullptr)) > 0)
		exit (iretn);

	// Initialization stuff

	// RS232 Library Initialization
//	RS232_OpenComport(SERIAL_USB1,19200); // 16=ttyUSB0, 19200bps
	handle = cssl_open("/dev/", 19200, 8, 0, 1);


	// Initialize the Agent
	// near future: support cubesat space protocol
	// port number = 0 in this case, automatic assignment of port
    if (!(agent = new Agent(node, agentname, 1., MAXBUFFERSIZE)))
		exit (iretn);

	// Add additional requests
    if ((iretn=agent->add_request("tx",transmit_kiss)))
		exit (iretn);

	// Start our own thread
    iretn = agent_transmitter2();
}

int agent_transmitter2()
{
	//FILE *fp;

	// Start performing the body of the agent
    while(agent->running())
	{
		unsigned int rxcount;
		unsigned char serial_input[1000];

		// Read Serial Port(s)
//		rxcount = RS232_PollComport(SERIAL_USB1, serial_input, 1000);
		rxcount = cssl_getdata(handle, serial_input, 1000);

		// Print Inbound Characters if New Input Found
		if (rxcount)
		{
			printf("RX: ");
			for(uint32_t i=0;i < rxcount;i++)
			{
				printf("%c",serial_input[i]);
			}
			printf("\n");
		}



		COSMOS_USLEEP(100000); // no support in win, 100ms reporting interval
	}
	return 0;
}

// the name of this fn will always be changed
int32_t transmit_kiss(char *request, char* response, Agent *)
{
	int32_t iretn = 0;
	unsigned char packet_buffer[600]; // w/c count will be 529 bytes (18+1+255*2)
	int payload_size; // Packet Byte Count
	int input_size; // Request Input Byte Count

	request += 3;  // Advance pointer to skip "tx ", transmit the rest

	printf("Received Request");

	// Check Input Length
	input_size = strlen(request); // Get size of null terminated string
	printf(".");
	/*
if(input_size == -1 || input_size > 255)
{
  memcpy(response,"Error: Input larger than 255 maximum", 36);
  iretn=1;
  return iretn;
}
*/

	// Encode KISS Packet
	payload_size = kissEncode((unsigned char *)request,input_size,packet_buffer);

	// Test Print
	printf("TX: ");
	for(int z=0;z<payload_size;z++)
	{
		printf("%02x ", packet_buffer[z]);
	}
	printf("\n\n");


//	RS232_SendBuf(SERIAL_USB1,packet_buffer,payload_size);
	cssl_putdata(handle, packet_buffer, payload_size);


	//#printf("Transmitting: %s\n",request+3);


	return iretn;
}
