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
#include "math/mathlib.h"
#include "jsonlib.h"
#include "datalib.h"
#include "jsonlib.h"
#include "timelib.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

//#define MMCORRECT -2.766301289e-10
#define MMCORRECT 0.0

cosmosstruc *cdata;
stkstruc stk;
shorteventstruc events[20];
string mainjstring;
int ecount;

int main(int argc, char *argv[])
{
	int i;
	FILE *fp1, *fp2, *fp3;
	cartpos npos;
	double lmjd=0.;
	double nmjd;
	double utc;
	rvector bearth;
	ssenstruc ssen[1];
	tsenstruc tsen[14];


	cdata[0].physics.mode = atol(argv[1]);
	json_setup_node((char *)"dice",cdata);

	load_lines((char *)"tle_dice1.tle", cdata[0].tle);

	fp1 = fopen("dice_attitude_mag_gpsweek_1691.txt","r");
	fp2 = fopen("dice_attitude_sun_gpsweek_1691.txt","r");
	fp3 = fopen("dice_housekeeping_tmp_gpsweek_1691.txt","r");

	fscanf(fp1,"%lf %lf %lf %lf",&utc,&bearth.col[0],&bearth.col[1],&bearth.col[2]);
	lmjd = (utc * 864000.) / 864000.;
	nmjd = (int)(lmjd);

	for (utc=nmjd; utc<lmjd; utc+=10./86400.)
	{
		cdata[0].node.loc.utc = currentmjd(cdata[0].node.utcoffset);
		if (lines2eci(utc, cdata[0].tle, npos) < 0)
			exit (1);
		cdata[0].node.loc.utc = utc;
		update_eci(*cdata, cdata[0].node.loc.utc, npos);
		log_write((char *)"dice",DATA_LOG_TYPE_SOH,utc,json_of_soh(mainjstring, cdata));
		//		ecount = check_events(events,20,cdata);
		for (i=0; i<ecount; i++)
		{
			cdata[0].event[0].s = events[i];
			strcpy(cdata[0].event[0].l.condition,cdata[0].emap[events[i].handle.hash][events[i].handle.index].text);
			log_write((char *)"dice",DATA_LOG_TYPE_EVENT,utc,json_of_event(mainjstring, cdata));
		}
	}

	cdata[0].node.loc.utc = lmjd;

	while (1)
	{
		do
		{
			if (fscanf(fp1,"%lf %lf %lf %lf",&utc,&bearth.col[0],&bearth.col[1],&bearth.col[2]) == EOF)
				break;
		} while (utc < lmjd);
		if (feof(fp1))
			break;

		do
		{
			if (fscanf(fp2,"%lf %f %f %f %f",&utc,&ssen[0].qva,&ssen[0].qvb,&ssen[0].qvc,&ssen[0].qvd) == EOF)
				break;
		} while (utc < lmjd);
		if (feof(fp1))
			break;

		do
		{
			if (fscanf(fp3,"%lf",&utc) == EOF)
				break;
			for (i=0; i<14; i++)
			{
				if (fscanf(fp3," %f",&tsen[i].gen.temp) == EOF)
					break;
			}
		} while (utc < lmjd);
		if (feof(fp1))
			break;

		cdata[0].node.loc.utc = currentmjd(cdata[0].node.utcoffset);
		if (lines2eci(utc, cdata[0].tle, npos) < 0)
			exit (1);
		cdata[0].node.loc.utc = utc;
		update_eci(*cdata, cdata[0].node.loc.utc, npos);
		bearth.col[2] = 0.;
		cdata[0].node.loc.bearth = bearth;
		cdata[0].devspec.imu[0]->mag = bearth;
		*cdata[0].devspec.ssen[0] = ssen[0];
		for (i=0; i<14; i++)
		{
			*cdata[0].devspec.tsen[i] = tsen[i];
		}
		log_write((char *)"dice",DATA_LOG_TYPE_SOH,utc,(char *)json_of_soh(mainjstring, cdata));
		lmjd += 10./86400.;
		//		ecount = check_events(events,20,cdata);
		for (i=0; i<ecount; i++)
		{
			cdata[0].event[0].s = events[i];
			strcpy(cdata[0].event[0].l.condition,cdata[0].emap[events[i].handle.hash][events[i].handle.index].text);
			log_write((char *)"dice",DATA_LOG_TYPE_EVENT,utc,json_of_event(mainjstring, cdata));
		}
	}

	nmjd = (int)(lmjd+1);
	for (utc=lmjd; utc<nmjd; utc+=10./86400.)
	{
		cdata[0].node.loc.utc = currentmjd(cdata[0].node.utcoffset);
		if (lines2eci(utc, cdata[0].tle, npos) < 0)
			exit (1);
		cdata[0].node.loc.utc = utc;
		update_eci(*cdata, cdata[0].node.loc.utc, npos);
		log_write((char *)"dice",DATA_LOG_TYPE_SOH,utc,json_of_soh(mainjstring, cdata));
		//		ecount = check_events(events,20,cdata);
		for (i=0; i<ecount; i++)
		{
			cdata[0].event[0].s = events[i];
			strcpy(cdata[0].event[0].l.condition,cdata[0].emap[events[i].handle.hash][events[i].handle.index].text);
			log_write((char *)"dice",DATA_LOG_TYPE_EVENT,utc,json_of_event(mainjstring, cdata));
		}
	}

}
