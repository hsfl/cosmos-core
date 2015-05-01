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
	rvector mag, magm, mags;
	uint32_t count;

	iretn = microstrain_connect(argv[1]);
	COSMOS_USLEEP(100000);

	if (iretn < 0)
		{
		printf("Error: microstrain_connect() %d\n",iretn);
		exit (1);
		}

	handle = iretn;

	count = 1;
	iretn = microstrain_magfield(handle, &magm);
	COSMOS_USLEEP(100000);
	mags = rv_mult(magm, magm);
	for (uint16_t i=0; i<19; ++i)
	{
		iretn = microstrain_magfield(handle, &mag);
		COSMOS_USLEEP(100000);
		magm = rv_add(magm, mag);
		mags = rv_add(mags, rv_mult(mag, mag));
		++count;
	}

	mags = rv_sqrt(rv_smult(1./19., rv_sub(mags, rv_smult(1./20., rv_mult(magm, magm)))));
	magm = rv_smult(1./20., magm);

	printf("%+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f\n",magm.col[0], magm.col[1], magm.col[2], mags.col[0], mags.col[1], mags.col[2], length_rv(magm));
	iretn = microstrain_disconnect(handle);
}
