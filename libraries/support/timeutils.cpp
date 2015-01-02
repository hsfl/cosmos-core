#include "timeutils.hpp"

//typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>>::type> Days; /* UTC: +8:00 */


// Convert time_t value to string
string TimeUtils::timeString(const std::chrono::system_clock::time_point& tp)
{
    time_t t = chrono::system_clock::to_time_t(tp);
    string ts = ctime(&t);   // convert to calendar time
    ts.resize(ts.size()-1);  // skip trailing newline
    return ts;
}

// convert calendar time to timepoint
chrono::system_clock::time_point TimeUtils::makeTimePoint (int year, int mon, int day, int hour, int min, int sec)
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

    auto epoch = makeTimePoint(1970,01,01,00,00,00);
    //    std::cout << timeString(tp1) << std::endl;
    auto utc = timePointUtc();
    auto now = std::chrono::system_clock::now();

    // alternative to get seconds since epoch, but it's not guaranteed
    //    unsigned double now = std::chrono::duration_cast<std::chrono::seconds> (std::chrono::system_clock::now().time_since_epoch()).count();

    std::chrono::system_clock::duration elapsedSeconds = now-epoch;
    return chrono::duration_cast<std::chrono::milliseconds>(elapsedSeconds).count()/1000.0;

}


chrono::system_clock::duration TimeUtils::secondsSinceMidnight() {
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
    //    cout <<  unix_timestamp << " s" << endl;
}

