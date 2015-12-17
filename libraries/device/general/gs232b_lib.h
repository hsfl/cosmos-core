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
* Contains definitions and functions for running a Yaesu GS-232B
* controller.
* @file gs232b_lib.h
* @brief GS-232B controller includes
*/

#ifndef _GS232BLIB_H
#define _GS232BLIB_H 1

#include "configCosmos.h"

#include "math/mathlib.h"
#include "cosmos-errno.h"
#include "cssl_lib.h"

#include <stdio.h>

#define GS232B_ID 1
#define GS232B_BAUD 9600
#define GS232B_BITS 8
#define GS232B_PARITY 0
#define GS232B_STOPBITS 1

#define GS_STATE_STOWED 0
#define GS_STATE_FIXED 1
#define GS_STATE_SEEKING 2
#define GS_STATE_TRACKING 3

/**
* Internal structure containing the state of the GS-232B and its
* associated G-5500.
* @brief GS-232B state
*/
struct gs232b_state
	{
	int32_t az_center;
	int32_t az_mode;
	int32_t az_offset_waiting;
	float az_offset;
	float currentaz;
	float targetaz;
	int32_t el_offset_waiting;
	float el_offset;
	float currentel;
	float targetel;
	float sensitivity;
	int32_t in_command;
	int32_t data_received;
	} ;

int32_t gs232b_connect(std::string dev);
int32_t gs232b_disconnect();
int32_t gs232b_calibrate_offset(int32_t axis);
//void gs232b_full_scale_calibration(int32_t axis);
//void gs232b_rotate(int32_t axis, int32_t direction);
float gs232b_get_az();
float gs232b_get_el();
int32_t gs232b_get_az_el(float &az, float &el);
float gs232b_get_az_offset();
float gs232b_get_el_offset();
void gs232b_get_state(gs232b_state &state);
int32_t gs232b_goto(float az, float el);
int32_t gs232b_stop();
int32_t gs232b_az_speed(int32_t speed);
int32_t gs232b_getdata(char *buf, int32_t buflen);
int32_t gs232b_send(char *buf, bool force);
int32_t gs232b_test();
int32_t gs232b_set_sensitivity(float sensitivity);

#endif
