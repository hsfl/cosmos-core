// just for the old code for timeval, deprecated
//#if defined(_MSC_VER) && (_MSC_VER >= 1020)
//#include <winsock.h>
//#endif

#ifndef TIME_UTILS
#define TIME_UTILS

#include "configCosmos.h"
#include <iostream>
#include <string>
#include <chrono>
#include <ctime>

using namespace std;

class TimeUtils {

public:

    string timeString(const std::chrono::system_clock::time_point &tp);
    chrono::system_clock::time_point makeTimePoint(int year, int mon, int day, int hour, int min, int sec);
    std::chrono::system_clock::time_point timePointUtc();
    double secondsSinceEpoch();
    chrono::system_clock::duration secondsSinceMidnight();
    void testSecondsSinceMidnight();
};


#endif
