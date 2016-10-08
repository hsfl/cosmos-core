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

#include "device/general/kpc9612p_lib.h"
#include "support/timelib.h"

kpc9612p_handle handle;
char device[15]="/dev/ttyUSB0";
char source[15], destination[15];
uint8_t message[300];

int main(int argc, char *argv[])
{
	int32_t iretn;
//	double lmjd;
	
	if (argc == 2) strcpy(device,argv[1]);

	if ((iretn=kpc9612p_connect(device, &handle, 0x00)) < 0)
	{
		printf("Failed to open KPC9612P on %s, error %d\n",device,iretn);
		exit (-1);
	}

//	lmjd = currentmjd(0.);
	handle.frame.size = 255;
	for (uint16_t i=0; i<2000; ++i)
	{

		for (uint16_t j=1; j<=handle.frame.size; ++j)
		{
			message[j] = i%256;
		}
		message[0] = 0x10;
		if ((iretn=cssl_putslip(handle.serial, message, handle.frame.size+1)) < 0)
		{
			printf("Failed to send frame, error %d\r",iretn);
		}
		else
		{
			printf("[%u] Sent %d bytes\n",i,handle.frame.size);
		}
		COSMOS_USLEEP(2000000);
//		lmjd = currentmjd(0.);
	}
}
