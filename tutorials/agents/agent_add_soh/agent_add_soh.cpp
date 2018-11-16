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

// Example of an agent making a request to another agent
// agent 001 makes request to 002 upon activation

// the single quote only works on linux
// test: agent telem 001 'setvalue {"device_telem_vint16_000":10}'
// test: agent telem 001 'getvalue {"device_telem_vint16_000"}'

// the single quote only works on windows
// test: agent telem 001 "setvalue {\"device_telem_vint16_000\":10}"
// test: agent telem 001 "getvalue {\"device_telem_vint16_000\"}"

#include "support/configCosmos.h"
#include "support/elapsedtime.h"
#include "support/timeutils.h"
#include "agent/agentclass.h"

#include <iostream>
#include <string>
using namespace std;


int myagent();

string agentname    = "add_soh";
string nodename     = "telem";

int waitsec = 1; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat
char buf4[512];

beatstruc beat_agent_002;
Agent *agent; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffer for agents

int main(int argc, char *argv[])
{
    int32_t iretn;
    cout << "Starting agent " << endl;

    // Establish the command channel and heartbeat
    agent = new Agent(nodename, agentname);
    if (agent->cinfo == nullptr || !agent->running())
    {
        cout << "agent_setup_server failed (error <" << AGENT_ERROR_JSON_CREATE << ">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
        cout<<"Starting " << agentname << " ... OK" << endl;
    }


    // Set SOH String
    char sohstring[2000] = "{\"node_loc_utc\","
                           "\"node_loc_pos_eci\","
                           "\"node_loc_att_icrf\"}"
                           ;
    agent->set_sohstring(sohstring);

    // Start our own thread
	myagent();

    return iretn;
}

int myagent()
{
    cout << "agent " << agentname <<  " ...online " << endl;

    rvector omega;
    omega = {1.,2.,3.};

    cartpos pos_eci;
    pos_eci.s = {4,5,6};
    pos_eci.v = {0.1,0,0};

    // Start executing the agent
    while(agent->running())
    {
        pos_eci.utc = currentmjd(0);
        agent->cinfo->node.loc.pos.eci = pos_eci;
    }

    return (0);

}
