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

//#include <sys/vfs.h>
#include <stdio.h>

#include "agentlib.h"

int myagent();

char agentname[COSMOS_MAX_NAME] = "example";
char node[50] = "null";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int32_t request_run_program(char *request, char* response, void *cdata); // extra request

cosmosstruc *cdata; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 2560 // comm buffe for agents

#define REQUEST_RUN_PROGRAM 0 // mst check

// Here are some variables we will map to JSON names
int32_t diskfree;
int32_t stateflag;
int32_t myport;

int main(int argc, char *argv[])
{
	int32_t iretn;

	// Check for other instance of this agent
	if (argc == 2)
		strcpy(node,argv[1]);

	// Initialization stuff


	// Initialize the Agent
	// near future: support cubesat space protocol
	// port number = 0 in this case, automatic assignment of port
	if (!(cdata = agent_setup_server(SOCKET_TYPE_UDP,(char *)node,agentname,1.,0,MAXBUFFERSIZE,(bool)true)))
		exit (AGENT_ERROR_JSON_CREATE);

	// Add additional requests
	if ((iretn=agent_add_request(cdata, "runprogram",request_run_program)))
		exit (iretn);

	// Start main thread
#if !defined(COSMOS_WIN_OS)
	FILE *fp;
	struct statfs fsbuf;
#endif
	double nmjd;
	int32_t sleept;

	// Start performing the body of the agent
	nmjd = currentmjd(0.);
	while(agent_running(cdata))
	{
		// Set beginning of next cycle;
		nmjd += cdata[0].agent[0].aprd/86400.;
		// Gather system information
#if !defined(COSMOS_WIN_OS)
		fp = fopen("/proc/meminfo","r");
		fscanf(fp,"MemTotal: %f kB\nMemFree: %f",&cdata[0].devspec.cpu[0]->maxmem,&cdata[0].devspec.cpu[0]->mem);
		fclose(fp);
		fp = fopen("/proc/loadavg","r");
		fscanf(fp,"%f",&cdata[0].devspec.cpu[0]->load);
		fclose(fp);
		statfs("/",&fsbuf);
		cdata[0].devspec.cpu[0]->disk = fsbuf.f_blocks;
		diskfree = fsbuf.f_bfree;
#endif

		sleept = (int32_t)((nmjd - currentmjd(0.))*86400000000.);
		if (sleept < 0) sleept = 0;
		COSMOS_USLEEP(sleept);
	}
	return 0;
}

// the name of this fn will always be changed
int32_t request_run_program(char *request, char* response, void *cdata)
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
		// open process and read response
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
