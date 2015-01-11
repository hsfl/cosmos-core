/**********************************************************************************************
 * arduino_agent.cpp
 * ********************************************************************************************/
//#include <sys/statfs.h>
#include <sys/stat.h>
#include <iostream>

using namespace std;
#include <stdio.h>

#include "jsonlib.h"
#include "agentlib.h"
#include "cosmos-defs.h"
#include "physicslib.h"
#include "mathlib.h"
#include "jsonlib.h"

cosmosstruc *cdata;

int myagent();

// request functions 

int32_t request_setspeed (char *request, char *response, void *cdata) ;
int32_t request_setheading (char *request, char *response, void *cdata) ;
int32_t request_getpos (char *request, char *response, void *cdata) ;
int32_t request_getutc (char *request, char *response, void *cdata) ;

char fname[100], base[100];
double year, day;
int32_t iyear, iday, iseconds;
char 	agentname[COSMOS_MAX_NAME] = "rover";
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
	if (!(cdata = agent_setup_server(SOCKET_TYPE_MULTICAST,argv[1],agentname,1.,0,MAXBUFFERSIZE)) != 0)
		exit (AGENT_ERROR_JSON_CREATE);
}
else
{
	if (!(cdata = agent_setup_server(SOCKET_TYPE_MULTICAST,(char *)"rover",agentname,1.,0,MAXBUFFERSIZE)) != 0)
		exit (AGENT_ERROR_JSON_CREATE);
}

// initialize position for kicks....
nmjd = lmjd = cmjd = currentmjd(cdata->node.utcoffset) ;
speed = heading = 0.;
cspeed = cheading = 0.;
dspeed = dheading = 0.;
cdata->node.loc.pos.geod.v.lat = cdata->node.loc.pos.geod.v.lon = cdata->node.loc.pos.geod.v.h = 0.;
cdata->node.loc.pos.geod.a.lat = cdata->node.loc.pos.geod.a.lon = cdata->node.loc.pos.geod.a.h = 0.;
cdata->node.loc.pos.geod.s.lat = RADOF(21.29747);
cdata->node.loc.pos.geod.s.lon = RADOF(-157.815967);
cdata->node.loc.pos.geod.utc = cmjd ;
cdata->node.loc.pos.geod.s.h = 22.;
pos_geod(&cdata->node.loc);

// sets rover speed
if ((iretn=agent_add_request(cdata, (char *)"setspeed", request_setspeed)) !=0)
	exit (iretn) ;
// sets rover heading
if ((iretn=agent_add_request(cdata, (char *)"setheading", request_setheading)) !=0)
	exit (iretn) ;
// gets rover position
if ((iretn=agent_add_request(cdata, (char *)"getpos", request_getpos)) !=0)
	exit (iretn) ;
// gets satellite time
if ((iretn=agent_add_request(cdata, (char *)"getutc", request_getutc)) !=0)
	exit (iretn) ;

// Start our own thread
iretn = myagent();
}

int myagent()
{
rvector testv;
double dt;
int  	i;
string jstring;
period = 1./86400.;
fname[0] = 0;

// Start performing the body of the agent
// Information gathering thing....
//
FILE *fout =  NULL ;

// start the loop
printf ("start agent ...\r\n") ;
while(agent_running(cdata))
	{
	cmjd = currentmjd (cdata->node.utcoffset) ;
	cdata->node.loc.utc = cmjd;
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
		cdata->devspec.motr[i]->spd = speed/cdata->devspec.motr[i]->rat;
		}
	cdata->node.loc.pos.geod.a.lon = dspeed * sin(cheading) / (dt*cos(cdata->node.loc.pos.geod.s.lat)*cdata->node.loc.pos.geos.s.r);
	cdata->node.loc.pos.geod.a.lat = dspeed * cos(cheading) / (dt*cdata->node.loc.pos.geos.s.r);
	cdata->node.loc.pos.geod.v.lon = cspeed * sin(cheading) / (cos(cdata->node.loc.pos.geod.s.lat)*cdata->node.loc.pos.geos.s.r);
	cdata->node.loc.pos.geod.v.lat = cspeed * cos(cheading) / (cdata->node.loc.pos.geos.s.r);
	cdata->node.loc.pos.geod.v.h = cdata->node.loc.pos.geod.a.h = 0.;
	cdata->node.loc.pos.geod.s.lon += cdata->node.loc.pos.geod.v.lon * dt;
	cdata->node.loc.pos.geod.s.lat += cdata->node.loc.pos.geod.v.lat * dt;
	cdata->node.loc.pos.geod.s.h += cdata->node.loc.pos.geod.v.h * dt;
	cdata->node.loc.pos.geod.utc = cdata->node.loc.utc;

	// Set attitude in Topocentric
	cdata->node.loc.att.topo.utc = cdata->node.loc.pos.eci.utc;
	cdata->node.loc.att.topo.s = q_change_around_z(-cheading);
	cdata->node.loc.att.topo.v = rv_smult(dheading/dt,rv_unitz());
	cdata->node.loc.att.topo.a = rv_zero();

	// Synchronize positions and attitudes
	pos_geod(&cdata->node.loc);
	att_planec2topo(&cdata->node.loc);

	testv = rotate_q(q_conjugate(cdata->node.loc.att.icrf.s),rv_unitz());
	testv = rv_normal(cdata->node.loc.pos.eci.s);
	testv = rotate_q(q_conjugate(cdata->node.loc.att.geoc.s),rv_unitz());
	testv = rv_normal(cdata->node.loc.pos.geoc.s);

	// Simulate hardware
	cdata->physics.dt = dt;
	simulate_hardware(*cdata, cdata->node.loc);
	agent_post(cdata, AGENT_MESSAGE_SOH,json_of_soh(jstring, cdata));

	// Broadcast and Log SOH
	if (cmjd > nmjd)
		{

//        printf("%.15g %.15g %.15g %.15g %.15g %.15g\n",cdata->node.loc.utc,DEGOF(cdata->node.loc.pos.geod.s.lat),DEGOF(cdata->node.loc.pos.geod.s.lon),cdata->node.loc.pos.eci.s.col[0],cdata->node.loc.pos.eci.s.col[1],cdata->node.loc.pos.eci.s.col[2]);
		printf("%.15g %.10g %.10g %.10g %.10g %.10g %.10g %.10g\n",cdata->node.loc.utc,DEGOF(cdata->node.loc.pos.geod.s.lat),DEGOF(cdata->node.loc.pos.geod.s.lon),cspeed,cheading,cdata->node.powgen,cdata->node.powuse,dt);

        if ((int)cmjd != (int)nmjd || fname[0] == 0)
			{
            sprintf(fname,"%s/data",base);
			COSMOS_MKDIR(fname,00777);
			COSMOS_MKDIR(fname,00777);
            year = mjd2year(cdata->node.loc.utc);
			iyear = (int)year;
            sprintf(fname,"%s/data/%04d",base,iyear);
			COSMOS_MKDIR(fname,00777);
            day = 365.26 * (year-iyear) + 1.;
			iday = (int)day;
            sprintf(fname,"%s/data/%04d/%03d",base,iyear,iday);
			COSMOS_MKDIR(fname,00777);
            iseconds = (int)(86400.*(day-iday));
			sprintf(fname,"%s/data/%04d/%03d/%s.%4d%03d%05d.telemetry",base,iyear,iday,cdata->node.name,iyear,iday,iseconds);
//			sprintf(ename,"data/%04d/%03d/%s.%4d%03d%05d.event",iyear,iday,cdata->node.name,iyear,iday,iseconds);
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

int32_t request_setheading (char *request, char *output, void *cdata)
{
double value;

sscanf(request,"setheading %lf",&value);
heading = RADOF(value);
printf("Setheading: %f\n",value);

return 0 ;
}

int32_t request_setspeed (char *request, char *output, void *cdata)
{
double value;

sscanf(request,"setspeed %lf",&value);
speed = value;
printf("Setspeed: %f\n",value);

return 0 ;

}

int32_t request_getutc(char *request, char* output, void *cdata)
{

sprintf(output,"%f",((cosmosstruc *)cdata)->node.loc.utc);
return 0;
}

int32_t request_getpos(char *request, char *output, void *cdata)
{

sprintf(output,"%.15g %.15g %.15g",DEGOF(((cosmosstruc *)cdata)->node.loc.pos.geod.s.lat),DEGOF(((cosmosstruc *)cdata)->node.loc.pos.geod.s.lon),((cosmosstruc *)cdata)->node.loc.pos.geod.s.h);
return 0 ;
}

