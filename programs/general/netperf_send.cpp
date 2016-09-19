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

#include "agent/agentclass.h"
#include "jsondef.h"
#include "sliplib.h"

#define INFO_SIZE 32

char address[20] = "127.0.0.1";
uint16_t bsize = 1500;
uint16_t delay = 1;
uint16_t packets = 10000;
float speed = 281250.;
double cmjd;

int main(int argc, char *argv[])
{
	bool error_injector_data_enable = false; // Choose to enable error in data section
	int error_injector_data_counter = 100; // Inject error starting at packet 100
	bool error_injector_runt_enable = false; // Choose to enable error in data section
	int error_injector_runt_counter = 150; // Inject error starting at packet 150
	bool error_injector_drop_enable = false; // Choose to enable error in data section
	int error_injector_drop_counter = 175; // Inject error starting at packet 175

	int32_t iretn;
	uint16_t count=0;
	socket_channel chan;
	uint8_t buf1[10000];
	char buf2[10000];
	int32_t lsleep;
	uint16_t nbytes;

	switch (argc)
	{
	case 6:
		delay = atoi(argv[5]);
	case 5:
		packets = atoi(argv[4]);
		if (packets > 9999)
		{
			packets = 9999;
		}
	case 4:	
		speed = atoi(argv[3]);
	case 3:
		bsize = atoi(argv[2]);
		if (bsize > 9999)
		{
			bsize = 9999;
		}
		if (bsize < INFO_SIZE)
		{
			bsize = INFO_SIZE;
		}
	case 2:
		strcpy(address,argv[1]);
		break;
	}

	if ((iretn=socket_open(&chan, NetworkType::UDP, address, 6101, AGENT_TALK, AGENT_BLOCKING, AGENTRCVTIMEO)) < 0)
	{
		printf("Unable to open connection to [%s:6101]\n",address);
	}

	COSMOS_USLEEP(delay*1000000);
	while (count < packets)
	{
		cmjd = currentmjd(0.);
		for (uint16_t i=0; i<bsize-INFO_SIZE; i++)
		{
#if defined(COSMOS_WIN_OS)
			buf1[i] = (char)rand();
#else
			buf1[i] = (char)random();
#endif
		}


		sprintf(buf2,"%.15g %4d %4x %4d ",currentmjd(0.), count++, slip_calc_crc((uint8_t *)buf1,bsize-INFO_SIZE), bsize);
		memcpy(&buf2[INFO_SIZE],(const char *)buf1,bsize-INFO_SIZE);

		// Added 20130726JC: Inject errors to test listener
		if (error_injector_data_counter-- == 0 && error_injector_data_enable)
		{
			buf2[500] = (uint8_t)0xf1;
			error_injector_data_counter = 100; // Error every 100 packets
		}
		if (error_injector_runt_counter-- == 0 && error_injector_runt_enable)
		{
			error_injector_runt_counter = 100; // Error every 100 packets
		}
		if (error_injector_drop_counter-- == 0 && error_injector_drop_enable)
		{
			// Skip packet
			error_injector_drop_counter = 100; // Error every 100 packets
			nbytes = 0;
		} 
		else
		{
			// Send packet
			nbytes = sendto(chan.cudp, (const char *)buf2, bsize, 0, (struct sockaddr *)&chan.caddr, sizeof(struct sockaddr_in));
		}
		lsleep =  1e6 * ((nbytes+28)/speed - 86400. * (currentmjd(0.) - cmjd));
		if (lsleep < 0) lsleep = 0;
		printf("[%6d: %4d] %6d\r", count, nbytes, lsleep);
		fflush(stdout);
		COSMOS_USLEEP(lsleep);
	}

}
