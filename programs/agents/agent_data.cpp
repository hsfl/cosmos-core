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

// code from testbed/blank_agent.cpp

#include "configCosmos.h"

#include <sys/stat.h>

#include <stdio.h>

#include "agent/agentlib.h"
#include "physics/physicslib.h" // long term we may move this away
#include "jsonlib.h"
#include "datalib.h"
#include <cstring>
#include <fstream>
#include <iostream>
//#include "stringlib.h"
//#include "timelib.h"

int myagent();

std::vector<agentstruc> agent;
agentstruc tempagent;

//what else?

std::vector<cosmosstruc> nodes;
char tempname[100];
char agentname[COSMOS_MAX_NAME+1] = "data";
std::string dataDir;
int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int32_t request_login(char *request, char* response, void *cdata);
int32_t request_getnodelist(char *request, char* response, void *cdata);
int32_t request_log(char *request, char* response, void *cdata);

cosmosstruc *cdata; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffe for agents

#define REQUEST_RUN_PROGRAM 0 // mst check

int usercount=0;
char username[1000][50];        //move up when finished
char password[1000][50];

// Here are some variables we will map to JSON names
int32_t diskfree;
int32_t stateflag;
int32_t myport;

int main(int argc, char *argv[])
{
int32_t iretn;
uint32_t i;
char input[100];
FILE *fd;

// Setting the directory where the data is
if (set_cosmosnodes(dataDir) < 0)
{
	printf("Couldn't find Nodes directory\n");
	exit (1);
}

// check if we are already running the agent
if ((iretn=agent_get_server(cdata, (char *)"hmoc",agentname,waitsec,(beatstruc *)NULL)) > 0)
	exit (iretn);

// Load user list
usercount = 0;
sprintf(input,"%s/login.dat",dataDir.c_str());

if ((fd = fopen(input,"r")) != NULL)
	{
	while(!feof(fd))
		{//read strings into array
		if (fgets(input,100,fd) != NULL)
			{
			sscanf(input, "%s %s",username[usercount],password[usercount]);
			usercount++;
			}
		}
	fclose(fd);
	}

// Load Node list
data_get_nodes(nodes);

for (i=0; i<nodes.size(); ++i)
	{
#if defined(COSMOS_WIN_OS)
	char command_line[100];
	sprintf(command_line, "agent_node %s", (const char*)nodes[i].node.name);

    STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

#if defined(COSMOS_WIN_BUILD_MSVC)
        if (CreateProcess(NULL, (LPWSTR) command_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
#else
        if (CreateProcessA(NULL, (LPSTR) command_line, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
#endif
	{
		strcpy(tempagent.beat.node,nodes[i].node.name);
		tempagent.pid = pi.dwProcessId;
		agent.push_back(tempagent);
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );
	}
#else
	int32_t pid =fork();
	switch(pid)
	{
	case -1:
		strcpy(tempagent.beat.node,nodes[i].node.name);
		tempagent.pid=pid;
		agent.push_back(tempagent);
		break;
	case 0:
		int devn;
		devn = open("/dev/null",O_RDWR);
		dup2(STDIN_FILENO, devn);
		dup2(STDOUT_FILENO, devn);
		dup2(STDERR_FILENO,	devn);
		close(devn);
		execl("agent_node",(const char*)nodes[i].node.name,(char*) NULL);

		fflush(stdout);
		exit(0);
		break;
	default:
		break;	
		}
#endif
	}
// Initialize the Agent
// near future: support cubesat space protocol
// port number = 0 in this case, automatic assignment of port
if (!(cdata = agent_setup_server(NetworkType::UDP,(char *)"hmoc",agentname,1.,0,MAXBUFFERSIZE)))
	exit (iretn);

// Add additional requests
if ((iretn=agent_add_request(cdata, "login",request_login)))
	exit (iretn);
if ((iretn=agent_add_request(cdata, "getnodelist",request_getnodelist)))
	exit (iretn);
if ((iretn=agent_add_request(cdata, "log",request_log)))
	exit (iretn);

// Start our own thread
iretn = myagent();
}

int myagent()
{

// Start performing the body of the agent
while(agent_running(cdata))
	{
	// Gather system information


	COSMOS_USLEEP(10); // no support in win
	}
return 0;
}

//Verify Login Info
int32_t request_login(char *request, char* response, void *)
{
char user[COSMOS_MAX_NAME+1];
char pass[COSMOS_MAX_NAME+1];
int i;
bool login;

login=false;
user[0] = 0;	// These need to be reset, otherwise, logging in with no name lets these retain the previous users name/password.
pass[0] = 0;

sscanf(request,"%*s %s %s",user,pass);

for(i=0;i<usercount;i++)
	{   //match given data against what is stored in the array
	if((strcmp(username[i],user)==0) && strcmp(password[i],pass)==0){
		login=true;
		break;
		}
	}

if (login==true)
	strcpy(response,"yes");
else
	strcpy(response,"no");
return 0;
}

// the name of this fn will always be changed
int32_t request_getnodelist(char *, char* response, void *)
{
uint32_t i;

response[0] = 0;
for (i=0; i<nodes.size(); i++)
	{
	sprintf(&response[strlen(response)],"%s,%d,",nodes[i].node.name,nodes[i].node.type);
	}
return 0;
}

//Takes event and stores to log
int32_t request_log(char *request, char* , void *cdata)
{
FILE *log;
uint16_t i;
double utc;

for (i=0; i<strlen(request); ++i)
{
	if (request[i] == '{')
		break;
}

json_parse(&request[i],(cosmosstruc *) cdata); //cdata.stat.agent.user
													// .event
													// .utc ->date
													// .node
utc = (int)(((cosmosstruc *)cdata)->event[0].s.utc);
log = data_open(data_type_path(((cosmosstruc *)cdata)->event[0].s.node, (char *)"data", (char *)"outgoing", utc, (char*)"request_log"), (char*)"a");
fprintf(log,"%s\n",&request[i]);
fclose(log);
return 0;
}

//Request info
/*int32_t request_pid(char *request, char*response, void *cdata)
//{

}*/

//Shutdown all running??


