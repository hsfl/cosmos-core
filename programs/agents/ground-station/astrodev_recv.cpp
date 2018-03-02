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

#include "device/astrodev/astrodev_lib.h"
#include "support/timelib.h"

astrodev_handle handle;
char device[15]="/dev/ttyUSB2";
char source[15], destination[15];

int main(int argc, char *argv[])
{
	int32_t iretn;
	uint8_t buf[500];
	double mean, lmjd;
	uint16_t count;

	if (argc == 2) strcpy(device,argv[1]);

	if ((iretn=astrodev_connect(device, &handle)) < 0)
	{
		printf("Failed to open astrodev on %s, error %d\n",device,iretn);
		exit (-1);
	}

	printf("Successful\n");

	/*
	if ((iretn=astrodev_gettcvconfig(&handle)) < 0)
	{
		printf("Failed to get Configuration\n");
		exit(-1);
	}

	printf("Configuration: TX %f RX %f\n",handle.frame.tcv.tx_freq_high*65535.+handle.frame.tcv.tx_freq_low, handle.frame.tcv.rx_freq_high*65535.+handle.frame.tcv.rx_freq_low);
	*/

	handle.frame.tcv.rx_modulation = ASTRODEV_MODULATION_GFSK;
	handle.frame.tcv.tx_modulation = ASTRODEV_MODULATION_GFSK;
	handle.frame.tcv.tx_freq_high = 440030/65536;
	handle.frame.tcv.tx_freq_low = 440030%65536;
	handle.frame.tcv.rx_freq_high = 440030/65536;
	handle.frame.tcv.rx_freq_low = 440030%65536;
	handle.frame.header.command = ASTRODEV_SETTCVCONFIG;
	handle.frame.header.size = sizeof(astrodev_tcv_config);
	if ((iretn=astrodev_sendframe(&handle)) < 0)
	{
		printf("Failed to settcvconfig astrodev, error %d\n",iretn);
	}

lmjd = currentmjd(0.);
count = 0;
while (count < 100)
{
	if ((iretn=astrodev_recvframe(&handle)) >= 0)
	{
		astrodev_unloadframe(&handle, buf, 500);
		mean = 0.;
		for (uint16_t j=0; j<handle.frame.header.size-20; ++j)
		{
			mean += buf[16+j];
		}
		mean /= handle.frame.header.size-20;
		++count;
		printf("[%f] Received #%u: %u bytes [%f]\n",86400.*(currentmjd(0.)-lmjd),count,handle.frame.header.size,mean);
		fflush(stdout);
		lmjd = currentmjd(0.);
	}
fflush(stdout);
}

}
