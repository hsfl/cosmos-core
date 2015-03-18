// Example of an agent making a request to another agent
// agent 001 makes request to 002 upon activation

#include "configCosmos.h"
#include "elapsedtime.hpp"
#include "timeutils.hpp"
#include "agentlib.h"

#include <iostream>
#include <string>
using namespace std;


int myagent();

string agentname     = "001";
string nodename      = "cubesat1";
string requestname   = "002"; //name of the agent that the request is directed to

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


    //    TimeUtils time;
    //    while(1){

    //        cout << time.secondsSinceEpoch() << endl;
    //        time.testSecondsSinceMidnight();
    //        COSMOS_SLEEP(1);

    //    }



    // Establish the command channel and heartbeat
    if (!(cdata = agent_setup_server(AGENT_TYPE_UDP,
                                     nodename.c_str(),
                                     agentname.c_str(),
                                     1.0,
                                     0,
                                     AGENTMAXBUFFER)))
    {
        cout << agentname << ": agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
        cout<<"Starting " << agentname << " ... OK" << endl;
        //        cdata->agent[0].sub
    }

    //agent_get_server(nodename,requestname,8,&cbeat);

    beat_agent_002 = agent_find_server(cdata, nodename, "002", 5.);

    cout << "beat agent 002 node: " << beat_agent_002.node << endl;

    // Add additional requests
    //if ((iretn=agent_add_request("tcu_reset",request_reset)))
    //    exit (iretn);

    // Start our own thread
    iretn = myagent();

    return 0;
}

int myagent()
{
    cout << "agent " << agentname <<  " ...online " << endl;

    string requestString = "request_hello";
    //    string requestString = "status";
    char response_c_str[300];
    string response;

    // Start executing the agent
    while(agent_running(cdata))
    {
        // makes a request to agent 002
        int iretn = agent_send_request(cdata,
                                       beat_agent_002,
                                       requestString.c_str(),
                                       response_c_str, // convert to C++ string
                                       512,
                                       2);
        response = string(response_c_str);

        //        cout << "return:" << iretn << endl;
        if ( response.size() > 1){
            cout << "Received from agent_002: " << response.size() << " bytes : " << response << endl;
            response = "";
        } else {
            cout << "What happened to agent_002 ??? " << endl;
            beat_agent_002.node[0] = '\0'; // reset
            beat_agent_002 = agent_find_server(cdata, nodename, "002", 5.);
            cout << "beat agent 002 node: " << beat_agent_002.utc << endl;
        }
        COSMOS_SLEEP(1);

    }
    return (0);

}
