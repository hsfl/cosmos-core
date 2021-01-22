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
int32_t are_you_out_there(string &request, string &response, Agent *cdata);
int32_t req_pass_message(string& request, string& response, Agent *);

/// ensure the Agent constructor creates only one instance per process
static Agent *agent;
string node_name = "worker_02"; 
string agent_name = "beeber";
string node_agent_name = "["+node_name+":"+agent_name+"]";

bool once = false;
string tel_message = "EMPTY";

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
	agent->add_request("are_you_out_there", are_you_out_there, "\n\t\trequest to support the reporting of existence");
    agent->add_request("pass_message", req_pass_message, "\n\t\trequest to pass message");

	cosmosstruc* c = agent->cinfo;
    // set the orbital elements (psuedo ISS orbit)
    c->a = 6738000; // 6738 km
    c->e = 0.0001640; // no units!
    c->i =  51.6407 * (M_PI/180.0); // radians
    c->O = 126.7653 * (M_PI/180.0); // radians
    c->w = 163.1572 * (M_PI/180.0); // radians
    c->tau = 1.0;

    // n = mean angular motion (rad/s) [ used to find a in TLEs ]
    c->n = pow( (c->mu / pow(c->a,3.0) ), (0.5) );
    // T = period of orbit (seconds)
    c->T = ( 2.0 * M_PI ) / c->n;

    string request = "are_you_out_there";
	string response = "";
    string workernames[3] = {"albeert", "elizabeth", "corbee"};

    while (agent->running()) {

		cout<<node_agent_name<<" running..."<<endl;

		// see if you can locate the mothership
		// see if you can locate each of the other daughters
        if(once) {
			int workernum = rand() % (sizeof(workernames)/sizeof(*workernames));
			string worker = "worker_0" + to_string(workernum+1);
			if(workernum == 1) {
				worker = "queen";
			}
			agent->send_request(agent->find_agent(worker, workernames[workernum], 2.), request, response, 2.);
			if(response.size())	{
				string s = "\t[" + worker + ":" + workernames[workernum] + "]";
				cout<<left<<setw(40)<< s <<setw(16)<<"\033[1;32mFOUND\033[0m"<<endl;
				response.clear();
				agent->send_request(agent->find_agent(worker, workernames[workernum], 2.), "pass_message " + tel_message, response, 2);
				// ask for their location
				//agent->send_request(agent->find_agent("daughter_01", "allison", 2.), "get_position " + time, response, 2.);
				cout<<response<<endl;
				response.clear();
				once = false;
			} else {
				string s = "\t[" + worker + ":" + workernames[workernum] + "]";
				cout<<left<<setw(40) << s <<"\033[1;31mNOT FOUND\033[0m"<<endl;
			}
		}
        // Sleep for 5 sec
        COSMOS_SLEEP(5.);
    }
    return 0;
}



int32_t are_you_out_there(string & request, string &response, Agent *)
{
    // Send response back to the agent who made the request
	response = "Yes!  I am the one they call " + node_agent_name + ".";
    return 0;
}

// Playing a game of telephone
int32_t req_pass_message(string& request, string& response, Agent *) {
	cout<<"\tincoming request          = <"<<request<<">"<<endl;

	// remove function call and space
	request.erase(0,13);

	// read in mjdtime
	string timemjd = request;

	int charidx = rand() % timemjd.size();
	char newchar = rand() % 26 + 65;
	timemjd[charidx] = newchar;
	tel_message = timemjd;

	cout << "\tmessage: " << tel_message << endl;

	once = true;

	return 0;
}