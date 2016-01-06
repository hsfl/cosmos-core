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

#include "physicslib.h"
#include "jsonlib.h"
#include "jsonlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

cosmosstruc *cdata;

int main(int argc, char *argv[])
{
int16_t n;
std::string output;
FILE *odes;
std::string jstring;

cdata = json_create();
load_databases(argv[1],(uint16_t)atol(argv[2]),cdata);

// Battery capacity
cdata[0].node.battcap = 0.;
for (n=0; n<cdata[0].devspec.batt_cnt; n++)
	{
	cdata[0].node.battcap += cdata[0].devspec.batt[n]->capacity;
	}
cdata[0].node.battlev = cdata[0].node.battcap;

output = json_of_node(jstring, cdata);
odes = fopen("node.ini","w");
fputs(output.c_str(),odes);
fclose(odes);
//output = json_groundstation(jstring, cdata);
//odes = fopen("groundstation.ini","w");
//fputs(output,odes);
//fclose(odes);
//create_ui_tcs();
}
