#include "support/configCosmos.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "support/elapsedtime.h"

#define BUFSIZE 10000000

double buffer1[10001];
double buffer2[10001];
double buffer3a[100][100];
double buffer3b[1000][1000];
double buffer3c[10000][10000];
char *srcbuf;
ElapsedTime et;
unsigned long size, count;
long start_s, start_u, diff_u, time1, time2, time3, time4, time5;
double start_t;
long i, j, k, ii;
int inb;
struct stat sbuf;
double answer, base, speed1, speed2, speed3, speed4, speed5;
size_t loopcnt;

int main(int argc, char **argv)
{
    speed1 = speed2 = speed3 = 0.;
    time1 = time2 = time3 =time4 = time5 = 1000000000L;

    for (size_t i=0; i<10000; ++i)
    {
        buffer1[i] = et.split();
        buffer2[i] = et.split();
    }

    // Check speed of assignment
    loopcnt = 0;
    et.reset();
    do
    {
        for (size_t i=0; i<10000; ++i)
        {
            buffer1[i] = buffer2[i];
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dassign = et.split() / (loopcnt*10000.);
    printf("%6.3lf Massignps (%.1lf) : ",1e-6/dassign, dassign/dassign);
    fflush(stdout);

//    // Check speed of split
//    loopcnt = 0;
//    et.reset();
//    do
//    {
//        for (size_t i=0; i<10000; ++i)
//        {
//            buffer1[i] = et.split();
//        }
//        ++loopcnt;
//    } while (et.split() < 5.);
//    double dsplit = (et.split() / (loopcnt*10000.)) - dassign;
//    printf("%6.3lf Msplitps (%.1lf) : ",1e-6/dsplit, dsplit/dassign);
//    fflush(stdout);

    // Check speed of multiply by constant
    loopcnt = 0;
    et.reset();
    do
    {
        for (size_t i=0; i<10000; ++i)
        {
            buffer1[i] = 1.234 * buffer2[i];
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dscalar = (et.split() / (loopcnt*10000.)) - (dassign);
//    double dscalar = (et.split() / (loopcnt*10000.));
    printf("%6.3lf Msflops (%.1lf) : ",1e-6/dscalar, dscalar/dassign);
    fflush(stdout);

    // Check speed of multiply by variable
    loopcnt = 0;
    et.reset();
    do
    {
        for (size_t i=0; i<10000; ++i)
        {
            buffer1[i] = buffer1[i] * buffer2[i];
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dvariable = (et.split() / (loopcnt*10000.)) - (dassign);
    printf("%6.3lf Mvflops (%.1lf) : ",1e-6/dvariable, dvariable/dassign);
    fflush(stdout);

    // Check speed of multiply by variable, 100x100
    loopcnt = 0;
    et.reset();
    do
    {
        for (size_t i=0; i<100; ++i)
        {
            for (size_t j=0; j<100; ++j)
            {
                buffer3a[i][j] = buffer1[i] * buffer2[j];
            }
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dvariable100 = (et.split() / (loopcnt*10000.)) - (dassign);
    printf("%6.3lf Mv100flops (%.1lf) : ",1e-6/dvariable100, dvariable100/dassign);
    fflush(stdout);


    // Check speed of multiply by variable, 1000x1000
    loopcnt = 0;
    for (size_t i=0; i<10000; ++i)
    {
        buffer1[i] = et.split();
        buffer2[i] = et.split();
    }
    et.reset();
    do
    {
        for (size_t i=0; i<1000; ++i)
        {
            for (size_t j=0; j<1000; ++j)
            {
                buffer3a[i][j] = buffer1[i] * buffer2[j];
            }
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dvariable1000 = (et.split() / (loopcnt*1000000.)) - (dassign);
    printf("%6.3lf Mv1000flops (%.1lf) : ",1e-6/dvariable1000, dvariable1000/dassign);
    fflush(stdout);

    // Check speed of multiply by variable, 10000x10000
    loopcnt = 0;
    for (size_t i=0; i<10000; ++i)
    {
        buffer1[i] = et.split();
        buffer2[i] = et.split();
    }
    et.reset();
    do
    {
        for (size_t i=0; i<10000; ++i)
        {
            for (size_t j=0; j<10000; ++j)
            {
                buffer3a[i][j] = buffer1[i] * buffer2[j];
            }
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dvariable10000 = (et.split() / (loopcnt*100000000.)) - (dassign);
    printf("%6.3lf Mv10000flops (%.1lf)",1e-6/dvariable10000, dvariable10000/dassign);
    printf("\n");

}
