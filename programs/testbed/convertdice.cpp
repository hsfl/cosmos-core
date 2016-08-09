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

#include "physics/physicslib.h"
#include "math/mathlib.h"
#include "jsonlib.h"
#include "datalib.h"
#include "jsonlib.h"
#include "timelib.h"
#include "agent/agent.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

//#define MMCORRECT -2.766301289e-10
#define MMCORRECT 0.0

stkstruc stk;
shorteventstruc events[20];
std::string mainjstring;
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


    cosmosAgent agent(NetworkType::UDP, "dice");
    agent.cinfo->pdata.physics.mode = atol(argv[1]);

    load_lines("tle_dice1.tle", agent.cinfo->pdata.tle);

	fp1 = fopen("dice_attitude_mag_gpsweek_1691.txt","r");
	fp2 = fopen("dice_attitude_sun_gpsweek_1691.txt","r");
	fp3 = fopen("dice_housekeeping_tmp_gpsweek_1691.txt","r");

	fscanf(fp1,"%lf %lf %lf %lf",&utc,&bearth.col[0],&bearth.col[1],&bearth.col[2]);
	lmjd = (utc * 864000.) / 864000.;
	nmjd = (int)(lmjd);

	for (utc=nmjd; utc<lmjd; utc+=10./86400.)
	{
        agent.cinfo->pdata.node.loc.utc = currentmjd(agent.cinfo->pdata.node.utcoffset);
        if (lines2eci(utc, agent.cinfo->pdata.tle, npos) < 0)
			exit (1);
        agent.cinfo->pdata.node.loc.utc = utc;
        update_eci(agent.cinfo->pdata, agent.cinfo->pdata.node.loc.utc, npos);
        log_write((char *)"dice",DATA_LOG_TYPE_SOH,utc,json_of_soh(mainjstring, agent.cinfo->meta, agent.cinfo->pdata));
        //		ecount = check_events(events,20,cinfo);
		for (i=0; i<ecount; i++)
		{
            agent.cinfo->pdata.event[0].s = events[i];
            strcpy(agent.cinfo->pdata.event[0].l.condition,agent.cinfo->meta.emap[events[i].handle.hash][events[i].handle.index].text);
            log_write((char *)"dice",DATA_LOG_TYPE_EVENT,utc,json_of_event(mainjstring, agent.cinfo->meta, agent.cinfo->pdata));
		}
	}

    agent.cinfo->pdata.node.loc.utc = lmjd;

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

        agent.cinfo->pdata.node.loc.utc = currentmjd(agent.cinfo->pdata.node.utcoffset);
        if (lines2eci(utc, agent.cinfo->pdata.tle, npos) < 0)
			exit (1);
        agent.cinfo->pdata.node.loc.utc = utc;
        update_eci(agent.cinfo->pdata, agent.cinfo->pdata.node.loc.utc, npos);
		bearth.col[2] = 0.;
        agent.cinfo->pdata.node.loc.bearth = bearth;
        agent.cinfo->pdata.devspec.imu[0]->mag = bearth;
        *agent.cinfo->pdata.devspec.ssen[0] = ssen[0];
		for (i=0; i<14; i++)
		{
            *agent.cinfo->pdata.devspec.tsen[i] = tsen[i];
		}
        log_write((char *)"dice",DATA_LOG_TYPE_SOH,utc,(char *)json_of_soh(mainjstring, agent.cinfo->meta, agent.cinfo->pdata));
		lmjd += 10./86400.;
        //		ecount = check_events(events,20,cinfo);
		for (i=0; i<ecount; i++)
		{
            agent.cinfo->pdata.event[0].s = events[i];
            strcpy(agent.cinfo->pdata.event[0].l.condition,agent.cinfo->meta.emap[events[i].handle.hash][events[i].handle.index].text);
            log_write((char *)"dice",DATA_LOG_TYPE_EVENT,utc,json_of_event(mainjstring, agent.cinfo->meta, agent.cinfo->pdata));
		}
	}

	nmjd = (int)(lmjd+1);
	for (utc=lmjd; utc<nmjd; utc+=10./86400.)
	{
        agent.cinfo->pdata.node.loc.utc = currentmjd(agent.cinfo->pdata.node.utcoffset);
        if (lines2eci(utc, agent.cinfo->pdata.tle, npos) < 0)
			exit (1);
        agent.cinfo->pdata.node.loc.utc = utc;
        update_eci(agent.cinfo->pdata, agent.cinfo->pdata.node.loc.utc, npos);
        log_write((char *)"dice",DATA_LOG_TYPE_SOH,utc,json_of_soh(mainjstring, agent.cinfo->meta, agent.cinfo->pdata));
        //		ecount = check_events(events,20,cinfo);
		for (i=0; i<ecount; i++)
		{
            agent.cinfo->pdata.event[0].s = events[i];
            strcpy(agent.cinfo->pdata.event[0].l.condition,agent.cinfo->meta.emap[events[i].handle.hash][events[i].handle.index].text);
            log_write((char *)"dice",DATA_LOG_TYPE_EVENT,utc,json_of_event(mainjstring, agent.cinfo->meta, agent.cinfo->pdata));
		}
	}

}
