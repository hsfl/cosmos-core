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
#include "support/configCosmos.h"
#include <sys/stat.h>
#include <iostream>

//using namespace std;
#include <stdio.h>

#include "support/jsonlib.h"
#include "agent/agentclass.h"
#include "support/cosmos-defs.h"
#include "physics/physicslib.h"
#include "math/mathlib.h"
#include "support/jsonlib.h"

Agent *agent;

int myagent();

// request functions 

int32_t request_setspeed (char *request, char *response, Agent *agent) ;
int32_t request_setheading (char *request, char *response, Agent *agent) ;
int32_t request_getpos (char *request, char *response, Agent *agent) ;
int32_t request_getutc (char *request, char *response, Agent *agent) ;

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
    if (!(agent = new Agent(argv[1], agentname, 1., MAXBUFFERSIZE, false, 0, NetworkType::MULTICAST)) != 0)
		exit (AGENT_ERROR_JSON_CREATE);
}
else
{
    if (!(agent = new Agent("rover", agentname, 1., MAXBUFFERSIZE, false, 0, NetworkType::MULTICAST)) != 0)
		exit (AGENT_ERROR_JSON_CREATE);
}

// initialize position for kicks....
nmjd = lmjd = cmjd = currentmjd(agent->cinfo->pdata.node.utcoffset) ;
speed = heading = 0.;
cspeed = cheading = 0.;
dspeed = dheading = 0.;
agent->cinfo->pdata.node.loc.pos.geod.v.lat = agent->cinfo->pdata.node.loc.pos.geod.v.lon = agent->cinfo->pdata.node.loc.pos.geod.v.h = 0.;
agent->cinfo->pdata.node.loc.pos.geod.a.lat = agent->cinfo->pdata.node.loc.pos.geod.a.lon = agent->cinfo->pdata.node.loc.pos.geod.a.h = 0.;
agent->cinfo->pdata.node.loc.pos.geod.s.lat = RADOF(21.29747);
agent->cinfo->pdata.node.loc.pos.geod.s.lon = RADOF(-157.815967);
agent->cinfo->pdata.node.loc.pos.geod.utc = cmjd ;
agent->cinfo->pdata.node.loc.pos.geod.s.h = 22.;
pos_geod(&agent->cinfo->pdata.node.loc);

// sets rover speed
if ((iretn=agent->add_request("setspeed", request_setspeed)) !=0)
	exit (iretn) ;
// sets rover heading
if ((iretn=agent->add_request("setheading", request_setheading)) !=0)
	exit (iretn) ;
// gets rover position
if ((iretn=agent->add_request("getpos", request_getpos)) !=0)
	exit (iretn) ;
// gets satellite time
if ((iretn=agent->add_request("getutc", request_getutc)) !=0)
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
while(agent->running())
	{
    cmjd = currentmjd (agent->cinfo->pdata.node.utcoffset) ;
    agent->cinfo->pdata.node.loc.utc = cmjd;
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
        agent->cinfo->pdata.devspec.motr[i]->spd = speed/agent->cinfo->pdata.devspec.motr[i]->rat;
		}
    agent->cinfo->pdata.node.loc.pos.geod.a.lon = dspeed * sin(cheading) / (dt*cos(agent->cinfo->pdata.node.loc.pos.geod.s.lat)*agent->cinfo->pdata.node.loc.pos.geos.s.r);
    agent->cinfo->pdata.node.loc.pos.geod.a.lat = dspeed * cos(cheading) / (dt*agent->cinfo->pdata.node.loc.pos.geos.s.r);
    agent->cinfo->pdata.node.loc.pos.geod.v.lon = cspeed * sin(cheading) / (cos(agent->cinfo->pdata.node.loc.pos.geod.s.lat)*agent->cinfo->pdata.node.loc.pos.geos.s.r);
    agent->cinfo->pdata.node.loc.pos.geod.v.lat = cspeed * cos(cheading) / (agent->cinfo->pdata.node.loc.pos.geos.s.r);
    agent->cinfo->pdata.node.loc.pos.geod.v.h = agent->cinfo->pdata.node.loc.pos.geod.a.h = 0.;
    agent->cinfo->pdata.node.loc.pos.geod.s.lon += agent->cinfo->pdata.node.loc.pos.geod.v.lon * dt;
    agent->cinfo->pdata.node.loc.pos.geod.s.lat += agent->cinfo->pdata.node.loc.pos.geod.v.lat * dt;
    agent->cinfo->pdata.node.loc.pos.geod.s.h += agent->cinfo->pdata.node.loc.pos.geod.v.h * dt;
    agent->cinfo->pdata.node.loc.pos.geod.utc = agent->cinfo->pdata.node.loc.utc;

	// Set attitude in Topocentric
    agent->cinfo->pdata.node.loc.att.topo.utc = agent->cinfo->pdata.node.loc.pos.eci.utc;
    agent->cinfo->pdata.node.loc.att.topo.s = q_change_around_z(-cheading);
    agent->cinfo->pdata.node.loc.att.topo.v = rv_smult(dheading/dt,rv_unitz());
    agent->cinfo->pdata.node.loc.att.topo.a = rv_zero();

	// Synchronize positions and attitudes
    pos_geod(&agent->cinfo->pdata.node.loc);
    att_planec2topo(&agent->cinfo->pdata.node.loc);

    testv = rotate_q(q_conjugate(agent->cinfo->pdata.node.loc.att.icrf.s),rv_unitz());
    testv = rv_normal(agent->cinfo->pdata.node.loc.pos.eci.s);
    testv = rotate_q(q_conjugate(agent->cinfo->pdata.node.loc.att.geoc.s),rv_unitz());
    testv = rv_normal(agent->cinfo->pdata.node.loc.pos.geoc.s);

	// Simulate hardware
    agent->cinfo->pdata.physics.dt = dt;
    simulate_hardware(agent->cinfo->pdata, agent->cinfo->pdata.node.loc);
    agent->post(Agent::AGENT_MESSAGE_SOH,json_of_soh(jstring, agent->cinfo->meta, agent->cinfo->pdata));

	// Broadcast and Log SOH
	if (cmjd > nmjd)
		{

//        printf("%.15g %.15g %.15g %.15g %.15g %.15g\n", agent->cinfo->pdata.node.loc.utc,DEGOF(agent->cinfo->pdata.node.loc.pos.geod.s.lat),DEGOF(agent->cinfo->pdata.node.loc.pos.geod.s.lon), agent->cinfo->pdata.node.loc.pos.eci.s.col[0], agent->cinfo->pdata.node.loc.pos.eci.s.col[1], agent->cinfo->pdata.node.loc.pos.eci.s.col[2]);
        printf("%.15g %.10g %.10g %.10g %.10g %.10g %.10g %.10g\n", agent->cinfo->pdata.node.loc.utc,DEGOF(agent->cinfo->pdata.node.loc.pos.geod.s.lat),DEGOF(agent->cinfo->pdata.node.loc.pos.geod.s.lon),cspeed,cheading, agent->cinfo->pdata.node.powgen, agent->cinfo->pdata.node.powuse,dt);

        if ((int)cmjd != (int)nmjd || fname[0] == 0)
			{
            sprintf(fname,"%s/data",base);
			COSMOS_MKDIR(fname,00777);
			COSMOS_MKDIR(fname,00777);
            year = mjd2year(agent->cinfo->pdata.node.loc.utc);
			iyear = (int)year;
            sprintf(fname,"%s/data/%04d",base,iyear);
			COSMOS_MKDIR(fname,00777);
            day = 365.26 * (year-iyear) + 1.;
			iday = (int)day;
            sprintf(fname,"%s/data/%04d/%03d",base,iyear,iday);
			COSMOS_MKDIR(fname,00777);
            iseconds = (int)(86400.*(day-iday));
            sprintf(fname,"%s/data/%04d/%03d/%s.%4d%03d%05d.telemetry",base,iyear,iday, agent->cinfo->pdata.node.name,iyear,iday,iseconds);
//			sprintf(ename,"data/%04d/%03d/%s.%4d%03d%05d.event",iyear,iday, agent->cinfo->pdata.node.name,iyear,iday,iseconds);
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

int32_t request_setheading (char *request, char *output, Agent *agent)
{
double value;

sscanf(request,"setheading %lf",&value);
heading = RADOF(value);
printf("Setheading: %f\n",value);

return 0 ;
}

int32_t request_setspeed (char *request, char *output, Agent *agent)
{
double value;

sscanf(request,"setspeed %lf",&value);
speed = value;
printf("Setspeed: %f\n",value);

return 0 ;

}

int32_t request_getutc(char *request, char* output, Agent *agent)
{

sprintf(output,"%f",agent->cinfo->pdata.node.loc.utc);
return 0;
}

int32_t request_getpos(char *request, char *output, Agent *agent)
{

sprintf(output,"%.15g %.15g %.15g",DEGOF(agent->cinfo->pdata.node.loc.pos.geod.s.lat),DEGOF(agent->cinfo->pdata.node.loc.pos.geod.s.lon),agent->cinfo->pdata.node.loc.pos.geod.s.h);
return 0 ;
}

