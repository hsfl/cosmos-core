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

#include "configCosmos.h"
//#include "elapsedtime.hpp"
#include "timeutils.hpp"
#include "agentlib.h"

#include <iostream>
#include <string>
using namespace std;


int myagent();
int32_t request_hello(char *request, char* response, void *cdata); // function prototype of agent request

string agentname     = "002";
string nodename      = "telem";
string cosmosPath   = "C:/COSMOS/"; // change this to where your COSMOS folder is

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat
char buf4[512];

beatstruc beat_agent_002;
cosmosstruc *cdata; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffe for agents

int main(int argc, char *argv[])
{
    setEnvCosmos(cosmosPath);

    cout << "Starting agent " << endl;

    int iretn;

    // Establish the command channel and heartbeat
    if (!(cdata = agent_setup_server(AGENT_TYPE_UDP,
                                     nodename.c_str(),
                                     agentname.c_str(),
                                     1.0,
                                     0,
                                     AGENTMAXBUFFER)))
    {
        cout << agentname << " : agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
        cout<<"Starting " << agentname << " ... OK" << endl;
    }
    // Add additional requests
    if ((iretn=agent_add_request(cdata,"request_hello",request_hello)))
        exit (iretn);
    
    // Start our own thread
    iretn = myagent();

    return 0;
}

int myagent()
{
    cout << "agent 002...online " << endl;

    string requestString = "request_hello";
    char response[300];

    // Start executing the agent
    while(agent_running(cdata))
    {
        COSMOS_SLEEP(1.00);
    }
    return (0);

}


int32_t request_hello(char *request, char* response, void *cdata)
{

    //    sprintf(response,"%f",300.23);
    sprintf(response,"hello");

    cout << "agent 002 got request! response is: " << response << endl;

    return 0;
}
