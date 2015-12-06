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

#ifndef _DATADEF_H
#define _DATADEF_H 1

/*! \file datadef.h
*	\brief Data Management definitions file
*/

#include "configCosmos.h"
//#include <cstring>
//#include <string>

//! \ingroup datalib
//! \defgroup datalib_constants Data Management contants
//! @{

#define DATA_LOG_TYPE_SOH 0
#define DATA_LOG_TYPE_EVENT 1
#define DATA_LOG_TYPE_BEACON 2
#define DATA_LOG_TYPE_PROGRAM 3 // to log program status information while running
//! @}

//! \ingroup datalib
//! \defgroup datalib_typedefs Data Management type definitions
//! @{
typedef struct
{
	string node;
	string agent;
	string name;
	string type;
	string path;
	size_t size;
	uint16_t year;
	uint16_t jday;
	uint32_t seconds;
	double utc;
} filestruc;
//! @}

#endif
