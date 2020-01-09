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
// agent 005 makes request to 002 upon activation

#include "support/configCosmos.h"
#include "support/elapsedtime.h"
#include "support/timeutils.h"
#include "agent/agentclass.h"

#include <iostream>
#include <string>
static Agent *agent;

//!
//! \brief agent_005 is a test agent that demonstrates the interconnectivity with another agent, namely agent_002, through the use of agent requests.
//! This agent will send a request to agent_002 and will print the response that agent_002 provides.
//! \param argc Number of arguments provided
//! \param argv The arguments provided
//! \return int
//!
int main(int, char **)
{
    cout << "Starting agent_005" << endl;

    // Initialize agent parameters; its name, node and communicating agent
    string agentname = "005"; // Forward facing name of the agent
    string nodename = "cubesat1"; // The node that the agent will run on
    string agent002 = "002"; // The name of the agent this agent will speak to

    // Construct agent with above parameters
    agent = new Agent(nodename, agentname);

    // Exit if error is found
    if (agent->last_error() < 0)
    {
        cout << "unable to start agent_exec (" << agent->last_error() << ") " << cosmos_error_string(agent->last_error()) << endl;
        exit(1);
    }

    beatstruc beat_agent_002 = agent->find_server(nodename, agent002, 2.);

    string requestString = "request_hello"; // The name of agent_002's request
    std::string response; // Variable to store agent_002's response

    // Start executing the agent
    while (agent->running())
    {
        // Initiate request from agent_002
        agent->send_request(beat_agent_002, requestString, response, 2.);

        // Check for response from agent_002
        if (response.size() > 1) {
            // The case if agent_002 is on and successfully receives the request
            cout << "Received from agent_002: " << response.size() << " bytes : " << response << endl;

            // Clear the response for next time
            response.clear();
        }
        else
        {
            // The case if agent_002 is not running
            cout << "What happened to agent_002? Let's try to find it..." << endl;

            beat_agent_002.node[0] = '\0'; // reset
            beat_agent_002 = agent->find_server(nodename, agent002, 2.);

            cout << "Beat agent_002 node: " << beat_agent_002.utc << endl;
        }

        // Sleep for 1 sec
        COSMOS_SLEEP(1.);
    }

    return 0;
}
