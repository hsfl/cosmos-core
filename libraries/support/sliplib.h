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

#ifndef _SLIPLIB_H
#define _SLIPLIB_H

/*!	\file sliplib.h
	\brief COSMOS SLIP support include file
*/

//! \ingroup support
//! \defgroup sliplib COSMOS SLIP encoding library
//! A library providing support functions for the COSMOS specific SLIP protocol.


#include "support/configCosmosKernel.h"

#include "support/cosmos-errno.h"
#include <cstring>

//! \ingroup sliplib
//! \defgroup sliplib_constants COSMOS SLIP support constants
//! @{

// **************************************************************************
// SLIP Byte Definitions - Added 20140610 - To prevent errors with other SLIP Libs
// **************************************************************************
//! SLIP Buffer End character
#ifndef SLIP_FEND
    #define SLIP_FEND 0xC0
#endif
//! SLIP Buffer Escape character
#ifndef SLIP_FESC
    #define SLIP_FESC 0xDB
#endif
//! SLIP Buffer Escaped End character
#ifndef SLIP_TFEND
    #define SLIP_TFEND 0xDC
#endif
//! SLIP Buffer Escaped Escape character
#ifndef SLIP_TFESC
    #define SLIP_TFESC 0xDD
#endif

//! @}

//! \ingroup sliplib
//! \defgroup sliplib_functions COSMOS SLIP support functions
//! @{

int32_t slip_unpack(uint8_t *sbuf, uint16_t ssize, uint8_t *rbuf, uint16_t rsize);
int32_t slip_decode(uint8_t *sbuf, uint16_t ssize, uint8_t *rbuf, uint16_t rsize);
int32_t slip_pack(uint8_t *rbuf, uint16_t rsize, uint8_t *sbuf, uint16_t ssize);
int32_t slip_encode(uint8_t *rbuf, uint16_t rsize, uint8_t *sbuf, uint16_t ssize);
int32_t slip_unpack(vector<uint8_t> &sbuf, vector<uint8_t> &rbuf);
int32_t slip_decode(vector<uint8_t> &sbuf, vector<uint8_t> &rbuf);
int32_t slip_pack(vector<uint8_t> &rbuf, vector<uint8_t> &sbuf);
int32_t slip_encode(vector<uint8_t> &rbuf, vector<uint8_t> &sbuf);
int32_t slip_extract(FILE *fp, vector<uint8_t> &buf);
uint16_t slip_calc_crc(uint8_t *buf, uint16_t size);
uint16_t slip_calc_crc(vector<uint8_t> &buf);


//! @}

#endif
