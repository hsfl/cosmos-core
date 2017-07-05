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
//#include "agent/agentclasslib.h"
#include "agent/agentclass.h"

#include <iostream>
#include <string>
Agent *agent;

int main(int, char **)
{

    string agentname = "008";
    string nodename  = "cubesat1";
    string agent004  = "004"; //name of the agent that the request is directed to
    agent = new Agent(nodename, agentname);

    if (agent->last_error()<0)
    {
        cout<<"unable to start agent_exec (" << agent->last_error() << ") " << cosmos_error_string(agent->last_error()) <<endl;
        exit(1);
    }

    beatstruc beat_agent_004;

    beat_agent_004 = agent->find_server(nodename, agent004, 2.);

    string requestString = "request_hello";
    std::string response;

    // Start executing the agent
    while(agent->running())
    {
        agent->send_request(beat_agent_004, requestString, response, 2.);

        if ( response.size() > 1) {

            cout << "Received from agent_004: " << response.size() << " bytes : " << response << endl;
            // clear the response for next time
            response.clear();

        } else {

            cout << "What happened to agent_004 ??? let's try to find it ..." << endl;
            beat_agent_004.node[0] = '\0'; // reset
            beat_agent_004 = agent->find_server(nodename, agent004, 2.);
            cout << "beat agent 004 node: " << beat_agent_004.utc << endl;

        }

        // sleep for 1 sec
        COSMOS_SLEEP(1);
    }

    return 0;
}
