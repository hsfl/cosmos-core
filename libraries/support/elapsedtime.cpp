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

// TODO: rename calss from ElapseTime to StopWatch
// change class elapsedtime to stopwatch with functions jsut like a stop watch:
// - start
// - stop
// - lap
// - reset

#include "support/elapsedtime.h"
#include "support/timelib.h"

using std::cout;
using std::endl;

// -------------------------------------------------------------------
// ElapsedTime class
// Example use:

// ElapsedTime ep;

// typical use to count elapsed time
// ep.start();
// //do something
// ep.stop();
// ep.printElapsedTime();

// use for printing elapsed time as in Matlab
// ep.tic();
// //do something
// ep.toc();

// use as a stopwatch, to keep counting time
// ep.start();
// //do something
// ep.lap();
// ep.start();
// //do something
// ep.stop();
// ep.printElapsedTime();

// basic code
//    auto start = chrono::steady_clock::now();
//    auto end = chrono::steady_clock::now();
//    auto diff = end - start;
//    double test = chrono::duration <double> (diff).count();
//    std::cout <<  test << " s" << std::endl;




/*!
 * \brief ElapsedTime::ElapsedTime
 */
ElapsedTime::ElapsedTime()
{
    // by default start the timer
    start();
}

//
/*!
 * \brief ElapsedTime::info, combines toc and print, this simplifies the calling of functions
 */
void ElapsedTime::info(){
#ifndef CROSS_TYPE_arm
    std::cout << "system_clock" << std::endl;
    std::cout << std::chrono::system_clock::period::num << std::endl;
    std::cout << std::chrono::system_clock::period::den << std::endl;
    std::cout << "steady = " << std::boolalpha << std::chrono::system_clock::is_steady << std::endl << std::endl;

    std::cout << "high_resolution_clock" << std::endl;
    std::cout << std::chrono::high_resolution_clock::period::num << std::endl;
    std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
    std::cout << "steady = " << std::boolalpha << std::chrono::high_resolution_clock::is_steady << std::endl << std::endl;

    std::cout << "steady_clock" << std::endl;
    std::cout << std::chrono::steady_clock::period::num << std::endl;
    std::cout << std::chrono::steady_clock::period::den << std::endl;
    std::cout << "steady = " << std::boolalpha << std::chrono::steady_clock::is_steady << std::endl << std::endl;
#endif

}

/*!
 * \brief ElapsedTime::printElapsedTime
 */
void ElapsedTime::printElapsedTime()
{
    if (print){
        //char buffer[50];
        //sprintf(buffer,"Elapsed Time: %.6f s",elapsedTime);
        //std::cout << buffer << std::endl;
        std::cout << "Elapsed Time "<< elapsedTime << " s" << std::endl;
    }
}

/*!
 * \brief ElapsedTime::printElapsedTime
 * \param text
 */
void ElapsedTime::printElapsedTime(std::string text)
{
    if (print){
        //toc();
        //char buffer[50];
        //sprintf(buffer,"Elapsed Time (%s): %.6f s",text.c_str(),elapsedTime);
        //std::cout << buffer << std::endl;
        std::cout << "Elapsed Time (" << text << "): "<< elapsedTime<< " s" << std::endl;
    }
}

//! Lap Time
/*! This is the elapsed time since the last Lap Time. ::ElapsedTime::timeCheck is set in order
 * to keep track of this event.
 * \return Time since last call to lap(), reset(), or start(), in seconds.
*/
double ElapsedTime::lap()
{
#ifdef CROSS_TYPE_arm
    //	clock_gettime(CLOCK_MONOTONIC, &timeNow);
    //	elapsedTime = (timeNow.tv_sec - timeCheck.tv_sec) + (timeNow.tv_nsec - timeCheck.tv_nsec) / 1e9;
    gettimeofday(&timeNow, nullptr);
    elapsedTime = (timeNow.tv_sec - timeCheck.tv_sec) + (timeNow.tv_usec - timeCheck.tv_usec) / 1e6;
#else
    timeNow = std::chrono::steady_clock::now();
    elapsedTime =  std::chrono::duration<double>(timeNow - timeCheck).count();
#endif

    timeCheck = timeNow;
    return elapsedTime;

}


/*!
 * \brief ElapsedTime::tic, equivalent to matlab to start a stopwatch timer
 */
void ElapsedTime::tic(){
    start();
}

/*!
 * \brief ElapsedTime::toc, equivalent to matlab to stop a stopwatch timer
 * \return
 */
double ElapsedTime::toc(){

    //    stop();
    split();
    printElapsedTime();

    return elapsedTime;
}


/*!
 * \brief ElapsedTime::toc, equivalent to matlab to stop a stopwatch timer
 * \param text
 * \return
 */
double ElapsedTime::toc(std::string text)
{

    split();

    // print the text
    printElapsedTime(text);

    return elapsedTime;
}

/*!
 * \brief ElapsedTime::start
 */
void ElapsedTime::start()
{
    //Get the start time
#ifdef CROSS_TYPE_arm
    //	clock_gettime(CLOCK_MONOTONIC, &timeStart);
    gettimeofday(&timeStart, nullptr);
#else
    // On windows using MinGw32 it does not get better than 1ms
    // new c++11
    timeStart = std::chrono::steady_clock::now();
#endif
    timeCheck = timeStart;
    elapsedTime = 0;
}

/*!
 * \brief ElapsedTime::stop
 * \return
 */
// TODO: if calling start again it should allow you to continue from the previous stop time
double ElapsedTime::stop()
{
    //elapsedTime = 0;
    return split();
}

/*!
 * \brief ElapsedTime::split, gets the current elapsed time since the start()
 * \return
 * was previously stop()
 */
double ElapsedTime::split()
{
    //Get the final time

#ifdef CROSS_TYPE_arm
    //	clock_gettime(CLOCK_MONOTONIC, &timeNow);
    //	elapsedTime = (timeNow.tv_sec - timeStart.tv_sec) + (timeNow.tv_nsec - timeStart.tv_nsec) / 1e9;
    gettimeofday(&timeNow, nullptr);
    elapsedTime = (timeNow.tv_sec - timeStart.tv_sec) + (timeNow.tv_usec - timeStart.tv_usec) / 1e6;
#else
    timeNow = std::chrono::steady_clock::now();
    elapsedTime =  std::chrono::duration<double>(timeNow - timeStart).count();
#endif

    return elapsedTime;
}

double ElapsedTime::getElapsedTime(){
    //Get the elapsedTime from start
    return split();
}

double ElapsedTime::getElapsedTimeSince(double startMjd){
    //Get the elapsedTime from start to now
    return (currentmjd()-startMjd)*86400;
}


/*!
 * \brief compute the elapsed time between the two provided times
 * \param startMjd First time.
 * \param endMjd Second time.
 * \return elapsed time in seconds
 */
double ElapsedTime::getElapsedTime(double startMjd, double endMjd)
{
    // compute the elapsed time between start and end
    return (endMjd-startMjd)*86400;
}

/*!
 * \brief ElapsedTime::reset
 */
//TODO: just reset the timer values, don't start counting again
void ElapsedTime::reset(){
    start();
}

// -------------OLD Code
////!  Timeval subtraction
///*! Determines the difference between two timeval structures
//     \param x,y timeval structure with members long int tv_sec, tv_usec
//     \param result timeval structure difference between x and y
//     \return 1 if result is negative
//*/

//int ElapsedTime::timeval_subtract (struct timeval* result, struct timeval* x, struct timeval* y)
//{
//    /* Perform the carry for the later subtraction by updating y. */
//    if (x->tv_usec < y->tv_usec)
//    {
//        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
//        y->tv_usec -= 1000000 * nsec;
//        y->tv_sec += nsec;
//    }
//    if (x->tv_usec - y->tv_usec > 1000000)
//    {
//        int nsec = (y->tv_usec - x->tv_usec) / 1000000;
//        y->tv_usec += 1000000 * nsec;
//        y->tv_sec -= nsec;
//    }

//    /* Compute the time remaining to wait.
//     tv_usec is certainly positive. */
//    result->tv_sec = x->tv_sec - y->tv_sec;
//    result->tv_usec = x->tv_usec - y->tv_usec;

//    /* Return 1 if result is negative. */
//    return x->tv_sec < y->tv_sec;
//}


////! Elapsed time
///*! Calculates the difference in milliseconds between two timeval structures
//     \param a,b timeval structure with members long int tv_sec, tv_usec
//     \param dif timeval structure difference between a and b
//     \return elapsed time in milliseconds
//     Example use without using the class:
//        struct timeval first, last;
//        struct timezone x;
//        float timeDiff;
//        gettimeofday(&first, &x); //Get the initial time
//        // do something
//        gettimeofday(&last, &x); //Get the final time
//        printf("Elapsed time: %f milliseconds\n", timeDiff);
//*/
//// old plain c
//double ElapsedTime::getElapsedTimeMiliSeconds()
//{
//    struct timeval dif;

//    // Subtract the timevals
//    timeval_subtract(&dif,&timeStart,&time2);

//    // Calculate and return difference in milliseconds
//    return -dif.tv_sec*1000.0-dif.tv_usec/1000.0;
//}
