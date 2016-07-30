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

/*! \file agent_client.cpp
* \brief Agent control program source
*/

//! \ingroup general
//! \defgroup agent_client Agent control program
//! This program allows communication with any of the Agents on the local network.
//! With it you can:
//! - list available Agents
//! - acquire the request list of specific Agents
//! - command specific Agents
//! - monitor Agent traffic

#include "configCosmos.h"
#include <stdlib.h>
#include "agent/agent.h"
#include "jsonlib.h"
#include "physics/physicslib.h"
#include "datalib.h"
#include "sys/stat.h"
#include "limits.h"
#include <iostream>

//using namespace std;

std::string output;

const int REQUEST_WAIT_TIME = 2;
const int SERVER_WAIT_TIME = 4;

//void print_node_list(std::vector<std::string>& nlp) {

//    if(nlp.empty())
//    {
//        return;
//    }

//    for(std::string n: nlp)
//    {
//        printf("    %s\n", n.c_str());
//    }
//    return;
//}

int main(int argc, char *argv[])
{
    int nbytes;
    beatstruc cbeat;
    std::vector<std::string> nl;
    data_list_nodes(nl);
    cosmosAgent agent;

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
            cosmosAgent::pollstruc meta;
            int i, pretn;
            locstruc loc;

            if(argc == 3)
            {
                channel = argv[2];
                if (channel == "soh")
                {
                    cnum = cosmosAgent::AGENT_MESSAGE_SOH;
                }
                else
                {
                    if (channel == "beat")
                    {
                        cnum = cosmosAgent::AGENT_MESSAGE_BEAT;
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
                cnum = cosmosAgent::AGENT_MESSAGE_ALL;
            }

            while (1)
            {
                if ((pretn=agent.poll(meta, message, cosmosAgent::AGENT_MESSAGE_ALL)) > 0)
                {
                    header.resize(meta.jlength);
                    memcpy(&header[0], &message[3], meta.jlength);
                    if (!channel.empty() && cnum != pretn)
                    {
                        continue;
                    }

                    if (pretn < 128)
                    {
                        json_clear_cosmosstruc(JSON_GROUP_NODE,&agent.cdata[1]);
                        json_clear_cosmosstruc(JSON_GROUP_DEVICE,&agent.cdata[1]);
                        json_parse(message.c_str(),&agent.cdata[1]);
                    }

                    switch (pretn)
                    {
                    case cosmosAgent::AGENT_MESSAGE_SOH:
                        printf("[SOH]");
                        break;
                    case cosmosAgent::AGENT_MESSAGE_BEAT:
                        printf("[BEAT]");
                        break;
                    default:
                        printf("[%d]",pretn);
                        break;
                    }
                    printf("%.15g:[%s:%s][%s:%u](%" PRIu32 ":%" PRIu32 ")\n",meta.beat.utc, meta.beat.node, meta.beat.proc, meta.beat.addr, meta.beat.port, header.size(), message.size());
                    if (pretn < 128 && !channel.empty())
                    {
                        printf("%s\n",message.c_str());
                    }
                    if ((channel=="info") && pretn == cosmosAgent::AGENT_MESSAGE_TRACK)
                    {
                        if (agent.cdata[0].node.loc.utc > 0.)
                        {
                            if (lmjd > 0.)
                                dmjd = 86400.*(agent.cdata[0].node.loc.utc-lmjd);
                            else
                                dmjd = 0.;
                            loc.pos.icrf.s = agent.cdata[0].node.loc.pos.icrf.s;
                            loc.pos.utc = agent.cdata[0].node.loc.utc;
                            pos_eci(&loc);
                            printf("%16.15g %6.4g %s %8.3f %8.3f %8.3f %5.1f %5.1f %5.1f\n",agent.cdata[0].node.loc.utc,dmjd,agent.cdata[0].node.name,DEGOF(loc.pos.geod.s.lon),DEGOF(loc.pos.geod.s.lat),loc.pos.geod.s.h,agent.cdata[0].node.powgen,agent.cdata[0].node.powuse,agent.cdata[0].node.battlev);
                            lmjd = agent.cdata[0].node.loc.utc;
                        }
                    }
                    if ((channel=="imu") && pretn == cosmosAgent::AGENT_MESSAGE_IMU)
                    {
                        for (i=0; i<agent.cdata[0].devspec.imu_cnt; i++)
                        {
                            if (agent.cdata[0].agent[0].beat.utc > 0.)
                            {
                                if (lmjd > 0.)
                                    dmjd = 86400.*(agent.cdata[0].agent[0].beat.utc-lmjd);
                                else
                                    dmjd = 0.;
                                printf("%.15g %.4g\n",loc.utc,dmjd);
                                lmjd = agent.cdata[0].agent[0].beat.utc;
                            }
                        }
                    }
                }
                fflush(stdout);
            } //end infinite while loop
        }
        else if (!strcmp(argv[1],"list"))
        {
            size_t agent_count = 0;
            ElapsedTime et;
            do
            {
                if (agent.agent_list.size() > agent_count)
                {
                    for (size_t i=agent_count; i<agent.agent_list.size(); ++i)
                    {
                        beatstruc cbeat = agent.agent_list[i];
                        agent.send_request(cbeat,(char *)"getvalue {\"agent_pid\"}", output, REQUEST_WAIT_TIME);
                        printf("[%d] %.15g %s %s %s %hu %u\n",i,cbeat.utc,cbeat.node,cbeat.proc,cbeat.addr,cbeat.port,cbeat.bsz);
                        printf("\t%s\n",output.c_str());
                        fflush(stdout);
                    }
                    agent_count = agent.agent_list.size();
                }
                COSMOS_SLEEP(.1);
            } while (et.split() < SERVER_WAIT_TIME);
            exit(0);
        }
        break;
    default:
        if (!strcmp(argv[1],"dump"))
        {
            double lmjd = 0., dmjd;
            std::string channel;
            uint8_t cnum;
            std::string message;
            std::string header;
            cosmosAgent::pollstruc meta;
            int i, pretn;
            locstruc loc;

            if(argc == 3)
            {
                channel = argv[2];
                if (channel == "soh")
                {
                    cnum = cosmosAgent::AGENT_MESSAGE_SOH;
                }
                else
                {
                    if (channel == "beat")
                    {
                        cnum = cosmosAgent::AGENT_MESSAGE_BEAT;
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
                cnum = cosmosAgent::AGENT_MESSAGE_ALL;
            }

            while (1)
            {
                if ((pretn=agent.poll(meta, message,  cosmosAgent::AGENT_MESSAGE_ALL, 1)) > 0)
                {
                    header.resize(meta.jlength);
                    memcpy(&header[0], &message[3], meta.jlength);
                    if (!channel.empty() && cnum != pretn)
                    {
                        continue;
                    }

                    if (pretn < 128)
                    {
                        json_clear_cosmosstruc(JSON_GROUP_NODE,&agent.cdata[1]);
                        json_clear_cosmosstruc(JSON_GROUP_DEVICE,&agent.cdata[1]);
                        json_parse(message.c_str(),&agent.cdata[1]);
                    }

                    switch (pretn)
                    {
                    case cosmosAgent::AGENT_MESSAGE_SOH:
                        printf("[SOH]");
                        break;
                    case cosmosAgent::AGENT_MESSAGE_BEAT:
                        printf("[BEAT]");
                        break;
                    default:
                        printf("[%d]",pretn);
                        break;
                    }
                    printf("[%d] %.15g %s %s %s %hu %u\n",i,cbeat.utc,cbeat.node,cbeat.proc,cbeat.addr,cbeat.port,cbeat.bsz);
                    if (pretn < 128 && !channel.empty())
                    {
                        printf("%s\n",message.c_str());
                    }
                    if ((channel=="info") && pretn == cosmosAgent::AGENT_MESSAGE_TRACK)
                    {
                        if (agent.cdata[0].node.loc.utc > 0.)
                        {
                            if (lmjd > 0.)
                                dmjd = 86400.*(agent.cdata[0].node.loc.utc-lmjd);
                            else
                                dmjd = 0.;
                            loc.pos.icrf.s = agent.cdata[0].node.loc.pos.icrf.s;
                            loc.pos.utc = agent.cdata[0].node.loc.utc;
                            pos_eci(&loc);
                            printf("%16.15g %6.4g %s %8.3f %8.3f %8.3f %5.1f %5.1f %5.1f\n",agent.cdata[0].node.loc.utc,dmjd,agent.cdata[0].node.name,DEGOF(loc.pos.geod.s.lon),DEGOF(loc.pos.geod.s.lat),loc.pos.geod.s.h,agent.cdata[0].node.powgen,agent.cdata[0].node.powuse,agent.cdata[0].node.battlev);
                            lmjd = agent.cdata[0].node.loc.utc;
                        }
                    }
                    if ((channel=="imu") && pretn == cosmosAgent::AGENT_MESSAGE_IMU)
                    {
                        for (i=0; i<agent.cdata[0].devspec.imu_cnt; i++)
                        {
                            if (agent.cdata[0].agent[0].beat.utc > 0.)
                            {
                                if (lmjd > 0.)
                                    dmjd = 86400.*(agent.cdata[0].agent[0].beat.utc-lmjd);
                                else
                                    dmjd = 0.;
                                printf("%.15g %.4g\n",loc.utc,dmjd);
                                lmjd = agent.cdata[0].agent[0].beat.utc;
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

        if ((nbytes = agent.get_server(argv[1], argv[2], SERVER_WAIT_TIME, &cbeat)) > 0)
        {
            if(argc == 3)
            {
                printf("List of available requests:\n");
                nbytes = agent.send_request(cbeat,(char*)"help", output, REQUEST_WAIT_TIME);
                printf("%s [%d]\n", output.c_str(), nbytes);
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
                nbytes = agent.send_request(cbeat,request.c_str(), output, REQUEST_WAIT_TIME);
                printf("%s [%d]\n", output.c_str(), nbytes);
            }
        }
        else
        {
            if (!nbytes)
                fprintf(stderr,"node-agent pair [%s:%s] not found\n",argv[1],argv[2]);
            else
                printf("Error: %d\n", nbytes);
        }
    }
    }
}
