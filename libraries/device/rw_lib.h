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


#ifndef _RWLIB_H
#define _RWLIB_H 1

#include "configCosmos.h"
#if defined(COSMOS_WIN_OS) || defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

#include <stdio.h>

#include "cssl_lib.h"
#include "math/mathlib.h"
#include "cosmos-errno.h"
#include <cstring>
#ifndef COSMOS_WIN_BUILD_MSVC
//#include <sys/time.h>
#endif

#define RW_ID 1
#define RW_BAUD 9600
#define RW_BITS 8
#define RW_PARITY 0
#define RW_STOPBITS 1
#define RW_DEVICE "/dev/ttyOS1" //"/dev/tty.usbmodem1d11"

/**
* Internal structure containing the state of the reaction wheel.
* @brief Reaction wheel state
*/
typedef struct
	{
	float velocity;
	float acceleration;
	} rw_state;

int rw_connect(char *dev);
int rw_disconnect();

float rw_get_accel();
float rw_get_vel();
int rw_set_vel(double vel);
int rw_set_accel(double acc);
int rw_getdata(char *buf, int buflen);
int rw_send(char *buf, int force);
float rw_get_stored_vel();
float rw_get_stored_accel();
float rw_get_inertia();


#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)

#endif /* _RWLIB_H */

