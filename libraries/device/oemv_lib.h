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

/*! \file oemv_lib.h
 * 	\brief oemv_lib include file.
*/

//! \ingroup devices
//!	\defgroup oemv_lib NovAtel OEMV OEMV device library.
//! OEMV OEMV Device Library.
//!
//! Device level support for the OEMV family of devices, connected through a serial
//! interface. The unit is expected not to be in sending data continuously.

#ifndef _OEMVLIB_H
#define _OEMVLIB_H

#include "configCosmos.h"
#include "cssl_lib.h"
#include "jsonlib.h"
#include "timelib.h"

#define OEMV_BAUD 9600
#define OEMV_BITS 8
#define OEMV_PARITY 0
#define OEMV_STOPBITS 1

#define OEMV_SYNC1 0xaa
#define OEMV_SYNC2 0x44
#define OEMV_SYNC3 0x12

#define OEMV_PORT_NO_PORTS 0
#define OEMV_PORT_COM1	0x20
#define OEMV_PORT_COM2	0x40
#define OEMV_PORT_THISPORT 0xc0

#define OEMV_TIME_UNKNOWN 20
#define OEMV_TIME_APPROXIMATE 60
#define OEMV_TIME_COARSEADJUSTING 80
#define OEMV_TIME_COARSE 100
#define OEMV_TIME_COARSESTEERING 120
#define OEMV_TIME_FREEWHEELING 130
#define OEMV_TIME_FINEADJUSTING 140
#define OEMV_TIME_FINE 160
#define OEMV_TIME_FINESTEERING 180
#define OEMV_TIME_SATTIME 200

#define OEMV_MESSAGE_TYPE_BINARY 0x00
#define OEMV_MESSAGE_TYPE_ASCII 0x20
#define OEMV_MESSAGE_TYPE_NMEA 0x40
#define OEMV_MESSAGE_TYPE_RESPONSE 0x80

#define OEMV_MESSAGE_TRIGGER_ONNEW 0
#define OEMV_MESSAGE_TRIGGER_ONCHANGED 1
#define OEMV_MESSAGE_TRIGGER_ONTIME 2
#define OEMV_MESSAGE_TRIGGER_ONNEXT 3
#define OEMV_MESSAGE_TRIGGER_ONCE 4
#define OEMV_MESSAGE_TRIGGER_ONMARK 5

#define OEMV_MESSAGE_ID_LOG 1
#define OEMV_MESSAGE_ID_INTERFACEMODE 3
#define OEMV_MESSAGE_ID_COM 4
#define OEMV_MESSAGE_ID_CLOCKADJUST 15
#define OEMV_MESSAGE_ID_RESET 18
#define OEMV_MESSAGE_ID_SAVECONFIG 19
#define OEMV_MESSAGE_ID_VERSION 37
#define OEMV_MESSAGE_ID_UNLOGALL 38
#define OEMV_MESSAGE_ID_BESTPOS 42
#define OEMV_MESSAGE_ID_BESTVEL 99
#define OEMV_MESSAGE_ID_TIME 101
#define OEMV_MESSAGE_ID_BESTXYZ 241

#define OEMV_TRIGGER_ONNEW 0
#define OEMV_TRIGGER_ONCHANGED 1
#define OEMV_TRIGGER_ONTIME 2
#define OEMV_TRIGGER_ONNEXT 3
#define OEMV_TRIGGER_ONCE 4
#define OEMV_TRIGGER_ONMARK 5

#define OEMV_RESPONSE_OK 1

#define CRC32_POLYNOMIAL 0xEDB88320L

#define OEMV_MAX_DATA 2048

typedef struct
{
	uint32_t id;
	char text[48];
} oemv_response_type;

typedef struct
{
	uint32_t port_address;// 0
	uint16_t message_id;// 4
	uint8_t message_type;// 6
	uint8_t reserved;// 7
	uint32_t trigger;// 8
	double period;// 12
	double offset;// 20
	uint32_t hold;// 28
} oemv_log_type;

typedef struct
{
	uint32_t solution_status;// 0
	uint32_t position_type;// 4
	double latitude;// 8
	double longitude;// 14
	double height;// 24
	float undulation;// 32
	uint32_t datum_id;// 36
	float latitude_sd;// 40
	float longitude_sd;// 44
	float height_sd;// 48
	char station_id[4];// 52
	float differential_age;// 56
	float solution_age;// 60
	uint8_t tracked_cnt;// 64
	uint8_t solution_cnt;// 65
	uint8_t glonassl1_cnt;// 66
	uint8_t glonassl1l2_cnt;// 67
	uint8_t reserved1;// 68
	uint8_t ext_sol_stat;// 69
	uint8_t reserved2;// 70
	uint8_t signal_mask;// 71
} oemv_bestpos_type;

typedef struct
{
	uint32_t solution_status;
	uint32_t velocity_type;
	float latency;
	float differential_age;
	double horizontal_speed;
	double ground_track;
	double vertical_speed;
	uint8_t reserved;
} oemv_bestvel_type;

typedef struct
{
	uint32_t position_status;// 0
	uint32_t position_type;// 4
	double position_x;// 8
	double position_y;// 16
	double position_z;// 24
	float position_x_sd;// 32
	float position_y_sd;// 36
	float position_z_sd;// 40
	uint32_t velocity_status;// 44
	uint32_t velocity_type;// 48
	double velocity_x;// 52
	double velocity_y;// 60
	double velocity_z;// 68
	float velocity_x_sd;// 76
	float velocity_y_sd;// 80
	float velocity_z_sd;// 84
	char station_id[4];// 88
	float latency;// 92
	float differential_age;// 96
	float solution_age;// 100
	uint8_t tracked_cnt;// 104
	uint8_t solution_cnt;// 105
	uint8_t glonassl1_cnt;// 106
	uint8_t glonassl1l2_cnt;// 107
	uint8_t reserved1;// 108
	uint8_t solution_status_x;// 109
	uint8_t reserved2;// 110
	uint8_t signal_mask;// 111
} oemv_bestxyz_type;

typedef struct
{
	uint32_t clock_status;
	double offset;
	double offset_std;
	double utc_offset;
	uint32_t utc_year;
	uint8_t utc_month;
	uint8_t utc_day;
	uint8_t utc_hour;
	uint8_t utc_minute;
	uint32_t utc_ms;
	uint32_t utc_status;
} oemv_time_type;

typedef struct
{
	uint32_t numcomp;
	char model[16];
	char psn[16];
	char hw_version[16];
	char sw_version[16];
	char boot_version[16];
	char comp_date[12];
	char comp_time[12];
} oemv_version_type;

typedef struct
{
	uint32_t word;
	uint32_t pri_mask;
	uint32_t set_mask;
	uint32_t clear_mask;
} oemv_rxstatus_status;

typedef struct
{
	uint32_t error;
	uint32_t count;
	oemv_rxstatus_status rx[10];
} oemv_rxstatus_type;

typedef struct
{
	struct
	{
		uint8_t sync1;// 0
		uint8_t sync2;// 1
		uint8_t sync3;//2
		uint8_t header_size;// 3
		uint16_t message_id;// 4
		uint8_t message_type;// 6
		uint8_t port_address;// 7
		uint16_t message_size;// 8
		uint16_t sequence;// 10
		uint8_t idle_time;// 12
		uint8_t time_status;// 13
		uint16_t gps_week;// 14
		float gps_second;// 16
		uint32_t rxr_status;// 20
		uint16_t reserved;// 24
		uint16_t rxr_version;// 26
	} header;
	union
	{
		oemv_response_type response;
		oemv_log_type log;
		oemv_bestpos_type bestpos;
		oemv_bestvel_type bestvel;
		oemv_bestxyz_type bestxyz;
		oemv_time_type time;
		oemv_version_type version;
		oemv_rxstatus_type rxstatus;
        gvector geo;
	};
    int16_t n_sats_visible;
    int16_t n_sats_used;
} oemvstruc;

typedef struct
{
	cssl_t *serial;
	oemvstruc message;
	uint8_t data[OEMV_MAX_DATA];
} oemv_handle;

int32_t oemv_connect(char *dev, oemv_handle *handle);
int32_t oemv_disconnect(oemv_handle *handle);
int32_t oemv_bestpos(oemv_handle *handle);
int32_t oemv_bestvel(oemv_handle *handle);
int32_t oemv_bestxyz(oemv_handle *handle);
int32_t oemv_unlogall(oemv_handle *handle);
int32_t oemv_reset(oemv_handle *handle);
int32_t oemv_freset(oemv_handle *handle);
int32_t oemv_saveconfig(oemv_handle *handle);
int32_t oemv_setapproxpos(oemv_handle *handle, gvector pos);
int32_t oemv_setapproxtime(oemv_handle *handle, double utc);
int32_t oemv_version(oemv_handle *handle);
int32_t oemv_rxstatus(oemv_handle *handle);
int32_t oemv_time(oemv_handle *handle);
int32_t oemv_satvis(oemv_handle *handle);
int32_t oemv_gpgga(oemv_handle *handle);
int32_t oemv_gpgsv(oemv_handle *handle);
int32_t oemv_trackstat(oemv_handle *handle);
int32_t oemv_log(oemv_handle *handle);
uint32_t oemv_calc_crc32(uint8_t *data, uint16_t size);
int32_t oemv_putmessage(oemv_handle *handle);
int32_t oemv_getmessage(oemv_handle *handle);
int32_t oemv_getascii(oemv_handle *handle);
int32_t oemv_getascii_gpgga(oemv_handle *handle);
int32_t oemv_putascii(oemv_handle *handle);
int32_t oemv_getbinary(oemv_handle *handle);
int32_t oemv_putbinary(oemv_handle *handle);
int32_t oemv_talkbinary(oemv_handle *handle);
int32_t oemv_talkascii(oemv_handle *handle, bool data_flag);

#endif
