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

static string nodename = "";
static string agentname = "control";
static string trajectoryname = "";
static Agent *agent;
static mutex cdata_mutex;

static vector <double> lastantutc;
static vector <double> lasttcvutc;

struct radiostruc
{
    string name;
	tcvstruc info;
	uint16_t otherradioindex;
	beatstruc beat;
	float dfreq;
};
static vector <radiostruc> myradios;

static size_t trackindex = 9999;
static size_t highestindex = 9999;
static float highestvalue = -DPI2;
struct trackstruc
{
    uint16_t type;
	targetstruc target;
	physicsstruc physics;
    string name;
	gj_handle gjh;
    vector <LsFit> position;
    vector <radiostruc> radios;
};
static vector <trackstruc> track;

struct antennastruc
{
    string name;
	antstruc info;
	beatstruc beat;
};
static vector <antennastruc> myantennas;

static bool debug;

int32_t request_debug(string &request, string &response, Agent *);
int32_t request_get_state(string &request, string &response, Agent *);
int32_t request_list_tracks(string &request, string &response, Agent *);
int32_t request_set_track(string &request, string &response, Agent *);
int32_t request_get_track(string &request, string &response, Agent *);
int32_t request_list_radios(string &request, string &response, Agent *);
int32_t request_match_radio(string &request, string &response, Agent *);
int32_t request_list_antennas(string &request, string &response, Agent *);
int32_t request_get_highest(string &req, string &response, Agent *);
int32_t request_unmatch_radio(string &request, string &response, Agent *);

void monitor();
string opmode2string(uint8_t opmode);

int main(int argc, char *argv[])
{
	int32_t iretn;

	switch (argc)
	{
    case 3:
        nodename = argv[2];
    case 2:
        trajectoryname = argv[1];
    case 1:
		break;
	default:
        printf("Usage: agent->control {nodename}");
		exit (1);
		break;
	}

	// Establish the command channel and heartbeat
    if (nodename.empty())
    {
        agent = new Agent("", agentname, 5.);
    }
    else
    {
        agent = new Agent(nodename, agentname, 5.);
    }

    if ((iretn = agent->wait()) < 0)
    {
        fprintf(agent->get_debug_fd(), "%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    else
    {
        fprintf(agent->get_debug_fd(), "%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
    }

    // Build up table of our radios
    myradios.resize(agent->cinfo->devspec.tcv_cnt);
	for (size_t i=0; i<myradios.size(); ++i)
	{
        myradios[i].name = agent->cinfo->pieces[agent->cinfo->device[agent->cinfo->devspec.tcv[i]].all.pidx].name;
        myradios[i].info = agent->cinfo->device[agent->cinfo->devspec.tcv[i]].tcv;
		myradios[i].otherradioindex = 9999;
        myradios[i].beat = agent->find_server(nodename, myradios[i].name, 3.);
	}

	// Build up table of our antennas
    myantennas.resize(agent->cinfo->devspec.ant_cnt);
	for (size_t i=0; i<myantennas.size(); ++i)
	{
        myantennas[i].name = agent->cinfo->pieces[agent->cinfo->device[agent->cinfo->devspec.ant[i]].all.pidx].name;
        myantennas[i].info = agent->cinfo->device[agent->cinfo->devspec.ant[i]].ant;
        myantennas[i].beat = agent->find_server(nodename, myantennas[i].name, 3.);
	}

	// Build up table of other nodes. Look through node directory and choose any valid node that
	// is a satellite or a celestial object. First entry is null, none tracking.
	track.resize(1);
	trackindex = 0;
	track[0].name = "idle";
	track[0].target.type = NODE_TYPE_DATA;
    vector <string> nodes;
	iretn = data_list_nodes(nodes);
	for (size_t i=0; i<nodes.size(); ++i)
	{
        string path = data_base_path(nodes[i]) + "/node.ini";
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
                cosmosstruc *cinfo = json_init();
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
                        ttrack.radios[i].name = cinfo->pieces[cinfo->device[cinfo->devspec.tcv[i]].all.pidx].name;
                        ttrack.radios[i].info = cinfo->device[cinfo->devspec.tcv[i]].tcv;
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
    vector <tlestruc> tle;
	if ((iretn=load_lines_multi(fname, tle)) > 0)
	{
		for (size_t i=0; i<tle.size(); ++i)
		{
			// Valid node. Initialize tracking and push it to list
			trackstruc ttrack;
            ttrack.type = 0;
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

    struct gentry
    {
        double second;
        gvector geod;
        rvector geoc;
    };
    vector <gentry> trajectory;

    if (!trajectoryname.empty() && data_isfile(trajectoryname))
    {
        FILE *fp = fopen(trajectoryname.c_str(), "r");
        if (fp != nullptr)
        {
            while (!feof(fp))
            {
                gentry tentry;
                iretn = fscanf(fp, "%lf %lf %lf %lf\n", &tentry.second, &tentry.geod.lat, &tentry.geod.lon, &tentry.geod.h);
                if (iretn == 4)
                {
                    trajectory.push_back(tentry);
                }
            }
            fclose(fp);
            if (trajectory.size() > 2)
            {
                trackstruc ttrack;
                ttrack.type = 0;
                ttrack.name = trajectoryname;
                ttrack.target.type = NODE_TYPE_SATELLITE;
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

                LsFit tfit(3);
                tfit.update(trajectory[0].second, trajectory[0].geod);
                tfit.update(trajectory[1].second, trajectory[1].geod);
                for (uint16_t i=0; i<(trajectory[0].second+trajectory[1].second)/2; ++i)
                {
                    ttrack.position.push_back(tfit);
                }
                for (uint16_t j=1; j<trajectory.size()-1; ++j)
                {
                    tfit.initialize(3);
                    tfit.update(trajectory[j-1].second, trajectory[j-1].geod);
                    tfit.update(trajectory[j].second, trajectory[j].geod);
                    tfit.update(trajectory[j+1].second, trajectory[j+1].geod);
                    for (uint16_t i=static_cast<uint16_t>(.5+(trajectory[j-1].second+trajectory[j].second)/2); i<(trajectory[j].second+trajectory[j+1].second)/2; ++i)
                    {
                        ttrack.position.push_back(tfit);
                    }
                }
                tfit.initialize(3);
                tfit.update(trajectory[trajectory.size()-2].second, trajectory[trajectory.size()-2].geod);
                tfit.update(trajectory[trajectory.size()-1].second, trajectory[trajectory.size()-1].geod);
                for (uint16_t i=static_cast<uint16_t>(.5+(trajectory[trajectory.size()-2].second+trajectory[trajectory.size()-1].second)/2); i<trajectory[trajectory.size()-1].second; ++i)
                {
                    ttrack.position.push_back(tfit);
                }

				for (double timestep=0.; timestep<=trajectory[trajectory.size()-1].second; timestep+=1.)
                {
                    uint16_t timeidx = static_cast<uint16_t>(timestep);
                    gvector tpos = ttrack.position[timeidx].evalgvector(timestep);
                    gvector tvel = ttrack.position[timeidx].slopegvector(timestep);
                    printf("%f %f %f %f %f %f %f\n", timestep, tpos.lat, tpos.lon, tpos.h, tvel.lat, tvel.lon, tvel.h);
                }
            }
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
                string output;
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

int32_t request_list_tracks(string &request, string &response, Agent *)
{
    response.clear();
	for (size_t i=0; i<track.size(); ++i)
	{
        response += '\n' + track[i].name + ' ' + to_angle(DPI2-track[i].target.elfrom, 'D');
	}
    response += '\n';

	return 0;
}

int32_t request_set_track(string &request, string &response, Agent *)
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
		sscanf(request.c_str(), "set_track %lu", &tracki);
		if (tracki < track.size())
		{
			trackindex = tracki;
		}
		break;
	default:
		char trackname[41];
		sscanf(request.c_str(), "set_track %40s", trackname);
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

int32_t request_get_track(string &request, string &response, Agent *)
{
	if (trackindex != 9999)
    {
        response = to_unsigned(trackindex) + ' ' +  track[trackindex].name + ' ' + to_double(track[trackindex].target.range) + ' ' + to_double(track[trackindex].target.range/track[trackindex].target.close);
	}

	return 0;
}

int32_t request_list_radios(string &request, string &response, Agent *)
{
    response = ("My Radios\n");
	for (size_t i=0; i<myradios.size(); ++i)
    {
        response += myradios[i].name + ' ' + to_unsigned(myradios[i].otherradioindex) + ' ' + to_double(myradios[i].info.freq) + ' ' + opmode2string(myradios[i].info.opmode);
	}

    response += ("Other Radios\n");
	for (size_t i=0; i<track[trackindex].radios.size(); ++i)
	{
        response += ( to_unsigned(i) + ' ' + track[trackindex].radios[i].name.c_str() + ' ' + to_unsigned(track[trackindex].radios[i].otherradioindex) + ' ' + to_double(track[trackindex].radios[i].info.freq) + ' ' + opmode2string(track[trackindex].radios[i].info.opmode).c_str());
	}

	return 0;
}

int32_t request_match_radio(string &request, string &response, Agent *)
{
	char fromname[41];
	char toname[41];
	uint16_t fromi = 9999;
	uint16_t toi = 9999;

	sscanf(request.c_str(), "match_radio %40s %40s", fromname, toname);

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
        response = ("Matched " + myradios[fromi].name + " to " + track[trackindex].radios[toi].name);
    }
	else
	{
        response = ("No match");
	}

	return 0;
}

int32_t request_unmatch_radio(string &request, string &response, Agent *)
{
	char fromname[41];
	uint16_t fromi = 9999;
	uint16_t toi = 9999;

	sscanf(request.c_str(), "unmatch_radio %40s", fromname);

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
        response = ("Unatched %s to %s", myradios[fromi].name.c_str(), track[trackindex].radios[toi].name.c_str());
	}
	else
	{
        response = ("No match");
	}

	return 0;
}

int32_t request_list_antennas(string &request, string &response, Agent *)
{
	for (size_t i=0; i<myantennas.size(); ++i)
	{
        response += ("%lu %s %f\n", i, myantennas[i].name.c_str(), myantennas[i].info.minelev);
	}

	return 0;
}

int32_t request_get_state(string &req, string &response, Agent *)
{
	if (trackindex == 0)
	{
        response = ("[%.6f] 0: idle [na na] CxTime: ",
                currentmjd());
	}
	else
	{
        response = ("[%.6f] %lu: %s [%6.1f %6.1f] CxTime: ",
                currentmjd(),
                trackindex,
                track[trackindex].name.c_str(),
                DEGOF(fixangle(track[trackindex].target.azfrom)),
                DEGOF((track[trackindex].target.elfrom)));
	}
	for (size_t i=0; i<myradios.size(); ++i)
	{
        response += ("{%s  %9.0f ", myradios[i].name.c_str(), myradios[i].info.freq);
		if (myradios[i].otherradioindex != 9999)
		{
			size_t idx = myradios[i].otherradioindex;
            response += ("(%s %.3f)} ", track[trackindex].radios[idx].name.c_str(), 86400. * (currentmjd() - myradios[i].beat.utc));
		}
		else
		{
            response += ("} ");
		}
	}
	for (size_t i=0; i<myantennas.size(); ++i)
	{
        response += ("{%s (%.3f)}  ", myantennas[i].name.c_str(), 86400.*(currentmjd()-myantennas[i].beat.utc));
	}
	return (0);
}

int32_t request_get_highest(string &req, string &response, Agent *)
{
	if (highestindex != 9999)
	{
        response = ("%lu: %s [%6.1f %6.1f]", highestindex, track[highestindex].name.c_str(), DEGOF(fixangle(track[highestindex].target.azfrom)), DEGOF((track[highestindex].target.elfrom)));
	}
	return 0;
}

int32_t request_debug(string &req, string &response, Agent *)
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

string opmode2string(uint8_t opmode)
{
    string result;
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
