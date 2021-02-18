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
#include "support/pybind11/pybind11/embed.h"

#include <iostream>
#include <string>

// The request function prototype
int32_t are_you_out_there(string &request, string &response, Agent *cdata);

// ensure the Agent constructor creates only one instance per process
static Agent *agent;
string node_name = "mothership"; 
string agent_name = "mother";
string node_agent_name = "["+node_name+":"+agent_name+"]";


int main(int argc, char **argv)
{
    // construct agent
    cout << node_agent_name << " starting..."<<endl;
    agent = new Agent(node_name, agent_name, 1.);

	// turn off debug
	agent->debug_level=0;

    // exit with error if unable to start agent
    if(agent->last_error() < 0) {
        cerr<<"error: unable to start "<<node_agent_name
			<<" ("<<agent->last_error()<<") "
			<<cosmos_error_string(agent->last_error())<<endl;
        exit(1);
    } else {
    	cout << node_agent_name << " started."<<endl;
	}

	// add custom request functions for this agent
	agent->add_request("are_you_out_there", are_you_out_there, "\n\t\trequest to determine if specific agent exists");


	// shorter name for agent->cinfo
	cosmosstruc* c = agent->cinfo;

    // set the orbital elements (psuedo ISS orbit)
    c->a = 6738000; // 6738 km
    c->e = 0.0001640; // no units!
    c->i =  51.6407 * (M_PI/180.0); // radians
    c->O = 126.7653 * (M_PI/180.0); // radians
    c->w = 163.1572 * (M_PI/180.0); // radians
    c->tau = 0.00; // seconds since periasis

	// n = mean angular motion (rad/s) [ used to find a in TLEs ]
	c->n = pow( (c->mu / pow(c->a,3.0) ), (0.5) );
    // T = period of orbit (seconds)
    c->T = ( 2.0 * M_PI ) / c->n;

	cout<<"\tPeriod in seconds = "<<setprecision(numeric_limits<double>::digits10)<<c->T<<endl;
	cout<<"\tRevolutions / day = "<<setprecision(numeric_limits<double>::digits10)<<86400.0/c->T<<endl;


	// Start interpreter, dies when out of scope
	pybind11::scoped_interpreter guard{};

	// agent loop
    while (agent->running()) {

		cout<<endl<<node_agent_name<<" running..."<<endl;


		// first, try to update current state
		cout<<"\ttrying to update my own damn position..."<<endl;
		cout<<"\tmight as well find my damn specifier..."<<endl;
		cout<<"node  = <"<<c->agent[0].beat.node<<">"<<endl;
		cout<<"agent = <"<<c->agent[0].beat.proc<<">"<<endl;
		cout<<"works!"<<endl;


		// output state
		// one way
		//cout<<c->get_json_pretty("sim_states[0]")<<endl;
		// another way (if the type is not known to COSMOS)
		//cout<<c->get_json_pretty<sim_state>("sim_state[0]")<<endl;

		// set state
		double t = currentmjd();
		c->update_sim_state(t);

		// output state
		cout<<c->get_json_pretty("sim_states")<<endl;


		// stringify the current time
		stringstream sss;
		sss<<setprecision(numeric_limits<double>::digits10)<<t;
		string time = sss.str();


// second, see if you can locate each of the daughters
		string request = "are_you_out_there";
		string response = "";

//  ALLISON
		response.clear();
		agent->send_request(agent->find_agent("daughter_01", "allison", 2.), request, response, 2.);
		// if daughter found
		if(response.size())	{
			cout<<left<<setw(40)<<"\t[daughter_01:allison]"<<setw(16)<<"\033[1;32mFOUND\033[0m";
			response.clear();
			// ask for their location
			agent->send_request(agent->find_agent("daughter_01", "allison", 2.), "get_position " + time, response, 2.);
			cout<<"\n"<<response<<endl;
			// ask for their state
			response.clear();
			agent->send_request(agent->find_agent("daughter_01", "allison", 2.), "get_state", response, 2.);
			// update the record for the mothership
			c->get_pointer<sim_state>("sim_states[1]")->from_json(response);
		} else {
			cout<<left<<setw(40)<<"\t[daughter_01:allison]"<<"\033[1;31mNOT FOUND\033[0m"<<endl;
		}

//  BECKY
		response.clear();
		agent->send_request(agent->find_agent("daughter_02", "becky", 2.), request, response, 2.);
		// if daughter found
		if(response.size())	{
			cout<<left<<setw(40)<<"\t[daughter_02:becky]"<<setw(16)<<"\033[1;32mFOUND\033[0m";
			// ask for their location
			response.clear();
			agent->send_request(agent->find_agent("daughter_02", "becky", 2.), "get_position " + time, response, 2.);
			cout<<"\n"<<response<<endl;
			// ask for their state
			response.clear();
			agent->send_request(agent->find_agent("daughter_02", "becky", 2.), "get_state", response, 2.);
			// update the record for the mothership
			c->get_pointer<sim_state>("sim_states[2]")->from_json(response);
		} else {
			cout<<left<<setw(40)<<"\t[daughter_02:becky]"<<"\033[1;31mNOT FOUND\033[0m"<<endl;
		}

//  CECILIA
		response.clear();
		agent->send_request(agent->find_agent("daughter_03", "cecilia", 2.), request, response, 2.);
		// if daughter found
		if(response.size())	{
			cout<<left<<setw(40)<<"\t[daughter_03:cecilia]"<<setw(16)<<"\033[1;32mFOUND\033[0m";
			// ask for their location
			response.clear();
			agent->send_request(agent->find_agent("daughter_03", "cecilia", 2.), "get_position " + time, response, 2.);
			cout<<"\n"<<response<<endl;
			// ask for their state
			response.clear();
			agent->send_request(agent->find_agent("daughter_03", "cecilia", 2.), "get_state", response, 2.);
			// update the record for the mothership
			c->get_pointer<sim_state>("sim_states[3]")->from_json(response);
		} else {
			cout<<left<<setw(40)<<"\t[daughter_03:cecilia]"<<"\033[1;31mNOT FOUND\033[0m"<<endl;
		}

//  DELILAH
		response.clear();
		// if daughter found
		agent->send_request(agent->find_agent("daughter_04", "delilah", 2.), request, response, 2.);
		if(response.size())	{
			cout<<left<<setw(40)<<"\t[daughter_04:delilah]"<<setw(16)<<"\033[1;32mFOUND\033[0m";
			// ask for their location
			response.clear();
			agent->send_request(agent->find_agent("daughter_04", "delilah", 2.), "get_position " + time, response, 2.);
			cout<<"\n"<<response<<endl;
			// ask for their state
			response.clear();
			agent->send_request(agent->find_agent("daughter_04", "delilah", 2.), "get_state", response, 2.);
			// update the record for the mothership
			c->get_pointer<sim_state>("sim_states[4]")->from_json(response);
		} else {
			cout<<left<<setw(40)<<"\t[daughter_04:delilah]"<<"\033[1;31mNOT FOUND\033[0m"<<endl;
		}

		// Everything is Up to Date (!)


		// insert ability to call a Python function that takes vector<sim_state> (or get the individual info and call f(a,b,c,d,...) )
		// returns a (hopefully updated) sim_state
		
		// Find the function "modify_sim_states" in the python script "testscript.py" (which should be in the same directory as this executable)
		pybind11::function modify_sim_states = pybind11::reinterpret_borrow<pybind11::function>(
			pybind11::module::import("testscript").attr("modify_sim_states")
		);
		// Pass in the json string of the sim_states
		string modifiedStates = modify_sim_states(c->get_json("sim_states")).cast<string>();
		c->from_json(modifiedStates);


		
        // Sleep for 1 sec
        COSMOS_SLEEP(1.);
    }


    return 0;
}



// Identify this agent to the agent who made the request
int32_t are_you_out_there(string & request, string &response, Agent *)
{
	response = "Yes!  I am the one they call " + node_agent_name + ".";
    return 0;
}
