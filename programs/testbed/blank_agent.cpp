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

#ifdef COSMOS_MAC_OS
#include <sys/param.h>
#include <sys/mount.h>
#elif !defined(COSMOS_WIN_OS)
#include <sys/vfs.h>
#endif // COSMOS_MAC_OS

#include <stdio.h>

#include "agent/agentclass.h"
#include "physics/physicslib.h"
#include "support/jsonlib.h"
//#include "support/stringlib.h"
//#include "support/timelib.h"

int myagent();

char agentname[COSMOS_MAX_NAME+1] = "blank";
char ipaddress[16] = "192.168.150.1";
int waitsec = 5;
int32_t request_run_program(char *request, char* response, Agent *);

Agent *agent;

#define MAXBUFFERSIZE 256

#define REQUEST_RUN_PROGRAM 0

// Here are some variables we will map to JSON names
int32_t diskfree;
int32_t stateflag;
int32_t myport;

int main(int argc, char *argv[])
{
int32_t iretn;

// Check for other instance of this agent
if (argc == 2)
	strcpy(agentname,argv[1]);

if ((iretn=agent->get_server(NULL,agentname,waitsec,(beatstruc *)NULL)) > 0)
	exit (iretn);

// Initialization stuff


// Initialize the Agent
if (!(agent = new Agent("", agentname, .1, MAXBUFFERSIZE)))
	exit (iretn);

// Add additional requests
if ((iretn=agent->add_request("runprogram",request_run_program)))
	exit (iretn);

// Start our own thread
iretn = myagent();
}

int myagent()
{

// Start performing the body of the agent
while(agent->running())
	{


	COSMOS_USLEEP(10);
	}
return 0;
}

int32_t request_run_program(char *request, char* response, Agent *)
{
int i;
int32_t iretn = 0;
FILE *pd;
bool flag;

// Run Program
flag = false;
for (i=0; i<AGENTMAXBUFFER-1; i++)
	{
	if (flag)
		{
		if (request[i] != ' ')
			break;
		}
	else
		{
		if (request[i] == ' ')
			flag = true;
		}
	}

if (i == AGENTMAXBUFFER-1)
	{
	sprintf(response,"unmatched");
	}
else
	{
#ifdef COSMOS_WIN_BUILD_MSVC
		if ((pd=_popen(&request[i], "r")) != NULL)
#else
		if ((pd=popen(&request[i],"r")) != NULL)
#endif
		{
		iretn = fread(response,1,AGENTMAXBUFFER-1,pd);
		response[iretn] = 0;
		iretn = 1;
#ifdef COSMOS_WIN_BUILD_MSVC
			_pclose(pd);
#else
			pclose(pd); // close process
#endif
		}
	else
		{
		response[0] = 0;
		iretn = 0;
		}
	}

return (iretn);
}
