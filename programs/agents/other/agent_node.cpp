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

#include "support/configCosmos.h"
#include <stdio.h>
//#ifdef _MSC_BUILD
//#include "dirent/dirent.h"
//#else
//#include <dirent.h>
//#endif

#include "agent/agentclass.h"
#include "support/jsonlib.h"
#include "support/jsonlib.h"
#include "physics/physicslib.h"

#include <iostream>

#define MAXEPHEM 3

char ibuf[AGENTMAXBUFFER];

//int agent_node();

NetworkType ntype = NetworkType::UDP;
int waitsec = 5;
std::string reqjstring;
std::string myjstring;

typedef struct
{
	double mjd;
	double utime;
	uint32_t tindex;
    std::vector<std::string> telem;
	uint32_t eindex;
    std::vector<std::string> event;
} cachestruc;

cachestruc cache[3+MAXEPHEM+1];
int cindex;

uint32_t dindex;

int32_t Mcount;
int32_t mindex;
nodestruc *node;

//eventstruc tevent;
std::vector<shorteventstruc> eventdict;
std::vector<shorteventstruc> commanddict;

Agent *agent;
nodestruc statnode;
gj_handle gjh;

// Internal variables
int32_t myport;
double lastday, firstday, currentday, newlastday;

// Internal functions
double loadmjd(double mjd);
//void loadevents();
void loadephemeris();
void loanode();

// Function declarations for internal requests
int32_t request_loadmjd(string &request, string &response, Agent *);
int32_t request_counts(string &request, string &response, Agent *);
int32_t request_indexes(string &request, string &response, Agent *);
int32_t request_days(string &request, string &response, Agent *);
int32_t request_rewind(string &request, string &response, Agent *);
int32_t request_next(string &request, string &response, Agent *);
int32_t request_getmjd(string &request, string &response, Agent *);
int32_t request_getnode(string &request, string &response, Agent *);

int main(int argc, char *argv[])
{
	int32_t iretn, sleept;
	double nextmjd;


	// Check the Node Name was supplied
	if (argc!=2)
	{
		printf("Usage: agent_node node\n");
		exit(1);
	}

	// Initialize the Agent
    if (!(agent = new Agent(argv[1], "node")))
		exit (JSON_ERROR_NOJMAP);

	// Set period of broadcasting SOH
    agent->cinfo->agent[0].aprd = 1.;

	// Add additional requests
    if ((iretn=agent->add_request("loadmjd",request_loadmjd)))
		exit (iretn);
    if ((iretn=agent->add_request("counts",request_counts)))
		exit (iretn);
    if ((iretn=agent->add_request("indexes",request_indexes)))
		exit (iretn);
    if ((iretn=agent->add_request("days",request_days)))
		exit (iretn);
    if ((iretn=agent->add_request("rewind",request_rewind)))
		exit (iretn);
    if ((iretn=agent->add_request("next",request_next)))
		exit (iretn);
    if ((iretn=agent->add_request("getmjd",request_getmjd)))
		exit (iretn);
    if ((iretn=agent->add_request("getnode",request_getnode)))
		exit (iretn);


	// Check Node directory
    if (get_nodedir(agent->cinfo->node.name).empty())
	{
		printf("Couldn't find Node directory %s\n",argv[1]);
		exit (1);
	}

	// Find most recent day in archive
    lastday = findlastday(agent->cinfo->node.name);

	// Find oldest day in archive
    firstday = findfirstday(agent->cinfo->node.name);

	printf("Found days %f to %f\n",firstday, lastday);

	// Initialize data cache and load most recent day
	for (int i=0; i<3+MAXEPHEM+1; ++i)
	{
		cache[i].mjd = 0.;
		cache[i].telem.resize(0);
		cache[i].event.resize(0);
	}
    if (loadmjd(lastday) == 0. && agent->cinfo->node.type == NODE_TYPE_GROUNDSTATION)
	{
        agent->cinfo->node.loc.utc = currentmjd(0.);
        cache[3].telem.push_back(json_of_list(myjstring,(char *)"{\"node_utc\",\"node_loc_pos_geod\",\"node_loc_att_topo\",\"node_powgen\",\"node_powuse\",\"node_battlev\"", agent->cinfo));
	}

	printf("Loaded day %f\n",lastday);

	// Load Event and Command dictionaries
	//	loadevents();
    load_dictionary(eventdict,  agent->cinfo, (char *)"events.dict");
    load_dictionary(commanddict,  agent->cinfo, (char *)"commands.dict");

	printf("Loaded events\n");

	// Initialize ephemeris for this day
	loadephemeris();

	printf("Loaded Ephemeris\n");

	// Start performing the body of the agent
    json_parse((char *)(cache[3].telem[cache[3].telem.size()-1].c_str()), agent->cinfo);
//    loc_update(&agent->cinfo->node.loc);

    switch (agent->cinfo->node.type)
	{
	case NODE_TYPE_SATELLITE:
		// Initialize hardware
        hardware_init_eci(agent->cinfo, agent->cinfo->node.loc);
		// Initialize orbit
        gauss_jackson_init_eci(gjh, 8, 0, .1, agent->cinfo->node.loc.utc, agent->cinfo->node.loc.pos.eci, agent->cinfo->node.loc.att.icrf, agent->cinfo->physics, agent->cinfo->node.loc);
        simulate_hardware(agent->cinfo, agent->cinfo->node.loc);
        agent->cinfo->node.utcoffset = agent->cinfo->node.loc.utc - currentmjd(0.);
        agent->set_sohstring((char *)"{\"node_utc\",\"node_name\",\"node_type\",\"node_loc_pos_eci\",\"node_loc_att_icrf\"}");
        printf("Initialized satellite starting at %.15g [%.8g %.8g %.8g]\n", agent->cinfo->node.loc.pos.eci.utc, agent->cinfo->node.loc.pos.eci.s.col[0], agent->cinfo->node.loc.pos.eci.s.col[1], agent->cinfo->node.loc.pos.eci.s.col[2]);
		break;
	case NODE_TYPE_GROUNDSTATION:
	case NODE_TYPE_MOC:
	case NODE_TYPE_UAV:
	case NODE_TYPE_VEHICLE:
	default:
        agent->cinfo->node.utcoffset = agent->cinfo->node.loc.utc - currentmjd(0.);
        agent->set_sohstring((char *)"{\"node_utc\",\"node_name\",\"node_type\",\"node_loc_pos_geod\",\"node_loc_att_topo\"}");
        printf("Initialized ground node starting at %.15g [%.8g %.8g %.8g]\n", agent->cinfo->node.loc.pos.geod.utc, agent->cinfo->node.loc.pos.geod.s.lon, agent->cinfo->node.loc.pos.geod.s.lat, agent->cinfo->node.loc.pos.geod.s.h);
		break;
	}

	nextmjd = currentmjd(0.);
    while(agent->running())
	{
        nextmjd += agent->cinfo->agent[0].aprd/86400.;
        if ((newlastday=findlastday(agent->cinfo->node.name)) != lastday)
		{

		}
        switch (agent->cinfo->node.type)
		{
		case NODE_TYPE_SATELLITE:
            gauss_jackson_propagate(gjh, agent->cinfo->physics, agent->cinfo->node.loc, currentmjd(agent->cinfo->node.utcoffset));
            simulate_hardware(agent->cinfo, agent->cinfo->node.loc);
            break;
		default:
            agent->cinfo->node.loc.utc = agent->cinfo->node.loc.pos.geod.utc = currentmjd(agent->cinfo->node.utcoffset);
            ++agent->cinfo->node.loc.pos.geod.pass;
            pos_geod(&agent->cinfo->node.loc);
            update_target(agent->cinfo);
            agent->cinfo->node.loc.att.topo.s = q_eye();
            for (uint32_t i=0; i<agent->cinfo->node.target_cnt; ++i)
			{
                if (agent->cinfo->target[i].elfrom > 0.)
				{
                    agent->cinfo->node.loc.att.topo.s = q_fmult(q_change_around_x(agent->cinfo->target[i].elfrom),q_change_around_z(agent->cinfo->target[i].azfrom));
				}
			}
            ++agent->cinfo->node.loc.att.topo.pass;
            att_topo(&agent->cinfo->node.loc);
			break;
		}
//		agent->post(Agent::AgentMessage::SOH, json_of_list(myjstring, agent->cinfo->agent[0].sohstring,cinfo));
        agent->post(Agent::AgentMessage::SOH, json_of_table(myjstring, agent->sohtable,  agent->cinfo));
		sleept = (int)((nextmjd-currentmjd(0.))*86400000000.);
		if (sleept < 0) sleept = 0;
		COSMOS_USLEEP(sleept);
	}

	return 0;
}

//! Load Event templates
/*! Copy Event templates from file events.dict into a vector of
 * ::eventstruc. These will then be used to generate Events by
 * matching against the current data.
*/

/*
void loadevents()
{
	FILE *ifh;
	char dtemp[100];

    sprintf(dtemp,"%s/events.dict",get_nodedir(agent->cinfo->node.name));
	if ((ifh=fopen(dtemp,"r")) != NULL)
	{
		while (fgets(ibuf,AGENTMAXBUFFER,ifh) != NULL)
		{
            json_parse(ibuf, agent->cinfo);
            eventdict.push_back(agent->cinfo->event[0]);
		}
		fclose(ifh);
	}

}
*/

//! Load Ephemeris
/*! Calculate ephemeris every 20 seconds, starting with last archived
 * value, and going through next full day.
 */
void loadephemeris()
{
	int j;
	uint32_t k;
	double stime, ctime, etime;
    std::vector<shorteventstruc> events;

	// Return immediately if we haven't loaded any data
	if (cache[3].utime > 0.)
	{
		stime = cache[3].utime;
	}
	else
	{
		return;
	}

	// Cache[j] holds most recent day
    json_parse((char *)(cache[3].telem[cache[3].telem.size()-1].c_str()), agent->cinfo);
    loc_update(&agent->cinfo->node.loc);

	for (j=4; j<MAXEPHEM+4; ++j)
	{
		cache[j].telem.clear();
		cache[j].event.clear();
	}

	// Load Ephemeris with most recent data, calculated from last point of most recent day
    ctime = agent->cinfo->node.loc.utc;
	stime = (int)ctime;
	etime = stime + MAXEPHEM + 1;
    gauss_jackson_init_eci(gjh, 8, 1, 10., ctime, agent->cinfo->node.loc.pos.eci, agent->cinfo->node.loc.att.icrf, agent->cinfo->physics, agent->cinfo->node.loc);
    simulate_hardware(agent->cinfo, agent->cinfo->node.loc);
    update_target(agent->cinfo);
	do
	{
        cache[3+(int)(ctime-stime)].telem.push_back(json_of_list(myjstring,(char *)"{\"node_utc\",\"node_loc_pos_eci\",\"node_loc_att_icrf\",\"node_powgen\",\"node_powuse\",\"node_battlev\"", agent->cinfo));
        calc_events(eventdict,  agent->cinfo, events);
		for (k=0; k<events.size(); ++k)
		{
            memcpy(&agent->cinfo->event[0].s,&events[k],sizeof(shorteventstruc));
            strcpy(agent->cinfo->event[0].l.condition, agent->cinfo->emap[events[k].handle.hash][events[k].handle.index].text);
            cache[3+(int)(ctime-stime)].event.push_back(json_of_event(myjstring, agent->cinfo));
		}
		cache[3+(int)(ctime-stime)].mjd = (int)ctime;
		cache[3+(int)(ctime-stime)].utime = ctime;
		ctime += 20./86400.;
        gauss_jackson_propagate(gjh, agent->cinfo->physics, agent->cinfo->node.loc, ctime);
        simulate_hardware(agent->cinfo, agent->cinfo->node.loc);
        update_target(agent->cinfo);
	} while (ctime < etime);

}

//! Load a days worth of telemetry from disk
/*! Attempt to load the indicated MJD from disk. If not found, start
 * working forwards and backwards 1 day at a time, up to 500 days.
	\param mjd Desired Modified Julian Day.
	\return MJD of day actually loaded, otherwise 0.
*/
double loadmjd(double mjd)
{
	int i;
	double utime;

	if (mjd == 0.)
	{
		return (mjd);
	}

	//! See if we already have this day in the cache
	for (i=0; i<3+MAXEPHEM+1; ++i)
	{
		if (mjd == cache[i].mjd)
		{
			cindex = i;
			cache[i].tindex = cache[i].eindex = 0;
			return (mjd);
		}
	}

	//! If the day is not in the cache, choose the oldest cache entry
	//and replace it from the archive.
	if ((int)mjd == lastday)
	{
		// Load to 4th cache entry
		cindex = 3;
	}
	else
	{
		utime = cache[cindex].utime;
		for (i=0; i<3; ++i)
		{
			if (cache[i].utime < utime)
			{
				cindex = i;
				utime = cache[i].utime;
			}
		}
	}

    if (!data_load_archive(mjd, cache[cindex].telem, cache[cindex].event, agent->cinfo))
	{
		cache[cindex].mjd = (int)mjd;
        cache[cindex].utime = currentmjd(agent->cinfo->node.utcoffset);
		cache[cindex].tindex = cache[cindex].eindex = 0;
		return (mjd);
	}
	else
		return (0.);
}

//! Load a given day's data
/*! 
*/
int32_t request_loadmjd(string &request, string &response, Agent *)
{
	double value;

	sscanf(request.c_str(),"%*s %lf",&value);

	value = loadmjd(value);

	response = ("%f",cache[cindex].mjd);
	return 0;
}

//! Gives the number of event and telemetry records
/*! 
*/
int32_t request_counts(string &, string &response, Agent *)
{
    response = ("%" PRIu64 " %" PRIu64 " %" PRIu64 " %d",cache[cindex].telem.size(),cache[cindex].event.size(),commanddict.size(),agent->cinfo->node.target_cnt);
	return 0;
}

//! Tells first and last day in archive
/*! 
*/
int32_t request_days(string &request, string &response, Agent *)
{
	response = ("%d %d",(int)firstday,(int)lastday);
	return 0;
}

//! Goes to the first record for either events or telemetry for the loaded day
/*! 
*/
int32_t request_rewind(string &request, string &response, Agent *)
{
	char arg[50];

	sscanf(request.c_str(),"%*s %s",arg);

	switch (arg[0])
	{
	case 'e':
		cache[cindex].eindex = 0;
		break;
	case 't':
		cache[cindex].tindex = 0;
		break;
	case 'd':
		dindex = 0;
		break;
	default:
		cache[cindex].eindex = 0;
		cache[cindex].tindex = 0;
		dindex = 0;
		break;
	}

	response.clear();
	return 0;
}

//! Returns the current record of both event and telemetry data
/*! 
*/
int32_t request_indexes(string &, string &response, Agent *)
{
	response = ("%d %d %d %d",cache[cindex].tindex,cache[cindex].eindex,dindex,mindex);
	return 0;
}

//! Returns the day that is loaded
/*! 
*/
int32_t request_getmjd(string &, string &response, Agent *)
{
	response = ("%f",cache[cindex].mjd);
	return 0;
}

//! Gets next event, telemetry or data dictionary record entry.
/*! 
*/
int32_t request_next(string &request, string &response, Agent *)
{
	char arg[50];

	sscanf(request.c_str(),"%*s %s",arg);

	switch (arg[0])
	{
	case 'e':
		if (cache[cindex].eindex < cache[cindex].event.size())
		{
			response = (cache[cindex].event[cache[cindex].eindex].c_str());
			if (cache[cindex].eindex < cache[cindex].event.size()-1)
				cache[cindex].eindex++;
			return 0;
		}
		break;
	case 't':
		switch (arg[1])
		{
		case 'e':
			if (cache[cindex].tindex < cache[cindex].telem.size())
			{
				response = (cache[cindex].telem[cache[cindex].tindex].c_str());
				if (cache[cindex].tindex < cache[cindex].telem.size()-1)
					cache[cindex].tindex++;
				return 0;
			}
			break;
		case 'r':
			if (mindex >= 0)
			{
                response = (json_of_target(reqjstring, agent->cinfo, mindex));
                if (mindex < agent->cinfo->node.target_cnt-1)
					++mindex;
				return 0;
			}
			break;
		}
		break;
	case 'd':
		if (dindex <= commanddict.size())
		{
            agent->cinfo->event[0].s = commanddict[dindex];
            response = (json_of_event(myjstring, agent->cinfo));
			if (dindex < commanddict.size()-1)
				++dindex;
			return 0;
		}
		break;
	}
	response = ("");
	return (AGENT_ERROR_REQUEST);
}

//! Returns the current node.ini
/*! 
*/
int32_t request_getnode(string &request, string &response, Agent *)
{
    response = (json_of_node(reqjstring, agent->cinfo));
	return 0;
}
