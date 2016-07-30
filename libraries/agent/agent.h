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

#include "configCosmos.h"
#include "cosmos-errno.h"
#include "stringlib.h"
#include "timelib.h"
#include "jsondef.h"
#include "sliplib.h"
#include "socketlib.h"
#include "jsonlib.h"
#include "elapsedtime.h"
#include "device/cpu/devicecpu.h"


class cosmosAgent
{
public:
//    cosmosAgent();
//    cosmosAgent(NetworkType ntype);
//    cosmosAgent(NetworkType ntype, std::string &nname);
//    cosmosAgent(NetworkType ntype, std::string &nname, std::string &aname);
//    cosmosAgent(NetworkType ntype, std::string &nname, std::string &aname, double bprd);
//    cosmosAgent(NetworkType ntype, std::string &nname, std::string &aname, double bprd, uint32_t bsize);
//    cosmosAgent(NetworkType ntype, std::string &nname, std::string &aname, double bprd, uint32_t bsize, bool mflag);
//    cosmosAgent(NetworkType ntype, std::string &nname, std::string &aname, double bprd, uint32_t bsize, bool mflag, int32_t port);
    cosmosAgent(NetworkType ntype = NetworkType::UDP, const std::string &nname = "", const std::string &aname = "", double bprd = 1., uint32_t bsize = AGENTMAXBUFFER, bool mflag = false, int32_t portnum = 0);
//    cosmosAgent(NetworkType ntype = NetworkType::UDP, std::string nname = "", std::string aname = "", double bprd = 1., uint32_t bsize = AGENTMAXBUFFER, bool mflag = false, int32_t portnum = 0);
    ~cosmosAgent();

    enum class AgentState : uint16_t
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
    #define AGENT_BLOCKING true
    //! Non-blocking Agent
    #define AGENT_NONBLOCKING false
    //! Talk followed by optional listen (sendto address)
    #define AGENT_TALK 0
    //! Listen followed by optional talk (recvfrom INADDRANY)
    #define AGENT_LISTEN 1
    //! Communicate socket (sendto followed by recvfrom)
    #define AGENT_COMMUNICATE 2
    //! Talk over multiple interfaces
    #define AGENT_JABBER 3

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

    //! Type of Agent Message. Types >128 are binary.
    enum AGENT_MESSAGE
        {
        //! All Message types
        AGENT_MESSAGE_ALL=1,
        //! Heartbeat Messages
        AGENT_MESSAGE_BEAT=2,
        //! State of Health Messages
        AGENT_MESSAGE_SOH=3,
        //! Generic Mesages
        AGENT_MESSAGE_GENERIC=4,
        AGENT_MESSAGE_TIME=5,
        AGENT_MESSAGE_LOCATION=6,
        AGENT_MESSAGE_TRACK=7,
        AGENT_MESSAGE_IMU=8,
        //! Event Messsages
        AGENT_MESSAGE_EVENT=9
        };

    //! @}

    #define MAXARGCOUNT 100

    //! \ingroup agentlib
    //! \defgroup agentlib_typedefs Agent Server and Client Library typedefs
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
        uint8_t type;
        uint16_t jlength;
        beatstruc beat;
    };

    //! Agent Request Function
    //! Format of a user supplied function to handle a given request
    typedef int32_t (cosmosAgent::*internal_request_function)(char* request_string, char* output_string);
    typedef int32_t (*external_request_function)(char* request_string, char* output_string, cosmosAgent* agent);

    //! @}
    //!
    // agent functions
    int32_t start();
//    int32_t add_request(std::string token, request_function function);
//    int32_t add_request(std::string token, request_function function, std::string description);
    int32_t add_request_internal(std::string token, internal_request_function function, std::string synopsis="", std::string description="");
    int32_t add_request(std::string token, external_request_function function, std::string synopsis="", std::string description="");
    int32_t send_request(beatstruc cbeat, std::string request, std::string &output, float waitsec);
    int32_t get_server(std::string node, std::string name, float waitsec, beatstruc *cbeat);
    std::vector<beatstruc> find_servers(float waitsec);
    beatstruc find_server(std::string node, std::string proc, float waitsec);
    beatstruc find_agent(std::string node, std::string proc);
    uint16_t running();
    int32_t set_sohstring(std::string list);
    cosmosstruc *get_cosmosstruc();
    void get_ip(char* buffer, size_t buflen);
    void get_ip_list(uint16_t port);
    int32_t unpublish();
    int32_t post(uint8_t type, std::string message);
    int32_t publish(NetworkType type, uint16_t port);
    int32_t subscribe(NetworkType type, char *address, uint16_t port);
    int32_t subscribe(NetworkType type, char *address, uint16_t port, uint32_t usectimeo);
    int32_t unsubscribe();
    int32_t poll(pollstruc &meta, std::string& message, uint8_t type, float waitsec = 1.);
    timestruc poll_time(float waitsec);
    beatstruc poll_beat(float waitsec);
    locstruc poll_location(float waitsec);
    nodestruc poll_info(float waitsec);
    imustruc poll_imu(float waitsec);
    int json_map_agentstruc(agentstruc **agent);
    std::vector<socket_channel> find_addresses(NetworkType ntype);
    int32_t shutdown();
    int32_t send(uint8_t address, std::string message);
    int32_t receive(uint8_t address, std::string &message);
    int32_t receiveAll(uint8_t address, std::string &message);

    // poll
    pollstruc metaRx;
    std::string metaHeader;


    void log(std::string log_entry);
    bool setSoh(std::string sohFields);
    cosmosstruc *cdata;
    jsonnode cjson;

    //! List of active agents
    std::vector <beatstruc> agent_list;
    // agent variables
private:

    NetworkType networkType = NetworkType::UDP;
    std::string nodeName;
    std::string agentName;
    double beatPeriod   = 1.0; // in seconds
    uint32_t bufferSize = AGENTMAXBUFFER;
    bool     multiflag   = false;
    int32_t  portNumber        = 0;

    std::string version  = "0.0";
    float    timeoutSec  = 1.0;
    bool printMessages   = true; // by default?
    bool logTime         = true; // by default
    double timeStart; // UTC starting time for this agent in MJD
    std::string hbjstring;
    std::vector<beatstruc> slist;
    //! Handle for request thread
    std::thread cthread;
    //! Handle for heartbeat thread
    std::thread hthread;
    //! Handle for message thread
    std::thread mthread;

    //! Storage for messages
    struct message
    {
        pollstruc meta;
        std::string data;
    };
    //! Ring buffer for incoming messages
    std::vector <message> message_ring;
    //! Size of message ring buffer
    int32_t message_count = 100;
    //! Current leading edge of message ring buffer
    int32_t message_position;

    //! Agent Request Entry
    //! Structure representing a single Agent request.
    struct request_entry
    {
        //! Character token for request
        std::string token;
        //! Pointer to function to call with request string as argument and returning any error
        internal_request_function ifunction;
        external_request_function efunction;
        std::string synopsis;
        std::string description;
    };

    std::vector <request_entry> reqs;

    void heartbeat_loop();
    void request_loop();
    void message_loop();

    char * parse_request(char *input);
    DeviceCpu deviceCpu_;

    static int32_t req_forward(char *request, char* response, cosmosAgent *agent);
    static int32_t req_echo(char *request, char* response, cosmosAgent *agent);
    static int32_t req_help(char *request, char* response, cosmosAgent *agent);
    static int32_t req_shutdown(char *request, char* response, cosmosAgent *agent);
    static int32_t req_idle(char *request, char* response, cosmosAgent *agent);
    static int32_t req_monitor(char *request, char* response, cosmosAgent *agent);
    static int32_t req_run(char *request, char* response, cosmosAgent *agent);
    static int32_t req_status(char *request, char* response, cosmosAgent *agent);
    static int32_t req_getvalue(char *request, char* response, cosmosAgent *agent);
    static int32_t req_setvalue(char *request, char* response, cosmosAgent *agent);
    static int32_t req_listnames(char *request, char* response, cosmosAgent *agent);
    static int32_t req_nodejson(char *request, char* response, cosmosAgent *agent);
    static int32_t req_statejson(char *request, char* response, cosmosAgent *agent);
    static int32_t req_utcstartjson(char *request, char* response, cosmosAgent *agent);
    static int32_t req_piecesjson(char *request, char* response, cosmosAgent *agent);
    static int32_t req_devgenjson(char *request, char* response, cosmosAgent *agent);
    static int32_t req_devspecjson(char *request, char* response, cosmosAgent *agent);
    static int32_t req_portsjson(char *request, char* response, cosmosAgent *agent);
    static int32_t req_targetsjson(char *request, char* response, cosmosAgent *agent);
    static int32_t req_aliasesjson(char *request, char* response, cosmosAgent *agent);

};

#endif // COSMOSAGENT_H
