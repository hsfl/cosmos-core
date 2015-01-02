#include "configCosmos.h"

#include <stdio.h>

#include "agentlib.h"

void incoming_thread();
char agentname[COSMOS_MAX_NAME] = "forward";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds

cosmosstruc *cdata;
socket_channel rcvchan;

#define MAXBUFFERSIZE 2560 // comm buffer for agents

int main(int argc, char *argv[])
{
	int32_t iretn;
	socket_channel tempchan;
	vector<socket_channel> sendchan;

	if (argc < 2)
	{
		printf("Usage: agent_forward {ipaddresses1} [{ipaddress2} {ipaddress3} ...]\n");
		exit (1);
	}

	// Initialize the Agent
	if (!(cdata = agent_setup_server(SOCKET_TYPE_UDP,(char *)NULL,(char *)"forward",1.,AGENTRECVPORT,MAXBUFFERSIZE,AGENT_SINGLE)))
	{
		exit (AGENT_ERROR_JSON_CREATE);
	}

	// Open sockets to each address to be used for outgoing forwarding.
	for (uint16_t i=1; i<argc; ++i)
	{
		if ((iretn=socket_open(&tempchan, SOCKET_TYPE_UDP, argv[i], AGENTRECVPORT, AGENT_TALK, AGENT_BLOCKING, AGENTRCVTIMEO)) == 0)
		{
			sendchan.push_back(tempchan);
		}
	}

	// Open the socket for incoming forwarding.
	if ((iretn=socket_open(&rcvchan, SOCKET_TYPE_UDP, "", AGENTRECVPORT, AGENT_LISTEN, AGENT_BLOCKING, AGENTRCVTIMEO)) != 0)
	{
		for (uint16_t i=0; i<sendchan.size(); ++i)
		{
			close(sendchan[i].cudp);
		}
		agent_shutdown_server(cdata);
		printf("Could not open incoming socket for forwarding: %d\n", iretn);
		exit (1);
	}

	// Start thread for incoming forwarding.
	thread thread_incoming;
	thread_incoming = thread(incoming_thread);

	// Start performing the body of the agent
	int nbytes;
	char input[AGENTMAXBUFFER];
	while(agent_running(cdata))
	{

		nbytes = recvfrom(cdata->agent[0].sub.cudp,input,AGENTMAXBUFFER,0,(struct sockaddr *)NULL,(socklen_t *)NULL);
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
	agent_shutdown_server(cdata);
}

void incoming_thread()
{
	//	int32_t iretn;
	int32_t nbytes;
	char input[AGENTMAXBUFFER];

	while(agent_running(cdata))
	{
		if ((nbytes = recvfrom(rcvchan.cudp,input,AGENTMAXBUFFER,0,(struct sockaddr *)NULL,(socklen_t *)NULL)) > 0)
		{
			for (uint16_t i=0; i<cdata->agent[0].ifcnt; ++i)
			{
				sendto(cdata->agent[0].pub[i].cudp,(const char *)input,nbytes,0,(struct sockaddr *)&((cosmosstruc *)cdata)->agent[0].pub[i].caddr,sizeof(struct sockaddr_in));
			}
		}
	}
}
