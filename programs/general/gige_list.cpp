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

#include "gige_lib.h"
#include "agentlib.h"
#include "datalib.h"
#include "time.h"
#include "elapsedtime.h"

int main(int argc, char *argv[])
{
    std::vector<gige_acknowledge_ack> gige_list;
    double wait_time = 0.;

    if (argc == 2)
    {
        wait_time = atof(argv[1]);
    }

    ElapsedTime et;
    et.start();
    do
    {
	gige_list = gige_discover();
    } while (gige_list.size() == 0 && et.split() < wait_time);

	if (!gige_list.size())
	{
		printf("Couldn't find any cameras\n");
		exit(1);
	}

	for (uint16_t i=0; i<gige_list.size(); ++i)
	{
		printf("Camera %u: %s %s %s\n", i, gige_value_to_address(gige_list[i].address), gige_list[i].serial_number, gige_list[i].manufacturer);
	}

    exit(0);
}
