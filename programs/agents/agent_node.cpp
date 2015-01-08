//#include <sys/vfs.h>
#include <stdio.h>
#include <dirent.h>

#include "agentlib.h"
#include "jsonlib.h"
#include "jsonlib.h"
#include "physicslib.h"

#include <iostream>

using namespace std;

#define MAXEPHEM 3

char ibuf[AGENTMAXBUFFER];

int myagent();

int ntype = SOCKET_TYPE_UDP;
int waitsec = 5;
jstring reqjstring={0,0,0};
jstring myjstring={0,0,0};

typedef struct
{
	double mjd;
	double utime;
	uint32_t tindex;
	vector<string> telem;
	uint32_t eindex;
	vector<string> event;
} cachestruc;

cachestruc cache[3+MAXEPHEM+1];
int cindex;

uint32_t dindex;

int32_t Mcount;
int32_t mindex;
nodestruc *node;

//eventstruc tevent;
vector<shorteventstruc> eventdict;
vector<shorteventstruc> commanddict;

cosmosstruc *cdata;
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
int32_t request_loadmjd(char *request, char* response, void *cdata);
int32_t request_counts(char *request, char* response, void *cdata);
int32_t request_indexes(char *request, char* response, void *cdata);
int32_t request_days(char *request, char* response, void *cdata);
int32_t request_rewind(char *request, char* response, void *cdata);
int32_t request_next(char *request, char* response, void *cdata);
int32_t request_getmjd(char *request, char* response, void *cdata);
int32_t request_getnode(char *request, char* response, void *cdata);

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
	if (!(cdata = agent_setup_server(ntype,argv[1],(char *)"node",1.,0,AGENTMAXBUFFER)))
		exit (JSON_ERROR_NOJMAP);

	// Set period of broadcasting SOH
	cdata->agent[0].aprd = 1.;

	// temp cosmosstruc for next track requests
	json_clone (cdata) ;

	// Add additional requests
	if ((iretn=agent_add_request(cdata, (char *)"loadmjd",request_loadmjd)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"counts",request_counts)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"indexes",request_indexes)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"days",request_days)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"rewind",request_rewind)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"next",request_next)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"getmjd",request_getmjd)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"getnode",request_getnode)))
		exit (iretn);


	// Check Node directory
	if (get_nodedir(cdata->node.name).empty())
	{
		printf("Couldn't find Node directory %s\n",argv[1]);
		exit (1);
	}

	// Find most recent day in archive
	lastday = findlastday(cdata->node.name);

	// Find oldest day in archive
	firstday = findfirstday(cdata->node.name);

	printf("Found days %f to %f\n",firstday, lastday);

	// Initialize data cache and load most recent day
	for (int i=0; i<3+MAXEPHEM+1; ++i)
	{
		cache[i].mjd = 0.;
		cache[i].telem.resize(0);
		cache[i].event.resize(0);
	}
	if (loadmjd(lastday) == 0. && cdata->node.type == NODE_TYPE_GROUNDSTATION)
	{
		cdata->node.loc.utc = currentmjd(0.);
		cache[3].telem.push_back(json_of_list(&myjstring,(char *)"{\"node_utc\",\"node_loc_pos_geod\",\"node_loc_att_topo\",\"node_powgen\",\"node_powuse\",\"node_battlev\"",cdata));
	}

	printf("Loaded day %f\n",lastday);

	// Load Event and Command dictionaries
	//	loadevents();
	load_dictionary(eventdict, cdata, (char *)"events.dict");
	load_dictionary(commanddict, cdata, (char *)"commands.dict");

	printf("Loaded events\n");

	// Initialize ephemeris for this day
	loadephemeris();

	printf("Loaded Ephemeris\n");

	// Start performing the body of the agent
	json_parse((char *)(cache[3].telem[cache[3].telem.size()-1].c_str()),cdata);
	loc_update(&cdata->node.loc);

	switch (cdata->node.type)
	{
	case NODE_TYPE_SATELLITE:
		// Initialize hardware
		hardware_init_eci(cdata->devspec, cdata->node.loc);
		// Initialize orbit
		gauss_jackson_init_eci(gjh, 8, 0, .1, cdata->node.loc.utc, cdata->node.loc.pos.eci, cdata->node.loc.att.icrf, *cdata);
		cdata->node.utcoffset = cdata->node.loc.utc - currentmjd(0.);
		agent_set_sohstring(cdata, (char *)"{\"node_utc\",\"node_name\",\"node_type\",\"node_loc_pos_eci\",\"node_loc_att_icrf\"}");
		printf("Initialized satellite starting at %.15g [%.8g %.8g %.8g]\n",cdata->node.loc.pos.eci.utc,cdata->node.loc.pos.eci.s.col[0],cdata->node.loc.pos.eci.s.col[1],cdata->node.loc.pos.eci.s.col[2]);
		break;
	case NODE_TYPE_GROUNDSTATION:
	case NODE_TYPE_MOC:
	case NODE_TYPE_UAV:
	case NODE_TYPE_VEHICLE:
	default:
		cdata->node.utcoffset = cdata->node.loc.utc - currentmjd(0.);
		agent_set_sohstring(cdata, (char *)"{\"node_utc\",\"node_name\",\"node_type\",\"node_loc_pos_geod\",\"node_loc_att_topo\"}");
		printf("Initialized ground node starting at %.15g [%.8g %.8g %.8g]\n",cdata->node.loc.pos.geod.utc,cdata->node.loc.pos.geod.s.lon,cdata->node.loc.pos.geod.s.lat,cdata->node.loc.pos.geod.s.h);
		break;
	}

	nextmjd = currentmjd(0.);
	while(agent_running(cdata))
	{
		nextmjd += cdata->agent[0].aprd/86400.;
		if ((newlastday=findlastday(cdata->node.name)) != lastday)
		{

		}
		switch (cdata->node.type)
		{
		case NODE_TYPE_SATELLITE:
			gauss_jackson_propagate(gjh, *cdata, currentmjd(cdata->node.utcoffset));
			break;
		default:
			cdata->node.loc.utc = cdata->node.loc.pos.geod.utc = currentmjd(cdata->node.utcoffset);
			++cdata->node.loc.pos.geod.pass;
			pos_geod(&cdata->node.loc);
			update_target(cdata);
			cdata->node.loc.att.topo.s = q_eye();
			for (uint32_t i=0; i<cdata->node.target_cnt; ++i)
			{
				if (cdata->target[i].elfrom > 0.)
				{
					cdata->node.loc.att.topo.s = q_mult(q_change_around_x(cdata->target[i].elfrom),q_change_around_z(cdata->target[i].azfrom));
				}
			}
			++cdata->node.loc.att.topo.pass;
			att_topo(&cdata->node.loc);
			break;
		}
//		agent_post(cdata, AGENT_MESSAGE_SOH, json_of_list(&myjstring,cdata->agent[0].sohstring,cdata));
		agent_post(cdata, AGENT_MESSAGE_SOH, json_of_table(&myjstring, cdata->agent[0].sohtable, cdata));
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

	sprintf(dtemp,"%s/events.dict",get_nodedir(cdata->node.name));
	if ((ifh=fopen(dtemp,"r")) != NULL)
	{
		while (fgets(ibuf,AGENTMAXBUFFER,ifh) != NULL)
		{
			json_parse(ibuf,cdata);
			eventdict.push_back(cdata->event[0]);
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
	vector<shorteventstruc> events;

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
	json_parse((char *)(cache[3].telem[cache[3].telem.size()-1].c_str()),cdata);
	loc_update(&cdata->node.loc);

	for (j=4; j<MAXEPHEM+4; ++j)
	{
		cache[j].telem.clear();
		cache[j].event.clear();
	}

	// Load Ephemeris with most recent data, calculated from last point of most recent day
	ctime = cdata->node.loc.utc;
	stime = (int)ctime;
	etime = stime + MAXEPHEM + 1;
	gauss_jackson_init_eci(gjh, 8, 1, 10., ctime, cdata->node.loc.pos.eci, cdata->node.loc.att.icrf, *cdata);
	update_target(cdata);
	do
	{
		cache[3+(int)(ctime-stime)].telem.push_back(json_of_list(&myjstring,(char *)"{\"node_utc\",\"node_loc_pos_eci\",\"node_loc_att_icrf\",\"node_powgen\",\"node_powuse\",\"node_battlev\"",cdata));
		calc_events(eventdict, cdata, events);
		for (k=0; k<events.size(); ++k)
		{
			memcpy(&cdata->event[0].s,&events[k],sizeof(shorteventstruc));
			strcpy(cdata->event[0].l.condition,cdata->emap[events[k].handle.hash][events[k].handle.index].text);
			cache[3+(int)(ctime-stime)].event.push_back(json_of_event(&myjstring,cdata));
		}
		cache[3+(int)(ctime-stime)].mjd = (int)ctime;
		cache[3+(int)(ctime-stime)].utime = ctime;
		ctime += 20./86400.;
		gauss_jackson_propagate(gjh, *cdata, ctime);
		update_target(cdata);
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

	if (!data_load_archive(mjd, cache[cindex].telem, cache[cindex].event, cdata))
	{
		cache[cindex].mjd = (int)mjd;
		cache[cindex].utime = currentmjd(cdata->node.utcoffset);
		cache[cindex].tindex = cache[cindex].eindex = 0;
		return (mjd);
	}
	else
		return (0.);
}

//! Load a given day's data
/*! 
*/
int32_t request_loadmjd(char *request, char* response, void *cdata)
{
	double value;

	sscanf(request,"%*s %lf",&value);

	value = loadmjd(value);

	sprintf(response,"%f",cache[cindex].mjd);
	return 0;
}

//! Gives the number of event and telemetry records
/*! 
*/
int32_t request_counts(char *, char* response, void *cdata)
{
	sprintf(response,"%" PRIu32 " %" PRIu32 " %" PRIu32 " %d",cache[cindex].telem.size(),cache[cindex].event.size(),commanddict.size(),((cosmosstruc *)cdata)->node.target_cnt);
	return 0;
}

//! Tells first and last day in archive
/*! 
*/
int32_t request_days(char *request, char* response, void *cdata)
{
	sprintf(response,"%d %d",(int)firstday,(int)lastday);
	return 0;
}

//! Goes to the first record for either events or telemetry for the loaded day
/*! 
*/
int32_t request_rewind(char *request, char* response, void *cdata)
{
	char arg[50];

	sscanf(request,"%*s %s",arg);

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

	response[0] = 0;
	return 0;
}

//! Returns the current record of both event and telemetry data
/*! 
*/
int32_t request_indexes(char *, char* response, void *cdata)
{
	sprintf(response,"%d %d %d %d",cache[cindex].tindex,cache[cindex].eindex,dindex,mindex);
	return 0;
}

//! Returns the day that is loaded
/*! 
*/
int32_t request_getmjd(char *, char* response, void *cdata)
{
	sprintf(response,"%f",cache[cindex].mjd);
	return 0;
}

//! Gets next event, telemetry or data dictionary record entry.
/*! 
*/
int32_t request_next(char *request, char* response, void *cdata)
{
	char arg[50];

	sscanf(request,"%*s %s",arg);

	switch (arg[0])
	{
	case 'e':
		if (cache[cindex].eindex < cache[cindex].event.size())
		{
			strcpy(response,cache[cindex].event[cache[cindex].eindex].c_str());
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
				strcpy(response,cache[cindex].telem[cache[cindex].tindex].c_str());
				if (cache[cindex].tindex < cache[cindex].telem.size()-1)
					cache[cindex].tindex++;
				return 0;
			}
			break;
		case 'r':
			if (mindex >= 0)
			{
				strcpy(response,json_of_target(&reqjstring,(cosmosstruc *)cdata,mindex));
				if (mindex < ((cosmosstruc *)cdata)->node.target_cnt-1)
					++mindex;
				return 0;
			}
			break;
		}
		break;
	case 'd':
		if (dindex <= commanddict.size())
		{
			((cosmosstruc *)cdata)->event[0].s = commanddict[dindex];
			strcpy(response,json_of_event(&myjstring,(cosmosstruc *)cdata));
			if (dindex < commanddict.size()-1)
				++dindex;
			return 0;
		}
		break;
	}
	strcpy(response,"");
	return (AGENT_ERROR_REQUEST);
}

//! Returns the current node.ini
/*! 
*/
int32_t request_getnode(char *request, char* response, void *cdata)
{
	strcpy(response,json_of_node(&reqjstring,(cosmosstruc *)cdata));
	return 0;
}
