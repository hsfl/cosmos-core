// Example of an agent making a request to another agent 
// agent 001 makes request to 002 upon activation

#include <iostream>
#include <cstring>
using namespace std;

#include "configCosmos.h"

#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "agentlib.h"
#include "jsonlib.h"

int myagent();

char agentname[100]     = "001";
char nodename[100]      = "cubesat1";
char requestname[100]   = "002"; //name of the agent that the request is directed to

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat

beatstruc beat_agent_002;

char buf4[512];

cosmosstruc *cdata; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffe for agents

//#define REQUEST_RUN_PROGRAM 0 // mst check

// Here are some variables we will map to JSON names
//int32_t diskfree;
//int32_t stateflag;
//int32_t myport;

int main(int argc, char *argv[])
{
    int iretn;

    // Check for other instance of this agent
    if (argc == 2)
        strcpy(agentname,argv[1]);

    // Establish the command channel and heartbeat
    if (!(cdata = agent_setup_server(AGENT_TYPE_UDP,
                                     nodename,
                                     agentname,
                                     1.0,
                                     0,
                                     AGENTMAXBUFFER)))
    {
        cout<<"agent_001: agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
         cout<<"Starting agent_001 ... OK" << endl;
    }

    //agent_get_server(nodename,requestname,8,&cbeat);

    beat_agent_002 = agent_find_server(cdata, nodename, "002", 5.);

    // Add additional requests
    //if ((iretn=agent_add_request("tcu_reset",request_reset)))
    //    exit (iretn);
    
    // Start our own thread
    iretn = myagent();

    return 0;
}

int myagent()
{
    cout << "agent 001...online " << endl;

    string requestString = "test_request";
    char response[300];
    int32_t err=0;

    // Start executing the agent
    while(agent_running(cdata))
    {
        // makes a request to agent 002
        err = agent_send_request(cdata,
                           beat_agent_002,
                           requestString.c_str(),
                           response,
                           512,
                           2);


        cout << "Received from agent_002: " << err << " : " << response << endl;
        COSMOS_SLEEP(1);
		
    }
    return (0);
	
}

