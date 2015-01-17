#include <stdlib.h>
#include "agentlib.h"
#include "jsonlib.h"
#include "jsonlib.h"

char request[AGENTMAXBUFFER], output[AGENTMAXBUFFER];
cosmosstruc *cdata;

int main(int argc, char *argv[])
{
int nbytes, bus=0, state=0;
beatstruc cbeat;

switch (argc)
	{
case 4:
	state = atol(argv[3]);
case 3:
	bus = atol(argv[2]);
case 2:
	node_init(argv[1],cdata);
	break;
default:
	printf("Usage: power_switch node{ bus {state}}\n");
	exit (1);
	break;
	}

if ((nbytes = agent_get_server(cdata, cdata[0].node.name,(char *)"engine",8,&cbeat)) > 0)
	{
	sprintf(request,"set_bus %d %d",bus,state);
	nbytes = agent_send_request(cdata, cbeat,request,output,AGENTMAXBUFFER,5);
	printf("%s [%d]\n",output,nbytes);
	}
else
	{
	if (!nbytes)
		printf("%s not found\n",argv[1]);
	else
		printf("Error: %d\n",nbytes);
	}

}
