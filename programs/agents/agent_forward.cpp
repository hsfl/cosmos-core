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

#include "configCosmos.h"

#include <stdio.h>

#include "agent/agentclass.h"

void incoming_thread();
char agentname[COSMOS_MAX_NAME+1] = "forward";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds

CosmosAgent *agent;
socket_channel rcvchan;

#define MAXBUFFERSIZE 2560 // comm buffer for agents

int main(int argc, char *argv[])
{
	int32_t iretn;
	socket_channel tempchan;
	std::vector<socket_channel> sendchan;

	if (argc < 2)
	{
		printf("Usage: agent_forward {ipaddresses1} [{ipaddress2} {ipaddress3} ...]\n");
		exit (1);
	}

	// Initialize the Agent
    if (!(agent = new CosmosAgent(NetworkType::UDP, "", "forward", 5., MAXBUFFERSIZE, false, AGENTRECVPORT)))
	{
		exit (AGENT_ERROR_JSON_CREATE);
	}

	// Open sockets to each address to be used for outgoing forwarding.
	for (uint16_t i=1; i<argc; ++i)
	{
		if ((iretn=socket_open(&tempchan, NetworkType::UDP, argv[i], AGENTRECVPORT, AGENT_TALK, AGENT_BLOCKING, AGENTRCVTIMEO)) == 0)
		{
			sendchan.push_back(tempchan);
		}
	}

	// Open the socket for incoming forwarding.
	if ((iretn=socket_open(&rcvchan, NetworkType::UDP, "", AGENTRECVPORT, AGENT_LISTEN, AGENT_BLOCKING, AGENTRCVTIMEO)) != 0)
	{
		for (uint16_t i=0; i<sendchan.size(); ++i)
		{
			close(sendchan[i].cudp);
		}
        agent->shutdown();
		printf("Could not open incoming socket for forwarding: %d\n", iretn);
		exit (1);
	}

	// Start thread for incoming forwarding.
	std::thread thread_incoming;
	thread_incoming = std::thread(incoming_thread);

	// Start performing the body of the agent
	int nbytes;
	char input[AGENTMAXBUFFER];
    while(agent->running())
	{

        nbytes = recvfrom(agent->cinfo->pdata.agent[0].sub.cudp,input,AGENTMAXBUFFER,0,(struct sockaddr *)NULL,(socklen_t *)NULL);
		for (uint16_t i=0; i<sendchan.size(); ++i)
		{
			iretn = sendto(sendchan[i].cudp,(const char *)input,nbytes,0,(struct sockaddr *)&sendchan[i].caddr,sizeof(struct sockaddr_in));
		}
	}

	thread_incoming.join();
	for (uint16_t i=0; i<sendchan.size(); ++i)
	{
		close(sendchan[i].cudp);
	}
    agent->shutdown();
}

void incoming_thread()
{
	//	int32_t iretn;
	int32_t nbytes;
	char input[AGENTMAXBUFFER];

    while(agent->running())
	{
		if ((nbytes = recvfrom(rcvchan.cudp,input,AGENTMAXBUFFER,0,(struct sockaddr *)NULL,(socklen_t *)NULL)) > 0)
		{
            for (size_t i=0; i<agent->cinfo->pdata.agent[0].ifcnt; ++i)
			{
                sendto(agent->cinfo->pdata.agent[0].pub[i].cudp,(const char *)input,nbytes,0,(struct sockaddr *)&agent->cinfo->pdata.agent[0].pub[i].caddr,sizeof(struct sockaddr_in));
			}
		}
	}
}
