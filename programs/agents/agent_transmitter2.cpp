// code from testbed/blank_agent.cpp

#include "configCosmos.h"

//#ifdef COSMOS_MAC_OS
//#include <sys/param.h>
//#include <sys/mount.h>
//#else
//#include <sys/vfs.h>
//#endif // COSMOS_MAC_OS

#include <stdio.h>

#include "agentlib.h"
#include "physicslib.h" // long term we may move this away
#include "jsonlib.h"
//#include "stringlib.h"
//#include "timelib.h"

// Added 20130223JC: Libraries
#include <cstring>
#include "kisslib.h"
#include "rs232_lib.h"
#define SERIAL_USB0 16
#define SERIAL_USB1 17

int myagent();

char agentname[COSMOS_MAX_NAME] = "gs_tx2";
char node[50] = "otb";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int32_t transmit_kiss(char *request, char* response, void *cdata); // extra request

cosmosstruc *cdata; // to access the cosmos data, will change later

#define MAXBUFFERSIZE 256 // comm buffe for agents

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

	// check if we are already running the agent
	if ((iretn=agent_get_server(cdata, (char *)node,agentname,waitsec,(beatstruc *)NULL)) > 0)
		exit (iretn);

	// Initialization stuff

	// RS232 Library Initialization
	RS232_OpenComport(SERIAL_USB1,19200); // 16=ttyUSB0, 19200bps


	// Initialize the Agent
	// near future: support cubesat space protocol
	// port number = 0 in this case, automatic assignment of port
	if (!(cdata = agent_setup_server(SOCKET_TYPE_UDP,(char *)node,agentname,1.,0,MAXBUFFERSIZE)))
		exit (iretn);

	// Add additional requests
	if ((iretn=agent_add_request(cdata, "tx",transmit_kiss)))
		exit (iretn);

	// Start our own thread
	iretn = myagent();
}

int myagent()
{
	//FILE *fp;

	// Start performing the body of the agent
	while(agent_running(cdata))
	{
		unsigned int rxcount;
		unsigned char serial_input[1000];

		// Read Serial Port(s)
		rxcount = RS232_PollComport(SERIAL_USB1, serial_input, 1000);

		// Print Inbound Characters if New Input Found
		if (rxcount)
		{
			printf("RX: ");
			for(uint32_t i=0;i < rxcount;i++)
			{
				printf("%c",serial_input[i]);
			}
			printf("\n");
		}


		// Gather system information
#ifdef COSMOS_LINUX_OS
		FILE *fp;
		fp = fopen("/proc/meminfo","r");
		fscanf(fp,"MemTotal: %f kB\nMemFree: %f",&cdata[0].devspec.cpu[0]->maxmem,&cdata[0].devspec.cpu[0]->mem);
		fclose(fp);
		fp = fopen("/proc/loadavg","r");
		fscanf(fp,"%f",&cdata[0].devspec.cpu[0]->load);
		fclose(fp);
		struct statfs fsbuf;
		statfs("/",&fsbuf);
		cdata[0].devspec.cpu[0]->disk = fsbuf.f_blocks;
		diskfree = fsbuf.f_bfree;
#endif

		COSMOS_USLEEP(100000); // no support in win, 100ms reporting interval
	}
	return 0;
}

// the name of this fn will always be changed
int32_t transmit_kiss(char *request, char* response, void *cdata)
{
	int32_t iretn = 0;
	unsigned char packet_buffer[600]; // w/c count will be 529 bytes (18+1+255*2)
	int payload_size; // Packet Byte Count
	int input_size; // Request Input Byte Count

	request += 3;  // Advance pointer to skip "tx ", transmit the rest

	printf("Received Request");

	// Check Input Length
	input_size = strlen(request); // Get size of null terminated string
	printf(".");
	/*
if(input_size == -1 || input_size > 255)
{
  memcpy(response,"Error: Input larger than 255 maximum", 36);
  iretn=1;
  return (iretn);
}
*/

	// Encode KISS Packet
	payload_size = kissEncode((unsigned char *)request,input_size,packet_buffer);

	// Test Print
	printf("TX: ");
	for(int z=0;z<payload_size;z++)
	{
		printf("%02x ", packet_buffer[z]);
	}
	printf("\n\n");


	RS232_SendBuf(SERIAL_USB1,packet_buffer,payload_size);


	//#printf("Transmitting: %s\n",request+3);


	return (iretn);
}
