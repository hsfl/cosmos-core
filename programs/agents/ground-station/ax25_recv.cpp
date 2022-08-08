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
#include "support/configCosmos.h"
#include "support/jsondef.h"
#include "device/general/ax25class.h"
#include "support/timelib.h"
#include "device/serial/serialclass.h"
#include "support/socketlib.h"
#include "support/elapsedtime.h"
#include "support/stringlib.h"

static Ax25Handle handle;
static string device = "127.0.0.1:10001";
static string device_addr = "127.0.0.1";
static uint16_t device_port = 10003;
static string source;
static string destination;
static vector <uint8_t> message;

int main(int argc, char *argv[])
{
    PORT_TYPE device_type;
    Serial *shandle = nullptr;
    socket_channel schannel;

    int32_t iretn = 0;
    size_t tcount = 2000;

    switch (argc)
    {
    case 3:
        tcount = static_cast <size_t>(atol(argv[2]));
    case 2:
        device = argv[1];
        if (device.find(':') != string::npos)
        {
            device_type = PORT_TYPE_ETHERNET;
            device_port = static_cast <uint16_t>(stoi(device.substr(device.find(':')+1)));
            device_addr = device.substr(0, device.find(':'));
            iretn = socket_open(&schannel, NetworkType::UDP, "", device_port, SOCKET_LISTEN, true, 5000000);
        }
        else
        {
            device_type = PORT_TYPE_RS232;
            device_addr = device;
            shandle = new Serial(device_addr, 19200);
            shandle->set_timeout(5.);
        }
        break;
    default:
        printf("Usage: ax25_recv [device [tcount]]\n");
        exit(-1);
    }



    for (size_t i=0; i<tcount; ++i)
    {
        ElapsedTime et;
        vector <uint8_t> packet;

        if (device_type == PORT_TYPE_ETHERNET)
        {
            iretn = socket_recvfrom(schannel, packet, 1500);
        }
        else
        {
            iretn = shandle->get_data(packet);
        }
        if (iretn > 0)
        {
            handle.set_ax25_packet(packet);
            handle.unload();
            printf("Packet: [%s %u %s %u %u %u] ", handle.get_destination_callsign().c_str(), handle.get_destination_stationID(), handle.get_source_callsign().c_str(), handle.get_source_stationID(), handle.get_control(), handle.get_protocolID());
            printf("%s\n", to_hex_string(handle.get_data(), true).c_str());
        }
//        else
//        {
//            printf("Timeout: [%d] %s\n", iretn, cosmos_error_string(iretn).c_str());
//        }

        fflush(stdout);
    }
}
