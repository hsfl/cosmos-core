#include "support/configCosmos.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "support/elapsedtime.h"
#include "device/cpu/devicecpu.h"

DeviceCpu deviceCpu;

//#define BUFSIZE 10000000

//double buffer1[10001];
//double buffer2[10001];
//double buffer3a[100][100];
//double buffer3b[1000][1000];
//double buffer3c[10000][10000];

char *srcbuf;
ElapsedTime et;
unsigned long size, count;
long start_s, start_u, diff_u;
//, time1, time2, time3, time4, time5;
double start_t;
long i, j, k, ii;
int inb;
struct stat sbuf;
double answer, base, speed1, speed2, speed3, speed4, speed5;
size_t loopcnt;

int main(int argc, char **argv)
{
    double free = deviceCpu.getVirtualMemoryTotal() - deviceCpu.getVirtualMemoryUsed();
    size_t sizec = sqrt(free) / 12;
    size_t sizeb = sizec / 3;
    size_t sizea = sizeb / 3;
    size_t sized = sizec * sizec;
    double* buffer1 = new double[sizec + 1];
    double* buffer2 = new double[sizec + 1];
    double* buffer3 = new double[sizec * sizec];
    printf("Performing calculations on %lu %lu %lu %lu\n", sizea, sizeb, sizec, sized);

    speed1 = speed2 = speed3 = 0.;
//    time1 = time2 = time3 =time4 = time5 = sized;

    for (size_t i=0; i<sizec; ++i)
    {
        buffer1[i] = et.split();
        buffer2[i] = et.split();
    }

    // Check speed of assignment
    loopcnt = 0;
    et.reset();
    do
    {
        for (size_t i=0; i<sizec; ++i)
        {
            buffer1[i] = buffer2[i];
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dtime = et.split();
    double dassign = dtime / (loopcnt*sizec);
    printf("Assign %6.3lf Massignps (%.3lf)\n",1e-6/dassign, dassign/dassign);
    fflush(stdout);

    // Check speed of multiply by constant
    loopcnt = 0;
    et.reset();
    do
    {
        for (size_t i=0; i<sizec; ++i)
        {
            buffer1[i] = 1.234 * buffer2[i];
        }
        ++loopcnt;
    } while (et.split() < 5.);
    dtime = et.split();
    double dscalar = (dtime / (loopcnt*sizec));
    printf("Scalar Multiply %6.3lf Msflops (%.3lf)\n",1e-6/dscalar, dassign/dscalar);
    fflush(stdout);

    // Check speed of multiply by variable
    loopcnt = 0;
    et.reset();
    do
    {
        for (size_t i=0; i<sizec; ++i)
        {
            buffer1[i] = buffer1[i] * buffer2[i];
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dvariable = (et.split() / (loopcnt*sizec));
    printf("Variable Multiply %6.3lf Mvflops (%.3lf)\n",1e-6/dvariable, dassign/dvariable);
    fflush(stdout);

    // Check speed of multiply by variable, 100x100
    loopcnt = 0;
    et.reset();
    do
    {
        size_t k = 0;
        for (size_t i=0; i<sizea; ++i)
        {
            for (size_t j=0; j<sizea; ++j)
            {
                buffer3[k++] = buffer1[i] * buffer2[j];
            }
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dvariablesizea = (et.split() / (loopcnt*sizea*sizea));
    printf("Small Array Multiply %6.3lf Mvsizeaflops (%.3lf)\n",1e-6/dvariablesizea, dassign/dvariablesizea);
    fflush(stdout);


    // Check speed of multiply by variable, sizebxsizeb
    loopcnt = 0;
    for (size_t i=0; i<sizec; ++i)
    {
        buffer1[i] = et.split();
        buffer2[i] = et.split();
    }
    et.reset();
    do
    {
        size_t k = 0;
        for (size_t i=0; i<sizeb; ++i)
        {
            for (size_t j=0; j<sizeb; ++j)
            {
                buffer3[k++] = buffer1[i] * buffer2[j];
            }
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dvariablesizeb = (et.split() / (loopcnt*sizeb*sizeb));
    printf("Medium Array Multiply %6.3lf Mvsizebflops (%.3lf)\n",1e-6/dvariablesizeb, dassign/dvariablesizeb);
    fflush(stdout);

    // Check speed of multiply by variable, sizecxsizec
    loopcnt = 0;
    for (size_t i=0; i<sizec; ++i)
    {
        buffer1[i] = et.split();
        buffer2[i] = et.split();
    }
    et.reset();
    do
    {
        size_t k = 0;
        for (size_t i=0; i<sizec; ++i)
        {
            for (size_t j=0; j<sizec; ++j)
            {
                buffer3[k++] = buffer1[i] * buffer2[j];
            }
        }
        ++loopcnt;
    } while (et.split() < 5.);
    double dvariablesizec = (et.split() / (loopcnt*sized));
    printf("Large Array Multiply %6.3lf Mvsizecflops (%.3lf)",1e-6/dvariablesizec, dassign/dvariablesizec);
    printf("\n");

}
