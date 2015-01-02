#include "timelib.h"
#include "convertlib.h"
#include "datalib.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[])
{
double utc, hour, min, sec;
double ut1, dut1, dt, theta, gmst, gast, eps, deps, dpsi;
double deg, amin, asec;

get_resdir();

for (ut1=49809.; ut1<=53809.; ut1+=100.)
	{
	dut1 = utc2dut1(ut1);
	utc = ut1 - dut1;
	dut1 = utc2dut1(utc);
	utc = ut1 - dut1;
	dt = utc2tt(utc) - ut1;
	printf("%14.6f %7.3f %7.3f ",ut1+2400000.5,dt*86400.,dut1*86400.);
	theta = utc2theta(utc);
	deg = DEGOF(theta);
	amin = 60.*(deg - (int)deg);
	asec = 60.*(amin - (int)amin);
//	printf("[%3d %2d %7.4f] ",(int)deg,(int)amin,asec);
	printf("%.15g ",theta);
	gmst = utc2gmst(utc);
	hour = 24. * gmst / D2PI;
	min = 60.*(hour - (int)hour);
	sec = 60.*(min - (int)min);
	printf("[%2d %2d %7.4f] ",(int)hour,(int)min,sec);
//	printf("%.15g ",gmst);
	gast = utc2gast(utc);
	hour = 24. * gast / D2PI;
	min = 60.*(hour - (int)hour);
	sec = 60.*(min - (int)min);
//	printf("[%2d %2d %7.4f] ",(int)hour,(int)min,sec);
	printf("%.15g ",gast);
	eps = utc2epsilon(utc);
	deg = DEGOF(eps);
	amin = 60.*(deg - (int)deg);
	asec = 60.*(amin - (int)amin);
//	printf("[%3d %2d %7.4f] ",(int)deg,(int)amin,asec);
	printf("%.15g ",eps);
	deps = utc2depsilon(utc);
	asec = deps / DAS2R;
	printf("%8.4f ",asec);
	dpsi = utc2dpsi(utc);
	asec = dpsi / DAS2R;
	printf("%8.4f\n",asec);
	}
}
