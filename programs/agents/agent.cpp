/*! \file agent.cpp
* \brief Agent control program source
*/

//! \ingroup general
//! \defgroup agent Agent control program
//! This program allows communication with any of the Agents on the local network.
//! With it you can:
//! - list available Agents
//! - acquire the request list of specific Agents
//! - command specific Agents
//! - monitor Agent traffic

#include "configCosmos.h"
#include <stdlib.h>
#include "agentlib.h"
#include "jsonlib.h"
#include "physicslib.h"
#include "datalib.h"
#include "sys/stat.h"
//#ifdef _MSC_BUILD
//#include "dirent/dirent.h"
//#else
//#include <dirent.h>
//#endif
#include "limits.h"
#include <iostream>

using namespace std;

char request[AGENTMAXBUFFER], output[AGENTMAXBUFFER];
cosmosstruc *cdata;

const int REQUEST_WAIT_TIME = 2;
const int SERVER_WAIT_TIME = 4;

bool is_node(vector<string> nl, string node_name)
{

	for (string node: nl)
	{
		if(node == node_name)
		{
			return true;
		}
	}
	return false;
}


void print_node_list(vector<string>& nlp) {

	if(nlp.empty())
	{
		return;
	}

	for(string n: nlp)
	{
		printf("	%s\n", n.c_str());
	}
	return;
}

int main(int argc, char *argv[])
{
	int nbytes;
	beatstruc cbeat;
	bool user_is_clueless = false;
	vector<string> nl;
	data_list_nodes(nl);

	// check command line arguments
	if (argc == 1)
	{
		printf("\n  Usage: agent [ list | dump | node_name agent_name \"request [ arguments ]\" ]\n");
		printf("\n    List of available nodes:\n\n");
		print_node_list(nl);
		printf("\n");
		exit(1);
	}

    cdata = agent_setup_client(SOCKET_TYPE_UDP, "", 1000);

	// agent dump request
	if ((argc== 2 || argc == 3) && !strcmp(argv[1],"dump"))
	{

		double lmjd = 0., dmjd;
		char channel[30];
		string message;
		int i, pretn;
		locstruc loc;

		if (	argc == 3 
			&& strcmp(argv[2],"all") 
			&& strcmp(argv[2],"beat") 
			&& strcmp(argv[2],"imu") 
			&& strcmp(argv[2],"info") 
			&& strcmp(argv[2],"loc") 
			&& strcmp(argv[2],"soh") 
			&& strcmp(argv[2],"some") 
			&& strcmp(argv[2],"time")        )
		{

			printf("\n  Usage: agent dump [ all | beat | imu | info | loc | soh | some | time ]\n\n");
			exit(1);
		}
		if(argc == 3)
			strcpy(channel,argv[2]);
		else
			strcpy(channel,"some");

		while (1)
		{
			if ((pretn=agent_poll(cdata, message,  AGENT_MESSAGE_ALL, 1)) > 0)
			{
				json_clear_cosmosstruc(JSON_GROUP_NODE,cdata);
				json_clear_cosmosstruc(JSON_GROUP_DEVICE,cdata);
				string utc = json_extract_namedobject(message.c_str(), "agent_utc");
				string node = json_convert_string(json_extract_namedobject(message.c_str(), "agent_node"));
				string proc = json_extract_namedobject(message.c_str(), "agent_proc");
				string addr = json_convert_string(json_extract_namedobject(message.c_str(), "agent_addr"));
				string port = json_extract_namedobject(message.c_str(), "agent_port");
				if ((!strcmp(channel,"soh") && pretn != AGENT_MESSAGE_SOH) || (!strcmp(channel,"beat") && pretn != AGENT_MESSAGE_BEAT))
					continue;
				json_parse(message.c_str(),cdata);
				switch (pretn)
				{
				case AGENT_MESSAGE_SOH:
					printf("[SOH]");
					break;
				case AGENT_MESSAGE_BEAT:
					printf("[BEAT]");
					break;
				default:
					printf("[%d]",pretn);
					break;
				}
				printf("%s:[%s:%s][%s:%s](%" PRIu32 ")\n",utc.c_str(), node.c_str(), proc.c_str(), addr.c_str(), port.c_str(), message.size());
				if (!strcmp(channel,"all"))
				{
					printf("%s\n",message.c_str());
				}
				if (!strcmp(channel,"info") && pretn == AGENT_MESSAGE_TRACK)
				{
					if (cdata[0].node.loc.utc > 0.)
					{
						if (lmjd > 0.)
							dmjd = 86400.*(cdata[0].node.loc.utc-lmjd);
						else
							dmjd = 0.;
						loc.pos.baryc.s = cdata[0].node.loc.pos.baryc.s;
						loc.pos.utc = cdata[0].node.loc.utc;
						pos_eci(&loc);
						printf("%16.15g %6.4g %s %8.3f %8.3f %8.3f %5.1f %5.1f %5.1f\n",cdata[0].node.loc.utc,dmjd,cdata[0].node.name,DEGOF(loc.pos.geod.s.lon),DEGOF(loc.pos.geod.s.lat),loc.pos.geod.s.h,cdata[0].node.powgen,cdata[0].node.powuse,cdata[0].node.battlev);
						lmjd = cdata[0].node.loc.utc;
					}
				}
				if (!strcmp(channel,"imu") && pretn == AGENT_MESSAGE_IMU)
				{
					for (i=0; i<cdata[0].devspec.imu_cnt; i++)
					{
						if (cdata[0].agent[0].beat.utc > 0.)
						{
							if (lmjd > 0.)
								dmjd = 86400.*(cdata[0].agent[0].beat.utc-lmjd);
							else
								dmjd = 0.;
							printf("%.15g %.4g\n",loc.utc,dmjd);
							lmjd = cdata[0].agent[0].beat.utc;
						}
					}
				}
			}
			fflush(stdout);
		} //end infinite while loop
	}

	if (argc == 2 && !strcmp(argv[1],"list"))
	{
		vector<beatstruc> cbeat;
		cbeat = agent_find_servers(cdata, SERVER_WAIT_TIME);

        	if (cbeat.size() > 0)
        	{
				cout<<"Number of Agents found: "<<cbeat.size()<<endl;
                	for (unsigned int i=0; i<cbeat.size(); i++)
                	{
							agent_send_request(cdata, cbeat[i],(char *)"getvalue {\"agent_pid\"}",output,AGENTMAXBUFFER,REQUEST_WAIT_TIME);
                        	printf("[%d] %.15g %s %s %s %hu %u\n",i,cbeat[i].utc,cbeat[i].node,cbeat[i].proc,cbeat[i].addr,cbeat[i].port,cbeat[i].bsz);
                        	printf("\t%s\n",output);
                	}
        	}
		exit(0);
	}

	if (argc == 2)
	{
		printf("\n  Usage: agent [ list | dump | node_name agent_name \"request [ arguments ]\" ]\n");

		if (is_node(nl,argv[1]))
		{
			vector<beatstruc> cbeat;
			cbeat = agent_find_servers(cdata, SERVER_WAIT_TIME);

			printf("\n    List of available agents:\n\n");

    			if (!cbeat.empty()) {
				cout<<"Number of Agents found: "<<cbeat.size()<<endl;
				for (unsigned int i=0; i<cbeat.size(); i++)
					if(!strcmp(argv[1],cbeat[i].node))
						printf("	%s\n", cbeat[i].proc);

				}
			printf("\n");
		}
		else
		{
			printf("\n  Node <%s> not found.\n",argv[1]);
			printf("\n    List of available nodes:\n\n");
			print_node_list(nl);
			printf("\n");
		}
		exit(1);
	}


	if (argc != 4)
	{
		printf("\n  Usage: agent [ list | dump | node_name agent_name \"request [ arguments ]\" ]\n");
		user_is_clueless = true;
		//exit (1);
	}

	nl.clear();

	if ((nbytes = agent_get_server(cdata, argv[1],argv[2],SERVER_WAIT_TIME,&cbeat)) > 0)
	{
		if(user_is_clueless)
		{
			printf("\n    List of available requests:\n");
			nbytes =
			agent_send_request(cdata, cbeat,(char*)"help",output,AGENTMAXBUFFER,REQUEST_WAIT_TIME);
			printf("%s [%d]\n",output,nbytes);
		}
		else
		{
			strcpy(request,argv[3]);
			nbytes =
			agent_send_request(cdata, cbeat,request,output,AGENTMAXBUFFER,REQUEST_WAIT_TIME);
			printf("%s [%d]\n",output,nbytes);
		}
	}
	else
	{
		if (!nbytes)
			fprintf(stderr,"\n  node-agent pair [%s:%s] not found\n\n",argv[1],argv[2]);
			//printf("\n  node-agent pair [%s:%s] not found\n\n",argv[1],argv[2]);
		else
			printf("Error: %d\n",nbytes);
	}
}
