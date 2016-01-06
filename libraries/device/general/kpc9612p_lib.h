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

/**
 * * Contains definitions and functions for communicating with
 * * a Kantronix KPC-9612P TNC in KISS mode.
 * * @file kpc9612p_lib.h
 * * @brief KPC-9612P controller includes
 * */

#ifndef _KPC9612PLIB_H
#define _KPC9612PLIB_H 1


#include "configCosmos.h"

#include <stdio.h>
#include <errno.h>

#include "cssl_lib.h"
#include "cosmos-errno.h"

#define KPC9612P_MTU 254
#define KPC9612P_PAYLOAD_SIZE KPC9612P_MTU

//#define KPC9612P_PAYLOAD_SIZE 255
#define KPC9612P_FULL_SIZE (KPC9612P_PAYLOAD_SIZE+19)
#define AX25_FLAG 0x7E
#define AX25_NOLAYER3 0xF0

#define KPC9612P_ID 1
#define KPC9612P_BAUD 19200
#define KPC9612P_BITS 8
#define KPC9612P_PARITY 0
#define KPC9612P_STOPBITS 1

struct kpc9612p_frame
{
    union
    {
        struct
        {
            union
            {
                struct
                {
                    uint8_t flag;
                    char destination_address[7];
                    char source_address[7];
                    uint8_t control;
                    uint8_t protocol_id;
                } header;
                uint8_t preamble[17];
            };
            uint8_t payload[KPC9612P_PAYLOAD_SIZE+2];
        } part;
        uint8_t full[KPC9612P_FULL_SIZE];
    };
    uint16_t size;
};

struct kpc9612p_handle
{
    cssl_t *serial;
    kpc9612p_frame frame;
    uint8_t flag;
};

int32_t kpc9612p_connect(char *dev, kpc9612p_handle *handle, uint8_t flag);
int32_t kpc9612p_disconnect(kpc9612p_handle *handle);
int32_t kpc9612p_recvframe(kpc9612p_handle *handle);
int32_t kpc9612p_sendframe(kpc9612p_handle *handle);
int32_t kpc9612p_loadframe(kpc9612p_handle *handle, uint8_t *data, uint16_t size);
int32_t kpc9612p_loadframe(kpc9612p_handle *handle, std::vector<uint8_t> data);
int32_t kpc9612p_unloadframe(kpc9612p_handle *handle, uint8_t *data, uint16_t size);
int32_t kpc9612p_unloadframe(kpc9612p_handle *handle, std::vector<uint8_t> &data);
uint16_t kpc9612p_calc_fcs(uint8_t *data, uint16_t length);
int32_t kpc9612p_exitkiss(kpc9612p_handle *handle);
int32_t kpc9612p_enterkiss(kpc9612p_handle *handle);


#endif
