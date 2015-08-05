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

#ifndef _VMT35LIB_H
#define _VMT35LIB_H 1

#include "configCosmos.h"

/**
* Contains definitions and functions for running a VMT-35
* Torque Rod controller.
* @file vmt35_lib.h
* @brief VMT35 controller includes
*/

//! \ingroup devices
//!	\defgroup vmt35 VMT-35 Magnetic Torque Rod Controller
//! Vectronic Aerospace Magnetic Torque Rod control library

//#include <stdio.h>
//#include <errno.h>

#include "mathlib.h"
#include "cssl_lib.h"
#include "cosmos-errno.h"

#define VMT35_ID 1
#define VMT35_BAUD 115200
#define VMT35_BITS 8
#define VMT35_PARITY 0
#define VMT35_STOPBITS 1


/**
* Internal structure containing the state of the VMT35.
* @brief VMT35 state
*/
typedef struct
	{
	uint16_t status;
	uint16_t count;
	uint16_t invalidcount;
	int32_t dac[3];
	uint16_t temp;
	uint16_t voltage;
	uint8_t resetcount;
	uint8_t crc;
	} vmt35_telemetry;

typedef struct
{
	cssl_t *serial;
	vmt35_telemetry telem;
} vmt35_handle;

int32_t vmt35_connect(char *dev, vmt35_handle *handle);
int32_t vmt35_disconnect(vmt35_handle *handle);
int32_t vmt35_reset(vmt35_handle *handle);
int32_t vmt35_set_voltage(vmt35_handle *handle, uint8_t channel, uint16_t voltage);
int32_t vmt35_set_percent_voltage(vmt35_handle *handle, int16_t ch, int16_t percentage);
int32_t vmt35_set_current_dac(vmt35_handle *handle, uint8_t channel, uint16_t current);
int32_t vmt35_set_current(vmt35_handle *handle, uint8_t channel, float current);
int32_t vmt35_enable(vmt35_handle *handle);
int32_t vmt35_disable(vmt35_handle *handle);
int32_t vmt35_reverse(vmt35_handle *handle, uint8_t channel);
int32_t vmt35_get_telemetry(vmt35_handle *handle);
int32_t vmt35_get_voltage(vmt35_handle *handle, uint8_t channel, uint16_t *voltage);
int32_t vmt35_get_current(vmt35_handle *handle, uint8_t channel, int16_t *current);
int32_t vmt35_set_amps(vmt35_handle *handle, uint8_t channel,float current);
int32_t vmt35_set_moment(vmt35_handle *handle, uint16_t channel, double mom, float npoly[7], float ppoly[7]);
int32_t vmt35_set_moments(vmt35_handle *handle, rvector mom, float npoly[3][7], float ppoly[3][7]);
int32_t vmt35_putbyte(vmt35_handle *handle, uint8_t byte);
uint8_t vmt35_crc(uint8_t *string, uint16_t length);
double vmt35_calc_amp(double mom, float npoly[6], float ppoly[6]);
double vmt35_calc_moment(double amp, float npoly[6], float ppoly[6]);
int32_t vmt35_calc_currents_from_moment(rvector mom, rvector &amp, float npoly[3][7], float ppoly[3][7]);

#endif
