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

astrodev_handle handle;
char device[15]="/dev/ttyUSB2";
char source[15], destination[15];

int main(int argc, char *argv[])
{
	int32_t iretn;
	uint8_t buf[500];

	if (argc == 2) strcpy(device,argv[1]);

    if ((iretn=astrodev_connect(device, handle)) < 0)
	{
		printf("Failed to open astrodev on %s, error %d\n",device,iretn);
		exit (-1);
	}

    if ((iretn=astrodev_ping(handle)) < 0)
	{
		printf("Failed to ping astrodev, error %d\n",iretn);
		exit (-1);
	}

	printf("Successful\n");

/*
    if ((iretn=astrodev_firmwarerev(handle)) < 0)
	{
		printf("Failed to firmwarerev astrodev, error %d\n",iretn);
	}
else
	{
	printf("Firmware Rev: %f\n",handle.frame.firmware.rev);
}
*/

    if ((iretn=astrodev_gettcvconfig(handle)) < 0)
	{
		printf("Failed to gettcvconfig astrodev, error %d\n",iretn);
	}
else
	{
	printf("Interface Baud: %d\n",9600<<handle.frame.tcv.interface_baud_rate);
	printf("Power Amp Level: %d\n",handle.frame.tcv.power_amp_level);
	printf("Receive Baud: %d Modulation: %u Frequency: %u\n",(handle.frame.tcv.rx_baud_rate?4800<<handle.frame.tcv.rx_baud_rate:1200),handle.frame.tcv.rx_modulation,(uint32_t)(65536*handle.frame.tcv.rx_freq_high+handle.frame.tcv.rx_freq_low));
	printf("Transmit Baud: %d Modulation: %u Frequency: %u\n",(handle.frame.tcv.tx_baud_rate?4800<<handle.frame.tcv.tx_baud_rate:1200),handle.frame.tcv.tx_modulation,(uint32_t)(65536*handle.frame.tcv.rx_freq_high+handle.frame.tcv.tx_freq_low));
	strncpy(source,(char *)handle.frame.tcv.ax25_source,6);
	source[6] = 0;
	strncpy(destination,(char *)handle.frame.tcv.ax25_destination,6);
	destination[6] = 0;
	printf("AX.25 Source: %s Destination: %s Preamble: %d Postamble: %d\n",source,destination,handle.frame.tcv.ax25_preamble_length,handle.frame.tcv.ax25_postamble_length);
	printf("Radio TX Baud: %d RX Baud: %d\n",(handle.frame.tcv.rx_baud_rate?4800<<handle.frame.tcv.rx_baud_rate:1200),(handle.frame.tcv.tx_baud_rate?4800<<handle.frame.tcv.tx_baud_rate:1200));
	}

	handle.frame.tcv.rx_modulation = ASTRODEV_MODULATION_GFSK;
	handle.frame.tcv.tx_modulation = ASTRODEV_MODULATION_GFSK;
	handle.frame.tcv.tx_freq_high = 440030/65536;
	handle.frame.tcv.tx_freq_low = 440030%65536;
	handle.frame.tcv.rx_freq_high = 440030/65536;
	handle.frame.tcv.rx_freq_low = 440030%65536;
    while ((iretn=astrodev_settcvconfig(handle)) < 0)
	{
		printf("Failed to settcvconfig astrodev, error %d\n",iretn);
		COSMOS_SLEEP(1);
	}

for (uint16_t i=0; i<255; ++i)
{
    if ((iretn=astrodev_telemetry(handle)) < 0)
	{
		printf("Failed to telemetry astrodev, error %d\n",iretn);
		COSMOS_SLEEP(1);
	}
else
	{

	printf("[%u] Op Counter: %hu, MSP430 Temp: %hd, ",i,handle.frame.telemetry.op_counter,handle.frame.telemetry.msp430_temp);
	printf("Time Count: %hu %hu %hu, RSSI: %hu, ",handle.frame.telemetry.time_rssi[0],handle.frame.telemetry.time_rssi[1],handle.frame.telemetry.time_rssi[2],handle.frame.telemetry.time_rssi[3]);
	printf("Bytes RX: %u, Bytes TX: %u\n",handle.frame.telemetry.bytes_rx,handle.frame.telemetry.bytes_tx);

		for (uint16_t j=0; j<255; ++j)
		{
			buf[j] = i%256;
		}
        if ((iretn=astrodev_transmit(handle, buf, 255)) < 0)
		{
			printf("Failed to transmit, error %d\n",iretn);
		}

	}
COSMOS_USLEEP(350000); // Orig 500k
  // testing done 20140428JC, two AstroDev Li-1 radios back to back toward ISC
  // working combo: 200k sleep, 25 size (18+25)=43, DataRate = 2150bps
  // Working combo: 500k, 255 size (18+255)=273, 5460bps, 100% success
  // Working combo: 400k, 255 size (18+255)=273, 6825bps, 97% success
  // Working combo: 380k, 255 size (18+255)=273, 7184bps, 99% success
  // Working combo: 360k, 255 size (18+255)=273, 7583bps, 99% success
  // Working combo: 350k, 255 size (18+255)=273, 7800bps, 100% success, perfect continuous xmit
  // Working combo: 340k, 255 size (18+255)=273, 8029bps, 90% success, radio timeouts
  // Nonworking: 320k 255 size, failed to get some responses from local radio
  // maybe 75% working combo: 350k, 255 size (18+255 size)=273, 7800bps ~85% success
  // 
  // Assuming best working combo, 350k, 255 size, 7800bps: HEC+AX.25 overhead = 18+10 = 28
  //  - User data rate: (273-28)/273*7800/8 = 875 bytes/sec = 7kbps
}

}
