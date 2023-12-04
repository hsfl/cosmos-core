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

/*! \file agentclass.cpp
    \brief Agent support functions
*/

#include "agent/agentclass.h"
#include "support/cosmos-errno.h"
#include "support/stringlib.h"
#include "support/timelib.h"
//#include "support/jsondef.h"
#include "support/socketlib.h"
#include "support/elapsedtime.h"
#if defined (COSMOS_MAC_OS)
#include <net/if.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#endif

// Used to mark unused variables as known
#ifndef UNUSED_VARIABLE_LOCALDEF
#define UNUSED_VARIABLE_LOCALDEF(x) (void)(x)
#endif // UNUSED_VARIABLE_LOCALDEF

#ifndef _SIZEOF_ADDR_IFREQ
#define _SIZEOF_ADDR_IFREQ sizeof
#endif

#include<iostream>

namespace Cosmos
{
    namespace Support
    {
        //! \ingroup agentclass
        //! \ingroup agentclass
        //! \defgroup agentclass_functions Agent Server and Client functions
        //! @{
        
        //! Creates a skeleton agent with no setup
        //! \param placeholder Does nothing but provide a different function signature for overloading
        Agent::Agent(uint8_t placeholder) {}

        //! Add COSMOS awareness.
        //! Sets up minimum framework for COSMOS awareness. The minimum call makes a nodeless client, setting up the
        //! message ring buffer thread, and a main thread of execution. Additional parameters are related to making
        //! the program a true Agent by tieing it to a node, and starting the request and heartbeat threads.
        //! \param ntype Transport Layer protocol to be used, taken from ::NetworkType. Defaults to UDP Broadcast.
        //! \param realm_name Realm name. Defaults to empty.
        //! \param node_name Node name. Defaults to empty.
        //! \param agent_name Agent name. Defaults to empty. If this is defined, the full Agent code will be started.
        //! \param bprd Period, in seconds, for heartbeat. Defaults to 1.
        //! \param bsize Size of interagent communication buffer. Defaults to ::AGENTMAXBUFFER.
        //! \param mflag Boolean controlling whether or not multiple instances of the same Agent can start. If true, then Agent names
        //! will have an index number appended (eg: myname_001). If false, agent will listen for 5 seconds and terminate if it senses
        //! the Agent already running.
        //! \param portnum The network port to listen on for requests. Defaults to 0 whereupon it will use whatever th OS assigns.
        //! \param dlevel debug level. Defaults to 1 so that if there is an error the user can immediately see it. also initialized in the namespace variables
        Agent::Agent(string realm_name,
                     string node_name,
                     string agent_name,
                     double bprd,
                     uint32_t bsize,
                     bool mflag,
                     int32_t portnum,
                     NetworkType ntype,
                     uint16_t dlevel)
        {
            int32_t iretn = 0;
            uptime.reset();

            // Set up node: use hostname if it's empty.
            if (node_name.empty())
            {
                char hostname[60];
                gethostname(hostname, sizeof (hostname));
                node_name = hostname;
            }

            // Initialize logging
            debug_level = dlevel;
            debug_log.Set(dlevel, true,  data_base_path(node_name, "temp", agent_name), 1800., "debug");

            tasks.Start();
            debug_log.Printf("Started Tasks\n");

            // Initialize COSMOS data space
            cinfo = json_init(node_name);

            if (cinfo == nullptr) {
                error_value = AGENT_ERROR_JSON_CREATE;
                debug_log.Printf("Failed to initialize Namespace\n");
                shutdown();
                return;
            }
            debug_log.Printf("Initialized Namespace\n");

            cinfo->agent0.stateflag = static_cast<uint16_t>(State::INIT);

            // Establish subscribe channel
            iretn = subscribe(NetworkType::UDP, AGENTLOOPBACK, AGENTSENDPORT, 1000);
            if (iretn) {
                error_value = iretn;
                debug_log.Printf("Failed to open Subscribe channel\n");
                shutdown();
                return;
            }
            debug_log.Printf("Opened Subscribe channel\n");

            if ((iretn=json_setup_node(node_name, cinfo)) != 0) {
                error_value = iretn;
                debug_log.Printf("Failed to set up Namespace\n");
                shutdown();
                return;
            }
            debug_log.Printf("Set Up Namespace\n");

            cinfo->agent0.client = 1;
            cinfo->node.utc = 0.;
            cinfo->agent0.beat.node = cinfo->node.name;

            // Establish publish channel
            cinfo->agent0.beat.ntype = ntype;
            iretn = publish(cinfo->agent0.beat.ntype, AGENTSENDPORT);
            if (iretn) {
                error_value = iretn;
                debug_log.Printf("Failed to open publish channel\n");
                shutdown();
                return;
            }
            debug_log.Printf("Opened Publish channel\n");

            // Set Realm:
            // if empty, use "all"
            // if Node is not in Realm, add it
            iretn = load_node_ids(cinfo, realm_name);
            if (iretn < 0)
            {
                error_value = iretn;
                debug_log.Printf("Failed to find Realm %s\n", realm_name.c_str());
                shutdown();
                return;
            }
            for (auto id : cinfo->realm.node_ids)
            {
                debug_log.Printf("Node Index: %u Name: %s\n", static_cast<uint8_t>(id.second), id.first.c_str());
            }

            // Find Node in Realm
            iretn = lookup_node_id(cinfo, cinfo->node.name);
            if (iretn < 0)
            {
                error_value = iretn;
                debug_log.Printf("Failed to find Node %s in Realm %s\n", cinfo->node.name.c_str(), realm_name.c_str());
                shutdown();
                return;
            }
            else if (iretn == 0)
            {
                iretn = add_node_id(cinfo, cinfo->node.name);
            }
            nodeId = iretn;

            // Start message listening thread
            mthread = thread([=] { message_loop(); });
            secondsleep(.1);

            // Return if all we are doing is setting up client.
            if (agent_name.empty())
            {
                cinfo->agent0.beat.proc = "";
                cinfo->agent0.stateflag = static_cast <uint16_t>(Agent::State::RUN);
                return;
            }

            //        if (strlen(cinfo->node.name)>COSMOS_MAX_NAME || agent_name.length()>COSMOS_MAX_NAME) {
            if (cinfo->node.name.size() > COSMOS_MAX_NAME || agent_name.length()>COSMOS_MAX_NAME) {
                error_value = JSON_ERROR_NAME_LENGTH;
                debug_log.Printf("Node or Agent name too large\n");
                shutdown();
                return;
            }


            // If not Multi, check if this Agent is already running
            char tname[COSMOS_MAX_NAME+1];
            if (!mflag) {
                if (check_agent(cinfo->node.name, agent_name, timeoutSec)) {
                    error_value = AGENT_ERROR_SERVER_RUNNING;
                    debug_log.Printf("Agent is already running\n");
                    shutdown();
                    return;
                }
                strcpy(tname,agent_name.c_str());
            } else {
                // then there is an agent running with the given name, so let's make the name unique
                //            if (strlen(cinfo->node.name)>COSMOS_MAX_NAME-4 || agent_name.size()>COSMOS_MAX_NAME-4) {
                if (cinfo->node.name.size()>COSMOS_MAX_NAME-4 || agent_name.size()>COSMOS_MAX_NAME-4) {
                    error_value = JSON_ERROR_NAME_LENGTH;
                    debug_log.Printf("Node or Agent name too large\n");
                    shutdown();
                    return;
                }

                uint32_t i=0;
                do
                {
                    sprintf(tname,"%s_%03d",agent_name.c_str(),i);
                    if (!check_agent(cinfo->node.name, tname, timeoutSec))
                    {
                        break;
                    }
                } while (++i<100);
            }

            // Initialize important server variables
            cinfo->agent0.beat.node = cinfo->node.name;
            cinfo->agent0.beat.proc = tname;
//            cinfo->agent0.name = cinfo->agent0.beat.proc;
            cinfo->agent0.name = cinfo->agent0.beat.proc;
            timeStart = currentmjd();

            if (debug_level>2) {
                debug_log.Printf("------------------------------------------------------\n");
                debug_log.Printf("COSMOS AGENT '%s' on node '%s'\n", agent_name.c_str(), cinfo->node.name.c_str());
                debug_log.Printf("Version %s built on %s %s\n", version.c_str(),  __DATE__, __TIME__);
                debug_log.Printf("Agent started at %s\n", mjdToGregorian(timeStart).c_str());
                debug_log.Printf("Debug level %u\n", debug_level);
                debug_log.Printf("------------------------------------------------------\n");
            }

            if (bprd >= AGENT_HEARTBEAT_PERIOD_MIN) {
                cinfo->agent0.beat.bprd = bprd;
            } else {
                cinfo->agent0.beat.bprd = 0.;
            }
            cinfo->agent0.stateflag = static_cast<uint16_t>(State::INIT);
            cinfo->agent0.beat.port = static_cast<uint16_t>(portnum);
            cinfo->agent0.beat.bsz = (bsize<=AGENTMAXBUFFER-4?bsize:AGENTMAXBUFFER-4);

#ifdef COSMOS_WIN_BUILD_MSVC
            cinfo->agent0.pid = _getpid();
#else
            cinfo->agent0.pid = getpid();
#endif
            cinfo->agent0.aprd = 1.;
            cinfo->agent0.beat.user = "cosmos";

            // Start the heartbeat and request threads running
            //    iretn = start();
            hthread = thread([=] { heartbeat_loop(); });
            cthread = thread([=] { request_loop(); });
            if (!hthread.joinable() || !cthread.joinable()) {
                // TODO: create error value
                //error_value = iretn;
                debug_log.Printf("Failed to start Heartbeat and/or Request Loops\n");
                shutdown();
                return;
            }

            //! Set up initial requests
            add_request("help",req_help,"","list of available requests for this agent");
            add_request("help_json",req_help_json,"","list of available requests for this agent (but in json)");
            add_request("shutdown",req_shutdown,"","request to shutdown this agent");
            add_request("idle",req_idle,"","request to transition this agent to idle state");
            add_request("init",req_init,"","request to transition this agent to init state");
            add_request("monitor",req_monitor,"","request to transition this agent to monitor state");
            add_request("reset",req_reset,"","request to transition this agent to reset state");
            add_request("run",req_run,"","request to transition this agent to run state");
            add_request("status",req_status,"","request the status of this agent");
            add_request("debug_level",req_debug_level,"{\"name1\",\"name2\",...}","get/set debug_level of agent");
            add_request("getvalue",req_getvalue,"{\"name1\",\"name2\",...}","get specified value(s) from agent");
            add_request("get_value",req_get_value,"[{] \"name1\",\"name2\",... [}]","get specified value(s) from agent (Namespace 2.0)");
            add_request("get_state",req_get_state,"[{] \"name1\",\"name2\",... [}]","get current state value(s) from agent");
            add_request("get_time",req_get_time,"","return the current time of the agent");
            add_request("get_position",req_get_position,"","return the current perifocal position of the agent");
            add_request("get_position_data",req_get_position_data,"","return the current perifocal position of the agent");
            add_request("setvalue",req_setvalue,"{\"name1\":value},{\"name2\":value},...","set specified value(s) in agent");
            add_request("set_value",req_set_value,"{\"name1\":value} [,] {\"name2\":value} [,] ...","set specified value(s) in agent (Namespace 2.0)");
            add_request("listnames",req_listnames,"","list the Namespace of the agent");
            add_request("forward",req_forward,"nbytes packet","Broadcast JSON packet to the default SEND port on local network");
            add_request("echo",req_echo,"utc crc nbytes bytes","echo array of nbytes bytes, sent at time utc, with CRC crc.");
            add_request("nodejson",req_nodejson,"","return description JSON for Node");
            add_request("statejson",req_statejson,"","return description JSON for State vector");
            //        add_request("utcstartjson",req_utcstartjson,"","return description JSON for UTC Start time");
            add_request("piecesjson",req_piecesjson,"","return description JSON for Pieces");
            add_request("vertexsjson",req_vertexsjson,"","return description JSON for Pieces");
            add_request("facesjson",req_facesjson,"","return description JSON for Pieces");
            add_request("devgenjson",req_devgenjson,"","return description JSON for General Devices");
            add_request("devspecjson",req_devspecjson,"","return description JSON for Specific Devices");
            add_request("portsjson",req_portsjson,"","return description JSON for Ports");
            add_request("targetsjson",req_targetsjson,"","return description JSON for Targets");
            add_request("aliasesjson",req_aliasesjson,"","return description JSON for Aliases");
            add_request("heartbeat",req_heartbeat,"","Post a hearbeat");
            add_request("postsoh",req_postsoh,"","Post a SOH");
            add_request("utc",req_utc,"","Get UTC as both Modified Julian Day and Unix Time");
            add_request("soh",req_soh,"","Get Limited SOH string");
            add_request("fullsoh",req_fullsoh,"","Get Full SOH string");
            add_request("jsondump",req_jsondump,"","Dump JSON ini files to node folder");
            add_request("all_names_types",req_all_names_types,"","return text with all names and types in Namespace 2.0");
            add_request("command", req_command, "node radioout:radioin:repeat command arg1 arg2 arg3 ...",
                        "Command sent to node over radiout expecting response over readioin repeat times.\n"
                        "Commands:\n"
                        "    Reset [seconds_delay]\n"
                        "    Reboot [seconds_delay]\n"
                        "    Halt [seconds_delay]\n"
                        "    SendBeacon [type:count]\n"
                        "    ClearQueue [channel]\n"
                        "    ExternalCommand command parameters\n"
                        "    ExternalTask command parameters\n"
                        "    TestRadio start step count bytes\n"
                        "    ListDirectory node:agent\n"
                        "    TransferFile node:agent:file\n"
                        "    TransferNode node\n"
                        "    TransferRadio\n"
                        "    InternalRequest request parameters\n"
                        "    Ping {string}\n"
                        "    SetTime {MJD|delta {limit}}\n"
                        "    GetTimeHuman\n"
                        "    GetTimeBinary\n"
                        "    SetOpsMode [modestring]\n"
                        "    EnableChannel [channelstring | channelnumber] {State}\n"
                        "    EpsCommunicate sbid:command:hexstring:response_size\n"
                        "    EpsSwitchStatus [0-1|vbattbus|5vbus|hdrm|3v3bus|adcs|adcsalt|gps|sband|xband|mcce|unibap|ext200|sif]\n"
                        "    EpsState {0|1|2|3}\n"
                        "    EpsSwitchName {vbattbus|5vbus|hdrm|3v3bus|adcs|adcsalt|gps|sband|xband|mcce|unibap|ext200|sif} [0|1|2]\n"
                        "    EpsSwitchNumber boardid switchid [0|1]\n"
                        "    EpsSwitchNames {vbattbus ...} [0|1]\n"
                        "    AdcsOrbitParameters inc ecc raan ap bstar mm ma epoch\n"
                        "    AdcsState {0-7} {0-255} ... \n"
                        "    AdcsCommunicate command:hexstring:response_size\n"
                        "    CameraCapture [frames [usec [ startcol width [startrow height [name]]]]]\n"
                        "    ExecAddCommand json_event_string\n"
                        "");
            add_request("list_channels", req_list_channels, "", "List current channels");
            add_request("run_command", req_run_command, "command parameters", "Run external command for immediate response");
            add_request("add_task", req_add_task, "command parameters", "Start external command as Task for output to file");
            add_request("test_channel", req_test_channel, "channel radio dest start step count bytes", "Run channel performance test");
            add_request("channel_enable", req_channel_enable, "channel state", "Set channel enabled state to the specified value");
            add_request("channel_touch", req_channel_touch, "channel [seconds]", "Reset channel age");
            // Set up Full SOH string
            //            set_fullsohstring(json_list_of_fullsoh(cinfo));

            cinfo->agent0.server = 1;
            cinfo->agent0.stateflag = static_cast<uint16_t>(Agent::State::RUN);
            activeTimeout = currentmjd() + cinfo->agent0.aprd / 86400.;
        }

        Agent::~Agent() { Agent::shutdown(); }

        //! Add internal request to Agent request list with description and synopsis.
        /*! Adds access to the indicated function by way of the given token. The provided
* function, internal to the agent, will be called with the request string as its argument. Any result will then
* be returned in the output pointer.
    \param token A string of maximum length ::COSMOS_MAX_NAME containing the token
    for the request. This should be the first word in the request.
    \param function The user supplied function to parse the specified request.
    \param description A brief description of the function performed.
    \param synopsis A usage synopsis for the request.
    \return Error, if any, otherwise zero.
*/
        //        int32_t Agent::add_request_internal(string token, Agent::internal_request_function function, string synopsis, string description)
        //        {
        //            if (reqs.size() > AGENTMAXREQUESTCOUNT)
        //                return (AGENT_ERROR_REQ_COUNT);

        //            request_entry tentry;
        //            if (token.size() > COSMOS_MAX_NAME)
        //            {
        //                token.resize(COSMOS_MAX_NAME);
        //            }
        //            tentry.token = token;
        //            tentry.ifunction = function;
        //            tentry.efunction = nullptr;
        //            tentry.synopsis = synopsis;
        //            tentry.description = description;
        //            reqs.push_back(tentry);
        //            return 0;
        //        }

        //! Add external request to Agent request list with description and synopsis.
        /*! Adds access to the indicated function by way of the given token. The provided
* function, external to the agent, will be called with the request string as its argument. Any result will then
* be returned in the output pointer.
    \param token A string of maximum length ::COSMOS_MAX_NAME containing the token
    for the request. This should be the first word in the request.
    \param function The user supplied function to parse the specified request.
    \param description A brief description of the function performed.
    \param synopsis A usage synopsis for the request.
    \return Error, if any, otherwise zero.
*/
        int32_t Agent::add_request(string token, Agent::external_request_function function, string synopsis, string description)
        {
            if (reqs.size() > AGENTMAXREQUESTCOUNT) return (AGENT_ERROR_REQ_COUNT);

            request_entry tentry;
            if (token.size() > COSMOS_MAX_NAME) { token.resize(COSMOS_MAX_NAME); }
            tentry.token = token;
            tentry.efunction = function;
            tentry.synopsis = synopsis;
            tentry.description = description;
            reqs[token] = tentry;
            return 0;
        }


        //! Start Agent Request and Heartbeat loops
        /*!	Starts the request and heartbeat threads for an Agent server initialized with
 * Cosmos::Agent::Agent. The Agent will open its request and heartbeat channels using the
 * address and port supplied in cinfo. The heartbeat will cycle with the period requested in cinfo.
    \return value returned by request thread create
*/
        int32_t Agent::start() {
            // start heartbeat thread
            hthread = thread([=] { heartbeat_loop(); });
            cthread = thread([=] { request_loop(); });
            return 0;
        }

        //! Begin Active Loop
        //! Initializes timer for active loop using ::aprd
        //! \return Zero or negative error.
        int32_t Agent::start_active_loop() {
            activeTimeout = currentmjd() + cinfo->agent0.aprd / 86400.;
            return 0;
        }

        //! Finish active loop
        //! Sleep for the remainder of this loops ::aprd as initialized in ::start_active_loop.
        //! \return Zero or negative error.
        int32_t Agent::finish_active_loop() {
            double sleepsec = 86400.*(activeTimeout - currentmjd());
            activeTimeout += cinfo->agent0.aprd / 86400.;
            cinfo->agent0.beat.dcycle = (cinfo->agent0.aprd - sleepsec) / cinfo->agent0.aprd;
            secondsleep(sleepsec);
            return sleepsec*1000000;
        }

        //! Shutdown agent gracefully
        /*! Waits for threads to stop running if we are a server, then releases everything.
 * \return 0 or negative error.
 */
        int32_t Agent::shutdown()
        {
            if (debug_level) {
                debug_log.Printf("Shutting down Agent. Last error: %s\n", cosmos_error_string(error_value).c_str());
                fflush(stdout);
            }

            if (cinfo != nullptr)
            {
                cinfo->agent0.stateflag = static_cast <uint16_t>(Agent::State::SHUTDOWN);
                if (cinfo->agent0.name.size())
                {
                    if (hthread.joinable()) { hthread.join(); }
                    if (cthread.joinable()) { cthread.join(); }
                    Agent::unpublish();
                }
            }

            if (mthread.joinable()) { mthread.join(); }
            Agent::unsubscribe();
            json_destroy(cinfo);
            return 0;
        }

        //! Check if we're supposed to be running
        /*!	Returns the value of the internal variable that indicates that
 * the threads are running.
    \return Value of internal state variable, as enumerated in Cosmos::Agent:State.
*/
        uint16_t Agent::running() { return cinfo->agent0.stateflag; }

        //! Wait on state
        //! Wait for up to waitsec seconds for Agent to enter requested state
        //! \param state Desired ::Agent::State.
        //! \param waitsec Maximum number of seconds to wait.
        //! \return Zero, or timeout error.
        int32_t Agent::wait(State state, double waitsec) {
            if (cinfo == nullptr) { return AGENT_ERROR_NULL; }

            ElapsedTime et;
            while (cinfo->agent0.stateflag != static_cast <uint16_t>(state) && et.split() < waitsec) {
                secondsleep(.1);
            }
            if (cinfo->agent0.stateflag == static_cast <uint16_t>(state)) {
                return 0;
            } else {
                return GENERAL_ERROR_TIMEOUT;
            }
        }

        //! Last error value.
        //! Get value of last error returned by any function.
        int32_t Agent::last_error() { return (error_value); }

        //! Send a request over AGENT
        /*! Send a request string to the process at the provided address
    \param hbeat The agent ::beatstruc
    \param request the request and its arguments
    \param output any output returned by the request
    \param waitsec Maximum number of seconds to wait
    \return Either the number of bytes returned, or an error number.
*/
        int32_t Agent::send_request(beatstruc hbeat, string request, string &output, float waitsec, double delay_send, double delay_receive) {

            secondsleep(delay_send);

            socket_channel sendchan;
            int32_t iretn = 0;
            int32_t nbytes;
            vector <char> toutput;
            toutput.resize(AGENTMAXBUFFER+1);

            if (hbeat.utc == 0. || hbeat.addr[0] == 0 || hbeat.port == 0) return (AGENT_ERROR_SOCKET);

            ElapsedTime ep;
            ep.start();

            if ((iretn=socket_open(&sendchan, NetworkType::UDP, hbeat.addr, hbeat.port, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) < 0) { return (-errno); }
            //nbytes = strnlen(request.c_str(), hbeat.bsz);
            nbytes = std::min(request.size(), (size_t)hbeat.bsz);
            nbytes=sendto(sendchan.cudp, request.c_str(), nbytes, 0, (struct sockaddr *)&sendchan.caddr, sizeof(struct sockaddr_in));
            if (debug_level) {
                debug_log.Printf("Send Request: [%s:%u:%d] %s\n", sendchan.address, sendchan.cport, iretn, &request[0]);
            }

            if ((nbytes) < 0) {
                CLOSE_SOCKET(sendchan.cudp);
#ifdef COSMOS_WIN_OS
                return(-WSAGetLastError());
#else
                return (-errno);
#endif
            }

            do
            {
                nbytes = recvfrom(sendchan.cudp, toutput.data(), AGENTMAXBUFFER, 0, static_cast<struct sockaddr *>(nullptr), static_cast<socklen_t *>(nullptr));
            } while ( (nbytes <= 0) && (ep.split() <= waitsec) );

            CLOSE_SOCKET(sendchan.cudp);

            if (nbytes < 0)
            {
#ifdef COSMOS_WIN_OS
                return(-WSAGetLastError());
#else
                return (-errno);
#endif
            }
            else
            {
                //toutput[nbytes] = 0;
                toutput.resize(nbytes);
                string reply(toutput.begin(), toutput.end());
                output = reply;

                secondsleep(delay_receive);
                return (nbytes);
            }
        }

        //! Send request for Node JSON
        /*! Send a set of requests to return the various JSON strings that make up a ::jsonnode.
    \param hbeat The agent ::beatstruc
    \param jnode ::jsonnode cotaining Node information.
    \param waitsec Maximum number of seconds to wait.
    \return Either the number of bytes returned, or an error number.
*/
        int32_t Agent::send_request_jsonnode(beatstruc hbeat, jsonnode &jnode, float waitsec) {
            int32_t iretn = 0;

            jnode.name = hbeat.node;
            iretn = send_request(hbeat, "nodejson", jnode.node, waitsec);
            if (iretn < 0) { return iretn; }
            iretn = send_request(hbeat, "statejson", jnode.state, waitsec);
            if (iretn < 0) { return iretn; }
            iretn = send_request(hbeat, "piecesjson", jnode.pieces, waitsec);
            if (iretn < 0) { return iretn; }
            iretn = send_request(hbeat, "facesjson", jnode.faces, waitsec);
            if (iretn < 0) { return iretn; }
            iretn = send_request(hbeat, "vertexsjson", jnode.vertexs, waitsec);
            if (iretn < 0) { return iretn; }
            iretn = send_request(hbeat, "devgenjson", jnode.devgen, waitsec);
            if (iretn < 0) { return iretn; }
            iretn = send_request(hbeat, "devspecjson", jnode.devspec, waitsec);
            if (iretn < 0) { return iretn; }
            iretn = send_request(hbeat, "portsjson", jnode.ports, waitsec);
            if (iretn < 0) { return iretn; }
            iretn = send_request(hbeat, "aliasesjson", jnode.aliases, waitsec);
            if (iretn < 0) { return iretn; }
            iretn = send_request(hbeat, "targetsjson", jnode.targets, waitsec);
            return iretn;
        }

        //! Get specific server.
        /*! Listen to the multicast/broadcast traffic for a set amount of time,
 * waiting for a specific named server to appear, then return its
 * heartbeat.
    \param node Node for the server.
    \param name Name of the server.
    \param waitsec Maximum number of seconds to wait
    \param rbeat pointer to a location to store the heartbeat
    \return 1 if found, otherwise 0, or an error number
*/
        int32_t Agent::get_agent(string node, string agent, double waitsec, beatstruc &rbeat)
        {

            post(AgentMessage::REQUEST, "heartbeat");
            secondsleep(.1);

            ElapsedTime ep;
            ep.start();
            do {
                for (beatstruc &it : agent_list)
                {
                    if ((node == "any" || string(it.node) == node) && string(it.proc) == agent)
                    {
                        it.exists = true;
                        rbeat = it;
                        return 1;
                    }
                }
                secondsleep(.1);
            } while (ep.split() < waitsec);

            rbeat.exists = false;
            rbeat.node[0] = '\0';
            return 0;
        }

        //! Check agent
        /*! Check the Cosmos::Agent::agent_list for the particular agent
    \param agent Name of agent.
    \param node Node that agent is in.
    \param waitsec Number of seconds to wait
    \return 1 if found, otherwise 0, or an error number
 */
        int32_t Agent::check_agent(string node, string agent, double waitsec)
        {
            beatstruc tbeat;
            return get_agent(node, agent, waitsec, tbeat);
        }

        //! Find agent
        /*! Check the Cosmos::Agent::agent_list for the particular agent,
     * returning its heartbeat if found.
        \param agent Name of agent.
        \param node Node that agent is in.
        \return ::beatstruc of located agent, otherwise empty ::beatstruc.
     */
        beatstruc Agent::find_agent(string node, string agent, double waitsec)
        {
            beatstruc rbeat;
            get_agent(node, agent, waitsec, rbeat);
            return rbeat;

            // ask all existing agents to send out a heartbeat
            //        post(AgentMessage::REQUEST, "heartbeat");
            //        secondsleep(.1);

            //        ElapsedTime ep;
            //        ep.start();
            //        do {
            //            for (beatstruc &it : agent_list)
            //            {
            //                if ((node == "any" || it.node == node) && it.proc == agent)
            //                {
            //                    it.exists = true;
            //                    return it;
            //                }
            //            }
            //            secondsleep(.1);
            //        } while (ep.split() < waitsec);

            //        beatstruc nobeat;
            //        nobeat.exists = false;
            //        nobeat.node[0] = '\0';

            //        return nobeat;
        }

        //! Find single server
        /*! Listen to the local subnet for a set amount of time,
 * collecting heartbeats, searching for a particular agent.
    \param node Node that agent is in.
    \param proc Name of agent.
    \param waitsec Maximum number of seconds to wait.
    \return ::beatstruc of located agent, otherwise empty ::beatstruc.
 */

        //! Generate a list of request servers.
        /*! Listen to the local subnet for a set amount of time,
 * collecting heartbeats. Return a list of heartbeats collected.
    \param waitsec Maximum number of seconds to wait.
    \return A vector of ::beatstruc entries listing the unique servers found.
*/
        vector<beatstruc> Agent::find_agents(double waitsec) {
            beatstruc tbeat;

            //! Loop for ::waitsec seconds, filling list with any discovered heartbeats.

            ElapsedTime ep;
            ep.start();

            do {
                for (size_t k=0; k<agent_list.size(); ++k) {
                    size_t i;
                    for (i=0; i<slist.size(); i++) {
                        if (!agent_list[k].node.compare(slist[i].node) && !agent_list[k].proc.compare(slist[i].proc))
                            break;
                    }
                    if (i == slist.size()) {
                        slist.push_back(agent_list[k]);
                        for (size_t j=i; j>0; j--) {
                            if (slist[j].port > slist[j-1].port) { break; }
                            tbeat = slist[j];
                            slist[j] = slist[j-1];
                            slist[j-1] = tbeat;
                        }
                    }
                }
            } while (ep.split() <= waitsec);
            return(slist);
        }

        //! Set Limited SOH string
        /*! Set the Limited SOH string to a JSON list of \ref jsonlib_namespace names. A
 * proper JSON list will begin and end with matched curly braces, be comma separated,
 * and have all strings in double quotes.
    \param list Properly formatted list of JSON names.
    \return 0, otherwise a negative error.
*/
        int32_t Agent::set_sohstring(string list) {
            if (!sohtable.empty()) { sohtable.clear(); }
            json_table_of_list(sohtable, list, cinfo);
            return 0;
        }

        int32_t Agent::set_sohstring(vector<string> list)
        {
            if(list.size() == 0) return ErrorNumbers::COSMOS_GENERAL_ERROR_EMPTY;
            string jsonlist = "{";
            for(string name: list){
                jsonlist += "\"" + name + "\",";
            }
            jsonlist.pop_back(); // remove last ","
            jsonlist += "}";
            return set_sohstring(jsonlist);
        }

        //! Set SOH string
        /*! Set the SOH string to a json list of \ref namespace 2.0 names.
        \param list Vector of strings of namespace 2.0 names.
        \return 0, otherwise a negative error.
    */
        int32_t Agent::set_sohstring2(vector<string> list) {
            sohstring = list;

            return 0;
        }

        //! Set Full SOH string
        /*! Set the Full SOH string to a JSON list of \ref jsonlib_namespace names. A
 * proper JSON list will begin and end with matched curly braces, be comma separated,
 * and have all strings in double quotes.
    \param list Properly formatted list of JSON names.
    \return 0, otherwise a negative error.
*/
        int32_t Agent::set_fullsohstring(string list) {
            if (!fullsohtable.empty()) { fullsohtable.clear(); }
            json_table_of_list(fullsohtable, list, cinfo);
            return 0;
        }

        //! Return Agent ::cosmosstruc
        /*! Return a pointer to the Agent's internal copy of the ::cosmosstruc.
    \return A pointer to the Cosmos::Support::cosmosstruc, otherwise NULL.
*/
        cosmosstruc *Agent::get_cosmosstruc() { return (cinfo); }

        //! Heartbeat Loop
        /*! This function is run as a thread to provide the Heartbeat for the Agent. The Heartbeat will
 * consist of the contents of Agent::AGENT_MESSAG::BEAT in Cosmos::Agent::poll, plus the contents of the
 * Cosmos::Agent::sohstring. It will come every beatstruc::bprd seconds.
 */
        void Agent::heartbeat_loop() {
            ElapsedTime timer_beat;

            while (cinfo->agent0.stateflag) {

                // compute the jitter
                if (cinfo->agent0.beat.bprd == 0.) {
                    cinfo->agent0.beat.jitter = timer_beat.split() - 1.;
                } else {
                    cinfo->agent0.beat.jitter = timer_beat.split() - cinfo->agent0.beat.bprd;
                }
                timer_beat.start();

                // post comes first
                if (cinfo->agent0.beat.bprd != 0.) {
                    post_beat();
                }

                // TODO: move the monitoring calculations to another thread with its own loop time that can be controlled
                // Compute other monitored quantities if monitoring
                if (cinfo->agent0.stateflag == static_cast <uint16_t>(Agent::State::MONITOR)) {
                    // TODO: rename beat.cpu to beat.cpu_percent
                    // add beat.cpu_load
                    cinfo->agent0.beat.cpu    = deviceCpu_.getPercentUseForCurrentProcess();//cpu.getLoad();
                    cinfo->agent0.beat.memory = deviceCpu_.getVirtualMemoryUsed();
                }

                if (cinfo->agent0.stateflag == static_cast <uint16_t>(Agent::State::SHUTDOWN)) {
                    cinfo->agent0.beat.cpu = 0;
                    cinfo->agent0.beat.memory = 0;
                }


                if (cinfo->agent0.beat.bprd < AGENT_HEARTBEAT_PERIOD_MIN) {
                    cinfo->agent0.beat.bprd = 0.;
                }

                if (cinfo->agent0.beat.bprd == 0.) {
                    secondsleep(1.);
                } else {
                    if (timer_beat.split() <= cinfo->agent0.beat.bprd) {
                        secondsleep(cinfo->agent0.beat.bprd - timer_beat.split());
                    }
                }
            }
            Agent::unpublish();
        }

        //! Request Loop
        /*! This function is run as a thread to service requests to the Agent. It receives requests on
 * it assigned port number, matches the first word of the request against its set of requests,
 * and then either performs the matched function, or returns [NOK].
 */
        void Agent::request_loop() noexcept {
            int32_t iretn = 0;
            string bufferin;

            if ((iretn = socket_open(&cinfo->agent0.req, NetworkType::UDP, (char *)"", cinfo->agent0.beat.port, SOCKET_LISTEN, SOCKET_BLOCKING, 2000000)) < 0) { return; }

            cinfo->agent0.beat.port = cinfo->agent0.req.cport;

            while (cinfo->agent0.stateflag)
            {
                bufferin.resize(cinfo->agent0.beat.bsz);
                cinfo->agent0.req.addrlen = sizeof(cinfo->agent0.req.caddr);
                iretn = recvfrom(cinfo->agent0.req.cudp, &bufferin[0], bufferin.size(), 0, (struct sockaddr *)&cinfo->agent0.req.caddr, (socklen_t *)&cinfo->agent0.req.addrlen);

                if (iretn > 0)
                {
                    string bufferout;
                    bufferin.resize(iretn);
                    process_request(bufferin, bufferout);
                }
            }
            return;
        }

        int32_t Agent::process_request(string &bufferin, string &bufferout)
        {
            size_t i;
            int32_t iretn = 0;

            process_mutex.lock();

            if (debug_level) {
                debug_log.Printf("Request: [%lu] %s\n",bufferin.size(), &bufferin[0]);
                fflush(stdout);
            }

            string variable;
            string request;
            request.resize(AGENTMAXBUFFER+1);
            for (i=0; i<COSMOS_MAX_NAME; i++) {
                if (bufferin[i] == ' ' || bufferin[i] == 0)	{
                    break;
                }
                request[i] = bufferin[i];
            }
            //request[i] = 0;
            request.resize(i);

            bufferout = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " " + &bufferin[0] + "\n";
            if(reqs.find(request) == reqs.end())
            {
                iretn = AGENT_ERROR_NULL;
                bufferout += "[NOK] " + std::to_string(iretn);
            }
            else
            {
                request_entry &rentry = reqs[request];
                iretn = -1;
                if (rentry.efunction != nullptr)
                {
                    iretn = rentry.efunction(bufferin, request, this);
                }
                if (iretn >= 0)
                {
                    request.resize(strlen(&request[0]));
                    bufferout += request;
                    if (bufferout.back() != '\n')
                    {
                        bufferout += "\n";
                    }
                }
                else
                {
                    bufferout += "[NOK] " + std::to_string(iretn) + "\n";
                }
            }

            iretn = sendto(cinfo->agent0.req.cudp, bufferout.data(), bufferout.size(), 0, (struct sockaddr *)&cinfo->agent0.req.caddr, sizeof(struct sockaddr_in));
            if (iretn < 0)
            {
                iretn = -errno;
            }
            debug_log.Printf("Response: [%d:%s:%x:%u:%d] %s\n", cinfo->agent0.req.cudp, cinfo->agent0.req.address, ntohl(cinfo->agent0.req.caddr.sin_addr.s_addr), cinfo->agent0.req.cport, iretn, &bufferout[0]);

            process_mutex.unlock();

            return iretn;
        }

        //! Start listening for incoming messages over COSMOS Agent channel.
        //! Keep track of any new Agents.
        //! Add messages to message ring.
        //! Perform and broadcast requests.
        void Agent::message_loop() {
            messstruc mess;
            int32_t iretn = 0;
            // Initialize things
            //            message_ring.resize(MESSAGE_RING_SIZE);
            while (Agent::running()) {
                iretn = Agent::poll(mess, AgentMessage::ALL, 0.);
                if (iretn > 0)
                {
                    if (!mess.meta.beat.proc.compare("") && mess.adata.empty()) {
                        continue;
                    }

                    if (mess.meta.beat.port)
                    {
                        bool agent_found = false;

                        for (beatstruc &i : agent_list) {
                            if (!i.node.compare(mess.meta.beat.node) && !i.proc.compare(mess.meta.beat.proc)) {
                                agent_found = true;
                                // update all information for the last contact with given (node, agent)...
                                i = mess.meta.beat;
                                break;
                            }
                        }

                        if (!agent_found) {
                            agent_list.push_back(mess.meta.beat);
                        }
                    }

                    if (mess.meta.type == AgentMessage::REQUEST && cinfo->agent0.beat.proc.compare(""))
                    {
                        string response;
                        process_request(mess.adata, response);
                        Agent::post(AgentMessage::RESPONSE, response);
                    }
                    else if (mess.meta.type == AgentMessage::COMM)
                    {
                        PacketComm packet;
                        packet.wrapped = mess.bdata;
                        if (packet.Unwrap() > 0 && packet.header.chanout < channel_count())
                        {
                            channel_push(packet.header.chanout, packet);
                        }
                    }
                    else
                    {
                        message_queue.push_back(mess);
                        if (message_queue.size() > MESSAGE_RING_SIZE) { message_queue.pop_front(); }
                    }
                }
            }
        }

        //! Built-in Forward request
        /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_forward(string & request, string & output, Agent *, Agent* agent)
        int32_t Agent::req_forward(string &request, string &output, Agent* agent) {
            uint16_t count;
            int32_t iretn=-1;

            //            sscanf(&request[0],"%*s %hu",&count);
            sscanf(&request[0],"%*s %hu",&count);
            for (uint16_t i=0; i<agent->cinfo->agent0.ifcnt; ++i)
            {
                //                iretn = sendto(agent->cinfo->agent0.pub[i].cudp,(const char *)&request[request.length()-count],count,0,(struct sockaddr *)&agent->cinfo->agent0.pub[i].baddr,sizeof(struct sockaddr_in));
                iretn = sendto(agent->cinfo->agent0.pub[i].cudp,(const char *)&request[request.size()-count],count,0,(struct sockaddr *)&agent->cinfo->agent0.pub[i].baddr,sizeof(struct sockaddr_in));
                if (agent->get_debug_level()) {
                    agent->debug_log.Printf("Forward: [%s:%u:%d] %s\n", agent->cinfo->agent0.pub[i].address, agent->cinfo->agent0.pub[i].cport, iretn, (const char *)&request[request.size()-count]);
                }
            }
            //            sprintf(output,"%.17g %d ",currentmjd(0),iretn);
            output = std::to_string(currentmjd()) + ' ' + std::to_string(iretn);
            return(0);
        }

        //! Built-in Echo request
        /*! Returns the received packet, reaclculating the CRC, and adding the time.
 * \param request Text of echo packet.
 * \param output Text of echoed packet.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_echo(string & request, string & output, Agent *, Agent*)
        int32_t Agent::req_echo(string &request, string &output, Agent*) {
            double mjd;
            uint16_t crc, count;

            //            sscanf(&request[0],"%*s %lf %hx %hu",&mjd,&crc,&count);
            //            sprintf(output,"%.17g %x %u ",currentmjd(0),slip_calc_crc((uint8_t *)&request[request.length()-count],count),count);
            //            strncpy(&output[strlen(output)],&request[request.length()-count],count+1);
            sscanf(&request[0],"%*s %lf %hx %hu",&mjd,&crc,&count);
            output = to_mjd(currentmjd()) + ' ' + to_hex(crc) + ' ' + std::to_string(count) + ' ' + request;
            return 0;
        }

        //! Built-in Help request
        /*! Send help response.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        int32_t Agent::req_help_json(string &request, string &output, Agent* agent)
        {
            string function = "all";
            if (request.size())
            {
                vector<string> args = string_split(request);
                if (args.size() > 1 && args[0] == "help")
                {
                    function = args[1];
                }
            }
            string help_string, s;
            size_t qpos, prev_qpos = 0;
            help_string += "{\"requests\": [";
            map<string, request_entry>::iterator it;
            if (function == "all")
            {
                for(it = agent->reqs.begin(); it != agent->reqs.end(); ++it)
                {
                    if(it != agent->reqs.begin()) help_string+=",";
                    help_string += "{\"token\": \"";
                    help_string += it->second.token;
                    help_string += "\", \"synopsis\": \"";
                    qpos = 0;
                    prev_qpos = 0;
                    s = it->second.synopsis;
                    while((qpos=s.substr(prev_qpos).find("\""))!= string::npos)
                    {
                        s.replace(qpos+prev_qpos, 1, "\\\"");
                        prev_qpos +=qpos+2;
                    }
                    help_string+= s;
                    help_string += "\", \"description\": \"";
                    qpos = 0;
                    prev_qpos = 0;
                    s = it->second.description;
                    while((qpos=s.substr(prev_qpos).find("\""))!= string::npos){
                        s.replace(qpos+prev_qpos, 1, "\\\"");
                        prev_qpos +=qpos+2;
                    }
                    help_string+= s;

                    help_string +="\"}";
                }
            }
            else if ((it = agent->reqs.find(function)) != agent->reqs.end())
            {
                help_string += "{\"token\": \"";
                help_string += it->second.token;
                help_string += "\", \"synopsis\": \"";
                qpos = 0;
                prev_qpos = 0;
                s = it->second.synopsis;
                while((qpos=s.substr(prev_qpos).find("\""))!= string::npos)
                {
                    s.replace(qpos+prev_qpos, 1, "\\\"");
                    prev_qpos +=qpos+2;
                }
                help_string+= s;
                help_string += "\", \"description\": \"";
                qpos = 0;
                prev_qpos = 0;
                s = it->second.description;
                while((qpos=s.substr(prev_qpos).find("\""))!= string::npos){
                    s.replace(qpos+prev_qpos, 1, "\\\"");
                    prev_qpos +=qpos+2;
                }
                help_string+= s;

                help_string +="\"}";
            }
            else
            {
                help_string += "{\"token\": \"";
                help_string += function;
                help_string += "\", \"synopsis\": \"Unknown Function";
                help_string +="\"}";
            }
            help_string += "]}";
            output = help_string;
            return 0;
        }

        int32_t Agent::req_help(string &request, string &output, Agent* agent)
        {
            string function = "all";
            if (request.size())
            {
                vector<string> args = string_split(request);
                if (args.size() > 1 && args[0] == "help")
                {
                    function = args[1];
                }
            }
            string help_string;
            help_string += "\n";
            map<string, request_entry>::iterator it;
            if (function == "all")
            {
                for(it = agent->reqs.begin(); it != agent->reqs.end(); ++it)
                {
                    help_string += "        ";
                    help_string += it->second.token;
                    help_string += " ";
                    help_string += it->second.synopsis;
                    help_string += "\n";
                    help_string += "                ";
                    help_string += it->second.description;
                    help_string += "\n\n";
                }
            }
            else if ((it = agent->reqs.find(function)) != agent->reqs.end())
            {
                help_string += "        ";
                help_string += it->second.token;
                help_string += " ";
                help_string += it->second.synopsis;
                help_string += "\n";
                help_string += "                ";
                help_string += it->second.description;
                help_string += "\n\n";
            }
            else
            {
                help_string += "Unknown Function";
                help_string += "\n";
            }
            help_string += "\n";
            output = help_string;
            return 0;
        }

        //! Built-in Set state to Run request
        /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_run(char*, char* output, Agent* agent)
        int32_t Agent::req_run(string &, string &output, Agent* agent) {
            agent->cinfo->agent0.stateflag = static_cast <uint16_t>(Agent::State::RUN);
            output[0] = 0;
            return(0);
        }

        //! Built-in Set state to Init request
        /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_init(char*, char* output, Agent* agent)
        int32_t Agent::req_init(string &, string &output, Agent* agent) {
            agent->cinfo->agent0.stateflag = static_cast <uint16_t>(Agent::State::INIT);
            output[0] = 0;
            return(0);
        }

        //! Built-in Set state to Idle request
        /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_idle(char*, char* output, Agent* agent)
        int32_t Agent::req_idle(string &, string &output, Agent* agent) {
            agent->cinfo->agent0.stateflag = static_cast <uint16_t>(Agent::State::IDLE);
            output[0] = 0;
            return(0);
        }

        //! Built-in Set state to Monitor request
        /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_monitor(char*, char* output, Agent* agent)
        int32_t Agent::req_monitor(string &, string &output, Agent* agent) {
            agent->cinfo->agent0.stateflag = static_cast <uint16_t>(Agent::State::MONITOR);
            output[0] = 0;
            return(0);
        }

        //! Built-in Set state to Reset request
        /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_reset(char*, char* output, Agent* agent)
        int32_t Agent::req_reset(string &, string &output, Agent* agent) {
            agent->cinfo->agent0.stateflag = static_cast <uint16_t>(Agent::State::RESET);
            output[0] = 0;
            return(0);
        }

        //! Built-in Set state to Shutdown request
        /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_shutdown(char*, char* output, Agent* agent)
        int32_t Agent::req_shutdown(string &, string &output, Agent* agent) {
            agent->cinfo->agent0.stateflag = static_cast <uint16_t>(Agent::State::SHUTDOWN);
            //            output[0] = 0;
            output.clear();
            return(0);
        }

        // TODO: add a line break (\n) when printing the data
        // this makes it easier to read
        //! Built-in Status request
        /*! Returns agent status.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_status(char*, char* output, Agent* agent)
        int32_t Agent::req_status(string &, string &output, Agent* agent) {
            string jstring;

            if (json_of_agent(jstring, agent->cinfo) != NULL)
            {
                //                strncpy(output, jstring.c_str(),agent->cinfo->agent0.beat.bsz);
                //                output[agent->cinfo->agent0.beat.bsz-1] = 0;
                output = jstring;
                if (output.length() > agent->cinfo->agent0.beat.bsz)
                {
                    output[agent->cinfo->agent0.beat.bsz-1] = 0;
                }
                return 0;
            } else {
                output = "error";
                return(JSON_ERROR_SCAN);
            }
        }

        //! Built-in Get Debug Level request
        //! Returns or sets the debug_level value.
        //! \param request Text of request.
        //! \param output Text of response to request.
        //! \param agent Pointer to Cosmos::Agent to use.
        //! \return 0, or negative error.
        //        int32_t Agent::req_debug_level(string &request, char* output, Agent* agent)
        int32_t Agent::req_debug_level(string &request, string &output, Agent* agent) {
            if (request != "debug_level") {
                uint16_t level;
                sscanf(&request[0], "debug_level %hu", &level);
                agent->set_debug_level(level);
            }
            output = std::to_string(agent->get_debug_level());
            return 0;
        }

        //! Built-in Get Internal Value request
        /*! Returns the current value of the requested Name Space values. Names are expressed as a JSON object.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        int32_t Agent::req_getvalue(string &request, string &output, Agent* agent)
        {
            //	cout<<"req_getvalue(): incoming request          = <"<<request<<">"<<endl;
            //	cout<<"req_getvalue(): incoming request.size()   = "<<request.size()<<endl;
            //	cout<<"req_getvalue(): incoming request.length() = "<<request.length()<<endl;
            string jstring;
            if (json_of_list(jstring, request, agent->cinfo) != NULL) {
                output = jstring;
                if (output.length() > agent->cinfo->agent0.beat.bsz) {
                    output[agent->cinfo->agent0.beat.bsz-1] = 0;
                }
                //	cout<<"req_getvalue(): outgoing response         = <"<<output<<">"<<endl;
                return 0;
            } else {
                return (JSON_ERROR_EOS);
            }
        }

        int32_t Agent::req_get_value(string &request, string &response, Agent* agent)	{
            string req = request;
            //	cout<<"req_get_value():incoming request          = <"<<request<<">"<<endl;
            //	cout<<"req_get_value():incoming request.size()   = "<<request.size()<<endl;
            //	cout<<"req_get_value():incoming request.length() = "<<request.length()<<endl;
            // remove function call and space
            req.erase(0,10);
            // strip out requested names
            vector<string> names;
            for(size_t i = 0; i < req.size(); ++i)   {
                if(req[i]=='"')  {
                    string name("");
                    while(req[++i]!='"'&&i<req.size())    { name.push_back(req[i]); }
                    names.push_back(name);
                }
            }


            // ERIC: response string comes to here with the request function name inside it? it that a bug or a feature?
            response.clear();
            for(size_t i = 0; i < names.size(); ++i)   {
                response += agent->cinfo->get_json(names[i]);
            }
            //	cout<<"req_get_value():outgoing response         = <"<<response<<">"<<endl;
            return 0;
        }


        // returns state of agent recieving request as JSON
        int32_t Agent::req_get_state(string &request, string &response, Agent* agent)	{
            string req = request;
            // remove function call and space
            req.erase(0,10);

            response.clear();
            // find index of calling agent in sim_states[]
            for(size_t i = 0; i < agent->cinfo->sim_states.size(); ++i)   {
                //        string node_name(agent->cinfo->agent0.beat.node);
                //        string agent_name(agent->cinfo->agent0.beat.proc);
                //        if(agent->cinfo->sim_states[i].node_name == node_name && agent->cinfo->sim_states[i].agent_name == agent_name)  {
                if(agent->cinfo->sim_states[i].node_name == agent->cinfo->agent0.beat.node)
                {
                    // this wraps as a json object
                    //string j = "sim_states[" + to_string(i) + "]";
                    //response = agent->cinfo->get_json(j);

                    // this does not wrap
                    response = agent->cinfo->sim_states[i].to_json().dump();
                    break;
                }
            }

            //cout<<"req_get_state():outgoing response         = <"<<response<<">"<<endl;
            return 0;
        }



        int32_t Agent::req_get_time(string &request, string &response, Agent* agent)	{
            stringstream ss;
            ss<<setprecision(numeric_limits<double>::digits10)<<currentmjd();
            response = ss.str();
            return 0;
        }

        // request = "get_position mjdtime"
        int32_t Agent::req_get_position_data(string &request, string &response, Agent* agent)	{

            //cout<<"\tincoming request          = <"<<request<<">"<<endl;
            //cout<<"req_get_position():incoming request.size()   = "<<request.size()<<endl;
            //cout<<"req_get_position():incoming request.length() = "<<request.length()<<endl;

            // remove function call and space
            request.erase(0,18);

            // read in mjdtime
            stringstream ss;
            ss<<request;
            double timemjd;
            ss>>timemjd;
            //cout<<"timemjd = <"<<setprecision(numeric_limits<double>::digits10)<<timemjd<<">"<<endl;
            ///*
            // use mjd to calculate position
            cosmosstruc* c = agent->cinfo;
            // orbital equations

            c->set_PQW(timemjd);
            //cout<<setprecision(numeric_limits<double>::digits10)<<timemjd<<", "<<c->P_pos_t<<", "<<c->Q_pos_t<<", "<<c->W_pos_t<<", "<<c->P_vel_t<<", "<<c->Q_vel_t<<", "<<c->W_vel_t<<", "<<c->P_acc_t<<", "<<c->Q_acc_t<<", "<<c->W_acc_t<<endl;

            c->set_IJK_from_PQW();
            //cout<<setprecision(numeric_limits<double>::digits10)<<timemjd<<", "<<c->I_pos_t<<", "<<c->J_pos_t<<", "<<c->K_pos_t<<", "<<c->I_vel_t<<", "<<c->J_vel_t<<", "<<c->K_vel_t<<", "<<c->I_acc_t<<", "<<c->J_acc_t<<", "<<c->K_acc_t<<endl;

            /*
        // to find position and velocity at time t
            // 0    Make sure all necessary orbital elements are set
            c->t = timemjd;
            c->l = c->a*(1.0-pow(c->e,2.0));
            // 1    Calculate mean anamoly (M)
            c->M = fmod(c->n * (c->t - c->tau), 2*M_PI);
            // 2    Calculate true anamoly (v)
            c->v = c->M + (2.0*c->e-0.25*pow(c->e,3.0))*sin(c->M) + 1.25*pow(c->e,2.0)*sin(2.0*c->M) + (13.0/12.0)*pow(c->e,3.0)*sin(3.0*c->M);
            // 3    Calculate radius (r)
            c->r = c->l / (1.0 + c->e*cos(c->v));
            // 4    Calculate position vector <P_pos_t, Q_pos_t, W_pos_t>
            c->P_pos_t = c->r * cos(c->v);
            c->Q_pos_t = c->r * sin(c->v);
            c->W_pos_t = 0.0;
            c->P_vel_t = sqrt(c->mu/c->l) * -sin(c->v);
            //c->Q_vel_t = sqrt(c->mu/c->l) * (c->e+cos(c->v))*sin(c->v);
            c->Q_vel_t = sqrt(c->mu/c->l) * (c->e+cos(c->v));
            c->W_vel_t = 0.0;
*/

            response.clear();

            //response =    "\t\ttime:     [ " + request + " ]";
            response =    request + "\t";
            //response += "\n\t\tposition: [ ";
            stringstream sss;
            sss<<setprecision(numeric_limits<double>::digits10)<<c->P_pos_t;
            response += sss.str();
            sss.str("");
            //response += " ";
            response += "\t";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->Q_pos_t;
            response += sss.str();
            sss.str("");
            //response += " ";
            response += "\t";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->W_pos_t;
            response += sss.str();
            sss.str("");
            //response += " ]";
            response += "\t";

            // 5    Calculate velocity vector <P_vel_t, Q_vel_t, W_vel_t>
            //response += "\n\t\tvelocity: [ ";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->P_vel_t;
            response += sss.str();
            sss.str("");
            //response += " ";
            response += "\t";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->Q_vel_t;
            response += sss.str();
            sss.str("");
            //response += " ";
            response += "\t";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->W_vel_t;
            response += sss.str();
            sss.str("");
            //response += " ]";

            // 6    Transform perifocal (PQW) co-ords to geocentric equatorial (IJK) co-ords

            // also calculate distance from mothership?

            // ...

            //response = request;
            //cout<<"req_get_position():outgoing response         = <"<<response<<">"<<endl;
            return 0;
        }

        // request = "get_position mjdtime"
        int32_t Agent::req_get_position(string &request, string &response, Agent* agent)	{

            //cout<<"\tincoming request          = <"<<request<<">"<<endl;
            //cout<<"req_get_position():incoming request.size()   = "<<request.size()<<endl;
            //cout<<"req_get_position():incoming request.length() = "<<request.length()<<endl;

            // remove function call and space
            request.erase(0,13);

            // read in mjdtime
            stringstream ss;
            ss<<request;
            double timemjd;
            ss>>timemjd;
            //cout<<"timemjd = <"<<setprecision(numeric_limits<double>::digits10)<<timemjd<<">"<<endl;

            // use mjd to calculate position
            cosmosstruc* c = agent->cinfo;
            // orbital equations

            // to find position and velocity at time t
            // 0    Make sure all necessary orbital elements are set

            //JIMFIX:  this should actually be seconds into orbit for Kepler calcluation
            c->t = timemjd;
            c->l = c->a*(1.0-pow(c->e,2.0));
            // 1    Calculate mean anamoly (M)
            c->M = fmod(c->n * (c->t - c->tau), 2*M_PI);
            // 2    Calculate true anamoly (v)
            c->v = c->M + (2.0*c->e-0.25*pow(c->e,3.0))*sin(c->M) + 1.25*pow(c->e,2.0)*sin(2.0*c->M) + (13.0/12.0)*pow(c->e,3.0)*sin(3.0*c->M);
            // 3    Calculate radius (r)
            c->r = c->l / (1.0 + c->e*cos(c->v));
            // 4    Calculate position vector <P_pos_t, Q_pos_t, W_pos_t>
            c->P_pos_t = c->r * cos(c->v);
            c->Q_pos_t = c->r * sin(c->v);
            c->W_pos_t = 0.0;
            c->P_vel_t = sqrt(c->mu/c->l) * -sin(c->v);
            //c->Q_vel_t = sqrt(c->mu/c->l) * (c->e+cos(c->v))*sin(c->v);
            c->Q_vel_t = sqrt(c->mu/c->l) * (c->e+cos(c->v));
            c->W_vel_t = 0.0;

            response.clear();

            response =    "\t\ttime:     [ " + request + " ]";
            response += "\n\t\tposition: [ ";
            stringstream sss;
            sss<<setprecision(numeric_limits<double>::digits10)<<c->P_pos_t;
            response += sss.str();
            sss.str("");
            response += " ";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->Q_pos_t;
            response += sss.str();
            sss.str("");
            response += " ";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->W_pos_t;
            response += sss.str();
            sss.str("");
            response += " ]";

            // 5    Calculate velocity vector <P_vel_t, Q_vel_t, W_vel_t>
            response += "\n\t\tvelocity: [ ";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->P_vel_t;
            response += sss.str();
            sss.str("");
            response += " ";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->Q_vel_t;
            response += sss.str();
            sss.str("");
            response += " ";
            sss<<setprecision(numeric_limits<double>::digits10)<<c->W_vel_t;
            response += sss.str();
            sss.str("");
            response += " ]";

            // 6    Transform perifocal (PQW) co-ords to geocentric equatorial (IJK) co-ords

            // also calculate distance from mothership?

            // ...

            //response = request;
            //cout<<"req_get_position():outgoing response         = <"<<response<<">"<<endl;
            return 0;
        }



        //! Built-in Set Internal Value request
        /*! Sets the current value of the requested Name Space values. Names and values are expressed as a JSON object.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        int32_t Agent::req_setvalue(string &request, string &output, Agent* agent) {
            int32_t iretn = 0;
            iretn = json_parse(request, agent->cinfo);
            output = std::to_string(iretn);
            return(iretn);
        }

        /**
 * @brief Agent::req_set_value (Namespace 2.0)
 * @param request
 * @param response
 * @param agent
 * @return
 */
        int32_t Agent::req_set_value(string &request, string &response, Agent* agent) {
            // remove function call and space ('set_value ')
            request.erase(0,10);
            cout<<"req_set_value():incoming request          = <"<<request<<">"<<endl;
            cout<<"req_set_value():incoming request.size()   = "<<request.size()<<endl;
            cout<<"req_set_value():incoming request.length() = "<<request.length()<<endl;
            cout<<"req_set_value():incoming response         = <"<<response<<">"<<endl;

            // set the value from json string
            agent->cinfo->set_json(request);
            cout<<"req_set_value():outgoing response         = <"<<response<<">"<<endl;
            return 0;
        }

        //! Built-in List Name Space Names request
        /*! Returns a list of all names in the JSON Name Space.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_listnames(char *, char* output, Agent* agent)
        int32_t Agent::req_listnames(string &, string &output, Agent* agent) {
            output = json_list_of_all(agent->cinfo);
            if (output.length() > agent->cinfo->agent0.beat.bsz) { output[agent->cinfo->agent0.beat.bsz-1] = 0; }
            return 0;
        }

        //! Built-in Return Node JSON request
        /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_nodejson(char *, char* output, Agent* agent)
        int32_t Agent::req_nodejson(string &, string &output, Agent* agent) {
            output = agent->cinfo->json.node.c_str();
            if (output.length() > agent->cinfo->agent0.beat.bsz) { output[agent->cinfo->agent0.beat.bsz-1] = 0; }
            return 0;
        }

        //! Built-in Return State Vector JSON request
        /*! Returns a JSON string representing the State Vector.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_statejson(char *, char* output, Agent* agent)
        int32_t Agent::req_statejson(string &, string &output, Agent* agent) {
            output = agent->cinfo->json.state.c_str();
            if (output.length() > agent->cinfo->agent0.beat.bsz) { output[agent->cinfo->agent0.beat.bsz-1] = 0; }
            return 0;
        }

        //! Built-in Return UTC Start Time JSON request
        /*! Returns a JSON string representing the UTC Start Time.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //    int32_t Agent::req_utcstartjson(string &, string &output, Agent* agent) {
        //        output = agent->cinfo->json.utcstart.c_str();
        //        if (output.length() > agent->cinfo->agent0.beat.bsz) { output[agent->cinfo->agent0.beat.bsz-1] = 0; }
        //        return 0;
        //    }

        //! Built-in Return Pieces JSON request
        /*! Returns a JSON string representing the Piece information.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_piecesjson(char *, char* output, Agent* agent)
        int32_t Agent::req_piecesjson(string &, string &output, Agent* agent) {
            output = agent->cinfo->json.pieces.c_str();
            if (output.length() > agent->cinfo->agent0.beat.bsz) { output[agent->cinfo->agent0.beat.bsz-1] = 0; }
            return 0;
        }

        //! Built-in Return Face JSON request
        /*! Returns a JSON string representing the Face information.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_facesjson(char *, char* output, Agent* agent)
        int32_t Agent::req_facesjson(string &, string &output, Agent* agent) {
            output = agent->cinfo->json.faces.c_str();
            if (output.length() > agent->cinfo->agent0.beat.bsz) { output[agent->cinfo->agent0.beat.bsz-1] = 0; }
            return 0;
        }

        //! Built-in Return Vertex JSON request
        /*! Returns a JSON string representing the Vertex information.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_vertexsjson(char *, char* output, Agent* agent)
        int32_t Agent::req_vertexsjson(string &, string &output, Agent* agent) {
            output = agent->cinfo->json.vertexs.c_str();
            if (output.length() > agent->cinfo->agent0.beat.bsz) { output[agent->cinfo->agent0.beat.bsz-1] = 0; }
            return 0;
        }

        //! Built-in Return devgen JSON request
        /*! Returns a JSON string representing the generic device information.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_devgenjson(char *, char* output, Agent* agent)
        int32_t Agent::req_devgenjson(string &, string &output, Agent* agent) {
            output = agent->cinfo->json.devgen.c_str();
            if (output.length() > agent->cinfo->agent0.beat.bsz) { output[agent->cinfo->agent0.beat.bsz-1] = 0; }
            return 0;
        }

        //! Built-in Return devspec JSON request
        /*! Returns a JSON string representing the special device information.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_devspecjson(char *, char* output, Agent* agent)
        int32_t Agent::req_devspecjson(string &, string &output, Agent* agent) {
            output = agent->cinfo->json.devspec.c_str();
            if (output.length() > agent->cinfo->agent0.beat.bsz) { output[agent->cinfo->agent0.beat.bsz-1] = 0; }
            return 0;
        }

        //! Built-in Return Ports JSON request
        /*! Returns a JSON string representing the Port information.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_portsjson(char *, char* output, Agent* agent)
        int32_t Agent::req_portsjson(string &, string &output, Agent* agent) {
            //            strncpy(output, agent->cinfo->json.ports.c_str(), agent->cinfo->json.ports.size()<agent->cinfo->agent0.beat.bsz-1?agent->cinfo->json.ports.size():agent->cinfo->agent0.beat.bsz-1);
            //            output[agent->cinfo->agent0.beat.bsz-1] = 0;
            output = agent->cinfo->json.ports.c_str();
            if (output.length() > agent->cinfo->agent0.beat.bsz)
            {
                output[agent->cinfo->agent0.beat.bsz-1] = 0;
            }
            return 0;
        }

        //! Built-in Return Target JSON request
        /*! Returns a JSON string representing the Target information.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_targetsjson(char *, char* output, Agent* agent)
        int32_t Agent::req_targetsjson(string &, string &output, Agent* agent) {
            //            strncpy(output, agent->cinfo->json.targets.c_str(), agent->cinfo->json.targets.size()<agent->cinfo->agent0.beat.bsz-1?agent->cinfo->json.targets.size():agent->cinfo->agent0.beat.bsz-1);
            //            output[agent->cinfo->agent0.beat.bsz-1] = 0;
            output = agent->cinfo->json.targets.c_str();
            if (output.length() > agent->cinfo->agent0.beat.bsz)
            {
                output[agent->cinfo->agent0.beat.bsz-1] = 0;
            }
            return 0;
        }

        //! Built-in Return Alias JSON request
        /*! Returns a JSON string representing the alias information.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_aliasesjson(char *, char* output, Agent* agent)
        int32_t Agent::req_aliasesjson(string &, string & output, Agent* agent) {
            //            strncpy(output, agent->cinfo->json.aliases.c_str(), agent->cinfo->json.aliases.size()<agent->cinfo->agent0.beat.bsz-1?agent->cinfo->json.aliases.size():agent->cinfo->agent0.beat.bsz-1);
            output = agent->cinfo->json.aliases;
            if (output.size() > agent->cinfo->agent0.beat.bsz) {
                output[agent->cinfo->agent0.beat.bsz-1] = 0;
            }
            return 0;
        }

        //! Built-in Send Heartbeat request
        /*! Send a Heartbeat out of the regular time for heartbeats.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
        //        int32_t Agent::req_heartbeat(char *, char* output, Agent* agent)
        int32_t Agent::req_heartbeat(string &, string &output, Agent* agent) {
            //            output[0] = 0;
            output.clear();
            int32_t iretn = 0;
            iretn = agent->post_beat();
            return iretn;
        }

        //        int32_t Agent::req_postsoh(char *, char* output, Agent* agent)
        int32_t Agent::req_postsoh(string &, string &output, Agent* agent) {
            //            output[0] = 0;
            output.clear();
            int32_t iretn = 0;
            iretn = agent->post_soh();
            return iretn;
        }

        int32_t Agent::req_utc(string &, string &response, Agent *agent) {
            //            response =  " %.15g %lf ", agent->agent_time_producer(), utc2unixseconds(agent->agent_time_producer()));
            response = to_mjd(agent->agent_time_producer()) + ' ' + std::to_string(utc2unixseconds(agent->agent_time_producer()));
            return 0;
        }

        int32_t Agent::req_soh(string &, string &response, Agent *agent) {
            // Return Namespace 1.0 soh
            if(!agent->sohtable.empty()) {
                string rjstring;
                response = json_of_table(rjstring, agent->sohtable, agent->cinfo);
            }
            // Return Namespace 2.0 soh
            else {
                string jsonlist = "{";
                // Iterate through list of names, get json for each name if it's in Namespace 2.0
                for(string name: agent->sohstring) {
                    string jsonname = agent->cinfo->get_json(name);
                    if(jsonname.size() > 1) {
                        // Trim beginning and ending curly braces
                        jsonname = jsonname.substr(1, jsonname.size()-2);
                        jsonlist += jsonname + ",";
                    }
                }
                if(jsonlist.size() > 1) {
                    jsonlist.pop_back(); // remove last ","
                    jsonlist += "}";
                } else {
                    jsonlist = "";
                }
                response = jsonlist;
            }
            return 0;
        }

        int32_t Agent::req_fullsoh(string &, string &response, Agent *agent) {
            string rjstring;
            response = json_of_table(rjstring, agent->fullsohtable, agent->cinfo);
            return 0;
        }

        //        int32_t Agent::req_jsondump(char *, char*, Agent *agent)
        int32_t Agent::req_jsondump(string &, string &, Agent *agent) {
            json_dump_node(agent->cinfo);
            return 0;
        }

        // Return raw text of all names and associated types in agent's namespace
        // Format is:
        // name,\ttype\nname,\ttype ...etc
        int32_t Agent::req_all_names_types(string &, string &response, Agent *agent)
        {
            response = "";
            map<string,void*>::const_iterator n = agent->cinfo->names.begin();
            while(n != agent->cinfo->names.end())	{
                map<string,string>::const_iterator t = agent->cinfo->types.find(n->first);
                if(t == agent->cinfo->types.end())	{
                    response += (n++)->first + "\n";
                } else {
                    response += (n++)->first + ",\t" + t->second + "\n";
                }
            }
            // Remove last \n character
            if(response.size() > 1) {
                response.pop_back();
            }

            return 0;
        }


        //! Send PacketComm command over requested radio
        /*! Package up the specified command in a PacketComm packet. Place it on the queue for the
         * specified radio.
     * \param request Text of request.
     * \param output Text of response to request.
     * \param agent Pointer to Cosmos::Agent to use.
     * \return 0, or negative error.
     */
        int32_t Agent::req_command(string &request, string &response, Agent *agent)
        {
            PacketComm packet;
            string type = "Ping";
            string dest = "iobc";
            int32_t outchannel = 0;
            int32_t inchannel = 0;
            vector<string> parms;
            string parmstring;
            uint16_t repeat = 1;

            vector<string> args = string_split(request);
            if (args.size() > 1)
            {
                if (lookup_node_id(agent->cinfo, args[1]) != NODEIDUNKNOWN)
                {
                    dest = args[1];
                }
                if (args.size() > 2)
                {
                    vector<string> radios = string_split(args[2], ":");
                    if (radios.size() > 0)
                    {
                        outchannel = agent->channel_number(radios[0]);
                        if (outchannel < 0)
                        {
                            outchannel = 0;
                        }
                        if (radios.size() > 1)
                        {
                            inchannel = agent->channel_number(radios[1]);
                            if (inchannel < 0)
                            {
                                inchannel = 0;
                            }
                            if (radios.size() > 2)
                            {
                                repeat = stoi(radios[2]);
                            }
                        }
                    }
                    if (args.size() > 3)
                    {
                        if (packet.StringType.find(args[3]) != packet.StringType.end())
                        {
                            type = args[3];
                        }
                        if (args.size() > 4)
                        {
                            for (uint16_t i=4; i<args.size(); ++i)
                            {
                                parms.push_back(args[i]);
                            }
                            parmstring = request.substr(string_find(request, " ", 4));
                        }
                    }
                }
            }

            if (packet.StringType.find(type) == packet.StringType.end())
            {
                response = type;
                return GENERAL_ERROR_OUTOFRANGE;
            }
            packet.header.type = packet.StringType[type];
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = lookup_node_id(agent->cinfo, dest);
            packet.header.chanin = inchannel;
            packet.header.chanout = outchannel;
            response = "dest:" + dest + " radioup:" + agent->channels.channel[outchannel].name + " radiodown:" + agent->channels.channel[inchannel].name + " " + type;
            switch (packet.header.type)
            {
            case PacketComm::TypeId::CommandObcReset:
                packet.data.resize(8, 0);
                uint32to(agent->get_verification(), &packet.data[0], ByteOrder::LITTLEENDIAN);
                if (parms.size() > 0)
                {
                    uint32_t seconds = stoi(parms[0]);
                    uint32to(seconds, &packet.data[4], ByteOrder::LITTLEENDIAN);
                }
                break;
            case PacketComm::TypeId::CommandObcReboot:
                packet.data.resize(8, 0);
                uint32to(agent->get_verification(), &packet.data[0], ByteOrder::LITTLEENDIAN);
                if (parms.size() > 0)
                {
                    uint32_t seconds = stoi(parms[0]);
                    uint32to(seconds, &packet.data[4], ByteOrder::LITTLEENDIAN);
                }
                break;
            case PacketComm::TypeId::CommandObcHalt:
                packet.data.resize(8, 0);
                uint32to(agent->get_verification(), &packet.data[0], ByteOrder::LITTLEENDIAN);
                if (parms.size() > 0)
                {
                    uint32_t seconds = stoi(parms[0]);
                    uint32to(seconds, &packet.data[4], ByteOrder::LITTLEENDIAN);
                }
                break;
            case PacketComm::TypeId::CommandObcSendBeacon:
                {
                    uint8_t btype = (uint8_t)Beacon::TypeId::CPU1BeaconS;
                    uint8_t bcount = 1;
                    if (parms.size() > 0)
                    {
                        Beacon tbeacon;
                        for (auto type : tbeacon.TypeString)
                        {
                            if (type.second == parms[0])
                            {
                                btype = (uint8_t)type.first;
                                break;
                            }
                        }
                        if (parms.size() > 1)
                        {
                            bcount = stoi(parms[1]);
                        }
                    }
                    packet.data.resize(2);
                    packet.data[0] = btype;
                    packet.data[1] = bcount;
                    response += " " + to_unsigned(packet.data[0]) + " " + to_unsigned(packet.data[1]);
                }
                break;
            case PacketComm::TypeId::CommandExecClearQueue:
                {
                    packet.data.resize(4, 0);
                    uint32to(agent->get_verification(), &packet.data[0], ByteOrder::LITTLEENDIAN);
                    packet.data[4] = 0;
                    if (parms.size() > 0)
                    {
                        packet.data[4] = stoi(parms[1]);
                    }
                    response += " " + to_hex(agent->get_verification()) + " " + to_unsigned(packet.data[4]);
                }
                break;
            case PacketComm::TypeId::CommandObcExternalCommand:
            case PacketComm::TypeId::CommandObcExternalTask:
                {
                    string command = "uptime";
                    if (parms.size() > 0)
                    {
                        command = parms[0];
                        if (parms.size() > 1)
                        {
                            for (uint16_t i=1; i<parms.size(); ++i)
                            {
                                command += " ";
                                command += parms[i];
                            }
                        }
                    }
                    packet.data.resize(4);
                    uint32_t centi = centisec();
                    uint32to(centi, &packet.data[0], ByteOrder::LITTLEENDIAN);
                    packet.data.insert(packet.data.end(), command.begin(), command.end());
                    response += " " + to_unsigned(centi) + " " + command;
                }
                break;

            case PacketComm::TypeId::CommandRadioAstrodevCommunicate:
                packet.data.clear();
                try
                {
                    // String args are converted to uint8_t and pushed straight in
                    for (size_t i=0; i<parms.size(); ++i)
                    {
                        packet.data.push_back(std::stoi(parms[i]) & 0xff);
                    }
                }
                catch (const std::invalid_argument& e)
                {
                    response += "Caught invalid_argument exception";
                    break;
                }
                catch (const std::out_of_range& e)
                {
                    response += "Caught out_of_range exception";
                    break;
                }
                break;
            case PacketComm::TypeId::CommandRadioTest:
                {
                    uint8_t start = 0;
                    uint8_t step = 1;
                    uint8_t count = 255;
                    uint32_t bytes = 4000;
                    if (parms.size() == 4)
                    {
                        start = stoi(parms[0]);
                        step = stoi(parms[1]);
                        count = stoi(parms[2]);
                        bytes = stoi(parms[3]);
                    }
                    packet.data.resize(11);
                    packet.data[0] = start;
                    packet.data[1] = step;
                    packet.data[2] = count;
                    uint32_t test_id = centisec();
                    uint32to(test_id, &packet.data[3], ByteOrder::LITTLEENDIAN);
                    uint32to(bytes, &packet.data[7], ByteOrder::LITTLEENDIAN);
                    response += " " + to_unsigned(test_id)
                                + " " + to_unsigned(start)
                                + " " + to_unsigned(step)
                                + " " + to_unsigned(count)
                                +  " " + to_unsigned(bytes)
                                ;
                }
                break;
            case PacketComm::TypeId::CommandFileListDirectory:
                {
                    string node = "any";
                    string agent = "any";
                    if (parms.size() > 0)
                    {
                        node = parms[0];
                        if (parms.size() > 1)
                        {
                            agent = parms[1];
                        }
                    }
                    packet.data.resize(4);
                    uint32to(centisec(), &packet.data[0], ByteOrder::LITTLEENDIAN);
                    packet.data.push_back((uint8_t)node.size());
                    packet.data.insert(packet.data.end(), node.begin(), node.end());
                    packet.data.push_back((uint8_t)agent.size());
                    packet.data.insert(packet.data.end(), agent.begin(), agent.end());
                }
                break;
            case PacketComm::TypeId::CommandFileTransferFile:
                {
                    string node = agent->cinfo->node.name;
                    string agentname = agent->cinfo->agent0.name;
                    string file = "";
                    if (parms.size() > 0)
                    {
                        node = parms[0];
                        if (parms.size() > 1)
                        {
                            agentname = parms[1];
                            if (parms.size() > 2)
                            {
                                file = parms[2];
                            }
                        }
                    }
                    packet.data.resize(1);
                    packet.data[0] = parms[0].size();

                    packet.data.push_back((uint8_t)node.size());
                    packet.data.insert(packet.data.end(), node.begin(), node.end());
                    packet.data.push_back((uint8_t)agentname.size());
                    packet.data.insert(packet.data.end(), agentname.begin(), agentname.end());
                    packet.data.push_back((uint8_t)file.size());
                    packet.data.insert(packet.data.end(), file.begin(), file.end());
                }
                break;
            case PacketComm::TypeId::CommandFileTransferNode:
                {
                    string node = agent->cinfo->node.name;
                    if (parms.size() > 0)
                    {
                        node = parms[0];
                    }
                    packet.data.resize(1);
                    packet.data[0] = parms[0].size();

                    packet.data.push_back((uint8_t)node.size());
                    packet.data.insert(packet.data.end(), node.begin(), node.end());
                }
                break;
            case PacketComm::TypeId::CommandFileTransferRadio:
                {
                    if (parms.size() < 2)
                    {
                        response = "Invalid arguments";
                        return response.size();
                    }
                    int32_t ch = agent->channel_number(parms[0]);
                    if (ch < 0)
                    {
                        response = "Invalid channel";
                        return response.size();
                    }
                    packet.data.resize(2);
                    packet.data[0] = ch & 0xFF;
                    try
                    {
                        packet.data[1] = stoi(parms[1]);
                        response = "Setting radio " + std::to_string(unsigned(packet.data[0])) + " to " + std::to_string(unsigned(packet.data[1]));
                    }
                    catch(const std::exception& e)
                    {
                        response = "Invalid parm[1]";
                        return response.size();
                    }
                }
                break;
            case PacketComm::TypeId::CommandObcInternalRequest:
                {
                    string command = "status";
                    if (parms.size() > 0)
                    {
                        command = parms[0];
                        if (parms.size() > 1)
                        {
                            for (uint16_t i=1; i<parms.size(); ++i)
                            {
                                command += " ";
                                command += parms[i];
                            }
                        }
                    }
                    packet.data.resize(4);
                    uint32_t centi = centisec();
                    uint32to(centi, &packet.data[0], ByteOrder::LITTLEENDIAN);
                    packet.data.insert(packet.data.end(), command.begin(), command.end());
                    response += " " + to_unsigned(centi) + " " + command;
                }
                break;
            case PacketComm::TypeId::CommandObcPing:
                {
                    string ping = utc2iso8601(currentmjd());
                    if (parms.size() > 0)
                    {
                        ping = parms[0];
                    }
                    packet.data.resize(4);
                    uint32_t centi = centisec();
                    uint32to(centi, &packet.data[0], ByteOrder::LITTLEENDIAN);
                    packet.data.insert(packet.data.end(), ping.begin(), ping.end());
                    response += " " + to_unsigned(centi) + " " + ping;
                }
                break;
            case PacketComm::TypeId::CommandObcSetTime:
                {
                    double mjd = currentmjd();
                    float limit = 0.;
                    uint16_t bootseconds = 0;
                    if (parms.size() > 0)
                    {
                        if (parms[0][0] == '+' || parms[0][0] == '-')
                        {
                            mjd += atof(parms[0].c_str()) / 86400.;
                            response += " by " + to_floating(atof(parms[0].c_str()) / 86400., .1);
                        }
                        else
                        {
                            mjd = atof(parms[0].c_str());
                        }
                        if (parms.size() > 1)
                        {
                            limit = atof(parms[1].c_str());
                        }
                        if (parms.size() > 2)
                        {
                            bootseconds = atoi(parms[2].c_str());
                        }
                    }
                    packet.data.resize(14);
                    doubleto(mjd, &packet.data[0], ByteOrder::LITTLEENDIAN);
                    floatto(limit, &packet.data[8]);
                    uint16to(bootseconds, &packet.data[12]);
                    response += " " + to_iso8601(mjd) + " " + to_floating(limit, 1) + " " + to_unsigned(bootseconds);
                }
                break;
            case PacketComm::TypeId::CommandObcGetTimeHuman:
            case PacketComm::TypeId::CommandObcGetTimeBinary:
                {
                }
                break;
            case PacketComm::TypeId::CommandExecSetOpsMode:
                {
                    packet.data.clear();
                    if (parms.size())
                    {
                        packet.data.insert(packet.data.end(), parms[0].begin(), parms[0].end());
                    }
                }
                break;
            case PacketComm::TypeId::CommandExecAddCommand:
                {
                    packet.data.clear();
                }
                if (parmstring.size())
                {
                    packet.data.insert(packet.data.end(), parmstring.begin(), parmstring.end());
                }
                break;
            case PacketComm::TypeId::CommandExecEnableChannel:
                {
                    packet.data.clear();
                    if (parms.size() > 0)
                    {
                        packet.data.resize(1);
                        if (parms.size() > 1)
                        {
                            packet.data[0] = stoi(parms[1]);
                        }
                        else
                        {
                            packet.data[0] = 0;
                        }
                        if (isdigit(parms[0][0]))
                        {
                            packet.data[1] = stoi(parms[0]);
                            response += " " + to_unsigned(packet.data[1]) + " " + to_unsigned(packet.data[0]);
                        }
                        else
                        {
                            packet.data.insert(packet.data.end(), parms[0].begin(), parms[0].end());
                            response += " " + parms[0] + " " + to_unsigned(packet.data[0]);
                        }
                    }
                }
                break;
            case PacketComm::TypeId::CommandEpsCommunicate:
                {
                    if (parms.size())
                    {
                        parms = string_split(parms[0], ":", false);
                        if (parms.size() > 2)
                        {
                            PacketComm::CommunicateHeader header;
                            header.unit = stoi(parms[0]);
                            header.command = stoi(parms[1]);
                            header.responsecount = stoi(parms[2]);
                            packet.data.resize(4);
                            memcpy(packet.data.data(), &header, sizeof(header));
                            if (parms.size() > 3)
                            {
                                vector<uint8_t> bytes = from_hex_string(parms[3]);
                                packet.data.insert(packet.data.end(), bytes.begin(), bytes.end());
                            }
                        }
                    }
                }
                break;
            case PacketComm::TypeId::CommandAdcsOrbitParameters:
                {
                    double d1 = 0.0;
                    double d2 = 0.0;
                    double d3 = 0.0;
                    double d4 = 0.0;
                    double d5 = 0.0;
                    double d6 = 0.0;
                    double d7 = 0.0;
                    double d8 = 0.0;

                    if(parms.size() == 8)	{
                        d1 = stod(parms[0]);
                        const unsigned char* ptr = reinterpret_cast<const unsigned char*>(&d1);
                        for (size_t i = 0; i < sizeof(double); ++i)	{ packet.data.push_back(ptr[i]); }

                        d2 = stod(parms[1]);
                        ptr = reinterpret_cast<const unsigned char*>(&d2);
                        for (size_t i = 0; i < sizeof(double); ++i)	{ packet.data.push_back(ptr[i]); }

                        d3 = stod(parms[2]);
                        ptr = reinterpret_cast<const unsigned char*>(&d3);
                        for (size_t i = 0; i < sizeof(double); ++i)	{ packet.data.push_back(ptr[i]); }

                        d4 = stod(parms[3]);
                        ptr = reinterpret_cast<const unsigned char*>(&d4);
                        for (size_t i = 0; i < sizeof(double); ++i)	{ packet.data.push_back(ptr[i]); }

                        d5 = stod(parms[4]);
                        ptr = reinterpret_cast<const unsigned char*>(&d5);
                        for (size_t i = 0; i < sizeof(double); ++i)	{ packet.data.push_back(ptr[i]); }

                        d6 = stod(parms[5]);
                        ptr = reinterpret_cast<const unsigned char*>(&d6);
                        for (size_t i = 0; i < sizeof(double); ++i)	{ packet.data.push_back(ptr[i]); }

                        d7 = stod(parms[6]);
                        ptr = reinterpret_cast<const unsigned char*>(&d7);
                        for (size_t i = 0; i < sizeof(double); ++i)	{ packet.data.push_back(ptr[i]); }

                        d8 = stod(parms[7]);
                        ptr = reinterpret_cast<const unsigned char*>(&d8);
                        for (size_t i = 0; i < sizeof(double); ++i)	{ packet.data.push_back(ptr[i]); }
                    }
                }
                break;
            case PacketComm::TypeId::CommandAdcsState:
                {
                    packet.data.resize(1);
                    packet.data[0] = stoi(parms[0]);
                    for (uint16_t i=1; i<parms.size(); ++i)
                    {
                        string arg = parms[i] + " ";
                        packet.data.insert(packet.data.end(), arg.begin(), arg.end());
                    }
                }
                break;
            case PacketComm::TypeId::CommandAdcsCommunicate:
                {
                    if (parms.size())
                    {
                        parms = string_split(parms[0], ":", false);
                        if (parms.size() > 2)
                        {
                            PacketComm::CommunicateHeader header;
                            header.unit = stoi(parms[0]);
                            header.command = stoi(parms[1]);
                            header.responsecount = stoi(parms[2]);
                            packet.data.resize(4);
                            memcpy(packet.data.data(), &header, sizeof(header));
                            if (parms.size() > 3)
                            {
                                vector<uint8_t> bytes = from_hex_string(parms[3]);
                                packet.data.insert(packet.data.end(), bytes.begin(), bytes.end());
                            }
                        }
                    }
                }
                break;
            case PacketComm::TypeId::CommandEpsSwitchName:
                {
                    if (parms.size())
                    {
                        packet.data.resize(1);
                        if (parms.size() > 1)
                        {
                            packet.data[0] = stoi(parms[1]);
                        }
                        else
                        {
                            packet.data[0] = 1;
                        }
                        packet.data.insert(packet.data.end(), parms[0].begin(), parms[0].end());
                    }
                }
                break;
            case PacketComm::TypeId::CommandEpsSwitchNumber:
                {
                    if (parms.size() > 1)
                    {
                        packet.data.resize(3);
                        if (parms.size() > 2)
                        {
                            packet.data[0] = stoi(parms[2]);
                        }
                        else
                        {
                            packet.data[0] = 1;
                        }
                        packet.data[1] = stoi(parms[0]);
                        packet.data[2] = stoi(parms[1]);
                        //                        uint16to(stoi(parms[0]), &packet.data[1], ByteOrder::LITTLEENDIAN);
                    }
                }
                break;
            case PacketComm::TypeId::CommandEpsReset:
                {
                    packet.data.resize(2);
                    uint16to(stoi(parms[0]), &packet.data[0], ByteOrder::LITTLEENDIAN);
                }
                break;
            case PacketComm::TypeId::CommandEpsState:
                {
                    packet.data.resize(1);
                    packet.data[0] = stoi(parms[0]);
                }
                break;
            case PacketComm::TypeId::CommandEpsWatchdog:
                {
                    packet.data.resize(2);
                    uint16to(stoi(parms[0]), &packet.data[0], ByteOrder::LITTLEENDIAN);
                }
                break;
            case PacketComm::TypeId::CommandEpsSetTime:
                {
                    double mjd = currentmjd();
                    if (parms.size())
                    {
                        mjd = stof(parms[0]);
                    }
                    packet.data.resize(8);
                    doubleto(mjd, &packet.data[0], ByteOrder::LITTLEENDIAN);
                    break;
                }
            case PacketComm::TypeId::CommandEpsSwitchNames:
                {
                    string names = string_join(parms, " ");
                    packet.data.insert(packet.data.end(), names.begin(), names.end());
                }
                break;
            case PacketComm::TypeId::CommandEpsSwitchStatus:
                {
                    packet.data.clear();
                }
                break;
            case PacketComm::TypeId::CommandCameraOn:
                {
                    packet.data.resize(1);
                    if (parms.size())
                    {
                        packet.data[0] = atoi(parms[0].c_str());
                    }
                    else
                    {
                        packet.data[0] = 0;
                    }
                }
                break;
            case PacketComm::TypeId::CommandCameraCapture:
                {
                    packet.data.resize(12);
                    uint16to(1, &packet.data[0], ByteOrder::LITTLEENDIAN);
                    uint16to(350, &packet.data[2], ByteOrder::LITTLEENDIAN);
                    uint16to(0, &packet.data[4], ByteOrder::LITTLEENDIAN);
                    uint16to(640, &packet.data[6], ByteOrder::LITTLEENDIAN);
                    uint16to(0, &packet.data[8], ByteOrder::LITTLEENDIAN);
                    uint16to(512, &packet.data[10], ByteOrder::LITTLEENDIAN);
                    if (parms.size() > 0)
                    {
                        uint16to(atoi(parms[0].c_str()), &packet.data[0], ByteOrder::LITTLEENDIAN);
                        if (parms.size() > 1)
                        {
                            uint16to(atoi(parms[1].c_str()), &packet.data[2], ByteOrder::LITTLEENDIAN);
                            if (parms.size() > 3)
                            {
                                uint16to(atoi(parms[2].c_str()), &packet.data[4], ByteOrder::LITTLEENDIAN);
                                uint16to(atoi(parms[3].c_str()), &packet.data[6], ByteOrder::LITTLEENDIAN);
                                if (parms.size() > 5)
                                {
                                    uint16to(atoi(parms[4].c_str()), &packet.data[8], ByteOrder::LITTLEENDIAN);
                                    uint16to(atoi(parms[5].c_str()), &packet.data[10], ByteOrder::LITTLEENDIAN);
                                    if (parms.size() > 6)
                                    {
                                        packet.data.insert(packet.data.end(), parms[6].begin(), parms[6].end());
                                    }
                                }
                            }
                        }
                    }
                }
                break;
            default:
                return 0;
                break;
            }

            for (uint16_t i=0; i<repeat; ++i)
            {
//                agent->channel_push(outchannel, packet);
                agent->channel_push(0, packet);
                secondsleep(.1);
            }
            return response.length();
        }

        //! Run immediate command
        /*! Run the specified command with specified parameters and return the response.
     * \param request Text of request.
     * \param output Text of response to request.
     * \param agent Pointer to Cosmos::Agent to use.
     * \return 0, or negative error.
     */
        int32_t Agent::req_run_command(string &request, string &response, Agent *agent)
        {
            if (request.find(" ") == string::npos)
            {
                return GENERAL_ERROR_ARGS;
            }

            request.erase(0, request.find(" ")+1);
            return data_execute(request, response);
        }

        //! Launch background Task
        /*! Run the specified command with specified parameters in an external shell and save the response
         * to a file.
     * \param request Text of request.
     * \param output Text of response to request.
     * \param agent Pointer to Cosmos::Agent to use.
     * \return 0, or negative error.
     */
        int32_t Agent::req_add_task(string &request, string &response, Agent *agent)
        {
            if (request.find(" ") == string::npos)
            {
                return GENERAL_ERROR_ARGS;
            }

            request.erase(0, request.find(" ")+1);
            int32_t iretn = agent->task_add(request);
            response = "Task: " + agent->task_command(iretn) + " in " + agent->task_path(iretn) + " #" + to_unsigned(agent->task_size());
            return iretn;
        }

        //! List current channels
        /*! Provide a detailed list of all the Channels currently defined
     * \return 0, or negative error.
     */
        int32_t Agent::req_list_channels(string &request, string &response, Agent *agent)
        {
            response.clear();
            for (uint16_t i=0; i<agent->channels.channel.size(); ++i)
            {
                response += to_label("Name", agent->channel_name(i));
                response += " " + to_label("Number", i);
                response += " " + to_label("Age", agent->channel_age(i));
                response += " " + to_label("Size", agent->channel_size(i));
                response += "\n";
            }
            return response.size();
        }

        //! \brief Run a performance test on requested channel.
        //! \param request Request.
        //! \param response Any response.
        //! \param agent Pointer to Agent.
        //! \return Zero or negative error.
        int32_t Agent::req_test_channel(string &request, string &response, Agent *agent)
        {
            vector<string> args = string_split(request);
            if (args.size() > 3)
            {
                uint8_t start = 0;
                uint8_t step = 1;
                uint8_t stop = 255;
                uint32_t total = 0;

                uint8_t channel = agent->channel_number(args[1]);
                total = agent->channel_speed(channel) * 10.;
                uint8_t radio = agent->channel_number(args[2]);
                uint8_t dest = lookup_node_id(agent->cinfo, args[3]);
                uint8_t orig = agent->nodeId;
                if (args.size() > 6)
                {
                    start =  stoi(args[4]);
                    step =  stoi(args[5]);
                    stop =  stoi(args[6]);
                    if (args.size() > 7)
                    {
                        total = stoi(args[7]);
                    }
                }
                uint32_t id = centisec();
                agent->channel_teststart(channel, radio, id, orig, dest, start, step, stop, total);
                response = "Test:";
                response += to_label(" Channel", channel);
                response += to_label(" Radio", radio);
                response += to_label(" Orig", orig);
                response += to_label(" Dest", dest);
                response += to_label(" Start", start);
                response += to_label(" Step", step);
                response += to_label(" Stop", stop);
                response += to_label(" Total", total);
                return 0;
            }
            else
            {
                return GENERAL_ERROR_ARGS;
            }
        }

        //! \brief Set the specified channel to the specified state
        //! Expected args:
        //! [0] channel_enable
        //! [1] String name of channel
        //! [2] int8_t value to set channel enabled status to
        //! \param request Request.
        //! \param response Any response.
        //! \param agent Pointer to Agent.
        //! \return Zero or negative error.
        int32_t Agent::req_channel_enable(string &request, string &response, Agent *agent)
        {
            // Expected args:
            vector<string> args = string_split(request);
            if (args.size() < 3)
            {
                response = "Incorrect args";
                return 0;
            }
            try
            {
                int8_t new_state = std::stoi(args[2]) & 0xff;
                agent->channel_enable(args[1], new_state);
                response = "Set channel " + args[1] + " to " + std::to_string(agent->channel_enabled(args[1]));
                return 0;
            }
            catch(...)
            {
                response = "Incorrect args";
                return 0;
            }
        }

        //! \brief Reset the Age of the specified channel to the specifed value.
        //! Expected args:
        //! [0] channel_enable
        //! [1] String name of channel
        //! [2] float time in seconds to set channel age to
        //! \param request Request.
        //! \param response Any response.
        //! \param agent Pointer to Agent.
        //! \return Zero or negative error.
        int32_t Agent::req_channel_touch(string &request, string &response, Agent *agent)
        {
            // Expected args:
            vector<string> args = string_split(request);
            if (args.size() < 2)
            {
                response = "Incorrect args";
                return 0;
            }
            if (args.size() > 2)
            {
                double seconds = atof(args[2].c_str());
                agent->channel_touch(args[1], seconds);
                response = "Set channel " + args[1] + " age to " + std::to_string(agent->channel_age(args[1]));
            }
            else
            {
                agent->channel_touch(args[1]);
                response = "Set channel " + args[1] + " age to " + std::to_string(agent->channel_age(args[1]));
            }
            return 0;
        }

        //! Open COSMOS output channel
        /*! Establish a multicast socket for publishing COSMOS messages using the specified address and
 * port.
 * \param type One of ::NetworkType.
 * \param port Port number to publish on.
 * \return 0, otherwise negative error.
*/
        int32_t Agent::publish(NetworkType type, uint16_t port)
        {
            int32_t iretn = 0;
            int on = 1;

            // Return immediately if we've already done this
            if (cinfo->agent0.pub[0].cport)
                return 0;

            switch (type)
            {
            case NetworkType::MULTICAST:
            case NetworkType::UDP:
            case NetworkType::BROADCAST:
                {
                    for (uint32_t i=0; i<AGENTMAXIF; i++) { cinfo->agent0.pub[i].cudp = -1; }

                    if ((cinfo->agent0.pub[0].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0) { return (AGENT_ERROR_SOCKET); }

#ifdef COSMOS_WIN_OS
                    u_long nonblocking = 1;;
                    if (ioctlsocket(cinfo->agent0.pub[0].cudp, FIONBIO, &nonblocking) != 0) { iretn = -WSAGetLastError(); }
#else
                    if (fcntl(cinfo->agent0.pub[0].cudp, F_SETFL,O_NONBLOCK) < 0) { iretn = -errno; }
#endif
                    if (iretn < 0) {
                        CLOSE_SOCKET(cinfo->agent0.pub[0].cudp);
                        cinfo->agent0.pub[0].cudp = iretn;
                        return iretn;
                    }

                    // Use above socket to find available interfaces and establish
                    // publication on each.
                    cinfo->agent0.ifcnt = 0;

#if defined(COSMOS_WIN_OS)
                    struct sockaddr_storage ss;
                    int sslen;
                    INTERFACE_INFO ilist[20];
                    unsigned long nbytes;
                    uint32_t nif;
                    if (WSAIoctl(cinfo->agent0.pub[0].cudp, SIO_GET_INTERFACE_LIST, 0, 0, &ilist,sizeof(ilist), &nbytes, 0, 0) == SOCKET_ERROR) {
                        CLOSE_SOCKET(cinfo->agent0.pub[0].cudp);
                        return (AGENT_ERROR_DISCOVERY);
                    }

                    nif = nbytes / sizeof(INTERFACE_INFO);
                    for (uint32_t i=0; i<nif; i++) {
                        inet_ntop(ilist[i].iiAddress.AddressIn.sin_family,&ilist[i].iiAddress.AddressIn.sin_addr,cinfo->agent0.pub[cinfo->agent0.ifcnt].address,sizeof(cinfo->agent0.pub[cinfo->agent0.ifcnt].address));
                        //            strcpy(cinfo->agent0.pub[cinfo->agent0.ifcnt].address,inet_ntoa(((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr));
                        if (!strcmp(cinfo->agent0.pub[cinfo->agent0.ifcnt].address,"127.0.0.1")) {
                            if (cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp >= 0) {
                                CLOSE_SOCKET(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp);
                                cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp = -1;
                            }
                            continue;
                        }

                        // No need to open first socket again
                        if (cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp < 0) {
                            if ((cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
                                continue;
                            }
                            u_long nonblocking = 1;
                            if (ioctlsocket(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp, FIONBIO, &nonblocking) != 0) {
                                continue;
                            }
                        }

                        memset(&cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr,0,sizeof(struct sockaddr_in));
                        cinfo->agent0.pub[i].caddr.sin_family = AF_INET;
                        cinfo->agent0.pub[i].baddr.sin_family = AF_INET;
                        if (type == NetworkType::MULTICAST) {
                            sslen = sizeof(ss);
                            WSAStringToAddressA((char *)AGENTMCAST,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
                            cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                            cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                        } else {
                            if ((iretn = setsockopt(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                            {
                                CLOSE_SOCKET(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp);
                                continue;
                            }

                            cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
                            cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;

                            uint32_t ip, net, bcast;
                            ip = ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.S_un.S_addr;
                            net = ((struct sockaddr_in*)&(ilist[i].iiNetmask))->sin_addr.S_un.S_addr;
                            bcast = ip | (~net);
                            cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr.S_un.S_addr = bcast;
                        }
                        cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_port = htons(port);
                        cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_port = htons(port);
                        cinfo->agent0.pub[cinfo->agent0.ifcnt].type = type;
                        cinfo->agent0.pub[cinfo->agent0.ifcnt].cport = port;
                        cinfo->agent0.ifcnt++;
                    }
#elif defined(COSMOS_MAC_OS)
                    struct ifaddrs *if_addrs = NULL;
                    struct ifaddrs *if_addr = NULL;
                    if (0 == getifaddrs(&if_addrs)) {
                        for (if_addr = if_addrs; if_addr != NULL; if_addr = if_addr->ifa_next) {
                            if (if_addr->ifa_addr->sa_family != AF_INET) { continue; }
                            inet_ntop(if_addr->ifa_addr->sa_family,&((struct sockaddr_in*)if_addr->ifa_addr)->sin_addr,cinfo->agent0.pub[cinfo->agent0.ifcnt].address,sizeof(cinfo->agent0.pub[cinfo->agent0.ifcnt].address));
                            memcpy((char *)&cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr, (char *)if_addr->ifa_addr, sizeof(if_addr->ifa_addr));

                            if ((if_addr->ifa_flags & IFF_POINTOPOINT) || (if_addr->ifa_flags & IFF_UP) == 0 || (if_addr->ifa_flags & IFF_LOOPBACK) || (if_addr->ifa_flags & (IFF_BROADCAST)) == 0)
                            {
                                continue;
                            }

                            // No need to open first socket again
                            if (cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp < 0) {
                                if ((cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
                                    continue;
                                }

                                if (fcntl(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp, F_SETFL,O_NONBLOCK) < 0) {
                                    iretn = -errno;
                                    CLOSE_SOCKET(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp);
                                    cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp = iretn;
                                    continue;
                                }
                            }

                            if (type == NetworkType::MULTICAST) {
                                inet_pton(AF_INET,AGENTMCAST,&cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_addr);
                                inet_pton(AF_INET,AGENTMCAST,&cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr);
                            } else {
                                if ((iretn = setsockopt(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0) {
                                    CLOSE_SOCKET(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp);
                                    continue;
                                }

                                //                    if (ioctl(cinfo->agent0.pub[0].cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                                //                    {
                                //                        continue;
                                //                    }
                                //                    cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr = cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr;
                                memcpy((char *)&cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr, (char *)if_addr->ifa_netmask, sizeof(if_addr->ifa_netmask));

                                uint32_t ip, net, bcast;
                                ip = cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_addr.s_addr;
                                net = cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr.s_addr;
                                bcast = ip | (~net);
                                cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr.s_addr = bcast;
                                inet_ntop(if_addr->ifa_netmask->sa_family,&cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr,cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress,sizeof(cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress));
                            }
                            cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_port = htons(port);
                            cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_port = htons(port);
                            cinfo->agent0.pub[cinfo->agent0.ifcnt].type = type;
                            cinfo->agent0.pub[cinfo->agent0.ifcnt].cport = port;
                            cinfo->agent0.ifcnt++;
                        }
                        freeifaddrs(if_addrs);
                        if_addrs = NULL;
                    }
#else
                    struct ifconf confa;
                    struct ifreq *ifra;
                    char data[512];

                    confa.ifc_len = sizeof(data);
                    confa.ifc_buf = (caddr_t)data;
                    if (ioctl(cinfo->agent0.pub[0].cudp,SIOCGIFCONF,&confa) < 0)
                    {
                        CLOSE_SOCKET(cinfo->agent0.pub[0].cudp);
                        return (AGENT_ERROR_DISCOVERY);
                    }
                    // Use result to discover interfaces.
                    ifra = confa.ifc_req;
                    //bool found_bcast = false;
                    //int16_t lo_index = -1;
                    for (int32_t n=confa.ifc_len/sizeof(struct ifreq); --n >= 0; ifra++) {

                        if (ifra->ifr_addr.sa_family != AF_INET)
                        {
                            continue;
                        }

                        inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,cinfo->agent0.pub[cinfo->agent0.ifcnt].address,sizeof(cinfo->agent0.pub[cinfo->agent0.ifcnt].address));
                        memcpy((char *)&cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr, (char *)&ifra->ifr_addr, sizeof(ifra->ifr_addr));

                        if (ioctl(cinfo->agent0.pub[0].cudp,SIOCGIFFLAGS, (char *)ifra) < 0)
                        {
                            continue;
                        }
                        cinfo->agent0.pub[cinfo->agent0.ifcnt].flags = ifra->ifr_flags;

                        if ((cinfo->agent0.pub[cinfo->agent0.ifcnt].flags & IFF_POINTOPOINT) || (cinfo->agent0.pub[cinfo->agent0.ifcnt].flags & IFF_UP) == 0)
                        {
                            continue;
                        }

                        // Open socket again if we had to close it
                        if (cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp < 0)
                        {
                            if ((cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                            {
                                continue;
                            }

                            if (fcntl(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp, F_SETFL,O_NONBLOCK) < 0)
                            {
                                iretn = -errno;
                                CLOSE_SOCKET(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp);
                                cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp = iretn;
                                continue;
                            }
                        }

                        if (type == NetworkType::MULTICAST)
                        {
                            inet_pton(AF_INET,AGENTMCAST,&cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_addr);
                            inet_pton(AF_INET,AGENTMCAST,&cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr);
                        }
                        else
                        {
                            //                            int val = IP_PMTUDISC_DO;
                            //                            if ((iretn = setsockopt(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp, IPPROTO_IP, IP_MTU_DISCOVER, &val, sizeof(val))) < 0)
                            //                            {
                            //                                CLOSE_SOCKET(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp);
                            //                                continue;
                            //                            }

                            if ((cinfo->agent0.pub[cinfo->agent0.ifcnt].flags & IFF_POINTOPOINT))
                            {
//                                if (ioctl(cinfo->agent0.pub[0].cudp,SIOCGIFDSTADDR,(char *)ifra) < 0)
//                                {
//                                    continue;
//                                }
//                                cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr = cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr;
//                                inet_ntop(ifra->ifr_dstaddr.sa_family,&((struct sockaddr_in*)&ifra->ifr_dstaddr)->sin_addr,cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress,sizeof(cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress));
//                                inet_pton(AF_INET,cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress,&cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr);
                                cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr = cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr;
                                strncpy(cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress, cinfo->agent0.pub[cinfo->agent0.ifcnt].address, 17);
                                inet_pton(AF_INET, cinfo->agent0.pub[cinfo->agent0.ifcnt].address, &cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr);
                            }
                            else if ((cinfo->agent0.pub[cinfo->agent0.ifcnt].flags & IFF_LOOPBACK))
                            {
                                cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr = cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr;
                                strncpy(cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress, cinfo->agent0.pub[cinfo->agent0.ifcnt].address, 17);
                                inet_pton(AF_INET,AGENTLOOPBACK,&cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr);
                            }
                            else
                            {
                                if ((iretn = setsockopt(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on))) < 0)
                                {
                                    CLOSE_SOCKET(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp);
                                    continue;
                                }

                                if (ioctl(cinfo->agent0.pub[0].cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                                {
                                    continue;
                                }
                                cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr = cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr;
                                inet_ntop(ifra->ifr_broadaddr.sa_family,&((struct sockaddr_in*)&ifra->ifr_broadaddr)->sin_addr,cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress,sizeof(cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress));
                                inet_pton(AF_INET,cinfo->agent0.pub[cinfo->agent0.ifcnt].baddress,&cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_addr);
                            }

                            if (ioctl(cinfo->agent0.pub[0].cudp,SIOCGIFADDR,(char *)ifra) < 0)
                            {
                                continue;
                            }
                            inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,cinfo->agent0.pub[cinfo->agent0.ifcnt].address,sizeof(cinfo->agent0.pub[cinfo->agent0.ifcnt].address));
                            inet_pton(AF_INET,cinfo->agent0.pub[cinfo->agent0.ifcnt].address,&cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_addr);
                        }

                        iretn = sendto(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp,       // socket
                                (const char *)nullptr,                         // buffer to send
                                0,                      // size of buffer
                                0,                                          // flags
                                (struct sockaddr *)&cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr, // socket address
                                sizeof(struct sockaddr_in)                  // size of address to socket pointer
                                );
                        // Find assigned port, place in cport, and set caddr to requested port
                        socklen_t namelen = sizeof(struct sockaddr_in);
                        if ((iretn = getsockname(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp, (sockaddr*)&cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr, &namelen)) == -1)
                        {
                            CLOSE_SOCKET(cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp);
                            cinfo->agent0.pub[cinfo->agent0.ifcnt].cudp = -errno;
                            return (-errno);
                        }
                        cinfo->agent0.pub[cinfo->agent0.ifcnt].cport = ntohs(cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_port);
                        cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_port = htons(port);
                        inet_pton(AF_INET,cinfo->agent0.pub[cinfo->agent0.ifcnt].address,&cinfo->agent0.pub[cinfo->agent0.ifcnt].caddr.sin_addr);
                        cinfo->agent0.pub[cinfo->agent0.ifcnt].baddr.sin_port = htons(port);
                        cinfo->agent0.pub[cinfo->agent0.ifcnt].type = type;


                        cinfo->agent0.ifcnt++;
                    }
#endif // COSMOS_WIN_OS
                }
                break;
            default:
                return SOCKET_ERROR_PROTOCOL;
                break;
            }

            return 0;
        }

        //! Discover interfaces
        /*! Return a vector of ::socket_channel containing info on each valid interface. For IPV4 this
 *	will include the address and broadcast address, in both string sockaddr_in format.
\param ntype Type of network (Multicast, Broadcast UDP, CSP)
\return Vector of interfaces
*/
        vector<socket_channel> Agent::find_addresses(NetworkType ntype)
        {
            vector<socket_channel> iface;
            socket_channel tiface;

#ifdef COSMOS_WIN_OS
            struct sockaddr_storage ss;
            int sslen;
            INTERFACE_INFO ilist[20];
            unsigned long nbytes;
            size_t nif, ssize;
            uint32_t ip, net, bcast;
#else
            struct ifconf confa;
            struct ifreq *ifra;
            char data[512];
#endif // COSMOS_WIN_OS
            int32_t iretn = 0;
            int on = 1;
            int32_t cudp;

            switch (ntype)
            {
            case NetworkType::MULTICAST:
            case NetworkType::UDP:
                {
                    if ((cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0) { return (iface); }

                    // Use above socket to find available interfaces and establish
                    // publication on each.
#ifdef COSMOS_WIN_OS
                    if (WSAIoctl(cudp, SIO_GET_INTERFACE_LIST, 0, 0, &ilist,sizeof(ilist), &nbytes, 0, 0) == SOCKET_ERROR) {
                        CLOSE_SOCKET(cudp);
                        return (iface);
                    }

                    nif = nbytes / sizeof(INTERFACE_INFO);
                    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
                    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
                    pAddresses = (IP_ADAPTER_ADDRESSES *) calloc(sizeof(IP_ADAPTER_ADDRESSES), 2*nif);
                    ULONG outBufLen = sizeof(IP_ADAPTER_ADDRESSES) * 2 * nif;
                    DWORD dwRetVal;
                    if ((dwRetVal=GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, pAddresses, &outBufLen)) == ERROR_BUFFER_OVERFLOW)
                    {
                        free(pAddresses);
                        return (iface);
                    }

                    for (uint32_t i=0; i<nif; i++)
                    {
                        inet_ntop(ilist[i].iiAddress.AddressIn.sin_family,&ilist[i].iiAddress.AddressIn.sin_addr,tiface.address,sizeof(tiface.address));
                        //            strcpy(tiface.address,inet_ntoa(((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr));
                        if (!strcmp(tiface.address,"127.0.0.1")) { continue; }

                        pCurrAddresses = pAddresses;
                        while (pAddresses) {
                            if (((struct sockaddr_in *)(pCurrAddresses->FirstUnicastAddress->Address.lpSockaddr))->sin_addr.s_addr == ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.s_addr)
                            {
                                strcpy(tiface.name, pCurrAddresses->AdapterName);
                                break;
                            }
                            pCurrAddresses = pCurrAddresses->Next;
                        }
                        memset(&tiface.caddr,0,sizeof(struct sockaddr_in));
                        memset(&tiface.baddr,0,sizeof(struct sockaddr_in));
                        tiface.caddr.sin_family = AF_INET;
                        tiface.baddr.sin_family = AF_INET;
                        if (ntype == NetworkType::MULTICAST)
                        {
                            sslen = sizeof(ss);
                            WSAStringToAddressA((char *)AGENTMCAST,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
                            tiface.caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                            tiface.baddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                        } else {
                            if ((iretn = setsockopt(cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                            {
                                continue;
                            }
                            ip = ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.S_un.S_addr;
                            net = ((struct sockaddr_in*)&(ilist[i].iiNetmask))->sin_addr.S_un.S_addr;
                            bcast = ip | (~net);

                            tiface.caddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
                            tiface.caddr.sin_addr.S_un.S_addr = ip;
                            tiface.baddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
                            tiface.baddr.sin_addr.S_un.S_addr = bcast;
                        }
                        ((struct sockaddr_in *)&ss)->sin_addr = tiface.caddr.sin_addr;
                        ssize = strlen(tiface.address);
                        WSAAddressToStringA((struct sockaddr *)&tiface.caddr.sin_addr, sizeof(struct sockaddr_in), 0, tiface.address, (LPDWORD)&ssize);
                        ssize = strlen(tiface.baddress);
                        WSAAddressToStringA((struct sockaddr *)&tiface.baddr.sin_addr, sizeof(struct sockaddr_in), 0, tiface.baddress, (LPDWORD)&ssize);
                        tiface.type = ntype;
                        iface.push_back(tiface);
                    }
#else
                    confa.ifc_len = sizeof(data);
                    confa.ifc_buf = (caddr_t)data;
                    if (ioctl(cudp,SIOCGIFCONF,&confa) < 0)
                    {
                        CLOSE_SOCKET(cudp);
                        return (iface);
                    }
                    // Use result to discover interfaces.
                    ifra = confa.ifc_req;
                    for (int32_t n=confa.ifc_len/sizeof(struct ifreq); --n >= 0; ifra++)
                    {
                        if (ifra->ifr_addr.sa_family != AF_INET) { continue; }
                        inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,tiface.address,sizeof(tiface.address));

                        if (ioctl(cudp,SIOCGIFFLAGS, (char *)ifra) < 0) { continue; }
                        tiface.flags = ifra->ifr_flags;

                        if ((tiface.flags & IFF_UP) == 0 || (tiface.flags & IFF_LOOPBACK) || (tiface.flags & (IFF_BROADCAST)) == 0)
                        {
                            continue;
                        }

                        // Open socket again if we had to close it
                        if (cudp < 0) {
                            if ((cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0) { continue; }
                        }

                        if (ntype == NetworkType::MULTICAST) {
                            inet_pton(AF_INET,AGENTMCAST,&tiface.caddr.sin_addr);
                            //                        strcpy(tiface.baddress, AGENTMCAST);
                            inet_pton(AF_INET,AGENTMCAST,&tiface.baddr.sin_addr);
                        } else {
                            if ((iretn = setsockopt(cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0) {
                                CLOSE_SOCKET(cudp);
                                continue;
                            }

                            //                        strncpy(tiface.name, ifra->ifr_name, COSMOS_MAX_NAME);
                            if (ioctl(cudp,SIOCGIFBRDADDR,(char *)ifra) < 0) { continue; }
                            memcpy((char *)&tiface.baddr, (char *)&ifra->ifr_broadaddr, sizeof(ifra->ifr_broadaddr));
                            if (ioctl(cudp,SIOCGIFADDR,(char *)ifra) < 0) { continue; }
                            memcpy((char *)&tiface.caddr, (char *)&ifra->ifr_addr, sizeof(ifra->ifr_addr));
                            inet_ntop(tiface.baddr.sin_family,&tiface.baddr.sin_addr,tiface.baddress,sizeof(tiface.baddress));
                        }
                        tiface.type = ntype;
                        iface.push_back(tiface);
                    }

#endif // COSMOS_WIN_OS
                }
                break;
            default:
                break;
            }
            return (iface);
        }

        //! Post a Cosmos::Agent::messstruc
        /*! Post an already defined message on the previously opened publication channel.
\param mess Cosmos::Agent::messstruc containing everything necessary, including type, header and data.
\return 0, otherwise negative error.
*/
        int32_t Agent::post(messstruc mess) {
            int32_t iretn = 0;
            if (mess.meta.type < Agent::AgentMessage::BINARY) {
                iretn = post(mess.meta.type, mess.adata);
            } else {
                iretn = post(mess.meta.type, mess.bdata);
            }
            return iretn;
        }

        //! Post a JSON message
        /*! Post a vector of bytes on the previously opened publication channel.
\param type A byte indicating the type of message.
\param message A NULL terminated JSON text string to post.
\return 0, otherwise negative error.
*/
        int32_t Agent::post(AgentMessage type, string message)
        {
            int32_t iretn = 0;
            vector<uint8_t> bytes(message.begin(), message.end());
            //        bytes.push_back(0);
            iretn = post(type, bytes);
            return iretn;
        }

        //! Post a binary message
        /*! Post a vector of bytes on the previously opened publication channel.
\param type A byte indicating the type of message.
\param message An array of bytes to post.
\return 0, otherwise negative error.
*/
        int32_t Agent::post(AgentMessage type, vector <uint8_t> message)
        {
            size_t nbytes;
            int32_t iretn=0;
            uint8_t post[AGENTMAXBUFFER];

            cinfo->agent0.beat.utc = currentmjd();
            post[0] = (uint8_t)type;
            // this will broadcast messages to all external interfaces (ifcnt = interface count)
            for (size_t i=0; i<cinfo->agent0.ifcnt; i++)
            {
                sprintf((char *)&post[3],"{\"agent_utc\":%.15g,\"agent_node\":\"%s\",\"agent_proc\":\"%s\",\"agent_addr\":\"%s\",\"agent_port\":%u,\"agent_bprd\":%f,\"agent_bsz\":%u,\"agent_cpu\":%f,\"agent_memory\":%f,\"agent_jitter\":%f,\"agent_dcycle\":%f,\"node_utcoffset\":%.15g}",
                        cinfo->agent0.beat.utc,
                        cinfo->agent0.beat.node.c_str(),
                        cinfo->agent0.beat.proc.c_str(),
                        cinfo->agent0.pub[i].address,
                        cinfo->agent0.beat.port,
                        cinfo->agent0.beat.bprd,
                        cinfo->agent0.beat.bsz,
                        cinfo->agent0.beat.cpu,
                        cinfo->agent0.beat.memory,
                        cinfo->agent0.beat.jitter,
                        cinfo->agent0.beat.dcycle,
                        cinfo->node.utcoffset);
//                debug_log.Printf("POST %u %x %s %s %s\n", i, cinfo->agent0.pub[i].flags, cinfo->agent0.pub[i].baddress, cinfo->agent0.pub[i].address, (char *)&post[3]);
                size_t hlength = strlen((char *)&post[3]);
                post[1] = hlength%256;
                post[2] = hlength / 256;
                nbytes = hlength + 3;

                if (message.size())
                {
                    if (nbytes+message.size() > AGENTMAXBUFFER)
                        return (AGENT_ERROR_BUFLEN);
                    memcpy(&post[nbytes], &message[0], message.size());
                }
                if (cinfo->agent0.pub[i].flags & IFF_POINTOPOINT)
                {
//                    if (cinfo->agent0.ifcnt == 1)
//                    {
//                        iretn = sendto(cinfo->agent0.pub[i].cudp,       // socket
//                                (const char *)post,                         // buffer to send
//                                nbytes+message.size(),                      // size of buffer
//                                0,                                          // flags
//                                (struct sockaddr *)&cinfo->agent0.pub[i].caddr, // socket address
//                                sizeof(struct sockaddr_in)                  // size of address to socket pointer
//                                );
//                    }
                    iretn = sendto(cinfo->agent0.pub[i].cudp,       // socket
                            (const char *)post,                         // buffer to send
                            nbytes+message.size(),                      // size of buffer
                            0,                                          // flags
                            (struct sockaddr *)&cinfo->agent0.pub[i].caddr, // socket address
                            sizeof(struct sockaddr_in)                  // size of address to socket pointer
                            );
//                    debug_log.Printf("Send P2P: %s %u %u %d\n", cinfo->agent0.pub[i].address, ntohs(cinfo->agent0.pub[i].caddr.sin_port), (uint8_t)type, iretn);
                }
                else if (cinfo->agent0.pub[i].flags & IFF_LOOPBACK)
                {
                    if (cinfo->agent0.ifcnt == 1)
                    {
                        iretn = sendto(cinfo->agent0.pub[i].cudp,       // socket
                                (const char *)post,                         // buffer to send
                                nbytes+message.size(),                      // size of buffer
                                0,                                          // flags
                                (struct sockaddr *)&cinfo->agent0.pub[i].caddr, // socket address
                                sizeof(struct sockaddr_in)                  // size of address to socket pointer
                                );
//                        debug_log.Printf("Send Loopback: %s %x %u %u %d\n", cinfo->agent0.pub[i].address, ntohl(cinfo->agent0.pub[i].caddr.sin_addr.s_addr), ntohs(cinfo->agent0.pub[i].caddr.sin_port), (uint8_t)type, iretn);
                    }
                }
                else
                {
                    iretn = sendto(cinfo->agent0.pub[i].cudp,       // socket
                            (const char *)post,                         // buffer to send
                            nbytes+message.size(),                      // size of buffer
                            0,                                          // flags
                            (struct sockaddr *)&cinfo->agent0.pub[i].baddr, // socket address
                            sizeof(struct sockaddr_in)                  // size of address to socket pointer
                            );
//                    debug_log.Printf("Send Generic: %s %u %u %d\n", cinfo->agent0.pub[i].address, ntohs(cinfo->agent0.pub[i].baddr.sin_port), (uint8_t)type, iretn);
                }
                if (iretn < 0)
                {
#ifdef COSMOS_WIN_OS
                    if (WSAGetLastError() != EAGAIN && WSAGetLastError() != EWOULDBLOCK)
                    {
                        iretn = -WSAGetLastError();
                    }
#else
                    if (errno != EAGAIN && errno != EWOULDBLOCK)
                    {
                        iretn = -errno;
                    }
#endif
                    else
                    {
                        iretn= 0;
                    }
                }
            }

            if (iretn<0)
            {
#ifdef COSMOS_WIN_OS
                return(-WSAGetLastError());
#else
                return (-errno);
#endif
            }
            return 0;
        }

        int32_t Agent::post_beat()
        {
            int32_t iretn = 0;
            cinfo->agent0.beat.utc = currentmjd(0.);
            iretn = post(AgentMessage::BEAT);
            //            if (!sohtable.empty())
            //            {
            //                iretn = post(AgentMessage::BEAT, json_of_table(hbjstring, sohtable, (cosmosstruc *)cinfo));
            //            }
            //            else
            //            {
            //                iretn = post(AgentMessage::BEAT,"");
            //            }
            return iretn;
        }

        int32_t Agent::post_soh() {
            int32_t iretn = 0;
            cinfo->agent0.beat.utc = currentmjd(0.);
            iretn = post(AgentMessage::SOH, json_of_table(hbjstring, sohtable, (cosmosstruc *)cinfo));
            return iretn;
        }

        //! Close COSMOS output channel
        /*! Close previously opened publication channels and recover any allocated resources.
\return 0, otherwise negative error.
*/
        int32_t Agent::unpublish() {
            if (cinfo == nullptr) { return 0; }
            for (size_t i=0; i<cinfo->agent0.ifcnt; ++i) { CLOSE_SOCKET(cinfo->agent0.pub[i].cudp); }
            return 0;
        }

        //! Open COSMOS channel for polling
        /*! This establishes a multicast channel for subscribing to COSMOS messages.
\param type 0=Multicast, 1=Broadcast UDP, 2=Broadcast CSP.
\param address The IP Multicast address of the channel.
\param port The port to use for the channel.
\param usectimeo Blocking read timeout in micro seconds.
\return 0, otherwise negative error.
*/
        int32_t Agent::subscribe(NetworkType type, const char *address, uint16_t port, uint32_t usectimeo)
        {
            int32_t iretn = 0;

            // ?? this is preventing from running socket_open if
            // for some reason cinfo->agent0.sub.cport was ill initialized
#ifndef COSMOS_WIN_BUILD_MSVC
            if (cinfo->agent0.sub.cport) { return SOCKET_ERROR_OPEN; }
#endif
            if ((iretn=socket_open(&cinfo->agent0.sub,type,address,port,SOCKET_LISTEN,SOCKET_BLOCKING, usectimeo)) < 0) {
                return iretn;
            }
            return 0;
        }

        //! Open COSMOS channel for polling with 100 usec timeout.
        /*! This establishes a multicast channel for subscribing to COSMOS messages. The timeout is set to
 * 100 usec.
\param type 0=Multicast, 1=Broadcast UDP, 2=Broadcast CSP.
\param address The IP Multicast address of the channel.
\param port The port to use for the channel.
\return 0, otherwise negative error.
*/
        int32_t Agent::subscribe(NetworkType type, const char *address, uint16_t port) {
            int32_t iretn = 0;
            if ((iretn=Agent::subscribe(type, address, port, 100)) < 0) { return iretn; }
            return 0;
        }
        //! Close COSMOS subscription channel
        /*! Close channel previously opened for polling for messages and recover resources.
\return 0, otherwise negative error.
*/
        int32_t Agent::unsubscribe() {
            if (cinfo != nullptr) { CLOSE_SOCKET(cinfo->agent0.sub.cudp); }
            return 0;
        }

        //! Listen for message
        /*! Poll the subscription channel for the requested amount of time. Return as soon as a single message
 * comes in, or the timer runs out.
\param mess Cosmos::Agent::messstruc for storing incoming message.
\param type Type of message to look for, taken from Cosmos::Agent::AgentMessage.
\param waitsec Number of seconds in timer.
\return If a message comes in, return its type. If none comes in, return zero, otherwise negative error.
*/
        int32_t Agent::poll(messstruc &mess, AgentMessage type, float waitsec)
        {
            int nbytes;
            uint8_t input[AGENTMAXBUFFER+1];

            if (cinfo == nullptr) {
                return AGENT_ERROR_NULL;
            }

            if (!cinfo->agent0.sub.cport) {
                return (AGENT_ERROR_CHANNEL);
            }

            // Clear out message
            mess.meta.beat.addr[0] = 0;
            mess.meta.beat.bprd = 0;
            mess.meta.beat.bsz = 0;
            mess.meta.beat.cpu = 0;
            mess.meta.beat.exists = false;
            mess.meta.beat.jitter = 0;
            mess.meta.beat.dcycle = 0;
            mess.meta.beat.memory = 0;
            mess.meta.beat.ntype = NetworkType::BROADCAST;
            mess.meta.beat.port = 0;
            mess.meta.beat.user[0] = 0;
            mess.meta.beat.utc = 0;
            mess.meta.beat.node.clear();
            mess.meta.beat.proc.clear();

            //        memset(&mess.meta.beat, 0, COSMOS_SIZEOF(beatstruc));

            ElapsedTime ep;
            ep.start();
            do
            {
                nbytes = 0;
                switch (cinfo->agent0.sub.type)
                {
                case NetworkType::MULTICAST:
                case NetworkType::UDP:
                    {
                        cinfo->agent0.sub.addrlen = sizeof(cinfo->agent0.sub.caddr);
                        nbytes = recvfrom(cinfo->agent0.sub.cudp, (char *)input,AGENTMAXBUFFER, 0, (struct sockaddr *)&cinfo->agent0.sub.caddr, (socklen_t *)&cinfo->agent0.sub.addrlen);

                        // Return if error
                        if (nbytes < 0)
                        {
                            return nbytes;
                        }
                        if (cinfo->agent0.sub.addrlen == sizeof(cinfo->agent0.sub.caddr))
                        {
                            inet_ntop(cinfo->agent0.sub.caddr.sin_family,&(cinfo->agent0.sub.caddr.sin_addr),cinfo->agent0.sub.address,sizeof(cinfo->agent0.sub.address));
//                            debug_log.Printf("RECVFROM: [%s %u] %u %d \n", cinfo->agent0.sub.address, htons(cinfo->agent0.sub.caddr.sin_port), input[0], nbytes);
                        }
//                        else
//                        {
//                            debug_log.Printf("RECVFROM: [Unknown 0] %u %d \n", input[0], nbytes);
//                        }

                        // Return if port and address are our own
                        for (uint16_t i=1; i<cinfo->agent0.ifcnt; ++i)
                        {
                            if (cinfo->agent0.sub.caddr.sin_port == ntohs(cinfo->agent0.pub[i].cport) &&
                                cinfo->agent0.sub.caddr.sin_addr.s_addr == cinfo->agent0.pub[i].caddr.sin_addr.s_addr)
                            {
                                return 0;
                            }
                        }
                    }
                    break;
                default:
                    break;
                }

                if (nbytes > 0)
                {
                    if (type == Agent::AgentMessage::ALL || type == (AgentMessage)input[0])
                    {
                        // Determine if old or new message
                        uint8_t start_byte;
                        if (input[1] == '{')
                        {
                            start_byte = 1;
                        }
                        else
                        {
                            start_byte = 3;
                        }
                        // Provide support for older messages that did not include jlength
                        if (start_byte > 1)
                        {
                            mess.meta.type = (AgentMessage)input[0];
                            mess.meta.jlength = input[1] + 256 * input[2];
                        }
                        else
                        {
                            mess.meta.type = (AgentMessage)(input[0] + 1);
                            mess.meta.jlength = nbytes;
                        }

                        // Copy message parts to ring, placing in appropriate buffers.
                        // First: JSON header
                        mess.jdata.assign((const char *)&input[start_byte], mess.meta.jlength);

                        // Next: ASCII or BINARY message, depending on message type.
                        if (mess.meta.type < Agent::AgentMessage::BINARY)
                        {
                            mess.adata.clear();
                            mess.adata.assign((const char *)&input[start_byte+mess.meta.jlength], nbytes - (start_byte + mess.meta.jlength));
                        }
                        else
                        {
                            mess.bdata.resize(nbytes - (start_byte + mess.meta.jlength));
                            memcpy(mess.bdata.data(), &input[start_byte + mess.meta.jlength], nbytes - (start_byte + mess.meta.jlength));
                        }

                        // Extract meta data
                        if (mess.jdata.find("}{") == string::npos)
                        {
                            char node[COSMOS_MAX_NAME+1] = {};
                            char proc[COSMOS_MAX_NAME+1] = {};
                            sscanf((const char *)mess.jdata.data(), "{\"agent_utc\":%lg,\"agent_node\":\"%40[^\"]\",\"agent_proc\":\"%40[^\"]\",\"agent_addr\":\"%17[^\"]\",\"agent_port\":%hu,\"agent_bprd\":%lf,\"agent_bsz\":%u,\"agent_cpu\":%f,\"agent_memory\":%f,\"agent_jitter\":%lf}",
                                   &mess.meta.beat.utc,
                                   node,
                                   proc,
                                   mess.meta.beat.addr,
                                   &mess.meta.beat.port,
                                   &mess.meta.beat.bprd,
                                   &mess.meta.beat.bsz,
                                   &mess.meta.beat.cpu,
                                   &mess.meta.beat.memory,
                                   &mess.meta.beat.jitter);
                            if(node[0] != '\0')
                            {
                                mess.meta.beat.node = node;
                            }
                            if(proc[0] != '\0')
                            {
                                mess.meta.beat.proc = proc;
                            }
                        }
                        else if (mess.jdata.find("agent_bprd") == string::npos)
                        {
                            char node[COSMOS_MAX_NAME+1] = {};
                            char proc[COSMOS_MAX_NAME+1] = {};
                            sscanf((const char *)mess.jdata.data(), "{\"agent_utc\":%lg}{\"agent_node\":\"%40[^\"]\"}{\"agent_proc\":\"%40[^\"]\"}{\"agent_addr\":\"%17[^\"]\"}{\"agent_port\":%hu}{\"agent_bsz\":%u}{\"agent_cpu\":%f}{\"agent_memory\":%f}{\"agent_jitter\":%lf}",
                                   &mess.meta.beat.utc,
                                   node,
                                   proc,
                                   mess.meta.beat.addr,
                                   &mess.meta.beat.port,
                                   &mess.meta.beat.bsz,
                                   &mess.meta.beat.cpu,
                                   &mess.meta.beat.memory,
                                   &mess.meta.beat.jitter);
                            if(node[0] != '\0')
                            {
                                mess.meta.beat.node = node;
                            }
                            if(proc[0] != '\0')
                            {
                                mess.meta.beat.proc = proc;
                            }
                        }
                        else
                        {
                            char node[COSMOS_MAX_NAME+1] = {};
                            char proc[COSMOS_MAX_NAME+1] = {};
                            sscanf((const char *)mess.jdata.data(), "{\"agent_utc\":%lg}{\"agent_node\":\"%40[^\"]\"}{\"agent_proc\":\"%40[^\"]\"}{\"agent_addr\":\"%17[^\"]\"}{\"agent_port\":%hu}{\"agent_bprd\":%lf}{\"agent_bsz\":%u}{\"agent_cpu\":%f}{\"agent_memory\":%f}{\"agent_jitter\":%lf}",
                                   &mess.meta.beat.utc,
                                   node,
                                   proc,
                                   mess.meta.beat.addr,
                                   &mess.meta.beat.port,
                                   &mess.meta.beat.bprd,
                                   &mess.meta.beat.bsz,
                                   &mess.meta.beat.cpu,
                                   &mess.meta.beat.memory,
                                   &mess.meta.beat.jitter);
                            if(node[0] != '\0')
                            {
                                mess.meta.beat.node = node;
                            }
                            if(proc[0] != '\0')
                            {
                                mess.meta.beat.proc = proc;
                            }
                        }
                        if (mess.meta.beat.node.compare(cinfo->agent0.beat.node) || mess.meta.beat.proc.compare(cinfo->agent0.beat.proc))
                        {
                            return ((int)mess.meta.type);
                        }
                    }
                }
                if (ep.split() >= waitsec) {
                    nbytes = 0;
                } else {
                    secondsleep(.1);
                }
            } while (nbytes != 0);

            return 0;
        }

        //! Check Ring for message
        /*! Check the message ring for the requested amount of time. Return as soon as a message of the right type
 * is available, or the timer runs out.
\param message Vector for storing incoming message.
\param type Type of message to look for, taken from Cosmos::Agent::AgentMessage.
\param waitsec Number of seconds in timer. If 0, return last message in ring immediatelly.
\param where One of Where::HEAD or Where::TAIL, indicating whether to start at the head or tail of the ring.
\return If a message comes in, return its type. If none comes in, return zero, otherwise negative error.
*/
        int32_t Agent::readring(messstruc &message, AgentMessage type, float waitsec, Where where, string proc, string node)
        {
            if (waitsec < 0.f) { waitsec = 0.; }
            if (cinfo == nullptr) { return AGENT_ERROR_NULL; }
            if (where == Where::HEAD) { message_queue.clear(); }
            ElapsedTime ep;
            ep.start();
            do {
                while (message_queue.size()) {
                    message = message_queue.front();
                    message_queue.pop_front();
                    if (type == Agent::AgentMessage::ALL || type == static_cast<Agent::AgentMessage>(message.meta.type)) {
                        if (proc.empty() || !proc.compare(message.meta.beat.proc)) {
                            if (node.empty() || !node.compare(message.meta.beat.node)) {
                                return (static_cast<int32_t>(message.meta.type));
                            }
                        }
                    }
                }

                if (ep.split() < waitsec) {
                    if (waitsec - ep.split() > .1) {
                        secondsleep(.1);
                    } else {
                        secondsleep(.05);
                    }
                }
            } while (ep.split() < waitsec);

            return 0;
        }

        //! Check Ring for message
        /*! Check the message ring for the requested amount of time. Return as soon as a message of the right type
     * is available, or the timer runs out.
    \param message Vector for storing incoming message.
    \param realm Vector of node names that are within the realm to read
    \param type Type of message to look for, taken from Cosmos::Agent::AgentMessage.
    \param waitsec Number of seconds in timer. If 0, return last message in ring immediatelly.
    \param where One of Where::HEAD or Where::TAIL, indicating whether to start at the head or tail of the ring.
    \return If a message comes in, return its type. If none comes in, return zero, otherwise negative error.
    */
        int32_t Agent::readring(messstruc &message, vector<string> realm, AgentMessage type, float waitsec, Where where)
        {
            if (waitsec < 0.f) { waitsec = 0.; }
            if (cinfo == nullptr) { return AGENT_ERROR_NULL; }
            if (where == Where::HEAD) { message_queue.clear(); }
            ElapsedTime ep;
            ep.start();
            do {
                while (message_queue.size()) {
                    message = message_queue.front();
                    message_queue.pop_front();
                    if (type == Agent::AgentMessage::ALL || type == static_cast<Agent::AgentMessage>(message.meta.type)) {
                        if (realm.empty() || std::find(realm.begin(), realm.end(), message.meta.beat.node) != realm.end()) {
                            return (static_cast<int32_t>(message.meta.type));
                        }
                    }
                }

                if (ep.split() < waitsec) {
                    if (waitsec - ep.split() > .1) {
                        secondsleep(.1);
                    } else {
                        secondsleep(.05);
                    }
                }
            } while (ep.split() < waitsec);

            return 0;
        }

        //! Parse next message from ring
        int32_t Agent::parsering(AgentMessage type, float waitsec, Where where, string proc, string node)
        {
            int32_t iretn = 0;
            messstruc message;

            if (where == Where::HEAD) { message_queue.clear(); }
            post(Agent::AgentMessage::REQUEST, "heartbeat");
            ElapsedTime et;
            do {
                iretn = readring(message, type, waitsec, where, proc, node);
            } while (et.split() < waitsec);

            if (iretn >= 0 && iretn < static_cast <int32_t>(Agent::AgentMessage::BINARY))
            {
                json_parse(message.adata, cinfo);
                return iretn;
            }

            return GENERAL_ERROR_TIMEOUT;
        }

        //! Change size of message ring.
        //! Resize the message ring to hold a new maximum number of messages. Adjust the message pointers in the
        //! ring to be appropriate.
        //! \param newsize New maximum message count.
        //! \return Negative error, or zero.
        int32_t Agent::resizering(size_t newsize)
        {
            if (message_head >= newsize) { message_head = 0; }
            if (message_tail >= newsize) { message_tail = newsize - 1; }
            return 0;
        }

        //! Empty message ring.
        //! Set the internal pointers such that it appears that we have read any messages that are
        //! in the message ring. This has the effect of emptying the message ring as far as Cosmos::Agent::readring
        //! is concerned.
        //! \return Negative error or zero.
        int32_t Agent::clearring()
        {
            message_tail = message_head;
            return 0;
        }

        //! Listen for heartbeat
        /*! Poll the subscription channel until you receive a heartbeat message, or the timer runs out.
\param waitsec Number of seconds to wait before timing out.
\return ::beatstruc with acquired heartbeat. The UTC will be set to 0 if no heartbeat was
acquired.
*/
        //    beatstruc Agent::poll_beat(float waitsec)
        //    {
        //        int32_t iretn = 0;
        //        beatstruc beat;
        //        messstruc mess;

        //        iretn = Agent::poll(mess, Agent::AgentMessage::BEAT, waitsec);

        //        beat.utc = 0.;
        //        if (iretn == Agent::AgentMessage::BEAT)
        //        {
        //            beat = mess.meta.beat;
        //        }

        //        return (beat);
        //    }

        //! Listen for Time
        /*! Poll the subscription channel until you receive a time message, or the timer runs out.
\param waitsec Number of seconds to wait before timing out.
\return ::timestruc with acquired time. The UTC will be set to 0 if no heartbeat was
acquired.
*/
        //    timestruc Agent::poll_time(float waitsec)
        //    {
        //        int32_t iretn = 0;
        //        timestruc time;
        //        messstruc mess;

        //		iretn = Agent::poll(mess, Agent::AgentMessage::TIME, waitsec);

        //		if (iretn == Agent::AgentMessage::TIME)
        //        {
        //            iretn = json_parse(mess.adata, cinfo->sdata);
        //            if (iretn >= 0)
        //            {
        //                time.mjd = cinfo->sdata.node.loc.utc;
        //            }
        //        }

        //        return (time);
        //    }

        //! Listen for Location
        /*! Poll the subscription channel until you receive a location message, or the timer runs out.
\param waitsec Number of seconds to wait before timing out.
\return ::locstruc with acquired location. The UTC will be set to 0 if no heartbeat was
acquired.
*/
        //    Convert::locstruc Agent::poll_location(float waitsec)
        //    {
        //        int32_t iretn = 0;
        //        Convert::locstruc loc;
        //        messstruc mess;

        //		iretn = Agent::poll(mess, Agent::AgentMessage::LOCATION, waitsec);

        //		if (iretn == Agent::AgentMessage::LOCATION)
        //        {
        //            iretn = json_parse(mess.adata, cinfo->sdata);
        //            if (iretn >= 0)
        //            {
        //                loc = cinfo->sdata.node.loc;
        //            }
        //        }

        //        return (loc);
        //    }

        //! Listen for Beacon
        /*! Poll the subscription channel until you receive a info message, or the timer runs out.
\param waitsec Number of seconds to wait before timing out.
\return ::nodestruc with acquired info. The UTC will be set to 0 if no info was
acquired.
*/
        //    nodestruc Agent::poll_info(float waitsec)
        //    {
        //        int32_t iretn = 0;
        //        //summarystruc info;
        //        nodestruc info;
        //        messstruc mess;

        //		iretn = Agent::poll(mess, Agent::AgentMessage::TRACK, waitsec);

        //		if (iretn == Agent::AgentMessage::TRACK)
        //        {
        //            iretn = json_parse(mess.adata, cinfo->sdata);
        //            if (iretn >= 0)
        //            {
        //                strcpy(info.name,cinfo->sdata.node.name);
        //                //			info.utc = cinfo->node.loc.utc;
        //                info.loc = cinfo->sdata.node.loc;
        //                info.powgen = cinfo->sdata.node.phys.powgen;
        //                info.powuse = cinfo->sdata.node.phys.powuse;
        //                info.battlev = cinfo->sdata.node.phys.battlev;
        //            }
        //        }
        //        else
        //            info.loc.utc = 0.;

        //        return (info);
        //    }

        //! Listen for IMU device
        /*! Poll the subscription channel until you receive a IMU device message, or the timer runs out.
\param waitsec Number of seconds to wait before timing out.
\return ::beatstruc with acquired heartbeat. The UTC will be set to 0 if no heartbeat was
acquired.
*/
        //    imustruc Agent::poll_imu(float waitsec)
        //    {
        //        int32_t iretn = 0;
        //        imustruc imu;
        //        messstruc mess;

        //		iretn = Agent::poll(mess, Agent::AgentMessage::IMU, waitsec);

        //		if (iretn == Agent::AgentMessage::IMU)
        //        {
        //            iretn = json_parse(mess.adata, cinfo->sdata);
        //            if (iretn >= 0)
        //            {
        //                imu = *cinfo->sdata.devspec.imu[0].;
        //            }
        //        }

        //        return (imu);
        //    }

        string Agent::getNode() { return cinfo->node.name; }

        string Agent::getAgent() { return cinfo->agent0.name; }


        int32_t Agent::getJson(string node, jsonnode &jnode)
        {
            int32_t iretn=0;

            bool node_found = false;
            for (jsonnode json : node_list)
            {
                if (!node.compare(json.name))
                {
                    node_found = true;
                }
            }

            if (!node_found)
            {
                for (beatstruc beat : agent_list)
                {
                    if (!node.compare(beat.node))
                    {
                        if ((iretn=send_request_jsonnode(beat, jnode)) >= 0)
                        {
                            node_list.push_back(jnode);
                            node_found = true;
                        }
                    }
                }
            }

            if (!node_found) { iretn = GENERAL_ERROR_UNDEFINED; }

            return iretn;
        }

        int32_t Agent::set_debug_level(uint16_t level)
        {
            debug_level = level;
            debug_log.Set(level, true,  data_base_path(cinfo->node.name, "temp", cinfo->agent0.name), 1800., "debug");
            return debug_log.Type();
        }

        int32_t Agent::get_debug_level()
        {
            return debug_log.Type();
        }

        FILE *Agent::get_debug_fd(double mjd)
        {
            return debug_log.Open();
        }

        int32_t Agent::close_debug_fd()
        {
            return debug_log.Close();
        }

        // Set our producer for all functions associated with time authority (i.e. the mjd request).
        int32_t Agent::set_agent_time_producer(double (*source)()) {
            this->agent_time_producer = source;
            return 0;
        }

        // A Cristian's algorithm approach to time synchronization, with our remote node as the time server.
        // This is meant to be run on a time sink agent (a requester).
        int32_t Agent::get_agent_time(double &agent_time, double &epsilon, double &delta, string agent, string node, double wait_sec) {
            static beatstruc agent_beat;
            string agent_response;
            double mjd_0, mjd_1;
            int32_t iretn = 0;

            if (!agent_beat.exists) {
                agent_beat = find_agent(node, agent, wait_sec);
            } else {
                if (node.compare(agent_beat.node) || (agent.compare(agent_beat.proc))) {
                    agent_beat = find_agent(node, agent, wait_sec);
                }
            }

            // Do not proceed if we cannot find the agent.
            if (!agent_beat.exists) return AGENT_ERROR_DISCOVERY;

            mjd_0 = currentmjd();
            iretn = send_request(agent_beat, "utc", agent_response, 0);
            if (iretn >= 0) {
                mjd_1 = currentmjd();

                epsilon = (mjd_1 - mjd_0) / 2.0;  // RTT / 2.0
                agent_time = stod(agent_response.substr(0, agent_response.find("["))) + epsilon;
                delta = agent_time - mjd_1; // We do not have a lower bound on the time to transmit a message one way.

                return 0;
            } else {
                agent_time = 0.;
                epsilon = 0.;
                return GENERAL_ERROR_TIMEOUT;
            }
        }

        int32_t Agent::set_activity_period(double period)
        {
            this->cinfo->agent0.aprd = period;
            return 0;
        }

        /**
     * @brief Agent::add_device
     * @param name piecename
     * @param type of device
     * @param device pointer to devicestruc
     * @return status (negative on error)
     */
        int32_t Agent::add_device(string name, DeviceType type, devicestruc **device)
        {
            int32_t pindex = json_createpiece(cinfo, name, type);
            if(pindex < 0) {
                device = nullptr;
                return pindex;
            }
            int32_t cindex = cinfo->pieces[pindex].cidx;
            *device = cinfo->device[cindex];
            return cindex;
        }

        /**
     * @brief Agent::device_property_name
     * @param device (piecename) - for looking up the device index
     * @param property
     * @param name reference to output (ex: device_imu_alpha_000)
     * @return status (negative on error)
     */
        int32_t Agent::device_property_name(string device, string property, string &name)
        {
            //! get the device index
            int32_t pidx = json_findpiece(cinfo, device);
            if(pidx < 0 ) {
                return pidx;
            }

            int32_t cindex = cinfo->pieces[pidx].cidx;
            int32_t didx = cinfo->device[cindex]->didx;
            if(didx < 0) {
                return didx;
            }

            //! get the device type
            uint16_t device_type = cinfo->device[cindex]->type;
            string devtype = device_type_name(device_type);

            //! check if property exists in device
            if(!device_has_property(device_type, property)){
                return ErrorNumbers::COSMOS_GENERAL_ERROR_NAME;

            }

            char dindex[4];
            sprintf(dindex, "%03u", didx); //! int to string conversion

            name = "device_" + devtype + "_" + property + "_" + string(dindex);
            return 0;
        }

        /**
     * @brief creating and storing an alias for a device property
     *   ex: creating alias from device_imu_alpha_000 to imu_acceleration
     * @param devicename piecename - for looking up the device index
     * @param propertyname
     * @param alias name that will replace property name in alias
     * @param error reference for returning an error
     * @return status (negative on error)
     */
        int32_t Agent::create_device_value_alias(string devicename, string propertyname, string alias)
        {
            int32_t status = 0;
            string cosmos_soh_name;
            status = device_property_name(devicename, propertyname, cosmos_soh_name);
            if(status < 0) return status;

            status = create_alias(cosmos_soh_name, alias);
            return status;
        }

        /**
     * @brief creating a direct alias (Namespace 1.0 method)
     *   ex: creating alias from device_imu_alpha_000 to imu_acceleration
     * @param cosmosname the default SOH name
     * @param alias name that will replace cosmosname
     * @return error
     */
        int32_t Agent::create_alias(string cosmosname, string alias)
        {
            //! Namespace 1.0 method of adding aliases
            string equation = "(\""+cosmosname+"\"*1.0)";
            jsonhandle eqhandle;
            int32_t status = json_equation_map(equation, cinfo, &eqhandle);
            if(status < 0) return status;

            status = json_addentry(alias, equation, cinfo);
            return status;
        }

        int32_t Agent::send_request_getvalue(beatstruc agent, vector<string> names, Json::Object &jobj)
        {
            int32_t status = 0;
            if(names.size() == 0){
                return ErrorNumbers::COSMOS_GENERAL_ERROR_EMPTY;
            }
            string request_args = "{";
            for(string name : names){
                request_args += "\"" + name + "\",";
            }
            request_args.pop_back(); // remove last ,
            request_args += "}";

            if(!agent.exists) {
                return ErrorNumbers::COSMOS_AGENT_ERROR_NULL;
            }
            string response;
            status = send_request(agent, "getvalue " + request_args, response);
            if(status < 0) {
                return status;
            }
            Json jresult;
            status = jresult.extract_contents(response);
            jobj = jresult.ObjectContents;

            return status;
        }


        int32_t Agent::set_value(string jsonname, double value)
        {
            if(jsonname.length() == 0) {
                return ErrorNumbers::COSMOS_GENERAL_ERROR_NAME;
            }
            jsonentry* jentry = json_entry_of(jsonname, cinfo);
            return json_set_number(value, jentry, cinfo);

        }

        double Agent::get_value(string jsonname)
        {
            return json_get_double(jsonname, cinfo);
        }

        /**
     * @brief Agent::get_device_values
     * @param device device name or piece name
     * @param props property names
     * @param json json string of device values {"name1": value ,"name2": value2}
     * @return status (negative on error)
     */
        int32_t Agent::get_device_values(string device, vector<string> props, string &json)
        {
            if(props.size() == 0) return 0;
            int32_t pidx = json_findpiece(cinfo, device);
            if(pidx < 0 ) {
                return pidx;
            }
            int32_t cindex = cinfo->pieces[pidx].cidx;
            int32_t didx = cinfo->device[cindex]->didx;
            if(didx < 0) {
                return didx;
            }

            uint16_t type = cinfo->device[cindex]->type;
            string devtype = device_type_name(type);
            // check if property exists in device

            char dindex[4];
            sprintf(dindex, "%03u", didx);
            string name;
            vector<string> names;
            for(string p: props){
                name = "device_" + devtype+"_"+p + "_" + string(dindex);
                if(!device_has_property(type, p) && json_entry_of(name, cinfo) == nullptr){
                    return ErrorNumbers::COSMOS_GENERAL_ERROR_NAME;
                }
                names.push_back(name);
            }
            return get_values(names, json);
        }


        int32_t Agent::get_values(vector<string> names, string &json)
        {
            if(names.size() == 0) return 0;
            string jsonlist = "{";
            for(string p: names) {
                jsonlist += "\"" + p + "\",";
            }
            jsonlist.pop_back();
            jsonlist+= "}";

            int32_t status = req_getvalue(jsonlist, json, this);
            return status;
        }

        int32_t Agent::init_channels(uint32_t verification)
        {
            return channels.Init(verification);
        }

        int32_t Agent::set_verification(uint32_t verification)
        {
            channels.verification = verification;
            return 0;
        }

        uint32_t Agent::get_verification()
        {
            return channels.verification;
        }

        int32_t Agent::check_verification(uint32_t verification)
        {
            return channels.Check(verification);
        }

        int32_t Agent::push_response(string name, uint8_t sourceid, uint8_t dest, uint32_t id, vector<uint8_t> response)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }
            return push_response(number, sourceid, dest, id, response);
        }

        int32_t Agent::push_response(uint8_t number, uint8_t sourceid, uint8_t dest, uint32_t id, vector<uint8_t> response)
        {
            // int32_t iretn = 0;
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            if (response.size())
            {
                PacketComm packet;
                PacketComm::ResponseHeader header;
                header.source_id = sourceid;
                if (id)
                {
                    header.response_id = id;
                }
                else
                {
                    header.response_id = centisec();
                }
                header.deci = decisec();
                uint16_t chunk_size = channels.channel[number].datasize - COSMOS_SIZEOF(PacketComm::ResponseHeader);

                if (response.size() / chunk_size > 254)
                {
                    header.chunks = 255;
                }
                else
                {
                    header.chunks = (response.size() - 1) / chunk_size + 1;
                }
                packet.header.type = PacketComm::TypeId::DataObcResponse;
                packet.header.nodeorig = nodeId;
                packet.header.nodedest = dest;
                for (header.chunk_id=0; header.chunk_id<header.chunks; ++header.chunk_id)
                {
                    uint16_t chunk_begin = header.chunk_id * chunk_size;
                    uint16_t chunk_end = chunk_begin + chunk_size;
                    if (chunk_end > response.size())
                    {
                        chunk_end = response.size();
                    }
                    packet.data.resize(COSMOS_SIZEOF(PacketComm::ResponseHeader));
                    memcpy(packet.data.data(), &header, COSMOS_SIZEOF(PacketComm::ResponseHeader));
                    packet.data.insert(packet.data.end(), &response[chunk_begin], &response[chunk_end]);
                    channel_push(number, packet); // channels.Push(number, packet);
                    monitor_unwrapped(number, packet, "Response");
                }
            }
            return response.size();
        }

        int32_t Agent::push_response(string name, uint8_t sourceid, uint8_t dest, uint32_t id, string response)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }
            return push_response(number, sourceid, dest, id, response);
        }

        int32_t Agent::push_response(uint8_t number, uint8_t sourceid, uint8_t dest, uint32_t id, string response)
        {
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            vector<uint8_t> bresponse;
            bresponse.insert(bresponse.end(), response.begin(), response.end());
            return push_response(number, sourceid, dest, id, bresponse);
        }

        int32_t Agent::push_hardware_response(PacketComm::TypeId type, string name, uint8_t dest, uint8_t unit, uint8_t command, vector<uint8_t> response)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }
            return push_hardware_response(type, number, dest, unit, command, response);
        }

        int32_t Agent::push_hardware_response(PacketComm::TypeId type, uint8_t number, uint8_t dest, uint8_t unit, uint8_t command, vector<uint8_t> response)
        {
            // int32_t iretn = 0;
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            if (response.size())
            {
                PacketComm packet;
                PacketComm::CommunicateResponseHeader header;
                header.deci = decisec();
                header.unit = unit;
                header.command = command;
                uint16_t chunk_size = channels.channel[number].datasize - sizeof(header);

                if (response.size() / chunk_size > 254)
                {
                    header.chunks = 255;
                }
                else
                {
                    header.chunks = (response.size() - 1) / chunk_size + 1;
                }
                packet.header.type = type;
                packet.header.chanin = number;
                packet.header.nodeorig = nodeId;
                packet.header.nodedest = dest;
                for (header.chunk_id=0; header.chunk_id<header.chunks; ++header.chunk_id)
                {
                    uint16_t chunk_begin = header.chunk_id * chunk_size;
                    uint16_t chunk_end = chunk_begin + chunk_size;
                    if (chunk_end > response.size())
                    {
                        chunk_end = response.size();
                    }
                    packet.data.resize(sizeof(header));
                    memcpy(packet.data.data(), &header, sizeof(header));
                    packet.data.insert(packet.data.end(), &response[chunk_begin], &response[chunk_end]);
                    /*iretn =*/ channels.Push(number, packet);
                    //                    if (iretn > 0)
                    //                    {
                    //                        monitor_unwrapped(number, packet, to_label("Response", static_cast<uint8_t>(type)));
                    //                    }
                }
            }
            return response.size();
        }

        int32_t Agent::monitor_unwrapped(string name, PacketComm &packet, string extra)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }
            return monitor_unwrapped(number, packet, extra);
        }

        int32_t Agent::monitor_unwrapped(uint8_t number, PacketComm &packet, string extra)
        {
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            string bytes;
            for (uint16_t i=0; i<std::min(static_cast<size_t>(12), packet.data.size()); ++i)
            {
                bytes += " " + to_hex(packet.data[i], 2, true);
            }
            if (extra.empty())
            {
                debug_log.Printf("[Name=%s, Enabled=%u, Size=%lu Age=%f Length=%u PacketCnt=%u ByteCnt=%lu] [Type=0x%x, Size=%lu Node=[%u %u] Chan=[%u %u]] %s\n", channel_name(number).c_str(), channel_enabled(number), packet.packetized.size(), channel_age(number), channel_size(number), channel_packets(number), channel_bytes(number), static_cast<uint16_t>(packet.header.type), packet.data.size(), packet.header.nodeorig, packet.header.nodedest, packet.header.chanin, packet.header.chanout, bytes.c_str());
            }
            else
            {
                debug_log.Printf("%s [Name=%s, Enabled=%u, Size=%lu Age=%f Length=%u PacketCnt=%u ByteCnt=%lu] [Type=0x%x, Size=%lu Node=[%u %u] Chan=[%u %u]] %s\n", extra.c_str(), channel_name(number).c_str(), channel_enabled(number), packet.packetized.size(), channel_age(number), channel_size(number), channel_packets(number), channel_bytes(number), static_cast<uint16_t>(packet.header.type), packet.data.size(), packet.header.nodeorig, packet.header.nodedest, packet.header.chanin, packet.header.chanout, bytes.c_str());
            }
            return 0;
        }

        int32_t Agent::monitor_unpacketized(string name, PacketComm &packet, string extra)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }
            return monitor_unpacketized(number, packet, extra);
        }

        int32_t Agent::monitor_unpacketized(uint8_t number, PacketComm &packet, string extra)
        {
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            string bytes;
            for (uint16_t i=0; i<std::min(static_cast<size_t>(12), packet.data.size()); ++i)
            {
                bytes += " " + to_hex(packet.data[i], 2, true);
            }
            if (extra.empty())
            {
                debug_log.Printf("[Name=%s, Enabled=%u, Size=%lu Age=%f Length=%u PacketCnt=%u ByteCnt=%lu] %s\n", channel_name(number).c_str(), channel_enabled(number), packet.packetized.size(), channel_age(number), channel_size(number), channel_packets(number), channel_bytes(number), bytes.c_str());
            }
            else
            {
                debug_log.Printf("%s [Name=%s, Enabled=%u, Size=%lu Age=%f Length=%u PacketCnt=%u ByteCnt=%lu]\n", extra.c_str(), channel_name(number).c_str(), channel_enabled(number), packet.packetized.size(), channel_age(number), channel_size(number), channel_packets(number), channel_bytes(number), bytes.c_str());
            }
            return 0;
        }

        int32_t Agent::channel_count()
        {
            return channels.channel.size();
        }

        int32_t Agent::channel_push(string name, PacketComm& packet)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }
            return channel_push(number, packet);
        }

        int32_t Agent::channel_push(uint8_t number, PacketComm& packet)
        {
            int32_t iretn=0;
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            iretn = channels.Push(number, packet);

            return iretn;
        }

        int32_t Agent::channel_push(PacketComm& packet)
        {
            int32_t iretn=0;
            if (packet.header.chanout >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            iretn = channels.Push(packet.header.chanout, packet);

            return iretn;
        }

        int32_t Agent::channel_push(string name, vector<PacketComm>& packets)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }
            return channel_push(number, packets);
        }

        int32_t Agent::channel_push(uint8_t number, vector<PacketComm>& packets)
        {
            int32_t iretn;
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            for (PacketComm &p : packets)
            {
                iretn = channel_push(number, p);
                if (iretn < 0)
                {
                    return iretn;
                }
            }
            return packets.size();
        }

        int32_t Agent::channel_pull(string name, PacketComm &packet)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }
            return channel_pull(number, packet);
        }

        int32_t Agent::channel_pull(uint8_t number, PacketComm &packet)
        {
            int32_t iretn=0;
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            iretn = channels.Pull(number, packet);

            return iretn;
        }

        int32_t Agent::channel_add(string name, uint16_t datasize, uint16_t rawsize, float byte_rate, uint16_t maximum)
        {
            int32_t iretn = 0;
            if (channels.channel.size() == 0)
            {
                iretn = channels.Init(channels.verification);
                if (iretn < 0)
                {
                    return iretn;
                }
            }
            if (maximum == 0)
            {
                if (byte_rate <= 0.)
                {
                    if (rawsize == 0)
                    {
                        if (datasize == 0)
                        {
                            return channels.Add(name);
                        }
                        else
                        {
                            return channels.Add(name, datasize);
                        }
                    }
                    else
                    {
                        return channels.Add(name, datasize, rawsize);
                    }
                }
                else
                {
                    return channels.Add(name, datasize, rawsize, byte_rate);
                }
            }
            else
            {
                return channels.Add(name, datasize, rawsize, byte_rate, maximum);
            }
        }

        int32_t Agent::channel_update(string name, uint16_t datasize, uint16_t rawsize, float byte_rate, uint16_t maximum)
        {
            return channels.Update(name, datasize, rawsize, byte_rate, maximum);
        }

        int32_t Agent::channel_update(uint8_t number, uint16_t datasize, uint16_t rawsize, float byte_rate, uint16_t maximum)
        {
            return channels.Update(number, datasize, rawsize, byte_rate, maximum);
        }

        int32_t Agent::channel_size(string name)
        {
            return channels.Size(name);
        }

        int32_t Agent::channel_size(uint8_t number)
        {
            return channels.Size(number);
        }

        float Agent::channel_speed(string name)
        {
            return channels.ByteRate(name);
        }

        float Agent::channel_speed(uint8_t number)
        {
            return channels.ByteRate(number);
        }

        double Agent::channel_age(string name)
        {
            return channels.Age(name);
        }

        double Agent::channel_age(uint8_t number)
        {
            return channels.Age(number);
        }

        double Agent::channel_wakeup_timer(string name, double value)
        {
            return channels.WakeupTimer(name, value);
        }

        double Agent::channel_wakeup_timer(uint8_t number, double value)
        {
            return channels.WakeupTimer(number, value);
        }

        size_t Agent::channel_bytes(string name)
        {
            return channels.Bytes(name);
        }

        size_t Agent::channel_bytes(uint8_t number)
        {
            return channels.Bytes(number);
        }

        size_t Agent::channel_level(string name)
        {
            return channels.Level(name);
        }

        size_t Agent::channel_level(uint8_t number)
        {
            return channels.Level(number);
        }

        uint32_t Agent::channel_packets(string name)
        {
            return channels.Packets(name);
        }

        uint32_t Agent::channel_packets(uint8_t number)
        {
            return channels.Packets(number);
        }

        double Agent::channel_touch(string name, double seconds)
        {
            return channels.Touch(name, seconds);
        }

        double Agent::channel_touch(uint8_t number, double seconds)
        {
            return channels.Touch(number, seconds);
        }

        ssize_t Agent::channel_increment(string name, size_t bytes, uint32_t packets)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }

            return channel_increment(number, bytes, packets);
        }

        ssize_t Agent::channel_increment(uint8_t number, size_t bytes, uint32_t packets)
        {
            int32_t iretn = 0;
            iretn = channels.Increment(number, bytes, packets);
            return iretn;
        }

//        ssize_t Agent::channel_decrement(string name, size_t bytes, uint32_t packets)
//        {
//            int32_t number = channel_number(name);
//            if (number < 0)
//            {
//                return number;
//            }

//            return channel_decrement(number, bytes, packets);
//        }

//        ssize_t Agent::channel_decrement(uint8_t number, size_t bytes, uint32_t packets)
//        {
//            int32_t iretn = 0;
//            iretn = channels.Decrement(number, bytes, packets);
//            return iretn;
//        }

        int32_t Agent::channel_teststart(string name, string radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }
            int32_t nradio = channel_number(radio);
            if (nradio < 0)
            {
                return nradio;
            }

            return channel_teststart(number, nradio, id, orig, dest, start, step, stop, total);
        }

        int32_t Agent::channel_teststart(uint8_t number, uint8_t radio, uint32_t id, uint8_t orig, uint8_t dest, uint8_t start, uint8_t step, uint8_t stop, uint32_t total)
        {
            int32_t iretn = 0;
            iretn = channels.TestStart(number, radio, id, orig, dest, start, step, stop, total);
            return iretn;
        }

        int32_t Agent::channel_teststop(string name, float seconds)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }

            return channel_teststop(number, seconds);
        }

        int32_t Agent::channel_teststop(uint8_t number, float seconds)
        {
            int32_t iretn = 0;
            iretn = channels.TestStop(number, seconds);
            return iretn;
        }

        int32_t Agent::channel_enable(string name, int8_t value)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }

            return channels.Enable(number, value);
        }

        int32_t Agent::channel_enable(uint8_t number, int8_t value)
        {
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            return channels.Enable(number, value);
        }

        int32_t Agent::channel_enabled(string name)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }

            return channels.Enabled(number);
        }

        int32_t Agent::channel_enabled(uint8_t number)
        {
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            return channels.Enabled(number);
        }

        //        int32_t Agent::channel_disable(string name)
        //        {
        //            int32_t number = channel_number(name);
        //            if (number < 0)
        //            {
        //                return number;
        //            }

        //            return channels.Disable(number);
        //        }

        //        int32_t Agent::channel_disable(uint8_t number)
        //        {
        //            if (number >= channels.channel.size())
        //            {
        //                return GENERAL_ERROR_OUTOFRANGE;
        //            }

        //            return channels.Disable(number);
        //        }

        int32_t Agent::channel_clear(string name)
        {
            int32_t number = channel_number(name);
            if (number < 0)
            {
                return number;
            }

            return channels.Clear(number);
        }

        int32_t Agent::channel_clear(uint8_t number)
        {
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }

            return channels.Clear(number);
        }

        int32_t Agent::channel_number(string name)
        {
            int32_t iretn = 0;
            if (channels.channel.size() == 0)
            {
                iretn = channels.Init();
                if (iretn < 0)
                {
                    return iretn;
                }
            }
            return channels.Find(name);
        }

        string Agent::channel_name(uint8_t number)
        {
            return channels.Find(number);
        }

        int32_t Agent::channel_datasize(string name)
        {
            int32_t iretn = channel_number(name);
            if (iretn < 0)
            {
                return iretn;
            }
            return channels.channel[iretn].datasize;
        }

        int32_t Agent::channel_datasize(uint8_t number)
        {
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            return channels.channel[number].datasize;
        }

        int32_t Agent::channel_rawsize(string name)
        {
            int32_t iretn = channel_number(name);
            if (iretn < 0)
            {
                return iretn;
            }
            return channels.channel[iretn].rawsize;
        }

        int32_t Agent::channel_rawsize(uint8_t number)
        {
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            return channels.channel[number].rawsize;
        }

        int32_t Agent::channel_maximum(string name)
        {
            int32_t iretn = channel_number(name);
            if (iretn < 0)
            {
                return iretn;
            }
            return channel_maximum(iretn);
        }

        int32_t Agent::channel_maximum(uint8_t number)
        {
            if (number >= channels.channel.size())
            {
                return GENERAL_ERROR_OUTOFRANGE;
            }
            return channels.channel[number].maximum;
        }

        int32_t Agent::task_add(string command, string source)
        {
            return tasks.Add(command, source);
        }

        int32_t Agent::task_del(uint32_t deci)
        {
            return tasks.Del(deci);
        }

        int32_t Agent::task_iretn(uint16_t number)
        {
            return tasks.Iretn(number);
        }

        uint32_t Agent::task_deci(uint16_t number)
        {
            return tasks.Deci(number);
        }

        double Agent::task_startmjd(uint16_t number)
        {
            return tasks.Startmjd(number);
        }

        uint8_t Agent::task_state(uint16_t number)
        {
            return tasks.State(number);
        }

        string Agent::task_command(uint16_t number)
        {
            return tasks.Command(number);
        }

        string Agent::task_path(uint16_t number)
        {
            return tasks.Path(number);
        }

        int32_t Agent::task_size()
        {
            return tasks.Size();
        }

        double Agent::get_timeStart()
        {
            return timeStart;
        }
    } // end of namespace Support
} // end namespace Cosmos

//! @}
