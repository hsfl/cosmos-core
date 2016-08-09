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
#include "physics/physicslib.h"
#include "agentlib.h"

#define TLE 0
#define STK 1
#define INI 2
//#define DT (1./3.)
#define DT 1.
#define MT 10

int main(int argc, char *argv[])
{
	cartpos eci;
	std::vector <tlestruc> tle;
	stkstruc stk;
	double utc = 0.;
	std::string modelname;
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

	switch (modelname[modelname.size()-3])
	{
	case 't':
		// TLE
		{
			modeltype = TLE;
			load_lines(argv[1], tle);
			tle2eci(tle[0].utc, tle[0], eci);
			tlestruc ttle;
			eci2tle(tle[0].utc, eci, ttle);
			if (utc == 0.)
			{
				utc = tle[0].utc + DT*MT/86400.;
				tle2eci(utc, tle[0], eci);
			}
		}
		break;
	case 's':
		// STK
		{
			modeltype = STK;
			load_stk(argv[1], stk);
			if (utc == 0.)
			{
				utc = stk.pos[0].utc;
				eci = stk.pos[0].pos;
			}
		}
		break;
	case 'i':
		// state.ini
		{
			locstruc loc;
			modeltype = INI;
			FILE *fdes;
			if ((fdes=fopen(argv[1],"r")) != NULL)
			{
				struct stat fstat;
				stat(argv[1], &fstat);
				char *ibuf = (char *)calloc(1,fstat.st_size+1);
				fgets(ibuf,fstat.st_size,fdes);
				switch(ibuf[15])
				{
				case 'e':
					sscanf(ibuf, "{\"node_loc_pos_eci\":{\"utc\":%lf,\"pos\":[%lf,%lf,%lf],\"vel\":[%lf,%lf,%lf]", &loc.pos.eci.utc, &loc.pos.eci.s.col[0], &loc.pos.eci.s.col[1], &loc.pos.eci.s.col[2], &loc.pos.eci.v.col[0], &loc.pos.eci.v.col[1], &loc.pos.eci.v.col[2]);
					break;
				case 'g':
					switch (ibuf[18])
					{
					case 'c':
						sscanf(ibuf, "{\"node_loc_pos_geoc\":{\"utc\":%lf,\"pos\":[%lf,%lf,%lf],\"vel\":[%lf,%lf,%lf]", &loc.pos.geoc.utc, &loc.pos.geoc.s.col[0], &loc.pos.geoc.s.col[1], &loc.pos.geoc.s.col[2], &loc.pos.geoc.v.col[0], &loc.pos.geoc.v.col[1], &loc.pos.geoc.v.col[2]);
						pos_geoc2eci(&loc);
						break;
					case 'd':
						sscanf(ibuf, "{\"node_loc_pos_geod\":{\"utc\":%lf,\"pos\":[%lf,%lf,%lf],\"vel\":[%lf,%lf,%lf]", &loc.pos.geod.utc, &loc.pos.geod.s.lat, &loc.pos.geod.s.lon, &loc.pos.geod.s.h, &loc.pos.geod.v.lat, &loc.pos.geod.v.lon, &loc.pos.geod.v.h);
						pos_geod2geoc(&loc);
						pos_geoc2eci(&loc);
						break;
					}
				}
//				json_parse(ibuf, cinfo->meta, cinfo->pdata);
				free(ibuf);
				eci = loc.pos.eci;
			}
			utc = eci.utc;
		}
		break;
	}

	eci.utc = utc;

	gj_handle gjh;
	cosmosstruc *cinfo;
	std::string node = "";
	if (!(cinfo = agent_setup_client(NetworkType::BROADCAST, node.c_str(), 1000)))
	{
			printf("Failed to setup client for node %s: %d\n", node.c_str(), AGENT_ERROR_JSON_CREATE);
			exit (AGENT_ERROR_JSON_CREATE);
	}

	qatt att;
	att.s = q_eye();
	att.v = rv_zero();
	att.a = rv_zero();
    cinfo->pdata.physics.mass = 400000.;
    cinfo->pdata.physics.area = 200.;
//	cinfo->pdata.physics.mass = 55.;
//	cinfo->pdata.physics.area = .01;

    locstruc loc;
    loc.pos.eci = eci;
    loc.att.icrf = att;
    loc.pos.eci.pass++;
    pos_eci(&loc);
    hardware_init_eci(cinfo->pdata.devspec, loc);
    gauss_jackson_init_eci(gjh, 12, 0, DT, utc, eci, att, cinfo->pdata.physics, cinfo->pdata.node.loc);
    simulate_hardware(cinfo->pdata, cinfo->pdata.node.loc);

	size_t total_count;
	switch (modeltype)
	{
	case TLE:
	case INI:
		{
			total_count = 86400.;
		}
		break;
	case STK:
		{
			total_count = stk.count-1;
		}
		break;
	}

	for (size_t i=1; i<total_count; ++i)
	{
		double cmjd;
		switch (modeltype)
		{
		case TLE:
			{
				cmjd = utc + i*DT*MT/86400.;
                gauss_jackson_propagate(gjh, cinfo->pdata.physics, cinfo->pdata.node.loc, cmjd);
                simulate_hardware(cinfo->pdata, cinfo->pdata.node.loc);
				tle2eci(cmjd, tle[0], eci);
			}
			break;
		case STK:
			{
				cmjd = stk.pos[i].utc;
                gauss_jackson_propagate(gjh, cinfo->pdata.physics, cinfo->pdata.node.loc, cmjd);
                simulate_hardware(cinfo->pdata, cinfo->pdata.node.loc);
				stk2eci(cmjd, stk, eci);
			}
			break;
		case INI:
			{
				cmjd = utc + i*DT*MT/86400.;
                gauss_jackson_propagate(gjh, cinfo->pdata.physics, cinfo->pdata.node.loc, cmjd);
                simulate_hardware(cinfo->pdata, cinfo->pdata.node.loc);
                eci = cinfo->pdata.node.loc.pos.eci;
			}
			break;
		}
		printf("%.15g\t", cmjd);
		kepstruc kep;
		eci2kep(eci, kep);
//		locstruc station;
//		svector ground;
//		station.pos.geod.s = {0.383658276, -2.788325796, 107.};
//		ground = groundstation(cinfo->pdata.node.loc, station);
//		printf("%.10g\t%.10g\t", ground.lambda, ground.phi);
//		station.pos.geod.s = {RADOF(64.8589), RADOF(-147.8356), 190.};
//		ground = groundstation(cinfo->pdata.node.loc, station);
//		printf("%.10g\t%.10g\t", ground.lambda, ground.phi);
//		station.pos.geod.s = {RADOF(51.2437), RADOF(-0.58896), 55.};
//		ground = groundstation(cinfo->pdata.node.loc, station);
//		printf("%.10g\t%.10g\t", ground.lambda, ground.phi);
//		station.pos.geod.s = {RADOF(19.826236), RADOF(-155.471951), 4160.};
//		ground = groundstation(cinfo->pdata.node.loc, station);
//		printf("%.10g\t%.10g\t", ground.lambda, ground.phi);
//		station.pos.geod.s = {RADOF(20.708137), RADOF(-156.2567), 3034.};
//		ground = groundstation(cinfo->pdata.node.loc, station);
//		printf("%.10g\t%.10g\t", ground.lambda, ground.phi);
        printf("%.10g\t%.10g\t%.10g\t%.10g\t%.10g\t%.10g\t", eci.s.col[0], eci.s.col[1], eci.s.col[2], cinfo->pdata.node.loc.pos.eci.s.col[0], cinfo->pdata.node.loc.pos.eci.s.col[1], cinfo->pdata.node.loc.pos.eci.s.col[2]);
        printf("%.10g\t%.10g\t%.10g\t%.10g\t%.10g\t%.10g\t", eci.v.col[0], eci.v.col[1], eci.v.col[2], cinfo->pdata.node.loc.pos.eci.v.col[0], cinfo->pdata.node.loc.pos.eci.v.col[1], cinfo->pdata.node.loc.pos.eci.v.col[2]);
        printf("%.10g\t%.10g\t%.10g\t%.10g\t%.10g\t%.10g\t", eci.a.col[0], eci.a.col[1], eci.a.col[2], cinfo->pdata.node.loc.pos.eci.a.col[0], cinfo->pdata.node.loc.pos.eci.a.col[1], cinfo->pdata.node.loc.pos.eci.a.col[2]);
		printf("%.10g\t%.10g\t%.10g\t%.10g\t%.10g\t%.10g\n", kep.ea, kep.e, kep.a, kep.raan, kep.i, kep.ap);
	}
}
