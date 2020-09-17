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
#include "support/socketlib.h"
#include "support/cosmos-errno.h"
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

        //! Add COSMOS awareness.
        //! Sets up minimum framework for COSMOS awareness. The minimum call makes a nodeless client, setting up the
        //! message ring buffer thread, and a main thread of execution. Additional parameters are related to making
        //! the program a true Agent by tieing it to a node, and starting the request and heartbeat threads.
        //! \param ntype Transport Layer protocol to be used, taken from ::NetworkType. Defaults to UDP Broadcast.
        //! \param nname Node name. Defaults to empty.
        //! \param aname Agent name. Defaults to empty. If this is defined, the full Agent code will be started.
        //! \param bprd Period, in seconds, for heartbeat. Defaults to 1.
        //! \param bsize Size of interagent communication buffer. Defaults to ::AGENTMAXBUFFER.
        //! \param mflag Boolean controlling weyher or not multiple instances of the same Agent can start. If true, then Agent names
        //! will have an index number appended (eg: myname_001). If false, agent will listen for 5 seconds and terminate if it senses
        //! the Agent already running.
        //! \param portnum The network port to listen on for requests. Defaults to 0 whereupon it will use whatever th OS assigns.
        //! \param dlevel debug level. Defaults to 1 so that if there is an error the user can immediately see it. also initialized in the namespace variables
        Agent::Agent(const string &nname, const string &aname, double bprd, uint32_t bsize, bool mflag, int32_t portnum, NetworkType ntype, uint16_t dlevel)
        {
            int32_t iretn;

            double timeStart = currentmjd();
            debug_level = dlevel;


            // Initialize COSMOS data space
            cinfo = json_init();

            if (cinfo == nullptr)
            {
                return;
            }

            cinfo->agent[0].stateflag = static_cast<uint16_t>(State::INIT);

            // Establish subscribe channel
            iretn = subscribe(ntype, AGENTMCAST, AGENTSENDPORT, 1000);
            if (iretn)
            {
                shutdown();
                error_value = iretn;
                return;
            }

            // Set up node: shorten if too long, use hostname if it's empty.
            nodeName = nname;
            if ((iretn=json_setup_node(nodeName, cinfo)) != 0)
            {
                error_value = iretn;
                shutdown();
                return;
            }

            strcpy(cinfo->node.name, nodeName.c_str());

            cinfo->agent[0].client = 1;
            cinfo->node.utc = 0.;
            strncpy(cinfo->agent[0].beat.node, cinfo->node.name ,COSMOS_MAX_NAME);

            // Establish publish channel
            cinfo->agent[0].beat.ntype = ntype;
            iretn = publish(cinfo->agent[0].beat.ntype, AGENTSENDPORT);
            if (iretn)
            {
                error_value = iretn;
                shutdown();
                return;
            }

            // Start message listening thread
            mthread = thread([=] { message_loop(); });
            COSMOS_SLEEP(.1);

            // Return if all we are doing is setting up client.
            if (aname.empty())
            {
                strcpy(cinfo->agent[0].beat.proc, "null");
                cinfo->agent[0].stateflag = static_cast <uint16_t>(Agent::State::RUN);
                return;
            }

            if (strlen(cinfo->node.name)>COSMOS_MAX_NAME || aname.length()>COSMOS_MAX_NAME)
            {
                error_value = JSON_ERROR_NAME_LENGTH;
                shutdown();
                return;
            }


            // If not Multi, check if this Agent is already running
            char tname[COSMOS_MAX_NAME+1];
            if (!mflag)
            {
                if (get_server(cinfo->node.name, aname, timeoutSec, (beatstruc *)nullptr))
                {
                    error_value = AGENT_ERROR_SERVER_RUNNING;
                    shutdown();
                    return;
                }
                strcpy(tname,aname.c_str());
            }
            else // then there is an agent running with the given name, so let's make the name unique
            {
                if (strlen(cinfo->node.name)>COSMOS_MAX_NAME-4 || aname.size()>COSMOS_MAX_NAME-4)
                {
                    error_value = JSON_ERROR_NAME_LENGTH;
                    shutdown();
                    return;
                }

                uint32_t i=0;
                do
                {
                    sprintf(tname,"%s_%03d",aname.c_str(),i);
                    if (!get_server(cinfo->node.name, tname, timeoutSec, (beatstruc *)nullptr))
                    {
                        break;
                    }
                } while (++i<100);
            }

            // Initialize important server variables

            strncpy(cinfo->agent[0].beat.node, cinfo->node.name, COSMOS_MAX_NAME);
            strncpy(cinfo->agent[0].beat.proc, tname, COSMOS_MAX_NAME);
            agentName = cinfo->agent[0].beat.proc;

            if (debug_level)
            {
                fprintf(get_debug_fd(), "------------------------------------------------------\n");
                fprintf(get_debug_fd(), "COSMOS AGENT '%s' on node '%s'\n", aname.c_str(), nname.c_str());
                fprintf(get_debug_fd(), "Version %s built on %s %s\n", version.c_str(),  __DATE__, __TIME__);
                fprintf(get_debug_fd(), "Agent started at %s\n", mjdToGregorian(timeStart).c_str());
                fprintf(get_debug_fd(), "Debug level %u\n", debug_level);
                fprintf(get_debug_fd(), "------------------------------------------------------\n");
            }

            if (bprd >= AGENT_HEARTBEAT_PERIOD_MIN)
            {
                cinfo->agent[0].beat.bprd = bprd;
            }
            else
            {
                cinfo->agent[0].beat.bprd = 0.;
            }
            cinfo->agent[0].stateflag = static_cast<uint16_t>(State::INIT);
            cinfo->agent[0].beat.port = static_cast<uint16_t>(portnum);
            cinfo->agent[0].beat.bsz = (bsize<=AGENTMAXBUFFER-4?bsize:AGENTMAXBUFFER-4);

#ifdef COSMOS_WIN_BUILD_MSVC
            cinfo->agent[0].pid = _getpid();
#else
            cinfo->agent[0].pid = getpid();
#endif
            cinfo->agent[0].aprd = 1.;
            strncpy(cinfo->agent[0].beat.user, "cosmos", COSMOS_MAX_NAME);

            // Start the heartbeat and request threads running
            //    iretn = start();
            hthread = thread([=] { heartbeat_loop(); });
            cthread = thread([=] { request_loop(); });
            if (!hthread.joinable() || !cthread.joinable())
            {
                // TODO: create error value
                //error_value = iretn;
                shutdown();
                return;
            }

            //! Set up initial requests
            add_request("help",req_help,"","list of available requests for this agent");
            add_request("help_json",req_help_json,"","list of available requests for this agent (but in json)");
            add_request("shutdown",req_shutdown,"","request to shutdown this agent");
            Agent::add_request("idle",Agent::req_idle,"","request to transition this agent to idle state");
            Agent::add_request("init",Agent::req_init,"","request to transition this agent to init state");
            Agent::add_request("monitor",Agent::req_monitor,"","request to transition this agent to monitor state");
            Agent::add_request("reset",Agent::req_reset,"","request to transition this agent to reset state");
            Agent::add_request("run",Agent::req_run,"","request to transition this agent to run state");
            Agent::add_request("status",Agent::req_status,"","request the status of this agent");
            Agent::add_request("debug_level",Agent::req_debug_level,"{\"name1\",\"name2\",...}","get/set debug_level of agent");
            Agent::add_request("getvalue",Agent::req_getvalue,"{\"name1\",\"name2\",...}","get specified value(s) from agent");
            Agent::add_request("setvalue",Agent::req_setvalue,"{\"name1\":value},{\"name2\":value},...}","set specified value(s) in agent");
            Agent::add_request("listnames",Agent::req_listnames,"","list the Namespace of the agent");
            Agent::add_request("forward",Agent::req_forward,"nbytes packet","Broadcast JSON packet to the default SEND port on local network");
            Agent::add_request("echo",Agent::req_echo,"utc crc nbytes bytes","echo array of nbytes bytes, sent at time utc, with CRC crc.");
            Agent::add_request("nodejson",Agent::req_nodejson,"","return description JSON for Node");
            Agent::add_request("statejson",Agent::req_statejson,"","return description JSON for State vector");
            Agent::add_request("utcstartjson",Agent::req_utcstartjson,"","return description JSON for UTC Start time");
            Agent::add_request("piecesjson",Agent::req_piecesjson,"","return description JSON for Pieces");
            Agent::add_request("vertexsjson",Agent::req_vertexsjson,"","return description JSON for Pieces");
            Agent::add_request("facesjson",Agent::req_facesjson,"","return description JSON for Pieces");
            Agent::add_request("devgenjson",Agent::req_devgenjson,"","return description JSON for General Devices");
            Agent::add_request("devspecjson",Agent::req_devspecjson,"","return description JSON for Specific Devices");
            Agent::add_request("portsjson",Agent::req_portsjson,"","return description JSON for Ports");
            Agent::add_request("targetsjson",Agent::req_targetsjson,"","return description JSON for Targets");
            Agent::add_request("aliasesjson",Agent::req_aliasesjson,"","return description JSON for Aliases");
            Agent::add_request("heartbeat",Agent::req_heartbeat,"","Post a hearbeat");
            Agent::add_request("postsoh",Agent::req_postsoh,"","Post a SOH");
            Agent::add_request("utc",Agent::req_utc,"utc","Get UTC as both Modified Julian Day and Unix Time");
            Agent::add_request("soh",Agent::req_soh,"soh","Get Limited SOH string");
            Agent::add_request("fullsoh",Agent::req_fullsoh,"fullsoh","Get Full SOH string");
            Agent::add_request("jsondump",Agent::req_jsondump,"jsondump","Dump JSON ini files to node folder");

            // Set up Full SOH string
//            set_fullsohstring(json_list_of_fullsoh(cinfo));

            cinfo->agent[0].server = 1;
            cinfo->agent[0].stateflag = static_cast<uint16_t>(Agent::State::RUN);


            activeTimeout = currentmjd() + cinfo->agent[0].aprd / 86400.;


        }


        Agent::~Agent()
        {
            Agent::shutdown();
        }

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
            if (reqs.size() > AGENTMAXREQUESTCOUNT)
                return (AGENT_ERROR_REQ_COUNT);

            request_entry tentry;
            if (token.size() > COSMOS_MAX_NAME)
            {
                token.resize(COSMOS_MAX_NAME);
            }
            tentry.token = token;
//            tentry.ifunction = nullptr;
            tentry.efunction = function;
            tentry.synopsis = synopsis;
            tentry.description = description;
            reqs.push_back(tentry);
            return 0;
        }

        //! Start Agent Request and Heartbeat loops
        /*!	Starts the request and heartbeat threads for an Agent server initialized with
 * Cosmos::Agent::Agent. The Agent will open its request and heartbeat channels using the
 * address and port supplied in cinfo. The heartbeat will cycle with the period requested in cinfo.
    \return value returned by request thread create
*/
        int32_t Agent::start()
        {

            // start heartbeat thread
            hthread = thread([=] { heartbeat_loop(); });
            cthread = thread([=] { request_loop(); });
            return 0;
        }

        //! Begin Active Loop
        //! Initializes timer for active loop using ::aprd
        //! \return Zero or negative error.
        int32_t Agent::start_active_loop()
        {
            activeTimeout = currentmjd() + cinfo->agent[0].aprd / 86400.;
            return 0;
        }

        //! Finish active loop
        //! Sleep for the remainder of this loops ::aprd as initialized in ::start_active_loop.
        //! \return Zero or negative error.
        int32_t Agent::finish_active_loop()
        {
            double sleepsec = 86400.*(activeTimeout - currentmjd());
            activeTimeout += cinfo->agent[0].aprd / 86400.;
            COSMOS_SLEEP(sleepsec);
            return 0;
        }

        //! Shutdown agent gracefully
        /*! Waits for threads to stop running if we are a server, then releases everything.
 * \return 0 or negative error.
 */
        int32_t Agent::shutdown()
        {
            if (debug_level)
            {
                fprintf(get_debug_fd(), "Shutting down Agent. Last error: %s\n", cosmos_error_string(error_value).c_str());
                fflush(stdout);
            }

            if (cinfo != nullptr)
            {
                cinfo->agent[0].stateflag = static_cast <uint16_t>(Agent::State::SHUTDOWN);
            }
            if (agentName.size())
            {
                if (hthread.joinable())
                {
                    hthread.join();
                }
                if (cthread.joinable())
                {
                    cthread.join();
                }
                Agent::unpublish();
            }
            if (mthread.joinable())
            {
                mthread.join();
            }
            Agent::unsubscribe();
            json_destroy(cinfo);
            cinfo = nullptr;
            return 0;
        }

        //! Check if we're supposed to be running
        /*!	Returns the value of the internal variable that indicates that
 * the threads are running.
    \return Value of internal state variable, as enumerated in Cosmos::Agent:State.
*/
        uint16_t Agent::running()
        {
            return (cinfo->agent[0].stateflag);
        }

        //! Wait on state
        //! Wait for up to waitsec seconds for Agent to enter requested state
        //! \param state Desired ::Agent::State.
        //! \param waitsec Maximum number of seconds to wait.
        //! \return Zero, or timeout error.
        int32_t Agent::wait(State state, float waitsec)
        {
            if (cinfo == nullptr)
            {
                return AGENT_ERROR_NULL;
            }

            ElapsedTime et;
            while (cinfo->agent[0].stateflag != static_cast <uint16_t>(state) && et.split() < waitsec)
            {
                COSMOS_SLEEP(.1);
            }
            if (cinfo->agent[0].stateflag == static_cast <uint16_t>(state))
            {
                return 0;
            }
            else
            {
                return GENERAL_ERROR_TIMEOUT;
            }
        }

        //! Last error value.
        //! Get value of last error returned by any function.
        int32_t Agent::last_error()
        {
            return (error_value);
        }

        //! Send a request over AGENT
        /*! Send a request string to the process at the provided address
    \param hbeat The agent ::beatstruc
    \param request the request and its arguments
    \param output any output returned by the request
    \param waitsec Maximum number of seconds to wait
    \return Either the number of bytes returned, or an error number.
*/
        int32_t Agent::send_request(beatstruc hbeat, string request, string &output, float waitsec)
        {
            socket_channel sendchan;
            int32_t iretn;
            int32_t nbytes;

            vector <char> toutput;
            toutput.resize(AGENTMAXBUFFER+1);

            if (hbeat.utc == 0. || hbeat.addr[0] == 0 || hbeat.port == 0)
                return (AGENT_ERROR_SOCKET);

            ElapsedTime ep;
            ep.start();

            if ((iretn=socket_open(&sendchan, NetworkType::UDP, hbeat.addr, hbeat.port, SOCKET_TALK, SOCKET_BLOCKING, AGENTRCVTIMEO)) < 0)
            {
                return (-errno);
            }

            nbytes = strnlen(request.c_str(), hbeat.bsz);
            if ((nbytes=sendto(sendchan.cudp, request.c_str(), nbytes, 0, (struct sockaddr *)&sendchan.caddr, sizeof(struct sockaddr_in))) < 0)
            {
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
                toutput[nbytes] = 0;
                toutput.resize(nbytes+1);
                string reply(toutput.begin(), toutput.end());
                output = reply;
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
        int32_t Agent::send_request_jsonnode(beatstruc hbeat, jsonnode &jnode, float waitsec)
        {
            int32_t iretn;

            jnode.name = hbeat.node;
            iretn = send_request(hbeat, "nodejson", jnode.node, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = send_request(hbeat, "statejson", jnode.state, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = send_request(hbeat, "utcstartjson", jnode.utcstart, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = send_request(hbeat, "piecesjson", jnode.pieces, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = send_request(hbeat, "facesjson", jnode.faces, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = send_request(hbeat, "vertexsjson", jnode.vertexs, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = send_request(hbeat, "devgenjson", jnode.devgen, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = send_request(hbeat, "devspecjson", jnode.devspec, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = send_request(hbeat, "portsjson", jnode.ports, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = send_request(hbeat, "aliasesjson", jnode.aliases, waitsec);
            if (iretn < 0)
            {
                return iretn;
            }
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
        int32_t Agent::get_server(string node, string name, float waitsec, beatstruc *rbeat)
        {
            if (node.empty())
            {
                node = nodeName;
            }

            //! 3. Loop for ::waitsec seconds, or until we discover desired heartbeat.

            ElapsedTime ep;
            ep.start();

            post(Agent::AgentMessage::REQUEST, "heartbeat");
            COSMOS_SLEEP(.1);
            do
            {
                for (size_t i=0; i<agent_list.size(); ++i)
                {
                    if (name == agent_list[i].proc && (node == "any" || node == agent_list[i].node))
                    {
                        if (rbeat != NULL)
                        {
                            *rbeat = agent_list[i];
                        }
                        return (1);
                    }
                }
                COSMOS_SLEEP(.1);

            } while (ep.split() <= waitsec);

            return(0);
        }

        //! Find agent
        /*! Check the Cosmos::Agent::agent_list for the particular agent,
 * returning its heartbeat if found.
    \param agent Name of agent.
    \param node Node that agent is in.
    \return ::beatstruc of located agent, otherwise empty ::beatstruc.
 */
        beatstruc Agent::find_agent(string node, string agent, float waitsec)
        {
            if (node.empty())
            {
                node = nodeName;
            }
            post(AgentMessage::REQUEST, "heartbeat");
            COSMOS_SLEEP(.1);

            ElapsedTime ep;
            ep.start();
            do
            {
                for (beatstruc &it : agent_list)
                {
                    if ((node == "any" || it.node == node) && it.proc == agent)
                    {
                        it.exists = true;
                        return it;
                    }
                }
                COSMOS_SLEEP(.1);
            } while (ep.split() < waitsec);

            beatstruc nobeat;
            nobeat.exists = false;
            nobeat.node[0] = '\0';
            return nobeat;
        }

        //! Find single server
        /*! Listen to the local subnet for a set amount of time,
 * collecting heartbeats, searching for a particular agent.
    \param node Node that agent is in.
    \param proc Name of agent.
    \param waitsec Maximum number of seconds to wait.
    \return ::beatstruc of located agent, otherwise empty ::beatstruc.
 */
        beatstruc Agent::find_server(string node, string agent, float waitsec)
        {
            beatstruc cbeat;

            cbeat = find_agent(agent, node, waitsec);

            return cbeat;
        }

        //! Generate a list of request servers.
        /*! Listen to the local subnet for a set amount of time,
 * collecting heartbeats. Return a list of heartbeats collected.
    \param waitsec Maximum number of seconds to wait.
    \return A vector of ::beatstruc entries listing the unique servers found.
*/
        vector<beatstruc> Agent::find_servers(float waitsec)
        {
            beatstruc tbeat;

            //! Loop for ::waitsec seconds, filling list with any discovered heartbeats.

            ElapsedTime ep;
            ep.start();

            do
            {
                for (size_t k=0; k<agent_list.size(); ++k)
                {
                    size_t i;
                    for (i=0; i<slist.size(); i++)
                    {
                        if (!strcmp(agent_list[k].node, slist[i].node) && !strcmp(agent_list[k].proc, slist[i].proc))
                            break;
                    }
                    if (i == slist.size())
                    {
                        slist.push_back(agent_list[k]);
                        for (size_t j=i; j>0; j--)
                        {
                            if (slist[j].port > slist[j-1].port)
                                break;
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
        int32_t Agent::set_sohstring(string list)
        {

            if (!sohtable.empty())
            {
                sohtable.clear();
            }

            json_table_of_list(sohtable, list, cinfo);
            return 0;
        }

        //! Set Full SOH string
        /*! Set the Full SOH string to a JSON list of \ref jsonlib_namespace names. A
 * proper JSON list will begin and end with matched curly braces, be comma separated,
 * and have all strings in double quotes.
    \param list Properly formatted list of JSON names.
    \return 0, otherwise a negative error.
*/
        int32_t Agent::set_fullsohstring(string list)
        {

            if (!fullsohtable.empty())
            {
                fullsohtable.clear();
            }

            json_table_of_list(fullsohtable, list, cinfo);
            return 0;
        }

        //! Return Agent ::cosmosstruc
        /*! Return a pointer to the Agent's internal copy of the ::cosmosstruc.
    \return A pointer to the ::cosmosstruc, otherwise NULL.
*/
        cosmosstruc *Agent::get_cosmosstruc()
        {
            return (cinfo);
        }

        //! Heartbeat Loop
        /*! This function is run as a thread to provide the Heartbeat for the Agent. The Heartbeat will
 * consist of the contents of Agent::AGENT_MESSAG::BEAT in Cosmos::Agent::poll, plus the contents of the
 * Cosmos::Agent::sohstring. It will come every beatstruc::bprd seconds.
 */
        void Agent::heartbeat_loop()
        {
            ElapsedTime timer_beat;

            while (cinfo->agent[0].stateflag)
            {

                // compute the jitter
                if (cinfo->agent[0].beat.bprd == 0.)
                {
                    cinfo->agent[0].beat.jitter = timer_beat.split() - 1.;
                }
                else
                {
                    cinfo->agent[0].beat.jitter = timer_beat.split() - cinfo->agent[0].beat.bprd;
                }
                timer_beat.start();

                // post comes first
                if (cinfo->agent[0].beat.bprd != 0.)
                {
                    post_beat();
                }

                // TODO: move the monitoring calculations to another thread with its own loop time that can be controlled
                // Compute other monitored quantities if monitoring
                if (cinfo->agent[0].stateflag == static_cast <uint16_t>(Agent::State::MONITOR))
                {
                    // TODO: rename beat.cpu to beat.cpu_percent
                    // add beat.cpu_load
                    cinfo->agent[0].beat.cpu    = deviceCpu_.getPercentUseForCurrentProcess();//cpu.getLoad();
                    cinfo->agent[0].beat.memory = deviceCpu_.getVirtualMemoryUsed();
                }

                if (cinfo->agent[0].stateflag == static_cast <uint16_t>(Agent::State::SHUTDOWN))
                {
                    cinfo->agent[0].beat.cpu = 0;
                    cinfo->agent[0].beat.memory = 0;
                }


                if (cinfo->agent[0].beat.bprd < AGENT_HEARTBEAT_PERIOD_MIN)
                {
                    cinfo->agent[0].beat.bprd = 0.;
                }

                if (cinfo->agent[0].beat.bprd == 0.)
                {
                    COSMOS_SLEEP(1.);
                }
                else
                {
                    if (timer_beat.split() <= cinfo->agent[0].beat.bprd)
                    {
                        COSMOS_SLEEP(cinfo->agent[0].beat.bprd - timer_beat.split());
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
        void Agent::request_loop() noexcept
        {
            int32_t iretn;
            string bufferin;

            if ((iretn = socket_open(&cinfo->agent[0].req, NetworkType::UDP, (char *)"", cinfo->agent[0].beat.port, SOCKET_LISTEN, SOCKET_BLOCKING, 2000000)) < 0)
            {
                return;
            }

            cinfo->agent[0].beat.port = cinfo->agent[0].req.cport;

            bufferin.resize(cinfo->agent[0].beat.bsz);

            while (cinfo->agent[0].stateflag)
            {
                iretn = recvfrom(cinfo->agent[0].req.cudp, &bufferin[0], bufferin.size(), 0, (struct sockaddr *)&cinfo->agent[0].req.caddr, (socklen_t *)&cinfo->agent[0].req.addrlen);

                if (iretn > 0)
                {
                    string bufferout;
                    bufferin[iretn] = 0;
                    process_request(bufferin, bufferout);
                }
            }
            return;
        }

        int32_t Agent::process_request(string &bufferin, string &bufferout)
        {
            size_t i;
            int32_t iretn;

            if (cinfo->agent[0].stateflag == static_cast <uint16_t>(Agent::State::DEBUG))
            {
                printf("Request: [%lu] %s ",bufferin.size(), &bufferin[0]);
                fflush(stdout);
            }

            string variable;
            string request;
            request.resize(AGENTMAXBUFFER+1);
            for (i=0; i<COSMOS_MAX_NAME; i++)
            {
                if (bufferin[i] == ' ' || bufferin[i] == 0)
                    break;
                request[i] = bufferin[i];
            }
            request[i] = 0;

            for (i=0; i<reqs.size(); i++)
            {
                if (!strcmp(&request[0],reqs[i].token.c_str()))
                    break;
            }

            if (i < reqs.size())
            {
                iretn = -1;
//                if (reqs[i].ifunction)
//                {
//                    iretn = (this->*reqs[i].ifunction)(&bufferin[0], &request[0]);
//                }
//                else
//                {
//                    if (reqs[i].efunction != nullptr)
//                    {
//                        iretn = reqs[i].efunction(&bufferin[0], &request[0], this);
//                    }
//                }
                if (reqs[i].efunction != nullptr)
                {
                    iretn = reqs[i].efunction(bufferin, request, this);
                }
                if (iretn >= 0)
                {
                    request.resize(strlen(&request[0]));
                    bufferout = request;
                }
                else
                {
                    bufferout = "[NOK] " + std::to_string(iretn);
                }
            }
            else
            {
                iretn = AGENT_ERROR_NULL;
                bufferout = "[NOK] " + std::to_string(iretn);
            }

            iretn = sendto(cinfo->agent[0].req.cudp, bufferout.data(), bufferout.size(), 0, (struct sockaddr *)&cinfo->agent[0].req.caddr, sizeof(struct sockaddr_in));
            if (cinfo->agent[0].stateflag == static_cast <uint16_t>(Agent::State::DEBUG))
            {
                printf("[%d] %s\n", iretn, bufferout.data());
            }

            return iretn;
        }

        // TODO: describe function, what does it do?
        void Agent::message_loop()
        {
            messstruc mess;
            int32_t iretn;

            // Initialize things
//            message_ring.resize(MESSAGE_RING_SIZE);

            while (Agent::running())
            {
                iretn = Agent::poll(mess, AgentMessage::ALL, 0.);
                if (iretn > 0)
                {
                    if (!strcmp(mess.meta.beat.proc, "null") && mess.adata.empty())
                    {
                        continue;
                    }

                    bool agent_found = false;
                    for (beatstruc &i : agent_list)
                    {
                        if (!strcmp(i.node, mess.meta.beat.node) && !strcmp(i.proc, mess.meta.beat.proc))
                        {
                            agent_found = true;
                            i.utc = mess.meta.beat.utc;
                            break;
                        }
                    }

                    if (!agent_found)
                    {
                        agent_list.push_back(mess.meta.beat);
                    }

                    if (mess.meta.type == AgentMessage::REQUEST && strcmp(cinfo->agent[0].beat.proc, "null"))
                    {
                        string response;
                        process_request(mess.adata, response);
                        Agent::post(AgentMessage::RESPONSE, response);
                    }
                    else
                    {
//                        size_t new_position;
//                        new_position = message_head + 1;
//                        if (new_position >= message_ring.size())
//                        {
//                            new_position = 0;
//                        }
//                        message_ring[new_position] = mess;
//                        message_head = new_position;
                        message_queue.push_back(mess);
                        if (message_queue.size() > MESSAGE_RING_SIZE)
                        {
                            message_queue.pop_front();
                        }
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
        int32_t Agent::req_forward(string &request, string &output, Agent* agent)
        {
            uint16_t count;
            int32_t iretn=-1;

//            sscanf(request.c_str(),"%*s %hu",&count);
            sscanf(request.c_str(),"%*s %hu",&count);
            for (uint16_t i=0; i<agent->cinfo->agent[0].ifcnt; ++i)
            {
//                iretn = sendto(agent->cinfo->agent[0].pub[i].cudp,(const char *)&request[request.length()-count],count,0,(struct sockaddr *)&agent->cinfo->agent[0].pub[i].baddr,sizeof(struct sockaddr_in));
                iretn = sendto(agent->cinfo->agent[0].pub[i].cudp,(const char *)&request[request.size()-count],count,0,(struct sockaddr *)&agent->cinfo->agent[0].pub[i].baddr,sizeof(struct sockaddr_in));
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
        int32_t Agent::req_echo(string &request, string &output, Agent*)
        {
            double mjd;
            uint16_t crc, count;

//            sscanf(request.c_str(),"%*s %lf %hx %hu",&mjd,&crc,&count);
//            sprintf(output,"%.17g %x %u ",currentmjd(0),slip_calc_crc((uint8_t *)&request[request.length()-count],count),count);
//            strncpy(&output[strlen(output)],&request[request.length()-count],count+1);
            sscanf(request.c_str(),"%*s %lf %hx %hu",&mjd,&crc,&count);
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
//        int32_t Agent::req_help_json(char*, char* output, Agent* agent)
        int32_t Agent::req_help_json(string &, string &output, Agent* agent)
        {
            string help_string, s;
            size_t qpos, prev_qpos = 0;
            //        help_string += "\n";
            help_string += "{\"requests\": [";
            for(uint32_t i = 0; i < agent->reqs.size(); ++i)
            {

                //            help_string += "        ";
                if(i>0) help_string+=",";
                help_string += "{\"token\": \"";
                help_string += agent->reqs[i].token;
                //            help_string += " ";
                help_string += "\", \"synopsis\": \"";
                //            help_string += agent->reqs[i].synopsis;
                qpos = 0;
                prev_qpos = 0;
                s = agent->reqs[i].synopsis;
                while((qpos=s.substr(prev_qpos).find("\""))!= string::npos)
                {
                    s.replace(qpos+prev_qpos, 1, "\\\"");
                    prev_qpos +=qpos+2;
                }
                help_string+= s;
                //            help_string += "\n";
                //size_t blanks = (20 - (signed int)strlen(agent->reqs[i].token)) > 0 ? 20 - strlen(agent->reqs[i].token) : 4;
                //string blank(blanks,' ');
                //help_string += blank;
                //            help_string += "                ";
                help_string += "\", \"description\": \"";
                qpos = 0;
                prev_qpos = 0;
                s = agent->reqs[i].description;
                while((qpos=s.substr(prev_qpos).find("\""))!= string::npos){
                    s.replace(qpos+prev_qpos, 1, "\\\"");
                    prev_qpos +=qpos+2;
                }
                help_string+= s;
                //            help_string += agent->reqs[i].description;
                //            help_string += "\n\n";
                help_string +="\"}";
            }
            //        help_string += "\n";
            help_string += "]}";
//            strcpy(output, (char*)help_string.c_str());
            output = help_string;
            return 0;
        }

//        int32_t Agent::req_help(char*, char* output, Agent* agent)
        int32_t Agent::req_help(string &, string &output, Agent* agent)
        {
            string help_string;
            help_string += "\n";
            for(uint32_t i = 0; i < agent->reqs.size(); ++i)
            {
                help_string += "        ";
                help_string += agent->reqs[i].token;
                help_string += " ";
                help_string += agent->reqs[i].synopsis;
                help_string += "\n";
                //size_t blanks = (20 - (signed int)strlen(agent->reqs[i].token)) > 0 ? 20 - strlen(agent->reqs[i].token) : 4;
                //string blank(blanks,' ');
                //help_string += blank;
                help_string += "                ";
                help_string += agent->reqs[i].description;
                help_string += "\n\n";
            }
            help_string += "\n";
//            strcpy(output, (char*)help_string.c_str());
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
        int32_t Agent::req_run(string &, string &output, Agent* agent)
        {
            agent->cinfo->agent[0].stateflag = static_cast <uint16_t>(Agent::State::RUN);
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
        int32_t Agent::req_init(string &, string &output, Agent* agent)
        {
            agent->cinfo->agent[0].stateflag = static_cast <uint16_t>(Agent::State::INIT);
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
        int32_t Agent::req_idle(string &, string &output, Agent* agent)
        {
            agent->cinfo->agent[0].stateflag = static_cast <uint16_t>(Agent::State::IDLE);
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
        int32_t Agent::req_monitor(string &, string &output, Agent* agent)
        {
            agent->cinfo->agent[0].stateflag = static_cast <uint16_t>(Agent::State::MONITOR);
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
        int32_t Agent::req_reset(string &, string &output, Agent* agent)
        {
            agent->cinfo->agent[0].stateflag = static_cast <uint16_t>(Agent::State::RESET);
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
        int32_t Agent::req_shutdown(string &, string &output, Agent* agent)
        {
            agent->cinfo->agent[0].stateflag = static_cast <uint16_t>(Agent::State::SHUTDOWN);
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
        int32_t Agent::req_status(string &, string &output, Agent* agent)
        {
            string jstring;

            if (json_of_agent(jstring, agent->cinfo) != NULL)
            {
//                strncpy(output, jstring.c_str(),agent->cinfo->agent[0].beat.bsz);
//                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
                output = jstring;
                if (output.length() > agent->cinfo->agent[0].beat.bsz)
                {
                    output[agent->cinfo->agent[0].beat.bsz-1] = 0;
                }
                return 0;
            }
            else
            {
//                strcpy(output,"error");
//                output[5] = 0;
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
        int32_t Agent::req_debug_level(string &request, string &output, Agent* agent)
        {
//            if (strcmp(request, "debug_level"))
//            {
//                sscanf(request.c_str(), "debug_level %hu", &agent->debug_level);
//            }
//            sprintf(output, "%d", agent->debug_level);
            if (request != "debug_level")
            {
                sscanf(request.c_str(), "debug_level %hu", &agent->debug_level);
            }
            output = std::to_string(agent->debug_level);
            return 0;
        }

        //! Built-in Get Internal Value request
        /*! Returns the current value of the requested Name Space values. Names are expressed as a JSON object.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
//        int32_t Agent::req_getvalue(string &request, char* output, Agent* agent)
        int32_t Agent::req_getvalue(string &request, string &output, Agent* agent)
        {
            string jstring;

            if (json_of_list(jstring, request, agent->cinfo) != NULL)
            {
//                strncpy(output, jstring.c_str(), agent->cinfo->agent[0].beat.bsz);
//                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
                output = jstring;
                if (output.length() > agent->cinfo->agent[0].beat.bsz)
                {
                    output[agent->cinfo->agent[0].beat.bsz-1] = 0;
                }
                return 0;
            }
            else
                return (JSON_ERROR_EOS);
        }

        //! Built-in Set Internal Value request
        /*! Sets the current value of the requested Name Space values. Names and values are expressed as a JSON object.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
//        int32_t Agent::req_setvalue(string &request, char* output, Agent* agent)
        int32_t Agent::req_setvalue(string &request, string &output, Agent* agent)
        {
            int32_t iretn;
            iretn = json_parse(request, agent->cinfo);

//            sprintf(output,"%d",iretn);
            output = std::to_string(iretn);

            return(iretn);
        }

        //! Built-in List Name Space Names request
        /*! Returns a list of all names in the JSON Name Space.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
//        int32_t Agent::req_listnames(char *, char* output, Agent* agent)
        int32_t Agent::req_listnames(string &, string &output, Agent* agent)
        {
//            string result = json_list_of_all(agent->cinfo);
//            strncpy(output, result.c_str(), agent->cinfo->agent[0].beat.bsz);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = json_list_of_all(agent->cinfo);
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            }
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
        int32_t Agent::req_nodejson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.node.c_str(), agent->cinfo->json.node.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.node.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.node.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            }
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
        int32_t Agent::req_statejson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.state.c_str(), agent->cinfo->json.state.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.state.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.state.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            }
            return 0;
        }

        //! Built-in Return UTC Start Time JSON request
        /*! Returns a JSON string representing the UTC Start Time.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
//        int32_t Agent::req_utcstartjson(char *, char* output, Agent* agent)
        int32_t Agent::req_utcstartjson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.utcstart.c_str(), agent->cinfo->json.utcstart.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.utcstart.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.utcstart.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            }
            return 0;
        }

        //! Built-in Return Pieces JSON request
        /*! Returns a JSON string representing the Piece information.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to Cosmos::Agent to use.
 * \return 0, or negative error.
 */
//        int32_t Agent::req_piecesjson(char *, char* output, Agent* agent)
        int32_t Agent::req_piecesjson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.pieces.c_str(), agent->cinfo->json.pieces.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.pieces.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.pieces.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            }
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
        int32_t Agent::req_facesjson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.faces.c_str(), agent->cinfo->json.faces.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.faces.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.faces.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            }
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
        int32_t Agent::req_vertexsjson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.vertexs.c_str(), agent->cinfo->json.vertexs.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.vertexs.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.vertexs.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            }
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
        int32_t Agent::req_devgenjson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.devgen.c_str(), agent->cinfo->json.devgen.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.devgen.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.devgen.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            }
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
        int32_t Agent::req_devspecjson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.devspec.c_str(), agent->cinfo->json.devspec.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.devspec.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.devspec.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            }
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
        int32_t Agent::req_portsjson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.ports.c_str(), agent->cinfo->json.ports.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.ports.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.ports.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
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
        int32_t Agent::req_targetsjson(string &, string &output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.targets.c_str(), agent->cinfo->json.targets.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.targets.size():agent->cinfo->agent[0].beat.bsz-1);
//            output[agent->cinfo->agent[0].beat.bsz-1] = 0;
            output = agent->cinfo->json.targets.c_str();
            if (output.length() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
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
        int32_t Agent::req_aliasesjson(string &, string & output, Agent* agent)
        {
//            strncpy(output, agent->cinfo->json.aliases.c_str(), agent->cinfo->json.aliases.size()<agent->cinfo->agent[0].beat.bsz-1?agent->cinfo->json.aliases.size():agent->cinfo->agent[0].beat.bsz-1);
            output = agent->cinfo->json.aliases;
            if (output.size() > agent->cinfo->agent[0].beat.bsz)
            {
                output[agent->cinfo->agent[0].beat.bsz-1] = 0;
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
        int32_t Agent::req_heartbeat(string &, string &output, Agent* agent)
        {
//            output[0] = 0;
            output.clear();
            int32_t iretn = 0;
            iretn = agent->post_beat();
            return iretn;
        }

//        int32_t Agent::req_postsoh(char *, char* output, Agent* agent)
        int32_t Agent::req_postsoh(string &, string &output, Agent* agent)
        {
//            output[0] = 0;
            output.clear();
            int32_t iretn = 0;
            iretn = agent->post_soh();
            return iretn;
        }

//        int32_t Agent::req_utc(string &, string &response, Agent *agent)
        int32_t Agent::req_utc(string &, string &response, Agent *agent)
        {
//            response =  " %.15g %lf ", agent->agent_time_producer(), utc2unixseconds(agent->agent_time_producer()));
            response = to_mjd(agent->agent_time_producer()) + ' ' + std::to_string(utc2unixseconds(agent->agent_time_producer()));
            return 0;
        }

//        int32_t Agent::req_soh(string &, string &response, Agent *agent)
        int32_t Agent::req_soh(string &, string &response, Agent *agent)
        {
            string rjstring;
//            response = (json_of_table(rjstring, agent->sohtable, agent->cinfo));
            response = json_of_table(rjstring, agent->sohtable, agent->cinfo);

            return 0;
        }

//        int32_t Agent::req_fullsoh(string &, string &response, Agent *agent)
        int32_t Agent::req_fullsoh(string &, string &response, Agent *agent)
        {
            string rjstring;
//            response = (json_of_table(rjstring, agent->fullsohtable, agent->cinfo));
            response = json_of_table(rjstring, agent->fullsohtable, agent->cinfo);

            return 0;
        }

//        int32_t Agent::req_jsondump(char *, char*, Agent *agent)
        int32_t Agent::req_jsondump(string &, string &, Agent *agent)
        {
            json_dump_node(agent->cinfo);
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
            if (cinfo->agent[0].pub[0].cport)
                return 0;

            switch (type)
            {
            case NetworkType::MULTICAST:
            case NetworkType::UDP:
            case NetworkType::BROADCAST:
                {
                    for (uint32_t i=0; i<AGENTMAXIF; i++)
                    {
                        cinfo->agent[0].pub[i].cudp = -1;
                    }

                    if ((cinfo->agent[0].pub[0].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                    {
                        return (AGENT_ERROR_SOCKET);
                    }

#ifdef COSMOS_WIN_OS
                    u_long nonblocking = 1;;
                    if (ioctlsocket(cinfo->agent[0].pub[0].cudp, FIONBIO, &nonblocking) != 0)
                    {
                        iretn = -WSAGetLastError();
                    }
#else
                    if (fcntl(cinfo->agent[0].pub[0].cudp, F_SETFL,O_NONBLOCK) < 0)
                    {
                        iretn = -errno;
                    }
#endif
                    if (iretn < 0)
                    {
                        CLOSE_SOCKET(cinfo->agent[0].pub[0].cudp);
                        cinfo->agent[0].pub[0].cudp = iretn;
                        return iretn;
                    }

                    // Use above socket to find available interfaces and establish
                    // publication on each.
                    cinfo->agent[0].ifcnt = 0;

#if defined(COSMOS_WIN_OS)
                    struct sockaddr_storage ss;
                    int sslen;
                    INTERFACE_INFO ilist[20];
                    unsigned long nbytes;
                    uint32_t nif;
                    if (WSAIoctl(cinfo->agent[0].pub[0].cudp, SIO_GET_INTERFACE_LIST, 0, 0, &ilist,sizeof(ilist), &nbytes, 0, 0) == SOCKET_ERROR)
                    {
                        CLOSE_SOCKET(cinfo->agent[0].pub[0].cudp);
                        return (AGENT_ERROR_DISCOVERY);
                    }

                    nif = nbytes / sizeof(INTERFACE_INFO);
                    for (uint32_t i=0; i<nif; i++)
                    {
                        inet_ntop(ilist[i].iiAddress.AddressIn.sin_family,&ilist[i].iiAddress.AddressIn.sin_addr,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address,sizeof(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address));
                        //            strcpy(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address,inet_ntoa(((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr));
                        if (!strcmp(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address,"127.0.0.1"))
                        {
                            if (cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp >= 0)
                            {
                                CLOSE_SOCKET(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp);
                                cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp = -1;
                            }
                            continue;
                        }

                        // No need to open first socket again
                        if (cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp < 0)
                        {
                            if ((cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                            {
                                continue;
                            }
                            u_long nonblocking = 1;
                            if (ioctlsocket(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp, FIONBIO, &nonblocking) != 0)
                            {
                                continue;
                            }
                        }

                        memset(&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr,0,sizeof(struct sockaddr_in));
                        cinfo->agent[0].pub[i].caddr.sin_family = AF_INET;
                        cinfo->agent[0].pub[i].baddr.sin_family = AF_INET;
                        if (type == NetworkType::MULTICAST)
                        {
                            sslen = sizeof(ss);
                            WSAStringToAddressA((char *)AGENTMCAST,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                        }
                        else
                        {
                            if ((iretn = setsockopt(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                            {
                                CLOSE_SOCKET(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp);
                                continue;
                            }

                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;

                            uint32_t ip, net, bcast;
                            ip = ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.S_un.S_addr;
                            net = ((struct sockaddr_in*)&(ilist[i].iiNetmask))->sin_addr.S_un.S_addr;
                            bcast = ip | (~net);
                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr.S_un.S_addr = bcast;
                        }
                        cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_port = htons(port);
                        cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_port = htons(port);
                        cinfo->agent[0].pub[cinfo->agent[0].ifcnt].type = type;
                        cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cport = port;
                        cinfo->agent[0].ifcnt++;
                    }
#elif defined(COSMOS_MAC_OS)
                    struct ifaddrs *if_addrs = NULL;
                    struct ifaddrs *if_addr = NULL;
                    if (0 == getifaddrs(&if_addrs))
                    {
                        for (if_addr = if_addrs; if_addr != NULL; if_addr = if_addr->ifa_next)
                        {

                            if (if_addr->ifa_addr->sa_family != AF_INET)
                            {
                                continue;
                            }
                            inet_ntop(if_addr->ifa_addr->sa_family,&((struct sockaddr_in*)if_addr->ifa_addr)->sin_addr,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address,sizeof(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address));
                            memcpy((char *)&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr, (char *)if_addr->ifa_addr, sizeof(if_addr->ifa_addr));

                            if ((if_addr->ifa_flags & IFF_POINTOPOINT) || (if_addr->ifa_flags & IFF_UP) == 0 || (if_addr->ifa_flags & IFF_LOOPBACK) || (if_addr->ifa_flags & (IFF_BROADCAST)) == 0)
                            {
                                continue;
                            }

                            // No need to open first socket again
                            if (cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp < 0)
                            {
                                if ((cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                                {
                                    continue;
                                }

                                if (fcntl(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp, F_SETFL,O_NONBLOCK) < 0)
                                {
                                    iretn = -errno;
                                    CLOSE_SOCKET(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp);
                                    cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp = iretn;
                                    continue;
                                }
                            }

                            if (type == NetworkType::MULTICAST)
                            {
                                inet_pton(AF_INET,AGENTMCAST,&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_addr);
                                inet_pton(AF_INET,AGENTMCAST,&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr);
                            }
                            else
                            {
                                if ((iretn = setsockopt(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                                {
                                    CLOSE_SOCKET(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp);
                                    continue;
                                }

                                //                    if (ioctl(cinfo->agent[0].pub[0].cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                                //                    {
                                //                        continue;
                                //                    }
                                //                    cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr = cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr;
                                memcpy((char *)&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr, (char *)if_addr->ifa_netmask, sizeof(if_addr->ifa_netmask));

                                uint32_t ip, net, bcast;
                                ip = cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_addr.s_addr;
                                net = cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr.s_addr;
                                bcast = ip | (~net);
                                cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr.s_addr = bcast;
                                inet_ntop(if_addr->ifa_netmask->sa_family,&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddress,sizeof(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddress));
                            }
                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_port = htons(port);
                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_port = htons(port);
                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].type = type;
                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cport = port;
                            cinfo->agent[0].ifcnt++;
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
                    if (ioctl(cinfo->agent[0].pub[0].cudp,SIOCGIFCONF,&confa) < 0)
                    {
                        CLOSE_SOCKET(cinfo->agent[0].pub[0].cudp);
                        return (AGENT_ERROR_DISCOVERY);
                    }
                    // Use result to discover interfaces.
                    ifra = confa.ifc_req;
                    for (int32_t n=confa.ifc_len/sizeof(struct ifreq); --n >= 0; ifra++)
                    {
                        if (ifra->ifr_addr.sa_family != AF_INET)
                        {
                            continue;
                        }
                        inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address,sizeof(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address));
                        memcpy((char *)&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr, (char *)&ifra->ifr_addr, sizeof(ifra->ifr_addr));

                        if (ioctl(cinfo->agent[0].pub[0].cudp,SIOCGIFFLAGS, (char *)ifra) < 0) continue;

                        if ((ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK))
//                            if ((ifra->ifr_flags & IFF_POINTOPOINT) || (ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK) || (ifra->ifr_flags & (IFF_BROADCAST)) == 0)
                        {
                            continue;
                        }

                        // Open socket again if we had to close it
                        if (cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp < 0)
                        {
                            if ((cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                            {
                                continue;
                            }

                            if (fcntl(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp, F_SETFL,O_NONBLOCK) < 0)
                            {
                                iretn = -errno;
                                CLOSE_SOCKET(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp);
                                cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp = iretn;
                                continue;
                            }
                        }

                        if (type == NetworkType::MULTICAST)
                        {
                            inet_pton(AF_INET,AGENTMCAST,&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_addr);
                            inet_pton(AF_INET,AGENTMCAST,&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr);
                        }
                        else
                        {
//                            int val = IP_PMTUDISC_DO;
//                            if ((iretn = setsockopt(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp, IPPROTO_IP, IP_MTU_DISCOVER, &val, sizeof(val))) < 0)
//                            {
//                                CLOSE_SOCKET(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp);
//                                continue;
//                            }

                            if ((ifra->ifr_flags & IFF_POINTOPOINT))
                            {
                                if (ioctl(cinfo->agent[0].pub[0].cudp,SIOCGIFDSTADDR,(char *)ifra) < 0)
                                {
                                    continue;
                                }
                                cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr = cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr;
                                inet_ntop(ifra->ifr_dstaddr.sa_family,&((struct sockaddr_in*)&ifra->ifr_dstaddr)->sin_addr,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddress,sizeof(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddress));
                                inet_pton(AF_INET,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddress,&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr);

                            }
                            else
                            {
                                if ((iretn = setsockopt(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on))) < 0)
                                {
                                    CLOSE_SOCKET(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp);
                                    continue;
                                }

                                if (ioctl(cinfo->agent[0].pub[0].cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                                {
                                    continue;
                                }
                                cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr = cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr;
                                inet_ntop(ifra->ifr_broadaddr.sa_family,&((struct sockaddr_in*)&ifra->ifr_broadaddr)->sin_addr,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddress,sizeof(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddress));
                                inet_pton(AF_INET,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddress,&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_addr);
                            }

                            if (ioctl(cinfo->agent[0].pub[0].cudp,SIOCGIFADDR,(char *)ifra) < 0)
                            {
                                continue;
                            }
                            inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address,sizeof(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address));
                            inet_pton(AF_INET,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address,&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_addr);
                        }

                        iretn = sendto(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp,       // socket
                                (const char *)nullptr,                         // buffer to send
                                0,                      // size of buffer
                                0,                                          // flags
                                (struct sockaddr *)&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr, // socket address
                                sizeof(struct sockaddr_in)                  // size of address to socket pointer
                                );
                        // Find assigned port, place in cport, and set caddr to requested port
                        socklen_t namelen = sizeof(struct sockaddr_in);
                        if ((iretn = getsockname(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp, (sockaddr*)&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr, &namelen)) == -1)
                        {
                            CLOSE_SOCKET(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp);
                            cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cudp = -errno;
                            return (-errno);
                        }
                        cinfo->agent[0].pub[cinfo->agent[0].ifcnt].cport = ntohs(cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_port);
                        cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_port = htons(port);
                        inet_pton(AF_INET,cinfo->agent[0].pub[cinfo->agent[0].ifcnt].address,&cinfo->agent[0].pub[cinfo->agent[0].ifcnt].caddr.sin_addr);
                        cinfo->agent[0].pub[cinfo->agent[0].ifcnt].baddr.sin_port = htons(port);
                        cinfo->agent[0].pub[cinfo->agent[0].ifcnt].type = type;
                        cinfo->agent[0].ifcnt++;
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
            int32_t iretn;
            int on = 1;
            int32_t cudp;

            switch (ntype)
            {
            case NetworkType::MULTICAST:
            case NetworkType::UDP:
                {
                    if ((cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                    {
                        return (iface);
                    }

                    // Use above socket to find available interfaces and establish
                    // publication on each.
#ifdef COSMOS_WIN_OS
                    if (WSAIoctl(cudp, SIO_GET_INTERFACE_LIST, 0, 0, &ilist,sizeof(ilist), &nbytes, 0, 0) == SOCKET_ERROR)
                    {
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
                        if (!strcmp(tiface.address,"127.0.0.1"))
                        {
                            continue;
                        }

                        pCurrAddresses = pAddresses;
                        while (pAddresses)
                        {
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
                        }
                        else
                        {
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
                        if (ifra->ifr_addr.sa_family != AF_INET)
                        {
                            continue;
                        }
                        inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,tiface.address,sizeof(tiface.address));

                        if (ioctl(cudp,SIOCGIFFLAGS, (char *)ifra) < 0) continue;

                        if ((ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK) || (ifra->ifr_flags & (IFF_BROADCAST)) == 0)
                        {
                            continue;
                        }

                        // Open socket again if we had to close it
                        if (cudp < 0)
                        {
                            if ((cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                            {
                                continue;
                            }
                        }

                        if (ntype == NetworkType::MULTICAST)
                        {
                            inet_pton(AF_INET,AGENTMCAST,&tiface.caddr.sin_addr);
                            //                        strcpy(tiface.baddress, AGENTMCAST);
                            inet_pton(AF_INET,AGENTMCAST,&tiface.baddr.sin_addr);
                        }
                        else
                        {
                            if ((iretn = setsockopt(cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                            {
                                CLOSE_SOCKET(cudp);
                                continue;
                            }

                            //                        strncpy(tiface.name, ifra->ifr_name, COSMOS_MAX_NAME);
                            if (ioctl(cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                            {
                                continue;
                            }
                            memcpy((char *)&tiface.baddr, (char *)&ifra->ifr_broadaddr, sizeof(ifra->ifr_broadaddr));
                            if (ioctl(cudp,SIOCGIFADDR,(char *)ifra) < 0) continue;
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
        int32_t Agent::post(messstruc mess)
        {
            int32_t iretn;

            if (mess.meta.type < Agent::AgentMessage::BINARY)
            {
                iretn = post(mess.meta.type, mess.adata);
            }
            else
            {
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
            int32_t iretn;
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

            cinfo->agent[0].beat.utc = currentmjd();
            post[0] = (uint8_t)type;
            // this will broadcast messages to all external interfaces (ifcnt = interface count)
            for (size_t i=0; i<cinfo->agent[0].ifcnt; i++)
            {
                sprintf((char *)&post[3],"{\"agent_utc\":%.15g,\"agent_node\":\"%s\",\"agent_proc\":\"%s\",\"agent_addr\":\"%s\",\"agent_port\":%u,\"agent_bprd\":%f,\"agent_bsz\":%u,\"agent_cpu\":%f,\"agent_memory\":%f,\"agent_jitter\":%f,\"node_utcoffset\":%.15g}",
                        cinfo->agent[0].beat.utc,
                        cinfo->agent[0].beat.node,
                        cinfo->agent[0].beat.proc,
                        cinfo->agent[0].pub[i].address,
                        cinfo->agent[0].beat.port,
                        cinfo->agent[0].beat.bprd,
                        cinfo->agent[0].beat.bsz,
                        cinfo->agent[0].beat.cpu,
                        cinfo->agent[0].beat.memory,
                        cinfo->agent[0].beat.jitter,
                        cinfo->node.utcoffset);
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
                iretn = sendto(cinfo->agent[0].pub[i].cudp,       // socket
                        (const char *)post,                         // buffer to send
                        nbytes+message.size(),                      // size of buffer
                        0,                                          // flags
                        (struct sockaddr *)&cinfo->agent[0].pub[i].baddr, // socket address
                        sizeof(struct sockaddr_in)                  // size of address to socket pointer
                        );
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
                //            printf("Post: Type: %d Port: %d %d\n", type, cinfo->agent[0].pub[i].cport, htons(cinfo->agent[0].pub[i].caddr.sin_port));
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
            cinfo->agent[0].beat.utc = currentmjd(0.);
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

        int32_t Agent::post_soh()
        {
            int32_t iretn = 0;
            cinfo->agent[0].beat.utc = currentmjd(0.);
            iretn = post(AgentMessage::SOH, json_of_table(hbjstring, sohtable, (cosmosstruc *)cinfo));
            return iretn;
        }

        //! Close COSMOS output channel
        /*! Close previously opened publication channels and recover any allocated resources.
    \return 0, otherwise negative error.
    */
        int32_t Agent::unpublish()
        {
            if (cinfo == nullptr)
            {
                return 0;
            }
            for (size_t i=0; i<cinfo->agent[0].ifcnt; ++i)
            {
                CLOSE_SOCKET(cinfo->agent[0].pub[i].cudp);
            }
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
            // for some reason cinfo->agent[0].sub.cport was ill initialized
#ifndef COSMOS_WIN_BUILD_MSVC
            if (cinfo->agent[0].sub.cport)
                return 0;
#endif
            if ((iretn=socket_open(&cinfo->agent[0].sub,type,address,port,SOCKET_LISTEN,SOCKET_BLOCKING, usectimeo)) < 0)
            {
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
        int32_t Agent::subscribe(NetworkType type, const char *address, uint16_t port)
        {
            int32_t iretn = 0;

            if ((iretn=Agent::subscribe(type, address, port, 100)) < 0)
            {
                return iretn;
            }

            return 0;
        }
        //! Close COSMOS subscription channel
        /*! Close channel previously opened for polling for messages and recover resources.
    \return 0, otherwise negative error.
*/
        int32_t Agent::unsubscribe()
        {
            if (cinfo != nullptr)
            {
                CLOSE_SOCKET(cinfo->agent[0].sub.cudp);
            }
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

            if (cinfo == nullptr)
            {
                return AGENT_ERROR_NULL;
            }

            if (!cinfo->agent[0].sub.cport)
                return (AGENT_ERROR_CHANNEL);

            ElapsedTime ep;
            ep.start();
            do
            {
                nbytes = 0;
                switch (cinfo->agent[0].sub.type)
                {
                case NetworkType::MULTICAST:
                case NetworkType::UDP:

                    nbytes = recvfrom(cinfo->agent[0].sub.cudp, (char *)input,AGENTMAXBUFFER, 0, (struct sockaddr *)&cinfo->agent[0].sub.caddr, (socklen_t *)&cinfo->agent[0].sub.addrlen);

                    // Return if error
                    if (nbytes < 0)
                    {
                        return nbytes;
                    }

                    //                printf("Poll: %f %f Type: %d Port %d %d\n", 86400.*(currentmjd()-58496), ep.split(), input[0], cinfo->agent[0].sub.cport, htons(cinfo->agent[0].sub.caddr.sin_port));

                    // Return if port and address are our own
                    for (uint16_t i=0; i<cinfo->agent[0].ifcnt; ++i)
                    {
                        if (cinfo->agent[0].sub.caddr.sin_port == ntohs(cinfo->agent[0].pub[i].cport) &&
                            cinfo->agent[0].sub.caddr.sin_addr.s_addr == cinfo->agent[0].pub[i].caddr.sin_addr.s_addr)
                        {
                            return 0;
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
                            sscanf((const char *)mess.jdata.data(), "{\"agent_utc\":%lg,\"agent_node\":\"%40[^\"]\",\"agent_proc\":\"%40[^\"]\",\"agent_addr\":\"%17[^\"]\",\"agent_port\":%hu,\"agent_bprd\":%lf,\"agent_bsz\":%u,\"agent_cpu\":%f,\"agent_memory\":%f,\"agent_jitter\":%lf}",
                                   &mess.meta.beat.utc,
                                   mess.meta.beat.node,
                                   mess.meta.beat.proc,
                                   mess.meta.beat.addr,
                                   &mess.meta.beat.port,
                                   &mess.meta.beat.bprd,
                                   &mess.meta.beat.bsz,
                                   &mess.meta.beat.cpu,
                                   &mess.meta.beat.memory,
                                   &mess.meta.beat.jitter);
                        }
                        else if (mess.jdata.find("agent_bprd") == string::npos)
                        {
                            sscanf((const char *)mess.jdata.data(), "{\"agent_utc\":%lg}{\"agent_node\":\"%40[^\"]\"}{\"agent_proc\":\"%40[^\"]\"}{\"agent_addr\":\"%17[^\"]\"}{\"agent_port\":%hu}{\"agent_bsz\":%u}{\"agent_cpu\":%f}{\"agent_memory\":%f}{\"agent_jitter\":%lf}",
                                   &mess.meta.beat.utc,
                                   mess.meta.beat.node,
                                   mess.meta.beat.proc,
                                   mess.meta.beat.addr,
                                   &mess.meta.beat.port,
                                   &mess.meta.beat.bsz,
                                   &mess.meta.beat.cpu,
                                   &mess.meta.beat.memory,
                                   &mess.meta.beat.jitter);
                        }
                        else
                        {
                            sscanf((const char *)mess.jdata.data(), "{\"agent_utc\":%lg}{\"agent_node\":\"%40[^\"]\"}{\"agent_proc\":\"%40[^\"]\"}{\"agent_addr\":\"%17[^\"]\"}{\"agent_port\":%hu}{\"agent_bprd\":%lf}{\"agent_bsz\":%u}{\"agent_cpu\":%f}{\"agent_memory\":%f}{\"agent_jitter\":%lf}",
                                   &mess.meta.beat.utc,
                                   mess.meta.beat.node,
                                   mess.meta.beat.proc,
                                   mess.meta.beat.addr,
                                   &mess.meta.beat.port,
                                   &mess.meta.beat.bprd,
                                   &mess.meta.beat.bsz,
                                   &mess.meta.beat.cpu,
                                   &mess.meta.beat.memory,
                                   &mess.meta.beat.jitter);
                        }

                        return ((int)mess.meta.type);
                    }
                }
                if (ep.split() >= waitsec)
                {
                    nbytes = 0;
                }
                else
                {
                    COSMOS_SLEEP(.1);
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
            if (waitsec < 0.f)
            {
                waitsec = 0.;
            }

            if (cinfo == nullptr)
            {
                return AGENT_ERROR_NULL;
            }

            if (where == Where::HEAD)
            {
                message_queue.clear();
            }
            ElapsedTime ep;
            ep.start();
            do
            {
                while (message_queue.size())
                {
                    message = message_queue.front();
                    message_queue.pop_front();
                    if (type == Agent::AgentMessage::ALL || type == static_cast<Agent::AgentMessage>(message.meta.type))
                    {
                        if (proc.empty() || proc == message.meta.beat.proc)
                        {
                            if (node.empty() || node == message.meta.beat.node)
                            {
                                return (static_cast<int32_t>(message.meta.type));
                            }
                        }
                    }
                }

                if (ep.split() < waitsec)
                {
                    if (waitsec - ep.split() > .1)
                    {
                        COSMOS_SLEEP(.1);
                    }
                    else
                    {
                        COSMOS_SLEEP(.05);
                    }
                }
            } while (ep.split() < waitsec);

            return 0;
        }

        //! Parse next message from ring
        int32_t Agent::parsering(AgentMessage type, float waitsec, Where where, string proc, string node)
        {
            int32_t iretn;
            messstruc message;

            if (where == Where::HEAD)
            {
                message_queue.clear();
            }
            post(Agent::AgentMessage::REQUEST, "heartbeat");
            ElapsedTime et;
            do
            {
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
            if (message_head >= newsize)
            {
                message_head = 0;
            }

            if (message_tail >= newsize)
            {
                message_tail = newsize - 1;
            }

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
        //        int32_t iretn;
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
        //        int32_t iretn;
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
        //    locstruc Agent::poll_location(float waitsec)
        //    {
        //        int32_t iretn;
        //        locstruc loc;
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
        //        int32_t iretn;
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
        //                info.powgen = cinfo->sdata.node.powgen;
        //                info.powuse = cinfo->sdata.node.powuse;
        //                info.battlev = cinfo->sdata.node.battlev;
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
        //        int32_t iretn;
        //        imustruc imu;
        //        messstruc mess;

        //		iretn = Agent::poll(mess, Agent::AgentMessage::IMU, waitsec);

        //		if (iretn == Agent::AgentMessage::IMU)
        //        {
        //            iretn = json_parse(mess.adata, cinfo->sdata);
        //            if (iretn >= 0)
        //            {
        //                imu = *cinfo->sdata.device[agent->cinfo->devspec.imu[0]].imu.;
        //            }
        //        }

        //        return (imu);
        //    }

        string Agent::getNode()
        {
            return nodeName;
        }

        string Agent::getAgent()
        {
            return agentName;
        }


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

            if (!node_found)
            {
                iretn = GENERAL_ERROR_UNDEFINED;
            }

            return iretn;
        }

        FILE *Agent::get_debug_fd(double mjd)
        {
            static double oldmjd=0.;
            if (debug_level == 0)
            {
                debug_fd = nullptr;
                debug_pathName.clear();
            }
            else if (debug_level == 1)
            {
                if (debug_fd != stdout)
                {
                    if (debug_fd != nullptr)
                    {
                        fclose(debug_fd);
                    }
                        debug_fd = stdout;
                    debug_pathName.clear();
                }
            }
            else
            {
                if (mjd == 0.)
                {
                    mjd = currentmjd();
                    oldmjd = mjd;
                }
                mjd = mjd - fmod(mjd, 1./24.);
                string pathName = data_type_path(nodeName, "temp", agentName, mjd, agentName, "debug");

                if (debug_fd != nullptr)
                {
                    if (pathName != debug_pathName)
                    {
                        FILE *fd = fopen(pathName.c_str(), "a");
                        if (fd != nullptr)
                        {
                            if (debug_fd != stdout)
                            {
                                fclose(debug_fd);
                                string final_filepath = data_type_path(nodeName, "outgoing", agentName, oldmjd, agentName, "debug");
                                rename(debug_pathName.c_str(), final_filepath.c_str());
                            }
                            debug_fd = fd;
                            debug_pathName = pathName;
                        }
                        oldmjd = mjd;
                    }
                }
                else
                {
                    FILE *fd = fopen(pathName.c_str(), "a");
                    if (fd != nullptr)
                    {
                        debug_fd = fd;
                        debug_pathName = pathName;
                    }
                }
            }

            return debug_fd;
        }

        int32_t Agent::close_debug_fd()
        {
            int32_t iretn;
            if (debug_fd != nullptr && debug_fd != stdout)
            {
                iretn = fclose(debug_fd);
                if (iretn != 0)
                {
                    return -errno;
                }
                debug_fd = nullptr;
            }

            return 0;
        }

        // Set our producer for all functions associated with time authority (i.e. the mjd request).
        int32_t Agent::set_agent_time_producer(double (*source)()) {
            this->agent_time_producer = source;
            return 0;
        }

        // A Cristian's algorithm approach to time synchronization, with our remote node as the time server.
        // This is meant to be run on a time sink agent (a requester).
        int32_t Agent::get_agent_time(double &agent_time, double &epsilon, double &delta, string agent, string node, double wait_sec)
        {
            static beatstruc agent_beat;
            string agent_response;
            double mjd_0, mjd_1;
            int32_t iretn;

            if (!agent_beat.exists)
            {
                agent_beat = find_agent(node, agent, wait_sec);
            }
            else
            {
                if (node != agent_beat.node || (agent != "any" && agent != agent_beat.proc))
                {
                    agent_beat = find_agent(node, agent, wait_sec);
                }
            }

            // Do not proceed if we cannot find the agent.
            if (!agent_beat.exists) return AGENT_ERROR_DISCOVERY;

            mjd_0 = currentmjd();
            iretn = send_request(agent_beat, "utc", agent_response, 0);
            if (iretn >= 0)
            {
                mjd_1 = currentmjd();

                epsilon = (mjd_1 - mjd_0) / 2.0;  // RTT / 2.0
                agent_time = stod(agent_response.substr(0, agent_response.find("["))) + epsilon;
                delta = agent_time - mjd_1; // We do not have a lower bound on the time to transmit a message one way.

            return 0;
            }
            else
            {
                agent_time = 0.;
                epsilon = 0.;
                return GENERAL_ERROR_TIMEOUT;
            }
        }


    } // end of namespace Support
} // end namespace Cosmos

//! @}
