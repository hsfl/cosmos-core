// Example of an agent making a request to another agent 
// agent 002 makes request to 002 upon activation

#include "configCosmos.h"
#include "elapsedtime.hpp"
#include "timeutils.hpp"
#include "agentlib.h"

#include <iostream>
#include <string>
using namespace std;


int myagent();
int32_t request_hello(char *request, char* response, void *cdata); // function prototype of agent request

string agentname     = "002";
string nodename      = "cubesat1";

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat
char buf4[512];

beatstruc beat_agent_002;
cosmosstruc *cdata; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffe for agents

int main(int argc, char *argv[])
{
    cout << "Starting agent " << endl;

    int iretn;

    //    // Check for other instance of this agent
    //    if (argc == 2)
    //        agentname = argv[1];


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
