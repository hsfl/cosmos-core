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

/*! \file agent_file.cpp
* \brief File Transfer Agent source file
*/

//! \ingroup agents
//! \ingroup agent_file
//! Sends a string as an agent_file::command_type
//!
//! Usage: file_send_command command node:ip_address

#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "support/jsonlib.h"
#include "support/transferlib.h"

typedef struct
{
    string node="";
    socket_channel chansock;
    string chanip="";
    PACKET_CHUNK_SIZE_TYPE packet_size=(200-(PACKET_DATA_OFFSET_HEADER_TOTAL+28));
    uint32_t throughput=1000;
    double limjd;
    double lomjd;
    double nmjd;
    double fmjd;
    packet_struct_heartbeat heartbeat;
    double send_queue;
    double send_reqmeta;
    //    bool send_meta;
    double send_reqdata;
    //    bool send_data;
    double send_complete;
} channelstruc;

static vector <channelstruc> out_comm_channel;

int main(int argc, char*argv[])
{
    int32_t iretn;

    Agent *agent = new Agent();

    string command = argv[2];

    out_comm_channel.resize(1);
    if((iretn = socket_open(&out_comm_channel[0].chansock, NetworkType::UDP, "", AGENTRECVPORT, SOCKET_LISTEN, SOCKET_BLOCKING, 5000000)) < 0)
    {
        fprintf(agent->get_debug_fd(), "%16.10f Main: Node: %s Agent: %s - Listening socket failure\n", currentmjd(), agent->nodeName.c_str(), agent->agentName.c_str());
        agent->shutdown();
        exit (-errno);
    }

    inet_ntop(out_comm_channel[0].chansock.caddr.sin_family, &out_comm_channel[0].chansock.caddr.sin_addr, out_comm_channel[0].chansock.address, sizeof(out_comm_channel[0].chansock.address));
    out_comm_channel[0].chanip = out_comm_channel[0].chansock.address;
    out_comm_channel[0].nmjd = currentmjd();
    out_comm_channel[0].limjd = out_comm_channel[0].nmjd;
    out_comm_channel[0].lomjd = out_comm_channel[0].nmjd;
    out_comm_channel[0].fmjd = out_comm_channel[0].nmjd;
    out_comm_channel[0].node = "";
    out_comm_channel[0].throughput = 1000;
    out_comm_channel[0].packet_size = (200-(PACKET_DATA_OFFSET_HEADER_TOTAL+28));
    fprintf(agent->get_debug_fd(), "%16.10f Node: %s Agent: %s - Listening socket open\n", currentmjd(), agent->nodeName.c_str(), agent->agentName.c_str());
    fflush(agent->get_debug_fd()); // Ensure this gets printed before blocking call

    out_comm_channel.resize(2);
    out_comm_channel[1].node = argv[1];
    size_t tloc = out_comm_channel[1].node.find(":");
    if (tloc != string::npos)
    {
        out_comm_channel[1].chanip = out_comm_channel[1].node.substr(tloc+1, out_comm_channel[1].node.size()-tloc+1);
        out_comm_channel[1].node = out_comm_channel[1].node.substr(0, tloc);
    }
    vector<PACKET_BYTE> packet;
    make_command_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(lookup_node_id(out_comm_channel[1].node)), command);

    if((iretn = socket_open(&out_comm_channel[1].chansock, NetworkType::UDP, out_comm_channel[1].chanip.c_str(), AGENTRECVPORT, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) < 0)
    {
        fprintf(agent->get_debug_fd(), "%16.10f Node: %s IP: %s - Sending socket failure\n", currentmjd(), out_comm_channel[1].node.c_str(), out_comm_channel[1].chanip.c_str());
        agent->shutdown();
        exit (-errno);
    }
    out_comm_channel[1].nmjd = currentmjd();
    out_comm_channel[1].limjd = out_comm_channel[1].nmjd;
    out_comm_channel[1].lomjd = out_comm_channel[1].nmjd;
    out_comm_channel[1].fmjd = out_comm_channel[1].nmjd;
    out_comm_channel[1].throughput = 1000;
    out_comm_channel[1].packet_size = (200-(PACKET_DATA_OFFSET_HEADER_TOTAL+28));
    fprintf(agent->get_debug_fd(), "%16.10f Network: Old: %u %s %s %u\n", currentmjd(), 1, out_comm_channel[1].node.c_str(), out_comm_channel[1].chanip.c_str(), ntohs(out_comm_channel[1].chansock.caddr.sin_port));
    fflush(agent->get_debug_fd());

    iretn = sendto(out_comm_channel[1].chansock.cudp, reinterpret_cast<const char*>(&packet[0]), packet.size(), 0, reinterpret_cast<sockaddr*>(&out_comm_channel[1].chansock.caddr), sizeof(struct sockaddr_in));

}
