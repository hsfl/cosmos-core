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


	// agent loop
    while (agent->running()) {

		cout<<node_agent_name<<" running..."<<endl;

		// see if you can locate each of the daughters
		string request = "are_you_out_there";
		string response = "";

		// stringify the current time
		double t = currentmjd();
		stringstream sss;
		sss<<setprecision(numeric_limits<double>::digits10)<<t;
		string time = sss.str();

		// calculate position of Mothership
		//double mother_x = 0.0, mother_y = 0.0, mother_z = 0.0;


		agent->send_request(agent->find_agent("daughter_01", "allison", 2.), request, response, 2.);
		if(response.size())	{
			cout<<left<<setw(40)<<"\t[daughter_01:allison]"<<setw(16)<<"\033[1;32mFOUND\033[0m";
			response.clear();


			// (see how long to send/receive requests)
			//double tx_time = currentmjd();
			//agent->send_request(agent->find_agent("daughter_01", "allison", 2.), "get_time", response, 2.);
			//double rx_time = currentmjd();
			//cout<<"get_time = "<<response<<endl;
			//stringstream ss;
			//ss<<response;
			//double go_time;
			//ss>>go_time;
			//cout<<"\t\trequest time = <"<<setprecision(numeric_limits<double>::digits10)<<go_time-tx_time<<">"<<endl;
			//cout<<"\t\treceive time = <"<<setprecision(numeric_limits<double>::digits10)<<rx_time-go_time<<">"<<endl;

			//cout<<setprecision(numeric_limits<double>::digits10)<<"time = "<<tx_time<<endl;
			//cout<<setprecision(numeric_limits<double>::digits10)<<"time = "<<go_time<<endl;
			//cout<<setprecision(numeric_limits<double>::digits10)<<"time = "<<rx_time<<endl;


			// ask for their location
			agent->send_request(agent->find_agent("daughter_01", "allison", 2.), "get_position " + time, response, 2.);
			cout<<"\n"<<response<<endl;

			// de-stringify location data
			//stringstream ss;
			//string junk;
			//double x_coord = 0.0;
			//double y_coord = 0.0;
			//double z_coord = 0.0;
			//ss<<response;
			//ss>>junk>>junk>>junk>>junk>>junk>>junk>>x_coord>>y_coord>>z_coord;
			//ss>>x_coord>>y_coord>>z_coord;

			
		} else {
			cout<<left<<setw(40)<<"\t[daughter_01:allison]"<<"\033[1;31mNOT FOUND\033[0m"<<endl;
		}

		response.clear();
		agent->send_request(agent->find_agent("daughter_02", "becky", 2.), request, response, 2.);
		if(response.size())	{
			cout<<left<<setw(40)<<"\t[daughter_02:becky]"<<setw(16)<<"\033[1;32mFOUND\033[0m";
			// ask for their location
			response.clear();
			agent->send_request(agent->find_agent("daughter_02", "becky", 2.), "get_position " + time, response, 2.);
			cout<<"\n"<<response<<endl;
		} else {
			cout<<left<<setw(40)<<"\t[daughter_02:becky]"<<"\033[1;31mNOT FOUND\033[0m"<<endl;
		}

		response.clear();
		agent->send_request(agent->find_agent("daughter_03", "cecilia", 2.), request, response, 2.);
		if(response.size())	{
			cout<<left<<setw(40)<<"\t[daughter_03:cecilia]"<<setw(16)<<"\033[1;32mFOUND\033[0m";
			// ask for their location
			response.clear();
			agent->send_request(agent->find_agent("daughter_03", "cecilia", 2.), "get_position " + time, response, 2.);
			cout<<"\n"<<response<<endl;
		} else {
			cout<<left<<setw(40)<<"\t[daughter_03:cecilia]"<<"\033[1;31mNOT FOUND\033[0m"<<endl;
		}

		response.clear();
		agent->send_request(agent->find_agent("daughter_04", "delilah", 2.), request, response, 2.);
		if(response.size())	{
			cout<<left<<setw(40)<<"\t[daughter_04:deliliah]"<<setw(16)<<"\033[1;32mFOUND\033[0m";
			// ask for their location
			response.clear();
			agent->send_request(agent->find_agent("daughter_04", "delilah", 2.), "get_position " + time, response, 2.);
			cout<<"\n"<<response<<endl;
		} else {
			cout<<left<<setw(40)<<"\t[daughter_04:deliliah]"<<"\033[1;31mNOT FOUND\033[0m"<<endl;
		}

        // Sleep for 5 sec
        COSMOS_SLEEP(5.);
    }
    return 0;
}



// Identify this agent to the agent who made the request
int32_t are_you_out_there(string & request, string &response, Agent *)
{
	response = "Yes!  I am the one they call " + node_agent_name + ".";
    return 0;
}
