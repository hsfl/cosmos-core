#include "elapsedtime.hpp"

// -------------------------------------------------------------------
// ElapsedTime class
// Example use:
// ElapsedTime ep;
// ep.tic();
// //do something
// ep.toc();
// ep.printElapsedTime();


// basic code
//    auto start = chrono::steady_clock::now();
//    auto end = chrono::steady_clock::now();
//    auto diff = end - start;
//    double test = chrono::duration <double> (diff).count();
//    cout <<  test << " s" << endl;


void ElapsedTime::info(){
    cout << "system_clock" << endl;
    cout << chrono::system_clock::period::num << endl;
    cout << chrono::system_clock::period::den << endl;
    cout << "steady = " << boolalpha << chrono::system_clock::is_steady << endl << endl;

    cout << "high_resolution_clock" << endl;
    cout << chrono::high_resolution_clock::period::num << endl;
    cout << chrono::high_resolution_clock::period::den << endl;
    cout << "steady = " << boolalpha << chrono::high_resolution_clock::is_steady << endl << endl;

    cout << "steady_clock" << endl;
    cout << chrono::steady_clock::period::num << endl;
    cout << chrono::steady_clock::period::den << endl;
    cout << "steady = " << boolalpha << chrono::steady_clock::is_steady << endl << endl;

}


// new function
// combines toc and print, this simplifies the calling of functions
void ElapsedTime::printElapsedTime()
{
    if (print){
        //char buffer[50];
        //sprintf(buffer,"Elapsed Time: %.6f s",elapsedTime);
        //cout << buffer << endl;
        cout << "Elapsed Time "<< elapsedTime<< " s" << endl;
    }
}

void ElapsedTime::printElapsedTime(string text)
{
    if (print){
        //toc();
        //char buffer[50];
        //sprintf(buffer,"Elapsed Time (%s): %.6f s",text.c_str(),elapsedTime);
        //cout << buffer << endl;
        cout << "Elapsed Time (" << text << "): "<< elapsedTime<< " s" << endl;
    }
}


double ElapsedTime::getElapsedTime()
{
    // return elapsed time in seconds (instead of miliseconds)
    // old plain c
    //return getElapsedTimeMiliSeconds()/1000.;

    // new with c++11
    // On windows using MinGw32 it does not get better than 1ms
    return chrono::duration<double>(time2 - time1).count();

}


// equivalent to matlab to start a stopwatch timer
void ElapsedTime::tic(){

    start();

}

// equivalent to matlab to stop a stopwatch timer
double ElapsedTime::toc(){

    stop();

    return elapsedTime;
}


// equivalent to matlab to stop a stopwatch timer
//double ElapsedTime::toc(bool print_flag){

//    print = print_flag;
//    toc();

//    return elapsedTime;
//}

double ElapsedTime::toc(string text){

    stop();

    // print the text
    printElapsedTime(text);

    return elapsedTime;
}


void ElapsedTime::start(){
    //Get the start time
    //gettimeofday(&time1, &x); //Get the initial time

    // new way with c++11
    time1 = chrono::steady_clock::now();
}

double ElapsedTime::stop(){
    //Get the final time
    //gettimeofday(&time2, &x); //Get the final time

    // On windows using MinGw32 it does not get better than 1ms
    // new c++11
    time2 = chrono::steady_clock::now();

    elapsedTime = getElapsedTime();
	return elapsedTime;
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
//    timeval_subtract(&dif,&time1,&time2);

//    // Calculate and return difference in milliseconds
//    return -dif.tv_sec*1000.0-dif.tv_usec/1000.0;
//}
