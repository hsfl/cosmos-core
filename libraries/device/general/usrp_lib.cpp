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

#include "device/general/usrp_lib.h"
#include "support/jsondef.h"

int32_t usrp_connect(string device, uint16_t port, usrp_handle &handle)
{
    int32_t iretn;

    iretn = socket_open(&handle.socket, NetworkType::UDP, device.c_str(), 10003, SOCKET_TALK);

    if (iretn < 0)
    {
        return (iretn);
    }

    handle.port = port;

    return iretn;
}

int32_t usrp_disconnect(usrp_handle &handle)
{
    int iretn;

    iretn = socket_close(&handle.socket);
    return iretn;
}

int32_t usrp_send(usrp_handle &handle, string data)
{
    int32_t iretn;

    iretn = socket_sendto(handle.socket, data);
    if (iretn >= 0)
    {
        iretn = socket_recvfrom(handle.socket, data, 100);
    }
    return iretn;
}

uint8_t usrp_byte(vector <uint8_t> response)
{
    uint8_t result = 0.;
    for (size_t i=0; i<2; ++i)
    {
        result *= 100.;
        result += 10. * (response[i] >> 4) + (response[i] % 16);
    }

    return result;
}

uint8_t usrp_freq2band(double frequency)
{
    uint8_t freqband;

    if (frequency < 1.8e6)
    {
        freqband = 14;
    }
    else if (frequency < 2.0e6)
    {
        freqband = 1;
    }
    else if (frequency >= 3.4e6 && frequency < 4.1e6)
    {
        freqband = 2;
    }
    else if (frequency >= 6.9e6 && frequency < 7.5e6)
    {
        freqband = 3;
    }
    else if (frequency >= 9.9e6 && frequency < 10.5e6)
    {
        freqband = 4;
    }
    else if (frequency >= 13.9e6 && frequency < 14.5e6)
    {
        freqband = 5;
    }
    else if (frequency >= 17.9e6 && frequency < 18.5e6)
    {
        freqband = 6;
    }
    else if (frequency >= 20.9e6 && frequency < 21.5e6)
    {
        freqband = 7;
    }
    else if (frequency >= 24.4e6 && frequency < 25.1e6)
    {
        freqband = 8;
    }
    else if (frequency >= 28.0e6 && frequency < 30.0e6)
    {
        freqband = 9;
    }
    else if (frequency >= 50.0e6 && frequency <= 54.0e6)
    {
        freqband = 10;
    }
    else if (frequency >= 108.0e6 && frequency <= 174.0e6)
    {
        freqband = 11;
    }
    else if (frequency >= 420.0e6 && frequency <= 480.0e6)
    {
        freqband = 12;
    }
    else if (frequency >= 1240.0e6 && frequency <1320.0e6)
    {
        freqband = 13;
    }
    else
    {
        freqband = 14;
    }
    return freqband;
}

int32_t usrp_get_frequency(usrp_handle &handle)
{
    int32_t iretn;

    string data = "get_downlink_freq";

    iretn = usrp_send(handle, data);

    return iretn;
}
int32_t usrp_set_frequency(usrp_handle &handle, double frequency)
{
    int32_t iretn = 0;

    string data = "downlink_freq" + to_double(handle.frequency);

    iretn = usrp_send(handle, data);
    if (iretn < 0)
    {
        return iretn;
    }

    if (frequency < 1.8e6)
    {
        handle.freqband = 14;
    }
    else if (frequency < 2.0e6)
    {
        handle.freqband = 1;
    }
    else if (frequency >= 3.4e6 && frequency < 4.1e6)
    {
        handle.freqband = 2;
    }
    else if (frequency >= 6.9e6 && frequency < 7.5e6)
    {
        handle.freqband = 3;
    }
    else if (frequency >= 9.9e6 && frequency < 10.5e6)
    {
        handle.freqband = 4;
    }
    else if (frequency >= 13.9e6 && frequency < 14.5e6)
    {
        handle.freqband = 5;
    }
    else if (frequency >= 17.9e6 && frequency < 18.5e6)
    {
        handle.freqband = 6;
    }
    else if (frequency >= 20.9e6 && frequency < 21.5e6)
    {
        handle.freqband = 7;
    }
    else if (frequency >= 24.4e6 && frequency < 25.1e6)
    {
        handle.freqband = 8;
    }
    else if (frequency >= 28.0e6 && frequency < 30.0e6)
    {
        handle.freqband = 9;
    }
    else if (frequency >= 50.0e6 && frequency <= 54.0e6)
    {
        handle.freqband = 10;
    }
    else if (frequency >= 108.0e6 && frequency <= 174.0e6)
    {
        handle.freqband = 11;
    }
    else if (frequency >= 420.0e6 && frequency <= 480.0e6)
    {
        handle.freqband = 12;
    }
    else if (frequency >= 1240.0e6 && frequency <1320.0e6)
    {
        handle.freqband = 13;
    }
    else
    {
        handle.freqband = 14;
    }
    handle.frequency = frequency;

    return 0;
}

