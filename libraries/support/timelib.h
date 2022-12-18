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

#include "support/configCosmos.h"
#include "support/elapsedtime.h"
#include "math/mathlib.h"

#ifdef COSMOS_WIN_BUILD_MSVC
#include "support/timeutils.h"
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
namespace Cosmos {
    namespace Support {


#define MJD_UNIX_OFFSET 40587.
#define JD_MJD_OFFSET 2400000.5
#define MJD2JD(mjd) (double)((mjd) + JD_MJD_OFFSET)
#define JD2MJD(jd) (double)((jd) - JD_MJD_OFFSET)
#define DAY_TO_SECONDS 86400.
        //#define CURR_MJD current_mjd()
        //#define CURRENT_TIME_us CURR_MJD*DAY_TO_SECONDS*1000*1000
#define SECONDS_TO_DAYS(x)              ((double)(x) / DAY_TO_SECONDS)
#define DAYS_TO_SECONDS(x)              ((double)(x) * DAY_TO_SECONDS)
#define TIME_SECS_TO_DAYS(x)            ((double)(x) / DAY_TO_SECONDS)
#define TIME_DAYS_TO_SECS(x)            ((double)(x) * DAY_TO_SECONDS)
#define TIME_SECS_SINCE_MJD(x)          (  ( (currentmjd(0.)-(x)) ) * DAY_TO_SECONDS   )
#define TIME_UNIXs_TO_MJD(unixSecs)     (  ( (unixSecs) / DAY_TO_SECONDS ) + MJD_UNIX_OFFSET  )            // From http://stackoverflow.com/questions/466321/convert-unix-timestamp-to-julian
#define TIME_MJD_TO_UNIXs(mjd)          (((mjd) - MJD_UNIX_OFFSET) * DAY_TO_SECONDS)                       // Inverse operation of above
#define TIME_UNIX_TV_TO_DOUBLE_SECS(x)  ( ((double)(x.tv_sec)) + ((double)(x.tv_usec)  / 1000000. ) )
#define UPTIME (DAY_TO_SECONDS*(currentmjd(0.)-mjd_start_time))

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
            int32_t month = 1;
            int32_t dom = 1;
            int32_t doy = 1;
            int32_t hour = 0;
            int32_t minute = 0;
            int32_t second = 0;
            int32_t nsecond = 0;
        };

        class DateTime {

        public:
            double mjd; // represented in Modified Julian Date

            DateTime();
            DateTime(int year, int month, int day, int hour, int minute, double seconds);
            //    ~DateTime();

        };

        //! @}

        //! \ingroup timelib
        //! \defgroup timelib_functions Time handling functions
        //! @{

        double currentmjd();
        double currentmjd(double offset);
        uint64_t get_unix_time(uint64_t offset=0);
        string get_local_time();

        // gregorian calendar (year, month, day) to another format
        double cal2mjd(calstruc date);
        double cal2mjd(double year);
        double cal2mjd(int32_t year, double dayOfYear=1.);
        double cal2mjd(int32_t year, int32_t month=1, double day=1.);
        double cal2mjd(int32_t year, int32_t month=1, int32_t day=1, int32_t hour=0, int32_t minute=0, int32_t second=0, int32_t nsecond=0);
        double gregorianToModJulianDate(int32_t year, int32_t month, int32_t day,
                                        int32_t hour, int32_t minute, double second);

        // utc to another format
        timeval utc2unix(double utc);
        double utc2unixseconds(double utc);
        double utc2gmst2000(double mjd);
        string utc2unixdate(double utc);
        string utc2iso8601(double mjd);
        double iso86012utc(string date);

        // gps to another format
        void    gps2week(double gps, uint32_t& week, double& seconds);
        double  week2gps(uint32_t week, double seconds);

        // mjd to another format
        double mjd2year(double mjd);
        double mjd2doy(double mjd);
        calstruc mjd2cal(double mjd);
        int32_t mjd2ymd(double mjd, int32_t &year, int32_t &month, double &day);
        int32_t mjd2ymd(double mjd, int32_t &year, int32_t &month, double &day, double &doy);
        string mjdToGregorian(double mjd);
        int32_t mjdToGregorian(double mjd, int32_t *year, int32_t *month, int32_t *day,
                               int32_t *hour, int32_t *minute, int32_t *second);
        string mjdToGregorianFormat(double mjd);
        string  mjdToGregorianDDMMMYYYY(double mjd);
        string  mjdToGregorianDDMmmYYYY(double mjd);
        string  mjd2iso8601(double mjd);
        double  mjd2jd(double mjd);
        double  jd2mjd(double jd);
        int32_t mjd2tlef(double mjd, string &tle);

        // other
        double unix2utc(struct timeval unixtime);
        double unix2utc(double unixtime);
        double  tt2tdb(double mjd);
        double  julcen(double mjd);
        double  ranrm(double angle);
        int16_t isleap(int32_t year);
        string  seconds2DDHHMMSS(double elapsed_seconds);
        int32_t timed_countdown(int32_t seconds, int32_t step=10., string message="");
        double set_local_clock(double utc_to, int8_t direction=0);
        int32_t microsleep(uint64_t usec);
        int32_t secondsleep(double seconds);
        double newyear(double mjd=0);
        double newdecade(double mjd=0);
        uint32_t centisec(double mjd=0.);
        double centisec2mjd(uint32_t centi);
        uint32_t decisec(double mjd=0.);
        double decisec2mjd(uint32_t deci);

    }
}
//! @}

#endif
