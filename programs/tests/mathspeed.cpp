#include "support/configCosmos.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "support/elapsedtime.h"

#define BUFSIZE 10000000

double buffer[10001][1000];
char *srcbuf;
ElapsedTime et;
//struct timeval tp;
unsigned long size, count;
long start_s, start_u, diff_u, time1, time2, time3, time4;
double start_t;
long i, j, k, ii;
int inb;
struct stat sbuf;
double answer, base, speed1, speed2, speed3, speed4;

main(int argc, char **argv)
{
    speed1 = speed2 = speed3 = 0.;
    time1 = time2 = time3 =time4 = 1000000000L;

    for (i=0;i<10001;i++)
        for (j=0; j<1000; j++)
            buffer[i][j] = j%256;

    for (k=0; k<5; k++)
    {
        //gettimeofday(&tp,NULL);
        //start_s = tp.tv_sec;
        //start_u = tp.tv_usec;
        et.reset();
        j = 5000000;
        for (i=0;i<5000;i++)
        {
            for (j=0; j<1000; j++)
            {
                answer = (double)25.;
            }
        }
        //gettimeofday(&tp,NULL);
        //diff_u = 1000000L*(tp.tv_sec-start_s)+(tp.tv_usec-start_u);
        diff_u = 1000000L * et.lap();
        if (time1 > diff_u)
            time1 = diff_u;
        base = 5000000./diff_u;
        if (speed1 < base)
            speed1 = base;
        fflush(stdout);
        //gettimeofday(&tp,NULL);
        //start_s = tp.tv_sec;
        //start_u = tp.tv_usec;
        et.reset();
        for (i=0; i<5000; i++)
        {
            for (j=0; j<1000; j++)
            {
                answer = (double)(5.) * (double)(5.);
            }
        }
        //gettimeofday(&tp,NULL);
        //diff_u = 1000000L*(tp.tv_sec-start_s)+(tp.tv_usec-start_u);
        diff_u = 1000000L * et.lap();
        if (time2 > diff_u)
            time2 = diff_u;
        base = 5000000./(time2);
        if (speed2 < base)
            speed2 = base;
        fflush(stdout);
        //gettimeofday(&tp,NULL);
        //start_s = tp.tv_sec;
        //start_u = tp.tv_usec;
        et.reset();
        for (i=0; i<200; i++)
        {
            for (ii=0;ii<25;ii++)
            {
                for (j=0; j<1000; j++)
                {
                    answer = buffer[0][j] * buffer[0][ii];
                }
            }
        }
        //gettimeofday(&tp,NULL);
        //diff_u = 1000000L*(tp.tv_sec-start_s)+(tp.tv_usec-start_u);
        diff_u = 1000000L * et.lap();
        if (time3 > diff_u)
            time3 = diff_u;
        base = 5000000./(time3);
        if (speed3 < base)
            speed3 = base;
        //fflush(stdout);
        //gettimeofday(&tp,NULL);
        //start_s = tp.tv_sec;
        //start_u = tp.tv_usec;
        et.reset();
        for (i=0; i<5000; i++)
        {
            for (j=0; j<1000; j++)
            {
                answer = buffer[i][j] * buffer[i+5000][j];
            }
        }
        //gettimeofday(&tp,NULL);
        //diff_u = 1000000L*(tp.tv_sec-start_s)+(tp.tv_usec-start_u);
        diff_u = 1000000L * et.lap();
        if (time4 > diff_u)
            time4 = diff_u;
        base = 5000000./(time4);
        if (speed4 < base)
            speed4 = base;
        fflush(stdout);
    }
    printf("%6.3lf MNops ",speed1);
    printf("%6.3lf Mflops (%.1lf) ",speed2,speed1/speed2);
    printf("%6.3lf Mflops (%.1lf) ",speed3,speed1/speed3);
    printf("%6.3lf Mflops (%.1lf)",speed4,speed1/speed4);
    printf("\n");
}
