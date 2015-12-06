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

#ifndef _AGENTLIB_H
#define _AGENTLIB_H 1

/*! \file agentlib.h
*	\brief Agent Server and Client header file
*
*/

//! \ingroup support
//! \defgroup agentlib Agent Server and Client Library
//! %Agent Server and Client.
//!
//! Agents are persistent programs that provide the system framework for any
//! COSMOS implementation. They are similar to UNIX Daemons or Windows Services in that
//! they run continuously until commanded to stop, and serve a constant function, either
//! automatically or on demand. Agents are started with a call to ::agent_setup_server.
//!
//! Clients are COSMOS aware programs that are started with a call to ::agent_setup_client. This establishes
//! their connection to the \ref json_namespace, thereby allowing them to communicate with Agents.
//!
//! Agents are implemented as at least three threads of execution. These threads provide the following services:
//!
//! - "Heartbeat": Delivered to a system specified Multicast (::AGENTMCAST), or Broadcast address as a \ref json_packet,
//! at some regular interval.
//!     - Provides the time, the name of the Node and %Agent, the IP address and
//! Port at which it can be reached, the size of its communication buffer, and the jitter in heartbeat period.
//!
//! - "Requests": available at the IP Port reported in the Heartbeat.
//! Requests are received as plain text commands and arguments. They are processed and
//! any response is sent back. The response, even if empty, always ends with [OK], if understood,
//! or [NOK] if not. Requests and their responses must be less than the size of the communications
//! buffer. Built in requests include:
//!     - "help" - list available requests for this %Agent.
//!     - "shutdown" - causes the %Agent to stop what it is doing and exit.
//!     - "status" - causes the agent to dump any \ref jsonlib variables it is monitoring.
//!     - "getvalue {\"json_name_000\"[,\"json_name_001\"]}" - requests the %Agent to return the values
//! of the indicated JSON names.
//!     - "setvalue {\"json_name_000\":value0[,\"json_name_001\":value1]}" - requests the %Agent to set
//! the values of the indicated JSON names.
//!     - "echo utc crc nbytes bytes" - requests the %Agent to echo the local time the request was received,
//! the CRC calculated for the bytes, and the bytes themselves.
//!     - Additional requests can be added using ::agent_add_request, that tie together user defined
//! functions with user defined ASCII strings.
//! - "Main Loop": The main activity of the %Agent. Once you have called ::agent_setup_server and performed any
//! other initializations necessary, you should enter a continuous loop protected by ::agent_running. Upon exiting from
//! this loop, you should call ::agent_shutdown_server.

#include "configCosmos.h"
#include "agentdef.h"

#include "cosmos-errno.h"
#include "stringlib.h"
#include "timelib.h"
#include "jsondef.h"
#include "sliplib.h"
#include "socketlib.h"
#include "jsonlib.h"
#include "elapsedtime.hpp"

#include <cstring>
#include <errno.h>

#if !defined(COSMOS_WIN_OS)
#include <sys/select.h>
#endif

//using namespace std;  // don't use this as it may cause conflicts with other namespaces

//! \ingroup agentlib
//! \defgroup agentlib_functions %Agent Server and Client Library functions
//! @{

int32_t agent_add_request(cosmosstruc *cdata, string token, agent_request_function function);
int32_t agent_add_request(cosmosstruc *cdata, string token, agent_request_function function, string description);
int32_t agent_add_request(cosmosstruc *cdata, string token, agent_request_function function, string synopsis, string description);
int32_t agent_start(cosmosstruc *cdata);
int32_t agent_send_request(cosmosstruc *cdata, beatstruc cbeat, string request, char* output, uint32_t clen, float waitsec);
int32_t agent_get_server(cosmosstruc *cdata, string node, string name, float waitsec, beatstruc *cbeat);
vector<beatstruc> agent_find_servers(cosmosstruc *cdata, float waitsec);
beatstruc agent_find_server(cosmosstruc* cdata, string node, string proc, float waitsec);
uint16_t agent_running(cosmosstruc *cdata);
int32_t agent_req_forward(char *request, char* response, void *root);
int32_t agent_req_echo(char *request, char* response, void *root);
int32_t agent_req_help(char *request, char* response, void *root);
int32_t agent_req_shutdown(char *request, char* response, void *root);
int32_t agent_req_idle(char *request, char* response, void *root);
int32_t agent_req_run(char *request, char* response, void *root);
int32_t agent_req_status(char *request, char* response, void *root);
int32_t agent_req_getvalue(char *request, char* response, void *root);
int32_t agent_req_setvalue(char *request, char* response, void *root);
int32_t agent_req_listnames(char *request, char* response, void *root);

// agent setup server
cosmosstruc* agent_setup_server(string nodename, string agentname);
cosmosstruc* agent_setup_server(int ntype, string node, string name, double bprd, int32_t port, uint32_t bsize);
cosmosstruc* agent_setup_server(int ntype, string node, string name, double bprd, int32_t port, uint32_t bsize, bool multiflag);
cosmosstruc* agent_setup_server(cosmosstruc* cdata, string name, double bprd, int32_t port, uint32_t bsize, bool multiflag, float timeoutSec);

// agent setup client
cosmosstruc* agent_setup_client(int ntype, string node);
cosmosstruc* agent_setup_client(int ntype, string node, uint32_t usectimeo);

int32_t agent_shutdown_server(cosmosstruc *cdata);
int32_t agent_shutdown_client(cosmosstruc *cdata);
int32_t agent_set_sohstring(cosmosstruc *cdata, string list);
cosmosstruc *agent_get_cosmosstruc(cosmosstruc *cdata);
void agent_get_ip(cosmosstruc *cdata, char* buffer, size_t buflen);
void agent_get_ip_list(cosmosstruc *cdata, uint16_t port);
int32_t agent_unpublish(cosmosstruc *cdata);
int32_t agent_post(cosmosstruc *cdata, uint8_t type, string message);
int32_t agent_publish(cosmosstruc *cdata, uint16_t type, uint16_t port);
int32_t agent_subscribe(cosmosstruc *cdata, uint16_t type, char *address, uint16_t port);
int32_t agent_subscribe(cosmosstruc *cdata, uint16_t type, char *address, uint16_t port, uint32_t usectimeo);
int32_t agent_unsubscribe(cosmosstruc *cdata);
int32_t agent_poll(cosmosstruc *cdata, pollstruc &meta, string& message, uint8_t type, float waitsec);
timestruc agent_poll_time(cosmosstruc *cdata, float waitsec);
beatstruc agent_poll_beat(cosmosstruc *cdata, float waitsec);
locstruc agent_poll_location(cosmosstruc *cdata, float waitsec);
nodestruc agent_poll_info(cosmosstruc *cdata, float waitsec);
imustruc agent_poll_imu(cosmosstruc *cdata, float waitsec);
int json_map_agentstruc(cosmosstruc *cdata, agentstruc **agent);
//int32_t agent_open_socket(socket_channel *channel, uint16_t ntype, const char *address, uint16_t port, uint16_t direction, bool blocking, uint32_t usectimeo);
vector<socket_channel> agent_find_addresses(uint16_t ntype);


// TODO: new agent class that will conventrate all of the above functions
class Agent {

public:
    Agent();
    Agent(string nodename, string agentname);

    // agent functions
    bool setupServer();
    bool setupServer(string nodename, string agentname);
    bool setupClient(string nodename);

    beatstruc findServer(string servername);
    beatstruc find(string servername);

    uint16_t isRunning();
    int32_t sendRequest(beatstruc beat, string request, string &response);
    int32_t shutdownServer();

    int32_t post(uint8_t type, string message);
    int32_t poll(uint8_t type, string &message);
    int32_t poll(pollstruc &meta, string& message, uint8_t type, float waitsec);
    int32_t pollParse(string &message);

    // agent variables
    cosmosstruc *cdata;
    string nodeName;
    string name;
    string version       = "0.0";
    double beat_period   = 1.0; // in seconds
    int32_t  port        = 0;
    uint32_t buffer_size = AGENTMAXBUFFER;
    bool     multiflag   = false;
    float    timeoutSec  = 1.0;
    bool printMessages   = true; // by default?

    // poll
    pollstruc metaRx;
    string metaHeader;


    int32_t send(uint8_t address, string message);
    int32_t receive(uint8_t address, string &message);
};


//! @}


#endif
