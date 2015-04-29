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

// #include <unistd.h>
#include "microstrain_lib.h"
#include "sinclair_lib.h"
#include "timelib.h"

int main(int argc, char *argv[])
{
int32_t iretn, mh, direction;
sinclair_state sh;
rvector vaccel, vrate, mag, irate;
rvector bias;
rmatrix matrix, imatrix;
double theading, cheading, dheading;
double maxaccel, trate, maxspeed;
double lastmjd, mjd, sspeed;
uint32_t count;
avector euler;

iretn = microstrain_connect((char *)"derv_imu");
if (iretn < 0)
	{
	printf("Error: microstrain_connect() %d\n",iretn);
	exit (1);
	}
mh = iretn;

iretn = sinclair_rw_connect((char *)"derv_wheel",0x11,0x3e, &sh);
if (iretn < 0)
	{
	printf("Error: sinclair_rw_connect() %d\n",iretn);
	iretn = microstrain_disconnect(mh);
	exit (1);
	}

microstrain_capture_gyro_bias(mh,10000,&bias);

iretn = microstrain_accel_rate_magfield_orientation(mh, &vaccel, &irate, &mag, &imatrix);
imatrix = rm_transpose(imatrix);
euler = a_quaternion2euler(q_dcm2quaternion_rm(matrix));
theading = 1.57;
cheading = 0.;

maxspeed = 400.;
maxaccel = 20.;
mjd = lastmjd = currentmjd(0.);
count = 0;
while (86400.*(lastmjd-mjd)<120.)
	{
//	iretn = microstrain_euler_angles(mh, &euler);
	iretn = microstrain_accel_rate_magfield_orientation(mh, &vaccel, &vrate, &mag, &matrix);
	matrix = rm_mmult(matrix,imatrix);
	vrate = rv_mmult(imatrix,rv_sub(vrate,irate));
	euler = a_quaternion2euler(q_dcm2quaternion_rm(matrix));
	cheading = -euler.h;
	dheading = theading - cheading;
	if (dheading < -DPI2)
		dheading += DPI;
	if (dheading > DPI2)
		dheading -= DPI;

	trate = sqrt(.004*2.*maxaccel*fabs(dheading));
	if (dheading < 0.)
		trate = -trate;
	direction = 0;
	if (vrate.col[2] > trate)
		direction = -1;
	else
		direction = 1;

	sspeed = sinclair_get_speed(&sh);
	if (fabs(sspeed) >= maxspeed || direction == 0)
		sinclair_mode_accel(&sh,0.);
	else
		sinclair_mode_accel(&sh,direction*maxaccel);

	lastmjd = currentmjd(0.);
	printf("%10.5f %10.5f %10.5f %10.5f %10.5f %10.5f %2d\n",86400.*(lastmjd-mjd),dheading,trate,cheading,vrate.col[2],sspeed,direction);
	fflush(stdout);
	count++;
	}
sinclair_mode_accel(&sh,0.);
sinclair_mode_speed(&sh,0.);
iretn = microstrain_disconnect(mh);
iretn = sinclair_disconnect(&sh);
}
