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
//! \defgroup agent_file File Transfer Agent program
//! Manages file transfers and message passing between COSMOS Nodes.
//!
//! Both file and message transfers are handled as an interchange of packets
//! between two instances of agent_file.
//!
//! Usage: agent_file chanip_address_lo [chanip_address_hi]


#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "support/jsonlib.h"
#include "support/transferlib.h"
#include "support/sliplib.h"
#include "support/jsonobject.h"

#include <algorithm>
#include <cstring>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#if !defined(COSMOS_WIN_OS)
#include <sys/select.h>
#endif

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

#ifdef COSMOS_CYGWIN_OS
#include<sstream>
template <typename T>
string std::to_string(T value)
{
    //create an output string stream
    std::ostringstream os ;
    //throw the value into the string stream, assuming std::stringstream
    //handles it
    os << value ;
    //convert the string stream into a string and return
    return os.str() ;
}
#endif


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
//    double send_queue;
//    double send_reqmeta;
//    double send_reqdata;
//    double send_complete;
} channelstruc;

static PACKET_CHUNK_SIZE_TYPE default_packet_size=PACKET_SIZE_HI;
static uint32_t default_throughput=THROUGHPUT_HI;

static vector <channelstruc> out_comm_channel;

typedef struct
{
    string type;
    uint32_t channel;
    double nmjd;
    double time_step;
    vector<PACKET_BYTE> packet;
} transmit_queue_entry;

static std::queue<transmit_queue_entry> transmit_queue;
//static std::condition_variable transmit_queue_check;


//Send and receive thread info
void send_loop() noexcept;
void recv_loop() noexcept;
void transmit_loop() noexcept;
void profile_check(int32_t line_number, uint16_t thread=0);
static ElapsedTime tet;

// Mutexes to avoid thread collisions
static std::mutex txqueue_lock;
static std::mutex incoming_tx_lock;
static std::mutex outgoing_tx_lock;
static std::mutex debug_fd_lock;

static double last_data_receive_time = 0.;
//static double next_reqmeta_time = 0.;

// Counters to keep track of things
static uint32_t packet_in_count = 0;
static uint32_t packet_out_count;
static uint32_t crc_error_count = 0;
static uint32_t timeout_error_count = 0;
static uint32_t type_error_count = 0;
static uint32_t send_error_count = 0;
static uint32_t recv_error_count = 0;

typedef struct
{
    bool activity = false;
    bool sendqueue = false;
    bool sentqueue = false;
    bool rcvdqueue = false;
    bool rcvdmeta = false;
    bool rcvddata = false;
    PACKET_TX_ID_TYPE size;
    PACKET_TX_ID_TYPE next_id;
    string node_name="";
    tx_progress progress[PROGRESS_QUEUE_SIZE];
    double heartbeatclock = 0.;
    double reqmetaclock = 0.;
    double reqdataclock = 0.;
}	tx_entry;

typedef struct
{
    string node_name="";
    PACKET_NODE_ID_TYPE node_id;
    tx_entry incoming;
    tx_entry outgoing;
} tx_queue;

static vector<tx_queue> txq;

static string log_directory = "temp";
double logstride_sec = 10.;
ElapsedTime dt;

int32_t request_send_command(string &request, string &response, Agent *);
int32_t request_send_message(string &request, string &response, Agent *);
int32_t request_debug(string &request, string &, Agent *);
int32_t request_get_channels(string &request, string &response, Agent *agent);
int32_t request_set_throughput(string &request, string &response, Agent *agent);
int32_t request_remove_file(string &request, string &response, Agent *agent);
//int32_t request_send_file(string &request, string &response, Agent *agent);
int32_t request_ls(string &request, string &response, Agent *agent);
int32_t request_list_incoming(string &request, string &response, Agent *agent);
int32_t request_list_outgoing(string &request, string &response, Agent *agent);
int32_t request_list_incoming_json(string &request, string &response, Agent *agent);
int32_t request_list_outgoing_json(string &request, string &response, Agent *agent);
int32_t request_set_logstride(string &request, string &response, Agent *agent);
int32_t request_get_logstride(string &, string &response, Agent *);
int32_t outgoing_tx_add(tx_progress &tx_out);
int32_t outgoing_tx_add(string node_name, string agent_name, string file_name);
int32_t outgoing_tx_del(uint8_t node, uint16_t tx_id=PROGRESS_QUEUE_SIZE, bool remove_file=true);
int32_t outgoing_tx_purge(uint8_t node, uint16_t tx_id=PROGRESS_QUEUE_SIZE);
int32_t outgoing_tx_recount(uint8_t node);
int32_t outgoing_tx_load(uint8_t node);
int32_t incoming_tx_add(tx_progress &tx_in);
int32_t incoming_tx_add(string node_name, PACKET_TX_ID_TYPE tx_id);
int32_t incoming_tx_update(packet_struct_metashort meta);
int32_t incoming_tx_del(uint8_t node, uint16_t tx_id=PROGRESS_QUEUE_SIZE);
int32_t incoming_tx_purge(uint8_t node, uint16_t tx_id=PROGRESS_QUEUE_SIZE);
int32_t incoming_tx_recount(uint8_t node);
vector<file_progress> find_chunks_missing(tx_progress& tx);
vector<file_progress> find_chunks_togo(tx_progress& tx);
PACKET_FILE_SIZE_TYPE merge_chunks_overlap(tx_progress& tx);
double queuecheck(PACKET_NODE_ID_TYPE node_id);
int32_t queuesendto(PACKET_NODE_ID_TYPE node_id, string type, vector<PACKET_BYTE> packet);
int32_t mysendto(string type, int32_t use_channel, vector<PACKET_BYTE>& buf);
int32_t myrecvfrom(string type, socket_channel &channel, vector<PACKET_BYTE>& buf, uint32_t length, double dtimeout=1.);
void debug_packet(vector<PACKET_BYTE> buf, uint8_t direction, string type, int32_t use_channel);
int32_t write_meta(tx_progress& tx, double interval=5.);
int32_t read_meta(tx_progress& tx);
bool tx_progress_compare_by_size(const tx_progress& a, const tx_progress& b);
bool filestruc_smaller_by_size(const filestruc& a, const filestruc& b);
bool filestruc_larger_by_size(const filestruc& a, const filestruc& b);
PACKET_TX_ID_TYPE check_tx_id(tx_entry &txentry, PACKET_TX_ID_TYPE tx_id);
int32_t check_channel(PACKET_NODE_ID_TYPE node_id);
int32_t add_node_name(string node_name);
PACKET_TX_ID_TYPE choose_incoming_tx_id(uint8_t node_id);
PACKET_TX_ID_TYPE choose_outgoing_tx_id(uint8_t node_id);
int32_t next_incoming_tx(PACKET_NODE_ID_TYPE node, int32_t use_channel);
string json_list_incoming();
string json_list_outgoing();
string json_list_queue();
void write_queue_log(double logdate);
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
    if ((iretn = agent->wait()) < 0)
    {
        fprintf(agent->get_debug_fd(), "%.4f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    else
    {
        fprintf(agent->get_debug_fd(), "%.4f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
    }

    fprintf(agent->get_debug_fd(), "%.4f Node: %s Agent: %s - Established\n", tet.split(), agent->nodeName.c_str(), agent->agentName.c_str());
    fflush(agent->get_debug_fd()); // Ensure this gets printed before blocking call

    out_comm_channel.resize(1);
    if((iretn = socket_open(&out_comm_channel[0].chansock, NetworkType::UDP, "", AGENTRECVPORT, SOCKET_LISTEN, SOCKET_BLOCKING, 5000000)) < 0)
    {
        fprintf(agent->get_debug_fd(), "%.4f Main: Node: %s Agent: %s - Listening socket failure\n", tet.split(), agent->nodeName.c_str(), agent->agentName.c_str());
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
    fprintf(agent->get_debug_fd(), "%.4f Node: %s Agent: %s - Listening socket open\n", tet.split(), agent->nodeName.c_str(), agent->agentName.c_str());
    fflush(agent->get_debug_fd()); // Ensure this gets printed before blocking call

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
//        size_t tloc = out_comm_channel[1].node.find(':');
//        if (tloc != string::npos)
//        {
//            out_comm_channel[1].chanip = out_comm_channel[1].node.substr(tloc+1, out_comm_channel[1].node.size()-tloc+1);
//            out_comm_channel[1].node = out_comm_channel[1].node.substr(0, tloc);
//        }
        if((iretn = socket_open(&out_comm_channel[1].chansock, NetworkType::UDP, out_comm_channel[1].chanip.c_str(), AGENTRECVPORT, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) < 0)
        {
            fprintf(agent->get_debug_fd(), "%.4f Node: %s IP: %s - Sending socket failure\n", tet.split(), out_comm_channel[1].node.c_str(), out_comm_channel[1].chanip.c_str());
            agent->shutdown();
            exit (-errno);
        }
        out_comm_channel[1].nmjd = currentmjd();
        out_comm_channel[1].limjd = out_comm_channel[1].nmjd;
        out_comm_channel[1].lomjd = out_comm_channel[1].nmjd;
        out_comm_channel[1].fmjd = out_comm_channel[1].nmjd;
        out_comm_channel[1].packet_size = default_packet_size;
        fprintf(agent->get_debug_fd(), "%.4f Network: Old: %u %s %s %u\n", tet.split(), 1, out_comm_channel[1].node.c_str(), out_comm_channel[1].chanip.c_str(), ntohs(out_comm_channel[1].chansock.caddr.sin_port));
        fflush(agent->get_debug_fd());

        log_directory = "temp"; // put log files in node/outgoing/file
        logstride_sec = 600.; // longer logstride
    }

    // Initialize Transfer Queue
    if ((iretn = load_nodeids()) < 2)
    {
        fprintf(agent->get_debug_fd(), "%.4f Couldn't load node lookup table\n", tet.split());
        agent->shutdown();
        exit (iretn);
    }
    txq.resize(iretn);
    for (uint16_t i=1; i<iretn; ++i)
    {
        if (check_node_id(i) > 0)
        {
            txq[i].node_id = i;
            txq[i].node_name = lookup_node_id_name(i);
        }
    }

    if (argc > 1 && (argv[1][0] >= '0' && argv[1][0] <= '9'))
    {
        agent->debug_level = argv[1][0] - '0';
    }
    else
    {
        agent->debug_level = 0;
    }

    // Restore in progress transfers from previous run
    for (string node_name : data_list_nodes())
    {
//        int32_t node_id = add_node_name(node_name);

        for(filestruc file : data_list_files(node_name, "temp", "file"))
        {
            // Add entry for each meta file
            if (file.type == "meta")
            {
                // Incoming
                if (!file.name.compare(0,3,"in_"))
                {
                    tx_progress tx_in;
                    tx_in.temppath = file.path.substr(0,file.path.find(".meta"));
                    if (read_meta(tx_in) >= 0)
                    {
                        merge_chunks_overlap(tx_in);
                        iretn = incoming_tx_add(tx_in);
                    }
                }

                // Outgoing
                if (!file.name.compare(0,4,"out_"))
                {
                    tx_progress tx_out;
                    tx_out.temppath = file.path.substr(0,file.path.find(".meta"));
                    if (read_meta(tx_out) >= 0)
                    {
                        find_chunks_togo(tx_out);
                        iretn = outgoing_tx_add(tx_out);
                    }
                }
            }
        }
    }

    // add agent_file requests
    if ((iretn=agent->add_request("get_channels",request_get_channels,"", "get channel information")))
        exit (iretn);
    if ((iretn=agent->add_request("set_throughput",request_set_throughput,"{n} [throughput]", "set channel throughput")))
        exit (iretn);
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
    if ((iretn=agent->add_request("list_incoming_json",request_list_incoming_json,"", "lists contents incoming queue")))
        exit (iretn);
    if ((iretn=agent->add_request("list_outgoing_json",request_list_outgoing_json,"", "lists contents outgoing queue")))
        exit (iretn);
    if ((iretn=agent->add_request("set_logstride",request_set_logstride,"sec","set time interval of log files")))
        exit (iretn);
    if ((iretn=agent->add_request("get_logstride",request_get_logstride,"","get time interval of log files")))
        exit (iretn);
    if ((iretn=agent->add_request("debug",request_debug,"{0|1}","Toggle Debug information")))
        exit (iretn);
    if ((iretn=agent->add_request("command",request_send_command,"nodename command","Submit command in event format to agent_file on requested Node")))
        exit (iretn);
    if ((iretn=agent->add_request("message",request_send_message,"nodename message","Send message to agent_file on requested Node")))
        exit (iretn);

    double nextdiskcheck = currentmjd(0.);
    for (uint16_t node=0; node<txq.size(); ++node)
    {
        iretn = outgoing_tx_load(node);
        if (iretn >= 0)
        {
            nextdiskcheck = currentmjd();
        }
    }

    send_loop_thread = thread([=] { send_loop(); });
    recv_loop_thread = thread([=] { recv_loop(); });
    transmit_loop_thread = thread([=] { transmit_loop(); });

    double nextlog = currentmjd();
    double sleepsec;
    ElapsedTime etloop;
    etloop.start();

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
            write_queue_log(currentmjd(0.));
            nextlog = currentmjd(0.) + logstride_sec/86400.;
        }

        // Check for new files to transmit if queue is not full and check is not delayed


        if (currentmjd() > nextdiskcheck)
        {

            nextdiskcheck = currentmjd(0.) + 10./86400.;
            for (uint16_t node=0; node<txq.size(); ++node)
            {
                iretn = outgoing_tx_load(node);
//                if (iretn >= 0)
//                {
//                    nextdiskcheck = currentmjd();
//                }
            }
        }
    } // End WHILE Loop

    if (agent->debug_level)
    {
        fprintf(agent->get_debug_fd(), "%.4f %.4f Main: Node: %s Agent: %s - Exiting\n", tet.split(), dt.lap(), agent->nodeName.c_str(), agent->agentName.c_str());
        fflush(agent->get_debug_fd());
    }

    send_loop_thread.join();
    recv_loop_thread.join();
    //    transmit_queue_check.notify_one();
    transmit_loop_thread.join();
    txq.clear();

    if (agent->debug_level)
    {
        fprintf(agent->get_debug_fd(), "%.4f %.4f Main: Node: %s Agent: %s - Shutting down\n", tet.split(), dt.lap(), agent->nodeName.c_str(), agent->agentName.c_str());
        fflush(agent->get_debug_fd());
    }

    agent->shutdown();

    exit (0);
}

void recv_loop() noexcept
{
    vector<PACKET_BYTE> recvbuf;
    string partial_filepath;
    int32_t nbytes = 0;
    socket_channel rchannel;
    std::vector<channelstruc>::size_type use_channel = 0;

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

        while (( nbytes = myrecvfrom("Incoming", rchannel, recvbuf, PACKET_MAX_LENGTH)) > 0)
        {
            // Generate extra network info
            inet_ntop(rchannel.caddr.sin_family, &rchannel.caddr.sin_addr, rchannel.address, sizeof(rchannel.address));

            // Check channels, update information if we are already handling it, otherwise add channel
            string packet_node_name;
            string node_name = lookup_node_id_name(recvbuf[PACKET_HEADER_OFFSET_NODE_ID]);
            int32_t node_id = check_node_id(recvbuf[PACKET_HEADER_OFFSET_NODE_ID]);
            switch (recvbuf[0] & 0x0f)
            {
            case PACKET_HEARTBEAT:
            case PACKET_REQQUEUE:
            case PACKET_QUEUE:
            case PACKET_REQMETA:
                packet_node_name = reinterpret_cast<char *>(&recvbuf[PACKET_HEADER_OFFSET_NODE_NAME]);
                break;
            case PACKET_METADATA:
            case PACKET_REQDATA:
            case PACKET_DATA:
            case PACKET_COMPLETE:
            case PACKET_CANCEL:
                break;
            }

            if (node_id <= 0 || node_name.empty())
            {
                continue;
            }

            txq[node_id].incoming.rcvdqueue = false;
            txq[node_id].incoming.rcvdmeta = false;
            txq[node_id].incoming.rcvddata = false;
            incoming_tx_lock.unlock();

            use_channel = out_comm_channel.size();
            for (std::vector<channelstruc>::size_type i=0; i<out_comm_channel.size(); ++i)
            {
                // Are we handling this Node?
                if (out_comm_channel[i].node == node_name)
                {
                    out_comm_channel[i].chansock = rchannel;
                    out_comm_channel[i].chanip = out_comm_channel[i].chansock.address;
                    use_channel = i;
                    break;
                }
            }

            if (use_channel == out_comm_channel.size())
            {
                channelstruc tchannel;
                tchannel.node = node_name;
                tchannel.nmjd = currentmjd(0.);
                tchannel.limjd = tchannel.nmjd;
                tchannel.lomjd = tchannel.nmjd;
                tchannel.fmjd = tchannel.nmjd;
                tchannel.chansock = rchannel;
                inet_ntop(tchannel.chansock.caddr.sin_family, &tchannel.chansock.caddr.sin_addr, tchannel.chansock.address, sizeof(tchannel.chansock.address));
                tchannel.chanip = tchannel.chansock.address;
                use_channel = out_comm_channel.size();
                out_comm_channel.push_back(tchannel);
            }

            out_comm_channel[use_channel].limjd = currentmjd();

            // Respond appropriately according to type of packet
            switch (recvbuf[0] & 0x0f)
            {
            case PACKET_COMMAND:
                {
                packet_struct_command command;

                extract_command(recvbuf, command);

                FILE *fp = data_open(("/cosmos/nodes/" + agent->nodeName + "/incoming/exec/file.command").c_str(), "w");
                if (fp)
                {
                    fwrite(&command.bytes[0], 1, command.length, fp);
                    fclose(fp);
                }
                }
                break;
            case PACKET_MESSAGE:
                {
                    packet_struct_message message;

                    extract_message(recvbuf, message);

                    string imessage;
                    imessage.resize(message.length);
                    memcpy(&imessage[0], message.bytes, message.length);
                    log_write(lookup_node_id_name(message.node_id), "file", tet.split(), "", "message", imessage, "incoming");
                }
                break;
            case PACKET_HEARTBEAT:
                {
                    packet_struct_heartbeat heartbeat;

                    extract_heartbeat(recvbuf, heartbeat);

                    out_comm_channel[use_channel].heartbeat = heartbeat;
                    txq[node_id].outgoing.sendqueue = true;
                    txq[node_id].outgoing.sentqueue = false;
                }
                break;
            case PACKET_REQQUEUE:
                {
                    packet_struct_reqqueue reqqueue;

                    extract_reqqueue(recvbuf, reqqueue);

                    txq[node_id].outgoing.sendqueue = true;
                    txq[node_id].outgoing.sentqueue = false;
                }
                break;
            case PACKET_QUEUE:
                {
                    packet_struct_queue queue;

                    extract_queue(recvbuf, queue);

                    incoming_tx_lock.lock();

                    txq[node_id].incoming.sentqueue = true;
                    txq[node_id].incoming.sendqueue = false;
                    txq[(node_id)].incoming.rcvdqueue = true;

                    // Sort through incoming queue and remove anything not in sent queue
                    for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                    {
                        bool valid = false;
                        for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                        {
                            if (queue.tx_id[i] && txq[node_id].incoming.progress[tx_id].tx_id == queue.tx_id[i])
                            {
                                // This is an incoming file we should handle
                                valid = true;
                                break;
                            }
                        }

                        if (txq[node_id].incoming.progress[tx_id].tx_id && !valid)
                        {
                            // This is not an incoming file we should handle
                            incoming_tx_del(node_id, tx_id);
                        }
                    }

                    // Sort through remotely sent queue and add anything not in our local incoming queue
                    for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                    {
                        if (queue.tx_id[i])
                        {
                            PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].incoming, queue.tx_id[i]);

                            if (tx_id == 0)
                            {
                                incoming_tx_add(queue.node_name, queue.tx_id[i]);
                            }
                        }
                    }

                    incoming_tx_lock.unlock();
                }
                break;
                //Request missing metadata
            case PACKET_REQMETA:
                {
                    packet_struct_reqmeta reqmeta;

                    extract_reqmeta(recvbuf, reqmeta);

                    outgoing_tx_lock.lock();
                    txq[node_id].outgoing.sentqueue = false;

                    // Send requested META packets
                    if (txq[node_id].node_id > 0)
                    {
                        for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                        {
                            PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].outgoing, reqmeta.tx_id[i]);
                            if (tx_id > 0)
                            {
                                txq[node_id].outgoing.progress[tx_id].sendmeta = true;
                                txq[node_id].outgoing.progress[tx_id].sentmeta = false;
                            }
                        }
                    }
                    outgoing_tx_lock.unlock();
                    break;
                }
            case PACKET_METADATA:
                {
                    packet_struct_metashort meta;

                    extract_metadata(recvbuf, meta);

                    incoming_tx_lock.lock();

                    incoming_tx_update(meta);

                    incoming_tx_lock.unlock();

                    break;
                }
            case PACKET_REQDATA:
                {
                    packet_struct_reqdata reqdata;

                    extract_reqdata(recvbuf, reqdata);

                    // Simple validity check
                    if (reqdata.hole_end < reqdata.hole_start)
                    {
                        break;
                    }

                    outgoing_tx_lock.lock();

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].outgoing, reqdata.tx_id);
                    // tx_id now points to the valid entry to which we should add the data

                    if (tx_id > 0)
                    {
                        // Perform sanity check
//                        if (reqdata.hole_start >= txq[node_id].outgoing.progress[tx_id].file_size || reqdata.hole_end > txq[node_id].outgoing.progress[tx_id].file_size)
//                            {
//                                outgoing_tx_del(node_id, tx_id, false);
//                                continue;
//                            }
                        txq[node_id].outgoing.progress[tx_id].sendmeta = false;
                        txq[node_id].outgoing.progress[tx_id].sentmeta = true;

                        // Add this chunk to the queue
                        file_progress tp;
                        tp.chunk_start = reqdata.hole_start;
                        tp.chunk_end = reqdata.hole_end;
                        PACKET_FILE_SIZE_TYPE byte_count = (reqdata.hole_end - reqdata.hole_start) + 1;

                        uint32_t check=0;
                        // Anything in the queue yet?
                        if (!txq[node_id].outgoing.progress[tx_id].file_info.size())
                        {
                            // Add first entry
                            txq[node_id].outgoing.progress[tx_id].file_info.push_back(tp);
                            txq[node_id].outgoing.progress[tx_id].total_bytes += byte_count;
                        }
                        else
                        {
                            // Check against existing data
                            for (uint32_t j=0; j<txq[node_id].outgoing.progress[tx_id].file_info.size(); ++j)
                            {
                                // If we match this entry
                                if (tp.chunk_start == txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_start && tp.chunk_end == txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_end)
                                {
                                    break;
                                }
                                // If we start before this entry
                                if (tp.chunk_start < txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_start)
                                {
                                    // If we end before this entry (at least one byte between), insert
                                    if (tp.chunk_end + 1 < txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_start)
                                    {
                                        txq[node_id].outgoing.progress[tx_id].file_info.insert(txq[node_id].outgoing.progress[tx_id].file_info.begin()+j, tp);
                                        txq[node_id].outgoing.progress[tx_id].total_bytes += byte_count;
                                        break;
                                    }
                                    // Otherwise, extend the near end
                                    else
                                    {
                                        tp.chunk_end = txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_start - 1;
                                        txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_start = tp.chunk_start;
                                        byte_count = (tp.chunk_end - tp.chunk_start) + 1;
                                        txq[node_id].outgoing.progress[tx_id].total_bytes += byte_count;
                                        break;
                                    }
                                }
                                else
                                {
                                    // If we overlap on the end, extend the far end
                                    if (tp.chunk_start <= txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_end + 1)
                                    {
                                        if (tp.chunk_end > txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_end)
                                        {
                                            byte_count = tp.chunk_end - txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_end;
                                            tp.chunk_start = txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_end + 1;
                                            txq[node_id].outgoing.progress[tx_id].file_info[j].chunk_end = tp.chunk_end;
                                            txq[node_id].outgoing.progress[tx_id].total_bytes += byte_count;
                                            break;
                                        }
                                    }
                                }
                                check = j + 1;
                            }


                            // If we are higher than everything currently in the list, then append
                            if (check == txq[node_id].outgoing.progress[tx_id].file_info.size())
                            {
                                txq[node_id].outgoing.progress[tx_id].file_info.push_back(tp);
                                txq[node_id].outgoing.progress[tx_id].total_bytes += byte_count;
                            }

                        }

                        // Save meta to disk
                        write_meta(txq[node_id].outgoing.progress[tx_id]);
                        txq[node_id].outgoing.sendqueue = false;
                        txq[node_id].outgoing.sentqueue = true;
                        txq[node_id].outgoing.progress[tx_id].senddata = true;
                        txq[node_id].outgoing.progress[tx_id].sentdata = false;
                        txq[node_id].outgoing.progress[tx_id].sendmeta = false;
                        txq[node_id].outgoing.progress[tx_id].sentmeta = true;
                    }

                    outgoing_tx_lock.unlock();
                    break;
                }
            case PACKET_DATA:
                {
                    packet_struct_data data;

                    extract_data(recvbuf, data.node_id, data.tx_id, data.byte_count, data.chunk_start, data.chunk);

                    last_data_receive_time = currentmjd();

                    // create transaction entry if new, and then add data

                    incoming_tx_lock.lock();

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].incoming, data.tx_id);

                    // Update corresponding incoming queue entry if it exists
                    if (tx_id > 0)
                    {
                        txq[node_id].incoming.rcvddata = true;
                        txq[node_id].incoming.progress[tx_id].datatime = currentmjd();

                        // tx_id now points to the valid entry to which we should add the data
                        file_progress tp;
                        tp.chunk_start = data.chunk_start;
                        tp.chunk_end = data.chunk_start + data.byte_count - 1;

                        packet_struct_data odata;
                        odata = data;

                        uint32_t check=0;
                        bool duplicate = false;
                        bool updated = false;

                        // Do we have any data yet?
                        if (!txq[node_id].incoming.progress[tx_id].file_info.size())
                        {
                            // Add first entry, then write data
                            txq[node_id].incoming.progress[tx_id].file_info.push_back(tp);
                            txq[node_id].incoming.progress[tx_id].total_bytes += data.byte_count;
                            updated = true;
                        }
                        else
                        {
                            // Check against existing data
                            for (uint32_t j=0; j<txq[node_id].incoming.progress[tx_id].file_info.size(); ++j)
                            {
                                // Check for duplicate
                                if (tp.chunk_start >= txq[node_id].incoming.progress[tx_id].file_info[j].chunk_start && tp.chunk_end <= txq[node_id].incoming.progress[tx_id].file_info[j].chunk_end)
                                {
                                    duplicate = true;
                                    break;
                                }
                                // If we start before this entry
                                if (tp.chunk_start < txq[node_id].incoming.progress[tx_id].file_info[j].chunk_start)
                                {
                                    // If we end before this entry (at least one byte between), insert
                                    if (tp.chunk_end + 1 < txq[node_id].incoming.progress[tx_id].file_info[j].chunk_start)
                                    {
                                        txq[node_id].incoming.progress[tx_id].file_info.insert(txq[node_id].incoming.progress[tx_id].file_info.begin()+j, tp);
                                        txq[node_id].incoming.progress[tx_id].total_bytes += data.byte_count;
                                        updated = true;
                                        break;
                                    }
                                    // Otherwise, extend the near end
                                    else
                                    {
                                        tp.chunk_end = txq[node_id].incoming.progress[tx_id].file_info[j].chunk_start - 1;
                                        txq[node_id].incoming.progress[tx_id].file_info[j].chunk_start = tp.chunk_start;
                                        data.byte_count = (tp.chunk_end - tp.chunk_start) + 1;
                                        txq[node_id].incoming.progress[tx_id].total_bytes += data.byte_count;
                                        updated = true;
                                        break;
                                    }
                                }
                                else
                                {
                                    // If we overlap on the end, extend the far end
                                    if (tp.chunk_start <= txq[node_id].incoming.progress[tx_id].file_info[j].chunk_end + 1)
                                    {
                                        if (tp.chunk_end > txq[node_id].incoming.progress[tx_id].file_info[j].chunk_end)
                                        {
                                            data.byte_count = tp.chunk_end - txq[node_id].incoming.progress[tx_id].file_info[j].chunk_end;
                                            tp.chunk_start = txq[node_id].incoming.progress[tx_id].file_info[j].chunk_end + 1;
                                            txq[node_id].incoming.progress[tx_id].file_info[j].chunk_end = tp.chunk_end;
                                            txq[node_id].incoming.progress[tx_id].total_bytes += data.byte_count;
                                            updated = true;
                                            break;
                                        }
                                    }
                                }
                                check = j + 1;
                            }


                            // If we are higher than everything currently in the list, then append
                            if (!duplicate && check == txq[node_id].incoming.progress[tx_id].file_info.size())
                            {
                                txq[node_id].incoming.progress[tx_id].file_info.push_back(tp);
                                txq[node_id].incoming.progress[tx_id].total_bytes += data.byte_count;
                                updated = true;
                            }

                        }

                        // Write to disk if this is new data
                        if (updated)
                        {
                            // Write incoming data to disk
                            if (txq[node_id].incoming.progress[tx_id].fp == nullptr)
                            {
                                partial_filepath = txq[node_id].incoming.progress[tx_id].temppath + ".file";
                                if (data_exists(partial_filepath))
                                {
                                    txq[node_id].incoming.progress[tx_id].fp = fopen(partial_filepath.c_str(), "r+");
                                }
                                else
                                {
                                    txq[node_id].incoming.progress[tx_id].fp = fopen(partial_filepath.c_str(), "w");
                                }
                            }

                            if (txq[node_id].incoming.progress[tx_id].fp == nullptr)
                            {
                                if (agent->debug_level)
                                {
                                    debug_fd_lock.lock();
                                    fprintf(agent->get_debug_fd(), "%.4f %.4f Incoming: File Error: %s %s on ID: %u Chunk: %u\n", tet.split(), dt.lap(), partial_filepath.c_str(), cosmos_error_string(-errno).c_str(), tx_id, tp.chunk_start);
                                    fflush(agent->get_debug_fd());
                                    debug_fd_lock.unlock();
                                }
                            }
                            else
                            {
                                fseek(txq[node_id].incoming.progress[tx_id].fp, tp.chunk_start, SEEK_SET);
                                fwrite(data.chunk, data.byte_count, 1, txq[node_id].incoming.progress[tx_id].fp);
                                fflush(txq[node_id].incoming.progress[tx_id].fp);
                                // Write latest meta data to disk
                                write_meta(txq[node_id].incoming.progress[tx_id]);
                                if (agent->debug_level)
                                {
                                    uint32_t total = 0;
                                    for (uint16_t i=0; i<data.byte_count; ++i)
                                    {
                                        total += data.chunk[i];
                                    }
                                }
                            }

                        }

                        // Check if file has been completely received
//                        if(txq[node_id].incoming.progress[tx_id].file_size == txq[node_id].incoming.progress[tx_id].total_bytes && txq[node_id].incoming.progress[tx_id].havemeta)
//                        {
//                            // See if we know what the remote node_id is for this
//                            if (txq[node_id].node_id > 0)
//                            {
//                                tx_progress tx_in = txq[node_id].incoming.progress[tx_id];
//                                if (agent->debug_level)
//                                {
//                                    debug_fd_lock.lock();
//                                    fprintf(agent->get_debug_fd(), "%.4f %.4f Incoming: Complete: %u %s %u %u\n", tet.split(), dt.lap(), tx_in.tx_id, tx_in.node_name.c_str(), tx_in.file_size, tx_in.total_bytes);
//                                    fflush(agent->get_debug_fd());
//                                    debug_fd_lock.unlock();
//                                }

//                                // Move file to its final location
//                                if (!txq[node_id].incoming.progress[tx_id].complete)
//                                {
//                                    if (txq[node_id].incoming.progress[tx_id].fp != nullptr)
//                                    {
//                                        fclose(txq[node_id].incoming.progress[tx_id].fp);
//                                        txq[node_id].incoming.progress[tx_id].fp = nullptr;
//                                    }
//                                    string final_filepath = tx_in.temppath + ".file";
//                                    int iret = rename(final_filepath.c_str(), tx_in.filepath.c_str());
//                                    // Make sure metadata is recorded
//                                    write_meta(txq[node_id].incoming.progress[tx_id], 0.);
//                                    if (agent->debug_level)
//                                    {
//                                        debug_fd_lock.lock();
//                                        fprintf(agent->get_debug_fd(), "%.4f %.4f Incoming: Renamed/Data: %d %s\n", tet.split(), dt.lap(), iret, tx_in.filepath.c_str());
//                                        fflush(agent->get_debug_fd());
//                                        debug_fd_lock.unlock();
//                                    }

//                                    // Mark complete
//                                    txq[node_id].incoming.progress[tx_id].complete = true;
//                                    txq[node_id].incoming.progress[tx_id].senddata = false;
//                                    txq[node_id].incoming.progress[tx_id].sentdata = true;
//                                }
//                            }
//                        }
                    }
                    else
                    {
                        txq[node_id].incoming.progress[data.tx_id].sendmeta = true;
                    }

                    incoming_tx_lock.unlock();

                    break;
                }
            case PACKET_COMPLETE:
                {
                    packet_struct_complete complete;

                    extract_complete(recvbuf, complete);

                    outgoing_tx_lock.lock();

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].outgoing, complete.tx_id);

                    if (tx_id > 0)
                    {
                        txq[node_id].outgoing.progress[tx_id].complete = true;
                        txq[node_id].outgoing.progress[tx_id].senddata = false;
                        txq[node_id].outgoing.progress[tx_id].sendmeta = false;
                        txq[node_id].outgoing.sendqueue = true;
                        txq[node_id].outgoing.sentqueue = false;
                    }

                    outgoing_tx_lock.unlock();
                    break;
                }
            case PACKET_CANCEL:
                {
                    profile_check(__LINE__, 1);

                    packet_struct_complete cancel;

                    extract_complete(recvbuf, cancel);

                    incoming_tx_lock.lock();

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].incoming, cancel.tx_id);

                    if (tx_id > 0)
                    {
                        // Remove the transaction
                        incoming_tx_del(node_id, tx_id);
                    }

                    incoming_tx_lock.unlock();
                    break;
                }
            default:
                ++type_error_count;
                break;
            }
            profile_check(__LINE__, 1);

        }
    }

    // Flush any active metadata
    for (uint16_t node=0; node<txq.size(); ++node)
    {
        for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
        {
            if (txq[(node)].incoming.progress[tx_id].tx_id && txq[(node)].incoming.progress[tx_id].havemeta)
            {
                write_meta(txq[(node)].incoming.progress[tx_id], 0.);
            }
        }
    }

}

void send_loop() noexcept
{
    vector<PACKET_BYTE> packet;
    int32_t use_channel=-1;

    while (agent->running())
    {

        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            COSMOS_SLEEP(1);
            continue;
        }

        for (uint8_t node_id=1; node_id<txq.size(); ++node_id)
        {
            // See if we have an active channel serving this Node
            if ((use_channel=check_channel(node_id)) <= 0)
            {
                continue;
            }

//            printf("use_channel: %d/%d\n", use_channel, out_comm_channel.size());

            // Sleep just a bit to let other threads act
//            COSMOS_SLEEP(out_comm_channel[use_channel].packet_size / out_comm_channel[use_channel].throughput);
            COSMOS_SLEEP(.1);

            // Set up to do the most important things first
            txq[(node_id)].outgoing.activity = false;
            txq[(node_id)].incoming.activity = false;

            // Let queue drain if necessary
            if (queuecheck(static_cast <PACKET_NODE_ID_TYPE>(node_id)) >= 2.)
            {
//                COSMOS_SLEEP(queuecheck(static_cast <PACKET_NODE_ID_TYPE>(node_id)) - 5.);
                continue;
            }

            // Send Heartbeat every 10 seconds, regardless
            if (txq[(node_id)].outgoing.heartbeatclock < currentmjd())
            {
                uint32_t funixtime = utc2unixseconds(out_comm_channel[use_channel].fmjd);
                make_heartbeat_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].node_name, 4, out_comm_channel[use_channel].throughput, funixtime);
                queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Outgoing", packet);
                txq[(node_id)].outgoing.heartbeatclock = currentmjd() + 10. / 86400.;
            }

            // Send Queue packet, if anything needs to be queued
            outgoing_tx_lock.lock();
            if (txq[(node_id)].outgoing.sendqueue && !txq[(node_id)].outgoing.sentqueue)
            {
                vector<PACKET_TX_ID_TYPE> tqueue (TRANSFER_QUEUE_LIMIT, 0);
                PACKET_TX_ID_TYPE iq = 0;
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].outgoing.progress[tx_id].tx_id == tx_id)
                    {
                        tqueue[iq++] = txq[(node_id)].outgoing.progress[tx_id].tx_id;
//                        txq[(node_id)].outgoing.progress[tx_id].sendmeta = true;
                    }
                    if (iq == TRANSFER_QUEUE_LIMIT)
                    {
                        break;
                    }
                }
                if (iq)
                {
                    make_queue_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].node_name, tqueue);
                    use_channel = queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Outgoing", packet);
                    if (use_channel >= 0)
                    {
                        txq[(node_id)].outgoing.sendqueue = false;
                        txq[(node_id)].outgoing.sentqueue = true;
                        txq[(node_id)].outgoing.activity = true;
                    }
                }
            }
            outgoing_tx_lock.unlock();

            profile_check(__LINE__, 2);

            // Send any  pending Metadata packets
            outgoing_tx_lock.lock();
            if (txq[(node_id)].outgoing.sentqueue)
            {
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].outgoing.progress[tx_id].tx_id == tx_id && txq[(node_id)].outgoing.progress[tx_id].sendmeta && !txq[(node_id)].outgoing.progress[tx_id].sentmeta)
                    {
                        tx_progress tx = txq[(node_id)].outgoing.progress[tx_id];
                        vector<PACKET_BYTE> packet;
                        make_metadata_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), tx.tx_id, (char *)tx.file_name.c_str(), tx.file_size, (char *)tx.agent_name.c_str());
                        use_channel = queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Outgoing", packet);
                        if (use_channel >= 0)
                        {
                            txq[(node_id)].outgoing.activity = true;
                            txq[(node_id)].outgoing.progress[tx_id].sendmeta = false;
                            txq[(node_id)].outgoing.progress[tx_id].havemeta = true;
                            txq[(node_id)].outgoing.progress[tx_id].sentmeta = true;
                            break;
                        }
                    }
                }
            }
            outgoing_tx_lock.unlock();

            profile_check(__LINE__, 2);

            // Send Data packets if we have data to send and we didn't do anything above
            outgoing_tx_lock.lock();
            if (txq[(node_id)].outgoing.sentqueue)
            {
                uint16_t tx_id = choose_outgoing_tx_id((node_id));
                if (txq[(node_id)].outgoing.progress[tx_id].tx_id == tx_id && txq[(node_id)].outgoing.progress[tx_id].sentmeta && txq[(node_id)].outgoing.progress[tx_id].senddata)
                {
                    if (txq[(node_id)].outgoing.progress[tx_id].file_size)
                    {
                        // Check if there is any more data to send
                        if (txq[(node_id)].outgoing.progress[tx_id].total_bytes == 0)
                        {
                            txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                            txq[(node_id)].outgoing.progress[tx_id].senddata = false;
                        }
                        else
                        {
                            // Attempt to open the outgoing progress file
                            if (txq[(node_id)].outgoing.progress[tx_id].fp == nullptr)
                            {
                                txq[(node_id)].outgoing.progress[tx_id].fp = fopen(txq[(node_id)].outgoing.progress[tx_id].filepath.c_str(), "r");
                            }

                            // If we're good, continue with the process
                            if(txq[(node_id)].outgoing.progress[tx_id].fp != nullptr)
                            {
                                file_progress tp;
                                tp = txq[(node_id)].outgoing.progress[tx_id].file_info[0];

                                PACKET_FILE_SIZE_TYPE byte_count = (tp.chunk_end - tp.chunk_start) + 1;
                                if (byte_count > out_comm_channel[use_channel].packet_size)
                                {
                                    byte_count = out_comm_channel[use_channel].packet_size;
                                }

                                tp.chunk_end = tp.chunk_start + byte_count - 1;

                                // Read the packet and send it
                                int32_t nbytes;
                                PACKET_BYTE* chunk = new PACKET_BYTE[byte_count]();
                                if (!(nbytes = fseek(txq[(node_id)].outgoing.progress[tx_id].fp, tp.chunk_start, SEEK_SET)))
                                {
                                    nbytes = fread(chunk, 1, byte_count, txq[(node_id)].outgoing.progress[tx_id].fp);
                                }
                                if (nbytes == byte_count)
                                {
                                    make_data_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].outgoing.progress[tx_id].tx_id, byte_count, tp.chunk_start, chunk);
                                    use_channel = queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Outgoing", packet);
                                    if (use_channel >= 0)
                                    {
                                        txq[(node_id)].outgoing.progress[tx_id].file_info[0].chunk_start = tp.chunk_end + 1;
                                        txq[(node_id)].outgoing.activity = true;
                                    }
                                }
                                else
                                {
                                    // Some problem with this transmission, ask other end to dequeue it
                                    // Remove transaction
                                    txq[(node_id)].outgoing.progress[tx_id].senddata = false;
                                    txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                                    txq[(node_id)].outgoing.progress[tx_id].complete = true;
                                }
                                delete[] chunk;

                                if (txq[(node_id)].outgoing.progress[tx_id].file_info[0].chunk_start > txq[(node_id)].outgoing.progress[tx_id].file_info[0].chunk_end)
                                {
                                    // All done with this file_info entry. Close file and remove entry.
                                    fclose(txq[(node_id)].outgoing.progress[tx_id].fp);
                                    txq[(node_id)].outgoing.progress[tx_id].fp = nullptr;
                                    txq[(node_id)].outgoing.progress[tx_id].file_info.pop_front();
                                }

                                write_meta(txq[(node_id)].outgoing.progress[tx_id]);
                            }
                            else
                            {
                                // Some problem with this transmission, ask other end to dequeue it

                                txq[(node_id)].outgoing.progress[tx_id].senddata = false;
                                txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                                txq[(node_id)].outgoing.progress[tx_id].complete = true;
                            }
                        }
                    }
                    // Zero length file, ask other end to dequeue it
                    else
                    {
                        txq[(node_id)].outgoing.progress[tx_id].senddata = false;
                        txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                        txq[(node_id)].outgoing.progress[tx_id].complete = true;
                    }
                }
            }
            outgoing_tx_lock.unlock();

            // Send Cancel packets if required
            outgoing_tx_lock.lock();
            if (txq[(node_id)].outgoing.sentqueue)
            {
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].outgoing.progress[tx_id].tx_id == tx_id && txq[(node_id)].outgoing.progress[tx_id].complete)
                    {
                        // Remove from queue
                        outgoing_tx_del(node_id, tx_id);

                        // Send a CANCEL packet
                        vector<PACKET_BYTE> packet;
                        make_cancel_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), tx_id);
                        use_channel = queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Outgoing", packet);
                        if (use_channel >= 0)
                        {
                            txq[(node_id)].outgoing.activity = true;
                        }
                    }
                }
            }
            outgoing_tx_lock.unlock();

            // Send Reqmeta packet if needed and it has been otherwise quiet
            incoming_tx_lock.lock();
            if (!txq[(node_id)].incoming.rcvdqueue && !txq[(node_id)].incoming.rcvdmeta && txq[(node_id)].incoming.reqmetaclock < currentmjd())
            {
                vector<PACKET_TX_ID_TYPE> tqueue (TRANSFER_QUEUE_LIMIT, 0);
                PACKET_TX_ID_TYPE iq = 0;
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].incoming.progress[tx_id].tx_id && !txq[(node_id)].incoming.progress[tx_id].sentmeta)
                    {
                        tqueue[iq++] = tx_id;
                        txq[(node_id)].incoming.progress[tx_id].sendmeta = true;
                    }
                    if (iq == TRANSFER_QUEUE_LIMIT)
                    {
                        break;
                    }
                }
                if (iq)
                {
                    vector<PACKET_BYTE> packet;
                    make_reqmeta_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].node_name, tqueue);
                    use_channel = queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Incoming", packet);
                }
                txq[(node_id)].incoming.reqmetaclock = currentmjd() + 5. / 86400.;
            }
            incoming_tx_lock.unlock();

            // Send Reqdata packet if there is still data to be gotten and it has been otherwise quiet
            incoming_tx_lock.lock();
            if (!txq[(node_id)].incoming.rcvdqueue && !txq[(node_id)].incoming.rcvdmeta && !txq[(node_id)].incoming.rcvddata && txq[(node_id)].incoming.reqdataclock < currentmjd())
            {
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].incoming.progress[tx_id].tx_id)
                    {
                        // Ask for missing data
                        vector<file_progress> missing;
                        missing = find_chunks_missing(txq[(node_id)].incoming.progress[tx_id]);
                        for (uint32_t j=0; j<missing.size(); ++j)
                        {
                            vector<PACKET_BYTE> packet;
                            make_reqdata_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].incoming.progress[tx_id].tx_id, missing[j].chunk_start, missing[j].chunk_end);
                            use_channel = queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Incoming", packet);
                        }
                    }
                }
                txq[(node_id)].incoming.reqdataclock = currentmjd() + 5. / 86400.;
            }
            incoming_tx_lock.unlock();

            // Send Complete packets if required
            incoming_tx_lock.lock();
            if (!txq[(node_id)].incoming.rcvdqueue && !txq[(node_id)].incoming.rcvdmeta && !txq[(node_id)].incoming.rcvddata)
            {
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].incoming.progress[tx_id].tx_id == tx_id && txq[node_id].incoming.progress[tx_id].file_size == txq[node_id].incoming.progress[tx_id].total_bytes)
                    {
                        // Remove from queue
                        txq[(node_id)].incoming.progress[tx_id].complete = true;
                        incoming_tx_del(node_id, tx_id);

                        // Send a COMPLETE packet
                        vector<PACKET_BYTE> packet;
                        make_complete_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), tx_id);
                        use_channel = queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Incoming", packet);
                        if (use_channel >= 0)
                        {
                            txq[(node_id)].incoming.activity = true;
                        }
                    }
                }
            }
            incoming_tx_lock.unlock();

        }
    }
}

void transmit_loop() noexcept
{
    int32_t iretn;
    //    std::mutex transmit_queue_lock;
    //    std::unique_lock<std::mutex> locker(transmit_queue_lock);

    while (agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            COSMOS_SLEEP(1);
            continue;
        }


        //        transmit_queue_check.wait(locker);

        while (!transmit_queue.empty())
        {
            // Get next packet from transceiver FIFO
            txqueue_lock.lock();
            transmit_queue_entry entry = transmit_queue.front();
            iretn = mysendto(entry.type, entry.channel, entry.packet);
            if (iretn >= 0)
            {
                out_comm_channel[entry.channel].fmjd = transmit_queue.back().nmjd;
                transmit_queue.pop();
            }
            txqueue_lock.unlock();
        }
        COSMOS_SLEEP(.001);
    }
}

double queuecheck(PACKET_NODE_ID_TYPE node_id)
{
    int32_t use_channel;

    use_channel = -1;
    profile_check(__LINE__, 2);

    for (uint16_t i=0; i<out_comm_channel.size(); ++i)
    {
        if (txq[node_id].node_name == out_comm_channel[i].node)
        {
            use_channel = i;
            break;
        }
    }

    if (use_channel == -1)
    {
        return -1.;
    }

    return (1. * transmit_queue.size() * out_comm_channel[use_channel].packet_size) / out_comm_channel[use_channel].throughput;
}

int32_t queuesendto(PACKET_NODE_ID_TYPE node_id, string type, vector<PACKET_BYTE> packet)
{
    transmit_queue_entry tentry;
    int32_t use_channel;

//    use_channel = -1;
//    for (uint16_t i=0; i<out_comm_channel.size(); ++i)
//    {
//        if (txq[node_id].node_name == out_comm_channel[i].node)
//        {
//            use_channel = i;
//            break;
//        }
//    }

//    if (use_channel > out_comm_channel.size())
    use_channel = check_channel(node_id);
    if (use_channel <= 0)
    {
        return -1.;
    }

    tentry.type = type;
    tentry.channel = use_channel;
    tentry.packet = packet;
    tentry.time_step = (28 + packet.size()) / (86400. * out_comm_channel[use_channel].throughput);
    txqueue_lock.lock();
    if (transmit_queue.empty())
    {
        tentry.nmjd = out_comm_channel[use_channel].nmjd + tentry.time_step;
    }
    else
    {
        tentry.nmjd = transmit_queue.back().nmjd + tentry.time_step;
    }
    transmit_queue.push(tentry);
    txqueue_lock.unlock();
    //    transmit_queue_check.notify_one();
    // Sleep just a bit to give other threads a chance
    COSMOS_SLEEP(.001);
    return use_channel;
}

int32_t mysendto(string type, int32_t use_channel, vector<PACKET_BYTE>& buf)
{
    int32_t iretn;
    double cmjd;

    profile_check(__LINE__, 3);

    if ((cmjd = currentmjd(0.)) < out_comm_channel[use_channel].nmjd)
    {
        COSMOS_SLEEP((86400. * (out_comm_channel[use_channel].nmjd - cmjd)));
    }

    profile_check(__LINE__, 3);

    iretn = sendto(out_comm_channel[use_channel].chansock.cudp, reinterpret_cast<const char*>(&buf[0]), buf.size(), 0, reinterpret_cast<sockaddr*>(&out_comm_channel[use_channel].chansock.caddr), sizeof(struct sockaddr_in));

    if (iretn >= 0)
    {
        profile_check(__LINE__, 3);

        ++packet_out_count;
        out_comm_channel[use_channel].lomjd = currentmjd();
        out_comm_channel[use_channel].nmjd = out_comm_channel[use_channel].lomjd + ((28+iretn) / (float)out_comm_channel[use_channel].throughput)/86400.;
        if (agent->debug_level)
        {
            debug_packet(buf, PACKET_OUT, type, use_channel);
        }
    }
    else
    {
        iretn = -errno;
        ++send_error_count;
    }

    profile_check(__LINE__, 3);

    return iretn;
}

int32_t myrecvfrom(string type, socket_channel &channel, vector<PACKET_BYTE>& buf, uint32_t length, double dtimeout)
{
    int32_t nbytes = 0;

    buf.resize(length);
    ElapsedTime et;
    do
    {
        profile_check(__LINE__, 1);

        fd_set set;
        FD_ZERO(&set);
        int fdmax = -1;
        for (uint16_t i=0; i<out_comm_channel.size(); ++i)
        {
            profile_check(__LINE__, 1);

            FD_SET(out_comm_channel[i].chansock.cudp, &set);
            if (out_comm_channel[i].chansock.cudp > fdmax)
            {
                fdmax = out_comm_channel[i].chansock.cudp;
            }
        }
        double rtimeout = dtimeout - et.split();
        if (rtimeout >= 0.)
        {
#if !defined(COSMOS_WIN_OS)
            profile_check(__LINE__, 1);

            timeval timeout;
            timeout.tv_sec = static_cast<int32_t>(rtimeout);
            timeout.tv_usec = static_cast<int32_t>(1000000. * (rtimeout - timeout.tv_sec));
            profile_check(__LINE__, 1);

            int rv = select(fdmax+1, &set, nullptr, nullptr, &timeout);
            profile_check(__LINE__, 1);

            if (rv == -1)
            {
                nbytes = -errno;
            }
            else if (rv == 0)
            {
                nbytes = GENERAL_ERROR_TIMEOUT;
                ++timeout_error_count;
            }
            else
            {
                profile_check(__LINE__, 1);

                for (uint16_t i=0; i<out_comm_channel.size(); ++i)
                {
                    if (FD_ISSET(out_comm_channel[i].chansock.cudp, &set))
                    {
                        profile_check(__LINE__, 1);

                        channel = out_comm_channel[i].chansock;
                        nbytes = recvfrom(channel.cudp, reinterpret_cast<char *>(&buf[0]), length, 0, reinterpret_cast<sockaddr*>(&channel.caddr), reinterpret_cast<socklen_t *>(&channel.addrlen));
                        if (nbytes > 0)
                        {
                            profile_check(__LINE__, 1);

                            uint16_t crccalc = calc_crc16ccitt(&buf[3], nbytes-3);
                            uint16_t crc;
                            memmove(&crc, &buf[0]+PACKET_HEADER_OFFSET_CRC, sizeof(PACKET_CRC));
                            if (crc != crccalc)
                            {
                                nbytes = GENERAL_ERROR_CRC;
                                ++crc_error_count;
                            }
                            else
                            {
                                ++packet_in_count;
                                buf.resize(nbytes);
                                if (agent->debug_level)
                                {
                                    debug_packet(buf, PACKET_IN, type, i);
                                }
                            }
                            return nbytes;
                        }
                        else
                        {
                            profile_check(__LINE__, 1);

                            if (nbytes < 0)
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
                    }
                }
            }
#else
            for (uint16_t i=0; i<out_comm_channel.size(); ++i)
            {
                channel = out_comm_channel[i].chansock;
                nbytes = recvfrom(channel.cudp, reinterpret_cast<char *>(&buf[0]), length, 0, reinterpret_cast<sockaddr*>(&channel.caddr), reinterpret_cast<socklen_t *>(&channel.addrlen));
                if (nbytes > 0)
                {
                    uint16_t crccalc = calc_crc16ccitt(&buf[3], nbytes-3);
                    uint16_t crc;
                    memmove(&crc, &buf[0]+PACKET_HEADER_OFFSET_CRC, sizeof(PACKET_CRC));
                    if (crc != crccalc)
                    {
                        nbytes = GENERAL_ERROR_CRC;
                        ++crc_error_count;
                    }
                    else
                    {
                        ++packet_in_count;
                        buf.resize(nbytes);
                        debug_packet(buf, PACKET_IN, type, i);
                    }
                    return nbytes;
                }
                else
                {
                    if (nbytes < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            COSMOS_SLEEP(.1);
                            break;
                        }
                        nbytes = -errno;
                        ++recv_error_count;
                    }
                    else
                    {
                        nbytes = GENERAL_ERROR_INPUT;
                        ++recv_error_count;
                    }
                }
            }
#endif // Not windows
        }
    } while (et.split() < dtimeout);

    return nbytes;
}

void debug_packet(vector<PACKET_BYTE> buf, uint8_t direction, string type, int32_t use_channel)
{
    static ElapsedTime dt;

    if (agent->debug_level)
    {
        debug_fd_lock.lock();

        string packet_node_name;
        string node_name = lookup_node_id_name(buf[PACKET_HEADER_OFFSET_NODE_ID]);
        uint8_t node_id = check_node_id(buf[PACKET_HEADER_OFFSET_NODE_ID]);
        switch (buf[0] & 0x0f)
        {
        case PACKET_HEARTBEAT:
        case PACKET_REQQUEUE:
        case PACKET_QUEUE:
        case PACKET_REQMETA:
            packet_node_name = reinterpret_cast<char *>(&buf[PACKET_HEADER_OFFSET_NODE_NAME]);
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
                    fprintf(agent->get_debug_fd(), "%.4f %.4f RECV L %u R %u %s %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, buf.size());
                }
                else
                {
                    fprintf(agent->get_debug_fd(), "%.4f %.4f RECV L %u R %u Unknown %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, buf.size());
                }
            }
            else
            {
                fprintf(agent->get_debug_fd(), "%.4f %.4f RECV L %u R %u %s %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, out_comm_channel[use_channel].node.c_str(), out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, buf.size());
            }
        }
        else if (direction == PACKET_OUT)
        {
            if (out_comm_channel[use_channel].node.empty())
            {
                if (!node_name.empty())
                {
                    fprintf(agent->get_debug_fd(), "%.4f %.4f SEND L %u R %u %s %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, buf.size());
                }
                else
                {
                    fprintf(agent->get_debug_fd(), "%.4f %.4f SEND L %u R %u Unknown %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, buf.size());
                }
            }
            else
            {
                fprintf(agent->get_debug_fd(), "%.4f %.4f SEND L %u R %u %s %s [%s] In: %u Out: %u Size: %lu ", tet.split(), dt.lap(), node_id, node_id, out_comm_channel[use_channel].node.c_str(), out_comm_channel[use_channel].chanip.c_str(), type.c_str(), packet_in_count, packet_out_count, buf.size());
            }
        }

        switch (buf[0] & 0x0f)
        {
        case PACKET_METADATA:
            {
                string file_name(&buf[PACKET_METASHORT_OFFSET_FILE_NAME], &buf[PACKET_METASHORT_OFFSET_FILE_NAME+TRANSFER_MAX_FILENAME]);
                fprintf(agent->get_debug_fd(), "[METADATA] %u %u %s ", node_id, buf[PACKET_METASHORT_OFFSET_TX_ID], file_name.c_str());
                break;
            }
        case PACKET_DATA:
            {
                fprintf(agent->get_debug_fd(), "[DATA] %u %u %u %u ", node_id, buf[PACKET_DATA_OFFSET_TX_ID], buf[PACKET_DATA_OFFSET_CHUNK_START]+256U*(buf[PACKET_DATA_OFFSET_CHUNK_START+1]+256U*(buf[PACKET_DATA_OFFSET_CHUNK_START+2]+256U*buf[PACKET_DATA_OFFSET_CHUNK_START+3])), buf[PACKET_DATA_OFFSET_BYTE_COUNT]+256U*buf[PACKET_DATA_OFFSET_BYTE_COUNT+1]);
                break;
            }
        case PACKET_REQDATA:
            {
                fprintf(agent->get_debug_fd(), "[REQDATA] %u %u %u %u ", node_id, buf[PACKET_REQDATA_OFFSET_TX_ID], buf[PACKET_REQDATA_OFFSET_HOLE_START]+256U*(buf[PACKET_REQDATA_OFFSET_HOLE_START+1]+256U*(buf[PACKET_REQDATA_OFFSET_HOLE_START+2]+256U*buf[PACKET_REQDATA_OFFSET_HOLE_START+3])), buf[PACKET_REQDATA_OFFSET_HOLE_END]+256U*(buf[PACKET_REQDATA_OFFSET_HOLE_END+1]+256U*(buf[PACKET_REQDATA_OFFSET_HOLE_END+2]+256U*buf[PACKET_REQDATA_OFFSET_HOLE_END+3])));
                break;
            }
        case PACKET_COMPLETE:
            {
                fprintf(agent->get_debug_fd(), "[COMPLETE] %u %u ", node_id, buf[PACKET_COMPLETE_OFFSET_TX_ID]);
                break;
            }
        case PACKET_CANCEL:
            {
                fprintf(agent->get_debug_fd(), "[CANCEL] %u %u ", node_id, buf[PACKET_CANCEL_OFFSET_TX_ID]);
                break;
            }
        case PACKET_REQMETA:
            {
                fprintf(agent->get_debug_fd(), "[REQMETA] %u %s ", node_id, &buf[PACKET_HEADER_OFFSET_NODE_NAME]);
                for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                    if (buf[PACKET_REQMETA_OFFSET_TX_ID+i])
                    {
                        fprintf(agent->get_debug_fd(), "%u ", buf[PACKET_REQMETA_OFFSET_TX_ID+i]);
                    }
                break;
            }
        case PACKET_REQQUEUE:
            {
                fprintf(agent->get_debug_fd(), "[REQQUEUE] %u %s ", node_id, &buf[PACKET_HEADER_OFFSET_NODE_NAME]);
            }
            break;
        case PACKET_QUEUE:
            {
                fprintf(agent->get_debug_fd(), "[QUEUE] %u %s ", node_id, &buf[PACKET_HEADER_OFFSET_NODE_NAME]);
                for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                    if (buf[PACKET_QUEUE_OFFSET_TX_ID+i])
                    {
                        fprintf(agent->get_debug_fd(), "%u ", buf[PACKET_QUEUE_OFFSET_TX_ID+i]);
                    }
            }
            break;
        case PACKET_HEARTBEAT:
            {
                fprintf(agent->get_debug_fd(), "[HEARTBEAT] %u %s %hu %u %u", node_id, &buf[PACKET_HEADER_OFFSET_NODE_NAME], buf[PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD]
                        , buf[PACKET_HEARTBEAT_OFFSET_THROUGHPUT]+256U*(buf[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+1]+256U*(buf[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+2]+256U*buf[PACKET_HEARTBEAT_OFFSET_THROUGHPUT+3]))
                        , buf[PACKET_HEARTBEAT_OFFSET_FUNIXTIME]+256U*(buf[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+1]+256U*(buf[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+2]+256U*buf[PACKET_HEARTBEAT_OFFSET_FUNIXTIME+3])));
                break;
            }
        case PACKET_MESSAGE:
            {
                fprintf(agent->get_debug_fd(), "[MESSAGE] %u %hu %s", node_id, buf[PACKET_MESSAGE_OFFSET_LENGTH], &buf[PACKET_MESSAGE_OFFSET_BYTES]);
                break;
            }
        case PACKET_COMMAND:
            {
                fprintf(agent->get_debug_fd(), "[COMMAND] %u %hu %s", node_id, buf[PACKET_COMMAND_OFFSET_LENGTH], &buf[PACKET_COMMAND_OFFSET_BYTES]);
                break;
            }
        }
        fprintf(agent->get_debug_fd(), "\n");
        fflush(agent->get_debug_fd());
        debug_fd_lock.unlock();
    }
}

int32_t write_meta(tx_progress& tx, double interval)
{
    vector<PACKET_BYTE> packet;
    std::ofstream file_name;

    if (currentmjd(0.) - tx.savetime > interval/86400.)
    {
        tx.savetime = currentmjd(0.);
        make_metadata_packet(packet, tx.tx_id, (char *)tx.file_name.c_str(), tx.file_size, (char *)tx.node_name.c_str(), (char *)tx.agent_name.c_str());
        file_name.open(tx.temppath + ".meta", std::ios::out|std::ios::binary);
        if(!file_name.is_open())
        {
            return (-errno);
        }

        uint16_t crc;
        file_name.write((char *)&packet[0], PACKET_METALONG_OFFSET_TOTAL);
        crc = slip_calc_crc((uint8_t *)&packet[0], PACKET_METALONG_OFFSET_TOTAL);
        file_name.write((char *)&crc, 2);
        for (file_progress progress_info : tx.file_info)
        {
            file_name.write((const char *)&progress_info, sizeof(progress_info));
            crc = slip_calc_crc((uint8_t *)&progress_info, sizeof(progress_info));
            file_name.write((char *)&crc, 2);
        }
        file_name.close();
    }

    return 0;
}

int32_t read_meta(tx_progress& tx)
{
    vector<PACKET_BYTE> packet(PACKET_METALONG_OFFSET_TOTAL,0);
    std::ifstream file_name;
    packet_struct_metalong meta;

    struct stat statbuf;
    if (!stat((tx.temppath + ".meta").c_str(), &statbuf) && statbuf.st_size >= COSMOS_SIZEOF(file_progress))
    {
        file_name.open(tx.temppath + ".meta", std::ios::out|std::ios::binary);
        if(!file_name.is_open())
        {
            return (-errno);
        }
    }
    else
    {
        //		remove((tx.temppath + ".meta").c_str());
        return DATA_ERROR_SIZE_MISMATCH;
    }

    tx.fp = nullptr;
    tx.savetime = 0.;
    tx.complete = false;


    // load metadata

    file_name.read((char *)&packet[0], PACKET_METALONG_OFFSET_TOTAL);
    if (file_name.eof())
    {
        return DATA_ERROR_SIZE_MISMATCH;
    }
    uint16_t crc;
    file_name.read((char *)&crc, 2);
    if (file_name.eof())
    {
        return DATA_ERROR_SIZE_MISMATCH;
    }
    if (crc != slip_calc_crc((uint8_t *)&packet[0], PACKET_METALONG_OFFSET_TOTAL))
    {
        file_name.close();
        return DATA_ERROR_CRC;
    }
    extract_metadata(packet, meta);
    tx.havemeta = true;
    tx.tx_id = meta.tx_id;
    tx.node_name = meta.node_name;
    tx.agent_name = meta.agent_name;
    tx.file_name = meta.file_name;
    tx.filepath = data_base_path(tx.node_name, "outgoing", tx.agent_name, tx.file_name);
    tx.file_size = meta.file_size;

    // load file progress
    file_progress progress_info;
    do
    {
        file_name.read((char *)&progress_info, sizeof(progress_info));
        if (file_name.eof())
        {
            break;
        }
        uint16_t crc;
        file_name.read((char *)&crc, 2);
        if (file_name.eof())
        {
            return DATA_ERROR_SIZE_MISMATCH;
        }
        if (crc != slip_calc_crc((uint8_t *)&progress_info, sizeof(progress_info)))
        {
            file_name.close();
            return DATA_ERROR_CRC;
        }

        tx.file_info.push_back(progress_info);
    } while(!file_name.eof());
    file_name.close();
    if (agent->debug_level)
    {
        debug_fd_lock.lock();
        fprintf(agent->get_debug_fd(), "%.4f %.4f Main: read_meta: %s tx_id: %d chunks: %lu\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str(), tx.tx_id, tx.file_info.size());
        fflush(agent->get_debug_fd());
        debug_fd_lock.unlock();
    }

    // fix any overlaps and count total bytes
    //    merge_chunks_overlap(tx);
    //    tx.state = STATE_REQDATA;

    return 0;
}

bool lower_chunk(file_progress i,file_progress j)
{
    return (i.chunk_start<j.chunk_start);
}

PACKET_FILE_SIZE_TYPE merge_chunks_overlap(tx_progress& tx)
{
    for (uint16_t i=tx.file_info.size()-1; i<tx.file_info.size(); --i)
    {
        if (tx.file_info[i].chunk_end >= tx.file_size)
        {
            tx.file_info.pop_back();
        }
    }
    switch (tx.file_info.size())
    {
    case 0:
        {
            tx.total_bytes = 0;
            break;
        }
    case 1:
        {
            tx.total_bytes = (tx.file_info[0].chunk_end - tx.file_info[0].chunk_start) + 1;
            break;
        }
    default:
        {
            tx.total_bytes = 0;
            sort(tx.file_info.begin(), tx.file_info.end(), lower_chunk);
            for (uint32_t i=0; i<tx.file_info.size(); ++i)
            {
                for (uint32_t j=i+1; j<tx.file_info.size(); ++j)
                {
                    while (j < tx.file_info.size() && tx.file_info[j].chunk_start <= tx.file_info[i].chunk_end+1)
                    {
                        if (tx.file_info[j].chunk_end > tx.file_info[i].chunk_end)
                        {
                            tx.file_info[i].chunk_end = tx.file_info[j].chunk_end;
                        }
                        tx.file_info.erase(tx.file_info.begin()+j);
                    }
                }
                tx.total_bytes += (tx.file_info[i].chunk_end - tx.file_info[i].chunk_start) + 1;
            }
            break;
        }
    }
    return tx.total_bytes;
}

vector<file_progress> find_chunks_missing(tx_progress& tx)
{
    vector<file_progress> missing;
    file_progress tp;

    if (!tx.havemeta)
    {
        return missing;
    }

    if (tx.file_info.size() == 0)
    {
        tp.chunk_start = 0;
        tp.chunk_end = tx.file_size - 1;
        missing.push_back(tp);
    }
    else
    {
        merge_chunks_overlap(tx);
        sort(tx.file_info.begin(), tx.file_info.end(), lower_chunk);

        // Check missing before first chunk
        if (tx.file_info[0].chunk_start)
        {
            tp.chunk_start = 0;
            tp.chunk_end = tx.file_info[0].chunk_start - 1;
            missing.push_back(tp);
        }

        // Check missing between chunks
        for (uint32_t i=1; i<tx.file_info.size(); ++i)
        {
            if (tx.file_info[i-1].chunk_end+1 != tx.file_info[i].chunk_start)
            {
                tp.chunk_start = tx.file_info[i-1].chunk_end + 1;
                tp.chunk_end = tx.file_info[i].chunk_start - 1;
                missing.push_back(tp);
            }
        }

        // Check missing after last chunk
        if (tx.file_info[tx.file_info.size()-1].chunk_end + 1 != tx.file_size)
        {
            tp.chunk_start = tx.file_info[tx.file_info.size()-1].chunk_end + 1;
            tp.chunk_end = tx.file_size - 1;
            missing.push_back(tp);
        }
    }

    // calculate bytes so far
    tx.total_bytes = 0;
    for (file_progress prog : tx.file_info)
    {
        tx.total_bytes += (prog.chunk_end - prog.chunk_start) + 1;
    }
    if (tx.total_bytes == tx.file_size)
    {
        tx.complete = true;
    }

    return (missing);
}

vector<file_progress> find_chunks_togo(tx_progress& tx)
{
    vector<file_progress> togo;
    file_progress tp;

    if (tx.file_info.size())
    {
        merge_chunks_overlap(tx);
        sort(tx.file_info.begin(), tx.file_info.end(), lower_chunk);

        // Set first chunk
        tp.chunk_start = tx.file_info[0].chunk_start;
        tp.chunk_end = tx.file_info[0].chunk_end;

        // Add middle chunks to go
        for (uint32_t i=1; i<tx.file_info.size(); ++i)
        {
            if (tx.file_info[i-1].chunk_end+1 == tx.file_info[i].chunk_start)
            {
                tp.chunk_end = tx.file_info[i].chunk_end;
            }
            else
            {
                togo.push_back(tp);
                tp.chunk_start = tx.file_info[i].chunk_start;
                tp.chunk_end = tx.file_info[i].chunk_end;
            }
        }

        // Add last chunk
        togo.push_back(tp);
    }

    // calculate bytes left
    tx.total_bytes = 0;
    for (file_progress prog : togo)
    {
        tx.total_bytes += (prog.chunk_end - prog.chunk_start) + 1;
    }

    return (togo);
}

int32_t request_ls(string &request, string &response, Agent *agent)
{

    //the request string == "ls directoryname"
    //get the directory name
    //    char directoryname[COSMOS_MAX_NAME+1];
    //    memmove(directoryname, request.substr(3), COSMOS_MAX_NAME);
    string directoryname = request.substr(3);

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
        response =  "unable to open directory " + directoryname;
    return 0;
}

int32_t request_list_incoming(string &request, string &response, Agent *agent)
{
    int32_t iretn;
    response.clear();
    for (uint16_t node = 0; node<txq.size(); ++node)
    {
        if ((iretn=check_node_id(node)) > 0)
        {
            response +=  std::to_string(node) + ' ' + txq[(node)].node_name + ' ' +  std::to_string(txq[(node)].incoming.size) +  std::to_string(iretn)+ " rcvdqueue: " +  std::to_string(txq[(node)].incoming.rcvdmeta) + " rcvdmeta: " +  std::to_string(txq[(node)].incoming.rcvdmeta) + " rcvddata: " +  std::to_string(txq[(node)].incoming.rcvddata) + "\n";
            for(tx_progress tx : txq[(node)].incoming.progress)
            {
                if (tx.tx_id)
                {
                    response += to_label("tx_id", tx.tx_id) + ' ';
                    response += to_label("node", tx.node_name) + ' ';
                    response += to_label("agent", tx.agent_name) + ' ';
                    response += to_label("name", tx.file_name) + ' ';
                    response += to_label("bytes", tx.total_bytes) + ' ';
                    response += "/" + to_unsigned(tx.file_size) + ' ';
                    response += to_label("havemeta", tx.havemeta) + ' ';
                    response += to_label("sendmeta", tx.sendmeta) + ' ';
                    response += to_label("sentmeta", tx.sentmeta) + ' ';
                    response += to_label("senddata", tx.senddata) + ' ';
                    response += to_label("sentdata", tx.sentdata) + ' ';
                    response += to_label("complete", tx.complete);
                    response += "\n";
                }
            }
        }
    }

    return 0;
}

int32_t request_list_outgoing(string &request, string &response, Agent *agent)
{
    int32_t iretn;
    response.clear();
    for (uint16_t node=0; node<txq.size(); ++node)
    {
        if ((iretn=check_node_id(node)) > 0)
        {
            response +=  std::to_string(node)+ ' ' + txq[(node)].node_name + ' ' + std::to_string(txq[(node)].outgoing.size) + "\n";
            for(tx_progress tx : txq[(node)].outgoing.progress)
            {
                if (tx.tx_id)
                {
                    response +=  "tx_id: " + std::to_string(tx.tx_id) + " node: " + tx.node_name + " agent: " + tx.agent_name + " name: " + tx.file_name + " bytes: " + std::to_string(tx.total_bytes) + "/" + std::to_string(tx.file_size);
                    response += " enabled: " + to_bool(tx.enabled);
                    response += " havemeta: " + to_bool(tx.havemeta) + " sendmeta: " + to_bool(tx.sendmeta) + " sentmeta: " + to_bool(tx.sentmeta);
                    response += " senddata: " + to_bool(tx.senddata) + " sentdata: " + to_bool(tx.sentdata) + " complete: " + to_bool(tx.complete) + "\n";
                }
            }
        }
    }

    return 0;
}

int32_t request_get_channels(string &request, string &response, Agent *agent)
{
    response.clear();
    for (uint16_t channel=0; channel<out_comm_channel.size(); ++channel)
    {
        response += "{";
        response += to_json("channel", channel);
        response += to_json("node", out_comm_channel[channel].node);
        response += to_json("ip", out_comm_channel[channel].chanip);
        response += to_json("size", out_comm_channel[channel].packet_size);
        response += to_json("throughput", out_comm_channel[channel].throughput);
        response += to_json("nmjd", out_comm_channel[channel].nmjd);
        response += to_json("limjd", out_comm_channel[channel].limjd);
        response += to_json("lomjd", out_comm_channel[channel].lomjd);
        response += to_json("fmjd", out_comm_channel[channel].fmjd);
        response += "}";
    }
    return 0;
}

int32_t request_set_throughput(string &request, string &response, Agent *agent)
{
    uint16_t channel=0;
    uint32_t throughput=0;

    sscanf(request.c_str(), "%*s %hu %u\n", &channel, &throughput);
    if (channel < out_comm_channel.size())
    {
        if (throughput)
        {
            out_comm_channel[channel].throughput = throughput;
        }
    }
    else
    {
        response =  "Channel " + to_unsigned(channel) + " too large";
    }
    return 0;

}

int32_t request_remove_file(string &request, string &response, Agent *agent)
{
    char type;
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
    }

    return 0;
}

int32_t outgoing_tx_add(tx_progress &tx_out)
{
    if (agent->debug_level)
    {
        debug_fd_lock.lock();
        fprintf(agent->get_debug_fd(), "%.4f %.4f Main: outgoing_tx_add: ", tet.split(), dt.lap());
        fflush(agent->get_debug_fd());
        debug_fd_lock.unlock();
    }

    int32_t node_id = lookup_node_id(tx_out.node_name);
    if (node_id <= 0)
    {
        if (agent->debug_level)
        {
            debug_fd_lock.lock();
            fprintf(agent->get_debug_fd(), "TRANSFER_ERROR_NODE\n");
            fflush(agent->get_debug_fd());
            debug_fd_lock.unlock();
        }
        if (node_id == 0)
        {
            return TRANSFER_ERROR_NODE;
        }
        else
        {
            return node_id;
        }
    }

    // Check for duplicate tx_id
    if (txq[(node_id)].outgoing.progress[tx_out.tx_id].tx_id)
    {
        return TRANSFER_ERROR_DUPLICATE;
    }
    // Only add if we have room
//    if (txq[(node_id)].outgoing.size == PROGRESS_QUEUE_SIZE)
//    {
//        if (agent->debug_level)
//        {
//            debug_fd_lock.lock();
//            fprintf(agent->get_debug_fd(), "TRANSFER_ERROR_QUEUEFULL\n");
//            fflush(agent->get_debug_fd());
//            debug_fd_lock.unlock();
//        }
//        return TRANSFER_ERROR_QUEUEFULL;
//    }

    //    tx_out.state = STATE_QUEUE;
    if (tx_out.file_name.size())
    {
        tx_out.filepath = data_base_path(tx_out.node_name, "outgoing", tx_out.agent_name, tx_out.file_name);
    }
    else
    {
        if (agent->debug_level)
        {
            debug_fd_lock.lock();
            fprintf(agent->get_debug_fd(), "TRANSFER_ERROR_FILENAME\n");
            fflush(agent->get_debug_fd());
            debug_fd_lock.unlock();
        }
        tx_out.filepath = "";
        return TRANSFER_ERROR_FILENAME;
    }

    tx_out.temppath = data_base_path(tx_out.node_name, "temp", "file", "out_"+std::to_string(tx_out.tx_id));

    // Check for a duplicate file name of something already in queue
    uint16_t minindex = 255 - static_cast<uint16_t>(pow(pow(TRANSFER_QUEUE_LIMIT,1.f/3.f), (3.f - log10f(tx_out.file_size) / 2.f)));
    for (uint16_t i=minindex; i<256; ++i)
    {
        if (!txq[(node_id)].outgoing.progress[i].filepath.empty() && tx_out.filepath == txq[(node_id)].outgoing.progress[i].filepath)
        {
            // Remove the META file
            if (agent->debug_level)
            {
                debug_fd_lock.lock();
                fprintf(agent->get_debug_fd(), "%u %s %s %s TRANSFER_ERROR_DUPLICATE\n", tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.filepath.c_str());
                fflush(agent->get_debug_fd());
                debug_fd_lock.unlock();
            }
            string filepath = tx_out.temppath + ".meta";
            remove(filepath.c_str());
            return TRANSFER_ERROR_DUPLICATE;
        }
    }

    tx_out.fp = nullptr;
    //get the file size
    tx_out.file_size = get_file_size(tx_out.filepath);
    tx_out.savetime = 0.;

    // save and queue metadata packet
    tx_out.havemeta = true;
    tx_out.sendmeta = true;
    tx_out.sentmeta = false;
    tx_out.senddata = false;
    tx_out.sentdata = false;
    tx_out.complete = false;

    if (agent->debug_level)
    {
        debug_fd_lock.lock();
        fprintf(agent->get_debug_fd(), "%u %s %s %s %d ", tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.filepath.c_str(), PROGRESS_QUEUE_SIZE);
        fflush(agent->get_debug_fd());
        debug_fd_lock.unlock();
    }

    // Good to go. Add it to queue.
    outgoing_tx_lock.lock();
    //    txq[(node_id)].outgoing.progress[tx_out.tx_id] = tx_out;

    txq[(node_id)].outgoing.progress[tx_out.tx_id].tx_id = tx_out.tx_id;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].havemeta = tx_out.havemeta;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].sendmeta = tx_out.sendmeta;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].sentmeta = tx_out.sentmeta;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].senddata = tx_out.senddata;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].sentdata = tx_out.sentdata;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].complete = tx_out.complete;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].node_name = tx_out.node_name;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].agent_name = tx_out.agent_name;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].file_name = tx_out.file_name;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].filepath = tx_out.filepath;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].temppath = tx_out.temppath;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].savetime = tx_out.savetime;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].datatime = tx_out.datatime;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].file_size = tx_out.file_size;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].total_bytes = tx_out.total_bytes;
    txq[(node_id)].outgoing.progress[tx_out.tx_id].file_info.clear();
    for (file_progress filep : tx_out.file_info)
    {
        txq[(node_id)].outgoing.progress[tx_out.tx_id].file_info.push_back(filep);
    }
    txq[(node_id)].outgoing.progress[tx_out.tx_id].fp = tx_out.fp;
    ++txq[(node_id)].outgoing.size;
    outgoing_tx_lock.unlock();

    if (agent->debug_level)
    {
        debug_fd_lock.lock();
        fprintf(agent->get_debug_fd(), " %u\n", txq[(node_id)].outgoing.size);
        fflush(agent->get_debug_fd());
        debug_fd_lock.unlock();
    }

    return outgoing_tx_recount(node_id);
}

int32_t outgoing_tx_add(string node_name, string agent_name, string file_name)
{
    if (node_name.empty() || agent_name.empty() || file_name.empty())
    {
        if (agent->debug_level)
        {
            debug_fd_lock.lock();
            fprintf(agent->get_debug_fd(), "%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_FILENAME\n", tet.split(), dt.lap());
            fflush(agent->get_debug_fd());
            debug_fd_lock.unlock();
        }
        return TRANSFER_ERROR_FILENAME;
    }

    // BEGIN GATHERING THE METADATA
    tx_progress tx_out;

    uint8_t node_id = lookup_node_id(node_name);
    if (node_id == 0)
    {
        return TRANSFER_ERROR_NODE;
    }

    // Only add if we have room
    if (txq[(node_id)].outgoing.size == PROGRESS_QUEUE_SIZE)
    {
        return TRANSFER_ERROR_QUEUEFULL;
    }

    // Locate next empty space
    //get the file size
    outgoing_tx_lock.lock();
    string filepath = data_base_path(node_name, "outgoing", agent_name, file_name);
    int32_t file_size = get_file_size(filepath);

    // Go through existing queue
    // - if it is already there and the size is different, remove it
    // - if it is already there, and the size is the same, set enable it
    // - if it is not already there and there is room to add is, go on to next step
    for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
    {
        if (txq[(node_id)].outgoing.progress[i].filepath == filepath)
        {
            if (txq[(node_id)].outgoing.progress[i].file_size == file_size)
            {
                txq[(node_id)].outgoing.progress[i].enabled = true;
                if (agent->debug_level)
                {
                    debug_fd_lock.lock();
                    fprintf(agent->get_debug_fd(), "%.4f %.4f Main: outgoing_tx_add: Enable %u %s %s %s %d ", tet.split(), dt.lap(), txq[(node_id)].outgoing.progress[i].tx_id, txq[(node_id)].outgoing.progress[i].node_name.c_str(), txq[(node_id)].outgoing.progress[i].agent_name.c_str(), txq[(node_id)].outgoing.progress[i].filepath.c_str(), PROGRESS_QUEUE_SIZE);
                    fflush(agent->get_debug_fd());
                    debug_fd_lock.unlock();
                }
                return node_id;
            }
            else
            {
                outgoing_tx_del(node_id, i, false);
                return TRANSFER_ERROR_FILESIZE;
            }
        }
    }

    uint16_t minindex = TRANSFER_QUEUE_LIMIT - static_cast<uint16_t>(pow(pow(TRANSFER_QUEUE_LIMIT,1.f/3.f), (3.f - log10f(file_size) / 2.f)));
    tx_out.tx_id = 0;
    for (uint16_t id=minindex; id<PROGRESS_QUEUE_SIZE; ++id)
    {
        if (txq[(node_id)].outgoing.progress[id].tx_id == 0)
        {
            tx_out.tx_id = id;
            break;
        }
    }
    outgoing_tx_lock.unlock();

    if (tx_out.tx_id > 0)
    {
        tx_out.havemeta = true;
        tx_out.sendmeta = true;
        tx_out.sentmeta = false;
        tx_out.senddata = false;
        tx_out.sentdata = false;
        tx_out.complete = false;
        tx_out.total_bytes = 0;
        tx_out.node_name = node_name;
        tx_out.agent_name = agent_name;
        tx_out.file_name = file_name;
        tx_out.temppath = data_base_path(tx_out.node_name, "temp", "file", "out_"+std::to_string(tx_out.tx_id));
        tx_out.filepath = filepath;
//        tx_out.filepath = data_base_path(tx_out.node_name, "outgoing", tx_out.agent_name, tx_out.file_name);
        tx_out.savetime = 0.;

        std::ifstream filename;

        //get the file size
//        tx_out.file_size = get_file_size(tx_out.filepath);
        tx_out.file_size = file_size;

        if(tx_out.file_size < 0)
        {
            if (agent->debug_level)
            {
                debug_fd_lock.lock();
                fprintf(agent->get_debug_fd(), "%.4f %.4f Main: outgoing_tx_add: DATA_ERROR_SIZE_MISMATCH\n", tet.split(), dt.lap());
                fflush(agent->get_debug_fd());
                debug_fd_lock.unlock();
            }
            return DATA_ERROR_SIZE_MISMATCH;
        }

        // see if file can be opened
        filename.open(tx_out.filepath, std::ios::in|std::ios::binary);
        if(!filename.is_open())
        {
            if (agent->debug_level)
            {
                debug_fd_lock.lock();
                fprintf(agent->get_debug_fd(), "%.4f %.4f Main: outgoing_tx_add: %s\n", tet.split(), dt.lap(), cosmos_error_string(-errno).c_str());
                fflush(agent->get_debug_fd());
                debug_fd_lock.unlock();
            }
            return -errno;
        }
        filename.close();

        file_progress tp;
        tp.chunk_start = 0;
        tp.chunk_end = tx_out.file_size - 1;
        tx_out.file_info.push_back(tp);

        write_meta(tx_out);

        int32_t iretn = outgoing_tx_add(tx_out);
        return iretn;
    }
    else
    {
        return TRANSFER_ERROR_MATCH;
    }
}

int32_t outgoing_tx_del(uint8_t node_id, uint16_t tx_id, bool remove_file)
{
    if (node_id == 0 || node_id >= txq.size())
    {
        return TRANSFER_ERROR_INDEX;
    }

    if (txq[(node_id)].outgoing.progress[tx_id].tx_id == 0)
    {
        return TRANSFER_ERROR_MATCH;
    }

    if (tx_id >= PROGRESS_QUEUE_SIZE)
    {
        for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
        {
            outgoing_tx_del(node_id, i);
        }
    }
    else
    {
        tx_progress tx_out = txq[(node_id)].outgoing.progress[tx_id];

        // erase the transaction

        txq[(node_id)].outgoing.progress[tx_id].fp = nullptr;
        txq[(node_id)].outgoing.progress[tx_id].enabled = false;
        txq[(node_id)].outgoing.progress[tx_id].tx_id = 0;
        txq[(node_id)].outgoing.progress[tx_id].complete = false;
        txq[(node_id)].outgoing.progress[tx_id].filepath = "";
        txq[(node_id)].outgoing.progress[tx_id].havemeta = false;
        txq[(node_id)].outgoing.progress[tx_id].savetime = 0;
        txq[(node_id)].outgoing.progress[tx_id].temppath = "";
        txq[(node_id)].outgoing.progress[tx_id].file_name = "";
        txq[(node_id)].outgoing.progress[tx_id].file_size = 0;
        txq[(node_id)].outgoing.progress[tx_id].node_name = "";
        txq[(node_id)].outgoing.progress[tx_id].agent_name = "";
        txq[(node_id)].outgoing.progress[tx_id].total_bytes = 0;
        txq[(node_id)].outgoing.progress[tx_id].file_info.clear();

        if (txq[(node_id)].outgoing.size)
        {
            --txq[(node_id)].outgoing.size;
        }

        // Remove the file
        if(remove_file && remove(tx_out.filepath.c_str()))
        {
            if (agent->debug_level)
            {
                debug_fd_lock.lock();
                fprintf(agent->get_debug_fd(), "%.4f %.4f Main/Outgoing: Del outgoing: %u %s %s %s - Unable to remove file\n", tet.split(), dt.lap(), tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str());
                fflush(agent->get_debug_fd());
                debug_fd_lock.unlock();
            }
        }

        // Remove the META file
        string meta_filepath = tx_out.temppath + ".meta";
        remove(meta_filepath.c_str());

        if (agent->debug_level)
        {
            debug_fd_lock.lock();
            fprintf(agent->get_debug_fd(), "%.4f %.4f Main/Outgoing: Del outgoing: %u %s %s %s\n", tet.split(), dt.lap(), tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str());
            fflush(agent->get_debug_fd());
            debug_fd_lock.unlock();
        }
    }

    return outgoing_tx_recount(node_id);
}

int32_t outgoing_tx_purge(uint8_t node_id, uint16_t tx_id)
{
    if (node_id == 0 || node_id >= txq.size())
    {
        return TRANSFER_ERROR_INDEX;
    }

    if (tx_id >= PROGRESS_QUEUE_SIZE)
    {
        for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
        {
            txq[(node_id)].outgoing.progress[i].fp = nullptr;
            txq[(node_id)].outgoing.progress[i].tx_id = 0;
            txq[(node_id)].outgoing.progress[i].complete = false;
            txq[(node_id)].outgoing.progress[i].filepath = "";
            txq[(node_id)].outgoing.progress[i].havemeta = false;
            txq[(node_id)].outgoing.progress[i].savetime = 0;
            txq[(node_id)].outgoing.progress[i].temppath = "";
            txq[(node_id)].outgoing.progress[i].file_name = "";
            txq[(node_id)].outgoing.progress[i].file_size = 0;
            txq[(node_id)].outgoing.progress[i].node_name = "";
            txq[(node_id)].outgoing.progress[i].agent_name = "";
            txq[(node_id)].outgoing.progress[i].total_bytes = 0;
            txq[(node_id)].outgoing.progress[i].file_info.clear();
        }
    }
    else {
        {
            txq[(node_id)].outgoing.progress[tx_id].fp = nullptr;
            txq[(node_id)].outgoing.progress[tx_id].tx_id = 0;
            txq[(node_id)].outgoing.progress[tx_id].complete = false;
            txq[(node_id)].outgoing.progress[tx_id].filepath = "";
            txq[(node_id)].outgoing.progress[tx_id].havemeta = false;
            txq[(node_id)].outgoing.progress[tx_id].savetime = 0;
            txq[(node_id)].outgoing.progress[tx_id].temppath = "";
            txq[(node_id)].outgoing.progress[tx_id].file_name = "";
            txq[(node_id)].outgoing.progress[tx_id].file_size = 0;
            txq[(node_id)].outgoing.progress[tx_id].node_name = "";
            txq[(node_id)].outgoing.progress[tx_id].agent_name = "";
            txq[(node_id)].outgoing.progress[tx_id].total_bytes = 0;
            txq[(node_id)].outgoing.progress[tx_id].file_info.clear();
        }
    }
    txq[(node_id)].outgoing.size = 0;

    return 0;
}

int32_t outgoing_tx_recount(uint8_t node_id)
{
    if (node_id == 0 || node_id >= txq.size())
    {
        return TRANSFER_ERROR_INDEX;
    }

    txq[(node_id)].outgoing.size = 0;
    for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
    {
        if (txq[(node_id)].outgoing.progress[i].tx_id)
        {
            ++txq[(node_id)].outgoing.size;
        }
    }
    return txq[(node_id)].outgoing.size;
}

int32_t outgoing_tx_load(uint8_t node_id)
{
    int32_t iretn=0;

    // Go through outgoing queue, removing files that no longer exist
    for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
    {
        if (txq[(node_id)].outgoing.progress[i].tx_id != 0 && !data_isfile(txq[(node_id)].outgoing.progress[i].filepath))
        {
            outgoing_tx_del(node_id, txq[(node_id)].outgoing.progress[i].tx_id);
        }
    }

    // Go through outgoing directories, adding files not already in queue
    if (txq[(node_id)].outgoing.size < PROGRESS_QUEUE_SIZE)
    {
        vector<filestruc> file_names;
        for (filestruc file : data_list_files(txq[(node_id)].node_name, "outgoing", ""))
        {
            if (file.type == "directory")
            {
                iretn = data_list_files(txq[(node_id)].node_name, "outgoing", file.name, file_names);
            }
        }

        // Sort list by size, then go through list of files found, adding to queue.
        sort(file_names.begin(), file_names.end(), filestruc_smaller_by_size);
        for(uint16_t i=0; i<file_names.size(); ++i)
        {
            filestruc file = file_names[i];
            if (txq[(node_id)].outgoing.size >= PROGRESS_QUEUE_SIZE)
            {
                break;
            }

            //Ignore sub-directories
            if (file.type == "directory")
            {
                continue;
            }

            // Ignore zero length files (may still be being formed)
//            if (file.size == 0)
//            {
//                continue;
//            }

            // Go through existing queue
            // - if it is already there and the size is different, remove it from queue
            // - if it is already there, and the size is the same, enable it
            // - if it is enabled and the size is zero, remove it from queue and remove file
            bool addtoqueue = true;
            outgoing_tx_lock.lock();
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[(node_id)].outgoing.progress[i].filepath == file.path)
                {
                    if (txq[(node_id)].outgoing.progress[i].file_size == file.size)
                    {
                        addtoqueue = false;
                        if (!txq[(node_id)].outgoing.progress[i].enabled)
                        {
                            txq[(node_id)].outgoing.progress[i].enabled = true;
                            if (agent->debug_level)
                            {
                                debug_fd_lock.lock();
                                fprintf(agent->get_debug_fd(), "%.4f %.4f Main: outgoing_tx_add: Enable %u %s %s %s %d\n", tet.split(), dt.lap(), txq[(node_id)].outgoing.progress[i].tx_id, txq[(node_id)].outgoing.progress[i].node_name.c_str(), txq[(node_id)].outgoing.progress[i].agent_name.c_str(), txq[(node_id)].outgoing.progress[i].filepath.c_str(), PROGRESS_QUEUE_SIZE);
                                fflush(agent->get_debug_fd());
                                debug_fd_lock.unlock();

                            }
                        }
                        iretn = 0;
                    }
                    else
                    {
                        outgoing_tx_del(node_id, i, false);
                        addtoqueue = false;
                        iretn = TRANSFER_ERROR_FILESIZE;
                    }
                    if (txq[(node_id)].outgoing.progress[i].enabled && txq[(node_id)].outgoing.progress[i].file_size == 0)
                    {
                        outgoing_tx_del(node_id, i, true);
                        addtoqueue = false;
                        iretn = TRANSFER_ERROR_FILEZERO;
                    }
                }
            }

            outgoing_tx_lock.unlock();

            if (addtoqueue)
            {
                iretn = outgoing_tx_add(file.node, file.agent, file.name);
                if (agent->debug_level && iretn != -471)
                {
                    debug_fd_lock.lock();
                    fprintf(agent->get_debug_fd(), "%.4f %.4f Main/Load: outgoing_tx_add: %s [%d]\n", tet.split(), dt.lap(), file.path.c_str(), iretn);
                    fflush(agent->get_debug_fd());
                    debug_fd_lock.unlock();
                }
            }
        }
    }

    return iretn;
}

int32_t incoming_tx_add(tx_progress &tx_in)
{
    uint8_t node_id = lookup_node_id(tx_in.node_name);
    if (node_id == 0)
    {
        if (agent->debug_level)
        {
            debug_fd_lock.lock();
            fprintf(agent->get_debug_fd(), "%.4f %.4f Main: incoming_tx_add: TRANSFER_ERROR_NODE\n", tet.split(), dt.lap());
            fflush(agent->get_debug_fd());
            debug_fd_lock.unlock();
        }
        return TRANSFER_ERROR_NODE;
    }

    // Check for an actual file name
    if (tx_in.file_name.size())
    {
        tx_in.filepath = data_base_path(tx_in.node_name, "incoming", tx_in.agent_name, tx_in.file_name);
    }
    else
    {
        tx_in.filepath = "";
    }

    string tx_name = "in_"+std::to_string(tx_in.tx_id);
    tx_in.temppath = data_base_path(tx_in.node_name, "temp", "file", tx_name);

    // Check for a duplicate file name of something already in queue
    for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
    {
        if (!txq[(node_id)].incoming.progress[i].filepath.empty() && tx_in.filepath == txq[(node_id)].incoming.progress[i].filepath)
        {
            if (agent->debug_level)
            {
                debug_fd_lock.lock();
                fprintf(agent->get_debug_fd(), "%u %s %s %s TRANSFER_ERROR_DUPLICATE\n", tx_in.tx_id, tx_in.node_name.c_str(), tx_in.agent_name.c_str(), tx_in.filepath.c_str());
                fflush(agent->get_debug_fd());
                debug_fd_lock.unlock();
            }
            // Remove the META file
            string filepath = tx_in.temppath + ".meta";
            remove(filepath.c_str());
            return TRANSFER_ERROR_DUPLICATE;
        }
    }

    tx_in.savetime = 0.;
    tx_in.fp = nullptr;
    //    tx_in.state = STATE_REQMETA;
    //    if (tx_in.havemeta)
    //    {
    //        tx_in.state = STATE_REQDATA;
    //    }
    //    if (tx_in.complete)
    //    {
    //        tx_in.state = STATE_COMPLETE;
    //    }

    // Put it in list
    //    txq[(node_id)].incoming.progress[tx_in.tx_id] = tx_in;
    txq[(node_id)].incoming.progress[tx_in.tx_id].tx_id = tx_in.tx_id;
    txq[(node_id)].incoming.progress[tx_in.tx_id].havemeta = tx_in.havemeta;
    txq[(node_id)].incoming.progress[tx_in.tx_id].sendmeta = tx_in.sendmeta;
    txq[(node_id)].incoming.progress[tx_in.tx_id].sentmeta = tx_in.sentmeta;
    txq[(node_id)].incoming.progress[tx_in.tx_id].senddata = tx_in.senddata;
    txq[(node_id)].incoming.progress[tx_in.tx_id].sentdata = tx_in.sentdata;
    txq[(node_id)].incoming.progress[tx_in.tx_id].complete = tx_in.complete;
    txq[(node_id)].incoming.progress[tx_in.tx_id].node_name = tx_in.node_name;
    txq[(node_id)].incoming.progress[tx_in.tx_id].agent_name = tx_in.agent_name;
    txq[(node_id)].incoming.progress[tx_in.tx_id].file_name = tx_in.file_name;
    txq[(node_id)].incoming.progress[tx_in.tx_id].filepath = tx_in.filepath;
    txq[(node_id)].incoming.progress[tx_in.tx_id].temppath = tx_in.temppath;
    txq[(node_id)].incoming.progress[tx_in.tx_id].savetime = tx_in.savetime;
    txq[(node_id)].incoming.progress[tx_in.tx_id].file_size = tx_in.file_size;
    txq[(node_id)].incoming.progress[tx_in.tx_id].total_bytes = tx_in.total_bytes;
    txq[(node_id)].incoming.progress[tx_in.tx_id].file_info.clear();
    for (file_progress filep : tx_in.file_info)
    {
        txq[(node_id)].incoming.progress[tx_in.tx_id].file_info.push_back(filep);
    }
    txq[(node_id)].incoming.progress[tx_in.tx_id].fp = tx_in.fp;
    ++txq[(node_id)].incoming.size;

    if (agent->debug_level)
    {
        debug_fd_lock.lock();
        fprintf(agent->get_debug_fd(), "%.4f %.4f Main/Incoming: Add incoming: %u %s %s %s %d\n", tet.split(), dt.lap(), tx_in.tx_id, tx_in.node_name.c_str(), tx_in.agent_name.c_str(), tx_in.filepath.c_str(), PROGRESS_QUEUE_SIZE);
        fflush(agent->get_debug_fd());
        debug_fd_lock.unlock();
    }

    return incoming_tx_recount(node_id);
}

int32_t incoming_tx_add(string node_name, PACKET_TX_ID_TYPE tx_id)
{
    tx_progress tx_in;

    tx_in.tx_id = tx_id;
    tx_in.sendmeta = true;
    tx_in.havemeta = false;
    tx_in.sentmeta = false;
    tx_in.senddata = false;
    tx_in.sentdata = false;
    tx_in.complete = false;
    tx_in.node_name = node_name;
    tx_in.agent_name = "";
    tx_in.file_name = "";
    tx_in.savetime = 0.;
    tx_in.file_size = 0;
    tx_in.total_bytes = 0;
    tx_in.file_info.clear();

    int32_t iretn = incoming_tx_add(tx_in);

    return iretn;
}

int32_t incoming_tx_update(packet_struct_metashort meta)
{
    int32_t node_id = check_node_id(meta.node_id);
    if (node_id <= 0)
    {
        if (node_id < 0)
        {
            return node_id;
        }
        else
        {
            return TRANSFER_ERROR_NODE;
        }
    }

    if (meta.tx_id)
    {
        txq[node_id].incoming.rcvdmeta = true;
        if (txq[(node_id)].incoming.progress[meta.tx_id].tx_id != meta.tx_id)
        {
            txq[(node_id)].incoming.progress[meta.tx_id].tx_id = meta.tx_id;
            txq[(node_id)].incoming.progress[meta.tx_id].havemeta = false;
        }

        txq[(node_id)].incoming.progress[meta.tx_id].sendmeta = false;
        txq[(node_id)].incoming.progress[meta.tx_id].sentmeta = true;
        txq[(node_id)].incoming.progress[meta.tx_id].senddata = false;
        txq[(node_id)].incoming.progress[meta.tx_id].sentdata = false;
        txq[(node_id)].incoming.progress[meta.tx_id].datatime = currentmjd();

        if (!txq[(node_id)].incoming.progress[meta.tx_id].havemeta)
        {
            // Core META information
            txq[(node_id)].incoming.progress[meta.tx_id].node_name = txq[(node_id)].node_name;
            txq[(node_id)].incoming.progress[meta.tx_id].agent_name = meta.agent_name;
            txq[(node_id)].incoming.progress[meta.tx_id].file_name = meta.file_name;
            txq[(node_id)].incoming.progress[meta.tx_id].file_size = meta.file_size;
            txq[(node_id)].incoming.progress[meta.tx_id].filepath = data_base_path(txq[(node_id)].incoming.progress[meta.tx_id].node_name, "incoming", txq[(node_id)].incoming.progress[meta.tx_id].agent_name, txq[(node_id)].incoming.progress[meta.tx_id].file_name);
            string tx_name = "in_"+std::to_string(txq[(node_id)].incoming.progress[meta.tx_id].tx_id);
            txq[(node_id)].incoming.progress[meta.tx_id].temppath = data_base_path(txq[(node_id)].incoming.progress[meta.tx_id].node_name, "temp", "file", tx_name);

            // Derivative META information
            //            txq[(node_id)].incoming.progress[meta.tx_id].state = STATE_REQDATA;
            txq[(node_id)].incoming.progress[meta.tx_id].savetime = 0.;
            txq[(node_id)].incoming.progress[meta.tx_id].havemeta = true;
            txq[(node_id)].incoming.progress[meta.tx_id].complete = false;
            txq[(node_id)].incoming.progress[meta.tx_id].total_bytes = 0;
            txq[(node_id)].incoming.progress[meta.tx_id].fp = nullptr;
            txq[(node_id)].incoming.progress[meta.tx_id].file_info.clear();

            // Save it to disk
            write_meta(txq[(node_id)].incoming.progress[meta.tx_id]);
        }

        if (agent->debug_level)
        {
            debug_fd_lock.lock();
            fprintf(agent->get_debug_fd(), "%.4f %.4f Incoming: Update incoming: %u %s %s %s\n", tet.split(), dt.lap(), txq[(node_id)].incoming.progress[meta.tx_id].tx_id, txq[(node_id)].incoming.progress[meta.tx_id].node_name.c_str(), txq[(node_id)].incoming.progress[meta.tx_id].agent_name.c_str(), txq[(node_id)].incoming.progress[meta.tx_id].file_name.c_str());
            fflush(agent->get_debug_fd());
            debug_fd_lock.unlock();
        }

        return meta.tx_id;
    }
    else
    {
        return TRANSFER_ERROR_INDEX;
    }
}

int32_t incoming_tx_del(uint8_t node_id, uint16_t tx_id)
{
    node_id = check_node_id(node_id);
    if (node_id == 0)
    {
        return TRANSFER_ERROR_NODE;
    }

    if (tx_id >= PROGRESS_QUEUE_SIZE)
    {
        for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
        {
            incoming_tx_del(node_id, i);
        }
    }
    else
    {
        if (txq[(node_id)].incoming.progress[tx_id].tx_id == 0)
        {
            return TRANSFER_ERROR_MATCH;
        }

        tx_progress tx_in = txq[(node_id)].incoming.progress[tx_id];

        txq[(node_id)].incoming.progress[tx_id].tx_id = 0;
        txq[(node_id)].incoming.progress[tx_id].havemeta = false;
        txq[(node_id)].incoming.progress[tx_id].havedata = false;
        txq[(node_id)].incoming.progress[tx_id].complete = false;
        if (txq[(node_id)].incoming.size)
        {
            --txq[(node_id)].incoming.size;
        }

        // Move file to its final location
        if (tx_in.complete)
        {
            // Close the DATA file
            if (tx_in.fp != nullptr)
            {
                fclose(tx_in.fp);
                tx_in.fp = nullptr;
            }
            string final_filepath = tx_in.temppath + ".file";
            int iret = rename(final_filepath.c_str(), tx_in.filepath.c_str());
            // Make sure metadata is recorded
            write_meta(tx_in, 0.);
            if (agent->debug_level)
            {
                debug_fd_lock.lock();
                fprintf(agent->get_debug_fd(), "%.4f %.4f Incoming: Renamed/Data: %d %s\n", tet.split(), dt.lap(), iret, tx_in.filepath.c_str());
                fflush(agent->get_debug_fd());
                debug_fd_lock.unlock();
            }

            // Mark complete
            tx_in.complete = true;
            tx_in.senddata = false;
            tx_in.sentdata = true;
        }

        string filepath;
        //Remove the DATA file
        filepath = tx_in.temppath + ".file";
        remove(filepath.c_str());

        // Remove the META file
        filepath = tx_in.temppath + ".meta";
        remove(filepath.c_str());

        if (agent->debug_level)
        {
            debug_fd_lock.lock();
            fprintf(agent->get_debug_fd(), "%.4f %.4f Incoming: Del incoming: %u %s\n", tet.split(), dt.lap(), tx_in.tx_id, tx_in.node_name.c_str());
            fflush(agent->get_debug_fd());
            debug_fd_lock.unlock();
        }
    }

    return incoming_tx_recount(node_id);

}

int32_t incoming_tx_purge(uint8_t node_id, uint16_t tx_id)
{
    if (node_id == 0 || node_id >= txq.size())
    {
        return TRANSFER_ERROR_INDEX;
    }

    if (tx_id >= PROGRESS_QUEUE_SIZE)
    {
        for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
        {
            txq[(node_id)].incoming.progress[i].fp = nullptr;
            txq[(node_id)].incoming.progress[i].tx_id = 0;
            txq[(node_id)].incoming.progress[i].complete = false;
            txq[(node_id)].incoming.progress[i].filepath = "";
            txq[(node_id)].incoming.progress[i].havemeta = false;
            txq[(node_id)].incoming.progress[i].savetime = 0;
            txq[(node_id)].incoming.progress[i].temppath = "";
            txq[(node_id)].incoming.progress[i].file_name = "";
            txq[(node_id)].incoming.progress[i].file_size = 0;
            txq[(node_id)].incoming.progress[i].node_name = "";
            txq[(node_id)].incoming.progress[i].agent_name = "";
            txq[(node_id)].incoming.progress[i].total_bytes = 0;
            txq[(node_id)].incoming.progress[i].file_info.clear();
        }
    }
    else {
        {
            txq[(node_id)].incoming.progress[tx_id].fp = nullptr;
            txq[(node_id)].incoming.progress[tx_id].tx_id = 0;
            txq[(node_id)].incoming.progress[tx_id].complete = false;
            txq[(node_id)].incoming.progress[tx_id].filepath = "";
            txq[(node_id)].incoming.progress[tx_id].havemeta = false;
            txq[(node_id)].incoming.progress[tx_id].savetime = 0;
            txq[(node_id)].incoming.progress[tx_id].temppath = "";
            txq[(node_id)].incoming.progress[tx_id].file_name = "";
            txq[(node_id)].incoming.progress[tx_id].file_size = 0;
            txq[(node_id)].incoming.progress[tx_id].node_name = "";
            txq[(node_id)].incoming.progress[tx_id].agent_name = "";
            txq[(node_id)].incoming.progress[tx_id].total_bytes = 0;
            txq[(node_id)].incoming.progress[tx_id].file_info.clear();
        }
    }
    txq[(node_id)].incoming.size = 0;

    return 0;
}

int32_t incoming_tx_recount(uint8_t node_id)
{
    if (node_id == 0 || node_id >= txq.size())
    {
        return TRANSFER_ERROR_INDEX;
    }

    txq[(node_id)].incoming.size = 0;
    for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
    {
        if (txq[(node_id)].incoming.progress[i].tx_id)
        {
            ++txq[(node_id)].incoming.size;
        }
    }
    return txq[(node_id)].incoming.size;
}

bool filestruc_smaller_by_size(const filestruc& a, const filestruc& b)
{
    return a.size < b.size;
}

bool filestruc_larger_by_size(const filestruc& a, const filestruc& b)
{
    return a.size > b.size;
}

bool tx_progress_compare_by_size(const tx_progress& a, const tx_progress& b)
{
    return a.file_size < b.file_size;
}

PACKET_TX_ID_TYPE choose_incoming_tx_id(uint8_t node_id)
{
    PACKET_TX_ID_TYPE tx_id = 0;

    if (node_id > 0 && node_id < txq.size())
    {
        // Choose file with least data left to send
        PACKET_FILE_SIZE_TYPE nsize = INT32_MAX;
        for (PACKET_FILE_SIZE_TYPE i=1; i < PROGRESS_QUEUE_SIZE; ++i)
        {
            // calculate bytes so far
            merge_chunks_overlap(txq[(node_id)].incoming.progress[i]);

            // Remove anything suspicious: file_size == 0, file_size < total_bytes
            if (txq[(node_id)].incoming.progress[i].havemeta && (txq[(node_id)].incoming.progress[i].file_size == 0 || txq[(node_id)].incoming.progress[i].file_size < txq[(node_id)].incoming.progress[i].total_bytes))
            {
                incoming_tx_del(node_id, txq[(node_id)].incoming.progress[i].tx_id);
            }
            // Choose transactions for which we: have meta and bytes remaining is minimized
            else if (txq[(node_id)].incoming.progress[i].tx_id && txq[(node_id)].incoming.progress[i].havemeta && (txq[(node_id)].incoming.progress[i].file_size - txq[(node_id)].incoming.progress[i].total_bytes) < nsize)
            {
                nsize = txq[(node_id)].incoming.progress[i].file_size - txq[(node_id)].incoming.progress[i].total_bytes;
                tx_id = txq[(node_id)].incoming.progress[i].tx_id;
            }
        }
    }

    return tx_id;
}

PACKET_TX_ID_TYPE choose_outgoing_tx_id(uint8_t node_id)
{
    PACKET_TX_ID_TYPE tx_id = 0;

    if (node_id > 0 && node_id < txq.size())
    {
        // Choose file with least data left to send
        PACKET_FILE_SIZE_TYPE nsize = INT32_MAX;
        for (PACKET_FILE_SIZE_TYPE i=1; i < PROGRESS_QUEUE_SIZE; ++i)
        {
            if (txq[(node_id)].outgoing.progress[i].tx_id && txq[(node_id)].outgoing.progress[i].enabled && txq[(node_id)].outgoing.progress[i].sentmeta && !txq[(node_id)].outgoing.progress[i].sentdata)
            {
                // Remove anything file_size == 0
                if (txq[(node_id)].outgoing.progress[i].file_size == 0)
                {
                    outgoing_tx_del(node_id, txq[(node_id)].outgoing.progress[i].tx_id);
                }
                else
                {
                    // calculate bytes so far
                    merge_chunks_overlap(txq[(node_id)].outgoing.progress[i]);

                    // Remove anything suspicious: file_size < total_bytes
                    if (txq[(node_id)].outgoing.progress[i].file_size < txq[(node_id)].outgoing.progress[i].total_bytes)
                    {
                        outgoing_tx_del(node_id, txq[(node_id)].outgoing.progress[i].tx_id, false);
                    }
                    // Choose unfinished transactions for which bytes remaining is minimized
                    else if (txq[(node_id)].outgoing.progress[i].total_bytes < nsize)
                    {
                        nsize = txq[(node_id)].outgoing.progress[i].total_bytes;
                        tx_id = txq[(node_id)].outgoing.progress[i].tx_id;
                    }
                }
            }
        }
    }

    if (tx_id)
    {
//        vector<file_progress> togo;
//        togo = find_chunks_togo(txq[(node_id)].outgoing.progress[tx_id]);
//        for (file_progress missed : togo)
//        {
//            txq[(node_id)].outgoing.progress[tx_id].file_info.push_back(missed);
//        }

        merge_chunks_overlap(txq[(node_id)].outgoing.progress[tx_id]);
        txq[(node_id)].outgoing.progress[tx_id].senddata = true;
    }
    return tx_id;
}

PACKET_TX_ID_TYPE check_tx_id(tx_entry &txentry, PACKET_TX_ID_TYPE tx_id)
{
    if (tx_id != 0 && txentry.progress[tx_id].tx_id == tx_id)
    {
        return tx_id;
    }
    else
    {
        return 0;
    }
}

int32_t check_channel(PACKET_NODE_ID_TYPE node_id)
{
    for(uint16_t i=0; i<out_comm_channel.size(); ++i)
    {
        if (out_comm_channel[i].node.size() && txq[node_id].node_name == out_comm_channel[i].node)
        {
            return i;
        }
    }
    return -1;
}

int32_t add_node_name(string node_name)
{
    uint8_t node_id;

    node_id = lookup_node_id(node_name);
    if (node_id == 0)
    {
        return GENERAL_ERROR_OUTOFRANGE;
    }

    if (txq[node_id].node_name == node_name)
    {
        return node_id;
    }

    txq[node_id].node_name = node_name;
    txq[node_id].incoming.size = 0;
    txq[node_id].outgoing.next_id = 1;
    txq[node_id].outgoing.size = 0;
    incoming_tx_purge(node_id);
    outgoing_tx_purge(node_id);
    return node_id;
}

int32_t next_incoming_tx(PACKET_NODE_ID_TYPE node, int32_t use_channel)
{
    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[(node)].incoming, choose_incoming_tx_id(node));

    if (tx_id < PROGRESS_QUEUE_SIZE && tx_id > 0)
    {
        // See if we know what the remote node_id is for this
        if (txq[node].node_id > 0)
        {
            // Check if file has been completely received
            if(txq[(node)].incoming.progress[tx_id].file_size == txq[(node)].incoming.progress[tx_id].total_bytes && txq[(node)].incoming.progress[tx_id].havemeta)
            {
                //                tx_progress tx_in = txq[(node)].incoming.progress[tx_id];
                if (agent->debug_level)
                {
                    debug_fd_lock.lock();
                    fprintf(agent->get_debug_fd(), "%.4f %.4f Incoming(next_incoming_tx): Complete: %u %s %u %u\n", tet.split(), dt.lap(), txq[(node)].incoming.progress[tx_id].tx_id, txq[(node)].incoming.progress[tx_id].node_name.c_str(), txq[(node)].incoming.progress[tx_id].file_size, txq[(node)].incoming.progress[tx_id].total_bytes);
                    fflush(agent->get_debug_fd());
                    debug_fd_lock.unlock();
                }

                // Move file to its final location
                if (!txq[(node)].incoming.progress[tx_id].complete)
                {
                    if (txq[(node)].incoming.progress[tx_id].fp != nullptr)
                    {
                        fclose(txq[(node)].incoming.progress[tx_id].fp);
                        txq[(node)].incoming.progress[tx_id].fp = nullptr;
                    }
                    string final_filepath = txq[(node)].incoming.progress[tx_id].temppath + ".file";
                    int32_t iret = rename(final_filepath.c_str(), txq[(node)].incoming.progress[tx_id].filepath.c_str());
                    // Make sure metadata is recorded
                    write_meta(txq[(node)].incoming.progress[tx_id], 0.);
                    if (agent->debug_level)
                    {
                        debug_fd_lock.lock();
                        fprintf(agent->get_debug_fd(), "%.4f %.4f Incoming(next_incoming_tx): Renamed: %d %s\n", tet.split(), dt.lap(), iret, txq[(node)].incoming.progress[tx_id].filepath.c_str());
                        fflush(agent->get_debug_fd());
                        debug_fd_lock.unlock();
                    }
                    txq[(node)].incoming.progress[tx_id].complete = true;
                }
            }
            else
            {
                if (agent->debug_level)
                {
                    debug_fd_lock.lock();
                    fprintf(agent->get_debug_fd(), "%.4f %.4f Incoming(next_incoming_tx): More: %u %s %u %u\n", tet.split(), dt.lap(), txq[(node)].incoming.progress[tx_id].tx_id, txq[(node)].incoming.progress[tx_id].node_name.c_str(), txq[(node)].incoming.progress[tx_id].file_size, txq[(node)].incoming.progress[tx_id].total_bytes);
                    fflush(agent->get_debug_fd());
                    debug_fd_lock.unlock();
                }
            }
        }
    }
    return tx_id;
}

// Send Command
int32_t request_send_command(string &request, string &response, Agent *)
{
    size_t f1 = request.find(' ');
    size_t f2 = request.find(' ', f1 + 1);
    string nodename = request.substr(f1+1, (f2-f1)-1);
    string tcommand = request.substr(f2+1);
    for (uint16_t i=0; i<tcommand.length(); ++i)
    {
        if (tcommand[i] == 0)
        {
            tcommand.resize(i);
            break;
        }
    }
    string command;
    if (tcommand[0] == '{')
    {
        command = tcommand;
    }
    else
    {
        JSONObject jobject("event_name", data_name(nodename, currentmjd(), "file_command", ""));
        jobject.addElement("event_utc", 0.);
        jobject.addElement("event_type", EVENT_TYPE_COMMAND);
        jobject.addElement("event_flag", 0);
        jobject.addElement("event_data", tcommand);
        command = jobject.to_json_string();
    }

    int32_t node_id = lookup_node_id(nodename);
    if (node_id > 0)
    {
        int32_t use_channel;
        if ((use_channel=check_channel(node_id)) > 0)
        {
            vector<PACKET_BYTE> packet;
            make_command_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), command);
            queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Outgoing", packet);
            response = command;
        }
        else
        {
            return use_channel;
        }
    }
    else
    {
        return node_id;
    }


    return 0;
}

// Send Message
int32_t request_send_message(string &request, string &response, Agent *)
{
    size_t f1 = request.find(' ');
    size_t f2 = request.find(' ', f1 + 1);
    string nodename = request.substr(f1+1, (f2-f1)-1);
    string message = request.substr(f2+1);
    for (uint16_t i=0; i<message.length(); ++i)
    {
        if (message[i] == 0)
        {
            message.resize(i);
            break;
        }
    }
    int32_t node_id = lookup_node_id(nodename);
    if (node_id > 0)
    {
        int32_t use_channel;
        if ((use_channel=check_channel(node_id)) > 0)
        {
            vector<PACKET_BYTE> packet;
            make_message_packet(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), message);
            queuesendto(static_cast <PACKET_NODE_ID_TYPE>(node_id), "Outgoing", packet);
        }
        else
        {
            return use_channel;
        }
    }
    else
    {
        return node_id;
    }


    return 0;
}

int32_t request_debug(string &request, string &, Agent *)
{

    string requestString = string(request);
    StringParser sp(requestString, ' ');

    agent->debug_level = sp.getFieldNumberAsDouble(2); // should be getFieldNumberAsBoolean

    std::cout << "agent->debug_level: " << agent->debug_level << std::endl;
    return 0;
}

int32_t request_set_logstride(string &request, string &, Agent *)
{
    double new_logstride;
    sscanf(request.c_str(),"set_logstride %lf",&new_logstride);
    if(new_logstride > 0. )
    {
        logstride_sec = new_logstride;
    }
    return 0;
}

int32_t request_get_logstride(string & , string &response, Agent *)
{
    response =  "{" + to_json("logstride", logstride_sec) + "}";
    return 0;
}

void write_queue_log(double logdate)
{
    string record = json_list_queue(); // to append to file

    log_write(agent->cinfo->node.name, "file", logdate, "", "log", record, log_directory);
    log_move(data_type_path(agent->cinfo->node.name, log_directory, "file", logdate, "log"), data_type_path(agent->cinfo->node.name, "outgoing", "file", logdate, "log"), true);


}

int32_t request_list_incoming_json(string &request, string &response, Agent *agent)
{
    (response = json_list_incoming());
    return 0;
}

int32_t request_list_outgoing_json(string &request, string &response, Agent *agent)
{
    (response = json_list_outgoing().c_str());
    return 0;
}

string json_list_incoming() {
    JSONObject jobj;
    JSONArray incoming;

    incoming.resize(txq.size());
    for (uint16_t node=0; node<txq.size(); ++node)
    {

        JSONObject node_obj("node", txq[node].node_name);
        node_obj.addElement("count", txq[node].incoming.size);

        JSONArray files;
        files.resize(txq[node].incoming.size);
        int i =0;
        for(tx_progress tx : txq[node].incoming.progress)
        {
            if (tx.tx_id)
            {
                JSONObject f("tx_id", tx.tx_id);
                f.addElement("agent", tx.agent_name);
                f.addElement("name", tx.file_name);
                f.addElement("bytes", tx.total_bytes);
                f.addElement("size", tx.file_size);
                files.at(i) = (JSONValue(f));
                i++;
            }
        }
        node_obj.addElement("files", files);
        incoming.at(node) = JSONValue(node_obj);

    }
    jobj.addElement("incoming", incoming);
    return jobj.to_json_string();
}

string json_list_outgoing() {
    JSONObject jobj;
    JSONArray outgoing;

//    outgoing.resize(txq.size());
    for (uint16_t node=0; node<txq.size(); ++node)
    {

        JSONObject node_obj("node", txq[node].node_name);
        node_obj.addElement("count", txq[node].outgoing.size);

        JSONArray files;
//        files.resize(txq[node].outgoing.size);
//        int i =0;
        for(tx_progress tx : txq[node].outgoing.progress)
        {
            if (tx.tx_id)
            {
                JSONObject f("tx_id", tx.tx_id);
                f.addElement("agent", tx.agent_name);
                f.addElement("name", tx.file_name);
                f.addElement("bytes", tx.total_bytes);
                f.addElement("size", tx.file_size);
                files.push_back(JSONValue(f));
//                files.at(i) = (JSONValue(f));
//                i++;
            }
        }
        node_obj.addElement("files", files);
//        outgoing.at(node) = JSONValue(node_obj);
        outgoing.push_back(node_obj);

    }
    jobj.addElement("outgoing", outgoing);
    return jobj.to_json_string();
}
string json_list_queue()
{
    JSONObject jobj;
    JSONArray incoming;
    JSONArray outgoing;

    outgoing.resize(txq.size());
    incoming.resize(txq.size());
    for (uint16_t node=0; node<txq.size(); ++node)
    {

        JSONObject node_in("node", txq[node].node_name);
        node_in.addElement("count", txq[node].incoming.size);

        JSONArray ifiles;
        //        ifiles.resize(txq[node].incoming.size);
        int i =0;
        for(tx_progress tx : txq[node].incoming.progress)
        {
            if (tx.tx_id)
            {
                JSONObject f("tx_id", tx.tx_id);
                f.addElement("agent", tx.agent_name);
                f.addElement("name", tx.file_name);
                f.addElement("bytes", tx.total_bytes);
                f.addElement("size", tx.file_size);
                ifiles.push_back(JSONValue(f));
                ifiles.at(i) = (JSONValue(f));
                //                i++;
            }
        }
        node_in.addElement("files", ifiles);
        incoming.at(node) = JSONValue(node_in);

        JSONObject node_out("node", txq[node].node_name);
        node_out.addElement("count", txq[node].incoming.size);
        JSONArray files;
        //        files.resize(txq[node].outgoing.size);
        for(tx_progress tx : txq[node].outgoing.progress)
        {
            if (tx.tx_id)
            {
                JSONObject f("tx_id", tx.tx_id);
                f.addElement("agent", tx.agent_name);
                f.addElement("name", tx.file_name);
                f.addElement("bytes", tx.total_bytes);
                f.addElement("size", tx.file_size);
                files.push_back(JSONValue(f));
                //                files.at(i) = (JSONValue(f));
                //                i++;
            }
        }
        node_out.addElement("files", files);
        outgoing.at(node) = JSONValue(node_out);

    }
    jobj.addElement("outgoing", outgoing);
    jobj.addElement("incoming", incoming);
    return jobj.to_json_string();
}

void profile_check(int32_t line_number, uint16_t thread)
{
    return;
    static FILE *fp = nullptr;
//    static mutex profile_lock;

    debug_fd_lock.lock();

    if (fp == nullptr)
    {
//        fp = fopen("profile_check.out", "a");
        fp = agent->get_debug_fd();
    }

    if (fp != nullptr)
    {
        fprintf(fp, "%.3f %u %d\n", tet.split(), thread, line_number);
        fflush(fp);
    }

    debug_fd_lock.unlock();

}
