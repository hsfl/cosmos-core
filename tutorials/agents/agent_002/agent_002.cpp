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
// agent 002 makes request to 002 upon activation

#include "support/configCosmos.h"
#include "agent/agentclass.h"

#include <iostream>
#include <string>

// The request function prototype
int32_t request_hello(char *request, char* response, Agent *cdata);

/// Count of the number of requests that have been run.
static int request_counter = 0;

/// The agent constructor
static Agent *agent;

//!
//! \brief agent_002 is a test agent that demonstrates the interconnectivity with another agent, namely agent_001, through the use of agent requests.
//! This agent will receive a request from agent_001 and will send back a response to it.
//! \param argc Number of arguments provided
//! \param argv The argument values
//! \return int
//!
int main(int argc, char **argv)
{
    cout << "Starting agent_002" << endl;

    // Initialize agent parameters; its name and node
    string agentname = "002"; // Forward facing name of the agent
    string nodename = "cubesat1"; // The node that the agent will run on

    // Specify agent parameters via command line arguments
    switch (argc)
    {
    case 3:
        nodename = argv[2];
    case 2:
        agentname = static_cast<string>(argv[1]) + "_002";
    }

    // Construct agent with above parameters
    agent = new Agent(nodename, agentname);

    // Define the request within the agent
    agent->add_request("request_hello", request_hello);

    // Start executing the agent
    while(agent->running())
    {
        // Sleep for 1 sec
        COSMOS_SLEEP(1.);
    }

    return 0;
}

//!
//! \brief The function to handle agent_001's request.
//! \param The response to send back to agent_001.
//! \return int
//!
int32_t request_hello(char *, char* response, Agent *)
{
    // Send response back to agent_001
    sprintf(response, "hello %d ", request_counter);

    cout << "agent_002 got the request! Its response is: " << response << endl;

    // Increment counter of how many requests were run
    request_counter++;

    return 0;
}
