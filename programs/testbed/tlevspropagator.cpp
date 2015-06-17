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

#include "configCosmos.h"
#include "convertlib.h"
#include "jsonlib.h"
#include "physicslib.h"
#include "agentlib.h"

int main(int argc, char *argv[])
{
	cartpos eci;
	vector <tlestruc> tle;
	double utc = 0.;
	string tlename;

	switch (argc)
	{
	case 3:
		utc = atof(argv[2]);
	case 2:
		tlename = argv[1];
		break;
	default:
		printf("Usage: tlevspropagator tlename [mjd] \n");
		exit(1);
		break;
	}

	load_lines(argv[1], tle);

	if (utc == 0.)
	{
		utc = tle[0].utc;
	}

	tle2eci(utc, tle[0], &eci);
	eci.utc = utc;

	gj_handle gjh;
	cosmosstruc *cdata;
	string node = "";
	if (!(cdata = agent_setup_client(SOCKET_TYPE_BROADCAST, node.c_str(), 1000)))
	{
			printf("Failed to setup client for node %s: %d\n", node.c_str(), AGENT_ERROR_JSON_CREATE);
			exit (AGENT_ERROR_JSON_CREATE);
	}

	qatt att;
	att.s = q_eye();
	att.v = rv_zero();
	att.a = rv_zero();
	cdata->physics.mass = 400000.;
	cdata->physics.area = 200.;
	gauss_jackson_init_eci(gjh, 6, 0, 10., utc, eci, att, *cdata);

	for (double cmjd=cdata->node.loc.utc; cmjd<utc+1.; cmjd+=cdata->physics.dtj)
	{
		gauss_jackson_propagate(gjh, *cdata, cmjd);
		tle2eci(cmjd, tle[0], &eci);
		printf("%.15g\t%.8g\t%.8g\t%.8g\t%.8g\t%.8g\t%.8g\n", cmjd, eci.s.col[0], eci.s.col[1], eci.s.col[2], cdata->node.loc.pos.eci.s.col[0], cdata->node.loc.pos.eci.s.col[1], cdata->node.loc.pos.eci.s.col[2]);
	}
}
