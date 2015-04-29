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

// #include <unistd.h>
#include "sinclair_lib.h"

int main(int argc, char *argv[])
{
int32_t iretn, i, j;
sinclair_state handle;
//char buf[256];
char device[15];

printf("About to connect");

for (uint16_t i=1; i<4; i++)
{
	sprintf(device,"/dev/ttyUSB%u",i);
	if ((iretn = sinclair_rw_connect(device,0x11,0x3e, &handle)) >= 0)
	{
		if ((iretn = nsp_ping(&handle)) >= 0) break;
	}
}

if (iretn < 0)
	{
	printf("Error: sinclair_rw_connect() %d\n",iretn);
	exit (1);
	}

printf("Connected");

printf("Ping: iretn=%d buf=%s\n",iretn,(char *)handle.mbuf.data);


for (i=1; i<4; i++)
	{
	iretn = sinclair_mode_speed(&handle,i*50.);
	for (j=0; j<3; j++)
		{
		printf("%.5f\r",sinclair_get_speed(&handle));
		fflush(stdout);
		COSMOS_SLEEP(1);
		}
	}
for (i=3; i>0; i--)
	{
	iretn = sinclair_mode_speed(&handle,i*50.);
	for (j=0; j<3; j++)
		{
		printf( "%.5f\r",sinclair_get_speed(&handle) );
		fflush(stdout);
		COSMOS_SLEEP(1);
		}
	}
iretn = sinclair_mode_speed(&handle,0.);
iretn = sinclair_disconnect(&handle);
}
