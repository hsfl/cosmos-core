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

/*! \file vn100_lib.h
 * 	\brief vn100_lib include file.
*/

//! \ingroup devices
//!	\defgroup vn100_lib Vectornav VN-100 Inertial Measurement Unit device library.
//! VN-100 Device Library.
//!
//! Device level support for the VN-100, connected through a serial interface. The unit is expected to be in
//! "command mode" and not sending data continuously. To achieve this, the Asynchronous Data Output Type Register
//! (system register 6), should be set to 0 (VNWRG,6,0).

#ifndef _VN100LIB_H
#define _VN100LIB_H

#include "support/configCosmos.h"
#include "device/general/cssl_lib.h"
#include "support/jsonlib.h"
#include "support/timelib.h"

#define VN100_BAUD 115200
#define VN100_BITS 8
#define VN100_PARITY 0
#define VN100_STOPBITS 1

typedef struct
{
	cssl_t *serial;
	imustruc imu;
	char buf[150];
} vn100_handle;

int32_t vn100_connect(std::string dev, vn100_handle *handle);
int32_t vn100_disconnect(vn100_handle *handle);
int32_t vn100_measurements(vn100_handle *handle);
int32_t vn100_voltages(vn100_handle *handle);
int32_t vn100_asynchoff(vn100_handle *handle);
int32_t vn100_magcal_off(vn100_handle *handle);
int32_t vn100_magcal_on(vn100_handle *handle);
int32_t vn100_magcal_get(vn100_handle *handle);
int32_t vn100_get_mag(vn100_handle *handle, double error, double delay);
#endif
