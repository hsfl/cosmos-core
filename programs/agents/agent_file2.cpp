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

#include <algorithm>
#include <cstring>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <sys/select.h>

#define TRANSFER_QUEUE_SIZE 256
#define MAXBUFFERSIZE 1024
// Corrected for 28 byte UDP header. Will have to get more clever if we start using CSP
#define PACKET_SIZE_LO (512-(PACKET_DATA_HEADER_SIZE+28))
#define PACKET_SIZE_PAYLOAD (PACKET_SIZE_LO-PACKET_DATA_HEADER_SIZE)
#define THROUGHPUT_LO 1000
#define PACKET_SIZE_HI (1472-(PACKET_DATA_HEADER_SIZE+28))
#define THROUGHPUT_HI 150000
//#define TRANSFER_QUEUE_LIMIT 10

// Debug Var
static bool debug_flag = true;

#ifdef COSMOS_CYGWIN_OS
#include<sstream>
template <typename T>
std::string std::to_string(T value)
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

/** the (global) name of the agent */
static std::string agentname = "file_";
/** the (global) name of the heartbeat structure */
static beatstruc cbeat;
/** the (global) name of the cosmos data structure */
static Agent *agent;
static uint16_t use_channel = 0;
/** the (global) structure of sending channels */
typedef struct
{
    string node="";
    socket_channel chansock;
    string chanip="";
    PACKET_CHUNK_SIZE_TYPE packet_size=PACKET_SIZE_LO;
    uint32_t throughput=THROUGHPUT_LO;
    double nmjd;
    double lmjd;
} channelstruc;

static vector <channelstruc> comm_channel;
//static socket_channel recv_channel[2];

typedef struct
{
    std::string type;
    uint32_t channel;
    std::vector<PACKET_BYTE> packet;
} transmit_queue_entry;

static std::queue<transmit_queue_entry> transmit_queue;
static std::condition_variable transmit_queue_check;


//Send and receive thread info
void send_loop();
void recv_loop();
void transmit_loop();

// Mutexes to avoid thread collisions
static std::mutex incoming_tx_lock;
static std::mutex outgoing_tx_lock;

static double last_data_receive_time = 0.;
static double next_reqmeta_time = 0.;

typedef struct
{
    PACKET_TX_ID_TYPE state;
    PACKET_TX_ID_TYPE size;
    PACKET_TX_ID_TYPE id;
    PACKET_TX_ID_TYPE next_id;
    double nmjd[7];
    std::string node_name;
    PACKET_NODE_ID_TYPE node_id;
    std::vector<tx_progress> progress;
    vector <PACKET_TX_ID_TYPE> meta_id;
}	tx_entry;

typedef struct
{
    std::string node_name;
    PACKET_NODE_ID_TYPE node_id;
    tx_entry incoming;
    tx_entry outgoing;
} tx_queue;

static std::vector<tx_queue> txq;

int32_t request_debug(char *request, char *response, Agent *agent);
int32_t request_use_channel(char* request, char* response, Agent *agent);
int32_t request_remove_file(char* request, char* response, Agent *agent);
//int32_t request_send_file(char* request, char* response, Agent *agent);
int32_t request_ls(char* request, char* response, Agent *agent);
int32_t request_list_incoming(char* request, char* response, Agent *agent);
int32_t request_list_outgoing(char* request, char* response, Agent *agent);
int32_t outgoing_tx_add(tx_progress tx_out);
int32_t outgoing_tx_add(std::string node_name, std::string agent_name, std::string file_name);
int32_t outgoing_tx_del(int32_t node, PACKET_TX_ID_TYPE tx_id);
int32_t incoming_tx_add(tx_progress tx_in);
int32_t incoming_tx_add(std::string node_name, PACKET_TX_ID_TYPE tx_id);
int32_t incoming_tx_update(packet_struct_metashort meta);
int32_t incoming_tx_del(int32_t node, PACKET_TX_ID_TYPE tx_id);
std::vector<file_progress> find_chunks_missing(tx_progress& tx);
PACKET_FILE_SIZE_TYPE merge_chunks_overlap(tx_progress& tx);
void transmit_loop();
double queuesendto(PACKET_NODE_ID_TYPE node_id, std::string type, std::vector<PACKET_BYTE> packet);
int32_t mysendto(std::string type, channelstruc &channel, std::vector<PACKET_BYTE>& buf);
int32_t myrecvfrom(std::string type, socket_channel &channel, std::vector<PACKET_BYTE>& buf, uint32_t length, double dtimeout=1.);
void debug_packet(std::vector<PACKET_BYTE> buf, std::string type);
int32_t write_meta(tx_progress& tx);
int32_t read_meta(tx_progress& tx);
bool tx_progress_compare_by_size(const tx_progress& a, const tx_progress& b);
bool filestruc_compare_by_size(const filestruc& a, const filestruc& b);
PACKET_TX_ID_TYPE check_tx_id(std::vector<tx_progress> tx_entry, PACKET_TX_ID_TYPE tx_id);
int32_t check_node_id(std::string node_name);
int32_t check_node_id(PACKET_NODE_ID_TYPE node_id);
int32_t check_channel(PACKET_NODE_ID_TYPE node_id);
int32_t lookup_remote_node_id(PACKET_NODE_ID_TYPE node_id);
int32_t set_remote_node_id(PACKET_NODE_ID_TYPE node_id, std::string node_name);
PACKET_TX_ID_TYPE choose_incoming_tx_id(int32_t node);
int32_t next_incoming_tx(PACKET_NODE_ID_TYPE node);

//main
int main(int argc, char *argv[])
{
    int32_t iretn;
    //open sockets for receiving and sending
    printf("- Opening recv socket...");
    fflush(stdout);

    comm_channel.resize(1);
    if((iretn = socket_open(&comm_channel[0].chansock, NetworkType::UDP, (char *)"", AGENTRECVPORT, SOCKET_LISTEN, SOCKET_BLOCKING, 5000000)) < 0)
    {
        std::cout << "iretn = " << iretn << std::endl;
        printf("- Could not successfully open recv socket... exiting \n");
        exit (-errno);
    }
    inet_ntop(comm_channel[0].chansock.caddr.sin_family, &comm_channel[0].chansock.caddr.sin_addr, comm_channel[0].chansock.address, sizeof(comm_channel[0].chansock.address));
    comm_channel[0].chanip = comm_channel[0].chansock.address;
    comm_channel[0].nmjd = currentmjd(0.);
    printf("\tSuccess.\n");

    switch (argc)
    {
    case 2:
        {
            printf("- Opening send socket...");
            fflush(stdout);
            comm_channel.resize(2);
            comm_channel[1].node = argv[1];
            size_t tloc = comm_channel[1].node.find(":");
            if (tloc != string::npos)
            {
                comm_channel[1].chanip = comm_channel[1].node.substr(tloc+1, comm_channel[1].node.size()-tloc+1);
                comm_channel[1].node = comm_channel[1].node.substr(0, tloc);
            }
            if((iretn = socket_open(&comm_channel[1].chansock, NetworkType::UDP, comm_channel[1].chanip.c_str(), AGENTRECVPORT, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) < 0)
            {
                std::cout << "iretn = " << iretn << std::endl;
                printf("- Could not successfully open send socket %s at size %u ... exiting \n", comm_channel[1].chanip.c_str(), comm_channel[1].packet_size);
                exit (-errno);
            }
            comm_channel[1].nmjd = currentmjd(0.);
            printf("\Opened %s on %s.\n", comm_channel[1].node.c_str(), comm_channel[1].chanip.c_str());
            break;
        }
    }

    // set this program up as a server
    // port number = 0 in this case, automatic assignment of port
    printf("- Setting up server...");
    fflush(stdout);

    char hostname[60];
    gethostname(hostname, sizeof (hostname));
    agentname += hostname;
    agent = new Agent("", agentname, 5.);
    if (agent->cinfo == nullptr || !agent->running())
    {
        cout << agentname << ": agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
        exit (AGENT_ERROR_JSON_CREATE);
    }
    printf("\t\tSuccess.\n");
    fflush(stdout); // Ensure this gets printed before blocking call

    // Restore in progress transfers from previous run
    for (std::string node_name : data_list_nodes())
    {
        int32_t node = check_node_id(node_name);

        if (node < 0)
        {
            node = txq.size();
            tx_queue tx;
            tx.node_name = node_name;
            tx.node_id = 0;
            tx.incoming.state = PACKET_QUEUE;
            for (uint16_t i=0; i<7; ++i)
            {
                tx.incoming.nmjd[i] = currentmjd();
            }
            tx.incoming.id = 0;
            tx.incoming.next_id = 1;
            tx.incoming.progress.resize(TRANSFER_QUEUE_SIZE);
            tx.incoming.size = 0;
            tx.outgoing.state = PACKET_QUEUE;
            for (uint16_t i=0; i<7; ++i)
            {
                tx.outgoing.nmjd[i] = currentmjd();
            }
            tx.outgoing.id = 0;
            tx.outgoing.next_id = 1;
            tx.outgoing.progress.resize(TRANSFER_QUEUE_SIZE);
            tx.outgoing.size = 0;
            txq.push_back(tx);
        }

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
                        incoming_tx_add(tx_in);
                    }
                }

                // Outgoing
                if (!file.name.compare(0,4,"out_"))
                {
                    tx_progress tx_out;
                    tx_out.temppath = file.path.substr(0,file.path.find(".meta"));
                    if (read_meta(tx_out) >= 0)
                    {
                        iretn = outgoing_tx_add(tx_out);
                    }
                }
            }
        }
    }

    // add agent_file requests
    if ((iretn=agent->add_request("use_channel",request_use_channel,"{0|1} [throughput]", "choose slow or fast channel")))
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
    if ((iretn=agent->add_request("debug",request_debug,"{0|1}","Toggle Debug information")))
        exit (iretn);

    std::thread send_loop_thread(send_loop);
    std::thread recv_loop_thread(recv_loop);
    std::thread transmit_loop_thread(transmit_loop);

    double nextdiskcheck = currentmjd(0.);
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

        double sleepsec = 86400. * (nextdiskcheck - currentmjd());
        if (sleepsec > 0.)
        {
//            if (debug_flag)
//            {
//                printf("Main Sleep: %f seconds\n", sleepsec);
//                fflush(stdout);
//            }
            COSMOS_USLEEP((uint32_t)(sleepsec*1e6));
        }

        // Check for new files to transmit if queue is not full and check is not delayed

        if (currentmjd() > nextdiskcheck)
        {
            nextdiskcheck = currentmjd(0.) + 10./86400.;
            for (uint16_t node=0; node<txq.size(); ++node)
            {
                if (txq[node].outgoing.size < TRANSFER_QUEUE_LIMIT)
                {
                    std::vector<filestruc> file_names;
                    for (filestruc file : data_list_files(txq[node].node_name, "outgoing", ""))
                    {
                        if (file.type == "directory")
                        {
                            iretn = data_list_files(txq[node].node_name, "outgoing", file.name, file_names);
                        }
                    }

                    // Sort list by size, then go through list of files found, adding to queue.
                    sort(file_names.begin(), file_names.end(), filestruc_compare_by_size);
                    for(filestruc file : file_names)
                    {
                        if (txq[node].outgoing.size >= TRANSFER_QUEUE_LIMIT)
                        {
                            break;
                        }

                        if (file.type == "directory")
                        {
                            continue;
                        }

                        bool addtoqueue = true;
                        outgoing_tx_lock.lock();
                        for(tx_progress progress : txq[node].outgoing.progress)
                        {
                            if (progress.tx_id && file.path == progress.filepath)
                            {
                                addtoqueue = false;
                                break;
                            }
                        }

                        outgoing_tx_lock.unlock();

                        if (addtoqueue)
                        {
                            iretn = outgoing_tx_add(file.node, file.agent, file.name);
                            if (iretn >= 0)
                            {
                                nextdiskcheck = currentmjd();
                            }
                            if (debug_flag)
                            {
                                printf("[%f] outgoing_tx_add: %s [%d]\n", etloop.split(), file.path.c_str(), iretn);
                            }
                        }
                    }
                }
            }
        }
    } // End WHILE Loop

    send_loop_thread.join();
    recv_loop_thread.join();
    transmit_queue_check.notify_one();
    transmit_loop_thread.join();

    agent->shutdown();

    exit (0);
}

void recv_loop()
{
    std::vector<PACKET_BYTE> recvbuf;
    std::string partial_filepath;

    while (agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            COSMOS_SLEEP(1);
            continue;
        }
        else {
            COSMOS_SLEEP(.001);
        }

        int32_t nbytes = 0;
        socket_channel rchannel;
        if (( nbytes = myrecvfrom("rx", rchannel, recvbuf, PACKET_MAX_LENGTH)) > 0)
        {
            // Add channel if this isn't someone we're already talking to
            bool found = false;
            for (uint16_t i=0; i<comm_channel.size(); ++i)
            {
                if (currentmjd() - comm_channel[i].lmjd > 60. / 86400.)
                {
                    comm_channel.erase(comm_channel.begin()+i);
                }
                if (comm_channel[i].chansock.caddr.sin_port == rchannel.caddr.sin_port && comm_channel[i].chansock.caddr.sin_addr.s_addr == rchannel.caddr.sin_addr.s_addr)
                {
                    found = true;
                    use_channel = i;
                }
            }
            if (!found)
            {
                channelstruc tchannel;
                tchannel.nmjd = currentmjd(0.);
                tchannel.chansock = rchannel;
                inet_ntop(tchannel.chansock.caddr.sin_family, &tchannel.chansock.caddr.sin_addr, tchannel.chansock.address, sizeof(tchannel.chansock.address));
                tchannel.chanip = tchannel.chansock.address;
                use_channel = comm_channel.size();
                comm_channel.push_back(tchannel);
            }
            comm_channel[use_channel].lmjd = currentmjd();

            // Respond appropriately according to type of packet
            switch (recvbuf[0] & 0x0f)
            {
            case PACKET_METADATA:
                {
                    packet_struct_metashort meta;

                    extract_metadata(recvbuf, meta);
                    int32_t node = check_node_id(meta.node_id);
                    if (node >= 0)
                    {
                        comm_channel[use_channel].node = txq[node].node_name;
                    }

                    incoming_tx_lock.lock();

                    incoming_tx_update(meta);

                    incoming_tx_lock.unlock();

                    break;
                }
            case PACKET_DATA:
                {
                    packet_struct_data data;

                    extract_data(recvbuf, data.node_id, data.tx_id, data.byte_count, data.chunk_start, data.chunk);

                    last_data_receive_time = currentmjd();

                    // create transaction entry if new, and then add data

                    incoming_tx_lock.lock();

                    int32_t node = check_node_id(data.node_id);

                    if (node >= 0)
                    {
                        comm_channel[use_channel].node = txq[node].node_name;
                        PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node].incoming.progress, data.tx_id);

                        // Update corresponding incoming queue entry if it exists
                        if (tx_id > 0)
                        {
                            // tx_id now points to the valid entry to which we should add the data
                            file_progress tp;
                            tp.chunk_start = data.chunk_start;
                            tp.chunk_end = data.chunk_start + data.byte_count - 1;

                            uint32_t check=0;
                            bool duplicate = false;
                            bool updated = false;

                            // Do we have any data yet?
                            if (!txq[node].incoming.progress[tx_id].file_info.size())
                            {
                                // Add first entry, then write data
                                txq[node].incoming.progress[tx_id].file_info.push_back(tp);
                                txq[node].incoming.progress[tx_id].total_bytes += data.byte_count;
                                updated = true;
                            }
                            else
                            {
                                // Check against existing data
                                for (uint32_t j=0; j<txq[node].incoming.progress[tx_id].file_info.size(); ++j)
                                {
                                    // Check for duplicate
                                    if (tp.chunk_start >= txq[node].incoming.progress[tx_id].file_info[j].chunk_start && tp.chunk_end <= txq[node].incoming.progress[tx_id].file_info[j].chunk_end)
                                    {
                                        duplicate = true;
                                        break;
                                    }
                                    // If we start before this entry
                                    if (tp.chunk_start < txq[node].incoming.progress[tx_id].file_info[j].chunk_start)
                                    {
                                        // If we end before this entry (at least one byte between), insert
                                        if (tp.chunk_end + 1 < txq[node].incoming.progress[tx_id].file_info[j].chunk_start)
                                        {
                                            txq[node].incoming.progress[tx_id].file_info.insert(txq[node].incoming.progress[tx_id].file_info.begin()+j, tp);
                                            txq[node].incoming.progress[tx_id].total_bytes += data.byte_count;
                                            updated = true;
                                            break;
                                        }
                                        // Otherwise, extend the near end
                                        else
                                        {
                                            tp.chunk_end = txq[node].incoming.progress[tx_id].file_info[j].chunk_start - 1;
                                            txq[node].incoming.progress[tx_id].file_info[j].chunk_start = tp.chunk_start;
                                            data.byte_count = (tp.chunk_end - tp.chunk_start) + 1;
                                            txq[node].incoming.progress[tx_id].total_bytes += data.byte_count;
                                            updated = true;
                                            break;
                                        }
                                    }
                                    else
                                    {
                                        // If we overlap on the end, extend the far end
                                        if (tp.chunk_start <= txq[node].incoming.progress[tx_id].file_info[j].chunk_end + 1)
                                        {
                                            if (tp.chunk_end > txq[node].incoming.progress[tx_id].file_info[j].chunk_end)
                                            {
                                                data.byte_count = tp.chunk_end - txq[node].incoming.progress[tx_id].file_info[j].chunk_end;
                                                tp.chunk_start = txq[node].incoming.progress[tx_id].file_info[j].chunk_end + 1;
                                                txq[node].incoming.progress[tx_id].file_info[j].chunk_end = tp.chunk_end;
                                                txq[node].incoming.progress[tx_id].total_bytes += data.byte_count;
                                                updated = true;
                                                break;
                                            }
                                        }
                                    }
                                    check = j + 1;
                                }


                                // If we are higher than everything currently in the list, then append
                                if (!duplicate && check == txq[node].incoming.progress[tx_id].file_info.size())
                                {
                                    txq[node].incoming.progress[tx_id].file_info.push_back(tp);
                                    txq[node].incoming.progress[tx_id].total_bytes += data.byte_count;
                                    updated = true;
                                }

                            }

                            // Write to disk if this is new data
                            if (updated)
                            {
                                // Write incoming data to disk
                                if (txq[node].incoming.progress[tx_id].fp == NULL)
                                {
                                    partial_filepath = txq[node].incoming.progress[tx_id].temppath + ".file";
                                    txq[node].incoming.progress[tx_id].fp = fopen(partial_filepath.c_str(), "w");
                                }

                                if (txq[node].incoming.progress[tx_id].fp == NULL)
                                {
                                    perror(partial_filepath.c_str());
                                }
                                else
                                {
                                    fseek(txq[node].incoming.progress[tx_id].fp, tp.chunk_start, SEEK_SET);
                                    fwrite(data.chunk, data.byte_count, 1, txq[node].incoming.progress[tx_id].fp);
                                    fflush(txq[node].incoming.progress[tx_id].fp);
                                    // Write latest meta data to disk
                                    write_meta(txq[node].incoming.progress[tx_id]);
                                }

                            }

                            // Check if file has been completely received
                            if(txq[node].incoming.progress[tx_id].file_size == txq[node].incoming.progress[tx_id].total_bytes && txq[node].incoming.progress[tx_id].havemeta)
                            {
                                // See if we know what the remote node_id is for this
                                int32_t remote_node = lookup_remote_node_id(node);
                                if (remote_node >= 0)
                                {
                                    tx_progress tx_in = txq[node].incoming.progress[tx_id];

                                    // inform other end that file has been received
                                    std::vector<PACKET_BYTE> packet;
                                    make_complete_packet(packet, remote_node, tx_in.tx_id);
                                    queuesendto(node, "rx", packet);

                                    // Move file to its final location
                                    if (!txq[node].incoming.progress[tx_id].complete)
                                    {
                                        if (txq[node].incoming.progress[tx_id].fp != nullptr)
                                        {
                                            fclose(txq[node].incoming.progress[tx_id].fp);
                                            txq[node].incoming.progress[tx_id].fp = nullptr;
                                        }
                                        std::string final_filepath = tx_in.temppath + ".file";
                                        int iret = rename(final_filepath.c_str(), tx_in.filepath.c_str());
                                        if (debug_flag)
                                        {
                                            printf("Renamed: %d %s\n", iret, tx_in.filepath.c_str());
                                        }
                                        // Mark complete
                                        txq[node].incoming.progress[tx_id].complete = true;
                                    }
                                }
                            }
                        }
                    }

                    incoming_tx_lock.unlock();

                    break;
                }
            case PACKET_REQDATA:
                {
                    packet_struct_reqdata reqdata;

                    extract_reqdata(recvbuf, reqdata);

                    // Simple validity check
                    int32_t node = check_node_id(reqdata.node_id);
                    if (node >= 0)
                    {
                        comm_channel[use_channel].node = txq[node].node_name;
                    }

                    if (node < 0 || reqdata.hole_end < reqdata.hole_start)
                    {
                        break;
                    }

//                    active_node = node;
                    outgoing_tx_lock.lock();

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node].outgoing.progress, reqdata.tx_id);
                    // tx_id now points to the valid entry to which we should add the data

                    if (tx_id > 0)
                    {
                        // Add this chunk to the queue
                        file_progress tp;
                        tp.chunk_start = reqdata.hole_start;
                        tp.chunk_end = reqdata.hole_end;
                        PACKET_FILE_SIZE_TYPE byte_count = (reqdata.hole_end - reqdata.hole_start) + 1;

                        uint32_t check=0;
                        // Anything in the queue yet?
                        if (!txq[node].outgoing.progress[tx_id].file_info.size())
                        {
                            // Add first entry
                            txq[node].outgoing.progress[tx_id].file_info.push_back(tp);
                            txq[node].outgoing.progress[tx_id].total_bytes += byte_count;
                            //							cout<<"[Send] In (new): "<<"["<<0<<":"<<txq[node].outgoing.progress[tx_id].file_info.size()<<"]"<<" tx_id: "<<txq[node].outgoing.progress[tx_id].tx_id<<" chunk:["<<reqdata.hole_start<<":"<<reqdata.hole_end<<"] now:["<<txq[node].outgoing.progress[tx_id].file_info[0].chunk_start<<":"<<txq[node].outgoing.progress[tx_id].file_info[0].chunk_end<< std::endl;
                        }
                        else
                        {
                            // Check against existing data
                            for (uint32_t j=0; j<txq[node].outgoing.progress[tx_id].file_info.size(); ++j)
                            {
                                // If we match this entry
                                if (tp.chunk_start == txq[node].outgoing.progress[tx_id].file_info[j].chunk_start && tp.chunk_end == txq[node].outgoing.progress[tx_id].file_info[j].chunk_end)
                                {
                                    break;
                                }
                                // If we start before this entry
                                if (tp.chunk_start < txq[node].outgoing.progress[tx_id].file_info[j].chunk_start)
                                {
                                    // If we end before this entry (at least one byte between), insert
                                    if (tp.chunk_end + 1 < txq[node].outgoing.progress[tx_id].file_info[j].chunk_start)
                                    {
                                        txq[node].outgoing.progress[tx_id].file_info.insert(txq[node].outgoing.progress[tx_id].file_info.begin()+j, tp);
                                        txq[node].outgoing.progress[tx_id].total_bytes += byte_count;
                                        //										cout<<"[Send] In (insert): "<<"["<<j<<":"<<txq[node].outgoing.progress[tx_id].file_info.size()<<"]"<<" tx_id: "<<txq[node].outgoing.progress[tx_id].tx_id<<" chunk:["<<reqdata.hole_start<<":"<<reqdata.hole_end<<"] now:["<<txq[node].outgoing.progress[tx_id].file_info[j].chunk_start<<":"<<txq[node].outgoing.progress[tx_id].file_info[j].chunk_end<< std::endl;
                                        break;
                                    }
                                    // Otherwise, extend the near end
                                    else
                                    {
                                        tp.chunk_end = txq[node].outgoing.progress[tx_id].file_info[j].chunk_start - 1;
                                        txq[node].outgoing.progress[tx_id].file_info[j].chunk_start = tp.chunk_start;
                                        byte_count = (tp.chunk_end - tp.chunk_start) + 1;
                                        txq[node].outgoing.progress[tx_id].total_bytes += byte_count;
                                        //										cout<<"[Send] In (extend near): "<<"["<<j<<":"<<txq[node].outgoing.progress[tx_id].file_info.size()<<"]"<<" tx_id: "<<txq[node].outgoing.progress[tx_id].tx_id<<" chunk:["<<reqdata.hole_start<<":"<<reqdata.hole_end<<"] now:["<<txq[node].outgoing.progress[tx_id].file_info[j].chunk_start<<":"<<txq[node].outgoing.progress[tx_id].file_info[j].chunk_end<< std::endl;
                                        break;
                                    }
                                }
                                else
                                {
                                    // If we overlap on the end, extend the far end
                                    if (tp.chunk_start <= txq[node].outgoing.progress[tx_id].file_info[j].chunk_end + 1)
                                    {
                                        if (tp.chunk_end > txq[node].outgoing.progress[tx_id].file_info[j].chunk_end)
                                        {
                                            byte_count = tp.chunk_end - txq[node].outgoing.progress[tx_id].file_info[j].chunk_end;
                                            tp.chunk_start = txq[node].outgoing.progress[tx_id].file_info[j].chunk_end + 1;
                                            txq[node].outgoing.progress[tx_id].file_info[j].chunk_end = tp.chunk_end;
                                            txq[node].outgoing.progress[tx_id].total_bytes += byte_count;
                                            //											cout<<"[Send] In (extend far): "<<"["<<j<<":"<<txq[node].outgoing.progress[tx_id].file_info.size()<<"]"<<" tx_id: "<<txq[node].outgoing.progress[tx_id].tx_id<<" chunk:["<<reqdata.hole_start<<":"<<reqdata.hole_end<<"] now:["<<txq[node].outgoing.progress[tx_id].file_info[j].chunk_start<<":"<<txq[node].outgoing.progress[tx_id].file_info[j].chunk_end<< std::endl;
                                            break;
                                        }
                                    }
                                }
                                check = j + 1;
                            }


                            // If we are higher than everything currently in the list, then append
                            if (check == txq[node].outgoing.progress[tx_id].file_info.size())
                            {
                                txq[node].outgoing.progress[tx_id].file_info.push_back(tp);
                                txq[node].outgoing.progress[tx_id].total_bytes += byte_count;
                                //								cout<<"[Send] In (append): "<<"["<<check<<":"<<txq[node].outgoing.progress[tx_id].file_info.size()<<"]"<<" tx_id: "<<txq[node].outgoing.progress[tx_id].tx_id<<" chunk:["<<reqdata.hole_start<<":"<<reqdata.hole_end<<"] now:["<<txq[node].outgoing.progress[tx_id].file_info[check].chunk_start<<":"<<txq[node].outgoing.progress[tx_id].file_info[check].chunk_end<< std::endl;
                            }

                        }

                        // Save meta to disk
                        write_meta(txq[node].outgoing.progress[tx_id]);
                        txq[node].outgoing.id = reqdata.tx_id;
                        txq[node].outgoing.nmjd[PACKET_DATA-8] = currentmjd();
                        txq[node].outgoing.state = PACKET_DATA;
                    }

                    outgoing_tx_lock.unlock();
                    break;
                }
                //Request missing metadata
            case PACKET_REQMETA:
                {
                    packet_struct_reqmeta reqmeta;

                    extract_reqmeta(recvbuf, reqmeta);

                    outgoing_tx_lock.lock();


                    // Send requested META packets
                    int32_t node = set_remote_node_id(reqmeta.node_id, reqmeta.node_name);
                    if (node >= 0)
                    {
                        comm_channel[use_channel].node = txq[node].node_name;
                        // See if we know what the remote node_id is for this
                        int32_t remote_node = lookup_remote_node_id(node);
                        if (remote_node >= 0)
                        {
                            txq[node].outgoing.meta_id.clear();
                            for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                            {
                                PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node].outgoing.progress, reqmeta.tx_id[i]);
                                if (tx_id > 0)
                                {
                                    txq[node].outgoing.meta_id.push_back(tx_id);
//                                    tx_progress tx = txq[node].outgoing.progress[tx_id];
//                                    std::vector<PACKET_BYTE> packet;
//                                    make_metadata_packet(packet, remote_node, tx.tx_id, (char *)tx.file_name.c_str(), tx.file_size, (char *)tx.agent_name.c_str());
//                                    queuesendto(node, "tx", packet);
                                }
                            }
                            txq[node].outgoing.nmjd[PACKET_METADATA-8] = currentmjd();
                            txq[node].outgoing.state = PACKET_METADATA;
                        }
                    }

                    outgoing_tx_lock.unlock();
                    break;
                }
            case PACKET_CANCEL:
                {
                    packet_struct_complete cancel;

                    extract_complete(recvbuf, cancel);

                    int32_t node = check_node_id(cancel.node_id);
                    if (node >= 0)
                    {
                        comm_channel[use_channel].node = txq[node].node_name;
                        incoming_tx_lock.lock();

                        PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node].incoming.progress, cancel.tx_id);

                        if (tx_id > 0)
                        {
                            // Remove the transaction
                            incoming_tx_del(node, tx_id);
                            //							active_node = -1;
                        }

                        next_incoming_tx(node);
                        incoming_tx_lock.unlock();
                    }
                    break;
                }
            case PACKET_COMPLETE:
                {
                    packet_struct_complete complete;

                    extract_complete(recvbuf, complete);

                    int32_t node = check_node_id(complete.node_id);
                    if (node >= 0)
                    {
                        outgoing_tx_lock.lock();
                        comm_channel[use_channel].node = txq[node].node_name;

                        PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node].outgoing.progress, complete.tx_id);

                        if (tx_id > 0)
                        {
                            // See if we know what the remote node_id is for this
                            int32_t remote_node = lookup_remote_node_id(node);
                            if (remote_node >= 0)
                            {
                                txq[node].outgoing.nmjd[PACKET_CANCEL-8] = currentmjd();
                                txq[node].outgoing.state = PACKET_CANCEL;
                                // Remove transaction
//                                outgoing_tx_del(node, tx_id);

                                // Send a CANCEL packet
//                                std::vector<PACKET_BYTE> packet;
//                                make_cancel_packet(packet, remote_node, complete.tx_id);
//                                queuesendto(node, "tx", packet);
                            }
                        }

                        outgoing_tx_lock.unlock();
                    }
                    break;
                }
            case PACKET_QUEUE:
                {
                    packet_struct_queue queue;

                    extract_queue(recvbuf, queue);

                    incoming_tx_lock.lock();

                    // Is this a node we are handling?
                    int32_t node = check_node_id(queue.node_name);
                    if (node >= 0)
                    {
                        comm_channel[use_channel].node = txq[node].node_name;

                        // Set remote node_id
                        txq[node].node_id = queue.node_id + 1;
//                        active_node = node;
                        // Sort through incoming queue and remove anything not in sent queue
                        for (uint16_t tx_id=0; tx_id<TRANSFER_QUEUE_SIZE; ++tx_id)
                        {
                            bool valid = false;
                            for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                            {
                                if (txq[node].incoming.progress[tx_id].tx_id == queue.tx_id[i])
                                {
                                    // Incoming transaction is in outgoing queue
//                                    active_node = node;
                                    valid = true;
                                    break;
                                }
//                                if (valid)
//                                {
//                                    break;
//                                }
                            }

                            if (tx_id && !valid)
                            {
//                                active_node = node;
                                incoming_tx_del(node, tx_id);
                            }
                        }

                        // Sort through sent queue and add anything not in incoming queue
                        for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                        {
                            if (queue.tx_id[i])
                            {
                                PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node].incoming.progress, queue.tx_id[i]);

                                if (tx_id == 0)
                                {
//                                    active_node = node;
                                    incoming_tx_add(queue.node_name, queue.tx_id[i]);
                                }
                            }
                        }

                        // Go through final incoming queue and request any missing meta data
                        if (currentmjd() > next_reqmeta_time)
                        {
                            next_reqmeta_time = currentmjd();
                            std::vector<PACKET_TX_ID_TYPE> tqueue (TRANSFER_QUEUE_LIMIT, 0);
                            PACKET_TX_ID_TYPE iq = 0;
                            for (uint16_t tx_id=1; tx_id<TRANSFER_QUEUE_SIZE; ++tx_id)
                            {
                                if (txq[node].incoming.progress[tx_id].tx_id && !txq[node].incoming.progress[tx_id].havemeta)
                                {
                                    next_reqmeta_time += sizeof(packet_struct_metashort) / (86400. * comm_channel[use_channel].throughput);
                                    tqueue[iq++] = tx_id;
                                }
                                if (iq == TRANSFER_QUEUE_LIMIT)
                                {
                                    break;
                                }
                            }
                            if (iq)
                            {
                                std::vector<PACKET_BYTE> packet;
//                                active_node = node;
                                make_reqmeta_packet(packet, node, txq[node].node_name, tqueue);
                                queuesendto(node, "rx", packet);
                            }
                        }

                        next_incoming_tx(node);
                    }
                    incoming_tx_lock.unlock();
                }
            }
        }
    }
}

void send_loop()
{
    std::vector<PACKET_BYTE> packet;
    uint32_t sleep_time = 1;
    double send_time = 0.;
    static double next_send_time = 0.;
    double current_time;

    current_time = currentmjd();

    while (agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            COSMOS_SLEEP(1);
            continue;
        }

        // If we did nothing last loop, wait at least 100 msec
        if (next_send_time == 0.)
        {
            // 100 msec in MJD
            next_send_time = 1.16e-6;
        }

        // Time it should be after we wait
        double next_time = current_time + next_send_time;
        // Time it actually is now
        current_time = currentmjd();
        // Sleep if the difference is greater than zero
        if (next_time > current_time)
        {
            sleep_time = 1000000 * 86400. * (next_time - current_time);
//            if (debug_flag)
//            {
//                printf("Send_Loop Sleep: %f seconds\n", sleep_time / 1000000.);
//                fflush(stdout);
//            }
            COSMOS_USLEEP(sleep_time);
        }

        // Bring us up to the present
        current_time = next_time;
        next_send_time = .1 / 86400.;

        for (int32_t node=0; node<txq.size(); ++node)
        {
            // See if we have an active channel serving this Node
            int32_t channel = check_channel(node);
            if (channel < 0)
            {
                continue;
            }

            // Decide what to do next based on our current state
            outgoing_tx_lock.lock();
            switch (txq[node].outgoing.state)
            {
            case PACKET_QUEUE:
                // If we are in Queue state, then the only thing we want to do is send a Queue packet, if enough time has passed
                if (currentmjd() > txq[node].outgoing.nmjd[PACKET_QUEUE - 8])
                {
                    std::vector<PACKET_TX_ID_TYPE> tqueue (TRANSFER_QUEUE_LIMIT, 0);
                    PACKET_TX_ID_TYPE iq = 0;
                    for (uint16_t i=1; i<TRANSFER_QUEUE_SIZE; ++i)
                    {
                        if (txq[node].outgoing.progress[i].tx_id != 0)
                        {
                            tqueue[iq++] = txq[node].outgoing.progress[i].tx_id;
                        }
                        if (iq == TRANSFER_QUEUE_LIMIT)
                        {
                            break;
                        }
                    }
                    make_queue_packet(packet, node, txq[node].node_name, tqueue);
                    queuesendto(node, "tx", packet);
                    txq[node].outgoing.nmjd[PACKET_QUEUE - 8] = currentmjd() + 10. / 86400.;
                }
                break;
            case PACKET_METADATA:
                if (currentmjd() > txq[node].outgoing.nmjd[PACKET_METADATA - 8])
                {
                    int32_t remote_node = lookup_remote_node_id(node);
                    if (remote_node >= 0)
                    {
                        for (uint16_t i=0; i<txq[node].outgoing.meta_id.size(); ++i)
                        {
                            PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node].outgoing.progress, txq[node].outgoing.meta_id[i]);
                            if (tx_id > 0)
                            {
                                tx_progress tx = txq[node].outgoing.progress[tx_id];
                                std::vector<PACKET_BYTE> packet;
                                make_metadata_packet(packet, remote_node, tx.tx_id, (char *)tx.file_name.c_str(), tx.file_size, (char *)tx.agent_name.c_str());
                                send_time = queuesendto(node, "tx", packet);
                                if (send_time >= 0.)
                                {
                                    next_send_time += send_time;
                                }
                                else
                                {
                                    next_send_time = 10. / 86400.;
                                }
                                txq[node].outgoing.state = PACKET_DATA;
                            }
                        }
                    }
                    txq[node].outgoing.nmjd[PACKET_METADATA - 8] = currentmjd() + 10. / 86400.;
                }
                break;
            case PACKET_DATA:
                if (currentmjd() > txq[node].outgoing.nmjd[PACKET_DATA - 8])
                {
                    // See if we have an active transfer
                    PACKET_TX_ID_TYPE  tx_id = check_tx_id(txq[node].outgoing.progress, txq[node].outgoing.id);
                    if (tx_id > 0 && txq[node].outgoing.progress[tx_id].file_info.size())
                    {
                        // Attempt to open the outgoing progress file
                        if (txq[node].outgoing.progress[tx_id].fp == nullptr)
                        {
                            txq[node].outgoing.progress[tx_id].fp = fopen(txq[node].outgoing.progress[tx_id].filepath.c_str(), "r");
                        }

                        // If we're good, continue with the process
                        if(txq[node].outgoing.progress[tx_id].fp != nullptr)
                        {
                            file_progress tp;
                            tp = txq[node].outgoing.progress[tx_id].file_info[0];

                            PACKET_FILE_SIZE_TYPE byte_count = (tp.chunk_end - tp.chunk_start) + 1;
                            if (byte_count > comm_channel[use_channel].packet_size)
                            {
                                byte_count = comm_channel[use_channel].packet_size;
                            }

                            tp.chunk_end = tp.chunk_start + byte_count - 1;

                            // Read the packet and send it
                            int32_t nbytes;
                            PACKET_BYTE* chunk = new PACKET_BYTE[byte_count]();
                            if (!(nbytes = fseek(txq[node].outgoing.progress[tx_id].fp, tp.chunk_start, SEEK_SET)))
                            {
                                nbytes = fread(chunk, 1, byte_count, txq[node].outgoing.progress[tx_id].fp);
                            }
                            if (nbytes == byte_count)
                            {
                                // See if we know what the remote node_id is for this
                                int32_t remote_node = lookup_remote_node_id(node);
                                if (remote_node >= 0)
                                {
                                    make_data_packet(packet, remote_node, txq[node].outgoing.progress[tx_id].tx_id, byte_count, tp.chunk_start, chunk);

                                    send_time = queuesendto(node, "tx", packet);
                                    if (send_time >= 0.)
                                    {
                                        next_send_time += send_time;
                                        txq[node].outgoing.progress[tx_id].file_info[0].chunk_start = tp.chunk_end + 1;
                                    }
                                    else
                                    {
                                        next_send_time = 10. / 86400.;
                                    }
                                }
                            }
                            else
                            {
                                // Some problem with this transmission, ask other end to dequeue it
                                // Remove transaction
                                next_send_time = 0.;
                                txq[node].outgoing.state = PACKET_CANCEL;
                            }
                            delete[] chunk;

                            if (txq[node].outgoing.progress[tx_id].file_info[0].chunk_start > txq[node].outgoing.progress[tx_id].file_info[0].chunk_end)
                            {
                                // All done with this file_info entry. Close file and remove entry.
                                fclose(txq[node].outgoing.progress[tx_id].fp);
                                txq[node].outgoing.progress[tx_id].fp = nullptr;
                                txq[node].outgoing.progress[tx_id].file_info.pop_front();
                            }

                            write_meta(txq[node].outgoing.progress[tx_id]);
                        }
                        else
                        {
                            // Some problem with this transmission, ask other end to dequeue it

                            next_send_time = 0.;
                            txq[node].outgoing.state = PACKET_CANCEL;
                        }
                    }
                    txq[node].outgoing.nmjd[PACKET_DATA - 8] = currentmjd() + next_send_time;
                }
                break;
//            case PACKET_COMPLETE:
//                if (currentmjd() > txq[node].outgoing.nmjd[PACKET_COMPLETE - 8])
//                {
//                    int32_t remote_node = lookup_remote_node_id(node);
//                    if (remote_node >= 0)
//                    {
//                        // Send a COMPLETE packet
//                        std::vector<PACKET_BYTE> packet;
//                        make_complete_packet(packet, remote_node, txq[node].outgoing.id);
//                        queuesendto(node, "tx", packet);
//                    }
//                    txq[node].outgoing.nmjd[PACKET_COMPLETE - 8] = currentmjd() + 10./86400.;
//                }
//                break;
            case PACKET_CANCEL:
                if (currentmjd() > txq[node].outgoing.nmjd[PACKET_CANCEL - 8])
                {
                    // See if we have an active transfer
                    PACKET_TX_ID_TYPE  tx_id = check_tx_id(txq[node].outgoing.progress, txq[node].outgoing.id);
                    outgoing_tx_del(node, tx_id);
                    int32_t remote_node = lookup_remote_node_id(node);
                    if (remote_node >= 0)
                    {
                        // Send a CANCEL packet
                        std::vector<PACKET_BYTE> packet;
                        make_cancel_packet(packet, remote_node, tx_id);
                        queuesendto(node, "tx", packet);
                    }
                    txq[node].outgoing.nmjd[PACKET_CANCEL - 8] = currentmjd() + 10. / 86400.;
                    txq[node].outgoing.state = PACKET_QUEUE;
                }
                break;
            }

            outgoing_tx_lock.unlock();
        }
    }
}

void transmit_loop()
{
    std::mutex transmit_queue_lock;
    std::unique_lock<std::mutex> locker(transmit_queue_lock);

    while (agent->running())
    {
        if (agent->running() == (uint16_t)Agent::State::IDLE)
        {
            COSMOS_SLEEP(1);
            continue;
        }


        transmit_queue_check.wait(locker);

        while (!transmit_queue.empty())
        {
            // Get next packet from transceiver FIFO
            transmit_queue_entry entry = transmit_queue.front();
            transmit_queue.pop();
            mysendto(entry.type, comm_channel[entry.channel], entry.packet);
        }
    }
}

double queuesendto(PACKET_NODE_ID_TYPE node_id, string type, std::vector<PACKET_BYTE> packet)
{
    transmit_queue_entry tentry;

    use_channel = -1;
    for (uint16_t i=0; i<comm_channel.size(); ++i)
    {
        if (txq[node_id].node_name == comm_channel[i].node)
        {
            use_channel = i;
            break;
        }
    }

    if (use_channel > comm_channel.size())
    {
        return -1.;
    }

    tentry.type = type;
    tentry.channel = use_channel;
    tentry.packet = packet;
    transmit_queue.push(tentry);
    transmit_queue_check.notify_one();
    double time_step = packet.size() / (86400. * comm_channel[use_channel].throughput);
    if (time_step > 0)
    {
        return time_step;
    }
    else
    {
        return 0.;
    }
}

int32_t mysendto(std::string type, channelstruc& channel, std::vector<PACKET_BYTE>& buf)
{
    int32_t iretn;
    double cmjd;

    if ((cmjd = currentmjd(0.)) < channel.nmjd)
    {
        if (debug_flag)
        {
            printf("Mysendto Sleep: %f seconds\n", 86400. * (channel.nmjd - cmjd));
            fflush(stdout);
        }
        COSMOS_USLEEP((uint32_t)(86400000000. * (channel.nmjd - cmjd)));
    }

    iretn = sendto(channel.chansock.cudp, reinterpret_cast<const char*>(&buf[0]), buf.size(), 0, reinterpret_cast<sockaddr*>(&channel.chansock.caddr), sizeof(struct sockaddr_in));

    if (iretn >= 0)
    {
        channel.nmjd = currentmjd() + ((28+iretn) / (float)channel.throughput)/86400.;
        debug_packet(buf, type+" out");
    }
    else
    {
        iretn = -errno;
    }

    return iretn;
}

int32_t myrecvfrom(std::string type, socket_channel &channel, std::vector<PACKET_BYTE>& buf, uint32_t length, double dtimeout)
{
    int32_t nbytes = 0;

    buf.resize(length);
    ElapsedTime et;
    do
    {
        fd_set set;
        FD_ZERO(&set);
        int fdmax = -1;
        for (uint16_t i=0; i<comm_channel.size(); ++i)
        {
            FD_SET(comm_channel[i].chansock.cudp, &set);
            if (comm_channel[i].chansock.cudp > fdmax)
            {
                fdmax = comm_channel[i].chansock.cudp;
            }
        }
        timeval timeout;
        double rtimeout = dtimeout - et.split();
        if (rtimeout >= 0.)
        {
            timeout.tv_sec = static_cast<int32_t>(rtimeout);
            timeout.tv_usec = static_cast<int32_t>(1000000. * (rtimeout - timeout.tv_sec));
            int rv = select(fdmax+1, &set, nullptr, nullptr, &timeout);
            if (rv == -1)
            {
                nbytes = -errno;
            }
            else if (rv == 0)
            {
                nbytes = GENERAL_ERROR_TIMEOUT;
            }
            else
            {
                for (uint16_t i=0; i<comm_channel.size(); ++i)
                {
                    if (FD_ISSET(comm_channel[i].chansock.cudp, &set))
                    {
                        channel = comm_channel[i].chansock;
                        nbytes = recvfrom(channel.cudp, reinterpret_cast<char *>(&buf[0]), length, 0, reinterpret_cast<sockaddr*>(&channel.caddr), reinterpret_cast<socklen_t *>(&channel.addrlen));
                        if (nbytes > 0)
                        {
                            buf.resize(nbytes);
                            debug_packet(buf, type+" in");
                            return nbytes;
                        }
                        else
                        {
                            if (nbytes < 0)
                            {
                                nbytes = -errno;
                            }
                            else
                            {
                                nbytes = GENERAL_ERROR_INPUT;
                            }
                        }

                    }
                }
            }
        }
    } while (et.split() < dtimeout);

//    if (( nbytes = recvfrom(channel.cudp, reinterpret_cast<char *>(&buf[0]), length, 0, reinterpret_cast<sockaddr*>(&channel.caddr), reinterpret_cast<socklen_t *>(&channel.addrlen))) > 0)
//    {
//        buf.resize(nbytes);
//        debug_packet(buf, type+" in");
//    }
//    else
//    {
//        nbytes = -errno;
//    }
    return nbytes;
}

void debug_packet(std::vector<PACKET_BYTE> buf, std::string type)
{
    if (debug_flag)
    {
        printf("[%.15g %s (%" PRIu32 ")] ", currentmjd(), type.c_str(), buf.size());
        switch (buf[0] & 0x0f)
        {
        case PACKET_METADATA:
            {
                std::string file_name(&buf[PACKET_METASHORT_FILE_NAME], &buf[PACKET_METASHORT_FILE_NAME+TRANSFER_MAX_FILENAME]);
                printf("[METADATA] %u %u %s ", buf[PACKET_METASHORT_NODE_ID], buf[PACKET_METASHORT_TX_ID], file_name.c_str());
                break;
            }
        case PACKET_DATA:
            {
                printf("[DATA] %u %u %u %u ", buf[PACKET_DATA_NODE_ID], buf[PACKET_DATA_TX_ID], buf[PACKET_DATA_CHUNK_START]+256U*(buf[PACKET_DATA_CHUNK_START+1]+256U*(buf[PACKET_DATA_CHUNK_START+2]+256U*buf[PACKET_DATA_CHUNK_START+3])), buf[PACKET_DATA_BYTE_COUNT]+256U*buf[PACKET_DATA_BYTE_COUNT+1]);
                break;
            }
        case PACKET_REQDATA:
            {
                printf("[REQDATA] %u %u %u %u ", buf[PACKET_REQDATA_NODE_ID], buf[PACKET_REQDATA_TX_ID], buf[PACKET_REQDATA_HOLE_START]+256U*(buf[PACKET_REQDATA_HOLE_START+1]+256U*(buf[PACKET_REQDATA_HOLE_START+2]+256U*buf[PACKET_REQDATA_HOLE_START+3])), buf[PACKET_REQDATA_HOLE_END]+256U*(buf[PACKET_REQDATA_HOLE_END+1]+256U*(buf[PACKET_REQDATA_HOLE_END+2]+256U*buf[PACKET_REQDATA_HOLE_END+3])));
                break;
            }
        case PACKET_REQMETA:
            {
                printf("[REQMETA] %u %s ", buf[PACKET_REQMETA_NODE_ID], &buf[PACKET_REQMETA_NODE_NAME]);
                for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                    if (buf[PACKET_REQMETA_TX_ID+i])
                    {
                        printf("%u ", buf[PACKET_REQMETA_TX_ID+i]);
                    }
                break;
            }
        case PACKET_COMPLETE:
            {
                printf("[COMPLETE] %u %u ", buf[PACKET_COMPLETE_NODE_ID], buf[PACKET_COMPLETE_TX_ID]);
                break;
            }
        case PACKET_CANCEL:
            {
                printf("[CANCEL] %u %u ", buf[PACKET_CANCEL_NODE_ID], buf[PACKET_CANCEL_TX_ID]);
                break;
            }
        case PACKET_QUEUE:
            {
                printf("[QUEUE] %u %s ", buf[PACKET_QUEUE_NODE_ID], &buf[PACKET_QUEUE_NODE_NAME]);
                for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                    if (buf[PACKET_QUEUE_TX_ID+i])
                    {
                        printf("%u ", buf[PACKET_QUEUE_TX_ID+i]);
                    }
            }
        }
        printf("\n");
        fflush(stdout);
    }
}

int32_t write_meta(tx_progress& tx)
{
    std::vector<PACKET_BYTE> packet;
    std::ofstream file_name;

    if (currentmjd(0.) - tx.savetime > 5./86400.)
    {
        //		std::cout<<"write_meta: "<< 86400.*(currentmjd(0.) - tx.savetime)<<" file: "<<tx.temppath + ".meta"<< std::endl;
        tx.savetime = currentmjd(0.);
        make_metadata_packet(packet, tx.tx_id, (char *)tx.file_name.c_str(), tx.file_size, (char *)tx.node_name.c_str(), (char *)tx.agent_name.c_str());
        file_name.open(tx.temppath + ".meta", std::ios::out|std::ios::binary);
        if(!file_name.is_open())
        {
            return (-errno);
        }

        uint16_t crc;
        file_name.write((char *)&packet[0], PACKET_METALONG_SIZE);
        crc = slip_calc_crc((uint8_t *)&packet[0], PACKET_METALONG_SIZE);
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
    std::vector<PACKET_BYTE> packet(PACKET_METALONG_SIZE,0);
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


    // load metadata
    tx.havemeta = true;

    file_name.read((char *)&packet[0], PACKET_METALONG_SIZE);
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
    if (crc != slip_calc_crc((uint8_t *)&packet[0], PACKET_METALONG_SIZE))
    {
        file_name.close();
        return DATA_ERROR_CRC;
    }
    extract_metadata(packet, meta);
    tx.tx_id = meta.tx_id;
    tx.node_name = meta.node_name;
    tx.agent_name = meta.agent_name;
    tx.file_name = meta.file_name;
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
    if (debug_flag)
    {
        printf("read_meta: %s tx_id: %u chunks: %" PRIu32 "\n", (tx.temppath + ".meta").c_str(), tx.tx_id, tx.file_info.size());
    }

    // fix any overlaps and count total bytes
    merge_chunks_overlap(tx);

    // calculate bytes so far
    //	tx.total_bytes = 0;
    //	for (file_progress prog : tx.file_info)
    //	{
    //		tx.total_bytes += (prog.chunk_end - prog.chunk_start) + 1;
    //	}
    return 0;
}

bool lower_chunk(file_progress i,file_progress j)
{
    return (i.chunk_start<j.chunk_start);
}

PACKET_FILE_SIZE_TYPE merge_chunks_overlap(tx_progress& tx)
{
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

std::vector<file_progress> find_chunks_missing(tx_progress& tx)
{
    std::vector<file_progress> missing;
    file_progress tp;

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

    return (missing);
}

int32_t request_ls(char* request, char* response, Agent *agent)
{

    //the request string == "ls directoryname"
    //get the directory name
//    char directoryname[COSMOS_MAX_NAME+1];
//    memmove(directoryname, request+3, COSMOS_MAX_NAME);
    std::string directoryname = request+3;

    DIR* dir;
    struct dirent* ent;

    std::string all_file_names;

    if((dir = opendir(directoryname.c_str())) != NULL)
    {
        while (( ent = readdir(dir)) != NULL)
        {
            all_file_names += ent->d_name;
            all_file_names += "\n";
        }
        closedir(dir);

        sprintf(response, "%s", all_file_names.c_str());
    }
    else
        sprintf(response, "unable to open directory <%s>", directoryname.c_str());
    return 0;
}

int32_t request_list_incoming(char* request, char* response, Agent *agent)
{
    response[0] = 0;
    for (uint16_t node = 0; node<txq.size(); ++node)
    {
        sprintf(&response[strlen(response)], "%u %s %u\n", node, txq[node].node_name.c_str(), txq[node].incoming.size);
        for(tx_progress tx : txq[node].incoming.progress)
        {
            if (tx.tx_id)
            {
                sprintf(&response[strlen(response)], "tx_id: %u node: %s agent: %s name: %s bytes: %u/%u\n", tx.tx_id, tx.node_name.c_str(), tx.agent_name.c_str(), tx.file_name.c_str(), tx.total_bytes, tx.file_size);
            }
        }
    }

    return 0;
}

int32_t request_list_outgoing(char* request, char* response, Agent *agent)
{
    response[0] = 0;
    for (uint16_t node=0; node<txq.size(); ++node)
    {
        sprintf(&response[strlen(response)], "%u %s %u\n", node, txq[node].node_name.c_str(), txq[node].outgoing.size);
        for(tx_progress tx : txq[node].outgoing.progress)
        {
            if (tx.tx_id)
            {
                sprintf(&response[strlen(response)], "tx_id: %u node: %s agent: %s name: %s bytes: %u/%u\n", tx.tx_id, tx.node_name.c_str(), tx.agent_name.c_str(), tx.file_name.c_str(), tx.total_bytes, tx.file_size);
            }
        }
    }

    return 0;
}

int32_t request_use_channel(char* request, char* response, Agent *agent)
{
    uint16_t channel=0;
    uint32_t throughput=0;

    sscanf(request, "%*s %hu %u\n", &channel, &throughput);
    if (channel < comm_channel.size())
    {
        use_channel = channel;
        if (throughput)
        {
            comm_channel[channel].throughput = throughput;
        }
    }
    else
    {
        sprintf(response, "Channel %u too large", channel);
    }
    return 0;

}

int32_t request_remove_file(char* request, char* response, Agent *agent)
{
    char type;
    uint32_t tx_id;

    sscanf(request, "%*s %c %u\n", &type, &tx_id);
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

//int32_t request_send_file(char* request, char* response, Agent *agent)
//{

//	//the request string == "send_file agent_name file_name packet_size"

//	//get the agent, file name and packet_size
//	char file_name[COSMOS_MAX_NAME];
//	char agent_name[COSMOS_MAX_NAME];
//	char node_name[COSMOS_MAX_NAME];
//	uint16_t channel=0;

//	sscanf(request+10, "%40s %40s %40s %hu", node_name, agent_name, file_name, &channel);

//	std::string node = node_name;
//	std::string agent = agent_name;
//	std::string file = file_name;
//	if (outgoing_tx_add(node, node_name, agent, file) < 0)
//	{
//		sprintf(response, "Could not queue %s %s %s %u", node_name, agent_name, file_name, channel);

//	}
//	else
//	{
//		sprintf(response, "Queued %s %s %s %u", node_name, agent_name, file_name, channel);
//	}

//	return 0;
//}

int32_t outgoing_tx_add(tx_progress tx_out)
{
    int32_t node = check_node_id(tx_out.node_name);
    if (node <0)
    {
        return TRANSFER_ERROR_NODE;
    }

    // Only add if we have room
    if (txq[node].outgoing.size == TRANSFER_QUEUE_LIMIT)
    {
        return TRANSFER_ERROR_QUEUEFULL;
    }

    tx_out.fp = nullptr;
    tx_out.total_bytes = 0;
    tx_out.filepath = data_base_path(tx_out.node_name, "outgoing", tx_out.agent_name, tx_out.file_name);
    //get the file size
    tx_out.file_size = get_file_size(tx_out.filepath);
    tx_out.temppath = data_base_path(tx_out.node_name, "temp", "file", "out_"+std::to_string(tx_out.tx_id));
    tx_out.savetime = 0.;

    // save and queue metadata packet
    //	tx_out.sendcomplete = false;
    //	tx_out.reqmeta = false;
    tx_out.havemeta = true;

    // Good to go. Add it to queue.
    outgoing_tx_lock.lock();
    txq[node].outgoing.progress[tx_out.tx_id] = tx_out;
    ++txq[node].outgoing.size;
    outgoing_tx_lock.unlock();

    if (debug_flag)
    {
        printf("Add outgoing: %u %s %s %s\n", tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str());
    }

    return 0;
}

int32_t outgoing_tx_add(std::string node_name, std::string agent_name, std::string file_name)
{
    // BEGIN GATHERING THE METADATA
    tx_progress tx_out;

    int32_t node = check_node_id(node_name);
    if (node <0)
    {
        return TRANSFER_ERROR_NODE;
    }

    // Only add if we have room
    if (txq[node].outgoing.size == TRANSFER_QUEUE_LIMIT)
    {
        return TRANSFER_ERROR_QUEUEFULL;
    }

    // Locate next empty space
    //get the file size
    outgoing_tx_lock.lock();
    tx_out.tx_id = 0;
    PACKET_TX_ID_TYPE id = txq[node].outgoing.next_id;
    do
    {
        // 0 is special case
        if (id == 0)
        {
            ++id;
        }

        if (txq[node].outgoing.progress[id].tx_id == 0)
        {
            tx_out.tx_id = id;
            txq[node].outgoing.next_id = id + 1;
            break;
        }
        // If no empty found, increment, allowing to wrap if necessary
    } while (++id != txq[node].outgoing.next_id);
    outgoing_tx_lock.unlock();

    if (tx_out.tx_id > 0)
    {
        tx_out.node_name = node_name;
        tx_out.agent_name = agent_name;
        tx_out.file_name = file_name;
        tx_out.temppath = data_base_path(node_name, "temp", "file", "out_"+std::to_string(tx_out.tx_id));

        std::ifstream filename;

        // set the file path
        std::string 	filepath = data_base_path(tx_out.node_name, "outgoing", tx_out.agent_name, tx_out.file_name);

        //get the file size
        tx_out.file_size = get_file_size(filepath);

        if(tx_out.file_size < 0)
        {
            return DATA_ERROR_SIZE_MISMATCH;
        }

        // see if file can be opened
        filename.open(filepath, std::ios::in|std::ios::binary);
        if(!filename.is_open())
        {
            return -errno;
        }
        filename.close();

        write_meta(tx_out);

        int32_t iretn = outgoing_tx_add(tx_out);
        return iretn;
    }
    else
    {
        return TRANSFER_ERROR_MATCH;
    }
}

int32_t outgoing_tx_del(int32_t node, PACKET_TX_ID_TYPE tx_id)
{
    if (node <0 || (uint32_t)node > txq.size())
    {
        return TRANSFER_ERROR_INDEX;
    }

    if (txq[node].outgoing.progress[tx_id].tx_id == 0)
    {
        return TRANSFER_ERROR_MATCH;
    }

    tx_progress tx_out = txq[node].outgoing.progress[tx_id];

    // erase the transaction
    //	outgoing_tx.erase(outgoing_tx.begin()+tx_id);
    txq[node].outgoing.progress[tx_id].tx_id = 0;
    --txq[node].outgoing.size;

    // Set current tx id back to 0
    txq[node].outgoing.id = 0;

    // Remove the file
    if(remove(tx_out.filepath.c_str()))
    {
        unable_to_remove(tx_out.filepath);
    }

    // Remove the META file
    std::string meta_filepath = tx_out.temppath + ".meta";
    remove(meta_filepath.c_str());

    if (debug_flag)
    {
        printf("Del outgoing: %u %s %s %s\n", tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str());
    }

    return 0;
}

int32_t incoming_tx_add(tx_progress tx_in)
{
    int32_t node = check_node_id(tx_in.node_name);
    if (node <0)
    {
        return TRANSFER_ERROR_NODE;
    }

    if (tx_in.file_name.size())
    {
        tx_in.filepath = data_base_path(tx_in.node_name, "incoming", tx_in.agent_name, tx_in.file_name);
    }
    else
    {
        tx_in.filepath = "";
    }
    std::string tx_name = "in_"+std::to_string(tx_in.tx_id);
    tx_in.temppath = data_base_path(tx_in.node_name, "temp", "file", tx_name);
    tx_in.savetime = 0.;
    tx_in.fp = nullptr;

    // Put it in list
    txq[node].incoming.progress[tx_in.tx_id] = tx_in;
    ++txq[node].incoming.size;

    if (debug_flag)
    {
        printf("Add incoming: %u %s\n", tx_in.tx_id, tx_in.node_name.c_str());
    }

    return 0;
}

int32_t incoming_tx_add(std::string node_name, PACKET_TX_ID_TYPE tx_id)
{
    tx_progress tx_in;

    tx_in.tx_id = tx_id;
    tx_in.node_name = node_name;
    tx_in.file_name = "";
    tx_in.agent_name = "";
    tx_in.havemeta = false;
    tx_in.file_size = 0;
    tx_in.total_bytes = 0;
    tx_in.complete = false;

    int32_t iretn = incoming_tx_add(tx_in);

    return iretn;
}

int32_t incoming_tx_update(packet_struct_metashort meta)
{
    int32_t node = check_node_id(meta.node_id);
    if (node <0)
    {
        return TRANSFER_ERROR_NODE;
    }

    // See if it's already in the queue
    if (txq[node].incoming.progress[meta.tx_id].tx_id != meta.tx_id)
    {
        return TRANSFER_ERROR_MATCH;
    }

    if (!txq[node].incoming.progress[meta.tx_id].havemeta)
    {
        // Core META information
        txq[node].incoming.progress[meta.tx_id].node_name = txq[node].node_name;
        txq[node].incoming.progress[meta.tx_id].agent_name = meta.agent_name;
        txq[node].incoming.progress[meta.tx_id].file_name = meta.file_name;
        txq[node].incoming.progress[meta.tx_id].file_size = meta.file_size;
        txq[node].incoming.progress[meta.tx_id].filepath = data_base_path(txq[node].incoming.progress[meta.tx_id].node_name, "incoming", txq[node].incoming.progress[meta.tx_id].agent_name, txq[node].incoming.progress[meta.tx_id].file_name);
        std::string tx_name = "in_"+std::to_string(txq[node].incoming.progress[meta.tx_id].tx_id);
        txq[node].incoming.progress[meta.tx_id].temppath = data_base_path(txq[node].incoming.progress[meta.tx_id].node_name, "temp", "file", tx_name);

        // Derivative META information
        txq[node].incoming.progress[meta.tx_id].savetime = 0.;
        txq[node].incoming.progress[meta.tx_id].havemeta = true;
        txq[node].incoming.progress[meta.tx_id].total_bytes = 0;
        txq[node].incoming.progress[meta.tx_id].fp = nullptr;

        // Save it to disk
        write_meta(txq[node].incoming.progress[meta.tx_id]);
    }

    if (debug_flag)
    {
        printf("Update incoming: %u %s %s %s\n", txq[node].incoming.progress[meta.tx_id].tx_id, txq[node].incoming.progress[meta.tx_id].node_name.c_str(), txq[node].incoming.progress[meta.tx_id].agent_name.c_str(), txq[node].incoming.progress[meta.tx_id].file_name.c_str());
    }

    return meta.tx_id;
}

int32_t incoming_tx_del(int32_t node, PACKET_TX_ID_TYPE tx_id)
{
    node = check_node_id(node);
    if (node <0)
    {
        return TRANSFER_ERROR_NODE;
    }

    if (txq[node].incoming.progress[tx_id].tx_id == 0)
    {
        return TRANSFER_ERROR_MATCH;
    }

    tx_progress tx_in = txq[node].incoming.progress[tx_id];

    txq[node].incoming.progress[tx_id].tx_id = 0;
    txq[node].incoming.progress[tx_id].havemeta = false;
    --txq[node].incoming.size;

    // Close the DATA file
    if (tx_in.fp != nullptr)
    {
        fclose(tx_in.fp);
        tx_in.fp = nullptr;
    }

    std::string filepath;
    //Remove the DATA file
    filepath = tx_in.temppath + ".file";
    remove(filepath.c_str());

    // Remove the META file
    filepath = tx_in.temppath + ".meta";
    remove(filepath.c_str());

    // Make sure we are not using this for incoming_tx_id
    if (tx_in.tx_id == txq[node].incoming.id)
    {
        txq[node].incoming.id = 0;
    }

    if (debug_flag)
    {
        printf("Del incoming: %u %s\n", tx_in.tx_id, tx_in.node_name.c_str());
    }

    return 0;

}

bool filestruc_compare_by_size(const filestruc& a, const filestruc& b)
{
    return a.size < b.size;
}

bool tx_progress_compare_by_size(const tx_progress& a, const tx_progress& b)
{
    return a.file_size < b.file_size;
}

PACKET_TX_ID_TYPE choose_incoming_tx_id(int32_t node)
{
    PACKET_TX_ID_TYPE tx_id = 0;

    if (node >= 0 && (uint32_t)node < txq.size())
    {
        // Choose file with least data left to send
        PACKET_FILE_SIZE_TYPE nsize = INT32_MAX;
        for (PACKET_FILE_SIZE_TYPE i=0; i < txq[node].incoming.progress.size(); ++i)
        {
            // calculate bytes so far
            merge_chunks_overlap(txq[node].incoming.progress[i]);
            //			txq[node].incoming.progress[i].total_bytes = 0;
            //			for (file_progress prog : txq[node].incoming.progress[i].file_info)
            //			{
            //				txq[node].incoming.progress[i].total_bytes += (prog.chunk_end - prog.chunk_start) + 1;
            //			}

            // Choose transactions for which we: have meta and bytes remaining is minimized
            if (txq[node].incoming.progress[i].tx_id && txq[node].incoming.progress[i].havemeta && (txq[node].incoming.progress[i].file_size - txq[node].incoming.progress[i].total_bytes) < nsize)
            {
                nsize = txq[node].incoming.progress[i].file_size - txq[node].incoming.progress[i].total_bytes;
                tx_id = txq[node].incoming.progress[i].tx_id;
            }
        }
    }

    return tx_id;
}

PACKET_TX_ID_TYPE check_tx_id(std::vector<tx_progress> tx_entry, PACKET_TX_ID_TYPE tx_id)
{
    if (tx_id != 0 && tx_entry[tx_id].tx_id == tx_id)
    {
        return tx_id;
    }
    else
    {
        return 0;
    }
}

int32_t check_node_id(std::string node_name)
{
    int32_t id = -1;
    for (uint16_t i=0; i<txq.size(); ++i)
    {
        if (txq[i].node_name == node_name)
        {
            id = i;
            break;
        }
    }
    return id;
}

int32_t check_node_id(PACKET_NODE_ID_TYPE node_id)
{
    int32_t id = -1;
    if (node_id >= 0 && node_id < txq.size())
    {
        id = node_id;
    }
    return id;
}

int32_t check_channel(PACKET_NODE_ID_TYPE node_id)
{
    for(uint16_t i=0; i<comm_channel.size(); ++i)
    {
        if (comm_channel[i].node == txq[node_id].node_name)
        {
            return i;
        }
    }
    return -1;
}

int32_t lookup_remote_node_id(PACKET_NODE_ID_TYPE node_id)
{
    int32_t id = -1;
    if (node_id >=0 && node_id < txq.size())
    {
        if (txq[node_id].node_id > 0)
        {
            id = txq[node_id].node_id - 1;
        }
    }
    return id;
}

int32_t set_remote_node_id(PACKET_NODE_ID_TYPE node_id, std::string node_name)
{
    int32_t id = -1;
    for (uint16_t i=0; i<txq.size(); ++i)
    {
        if (txq[i].node_name == node_name)
        {
            txq[i].node_id = node_id+1;
            id = i;
        }
    }
    return id;
}

int32_t next_incoming_tx(PACKET_NODE_ID_TYPE node)
{
    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node].incoming.progress, choose_incoming_tx_id(node));

    if (tx_id < TRANSFER_QUEUE_SIZE && tx_id > 0)
    {
        // See if we know what the remote node_id is for this
        int32_t remote_node = lookup_remote_node_id(node);
        if (remote_node >= 0)
        {
            // Check if file has been completely received
            if(txq[node].incoming.progress[tx_id].file_size == txq[node].incoming.progress[tx_id].total_bytes && txq[node].incoming.progress[tx_id].havemeta)
            {
                tx_progress tx_in = txq[node].incoming.progress[tx_id];

                // inform other end that file has been received
                std::vector<PACKET_BYTE> packet;
                make_complete_packet(packet, remote_node, tx_in.tx_id);
                queuesendto(node, "rx", packet);

                // Move file to its final location
                if (!txq[node].incoming.progress[tx_id].complete)
                {
                    if (txq[node].incoming.progress[tx_id].fp != nullptr)
                    {
                        fclose(txq[node].incoming.progress[tx_id].fp);
                        txq[node].incoming.progress[tx_id].fp = nullptr;
                    }
                    std::string final_filepath = tx_in.temppath + ".file";
                    rename(final_filepath.c_str(), tx_in.filepath.c_str());
                    txq[node].incoming.progress[tx_id].complete = true;
                }
            }
            else
            {
                // Ask for missing data
                std::vector<file_progress> missing;
                missing = find_chunks_missing(txq[node].incoming.progress[tx_id]);
                for (uint32_t j=0; j<missing.size(); ++j)
                {
                    std::vector<PACKET_BYTE> packet;
                    make_reqdata_packet(packet, remote_node, txq[node].incoming.progress[tx_id].tx_id, missing[j].chunk_start, missing[j].chunk_end);
                    queuesendto(node, "rx", packet);
                }
            }
        }
    }
    return tx_id;
}

int32_t request_debug(char *request, char *response, Agent *agent)
{

    std::string requestString = std::string(request);
    StringParser sp(requestString, ' ');

    debug_flag = sp.getFieldNumberAsDouble(2); // should be getFieldNumberAsBoolean

    std::cout << "debug_flag: " << debug_flag << std::endl;
    return 0;
}
