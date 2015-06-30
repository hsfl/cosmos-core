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

#define TLE 0
#define STK 1
#define DT .1

int main(int argc, char *argv[])
{
	cartpos eci;
	vector <tlestruc> tle;
	stkstruc stk;
	double utc = 0.;
	string modelname;
	int modeltype;

	switch (argc)
	{
	case 3:
		utc = atof(argv[2]);
	case 2:
		modelname = argv[1];
		break;
	default:
		printf("Usage: tlevspropagator modelname [mjd] \n");
		exit(1);
		break;
	}

	if (modelname[modelname.size()-3] == 't')
	{
		modeltype = TLE;
		load_lines(argv[1], tle);
	}
	else
	{
		modeltype = STK;
		load_stk(argv[1], &stk);
	}

	if (utc == 0.)
	{
		if (modeltype == TLE)
		{
			utc = tle[0].utc;
			tle2eci(utc, tle[0], &eci);
		}
		else
		{
			utc = stk.pos[1].utc;
			stk2eci(utc, &stk, &eci);
		}
	}

	double tt = cal2mjd(2004, 4, 6, 7, 51, 28, 386009000);
	double dpsi = DEGOF(utc2dpsi(tt));
	double deps = DEGOF(utc2depsilon(tt));
	double eps = DEGOF(utc2epsilon(tt));
	double omega = DEGOF(utc2omega(tt));
	double gmst = DEGOF(utc2gmst1982(tt));
	double gast = DEGOF(utc2gast(tt));
	tt = utc2jcentt(tt);

	rmatrix pm;
	double temeutc = cal2mjd(2000, 0, 182.78495062)+1;
	double eeq = DEGOF(utc2gast(temeutc) - utc2gmst1982(temeutc));
	dpsi = DEGOF(utc2dpsi(temeutc));
	deps = DEGOF(utc2depsilon(temeutc));
	eps = DEGOF(utc2epsilon(temeutc));
	//51726.78495062;
	rvector teme = {{-9060473.73569, 4645709.52502, 813686.73153}};
	teme2true(temeutc, &pm);
	teme = rv_mmult(pm, teme);
	true2mean(temeutc, &pm);
	teme = rv_mmult(pm, teme);
	mean2j2000(temeutc, &pm);
	teme = rv_mmult(pm, teme);
	j20002gcrf(temeutc, &pm);
	teme = rv_mmult(pm, teme);

	rvector mod = {{7022.465305, -1400.082889, 0.221526}};
	mean2j2000(utc, &pm);
	rvector j2000 = {{7022.312444, -1400.849398, -0.110870}};
	rvector my2000 = rv_mmult(pm, mod);
	gcrf2j2000(utc, &pm);
	my2000 = rv_mmult(pm, my2000);
	j20002gcrf(utc, &pm);
	my2000 = rv_mmult(pm, my2000);
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
//	cdata->physics.mass = 400000.;
//	cdata->physics.area = 200.;
	cdata->physics.mass = 3.;
	cdata->physics.area = .01;
	gauss_jackson_init_eci(gjh, 6, 0, DT, utc, eci, att, *cdata);

	for (size_t i=1; i<10000; ++i)
	{
		double cmjd;
		if (modeltype == TLE)
		{
			cmjd = utc + i*DT*10/86400.;
			gauss_jackson_propagate(gjh, *cdata, cmjd);
			tle2eci(cmjd, tle[0], &eci);
		}
		else
		{
			cmjd = utc + i*DT*10/86400.;
			gauss_jackson_propagate(gjh, *cdata, cmjd);
			stk2eci(cmjd, &stk, &eci);
		}
		printf("%.15g\t%.8g\t%.8g\t%.8g\t%.8g\t%.8g\t%.8g\n", cmjd, eci.s.col[0], eci.s.col[1], eci.s.col[2], cdata->node.loc.pos.eci.s.col[0], cdata->node.loc.pos.eci.s.col[1], cdata->node.loc.pos.eci.s.col[2]);
	}
}
