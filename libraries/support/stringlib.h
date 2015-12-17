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

#include "configCosmos.h"

#include <errno.h>
#include <string>
#include <sstream>
//using namespace std;  // don't use this as it may cause conflicts with other namespaces

//! \ingroup stringlib
//! \defgroup stringlib_functions String handling functions
//! @{

uint16_t string_parse(char *string, char *word[], uint16_t size);
int string_cmp(const char *wild, const char *string);

// Class to parse a comma delimited string
class StringParser {

    std::vector<std::string> vect;

public:
    // the offset allows you to move the index by an offset value
	// this can be useful if, for example, getFieldNumber(1) should logically be getFieldNumber(2)
    // the offset then is 1
    int offset;
    StringParser(std::string str);
    StringParser(std::string str, char delimiter);
    std::string getFieldNumber(uint32_t index);
	double getFieldNumberAsDouble(uint32_t index);
    void splitString(std::string str, char delimiter);
	size_t numberOfFields;
};

//! @}

#endif
