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
//#include "agent/agentclasslib.h"
#include "agent/agentclass.h"

#include <iostream>
#include <string>
using namespace std;

// function prototype of agent request
int32_t request_hello(char *request, char* response, Agent *cdata);

// counter to test number of requests
int countReq = 0;
Agent *agent;

int main(int, char **)
{
    //setEnvCosmos(cosmosPath);

    cout << "Starting agent " << endl;

    string agentname     = "002";
    string nodename      = "telem";
    agent = new Agent(nodename, agentname);

    agent->add_request("request_hello", request_hello);

    // start main loop
    while(agent->running())
    {
        // sleep for 1 sec
        COSMOS_SLEEP(1.00);
    }
    return 0;
}

// implement request function
int32_t request_hello(char *, char* response, Agent *)
{

    sprintf(response,"hello %d ",countReq);

    cout << "agent 002 got request! response is: " << response << endl;

    // add counter
    countReq ++;

    return 0;
}
