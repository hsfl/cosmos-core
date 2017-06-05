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

#include "support/configCosmos.h"
#include <stdlib.h>
#include "agent/agentclass.h"
#include "support/jsonlib.h"
#include "physics/physicslib.h"
#include "support/datalib.h"
#include "sys/stat.h"
#include "limits.h"
#include <iostream>

const int REQUEST_WAIT_TIME = 2;
const int SERVER_WAIT_TIME = 6;

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
std::string output;


int main(int argc, char *argv[])
{
    int nbytes;
    beatstruc cbeat;
    std::vector<std::string> nl;
    data_list_nodes(nl);
    Agent *agent;

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
            std::string channel;
            uint8_t cnum;
            Agent::messstruc message;
            int i, pretn;
            locstruc loc;

// JIMNOTE: this block will never be entered

            if(argc == 3)
            {
                channel = argv[2];
                if (channel == "soh")
                {
                    cnum = Agent::AGENT_MESSAGE_SOH;
                }
                else
                {
                    if (channel == "beat")
                    {
                        cnum = Agent::AGENT_MESSAGE_BEAT;
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
                cnum = Agent::AGENT_MESSAGE_ALL;
            }

            while (1)
            {
                if ((pretn=agent->readring(message, Agent::AGENT_MESSAGE_ALL, 1., Agent::Where::TAIL)) > 0)
                {
                    // Skip if either not AGENT_MESSAGE_ALL, or not desited AGENT_MESSAGE
                    if (!channel.empty() && cnum != pretn)
                    {
                        continue;
                    }

                    switch (pretn)
                    {
                    case Agent::AGENT_MESSAGE_SOH:
                        printf("[SOH]");
                        break;
                    case Agent::AGENT_MESSAGE_BEAT:
                        printf("[BEAT]");
                        break;
                    default:
                        printf("[%d]",pretn);
                        break;
                    }

                    printf("%.15g:[%s:%s][%s:%u](%" PRIu64 ":%" PRIu64 ":%" PRIu64 ")\n",message.meta.beat.utc, message.meta.beat.node, message.meta.beat.proc, message.meta.beat.addr, message.meta.beat.port, message.jdata.size(), message.adata.size(), message.bdata.size());
                    printf("%s\n",message.jdata.c_str());
                    if (pretn < Agent::AGENT_MESSAGE_BINARY)
                    {
                        if (!channel.empty())
                        {
                            printf("%s\n",message.adata.c_str());
                        }
                    }

                    if ((channel=="info") && pretn == Agent::AGENT_MESSAGE_TRACK)
                    {
                        if (agent->cinfo->pdata.node.loc.utc > 0.)
                        {
                            if (lmjd > 0.)
                                dmjd = 86400.*(agent->cinfo->pdata.node.loc.utc-lmjd);
                            else
                                dmjd = 0.;
                            loc.pos.icrf.s = agent->cinfo->pdata.node.loc.pos.icrf.s;
                            loc.pos.utc = agent->cinfo->pdata.node.loc.utc;
                            pos_eci(&loc);
                            printf("%16.15g %6.4g %s %8.3f %8.3f %8.3f %5.1f %5.1f %5.1f\n",agent->cinfo->pdata.node.loc.utc,dmjd,agent->cinfo->pdata.node.name,DEGOF(loc.pos.geod.s.lon),DEGOF(loc.pos.geod.s.lat),loc.pos.geod.s.h,agent->cinfo->pdata.node.powgen,agent->cinfo->pdata.node.powuse,agent->cinfo->pdata.node.battlev);
                            lmjd = agent->cinfo->pdata.node.loc.utc;
                        }
                    }

                    if ((channel=="imu") && pretn == Agent::AGENT_MESSAGE_IMU)
                    {
                        for (i=0; i<agent->cinfo->pdata.devspec.imu_cnt; i++)
                        {
                            if (agent->cinfo->pdata.agent[0].beat.utc > 0.)
                            {
                                if (lmjd > 0.)
                                    dmjd = 86400.*(agent->cinfo->pdata.agent[0].beat.utc-lmjd);
                                else
                                    dmjd = 0.;
                                printf("%.15g %.4g\n",loc.utc,dmjd);
                                lmjd = agent->cinfo->pdata.agent[0].beat.utc;
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
                if (agent->agent_list.size() > agent_count)
                {
                    for (size_t i=agent_count; i<agent->agent_list.size(); ++i)
                    {
                        beatstruc cbeat = agent->agent_list[i];
                        agent->send_request(cbeat,(char *)"getvalue {\"agent_pid\"}", output, REQUEST_WAIT_TIME);
                        printf("[%lu] %.15g %s %s %s %hu %u\n",i,cbeat.utc,cbeat.node,cbeat.proc,cbeat.addr,cbeat.port,cbeat.bsz);
                        printf("\t%s\n",output.c_str());
                        fflush(stdout);
                    }
                    agent_count = agent->agent_list.size();
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
            Agent::messstruc message;
            std::string header;
            int i, pretn;
            locstruc loc;

            if(argc == 3)
            {
                channel = argv[2];
                if (channel == "soh")
                {
                    cnum = Agent::AGENT_MESSAGE_SOH;
                }
                else
                {
                    if (channel == "beat")
                    {
                        cnum = Agent::AGENT_MESSAGE_BEAT;
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
                cnum = Agent::AGENT_MESSAGE_ALL;
            }

            while (1)
            {
                if ((pretn=agent->readring(message, Agent::AGENT_MESSAGE_ALL, 1., Agent::Where::TAIL)) > 0)
                {
                    // Skip if either not AGENT_MESSAGE_ALL, or not desited AGENT_MESSAGE
                    if (!channel.empty() && cnum != pretn)
                    {
                        continue;
                    }

                    header.resize(message.meta.jlength);
                    if (pretn < Agent::AGENT_MESSAGE_BINARY)
                    {
                        memcpy(&header[0], message.adata.data(), message.meta.jlength);
                        json_clear_cosmosstruc(JSON_STRUCT_NODE, agent->cinfo->meta, agent->cinfo->sdata);
                        json_clear_cosmosstruc(JSON_STRUCT_DEVICE, agent->cinfo->meta, agent->cinfo->sdata);
                        json_parse(message.adata.c_str(), agent->cinfo->meta, agent->cinfo->sdata);
                    }
                    else
                    {
                        memcpy(&header[0], message.bdata.data(), message.meta.jlength);
                    }

                    switch (pretn)
                    {
                    case Agent::AGENT_MESSAGE_SOH:
                        printf("[SOH]");
                        break;
                    case Agent::AGENT_MESSAGE_BEAT:
                        printf("[BEAT]");
                        break;
                    default:
                        printf("[%d]",pretn);
                        break;
                    }

                    printf("[%d] %.15g %s %s %s %hu %u\n",i,message.meta.beat.utc,message.meta.beat.node,message.meta.beat.proc,message.meta.beat.addr,message.meta.beat.port,message.meta.beat.bsz);

                    if (pretn < Agent::AGENT_MESSAGE_BINARY && !channel.empty())
                    {
                        printf("%s\n",message.adata.c_str());
                    }

                    if ((channel=="info") && pretn == Agent::AGENT_MESSAGE_TRACK)
                    {
                        if (agent->cinfo->pdata.node.loc.utc > 0.)
                        {
                            if (lmjd > 0.)
                                dmjd = 86400.*(agent->cinfo->pdata.node.loc.utc-lmjd);
                            else
                                dmjd = 0.;
                            loc.pos.icrf.s = agent->cinfo->pdata.node.loc.pos.icrf.s;
                            loc.pos.utc = agent->cinfo->pdata.node.loc.utc;
                            pos_eci(&loc);
                            printf("%16.15g %6.4g %s %8.3f %8.3f %8.3f %5.1f %5.1f %5.1f\n",agent->cinfo->pdata.node.loc.utc,dmjd,agent->cinfo->pdata.node.name,DEGOF(loc.pos.geod.s.lon),DEGOF(loc.pos.geod.s.lat),loc.pos.geod.s.h,agent->cinfo->pdata.node.powgen,agent->cinfo->pdata.node.powuse,agent->cinfo->pdata.node.battlev);
                            lmjd = agent->cinfo->pdata.node.loc.utc;
                        }
                    }

                    if ((channel=="imu") && pretn == Agent::AGENT_MESSAGE_IMU)
                    {
                        for (i=0; i<agent->cinfo->pdata.devspec.imu_cnt; i++)
                        {
                            if (agent->cinfo->pdata.agent[0].beat.utc > 0.)
                            {
                                if (lmjd > 0.)
                                    dmjd = 86400.*(agent->cinfo->pdata.agent[0].beat.utc-lmjd);
                                else
                                    dmjd = 0.;
                                printf("%.15g %.4g\n",loc.utc,dmjd);
                                lmjd = agent->cinfo->pdata.agent[0].beat.utc;
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

        if ((nbytes = agent->get_server(argv[1], argv[2], SERVER_WAIT_TIME, &cbeat)) > 0)
        {
            if(argc == 3)
            {
                printf("List of available requests:\n");
                nbytes = agent->send_request(cbeat,(char*)"help", std::ref(output), REQUEST_WAIT_TIME);
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
                nbytes = agent->send_request(cbeat,request.c_str(), output, REQUEST_WAIT_TIME);
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
