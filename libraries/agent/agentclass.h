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

#ifndef COSMOSAGENT_H
#define COSMOSAGENT_H

/*! \file agentclass.h
*	\brief Agent Server and Client header file
*
*/

//! \ingroup support
//! \defgroup agentclass Agent Server and Client Library
//! %Agent Server and Client.
//!
//! These functions support the transformation of a generic program into a COSMOS aware program. The first level of
//! transformation creates a COSMOS Client that is able to speak the COSMOS language and communicate with any active
//! COSMOS Agents, while the second level of transformation creates a full COSMOS Agent.
//!
//! Clients are COSMOS aware programs that are made aware of the \ref jsonlib_namespace, and are capable of receiving
//! messages broadcast by any Agents on the same ::NetworkType. These messages, composed of JSON from the \ref jsonlib_namespace,
//! contain an initial header containing key information about the sending Agent, plus any additional \ref jsonlib_namespace values
//! that the particular Agent cares to make available. This allows the Clients to collect information about the local system,
//! and make requests of Agents. COSMOS Clients are equipped with a background thread that collects COSMOS messages and
//! stores them in a ring. Reading of messages is accomplised through Cosmos::Agent::readring, which gives you the next
//! message in the ring until you reach the most recent message. Ring size defaults to 100 messages, but can by changed
//! with Cosmos::Agent::resizering. The ring can be flushed at any time with Cosmos::Agent::clearring. Requests to agents
//! are made with Cosmos::Agent::send_request. As part of its message collection thread, the Client also keeps a list of
//! discovered Agents. This list can be used to provide the Agent information required by Cosmos::Agent::send_request through
//! use of Cosmos::Agent::find_agent. Finally, Clients open a Publication Channel for the sending of messages to other
//! COSMOS aware software. Messages are broadcast, using whatever mechanism is appropriate for the ::NetworkType chosen,
//! using Cosmos::Agent::post. They can be assigned any of 256 types, following the rules of Cosmos::Agent::AgentMessage.
//! The actual content over the network will be a single type byte, a 2 byte unsigned integer in little_endian order
//! containing the length in bytes of the header, a JSON header using values from the \ref jsonlib_namespace to represent
//! meta information about the Agent, and optional data, either as bytes (if type > 127), or additional JSON values. The
//! header contains the following fields from the ::beatstruc, returned from either Cosmos::Agent::readring or
//! Cosmos::Agent::find_agent:
//! - ::beatstruc::utc: The time of posting, expressed in Modified Julian Day.
//! - ::beatstruc::node: The name of the associated Node.
//! - ::beatstruc::proc: The name of the associated Agent.
//! - ::beatstruc::addr: The appropriate address for the ::NetworkType of the sending machine.
//! - ::beatstruc::port: The network port the Agent is listening on.
//! - ::beatstruc::bprd: The period of the Heartbeat in seconds.
//! - ::beatstruc::bsz: The size, in bytes, of the Agents request buffer.
//! - ::beatstruc::cpu: The CPU load of the machine the Agent is running on.
//! - ::beatstruc::memory: The memory usage of the machine the Agent is running on.
//! - ::beatstruc::jitter: The residual jitter, in seconds, of the Agents heartbeat loop.
//! - ::nodestruc::utcoffset: The offset, in Days, being applied to this times to time shift it.
//!
//! Agents are persistent COSMOS aware programs that provide the system framework for any
//! COSMOS implementation. They are similar to UNIX Daemons or Windows Services in that
//! they run continuously until commanded to stop, and serve a constant function, either
//! automatically or on demand. In addition to the features listed for Clients, Agents are provided with two
//! additional features, implemented as two additional threads of execution.
//!
//! - "Heartbeat": This is a Message, as described above, sent at regular intervals, with type Agent::AgentMessage::BEAT.
//! The optional data can be filled with State of Health information, established through Cosmos::Agent::set_sohstring.
//!
//! - "Requests": Requests are received as plain text commands and arguments, at the IP Port reported in the Heartbeat.
//! They are processed and any response is sent back. The response, even if empty, always ends with [OK], if understood,
//! or [NOK] if not. Requests and their responses must be less than the size of the communications
//! buffer. There are a number of requests already built in to the Agent. Additional requests can be
//! added using Cosmos::Agent::add_request, by tieing together user defined
//! functions with user defined ASCII strings. Built in requests include:
//!     - "forward" - Resends the received request out all interfaces.
//!     - "echo utc crc nbytes bytes" - requests the %Agent to echo the local time the request was received,
//! the values of the indicated JSON names.
//! the CRC calculated for the bytes, and the bytes themselves.
//!     - "help" - list available requests for this %Agent.
//!     - "shutdown" - causes the %Agent to stop what it is doing and exit.
//!     - "idle" - causes the %Agent to transition to Cosmos::Agent::State::IDLE.
//!     - "init" - causes the %Agent to transition to Cosmos::Agent::State::INIT.
//!     - "monitor" - causes the %Agent to transition to Cosmos::Agent::State::MONITOR.
//!     - "run" - causes the %Agent to transition to Cosmos::Agent::State::RUN.
//!     - "status" - causes the agent to dump any \ref jsonlib variables it is monitoring.
//!     - "debug_level" - Returns current Debug Level, or if an argument is provides, sets it.
//!     - "getvalue {\"json_name_000\"[,\"json_name_001\"]}" - requests the %Agent to return the values
//! of the indicated JSON names.
//!     - "setvalue {\"json_name_000\":value0[,\"json_name_001\":value1]}" - requests the %Agent to set
//!     - "listnames" - Returns a list of all names in the JSON Name Space.
//!     - "nodejson" - return the JSON representing the contents of node.ini.
//!     - "statejson" - return the JSON representing the contents of state.ini.
//!     - "utcstartjson" - return the JSON representing the contents of utcstart.ini.
//!     - "piecesjson" - return the JSON representing the contents of pieces.ini.
//!     - "strucsjson" - return the JSON representing the contents of strucs.ini.
//!     - "facesjson" - return the JSON representing the contents of faces.ini.
//!     - "verticesjson" - return the JSON representing the contents of vertices.ini.
//!     - "devgenjson" - return the JSON representing the contents of devgen.ini.
//!     - "devspecjson" - return the JSON representing the contents of devspec.ini.
//!     - "portsjson" - return the JSON representing the contents of ports.ini.
//!     - "aliasesjson" - return the JSON representing the contents of aliases.ini.
//!     - "targetsjson" - return the JSON representing the contents of targets.ini.
//!     - "heartbeat" - Sends a Heartbeat immediatelly.
//!
//!
//! Both Clients and Agents are formed using Cosmos::Agent. Once you have performed any initializations necessary, you should
//! enter a continuous loop, protected by Cosmos::Agent::running, and preferably surrendering control periodically
//! with Cosmos::secondsleep. Upon exiting from this loop, you should call Cosmos::Agent::shutdown.

#include "support/configCosmos.h"
#include "support/logger.h"
#include "support/jsonlib.h"
#include "support/jsonclass.h"
#include "device/cpu/devicecpu.h"
#include "support/packetcomm.h"
#include "support/channellib.h"
#include "support/beacon.h"
#include "task.h"

namespace Cosmos
{
    namespace Support
    {
        //! \class Agent agentclass.h "agent/agentclass.h"
        //! Add COSMOS awareness.
        //! Sets up minimum framework for COSMOS awareness. The minimum call makes a nodeless client, setting up the
        //! message ring buffer thread, and a main thread of execution. Additional parameters are related to making
        //! the program a true Agent by tieing it to a node, and starting the request and heartbeat threads.
        class Agent
        {
        public:
            //! \brief Minimal placeholder constructor; creates an uninitialized Agent shell.
            //! \param placeholder Unused sentinel value that selects this overload.
            Agent(uint8_t placeholder);

            //! \brief Full Agent constructor; creates a COSMOS client or agent tied to a node.
            //! \param realm_name Name of the COSMOS realm this agent belongs to.
            //! \param node_name Name of the node this agent is associated with; empty for a nodeless client.
            //! \param agent_name Name identifying this agent process within the node.
            //! \param bprd Heartbeat period (s); 0 disables the heartbeat thread.
            //! \param bsize Size of the request/response socket buffer (bytes).
            //! \param mflag If true, allow multiple agents with the same name to coexist.
            //! \param portnum UDP port number to listen on; 0 lets the system assign a port.
            //! \param ntype Network transport type (e.g., UDP multicast).
            //! \param dlevel Initial debug verbosity level.
            Agent(string realm_name = "",
                  string node_name = "",
                  string agent_name = "",
                  double bprd = 0.,
                  uint32_t bsize = AGENTMAXBUFFER,
                  bool mflag = false,
                  int32_t portnum = 0,
                  NetworkType ntype = NetworkType::UDP,
                  uint16_t dlevel = 0);

            ~Agent();

            //! State of Health element vector
            vector<jsonentry*> sohtable;
            vector<jsonentry*> fullsohtable;
			vector<string> sohstring;

            enum class State : uint16_t {
                //! Shut down Agent
                SHUTDOWN=0,
                //! Agent Initializing
                INIT,
                //! Do minimal necessary to run
                IDLE,
                //! Run without monitoring
                RUN,
                //! Run with monitoring
                MONITOR,
                //! Agent in Safe State
                ASAFE,
                //! Agent in Debug State
                DEBUG,
                //! Reset Agent
                RESET
            };

            //! Multiple agents per name
#define AGENT_MULTIPLE true
            //! Single agent per name
#define AGENT_SINGLE false
            //! Blocking Agent
#define AGENT_BLOCKING SOCKET_BLOCKING
            //! Non-blocking Agent
#define AGENT_NONBLOCKING SOCKET_NONBLOCKING
            //! Talk followed by optional listen (sendto address)
#define AGENT_TALK SOCKET_TALK
            //! Listen followed by optional talk (recvfrom INADDRANY)
#define AGENT_LISTEN SOCKET_LISTEN
            //! Communicate socket (sendto followed by recvfrom)
#define AGENT_COMMUNICATE SOCKET_COMMUNICATE
            //! Talk over multiple interfaces
#define AGENT_JABBER SOCKET_JABBER

            //! Base AGENT port number
#define AGENTBASE 10020
            //! Default SEND port
#define AGENTSENDPORT 10020
            //! Default RECV port
#define AGENTRECVPORT 10021
            //! AGENT heartbeat Multicast address
#define AGENTMCAST "225.1.1.1"
            //! Loopback
#define AGENTLOOPBACK "127.0.0.1"
            //! Maximum AGENT server list count
#define AGENTMAXLIST 500
            //! Maximum AGENT heartbeat size
#define AGENTMAXHEARTBEAT 200
            //! Default AGENT socket RCVTIMEO (100 msec)
#define AGENTRCVTIMEO 100000
            //! Default minium heartbeat period (10 msec)
#define AGENT_HEARTBEAT_PERIOD_MIN 0.01

            //! Default size of message ring buffer
#define MESSAGE_RING_SIZE 10000

            //! Type of Agent Message. Types > 127 are binary.
            enum class AgentMessage : uint8_t {
                //! All Message types
                ALL=1,
                //! Heartbeat Messages
                BEAT=2,
                //! State of Health Messages
                SOH=3,
                //! Generic Mesages
                GENERIC=4,
                TIME=5,
                LOCATION=6,
                TRACK=7,
                IMU=8,
                //! Event Messsages
                EVENT=9,
                //! Request message
                REQUEST=10,
                //! Response message
                RESPONSE=11,
                //! >= 128 are binary
                BINARY=128,
                COMM=129
            };

            enum class Where : size_t {
                HEAD = 0,
                TAIL = 1
            };

            //! @}

#define MAXARGCOUNT 100

            //! \ingroup agentclass
            //! \defgroup agentclass_typedefs Agent Server and Client Library typedefs
            //! @{


            //! List of heartbeats.
            //! Heartbeats for multiple processes found on the multicast bus.
            struct beatstruc_list
            {
                //! Number of heartbeats in list
                int16_t count;
                //! Pointer to an array of pointers to heartbeats
                beatstruc *heartbeat[AGENTMAXLIST];
            };

            struct pollstruc
            {
                AgentMessage type; // > 128 is binary, <128 is json, look for AGENT_MESSAGE in agentclass.h
                uint16_t jlength; // length of JSON header
                beatstruc beat; // all the information of the heartbeat (name, ip, etc.)
            };

            //! Storage for messages
            struct messstruc
            {
                pollstruc meta; // agent control information
                vector <uint8_t> bdata; // binary data if present
                string adata; // ascii data if present
                string jdata; // json header data, always present
            };

            //! Agent Request Function
            //! Format of a user supplied function to handle a given request
            typedef int32_t (*external_request_function)(string& request_string, string& output_string, Agent* agent);

            //! Simplified Agent Request Function
            //! Format of a user supplied function to handle a given request
            //! returns response string
//            typedef string (*simple_request_function)(vector<string>& request_args, int32_t &error);
            //! Simplified Agent Request Function
            //! Format of a user supplied function to handle a given request
            //! returns response string
//            typedef string (*no_arg_request_function)(int32_t &error);

            //! @}
            //!s
            // agent functions

            //! \brief Start the agent's request and heartbeat background threads.
            //! \return 0 on success, negative error code on failure.
            int32_t start();

            //! \brief Begin the agent's active processing loop, recording the start time.
            //! \return 0 on success, negative error code on failure.
            int32_t start_active_loop();

            //! \brief End the agent's active processing loop and release associated resources.
            //! \return 0 on success, negative error code on failure.
            int32_t finish_active_loop();

            //! \brief Register a new request handler with the agent.
            //! \param token ASCII command token that triggers this request.
            //! \param function Pointer to the handler function invoked when the token is received.
            //! \param synopsis Short one-line usage summary shown by the "help" request.
            //! \param description Extended description shown by the "help" request.
            //! \return 0 on success, negative error code on failure.
            int32_t add_request(string token, external_request_function function, string synopsis="", string description="");
//            int32_t add_request(string token, simple_request_function function, string synopsis="", string description="");
//            int32_t add_request(string token, no_arg_request_function function, string synopsis="", string description="");

            //! \brief Send a text request to another agent and retrieve the response.
            //! \param cbeat Heartbeat structure of the target agent (address, port, etc.).
            //! \param request Request string to transmit.
            //! \param output String populated with the agent's response.
            //! \param waitsec Maximum time to wait for a response (s).
            //! \param delay_send Additional delay before sending the request (s).
            //! \param delay_receive Additional delay before reading the response (s).
            //! \return 0 on success, negative error code on failure.
            int32_t send_request(beatstruc cbeat, string request, string &output, float waitsec=5., double delay_send = 0.0, double delay_receive = 0.0);

            //! \brief Send a request that retrieves the target agent's full JSON node description.
            //! \param cbeat Heartbeat structure of the target agent.
            //! \param jnode JSON node structure populated with the response data.
            //! \param waitsec Maximum time to wait for a response (s).
            //! \return 0 on success, negative error code on failure.
            int32_t send_request_jsonnode(beatstruc cbeat, jsonnode &jnode, float waitsec=5.);

            //! \brief Retrieve the heartbeat of a named agent, waiting up to waitsec for it to appear.
            //! \param node Name of the node the target agent belongs to.
            //! \param agent Name of the target agent process.
            //! \param waitsec Maximum time to wait for discovery (s).
            //! \param cbeat Heartbeat structure populated when the agent is found.
            //! \return 0 on success, negative error code if the agent is not found in time.
            int32_t get_agent(string node, string agent, double waitsec, beatstruc &cbeat);

            //! \brief Check whether a named agent is currently active on the network.
            //! \param node Name of the node the target agent belongs to.
            //! \param agent Name of the target agent process.
            //! \param waitsec Maximum time to wait for a heartbeat (s).
            //! \return Positive value if the agent is alive, negative error code otherwise.
            int32_t check_agent(string node, string agent, double waitsec);

            //! \brief Find a named agent and return its heartbeat structure.
            //! \param node Name of the node the target agent belongs to.
            //! \param agent Name of the target agent process.
            //! \param waitsec Maximum time to wait for discovery (s); 0 returns immediately.
            //! \return Heartbeat structure of the found agent; empty if not found.
            beatstruc find_agent(string node, string agent, double waitsec=0.);

            //! \brief Return heartbeat structures for all currently discovered agents.
            //! \param waitsec Time to wait while collecting agent heartbeats (s).
            //! \return Vector of heartbeat structures for each discovered agent.
            vector<beatstruc> find_agents(double waitsec=0.);

            //! \brief Check whether the agent is still in a running state.
            //! \return Non-zero if the agent should continue running, 0 if it should stop.
            uint16_t running();

            //! \brief Block until the agent reaches the specified state or timeout expires.
            //! \param state Target agent state to wait for.
            //! \param waitsec Maximum time to wait (s).
            //! \return 0 on success, negative error code if the state was not reached in time.
            int32_t wait(State state=State::RUN, double waitsec=10.);

            //! \brief Retrieve the most recent error code recorded by the agent.
            //! \return Last error value; 0 indicates no error.
            int32_t last_error();

            //! \brief Set the State of Health string from a whitespace-delimited list of JSON names.
            //! \param list Whitespace-delimited string of JSON namespace names to include in the SOH.
            //! \return 0 on success, negative error code on failure.
            int32_t set_sohstring(string list);

            //! \brief Set the State of Health string from a vector of JSON names.
            //! \param list Vector of JSON namespace names to include in the SOH message.
            //! \return 0 on success, negative error code on failure.
            int32_t set_sohstring(vector<string> list);

            //! \brief Set the secondary State of Health string from a vector of JSON names.
            //! \param list Vector of JSON namespace names for the secondary SOH message.
            //! \return 0 on success, negative error code on failure.
            int32_t set_sohstring2(vector<string> list);

            //! \brief Set the full State of Health string from a whitespace-delimited list of JSON names.
            //! \param list Whitespace-delimited string of all JSON namespace names to include.
            //! \return 0 on success, negative error code on failure.
            int32_t set_fullsohstring(string list);

            //! \brief Return a pointer to the underlying COSMOS data structure.
            //! \return Pointer to the cosmosstruc held by this agent.
            cosmosstruc *get_cosmosstruc();

            //! \brief Get the primary IP address of the host as a C string.
            //! \param buffer Caller-supplied buffer to receive the null-terminated address string.
            //! \param buflen Size of the caller-supplied buffer (bytes).
            void get_ip(char* buffer, size_t buflen);

            //! \brief Populate the internal address list for all interfaces listening on a given port.
            //! \param port UDP port number to query.
            void get_ip_list(uint16_t port);

            //! \brief Close and release the publication (multicast send) socket.
            //! \return 0 on success, negative error code on failure.
            int32_t unpublish();

            //! \brief Post a pre-built message structure to the publication channel.
            //! \param mess Fully populated message structure to transmit.
            //! \return 0 on success, negative error code on failure.
            int32_t post(messstruc mess);

            //! \brief Post a text message of the specified type.
            //! \param type AgentMessage type tag for the outgoing message.
            //! \param message Optional text payload to append after the JSON header.
            //! \return 0 on success, negative error code on failure.
            int32_t post(AgentMessage type, string message="");

            //! \brief Post a binary message of the specified type.
            //! \param type AgentMessage type tag (should be >= AgentMessage::BINARY).
            //! \param message Binary payload bytes to transmit.
            //! \return 0 on success, negative error code on failure.
            int32_t post(AgentMessage type, vector <uint8_t> message);

            //! \brief Immediately broadcast a heartbeat message.
            //! \return 0 on success, negative error code on failure.
            int32_t post_beat();

            //! \brief Immediately broadcast a State of Health message.
            //! \return 0 on success, negative error code on failure.
            int32_t post_soh();

            //! \brief Open a publication (multicast send) socket on the given network and port.
            //! \param type Network transport type to use for publishing.
            //! \param port UDP port to publish on.
            //! \return 0 on success, negative error code on failure.
            int32_t publish(NetworkType type, uint16_t port);

            //! \brief Open a subscription socket to receive messages from a multicast address.
            //! \param type Network transport type to subscribe on.
            //! \param address Multicast group address to join.
            //! \param port UDP port to listen on.
            //! \return 0 on success, negative error code on failure.
            int32_t subscribe(NetworkType type, const char *address, uint16_t port);

            //! \brief Open a subscription socket with a custom receive timeout.
            //! \param type Network transport type to subscribe on.
            //! \param address Multicast group address to join.
            //! \param port UDP port to listen on.
            //! \param usectimeo Socket receive timeout (microseconds).
            //! \return 0 on success, negative error code on failure.
            int32_t subscribe(NetworkType type, const char *address, uint16_t port, uint32_t usectimeo);

            //! \brief Close the subscription socket.
            //! \return 0 on success, negative error code on failure.
            int32_t unsubscribe();

            //    int32_t poll(pollstruc &meta, string &message, uint8_t type, float waitsec = 1.);
            //    int32_t poll(pollstruc &meta, vector <uint8_t> &message, uint8_t type, float waitsec = 1.);

            //! \brief Wait for and retrieve the next message of a given type from the network.
            //! \param mess Message structure populated with received data.
            //! \param type AgentMessage type filter; only messages of this type are returned.
            //! \param waitsec Maximum time to wait for a message (s).
            //! \return 0 on success, negative error code on timeout or failure.
            int32_t poll(messstruc &mess, AgentMessage type, float waitsec = 1.);

            //! \brief Read the next matching message from the ring buffer (by proc/node filter).
            //! \param message Message structure populated with the retrieved entry.
            //! \param type AgentMessage type filter.
            //! \param waitsec Maximum time to wait if the ring is empty (s).
            //! \param where Whether to read from the HEAD or TAIL of the ring.
            //! \param proc Agent process name filter; empty string matches all.
            //! \param node Node name filter; empty string matches all.
            //! \return 0 on success, negative error code on failure.
            int32_t readring(messstruc &message, AgentMessage type = Agent::AgentMessage::ALL, float waitsec = 1., Where where=Where::TAIL, string proc="", string node="");

            //! \brief Read the next matching message from the ring buffer (by realm/node filter).
            //! \param message Message structure populated with the retrieved entry.
            //! \param realm Realm name filter; empty string matches all.
            //! \param node Node name filter; empty string matches all.
            //! \param type AgentMessage type filter.
            //! \param waitsec Maximum time to wait if the ring is empty (s).
            //! \param where Whether to read from the HEAD or TAIL of the ring.
            //! \return 0 on success, negative error code on failure.
            int32_t readring(messstruc &message, string realm="", string node="", AgentMessage type = Agent::AgentMessage::ALL, float waitsec = 1., Where where=Where::TAIL);

            //! \brief Read the next matching message from the ring buffer (by realm list filter).
            //! \param message Message structure populated with the retrieved entry.
            //! \param realm Vector of realm name strings to accept.
            //! \param type AgentMessage type filter.
            //! \param waitsec Maximum time to wait if the ring is empty (s).
            //! \param where Whether to read from the HEAD or TAIL of the ring.
            //! \return 0 on success, negative error code on failure.
            int32_t readring(messstruc &message, vector<string> realm, AgentMessage type = Agent::AgentMessage::ALL, float waitsec = 1., Where where=Where::TAIL);

            //! \brief Parse messages in the ring buffer and dispatch matching entries.
            //! \param type AgentMessage type filter.
            //! \param waitsec Maximum time to wait for new messages (s).
            //! \param where Whether to parse from the HEAD or TAIL of the ring.
            //! \param proc Agent process name filter; empty string matches all.
            //! \param node Node name filter; empty string matches all.
            //! \return Number of messages parsed, or negative error code on failure.
            int32_t parsering(AgentMessage type = Agent::AgentMessage::ALL, float waitsec=1., Where where=Where::HEAD, string proc="", string node="");

            //! \brief Resize the incoming message ring buffer.
            //! \param newsize New number of message slots in the ring.
            //! \return 0 on success, negative error code on failure.
            int32_t resizering(size_t newsize);

            //! \brief Discard all messages currently held in the ring buffer.
            //! \return 0 on success, negative error code on failure.
            int32_t clearring();

            //    timestruc poll_time(float waitsec);
            //    beatstruc poll_beat(float waitsec);
            //    Convert::locstruc poll_location(float waitsec);
            //    nodestruc poll_info(float waitsec);
            //    imustruc poll_imu(float waitsec);

            //! \brief Map the agent structure into the JSON namespace.
            //! \param agent Address of the pointer that will be set to the mapped agentstruc.
            //! \return 0 on success, negative error code on failure.
            int json_map_agentstruc(agentstruc **agent);

            //! \brief Return all network interface addresses matching the given network type.
            //! \param ntype Network transport type to filter by.
            //! \return Vector of socket_channel structures describing each matching interface.
            vector<socket_channel> find_addresses(NetworkType ntype);

            //! \brief Gracefully shut down the agent and stop all background threads.
            //! \return 0 on success, negative error code on failure.
            int32_t shutdown();

            //! \brief Send a raw message string to the specified channel address.
            //! \param address Channel address index to send on.
            //! \param message Message string to transmit.
            //! \return 0 on success, negative error code on failure.
            int32_t send(uint8_t address, string message);

            //! \brief Receive the next message from the specified channel address.
            //! \param address Channel address index to receive from.
            //! \param message String populated with the received message.
            //! \return 0 on success, negative error code on failure.
            int32_t receive(uint8_t address, string &message);

            //! \brief Receive and concatenate all pending messages from the specified channel address.
            //! \param address Channel address index to drain.
            //! \param message String populated with all received messages concatenated.
            //! \return 0 on success, negative error code on failure.
            int32_t receiveAll(uint8_t address, string &message);

            //! \brief Return the name of the node this agent is associated with.
            //! \return Node name string.
            string getNode();

            //! \brief Return the name of this agent process.
            //! \return Agent name string.
            string getAgent();

            //! \brief Retrieve the JSON node description for the named node.
            //! \param node Name of the node to query.
            //! \param jnode JSON node structure populated with the response.
            //! \return 0 on success, negative error code on failure.
            int32_t getJson(string node, jsonnode &jnode);

            //! \brief Register an external function as the agent's time source.
            //! \param source Pointer to a function returning the current time as a double MJD value.
            //! \return 0 on success, negative error code on failure.
            int32_t set_agent_time_producer(double (*source)());

            //! \brief Query another agent for its current time and compute the clock offset.
            //! \param agent_time Receives the remote agent's current time (MJD).
            //! \param epsilon Receives the estimated one-way propagation delay (s).
            //! \param delta Receives the computed clock offset between local and remote (s).
            //! \param agent Name of the agent to query.
            //! \param node Name of the node the target agent belongs to.
            //! \param wait_sec Maximum time to wait for a response (s).
            //! \return 0 on success, negative error code on failure.
            int32_t get_agent_time(double &agent_time, double &epsilon, double &delta, string agent, string node="any", double wait_sec=2.);

            // general functionality for artemis
            //! \brief Set the active loop period for rate-limited processing.
            //! \param period Desired loop period (s).
            //! \return 0 on success, negative error code on failure.
            int32_t set_activity_period(double period);

            //! \brief Add a new device to the COSMOS namespace for this agent's node.
            //! \param name Unique name to assign to the device in the namespace.
            //! \param type Device type identifier (e.g., GPS, IMU, battery).
            //! \param device Address of the pointer set to the newly allocated devicestruc.
            //! \return 0 on success, negative error code on failure.
            int32_t add_device(string name, DeviceType type, devicestruc **device);

            //! \brief Resolve the fully-qualified JSON namespace name for a device property.
            //! \param device Name of the device as registered in the namespace.
            //! \param property Name of the property on that device.
            //! \param name Receives the fully-qualified JSON namespace name string.
            //! \return 0 on success, negative error code on failure.
            int32_t device_property_name(string device, string property, string& name);

            //! \brief Send a "getvalue" request to an agent and parse the JSON response.
            //! \param agent Heartbeat structure of the target agent.
            //! \param names Vector of JSON namespace names to request.
            //! \param jobj JSON object populated with the name-value pairs from the response.
            //! \return 0 on success, negative error code on failure.
            int32_t send_request_getvalue(beatstruc agent, vector<string> names, Json::Object &jobj);

            //! \brief Create a namespace alias for a device property value.
            //! \param devicename Name of the device as registered in the namespace.
            //! \param propertyname Name of the property to alias.
            //! \param alias New alias string to register in the JSON namespace.
            //! \return 0 on success, negative error code on failure.
            int32_t create_device_value_alias(string devicename, string propertyname, string alias);

            //! \brief Create an alias for an existing JSON namespace name.
            //! \param cosmosname Existing fully-qualified JSON namespace name to alias.
            //! \param alias New alias string to map to cosmosname.
            //! \return 0 on success, negative error code on failure.
            int32_t create_alias(string cosmosname, string alias);

            //! \brief Set a numeric value in the JSON namespace by name.
            //! \param jsonname Fully-qualified JSON namespace name of the value to set.
            //! \param value New double-precision value to assign.
            //! \return 0 on success, negative error code on failure.
            int32_t set_value(string jsonname, double value);

            //! \brief Retrieve a numeric value from the JSON namespace by name.
            //! \param jsonname Fully-qualified JSON namespace name of the value to read.
            //! \return Current double-precision value, or 0.0 if not found.
            double get_value(string jsonname);

            //! \brief Retrieve multiple property values for a device as a JSON string.
            //! \param device Name of the device as registered in the namespace.
            //! \param props Vector of property names to retrieve.
            //! \param json Receives the JSON-formatted string of property name-value pairs.
            //! \return 0 on success, negative error code on failure.
            int32_t get_device_values(string device, vector<string>props, string& json);

            //! \brief Retrieve multiple namespace values as a JSON string.
            //! \param names Vector of fully-qualified JSON namespace names to read.
            //! \param json Receives the JSON-formatted string of name-value pairs.
            //! \return 0 on success, negative error code on failure.
            int32_t get_values(vector<string> names, string& json);

            //! \brief Return the UTC start time recorded when the agent was initialized.
            //! \return Agent start time in Modified Julian Date.
            double get_timeStart();



            // poll
            pollstruc metaRx;
            string metaHeader;

            //! \brief Write a timestamped entry to the agent's debug log.
            //! \param log_entry Text string to append to the log.
            void log(string log_entry);

            //! \brief Set the SOH field list from a comma-delimited string.
            //! \param sohFields Comma-delimited list of JSON namespace names to include in SOH.
            //! \return true on success, false if any field name was not found in the namespace.
            bool setSoh(string sohFields);
            cosmosstruc *cinfo;

            //! List of active agents
            vector <beatstruc> agent_list;
            //! List of active Nodes
            vector <jsonnode> node_list;

            //! Ring buffer for incoming messages
//            vector <messstruc> message_ring;
            deque <messstruc> message_queue;
            //! Last message placed in message ring buffer
            size_t message_head = MESSAGE_RING_SIZE;
            //! Last message rad in message ring buffer
            size_t message_tail = MESSAGE_RING_SIZE;

            //! Flag for level of debugging, keep it public so that it can be controlled from the outside
            //! \brief Set the debug verbosity level.
            //! \param level New debug level (0 = silent, higher values produce more output).
            //! \return 0 on success, negative error code on failure.
            int32_t set_debug_level(uint16_t level);

            //! \brief Return the current debug verbosity level.
            //! \return Current debug level value.
            int32_t get_debug_level();

            //! \brief Return the FILE pointer for the debug log, opening a dated file if needed.
            //! \param mjd MJD used to generate the log file name; 0 uses the current time.
            //! \return Pointer to the open debug log FILE, or nullptr on error.
            FILE *get_debug_fd(double mjd=0.);

            //! \brief Close the currently open debug log file descriptor.
            //! \return 0 on success, negative error code on failure.
            int32_t close_debug_fd();

            // Add implementation of new COSMOS Error
            Log::Logger debug_log;
//            int32_t Printf(string output);
//            int32_t Printf(const char *fmt, ...);

            // agent variables
            ElapsedTime uptime;
//            string cinfo->node.name;
//            string cinfo->agent0.name;
            vector<beatstruc> slist;
//            NodeList nodeData;
            NODE_ID_TYPE nodeId;

            //! \brief Parse an incoming request string and write the response.
            //! \param bufferin String containing the raw incoming request.
            //! \param bufferout String populated with the response (always ends with [OK] or [NOK]).
            //! \param send_response If true, the response is also sent back over the request socket.
            //! \return 0 on success, negative error code on failure.
            int32_t process_request(string &bufferin, string &bufferout, bool send_response=true);

            //! \brief Store the verification token used for authenticated channel operations.
            //! \param verification 32-bit verification token value.
            //! \return 0 on success, negative error code on failure.
            int32_t set_verification(uint32_t verification);

            //! \brief Retrieve the stored verification token.
            //! \return Current 32-bit verification token.
            uint32_t get_verification();

            //! \brief Verify a supplied token against the stored verification value.
            //! \param verification Token to check.
            //! \return 0 if the token matches, negative error code if it does not.
            int32_t check_verification(uint32_t verification);

            //! \brief Set the channel designated as the communications priority channel.
            //! \param number Index of the channel to promote to communications priority.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_set_comm_priority(uint8_t number);

            //! \brief Return the total number of registered channels.
            //! \return Number of channels currently registered with this agent.
            int32_t channel_count();

            //! \brief Push a packet onto the default (first) channel queue.
            //! \param packet PacketComm packet to enqueue.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_push(PacketComm &packet);

            //! \brief Push a packet onto the named channel queue.
            //! \param name Name of the target channel.
            //! \param packet PacketComm packet to enqueue.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_push(string name, PacketComm &packet);

            //! \brief Push a packet onto the indexed channel queue.
            //! \param number Index of the target channel.
            //! \param packet PacketComm packet to enqueue.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_push(uint8_t number, PacketComm& packet);

            //! \brief Push a vector of packets onto the named channel queue.
            //! \param name Name of the target channel.
            //! \param packets Vector of PacketComm packets to enqueue.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_push(string name, vector<PacketComm>& packets);

            //! \brief Push a vector of packets onto the indexed channel queue.
            //! \param number Index of the target channel.
            //! \param packets Vector of PacketComm packets to enqueue.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_push(uint8_t number, vector<PacketComm>& packets);

            //! \brief Push a text response packet onto the named channel.
            //! \param name Name of the target channel.
            //! \param sourceid Originating node/agent identifier.
            //! \param dest Destination node/agent identifier.
            //! \param id Request identifier this response corresponds to.
            //! \param response Optional text payload for the response packet.
            //! \return 0 on success, negative error code on failure.
            int32_t push_response(string name, uint8_t sourceid, uint8_t dest, uint32_t id, string response="");

            //! \brief Push a text response packet onto the indexed channel.
            //! \param number Index of the target channel.
            //! \param sourceid Originating node/agent identifier.
            //! \param dest Destination node/agent identifier.
            //! \param id Request identifier this response corresponds to.
            //! \param response Optional text payload for the response packet.
            //! \return 0 on success, negative error code on failure.
            int32_t push_response(uint8_t number, uint8_t sourceid, uint8_t dest, uint32_t id, string response="");

            //! \brief Push a binary response packet onto the named channel.
            //! \param name Name of the target channel.
            //! \param sourceid Originating node/agent identifier.
            //! \param dest Destination node/agent identifier.
            //! \param id Request identifier this response corresponds to.
            //! \param response Binary payload bytes for the response packet.
            //! \return 0 on success, negative error code on failure.
            int32_t push_response(string name, uint8_t sourceid, uint8_t dest, uint32_t id, vector<uint8_t> response);

            //! \brief Push a binary response packet onto the indexed channel.
            //! \param number Index of the target channel.
            //! \param sourceid Originating node/agent identifier.
            //! \param dest Destination node/agent identifier.
            //! \param id Request identifier this response corresponds to.
            //! \param response Binary payload bytes for the response packet.
            //! \return 0 on success, negative error code on failure.
            int32_t push_response(uint8_t number, uint8_t sourceid, uint8_t dest, uint32_t id, vector<uint8_t> response);

            //! \brief Push a hardware command response packet onto the named channel.
            //! \param type PacketComm type identifier for the hardware response.
            //! \param name Name of the target channel.
            //! \param dest Destination node/agent identifier.
            //! \param unit Hardware unit number on the target device.
            //! \param command Command opcode that triggered this response.
            //! \param response Binary payload bytes containing the hardware response data.
            //! \return 0 on success, negative error code on failure.
            int32_t push_hardware_response(PacketComm::TypeId type, string name, uint8_t dest, uint8_t unit, uint8_t command, vector<uint8_t> response);

            //! \brief Push a hardware command response packet onto the indexed channel.
            //! \param type PacketComm type identifier for the hardware response.
            //! \param number Index of the target channel.
            //! \param dest Destination node/agent identifier.
            //! \param unit Hardware unit number on the target device.
            //! \param command Command opcode that triggered this response.
            //! \param response Binary payload bytes containing the hardware response data.
            //! \return 0 on success, negative error code on failure.
            int32_t push_hardware_response(PacketComm::TypeId type, uint8_t number, uint8_t dest, uint8_t unit, uint8_t command, vector<uint8_t> response);

            //! \brief Pull the next available packet from the named channel queue.
            //! \param name Name of the source channel.
            //! \param packet PacketComm structure populated with the dequeued packet.
            //! \return 0 on success, negative error code if the channel is empty.
            int32_t channel_pull(string name, PacketComm& packet);

            //! \brief Pull the next available packet from the indexed channel queue.
            //! \param number Index of the source channel.
            //! \param packet PacketComm structure populated with the dequeued packet.
            //! \return 0 on success, negative error code if the channel is empty.
            int32_t channel_pull(uint8_t number, PacketComm& packet);

            //! \brief Log an unwrapped (pre-framing) packet to the named channel monitor log.
            //! \param name Name of the channel whose monitor log receives the entry.
            //! \param packet PacketComm packet to log.
            //! \param extra Optional extra annotation string appended to the log entry.
            //! \return 0 on success, negative error code on failure.
            int32_t monitor_unwrapped(string name, PacketComm& packet, string extra="");

            //! \brief Log an unwrapped packet to the indexed channel monitor log.
            //! \param number Index of the channel whose monitor log receives the entry.
            //! \param packet PacketComm packet to log.
            //! \param extra Optional extra annotation string appended to the log entry.
            //! \return 0 on success, negative error code on failure.
            int32_t monitor_unwrapped(uint8_t number, PacketComm& packet, string extra="");

            //! \brief Log an unpacketized (raw payload) frame to the named channel monitor log.
            //! \param name Name of the channel whose monitor log receives the entry.
            //! \param packet PacketComm packet whose raw payload is logged.
            //! \param extra Optional extra annotation string appended to the log entry.
            //! \return 0 on success, negative error code on failure.
            int32_t monitor_unpacketized(string name, PacketComm& packet, string extra="");

            //! \brief Log an unpacketized frame to the indexed channel monitor log.
            //! \param number Index of the channel whose monitor log receives the entry.
            //! \param packet PacketComm packet whose raw payload is logged.
            //! \param extra Optional extra annotation string appended to the log entry.
            //! \return 0 on success, negative error code on failure.
            int32_t monitor_unpacketized(uint8_t number, PacketComm& packet, string extra="");

            //! \brief Initialize the channel subsystem and validate the verification token.
            //! \param verification Expected verification token; default matches the built-in sentinel.
            //! \return 0 on success, negative error code if verification fails.
            int32_t init_channels(uint32_t verification=0x352e);

            //! \brief Register a new communication channel with the agent.
            //! \param name Unique name for the channel.
            //! \param datasize Maximum payload data size per packet (bytes); 0 uses default.
            //! \param rawsize Maximum raw (framed) packet size (bytes); 0 uses default.
            //! \param byte_rate Target throughput rate for this channel (bytes/s); 0 = unlimited.
            //! \param maximum Maximum number of packets allowed in the channel queue; 0 = unlimited.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_add(string name, uint16_t datasize=0, uint16_t rawsize=0, float byte_rate=0., uint16_t maximum=0);

            //! \brief Update parameters of an existing channel by name.
            //! \param name Name of the channel to update.
            //! \param datasize New maximum payload data size (bytes); 0 leaves unchanged.
            //! \param rawsize New maximum raw packet size (bytes); 0 leaves unchanged.
            //! \param byte_rate New target throughput rate (bytes/s); 0 leaves unchanged.
            //! \param maximum New maximum queue depth; 0 leaves unchanged.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_update(string name, uint16_t datasize=0, uint16_t rawsize=0, float byte_rate=0., uint16_t maximum=0);

            //! \brief Update parameters of an existing channel by index.
            //! \param number Index of the channel to update.
            //! \param datasize New maximum payload data size (bytes); 0 leaves unchanged.
            //! \param rawsize New maximum raw packet size (bytes); 0 leaves unchanged.
            //! \param byte_rate New target throughput rate (bytes/s); 0 leaves unchanged.
            //! \param maximum New maximum queue depth; 0 leaves unchanged.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_update(uint8_t number, uint16_t datasize=0, uint16_t rawsize=0, float byte_rate=0., uint16_t maximum=0);

            //! Number of packets currently in the channel
            //! \param name Name of the channel to query.
            //! \return Current number of packets queued in the channel, or negative error code.
            int32_t channel_size(string name);

            //! Number of packets currently in the channel
            //! \param number Index of the channel to query.
            //! \return Current number of packets queued in the channel, or negative error code.
            int32_t channel_size(uint8_t number);

            //! \brief Return the current throughput speed of the named channel.
            //! \param name Name of the channel to query.
            //! \return Measured throughput (bytes/s).
            float channel_speed(string name);

            //! \brief Return the current throughput speed of the indexed channel.
            //! \param number Index of the channel to query.
            //! \return Measured throughput (bytes/s).
            float channel_speed(uint8_t number);

            //! \brief Return the elapsed time since the last activity on the named channel.
            //! \param name Name of the channel to query.
            //! \return Age of the last channel activity (s).
            double channel_age(string name);

            //! \brief Return the elapsed time since the last activity on the indexed channel.
            //! \param number Index of the channel to query.
            //! \return Age of the last channel activity (s).
            double channel_age(uint8_t number);

            //! \brief Get or set the wakeup timer for the named channel.
            //! \param name Name of the channel to query or update.
            //! \param value New wakeup timer value (s); 0 queries without changing.
            //! \return Current wakeup timer value (s) after the call.
            double channel_wakeup_timer(string name, double value=0.);

            //! \brief Get or set the wakeup timer for the indexed channel.
            //! \param number Index of the channel to query or update.
            //! \param value New wakeup timer value (s); 0 queries without changing.
            //! \return Current wakeup timer value (s) after the call.
            double channel_wakeup_timer(uint8_t number, double value=0.);

            //! \brief Return the cumulative byte count transferred on the named channel.
            //! \param name Name of the channel to query.
            //! \return Total bytes transferred on this channel since initialization.
            size_t channel_bytes(string name);

            //! \brief Return the cumulative byte count transferred on the indexed channel.
            //! \param number Index of the channel to query.
            //! \return Total bytes transferred on this channel since initialization.
            size_t channel_bytes(uint8_t number);

            //! \brief Return the current queue occupancy level of the named channel.
            //! \param name Name of the channel to query.
            //! \return Number of bytes currently buffered in the channel queue.
            size_t channel_level(string name);

            //! \brief Return the current queue occupancy level of the indexed channel.
            //! \param number Index of the channel to query.
            //! \return Number of bytes currently buffered in the channel queue.
            size_t channel_level(uint8_t number);

            //! \brief Return the cumulative packet count for the named channel.
            //! \param name Name of the channel to query.
            //! \return Total packets processed by this channel since initialization.
            uint32_t channel_packets(string name);

            //! \brief Return the cumulative packet count for the indexed channel.
            //! \param number Index of the channel to query.
            //! \return Total packets processed by this channel since initialization.
            uint32_t channel_packets(uint8_t number);

            //! \brief Update or read the last-touch timestamp of the named channel.
            //! \param name Name of the channel to touch.
            //! \param seconds If non-zero, sets the touch timestamp to this MJD offset; 0 just reads.
            //! \return Current last-touch timestamp (MJD).
            double channel_touch(string name, double seconds=0.);

            //! \brief Update or read the last-touch timestamp of the indexed channel.
            //! \param number Index of the channel to touch.
            //! \param seconds If non-zero, sets the touch timestamp to this MJD offset; 0 just reads.
            //! \return Current last-touch timestamp (MJD).
            double channel_touch(uint8_t number, double seconds=0.);

            //! \brief Increment the byte and packet counters of the named channel.
            //! \param name Name of the channel to update.
            //! \param bytes Number of bytes to add to the channel's byte counter.
            //! \param packets Number of packets to add to the channel's packet counter.
            //! \return Updated cumulative byte count, or negative error code on failure.
            ssize_t channel_increment(string name, size_t bytes, uint32_t packets=1);

            //! \brief Increment the byte and packet counters of the indexed channel.
            //! \param number Index of the channel to update.
            //! \param bytes Number of bytes to add to the channel's byte counter.
            //! \param packets Number of packets to add to the channel's packet counter.
            //! \return Updated cumulative byte count, or negative error code on failure.
            ssize_t channel_increment(uint8_t number, size_t bytes, uint32_t packets=1);
//            ssize_t channel_decrement(string name, size_t bytes, uint32_t packets=1);
//            ssize_t channel_decrement(uint8_t number, size_t bytes, uint32_t packets=1);

            //! \brief Clear all queued packets from the named channel.
            //! \param name Name of the channel to clear.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_clear(string name);

            //! \brief Clear all queued packets from the indexed channel.
            //! \param number Index of the channel to clear.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_clear(uint8_t number);

            //! \brief Look up the index of a channel by name.
            //! \param name Name of the channel to find.
            //! \return Channel index on success, negative error code if not found.
            int32_t channel_number(string name);

            //! \brief Look up the name of a channel by index.
            //! \param number Index of the channel to query.
            //! \return Channel name string, or empty string if the index is out of range.
            string channel_name(uint8_t number);

            //! Max data size of channel
            //! \param name Name of the channel to query.
            //! \return Maximum payload data size (bytes), or negative error code if not found.
            int32_t channel_datasize(string name);

            //! Max data size of channel
            //! \param number Index of the channel to query.
            //! \return Maximum payload data size (bytes), or negative error code if out of range.
            int32_t channel_datasize(uint8_t number);

            //! Max packet size of channel
            //! \param name Name of the channel to query.
            //! \return Maximum raw packet size (bytes), or negative error code if not found.
            int32_t channel_rawsize(string name);

            //! Max packet size of channel
            //! \param number Index of the channel to query.
            //! \return Maximum raw packet size (bytes), or negative error code if out of range.
            int32_t channel_rawsize(uint8_t number);

            //! \brief Return the maximum queue depth of the named channel.
            //! \param name Name of the channel to query.
            //! \return Maximum queue depth (packet count), or negative error code if not found.
            int32_t channel_maximum(string name);

            //! \brief Return the maximum queue depth of the indexed channel.
            //! \param number Index of the channel to query.
            //! \return Maximum queue depth (packet count), or negative error code if out of range.
            int32_t channel_maximum(uint8_t number);

            //! \brief Start a channel throughput test sequence by name.
            //! \param name Name of the channel to test.
            //! \param radio Name of the radio interface to use for the test.
            //! \param id Unique test session identifier.
            //! \param orig Originating node identifier.
            //! \param dest Destination node identifier.
            //! \param start Starting packet size for the test sweep (bytes).
            //! \param step Increment in packet size between test steps (bytes).
            //! \param stop Final packet size for the test sweep (bytes).
            //! \param total Total number of packets to send in the test.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_teststart(string name, string radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total);

            //! \brief Start a channel throughput test sequence by index.
            //! \param number Index of the channel to test.
            //! \param nradio Index of the radio interface to use for the test.
            //! \param id Unique test session identifier.
            //! \param orig Originating node identifier.
            //! \param dest Destination node identifier.
            //! \param start Starting packet size for the test sweep (bytes).
            //! \param step Increment in packet size between test steps (bytes).
            //! \param stop Final packet size for the test sweep (bytes).
            //! \param total Total number of packets to send in the test.
            //! \return 0 on success, negative error code on failure.
            int32_t channel_teststart(uint8_t number, uint8_t nradio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total);

            //! \brief Stop an ongoing channel test on the named channel.
            //! \param name Name of the channel whose test should be stopped.
            //! \param seconds Grace period to wait for in-flight packets to drain (s).
            //! \return 0 on success, negative error code on failure.
            int32_t channel_teststop(string name, float seconds=5.);

            //! \brief Stop an ongoing channel test on the indexed channel.
            //! \param number Index of the channel whose test should be stopped.
            //! \param seconds Grace period to wait for in-flight packets to drain (s).
            //! \return 0 on success, negative error code on failure.
            int32_t channel_teststop(uint8_t number, float seconds=5.);

            //! \brief Query whether the named channel is currently enabled.
            //! \param name Name of the channel to check.
            //! \return Positive value if enabled, 0 if disabled, negative error code if not found.
            int32_t channel_enabled(string name);

            //! \brief Query whether the indexed channel is currently enabled.
            //! \param number Index of the channel to check.
            //! \return Positive value if enabled, 0 if disabled, negative error code if out of range.
            int32_t channel_enabled(uint8_t number);

            //! \brief Enable or disable the named channel.
            //! \param name Name of the channel to modify.
            //! \param value Positive to enable, 0 to disable, negative to toggle.
            //! \return 0 on success, negative error code if not found.
            int32_t channel_enable(string name, int8_t value);

            //! \brief Enable or disable the indexed channel.
            //! \param number Index of the channel to modify.
            //! \param value Positive to enable, 0 to disable, negative to toggle.
            //! \return 0 on success, negative error code if out of range.
            int32_t channel_enable(uint8_t number, int8_t value);
//            int32_t channel_disable(string name);
//            int32_t channel_disable(uint8_t number);

            //! \brief Schedule a shell command as a tracked background task.
            //! \param command Shell command string to execute.
            //! \param source Optional label identifying the source that requested this task.
            //! \param timeout Maximum wall-clock time (s) before the task is killed.
            //! \return Non-negative task slot index on success, negative error code on failure.
            int32_t task_add(string command, string source="", float timeout=60.);

            //! \brief Remove a tracked task by its decisecond timestamp.
            //! \param deci Decisecond-resolution timestamp identifying the task to remove.
            //! \return 0 on success, negative error code if the task is not found.
            int32_t task_del(uint32_t deci);

            //! \brief Check whether a task identified by decisecond timestamp is still registered.
            //! \param deci Decisecond-resolution timestamp identifying the task.
            //! \return Positive value if the task exists, 0 or negative if not found.
            int32_t task_exists(uint32_t deci);

            //! \brief Return the last exit/return code of the indexed task.
            //! \param number Index of the task to query.
            //! \return Exit code of the task, or negative error code if out of range.
            int32_t task_iretn(uint16_t number);

            //! \brief Return the decisecond timestamp of the indexed task.
            //! \param number Index of the task to query.
            //! \return Decisecond-resolution creation timestamp of the task.
            uint32_t task_deci(uint16_t number);

            //! \brief Return the MJD start time of the indexed task.
            //! \param number Index of the task to query.
            //! \return Start time of the task in Modified Julian Date.
            double task_startmjd(uint16_t number);

            //! \brief Return the current execution state of the indexed task.
            //! \param number Index of the task to query.
            //! \return State byte (e.g., running, complete, failed).
            uint8_t task_state(uint16_t number);

            //! \brief Return the command string of the indexed task.
            //! \param number Index of the task to query.
            //! \return Shell command string associated with the task.
            string task_command(uint16_t number);

            //! \brief Return the resolved executable path of the indexed task.
            //! \param number Index of the task to query.
            //! \return File system path of the executable being run by the task.
            string task_path(uint16_t number);

            //! \brief Return the number of tasks currently tracked by the agent.
            //! \return Current task count.
            int32_t task_size();

        protected:
        private:

            Task tasks;
            Channel channels;
            uint16_t debug_level = 0;
            NetworkType networkType = NetworkType::UDP;
            double activeTimeout = 0.0; // in MJD
            uint32_t bufferSize = AGENTMAXBUFFER;
            bool multiflag = false;
            int32_t portNumber = 0;
            FILE *debug_fd = nullptr;
            string debug_pathName;
            string version = "2.0";
            double timeoutSec = 2.0;
            bool logTime = true; // by default
            double timeStart; // UTC starting time for this agent in MJD
            string hbjstring;
            //! Handle for request thread
            thread cthread;
            //! Handle for heartbeat thread
            thread hthread;
            //! Handle for message thread
            thread mthread;
            //! Last error
            int32_t error_value = 0;
            //! mutex to protect process_request
            mutex process_mutex;
            //! mutex to protect ring
            mutex ring_mutex;

            //! Function in which we generate our time, for the mjd request.
            double (*agent_time_producer)() = currentmjd;

            //! Agent Request Entry
            //! Structure representing a single Agent request.
            struct request_entry
            {
                //! Character token for request
                string token;
                //! Pointer to function to call with request string as argument and returning any error
                external_request_function efunction;
//                simple_request_function sfunction;
//                no_arg_request_function nafunction;
                string synopsis;
                string description;
            };

            map<string, request_entry> reqs;

            void heartbeat_loop();
            void request_loop() noexcept;
            void message_loop();

            char* parse_request(char *input);
            DeviceCpu deviceCpu_;

            static int32_t req_forward(string &request, string &response, Agent *agent);
            static int32_t req_echo(string &request, string &response, Agent *agent);
            static int32_t req_help(string &request, string &response, Agent *agent);
            static int32_t req_help_json(string &request, string &response, Agent *agent);
            static int32_t req_shutdown(string &request, string &response, Agent *agent);
            static int32_t req_idle(string &request, string &response, Agent *agent);
            static int32_t req_init(string &request, string &response, Agent *agent);
            static int32_t req_monitor(string &request, string &response, Agent *agent);
            static int32_t req_reset(string &request, string &response, Agent *agent);
            static int32_t req_run(string &request, string &response, Agent *agent);
            static int32_t req_status(string &request, string &response, Agent *agent);
            static int32_t req_debug_level(string &request, string &response, Agent *agent);
            static int32_t req_getvalue(string &request, string &response, Agent *agent);
            static int32_t req_get_value(string &request, string &response, Agent *agent);
            static int32_t req_get_state(string &request, string &response, Agent *agent);
            static int32_t req_get_time(string &request, string &response, Agent *agent);
            static int32_t req_get_position(string &request, string &response, Agent *agent);
            static int32_t req_get_location(string &request, string &response, Agent *agent);
            static int32_t req_get_state_vector(string &request, string &response, Agent *agent);
            static int32_t req_get_position_data(string &request, string &response, Agent *agent);
            static int32_t req_setvalue(string &request, string &response, Agent *agent);
            static int32_t req_set_value(string &request, string &response, Agent *agent);
            static int32_t req_listnames(string &request, string &response, Agent *agent);
            static int32_t req_nodejson(string &request, string &response, Agent *agent);
            static int32_t req_statejson(string &request, string &response, Agent *agent);
//            static int32_t req_utcstartjson(string &request, string &response, Agent *agent);
            static int32_t req_piecesjson(string &request, string &response, Agent *agent);
            static int32_t req_verticesjson(string &request, string &response, Agent *agent);
            static int32_t req_strucsjson(string &request, string &response, Agent *agent);
            static int32_t req_facesjson(string &request, string &response, Agent *agent);
            static int32_t req_trianglesjson(string &request, string &response, Agent *agent);
            static int32_t req_devgenjson(string &request, string &response, Agent *agent);
            static int32_t req_devspecjson(string &request, string &response, Agent *agent);
            static int32_t req_portsjson(string &request, string &response, Agent *agent);
            static int32_t req_targetsjson(string &request, string &response, Agent *agent);
            static int32_t req_aliasesjson(string &request, string &response, Agent *agent);
            static int32_t req_heartbeat(string &request, string &response, Agent *agent);
            static int32_t req_postsoh(string &request, string &response, Agent *agent);
            static int32_t req_utc(string &request, string &response, Agent *agent);
            static int32_t req_soh(string &, string &response, Agent *agent);
            static int32_t req_fullsoh(string &, string &response, Agent *agent);
            static int32_t req_jsondump(string &, string &response, Agent *agent);
            static int32_t req_all_names_types(string &, string &response, Agent *agent);
            static int32_t req_command(string &, string &response, Agent *agent);
            static int32_t req_run_command(string &, string &response, Agent *agent);
            static int32_t req_add_task(string &, string &response, Agent *agent);
            static int32_t req_list_channels(string &, string &response, Agent *agent);
            static int32_t req_test_channel(string &, string &response, Agent *agent);
            static int32_t req_channel_enable(string &, string &response, Agent *agent);
            static int32_t req_channel_touch(string &, string &response, Agent *agent);
        };
    } // end of namespace Support
} // end of namespace Cosmos

#endif // COSMOSAGENT_H
