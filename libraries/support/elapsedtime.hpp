// ?? remove ElapsedTime from timelib.h and use this one instead

// just for the old code for timeval, deprecated
//#if defined(_MSC_VER) && (_MSC_VER >= 1020)
//#include <winsock.h>
//#endif

#ifndef ELAPSED_TIME
#define ELAPSED_TIME

#include "configCosmos.h"
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>

using namespace std;

// profiling class
// On windows using MinGw32 it does not get better than 1ms
class ElapsedTime {
    //new c++11
    chrono::steady_clock::time_point time1, time2;

public:
    //int timeval_subtract (struct timeval* result, struct timeval* x, struct timeval* y);
    //float elapsed_time(struct timeval a,struct timeval b);
    void printElapsedTime();
    void printElapsedTime(string text);
    double getElapsedTimeMiliSeconds();
    double getElapsedTime();

    void tic();
    double toc();
    //double toc(bool print_flag);
    double toc(string text);
	double split();
	double reset();

    void start();
	double stop();

    // turn on/off printing
    bool print = true; //
    double elapsedTime = 0.;
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

#endif
