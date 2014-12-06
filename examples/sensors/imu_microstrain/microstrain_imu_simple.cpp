#include <iostream>
#include <time.h>
#include "microstrain_lib.h"
#include "timelib.h"
#include <sys/time.h>
using namespace std;

int main()
{

    int iretn, handle;
    avector euler;
	
    struct timeval first, last;
    struct timezone x; 
    float timeDiff;
    float times[100];
    float sum;
    float avg;
    float stddev;

    iretn = microstrain_connect("COM1"); //"/dev/ttyOS3"

    if (iretn < 0)
        {
        printf("Error: microstrain_connect() %d\n",iretn);
        exit (1);
        }

    handle = iretn;

    iretn = microstrain_euler_angles(handle, &euler);
    printf(" %f %f %f\n",DEGOF(euler.b),DEGOF(euler.e),DEGOF(euler.h));
    for(int i=0;i<100;i++)
        {
        gettimeofday(&first, &x);
	    iretn = microstrain_euler_angles(handle, &euler);
        gettimeofday(&last, &x);
        printf("%f %f %f",DEGOF(euler.b),DEGOF(euler.e),DEGOF(euler.h));
        //timeDiff=elapsed_time(first,last);
        //times[i]=timeDiff;
        //sum+=times[i];
        //printf(" (%f milliseconds)\n", timeDiff);
        }

    /*
    avg=sum/100;

    for(int j=0;j<100;j++)
        {
        stddev+=(times[j]-avg)*(times[j]-avg);
        }

    stddev=sqrt(stddev/99);
    \
    printf("Sum:%f ms\nAverage:%f ms\nStandard Deviation:%f ms\n", sum, avg, stddev);

    */
    iretn = microstrain_disconnect(handle);
    void cssl_stop();


    return 0;
}

