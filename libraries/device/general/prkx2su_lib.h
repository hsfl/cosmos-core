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
* Contains definitions and functions for running a Yaesu prkx2su
* controller.
* @file prkx2su_lib.h
* @brief prkx2su controller includes
*/

#ifndef _prkx2suLIB_H
#define _prkx2suLIB_H 1

#include "support/configCosmos.h"

#include "math/mathlib.h"
#include "support/cosmos-errno.h"
#include "device/general/cssl_lib.h"

#include <stdio.h>

#define PRKX2SU_ID 1
#define PRKX2SU_BAUD 9600
#define PRKX2SU_BITS 8
#define PRKX2SU_PARITY 0
#define PRKX2SU_STOPBITS 1

#define PRKX2SU_AXIS_AZ 0
#define PRKX2SU_AXIS_EL 1

/**
* Internal structure containing the state of the prkx2su and its
* associated G-5500.
* @brief prkx2su state
*/
struct prkx2su_state
	{
	uint8_t azid;
	uint8_t azstatus;
	int32_t az_center;
	int32_t az_mode;
	int32_t az_offset_waiting;
	float az_offset;
	float currentaz;
	float targetaz;
	uint8_t elid;
	uint8_t elstatus;
	int32_t el_offset_waiting;
	float el_offset;
	float currentel;
	float targetel;
	float sensitivity;
	int32_t in_command;
	int32_t data_received;
	} ;

int32_t prkx2su_connect(std::string dev);
int32_t prkx2su_disconnect();
int32_t prkx2su_calibrate_offset(int32_t axis);
//void prkx2su_full_scale_calibration(int32_t axis);
//void prkx2su_rotate(int32_t axis, int32_t direction);
float prkx2su_get_az();
float prkx2su_get_el();
int32_t prkx2su_get_az_el(float &az, float &el);
float prkx2su_get_az_offset();
float prkx2su_get_el_offset();
void prkx2su_get_state(prkx2su_state &state);
int32_t prkx2su_goto(float az, float el);
int32_t prkx2su_stop(uint8_t axis);
int32_t prkx2su_ramp(uint8_t axis, uint8_t speed);
int32_t prkx2su_minimum_speed(uint8_t axis, uint8_t speed);
int32_t prkx2su_maximum_speed(uint8_t axis, uint8_t speed);
int32_t prkx2su_getdata(uint8_t axis, char *buf, int32_t buflen);
int32_t prkx2su_send(uint8_t axis, char *buf, bool force);
int32_t prkx2su_test(uint8_t axis);
int32_t prkx2su_set_sensitivity(float sensitivity);

#endif
