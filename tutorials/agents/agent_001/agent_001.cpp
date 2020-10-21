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
#include "support/elapsedtime.h"
#include "support/timeutils.h"
#include "agent/agentclass.h"

#include <iostream>
#include <string>

/// The agent constructor
//static Agent *agent;

//!
//! \brief agent_001 is a test agent that demonstrates the interconnectivity with another agent, namely agent_002, through the use of agent requests.
//! This agent will send a request to agent_002 and will print the response that agent_002 provides.
//! \param argc Number of arguments provided
//! \param argv The arguments provided
//! \return int
//!
int main(int argc, char **argv)
{

    // Initialize agent parameters; its name, node and communicating agent
    string agent_name = "001"; // Forward facing name of the agent
    string node_name = "cubesat1"; // The node that the agent will run on

    // Construct agent with above parameters
    cout << "Starting agent "<<agent_name<<"...";
    Agent* agent = new Agent(node_name, agent_name, 1.);

    // exit with error if unable to start agent
    if(agent->last_error() < 0) {
        cout<<"error: unable to start agent "<<agent_name<<" ("<<agent->last_error()<<") "<<cosmos_error_string(agent->last_error())<<endl;
        exit(1);
    } else {
		cout<<" started."<<endl;
	}

	// try to locate agent002
    string agent002 = "002"; // The name of the agent this agent will speak to
    beatstruc beat_agent_002 = agent->find_agent(node_name, agent002, 2.);
	//cout<<"outputting beatstruc from find_agent(..) call"<<endl;
	//cout<<beat_agent_002;

    string requestString = "request_hello"; // The name of agent_002's request
    string response; // Variable to store agent_002's response

    // Start executing the agent
    while (agent->running())
    {
		cout<<"agent "<<agent_name<<" is running..."<<endl;
        // Initiate request from agent_002
        agent->send_request(beat_agent_002, requestString, response, 2.);

        // Check for response from agent_002
        if (response.size() > 1) {
            // The case if agent_002 is on and successfully receives the request
            cout << "Received response from agent_002: " << response.size() << " bytes: " << response << endl;

            // Clear the response for next request
            response.clear();
        }
        else
        {
            // The case if agent_002 is not running
            cout << "What happened to agent_002? Let's try to find it..." << endl;

    		beat_agent_002 = agent->find_agent(node_name, agent002, 2.);

			//cout<<"Beat from agent 002 from call to find_agent(..):"<<endl;
			//cout<<beat_agent_002<<endl;
        }

        // Sleep for 1 sec
        COSMOS_SLEEP(1.);
    }

    return 0;
}
