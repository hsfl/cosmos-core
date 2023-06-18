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


#ifndef _COSMOS_PRINT_UTILS
#define _COSMOS_PRINT_UTILS

// COSMOS Library for Printing Vector and stuff like that to the stream

// v1.0 : 2014-10-05
// library created

//---------------------------------------------------------------
// Print class
#include "support/configCosmos.h"
#include "math/mathlib.h"

#include <iostream>     // std::cout
#include <string>       // string
#include <sstream>      // std::ostringstream

// TODO: remove the dependency of mathlib later, change rvector to vector?

//! \ingroup support
//! \defgroup printutils Output Utilities
//! Output Utilities.
//!
//! Various utilities to support output of the numerous COSMOS data types.

//! \ingroup printutils
//! \defgroup printutils_functions Output Utilties functions
//! @{

class PrintUtils
{

private:

public:

    PrintUtils();

    //main functions
    //-------------------------------------------------------------
    // print functions to help visualize the vectors and other data

    // main function
    // 20150818JC: Fixed mismatched prototype to include field width.  Default value in cpp
    //void vector(string text_prefix, rvector v, double scale, string text_suffix, int precision);
    void vector(string prefix, rvector v, double scale, string suffix, int precision, int fieldwidth);

    void vector(rvector v);
    //void vector(rvector v, int precision);
    void vector(string prefix, rvector v);
    //void vector(string vector_name, rvector v, int precision);
    void vector(string vector_name, rvector v, string suffix);
    void vector(string prefix, rvector v, string suffix, int precision);

    void vectorAndMag(string vector_name, rvector v);
    void vectorAndMag(string vector_name, rvector v, string suffix);


    //    void vectorScaled(string vector_name, rvector v, double scale, int precision);
    //	void vectorScaled(string vector_name, quaternion q, double scale, int precision);

    // split vector
    //    void vector(string vector_name, double a, double b, double c, string units);
    //    void vector(string vector_name, double a, double b, double c, string units, int precision);
    //    void vector(string vector_name, double a, double b, double c, double d, string units);


    void vector_endl(rvector v,
                     double factor,
                     string units,
                     int precision);

    void vector2(string name_v1, rvector v1, string name_v2, rvector v2);
    void vector2(string name_v1, rvector v1, string name_v2, rvector v2, int precision);
    void endline();

    // quaternions
    string quat(string prefix, quaternion q, string suffix, int precision, int fieldwidth);
    string quat(quaternion q);
    void quat(quaternion q, int precision);
    void quat(string prefix, quaternion q);
    void quat(string prefix, quaternion q, int precision);
    void quat(string prefix, quaternion q, string suffix);
    void quat(string prefix, quaternion q, string suffix, int precision);

    // control flags
    bool printOn; // to control printing
    int precision;
    int fieldwidth;
    double scale;
    string prefix;
    string suffix;
    string delimiter;
    bool delimiter_flag;
    bool use_brackets;
    string fullMessage;

    // reset to default values
    void reset();


    void text(string text);

    // scalar
    void scalar(string prefix, double s, string suffix);
    void scalar(string prefix, double s, double scale, string suffix, int precision, int fieldwidth);
    void scalar(double s, string suffix);
    void scalar(string prefix, double s);
    void scalar(double s);
    // TODO: add void scalar(double s, double precision);

};

//! @}


#endif //_COSMOS_PRINT_UTILS

