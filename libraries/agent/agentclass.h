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
            Agent(uint8_t placeholder);
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
            int32_t start();
            int32_t start_active_loop();
            int32_t finish_active_loop();
            int32_t add_request(string token, external_request_function function, string synopsis="", string description="");
//            int32_t add_request(string token, simple_request_function function, string synopsis="", string description="");
//            int32_t add_request(string token, no_arg_request_function function, string synopsis="", string description="");
            int32_t send_request(beatstruc cbeat, string request, string &output, float waitsec=5., double delay_send = 0.0, double delay_receive = 0.0);
            int32_t send_request_jsonnode(beatstruc cbeat, jsonnode &jnode, float waitsec=5.);
            int32_t get_agent(string node, string agent, double waitsec, beatstruc &cbeat);
            int32_t check_agent(string node, string agent, double waitsec);
            beatstruc find_agent(string node, string agent, double waitsec=0.);
            vector<beatstruc> find_agents(double waitsec=0.);
            uint16_t running();
            int32_t wait(State state=State::RUN, double waitsec=10.);
            int32_t last_error();
            int32_t set_sohstring(string list);
            int32_t set_sohstring(vector<string> list);
            int32_t set_sohstring2(vector<string> list);
            int32_t set_fullsohstring(string list);
            cosmosstruc *get_cosmosstruc();
            void get_ip(char* buffer, size_t buflen);
            void get_ip_list(uint16_t port);
            int32_t unpublish();
            int32_t post(messstruc mess);
            int32_t post(AgentMessage type, string message="");
            int32_t post(AgentMessage type, vector <uint8_t> message);
            int32_t post_beat();
            int32_t post_soh();
            int32_t publish(NetworkType type, uint16_t port);
            int32_t subscribe(NetworkType type, const char *address, uint16_t port);
            int32_t subscribe(NetworkType type, const char *address, uint16_t port, uint32_t usectimeo);
            int32_t unsubscribe();
            //    int32_t poll(pollstruc &meta, string &message, uint8_t type, float waitsec = 1.);
            //    int32_t poll(pollstruc &meta, vector <uint8_t> &message, uint8_t type, float waitsec = 1.);
            int32_t poll(messstruc &mess, AgentMessage type, float waitsec = 1.);
            int32_t readring(messstruc &message, AgentMessage type = Agent::AgentMessage::ALL, float waitsec = 1., Where where=Where::TAIL, string proc="", string node="");
            int32_t readring(messstruc &message, string realm="", string node="", AgentMessage type = Agent::AgentMessage::ALL, float waitsec = 1., Where where=Where::TAIL);
            int32_t readring(messstruc &message, vector<string> realm, AgentMessage type = Agent::AgentMessage::ALL, float waitsec = 1., Where where=Where::TAIL);
            int32_t parsering(AgentMessage type = Agent::AgentMessage::ALL, float waitsec=1., Where where=Where::HEAD, string proc="", string node="");
            int32_t resizering(size_t newsize);
            int32_t clearring();
            //    timestruc poll_time(float waitsec);
            //    beatstruc poll_beat(float waitsec);
            //    Convert::locstruc poll_location(float waitsec);
            //    nodestruc poll_info(float waitsec);
            //    imustruc poll_imu(float waitsec);
            int json_map_agentstruc(agentstruc **agent);
            vector<socket_channel> find_addresses(NetworkType ntype);
            int32_t shutdown();
            int32_t send(uint8_t address, string message);
            int32_t receive(uint8_t address, string &message);
            int32_t receiveAll(uint8_t address, string &message);
            string getNode();
            string getAgent();
            int32_t getJson(string node, jsonnode &jnode);

            int32_t set_agent_time_producer(double (*source)());
            int32_t get_agent_time(double &agent_time, double &epsilon, double &delta, string agent, string node="any", double wait_sec=2.);
            // general functionality for artemis
            int32_t set_activity_period(double period);

            int32_t add_device(string name, DeviceType type, devicestruc **device);
            int32_t device_property_name(string device, string property, string& name);

            int32_t send_request_getvalue(beatstruc agent, vector<string> names, Json::Object &jobj);
            int32_t create_device_value_alias(string devicename, string propertyname, string alias);
            int32_t create_alias(string cosmosname, string alias);

            int32_t set_value(string jsonname, double value);
            double get_value(string jsonname);
            int32_t get_device_values(string device, vector<string>props, string& json);
            int32_t get_values(vector<string> names, string& json);

            double get_timeStart();



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
//            vector <messstruc> message_ring;
            deque <messstruc> message_queue;
            //! Last message placed in message ring buffer
            size_t message_head = MESSAGE_RING_SIZE;
            //! Last message rad in message ring buffer
            size_t message_tail = MESSAGE_RING_SIZE;

            //! Flag for level of debugging, keep it public so that it can be controlled from the outside
            int32_t set_debug_level(uint16_t level);
            int32_t get_debug_level();
            FILE *get_debug_fd(double mjd=0.);
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

            int32_t process_request(string &bufferin, string &bufferout, bool send_response=true);

            int32_t set_verification(uint32_t verification);
            uint32_t get_verification();
            int32_t check_verification(uint32_t verification);
            int32_t channel_set_comm_priority(uint8_t number);
            int32_t channel_count();
            int32_t channel_push(PacketComm &packet);
            int32_t channel_push(string name, PacketComm &packet);
            int32_t channel_push(uint8_t number, PacketComm& packet);
            int32_t channel_push(string name, vector<PacketComm>& packets);
            int32_t channel_push(uint8_t number, vector<PacketComm>& packets);
            int32_t push_response(string name, uint8_t sourceid, uint8_t dest, uint32_t id, string response="");
            int32_t push_response(uint8_t number, uint8_t sourceid, uint8_t dest, uint32_t id, string response="");
            int32_t push_response(string name, uint8_t sourceid, uint8_t dest, uint32_t id, vector<uint8_t> response);
            int32_t push_response(uint8_t number, uint8_t sourceid, uint8_t dest, uint32_t id, vector<uint8_t> response);
            int32_t push_hardware_response(PacketComm::TypeId type, string name, uint8_t dest, uint8_t unit, uint8_t command, vector<uint8_t> response);
            int32_t push_hardware_response(PacketComm::TypeId type, uint8_t number, uint8_t dest, uint8_t unit, uint8_t command, vector<uint8_t> response);
            int32_t channel_pull(string name, PacketComm& packet);
            int32_t channel_pull(uint8_t number, PacketComm& packet);
            int32_t monitor_unwrapped(string name, PacketComm& packet, string extra="");
            int32_t monitor_unwrapped(uint8_t number, PacketComm& packet, string extra="");
            int32_t monitor_unpacketized(string name, PacketComm& packet, string extra="");
            int32_t monitor_unpacketized(uint8_t number, PacketComm& packet, string extra="");
            int32_t init_channels(uint32_t verification=0x352e);
            int32_t channel_add(string name, uint16_t datasize=0, uint16_t rawsize=0, float byte_rate=0., uint16_t maximum=0);
            int32_t channel_update(string name, uint16_t datasize=0, uint16_t rawsize=0, float byte_rate=0., uint16_t maximum=0);
            int32_t channel_update(uint8_t number, uint16_t datasize=0, uint16_t rawsize=0, float byte_rate=0., uint16_t maximum=0);
            //! Number of packets currently in the channel
            int32_t channel_size(string name);
            //! Number of packets currently in the channel
            int32_t channel_size(uint8_t number);
            float channel_speed(string name);
            float channel_speed(uint8_t number);
            double channel_age(string name);
            double channel_age(uint8_t number);
            double channel_wakeup_timer(string name, double value=0.);
            double channel_wakeup_timer(uint8_t number, double value=0.);
            size_t channel_bytes(string name);
            size_t channel_bytes(uint8_t number);
            size_t channel_level(string name);
            size_t channel_level(uint8_t number);
            uint32_t channel_packets(string name);
            uint32_t channel_packets(uint8_t number);
            double channel_touch(string name, double seconds=0.);
            double channel_touch(uint8_t number, double seconds=0.);
            ssize_t channel_increment(string name, size_t bytes, uint32_t packets=1);
            ssize_t channel_increment(uint8_t number, size_t bytes, uint32_t packets=1);
//            ssize_t channel_decrement(string name, size_t bytes, uint32_t packets=1);
//            ssize_t channel_decrement(uint8_t number, size_t bytes, uint32_t packets=1);
            int32_t channel_clear(string name);
            int32_t channel_clear(uint8_t number);
            int32_t channel_number(string name);
            string channel_name(uint8_t number);
            //! Max data size of channel
            int32_t channel_datasize(string name);
            //! Max data size of channel
            int32_t channel_datasize(uint8_t number);
            //! Max packet size of channel
            int32_t channel_rawsize(string name);
            //! Max packet size of channel
            int32_t channel_rawsize(uint8_t number);
            int32_t channel_maximum(string name);
            int32_t channel_maximum(uint8_t number);
            int32_t channel_teststart(string name, string radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total);
            int32_t channel_teststart(uint8_t number, uint8_t nradio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total);
            int32_t channel_teststop(string name, float seconds=5.);
            int32_t channel_teststop(uint8_t number, float seconds=5.);
            int32_t channel_enabled(string name);
            int32_t channel_enabled(uint8_t number);
            int32_t channel_enable(string name, int8_t value);
            int32_t channel_enable(uint8_t number, int8_t value);
//            int32_t channel_disable(string name);
//            int32_t channel_disable(uint8_t number);

            int32_t task_add(string command, string source="");
            int32_t task_del(uint32_t deci);
            int32_t task_iretn(uint16_t number);
            uint32_t task_deci(uint16_t number);
            double task_startmjd(uint16_t number);
            uint8_t task_state(uint16_t number);
            string task_command(uint16_t number);
            string task_path(uint16_t number);
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
