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

// This is an example agent that needs to run for testing
// agent_generic_device_test for generic device
#include "configCosmos.h"
//#include "elapsedtime.hpp"
#include "timeutils.hpp"
#include "agentlib.h"

#include <iostream>
#include <string>
using namespace std;

int myagent();

string agentname     = "agent_generic_device_neighbor";
string nodename      = "telem";

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat
char buf4[512];

beatstruc beat_agent_002;
cosmosstruc *cdata; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffe for agents

int main(int argc, char *argv[])
{
    cout << "Agent Name: " << agentname << endl;
    cout << "Agent Setup Server ... " ;
    // Establish the command channel and heartbeat
    if (!(cdata = agent_setup_server(AGENT_TYPE_UDP,
                                     nodename.c_str(),
                                     agentname.c_str(),
                                     1.0,
                                     0,
                                     AGENTMAXBUFFER)))
    {
        cout << "agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
        cout << "OK" << endl;
    }
    
    // Start our own thread
	myagent();

    return 0;
}

int myagent()
{
    cout << agentname << " is online now" << endl;

    // Start executing the agent
    while(agent_running(cdata))
    {
        COSMOS_SLEEP(1.00);
    }
    return (0);

}
