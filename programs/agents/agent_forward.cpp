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
#include <algorithm>
#include "agent/agentclass.h"

void opening_loop();
void forwarding_loop();

static Agent *agent;
static socket_channel rcvchan;
static vector<socket_channel> sendchan;

int32_t request_add_forward(string &req, string & , Agent *);
int32_t request_del_forward(string &req, string &, Agent *);

#define MAXBUFFERSIZE 2560 // comm buffer for agents

int main(int argc, char *argv[])
{
    int32_t iretn = 0;

    if (argc < 1)
    {
        printf("Usage: agent_forward [{ipaddresses1} {ipaddress2} {ipaddress3} ...]\n");
        exit (1);
    }

    // Initialize the Agent
    agent = new Agent("", "", "forward", 5.);
    if ((iretn = agent->wait()) < 0)
    {
        agent->debug_log.Printf("%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    else
    {
        agent->debug_log.Printf("%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
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
    std::thread thread_forwarding;
    thread_forwarding = std::thread(forwarding_loop);

    std::thread thread_opening;
    thread_opening = std::thread(opening_loop);

    // Start performing the body of the agent
    //    uint8_t post[AGENTMAXBUFFER];
    //    size_t nbytes;

    while(agent->running())
    {
        Agent::messstruc mess;
        iretn = agent->readring(mess, Agent::AgentMessage::ALL, 1., Agent::Where::TAIL);

        if (iretn > 0)
        {
            vector <uint8_t> post;
            post.resize(mess.jdata.length() + 3 + (mess.meta.type < Agent::AgentMessage::BINARY ? mess.adata.size() : mess.bdata.size()));
            post[0] = static_cast <uint8_t>(mess.meta.type);
            post[1] = static_cast <uint8_t>(mess.jdata.length() % 256);
            post[2] = static_cast <uint8_t>(mess.jdata.length() / 256);
            std::copy(mess.jdata.begin(), mess.jdata.end(), post.begin() + 3);

            if (post.size() <= AGENTMAXBUFFER)
            {
                if (mess.meta.type < Agent::AgentMessage::BINARY)
                {
                    if (mess.adata.size())
                    {
                        std::copy(mess.adata.begin(), mess.adata.end(), post.begin() + mess.jdata.length() + 3);
                    }
                }
                else
                {
                    if (mess.bdata.size())
                    {
                        std::copy(mess.bdata.begin(), mess.bdata.end(), post.begin() + mess.jdata.length() + 3);
                    }
                }
            }

            // Forward to all connected forwarders
            for (uint16_t i=0; i<sendchan.size(); ++i)
            {
                if (sendchan[i].cudp >= 0)
                {
                    iretn = socket_sendto(sendchan[i], post);
                    if (agent->get_debug_level() > 1)
                    {
                        if (iretn < 0)
                        {
                            agent->debug_log.Printf("%s: Failed To %s\n", mjd2iso8601(currentmjd()).c_str(), sendchan[i].address);
                        }
                        else
                        {
                            agent->debug_log.Printf("%s: Sent %zu Bytes To %s\n", mjd2iso8601(currentmjd()).c_str(), post.size(), sendchan[i].address);
                        }
                    }
                }
            }
        }
    }

    thread_forwarding.join();
    thread_opening.join();
    for (uint16_t i=0; i<sendchan.size(); ++i)
    {
        close(sendchan[i].cudp);
    }
    agent->shutdown();
}

void opening_loop()
{
    while(agent->running())
    {
        for (socket_channel &tchan : sendchan)
        {
            if (tchan.cudp < 0)
            {
                socket_channel tempchan;
                if ((socket_open(&tempchan, NetworkType::UDP, tchan.address, AGENTRECVPORT, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) == 0)
                {
                    tchan = tempchan;
                }
            }
        }
        secondsleep(.1);
    }
}

void forwarding_loop()
{
    int32_t nbytes;
    vector <uint8_t> input;

    while(agent->running())
    {
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
                strncpy(tempchan.address, rcvchan.address, 17);
                sendchan.push_back(tempchan);
                if (agent->get_debug_level())
                {
                    agent->debug_log.Printf("%s: Added %s\n", mjd2iso8601(currentmjd()).c_str(), tempchan.address);
                }
            }

            for (size_t i=0; i<agent->cinfo->agent0.ifcnt; ++i)
            {
                sendto(agent->cinfo->agent0.pub[i].cudp, (const char *)input.data(), input.size(), 0, (struct sockaddr *)&agent->cinfo->agent0.pub[i].baddr, sizeof(struct sockaddr_in));
//                socket_sendto(agent->cinfo->agent0.pub[i], input);
            }
        }
    }
}

int32_t request_add_forward(string &req, string &, Agent *)
{
    char address[50];
    sscanf(req.c_str(), "%*s %s", address);

    bool found = false;
    for (size_t i=0; i<sendchan.size(); ++i)
    {
        if (!strcmp(sendchan[i].address, address))
        {
            found = true;
            sendchan[i].cudp = -1;
            break;
        }
    }

    if (!found)
    {
        socket_channel tempchan;
        strncpy(tempchan.address, address, 17);
        sendchan.push_back(tempchan);
    }

    return 0;
}

int32_t request_del_forward(string &req, string & , Agent *)
{
    char address[50];
    sscanf(req.c_str(), "%*s %s", address);

    for (size_t i=0; i<sendchan.size(); ++i)
    {
        if (!strcmp(sendchan[i].address, address))
        {
            socket_close(&sendchan[i]);
            sendchan[i].cudp = -2;
        }
    }
    return 0;
}
