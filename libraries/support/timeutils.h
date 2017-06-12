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

// just for the old code for timeval, deprecated
//#if defined(_MSC_VER) && (_MSC_VER >= 1020)
//#include <winsock.h>
//#endif

#ifndef TIME_UTILS
#define TIME_UTILS

#include "support/configCosmos.h"
#include <iostream>
#include <string>

#ifndef CROSS_TYPE_arm
#include <chrono>
#include <ctime>

class TimeUtils {

public:

    std::string timeString(const std::chrono::system_clock::time_point &tp);
    std::chrono::system_clock::time_point makeTimePoint(int year, int mon, int day, int hour, int min, int sec);
    std::chrono::system_clock::time_point timePointUtc();
    double secondsSinceEpoch();
    std::chrono::system_clock::duration secondsSinceMidnight();
    void testSecondsSinceMidnight();
};

#endif

#endif
