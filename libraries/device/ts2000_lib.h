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
* Contains definitions and functions for running a TS2000
* controller.
* @file ts2000_lib.h
* @brief TS2000 controller includes
*/

#ifndef _TS2000LIB_H
#define _TS2000LIB_H 1

#include "configCosmos.h"

#include <stdio.h>
#include <errno.h>

#include "mathlib.h"
#include "cssl_lib.h"
#include "cosmos-errno.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TS2000_ID 1
#define TS2000_BAUD 9600
#define TS2000_BITS 8
#define TS2000_PARITY 0
#define TS2000_STOPBITS 1


/**
* Internal structure containing the state of the TS2000.
* @brief TS2000 state
*/
typedef struct
	{
	double frequency[2];
	} ts2000_state;

int ts2000_connect(char *dev);
int ts2000_disconnect();
double ts2000_get_frequency(int channel);
double ts2000_set_frequency(int channel, double frequency);

#ifdef __cplusplus
}
#endif

#endif
