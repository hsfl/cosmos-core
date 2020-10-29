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
#include "support/stringlib.h"

#include <iostream>
#include <string>

// The request function prototype
int32_t hello_agent_002_request_function(string &request, string &response, Agent *cdata);

/// number of requests that have been run since restarting agent
static uint64_t request_counter = 0;

/// ensure only one agent class instance per process
string node_name = "sat_001";
string agent_name = "agent_002";
string node_agent_name = "["+node_name+":"+agent_name+"]";
static Agent* agent;

/// Program to demonstrate inter-communication between agents
int main(int argc, char **argv)
{
		int jah = 420;
		int jah2 = 240;
    // construct agent 
	cout << node_agent_name <<" starting..."<<endl;
	agent = new Agent(node_name, agent_name, 1.);
    if(agent->last_error() < 0) {
        cout<<"error: unable to start "<<node_agent_name<<" ("<<agent->last_error()<<") "<<cosmos_error_string(agent->last_error())<<endl;
        exit(1);
    } else {
    	cout << ""<<node_agent_name<<" started."<<endl;
    }

    // add custom request functions for this agent
    agent->add_request("any_body_out_there", hello_agent_002_request_function, "a request to respond with 'hello'");

    // Start executing the agent
    while(agent->running()) {
		cout<<node_agent_name<<" running..."<<endl;
	
        // Sleep for 4 sec
        COSMOS_SLEEP(4.);
        // Initiate request to agent_001
		// name of agent target for request
		string agent_target = "agent_001";

		beatstruc agent_target_heartbeat = agent->find_agent(node_name, agent_target, 2.);
		string target_request_name = "identify_yourself";
		string response;
	    agent->send_request(agent_target_heartbeat, target_request_name, response, 2.);
		cout<<node_agent_name<<" transmit <"<<target_request_name<<"> request to ["<<node_name<<":"<<agent_target<<"]..."<<endl;

		if (response.size() > 1) {
        	// The case if agent_001 is on and successfully receives the request
        	cout << node_agent_name<<" received <"<<target_request_name<<"> response from ["<<agent_target_heartbeat.node<<":"<<agent_target_heartbeat.proc<<"]:"<<endl;
			cout<<"    RX: \""<< response <<"\" ("<<response.size()<<" bytes)"<< endl;
        }

        COSMOS_SLEEP(2.);
		string response2;
		string request2 = "setvalue {\"node_loc_orbit\":"+std::to_string((jah++))+"}";
		// orphan request for testing
		cout<<"attempting setting orbit value..."<<endl;
	   	agent->send_request(agent->find_agent(node_name, agent_target, 2.), request2, response2, 2.);

        COSMOS_SLEEP(2.);
		// orphan request for testing
		string response1;
		string request1 = "setvalue {\"node_loc_utc\":"+std::to_string((jah2--))+"}";
		cout<<"attempting setting UTC value..."<<endl;
	   	agent->send_request(agent->find_agent(node_name, agent_target, 2.), request1, response1, 2.);

		string response3;
		cout<<"attempting getting UTC value..."<<endl;
		string request3 = "getvalue {\"node_loc_utc\"}";
	   	agent->send_request(agent->find_agent(node_name, agent_target, 2.), request3, response3, 2.);
		cout<<"    response == <"<<response3<<">"<<endl;
    }
    return 0;
}

//!
//! \brief The function to handle agent_001's request.
//! \param The response to send back to agent_001.
//! \return int
//!
// JIMNOTE:  Why is Agent part of request function signature?
int32_t hello_agent_002_request_function(string & request, string &response, Agent *a)
{
    // Send response back to agent_001
    cout << "[" << node_name << ":" << agent_name <<"] received <"<<request<<"> request!"<<endl;
    response = "You have found me " + to_unsigned(++request_counter) + " times.";
	cout<<"["<<node_name<<":"<<agent_name<<"] transmit <"<<request<<"> response:\n    TX: \""<<response<<"\" ("<<response.size()<<" bytes)"<<endl;

	// agent is set to this pointer?
    //cout << "[" << node_name << ":" << agent_name <<"] received a request from ["<<a->nodeName<<":"<<a->agentName<<"]! Its response is: " << response << endl;
    return 0;
}
