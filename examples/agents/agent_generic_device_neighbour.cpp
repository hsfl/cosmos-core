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
#include "agentlib.h"

#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{

    string agentname     = "generic_device_neighbor";
    string nodename      = "telem";

    Agent agent;
    agent.setupServer(nodename, agentname);

    cout << agentname << " is online now" << endl;

    // Start executing the agent
    while(agent.isRunning())
    {
        COSMOS_SLEEP(1.00);
    }

    return 0;
}


