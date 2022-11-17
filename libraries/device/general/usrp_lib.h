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


#ifndef USRP_LIB_H
#define USRP_LIB_H

#include <mutex>
#include "support/configCosmos.h"
#include "support/socketlib.h"
#include "support/stringlib.h"

#define USRP_BAUD 19200
#define USRP_BITS 8
#define USRP_PARITY 0
#define USRP_STOPBITS 1

#define USRP_CHANNEL_A 0
#define USRP_CHANNEL_B 1
#define USRP_CHANNEL_SWAP 2

#define USRP_DATAMODE_OFF 0
#define USRP_DATAMODE_ON 1

#define USRP_9600MODE_OFF 0
#define USRP_9600MODE_ON 1

#define USRP_MODE_LSB 0
#define USRP_MODE_USB 1
#define USRP_MODE_AM 2
#define USRP_MODE_CW 3
#define USRP_MODE_RTTY 4
#define USRP_MODE_FM 5
#define USRP_MODE_CWR 7
#define USRP_MODE_RTTYR 8
#define USRP_MODE_DV 17

#define USRP_FILTER_1 1
#define USRP_FILTER_2 2
#define USRP_FILTER_3 3

struct usrp_handle
{
    uint8_t record=0;
    uint8_t channelnum;
    socket_channel socket;
    uint16_t port;
    std::mutex mut;
    vector <uint8_t> response;
    uint8_t freqband;
    uint8_t filtband;
    uint8_t mode;
    uint8_t datamode;
    uint8_t bps9600mode;
    uint8_t rfgain;
    uint8_t squelch;
    uint8_t rfpower;
    uint8_t smeter;
    uint8_t rfmeter;
    uint8_t swrmeter;
    uint8_t alcmeter;
    uint8_t compmeter;
    uint8_t opmode;
    float bandpass;
    float powerin;
    float powerout;
    float maxpower;
    double frequency;
};

int32_t usrp_connect(string device, uint16_t port, usrp_handle &handle);
int32_t usrp_disconnect(usrp_handle &handle);
int32_t usrp_send(usrp_handle &handle, string &data);
uint8_t usrp_byte(vector <uint8_t> response);
uint8_t usrp_freq2band(double frequency);
//int32_t usrp_read(usrp_handle &handle, vector <uint8_t> &message);
int32_t usrp_check_address(usrp_handle &handle);
int32_t usrp_set_channel(usrp_handle &handle, uint8_t channelnum);
int32_t usrp_set_frequency(usrp_handle &handle, double frequency);
int32_t usrp_get_frequency(usrp_handle &handle);
int32_t usrp_set_freqband(usrp_handle &handle, uint8_t mode);
int32_t usrp_get_freqband(usrp_handle &handle);
int32_t usrp_set_bandpass(usrp_handle &handle, double bandpass);
int32_t usrp_get_bandpass(usrp_handle &handle);
int32_t usrp_set_mode(usrp_handle &handle, uint8_t opmode);
int32_t usrp_set_mode(usrp_handle &handle, uint8_t opmode, uint8_t filtband);
int32_t usrp_get_mode(usrp_handle &handle);
int32_t usrp_set_rfgain(usrp_handle &handle, uint8_t rfgain);
int32_t usrp_get_rfgain(usrp_handle &handle);
int32_t usrp_set_rfpower(usrp_handle &handle, float power);
int32_t usrp_get_rfpower(usrp_handle &handle);
int32_t usrp_set_squelch(usrp_handle &handle, uint8_t squelch);
int32_t usrp_get_squelch(usrp_handle &handle);
int32_t usrp_set_datamode(usrp_handle &handle, uint8_t mode);
int32_t usrp_get_datamode(usrp_handle &handle);
int32_t usrp_set_bps9600mode(usrp_handle &handle, uint8_t mode);
int32_t usrp_get_bps9600mode(usrp_handle &handle);
int32_t usrp_get_smeter(usrp_handle &handle);
int32_t usrp_get_rfmeter(usrp_handle &handle);
int32_t usrp_get_swrmeter(usrp_handle &handle);
int32_t usrp_get_alcmeter(usrp_handle &handle);
int32_t usrp_get_compmeter(usrp_handle &handle);
int32_t usrp_get_record(usrp_handle &handle);
int32_t usrp_set_record(usrp_handle &handle, uint8_t record);

#endif // USRP_LIB_H

