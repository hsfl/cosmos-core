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

#include "vn100_lib.h"
#include "timelib.h"

#define RNUM 20

vn100_handle handle;
char device[15]="/dev/ttyUSB3";

int main(int argc, char *argv[])
{
	int32_t iretn;
	double lmjd;
	rvector imag;
	imustruc imum, imus;

	if (argc == 2) strcpy(device,argv[1]);

	if ((iretn=vn100_connect(device, &handle)) < 0)
	{
		printf("Failed to open VN100 on %s, error %d\n",device,iretn);
		exit (iretn);
	}

	iretn = vn100_asynchoff(&handle);
	lmjd = currentmjd(0.);

	vn100_measurements(&handle);
	imag = handle.imu.mag;

	if ((iretn=vn100_measurements(&handle)) == 0)
	{
		imum = handle.imu;
		imus.mag = rv_mult(handle.imu.mag, handle.imu.mag);
		imus.accel = rv_mult(handle.imu.accel, handle.imu.accel);
		imus.omega = rv_mult(handle.imu.omega, handle.imu.omega);
	}
	else
	{
		printf("Failed to read VN100, error %d\n",iretn);
		exit(1);
	}

	for (uint16_t i=0; i<(RNUM-1); ++i)
	{
		if ((iretn=vn100_measurements(&handle)) == 0)
		{
			imum.mag = rv_add(imum.mag, handle.imu.mag);
			imum.accel = rv_add(imum.accel, handle.imu.accel);
			imum.omega = rv_add(imum.omega, handle.imu.omega);
			imus.mag = rv_add(imus.mag, rv_mult(handle.imu.mag, handle.imu.mag));
			imus.accel = rv_add(imus.accel, rv_mult(handle.imu.accel, handle.imu.accel));
			imus.omega = rv_add(imus.accel, rv_mult(handle.imu.omega, handle.imu.omega));
		}
		else
		{
			printf("Failed to read VN100, error %d\n",iretn);
			exit (1);
		}
	COSMOS_USLEEP(10000);
	}

	imus.mag = rv_sqrt(rv_smult(1./(RNUM-1), rv_sub(imus.mag, rv_smult(1./RNUM, rv_mult(imum.mag, imum.mag)))));
	imus.accel = rv_sqrt(rv_smult(1./(RNUM-1), rv_sub(imus.accel, rv_smult(1./RNUM, rv_mult(imum.accel, imum.accel)))));
	imus.omega = rv_sqrt(rv_smult(1./(RNUM-1), rv_sub(imus.omega, rv_smult(1./RNUM, rv_mult(imum.omega, imum.omega)))));
	imum.mag = rv_smult(1./RNUM, imum.mag);
	imum.accel = rv_smult(1./RNUM, imum.accel);
	imum.omega = rv_smult(1./RNUM, imum.omega);
	lmjd = currentmjd(0.);
	printf("[%f] Mag: %f %f %f [%f], MagSD: %f %f %f [%f]\n",86400.*(currentmjd(0.)-lmjd),imum.mag.col[0],imum.mag.col[1],imum.mag.col[2],length_rv(imum.mag),imus.mag.col[0],imus.mag.col[1],imus.mag.col[2],length_rv(imus.mag));
}
