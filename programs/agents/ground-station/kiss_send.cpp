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

#include "device/general/kisslib.h"
#include "support/timelib.h"
#include "device/serial/serialclass.h"

static KissHandle handle("DESTIN", "SOURCE");
static Serial *shandle;
static string device ="/dev/tty_tnc";
static string source;
static string destination;
static vector <uint8_t> message;
size_t count = 254;

int main(int argc, char *argv[])
{
    int32_t iretn;
	
    switch (argc)
    {
    case 3:
        count = atol(argv[2]);
    case 2:
        device = argv[1];
    case 1:
        break;
    default:
        printf("Usage: kiss_send [device [count]]\n");
        exit(-1);
        break;
    }

    shandle = new Serial(device, 19200);
    if (!shandle->get_open())
	{
        printf("Failed to open Serial Channel on %s, error %d\n", device.c_str(), shandle->get_error());
		exit (-1);
	}
//    handle = new KissHandle("DESTIN", "SOURCE");

    if (count > 254)
    {
        count = 254;
    }
	for (uint16_t i=0; i<count; ++i)
	{
        message.clear();
        message.push_back(0x10);
        for (uint16_t j=1; j<=count; ++j)
		{
            message.push_back(i%256);
		}
        handle.set_data(message);
        handle.load_packet();
        if ((iretn=shandle->put_data(handle.get_slip_packet())) < 0)
		{
			printf("Failed to send frame, error %d\r",iretn);
		}
		else
		{
            printf("[%u] Sent %ld bytes\n", i, handle.get_slip_packet().size());
		}
		COSMOS_USLEEP(2000000);
	}

}
