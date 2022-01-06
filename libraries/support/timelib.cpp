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

namespace Cosmos {
    namespace Support {

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

        //! Julian Century.
        /*! Convert time supplied in Modified Julian Day to the Julian Century.
        \param mjd Time in Modified Julian Day.
        \return Decimal century.
        */
        double julcen(double mjd)
        {
            return ((mjd - 51544.5) / 36525.);
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



        //! Time for setting unix date
        /*! Represent the given UTC on a format appropriate for setting Unix date format:
 * MMDDhhmmYY.ss
 * \param utc Coordinated Universal Time expressed in Modified Julian Days.
 * \return C++ String containing the Unix date.
 */
        string utc2unixdate(double utc)
        {
            char buffer[25];
            int32_t iy=0, im=0, id=0, ihh, imm, iss;
            double fd=0.;

            mjd2ymd(utc, iy, im, fd);
            id = static_cast <int32_t>(fd);
            fd -= id;
            ihh = static_cast <int32_t>(24 * fd);
            fd -= ihh / 24.;
            imm = static_cast <int32_t>(1440 * fd);
            fd -= imm / 1440.;
            iss = static_cast <int32_t>(86400 * fd + .5);
            sprintf(buffer, "%02d%02d%02d%02d%02d.%02d", im, id, ihh, imm, iy, iss);

            return string(buffer);
        }

        //! ISO 8601 version of time
        /*! Represent the given UTC as an extended calendar format ISO 8601
 * string in the format:
 * YYYY-MM-DDTHH:MM:SS
 * \param utc Coordinated Universal Time expressed in Modified Julian Days.
 * \return C++ String containing the ISO 8601 date.
 */
        string utc2iso8601(double utc)
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
            iss = (int32_t)(86400 * fd);
            sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02d", iy, im, id, ihh, imm, iss);

            return string(buffer);
        }

        double iso86012utc(string date)
        {
            double utc = 0.;
            int32_t iy=0, im=0, id=0, ihh, imm, iss;

            sscanf(date.c_str(), "%d-%d-%dT%d:%d:%d", &iy, &im, &id, &ihh, &imm, &iss);
            utc = cal2mjd(iy, im, id, ihh, imm, iss);
            return utc;
        }

        // just call utc2iso8601(double utc)
        string mjd2iso8601(double mjd){
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
        string mjdToGregorian(double mjd)
        {
            char gregorianDateTime[25];
            int year=0, month=0, day=0;
            int hour=0, minute=0, second=0;

            mjdToGregorian(mjd, year, month, day, hour, minute, second);

            sprintf(gregorianDateTime, "%04d-%02d-%02d %02d:%02d:%02d",
                    year, month, day,
                    hour, minute, second);

            return string(gregorianDateTime);
        }


        // TODO: modifiy this function to accept a generic format as input.
        // example: "YYYY-MM-DDTHHMMSS"
        /*! Convert Modified Julian Date to international standard Gregorian
 * Date-Time in the format: YYYY-MM-DDTHHMMSS (ex. 2014-09-15T120000)
 * \param mjd Time in Modified Julian Days
 * \return gregorianDateTime in the format (YYYY-MM-DDTHHMMSS)
 */
        string mjdToGregorianFormat(double mjd)
        {
            char gregorianDateTime[25];
            int year=0, month=0, day=0;
            int hour=0, minute=0, second=0;

            mjdToGregorian(mjd, year, month, day, hour, minute, second);

            sprintf(gregorianDateTime, "%04d-%02d-%02dT%02d%02d%02d",
                    year, month, day,
                    hour, minute, second);

            return string(gregorianDateTime);
        }




        /*! Convert Modified Julian Date to US standard Gregorian Date-Time
 * with 3-letter month format:
 * DD-MMM-YYYY HH:MM:SS (ex. 15-SEP-2014 12:00:00)
 * (previously this function was named mjd2human2)
 * \param mjd Time in Modified Julian Days
 * \return gregorianDateTime (YYYY-MM-DD HH:MM:SS)
 */
        string mjdToGregorianDDMMMYYYY(double mjd)
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

            return string(gregorianDateTime);
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
        string mjdToGregorianDDMmmYYYY(double mjd)
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

            return string(gregorianDateTime);
        }



        //! Convert Elapsed Time in Seconds to Human Readable Format (used for GPS simulator)
        /*! UTC string in the format: DD HH:MM:SS
 * \param elapsed_seconds
 * \return C++ String containing human readable formated date.
 */
        string seconds2DDHHMMSS(double elapsed_seconds){

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
            return string(buffer);
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

        int32_t timed_countdown(int32_t seconds, int32_t step, string message)
        {
            ElapsedTime et;
            ElapsedTime set;

            if (message.length())
            {
                printf("%s", message.c_str());
                fflush(stdout);
            }

            while (et.split() < seconds)
            {
                int32_t nextstep = static_cast <int32_t>((seconds - et.split()) / step);
                secondsleep((seconds - et.split()) - nextstep * step);
                printf("...%d", nextstep * step);
                fflush(stdout);
            }
            printf("\n");
            return 0;
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
        int32_t mjd2tlef(double mjd, string &tle) {
            char year_buffer[3], days_buffer[13];

            // Compute our year field.
            sprintf(year_buffer, "%2d", static_cast<int>(floor(mjd2year(mjd))) % 1000);

            // Compute our days field.
            sprintf(days_buffer, "%012.8f", mjd2doy(mjd));

            tle = string(year_buffer) + string(days_buffer);
            return 0;
        }

        double set_local_clock(double utc_to)
        {
            int32_t iretn;
            double utc_from = currentmjd();
            double deltat = 86400.*(utc_to - utc_from);
            //    printf("Set Local Clock %f\n", deltat);
            if (fabs(deltat) > 1.)
            {
                // Gross adjustment to system clock
#if defined(COSMOS_WIN_OS)
                SYSTEMTIME newtime;
                SetSystemTime(&newtime);
#else
                struct timeval newtime = utc2unix(utc_to);

                // TODO: check with Eric if this is the right way to set the time?
                iretn = settimeofday(&newtime, nullptr);
                if (iretn < 0)
                {
                    return 0.;
                }
#endif
            }
            else
            {
                // Fine adjustment using adjtime()
                if (fabs(deltat) > .001)
                {
#if defined(COSMOS_WIN_OS)
                    double newdelta;
                    newdelta = deltat * 1e7;
                    SetSystemTimeAdjustment(newdelta,false);
#else

                    struct timeval newdelta, olddelta;
                    newdelta.tv_sec = deltat;
                    newdelta.tv_usec = 100000. * (deltat - newdelta.tv_sec) + .5;

                    // adjust the time
                    iretn = adjtime(&newdelta, &olddelta);
                    return 0.;
#endif
                }

            }
            return deltat;
        }

        int32_t microsleep(uint64_t usec)
        {

            struct timespec ts{};
            ts.tv_sec = usec / 1000000;
            ts.tv_nsec = (usec % 1000000) * 1000;

            int res{};
            do {
                res = nanosleep(&ts, &ts);
            } while ((res != 0) && errno == EINTR);

            if (res == 0)
            {
                return res;
            }
            else
            {
                return -errno;
            }
        }

        int32_t secondsleep(double seconds)
        {
            if (seconds < 0.)
            {
                seconds = 0.;
            }
            return microsleep(seconds * 1000000);
        }

        double newyear(int32_t years)
        {
            calstruc newyear =  mjd2cal(trunc(currentmjd()));
            newyear.year += years;
            return cal2mjd(newyear);
        }

        uint32_t centisec(double mjd)
        {
            if (mjd == 0.)
            {
                return 8640000. * (currentmjd() - newyear());
            }
            else
            {
                return 8640000. * (mjd - newyear());
            }
        }
    }
}
