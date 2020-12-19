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


// The request function prototype
int32_t sample_agent_request_function(string &request, string &response, Agent *cdata);
static uint64_t request_counter = 10000;


/// ensure the Agent constructor creates only one instance per process
static Agent *agent;
string node_name = "sat_001"; 
string agent_name = "agent_scoot";
string node_agent_name = "["+node_name+":"+agent_name+"]";

int main(int argc, char **argv)
{
    // construct agent
    cout << node_agent_name << " starting..."<<endl;
    agent = new Agent(node_name, agent_name, 1.);

    // exit with error if unable to start agent
    if(agent->last_error() < 0) {
        cout<<"error: unable to start "<<node_agent_name<<" ("<<agent->last_error()<<") "<<cosmos_error_string(agent->last_error())<<endl;
        exit(1);
    } else {
    	cout << node_agent_name << " started."<<endl;
	}

	// add custom request functions for this agent
	string request_name = "identify_yourself";
	agent->add_request(request_name, sample_agent_request_function, "\n\t\trequest to support the reporting of identification");

	// try to locate a specific agent (agent_002)
    string agent_target = "agent_002"; // The name of the agent this agent will send requests to
    beatstruc agent_target_heartbeat = agent->find_agent(node_name, agent_target, 2.);
	//cout<<"["<<node_name<<":"<<agent_name<<"] looking for ["<<node_name<<":"<<agent_target<<"]..."<<endl;

	if(agent->debug_level>1)	{
		cout<<"A agent "<<agent_target<<" beatstruc:"<<endl;
		cout<<agent_target_heartbeat;
	}

    string response; // Variable to store agent_002's response

    // Start executing the agent
	//auto test = agent->cinfo->get_json<vector<vector<unitstruc>>>("unit");
	//agent->cinfo->set_value<string>("unit[35][1].name", "NAME");
	//agent->cinfo->set_json(test);
	//cout<<"<vector<vector<unitstruc>>> = <"<<agent->cinfo->get_json_pretty<vector<vector<unitstruc>>>("unit")<<">"<<endl;

	vector<vector<uint16_t>> vvuint16 {{1},{1,2},{1,2,3},{1,2,3,4},{1,2,3,4,5}};
	agent->cinfo->add_name("vvuint16", &vvuint16, "vector<vector<uint16_t>>");
	agent->cinfo->add_name("vvuint16[4][4]", &vvuint16[4][4], "int16_t");
	auto j_vvuint16 = agent->cinfo->get_json("vvuint16");
	agent->cinfo->set_value<uint16_t>("vvuint16[4][4]", 9);
	agent->cinfo->set_json(j_vvuint16);
	cout<<"<vector<vector<uint16_t>>> = <"<<agent->cinfo->get_json_pretty<vector<vector<uint16_t>>>("vvuint16")<<">"<<endl;

	uint32_t t_uint32 = 1;
	agent->cinfo->add_name("t_uint32", &t_uint32, "uint32_t");
	auto j_t_uint32 = agent->cinfo->get_json("t_uint32"); // SCOTTNOTE: error if type is uint32_t, should fix get_json and get_json_pretty
	agent->cinfo->set_value<uint32_t>("t_uint32", 99);
	agent->cinfo->set_json(j_t_uint32);
	cout<<"uint32_t> = <"<<agent->cinfo->get_json_pretty<int>("t_uint32")<<">"<<endl;

	//auto a = agent->cinfo->get_value("")


    while (agent->running()) {

		cout<<"["<<node_name<<":"<<agent_name<<"] running..."<<endl;
        // Sleep for 5 sec
        COSMOS_SLEEP(5.);
    }
    return 0;
}

int32_t sample_agent_request_function(string & request, string &response, Agent *)
{
    // Send response back to agent_002
	response = "I am the one they call [sat_001:agent_scoot]";
    cout << "[" << node_name << ":" << agent_name << "]"<<" received <"<<request.c_str()<<"> request!"<<endl;
	cout << "[" << node_name << ":" << agent_name << "] transmit <"<<request.c_str()<<"> response:\n    TX: \"" << response << "\" ("<<response.size()<<" bytes)"<<endl;

    // Increment counter of how many requests were run
    request_counter--;

    return 0;
}
