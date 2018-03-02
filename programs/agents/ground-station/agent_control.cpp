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

/*! \file
    \brief Agent Control for Satellite Ground Station

    This is the main agent that controls the ground station. It it the channel for the T&C and it is also responsible for the \ref agent_antenna and \ref agent_radio

*/

/*! \ingroup agents
    \defgroup agent_control Agent Control
    This agent is listed in agent_control.cpp

@code
List of available requests:

        help
                list of available requests for this agent

        shutdown
                request to shutdown this agent

        idle
                request to transition this agent to idle state

        monitor
                request to transition this agent to monitor state

        run
                request to transition this agent to run state

        status
                request the status of this agent

        getvalue {"name1","name2",...}
                get specified value(s) from agent

        setvalue {"name1":value},{"name2":value},...}
                set specified value(s) in agent

        listnames
                list the Namespace of the agent

        forward nbytes packet
                Broadcast JSON packet to the default SEND port on local network

        echo utc crc nbytes bytes
                echo array of nbytes bytes, sent at time utc, with CRC crc.

        nodejson
                return description JSON for Node

        statejson
                return description JSON for State vector

        utcstartjson
                return description JSON for UTC Start time

        piecesjson
                return description JSON for Pieces

        devgenjson
                return description JSON for General Devices

        devspecjson
                return description JSON for Specific Devices

        portsjson
                return description JSON for Ports

        targetsjson
                return description JSON for Targets

        aliasesjson
                return description JSON for Aliases

        get_state
                returns current state

        list_tracks
                returns the list of possible tracks

        set_track
                sets the desired track

        get_track
                returns current setting for desired track

        list_radios
                returns the list of possible radios, mine and others

        match_radio
                matches one of my radios with one of other radios

        unmatch_radio
                matches one of my radios with one of other radios

        list_antennas
                returns the list of my antennas

        get_highest
                returns the highes trackable object

        debug
                TUrn debugging on or off.
@endcode
*/

#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "physics/physicslib.h"
#include "support/jsonlib.h"
#include "support/ephemlib.h"

std::string nodename;
std::string agentname;
Agent *agent;
std::mutex cdata_mutex;

std::vector <double> lastantutc;
std::vector <double> lasttcvutc;

struct radiostruc
{
    std::string name;
	tcvstruc info;
	uint16_t otherradioindex;
	beatstruc beat;
	float dfreq;
};
std::vector <radiostruc> myradios;

size_t trackindex = 9999;
size_t highestindex = 9999;
float highestvalue = -DPI2;
struct trackstruc
{
	targetstruc target;
	physicsstruc physics;
    std::string name;
	gj_handle gjh;
    std::vector <radiostruc> radios;
};
std::vector <trackstruc> track;

struct antennastruc
{
    std::string name;
	antstruc info;
	beatstruc beat;
};
std::vector <antennastruc> myantennas;

bool debug;

int32_t request_debug(char* request, char* response, Agent *);
int32_t request_get_state(char* request, char* response, Agent *);
int32_t request_list_tracks(char* request, char* response, Agent *);
int32_t request_set_track(char* request, char* response, Agent *);
int32_t request_get_track(char* request, char* response, Agent *);
int32_t request_list_radios(char* request, char* response, Agent *);
int32_t request_match_radio(char* request, char* response, Agent *);
int32_t request_list_antennas(char* request, char* response, Agent *);
int32_t request_get_highest(char *req, char* response, Agent *);
int32_t request_unmatch_radio(char* request, char* response, Agent *);

void monitor();
std::string opmode2string(uint8_t opmode);

int main(int argc, char *argv[])
{
	int32_t iretn;

	switch (argc)
	{
	case 2:
		nodename = argv[1];
		agentname = "control";
		break;
	default:
        printf("Usage: agent->control {nodename}");
		exit (1);
		break;
	}

	// Establish the command channel and heartbeat
    if (!(agent = new Agent(nodename, agentname)))
	{
        std::cout << agentname << ": agent->setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<std::endl;
		exit (AGENT_ERROR_JSON_CREATE);
	}
	else
	{
        std::cout<<"Starting " << agentname << " for Node: " << nodename << std::endl;
	}

	// Build up table of our radios
    myradios.resize(agent->cinfo->devspec.tcv_cnt);
	for (size_t i=0; i<myradios.size(); ++i)
	{
        myradios[i].name = agent->cinfo->pieces[agent->cinfo->devspec.tcv[i]->pidx].name;
        myradios[i].info = *agent->cinfo->devspec.tcv[i];
		myradios[i].otherradioindex = 9999;
        myradios[i].beat = agent->find_server(nodename, myradios[i].name, 3.);
	}

	// Build up table of our antennas
    myantennas.resize(agent->cinfo->devspec.ant_cnt);
	for (size_t i=0; i<myantennas.size(); ++i)
	{
        myantennas[i].name = agent->cinfo->pieces[agent->cinfo->devspec.ant[i]->pidx].name;
        myantennas[i].info = *agent->cinfo->devspec.ant[i];
        myantennas[i].beat = agent->find_server(nodename, myantennas[i].name, 3.);
	}

	// Build up table of other nodes. Look through node directory and choose any valid node that
	// is a satellite or a celestial object. First entry is null, none tracking.
	track.resize(1);
	trackindex = 0;
	track[0].name = "idle";
	track[0].target.type = NODE_TYPE_DATA;
    std::vector <std::string> nodes;
	iretn = data_list_nodes(nodes);
	for (size_t i=0; i<nodes.size(); ++i)
	{
        std::string path = data_base_path(nodes[i]) + "/node.ini";
		FILE *fp = fopen(path.c_str(), "r");
		if (fp != nullptr)
		{
			int32_t type;
			fscanf(fp, "{\"node_type\":%d", &type);
			fclose(fp);
			switch (type)
			{
			case NODE_TYPE_SATELLITE:
			case NODE_TYPE_SUN:
				trackstruc ttrack;
				ttrack.name = nodes[i];
                cosmosstruc *cinfo = json_create();
                iretn = json_setup_node(ttrack.name, cinfo);
                if (iretn == 0 && (currentmjd()-cinfo->node.loc.pos.eci.utc) < 10.)
				{
					// Valid node. Initialize tracking and push it to list
                    ttrack.target.type = cinfo->node.type;
                    ttrack.target.loc = cinfo->node.loc;
                    ttrack.physics = cinfo->physics;

					// Build up table of radios
                    ttrack.radios.resize(cinfo->devspec.tcv_cnt);
					for (size_t i=0; i<ttrack.radios.size(); ++i)
					{
                        ttrack.radios[i].name = cinfo->pieces[cinfo->devspec.tcv[i]->pidx].name;
                        ttrack.radios[i].info = *cinfo->devspec.tcv[i];
						ttrack.radios[i].otherradioindex = 9999;
					}

					if (type == NODE_TYPE_SATELLITE)
					{
                        printf("Propagating Node %s forward %f seconds\n", ttrack.name.c_str(), 86400.*(currentmjd()-ttrack.target.loc.pos.eci.utc));
						gauss_jackson_init_eci(ttrack.gjh, 12, 0, 1., ttrack.target.loc.pos.eci.utc, ttrack.target.loc.pos.eci, ttrack.target.loc.att.icrf, ttrack.physics, ttrack.target.loc);
						gauss_jackson_propagate(ttrack.gjh, ttrack.physics, ttrack.target.loc, currentmjd());
					}
					track.push_back(ttrack);
                    json_destroy(cinfo);
				}
			}
		}
	}

	// Look for TLE file
	char fname[200];
	sprintf(fname,"%s/tle.ini",get_nodedir(nodename).c_str());
    std::vector <tlestruc> tle;
	if ((iretn=load_lines_multi(fname, tle)) > 0)
	{
		for (size_t i=0; i<tle.size(); ++i)
		{
			// Valid node. Initialize tracking and push it to list
			trackstruc ttrack;
			ttrack.name = tle[i].name;
			ttrack.target.type = NODE_TYPE_SATELLITE;
			tle2eci(currentmjd()-10./86400., tle[i], ttrack.target.loc.pos.eci);
			ttrack.target.loc.att.icrf.s = q_eye();
			ttrack.target.loc.att.icrf.v = rv_zero();
			ttrack.target.loc.att.icrf.a = rv_zero();
			ttrack.physics.area = .01;
			ttrack.physics.mass = 1.;

			// Build up table of radios
			ttrack.radios.resize(1);
			ttrack.radios[0].name = "radio";
			//					ttrack.radios[0].info;
			ttrack.radios[0].otherradioindex = 9999;

			gauss_jackson_init_eci(ttrack.gjh, 6, 0, 1., ttrack.target.loc.pos.eci.utc, ttrack.target.loc.pos.eci, ttrack.target.loc.att.icrf, ttrack.physics, ttrack.target.loc);
			gauss_jackson_propagate(ttrack.gjh, ttrack.physics, ttrack.target.loc, currentmjd());
			track.push_back(ttrack);
		}
	}

	// Add requests
    if ((iretn=agent->add_request("get_state",request_get_state,"", "returns current state")))
		exit (iretn);
    if ((iretn=agent->add_request("list_tracks",request_list_tracks,"", "returns the list of possible tracks")))
		exit (iretn);
    if ((iretn=agent->add_request("set_track",request_set_track,"", "sets the desired track")))
		exit (iretn);
    if ((iretn=agent->add_request("get_track",request_get_track,"", "returns current setting for desired track")))
		exit (iretn);
    if ((iretn=agent->add_request("list_radios",request_list_radios,"", "returns the list of possible radios, mine and others")))
		exit (iretn);
    if ((iretn=agent->add_request("match_radio",request_match_radio,"", "matches one of my radios with one of other radios")))
		exit (iretn);
    if ((iretn=agent->add_request("unmatch_radio",request_unmatch_radio,"", "matches one of my radios with one of other radios")))
		exit (iretn);
    if ((iretn=agent->add_request("list_antennas",request_list_antennas,"", "returns the list of my antennas")))
		exit (iretn);
    if ((iretn=agent->add_request("get_highest",request_get_highest,"", "returns the highes trackable object")))
		exit (iretn);
    if ((iretn=agent->add_request("debug",request_debug,"", "TUrn debugging on or off.")))
		exit (iretn);

	// Start monitoring thread
	thread monitor_thread(monitor);
	ElapsedTime et;

	// Start main thread
    while (agent->running())
	{
		double mjdnow = currentmjd() + .1/86400.;

		// Update all node positions, track if enabled
		highestindex = 9999;
		highestvalue = -DPI2;
		for (size_t i=0; i<track.size(); ++i)
		{
			switch (track[i].target.type)
			{
			case NODE_TYPE_SATELLITE:
				gauss_jackson_propagate(track[i].gjh, track[i].physics, track[i].target.loc, mjdnow);
				break;
			case NODE_TYPE_SUN:
				jplpos(JPL_EARTH, JPL_SUN, mjdnow, &track[i].target.loc.pos.eci);
				track[i].target.loc.pos.eci.pass++;
				pos_eci(&track[i].target.loc);
				break;
			}

//			rvector topo, dv, ds;
//			geoc2topo(track[i].target.loc.pos.geod.s, agent->cinfo->node.loc.pos.geoc.s, topo);
//			topo2azel(topo, &track[i].target.azto, &track[i].target.elto);
//			geoc2topo(agent->cinfo->node.loc.pos.geod.s, track[i].target.loc.pos.geoc.s, topo);
//			topo2azel(topo, &track[i].target.azfrom,&track[i].target.elfrom);
//			ds = rv_sub(track[i].target.loc.pos.geoc.s, agent->cinfo->node.loc.pos.geoc.s);
//			track[i].target.range = length_rv(ds);
//			dv = rv_sub(track[i].target.loc.pos.geoc.v, agent->cinfo->node.loc.pos.geoc.v);
//			track[i].target.close = length_rv(rv_sub(ds,dv)) - length_rv(ds);
            update_target(agent->cinfo->node.loc, track[i].target);

			if (track[i].target.type == NODE_TYPE_SATELLITE)
			{
				if (highestindex == 9999)
				{
					highestindex = i;
					highestvalue = track[i].target.elfrom;
				}
				else
				{
					if (track[i].target.elfrom > highestvalue)
					{
						highestindex = i;
						highestvalue = track[i].target.elfrom;
					}
				}
			}

			if (trackindex && i == trackindex)
			{
                std::string output;
				char request[100];
				sprintf(request, "track_azel %f %f %f", mjdnow, DEGOF(fixangle(track[i].target.azfrom)), DEGOF((track[i].target.elfrom)));
				if (debug)
				{
					printf("%f: Request: %s\n", et.lap(), request);
				}

				// Command antennas to track
				for (size_t j=0; j<myantennas.size(); ++j)
				{
					if (mjdnow - myantennas[j].beat.utc < 10.)
					{
                        iretn = agent->send_request(myantennas[j].beat, request, output, 5.);
					}
				}

				// Calculate frequencies for radios
				for (size_t j=0; j<myradios.size(); ++j)
				{
					if (myradios[j].otherradioindex < track[i].radios.size())
					{
						size_t idx = myradios[j].otherradioindex;
						if (mjdnow - myradios[j].beat.utc < 10.)
						{
							track[i].radios[idx].dfreq = track[i].radios[idx].info.freq * track[i].target.close / CLIGHT;
							myradios[j].info.freq = track[i].radios[idx].info.freq + track[i].radios[idx].dfreq;
							sprintf(request, "set_frequency %f", track[i].radios[idx].info.freq + track[i].radios[idx].dfreq);
                            iretn = agent->send_request(myradios[j].beat, request, output, 5.);
							sprintf(request, "set_opmode %u", track[i].radios[idx].info.opmode);
                            iretn = agent->send_request(myradios[j].beat, request, output, 5.);
						}
					}
				}
			}
		}
		COSMOS_SLEEP(.1);
	}

	monitor_thread.join();
    agent->shutdown();
}


void monitor()
{
    Agent::AgentMessage iretn;

    while (agent->running())
	{
        Agent::messstruc mess;

        iretn = (Agent::AgentMessage)agent->readring(mess, Agent::AgentMessage::BEAT, 5.0);

		// Only process if this is a heartbeat message for our node
        if (iretn == Agent::AgentMessage::BEAT && !strcmp(mess.meta.beat.node, agent->cinfo->node.name))
		{
			cdata_mutex.lock();
			// Extract telemetry
            json_parse(mess.adata, agent->cinfo);

			// Extract agent information
			for (size_t i=0; i<myantennas.size(); ++i)
			{
                if (!strcmp(mess.meta.beat.proc, myantennas[i].name.c_str()))
				{
                    myantennas[i].beat = mess.meta.beat;
				}
			}
			for (size_t i=0; i<myradios.size(); ++i)
			{
                if (!strcmp(mess.meta.beat.proc, myradios[i].name.c_str()))
				{
                    myradios[i].beat = mess.meta.beat;
				}
			}
			cdata_mutex.unlock();

		}
	}

}

int32_t request_list_tracks(char* request, char* response, Agent *)
{
	for (size_t i=0; i<track.size(); ++i)
	{
		sprintf(&response[strlen(response)], "\n%lu %s %f", i, track[i].name.c_str(), DEGOF(DPI2-track[i].target.elfrom));
	}
	sprintf(&response[strlen(response)], "\n");

	return 0;
}

int32_t request_set_track(char* request, char* response, Agent *)
{
	size_t tracki;

	switch (request[10])
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		sscanf(request, "set_track %lu", &tracki);
		if (tracki < track.size())
		{
			trackindex = tracki;
		}
		break;
	default:
		char trackname[41];
		sscanf(request, "set_track %40s", trackname);
		for (tracki=0; tracki<track.size(); ++tracki)
		{
			if (!strcmp(track[tracki].name.c_str(), trackname))
			{
				trackindex = tracki;
				break;
			}
		}
		break;
	}

	for (size_t i=0; i<myradios.size(); ++i)
	{
		myradios[i].otherradioindex = 9999;
	}

	for (size_t i=0; i<track[trackindex].radios.size(); ++i)
	{
		track[trackindex].radios[i].otherradioindex = 9999;
	}

	return 0;
}

int32_t request_get_track(char* request, char* response, Agent *)
{
	if (trackindex != 9999)
	{
		sprintf(response, "\n%lu %s %f %f", trackindex, track[trackindex].name.c_str(), track[trackindex].target.range, track[trackindex].target.range/track[trackindex].target.close);
	}

	return 0;
}

int32_t request_list_radios(char* request, char* response, Agent *)
{
	sprintf(response, "My Radios\n");
	for (size_t i=0; i<myradios.size(); ++i)
	{
		sprintf(&response[strlen(response)], "%lu %s %u %f %s\n", i, myradios[i].name.c_str(), myradios[i].otherradioindex, myradios[i].info.freq, opmode2string(myradios[i].info.opmode).c_str());
	}

	sprintf(&response[strlen(response)], "Other Radios\n");
	for (size_t i=0; i<track[trackindex].radios.size(); ++i)
	{
		sprintf(&response[strlen(response)], "%lu %s %u %f %s\n", i, track[trackindex].radios[i].name.c_str(), track[trackindex].radios[i].otherradioindex, track[trackindex].radios[i].info.freq, opmode2string(track[trackindex].radios[i].info.opmode).c_str());
	}

	return 0;
}

int32_t request_match_radio(char* request, char* response, Agent *)
{
	char fromname[41];
	char toname[41];
	uint16_t fromi = 9999;
	uint16_t toi = 9999;

	sscanf(request, "match_radio %40s %40s", fromname, toname);

	if (fromname[0] < '0' && fromname[0] > '9')
	{
		for (fromi=0; fromi<myradios.size(); ++fromi)
		{
			if (!strcmp(myradios[fromi].name.c_str(), fromname))
			{
				break;
			}
		}
	}
	else
	{
		fromi = atoi(fromname);
	}

	if (toname[0] < '0' && toname[0] > '9')
	{
		for (toi=0; toi<myradios.size(); ++toi)
		{
			if (!strcmp(myradios[toi].name.c_str(), toname))
			{
				break;
			}
		}
	}
	else
	{
		toi = atoi(toname);
	}

	if (fromi < myradios.size() && toi < track[trackindex].radios.size())
	{
		myradios[fromi].otherradioindex = toi;
		track[trackindex].radios[toi].otherradioindex = fromi;
		sprintf(response, "Matched %s to %s", myradios[fromi].name.c_str(), track[trackindex].radios[toi].name.c_str());
	}
	else
	{
		sprintf(response, "No match");
	}

	return 0;
}

int32_t request_unmatch_radio(char* request, char* response, Agent *)
{
	char fromname[41];
	uint16_t fromi = 9999;
	uint16_t toi = 9999;

	sscanf(request, "unmatch_radio %40s", fromname);

	if (fromname[0] < '0' && fromname[0] > '9')
	{
		for (fromi=0; fromi<myradios.size(); ++fromi)
		{
			if (!strcmp(myradios[fromi].name.c_str(), fromname))
			{
				break;
			}
		}
	}
	else
	{
		fromi = atoi(fromname);
	}


	if (fromi < myradios.size())
	{
		toi = myradios[fromi].otherradioindex;
		myradios[fromi].otherradioindex = 9999;
		if (toi < track[trackindex].radios.size())
		{
			track[trackindex].radios[toi].otherradioindex = 9999;
		}
		sprintf(response, "Unatched %s to %s", myradios[fromi].name.c_str(), track[trackindex].radios[toi].name.c_str());
	}
	else
	{
		sprintf(response, "No match");
	}

	return 0;
}

int32_t request_list_antennas(char* request, char* response, Agent *)
{
	for (size_t i=0; i<myantennas.size(); ++i)
	{
		sprintf(&response[strlen(response)], "%lu %s %f\n", i, myantennas[i].name.c_str(), myantennas[i].info.minelev);
	}

	return 0;
}

int32_t request_get_state(char *req, char* response, Agent *)
{
	if (trackindex == 0)
	{
        sprintf(response, "[%.6f] 0: idle [na na] CxTime: ",
                currentmjd());
	}
	else
	{
        sprintf(response, "[%.6f] %lu: %s [%6.1f %6.1f] CxTime: ",
                currentmjd(),
                trackindex,
                track[trackindex].name.c_str(),
                DEGOF(fixangle(track[trackindex].target.azfrom)),
                DEGOF((track[trackindex].target.elfrom)));
	}
	for (size_t i=0; i<myradios.size(); ++i)
	{
		sprintf(&response[strlen(response)], "{%s  %9.0f ", myradios[i].name.c_str(), myradios[i].info.freq);
		if (myradios[i].otherradioindex != 9999)
		{
			size_t idx = myradios[i].otherradioindex;
			sprintf(&response[strlen(response)], "(%s %.3f)} ", track[trackindex].radios[idx].name.c_str(), 86400. * (currentmjd() - myradios[i].beat.utc));
		}
		else
		{
			sprintf(&response[strlen(response)], "} ");
		}
	}
	for (size_t i=0; i<myantennas.size(); ++i)
	{
		sprintf(&response[strlen(response)], "{%s (%.3f)}  ", myantennas[i].name.c_str(), 86400.*(currentmjd()-myantennas[i].beat.utc));
	}
	return (0);
}

int32_t request_get_highest(char *req, char* response, Agent *)
{
	if (highestindex != 9999)
	{
        sprintf(response, "%lu: %s [%6.1f %6.1f]", highestindex, track[highestindex].name.c_str(), DEGOF(fixangle(track[highestindex].target.azfrom)), DEGOF((track[highestindex].target.elfrom)));
	}
	return 0;
}

int32_t request_debug(char *req, char* response, Agent *)
{
    if (debug)
	{
		debug = false;
}
    else
    {
        debug = true;
	}

	return 0;
}

std::string opmode2string(uint8_t opmode)
{
    std::string result;
	switch (opmode)
	{
	case DEVICE_RADIO_MODE_AM:
		result = "AM";
		break;
	case DEVICE_RADIO_MODE_AMD:
		result = "AM Data";
		break;
	case DEVICE_RADIO_MODE_FM:
		result = "FM";
		break;
	case DEVICE_RADIO_MODE_FMD:
		result = "FM Data";
		break;
	case DEVICE_RADIO_MODE_LSB:
		result = "LSB";
		break;
	case DEVICE_RADIO_MODE_LSBD:
		result = "LSB Data";
		break;
	case DEVICE_RADIO_MODE_USB:
		result = "USB";
		break;
	case DEVICE_RADIO_MODE_USBD:
		result = "USB Data";
		break;
	case DEVICE_RADIO_MODE_RTTY:
		result = "RTTY";
		break;
	case DEVICE_RADIO_MODE_RTTYR:
		result = "RTTYR";
		break;
	case DEVICE_RADIO_MODE_DV:
		result = "DV";
		break;
	case DEVICE_RADIO_MODE_DVD:
		result = "DV Data";
		break;
	case DEVICE_RADIO_MODE_CWR:
		result = "CWR";
		break;
	case DEVICE_RADIO_MODE_CW:
		result = "CW";
		break;
	}
	return result;
}
