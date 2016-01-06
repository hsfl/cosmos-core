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

//using namespace std;

char output[AGENTMAXBUFFER];
cosmosstruc *cdata;

const int REQUEST_WAIT_TIME = 2;
const int SERVER_WAIT_TIME = 4;

bool is_node(std::vector<std::string> nl, std::string node_name)
{

    for (std::string node: nl)
    {
        if(node == node_name)
        {
            return true;
        }
    }
    return false;
}


void print_node_list(std::vector<std::string>& nlp) {

    if(nlp.empty())
    {
        return;
    }

    for(std::string n: nlp)
    {
        printf("    %s\n", n.c_str());
    }
    return;
}

int main(int argc, char *argv[])
{
    int nbytes;
    beatstruc cbeat;
    std::vector<std::string> nl;
    data_list_nodes(nl);

    cdata = agent_setup_client(NetworkType::UDP, "", 1000);

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
            std::string channel;
            uint8_t cnum;
            std::string message;
            std::string header;
            pollstruc meta;
            int i, pretn;
            locstruc loc;

            if(argc == 3)
            {
                channel = argv[2];
                if (channel == "soh")
                {
                    cnum = AGENT_MESSAGE_SOH;
                }
                else
                {
                    if (channel == "beat")
                    {
                        cnum = AGENT_MESSAGE_BEAT;
                    }
                    else
                    {
                        cnum = atoi(channel.c_str());
                    }
                }
            }
            else
            {
                channel.clear();
                cnum = AGENT_MESSAGE_ALL;
            }

            while (1)
            {
                if ((pretn=agent_poll(cdata, meta, message,  AGENT_MESSAGE_ALL, 1)) > 0)
                {
                    header.resize(meta.jlength);
                    memcpy(&header[0], &message[3], meta.jlength);
                    std::string utc = json_extract_namedobject(header.c_str(), "agent_utc");
                    std::string node = json_convert_string(json_extract_namedobject(header.c_str(), "agent_node"));
                    std::string proc = json_extract_namedobject(header.c_str(), "agent_proc");
                    std::string addr = json_convert_string(json_extract_namedobject(header.c_str(), "agent_addr"));
                    std::string port = json_extract_namedobject(header.c_str(), "agent_port");
                    if (!channel.empty() && cnum != pretn)
                    {
                        continue;
                    }

                    if (pretn < 128)
                    {
                        json_clear_cosmosstruc(JSON_GROUP_NODE,&cdata[1]);
                        json_clear_cosmosstruc(JSON_GROUP_DEVICE,&cdata[1]);
                        json_parse(message.c_str(),&cdata[1]);
                    }

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
                    printf("%s:[%s:%s][%s:%s](%" PRIu32 ":%" PRIu32 ")\n",utc.c_str(), node.c_str(), proc.c_str(), addr.c_str(), port.c_str(), header.size(), message.size());
                    if (pretn < 128 && !channel.empty())
                    {
                        printf("%s\n",message.c_str());
                    }
                    if ((channel=="info") && pretn == AGENT_MESSAGE_TRACK)
                    {
                        if (cdata[0].node.loc.utc > 0.)
                        {
                            if (lmjd > 0.)
                                dmjd = 86400.*(cdata[0].node.loc.utc-lmjd);
                            else
                                dmjd = 0.;
                            loc.pos.icrf.s = cdata[0].node.loc.pos.icrf.s;
                            loc.pos.utc = cdata[0].node.loc.utc;
                            pos_eci(&loc);
                            printf("%16.15g %6.4g %s %8.3f %8.3f %8.3f %5.1f %5.1f %5.1f\n",cdata[0].node.loc.utc,dmjd,cdata[0].node.name,DEGOF(loc.pos.geod.s.lon),DEGOF(loc.pos.geod.s.lat),loc.pos.geod.s.h,cdata[0].node.powgen,cdata[0].node.powuse,cdata[0].node.battlev);
                            lmjd = cdata[0].node.loc.utc;
                        }
                    }
                    if ((channel=="imu") && pretn == AGENT_MESSAGE_IMU)
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
        else if (!strcmp(argv[1],"list"))
        {
            std::vector<beatstruc> cbeat;
            cbeat = agent_find_servers(cdata, SERVER_WAIT_TIME);

            if (cbeat.size() > 0)
            {
                std::cout<<"Number of Agents found: "<<cbeat.size()<<std::endl;
                for (unsigned int i=0; i<cbeat.size(); i++)
                {
                    agent_send_request(cbeat[i],(char *)"getvalue {\"agent_pid\"}",output,AGENTMAXBUFFER,REQUEST_WAIT_TIME);
                    printf("[%d] %.15g %s %s %s %hu %u\n",i,cbeat[i].utc,cbeat[i].node,cbeat[i].proc,cbeat[i].addr,cbeat[i].port,cbeat[i].bsz);
                    printf("\t%s\n",output);
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
        //          cbeat = agent_find_servers(cdata, SERVER_WAIT_TIME);

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
            std::string channel;
            uint8_t cnum;
            std::string message;
            std::string header;
            pollstruc meta;
            int i, pretn;
            locstruc loc;

            if(argc == 3)
            {
                channel = argv[2];
                if (channel == "soh")
                {
                    cnum = AGENT_MESSAGE_SOH;
                }
                else
                {
                    if (channel == "beat")
                    {
                        cnum = AGENT_MESSAGE_BEAT;
                    }
                    else
                    {
                        cnum = atoi(channel.c_str());
                    }
                }
            }
            else
            {
                channel.clear();
                cnum = AGENT_MESSAGE_ALL;
            }

            while (1)
            {
                if ((pretn=agent_poll(cdata, meta, message,  AGENT_MESSAGE_ALL, 1)) > 0)
                {
                    header.resize(meta.jlength);
                    memcpy(&header[0], &message[3], meta.jlength);
                    std::string utc = json_extract_namedobject(header.c_str(), "agent_utc");
                    std::string node = json_convert_string(json_extract_namedobject(header.c_str(), "agent_node"));
                    std::string proc = json_extract_namedobject(header.c_str(), "agent_proc");
                    std::string addr = json_convert_string(json_extract_namedobject(header.c_str(), "agent_addr"));
                    std::string port = json_extract_namedobject(header.c_str(), "agent_port");
                    if (!channel.empty() && cnum != pretn)
                    {
                        continue;
                    }

                    if (pretn < 128)
                    {
                        json_clear_cosmosstruc(JSON_GROUP_NODE,&cdata[1]);
                        json_clear_cosmosstruc(JSON_GROUP_DEVICE,&cdata[1]);
                        json_parse(message.c_str(),&cdata[1]);
                    }

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
                    printf("%s:[%s:%s][%s:%s](%" PRIu32 ":%" PRIu32 ")\n",utc.c_str(), node.c_str(), proc.c_str(), addr.c_str(), port.c_str(), header.size(), message.size());
                    if (pretn < 128 && !channel.empty())
                    {
                        printf("%s\n",message.c_str());
                    }
                    if ((channel=="info") && pretn == AGENT_MESSAGE_TRACK)
                    {
                        if (cdata[0].node.loc.utc > 0.)
                        {
                            if (lmjd > 0.)
                                dmjd = 86400.*(cdata[0].node.loc.utc-lmjd);
                            else
                                dmjd = 0.;
                            loc.pos.icrf.s = cdata[0].node.loc.pos.icrf.s;
                            loc.pos.utc = cdata[0].node.loc.utc;
                            pos_eci(&loc);
                            printf("%16.15g %6.4g %s %8.3f %8.3f %8.3f %5.1f %5.1f %5.1f\n",cdata[0].node.loc.utc,dmjd,cdata[0].node.name,DEGOF(loc.pos.geod.s.lon),DEGOF(loc.pos.geod.s.lat),loc.pos.geod.s.h,cdata[0].node.powgen,cdata[0].node.powuse,cdata[0].node.battlev);
                            lmjd = cdata[0].node.loc.utc;
                        }
                    }
                    if ((channel=="imu") && pretn == AGENT_MESSAGE_IMU)
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
    else
        {
        nl.clear();

        if ((nbytes = agent_get_server(cdata, argv[1],argv[2],SERVER_WAIT_TIME,&cbeat)) > 0)
        {
            if(argc == 3)
            {
                printf("List of available requests:\n");
                nbytes = agent_send_request(cbeat,(char*)"help",output,AGENTMAXBUFFER,REQUEST_WAIT_TIME);
                printf("%s [%d]\n",output,nbytes);
            }
            else
            {
                std::string request;
                request = argv[3];
                for (size_t i=0; i<(size_t)argc-4; ++i)
                {
                    request += " ";
                    request += argv[i+4];
                }
                nbytes = agent_send_request(cbeat,request.c_str(),output,AGENTMAXBUFFER,REQUEST_WAIT_TIME);
                printf("%s [%d]\n",output,nbytes);
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
