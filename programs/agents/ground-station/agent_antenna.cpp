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

int32_t request_debug(char *req, char* response, Agent *);
int32_t request_get_offset(char *req, char* response, Agent *);
int32_t request_get_state(char *req, char* response, Agent *);

int32_t request_set_azel(char *req, char* response, Agent *);
int32_t request_track_azel(char *req, char* response, Agent *);
int32_t request_get_azel(char *req, char* response, Agent *);
int32_t request_jog(char *req, char* response, Agent *);
int32_t request_get_horizon(char *req, char* response, Agent *);
int32_t request_enable(char *req, char* response, Agent *);
int32_t request_disable(char *req, char* response, Agent *);
int32_t request_stop(char *req, char* response, Agent *);
int32_t request_pause(char *req, char* response, Agent *);
int32_t request_set_offset(char *req, char* response, Agent *);

int32_t connect_antenna();

float gsmin = RADOF(10.);
//char tlename[20];
std::string nodename;
std::string agentname;
std::string antdevice;
size_t devindex;
size_t antindex = 9999;
antstruc target;
antstruc current;
bool antconnected = false;
bool antenabled = false;
bool debug;

// Here are internally provided functions
//int json_init();
//int myinit();
//int load_gs_info(char *file);
int load_tle_info(char *file);

// Here are variables for internal use
static std::vector<tlestruc> tle;
int32_t numlines, bestn;
Agent *agent;

struct azelstruc
{
    float az;
    float el;
};
azelstruc antennaoffset = {0., 0.};

LsFit trackaz(5, 2);
LsFit trackel(5, 2);
bool trackflag = false;

int main(int argc, char *argv[])
{
    int iretn;
    //  rvector topo;
    //  locstruc loc;
    //  int32_t n;
    //  float taz, tel;


    // Initialization stuff

    switch (argc)
    {
    case 3:
        nodename = argv[1];
        agentname = argv[2];
        break;
    default:
        printf("Usage: agent_antenna node antenna");
        exit (1);
        break;
    }

    if (!(agent = new Agent(nodename, agentname)))
    {
        printf("Error %d: Setting up Agent antenna\n",JSON_ERROR_NOJMAP);
        exit (JSON_ERROR_NOJMAP);
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

    // Look for named antenna so we can use the right one
    for (size_t i=0; i<agent->cinfo->devspec.ant_cnt; ++i)
    {
        string aname = agent->cinfo->pieces[agent->cinfo->device[agent->cinfo->devspec.ant[i]].all.pidx].name;
        if (aname == argv[2])
            //      if (!strcmp(argv[2], agent->cinfo->pieces[agent->cinfo->devspec.ant[i]->pidx].name))
        {
            devindex = agent->cinfo->devspec.ant[i];
            antindex = i;
            break;
        }
    }

    if (antindex == 9999)
    {
        std::cout<<"Exiting " << agentname << " for Node: " << nodename << " no antenna found." << std::endl;
        agent->shutdown();
        exit (1);
    }

    // Set SOH string
    char sohstring[200];
    sprintf(sohstring, "{\"device_ant_temp_%03lu\",\"device_ant_align_%03lu\",\"device_ant_azim_%03lu\",\"device_ant_elev_%03lu\"}", antindex, antindex, antindex, antindex);
    agent->set_sohstring(sohstring);

    antdevice = agent->cinfo->port[agent->cinfo->device[devindex].all.portidx].name;

    // Connect to antenna and set sensitivity;
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

int32_t request_get_state(char *req, char* response, Agent *)
{
    sprintf(response,"[%.6f] Cx: %u En: %u Target: %6.1f %6.1f (%6.1f %6.1f) Actual: %6.1f %6.1f Offset: %6.1f %6.1f",
            currentmjd(),
            antconnected,
            antenabled,
            DEGOF(current.azim),
            DEGOF(current.elev),
            DEGOF(current.azim-agent->cinfo->device[devindex].ant.azim),
            DEGOF(current.elev-agent->cinfo->device[devindex].ant.elev),
            DEGOF(agent->cinfo->device[devindex].ant.azim+antennaoffset.az),
            DEGOF(agent->cinfo->device[devindex].ant.elev+antennaoffset.el),
            DEGOF(antennaoffset.az), DEGOF(antennaoffset.el));
    return (0);
}

int32_t request_stop(char *req, char* response, Agent *)
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

int32_t request_pause(char *req, char* response, Agent *)
{

    target = agent->cinfo->device[devindex].ant;

    return 0;
}

int32_t request_set_offset(char *req, char* response, Agent *)
{
    float targetaz;
    float targetel;

    sscanf(req,"%*s %f %f",&targetaz, &targetel);
    antennaoffset.az = RADOF(targetaz);
    antennaoffset.el = RADOF(targetel);
    return (0);
}

int32_t request_get_offset(char *req, char* response, Agent *)
{
    float az = antennaoffset.az;
    float el = antennaoffset.el;
    sprintf(response,"%f %f",DEGOF(az), DEGOF(el));
    return (0);
}

int32_t request_set_azel(char *req, char* response, Agent *)
{
    float targetaz;
    float targetel;

    sscanf(req,"%*s %f %f",&targetaz, &targetel);
    target.azim = RADOF(targetaz);
    target.elev = RADOF(targetel);
    trackflag = false;
    return (0);
}

int32_t request_get_azel(char *req, char* response, Agent *)
{
    double az = agent->cinfo->device[devindex].ant.azim;
    double el = agent->cinfo->device[devindex].ant.elev;
    sprintf(response,"%f %f",DEGOF(az), DEGOF(el));
    return (0);
}

int32_t request_jog(char *req, char* response, Agent *)
{
    float az, el;
    sscanf(req,"%*s %f %f", &az, &el);
    target.azim += RADOF(az);
    target.elev += RADOF(el);
    return (0);
}

int32_t request_get_horizon(char *req, char* response, Agent *)
{
    sscanf(req,"%*s %f",&gsmin);
    return (0);
}

int32_t request_enable(char *req, char* response, Agent *)
{
    antenabled = true;
    return 0;
}

int32_t request_disable(char *req, char* response, Agent *)
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
        iretn = prkx2su_connect(antdevice);

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

int32_t request_track_azel(char *req, char* response, Agent *)
{
    float az;
    float el;
    double utc;

    sscanf(req,"%*s %lf %f %f", &utc, &az, &el);
    trackaz.update(utc, RADOF(az));
    trackel.update(utc, RADOF(el));
    trackflag = true;
    return 0;
}
