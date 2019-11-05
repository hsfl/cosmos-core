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

/*! \file timelib.cpp
    \brief Time handling library source file
*/

#include "support/timelib.h"
#include "support/datalib.h"
#include "support/ephemlib.h"
#include "math/mathlib.h"

struct iersstruc
{
    uint32_t mjd;
    double pmx;
    double pmy;
    double dutc;
    uint32_t ls;
};

static std::vector<iersstruc> iers;
static uint32_t iersbase=0;

#define MAXLEAPS 26
double leaps[MAXLEAPS] =
{41370.,41498.,41682.,42047.,42412.,42777.,43143.,43508.,43873.,44238.,44785.,45150.,45515.,46246.,47160.,47891.,48256.,48803.,49168.,49533.,50082.,50629.,51178.,53735.,54831.,56108.};


//! \addtogroup timelib_functions
//! @{

//! Current UTC in Modified Julian Days
/*!
    \param offset MJD offset to be apllied to turn actual time in to
    simulated time.
    \return simulated time in Modified Julian Days.
*/
double currentmjd(double offset)
{
    double mjd;

    // unfortunatelly MSVC does not support gettimeofday
#ifdef COSMOS_WIN_BUILD_MSVC
    TimeUtils tu;
    mjd = unix2utc(tu.secondsSinceEpoch() + _timezone);
#else
    struct timeval mytime;
    gettimeofday(&mytime, NULL);
    mjd = unix2utc(mytime);
#endif
    return mjd+offset;
}

double currentmjd()
{
    return currentmjd(0.);
}

unsigned long int get_unix_time()
{
    unsigned long int unix_time = time(NULL);
    return unix_time;
}

string get_local_time()
{
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    //printf ("Current local time and date: %s", asctime(timeinfo));

    int year = 1900 + timeinfo->tm_year;
    int month = 1 + timeinfo->tm_mon;
    int day = timeinfo->tm_mday;
    int hour = timeinfo->tm_hour;
    int minute = timeinfo->tm_min;
    int second = timeinfo->tm_sec;

    char time_string[25];

    sprintf(time_string, "%04d-%02d-%02dT%02d%02d%02d", year, month, day, hour, minute, second);

    return string(time_string);
}

//! Unix time to UTC
/*! Convert Unix time in a timeval structure to a double precision number representing Mofidied Julian Day.
 * \param unixtime Unix time to be converted.
 * \return UTC as Modified Julian day.
 */
double unix2utc(struct timeval unixtime)
{
    double utc;
    utc = MJD_UNIX_OFFSET + (unixtime.tv_sec + unixtime.tv_usec / 1000000.) / 86400.;

    return utc;
}

//! Unix time to UTC
/*! Convert Unix time in seconds to a double precision number representing Mofidied Julian Day.
 * \param unixtime Unix time in decimal seconds.
 * \return UTC as Modified Julian day.
 */
double unix2utc(double unixtime)
{
    double utc;
    struct tm *mytm;
    time_t thetime;

    thetime = (time_t)unixtime;
    mytm = gmtime(&thetime);
    utc = cal2mjd(mytm->tm_year+1900,mytm->tm_mon+1,mytm->tm_mday);
    utc += ((mytm->tm_hour + (mytm->tm_min + (mytm->tm_sec + (unixtime-(time_t)unixtime)) / 60.) / 60.) / 24.);

    return utc;
}

//! UTC to Unix time
/*! Convert UTC in Modified Julian Day to a timeval structure representing Unix time.
 * \param utc as Modified Julian Day.
 * \return Timeval structure with Unix time.
 */
struct timeval utc2unix(double utc)
{
    struct timeval unixtime;
    double unixseconds = 86400. * (utc - MJD_UNIX_OFFSET);
    unixtime.tv_sec = (int)unixseconds;
    unixtime.tv_usec = 1000000. * (unixseconds - unixtime.tv_sec);

    return unixtime;
}

//! UTC to Unix time
/*! Convert UTC in Modified Julian Day to a double representing Unix time.
 * \param utc as Modified Julian Day.
 * \return Double with Unix time.
 */
double utc2unixseconds(double utc)
{
    struct timeval unixtime;
    double unixseconds = 86400. * (utc - MJD_UNIX_OFFSET);

    return unixseconds;
}

//! MJD to Calendar
/*! Convert Modified Julian Day to Calendar Year, Month, Day, Hour, Minute,
 * Second and Nanosecond.
 * \param mjd Modified Julian Day
 * \return Calendar representation in ::calstruc
*/
calstruc mjd2cal(double mjd)
{
    static double lmjd = 0.;
    static calstruc date;

    if (lmjd != mjd)
    {
        double dom;
        double doy;

        lmjd = mjd;

        mjd2ymd(mjd, date.year, date.month, dom, doy);
        date.doy = (int32_t)doy;
        date.dom = (int32_t)dom;
        doy = (doy - date.doy) * 24.;
        date.hour = (int32_t)doy;
        doy = (doy - date.hour) * 60.;
        date.minute = (int32_t)doy;
        doy = (doy - date.minute) * 60.;
        date.second = (int32_t)doy;
        doy = (doy - date.second) * 1e9;
        date.nsecond = (int32_t)(doy + .5);
    }

    return date;
}

//! MJD to Year, Month, and Decimal Day (overloaded)
/*! Convert Modified Julian Day to Calendar Year, Month, Decimal Day.
 * basically it just calls mjd2ymd with all the arguments
    \param mjd Modified Julian Day
    \param year Pointer to return Calendar Year
    \param month Pointer to return Calendar Month
    \param day Pointer to return Decimal Day of the Month
    \return 0, otherwise negative error
*/
int32_t mjd2ymd(double mjd, int32_t &year, int32_t &month, double &day)
{
    double doy;
    return mjd2ymd(mjd, year, month, day, doy);
}

//! MJD to Year, Month, Decimal Day, and Julian Day (overloaded)
/*! Convert Modified Julian Day to Calendar Year, Month, Decimal Day.
    \param mjd Modified Julian Day
    \param year Pointer to return Calendar Year
    \param month Pointer to return Calendar Month
    \param day Pointer to return Decimal Day of the Month
    \param doy Pointer to return Decimal Day of the Year
    \return 0, otherwise negative error
*/
int32_t mjd2ymd(double mjd, int32_t &year, int32_t &month, double &day, double &doy)
{
    static double lmjd = 0.;
    static int32_t lyear = 1858;
    static int32_t lmonth = 11;
    static double lday = 17.;
    static double ldoy = 321.;

    if (mjd != lmjd)
    {
        int32_t a, b, c, d, e, z, alpha;
        double f;

        lmjd = mjd;
        mjd += 2400001.;
        z = (int32_t)mjd;
        f = mjd - z;

        if (z<2299161)
            a = z;
        else
        {
            alpha = (int32_t)((z - 1867216.25)/36524.25);
            a = z +1 + alpha - (int32_t)(alpha/4);
        }

        b = a + 1524;
        c = (int32_t)((b - 122.1)/365.25);
        d = (int32_t)(365.25*c);
        e = (int32_t)((b - d)/30.6001);

        lday = b - d - (int32_t)(30.6001 * e) + f;
        if (e < 14)
            lmonth = e - 1;
        else
            lmonth = e - 13;
        if (lmonth > 2)
            lyear = c - 4716;
        else
            lyear = c - 4715;
        ldoy = (int32_t)((275 * lmonth)/9) - (2-isleap(lyear))*(int32_t)((lmonth+9)/12) + lday - 30;
    }

    year = lyear;
    month = lmonth;
    day = lday;
    doy  = ldoy;
    return 0;
}

//! Calendar representation to Modified Julian Day - full
/*! Convert a full calendar representation of date and time to MJD. If month
 * is given as zero, then day will be taken as day of the year.
 * \param year Full representation of year.
 * \param month Calendar month, or zero.
 * \param day Day of month if 1 <= month <=12, otherwise day of year.
 * \param hour Hour of day (0-23)
 * \param minute Minute of day (0-59)
 * \param second Second of day (0-59)
 * \param nsecond Nanosecond of day (0-999999999)
 * \return Modified Julian Day
*/
double cal2mjd(int32_t year, int32_t month, int32_t day, int32_t hour, int32_t minute, int32_t second, int32_t nsecond)
{
    double mjd;
    calstruc date;

    date.year = year;
    date.month = month;
    if (month == 0)
    {
        date.dom = 0;
        date.doy = day;
    }
    else
    {
        date.dom = day;
        date.doy = 0;
    }
    date.hour = hour;
    date.minute = minute;
    date.second = second;
    date.nsecond = nsecond;

    mjd = cal2mjd(date);

    return mjd;
}

//! Calendar representation YYYY.ffff to Modified Julian Day - overloaded
/*! Convert a shortened calendar representation of date to MJD.
 * \param year Full representation of decimal year.
 * \return Modified Julian Day
*/
double cal2mjd(double year)
{
    double dyear;
    double dday;

    dday = 365. * modf(year, &dyear);

    return cal2mjd(static_cast<int32_t>(dyear), 0 , dday);
}

//! Calendar representation YYYY,DDD.ffff to Modified Julian Day - overloaded
/*! Convert a shortened calendar representation of date to MJD.
 * Day argument is day of year.
 * \param year Full representation of year.
 * \param dayOfYear day of year.
 * \return Modified Julian Day
*/
double cal2mjd(int32_t year, double dayOfYear)
{
    return cal2mjd(year, 0 , dayOfYear);
}


//! Calendar representation to Modified Julian Day - shortened
/*! Convert a shortened calendar representation of date to MJD.
 * Time is taken from the fractional part of the day. If month
 * is given as zero, then day will be taken as day of the year.
 * \param year Full representation of year.
 * \param month Calendar month, or zero.
 * \param day Day of month if 1 <= month <=12, otherwise day of year.
 * \return Modified Julian Day
*/
double cal2mjd(int32_t year, int32_t month, double day)
{
    double mjd;
    calstruc date;

    date.year = year;
    date.month = month;
    if (month == 0)
    {
        date.dom = 0;
        date.doy = 1;
    }
    else
    {
        date.dom = 1;
        date.doy = 0;
    }
    date.hour = date.minute = date.second = date.nsecond = 0;

    mjd = cal2mjd(date);
    // Add day-1 because absolute date starts at day 0, but JD and DOM start at 1
    mjd += day - 1.;

    return mjd;
}

//! Calendar representation to Modified Julian Day - structure
/*! Convert a full calendar representation of date and time to MJD. If month
 * is given as zero, then day will be taken as day of the year.
 * \param date Full representation of date and time in ::calstruc.
 * \return Modified Julian Day
*/
double cal2mjd(calstruc date)
{
    double mjd;
    int32_t a, b;

    if (date.year < -4712)
        return (-2400001.);

    switch (date.month)
    {
    case 0:
        ++date.month;
    case 1:
    case 2:
        date.year--;
        date.month += 12;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        a = (int32_t)(date.year / 100);
        if ((date.year > 1582) || (date.year == 1582 && date.month > 10) || (date.year == 1582 && date.month == 10 && date.dom > 4))
            b = 2 - a + (int32_t)(a/4);
        else
            b = 0;
        break;
    default:
        return (-2400001.);
    }
    if (date.dom)
    {
        mjd = (int32_t)(365.25 * (date.year+4716)) + (int32_t)(30.6001*(date.month+1)) + date.dom + b - 2401525.;
    }
    else
    {
        mjd = (int32_t)(365.25 * (date.year+4716)) + (int32_t)(30.6001*(date.month+1)) + date.doy + b - 2401525.;
    }
    mjd += ((date.hour + (date.minute + (date.second + date.nsecond / 1e9) / 60.) / 60.) / 24.);

    return (mjd);
}


/*! Get Modified Julian Date from Gregorian Calendar Date in UTC
 * \param year
 * \param month
 * \param day
 * \param hour
 * \param minute
 * \param second (for high precision mjd)
 * \return mjd (Modified Julian Date)
 */
double gregorianToModJulianDate(int32_t year, int32_t month, int32_t day,
                                int32_t hour, int32_t minute, double second)
{

    double dayFraction = hour/24.0 + minute/(1440.) + second/(86400.);
    double mjd = cal2mjd(year, month, day + dayFraction); //cal2mjd2
    return mjd;
}

//! TT Julian Century
/*! Caculate the number of centuries since J2000, Terrestrial Time, for the provided date.
    \param mjd Date in Modified Julian Day.
    \return Julian century in decimal form, otherwise negative error.
*/
double utc2jcentt(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;

    if (mjd != lmjd)
    {
        double tt = utc2tt(mjd);
        if (tt <= 0.)
        {
            lcalc = tt;
        }
        else
        {
            lcalc = (tt - 51544.5) / 36525.;
            lmjd = mjd;
        }
    }
    return (lcalc);
}

//! UT1 Julian Century
/*! Caculate the number of centuries since J2000, UT1, for the provided date.
    \param mjd Date in Modified Julian Day.
    \return Julian century in decimal form.
*/
double utc2jcenut1(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;

    if (mjd != lmjd)
    {
        lcalc = (utc2ut1(mjd)-51544.5) / 36525.;
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation values
/*! Calculate the nutation values from the JPL Ephemeris for the provided UTC date.
 * Values are in order: Psi, Epsilon, dPsi, dEpsilon. Units are radians and
 * radians/second.
    \param mjd UTC in Modified Julian Day.
    \return Nutation values in an ::rvector.
*/
rvector utc2nuts(double mjd)
{
    static double lmjd=0.;
    static uvector lcalc={{{0.,0.,0.},0.}};

    if (mjd != lmjd)
    {
        double tt = utc2tt(mjd);
        if (tt > 0.)
        {
            jplnut(tt,(double *)&lcalc.a4);
            lmjd = mjd;
        }
    }
    return (lcalc.r);
}

//! Nutation Delta Psi value.
/*! Calculate the Delta Psi value (nutation in longitude), for use in the Nutation
    matrix, for the provided UTC date.
    \param mjd UTC in Modified Julian Day.
    \return Delta Psi in radians.
*/
double utc2dpsi(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;
    rvector nuts;

    if (mjd != lmjd)
    {
        nuts = utc2nuts(mjd);
        lcalc = nuts.col[0];
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation Delta Epsilon value.
/*! Calculate the Delta Psi value (nutation in obliquity), for use in the Nutation
    matrix, for the provided UTC date.
    \param mjd UTC in Modified Julian Day.
    \return Delta Psi in radians.longitudilon
*/
double utc2depsilon(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;
    rvector nuts;

    if (mjd != lmjd)
    {
        nuts = utc2nuts(mjd);
        lcalc = nuts.col[1];
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation Epsilon value.
/*! Calculate the Epsilon value (obliquity of the ecliptic), for use in the Nutation
    matrix, for the provided UTC date.
    \param mjd UTC in Modified Julian Day.
    \return Epsilon in radians.
*/
double utc2epsilon(double mjd)
{
    // Vallado, et al, AAS-06_134, eq. 17
    static double lmjd=0.;
    static double lcalc=0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R*(84381.406+jcen*(-46.836769+jcen*(-.0001831+jcen*(0.0020034+jcen*(-0.000000576+jcen*(-0.0000000434))))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation L value.
/*! Calculate the L value,  for use in the Nutation matrix, for the provided UTC date.
    \param mjd UTC in Modified Julian Day.
    \return L in radians.
*/
double utc2L(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R*(485868.249036+jcen*(1717915923.2178+jcen*(31.8792+jcen*(.051635+jcen*(-.0002447)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation L prime value.
/*! Calculate the L prime value,  for use in the Nutation matrix, for the provided UTC date.
    \param mjd UTC in Modified Julian Day.
    \return L prime in radians.
*/
double utc2Lp(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R*(1287104.79305+jcen*(129596581.0481+jcen*(-.5532+jcen*(.000136+jcen*(-.00001149)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation F value.
/*! Calculate the F value,  for use in the Nutation matrix, for the provided UTC date.
    \param mjd UTC in Modified Julian Day.
    \return F in radians.
*/
double utc2F(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R*(335779.526232+jcen*(1739527262.8478+jcen*(-12.7512+jcen*(-.001037+jcen*(.00000417)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation D value.
/*! Calculate the D value,  for use in the Nutation matrix, for the provided UTC date.
    \param mjd UTC in Modified Julian Day.
    \return D in radians.
*/
double utc2D(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R*(1072260.70369+jcen*(1602961601.209+jcen*(-6.3706+jcen*(.006593+jcen*(-.00003169)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation omega value.
/*! Calculate the omega value,  for use in the Nutation matrix, for the provided UTC date.
    \param mjd UTC in Modified Julian Day.
    \return Omega in radians.
*/
double utc2omega(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R*(450160.398036+jcen*(-6962890.5431+jcen*(7.4722+jcen*(.007702+jcen*(-.00005939)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Precession zeta value
/*! Calculate angle zeta used in the calculation of Precession, re.
 *  Capitaine, et. al, A&A, 412, 567-586 (2003)
 * Expressions for IAU 2000 precession quantities
 * Equation 40
 * \param utc Epoch in Modified Julian Day.
 * \return Zeta in radians
*/
double utc2zeta(double utc)
{
    double ttc = utc2jcentt(utc);
    //	double zeta = (2.650545 + ttc*(2306.083227 + ttc*(0.2988499 + ttc*(0.01801828 + ttc*(-0.000005971 + ttc*(0.0000003173))))))*DAS2R;
    // Vallado, eqn. 3-88
    double zeta = (ttc*(2306.2181 + ttc*(0.30188 + ttc*(0.017998))))*DAS2R;
    return zeta;
}

//! Precession z value
/*! Calculate angle z used in the calculation of Precession, re.
 *  Capitaine, et. al, A&A, 412, 567-586 (2003)
 * Expressions for IAU 2000 precession quantities
 * Equation 40
 * \param utc Epoch in Modified Julian Day.
 * \return Zeta in radians
*/
double utc2z(double utc)
{
    double ttc = utc2jcentt(utc);
    //	double z = (-2.650545 + ttc*(2306.077181 + ttc*(1.0927348 + ttc*(0.01826837 + ttc*(-0.000028596 + ttc*(0.0000002904))))))*DAS2R;
    // Vallado, eqn. 3-88
    double z = (ttc*(2306.2181 + ttc*(1.09468 + ttc*(0.018203))))*DAS2R;
    return z;
}

//! Precession theta value
/*! Calculate angle theta used in the calculation of Precession, re.
 *  Capitaine, et. al, A&A, 412, 567-586 (2003)
 * Expressions for IAU 2000 precession quantities
 * Equation 40
 * \param utc Epoch in Modified Julian Day.
 * \return Zeta in radians
*/
double utc2theta(double utc)
{
    double ttc = utc2jcentt(utc);
    //	double theta = ttc*(2004.191903 + ttc*(-0.4294934 + ttc*(-0.04182264 + ttc*(-0.000007089 + ttc*(-0.0000001274)))))*DAS2R;
    // Vallado, eqn. 3-88
    double theta = ttc*(2004.3109 + ttc*(-0.42665 + ttc*(-0.041833)))*DAS2R;
    return theta;
}

//! Calculate DUT1
/*! Calculate DUT1 = UT1 - UTC, based on lookup in IERS Bulletin A.
    \param mjd UTC in Modified Julian Day.
    \return DUT1 in Modified Julian Day, otherwise 0.
*/
double utc2dut1(double mjd)
{
    static double lmjd=0.;
    static double lcalc = 0.;
    double frac;
    //	uint32_t mjdi;
    uint32_t iersidx;

    if (mjd != lmjd)
    {
        if (load_iers() && iers.size() > 2)
        {
            if ((uint32_t)mjd >= iersbase)
            {
                if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
                {
                    iersidx = iers.size() - 2;
                }
            }
            else
            {
                iersidx = 0;
            }
            //			mjdi = (uint32_t)mjd;
            frac = mjd - (uint32_t)mjd;
            lcalc = ((frac*iers[1+iersidx].dutc+(1.-frac)*iers[iersidx].dutc)/86400.);
            lmjd = mjd;
        }
        else
            lcalc = 0.;
    }
    return (lcalc);
}

//! Convert UTC to UT1
/*! Convert Coordinated Universal Time to Universal Time by correcting for the offset
    * between them at the given time. Table of DUT1 is first initialized from disk if it
    * hasn't yet been.
    \param mjd UTC in Modified Julian Day.
    \return UTC1 in Modified Julian Day, otherwise 0.
*/
double utc2ut1(double mjd) 
{
    static double lmjd=0.;
    static double lut=0.;

    if (mjd != lmjd)
    {
        if (load_iers())
        {
            lut = mjd + utc2dut1(mjd);
            lmjd = mjd;
        }
        else
            lut = 0.;
    }
    return (lut);
}

//! Julian Century.
/*! Convert time supplied in Modified Julian Day to the Julian Century.
    \param mjd Time in Modified Julian Day.
    \return Decimal century.
*/
double julcen(double mjd)
{
    return ((mjd - 51544.5) / 36525.);
}

//! Convert UTC to TDB.
/*! Convert Coordinated Universal Time to Barycentric Dynamical Time by correcting for
 * the mean variations as a function of Julian days since 4713 BC Jan 1.5.
 \param mjd UTC in Modified Julian Day.
 \return TDB in Modified Julian Day, otherwise 0.
*/
double utc2tdb(double mjd)
{
    static double lmjd=0.;
    static double ltdb=0.;
    double tt, g;

    if (mjd != lmjd)
    {
        tt = utc2tt(mjd);
        if (tt > 0.)
        {
            g = 6.2400756746 + .0172019703436*(mjd-51544.5);
            ltdb = (tt + (.001658*sin(g)+.000014*sin(2*g))/86400.);
            lmjd = mjd;
        }
    }
    return (ltdb);
}

//! Convert TT to UTC.
/*! Convert Terrestrial Dynamical Time to Coordinated Universal Time by correcting for
 * the appropriate number of Leap Seconds. Leap Second table is first initialized
 * from disk if it hasn't yet been.
 \param mjd TT in Modified Julian Day.
 \return UTC in Modified Julian Day, otherwise 0.
*/
double tt2utc(double mjd)
{
    uint32_t iersidx;
    int32_t iretn;

    if ((iretn=load_iers()) && iers.size() > 1)
    {
        if ((uint32_t)mjd >= iersbase)
        {
            if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
            {
                iersidx = iers.size() - 1;
            }
        }
        else
        {
            iersidx = 0;
            return ((double)iretn);
        }
        return (mjd-(32.184+iers[iersidx].ls)/86400.);
    }
    else
        return (0.);
}

//! Convert UTC to TT.
/*! Convert Coordinated Universal Time to Terrestrial Dynamical Time by correcting for
 * the appropriate number of Leap Seconds. Leap Second table is first initialized from
 * disk if it hasn't yet been.
 \param mjd UTC in Modified Julian Day.
 \return TT in Modified Julian Day, otherwise negative error
*/
double utc2tt(double mjd)
{
    static double lmjd=0.;
    static double ltt=0.;
    uint32_t iersidx=0;
    int32_t iretn;

    if (mjd != lmjd)
    {
        if ((iretn=load_iers()) && iers.size() > 1)
        {
            if ((uint32_t)mjd >= iersbase)
            {
                if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
                {
                    iersidx = iers.size() - 1;
                }
            }
            else
            {
                iersidx = 0;
            }
            ltt = (mjd+(32.184+iers[iersidx].ls)/86400.);
            lmjd = mjd;
            return (ltt);
        }
        else
        {
            return ((double)iretn);
        }
    }
    return (ltt);
}

//! Convert UTC to GPS
/*! Convert Coordinated Universal Time to GPS Time, correcting for the appropriate
 * number of leap seconds. Leap Second table is first initialized from
 * disk if it hasn't yet been.
 * \param utc UTC expressed in Modified Julian Days
 * \return GPS Time expressed in Modified Julian Days, otherwise negative error
 */
double utc2gps(double utc)
{
    double gps;

    if ((gps=utc2tt(utc)) <= 0.)
    {
        return (gps);
    }

    gps -= 51.184 / 86400.;

    return (gps);
}

//! Convert GPS to UTC
/*! Convert GPS Time to Coordinated Universal Time, correcting for the appropriate
 * number of leap seconds. Leap Second table is first initialized from
 * disk if it hasn't yet been.
 * \param gps GPS Time expressed in Modified Julian Days
 * \return UTC expressed in Modified Julian Days, otherwise 0.
 */
double gps2utc(double gps)
{
    double utc;

    gps += 51.184 / 86400.;

    if ((utc=tt2utc(gps)) <= 0.)
    {
        return (utc);
    }

    return (utc);
}

//! GPS Weeks and Seconds from GPS time
/*! Calculate the appropriate GPS week and remaining seconds from the provided
 * GPS time.
 * \param gps GPS time expressed as Modified Julian Day
 * \param week Pointer to the returned GPS week.
 * \param seconds Ponter to the returned GPS seconds.
 */
void gps2week(double gps, uint32_t& week, double& seconds)
{
    double elapsed;

    elapsed = gps - 44244.;
    week = (uint32_t)(elapsed / 7.);
    seconds = 86400. * (elapsed - week * 7.);
    //	*week %= 1024;
}

//! GPS Time from GPS Week and Seconds
/*! Calculate the appropriate GPS Time from the provided GPS Week and Seconds.
 * \param week GPS Week.
 * \param seconds GPS Seconds.
 * \return GPS Time as Modified Julian Day.
 */
double week2gps(uint32_t week, double seconds)
{
    double elapsed;

    elapsed = 44244. + week * 7. + seconds / 86400.;

    return elapsed;
}

//! Year from MJD
/*! Return the Decimal Year for the provided MJD
    \param mjd Modified Julian Data
    \return Decimal year.
*/
double mjd2year(double mjd)
{
    double day, doy, dyear;
    int32_t month, year;

    mjd2ymd(mjd,year,month,day,doy);
    dyear = year + (doy-1) / (365.+isleap(year));
    return (dyear);
}

//! Day of Year from MJD
/*! Return the Decimal Day of Year for the provided MJD
    \param mjd Modified Julian Data
    \return Decimal doy.
*/
double mjd2doy(double mjd)
{
    double day, doy;
    int32_t month, year;

    mjd2ymd(mjd,year,month,day,doy);
    return (doy);
}

//! Earth Rotation Angle
/*! Calculate the Earth Rotation Angle for a given Earth Rotation Time based on the
 * provided UTC.
    \param mjd Coordinated Universal Time as Modified Julian Day.
    \return Earth Rotation Angle, theta, in radians.
*/
double utc2era(double mjd)
{
    static double lmjd=0.;
    static double ltheta=0.;
    double ut1;

    if (mjd != lmjd)
    {
        ut1 = utc2ut1(mjd);
        ltheta = D2PI * (.779057273264 + 1.00273781191135448 * (ut1 - 51544.5));
        //        ltheta = ranrm(ltheta);
    }

    return (ltheta);
}

//! UTC to GAST
/*! Convert current UTC to Greenwhich Apparent Sidereal Time. Accounts for nutations.
    \param mjd UTC as Modified Julian Day
    \return GAST as Modified Julian Day
*/
double utc2gast(double mjd)
{
    static double lmjd=0.;
    static double lgast=0.;
    double omega, F, D;

    if (mjd != lmjd)
    {
        omega = utc2omega(mjd);
        F = utc2F(mjd);
        D = utc2D(mjd);
        lgast = utc2gmst1982(mjd) + utc2dpsi(mjd) * cos(utc2epsilon(mjd));
        lgast += DAS2R * .00264096 * sin(omega);
        lgast += DAS2R * .00006352 * sin(2.*omega);
        lgast += DAS2R * .00001175 * sin(2.*F - 2.*D + 3.*omega);
        lgast += DAS2R * .00001121 * sin(2.*F - 2.*D + omega);
        lgast = ranrm(lgast);
        lmjd = mjd;
    }
    return (lgast);
}

//! UTC (Modified Julian Day) to GMST
/*! Convert current UT to Greenwhich Mean Sidereal Time
    \param mjd UT as Modified Julian Day
    \return GMST as radians
*/
double utc2gmst1982(double mjd)
{
    static double lmjd=0.;
    static double lcalc=0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = utc2era(mjd) + DS2R*(.014506+jcen*(4612.156534+jcen*(1.3915817+jcen*(-.00000044+jcen*(-.000029956+jcen*(-.0000000368))))))/15.;
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }

    return (lcalc);
}

double utc2gmst2000(double utc)
{
    static double lutc=0.;
    static double lgmst = 0.;
    double tt;

    if (utc != lutc)
    {
        //		ut1 = utc2ut1(utc);
        tt = utc2jcentt(utc);
        lgmst = 24110.54841 + 8640184.812866 * utc2jcenut1(utc) + tt * tt * (0.093104 + tt * (-0.0000062));
        lgmst = ranrm(lgmst);
    }

    return lgmst;
}

double ranrm(double angle)
{
    double result;

    result = fmod(angle,D2PI);

    return (result >= 0.)?result:result+D2PI;
}
//! Check for Leap year
/*! Check whether the specified year (Gregorian or Julian calendar) is a Leap year.
    \param year Year to check
    \return 0 if not a Leap year, 1 if a Leap year
*/
int16_t isleap(int32_t year)
{
    if (!(year % 4))
    {
        if (!(year%100))
        {
            if (!(year%400))
            {
                return (1);
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

//! Load IERS Polar Motion, UT1-UTC, Leap Seconds.
/*! Allocate and load array for storing data from IERS file. Each
 * record includes the MJD, the Polar Motion for X and Y in radians,
 * UT1-UTC in seconds of time, and the number of Leap Seconds since
 * the creation of TAI in 1958. The following are then defined:
 * - TT = TAI +32.184
 * - TAI = UTC + Leap_Seconds
 * - UT1 = UTC + (UT1-UTC)
 \return Number of records.
*/
int32_t load_iers()
{
    FILE *fdes;
    iersstruc tiers;

    if (iers.size() == 0)
    {
        std::string fname;
        int32_t iretn = get_cosmosresources(fname);
        if (iretn < 0)
        {
            return iretn;
        }
        fname += "/general/iers_pm_dut_ls.txt";
        if ((fdes=fopen(fname.c_str(),"r")) != NULL)
        {
            char data[100];
            while (fgets(data,100,fdes))
            {
                sscanf(data,"%u %lg %lg %lg %u",&tiers.mjd,&tiers.pmx,&tiers.pmy,&tiers.dutc,&tiers.ls);
                iers.push_back(tiers);
            }
            fclose(fdes);
        }
        if (iers.size())
            iersbase = iers[0].mjd;
    }
    return (iers.size());
}

//! Leap Seconds
/*! Returns number of leap seconds for provided Modified Julian Day.
    \param mjd Provided time.
    \return Number of leap seconds, or 0.
*/
int32_t leap_seconds(double mjd)
{
    uint32_t iersidx;

    if (load_iers() && iers.size() > 1)
    {
        if ((uint32_t)mjd >= iersbase)
        {
            if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
            {
                iersidx = iers.size() - 1;
            }
        }
        else
        {
            iersidx = 0;
        }
        return (iers[iersidx].ls);
    }
    else
        return 0;
}

//! Polar motion
/*! Returns polar motion in radians for provided Modified Julian Day.
    \param mjd Provided time.
    \return Polar motion in ::rvector.
*/
cvector polar_motion(double mjd)
{
    cvector pm;
    double frac;
    //	uint32_t mjdi;
    uint32_t iersidx;

    pm = cv_zero();
    if (load_iers() && iers.size() > 2)
    {
        if ((uint32_t)mjd >= iersbase)
        {
            if ((iersidx=(uint32_t)mjd-iersbase) > iers.size())
            {
                iersidx = iers.size() - 2;
            }
        }
        else
        {
            iersidx = 0;
        }
        //		mjdi = (uint32_t)mjd;
        frac = mjd - (uint32_t)mjd;
        pm = cv_zero();
        pm.x = frac*iers[1+iersidx].pmx+(1.-frac)*iers[iersidx].pmx;
        pm.y = frac*iers[1+iersidx].pmy+(1.-frac)*iers[iersidx].pmy;
    }

    return (pm);
}



//! ISO 8601 version of time
/*! Represent the given UTC as an extended calendar format ISO 8601
 * string in the format:
 * YYYY-MM-DDTHH:MM:SS
 * \param utc Coordinated Universal Time expressed in Modified Julian Days.
 * \return C++ String containing the ISO 8601 date.
 */
std::string utc2iso8601(double utc)
{
    char buffer[25];
    int32_t iy=0, im=0, id=0, ihh, imm, iss;
    double fd=0.;

    mjd2ymd(utc, iy, im, fd);
    id = (int32_t)fd;
    fd -= id;
    ihh = (int32_t)(24 * fd);
    fd -= ihh / 24.;
    imm = (int32_t)(1440 * fd);
    fd -= imm / 1440.;
    iss = (int32_t)(86400 * fd + .5);
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d", iy, im, id, ihh, imm, iss);

    return std::string(buffer);
}

// just call utc2iso8601(double utc)
std::string mjd2iso8601(double mjd){
    return utc2iso8601(mjd);
}


/*! Convert Modified Julian Date to Time of day (hour, minute, second.fff)
 * \param dayFraction
 * \param hour Pointer to return Hour of the day
 * \param minute Pointer to return Minute of the day
 * \param second Pointer to return Second of the day (decimal)
 * \return 0 or negative error.
 */
int32_t dayFraction2hms(double dayFraction, int32_t *hour, int32_t *minute, int32_t *second) //, double *secondFraction
{
    double secs;
    *hour = (int32_t)(24. * dayFraction);
    dayFraction -= *hour / 24.;
    *minute = (int32_t)(1440. * dayFraction);
    dayFraction -= *minute / 1440.;
    secs = (86400. * dayFraction);
    //    double secondOfMinute = (86400. * dayFraction);
    *second = (int32_t)round(secs);

    // don't let seconds be 60
    if (*second == 60.){
        *second = 0;
        *minute = *minute + 1;
    }

    // don't let minutes be 60
    if (*minute == 60){
        *minute = 0;
        *hour = *hour + 1;
    }

    return 0;
}

/*! Convert Modified Julian Date to standard Gregorian Date-Time
 * \param mjd (Modified Julian Days)
 * \param year Pointer to return Calendar Year
 * \param month Pointer to return Calendar Month
 * \param  day Pointer to return Decimal Day of the Month
 * \param  hour Pointer to return Hour of the day
 * \param  minute Pointer to return Minute of the day
 * \param second Pointer to return Second of the day (decimal)
 * \return 0 or negative error.
 */
int32_t mjdToGregorian(double mjd, int32_t &year, int32_t &month, int32_t &day,
                       int32_t &hour, int32_t &minute, int32_t &second)
{

    //    double dayFraction;
    //    mjd2ymdf(mjd,year,month,&dayFraction);
    //    *day = (int)dayFraction;

    //    int j;
    calstruc cal = mjd2cal(mjd); //, year, month, day, &dayFraction, &j);
    year = cal.year;
    month = cal.month;
    day = cal.dom;
    hour = cal.hour;
    minute = cal.minute;
    second = cal.second;

    // now remove the day part and keep the fraction only
    //    dayFraction --;
    //    dayFraction2hms(dayFraction,hour,minute,second);
    return 0;
}


/*! Convert Modified Julian Date to international standard Gregorian
 * Date-Time in the format: YYYY-MM-DD HH:MM:SS (ex. 2014-09-15 12:00:00)
 * (previously this function was named mjd2human)
 * \param mjd Time in Modified Julian Days
 * \return gregorianDateTime (YYYY-MM-DD HH:MM:SS)
 */
std::string mjdToGregorian(double mjd)
{
    char gregorianDateTime[25];
    int year=0, month=0, day=0;
    int hour=0, minute=0, second=0;

    mjdToGregorian(mjd, year, month, day, hour, minute, second);

    sprintf(gregorianDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
            year, month, day,
            hour, minute, second);

    return std::string(gregorianDateTime);
}


// TODO: modifiy this function to accept a generic format as input.
// example: "YYYY-MM-DDTHHMMSS"
/*! Convert Modified Julian Date to international standard Gregorian
 * Date-Time in the format: YYYY-MM-DDTHHMMSS (ex. 2014-09-15T120000)
 * \param mjd Time in Modified Julian Days
 * \return gregorianDateTime in the format (YYYY-MM-DDTHHMMSS)
 */
std::string mjdToGregorianFormat(double mjd)
{
    char gregorianDateTime[25];
    int year=0, month=0, day=0;
    int hour=0, minute=0, second=0;

    mjdToGregorian(mjd, year, month, day, hour, minute, second);

    sprintf(gregorianDateTime, "%04d-%02d-%02dT%02d%02d%02d",
            year, month, day,
            hour, minute, second);

    return std::string(gregorianDateTime);
}




/*! Convert Modified Julian Date to US standard Gregorian Date-Time
 * with 3-letter month format:
 * DD-MMM-YYYY HH:MM:SS (ex. 15-SEP-2014 12:00:00)
 * (previously this function was named mjd2human2)
 * \param mjd Time in Modified Julian Days
 * \return gregorianDateTime (YYYY-MM-DD HH:MM:SS)
 */
std::string mjdToGregorianDDMMMYYYY(double mjd)
{
    char gregorianDateTime[25];
    int year=0, month=0, day=0, hour=0, minute=0, second=0;
    //    double fd=0.; //?? is this fraction of day

    static const char month_name[][4] = {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
        "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
    };

    //mjd2cal(mjd, &year, &month, &day, &fd, &j);

    mjdToGregorian(mjd, year, month, day, hour, minute, second);

    //    hours = (int)(24 * fd);
    //    fd -= hours / 24.;
    //    minutes = (int)(1440 * fd);
    //    fd -= minutes / 1440.;
    //    seconds = (86400 * fd);
    //    int int_seconds = (int)round(seconds);
    sprintf(gregorianDateTime, "%02d-%3s-%04d %02d:%02d:%02d",
            day, month_name[month-1], year,
            hour, minute, second);

    return std::string(gregorianDateTime);
}

// TODO: Must fix milisecond accuracy
/*! Convert Modified Julian Date to US standard Gregorian Date-Time
 * with 3-letter month format in small caps:
 * DD mmm YYYY HH:MM:SS.FFF (15 Sep 2014 12:00:00.000)
 * Used for STK remote
 * (previously this function was named mjd2human3)
 * \param mjd Time in Modified Julian Days
 * \return gregorianDateTime (YYYY-MM-DD HH:MM:SS)
 */
std::string mjdToGregorianDDMmmYYYY(double mjd)
{
    char gregorianDateTime[50];
    int year=0, month=0, day=0, hour=0, minute=0, second=0;
    int msec = 0;
    //    double fd=0.;

    static const char month_name[][4] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    //    mjd2cal(mjd, &year, &month, &day, &fd, &j);
    mjdToGregorian(mjd, year, month, day, hour, minute, second);
    //    hh = (int)(24 * fd);
    //    fd -= hh / 24.;
    //    min = (int)(1440 * fd);
    //    fd -= min / 1440.;
    //    sec = (int)(86400 * fd);
    //    fd -= sec / 86400.;
    //    msec = (int)(86400*1000 * fd);
    sprintf(gregorianDateTime, "%02d %3s %04d %02d:%02d:%02d.%03d",
            day, month_name[month-1], year,
            hour, minute, second, msec);

    return std::string(gregorianDateTime);
}



//! Convert Elapsed Time in Seconds to Human Readable Format (used for GPS simulator)
/*! UTC string in the format: DD HH:MM:SS
 * \param elapsed_seconds
 * \return C++ String containing human readable formated date.
 */
std::string seconds2DDHHMMSS(double elapsed_seconds){

    char buffer[50];

    if (elapsed_seconds < 0){
        return "invalid time";
    }

    double days = elapsed_seconds/86400.;       // convert to elapsed days
    int day    = int(days);                     // get the day number
    double fd  = days-day;                    // get the day fraction
    /*
    temp       = elapsed_seconds - day * 86400; // get the seconds of the day

    int hour   = temp/3600.;                    // get the number of hours
    temp       = temp - hour*3600;              // take off the number of seconds of the hours run

    //int min  = ((elapsed_seconds) % 3600.)/60.;
    int min    = temp/60;                       // get the number of minutes

    //int sec  = (elapsed_seconds) % 60.;
    int sec    = temp - min*60;                 // get the number of seconds

    int msec    = (temp - sec)*1000;            // get the number of miliseconds
*/

    int hour = (int)(24 * fd);
    fd -= hour / 24.;
    int min = (int)(1440 * fd);
    fd -= min / 1440.;
    int sec = (int)(86400 * fd);
    fd -= sec / 86400.;
    int msec = (int)round((86400*1000 * fd));

    if (msec == 1000){
        sec ++;
        msec = 0;
    }
    // specific format for SimGEN
    //sprintf(buffer,"%d %02d:%02d:%.2f", day, hour, min, sec);
    sprintf(buffer,"%02d:%02d:%02d.%03d", hour, min, sec, msec);
    return std::string(buffer);
}

//! Modified Julian Day to Julian Day
/*! Convert date in Modified Julian Day format to date in Julian Day format.
 * \param mjd Date in Modified Julian Day format.
 * \return Date in Julian Day format.
*/
double  mjd2jd(double mjd){
    return MJD2JD(mjd);
}

//! Julian Day to Modified Julian Day
/*! Convert date in Julian Day format to date in Modified Julian Day format.
 * \param jd Date in Julian Day format.
 * \return Date in Modified Julian Day format.
*/
double  jd2mjd(double jd) {
    return JD2MJD(jd);
}

//! @}



DateTime::DateTime()
{
    mjd = currentmjd();
}

DateTime::DateTime(int year, int month, int day, int hour, int minute, double seconds)
{
    mjd = cal2mjd(year, month, day, hour, minute, seconds, 0);
}

//! Convert mjd to the TLE epoch format.
int32_t mjd2tlef(double mjd, std::string &tle) {
    char year_buffer[3], days_buffer[13];

    // Compute our year field.
    sprintf(year_buffer, "%2d", static_cast<int>(floor(mjd2year(mjd))) % 1000);

    // Compute our days field.
    sprintf(days_buffer, "%012.8f", mjd2doy(mjd));

    tle = std::string(year_buffer) + std::string(days_buffer);
    return 0;
}
