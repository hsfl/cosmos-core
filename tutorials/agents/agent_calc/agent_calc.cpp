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
#include "physics/physicslib.h" // long term we may move this away
#include "support/jsonlib.h"

int myagent();

//do we need a NODEMAXNAME? using COSMOS_MAX_NAME for now...
char nodename[COSMOS_MAX_NAME+1] = "otb";
char agentname[COSMOS_MAX_NAME+1] = "calc";

int waitsec = 5; // wait to find other agents of your 'type/name', seconds

//int32_t *request_run_program(char *request, char* response, Agent *agent); // extra request
int32_t request_add(char *request, char* response, Agent *agent);
int32_t request_sub(char *request, char* response, Agent *agent);
int32_t request_mul(char *request, char* response, Agent *agent);
int32_t request_div(char *request, char* response, Agent *agent);

int32_t request_change_node_name(char *request, char* response, Agent *agent);


#define MAXBUFFERSIZE 100000 // comm buffer for agents

Agent *agent; // to access the cosmos data, will change later

int main(int argc, char *argv[])
{
	int irtn;

	// process arguments if present

	// make node_name = 1st argument
	if (argc == 2)
		strcpy(nodename,argv[1]);
	// make agent_proc = 2st argument
	if (argc == 3)	{
		strcpy(agentname,argv[2]);
		strcpy(nodename,argv[1]);
	}

	// Initialization stuff
	if (argc > 1)
	{
        agent = new Agent(nodename, agentname);
        if (agent->cinfo == nullptr || !agent->running())
        {
			printf("Failed to open [%s:%s]\n",nodename,agentname);
			exit (1);
		}
		cout<<"Hello, I am an agent. My name is ["<<nodename<<":"<<agentname<<"]"<<endl<<endl;
	}
	else
	{
        agent = new Agent("", agentname);
        if (agent->cinfo == nullptr || !agent->running())
        {
			printf("Failed to open [null:%s]\n",agentname);
			exit (1);
		}
		cout<<"Hello, I am an agent. My name is [null:"<<agentname<<"]"<<endl<<endl;
	}


    for (uint16_t i=0; i<agent->cinfo->meta.jmap.size(); ++i)
	{
        if (agent->cinfo->meta.jmap[i].size())
		{
            cout<<"jmap["<<i<<"]:"<<agent->cinfo->meta.jmap[i][0].name<<endl;
		}
	}
    cout<<agent->cinfo->pdata.node.name<<endl;

	string jsp;
	json_out_name(jsp,(char *)"node_name");
	cout<<jsp<<endl;


	// Add additional requests
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
	sprintf(response,"%f",a+b);
	return 0;
}

// the name of this fn will always be changed
int32_t request_sub(char *request, char* response, Agent *agent)
{
	float a,b;
	sscanf(request,"%*s %f %f",&a,&b);
	sprintf(response,"%f",a-b);
	return 0;
}
// the name of this fn will always be changed
int32_t request_mul(char *request, char* response, Agent *agent)
{
	float a,b;
	sscanf(request,"%*s %f %f",&a,&b);
	sprintf(response,"%f",a*b);
	return 0;
}
// the name of this fn will always be changed
int32_t request_div(char *request, char* response, Agent *agent)
{
	float a,b;
	sscanf(request,"%*s %f %f",&a,&b);
	sprintf(response,"%f",a/b);
	return 0;
}

int32_t request_change_node_name(char *request, char* response, Agent *agent)
{
	char new_name[41];
	sscanf(request,"%*s %40s", new_name);

    strcpy(agent->cinfo->pdata.node.name, new_name);
    cout<<"The new node name is <"<< agent->cinfo->pdata.node.name <<">"<<endl;

	return 0;
}

