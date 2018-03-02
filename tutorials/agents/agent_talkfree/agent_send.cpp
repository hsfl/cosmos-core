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

// Example of an agent post broadcasting genetic data to the network
// the data is not in the COSMOS namespace

#include "agent/agentclass.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{

    Agent *agent;

    agent = new Agent("telem","send");
    if (agent->cinfo != nullptr && agent->running())
    {
        while (agent->running())
        {
            string message {"helloBB"};

            agent->post((Agent::AgentMessage)0xBB, message);

            cout << "tx: " <<  message << endl;

            COSMOS_SLEEP(1);
        }
    }

    return 0;
}
