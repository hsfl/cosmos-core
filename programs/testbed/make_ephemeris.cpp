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
#include "agent/agent.h"
#include "jsonlib.h"
#include "physicslib.h"
#include "math/mathlib.h"
#include "jsonlib.h"
#include "agentlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
//#include <sys/types.h>
#include <sys/stat.h>

std::vector<nodestruc> track;
gj_handle gjh;
char fname[200];
std::string buf;
svector azel;

int main(int argc, char *argv[])
{
	double fyear;
	beatstruc imubeat;
	double mjdnow, lastlat, lastsunradiance, fd, lastgsel[MAXTRACK];
	std::string output;
	char date[30];
	FILE *eout, *fout;
	int i, j, gsup[MAXTRACK];
	int year, month, day, hour, minute, second;
	struct stat sbuf;
	std::string jstring;

    cosmosAgent agent(NetworkType::UDP, argv[1]);
    agent.get_server(agent.cinfo->pdata.node.name,(char *)"simulator",5,&imubeat);
    agent.send_request(imubeat, "statevec", buf, 5);
    json_parse(buf, agent.cinfo->meta, agent.cinfo->pdata);
    agent.send_request(imubeat, "value node_name", buf, 5);
    json_parse(buf, agent.cinfo->meta, agent.cinfo->pdata);

    mjdnow = agent.cinfo->pdata.node.loc.utc;
	fyear = mjd2year(mjdnow);
	year = (int)fyear;
	day = (int)(365.26 * (fyear-year)) + 1;
	sprintf(fname,"data/%4d/%03d",year,day);
#if defined(COSMOS_WIN_OS)
	mkdir(fname);
#else
	mkdir(fname,00775);
#endif

	sprintf(fname,"data/%4d/%03d/orbitalevents",year,day);
	if (stat(fname,&sbuf) == -1)
	{
		// Move old orbitalevents to directory
		rename("orbitalevents",fname);
	}


    hardware_init_eci(agent.cinfo->pdata.devspec, agent.cinfo->pdata.node.loc);
    gauss_jackson_init_eci(gjh, 4, 0, 5., mjdnow, agent.cinfo->pdata.node.loc.pos.eci, agent.cinfo->pdata.node.loc.att.icrf, agent.cinfo->pdata.physics, agent.cinfo->pdata.node.loc);
    simulate_hardware(agent.cinfo->pdata, agent.cinfo->pdata.node.loc);
    mjdnow = agent.cinfo->pdata.node.loc.utc;
    lastlat = agent.cinfo->pdata.node.loc.pos.geod.s.lat;
	for (j=0; j<3; j++)
	{
		lastgsel[j] = azel.phi;
		gsup[j] = 0;
	}
    lastsunradiance = agent.cinfo->pdata.node.loc.pos.sunradiance;
	fout = fopen("ephemeris","w");
	eout = fopen("orbitalevents","w");
	for (i=0; i<8640; i++)
	{
		mjdnow += 10./86400.;
        gauss_jackson_propagate(gjh, agent.cinfo->pdata.physics, agent.cinfo->pdata.node.loc, mjdnow);
        simulate_hardware(agent.cinfo->pdata, agent.cinfo->pdata.node.loc);
        output = json_of_ephemeris(jstring,  agent.cinfo->meta, agent.cinfo->pdata);
		fprintf(fout,"%s\n",output.c_str());
		fflush(fout);
//		mjd2cal(mjdnow,&year,&month,&day,&fd,&iretn);
		mjd2ymd(mjdnow, year, month, fd);
		day = (int32_t)fd;
		fd -= day;
		hour = (int)(24. * fd);
		minute = (int)(1440. * fd - hour * 60.);
		second = (int)(86400. * fd - hour * 3600. - minute * 60.);
		sprintf(date,"%4d-%02d-%02d,%02d:%02d:%02d,O00000,",year,month,day,hour,minute,second);
        if (lastlat <= RADOF(-60.) && agent.cinfo->pdata.node.loc.pos.geod.s.lat >= RADOF(-60.))
			fprintf(eout,"%sS60A\n",date);
        if (lastlat <= RADOF(-30.) && agent.cinfo->pdata.node.loc.pos.geod.s.lat >= RADOF(-30.))
			fprintf(eout,"%sS30A\n",date);
        if (lastlat <= 0. && agent.cinfo->pdata.node.loc.pos.geod.s.lat >= 0.)
			fprintf(eout,"%sEQA\n",date);
        if (lastlat <= RADOF(30.) && agent.cinfo->pdata.node.loc.pos.geod.s.lat >= RADOF(30.))
			fprintf(eout,"%sN30A\n",date);
        if (lastlat <= RADOF(60.) && agent.cinfo->pdata.node.loc.pos.geod.s.lat >= RADOF(60.))
			fprintf(eout,"%sN60A\n",date);
        if (lastlat >= RADOF(-60.) && agent.cinfo->pdata.node.loc.pos.geod.s.lat <= RADOF(-60.))
			fprintf(eout,"%sS60D\n",date);
        if (lastlat >= RADOF(-30.) && agent.cinfo->pdata.node.loc.pos.geod.s.lat <= RADOF(-30.))
			fprintf(eout,"%sS30D\n",date);
        if (lastlat >= 0. && agent.cinfo->pdata.node.loc.pos.geod.s.lat <= 0.)
			fprintf(eout,"%sEQD\n",date);
        if (lastlat >= RADOF(30.) && agent.cinfo->pdata.node.loc.pos.geod.s.lat <= RADOF(30.))
			fprintf(eout,"%sN30D\n",date);
        if (lastlat >= RADOF(60.) && agent.cinfo->pdata.node.loc.pos.geod.s.lat <= RADOF(60.))
			fprintf(eout,"%sN60D\n",date);

        if (lastsunradiance == 0. && agent.cinfo->pdata.node.loc.pos.sunradiance > 0.)
		{
			date[20] = JSON_TYPE_UINT32;
			fprintf(eout,"%sUMB_OUT\n",date);
		}
        if (lastsunradiance > 0. && agent.cinfo->pdata.node.loc.pos.sunradiance == 0.)
		{
			date[20] = JSON_TYPE_UINT32;
			fprintf(eout,"%sUMB_IN\n",date);
		}
        lastlat = agent.cinfo->pdata.node.loc.pos.geod.s.lat;
        lastsunradiance = agent.cinfo->pdata.node.loc.pos.sunradiance;
		for (j=0; j<3; j++)
		{
			if (azel.phi <= lastgsel[j] && azel.phi > 0.)
			{
				if (gsup[j] > 0.)
				{
					date[20] = 'G';
					fprintf(eout,"%sMAX_%3s_%03.0f\n",date,track[j].name,DEGOF(azel.phi));
				}
				gsup[j] = -1;
			}
			else
				gsup[j] = 1;
			if (azel.phi < RADOF(10.) && lastgsel[j] >= RADOF(10.))
			{
				date[20] = 'G';
				fprintf(eout,"%sLOS+10_%3s\n",date,track[j].name);
			}
			if (azel.phi < RADOF(5.) && lastgsel[j] >= RADOF(5.))
			{
				date[20] = 'G';
				fprintf(eout,"%sLOS+5_%3s\n",date,track[j].name);
			}
			if (azel.phi < 0. && lastgsel[j] >= 0.)
			{
				date[20] = 'G';
				fprintf(eout,"%sLOS_%3s\n",date,track[j].name);
			}
			if (azel.phi >= 0. && lastgsel[j] < 0.)
			{
				date[20] = 'G';
				fprintf(eout,"%sAOS_%3s\n",date,track[j].name);
			}
			if (azel.phi >= RADOF(5.) && lastgsel[j] < RADOF(5.))
			{
				date[20] = 'G';
				fprintf(eout,"%sAOS+5_%3s\n",date,track[j].name);
			}
			if (azel.phi >= RADOF(10.) && lastgsel[j] < RADOF(10.))
			{
				date[20] = 'G';
				fprintf(eout,"%sAOS+10_%3s\n",date,track[j].name);
			}
			lastgsel[j] = azel.phi;
		}
	}
	fclose(fout);
	fclose(eout);
}
