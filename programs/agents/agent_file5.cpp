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

/*! \file agent_file5.cpp
* \brief File Transfer Agent source file
*/

//! \ingroup agents
//! \defgroup agent_file File Transfer Agent program
//! Manages file transfers and message passing between COSMOS Nodes.
//!
//! Both file and message transfers are handled as an interchange of packets
//! between two instances of agent_file.
//!
//! Assign an id and node_name string in nodes/nodeids.ini file
//!
//! The transferclass version of agent_file4
//!
//! Usage: agent_file5 debug_mode target_node_name:chanip_address[:throughput]


#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "support/jsonlib.h"
#include "support/transferclass.h"
#include "support/sliplib.h"
#include "support/jsonobject.h"
#include "support/socketlib.h"

#define PROGRESS_QUEUE_SIZE 256
// Corrected for 28 byte UDP header. Will have to get more clever if we start using CSP
#define PACKET_SIZE_LO (200-(PACKET_DATA_OFFSET_HEADER_TOTAL+28))
#define PACKET_SIZE_PAYLOAD (PACKET_SIZE_LO-PACKET_DATA_OFFSET_HEADER_TOTAL)
#define THROUGHPUT_LO 130
#define PACKET_SIZE_HI (1472-(PACKET_DATA_OFFSET_HEADER_TOTAL+28))
#define THROUGHPUT_HI 1000
//#define TRANSFER_QUEUE_LIMIT 10
#define PACKET_IN 1
#define PACKET_OUT 2

// Some global variables
/** the (global) name of the heartbeat structure */
static beatstruc cbeat;
/** the (global) name of the cosmos data structure */
static Agent *agent;
/** the (global) structure of sending channels */
typedef struct
{
    string node="";
    socket_channel chansock;
    string chanip="";
    PACKET_CHUNK_SIZE_TYPE packet_size=PACKET_SIZE_HI;
    uint32_t throughput=THROUGHPUT_HI;
    double limjd;
    double lomjd;
    double nmjd;
    double fmjd;
    packet_struct_heartbeat heartbeat;
} channelstruc;

static PACKET_CHUNK_SIZE_TYPE default_packet_size=PACKET_SIZE_HI;
static uint32_t default_throughput=THROUGHPUT_HI;

static vector <channelstruc> out_comm_channel;

//Send and receive thread info
void send_loop() noexcept;
void recv_loop() noexcept;
void transmit_loop() noexcept;
static ElapsedTime tet;
double logstride_sec = 10.;

static Transfer transfer;

// Mutexes to avoid thread collisions
static std::mutex txqueue_lock;
static std::mutex incoming_tx_lock;
static std::mutex outgoing_tx_lock;
static std::mutex debug_fd_lock;

// Counters to keep track of things
static uint32_t packet_in_count = 0;
static uint32_t packet_out_count;
static uint32_t crc_error_count = 0;
static uint32_t timeout_error_count = 0;
static uint32_t type_error_count = 0;
static uint32_t send_error_count = 0;
static uint32_t recv_error_count = 0;

// Function forward declarations
int32_t mysendto(string type, int32_t use_channel, vector<PACKET_BYTE>& buf);
int32_t myrecvfrom(string type, socket_channel &channel, PacketComm& buf, uint32_t length, double dtimeout=1.);
void debug_packet(PacketComm packet, uint8_t direction, string type, int32_t use_channel);


//main
int main(int argc, char *argv[])
{
    int32_t iretn;
    thread send_loop_thread;
    thread recv_loop_thread;
    thread transmit_loop_thread;

    if (static_cast<string>(argv[0]).find("slow") != string::npos)
    {
        default_throughput = THROUGHPUT_LO;
        default_packet_size = PACKET_SIZE_LO;
    }

    agent = new Agent("", "file", 0.);
    // Middle argument is debug mode
    if (argc > 1 && (argv[1][0] >= '0' && argv[1][0] <= '9'))
    {
        agent->set_debug_level(argv[1][0] - '0');
    }
    else
    {
        agent->set_debug_level(0);
    }
    if ((iretn = agent->wait()) < 0)
    {
        agent->debug_error.Printf("%.4f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    else
    {
        agent->debug_error.Printf("%.4f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
    }

    agent->debug_error.Printf("%.4f Node: %s Agent: %s - Established\n", tet.split(), agent->nodeName.c_str(), agent->agentName.c_str());

    out_comm_channel.resize(1);
    if((iretn = socket_open(&out_comm_channel[0].chansock, NetworkType::UDP, "", AGENTRECVPORT, SOCKET_LISTEN, SOCKET_BLOCKING, 5000000)) < 0)
    {
        agent->debug_error.Printf("%.4f Main: Node: %s Agent: %s - Listening socket failure\n", tet.split(), agent->nodeName.c_str(), agent->agentName.c_str());
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
    out_comm_channel[0].throughput = default_throughput;
    out_comm_channel[0].packet_size = default_packet_size;
    agent->debug_error.Printf("%.4f Node: %s Agent: %s - Listening socket open\n", tet.split(), agent->nodeName.c_str(), agent->agentName.c_str());

    if (argc > 1 && ((argv[1][0] < '0' || argv[1][0] > '9') || argc == 3))
    {
        out_comm_channel.resize(2);
        out_comm_channel[1].node = argv[argc-1];
        out_comm_channel[1].throughput = default_throughput;
        vector <string> cargs = string_split(argv[argc-1], ":");
        switch (cargs.size())
        {
        case 3:
            out_comm_channel[1].throughput = stol(cargs[2]);
        case 2:
            out_comm_channel[1].chanip = cargs[1];
        default:
            out_comm_channel[1].node = cargs[0];
        }
        if((iretn = socket_open(&out_comm_channel[1].chansock, NetworkType::UDP, out_comm_channel[1].chanip.c_str(), AGENTRECVPORT, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) < 0)
        {
            agent->debug_error.Printf("%.4f Node: %s IP: %s - Sending socket failure\n", tet.split(), out_comm_channel[1].node.c_str(), out_comm_channel[1].chanip.c_str());
            agent->shutdown();
            exit (-errno);
        }
        out_comm_channel[1].nmjd = currentmjd();
        out_comm_channel[1].limjd = out_comm_channel[1].nmjd;
        out_comm_channel[1].lomjd = out_comm_channel[1].nmjd;
        out_comm_channel[1].fmjd = out_comm_channel[1].nmjd;
        out_comm_channel[1].packet_size = default_packet_size;
        agent->debug_error.Printf("%.4f Network: Old: %u %s %s %u\n", tet.split(), 1, out_comm_channel[1].node.c_str(), out_comm_channel[1].chanip.c_str(), ntohs(out_comm_channel[1].chansock.caddr.sin_port));
    
        logstride_sec = 600.; // longer logstride
    }

    // Initialize Transfer class
    iretn = transfer.Init(agent);
    if (iretn < 0)
    {
        agent->debug_error.Printf("%.4f Error initializing transfer class!\n", tet.split());
        agent->shutdown();
        exit (iretn);
    }

    // Perform initial load
    double nextdiskcheck = currentmjd(0.);
    iretn = transfer.outgoing_tx_load();
    cout << "initial load" << endl;
    if (iretn >= 0)
    {
        nextdiskcheck = currentmjd();
    }

    send_loop_thread = thread([=] { send_loop(); });
    recv_loop_thread = thread([=] { recv_loop(); });
    transmit_loop_thread = thread([=] { transmit_loop(); });

    double nextlog = currentmjd();
    double sleepsec;
    ElapsedTime etloop;
    etloop.start();


    vector<PacketComm> packets;

    // start the agent
    while(agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            COSMOS_SLEEP(1);
            continue;
        }

        if(nextdiskcheck < nextlog ) {
            sleepsec = 86400. * (nextdiskcheck - currentmjd());
        } else {
            sleepsec = 86400. * (nextlog - currentmjd());
        }
        if (sleepsec > 0.)
        {
            COSMOS_SLEEP((sleepsec));
        }

        if(currentmjd() > nextlog) {
            // write_queue_log(currentmjd(0.));
            nextlog = currentmjd(0.) + logstride_sec/86400.;
        }

        
        // for (beatstruc &i : agent->agent_list) {

        // }

        // Check for new files to transmit if queue is not full and check is not delayed
        if (currentmjd() > nextdiskcheck)
        {
            nextdiskcheck = currentmjd(0.) + 10./86400.;
            transfer.outgoing_tx_load();
            transfer.get_outgoing_packets(packets);
            cout << "packets.size(): " << packets.size() << endl;
            for(auto& packet : packets) {
                packet.SLIPOut();
                mysendto("outgoing", 1, packet.dataout);
            }
        }

    } // End WHILE Loop

    if (agent->get_debug_level())
    {
        //agent->debug_error.Printf("%.4f %.4f Main: Node: %s Agent: %s - Exiting\n", tet.split(), dt.lap(), agent->nodeName.c_str(), agent->agentName.c_str());
    }

    send_loop_thread.join();
    recv_loop_thread.join();
    transmit_loop_thread.join();
    //txq.clear();

    if (agent->get_debug_level())
    {
        //agent->debug_error.Printf("%.4f %.4f Main: Node: %s Agent: %s - Shutting down\n", tet.split(), dt.lap(), agent->nodeName.c_str(), agent->agentName.c_str());
    }

    agent->shutdown();

    exit (0);
}

void recv_loop() noexcept
{
    PacketComm p;
    int32_t nbytes = 0;
    socket_channel rchannel;

    while (agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            COSMOS_SLEEP(1);
            continue;
        }
        else
        {
            COSMOS_SLEEP(.001);
        }

        while (( nbytes = myrecvfrom("Incoming", rchannel, p, PACKET_MAX_LENGTH)) > 0)
        {
            cout << "received?" << endl;
        }
    }
}

void send_loop() noexcept
{
    while (agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            COSMOS_SLEEP(1);
            continue;
        }
    }
}

void transmit_loop() noexcept
{
    while (agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            COSMOS_SLEEP(1);
            continue;
        }
    }
}


int32_t mysendto(string type, int32_t use_channel, vector<PACKET_BYTE>& buf)
{
    int32_t iretn;
    double cmjd;

    if ((cmjd = currentmjd(0.)) < out_comm_channel[use_channel].nmjd)
    {
        COSMOS_SLEEP((86400. * (out_comm_channel[use_channel].nmjd - cmjd)));
    }

    iretn = sendto(out_comm_channel[use_channel].chansock.cudp, reinterpret_cast<const char*>(&buf[0]), buf.size(), 0, reinterpret_cast<sockaddr*>(&out_comm_channel[use_channel].chansock.caddr), sizeof(struct sockaddr_in));

    if (iretn >= 0)
    {
        ++packet_out_count;
        out_comm_channel[use_channel].lomjd = currentmjd();
        out_comm_channel[use_channel].nmjd = out_comm_channel[use_channel].lomjd + ((28+iretn) / (float)out_comm_channel[use_channel].throughput)/86400.;
        if (agent->get_debug_level())
        {
            ///debug_packet(buf, PACKET_OUT, type, use_channel);
        }
    }
    else
    {
        iretn = -errno;
        ++send_error_count;
    }

    return iretn;
}

int32_t myrecvfrom(string type, socket_channel &channel, PacketComm& packet, uint32_t length, double dtimeout)
{
    int32_t nbytes = 0;
    int32_t iretn;

    for (uint16_t i=0; i<out_comm_channel.size(); ++i)
    {
        nbytes = socket_recvfrom(out_comm_channel[i].chansock, packet.dataout, length);
        // Bytes received
        if (nbytes > 0)
        {
            iretn = packet.SLIPIn();
            // CRC check fail
            if (iretn < 0)
            {
                nbytes = GENERAL_ERROR_CRC;
                ++crc_error_count;
            }
            else
            {
                ++packet_in_count;
                if (agent->get_debug_level())
                {
                    debug_packet(packet, PACKET_IN, type, i);
                }
            }
            channel = out_comm_channel[i].chansock;
            return nbytes;
        }
        else if (nbytes < 0)
        {
            nbytes = -errno;
            ++recv_error_count;
        }
        else
        {
            nbytes = GENERAL_ERROR_INPUT;
            ++recv_error_count;
        }
    }

    return nbytes;
}

void debug_packet(PacketComm packet, uint8_t direction, string type, int32_t use_channel)
{
    static ElapsedTime dt;

    if (agent->get_debug_level())
    {
        debug_fd_lock.lock();

        string packet_node_name;
        string node_name = transfer.lookup_node_id_name(packet.data[0]);
        uint8_t node_id = transfer.check_node_id(packet.data[0]);
        switch (packet.type)
        {
        case PACKET_HEARTBEAT:
        case PACKET_REQQUEUE:
        case PACKET_QUEUE:
        case PACKET_REQMETA:
            packet_node_name = reinterpret_cast<char *>(&packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)]);
            break;
        case PACKET_METADATA:
        case PACKET_REQDATA:
        case PACKET_DATA:
        case PACKET_COMPLETE:
        case PACKET_CANCEL:
            break;
        }
        if (direction == PACKET_IN)
        {
            if (out_comm_channel[use_channel].node.empty())
            {
                if (!node_name.empty())
                {
                    agent->debug_error.Printf("%.4f %.4f RECV L %u R %u %s %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, packet.data.size());
                }
                else
                {
                    agent->debug_error.Printf("%.4f %.4f RECV L %u R %u Unknown %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, packet.data.size());
                }
            }
            else
            {
                agent->debug_error.Printf("%.4f %.4f RECV L %u R %u %s %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, out_comm_channel[use_channel].node.c_str(), out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, packet.data.size());
            }
        }
        else if (direction == PACKET_OUT)
        {
            if (out_comm_channel[use_channel].node.empty())
            {
                if (!node_name.empty())
                {
                    agent->debug_error.Printf("%.4f %.4f SEND L %u R %u %s %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, packet.data.size());
                }
                else
                {
                    agent->debug_error.Printf("%.4f %.4f SEND L %u R %u Unknown %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, packet.data.size());
                }
            }
            else
            {
                agent->debug_error.Printf("%.4f %.4f SEND L %u R %u %s %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, out_comm_channel[use_channel].node.c_str(), out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, packet.data.size());
            }
        }

        switch (packet.type)
        {
        case PACKET_METADATA:
            {
                string file_name(&packet.data[PACKET_METASHORT_OFFSET_FILE_NAME], &packet.data[PACKET_METASHORT_OFFSET_FILE_NAME+TRANSFER_MAX_FILENAME]);
                agent->debug_error.Printf("[METADATA] %u %u %s ", node_id, packet.data[PACKET_METASHORT_OFFSET_TX_ID], file_name.c_str());
                break;
            }
        case PACKET_DATA:
            {
                agent->debug_error.Printf("[DATA] %u %u %u %u ", node_id, packet.data[PACKET_DATA_OFFSET_TX_ID], packet.data[PACKET_DATA_OFFSET_CHUNK_START]+256U*(packet.data[PACKET_DATA_OFFSET_CHUNK_START+1]+256U*(packet.data[PACKET_DATA_OFFSET_CHUNK_START+2]+256U*packet.data[PACKET_DATA_OFFSET_CHUNK_START+3])), packet.data[PACKET_DATA_OFFSET_BYTE_COUNT]+256U*packet.data[PACKET_DATA_OFFSET_BYTE_COUNT+1]);
                break;
            }
        case PACKET_REQDATA:
            {
                agent->debug_error.Printf("[REQDATA] %u %u %u %u ", node_id, packet.data[PACKET_REQDATA_OFFSET_TX_ID], packet.data[PACKET_REQDATA_OFFSET_HOLE_START]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_START+1]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_START+2]+256U*packet.data[PACKET_REQDATA_OFFSET_HOLE_START+3])), packet.data[PACKET_REQDATA_OFFSET_HOLE_END]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_END+1]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_END+2]+256U*packet.data[PACKET_REQDATA_OFFSET_HOLE_END+3])));
                break;
            }
        case PACKET_COMPLETE:
            {
                agent->debug_error.Printf("[COMPLETE] %u %u ", node_id, packet.data[PACKET_COMPLETE_OFFSET_TX_ID]);
                break;
            }
        case PACKET_CANCEL:
            {
                agent->debug_error.Printf("[CANCEL] %u %u ", node_id, packet.data[PACKET_CANCEL_OFFSET_TX_ID]);
                break;
            }
        case PACKET_REQMETA:
            {
                agent->debug_error.Printf("[REQMETA] %u %s ", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)]);
                for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                    if (packet.data[PACKET_REQMETA_OFFSET_TX_ID+i])
                    {
                        agent->debug_error.Printf("%u ", packet.data[PACKET_REQMETA_OFFSET_TX_ID+i]);
                    }
                break;
            }
        case PACKET_REQQUEUE:
            {
                agent->debug_error.Printf("[REQQUEUE] %u %s ", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)]);
            }
            break;
        case PACKET_QUEUE:
            {
                agent->debug_error.Printf("[QUEUE] %u %s ", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)]);
                for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                    if (packet.data[PACKET_QUEUE_OFFSET_TX_ID+i])
                    {
                        agent->debug_error.Printf("%u ", packet.data[PACKET_QUEUE_OFFSET_TX_ID+i]);
                    }
            }
            break;
        case PACKET_HEARTBEAT:
            {
                agent->debug_error.Printf("[HEARTBEAT] %u %s %hu %u %u", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)], packet.data[PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD]
                        , packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+1]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+2]+256U*packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+3]))
                        , packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+1]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+2]+256U*packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+3])));
                break;
            }
        case PACKET_MESSAGE:
            {
                agent->debug_error.Printf("[MESSAGE] %u %hu %s", node_id, packet.data[PACKET_MESSAGE_OFFSET_LENGTH], &packet.data[PACKET_MESSAGE_OFFSET_BYTES]);
                break;
            }
        case PACKET_COMMAND:
            {
                agent->debug_error.Printf("[COMMAND] %u %hu %s", node_id, packet.data[PACKET_COMMAND_OFFSET_LENGTH], &packet.data[PACKET_COMMAND_OFFSET_BYTES]);
                break;
            }
        }
        agent->debug_error.Printf("\n");
        // fflush(agent->get_debug_fd());
        debug_fd_lock.unlock();
    }
}
