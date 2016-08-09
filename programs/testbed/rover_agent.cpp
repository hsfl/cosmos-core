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

/**********************************************************************************************
 * arduino_agent.cpp
 * ********************************************************************************************/
#include "configCosmos.h"
#include <sys/stat.h>
#include <iostream>

//using namespace std;
#include <stdio.h>

#include "jsonlib.h"
#include "agentlib.h"
#include "cosmos-defs.h"
#include "physics/physicslib.h"
#include "math/mathlib.h"
#include "jsonlib.h"

cosmosstruc *cinfo;

int myagent();

// request functions 

int32_t request_setspeed (char *request, char *response, void *cinfo) ;
int32_t request_setheading (char *request, char *response, void *cinfo) ;
int32_t request_getpos (char *request, char *response, void *cinfo) ;
int32_t request_getutc (char *request, char *response, void *cinfo) ;

char fname[100], base[100];
double year, day;
int32_t iyear, iday, iseconds;
char 	agentname[COSMOS_MAX_NAME+1] = "rover";
char 	ipaddress[16] = "192.168.150.1";
int 	waitsec = 5;
double	lmjd, cmjd, nmjd, period;
double heading, speed;
double cheading, cspeed;
double dheading, dspeed;
int32_t myport;
 
 // strings for passing requests to the loop thread

#define MAXBUFFERSIZE 256

#define REQUEST_RUN_PROGRAM 0

// Here are some variables we will map to JSON names

int main(int argc, char *argv[])
{
int 	iretn ;


// Initialize Agent
if (argc == 2)
{
	if (!(cinfo = agent_setup_server(NetworkType::MULTICAST,argv[1],agentname,1.,0,MAXBUFFERSIZE)) != 0)
		exit (AGENT_ERROR_JSON_CREATE);
}
else
{
	if (!(cinfo = agent_setup_server(NetworkType::MULTICAST,(char *)"rover",agentname,1.,0,MAXBUFFERSIZE)) != 0)
		exit (AGENT_ERROR_JSON_CREATE);
}

// initialize position for kicks....
nmjd = lmjd = cmjd = currentmjd(cinfo->pdata.node.utcoffset) ;
speed = heading = 0.;
cspeed = cheading = 0.;
dspeed = dheading = 0.;
cinfo->pdata.node.loc.pos.geod.v.lat = cinfo->pdata.node.loc.pos.geod.v.lon = cinfo->pdata.node.loc.pos.geod.v.h = 0.;
cinfo->pdata.node.loc.pos.geod.a.lat = cinfo->pdata.node.loc.pos.geod.a.lon = cinfo->pdata.node.loc.pos.geod.a.h = 0.;
cinfo->pdata.node.loc.pos.geod.s.lat = RADOF(21.29747);
cinfo->pdata.node.loc.pos.geod.s.lon = RADOF(-157.815967);
cinfo->pdata.node.loc.pos.geod.utc = cmjd ;
cinfo->pdata.node.loc.pos.geod.s.h = 22.;
pos_geod(&cinfo->pdata.node.loc);

// sets rover speed
if ((iretn=agent_add_request(cinfo, (char *)"setspeed", request_setspeed)) !=0)
	exit (iretn) ;
// sets rover heading
if ((iretn=agent_add_request(cinfo, (char *)"setheading", request_setheading)) !=0)
	exit (iretn) ;
// gets rover position
if ((iretn=agent_add_request(cinfo, (char *)"getpos", request_getpos)) !=0)
	exit (iretn) ;
// gets satellite time
if ((iretn=agent_add_request(cinfo, (char *)"getutc", request_getutc)) !=0)
	exit (iretn) ;

// Start our own thread
iretn = myagent();
}

int myagent()
{
rvector testv;
double dt;
int  	i;
std::string jstring;
period = 1./86400.;
fname[0] = 0;

// Start performing the body of the agent
// Information gathering thing....
//
FILE *fout =  NULL ;

// start the loop
printf ("start agent ...\r\n") ;
while(agent_running(cinfo))
	{
	cmjd = currentmjd (cinfo->pdata.node.utcoffset) ;
	cinfo->pdata.node.loc.utc = cmjd;
	dt = 86400. * (cmjd-lmjd);

	// Adjust speed
    dspeed = 0.;
	if (cspeed < speed)
		{
		if (speed-cspeed > .1*dt)
			dspeed = .1*dt;
		else
            dspeed = speed - cspeed;
		}
	else
		{
		if (cspeed > speed)
			{
			if (cspeed-speed > .1*dt)
				dspeed = -.1*dt;
			else
                dspeed = speed - cspeed;
			}
		else
			dspeed = 0.;
		}
	cspeed += dspeed;

    // Adjust heading
    dheading = 0.;
	if (cheading < heading)
		{
        if (heading-cheading > .1*dt)
            dheading = .1*dt;
		else
            dheading = heading - cheading;
		}
	else
		{
		if (cheading > heading)
			{
            if (cheading-heading > .1*dt)
                dheading = -.1*dt;
			else
                dheading = heading - cheading;
			}
		}
	cheading += dheading;
	cheading = ranrm(cheading);

	// Calculate new location from speed and heading
	for (i=0; i<6; i++)
		{
		cinfo->pdata.devspec.motr[i]->spd = speed/cinfo->pdata.devspec.motr[i]->rat;
		}
	cinfo->pdata.node.loc.pos.geod.a.lon = dspeed * sin(cheading) / (dt*cos(cinfo->pdata.node.loc.pos.geod.s.lat)*cinfo->pdata.node.loc.pos.geos.s.r);
	cinfo->pdata.node.loc.pos.geod.a.lat = dspeed * cos(cheading) / (dt*cinfo->pdata.node.loc.pos.geos.s.r);
	cinfo->pdata.node.loc.pos.geod.v.lon = cspeed * sin(cheading) / (cos(cinfo->pdata.node.loc.pos.geod.s.lat)*cinfo->pdata.node.loc.pos.geos.s.r);
	cinfo->pdata.node.loc.pos.geod.v.lat = cspeed * cos(cheading) / (cinfo->pdata.node.loc.pos.geos.s.r);
	cinfo->pdata.node.loc.pos.geod.v.h = cinfo->pdata.node.loc.pos.geod.a.h = 0.;
	cinfo->pdata.node.loc.pos.geod.s.lon += cinfo->pdata.node.loc.pos.geod.v.lon * dt;
	cinfo->pdata.node.loc.pos.geod.s.lat += cinfo->pdata.node.loc.pos.geod.v.lat * dt;
	cinfo->pdata.node.loc.pos.geod.s.h += cinfo->pdata.node.loc.pos.geod.v.h * dt;
	cinfo->pdata.node.loc.pos.geod.utc = cinfo->pdata.node.loc.utc;

	// Set attitude in Topocentric
	cinfo->pdata.node.loc.att.topo.utc = cinfo->pdata.node.loc.pos.eci.utc;
	cinfo->pdata.node.loc.att.topo.s = q_change_around_z(-cheading);
	cinfo->pdata.node.loc.att.topo.v = rv_smult(dheading/dt,rv_unitz());
	cinfo->pdata.node.loc.att.topo.a = rv_zero();

	// Synchronize positions and attitudes
	pos_geod(&cinfo->pdata.node.loc);
	att_planec2topo(&cinfo->pdata.node.loc);

	testv = rotate_q(q_conjugate(cinfo->pdata.node.loc.att.icrf.s),rv_unitz());
	testv = rv_normal(cinfo->pdata.node.loc.pos.eci.s);
	testv = rotate_q(q_conjugate(cinfo->pdata.node.loc.att.geoc.s),rv_unitz());
	testv = rv_normal(cinfo->pdata.node.loc.pos.geoc.s);

	// Simulate hardware
	cinfo->pdata.physics.dt = dt;
    simulate_hardware(cinfo->pdata, cinfo->pdata.node.loc);
    agent_post(cinfo, AGENT_MESSAGE_SOH,json_of_soh(jstring, cinfo->meta, cinfo->pdata));

	// Broadcast and Log SOH
	if (cmjd > nmjd)
		{

//        printf("%.15g %.15g %.15g %.15g %.15g %.15g\n",cinfo->pdata.node.loc.utc,DEGOF(cinfo->pdata.node.loc.pos.geod.s.lat),DEGOF(cinfo->pdata.node.loc.pos.geod.s.lon),cinfo->pdata.node.loc.pos.eci.s.col[0],cinfo->pdata.node.loc.pos.eci.s.col[1],cinfo->pdata.node.loc.pos.eci.s.col[2]);
		printf("%.15g %.10g %.10g %.10g %.10g %.10g %.10g %.10g\n",cinfo->pdata.node.loc.utc,DEGOF(cinfo->pdata.node.loc.pos.geod.s.lat),DEGOF(cinfo->pdata.node.loc.pos.geod.s.lon),cspeed,cheading,cinfo->pdata.node.powgen,cinfo->pdata.node.powuse,dt);

        if ((int)cmjd != (int)nmjd || fname[0] == 0)
			{
            sprintf(fname,"%s/data",base);
			COSMOS_MKDIR(fname,00777);
			COSMOS_MKDIR(fname,00777);
			year = mjd2year(cinfo->pdata.node.loc.utc);
			iyear = (int)year;
            sprintf(fname,"%s/data/%04d",base,iyear);
			COSMOS_MKDIR(fname,00777);
            day = 365.26 * (year-iyear) + 1.;
			iday = (int)day;
            sprintf(fname,"%s/data/%04d/%03d",base,iyear,iday);
			COSMOS_MKDIR(fname,00777);
            iseconds = (int)(86400.*(day-iday));
			sprintf(fname,"%s/data/%04d/%03d/%s.%4d%03d%05d.telemetry",base,iyear,iday,cinfo->pdata.node.name,iyear,iday,iseconds);
//			sprintf(ename,"data/%04d/%03d/%s.%4d%03d%05d.event",iyear,iday,cinfo->pdata.node.name,iyear,iday,iseconds);
			}

		fout = fopen(fname,"a+");
		fprintf(fout,"%s\n",jstring.c_str());
		fclose(fout);
		nmjd += period;
		}

	lmjd = cmjd;
	COSMOS_USLEEP(100000);
	}


return 0;
}

int32_t request_setheading (char *request, char *output, void *cinfo)
{
double value;

sscanf(request,"setheading %lf",&value);
heading = RADOF(value);
printf("Setheading: %f\n",value);

return 0 ;
}

int32_t request_setspeed (char *request, char *output, void *cinfo)
{
double value;

sscanf(request,"setspeed %lf",&value);
speed = value;
printf("Setspeed: %f\n",value);

return 0 ;

}

int32_t request_getutc(char *request, char* output, void *cinfo)
{

sprintf(output,"%f",((cosmosstruc *)cinfo)->pdata.node.loc.utc);
return 0;
}

int32_t request_getpos(char *request, char *output, void *cinfo)
{

sprintf(output,"%.15g %.15g %.15g",DEGOF(((cosmosstruc *)cinfo)->pdata.node.loc.pos.geod.s.lat),DEGOF(((cosmosstruc *)cinfo)->pdata.node.loc.pos.geod.s.lon),((cosmosstruc *)cinfo)->pdata.node.loc.pos.geod.s.h);
return 0 ;
}

