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
using namespace std;

int myagent();
int32_t request_start(char *request, char* response_r, Agent *agent); // function prototype of agent request

string agentname     = "agent_cpu_device_test";
string nodename      = "computer";

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat
char buf4[512];

beatstruc beat_agent_cpu;
Agent *agent; // to access the cosmos data, will change later
ElapsedTime et;

#define MAXBUFFERSIZE 256 // comm buffe for agents

int main(int argc, char *argv[])
{
	cout << "Starting agent CPU Device Test" << endl;

	// Establish the command channel and heartbeat
    agent = new Agent(nodename, agentname);
    if (agent->cinfo == nullptr || !agent->running())
    {
		cout << agentname << ": agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
		exit (AGENT_ERROR_JSON_CREATE);
	} else {
		cout<<"Starting " << agentname << " ... OK" << endl;
		//        agent->cinfo->pdata.agent[0].sub
	}

    beat_agent_cpu = agent->find_server(nodename, "agent_cpu", 10.);

	// Start our own thread
	myagent();

	return 0;
}

int myagent()
{
	cout << "agent " << agentname <<  " ...online " << endl;
    string response;
	int count = 1;

	// Start executing the agent
	while(agent->running())
	{
		cout << "-------------------" << endl;
		cout << "Cycle: \t\t\t\t" << count << endl;


        agent->send_request(beat_agent_cpu,
								   "mem",
								   response,
								   2);
		cout << "Memory used in KB: \t\t" << response << endl;

        agent->send_request(beat_agent_cpu,
								   "disk",
								   response,
								   2);
		cout << "Disk used in KB: \t\t" << response << endl;


        agent->send_request(beat_agent_cpu,
								   "load",
								   response,
								   2);
		cout << "Load data in %: \t\t" << response << endl;

		count++;
		COSMOS_SLEEP(2.00);

	}
	return (0);

}
