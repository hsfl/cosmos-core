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

/*! \file agent_calc.cpp
* \brief Demonstration calculator Agent
*/

//! \ingroup agents
//! \defgroup agent_calc Calculation Agent
//! This program accepts requests to perform simple math operations and returns the result.
//! With it you can:
//! - add
//! - subtract
//! - multiply
//! - divide

// code from testbed/blank_agent.cpp

#include "support/configCosmos.h"

#include <stdio.h>
#include <iostream>

using namespace std;

#include "agent/agentclass.h"
#include "physics/physicslib.h"
#include "support/jsonlib.h"

int myagent();

static char nodename[COSMOS_MAX_NAME + 1] = "otb";
static char agentname[COSMOS_MAX_NAME + 1] = "calc";

int32_t request_add(char *request, char* response, Agent *agent);
int32_t request_sub(char *request, char* response, Agent *agent);
int32_t request_mul(char *request, char* response, Agent *agent);
int32_t request_div(char *request, char* response, Agent *agent);

int32_t request_change_node_name(char *request, char* response, Agent *agent);

#define MAXBUFFERSIZE 100000 // comm buffer for agents

static Agent *agent; // to access the cosmos data, will change later

int main(int argc, char *argv[])
{
	int irtn;

    // Process arguments if present

    // Make node_name = 1st argument
	if (argc == 2)
        strcpy(nodename, argv[1]);
    // Make agent_proc = 2st argument
	if (argc == 3)	{
        strcpy(agentname, argv[2]);
        strcpy(nodename, argv[1]);
	}

    // Initialize agents. Set nodename if provided through command line args
	if (argc > 1)
    {
        // Initialize agent instance with specified node and agent names
        agent = new Agent(nodename, agentname);

        // Check if agent was successfully constructed.
        if (agent->cinfo == nullptr || !agent->running())
        {
			printf("Failed to open [%s:%s]\n",nodename,agentname);
			exit (1);
        }

        cout << "Hello, I am an agent. My name is [" << nodename << ":" << agentname << "]" << endl << endl;
	}
	else
    {
        // Initialize agent instance with unspecified node name but specified agent name
        agent = new Agent("", agentname);

        // Check if agent was successfully constructed.
        if (agent->cinfo == nullptr || !agent->running())
        {
			printf("Failed to open [null:%s]\n",agentname);
			exit (1);
		}

        cout << "Hello, I am an agent. My name is [null:" << agentname << "]" << endl << endl;
	}

    for (uint16_t i=0; i<agent->cinfo->jmap.size(); ++i)
	{
        if (agent->cinfo->jmap[i].size())
		{
            cout << "jmap[" << i << "]:" << agent->cinfo->jmap[i][0].name << endl;
		}
	}

    cout << agent->cinfo->node.name << endl;

	string jsp;
    json_out_name(jsp, const_cast<char *>("node_name"));
    cout << jsp << endl;


    // Define the requests that we need for this agent
    if ((irtn=agent->add_request("add",request_add)))
		exit (irtn);

    if ((irtn=agent->add_request("sub",request_sub)))
		exit (irtn);

    if ((irtn=agent->add_request("mul",request_mul)))
		exit (irtn);

    if ((irtn=agent->add_request("div",request_div)))
		exit (irtn);

    if ((irtn=agent->add_request("node",request_change_node_name)))
		exit (irtn);

	// Start our own thread
	irtn = myagent();
}

int myagent()
{
	// Start performing the body of the agent
    while(agent->running())
	{
		COSMOS_SLEEP(0.1); // no support in win
	}

	return 0;
}

// the name of this fn will always be changed
int32_t request_add(char *request, char* response, Agent *agent)
{
	float a,b;

	sscanf(request,"%*s %f %f",&a,&b);
    sprintf(response,"%f",a + b);

	return 0;
}

// the name of this fn will always be changed
int32_t request_sub(char *request, char* response, Agent *agent)
{
	float a,b;

	sscanf(request,"%*s %f %f",&a,&b);
    sprintf(response,"%f", a - b);

	return 0;
}
// the name of this fn will always be changed
int32_t request_mul(char *request, char* response, Agent *agent)
{
	float a,b;

    sscanf(request,"%*s %f %f", &a, &b);
    sprintf(response,"%f", a * b);

	return 0;
}
// the name of this fn will always be changed
int32_t request_div(char *request, char* response, Agent *agent)
{
	float a,b;

    sscanf(request,"%*s %f %f", &a, &b);
    sprintf(response,"%f", a / b);

	return 0;
}

int32_t request_change_node_name(char *request, char* response, Agent *agent)
{
	char new_name[41];
	sscanf(request,"%*s %40s", new_name);

    strcpy(agent->cinfo->node.name, new_name);
    cout << "The new node name is <" << agent->cinfo->node.name << ">" << endl;

	return 0;
}
