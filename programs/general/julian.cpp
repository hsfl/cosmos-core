#include "support/configCosmos.h"
#include "support/timelib.h"
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

int main(int argc, char *argv[])
{
    struct timeval mytime;
    struct tm *mytm;
    int tut, value, ttype, i, iretn, tlen;
    int cflag = 0;
    int vflag = 0;
    time_t thetime;
    double mjd, ra, dec, diam, lat, lon, lst, ha, zd, air, slon, slat;

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
                vflag = 1;
                break;
            case 'l':
                /* Time in HST */
                tut = 0;
                break;
            case 'u':
                /* Time in UT */
                tut = 1;
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
                default:
                    /* Time in Seconds */
                    ttype = TSECOND;
                    break;
                }
                break;
            case 'm':
                /* Time in Minutes */
                ttype = TMINUTE;
                break;
            case 'h':
                /* Time in Hours */
                ttype = THOUR;
                break;
            case 'd':
                /* Time in Days */
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
            case 'n':
                /* Time in Months */
                ttype = TMDAY;
                break;
            case 'g':
                /* Time in Months */
                ttype = TMONTH;
                break;
            case 'y':
                /* Time in Years */
                ttype = TYEAR;
                break;
            case 'z':
                ttype = TZENITH;
                tut = 1;
                break;
            case 'a':
                ttype = TAIRMASS;
                tut = 1;
                break;
            case 'j':
                ttype = TMJD;
                tut = 1;
                break;
            }
        }
    }

    /* Now get time and format it appropriately */

    gettimeofday(&mytime, NULL);
    do
    {
        thetime = mytime.tv_sec;
        switch (tut)
        {
        case 0:
            mytm = localtime(&thetime);
            break;
        case 1:
            mytm = gmtime(&thetime);
            break;
        }

        mjd = cal2mjd(mytm->tm_year+1900, mytm->tm_mon+1, mytm->tm_mday);
        lat = 20.5 * (3.1415926/180.);
        lon = -157.5 * (3.1415926/180.);
//        slaRdplan(mjd,0,lon,lat,&ra,&dec,&diam);
        mjd += ((mytm->tm_hour + (mytm->tm_min + mytm->tm_sec / 60.) / 60.) / 24.);
//        lst = slaGmst(mjd) + lon;
        ha = lst - ra;
        slon = (lon - ha);
        if (slon < M_PI)
            slon += 2.*M_PI;
        slat = dec;
//        zd = slaZd(ha,dec,lat);
//        air = slaAirmas(zd);

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
        case TSECOND:
            value = mytm->tm_sec;
            printf("%02d\n",value);
            break;
        case TMINUTE:
            value = mytm->tm_min;
            printf("%02d\n",value);
            break;
        case THOUR:
            value = mytm->tm_hour;
            printf("%02d\n",value);
            break;
        case TDAY:
            value = mytm->tm_yday + 1;
            switch (tlen)
            {
            case 0:
                printf("%d\n",value);
                break;
            case 3:
                printf("%03d\n",value);
                break;
            case 2:
                printf("%02d\n",value);
                break;
            case 1:
                printf("%01d\n",value);
                break;
            }
            break;
        case TMDAY:
            value = mytm->tm_mday;
            printf("%02d\n",value);
            break;
        case TMONTH:
            value = mytm->tm_mon+1;
            printf("%02d\n",value);
            break;
        case TYEAR:
            value = mytm->tm_year+1900;
            printf("%04d\n",value);
            break;
        case TMJD:
            printf("%lf\n",mjd);
            break;
        case TALL:
            if (cflag)
                printf("%04d %02d %02d %03d %02d:%02d:%02d %10ld %6.2lf %6.3lf\r",mytm->tm_year+1900,mytm->tm_mon+1,mytm->tm_mday,mytm->tm_yday+1,mytm->tm_hour,mytm->tm_min,mytm->tm_sec,mytime.tv_sec,zd*(180./3.16149),air);
            else
                printf("%04d %02d %02d %03d %02d:%02d:%02d %10ld %6.2lf %6.3lf\n",mytm->tm_year+1900,mytm->tm_mon+1,mytm->tm_mday,mytm->tm_yday+1,mytm->tm_hour,mytm->tm_min,mytm->tm_sec,mytime.tv_sec,zd*(180./3.16149),air);
        }
        fflush(stdout);
        usleep(1000000);
    }
    while (cflag);


}
