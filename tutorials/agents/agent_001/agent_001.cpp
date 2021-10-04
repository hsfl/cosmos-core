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
#include "support/stringlib.h"
#include "support/convertlib.h"

#include <iostream>
#include <string>


// The request function prototype
int32_t sample_agent_request_function(string &request, string &response, Agent *cdata);
static uint64_t request_counter = 10000;


/// ensure the Agent constructor creates only one instance per process
static Agent *agent;
string node_name = "sat_111"; 
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


    // turn off debug
    agent->set_debug_level(0);


    // add custom request functions for this agent
    string request_name = "identify_yourself";
    agent->add_request(request_name, sample_agent_request_function, "\n\t\trequest to support the reporting of identification");

    // try to locate a specific agent (agent_002)
    string node_target = "sat_001";
    string agent_target = "agent_002"; // The name of the agent this agent will send requests to
    beatstruc agent_target_heartbeat = agent->find_agent(node_name, agent_target, 2.);
    //cout<<"["<<node_name<<":"<<agent_name<<"] looking for ["<<node_name<<":"<<agent_target<<"]..."<<endl;

    if(agent->get_debug_level()>1)	{
        cout<<"A agent "<<agent_target<<" beatstruc:"<<endl;
        cout<<agent_target_heartbeat;
    }

    string response; // Variable to store agent_002's response

    // Start executing the agent
    agent->cinfo->add_name("Short UTC", &agent->cinfo->node.loc.utc, "double");
    agent->cinfo->add_name("Longest Ever UTC", &agent->cinfo->node.loc.utc, "double");
    agent->cinfo->set_value<double>("Short UTC", 213.0);

    // test set_json with double
    //agent->cinfo->set_json("Short UTC", "{\"Short UTC\": 214.5}");
    agent->cinfo->set_json("{\"Short UTC\": 214.5}");

    cout<<"Short UTC        = <"<<agent->cinfo->get_value<double>("Short UTC")<<">"<<endl;
    cout<<"Longest Ever UTC = <"<<agent->cinfo->get_value<double>("Longest Ever UTC")<<">"<<endl;

    //agent->cinfo->add_name("cosmosdata", &*agent->cinfo, "cosmosstruc");
    agent->cinfo->add_name("cosmosdata", agent->cinfo, "cosmosstruc");
    cout<<"cosmosdata       = <"<<agent->cinfo->get_json_pretty<cosmosstruc>("cosmosdata")<<">"<<endl;

    agent->cinfo->add_name("My Favorite Users", &agent->cinfo->user, "vector<userstruc>");
    cout<<"Old User Data       = <"<<agent->cinfo->get_json<vector<userstruc>>("My Favorite Users")<<">"<<endl;

    string raw = "{\"My Favorite Users\": [{\"cpu\": \"\", \"name\": \"\", \"node\": \"\", \"tool\": \"\"},{\"cpu\": \"cpu2\", \"name\": \"name2\", \"node\": \"node2\", \"tool\": \"tool2\"}]}";
    agent->cinfo->set_json(raw);

    cout<<"New User Data       = <"<<agent->cinfo->get_json<vector<userstruc>>("My Favorite Users")<<">"<<endl;
    cout<<"New User Data       = <"<<agent->cinfo->get_json<userstruc>("user[1]")<<">"<<endl;


    //agent->set_sohstring2({});	// works!
    //agent->set_sohstring2({"Short UTC"});		// works!
    //agent->set_sohstring2({"A_NAME_NOT_IN_NAMESPACE"});	// works!
    agent->set_sohstring2({"Short UTC","Longest Ever UTC","A_NAME_NOT_IN_NAMESPACE","devspec"});

    // testing longitude/latitude/altitude (ie: geodetic) conversion to eci
    Convert::locstruc testtarget;
    Convert::loc_clear(testtarget);
    testtarget.pos.geod.s = {0.1047197551,-1.308996938995,1000}; // remember cosmos uses radians
    testtarget.pos.geod.utc = 55213.43097222;
    testtarget.pos.geod.pass++;
    agent->cinfo->add_name("testtarget", &testtarget, "locstruc");
    agent->cinfo->add_name("testtarget.pos.geod", &testtarget.pos.geod, "geoidpos");
    agent->cinfo->add_name("testtarget.pos.eci", &testtarget.pos.eci, "cartpos");
    Convert::pos_geod2geoc(agent->cinfo->get_pointer<Convert::locstruc>("testtarget"));
    Convert::pos_geoc2eci(agent->cinfo->get_pointer<Convert::locstruc>("testtarget"));
    cout<<"testtarget.pos.geod    = " << agent->cinfo->get_json_pretty<Convert::geoidpos>("testtarget.pos.geod") << endl;
    cout<<"testtarget.pos.eci     = " << agent->cinfo->get_json_pretty<Convert::cartpos>("testtarget.pos.eci") << endl;
    // Successful conversion should give us about: [-6.0744, -1.8289, 0.6685] * 1E06, which it does. Cool!

    // test orbital dynamics for simulation

    // km
    agent->cinfo->P_pos_t = 6285.0;
    agent->cinfo->Q_pos_t = 3628.6;
    agent->cinfo->W_pos_t = 0.0;
    // km/s
    agent->cinfo->P_vel_t = -2.4913;
    agent->cinfo->Q_vel_t = 11.290;
    agent->cinfo->W_vel_t = 0.0;


    agent->cinfo->O = 40.0 * ( M_PI / 180.0);
    agent->cinfo->i = 30.0 * ( M_PI / 180.0);
    agent->cinfo->w = 60.0 * ( M_PI / 180.0);

    agent->cinfo->set_up_rotation_matrix();
    cout<<"("<<agent->cinfo->R_0_0<<", "<<agent->cinfo->R_0_1<<", "<<agent->cinfo->R_0_2<<")"<<endl;
    cout<<"("<<agent->cinfo->R_1_0<<", "<<agent->cinfo->R_1_1<<", "<<agent->cinfo->R_1_2<<")"<<endl;
    cout<<"("<<agent->cinfo->R_2_0<<", "<<agent->cinfo->R_2_1<<", "<<agent->cinfo->R_2_2<<")"<<endl;

    agent->cinfo->set_IJK_from_PQW();

    cout<<"("<<agent->cinfo->I_pos_t<<", "<<agent->cinfo->J_pos_t<<", "<<agent->cinfo->K_pos_t<<")"<<endl;
    cout<<"("<<agent->cinfo->I_vel_t<<", "<<agent->cinfo->J_vel_t<<", "<<agent->cinfo->K_vel_t<<")"<<endl;

    agent->cinfo->P_pos_t = -1;
    agent->cinfo->Q_pos_t = -1;
    agent->cinfo->W_pos_t = -1;

    agent->cinfo->P_vel_t = -1;
    agent->cinfo->Q_vel_t = -1;
    agent->cinfo->W_vel_t = -1;

    cout<<"("<<agent->cinfo->P_pos_t<<", "<<agent->cinfo->Q_pos_t<<", "<<agent->cinfo->W_pos_t<<")"<<endl;
    cout<<"("<<agent->cinfo->P_vel_t<<", "<<agent->cinfo->Q_vel_t<<", "<<agent->cinfo->W_vel_t<<")"<<endl;
    agent->cinfo->set_PQW_from_IJK();
    cout<<"("<<agent->cinfo->P_pos_t<<", "<<agent->cinfo->Q_pos_t<<", "<<agent->cinfo->W_pos_t<<")"<<endl;
    cout<<"("<<agent->cinfo->P_vel_t<<", "<<agent->cinfo->Q_vel_t<<", "<<agent->cinfo->W_vel_t<<")"<<endl;

    cout<<"Okay, that co-ord transformation works!"<<endl<<endl;

    cout<<"Now to try to get the orbis of ISS..."<<endl;

    // shorter name for cosmosdata
    cosmosstruc* c = agent->cinfo;

    // here is a TLE
    // try to figure out the orbit!
    //		1 25544U 98067A   20358.21750033  .00016717  00000-0  10270-3 0  9095
    //		2 25544  51.6435 133.1544 0001122 148.6515 211.4702 15.49226224 21314
    //		         ******* --> inclination in degrees
    //		                 ******** --> longitude of the right ascending node in degrees
    //		                          ******* --> eccentricity (assumed leading decimal)
    //		                                  ******** --> argument of the perigee in degrees
    //		                                                    *********** --> mean (angular) motion in rev/day
    //		                                                    must convert to rad/s !!!

    // semi-major axis from mean motion of TLE (mean elements vs osculating elements)
    double N = 15.49226224; // rev/day

    // convert to rad/s

    c->n = (2.0*M_PI*N / 86400); // convert to rad / s
    c->a = pow(c->mu, 1.0/3.0) / pow(c->n, 2.0/3.0);
    cout<< "calculated a from TLE = "<<c->a<<endl;

    c->T = ( 2.0 * M_PI ) / c->n;
    cout<< "calculated T (in seconds) = "<<c->T<<endl;

    // eccentricity
    c->e = 0.0001122;
    // longitude of the right ascending node
    c->O = 133.1544 * (M_PI/180.0); // radians
    // inclination
    c->i =  51.6435 * (M_PI/180.0); // radians
    // argument of periapsis
    c->w = 148.6515 * (M_PI/180.0); // radians


    while (agent->running()) {

        cout<<"["<<node_name<<":"<<agent_name<<"] running..."<<endl;

        // get_value of beat
        agent->send_request(agent->find_agent(node_target, agent_target, 2.), "get_value {\"agent[0].beat\"}", response, 2.);
        cout<<"beat = "<<response<<endl;
        response.clear();

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
            if(agent->get_debug_level()>1)	{
                cout<<"B agent "<<agent_target<<" beatstruc:"<<endl;
                cout<<agent_target_heartbeat<<endl;
            }
        }
*/
        // Sleep for 5 sec

        cout<<to_unsigned(agent->agent_list.size()) + " Available Agents..."<<endl;
        for(size_t i = 0; i < agent->agent_list.size(); ++i)	{
            cout<<"Agent_"<<to_unsigned(i)<<" "<<agent->agent_list[i].node<<":"<<agent->agent_list[i].proc<<endl;
        }
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
