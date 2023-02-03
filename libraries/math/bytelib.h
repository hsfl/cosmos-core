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

#ifndef _BYTELIB_H
#define _BYTELIB_H

#include <vector>
#include "math/constants.h"

ByteOrder local_byte_order();

#if ((SIZE_WIDTH) == (UINT64_WIDTH))
uint64_t uintswap(uint8_t bits, uint64_t value, ByteOrder order=ByteOrder::LITTLEENDIAN);
#else
size_t uintswap(uint8_t bits, size_t value, ByteOrder order=ByteOrder::LITTLEENDIAN);
#endif
uint8_t uint8from(uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
uint8_t uint8to(uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
void uint8from(vector<uint8_t> src, vector<uint8_t> &dst, ByteOrder order=ByteOrder::LITTLEENDIAN);
void uint8to(vector<uint8_t> src, vector<uint8_t> dst, ByteOrder order=ByteOrder::LITTLEENDIAN);
uint16_t uint16from(uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
int16_t int16from(uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
uint32_t uint32from(const uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
int32_t int32from(uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
float floatfrom(uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
double doublefrom(uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
void uint32to(uint32_t value, uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
void int32to(int32_t value, uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
void uint16to(uint16_t value, uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
void int16to(int16_t value, uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
void floatto(float value, uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
void doubleto(double value, uint8_t *pointer, ByteOrder order=ByteOrder::LITTLEENDIAN);
vector<uint8_t> vector8invert(uint8_t* data, uint32_t count);


#endif
