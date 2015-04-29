/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

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

//get_cosmosresources();

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
