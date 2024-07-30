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
//! Utilizes the COSMOS agent discovery mechanism to discover another file agent
//! to perform file transfers with.
//!
//! Usage: agent_file
#include <sstream>
#include "agent/agentclass.h"
#include "module/file_module.h"
#include "module/websocket_module.h"
#include "support/packethandler.h"


Agent* agent;
beatstruc remote_agent;
// Reusable packet object
PacketComm packet;
PacketHandler packethandler;
// Avoid joining threads that haven't been started or have already been joined
bool threads_started = false;
// Reserved auto node ids
const uint8_t NODEIDAUTO1 = 252;
const uint8_t NODEIDAUTO2 = 253;
uint8_t self_node_id = 0;
uint8_t remote_node_id = 0;
// Timer object for checking remote agent activity
ElapsedTime remote_agent_activity_timer;
// Frequency of checking remote agent activity
double remote_agent_activity_check_interval = 20.;
// Timeout for remote agent activity
double remote_agent_activity_timeout = 60. / 86400.;


// Threads and modules
thread file_thread;
Cosmos::Module::FileModule* file_module;
thread websocket_thread;
Cosmos::Module::WebsocketModule* websocket_module;
const uint16_t FILETRANSFERPORT = 10076;

void init_agent(int argc, char *argv[]);
int32_t generate_node_ids();
string get_self_address(string remote_addr);
uint32_t ip_to_int(const std::string& ip);
void stop_subagents();

void Loop();

bool find_file_agent();
int32_t start_subagents(Agent *agent);
void file_transfer_loop();
void handle_incoming_packets();
bool check_remote_agent_activity();
void reset();

int main(int argc, char *argv[])
{
    ////////////////////////////////////
    // Initialization
    ////////////////////////////////////
    init_agent(argc, argv);


    ////////////////////////////////////
    // Main Loop
    ////////////////////////////////////
    Loop();


    ////////////////////////////////////
    // Cleanup
    ////////////////////////////////////
    int32_t iretn = agent->wait(Agent::State::SHUTDOWN);
    if (iretn < 0)
    {
        printf("Error in agent->wait() %d\n", iretn);
        exit(iretn);
    }
    stop_subagents();
    agent->shutdown();

    return 0;
}

////////////////////////////////////
// Main Loop
////////////////////////////////////
void Loop()
{
    // Start performing the body of the agent
    while(agent->running())
    {
        cout << "Searching for remote file agent..." << endl;
        // Find remote file agent
        while (!find_file_agent())
        {
            secondsleep(4.);
        }

        // Start comm and file subagents
        start_subagents(agent);

        // Perform file transfers while the remote agent is active
        file_transfer_loop();

        // Clean up anything that needs to be reset and return to finding the remote file agent
        reset();

        std::this_thread::yield();
    }
}

bool find_file_agent()
{
    // Utilize agent discovery mechanism to determine information of a remote file agent
    remote_agent = agent->find_agent("any", "file");
    if (!remote_agent.exists)
    {
        return false;
    }
    if (currentmjd() - remote_agent.utc > remote_agent_activity_timeout)
    {
        return false;
    }
    cout << "Found file agent on node: " << remote_agent.node << " @ " << remote_agent.addr << endl;
    return true;
}

void file_transfer_loop()
{
    remote_agent_activity_timer.set(remote_agent_activity_check_interval);
    // Continue file transfer while remote file agent appears to be active
    while(check_remote_agent_activity() && agent->running())
    {
        // Route incoming packets for file transfers
        handle_incoming_packets();
        std::this_thread::yield();
    }
    cout << "Remote file agent is inactive. Stopping file transfer." << endl;
}

void reset()
{
    // Stop the subagent threads
    stop_subagents();

    // Reset the node id list
    // Always remove self entry, since agent file will not persist in the node id list
    remove_node_id(agent->cinfo, agent->cinfo->node.name);
    // Remove the entry for the remote agent if it was auto-generated within this program
    if (remote_node_id == NODEIDAUTO1 || remote_node_id == NODEIDAUTO2)
    {
        remove_node_id(agent->cinfo, remote_agent.node);
    }
}

bool check_remote_agent_activity()
{
    if (remote_agent_activity_timer.timer() > 0.)
    {
        return true;
    }
    remote_agent_activity_timer.set(remote_agent_activity_check_interval);
    remote_agent = agent->find_agent("any", "file");
    if (!remote_agent.exists)
    {
        return false;
    }
    if (currentmjd() - remote_agent.utc > remote_agent_activity_timeout)
    {
        return false;
    }
    // TODO: Move this somewhere better
    agent->channel_touch(agent->channel_number("COMM"));
    return true;
}

void stop_subagents()
{
    if (!threads_started)
    {
        return;
    }

    // Stop the file subagent
    file_module->shutdown();
    file_thread.join();

    // Stop the websocket subagent
    websocket_module->shutdown();
    websocket_thread.join();

    threads_started = false;
}

void handle_incoming_packets()
{
    // Comm - Internal
    // Handle packets in the main-thread queue (0)
    while (agent->channel_pull(0, packet) > 0)
    {
        agent->monitor_unwrapped(0, packet, "Pull");
        // Handle if the packet destination is this node
        if (packet.header.nodedest == self_node_id)
        {
            string response;
            packethandler.process(packet, response);
            // Send back a response if a response was created when handling the packet
            if (response.size())
            {
                if (packet.header.chanin != 0)
                {
                    agent->push_response(packet.header.chanin, 0, packet.header.nodeorig, 0, response);
                    agent->monitor_unwrapped(packet.header.chanin, packet, "Respond");
                }
            }
        }
    }
}

////////////////////////////////////
// Initialization functions
////////////////////////////////////
void init_agent(int argc, char *argv[])
{
    uint16_t debug_level = 2;

    // The choice of realm name here is arbitrary, but it must not be blank, and/or should
    // be a realm with a nodeids.ini file such that this agent's nodename is NOT listed in it.
    agent = new Support::Agent("File", "", "file", 0., 10000, false, 0, NetworkType::UDP, debug_level);

    // Check if agent was successfully started
    int32_t iretn = 0;
    if ((iretn = agent->wait()) < 0) {
        agent->debug_log.Printf("%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    } else {
        agent->debug_log.Printf("%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
    }

    // Set channels
    agent->channel_add("COMM", Support::Channel::PACKETCOMM_DATA_SIZE, Support::Channel::PACKETCOMM_PACKETIZED_SIZE, 18000., 1000);

    // Initialize the packethandler, which helps handle and route packets
    packethandler.init(agent);

    agent->cinfo->agent0.aprd = 1;
    agent->start_active_loop();
}

int32_t generate_node_ids()
{
    // If a valid entry for the remote agent is in the node list (i.e., node id is not a reserved value),
    // then use the existing entry for the remote agent, and use an auto-generated one for self.
    bool use_existing_remote_node_id = false;
    int32_t iretn = lookup_node_id(agent->cinfo, remote_agent.node);
    uint8_t existing_remote_node_id = iretn;
    if (iretn >=0 && existing_remote_node_id > NODEIDUNKNOWN && existing_remote_node_id < NODEIDAUTO1)
    {
        use_existing_remote_node_id = true;
    }

    // If no entry for the remote agent was in the node id list, then a new
    // node id must be generated for both the remote agent and self.
    // Compare IP addresses, and generate node ids based on who is lower and higher,
    // where the node with the lower IP address number is assigned a node ID of 252 (NODEIDAUTO1)
    // and the node with the higher IP address number is assigned a node ID of 253 (NODEIDAUTO2).
    string self_addr = get_self_address(remote_agent.addr);
    if (ip_to_int(remote_agent.addr) < ip_to_int(self_addr))
    {
        remote_node_id = NODEIDAUTO1;
        self_node_id = NODEIDAUTO2;
    }
    else if (ip_to_int(remote_agent.addr) > ip_to_int(self_addr))
    {
        remote_node_id = NODEIDAUTO2;
        self_node_id = NODEIDAUTO1;
    }
    else
    {
        cerr << "Error: IP addresses are the same (" << self_addr << "). Not yet handled" << endl;
        exit(0);
    }
    if (use_existing_remote_node_id)
    {
        remote_node_id = existing_remote_node_id;
    }
    // Since the agentclass constructor (can) automatically assigns a node_id, we need to change it to the generated id.
    if (change_node_id(agent->cinfo, agent->cinfo->node.name, self_node_id) < 0)
    {
        cerr << "Error: Failed to add entry to node list for " << agent->cinfo->node.name << ", " << unsigned(self_node_id) << endl;
        exit(0);
    }
    // Add an entry for the remote node.
    if (add_node_id(agent->cinfo, remote_agent.node, remote_node_id) < 0)
    {
        cerr << "Error: Failed to add entry to node list for " << remote_agent.node << ", " << unsigned(remote_node_id) << endl;
        exit(0);
    }
    
    return 0;
}

// Since agent sends out a heartbeat on every available interface, compare
// against the obtained remote_addr to determine which interface and address was used.
string get_self_address(string remote_addr)
{
    // Do a string comparison to find the interface used.
    // E.g., For a remote_addr of 192.168.1.2, expect 192.168.1.3 to match more than 127.0.0.1
    uint16_t max_match = 0;
    string match_address = "";
    for (size_t i=0; i<agent->cinfo->agent0.ifcnt; i++)
    {
        string interface_addr = agent->cinfo->agent0.pub[i].address;
        uint16_t count = 0;
        for (size_t j=0; j<std::min(remote_addr.length(), interface_addr.length()); j++)
        {
            if (remote_addr[j] != interface_addr[j])
            {
                break;
            }
            count++;
        }
        if (count > max_match)
        {
            max_match = count;
            match_address = interface_addr;
        }
    }
    return match_address;
}

// Remove '.' from IP address and concatenate the numbers
// Expects iPV4 address
uint32_t ip_to_int(const std::string& ip)
{
    std::istringstream iss(ip);
    std::string token;
    uint32_t result = 0;
    int shift = 24;

    while (std::getline(iss, token, '.')) {
        result += (std::stoul(token) << shift);
        shift -= 8;
    }

    return result;
}

int32_t start_subagents(Agent *agent)
{
    int32_t iretn = 0;

    // Generate node ids if necessary for initializing the subagents
    iretn = generate_node_ids();
    if (iretn < 0)
    {
        return iretn;
    }

    // File subagent
    // For file transfers
    {
        file_module = new Cosmos::Module::FileModule();
        iretn = file_module->Init(agent, { remote_agent.node });
        if (iretn < 0)
        {
            printf("%f FILE: Init Error - Not Starting Loop: %s\n",agent->uptime.split(), cosmos_error_string(iretn).c_str());
            fflush(stdout);
        }
        else
        {
            file_thread = thread([=] { file_module->Loop(); });
            secondsleep(3.);
            printf("%f FILE: Thread started\n", agent->uptime.split());
            fflush(stdout);
        }
        // Set radios to use and in the order of the use priority, highest to lowest
        uint8_t COMM = agent->channel_number("COMM");
        file_module->set_radios({COMM});
    }

    // Websocket subagent
    // For communicating with PacketComm packets with websockets
    {
        websocket_module = new Cosmos::Module::WebsocketModule(Cosmos::Module::WebsocketModule::PacketizeFunction::Raw, Cosmos::Module::WebsocketModule::PacketizeFunction::Raw);
        iretn = websocket_module->Init(agent, remote_agent.addr, FILETRANSFERPORT, FILETRANSFERPORT, "COMM");
        if (iretn < 0)
        {
            printf("%f COMM: Init Error - Not Starting Loop: %s\n",agent->uptime.split(), cosmos_error_string(iretn).c_str());
            fflush(stdout);
        }
        else
        {
            websocket_thread = std::thread([=] { websocket_module->Loop(); });
            secondsleep(3.);
            printf("%f COMM: Thread started\n", agent->uptime.split());
            fflush(stdout);
        }
    }

    threads_started = true;

    printf("All threads started\n");
    fflush(stdout);

    // Enable file transfer and radio
    PacketHandler::QueueTransferRadio(agent->channel_number("COMM"), true, agent, self_node_id);
    agent->channel_touch(agent->channel_number("COMM"));

    return 0;
}

// TODOs:
// - Doesn't currently handle disconnection and relatively immediate connection before reset by a new file agent on another IP. What happens?
// - Add handshake before initiating file transfer. Currently, whoever is ready first will start firing packets at the other, who may not be ready yet.
// - Add terminal argument to be able to specify the nodename of the remote agent to search for.
// - Handle radio up-ness/age better? Auto-pings?
