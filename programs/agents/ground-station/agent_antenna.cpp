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
    \brief Agent Antenna for Satellite Ground Station

    This agent controls the ground station antenna (set elevatiom, azimuth, etc.).
    To read more about how this agent operates go to \ref agent_antenna
*/

/*! \ingroup agents
 \defgroup agent_antenna Agent Antenna
 This agent is listed in agent_antenna.cpp

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
        track_azel track_azel
                Supply next azimuth and elevation for tracking.
        debug debug
                Toggle debug messages.
        get_offset get_offset
                Return a report on the offset of the agent.
        get_state get_state
                Return a report on the state of the agent.
        jog jog {degrees aziumth} {degrees elevation}
                Jog the antenna azimuth and elevation in degrees.
        get_horizon get_horizon
                Return the antennas minimu elevation in degrees.
        get_azel get_azel
                Return the antenna azimuth and elevation in degrees.
        set_azel set_azel aaa.a eee.e
                Set the antenna azimuth and elevation in degrees.
        enable enable
                Enable antenna motion.
        disable disable
                Disable antenna motion.
        pause pause
                Stop where you are and make it your new target.
        stop stop
                Stop where you are, make it your new target AND disable antenna motion.
        set_offset set_offset aaa.a eee.e
                Set the antenna azimuth and elevation correction in degrees.
 @endcode

*/



#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include <sys/stat.h>
#include <fcntl.h>
//#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
//#include <mqueue.h>
#include <semaphore.h>
//#include <sys/vfs.h>
#include "support/jsonlib.h"
#include "support/stringlib.h"
#include "support/timelib.h"
#include "device/general/gs232b_lib.h"
#include "device/general/prkx2su_lib.h"
#include "support/convertlib.h"
#include "support/elapsedtime.h"

int32_t request_debug(string &req, string &response, Agent *);
int32_t request_get_offset(string &req, string &response, Agent *);
int32_t request_get_state(string &req, string &response, Agent *);

int32_t request_set_azel(string &req, string &response, Agent *);
int32_t request_track_azel(string &req, string &response, Agent *);
int32_t request_get_azel(string &req, string &response, Agent *);
int32_t request_jog(string &req, string &response, Agent *);
int32_t request_get_horizon(string &req, string &response, Agent *);
int32_t request_enable(string &req, string &response, Agent *);
int32_t request_disable(string &req, string &response, Agent *);
int32_t request_stop(string &req, string &response, Agent *);
int32_t request_pause(string &req, string &response, Agent *);
int32_t request_set_offset(string &req, string &response, Agent *);

int32_t connect_antenna();

static float gsmin = RADOF(10.);
//char tlename[20];
static string antbase = "";
static std::string nodename = "";
static std::string agentname;
static std::string antdevice;
static uint16_t devindex = -1;
static uint16_t antindex = -1;
static antstruc target;
static antstruc current;
static bool antconnected = false;
static bool antenabled = false;
static bool debug;

// Here are internally provided functions
//int json_init();
//int myinit();
//int load_gs_info(char *file);
int load_tle_info(char *file);

// Here are variables for internal use
static std::vector<tlestruc> tle;
static Agent *agent;

struct azelstruc
{
    float az;
    float el;
};
static azelstruc antennaoffset = {0., 0.};

static LsFit trackaz(5, 2);
static LsFit trackel(5, 2);
static bool trackflag = false;

int main(int argc, char *argv[])
{
    int iretn;

    // Initialization stuff

    switch (argc)
    {
    case 3:
        nodename = argv[2];
    case 2:
        antbase = argv[1];
        break;
    default:
        printf("Usage: agent_antenna antenna node");
        exit (1);
    }

    if (nodename.empty())
    {
        agent = new Agent("", (antbase+"antenna").c_str(), 5.);
    }
    else
    {
        agent = new Agent(nodename, (antbase+"antenna").c_str(), 5.);
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

    iretn = json_createpiece(agent->cinfo, antbase.c_str(), DeviceType::ANT);
    if (iretn < 0)
    {
        fprintf(agent->get_debug_fd(), "Failed to add %s ANT %s\n", antbase.c_str(), cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }
    devindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
    antindex = agent->cinfo->device[devindex].ant.didx;
    agent->cinfo->device[devindex].ant.minelev = RADOF(10.);
    if (antbase == "sband")
    {
        agent->cinfo->device[devindex].ant.model = DEVICE_MODEL_PRKX2SU;
    }
    else if (antbase == "yagi")
    {
        agent->cinfo->device[devindex].ant.model = DEVICE_MODEL_GS232B;
    }
    else
    {
        agent->cinfo->device[devindex].ant.model = DEVICE_MODEL_LOOPBACK;
    }

    iretn = json_dump_node(agent->cinfo);
    if (iretn < 0)
    {
        fprintf(agent->get_debug_fd(), "Failed to save node %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }

    // Add requests
    if ((iretn=agent->add_request("track_azel",request_track_azel,"track_azel", "Supply next azimuth and elevation for tracking.")))
        exit (iretn);
    if ((iretn=agent->add_request("debug",request_debug,"debug", "Toggle debug messages.")))
        exit (iretn);
    if ((iretn=agent->add_request("get_offset",request_get_offset,"get_offset", "Return a report on the offset of the agent.")))
        exit (iretn);
    if ((iretn=agent->add_request("get_state",request_get_state,"get_state", "Return a report on the state of the agent.")))
        exit (iretn);
    if ((iretn=agent->add_request("jog",request_jog,"jog {degrees aziumth} {degrees elevation}", "Jog the antenna azimuth and elevation in degrees.")))
        exit (iretn);
    if ((iretn=agent->add_request("get_horizon",request_get_horizon,"get_horizon", "Return the antennas minimu elevation in degrees.")))
        exit (iretn);
    if ((iretn=agent->add_request("get_azel",request_get_azel,"get_azel", "Return the antenna azimuth and elevation in degrees.")))
        exit (iretn);
    if ((iretn=agent->add_request("set_azel",request_set_azel,"set_azel aaa.a eee.e", "Set the antenna azimuth and elevation in degrees.")))
        exit (iretn);
    if ((iretn=agent->add_request("enable",request_enable,"enable", "Enable antenna motion.")))
        exit (iretn);
    if ((iretn=agent->add_request("disable",request_disable,"disable", "Disable antenna motion.")))
        exit (iretn);
    if ((iretn=agent->add_request("pause",request_pause,"pause", "Stop where you are and make it your new target.")))
        exit (iretn);
    if ((iretn=agent->add_request("stop",request_stop,"stop", "Stop where you are, make it your new target AND disable antenna motion.")))
        exit (iretn);
    if ((iretn=agent->add_request("set_offset",request_set_offset,"set_offset aaa.a eee.e", "Set the antenna azimuth and elevation correction in degrees.")))
        exit (iretn);

    // Set SOH string
    char sohstring[200];
    sprintf(sohstring, "{\"device_ant_temp_%03lu\",\"device_ant_align_%03lu\",\"device_ant_azim_%03lu\",\"device_ant_elev_%03lu\"}", antindex, antindex, antindex, antindex);
    agent->set_sohstring(sohstring);

//    antdevice = agent->cinfo->port[agent->cinfo->device[devindex].all.portidx].name;
    antdevice = "/dev/tty_" + antbase;

    // Connect to antenna and set sensitivity;
    if (agent->cinfo->device[devindex].all.model == DEVICE_MODEL_PRKX2SU)
    {
        iretn = prkx2su_init(antdevice);
    }

    iretn = connect_antenna();
    switch (agent->cinfo->device[devindex].all.model)
    {
    case DEVICE_MODEL_GS232B:
        gs232b_set_sensitivity(RADOF(1.));
        break;
    case DEVICE_MODEL_PRKX2SU:
        prkx2su_set_sensitivity(RADOF(.2));
        break;
    }

    ElapsedTime et;

    // Start performing the body of the agent
    while(agent->running())
    {
        if (antconnected)
        {
            // Find most recent position
            switch (agent->cinfo->device[devindex].all.model)
            {
            case DEVICE_MODEL_LOOPBACK:
                iretn = 0;
                break;
            case DEVICE_MODEL_GS232B:
                iretn = gs232b_get_az_el(current.azim, current.elev);
                break;
            case DEVICE_MODEL_PRKX2SU:
                iretn = prkx2su_get_az_el(current.azim, current.elev);
                break;
            }
            if (iretn < 0)
            {
                antconnected = false;
            }
            else
            {
                agent->cinfo->device[devindex].ant.azim = current.azim -  antennaoffset.az;
                agent->cinfo->device[devindex].ant.elev = current.elev -  antennaoffset.el;
                if (antenabled)
                {
                    if (trackflag)
                    {
                        double utc = currentmjd();
                        if (utc - trackaz.lastx() < trackaz.lastx() - trackaz.firstx())
                        {
                            current.azim = trackaz.eval(utc);
                            current.elev = trackel.eval(utc);
                        }
                        else
                        {
                            trackflag = false;
                        }
                    }
                    else
                    {
                        current.azim = target.azim;
                        current.elev = target.elev;
                    }
                    switch (agent->cinfo->device[devindex].all.model)
                    {
                    case DEVICE_MODEL_GS232B:
                        iretn = gs232b_goto(current.azim + antennaoffset.az, current.elev + antennaoffset.el);
                        break;
                    case DEVICE_MODEL_PRKX2SU:
                        iretn = prkx2su_goto(current.azim + antennaoffset.az, current.elev + antennaoffset.el);
                        break;
                    }
                    if (debug)
                    {
                        printf("%f: goto %f %f [%d]\n", et.lap(), DEGOF(current.azim + antennaoffset.az), DEGOF(current.elev + antennaoffset.el), iretn);
                    }
                    if (iretn < 0)
                    {
                        antconnected = false;
                    }
                }
            }
            COSMOS_SLEEP(.1);
        }
        else
        {
            if (debug)
            {
                printf("%f: Connect Antenna\n", et.lap());
            }
            connect_antenna();
            COSMOS_SLEEP(.1);
        }
    }

    agent->shutdown();
}

int32_t request_get_state(string &req, string &response, Agent *)
{
            response = "[";
            response += ' ' + to_mjd(currentmjd());
            response += ' ' + to_bool(antconnected);
            response += ' ' + to_bool(antenabled);
            response += ' ' + to_bool(DEGOF(current.azim));
            response += ' ' + to_angle((current.elev));
            response += ' ' + to_angle((current.azim-agent->cinfo->device[devindex].ant.azim), 'D');
            response += ' ' + to_angle((current.elev-agent->cinfo->device[devindex].ant.elev), 'D');
            response += ' ' + to_angle((agent->cinfo->device[devindex].ant.azim+antennaoffset.az), 'D');
            response += ' ' + to_angle((agent->cinfo->device[devindex].ant.elev+antennaoffset.el), 'D');
            response += ' ' + to_angle((antennaoffset.az), DEGOF(antennaoffset.el), 'D');
    return (0);
}

int32_t request_stop(string &req, string &response, Agent *)
{

    target = agent->cinfo->device[devindex].ant;
    antenabled = false;
    switch (agent->cinfo->device[devindex].all.model)
    {
    case DEVICE_MODEL_GS232B:
        gs232b_stop();
        break;
    case DEVICE_MODEL_PRKX2SU:
        prkx2su_stop(PRKX2SU_AXIS_AZ);
        prkx2su_stop(PRKX2SU_AXIS_EL);
        break;
    }

    return 0;
}

int32_t request_pause(string &req, string &response, Agent *)
{

    target = agent->cinfo->device[devindex].ant;

    return 0;
}

int32_t request_set_offset(string &req, string &response, Agent *)
{
    float targetaz;
    float targetel;

    sscanf(req.c_str() ,"%*s %f %f",&targetaz, &targetel);
    antennaoffset.az = RADOF(targetaz);
    antennaoffset.el = RADOF(targetel);
    return (0);
}

int32_t request_get_offset(string &req, string &response, Agent *)
{
    float az = antennaoffset.az;
    float el = antennaoffset.el;
    response = to_angle(az) + ' ' + to_angle(el);
    return (0);
}

int32_t request_set_azel(string &req, string &response, Agent *)
{
    float targetaz;
    float targetel;

    sscanf(req.c_str() ,"%*s %f %f",&targetaz, &targetel);
    target.azim = RADOF(targetaz);
    target.elev = RADOF(targetel);
    trackflag = false;
    return (0);
}

int32_t request_get_azel(string &req, string &response, Agent *)
{
    double az = agent->cinfo->device[devindex].ant.azim;
    double el = agent->cinfo->device[devindex].ant.elev;
    response = to_angle(az) + ' ' + to_angle(el);
    return (0);
}

int32_t request_jog(string &req, string &response, Agent *)
{
    float az, el;
    sscanf(req.c_str() ,"%*s %f %f", &az, &el);
    target.azim += RADOF(az);
    target.elev += RADOF(el);
    return (0);
}

int32_t request_get_horizon(string &req, string &response, Agent *)
{
    sscanf(req.c_str() ,"%*s %f",&gsmin);
    return (0);
}

int32_t request_enable(string &req, string &response, Agent *)
{
    antenabled = true;
    return 0;
}

int32_t request_disable(string &req, string &response, Agent *)
{
    antenabled = false;
    return 0;
}

int32_t connect_antenna()
{
    int32_t iretn;
    antconnected = false;

    switch (agent->cinfo->device[devindex].all.model)
    {
    case DEVICE_MODEL_LOOPBACK:
        antconnected = true;
        break;
    case DEVICE_MODEL_GS232B:
        iretn = gs232b_connect(antdevice);

        // Initialize values if we are connected
        if (iretn == 0)
        {
            iretn = gs232b_get_az_el(agent->cinfo->device[devindex].ant.azim, agent->cinfo->device[devindex].ant.elev);
            if (iretn >= 0)
            {
                target.azim = agent->cinfo->device[devindex].ant.azim - antennaoffset.az;
                target.elev = agent->cinfo->device[devindex].ant.elev - antennaoffset.el;
                antconnected = true;
            }
        }
        break;
    case DEVICE_MODEL_PRKX2SU:
        iretn = prkx2su_connect();

        // Initialize values if we are connected
        if (iretn == 0)
        {
            iretn = prkx2su_get_az_el(agent->cinfo->device[devindex].ant.azim, agent->cinfo->device[devindex].ant.elev);
            if (iretn >= 0)
            {
                target.azim = agent->cinfo->device[devindex].ant.azim - antennaoffset.az;
                target.elev = agent->cinfo->device[devindex].ant.elev - antennaoffset.el;
                antconnected = true;
            }
        }
        break;
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

int32_t request_track_azel(string &req, string &response, Agent *)
{
    float az;
    float el;
    double utc;

    sscanf(req.c_str() ,"%*s %lf %f %f", &utc, &az, &el);
    trackaz.update(utc, RADOF(az));
    trackel.update(utc, RADOF(el));
    trackflag = true;
    return 0;
}
