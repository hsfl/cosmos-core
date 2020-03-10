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
//!     - "facesjson" - return the JSON representing the contents of faces.ini.
//!     - "vertexsjson" - return the JSON representing the contents of vertexs.ini.
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
//! with Cosmos::COSMOS_SLEEP. Upon exiting from this loop, you should call Cosmos::Agent::shutdown.

#include "support/configCosmos.h"
#include "support/cosmos-errno.h"
#include "support/stringlib.h"
#include "support/timelib.h"
#include "support/jsondef.h"
#include "support/sliplib.h"
#include "support/socketlib.h"
#include "support/jsonlib.h"
#include "support/elapsedtime.h"
#include "device/cpu/devicecpu.h"

using std::string;
using std::vector;

namespace Cosmos
{
    namespace Support
    {
        class Agent
        {
        public:
            //    Agent(NetworkType ntype, const string &nname = "", const string &aname = "", double bprd = 1., uint32_t bsize = AGENTMAXBUFFER, bool mflag = false, int32_t portnum = 0);
            Agent(const string &nname = "", const string &aname = "", double bprd = 1., uint32_t bsize = AGENTMAXBUFFER, bool mflag = false, int32_t portnum = 0, NetworkType ntype = NetworkType::UDP,  uint16_t dlevel = 1);
            ~Agent();

            //! State of Health element vector
            vector<jsonentry*> sohtable;

            enum class State : uint16_t
                {
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
                DEBUG
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
            //! Maximum AGENT server list count
#define AGENTMAXLIST 500
            //! Maximum AGENT heartbeat size
#define AGENTMAXHEARTBEAT 200
            //! Default AGENT socket RCVTIMEO (100 msec)
#define AGENTRCVTIMEO 100000
            //! Default minium heartbeat period (10 msec)
#define AGENT_HEARTBEAT_PERIOD_MIN 0.01

            //! Default size of message ring buffer
#define MESSAGE_RING_SIZE 100

            //! Type of Agent Message. Types > 127 are binary.
            enum class AgentMessage : uint8_t
                {
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

            enum class Where : size_t
                {
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
            typedef int32_t (Agent::*internal_request_function)(char* request_string, char* output_string);
            typedef int32_t (*external_request_function)(char* request_string, char* output_string, Agent* agent);

            //! @}
            //!
            // agent functions
            int32_t start();
            int32_t start_active_loop();
            int32_t finish_active_loop();
            //    int32_t add_request(string token, request_function function);
            //    int32_t add_request(string token, request_function function, string description);
            int32_t add_request_internal(string token, internal_request_function function, string synopsis="", string description="");
            int32_t add_request(string token, external_request_function function, string synopsis="", string description="");
            int32_t send_request(beatstruc cbeat, string request, string &output, float waitsec=5.);
            int32_t send_request_jsonnode(beatstruc cbeat, jsonnode &jnode, float waitsec=5.);
            int32_t get_server(string node, string name, float waitsec, beatstruc *cbeat);
            vector<beatstruc> find_servers(float waitsec);
            beatstruc find_server(string node, string proc, float waitsec);
            beatstruc find_agent(string agent, string node="");
            uint16_t running();
            int32_t wait(State state=State::RUN, float waitsec=10.);
            int32_t last_error();
            int32_t set_sohstring(string list);
            cosmosstruc *get_cosmosstruc();
            void get_ip(char* buffer, size_t buflen);
            void get_ip_list(uint16_t port);
            int32_t unpublish();
            int32_t post(messstruc mess);
            int32_t post(AgentMessage type, string message="");
            int32_t post(AgentMessage type, vector <uint8_t> message);
            int32_t post_beat();
            int32_t publish(NetworkType type, uint16_t port);
            int32_t subscribe(NetworkType type, const char *address, uint16_t port);
            int32_t subscribe(NetworkType type, const char *address, uint16_t port, uint32_t usectimeo);
            int32_t unsubscribe();
            //    int32_t poll(pollstruc &meta, string &message, uint8_t type, float waitsec = 1.);
            //    int32_t poll(pollstruc &meta, vector <uint8_t> &message, uint8_t type, float waitsec = 1.);
            int32_t poll(messstruc &mess, AgentMessage type, float waitsec = 1.);
            int32_t readring(messstruc &message, AgentMessage type = Agent::AgentMessage::ALL, float waitsec = 1., Where where=Where::HEAD);
            int32_t resizering(size_t newsize);
            int32_t clearring();
            //    timestruc poll_time(float waitsec);
            //    beatstruc poll_beat(float waitsec);
            //    locstruc poll_location(float waitsec);
            //    nodestruc poll_info(float waitsec);
            //    imustruc poll_imu(float waitsec);
            int json_map_agentstruc(agentstruc **agent);
            vector<socket_channel> find_addresses(NetworkType ntype);
            int32_t shutdown();
            int32_t send(uint8_t address, string message);
            int32_t receive(uint8_t address, string &message);
            int32_t receiveAll(uint8_t address, string &message);
            std::string getNode();
            std::string getAgent();
            int32_t getJson(string node, jsonnode &jnode);

            int32_t set_agent_time_producer(double (*source)());
            int32_t get_agent_time(double &agent_time, double &epsilon, string agent, string node="");

            // poll
            pollstruc metaRx;
            string metaHeader;


            void log(string log_entry);
            bool setSoh(string sohFields);
            cosmosstruc *cinfo;

            //! List of active agents
            vector <beatstruc> agent_list;
            //! List of active Nodes
            vector <jsonnode> node_list;

            //! Ring buffer for incoming messages
            vector <messstruc> message_ring;
            //! Last message placed in message ring buffer
            size_t message_head = MESSAGE_RING_SIZE;
            //! Last message rad in message ring buffer
            size_t message_tail = MESSAGE_RING_SIZE;

            //! Flag for level of debugging, keep it public so that it can be controlled from the outside
            uint16_t debug_level = 0;
            FILE *get_debug_fd(double mjd=0.);
            int32_t close_debug_fd();

            // agent variables
            string nodeName;
            string agentName;

        protected:
        private:

            NetworkType networkType = NetworkType::UDP;
            double activeTimeout = 0.0; // in MJD
            uint32_t bufferSize = AGENTMAXBUFFER;
            bool multiflag = false;
            int32_t portNumber = 0;
            FILE *debug_fd = nullptr;
            string debug_pathName;

            string version = "0.0";
            float timeoutSec = 2.0;
            bool printMessages = true; // by default?
            bool logTime = true; // by default
            double timeStart; // UTC starting time for this agent in MJD
            string hbjstring;
            vector<beatstruc> slist;
            //! Handle for request thread
            thread cthread;
            //! Handle for heartbeat thread
            thread hthread;
            //! Handle for message thread
            thread mthread;
            //! Last error
            int32_t error_value;

            //! Function in which we generate our time, for the mjd request.
            double (*agent_time_producer)() = currentmjd;

            //! Agent Request Entry
            //! Structure representing a single Agent request.
            struct request_entry
            {
                //! Character token for request
                string token;
                //! Pointer to function to call with request string as argument and returning any error
                internal_request_function ifunction;
                external_request_function efunction;
                string synopsis;
                string description;
            };

            vector <request_entry> reqs;

            void heartbeat_loop();
            void request_loop();
            int32_t process_request(string &bufferin, string &bufferout);
            void message_loop();

            char * parse_request(char *input);
            DeviceCpu deviceCpu_;

            static int32_t req_forward(char *request, char* response, Agent *agent);
            static int32_t req_echo(char *request, char* response, Agent *agent);
            static int32_t req_help(char *request, char* response, Agent *agent);
            static int32_t req_help_json(char *request, char* response, Agent *agent);
            static int32_t req_shutdown(char *request, char* response, Agent *agent);
            static int32_t req_idle(char *request, char* response, Agent *agent);
            static int32_t req_init(char *request, char* response, Agent *agent);
            static int32_t req_monitor(char *request, char* response, Agent *agent);
            static int32_t req_run(char *request, char* response, Agent *agent);
            static int32_t req_status(char *request, char* response, Agent *agent);
            static int32_t req_debug_level(char *request, char* response, Agent *agent);
            static int32_t req_getvalue(char *request, char* response, Agent *agent);
            static int32_t req_setvalue(char *request, char* response, Agent *agent);
            static int32_t req_listnames(char *request, char* response, Agent *agent);
            static int32_t req_nodejson(char *request, char* response, Agent *agent);
            static int32_t req_statejson(char *request, char* response, Agent *agent);
            static int32_t req_utcstartjson(char *request, char* response, Agent *agent);
            static int32_t req_piecesjson(char *request, char* response, Agent *agent);
            static int32_t req_vertexsjson(char *request, char* response, Agent *agent);
            static int32_t req_facesjson(char *request, char* response, Agent *agent);
            static int32_t req_devgenjson(char *request, char* response, Agent *agent);
            static int32_t req_devspecjson(char *request, char* response, Agent *agent);
            static int32_t req_portsjson(char *request, char* response, Agent *agent);
            static int32_t req_targetsjson(char *request, char* response, Agent *agent);
            static int32_t req_aliasesjson(char *request, char* response, Agent *agent);
            static int32_t req_heartbeat(char *request, char* response, Agent *agent);
            static int32_t req_mjd(char *request, char* response, Agent *agent);
            static int32_t req_soh(char *, char* response, Agent *agent);
        };
    } // end of namespace Support
} // end of namespace Cosmos

#endif // COSMOSAGENT_H
