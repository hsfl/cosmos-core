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


#ifndef IC9100_LIB_H
#define IC9100_LIB_H

#include "support/configCosmos.h"
#include "device/general/cssl_lib.h"

#define IC9100_BAUD 19200
#define IC9100_BITS 8
#define IC9100_PARITY 0
#define IC9100_STOPBITS 1

#define IC9100_CHANNEL_A 0
#define IC9100_CHANNEL_B 1
#define IC9100_CHANNEL_SWAP 2

#define IC9100_DATAMODE_OFF 0
#define IC9100_DATAMODE_ON 1

#define IC9100_9600MODE_OFF 0
#define IC9100_9600MODE_ON 1

#define IC9100_MODE_LSB 0
#define IC9100_MODE_USB 1
#define IC9100_MODE_AM 2
#define IC9100_MODE_CW 3
#define IC9100_MODE_RTTY 4
#define IC9100_MODE_FM 5
#define IC9100_MODE_CWR 7
#define IC9100_MODE_RTTYR 8
#define IC9100_MODE_DV 17

#define IC9100_FILTER_1 1
#define IC9100_FILTER_2 2
#define IC9100_FILTER_3 3

struct ic9100_handle
{
    uint8_t address;
    uint8_t channelnum;
    cssl_t *serial;
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

int32_t ic9100_connect(string device, uint8_t address, ic9100_handle &handle);
int32_t ic9100_disconnect(ic9100_handle &handle);
int32_t ic9100_write_header(ic9100_handle &handle);
int32_t ic9100_write(ic9100_handle &handle, uint8_t command);
int32_t ic9100_write(ic9100_handle &handle, uint8_t command, uint8_t subcommand);
int32_t ic9100_write(ic9100_handle &handle, uint8_t command, vector <uint8_t> message);
int32_t ic9100_write(ic9100_handle &handle, uint8_t command, uint8_t subcommand, vector <uint8_t> message);
uint8_t ic9100_byte(vector <uint8_t> response);
uint8_t ic9100_freq2band(double frequency);
//int32_t ic9100_read(ic9100_handle &handle, vector <uint8_t> &message);
int32_t ic9100_check_address(ic9100_handle &handle);
int32_t ic9100_set_channel(ic9100_handle &handle, uint8_t channelnum);
int32_t ic9100_set_frequency(ic9100_handle &handle, double frequency);
int32_t ic9100_get_frequency(ic9100_handle &handle);
int32_t ic9100_set_freqband(ic9100_handle &handle, uint8_t mode);
int32_t ic9100_get_freqband(ic9100_handle &handle);
int32_t ic9100_set_bandpass(ic9100_handle &handle, double bandpass);
int32_t ic9100_get_bandpass(ic9100_handle &handle);
int32_t ic9100_set_mode(ic9100_handle &handle, uint8_t opmode);
int32_t ic9100_set_mode(ic9100_handle &handle, uint8_t opmode, uint8_t filtband);
int32_t ic9100_get_mode(ic9100_handle &handle);
int32_t ic9100_set_rfgain(ic9100_handle &handle, uint8_t rfgain);
int32_t ic9100_get_rfgain(ic9100_handle &handle);
int32_t ic9100_set_rfpower(ic9100_handle &handle, float power);
int32_t ic9100_get_rfpower(ic9100_handle &handle);
int32_t ic9100_set_squelch(ic9100_handle &handle, uint8_t squelch);
int32_t ic9100_get_squelch(ic9100_handle &handle);
int32_t ic9100_set_datamode(ic9100_handle &handle, uint8_t mode);
int32_t ic9100_get_datamode(ic9100_handle &handle);
int32_t ic9100_set_bps9600mode(ic9100_handle &handle, uint8_t mode);
int32_t ic9100_get_bps9600mode(ic9100_handle &handle);
int32_t ic9100_get_smeter(ic9100_handle &handle);
int32_t ic9100_get_rfmeter(ic9100_handle &handle);
int32_t ic9100_get_swrmeter(ic9100_handle &handle);
int32_t ic9100_get_alcmeter(ic9100_handle &handle);
int32_t ic9100_get_compmeter(ic9100_handle &handle);

#endif // IC9100_LIB_H

