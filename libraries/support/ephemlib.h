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

#ifndef EPHEMLIB_H
#define EPHEMLIB_H

#endif // EPHEMLIB_H


/*! \file ephemlib.h
        \brief ephemlib include file
        JPL Ephemeris.
*/

#include "math/mathlib.h"
#include "convertdef.h"

//! \ingroup support
//! \defgroup ephemlib Ephemeris library
//!         A library inplementing the ephemerides provided by the Jet Propulsion Laboratory. Currently uses DE405.
//! Full information can be found at http://ssd.jpl.nasa.gov/?planet_eph_export.


//! \ingroup ephemlib
//! \defgroup ephemlib_functions Ephemeris functions
//! @{

int32_t jplnut(double mjd, double nuts[]);
int32_t jplpos(long from, long to, double mjd, cartpos *pos);
int32_t jpllib(double utc,rmatrix *rm, rmatrix *drm);
int32_t jplopen();

//! @}
