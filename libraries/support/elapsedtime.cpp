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

// change class elapsedtime to stopwatch with functions jsut like a stop watch:
// - start
// - stop
// - lap
// - reset

#include "elapsedtime.hpp"

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
//    cout <<  test << " s" << endl;


void ElapsedTime::info(){
#ifndef BUILD_TYPE_arm
    cout << "system_clock" << endl;
    cout << std::chrono::system_clock::period::num << endl;
    cout << std::chrono::system_clock::period::den << endl;
    cout << "steady = " << std::boolalpha << std::chrono::system_clock::is_steady << endl << endl;

    cout << "high_resolution_clock" << endl;
    cout << std::chrono::high_resolution_clock::period::num << endl;
    cout << std::chrono::high_resolution_clock::period::den << endl;
    cout << "steady = " << std::boolalpha << std::chrono::high_resolution_clock::is_steady << endl << endl;

    cout << "steady_clock" << endl;
    cout << std::chrono::steady_clock::period::num << endl;
    cout << std::chrono::steady_clock::period::den << endl;
    cout << "steady = " << std::boolalpha << std::chrono::steady_clock::is_steady << endl << endl;
#endif

}


// new function
// combines toc and print, this simplifies the calling of functions
void ElapsedTime::printElapsedTime()
{
    if (print){
        //char buffer[50];
        //sprintf(buffer,"Elapsed Time: %.6f s",elapsedTime);
        //cout << buffer << endl;
        cout << "Elapsed Time "<< elapsedTime << " s" << endl;
    }
}

void ElapsedTime::printElapsedTime(std::string text)
{
    if (print){
        //toc();
        //char buffer[50];
        //sprintf(buffer,"Elapsed Time (%s): %.6f s",text.c_str(),elapsedTime);
        //cout << buffer << endl;
        cout << "Elapsed Time (" << text << "): "<< elapsedTime<< " s" << endl;
    }
}

//! Lap Time
/*! This is the elapsed time since the last Lap Time. ::timeCheck is set in order
 * to keep track of this event.
 * \return Time since last call to lap(), reset(), or start(), in seconds.
*/
double ElapsedTime::lap()
{
#ifdef BUILD_TYPE_arm
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


// equivalent to matlab to start a stopwatch timer
void ElapsedTime::tic(){
    start();
}

// equivalent to matlab to stop a stopwatch timer
double ElapsedTime::toc(){

    //    stop();
    split();
    printElapsedTime();

    return elapsedTime;
}


// equivalent to matlab to stop a stopwatch timer
//double ElapsedTime::toc(bool print_flag){

//    print = print_flag;
//    toc();

//    return elapsedTime;
//}

double ElapsedTime::toc(std::string text)
{

    split();

    // print the text
    printElapsedTime(text);

    return elapsedTime;
}


void ElapsedTime::start()
{
    //Get the start time
#ifdef BUILD_TYPE_arm
    //	clock_gettime(CLOCK_MONOTONIC, &timeStart);
    gettimeofday(&timeStart, nullptr);
#else
    timeStart = std::chrono::steady_clock::now();
#endif
    timeCheck = timeStart;
    elapsedTime = 0;
}

double ElapsedTime::stop()
{
    return split();
}

//was previously stop(){
double ElapsedTime::split()
{
    //Get the final time

#ifdef BUILD_TYPE_arm
    //	clock_gettime(CLOCK_MONOTONIC, &timeNow);
    //	elapsedTime = (timeNow.tv_sec - timeStart.tv_sec) + (timeNow.tv_nsec - timeStart.tv_nsec) / 1e9;
    gettimeofday(&timeNow, nullptr);
    elapsedTime = (timeNow.tv_sec - timeStart.tv_sec) + (timeNow.tv_usec - timeStart.tv_usec) / 1e6;
#else
    timeNow = std::chrono::steady_clock::now();
    elapsedTime =  std::chrono::duration<double>(timeNow - timeStart).count();
#endif

    timeCheck = timeNow;
    return elapsedTime;
}

double ElapsedTime::getElapsedTime(){
    //Get the elapsedTime from start
    return lap();
}

void ElapsedTime::reset(){
    // set elapsedTime to 0

    // On windows using MinGw32 it does not get better than 1ms
    // new c++11
    //time2 = chrono::steady_clock::now();

    //elapsedTime = getElapsedTime();
    //timeStart = chrono::steady_clock::now();
    start();
    //return elapsedTime;
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
