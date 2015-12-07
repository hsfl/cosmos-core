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

/*! \file timelib.h
	\brief timelib include file
	A library providing functions for handling various types of time.
*/

//! \ingroup support
//! \defgroup timelib Time handling library
//! Time systems support.
//!
//! The following time systems are supported withing COSMOS:
//! - Coordinated Universal Time (UTC)
//! - Universal Time (UT1)
//! - Terrestrial Time (TT)
//! - GPS Time
//! - Barycentric Dynamical Time (TDB)
//! - Greenwhich Mean Sidereal Time (GMST)
//! - Greenwhich Apparent Sidereal Time (GAST)
//! Except for Sidereal time, these are all represented internally as Modified Julian Day.
//! This library provides functions to convert between these systems, and to represent
//! MJD in various other forms.

#ifndef _TIMELIB_H
#define _TIMELIB_H 1

#include "configCosmos.h"

#include "math/mathlib.h"

#ifdef COSMOS_WIN_BUILD_MSVC
#include "timeutils.hpp"
#endif

#include <cmath>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <ctime>
#include <ratio>

//! \ingroup timelib
//! \defgroup timelib_constants Time handling constants
//! @{

#define MJD2JD(mjd) (double)((mjd) + 2400000.5)
#define JD2MJD(jd) (double)((jd) - 2400000.5)
//! @}

//! \ingroup timelib
//! \defgroup timelib_typedefs Time handling type definitions
//! @{
struct timestruc
{
	double mjd;
	int32_t year;
	int32_t month;
	int32_t day;
	double fd;
	double tt_mjd;
	double gmst_rad;
};

struct calstruc
{
	int32_t year;
	int32_t month;
	int32_t dom;
	int32_t doy;
	int32_t hour;
	int32_t minute;
	int32_t second;
	int32_t nsecond;
};

//! @}

//! \ingroup timelib
//! \defgroup timelib_functions Time handling functions
//! @{

double currentmjd(double offset);
double currentmjd();

// gregorian calendar (year, month, day) to another format
double cal2mjd(calstruc date);
double cal2mjd(int32_t year, double dayOfYear);
double cal2mjd(int32_t year, int32_t month, double day);
double cal2mjd(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, int32_t nsecond);
double gregorianToModJulianDate(int32_t year, int32_t month, int32_t day,
                                   int32_t hour, int32_t minute, double second);

// utc to another format
struct timeval utc2unix(double utc);
double utc2epsilon(double mjd);
double utc2depsilon(double mjd);
double utc2dpsi(double mjd);
double utc2L(double mjd);
double utc2Lp(double mjd);
double utc2F(double mjd);
double utc2D(double mjd);
double utc2omega(double mjd);
double utc2zeta(double mjd);
double utc2z(double mjd);
double utc2era(double mjd);
double utc2tt(double mjd);
double utc2gps(double utc);
double utc2ut1(double mjd);
double utc2dut1(double mjd);
double utc2tdb(double mjd);
double utc2tdb(double mjd);
double utc2gmst1982(double mjd);
double utc2gmst2000(double mjd);
double utc2gast(double mjd);
rvector utc2nuts(double mjd);
double utc2theta(double mjd);
double utc2jcentt(double mjd);
double utc2jcenut1(double mjd);
std::string utc2iso8601(double mjd);

// gps to another format
double  gps2utc(double gps);
void    gps2week(double gps, uint32_t& week, double& seconds);
double  week2gps(uint32_t week, double seconds);

// mjd to another format
double  mjd2year(double mjd);
calstruc mjd2cal(double mjd);
int32_t mjd2ymd(double mjd, int32_t &year, int32_t &month, double &day);
int32_t mjd2ymd(double mjd, int32_t &year, int32_t &month, double &day, double &doy);
std::string mjdToGregorian(double mjd);
int32_t mjdToGregorian(double mjd, int32_t *year, int32_t *month, int32_t *day,
                      int32_t *hour, int32_t *minute, int32_t *second);
std::string  mjdToGregorianDDMMMYYYY(double mjd);
std::string  mjdToGregorianDDMmmYYYY(double mjd);
std::string  mjd2iso8601(double mjd);
double  mjd2jd(double mjd);
double  jd2mjd(double jd);

// other
double unix2utc(struct timeval unixtime);
double unix2utc(double unixtime);
double  tt2utc(double mjd);
double  tt2tdb(double mjd);
double  julcen(double mjd);
cvector polar_motion(double mjd);
int32_t leap_seconds(double mjd);
double  ranrm(double angle);
int16_t isleap(int32_t year);
int32_t load_iers();
std::string  seconds2DDHHMMSS(double elapsed_seconds);


//class Time
//{

//public:
//    Time();
//    /*!
//     * \brief Function to compute the elapsed time since the given time in the function arguments
//     * \param startTimeMjd
//     * \return elapsed time in seconds
//     */
//    double elapsedTimeSince(double startTimeMjd);
//};

//! @}

#endif
