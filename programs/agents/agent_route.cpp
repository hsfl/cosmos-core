#include "configCosmos.h"

#include <stdio.h>

#include "agentlib.h"

char agentname[COSMOS_MAX_NAME] = "route";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds

cosmosstruc *cdata;
socket_channel rcvchan;

#define MAXBUFFERSIZE 2560 // comm buffer for agents

int main(int argc, char *argv[])
{
	// Initialize the Agent
	if (!(cdata = agent_setup_server(SOCKET_TYPE_UDP,(char *)NULL,(char *)"route",1.,0,MAXBUFFERSIZE,AGENT_SINGLE)))
	{
		exit (AGENT_ERROR_JSON_CREATE);
	}

	// Preload the information about the interfaces
	uint32_t addr_out[AGENTMAXIF];
//	uint32_t addr_to[AGENTMAXIF];
	for (uint16_t i=0; i<cdata->agent[0].ifcnt; ++i)
	{
#ifdef COSMOS_WIN_OS
		addr_out[i] = uint32from((uint8_t *)&cdata->agent[0].pub[i].caddr.sin_addr.S_un.S_addr, ORDER_NETWORK);
//		addr_to[i] = uint32from((uint8_t *)&cdata->agent[0].pub[i].baddr.sin_addr.S_un.S_addr, ORDER_NETWORK);
#else
		addr_out[i] = uint32from((uint8_t *)&cdata->agent[0].pub[i].caddr.sin_addr.s_addr, ORDER_NETWORK);
//		addr_to[i] = uint32from((uint8_t *)&cdata->agent[0].pub[i].baddr.sin_addr.s_addr, ORDER_NETWORK);
#endif
	}

	// Start performing the body of the agent
	int nbytes;
	char input[AGENTMAXBUFFER];
	while(agent_running(cdata))
	{
		nbytes = recvfrom(cdata->agent[0].sub.cudp,input,AGENTMAXBUFFER,0,(struct sockaddr *)&cdata->agent[0].sub.caddr,(socklen_t *)&cdata->agent[0].sub.addrlen);
		if (nbytes > 0)
		{
#ifdef COSMOS_WIN_OS
			uint32_t addr_in = uint32from((uint8_t *)&cdata->agent[0].sub.caddr.sin_addr.S_un.S_addr, ORDER_NETWORK);
#else
			uint32_t addr_in = uint32from((uint8_t *)&cdata->agent[0].sub.caddr.sin_addr.s_addr, ORDER_NETWORK);
#endif
			bool forward=true;
			for (uint16_t i=0; i<cdata->agent[0].ifcnt; ++i)
			{
				if (addr_in == addr_out[i])
				{
					forward = false;
					break;
				}
			}

			if (forward)
			{
				for (uint16_t i=0; i<cdata->agent[0].ifcnt; ++i)
				{
					uint32_t address_xor = addr_in ^ addr_out[i];
					if (address_xor > 255)
					{
//						printf("%x:%x:%x %d\n%s\n", addr_in, addr_out[i], addr_to[i], address_xor, input);
						sendto(cdata->agent[0].pub[i].cudp,(const char *)input,nbytes,0,(struct sockaddr *)&cdata->agent[0].pub[i].baddr,sizeof(struct sockaddr_in));
//						fflush(stdout);
					}
				}
			}
		}
	}

	agent_shutdown_server(cdata);
}

