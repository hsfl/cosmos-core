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
#include "sinclair_lib.h"
#include "timelib.h"

char device[50] = "/dev/wheel";

int main(int argc, char *argv[])
{
int32_t iretn, i;
sinclair_state handle;
//char buf[256];
double mjd, cmjd, speed;

if (argc == 2) strcpy(device,argv[1]);
iretn = sinclair_rw_connect(device,0x11,0x3e, &handle);

if (iretn < 0)
	{
	printf("Error: sinclair_rw_connect() %d\n",iretn);
	exit (1);
	}

iretn = nsp_ping(&handle);
printf("%s\n",(char *)handle.mbuf.data);
iretn = sinclair_mode_accel(&handle,50.);
for (i=1; i<4; i++)
	{
	mjd = currentmjd(0.);
	iretn = sinclair_mode_accel(&handle,i*50.);
	do
		{
		speed = sinclair_get_speed(&handle);
		printf("%.5f\r",speed);
	fflush(stdout);
		} while (speed < 400.);
	cmjd = currentmjd(0.);
	iretn = sinclair_mode_speed(&handle,0.);
	printf("\n%.15g %f\n",86400.*(cmjd-mjd),400./(86400.*(cmjd-mjd)));
	fflush(stdout);

	mjd = currentmjd(0.);
	iretn = sinclair_mode_accel(&handle,-i*50.);
	do
		{
		speed = sinclair_get_speed(&handle);
		printf("%.5f\r",speed);
	fflush(stdout);
		} while (speed > 0.);
	cmjd = currentmjd(0.);
	printf("\n%.15g %f\n",86400.*(cmjd-mjd),400./(86400.*(cmjd-mjd)));
	fflush(stdout);
	iretn = sinclair_mode_speed(&handle,0.);
	}
iretn = sinclair_mode_speed(&handle,0.);
iretn = sinclair_disconnect(&handle);
}
