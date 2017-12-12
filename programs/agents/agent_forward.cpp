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

#include "support/configCosmos.h"

#include <stdio.h>

#include "agent/agentclass.h"

void incoming_thread();
char agentname[COSMOS_MAX_NAME+1] = "forward";
int waitsec = 5; // wait to find other agents of your 'type/name', seconds

Agent *agent;
socket_channel rcvchan;
std::vector<socket_channel> sendchan;

int32_t request_add_forward(char *req, char* response, Agent *);
int32_t request_del_forward(char *req, char* response, Agent *);

#define MAXBUFFERSIZE 2560 // comm buffer for agents

int main(int argc, char *argv[])
{
    int32_t iretn;

    if (argc < 1)
    {
        printf("Usage: agent_forward [{ipaddresses1} {ipaddress2} {ipaddress3} ...]\n");
        exit (1);
    }

    // Initialize the Agent
    if (!(agent = new Agent("", "forward", 5.)) || agent->cinfo == nullptr)
    {
        exit (AGENT_ERROR_JSON_CREATE);
    }

    // Add requests
    if ((iretn=agent->add_request("add_forward",request_add_forward,"add_forward xxx.xxx.xxx.xxx", "Add address to forwarding list.")))
        exit (iretn);

    // Open sockets to each address to be used for outgoing forwarding.
    for (uint16_t i=1; i<argc; ++i)
    {
        socket_channel tempchan;
        if ((iretn=socket_open(&tempchan, NetworkType::UDP, argv[i], AGENTRECVPORT, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) == 0)
        {
            sendchan.push_back(tempchan);
        }
    }

    // Open the socket for incoming forwarding.
    if ((iretn=socket_open(&rcvchan, NetworkType::UDP, "", AGENTRECVPORT, SOCKET_LISTEN, SOCKET_BLOCKING, AGENTRCVTIMEO)) != 0)
    {
        for (uint16_t i=0; i<sendchan.size(); ++i)
        {
            close(sendchan[i].cudp);
        }
        agent->shutdown();
        printf("Could not open incoming socket for forwarding: %d\n", iretn);
        exit (1);
    }

    // Start thread for incoming forwarding.
    thread thread_incoming;
    thread_incoming = thread(incoming_thread);

    // Start performing the body of the agent
    //	char input[AGENTMAXBUFFER];
    uint8_t post[AGENTMAXBUFFER];
    size_t nbytes;

    while(agent->running())
    {
        Agent::messstruc mess;
        iretn = agent->readring(mess, Agent::AgentMessage::ALL, 1., Agent::Where::TAIL);

        if (iretn > 0)
        {
            post[0] = (uint8_t)mess.meta.type;
            sprintf((char *)&post[3], "%s", mess.jdata.c_str());
            size_t hlength = strlen((char *)&post[3]);
            post[1] = hlength%256;
            post[2] = hlength / 256;
            nbytes = hlength + 3;

            if (mess.meta.type < Agent::AgentMessage::BINARY && mess.adata.size())
            {
                if (nbytes+mess.adata.size() > AGENTMAXBUFFER)
                    continue;
                memcpy(&post[nbytes], &mess.adata[0], mess.adata.size());
                nbytes += mess.adata.size();
            }

            if (mess.meta.type >= Agent::AgentMessage::BINARY && mess.bdata.size())
            {
                if (nbytes+mess.bdata.size() > AGENTMAXBUFFER)
                    continue;
                memcpy(&post[nbytes], &mess.bdata[0], mess.bdata.size());
                nbytes += mess.bdata.size();
            }

            // Forward to all connected forwarders
            for (uint16_t i=0; i<sendchan.size(); ++i)
            {
                if (sendchan[i].address[0])
                {
                    iretn = sendto(sendchan[i].cudp, (const char *)post, nbytes, 0, (struct sockaddr *)&sendchan[i].caddr, sizeof(struct sockaddr_in));
                }
            }
        }
    }

    thread_incoming.join();
    for (uint16_t i=0; i<sendchan.size(); ++i)
    {
        close(sendchan[i].cudp);
    }
    agent->shutdown();
}

void incoming_thread()
{
    //	int32_t iretn;
    int32_t nbytes;
    int32_t iretn;
    vector <uint8_t> input;

    while(agent->running())
    {
        //        if ((nbytes = recvfrom(rcvchan.cudp,input,AGENTMAXBUFFER,0,(struct sockaddr *)&rcvchan.caddr,(socklen_t *)&rcvchan.addrlen)) > 0)
        if ((nbytes = socket_recvfrom(rcvchan, input, AGENTMAXBUFFER, 0)) > 0)
        {
            // New forwarder? Add to forwarding list
            bool found = false;
            for (size_t i=0; i<sendchan.size(); ++i)
            {
                if (!strcmp(sendchan[i].address, rcvchan.address))
                {
                    found = true;
                }
            }

            if (!found)
            {
                socket_channel tempchan;
                if ((iretn=socket_open(&tempchan, NetworkType::UDP, rcvchan.address, AGENTRECVPORT, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) == 0)
                {
                    bool added = false;
                    for (size_t i=0; i<sendchan.size(); ++i)
                    {
                        if (!sendchan[i].address[0])
                        {
                            sendchan[i] = tempchan;
                            added = true;
                            break;
                        }
                    }

                    if (!added)
                    {
                        {
                            sendchan.push_back(tempchan);
                        }
                    }
                }
            }

            for (size_t i=0; i<agent->cinfo->agent[0].ifcnt; ++i)
            {
                sendto(agent->cinfo->agent[0].pub[i].cudp, (const char *)input.data(), nbytes, 0, (struct sockaddr *)&agent->cinfo->agent[0].pub[i].caddr, sizeof(struct sockaddr_in));
            }
        }
    }
}

int32_t request_add_forward(char *req, char* response, Agent *)
{
    int32_t iretn;
    char address[50];
    sscanf(req, "%*s %s", address);
    socket_channel tempchan;
    if ((iretn=socket_open(&tempchan, NetworkType::UDP, address, AGENTRECVPORT, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) == 0)
    {
        bool added = false;
        for (size_t i=0; i<sendchan.size(); ++i)
        {
            if (!sendchan[i].address[0])
            {
                sendchan[i] = tempchan;
                added = true;
                break;
            }
        }

        if (!added)
        {
            {
                sendchan.push_back(tempchan);
            }
        }
    }
    return 0;
}

int32_t request_del_forward(char *req, char* response, Agent *)
{
    char address[50];
    sscanf(req, "%*s %s", address);

    for (size_t i=0; i<sendchan.size(); ++i)
    {
        if (!strcmp(sendchan[i].address, address))
        {
            socket_close(&sendchan[i]);
            sendchan[0].address[0] = 0;
        }
    }
    return 0;
}
