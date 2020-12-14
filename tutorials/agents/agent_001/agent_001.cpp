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
string agent_name = "agent_001";
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
	agent->add_request(request_name, sample_agent_request_function, "request to support the reporting of identification");

	// try to locate a specific agent (agent_002)
    string agent_target = "agent_002"; // The name of the agent this agent will send requests to
    beatstruc agent_target_heartbeat = agent->find_agent(node_name, agent_target, 2.);
	//cout<<"["<<node_name<<":"<<agent_name<<"] looking for ["<<node_name<<":"<<agent_target<<"]..."<<endl;

	if(agent->debug_level>1)	{
		cout<<"A agent "<<agent_target<<" beatstruc:"<<endl;
		cout<<agent_target_heartbeat;
	}

    string response; // Variable to store agent_002's response

	agent->cinfo->add_default_names();
    // Start executing the agent
	agent->cinfo->add_name("Short UTC", &agent->cinfo->node.loc.utc, "double");
	agent->cinfo->add_name("Longest Ever UTC", &agent->cinfo->node.loc.utc, "double");
	agent->cinfo->set_value<double>("Short UTC", 213.0);

	// test set_json with double
	//agent->cinfo->set_json("Short UTC", "{\"Short UTC\": 214.5}");
	agent->cinfo->set_json("{\"Short UTC\": 214.5}");

	cout<<"Short UTC        = <"<<agent->cinfo->get_value<double>("Short UTC")<<">"<<endl;
	cout<<"Longest Ever UTC = <"<<agent->cinfo->get_value<double>("Longest Ever UTC")<<">"<<endl;

	agent->cinfo->add_name("cosmosdata", &*agent->cinfo, "cosmosstruc");
	//cout<<"cosmosdata       = <"<<agent->cinfo->get_json_pretty<cosmosstruc>("cosmosdata")<<">"<<endl;

	agent->cinfo->add_name("My Favorite Users", &agent->cinfo->user, "vector<userstruc>");
	cout<<"Old User Data       = <"<<agent->cinfo->get_json<vector<userstruc>>("My Favorite Users")<<">"<<endl;

	string raw = "{\"My Favorite Users\": [{\"cpu\": \"\", \"name\": \"\", \"node\": \"\", \"tool\": \"\"},{\"cpu\": \"cpu2\", \"name\": \"name2\", \"node\": \"node2\", \"tool\": \"tool2\"}]}";
	agent->cinfo->set_json(raw);
	
	cout<<"New User Data       = <"<<agent->cinfo->get_json<vector<userstruc>>("My Favorite Users")<<">"<<endl;
	cout<<"New User Data       = <"<<agent->cinfo->get_json<userstruc>("user[1]")<<">"<<endl;
	//cout<<"names = "<<agent->cinfo->names.size()<<endl;
	//cout<<"names = "<<agent->cinfo->names.size()<<endl;
	//agent->cinfo->print_all_names_types_values();
	//cout<<"names = "<<agent->cinfo->names.size()<<endl;

    while (agent->running()) {

		cout<<"["<<node_name<<":"<<agent_name<<"] running..."<<endl;
/* old tests
		string target_request_name = "any_body_out_there";
		agent->cinfo->set_value<double>("Cooler UTC", 99.99);
		cout<<"\tUTC == "<< agent->cinfo->node.loc.utc <<endl;
		cout<<"\tCooler UTC == "<< agent->cinfo->get_value<double>("Cooler UTC")<<endl;
        cout<<"\tORBIT == "<< agent->cinfo->node.loc.pos.orbit <<endl;

		// this agent can set his own values ... obvs...  can another agent?
		//agent->cinfo->node.loc.utc = 13.456;

		cout<<node_agent_name<<" transmit <"<<target_request_name<<"> request to ["<<node_name<<":"<<agent_target<<"]..."<<endl;
        // Initiate request to agent_002
        agent->send_request(agent_target_heartbeat, "any_body_out_there", response, 2.);

        // Check for response from agent_002
        if (response.size() > 1) {
            // The case if agent_002 is on and successfully receives the request
            cout << node_agent_name << " received <"<<target_request_name<<"> response from ["<<agent_target_heartbeat.node<<":"<<agent_target_heartbeat.proc<<"]:\n    RX: \"" << response << "\" ("<<response.size()<<" bytes)"<<endl;

            // Clear the response for next request
            response.clear();
        } else {
            // The case if agent_002 is not running
			//cout<<"["<<node_name<<":"<<agent_name<<"] looking for ["<<node_name<<":"<<agent_target<<"]..."<<endl;

    		agent_target_heartbeat = agent->find_agent(node_name, agent_target, 2.);
			if(agent->debug_level>1)	{
				cout<<"B agent "<<agent_target<<" beatstruc:"<<endl;
				cout<<agent_target_heartbeat<<endl;
			}
        }
*/
        // Sleep for 5 sec
        COSMOS_SLEEP(5.);
    }
    return 0;
}

int32_t sample_agent_request_function(string & request, string &response, Agent *)
{
    // Send response back to agent_002
	response = "I am the one they call [sat_001:agent_001]";
	//JIMNOTE: there is a actually a bug with the string request variable supplied...  it is not passed properly to the user's request function after multiple different requests have been made.  the request is parsed properly, i.e. this function is still called, but the request string contains overflow characters from previous requests beyond the null character.  somewhere a string.resize() call is missing.  HACKY FIX: mixing char[] within a string only prints correctly if cast as c_str(), so do that for now.
    cout << "[" << node_name << ":" << agent_name << "]"<<" received <"<<request.c_str()<<"> request!"<<endl;
	cout << "[" << node_name << ":" << agent_name << "] transmit <"<<request.c_str()<<"> response:\n    TX: \"" << response << "\" ("<<response.size()<<" bytes)"<<endl;

    // Increment counter of how many requests were run
    request_counter--;

    return 0;
}
