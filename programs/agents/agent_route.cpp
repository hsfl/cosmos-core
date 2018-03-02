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

#include <stdio.h>

#include "agent/agentclass.h"

char agentname[COSMOS_MAX_NAME+1] = "route";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds

Agent *agent;
socket_channel rcvchan;

#define MAXBUFFERSIZE 2560 // comm buffer for agents

int main(int argc, char *argv[])
{
	// Initialize the Agent
    if (!(agent = new Agent("", "route", 1., MAXBUFFERSIZE)))
	{
		exit (AGENT_ERROR_JSON_CREATE);
	}

	// Preload the information about the interfaces
	uint32_t addr_out[AGENTMAXIF];
//	uint32_t addr_to[AGENTMAXIF];
    for (uint16_t i=0; i<agent->cinfo->agent[0].ifcnt; ++i)
	{
#ifdef COSMOS_WIN_OS
        addr_out[i] = uint32from((uint8_t *)&agent->cinfo->agent[0].pub[i].caddr.sin_addr.S_un.S_addr, ByteOrder::NETWORK);
//		addr_to[i] = uint32from((uint8_t *)&agent->cinfo->agent[0].pub[i].baddr.sin_addr.S_un.S_addr, ByteOrder::NETWORK);
#else
        addr_out[i] = uint32from((uint8_t *)&agent->cinfo->agent[0].pub[i].caddr.sin_addr.s_addr, ByteOrder::NETWORK);
//		addr_to[i] = uint32from((uint8_t *)&agent->cinfo->agent[0].pub[i].baddr.sin_addr.s_addr, ByteOrder::NETWORK);
#endif
	}

	// Start performing the body of the agent
	int nbytes;
	char input[AGENTMAXBUFFER];
    while(agent->running())
	{
        nbytes = recvfrom(agent->cinfo->agent[0].sub.cudp,input,AGENTMAXBUFFER,0,(struct sockaddr *)&agent->cinfo->agent[0].sub.caddr,(socklen_t *)&agent->cinfo->agent[0].sub.addrlen);
		if (nbytes > 0)
		{
#ifdef COSMOS_WIN_OS
            uint32_t addr_in = uint32from((uint8_t *)&agent->cinfo->agent[0].sub.caddr.sin_addr.S_un.S_addr, ByteOrder::NETWORK);
#else
            uint32_t addr_in = uint32from((uint8_t *)&agent->cinfo->agent[0].sub.caddr.sin_addr.s_addr, ByteOrder::NETWORK);
#endif
			bool forward=true;
            for (uint16_t i=0; i<agent->cinfo->agent[0].ifcnt; ++i)
			{
				if (addr_in == addr_out[i])
				{
					forward = false;
					break;
				}
			}

			if (forward)
			{
                for (uint16_t i=0; i<agent->cinfo->agent[0].ifcnt; ++i)
				{
					uint32_t address_xor = addr_in ^ addr_out[i];
					if (address_xor > 255)
					{
//						printf("%x:%x:%x %d\n%s\n", addr_in, addr_out[i], addr_to[i], address_xor, input);
                        sendto(agent->cinfo->agent[0].pub[i].cudp,(const char *)input,nbytes,0,(struct sockaddr *)&agent->cinfo->agent[0].pub[i].baddr,sizeof(struct sockaddr_in));
//						fflush(stdout);
					}
				}
			}
		}
	}

    agent->shutdown();
}

