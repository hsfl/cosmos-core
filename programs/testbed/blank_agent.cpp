#include "configCosmos.h"

#ifdef COSMOS_MAC_OS
#include <sys/param.h>
#include <sys/mount.h>
#elif !defined(COSMOS_WIN_OS)
#include <sys/vfs.h>
#endif // COSMOS_MAC_OS

#include <stdio.h>

#include "agentlib.h"
#include "physicslib.h"
#include "jsonlib.h"
//#include "stringlib.h"
//#include "timelib.h"

int myagent();

char agentname[COSMOS_MAX_NAME] = "blank";
char ipaddress[16] = "192.168.150.1";
int waitsec = 5;
int32_t request_run_program(char *request, char* response, void *cdata);

cosmosstruc *cdata;

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

if ((iretn=agent_get_server(cdata, NULL,agentname,waitsec,(beatstruc *)NULL)) > 0)
	exit (iretn);

// Initialization stuff


// Initialize the Agent
if (!(cdata = agent_setup_server(AGENT_TYPE_BROADCAST,nullptr,agentname,.1,0,MAXBUFFERSIZE)))
	exit (iretn);

// Add additional requests
if ((iretn=agent_add_request(cdata, "runprogram",request_run_program)))
	exit (iretn);

// Start our own thread
iretn = myagent();
}

int myagent()
{

// Start performing the body of the agent
while(agent_running(cdata))
	{


	COSMOS_USLEEP(10);
	}
return 0;
}

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
	if ((pd=popen(&request[i],"r")) != NULL)
		{
		iretn = fread(response,1,AGENTMAXBUFFER-1,pd);
		response[iretn] = 0;
		iretn = 1;
		pclose(pd);
		}
	else
		{
		response[0] = 0;
		iretn = 0;
		}
	}

return (iretn);
}
