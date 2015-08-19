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

// COSMOS Library for printing math types such as rvector to the stream

// v1.0 : 2014-10-05
// library created

//---------------------------------------------------------------
// Print class

#ifndef PRINT_UTILS_H
#define PRINT_UTILS_H

#include "mathlib.h"

#include <iostream>
#include <string>
#include <iomanip>      // std::setw

// TODO: remove the dependency of matlib later, change rvector to vector?

class PrintUtils {

private:

public:

    // main functions
    //-------------------------------------------------------------
    // print functions to help visualize the vectors and other data

    // main function
    void vector(string text_prefix, rvector v, double scale, string text_suffix, int precision, int fieldwidth);

    // split vector
    void vector(string vector_name, double a, double b, double c, string units);
    void vector(string vector_name, double a, double b, double c, string units, int precision);
    void vector(string vector_name, double a, double b, double c, double d, string units);

    void vector(rvector v);
    void vector(rvector v, int precision);

    void vector(string vector_name, rvector v);
    void vector(string vector_name, rvector v, int precision);
    void vector(string vector_name, rvector v, string text_suffix, int precision);

    void vectorScaled(string vector_name, rvector v, double scale, int precision);

    void vector_endl(rvector v,
                     double factor,
                     string units,
                     int precision);

    void vector2(string name_v1, rvector v1, string name_v2, rvector v2);
    void vector2(string name_v1, rvector v1, string name_v2, rvector v2, int precision);
    void end();

};

#endif // PRINT_UTILS_H
