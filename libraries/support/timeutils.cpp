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

#include "timeutils.hpp"

//typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>>::type> Days; /* UTC: +8:00 */


#ifndef BUILD_TYPE_arm
// Convert time_t value to string
std::string TimeUtils::timeString(const std::chrono::system_clock::time_point& tp)
{
    time_t t = std::chrono::system_clock::to_time_t(tp);
    std::string ts = ctime(&t);   // convert to calendar time
    ts.resize(ts.size()-1);  // skip trailing newline
    return ts;
}

// convert calendar time to timepoint
std::chrono::system_clock::time_point TimeUtils::makeTimePoint (int year, int mon, int day, int hour, int min, int sec)
{
    struct std::tm time;
    time.tm_year  = year-1900;  // year since 1900
    time.tm_mon   = mon-1;      // month of year (0 .. 11)
    time.tm_mday  = day;        // day of month (0 .. 31)
    time.tm_hour  = hour;       // hour of day (0 .. 23)
    time.tm_min   = min;        // minute of hour (0 .. 59)
    time.tm_sec   = sec;        // second of minute (0 .. 59 and 60 for leap seconds)
    time.tm_isdst    = -1;      // determine whether daylight saving time

    std::time_t tt = std::mktime(&time);

    if (tt == -1) {
        throw "no valid system time";
    }

    // alternative
    //    // create Epoch Time
    //    time_t rawtime;

    //    tm *timeEpoch = std::localtime(&rawtime);
    //    timeEpoch->tm_year = 1970;
    //    timeEpoch->tm_mon = 1;
    //    timeEpoch->tm_mday = 1;
    //    timeEpoch->tm_hour = 0;
    //    timeEpoch->tm_min = 0;
    //    timeEpoch->tm_sec = 0;
    //    auto epoch = std::chrono::system_clock::from_time_t(std::mktime(timeEpoch));

    return std::chrono::system_clock::from_time_t(tt);
}

// create timepoint for current UTC time
std::chrono::system_clock::time_point TimeUtils::timePointUtc ()
{
    // create UTC time (now)
    auto now = std::chrono::system_clock::now();
    time_t tnow = std::chrono::system_clock::to_time_t(now);
    tm *timeUtc = std::gmtime(&tnow);
    return std::chrono::system_clock::from_time_t(std::mktime(timeUtc));

}


double TimeUtils::secondsSinceEpoch() {

    //A unix time stamp, number of seconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
    auto epoch = makeTimePoint(1970,01,01,00,00,00);
    //    std::cout << timeString(tp1) << std::endl;
	timePointUtc();
    auto now = std::chrono::system_clock::now();

    // alternative to get seconds since epoch, but it's not guaranteed
    //    unsigned double now = std::chrono::duration_cast<std::chrono::seconds> (std::chrono::system_clock::now().time_since_epoch()).count();

    std::chrono::system_clock::duration elapsedSeconds = now-epoch;
    return std::chrono::duration_cast<std::chrono::milliseconds>(elapsedSeconds).count()/1000.0;

}


std::chrono::system_clock::duration TimeUtils::secondsSinceMidnight() {
    auto now = std::chrono::system_clock::now();

    time_t tnow = std::chrono::system_clock::to_time_t(now);
    tm *date = std::localtime(&tnow);
    date->tm_hour = 0;
    date->tm_min = 0;
    date->tm_sec = 0;
    auto midnight = std::chrono::system_clock::from_time_t(std::mktime(date));

    return now-midnight;
}


void TimeUtils::testSecondsSinceMidnight(){

    auto since_midnight = secondsSinceMidnight();

//    auto days = std::chrono::duration_cast<Days>(since_midnight);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(since_midnight);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(since_midnight - hours);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(since_midnight - hours - minutes);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(since_midnight - hours - minutes - seconds);
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(since_midnight - hours - minutes - seconds - milliseconds);
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(since_midnight - hours - minutes - seconds - milliseconds - microseconds);

//    std::cout << days.count() << "d ";
    std::cout << hours.count() << "h ";
    std::cout << minutes.count() << "m ";
    std::cout << seconds.count() << "s ";
    std::cout << milliseconds.count() << "ms ";
    std::cout << microseconds.count() << "us ";
    std::cout << nanoseconds.count() << "ns\n";

    //    std::chrono::system_clock::time_point epoch;
    //    auto start = chrono::steady_clock::now();
    //    auto end = chrono::steady_clock::now();
    //    double unix_timestamp = chrono::seconds(std::time(NULL)).count()/(60.0*60.0*24.0);
    ////    double test = chrono::duration <double> (start - unix_timestamp).count();
    //    std::cout <<  unix_timestamp << " s" << std::endl;
}

#endif
