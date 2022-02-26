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
#include <atomic>

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

// Send and receive thread info
void recv_loop() noexcept;
void send_loop() noexcept;
static ElapsedTime tet;
static ElapsedTime dt;
double logstride_sec = 10.;

static Transfer transfer;

static vector<PacketComm> packets;
static std::atomic<bool> file_transfer_enabled(true);
static std::atomic<bool> file_transfer_respond(false);
static vector<PacketComm> send_queue;

// Mutexes to avoid thread collisions
static std::mutex transfer_mtx;
static std::mutex send_queue_lock;
static std::mutex out_comm_lock;
static std::mutex debug_fd_lock;

// Counters to keep track of things
static uint32_t packet_in_count = 0;
static uint32_t packet_out_count;
static uint32_t crc_error_count = 0;
//static uint32_t timeout_error_count = 0;
static uint32_t type_error_count = 0;
static uint32_t send_error_count = 0;
static uint32_t recv_error_count = 0;

// Function forward declarations
int32_t mysendto(int32_t use_channel, PacketComm& packet);
int32_t myrecvfrom(string type, socket_channel &channel, PacketComm& buf, uint32_t length, double dtimeout=1.);
void debug_packet(PacketComm packet, uint8_t direction, string type, int32_t use_channel);

// Agent request functions
int32_t request_ls(string &request, string &response, Agent *agent);
int32_t request_list_incoming(string &request, string &response, Agent *agent);
int32_t request_list_outgoing(string &request, string &response, Agent *agent);
int32_t request_remove_file(string &request, string &response, Agent *agent);
//int32_t request_send_file(string &request, string &response, Agent *agent);
int32_t request_set_enabled(string &request, string &response, Agent *agent);

// main loop
int main(int argc, char *argv[])
{
    int32_t iretn;
    thread recv_loop_thread;
    thread send_loop_thread;

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
    out_comm_channel[0].node = agent->nodeName; // TODO: consider this
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

    // Add agent requests
    if ((iretn=agent->add_request("remove_file",request_remove_file,"in|out tx_id", "removes file from indicated queue")))
        exit (iretn);
    //	if ((iretn=agent->add_request("send_file",request_send_file,"", "creates and sends metadata/data packets")))
    //		exit (iretn);
    if ((iretn=agent->add_request("ls",request_ls,"", "lists contents of directory")))
        exit (iretn);
    if ((iretn=agent->add_request("list_incoming",request_list_incoming,"", "lists contents incoming queue")))
        exit (iretn);
    if ((iretn=agent->add_request("list_outgoing",request_list_outgoing,"", "lists contents outgoing queue")))
        exit (iretn);
    if ((iretn=agent->add_request("set_enabled",request_set_enabled,"node_id tx_id 0|1", "sets the enabled status of a tx_id")))
        exit (iretn);

    // Initialize Transfer class
    iretn = transfer.Init(agent->nodeName, &agent->debug_error);
    if (iretn < 0)
    {
        agent->debug_error.Printf("%.4f Error initializing transfer class!\n", tet.split());
        agent->shutdown();
        exit (iretn);
    }

    // Perform initial load
    double nextdiskcheck = currentmjd(0.);

    // Start send and recv threads
    recv_loop_thread = thread([=] { recv_loop(); });
    send_loop_thread = thread([=] { send_loop(); });

    ElapsedTime etloop;
    etloop.start();

    // Accumulate here before transfering to proper queues
    vector<PacketComm> file_packets;
    double diskcheckwait = 10./86400.;

    while(agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            secondsleep(1);
            continue;
        }
        
        // Check outgoing directories for new files
        if (currentmjd() > nextdiskcheck)
        {
            transfer_mtx.lock();
			for (size_t i = 1; i < out_comm_channel.size(); ++i)
			{
            	iretn = transfer.outgoing_tx_load(out_comm_channel[i].node);
			}
            transfer_mtx.unlock();
            nextdiskcheck = currentmjd(0.) + diskcheckwait;
        }

        // Check if any response-type packets need to be pushed
        if (file_transfer_respond.load())
        {
            for (size_t i = 1; i < out_comm_channel.size(); ++i)
            {
                transfer_mtx.lock();
                iretn = transfer.get_outgoing_rpackets(out_comm_channel[i].node, file_packets);
                transfer_mtx.unlock();
                if (iretn < 0 && agent->get_debug_level())
                {
                    agent->debug_error.Printf("%16.10f Error in get_outgoing_rpackets: %d\n", currentmjd(), iretn);
                }
            }
            file_transfer_respond.store(false);
        }

        // Get our own files' transfer packets if transfer is enabled
        if (file_transfer_enabled.load())
        {
            // Perform runs of file packet grabbing
            for (size_t i = 1; i < out_comm_channel.size(); ++i)
            {
                transfer_mtx.lock();
                iretn = transfer.get_outgoing_lpackets(out_comm_channel[i].node, file_packets);
                transfer_mtx.unlock();
                if (iretn < 0 && agent->get_debug_level())
                {
                    agent->debug_error.Printf("%16.10f Error in get_outgoing_lpackets: %d\n", currentmjd(), iretn);
                }
            }
        }
        
        if (!file_packets.size())
        {
            secondsleep(1.);
            continue;
        }

        // Transfer to net radio queue, for now
        send_queue_lock.lock();
        for (auto &packet : file_packets)
        {
            send_queue.push_back(packet);
        }
        send_queue_lock.unlock();

        // Don't forget to clear the vector before next use!
        file_packets.clear();

        secondsleep(.0001);
    }

    if (agent->get_debug_level())
    {
        agent->debug_error.Printf("%.4f %.4f Main: Node: %s Agent: %s - Exiting\n", tet.split(), dt.lap(), agent->nodeName.c_str(), agent->agentName.c_str());
    }

    recv_loop_thread.join();
    send_loop_thread.join();

    if (agent->get_debug_level())
    {
        agent->debug_error.Printf("%.4f %.4f Main: Node: %s Agent: %s - Shutting down\n", tet.split(), dt.lap(), agent->nodeName.c_str(), agent->agentName.c_str());
    }

    agent->shutdown();

    exit (0);
}

// Receive loop
void recv_loop() noexcept
{
    PacketComm p;
    int32_t nbytes = 0;
    socket_channel rchannel;
    int32_t iretn;

    while (agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            secondsleep(1);
            continue;
        }
        else
        {
            secondsleep(.001);
        }

        while (( nbytes = myrecvfrom("Incoming", rchannel, p, PACKET_MAX_LENGTH)) > 0)
        {
            transfer_mtx.lock();
            iretn = transfer.receive_packet(p);
            transfer_mtx.unlock();
            if (iretn == transfer.RESPONSE_REQUIRED)
            {
                file_transfer_respond.store(true);
            }

            if (iretn < 0) {
                if (agent->get_debug_level())
                {
                    agent->debug_error.Printf("%.4f %.4f Main: Node: %s Agent: %s - Error in receive_packet(): %d\n", tet.split(), dt.lap(), agent->nodeName.c_str(), agent->agentName.c_str(), iretn);
                }
                if (iretn == COSMOS_PACKET_TYPE_MISMATCH)
                {
                    // For debug logging
                    ++type_error_count;
                }
            }
            
            if (iretn >= 0)
            {
                // TODO: consider this section. If the incoming node id is the receiver's,
                // and that is us, the one receiving, and no info about the sender is given,
                // how should we determine what to add or who/where it's coming from?
                /*out_comm_lock.lock();
                // If packet is successfully received, check channels and update
                // information if we are already handling it, otherwise add the new channel.
                bool new_channel = true;
                for (std::vector<channelstruc>::size_type i=0; i<out_comm_channel.size(); ++i)
                {
                    // Are we handling this Node?
                    if (out_comm_channel[i].node == node_name)
                    {
                        //out_comm_channel[i].chansock = rchannel; // this causes some minor issues
                        out_comm_channel[i].chanip = out_comm_channel[i].chansock.address;
                        out_comm_channel[i].limjd = currentmjd();
                        new_channel = false;
                        break;
                    }
                }
                if (new_channel)
                {
                    channelstruc tchannel;
                    tchannel.node = node_name;
                    tchannel.nmjd = currentmjd();
                    tchannel.limjd = tchannel.nmjd;
                    tchannel.lomjd = tchannel.nmjd;
                    tchannel.fmjd = tchannel.nmjd;
                    tchannel.chansock = rchannel;
                    tchannel.chanip = tchannel.chansock.address;
                    out_comm_channel.push_back(tchannel);
                    if (agent->get_debug_level())
                    {
                        agent->debug_error.Printf("%.4f %.4f agent_file5: main loop: Adding new node:IP %s:%.17s\n", tet.split(), dt.lap(), node_name.c_str(), tchannel.chansock.address);
                    }
                }
                out_comm_lock.unlock();*/
            }
        }
    }
}

// Transmit loop
void send_loop() noexcept
{
    PacketComm p;
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
            COSMOS_SLEEP(1);
        }

        // Send out packets to the node
        send_queue_lock.lock();
        for(auto& packet : send_queue) {
            packet.SLIPPacketize();
            // TODO: reimplement dynamic sendy thingy
            mysendto(1, packet);
        }
        send_queue.clear();
        send_queue_lock.unlock();

    }
}

//! Send a packet out on a network socket.
//! Make sure to use SLIPOut or equivalent on packet first.
//! \param use_channel Index into out_comm_channel, which contains socket channel information necessary
//! \param packet A PacketComm packet containing data to send out. Make sure to use SLIPOut() or equivalent first
//! \return Non-negative on success
int32_t mysendto(int32_t use_channel, PacketComm& packet)
{
    int32_t iretn;
    double cmjd;

    if ((cmjd = currentmjd(0.)) < out_comm_channel[use_channel].nmjd)
    {
        secondsleep((86400. * (out_comm_channel[use_channel].nmjd - cmjd)));
    }

    iretn = sendto(out_comm_channel[use_channel].chansock.cudp, reinterpret_cast<const char*>(&packet.packetized[0]), packet.packetized.size(), 0, reinterpret_cast<sockaddr*>(&out_comm_channel[use_channel].chansock.caddr), sizeof(struct sockaddr_in));

    if (iretn >= 0)
    {
        ++packet_out_count;
        out_comm_channel[use_channel].lomjd = currentmjd();
        out_comm_channel[use_channel].nmjd = out_comm_channel[use_channel].lomjd + ((28+iretn) / (float)out_comm_channel[use_channel].throughput)/86400.;
        if (agent->get_debug_level())
        {
            debug_packet(packet, PACKET_OUT, "Outgoing", use_channel);
        }
    }
    else
    {
        iretn = -errno;
        ++send_error_count;
    }

    return iretn;
}

//! Listen for incoming packets on all defined channels.
//! Iterates over out_comm_channel and attempts to listen on each.
//! \param type Accepts either "incoming" or "outgoing". Only used for debugging purposes
//! \param channel Reference to a socket_channel to copy over information of the channel that the packet was obtained over
//! \param length Maximum length of packet
//! \param dtimeout Time to listen over channel, in seconds
//! \return Number of bytes received if success
int32_t myrecvfrom(string type, socket_channel &channel, PacketComm& packet, uint32_t length, double dtimeout)
{
    int32_t nbytes = 0;
    int32_t iretn;

    for (uint16_t i=0; i<out_comm_channel.size(); ++i)
    {
        nbytes = socket_recvfrom(out_comm_channel[i].chansock, packet.packetized, length);
        // Bytes received
        if (nbytes > 0)
        {
            iretn = packet.SLIPUnPacketize();
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

//! For printing out debug statements about incoming and outgoing packets.
//! \param packet An incoming or outgoing packet
//! \param direction PACKET_IN or PACKET_OUT
//! \param type Incoming or outgoing, used only in the print statement
//! \param use_channel Index into out_comm_channel
//! \return n/a
void debug_packet(PacketComm packet, uint8_t direction, string type, int32_t use_channel)
{
    if (agent->get_debug_level())
    {
        debug_fd_lock.lock();

        string node_name = NodeData::lookup_node_id_name(packet.data[0]);
        uint8_t node_id = NodeData::check_node_id(packet.data[0]);
        
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

        switch (packet.header.type)
        {
        case PacketComm::TypeId::FileMetaData:
            {
                string file_name(&packet.data[PACKET_METASHORT_OFFSET_FILE_NAME], &packet.data[PACKET_METASHORT_OFFSET_FILE_NAME+TRANSFER_MAX_FILENAME]);
                agent->debug_error.Printf("[METADATA] %u %u %s ", node_id, packet.data[PACKET_METASHORT_OFFSET_TX_ID], file_name.c_str());
                break;
            }
        case PacketComm::TypeId::FileChunkData:
            {
                agent->debug_error.Printf("[DATA] %u %u %u %u ", node_id, packet.data[PACKET_DATA_OFFSET_TX_ID], packet.data[PACKET_DATA_OFFSET_CHUNK_START]+256U*(packet.data[PACKET_DATA_OFFSET_CHUNK_START+1]+256U*(packet.data[PACKET_DATA_OFFSET_CHUNK_START+2]+256U*packet.data[PACKET_DATA_OFFSET_CHUNK_START+3])), packet.data[PACKET_DATA_OFFSET_BYTE_COUNT]+256U*packet.data[PACKET_DATA_OFFSET_BYTE_COUNT+1]);
                break;
            }
        case PacketComm::TypeId::FileReqData:
            {
                agent->debug_error.Printf("[REQDATA] %u %u %u %u ", node_id, packet.data[PACKET_REQDATA_OFFSET_TX_ID], packet.data[PACKET_REQDATA_OFFSET_HOLE_START]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_START+1]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_START+2]+256U*packet.data[PACKET_REQDATA_OFFSET_HOLE_START+3])), packet.data[PACKET_REQDATA_OFFSET_HOLE_END]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_END+1]+256U*(packet.data[PACKET_REQDATA_OFFSET_HOLE_END+2]+256U*packet.data[PACKET_REQDATA_OFFSET_HOLE_END+3])));
                break;
            }
        case PacketComm::TypeId::FileReqComplete:
            {
                agent->debug_error.Printf("[REQCOMPLETE] %u %u ", node_id, packet.data[PACKET_REQCOMPLETE_OFFSET_TX_ID]);
                break;
            }
        case PacketComm::TypeId::FileComplete:
            {
                agent->debug_error.Printf("[COMPLETE] %u %u ", node_id, packet.data[PACKET_COMPLETE_OFFSET_TX_ID]);
                break;
            }
        case PacketComm::TypeId::FileCancel:
            {
                agent->debug_error.Printf("[CANCEL] %u %u ", node_id, packet.data[PACKET_CANCEL_OFFSET_TX_ID]);
                break;
            }
        case PacketComm::TypeId::FileReqMeta:
            {
                agent->debug_error.Printf("[REQMETA] %u %s ", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)]);
                for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                    if (packet.data[PACKET_REQMETA_OFFSET_TX_ID+i])
                    {
                        agent->debug_error.Printf("%u ", packet.data[PACKET_REQMETA_OFFSET_TX_ID+i]);
                    }
                break;
            }
        case PacketComm::TypeId::FileQueue:
            {
                agent->debug_error.Printf("[QUEUE] %u %s ", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)]);
                // Note: this assumes that PACKET_QUEUE_FLAGS_TYPE is a uint16_t type
                for (PACKET_QUEUE_FLAGS_TYPE i=0; i<PACKET_QUEUE_FLAGS_LIMIT; ++i)
                {
                    PACKET_QUEUE_FLAGS_TYPE flags = uint16from(&packet.data[PACKET_QUEUE_OFFSET_TX_ID+(2*i)], ByteOrder::LITTLEENDIAN);
                    agent->debug_error.Printf("%u ", flags);
                }
            }
            break;
        case PacketComm::TypeId::FileHeartbeat:
            {
                agent->debug_error.Printf("[HEARTBEAT] %u %s %hu %u %u", node_id, &packet.data[COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)], packet.data[PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD]
                        , packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+1]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+2]+256U*packet.data[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+3]))
                        , packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+1]+256U*(packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+2]+256U*packet.data[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+3])));
                break;
            }
        case PacketComm::TypeId::FileMessage:
            {
                agent->debug_error.Printf("[MESSAGE] %u %hu %s", node_id, packet.data[PACKET_MESSAGE_OFFSET_LENGTH], &packet.data[PACKET_MESSAGE_OFFSET_BYTES]);
                break;
            }
        case PacketComm::TypeId::FileCommand:
            {
                agent->debug_error.Printf("[COMMAND] %u %hu %s", node_id, packet.data[PACKET_COMMAND_OFFSET_LENGTH], &packet.data[PACKET_COMMAND_OFFSET_BYTES]);
                break;
            }
        default:
            {
                agent->debug_error.Printf("[ERROR] %u %s", node_id, "Error in debug_packet switch on packet.header.type");
            }
        }
        agent->debug_error.Printf("\n");
        // fflush(agent->get_debug_fd());
        debug_fd_lock.unlock();
    }
}

// Agent request functions

int32_t request_ls(string &request, string &response, Agent *agent)
{

    //the request string == "ls directoryname"
    //get the directory name
    //    char directoryname[COSMOS_MAX_NAME+1];
    //    memmove(directoryname, request.substr(3), COSMOS_MAX_NAME);
    /*string directoryname = request.substr(3);

    DIR* dir;
    struct dirent* ent;

    string all_file_names;

    if((dir = opendir(directoryname.c_str())) != nullptr)
    {
        while (( ent = readdir(dir)) != nullptr)
        {
            all_file_names += ent->d_name;
            all_file_names += "\n";
        }
        closedir(dir);

        (response = all_file_names.c_str());
    }
    else
        response =  "unable to open directory " + directoryname;*/
    return 0;
}

//! Request a list of all files in the incoming queue.
//! Returns a json string list of incoming files, with the following keys:
//! - tx_id: The transaction ID
//! - file_name: Name of the file
//! - file_size: Size of the full file (in bytes)
//! - total_bytes: Total bytes received so far
//! - sent_meta: Whether the meta has been received for this file
int32_t request_list_incoming(string &request, string &response, Agent *agent)
{
    response.clear();
    response = transfer.list_incoming();

    return 0;
}

//! Request a list of all files in the outgoing queue.
//! Returns a json string list of outgoing files, with the following keys:
//! - tx_id: The transaction ID
//! - file_name: Name of the file
//! - file_size: Size of the file (in bytes)
//! - node_name: The name of the node to send to
//! - enabled: Whether the file is marked for transfer
int32_t request_list_outgoing(string &request, string &response, Agent *agent)
{
    response.clear();
    response = transfer.list_outgoing();

    return 0;
}

int32_t request_remove_file(string &request, string &response, Agent *agent)
{
    /*char type;
    uint32_t tx_id;

    sscanf(request.c_str(), "%*s %c %u\n", &type, &tx_id);
    switch (type)
    {
    case 'i':
        {
            break;
        }
    case 'o':
        {
            break;
        }
    }*/

    return 0;
}

//! Set the enabled status of an outgoing file transfer
int32_t request_set_enabled(string &request, string &response, Agent *agent)
{
    int32_t iretn = 0;
    uint8_t node_id;
    PACKET_TX_ID_TYPE tx_id;
    int enabled;

    if (sscanf(request.c_str(), "%*s %d %d %d", &node_id, &tx_id, &enabled) == 3)
    {
        iretn = transfer.set_enabled(node_id, tx_id, enabled);
    }

    return iretn;
}
