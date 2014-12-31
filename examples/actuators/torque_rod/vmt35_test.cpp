#include <stdio.h>
#include <unistd.h>
#include "vmt35_lib.h"
#include "microstrain_lib.h"
#include "timelib.h"

int main(int argc, char *argv[])
{
double mjd, lmjd;
int iretn, handle, ecount;
int16_t icurrent;
uint16_t ivoltage;
float current;
rvector magfield, mtrvec;
vmt35_telemetry telem;

iretn = vmt35_connect((char *)"dev_vmt35");

if (iretn < 0)
	{
	printf("Error: vmt35_connect() %d\n",iretn);
	exit (iretn);
	}

/*
iretn = microstrain_connect("dev_imu"); //"/dev/ttyUSB0"

if (iretn < 0)
     {
     printf("Error: microstrain_connect() %d\n",iretn);
        exit (1);
     }

handle = iretn;
*/

vmt35_enable();
vmt35_set_voltage(0,4095);
vmt35_set_voltage(1,4095);
vmt35_set_voltage(2,4095);

ecount = 0;
while (1)
	{
usleep(20000);
mjd = currentmjd();
	current = mjd/1000000.;
ecount = 0;
usleep(20000);
do
{
	iretn = vmt35_set_amps(0,0.);
ecount += iretn;
} while (iretn);
printf("%d ",ecount);
ecount = 0;
usleep(20000);
do
{
	iretn = vmt35_set_amps(1,0.);
ecount += iretn;
} while (iretn);
printf("%d ",ecount);
ecount = 0;
usleep(20000);
do
{
	iretn = vmt35_set_amps(2,0.);
ecount += iretn;
} while (iretn);
printf("%d ",ecount);
//	sleep(2);
//	microstrain_magfield(handle,&magfield);
//	magfield.col[1] = -magfield.col[1];
//	normalize_rv(&magfield);
//	mtrvec = rv_smult(1.,rv_cross(magfield,rv_unitz()));
//	normalize_rv(&mtrvec);
//	current = mtrvec.col[0]*.2;
ecount = 0;
usleep(20000);
do
{
	iretn = vmt35_set_amps(0,current);
ecount += iretn;
} while (iretn);
printf("%d ",ecount);
//	current = mtrvec.col[1]*.2;
ecount = 0;
usleep(20000);
do
{
	iretn = vmt35_set_amps(1,current/2);
ecount += iretn;
} while (iretn);
printf("%d ",ecount);
ecount = 0;
usleep(20000);
do
{
	iretn = vmt35_set_amps(2,current/3);
ecount += iretn;
} while (iretn);
printf("%d ",ecount);
ecount = 0;
usleep(20000);
do
{
	iretn = vmt35_get_telemetry(&telem);
ecount += iretn;
} while (iretn);
printf("%d\n",ecount);
	printf("%f %hu %hu %hu %d %d %d %hu %hu\n",(mjd-lmjd)*86400.,telem.status,telem.count,telem.invalidcount,telem.daca,telem.dacb,telem.dacc,telem.temp,telem.voltage);
//	printf("[ %5.3f %5.3f %5.3f ] [ %5.3f %5.3f %5.3f ]\n",magfield.col[0],magfield.col[1],magfield.col[2],mtrvec.col[0]*.2,mtrvec.col[1]*.2,mtrvec.col[2]*.2);
	fflush(stdout);
lmjd = mjd;
	}
vmt35_disable();
exit(1);
vmt35_set_amps(0,0.);
vmt35_set_current(1,0.);
vmt35_set_current(2,0.);

current = .1;

vmt35_set_current(0,current);
vmt35_get_current(0,&icurrent);
printf("0: %u ",icurrent);
vmt35_get_current(1,&icurrent);
printf("1: %u ",icurrent);
vmt35_get_current(2,&icurrent);
printf("2: %u\n\n",icurrent);
sleep(34);
vmt35_set_current(0,0.);
vmt35_set_current(1,current);
vmt35_get_current(0,&icurrent);
printf("0: %u ",icurrent);
vmt35_get_current(1,&icurrent);
printf("1: %u ",icurrent);
vmt35_get_current(2,&icurrent);
printf("2: %u\n\n",icurrent);
sleep(4);
vmt35_set_current(1,0.);
vmt35_set_current(2,current);
vmt35_get_current(0,&icurrent);
printf("0: %u ",icurrent);
vmt35_get_current(1,&icurrent);
printf("1: %u ",icurrent);
vmt35_get_current(2,&icurrent);
printf("2: %u\n\n",icurrent);



sleep(4);

current = -.1;

vmt35_set_current(2,0.);
vmt35_set_current(0,current);
vmt35_get_current(0,&icurrent);
printf("0: %u ",icurrent);
vmt35_get_current(1,&icurrent);
printf("1: %u ",icurrent);
vmt35_get_current(2,&icurrent);
printf("2: %u\n\n",icurrent);
sleep(4);
vmt35_set_current(0,0.);
vmt35_set_current(1,current);
vmt35_get_current(0,&icurrent);
printf("0: %u ",icurrent);
vmt35_get_current(1,&icurrent);
printf("1: %u ",icurrent);
vmt35_get_current(2,&icurrent);
printf("2: %u\n\n",icurrent);
sleep(4);
vmt35_set_amps(1,0.);
vmt35_set_amps(2,current);
vmt35_get_current(0,&icurrent);
printf("0: %u ",icurrent);
vmt35_get_current(1,&icurrent);
printf("1: %u ",icurrent);
vmt35_get_current(2,&icurrent);
printf("2: %u\n\n",icurrent);
sleep(4);
vmt35_set_amps(2,0.);
vmt35_get_current(0,&icurrent);
printf("0: %u ",icurrent);
vmt35_get_current(1,&icurrent);
printf("1: %u ",icurrent);
vmt35_get_current(2,&icurrent);
printf("2: %u\n\n",icurrent);

vmt35_disable();
}
