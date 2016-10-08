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
#include "support/jsondef.h"
#include "support/sliplib.h"

char address[] = "127.0.0.1";
uint16_t port = 6868;

uint16_t bsize = 1500;
uint16_t delay = 1;
uint32_t packets = 10000;
float speed = 281250.;
double cmjd, mjd;


int main(int argc, char *argv[])
{

	int32_t iretn;
	uint32_t count=0;
	socket_channel chan;
	char buf3[10000];
	int32_t lsleep, lat, lon, alt;
	uint16_t len3;
	int32_t hour, min;
	float sec;
	double imjd, elapsed;


    if ((iretn=socket_open(&chan, NetworkType::UDP, address, port, AGENT_TALK, AGENT_BLOCKING, AGENTRCVTIMEO)) < 0)
	{
		printf("Unable to open connection to [%s:6101]\n",address);
	}

    COSMOS_USLEEP(1*1000000);
	lat = 3705459;
	lon = -12083358;
	alt = 50;
	imjd = currentmjd(0.);

    while (true)
	{

        count++;
		mjd = currentmjd(0.);
		elapsed = 86400. * (mjd - imjd);
		mjd -= (int)mjd;
		hour =  mjd * 24;
		min = 1400 * (mjd - hour/24.);
		sec = 86400 * (mjd - (hour/24. + min/1400.));
        sprintf(buf3,"%02d%02d%02d%02d %7d %9d %d 1 100 293 293 27 0.09 0.00 1.01\n",hour,min,(int)sec,(int)(100*(sec-(int)sec)),(int)(lat+elapsed*2),(int)(lon+elapsed*1),(int)(alt+elapsed*5));
        len3 = strlen(buf3);

        // Send packet
		sendto(chan.cudp, (const char *)buf3, len3, 0, (struct sockaddr *)&chan.caddr, sizeof(struct sockaddr_in));

        printf("[%s]\r", buf3);
		fflush(stdout);
        lsleep = 1*1000000;
		COSMOS_USLEEP(lsleep);
	}

}
