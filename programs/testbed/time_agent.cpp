#include "configCosmos.h"
#include <stdio.h>

#include "agentlib.h"
#include "cosmos-defs.h"
#include "physicslib.h"
#include "jsonlib.h"

int myagent();
int32_t request_mjd(char *request, char* response, void *cdata);

char agentname[COSMOS_MAX_NAME] = "time";
char ipaddress[16] = "192.168.150.1";
int waitsec = 5;
cosmosstruc *cdata;


#define MAXBUFFERSIZE 256

#define REQUEST_RUN_PROGRAM 0

// Here are some variables we will map to JSON names
int32_t pid;
int32_t memtotal, memfree;
int32_t disktotal, diskfree;
double load;
int32_t stateflag;
uint16_t myport;

int main(int argc, char *argv[])
{
	int32_t iretn;

	// Initialization stuff

	// Initialize Agent
	if (!(cdata = agent_setup_server(SOCKET_TYPE_MULTICAST,argv[1],agentname,.1,0,MAXBUFFERSIZE)) != 0)
		exit (AGENT_ERROR_JSON_CREATE);

	// Add internal requests
	if ((iretn=agent_add_request(cdata, (char *)"mjd",request_mjd)) != 0)
		exit (iretn);

	// Start our own thread
	iretn = myagent();
}

int myagent()
{
	double cmjd, nmjd, period;
	struct timeval mytime;
	unsigned long usec;
	string jstring;

	// Initialize loop timing
	period = .1/86400.;
	cmjd = currentmjd();
	nmjd = cmjd + period;

	// Start performing the body of the agent
	while(agent_running(cdata))
	{
		// Calculate time and publish it
		cdata[0].node.loc.utc = currentmjd(cdata[0].node.utcoffset);
		agent_post(cdata, AGENT_MESSAGE_TIME,json_of_time(jstring, cdata));

		cmjd = currentmjd();
		if (nmjd > cmjd)
		{
			usec = (unsigned long)((nmjd-cmjd)*86400*1e6+.5);
			COSMOS_USLEEP(usec);
		}
		nmjd += period;
	}
	return 0;
}

int32_t request_mjd(char *request, char* output, void *cdata)
{

	sprintf(output,"%f",((cosmosstruc *)cdata)->node.loc.utc);

	return 0;
}
