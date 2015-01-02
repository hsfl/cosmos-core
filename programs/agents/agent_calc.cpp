/*! \file agent_calc.cpp
* \brief Demonstration calculator Agent
*/

//! \ingroup agents
//! \defgroup agent_calc Calculation Agent
//! This program accepts requests to perform simple math operations and returns the result.
//! With it you can:
//! - add
//! - subtract
//! - multiply
//! - divide

// code from testbed/blank_agent.cpp

#include "configCosmos.h"

#include <stdio.h>
#include <iostream>

using namespace std;

#include "agentlib.h"
#include "physicslib.h" // long term we may move this away
#include "jsonlib.h"
#include "jsonlib.h"	// need this for node_init

int myagent();

//do we need a NODEMAXNAME? using COSMOS_MAX_NAME for now...
char nodename[COSMOS_MAX_NAME] = "otb";
char agentname[COSMOS_MAX_NAME] = "calc";

int waitsec = 5; // wait to find other agents of your 'type/name', seconds

//int32_t *request_run_program(char *request, char* response, void *cdata); // extra request
int32_t request_add(char *request, char* response, void *cdata);
int32_t request_sub(char *request, char* response, void *cdata);
int32_t request_mul(char *request, char* response, void *cdata);
int32_t request_div(char *request, char* response, void *cdata);

int32_t request_change_node_name(char *request, char* response, void *cdata);


#define MAXBUFFERSIZE 100000 // comm buffer for agents

cosmosstruc *cosmos_data; // to access the cosmos data, will change later

int main(int argc, char *argv[])
{
    int irtn;

    // process arguments if present

    // make node_name = 1st argument
    if (argc == 2)
        strcpy(nodename,argv[1]);
    // make agent_proc = 2st argument
    if (argc == 3)	{
        strcpy(agentname,argv[2]);
        strcpy(nodename,argv[1]);
    }

    // Initialization stuff
    if (argc > 1)
    {
        if (!(cosmos_data=agent_setup_server(SOCKET_TYPE_UDP,nodename,agentname,1.,0,MAXBUFFERSIZE,(bool)false)))
        {
            printf("Failed to open [%s:%s]\n",nodename,agentname);
            exit (1);
        }
        cout<<"Hello, I am an agent. My name is ["<<nodename<<":"<<agentname<<"]"<<endl<<endl;
    }
    else
    {
        if (!(cosmos_data=agent_setup_server(SOCKET_TYPE_UDP,NULL,agentname,1.,0,MAXBUFFERSIZE,(bool)false)))
        {
            printf("Failed to open [null:%s]\n",agentname);
            exit (1);
        }
        cout<<"Hello, I am an agent. My name is [null:"<<agentname<<"]"<<endl<<endl;
    }


    for (uint16_t i=0; i<cosmos_data->jmap.size(); ++i)
    {
        if (cosmos_data->jmap[i].size())
        {
            cout<<"jmap["<<i<<"]:"<<cosmos_data->jmap[i][0].name<<endl;
        }
    }
    cout<<cosmos_data->node.name<<endl;

    jstring js = {0,0,0};
    jstring *jsp = &js;
    json_startout(jsp);
    json_out_name(jsp,(char *)"node_name");
    json_stopout(jsp);
    cout<<jsp->string<<endl;


    cout<<"address of cosmos_data is = "<<cosmos_data<<endl;
    // Add additional requests
    if ((irtn=agent_add_request(cosmos_data,"add",request_add)))
        exit (irtn);

    if ((irtn=agent_add_request(cosmos_data,"sub",request_sub)))
        exit (irtn);

    if ((irtn=agent_add_request(cosmos_data,"mul",request_mul)))
        exit (irtn);

    if ((irtn=agent_add_request(cosmos_data,"div",request_div)))
        exit (irtn);

    if ((irtn=agent_add_request(cosmos_data,"node",request_change_node_name)))
        exit (irtn);

    // Start our own thread
    irtn = myagent();
}

int myagent()
{

    // Start performing the body of the agent
    while(agent_running(cosmos_data))
    {
        COSMOS_SLEEP(0.1); // no support in win
    }
    return 0;
}

// the name of this fn will always be changed
int32_t request_add(char *request, char* response, void *cdata)
{
    float a,b;
    sscanf(request,"%*s %f %f",&a,&b);
    sprintf(response,"%f",a+b);
    return 0;
}

// the name of this fn will always be changed
int32_t request_sub(char *request, char* response, void *cdata)
{
    float a,b;
    sscanf(request,"%*s %f %f",&a,&b);
    sprintf(response,"%f",a-b);
    return 0;
}
// the name of this fn will always be changed
int32_t request_mul(char *request, char* response, void *cdata)
{
    float a,b;
    sscanf(request,"%*s %f %f",&a,&b);
    sprintf(response,"%f",a*b);
    return 0;
}
// the name of this fn will always be changed
int32_t request_div(char *request, char* response, void *cdata)
{
    float a,b;
    sscanf(request,"%*s %f %f",&a,&b);
    sprintf(response,"%f",a/b);
    return 0;
}

int32_t request_change_node_name(char *request, char* response, void *cdata)
{
    char new_name[40];
    sscanf(request,"%*s %s", new_name);

    cout<<"The new name is <"<<new_name<<">"<<endl;
    jstring js = {0,0,0};
    jstring *jsp = &js;
    sprintf(jsp->string,"%s",new_name);
    //sprintf(jsp->string,"{\"node_name\":\"%s\"}",new_name);

    cout<<"The jstring is <"<<jsp->string<<">"<<endl;
    json_parse(jsp->string,cosmos_data);
    cout<<"The new node name is <"<< cosmos_data->node.name <<">"<<endl;

    return 0;
}

