// Example of an agent making a request to another agent 
// agent 002 waits for agent 001 to make a request to it

#include "configCosmos.h"

#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "agentlib.h"
#include "jsonlib.h"

int myagent();

char agentname[100] = "002";
char nodename[100] = "hiakasat";
//char requestname[AGENTMAXNAME] = "001"; //name of the agent that the request is directed to

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1000; // period of heartbeat
beatstruc cbeat;
char buf4[512];

//char* request_002(char *request, char* response); // function prototype of agent request
int32_t request_test(char *request, char* response, void *cdata); // function prototype of agent request
//void yay();

cosmosstruc *cdata; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffe for agents

//#define REQUEST_RUN_PROGRAM 0 // mst check

// Here are some variables we will map to JSON names
int32_t diskfree;
int32_t stateflag;
int32_t myport;

int main(int argc, char *argv[])
{
    int iretn;

    // Check for other instance of this agent
    if (argc == 2)
        strcpy(agentname,argv[1]);

    // check if we are already running the agent
    if (!(cdata = agent_setup_server(AGENT_TYPE_UDP, //AGENT_TYPE_BROADCAST
                                     nodename,
                                     agentname,
                                     waitsec,
                                     0,
                                     AGENTMAXBUFFER))){
        cout<<"agent_002: agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
         cout<<"Starting agent_002 ... OK" << endl;
    }

    // Add additional requests
    if ((iretn=agent_add_request(cdata,"test_request",request_test)))
        exit (iretn);
    
    // Start our own thread
    iretn = myagent();
	
	
}

int myagent()
{
	printf("agent 002...online\n");

    // Start performing the body of the agent
    while(agent_running(cdata))
    {
		usleep(10); // no support in win
		
    }
    return (0);
	
}

int32_t request_test(char *request, char* response, void *cdata)
{
    cout << "agent 002 got request!" << endl;
    sprintf(response,"%f",300.23);
    return 0;
}
