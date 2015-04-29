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
#include "timelib.h"

int main(int argc, char *argv[])
{
int32_t iretn, handle;
rvector vaccel, vrate, mag;
rvector vrate2, vrate1;
rvector iaccel, irate;
rvector position, velocity, z_one;
rmatrix imatrix, matrix;
double t1, t2, t3, t4;
uint32_t count;
avector euler;

iretn = microstrain_connect(argv[1]);

if (iretn < 0)
	{
	printf("Error: microstrain_connect() %d\n",iretn);
	exit (1);
	}

handle = iretn;

iretn = microstrain_euler_angles(handle, &euler);
printf(" %f %f %f\n",DEGOF(euler.b),DEGOF(euler.e),DEGOF(euler.h));
velocity = rv_zero();
iretn = microstrain_accel_rate_magfield_orientation(handle, &iaccel, &irate, &mag, &imatrix);
//imatrix = rm_transpose(imatrix);
irate = rv_mmult(imatrix,irate);
count = 0;
z_one = velocity = position = rv_zero();
z_one.col[2] = 1.;
while (count < 10000000L)
	{
	iretn = microstrain_accel_rate_magfield_orientation(handle, &vaccel, &vrate, &mag, &matrix);
	matrix = rm_mmult(imatrix,rm_transpose(matrix));
	position = rv_mmult(matrix,z_one);
	vrate1 = rv_sub(rv_mmult(imatrix,vrate),irate);
	vrate2 = rv_sub(rv_mmult(matrix,vrate),irate);
	printf("%+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f\r",vrate1.col[0],vrate2.col[0],vrate1.col[1],vrate2.col[1],vrate1.col[2],vrate2.col[2],position.col[0],position.col[1],position.col[2]);
	fflush(stdout);
//	COSMOS_SLEEP(1);
	}
iretn = microstrain_temperature(handle,&t1,&t2,&t3,&t4);
printf("\nTemperatures: %f %f %f %f\n",t1,t2,t3,t4);
iretn = microstrain_disconnect(handle);
}
