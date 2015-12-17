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

#include <unistd.h>
#include "vmt35_lib.h"
#include "microstrain_lib.h"

int main(int argc, char *argv[])
{
int iretn, handle;
int16_t icurrent;
uint16_t ivoltage;
float current;
rvector magfield, mtrvec;

iretn = vmt35_connect((char *)"dev_vmt35");

if (iretn < 0)
	{
	printf("Error: vmt35_connect() %d\n",iretn);
	exit (iretn);
	}

vmt35_enable();
vmt35_set_amps(0,0.);
vmt35_set_amps(1,0.);
vmt35_set_amps(2,0.);
vmt35_disable();

}
