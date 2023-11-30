#include "support/configCosmos.h"
#include "support/timelib.h"
#include "support/convertlib.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define TSECOND 0
#define TMINUTE 1
#define THOUR 2
#define TDAY 3
#define TMONTH 4
#define TYEAR 5
#define TMDAY 6
#define TZENITH 7
#define TAIRMASS 8
#define TALL 9
#define TMJD 10
#define TSLON 11
#define TSLAT 12
#define TCENTI 13
#define TDECI 14
#define TNSECOND 15

int main(int argc, char *argv[])
{
    struct timeval mytime;
    int32_t tut, uvalue, ttype, i, tlen=0;
    int cflag = 0;
    //int vflag = 0;
    time_t thetime;
    double mjd, ra = 0., dec = 0., /*diam, lat,*/ lon, lst = 0., ha, zd = 0., air = 0., slon, slat;

    /* Set defaults */
    tut = 1;
    ttype = TDAY;

    /* Parse arguments */

    for (i=1; i<argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'c':
                /* Print continuously */
                cflag = 1;
                break;
            case 'v':
                /* Print verbosely */
                ttype = TALL;
                //vflag = 1;
                break;
            case 'L':
                /* Time in HST */
                tut = 0;
                break;
            case 'U':
                /* UTC */
                tut = 1;
                break;
            case 'G':
                /* GPS */
                tut = 2;
                break;
            case 'T':
                /* TT */
                tut = 3;
                break;
            case 'A':
                /* TAI */
                tut = 4;
                break;
            case 's':
                switch (argv[i][2])
                {
                case 'n':
                    /* Solar Longitude */
                    ttype = TSLON;
                    break;
                case 't':
                    /* Solar Latitude */
                    ttype = TSLAT;
                    break;
                }
                break;
            case 'N':
                /* Seconds */
                ttype = TNSECOND;
                break;
            case 'S':
                /* Seconds */
                ttype = TSECOND;
                break;
            case 'M':
                /* Time in Minutes */
                ttype = TMINUTE;
                break;
            case 'H':
                /* Time in Hours */
                ttype = THOUR;
                break;
            case 'j':
                /* DOY */
                ttype = TDAY;
                switch (argv[i][2])
                {
                case 0:
                    tlen = 0;
                    break;
                default:
                    tlen = argv[i][2] - '0';
                    break;
                }
                break;
            case 'd':
                /* Day of the Month, starting with 1 */
                ttype = TMDAY;
                break;
            case 'm':
                /* Month, starting with 1 */
                ttype = TMONTH;
                break;
            case 'y':
                /* Time in Years */
                ttype = TYEAR;
                break;
            case 'C':
                // Time in centiseconds
                ttype = TCENTI;
                break;
            case 'D':
                // Time in deciseconds
                ttype = TDECI;
                break;
            case 'z':
                ttype = TZENITH;
                tut = 1;
                break;
            case 'a':
                ttype = TAIRMASS;
                tut = 1;
                break;
            case 'J':
                ttype = TMJD;
                tut = 1;
                break;
            }
        }
    }

    /* Now get time and format it appropriately */

    gettimeofday(&mytime, NULL);
    mjd = currentmjd();
    do
    {
        switch (tut)
        {
        case 0:
            thetime = utc2unixseconds(mjd);
            struct tm *mytm;
            mytm = localtime(&thetime);
            mjd += mytm->tm_gmtoff / 86400.;
            break;
//        case 1:
//            thetime = utc2unixseconds(currentmjd());
//            mytm = gmtime(&thetime);
//            break;
        case 2:
            mjd = Convert::utc2gps(mjd);
//            thetime = utc2unixseconds(Convert::utc2gps(currentmjd()));
//            mytm = gmtime(&thetime);
            break;
        case 3:
            mjd = Convert::utc2tt(mjd);
//            thetime = utc2unixseconds(Convert::utc2tt(currentmjd()));
//            mytm = gmtime(&thetime);
            break;
        case 4:
            mjd = Convert::utc2tt(mjd) - 32.184 / 86400.;
//            thetime = utc2unixseconds(Convert::utc2tt(currentmjd()) - 32.184 / 86400.);
//            mytm = gmtime(&thetime);
            break;
        }

        calstruc mycal = mjd2cal(mjd);
        //lat = 20.5 * (3.1415926/180.);
        lon = -157.5 * (3.1415926/180.);
        ha = lst - ra;
        slon = (lon - ha);
        if (slon < M_PI)
            slon += 2.*M_PI;
        slat = dec;

        switch (ttype)
        {
        case TSLON:
            printf("%.2lf\n",slon*(180./3.16149));
            break;
        case TSLAT:
            printf("%.2lf\n",slat*(180./3.16149));
            break;
        case TAIRMASS:
            printf("%.2lf\n",air);
            break;
        case TZENITH:
            printf("%.2lf\n",zd*(180./3.16149));
            break;
        case TNSECOND:
            uvalue = mycal.nsecond;
            printf("%02d\n",uvalue);
            break;
        case TSECOND:
            uvalue = mycal.second;
            printf("%02d\n",uvalue);
            break;
        case TMINUTE:
            uvalue = mycal.minute;
            printf("%02d\n",uvalue);
            break;
        case THOUR:
            uvalue = mycal.hour;
            printf("%02d\n",uvalue);
            break;
        case TDAY:
            uvalue = mycal.doy;
            switch (tlen)
            {
            case 0:
                printf("%d\n",uvalue);
                break;
            case 3:
                printf("%03d\n",uvalue);
                break;
            case 2:
                printf("%02d\n",uvalue);
                break;
            case 1:
                printf("%01d\n",uvalue);
                break;
            }
            break;
        case TMDAY:
            uvalue = mycal.dom;
            printf("%02d\n",uvalue);
            break;
        case TMONTH:
            uvalue = mycal.month;
            printf("%02d\n",uvalue);
            break;
        case TYEAR:
            uvalue = mycal.year;
            printf("%04d\n",uvalue);
            break;
        case TCENTI:
            uvalue = centisec(mjd);
            printf("%010u\n",uvalue);
            break;
        case TDECI:
            uvalue = decisec(mjd);
            printf("%010u\n",uvalue);
            break;
        case TMJD:
            printf("%.15g\n",mjd);
            break;
        case TALL:
            if (cflag)
                printf("%04d %02d %02d %03d %02d:%02d:%02d %d %10ld %6.2lf %6.3lf\r",mycal.year,mycal.month,mycal.dom,mycal.doy+1,mycal.hour,mycal.minute,mycal.second,mycal.nsecond,mytime.tv_sec,zd*(180./3.16149),air);
            else
                printf("%04d %02d %02d %03d %02d:%02d:%02d %d %10ld %6.2lf %6.3lf\n",mycal.year,mycal.month,mycal.dom,mycal.doy+1,mycal.hour,mycal.minute,mycal.second,mycal.nsecond,mytime.tv_sec,zd*(180./3.16149),air);
        }
        fflush(stdout);
        if (cflag)
        {
            usleep(1000000);
        }
    }
    while (cflag);


}
