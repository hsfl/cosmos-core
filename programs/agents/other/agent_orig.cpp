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

#include "support/configCosmos.h"
#include <stdlib.h>
#include "agent/agentclass.h"
#include "support/jsonlib.h"
#include "physics/physicslib.h"
#include "support/datalib.h"
#include "sys/stat.h"
//#ifdef _MSC_BUILD
//#include "dirent/dirent.h"
//#else
//#include <dirent.h>
//#endif
#include "limits.h"
#include <iostream>

string output;
Agent *agent;

const int REQUEST_WAIT_TIME = 2;
const int SERVER_WAIT_TIME = 4;

bool is_node(std::vector<string> nl, string node_name)
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


void print_node_list(std::vector<string>& nlp) {

    if(nlp.empty())
    {
        return;
    }

    for(string n: nlp)
    {
        printf("    %s\n", n.c_str());
    }
    return;
}

int main(int argc, char *argv[])
{
    int nbytes;
    beatstruc cbeat;
    std::vector<string> nl;
    data_list_nodes(nl);

    agent = new Agent();

    // check command line arguments
    switch (argc)
    {
    case 1:
        {
            printf("Usage: agent [ list | dump [soh, beat, ###] | node_name agent_name \"request [ arguments ]\" ]\n");
            //      printf("\n    List of available nodes:\n\n");
            //      print_node_list(nl);
            //      printf("\n");
            exit(1);
        }
        break;
    case 2:
        // agent dump request
        if (!strcmp(argv[1],"dump"))
        {
            double lmjd = 0., dmjd;
            string channel;
            Agent::AgentMessage cnum;
            Agent::messstruc mess;
            int i;
            locstruc loc;

            if(argc == 3)
            {
                channel = argv[2];
                if (channel == "soh")
                {
                    cnum = Agent::AgentMessage::SOH;
                }
                else
                {
                    if (channel == "beat")
                    {
                        cnum = Agent::AgentMessage::BEAT;
                    }
                    else
                    {
                        cnum = (Agent::AgentMessage)atoi(channel.c_str());
                    }
                }
            }
            else
            {
                channel.clear();
                cnum = Agent::AgentMessage::ALL;
            }

            while (1)
            {
                int32_t iretn;
                if ((iretn=agent->readring(mess,  Agent::AgentMessage::ALL, 1)) > 0)
                {
                    Agent::AgentMessage pretn = (Agent::AgentMessage)iretn;
                    string utc = json_extract_namedobject(mess.jdata.c_str(), "agent_utc");
                    string node = json_convert_string(json_extract_namedobject(mess.jdata.c_str(), "agent_node"));
                    string proc = json_extract_namedobject(mess.jdata.c_str(), "agent_proc");
                    string addr = json_convert_string(json_extract_namedobject(mess.jdata.c_str(), "agent_addr"));
                    string port = json_extract_namedobject(mess.jdata.c_str(), "agent_port");
                    if (!channel.empty() && cnum != pretn)
                    {
                        continue;
                    }

                    if (pretn < Agent::AgentMessage::BINARY)
                    {
                        json_clear_cosmosstruc(JSON_STRUCT_NODE, agent->cinfo);
                        json_clear_cosmosstruc(JSON_STRUCT_DEVICE, agent->cinfo);
                        json_parse(mess.adata.c_str(), agent->cinfo);
                    }

                    switch (pretn)
                    {
                    case Agent::AgentMessage::SOH:
                        printf("[SOH]");
                        break;
                    case Agent::AgentMessage::BEAT:
                        printf("[BEAT]");
                        break;
                    default:
                        printf("[%d]",pretn);
                        break;
                    }
                    printf("%s:[%s:%s][%s:%s](%" PRIu64 ":%" PRIu64 ")\n",utc.c_str(), node.c_str(), proc.c_str(), addr.c_str(), port.c_str(), mess.jdata.size(), mess.adata.size());
                    if (pretn < Agent::AgentMessage::BINARY && !channel.empty())
                    {
                        printf("%s\n",mess.adata.c_str());
                    }
                    if ((channel=="info") && pretn == Agent::AgentMessage::TRACK)
                    {
                        if (agent->cinfo->node.loc.utc > 0.)
                        {
                            if (lmjd > 0.)
                                dmjd = 86400.*(agent->cinfo->node.loc.utc-lmjd);
                            else
                                dmjd = 0.;
                            loc.pos.icrf.s = agent->cinfo->node.loc.pos.icrf.s;
                            loc.pos.utc = agent->cinfo->node.loc.utc;
                            pos_eci(&loc);
                            printf("%16.15g %6.4g %s %8.3f %8.3f %8.3f %5.1f %5.1f %5.1f\n", agent->cinfo->node.loc.utc,dmjd, agent->cinfo->node.name,DEGOF(loc.pos.geod.s.lon),DEGOF(loc.pos.geod.s.lat),loc.pos.geod.s.h, agent->cinfo->node.powgen, agent->cinfo->node.powuse, agent->cinfo->node.battlev);
                            lmjd = agent->cinfo->node.loc.utc;
                        }
                    }
                    if ((channel=="imu") && pretn == Agent::AgentMessage::IMU)
                    {
                        for (i=0; i<agent->cinfo->devspec.imu_cnt; i++)
                        {
                            if (agent->cinfo->agent[0].beat.utc > 0.)
                            {
                                if (lmjd > 0.)
                                    dmjd = 86400.*(agent->cinfo->agent[0].beat.utc-lmjd);
                                else
                                    dmjd = 0.;
                                printf("%.15g %.4g\n",loc.utc,dmjd);
                                lmjd = agent->cinfo->agent[0].beat.utc;
                            }
                        }
                    }
                }
                fflush(stdout);
            } //end infinite while loop
        }
        else if (!strcmp(argv[1],"list"))
        {
            std::vector<beatstruc> cbeat;
            cbeat = agent->find_servers(SERVER_WAIT_TIME);

            if (cbeat.size() > 0)
            {
                std::cout<<"Number of Agents found: "<<cbeat.size()<<std::endl;
                for (unsigned int i=0; i<cbeat.size(); i++)
                {
                    agent->send_request(cbeat[i],(char *)"getvalue {\"agent_pid\"}",output, REQUEST_WAIT_TIME);
                    printf("[%d] %.15g %s %s %s %hu %u\n",i,cbeat[i].utc,cbeat[i].node,cbeat[i].proc,cbeat[i].addr,cbeat[i].port,cbeat[i].bsz);
                    printf("\t%s\n",output.c_str());
                }
            }
            exit(0);
        }
        break;
    default:

        //  if (argc == 2)
        //  {
        //      if (is_node(nl,argv[1]))
        //      {
        //          std::vector<beatstruc> cbeat;
        //          cbeat = agent->find_servers(SERVER_WAIT_TIME);

        //          printf("\n    List of available agents:\n\n");

        //              if (!cbeat.empty()) {
        //              std::cout<<"Number of Agents found: "<<cbeat.size()<<std::endl;
        //              for (unsigned int i=0; i<cbeat.size(); i++)
        //                  if(!strcmp(argv[1],cbeat[i].node))
        //                      printf("    %s\n", cbeat[i].proc);

        //              }
        //          printf("\n");
        //      }
        //      else
        //      {
        //          printf("\n  Node <%s> not found.\n",argv[1]);
        //          printf("\n    List of available nodes:\n\n");
        //          print_node_list(nl);
        //          printf("\n");
        //      }
        //      exit(1);
        //  }


        //  if (argc != 4)
        //  {
        //      printf("Usage: agent [ list | dump [soh, beat, ###] | node_name agent_name \"request [ arguments ]\" ]\n");
        //      user_is_clueless = true;
        //      //exit (1);
        //  }

        if (!strcmp(argv[1],"dump"))
        {
            double lmjd = 0., dmjd;
            string channel;
            Agent::AgentMessage cnum;
            Agent::messstruc mess;
            int i;
            locstruc loc;

            if(argc == 3)
            {
                channel = argv[2];
                if (channel == "soh")
                {
                    cnum = Agent::AgentMessage::SOH;
                }
                else
                {
                    if (channel == "beat")
                    {
                        cnum = Agent::AgentMessage::BEAT;
                    }
                    else
                    {
                        cnum = (Agent::AgentMessage)atoi(channel.c_str());
                    }
                }
            }
            else
            {
                channel.clear();
                cnum = Agent::AgentMessage::ALL;
            }

            while (1)
            {
                int32_t iretn;
                if ((iretn=agent->readring(mess,  Agent::AgentMessage::ALL, 1)) > 0)
                {
                    Agent::AgentMessage pretn = (Agent::AgentMessage)iretn;
                    string utc = json_extract_namedobject(mess.jdata.c_str(), "agent_utc");
                    string node = json_convert_string(json_extract_namedobject(mess.jdata.c_str(), "agent_node"));
                    string proc = json_extract_namedobject(mess.jdata.c_str(), "agent_proc");
                    string addr = json_convert_string(json_extract_namedobject(mess.jdata.c_str(), "agent_addr"));
                    string port = json_extract_namedobject(mess.jdata.c_str(), "agent_port");
                    if (!channel.empty() && cnum != pretn)
                    {
                        continue;
                    }

                    if (pretn < Agent::AgentMessage::BINARY)
                    {
                        json_clear_cosmosstruc(JSON_STRUCT_NODE, agent->cinfo);
                        json_clear_cosmosstruc(JSON_STRUCT_DEVICE, agent->cinfo);
                        json_parse(mess.adata.c_str(), agent->cinfo);
                    }

                    switch (pretn)
                    {
                    case Agent::AgentMessage::SOH:
                        printf("[SOH]");
                        break;
                    case Agent::AgentMessage::BEAT:
                        printf("[BEAT]");
                        break;
                    default:
                        printf("[%d]",pretn);
                        break;
                    }
                    printf("%s:[%s:%s][%s:%s](%" PRIu64 ":%" PRIu64 ")\n",utc.c_str(), node.c_str(), proc.c_str(), addr.c_str(), port.c_str(), mess.jdata.size(), mess.adata.size());
                    if (pretn < Agent::AgentMessage::BINARY && !channel.empty())
                    {
                        printf("%s\n",mess.adata.c_str());
                    }
                    if ((channel=="info") && pretn == Agent::AgentMessage::TRACK)
                    {
                        if (agent->cinfo->node.loc.utc > 0.)
                        {
                            if (lmjd > 0.)
                                dmjd = 86400.*(agent->cinfo->node.loc.utc-lmjd);
                            else
                                dmjd = 0.;
                            loc.pos.icrf.s = agent->cinfo->node.loc.pos.icrf.s;
                            loc.pos.utc = agent->cinfo->node.loc.utc;
                            pos_eci(&loc);
                            printf("%16.15g %6.4g %s %8.3f %8.3f %8.3f %5.1f %5.1f %5.1f\n", agent->cinfo->node.loc.utc,dmjd, agent->cinfo->node.name,DEGOF(loc.pos.geod.s.lon),DEGOF(loc.pos.geod.s.lat),loc.pos.geod.s.h, agent->cinfo->node.powgen, agent->cinfo->node.powuse, agent->cinfo->node.battlev);
                            lmjd = agent->cinfo->node.loc.utc;
                        }
                    }
                    if ((channel=="imu") && pretn == Agent::AgentMessage::IMU)
                    {
                        for (i=0; i<agent->cinfo->devspec.imu_cnt; i++)
                        {
                            if (agent->cinfo->agent[0].beat.utc > 0.)
                            {
                                if (lmjd > 0.)
                                    dmjd = 86400.*(agent->cinfo->agent[0].beat.utc-lmjd);
                                else
                                    dmjd = 0.;
                                printf("%.15g %.4g\n",loc.utc,dmjd);
                                lmjd = agent->cinfo->agent[0].beat.utc;
                            }
                        }
                    }
                }
                fflush(stdout);
            } //end infinite while loop
        }
    else
        {
        nl.clear();

        if ((nbytes = agent->get_server(argv[1],argv[2],SERVER_WAIT_TIME,&cbeat)) > 0)
        {
            if(argc == 3)
            {
                printf("List of available requests:\n");
                nbytes = agent->send_request(cbeat, "help", output, REQUEST_WAIT_TIME);
                printf("%s [%d]\n",output.c_str(), nbytes);
            }
            else
            {
                string request;
                request = argv[3];
                for (size_t i=0; i<(size_t)argc-4; ++i)
                {
                    request += " ";
                    request += argv[i+4];
                }
                nbytes = agent->send_request(cbeat ,request , output, REQUEST_WAIT_TIME);
                printf("%s [%d]\n",output.c_str(), nbytes);
            }
        }
        else
        {
            if (!nbytes)
                fprintf(stderr,"node-agent pair [%s:%s] not found\n",argv[1],argv[2]);
            else
                printf("Error: %d\n",nbytes);
        }
    }
    }
}
