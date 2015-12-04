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

/*! \file pic_lib.h
 * 	\brief pic_lib include file.
*/

/*!	\defgroup pic_lib PIC16 Communications device library.
 * @{
 * PIC16 Device Library.
 *
 * Device level support for communication with a PIC16 processor, connected through a
 * serial interface. The unit is expected to be in
 * "command mode" and not sending data continuously.
 * @}
*/

#ifndef _PICLIB_H
#define _PICLIB_H

#include "configCosmos.h"
#include "cssl_lib.h"
#include "jsonlib.h"

#define PIC_BAUD 9600
#define PIC_BITS 8
#define PIC_PARITY 0
#define PIC_STOPBITS 1

#ifdef COSMOS_WIN_BUILD_MSVC
#define PIC_HEADER_SIZE 6
#else
#define PIC_HEADER_SIZE (ptrdiff_t)(((pic_header*)0)+1)
#endif
#define PIC_DATA_SIZE 300

#define SUCHI_CMD_SET_HEATER_ON 0x10
#define SUCHI_CMD_SET_HEATER_OFF 0x11
#define SUCHI_CMD_SET_SHUTTER_OPEN 0x20
#define SUCHI_CMD_SET_SHUTTER_CLOSED 0x21
#define SUCHI_CMD_SET_CAMERA_ON 0x30
#define SUCHI_CMD_SET_CAMERA_OFF 0x31
#define SUCHI_CMD_GET_REPORT 0x41

#define SUCHI_STATE_HEATER_ON_1 0x0001
#define SUCHI_STATE_HEATER_ON_2 0x0002
#define SUCHI_STATE_HEATER_ON_3 0x0004
#define SUCHI_STATE_HEATER_ON_4 0x0008
#define SUCHI_STATE_SHUTTER_OPEN_1 0x0010
#define SUCHI_STATE_SHUTTER_OPEN_2 0x0020
#define SUCHI_STATE_SHUTTER_OPEN_3 0x0040
#define SUCHI_STATE_SHUTTER_OPEN_4 0x0080
#define SUCHI_STATE_CAMERA_ON 0x0100

enum
{
	ISC_CMD_SET_BUS='a',
	ISC_CMD_GET_BUS,
	ISC_CMD_GET_STRG
} ;

typedef struct
{
	uint8_t cmd;
	uint8_t flags;
	uint16_t seq;
	uint16_t size;
} pic_header;

typedef struct
{
} pic_cmd_args0;

typedef struct
{
	uint16_t arg;
} pic_cmd_args1;

typedef struct
{
	uint16_t suchi_state;
	uint16_t sw_wdt_heaters[4];
	uint16_t sw_wdt_shutters[4];
	uint16_t reserved;
	float temperatures[9];
	float pressure;
	uint32_t uptime;
	uint32_t uart_frames_rx;
	uint32_t uart_bytes_discard;
} pic_res_suchireport;

typedef struct
{
	union
	{
		struct
		{
			pic_header header;
			union
			{
				pic_cmd_args1 args_1_16;
				pic_cmd_args0 args_0;
				pic_res_suchireport suchi_report;
				uint8_t data[PIC_DATA_SIZE];
			};
		} packet;
		uint8_t raw[PIC_DATA_SIZE+PIC_HEADER_SIZE+2];
	} ;
	uint16_t seq;
#ifdef COSMOS_WIN_BUILD_MSVC
} picstruc;
#else
} __attribute__((__may_alias__)) picstruc;
#endif

typedef struct
{
	cssl_t *serial;
	picstruc buffer;
#ifdef COSMOS_WIN_BUILD_MSVC
} pic_handle;
#else
} __attribute__((__may_alias__)) pic_handle;
#endif

int32_t pic_connect(char *dev, pic_handle *handle);
int32_t pic_disconnect(pic_handle *handle);
int32_t suchi_heater(pic_handle *handle, uint8_t number, uint8_t state);
int32_t suchi_shutter(pic_handle *handle, uint8_t number, uint8_t state);
int32_t suchi_camera(pic_handle *handle, uint8_t number, uint8_t state);
int32_t suchi_report(pic_handle *handle);
int32_t isc_power_on(pic_handle *handle, uint8_t number, uint32_t seconds);
int32_t isc_report(pic_handle *handle);
int32_t isc_put(pic_handle *handle);
int32_t isc_get(pic_handle *handle);

#endif
