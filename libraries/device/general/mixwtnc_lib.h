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
 * * a MixW TNC in KISS mode (where TNC auto-generates FCS!).
 * * @file mixwtnc_lib.h
 * * @brief MIXWTNC controller includes
 * */

#ifndef _MIXWTNCLIB_H
#define _MIXWTNCLIB_H 1


#include "configCosmos.h"

#include <stdio.h>
#include <errno.h>

#include "cssl_lib.h"
#include "cosmos-errno.h"

#define MIXWTNC_MTU 254
#define MIXWTNC_PAYLOAD_SIZE MIXWTNC_MTU

//#define MIXWTNC_PAYLOAD_SIZE 255
#define MIXWTNC_FULL_SIZE (MIXWTNC_PAYLOAD_SIZE+19)
#define AX25_FLAG 0x7E
#define AX25_NOLAYER3 0xF0

#define MIXWTNC_ID 1
#define MIXWTNC_BAUD 19200
#define MIXWTNC_BITS 8
#define MIXWTNC_PARITY 0
#define MIXWTNC_STOPBITS 1

typedef struct
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
            uint8_t payload[MIXWTNC_PAYLOAD_SIZE+2];
		} part;
        uint8_t full[MIXWTNC_FULL_SIZE];
	};
	uint16_t size;
} mixwtnc_frame;

typedef struct
{
	cssl_t *serial;
    mixwtnc_frame frame;
	uint8_t flag;
} mixwtnc_handle;

int32_t mixwtnc_connect(char *dev, mixwtnc_handle *handle, uint8_t flag);
int32_t mixwtnc_disconnect(mixwtnc_handle *handle);
int32_t mixwtnc_recvframe(mixwtnc_handle *handle);
int32_t mixwtnc_sendframe(mixwtnc_handle *handle);
int32_t mixwtnc_loadframe(mixwtnc_handle *handle, uint8_t *data, uint16_t size);
int32_t mixwtnc_loadframe(mixwtnc_handle *handle, std::vector<uint8_t> data);
int32_t mixwtnc_unloadframe(mixwtnc_handle *handle, uint8_t *data, uint16_t size);
int32_t mixwtnc_unloadframe(mixwtnc_handle *handle, std::vector<uint8_t>& data);
int32_t mixwtnc_unloadframe(mixwtnc_frame *frame, std::vector<uint8_t>& data);
uint16_t mixwtnc_calc_fcs(uint8_t *data, uint16_t length);
int32_t mixwtnc_exitkiss(mixwtnc_handle *handle);
int32_t mixwtnc_enterkiss(mixwtnc_handle *handle);


#endif
