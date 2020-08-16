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

/*! \file stringlib.h
	\brief stringlib include file
	A library providing functions for handling various types of string.
*/

//! \ingroup support
//!	\defgroup stringlib String handling library

#ifndef _STRINGLIB_H
#define _STRINGLIB_H 1

#include "support/configCosmos.h"
#include "support/jsonobject.h"

#include <errno.h>
#include <string>
#include <sstream>

//! \ingroup stringlib
//! \defgroup stringlib_functions String handling functions
//! @{

vector < string > string_split(string in, string delimeters);
uint16_t string_parse(char *string, char *word[], uint16_t size);
int string_cmp(const char *wild, const char *string);
string to_string(char *value);
string to_hex(int64_t value, uint16_t digits=0);
string to_signed(int64_t value, uint16_t digits=0);
string to_unsigned(uint64_t value, uint16_t digits=0);
string to_double(double value, uint16_t precision=0);
string to_mjd(double value);
string to_temperature(double value, char units='K', uint8_t precision=4);
string to_angle(double value, char units='R', uint8_t precision=4);
string to_bool(bool value);
string to_json(string key, string value);
string to_json(string key, double value);
string to_json(string key, int64_t value);
string to_json(string key, int32_t value);
string to_json(string key, int16_t value);
string to_json(string key, int8_t value);
string to_json(string key, uint64_t value);
string to_json(string key, uint32_t value);
string to_json(string key, uint16_t value);
string to_json(string key, uint8_t value);
string to_label(string label, string value);
string to_label(string label, double value, uint16_t precision=0, bool mjd=false);
string to_label(string label, int64_t value, uint16_t digits=0, bool hex=false);
string to_label(string label, int32_t value, uint16_t digits=0, bool hex=false);
string to_label(string label, int16_t value, uint16_t digits=0, bool hex=false);
string to_label(string label, int8_t value, uint16_t digits=0, bool hex=false);
string to_label(string label, uint64_t value, uint16_t digits=0, bool hex=false);
string to_label(string label, uint32_t value, uint16_t digits=0, bool hex=false);
string to_label(string label, uint16_t value, uint16_t digits=0, bool hex=false);
string to_label(string label, uint8_t value, uint16_t digits=0, bool hex=false);
string to_label(string label, bool value);
string clean_string(string value);

// Class to parse a comma delimited string
class StringParser {

    vector<string> vect;

public:
    // the offset allows you to move the index by an offset value
	// this can be useful if, for example, getFieldNumber(1) should logically be getFieldNumber(2)
    // the offset then is 1
    int offset;
    StringParser(string str);
    StringParser(string str, char delimiter);
    string getFieldNumber(uint32_t index);
	double getFieldNumberAsDouble(uint32_t index);
    void splitString(string str, char delimiter);
    size_t numberOfFields;
    int getFieldNumberAsInteger(uint32_t index);
};

//! @}

#endif
