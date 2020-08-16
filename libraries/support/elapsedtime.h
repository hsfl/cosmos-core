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

//TODO: consider renaming this class to Timer or StopWatch

#ifndef ELAPSED_TIME
#define ELAPSED_TIME

#include "support/configCosmos.h"
#include <iostream>
#include <string>
#include <ctime>

#ifdef CROSS_TYPE_arm
#include <sys/time.h>
#else
#include <chrono>
#endif

/*! \file elapsedtime.h
*	\brief Elapsed %Time header file
*/

//! \ingroup support
//! \defgroup elapsedtime Elapsed Time
//! %Elapsed Time.
//! The functions in this library support the measurement of various types of elapsed time. Suppoert is provided
//! for both absolute elapsed time (ElapsedTime::split()) and relative elapsed time (ElapsedTime::lap()).

//! \ingroup elapsedtime
//! \defgroup elapsedtime_functions Elapsed Time function declarations
//! @{

// profiling class
// On windows using MinGw32 it does not get better than 1ms
class ElapsedTime {
    //new for c++11
#ifdef CROSS_TYPE_arm
    //	struct timespec timeStart, timeNow, timeCheck;
    timeval timeStart, timeStop, timeNow, timeCheck;
#else
    std::chrono::steady_clock::time_point timeStart, timeStop, timeNow, timeCheck; //
#endif

public:
    ElapsedTime();
    //int timeval_subtract (struct timeval* result, struct timeval* x, struct timeval* y);
    //float elapsed_time(struct timeval a,struct timeval b);
    void printElapsedTime();
    void printElapsedTime(string text);

    double getElapsedTimeMiliSeconds();
    double getElapsedTime();
    double getElapsedTimeSince(double startTimeMjd);
    double getElapsedTime(double startMjd, double endMjd);

    double lap();
    double split();
    double check();
    void reset();

    void start();
    double stop();

    void tic();
    double toc();
    //double toc(bool print_flag);
    double toc(string text);

    // turn on/off printing
    bool print = true; //
    double elapsedTime = 0.; // equivalent to reset chrono
    void info();


};



//class ElapsedTimeOld {
//    // old plain c
//    struct timeval time1, time2;

//    //IN UNIX the use of the timezone struct is obsolete;
//    //See http://linux.about.com/od/commands/l/blcmdl2_gettime.htm
//    struct timezone x;
//    float timeDiff;


//public:
//    int timeval_subtract (struct timeval* result, struct timeval* x, struct timeval* y);
//    float elapsed_time(struct timeval a,struct timeval b);
//};
//! @}


#endif
