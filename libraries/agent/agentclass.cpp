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

namespace Cosmos {

    //! \ingroup agentlib
    //! \ingroup agentlib
    //! \defgroup agentlib_functions Agent Server and Client functions
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
    Agent::Agent(const string &nname, const string &aname, double bprd, uint32_t bsize, bool mflag, int32_t portnum, NetworkType ntype, int32_t dlevel)
    {
        int32_t iretn;

        double timeStart = currentmjd();
        debug_level = dlevel;

        if (debug_level)
        {
            std::cout << "------------------------------------------------------" << std::endl;
            std::cout << "COSMOS AGENT '" << aname << "' on node '" << nname << "'" << std::endl;
            std::cout << "Version " << version << " built on " <<  __DATE__ << " " << __TIME__ << std::endl;
            std::cout << "Agent started at " << mjdToGregorian(timeStart) << std::endl;
            std::cout << "Debug level " << debug_level << endl;
            std::cout << "------------------------------------------------------" << std::endl;
        }


        // Initialize COSMOS data space
        if ((cinfo = json_create()) == nullptr)
        {
            return;
        }
        cinfo->pdata.agent[0].stateflag = (uint16_t)Agent::State::INIT;

        // Establish subscribe channel
        iretn = Agent::subscribe(ntype, (char *)AGENTMCAST, AGENTSENDPORT, 1000);
        if (iretn)
        {
            json_destroy(cinfo);
            cinfo = nullptr;
            return;
        }

        // Set up node if there is one.
        if (nname.length()>COSMOS_MAX_NAME || (!nname.empty() && (iretn=json_setup_node(nname, cinfo)) != 0))
        {
            error_value = iretn;
            Agent::shutdown();
            return;
        }

        if (nname.empty())
        {
            strcpy(cinfo->pdata.node.name,"null");
        }

        cinfo->pdata.agent[0].client = 1;
        cinfo->pdata.node.utc = 0.;
        strncpy(cinfo->pdata.agent[0].beat.node, cinfo->pdata.node.name ,COSMOS_MAX_NAME);
        cinfo->pdata.agent[0].beat.ntype = ntype;

        // Make copies
        if ((iretn=json_clone(cinfo->pdata, cinfo->sdata)) != 0)
        {
            Agent::shutdown();
            return;
        }

        // Start message listening thread
        mthread = thread([=] { message_loop(); });

        // Return if all we are doing is setting up client.
        if (aname.length() == 0)
        {
            return;
        }

        if (strlen(cinfo->pdata.node.name)>COSMOS_MAX_NAME || aname.length()>COSMOS_MAX_NAME)
        {
            error_value = JSON_ERROR_NAME_LENGTH;
            Agent::shutdown();
            return;
        }


        // If not Multi, check if this Agent is already running
        char tname[COSMOS_MAX_NAME+1];
        if (!mflag)
        {
            if (Agent::get_server(cinfo->pdata.node.name, aname, timeoutSec, (beatstruc *)NULL))
            {
                error_value = AGENT_ERROR_SERVER_RUNNING;
                Agent::shutdown();
                return;
            }
            strcpy(tname,aname.c_str());
        }
        else // then there is an agent running with the given name, so let's make the name unique
        {
            if (strlen(cinfo->pdata.node.name)>COSMOS_MAX_NAME-4 || aname.size()>COSMOS_MAX_NAME-4)
            {
                error_value = JSON_ERROR_NAME_LENGTH;
                Agent::shutdown();
                return;
            }

            uint32_t i=0;
            do
            {
                sprintf(tname,"%s_%03d",aname.c_str(),i);
                if (!Agent::get_server(cinfo->pdata.node.name, tname, timeoutSec, (beatstruc *)NULL))
                {
                    break;
                }
            } while (++i<100);
        }

        // Initialize important server variables

        strncpy(cinfo->pdata.agent[0].beat.node, cinfo->pdata.node.name, COSMOS_MAX_NAME);
        nodeName = cinfo->pdata.agent[0].beat.node;
        strncpy(cinfo->pdata.agent[0].beat.proc, tname, COSMOS_MAX_NAME);
        agentName = cinfo->pdata.agent[0].beat.proc;
        //	cinfo->pdata.agent[0].beat.ntype = ntype;
        if (bprd >= AGENT_HEARTBEAT_PERIOD_MIN)
            cinfo->pdata.agent[0].beat.bprd = bprd;
        else
            cinfo->pdata.agent[0].beat.bprd = AGENT_HEARTBEAT_PERIOD_MIN;
        cinfo->pdata.agent[0].stateflag = (uint16_t)Agent::State::INIT;
        cinfo->pdata.agent[0].beat.port = (uint16_t)portnum;
        cinfo->pdata.agent[0].beat.bsz = (bsize<=AGENTMAXBUFFER-4?bsize:AGENTMAXBUFFER-4);

#ifdef COSMOS_WIN_BUILD_MSVC
        cinfo->pdata.agent[0].pid = _getpid();
#else
        cinfo->pdata.agent[0].pid = getpid();
#endif
        cinfo->pdata.agent[0].aprd = 1.;
        strncpy(cinfo->pdata.agent[0].beat.user, "cosmos", COSMOS_MAX_NAME);
        //	cinfo->pdata.agent[0].sohstring[0] = 0;

        // Establish publish channel
        iretn = Agent::publish(cinfo->pdata.agent[0].beat.ntype, AGENTSENDPORT);
        if (iretn)
        {
            error_value = iretn;
            Agent::shutdown();
            return;
        }

        // Start the heartbeat and request threads running
        //    iretn = start();
        hthread = thread([=] { heartbeat_loop(); });
        cthread = thread([=] { request_loop(); });
        if (!hthread.joinable() || !cthread.joinable())
        {
            // TODO: create error value
            //error_value = iretn;
            Agent::shutdown();
            return;
        }

        //! Set up initial requests
        Agent::add_request("help",req_help,"","list of available requests for this agent");
        Agent::add_request("shutdown",Agent::req_shutdown,"","request to shutdown this agent");
        Agent::add_request("idle",Agent::req_idle,"","request to transition this agent to idle state");
        Agent::add_request("init",Agent::req_init,"","request to transition this agent to init state");
        Agent::add_request("monitor",Agent::req_monitor,"","request to transition this agent to monitor state");
        Agent::add_request("run",Agent::req_run,"","request to transition this agent to run state");
        Agent::add_request("status",Agent::req_status,"","request the status of this agent");
        Agent::add_request("getvalue",Agent::req_getvalue,"{\"name1\",\"name2\",...}","get specified value(s) from agent");
        Agent::add_request("setvalue",Agent::req_setvalue,"{\"name1\":value},{\"name2\":value},...}","set specified value(s) in agent");
        Agent::add_request("listnames",Agent::req_listnames,"","list the Namespace of the agent");
        Agent::add_request("forward",Agent::req_forward,"nbytes packet","Broadcast JSON packet to the default SEND port on local network");
        Agent::add_request("echo",Agent::req_echo,"utc crc nbytes bytes","echo array of nbytes bytes, sent at time utc, with CRC crc.");
        Agent::add_request("nodejson",Agent::req_nodejson,"","return description JSON for Node");
        Agent::add_request("statejson",Agent::req_statejson,"","return description JSON for State vector");
        Agent::add_request("utcstartjson",Agent::req_utcstartjson,"","return description JSON for UTC Start time");
        Agent::add_request("piecesjson",Agent::req_piecesjson,"","return description JSON for Pieces");
        Agent::add_request("devgenjson",Agent::req_devgenjson,"","return description JSON for General Devices");
        Agent::add_request("devspecjson",Agent::req_devspecjson,"","return description JSON for Specific Devices");
        Agent::add_request("portsjson",Agent::req_portsjson,"","return description JSON for Ports");
        Agent::add_request("targetsjson",Agent::req_targetsjson,"","return description JSON for Targets");
        Agent::add_request("aliasesjson",Agent::req_aliasesjson,"","return description JSON for Aliases");

        cinfo->pdata.agent[0].server = 1;
        cinfo->pdata.agent[0].stateflag = (uint16_t)Agent::State::RUN;





    }

    //Agent::Agent(NetworkType ntype, const string &nname, const string &aname, double bprd, uint32_t bsize, bool mflag, int32_t portnum)
    //{
    //    Agent(nname, aname, bprd, bsize, mflag, portnum, ntype);
    //}

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
    int32_t Agent::add_request_internal(string token, Agent::internal_request_function function, string synopsis, string description)
    {
        if (Agent::reqs.size() > AGENTMAXREQUESTCOUNT)
            return (AGENT_ERROR_REQ_COUNT);

        request_entry tentry;
        if (token.size() > COSMOS_MAX_NAME)
        {
            token.resize(COSMOS_MAX_NAME);
        }
        tentry.token = token;
        tentry.ifunction = function;
        tentry.efunction = nullptr;
        tentry.synopsis = synopsis;
        tentry.description = description;
        Agent::reqs.push_back(tentry);
        return 0;
    }

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
        if (Agent::reqs.size() > AGENTMAXREQUESTCOUNT)
            return (AGENT_ERROR_REQ_COUNT);

        request_entry tentry;
        if (token.size() > COSMOS_MAX_NAME)
        {
            token.resize(COSMOS_MAX_NAME);
        }
        tentry.token = token;
        tentry.ifunction = nullptr;
        tentry.efunction = function;
        tentry.synopsis = synopsis;
        tentry.description = description;
        Agent::reqs.push_back(tentry);
        return 0;
    }

    //! Start Agent Request and Heartbeat loops
    /*!	Starts the request and heartbeat threads for an Agent server initialized with
 * ::Agent::setup_server. The Agent will open its request and heartbeat channels using the
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

    //! Shutdown agent gracefully
    /*! Waits for threads to stop running if we are a server, then releases everything.
 * \return 0 or negative error.
 */
    int32_t Agent::shutdown()
    {
        if (debug_level)
        {
            printf("Shutting down Agent. Last error: %s\n", cosmos_error_string(error_value).c_str());
            fflush(stdout);
        }

        if (cinfo != nullptr)
        {
            cinfo->pdata.agent[0].stateflag = static_cast <uint16_t>(Agent::State::SHUTDOWN);
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
    \return Value of internal state variable, as enumerated in ::AGENT_STATE.
*/
    uint16_t Agent::running()
    {
        return (cinfo->pdata.agent[0].stateflag);
    }

    //! Send a request over AGENT
    /*! Send a request string to the process at the provided address
    \param hbeat The agent ::beatstruc
    \param request the request and its arguments
    \param output any output returned by the request
    \param waitsec Maximum number of seconds to wait
    \return Either the number of bytes returned, or an error number.
*/

    int32_t Agent::last_error()
    {
        return (error_value);
    }

    int32_t Agent::send_request(beatstruc hbeat, string request, string &output, float waitsec)
    {
        static socket_channel sendchan;
        int32_t iretn;
        int32_t nbytes;

        vector <char> toutput;
        toutput.resize(AGENTMAXBUFFER+1);

        if (hbeat.utc == 0. || hbeat.addr == 0 || hbeat.port == 0)
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

            nbytes = recvfrom(sendchan.cudp, toutput.data(), AGENTMAXBUFFER, 0, (struct sockaddr *)NULL, (socklen_t *)NULL);

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
    \param request the request and its arguments
    \param output any output returned by the request
    \param waitsec Maximum number of seconds to wait
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
        messstruc message;

        //! 3. Loop for ::waitsec seconds, or until we discover desired heartbeat.

        ElapsedTime ep;
        ep.start();

        do
        {
            for (size_t i=0; i<agent_list.size(); ++i)
            {
                if (name == agent_list[i].proc && node == agent_list[i].node)
                {
                    if (rbeat != NULL)
                    {
                        *rbeat = agent_list[i];
                    }
                    return (1);
                }
            }
            COSMOS_SLEEP(.1);
            //        int32_t type = Agent::readring(message, AGENT_MESSAGE_BEAT, waitsec-ep.split());

            //        if (type == AGENT_MESSAGE_BEAT && name == message.meta.beat.proc && node == message.meta.beat.node)
            //        {
            //            if (rbeat != NULL)
            //            {
            //                *rbeat = message.meta.beat;
            //            }
            //            return (1);
            //        }

            //        cbeat = Agent::poll_beat(1);

            //        if (!strcmp(cbeat.proc,name.c_str()) && !strcmp(cbeat.node,node.c_str()))
            //        {
            //            if (rbeat != NULL)
            //                *rbeat = cbeat;
            //            return (1);
            //        }

        } while (ep.split() <= waitsec);

        return(0);
    }

    //! Find agent
    /*! Check the ::agent_list for the particular agent,
 * returning its heartbeat if found.
    \param node Node that agent is in.
    \param proc Name of agent.
    \return ::beatstruc of located agent, otherwise empty ::beatstruc.
 */
    beatstruc Agent::find_agent(string agent, string node)
    {
        if (node.empty())
        {
            node = nodeName;
        }
        for (beatstruc &it : agent_list)
        {
            if (it.node == node && it.proc == agent)
            {
                it.exists = true;
                return it;
            }
        }
        beatstruc nobeat;
        nobeat.exists = false;
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
    beatstruc Agent::find_server(string node, string proc, float waitsec)
    {
        beatstruc cbeat = {0.,"","",NetworkType::MULTICAST,"",0,0,0.,"",0.,0.,0.,false};

        //! Loop for ::waitsec seconds, looking for desired agent.

        ElapsedTime ep;
        ep.start();

        do
        {
            cbeat = Agent::poll_beat(1);
            if (cbeat.utc != 0.)
            {
                if (!strcmp(cbeat.proc, proc.c_str()) && !strcmp(cbeat.node, node.c_str()))
                {
                    return cbeat;
                }
            }
        } while (ep.split() <= waitsec);

        // ?? do a complete reset of cbeat if agent not found, not just utc = 0
        cbeat.utc = 0.;
        cbeat.node[0] = '\0';
        // etc ...
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
        beatstruc cbeat, tbeat;
        uint32_t i, j;

        //! Loop for ::waitsec seconds, filling list with any discovered heartbeats.

        ElapsedTime ep;
        ep.start();

        do
        {
            cbeat = Agent::poll_beat(1);
            if (cbeat.utc != 0.)
            {
                for (i=0; i<slist.size(); i++)
                {
                    if (!strcmp(cbeat.node,slist[i].node) && !strcmp(cbeat.proc,slist[i].proc))
                        break;
                }
                if (i == slist.size())
                {
                    slist.push_back(cbeat);
                    for (j=i; j>0; j--)
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

    //! Set SOH string
    /*! Set the SOH string to a JSON list of \ref jsonlib_namespace names. A
 * proper JSON list will begin and end with matched curly braces, be comma separated,
 * and have all strings in double quotes.
    \param list Properly formatted list of JSON names.
    \return 0, otherwise a negative error.
*/
    int32_t Agent::set_sohstring(string list)
    {

        if (!cinfo->pdata.agent[0].sohtable.empty())
        {
            cinfo->pdata.agent[0].sohtable.clear();
        }

        json_table_of_list(cinfo->pdata.agent[0].sohtable, list, cinfo->meta);
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
 * consist of the contents of ::AGENT_MESSAGE_BEAT in ::Agent::poll, plus the contents of the
 * ::sohstring. It will come every ::bprd seconds.
 */
    void Agent::heartbeat_loop()
    {
        ElapsedTime timer_beat;

        while (cinfo->pdata.agent[0].stateflag)
        {

            // compute the jitter
            cinfo->pdata.agent[0].beat.jitter = timer_beat.split() - cinfo->pdata.agent[0].beat.bprd;
            timer_beat.start();

            // post comes first
            cinfo->pdata.agent[0].beat.utc = currentmjd(0.);
            if ((Agent::State)(cinfo->pdata.agent[0].stateflag) != Agent::State::IDLE && !cinfo->pdata.agent[0].sohtable.empty())
            {
                Agent::post(AGENT_MESSAGE_BEAT, json_of_table(hbjstring, cinfo->pdata.agent[0].sohtable, ((cosmosstruc *)cinfo)->meta, ((cosmosstruc *)cinfo)->pdata));
            }
            else
            {
                Agent::post(AGENT_MESSAGE_BEAT,"");
            }

            // TODO: move the monitoring calculations to another thread with its own loop time that can be controlled
            // Compute other monitored quantities if monitoring
            if (cinfo->pdata.agent[0].stateflag == static_cast <uint16_t>(Agent::State::MONITOR))
            {
                // TODO: rename beat.cpu to beat.cpu_percent
                // add beat.cpu_load
                cinfo->pdata.agent[0].beat.cpu    = deviceCpu_.getPercentUseForCurrentProcess();//cpu.getLoad();
                cinfo->pdata.agent[0].beat.memory = deviceCpu_.getVirtualMemoryUsed();
            }

            if (cinfo->pdata.agent[0].stateflag == static_cast <uint16_t>(Agent::State::SHUTDOWN))
            {
                cinfo->pdata.agent[0].beat.cpu = 0;
                cinfo->pdata.agent[0].beat.memory = 0;
            }


            if (cinfo->pdata.agent[0].beat.bprd < AGENT_HEARTBEAT_PERIOD_MIN)
            {
                cinfo->pdata.agent[0].beat.bprd = AGENT_HEARTBEAT_PERIOD_MIN;
            }

            if (timer_beat.split() <= cinfo->pdata.agent[0].beat.bprd)
            {
                COSMOS_SLEEP(cinfo->pdata.agent[0].beat.bprd - timer_beat.split());
            }
        }
        Agent::unpublish();
    }

    //! Request Loop
    /*! This function is run as a thread to service requests to the Agent. It receives requests on
 * it assigned port number, matches the first word of the request against its set of requests,
 * and then either performs the matched function, or returns [NOK].
 */
    void Agent::request_loop()
    {
        char ebuffer[6]="[NOK]";
        int32_t iretn, nbytes;
        char *bufferin, *bufferout;
        char request[AGENTMAXBUFFER+1];
        uint32_t i;

        if ((iretn = socket_open(&cinfo->pdata.agent[0].req, NetworkType::UDP, (char *)"", cinfo->pdata.agent[0].beat.port, SOCKET_LISTEN, SOCKET_BLOCKING, 2000000)) < 0)
        {
            return;
        }

        cinfo->pdata.agent[0].beat.port = cinfo->pdata.agent[0].req.cport;

        if ((bufferin=(char *)calloc(1,cinfo->pdata.agent[0].beat.bsz)) == NULL)
        {
            iretn = -errno;
            return;
        }

        while (cinfo->pdata.agent[0].stateflag)
        {
            iretn = recvfrom(cinfo->pdata.agent[0].req.cudp,bufferin,cinfo->pdata.agent[0].beat.bsz,0,(struct sockaddr *)&cinfo->pdata.agent[0].req.caddr,(socklen_t *)&cinfo->pdata.agent[0].req.addrlen);

            if (iretn > 0)
            {
                bufferin[iretn] = 0;

                if (cinfo->pdata.agent[0].stateflag == static_cast <uint16_t>(Agent::State::DEBUG))
                {
                    printf("Request: [%d] %s ",iretn,bufferin);
                }

                fflush(stdout);
                for (i=0; i<COSMOS_MAX_NAME; i++)
                {
                    if (bufferin[i] == ' ' || bufferin[i] == 0)
                        break;
                    request[i] = bufferin[i];
                }
                request[i] = 0;

                for (i=0; i<Agent::reqs.size(); i++)
                {
                    if (!strcmp(request,Agent::reqs[i].token.c_str()))
                        break;
                }

                if (i < Agent::reqs.size())
                {
                    iretn = -1;
                    if (reqs[i].ifunction)
                    {
                        iretn = (this->*Agent::reqs[i].ifunction)(bufferin, request);
                    }
                    else
                    {
                        if (reqs[i].efunction != nullptr)
                        {
                            iretn = reqs[i].efunction(bufferin, request, this);
                        }
                    }
                    if (iretn >= 0)
                        bufferout = (char *)&request;
                    else
                        bufferout = nullptr;
                }
                else
                {
                    iretn = AGENT_ERROR_NULL;
                    bufferout = nullptr;
                }

                if (bufferout == nullptr)
                {
                    bufferout = ebuffer;
                }
                else
                {
                    strcat(bufferout,"[OK]");
                    bufferout[cinfo->pdata.agent[0].beat.bsz+3] = 0;
                }
                nbytes = sendto(cinfo->pdata.agent[0].req.cudp,bufferout,strlen(bufferout),0,(struct sockaddr *)&cinfo->pdata.agent[0].req.caddr,sizeof(struct sockaddr_in));
                if (cinfo->pdata.agent[0].stateflag == static_cast <uint16_t>(Agent::State::DEBUG))
                {
                    printf("[%d] %s\n",nbytes,bufferout);
                }
            }
        }
        free(bufferin);
        return;
    }

    // TODO: describe function, what does it do?
    void Agent::message_loop()
    {
        messstruc mess;
        int32_t iretn;

        // Initialize things
        message_ring.resize(MESSAGE_RING_SIZE);

        while (Agent::running())
        {
            iretn = Agent::poll(mess, AGENT_MESSAGE_ALL, 5.);
            if (iretn > 0)
            {
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
                    bool node_found = false;
                    for (jsonnode &i : node_list)
                    {
                        if (!i.name.compare(mess.meta.beat.node))
                        {
                            node_found = true;
                        }
                    }
                    if (!node_found)
                    {
                        jsonnode jnode;
                        send_request_jsonnode(mess.meta.beat, jnode);
                        node_list.push_back(jnode);
                    }
                }

                size_t new_position;
                new_position = message_head + 1;
                if (new_position >= message_ring.size())
                {
                    new_position = 0;
                }
                message_ring[new_position] = mess;
                message_head = new_position;
            }
            COSMOS_SLEEP(.01);
        }
    }

    //! Built-in Forward request
    /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_forward(char* request, char* output, Agent* agent)
    {
        uint16_t count;
        int32_t iretn=-1;

        sscanf(request,"%*s %hu",&count);
        for (uint16_t i=0; i<agent->cinfo->pdata.agent[0].ifcnt; ++i)
        {
            iretn = sendto(agent->cinfo->pdata.agent[0].pub[i].cudp,(const char *)&request[strlen(request)-count],count,0,(struct sockaddr *)&agent->cinfo->pdata.agent[0].pub[i].baddr,sizeof(struct sockaddr_in));
        }
        sprintf(output,"%.17g %d ",currentmjd(0),iretn);
        return(0);
    }

    //! Built-in Echo request
    /*! Returns the received packet, reaclculating the CRC, and adding the time.
 * \param request Text of echo packet.
 * \param output Text of echoed packet.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_echo(char* request, char* output, Agent*)
    {
        double mjd;
        uint16_t crc, count;

        sscanf(request,"%*s %lf %hx %hu",&mjd,&crc,&count);
        sprintf(output,"%.17g %x %u ",currentmjd(0),slip_calc_crc((uint8_t *)&request[strlen(request)-count],count),count);
        strncpy(&output[strlen(output)],&request[strlen(request)-count],count+1);
        return(0);
    }

    //! Built-in Help request
    /*! Send help response.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_help(char*, char* output, Agent* agent)
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
        strcpy(output, (char*)help_string.c_str());
        return 0;
    }

    //! Built-in Set state to Run request
    /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_run(char*, char* output, Agent* agent)
    {
        agent->cinfo->pdata.agent[0].stateflag = static_cast <uint16_t>(Agent::State::RUN);
        output[0] = 0;
        return(0);
    }

    //! Built-in Set state to Init request
    /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_init(char*, char* output, Agent* agent)
    {
        agent->cinfo->pdata.agent[0].stateflag = static_cast <uint16_t>(Agent::State::INIT);
        output[0] = 0;
        return(0);
    }

    //! Built-in Set state to Idle request
    /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_idle(char*, char* output, Agent* agent)
    {
        agent->cinfo->pdata.agent[0].stateflag = static_cast <uint16_t>(Agent::State::IDLE);
        output[0] = 0;
        return(0);
    }

    //! Built-in Set state to Idle request
    /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_monitor(char*, char* output, Agent* agent)
    {
        agent->cinfo->pdata.agent[0].stateflag = static_cast <uint16_t>(Agent::State::MONITOR);
        output[0] = 0;
        return(0);
    }

    //! Built-in Set state to Shutdown request
    /*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_shutdown(char*, char* output, Agent* agent)
    {
        agent->cinfo->pdata.agent[0].stateflag = static_cast <uint16_t>(Agent::State::SHUTDOWN);
        output[0] = 0;
        return(0);
    }

    // TODO: add a line break (\n) when printing the data
    // this makes it easier to read
    //! Built-in Status request
    /*! Returns agent status.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_status(char*, char* output, Agent* agent)
    {
        string jstring;

        if (json_of_agent(jstring, agent->cinfo->meta, agent->cinfo->pdata) != NULL)
        {
            strncpy(output, jstring.c_str(),agent->cinfo->pdata.agent[0].beat.bsz);
            output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
            return 0;
        }
        else
        {
            strcpy(output,"error");
            output[5] = 0;
            return(JSON_ERROR_SCAN);
        }
    }

    //! Built-in Get Internal Value request
    /*! Returns the current value of the requested Name Space values. Names are expressed as a JSON object.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_getvalue(char *request, char* output, Agent* agent)
    {
        string jstring;

        if (json_of_list(jstring, request, agent->cinfo->meta, agent->cinfo->pdata) != NULL)
        {
            strncpy(output, jstring.c_str(), agent->cinfo->pdata.agent[0].beat.bsz);
            output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
            return 0;
        }
        else
            return (JSON_ERROR_EOS);
    }

    //! Built-in Set Internal Value request
    /*! Sets the current value of the requested Name Space values. Names and values are expressed as a JSON object.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_setvalue(char *request, char* output, Agent* agent)
    {
        int32_t iretn;
        iretn = json_parse(request,agent->cinfo->meta, agent->cinfo->pdata);

        sprintf(output,"%d",iretn);

        return(iretn);
    }

    //! Built-in List Name Space Names request
    /*! Returns a list of all names in the JSON Name Space.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_listnames(char *, char* output, Agent* agent)
    {
        string result = json_list_of_all(agent->cinfo->meta);
        strncpy(output, result.c_str(), agent->cinfo->pdata.agent[0].beat.bsz);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
        return 0;
    }

    //! Built-in Return Node JSON request
    /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_nodejson(char *, char* output, Agent* agent)
    {
        strncpy(output, agent->cinfo->json.node.c_str(), agent->cinfo->json.node.size()<agent->cinfo->pdata.agent[0].beat.bsz-1?agent->cinfo->json.node.size():agent->cinfo->pdata.agent[0].beat.bsz-1);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
        return 0;
    }

    //! Built-in Return State Vector JSON request
    /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_statejson(char *, char* output, Agent* agent)
    {
        strncpy(output, agent->cinfo->json.state.c_str(), agent->cinfo->json.state.size()<agent->cinfo->pdata.agent[0].beat.bsz-1?agent->cinfo->json.state.size():agent->cinfo->pdata.agent[0].beat.bsz-1);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
        return 0;
    }

    //! Built-in Return Node JSON request
    /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_utcstartjson(char *, char* output, Agent* agent)
    {
        strncpy(output, agent->cinfo->json.utcstart.c_str(), agent->cinfo->json.utcstart.size()<agent->cinfo->pdata.agent[0].beat.bsz-1?agent->cinfo->json.utcstart.size():agent->cinfo->pdata.agent[0].beat.bsz-1);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
        return 0;
    }

    //! Built-in Return Node JSON request
    /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_piecesjson(char *, char* output, Agent* agent)
    {
        strncpy(output, agent->cinfo->json.pieces.c_str(), agent->cinfo->json.pieces.size()<agent->cinfo->pdata.agent[0].beat.bsz-1?agent->cinfo->json.pieces.size():agent->cinfo->pdata.agent[0].beat.bsz-1);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
        return 0;
    }

    //! Built-in Return Node JSON request
    /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_devgenjson(char *, char* output, Agent* agent)
    {
        strncpy(output, agent->cinfo->json.devgen.c_str(), agent->cinfo->json.devgen.size()<agent->cinfo->pdata.agent[0].beat.bsz-1?agent->cinfo->json.devgen.size():agent->cinfo->pdata.agent[0].beat.bsz-1);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
        return 0;
    }

    //! Built-in Return Node JSON request
    /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_devspecjson(char *, char* output, Agent* agent)
    {
        strncpy(output, agent->cinfo->json.devspec.c_str(), agent->cinfo->json.devspec.size()<agent->cinfo->pdata.agent[0].beat.bsz-1?agent->cinfo->json.devspec.size():agent->cinfo->pdata.agent[0].beat.bsz-1);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
        return 0;
    }

    //! Built-in Return Node JSON request
    /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_portsjson(char *, char* output, Agent* agent)
    {
        strncpy(output, agent->cinfo->json.ports.c_str(), agent->cinfo->json.ports.size()<agent->cinfo->pdata.agent[0].beat.bsz-1?agent->cinfo->json.ports.size():agent->cinfo->pdata.agent[0].beat.bsz-1);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
        return 0;
    }

    //! Built-in Return Node JSON request
    /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_targetsjson(char *, char* output, Agent* agent)
    {
        strncpy(output, agent->cinfo->json.targets.c_str(), agent->cinfo->json.targets.size()<agent->cinfo->pdata.agent[0].beat.bsz-1?agent->cinfo->json.targets.size():agent->cinfo->pdata.agent[0].beat.bsz-1);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
        return 0;
    }

    //! Built-in Return Node JSON request
    /*! Returns a JSON string representing the Node description.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param agent Pointer to ::Agent to use.
 * \return 0, or negative error.
 */
    int32_t Agent::req_aliasesjson(char *, char* output, Agent* agent)
    {
        strncpy(output, agent->cinfo->json.aliases.c_str(), agent->cinfo->json.aliases.size()<agent->cinfo->pdata.agent[0].beat.bsz-1?agent->cinfo->json.aliases.size():agent->cinfo->pdata.agent[0].beat.bsz-1);
        output[agent->cinfo->pdata.agent[0].beat.bsz-1] = 0;
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
#ifdef COSMOS_WIN_OS
#else
#endif // COSMOS_WIN_OS
        int32_t iretn;
        int on = 1;
        //uint32_t ip, net, bcast;

        // Return immediately if we've already done this
        if (cinfo->pdata.agent[0].pub[0].cport)
            return 0;

        switch (type)
        {
        case NetworkType::MULTICAST:
        case NetworkType::UDP:
            {
                for (uint32_t i=0; i<AGENTMAXIF; i++)
                    cinfo->pdata.agent[0].pub[i].cudp = -1;

                if ((cinfo->pdata.agent[0].pub[0].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                {
                    return (AGENT_ERROR_SOCKET);
                }

                // Use above socket to find available interfaces and establish
                // publication on each.
                cinfo->pdata.agent[0].ifcnt = 0;

#if defined(COSMOS_WIN_OS)
                struct sockaddr_storage ss;
                int sslen;
                INTERFACE_INFO ilist[20];
                unsigned long nbytes;
                uint32_t nif;
                if (WSAIoctl(cinfo->pdata.agent[0].pub[0].cudp, SIO_GET_INTERFACE_LIST, 0, 0, &ilist,sizeof(ilist), &nbytes, 0, 0) == SOCKET_ERROR)
                {
                    CLOSE_SOCKET(cinfo->pdata.agent[0].pub[0].cudp);
                    return (AGENT_ERROR_DISCOVERY);
                }

                nif = nbytes / sizeof(INTERFACE_INFO);
                for (uint32_t i=0; i<nif; i++)
                {
                    inet_ntop(ilist[i].iiAddress.AddressIn.sin_family,&ilist[i].iiAddress.AddressIn.sin_addr,cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].address,sizeof(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].address));
                    //            strcpy(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].address,inet_ntoa(((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr));
                    if (!strcmp(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].address,"127.0.0.1"))
                    {
                        if (cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp >= 0)
                        {
                            CLOSE_SOCKET(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp);
                            cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp = -1;
                        }
                        continue;
                    }
                    // No need to open first socket again
                    if (cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp < 0)
                    {
                        if ((cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                        {
                            continue;
                        }
                    }

                    memset(&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr,0,sizeof(struct sockaddr_in));
                    cinfo->pdata.agent[0].pub[i].caddr.sin_family = AF_INET;
                    cinfo->pdata.agent[0].pub[i].baddr.sin_family = AF_INET;
                    if (type == NetworkType::MULTICAST)
                    {
                        sslen = sizeof(ss);
                        WSAStringToAddressA((char *)AGENTMCAST,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                    }
                    else
                    {
                        if ((iretn = setsockopt(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                        {
                            CLOSE_SOCKET(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp);
                            continue;
                        }

                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;

                        uint32_t ip, net, bcast;
                        ip = ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.S_un.S_addr;
                        net = ((struct sockaddr_in*)&(ilist[i].iiNetmask))->sin_addr.S_un.S_addr;
                        bcast = ip | (~net);
                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_addr.S_un.S_addr = bcast;
                    }
                    cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr.sin_port = htons(port);
                    cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_port = htons(port);
                    cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].type = type;
                    cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cport = port;
                    cinfo->pdata.agent[0].ifcnt++;
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
                        inet_ntop(if_addr->ifa_addr->sa_family,&((struct sockaddr_in*)if_addr->ifa_addr)->sin_addr,cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].address,sizeof(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].address));
                        memcpy((char *)&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr, (char *)if_addr->ifa_addr, sizeof(if_addr->ifa_addr));

                        if ((if_addr->ifa_flags & IFF_POINTOPOINT) || (if_addr->ifa_flags & IFF_UP) == 0 || (if_addr->ifa_flags & IFF_LOOPBACK) || (if_addr->ifa_flags & (IFF_BROADCAST)) == 0)
                        {
                            continue;
                        }

                        // No need to open first socket again
                        if (cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp < 0)
                        {
                            if ((cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                            {
                                continue;
                            }
                        }

                        if (type == NetworkType::MULTICAST)
                        {
                            inet_pton(AF_INET,AGENTMCAST,&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr.sin_addr);
                            inet_pton(AF_INET,AGENTMCAST,&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_addr);
                        }
                        else
                        {
                            if ((iretn = setsockopt(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                            {
                                CLOSE_SOCKET(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp);
                                continue;
                            }

                            //                    if (ioctl(cinfo->pdata.agent[0].pub[0].cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                            //                    {
                            //                        continue;
                            //                    }
                            //                    cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr = cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr;
                            memcpy((char *)&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr, (char *)if_addr->ifa_netmask, sizeof(if_addr->ifa_netmask));

                            uint32_t ip, net, bcast;
                            ip = cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr.sin_addr.s_addr;
                            net = cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_addr.s_addr;
                            bcast = ip | (~net);
                            cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_addr.s_addr = bcast;
                            inet_ntop(if_addr->ifa_netmask->sa_family,&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_addr,cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddress,sizeof(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddress));
                        }
                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr.sin_port = htons(port);
                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_port = htons(port);
                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].type = type;
                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cport = port;
                        cinfo->pdata.agent[0].ifcnt++;
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
                if (ioctl(cinfo->pdata.agent[0].pub[0].cudp,SIOCGIFCONF,&confa) < 0)
                {
                    CLOSE_SOCKET(cinfo->pdata.agent[0].pub[0].cudp);
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
                    inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].address,sizeof(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].address));
                    memcpy((char *)&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr, (char *)&ifra->ifr_addr, sizeof(ifra->ifr_addr));

                    if (ioctl(cinfo->pdata.agent[0].pub[0].cudp,SIOCGIFFLAGS, (char *)ifra) < 0) continue;

                    if ((ifra->ifr_flags & IFF_POINTOPOINT) || (ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK) || (ifra->ifr_flags & (IFF_BROADCAST)) == 0)
                    {
                        continue;
                    }

                    // No need to open first socket again
                    if (cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp < 0)
                    {
                        if ((cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                        {
                            continue;
                        }
                    }

                    if (type == NetworkType::MULTICAST)
                    {
                        inet_pton(AF_INET,AGENTMCAST,&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr.sin_addr);
                        inet_pton(AF_INET,AGENTMCAST,&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_addr);
                    }
                    else
                    {
                        if ((iretn = setsockopt(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                        {
                            CLOSE_SOCKET(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cudp);
                            continue;
                        }

                        if (ioctl(cinfo->pdata.agent[0].pub[0].cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                        {
                            continue;
                        }
                        cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr = cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr;
                        inet_ntop(ifra->ifr_broadaddr.sa_family,&((struct sockaddr_in*)&ifra->ifr_broadaddr)->sin_addr,cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddress,sizeof(cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddress));
                        inet_pton(AF_INET,cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddress,&cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_addr);
                    }
                    cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].caddr.sin_port = htons(port);
                    cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].baddr.sin_port = htons(port);
                    cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].type = type;
                    cinfo->pdata.agent[0].pub[cinfo->pdata.agent[0].ifcnt].cport = port;
                    cinfo->pdata.agent[0].ifcnt++;
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
                    if (ifra->ifr_addr.sa_family != AF_INET) continue;
                    inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,tiface.address,sizeof(tiface.address));

                    if (ioctl(cudp,SIOCGIFFLAGS, (char *)ifra) < 0) continue;

                    if ((ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK) || (ifra->ifr_flags & (IFF_BROADCAST)) == 0) continue;

                    if (ntype == NetworkType::MULTICAST)
                    {
                        inet_pton(AF_INET,AGENTMCAST,&tiface.caddr.sin_addr);\
                        strcpy(tiface.baddress, AGENTMCAST);
                        inet_pton(AF_INET,AGENTMCAST,&tiface.baddr.sin_addr);\
                    }
                    else
                    {
                        if ((iretn = setsockopt(cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                        {
                            continue;
                        }

                        strncpy(tiface.name, ifra->ifr_name, COSMOS_MAX_NAME);
                        if (ioctl(cudp,SIOCGIFBRDADDR,(char *)ifra) < 0) continue;
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

    //! Post a ::messstruc
    /*! Post an already defined message on the previously opened publication channel.
    \param mess ::messstruc containing everything necessary, including type, header and data.
    \return 0, otherwise negative error.
*/
    int32_t Agent::post(messstruc mess)
    {
        int32_t iretn;

        if (mess.meta.type < AGENT_MESSAGE_BINARY)
        {
            iretn = post(mess.meta.type, mess.adata);
        }
        else
        {
            iretn = post(mess.meta.type, mess.bdata);
        }
//        size_t nbytes;
//        int32_t iretn=0;
//        uint8_t post[AGENTMAXBUFFER];

//        cinfo->pdata.agent[0].beat.utc = cinfo->pdata.agent[0].beat.utc;
//        post[0] = mess.meta.type;
//        // this will broadcast messages to all external interfaces (ifcnt = interface count)
//        for (size_t i=0; i<cinfo->pdata.agent[0].ifcnt; i++)
//        {
//            sprintf((char *)&post[3], "%s", mess.jdata.c_str());
//            size_t hlength = strlen((char *)&post[3]);
//            post[1] = hlength%256;
//            post[2] = hlength / 256;
//            nbytes = hlength + 3;

//            if (mess.meta.type < AGENT_MESSAGE_BINARY && mess.adata.size())
//            {
//                if (nbytes+mess.adata.size() > AGENTMAXBUFFER)
//                    return (AGENT_ERROR_BUFLEN);
//                memcpy(&post[nbytes], &mess.adata[0], mess.adata.size());
//                nbytes += mess.adata.size();
//            }

//            if (mess.meta.type >= AGENT_MESSAGE_BINARY && mess.bdata.size())
//            {
//                if (nbytes+mess.bdata.size() > AGENTMAXBUFFER)
//                    return (AGENT_ERROR_BUFLEN);
//                memcpy(&post[nbytes], &mess.bdata[0], mess.bdata.size());
//                nbytes += mess.bdata.size();
//            }

//            iretn = sendto(cinfo->pdata.agent[0].pub[i].cudp, (const char *)post, nbytes, 0,(struct sockaddr *)&cinfo->pdata.agent[0].pub[i].baddr, sizeof(struct sockaddr_in));
//        }
//        if (iretn<0)
//        {
//#ifdef COSMOS_WIN_OS
//            return(-WSAGetLastError());
//#else
//            return (-errno);
//#endif
//        }
        return iretn;
    }

    //! Post a JSON message
    /*! Post a vector of bytes on the previously opened publication channel.
    \param type A byte indicating the type of message.
    \param message A NULL terminated JSON text string to post.
    \return 0, otherwise negative error.
*/
    int32_t Agent::post(uint8_t type, string message)
    {
        int32_t iretn;
        vector<uint8_t> bytes(message.begin(), message.end());
        bytes.push_back(0);
        iretn = post(type, bytes);
        return iretn;
    }

    //! Post a binary message
    /*! Post a vector of bytes on the previously opened publication channel.
    \param type A byte indicating the type of message.
    \param message An array of bytes to post.
    \return 0, otherwise negative error.
*/
    int32_t Agent::post(uint8_t type, vector <uint8_t> message)
    {
        size_t nbytes;
        int32_t iretn=0;
        uint8_t post[AGENTMAXBUFFER];

        cinfo->pdata.agent[0].beat.utc = cinfo->pdata.agent[0].beat.utc;
        post[0] = type;
        // this will broadcast messages to all external interfaces (ifcnt = interface count)
        for (size_t i=0; i<cinfo->pdata.agent[0].ifcnt; i++)
        {
            sprintf((char *)&post[3],"{\"agent_utc\":%.15g}{\"agent_node\":\"%s\"}{\"agent_proc\":\"%s\"}{\"agent_addr\":\"%s\"}{\"agent_port\":%u}{\"agent_bprd\":%f}{\"agent_bsz\":%u}{\"agent_cpu\":%f}{\"agent_memory\":%f}{\"agent_jitter\":%f}{\"node_utcoffset\":%.15g}",
                    cinfo->pdata.agent[0].beat.utc,
                    cinfo->pdata.agent[0].beat.node,
                    cinfo->pdata.agent[0].beat.proc,
                    cinfo->pdata.agent[0].pub[i].address,
                    cinfo->pdata.agent[0].beat.port,
                    cinfo->pdata.agent[0].beat.bprd,
                    cinfo->pdata.agent[0].beat.bsz,
                    cinfo->pdata.agent[0].beat.cpu,
                    cinfo->pdata.agent[0].beat.memory,
                    cinfo->pdata.agent[0].beat.jitter,
                    cinfo->pdata.node.utcoffset);
            size_t hlength = strlen((char *)&post[3]);
            post[1] = hlength%256;
            post[2] = hlength / 256;
            nbytes = hlength + 3;

            if (message.size())
            {
                if (nbytes+message.size() > AGENTMAXBUFFER)
                    return (AGENT_ERROR_BUFLEN);
                memcpy(&post[nbytes], &message[0], message.size());
                //            strcat(post,message);
            }
            iretn = sendto(cinfo->pdata.agent[0].pub[i].cudp,       // socket
                    (const char *)post,                         // buffer to send
                    nbytes+message.size(),                      // size of buffer
                    0,                                          // flags
                    (struct sockaddr *)&cinfo->pdata.agent[0].pub[i].baddr, // socket address
                    sizeof(struct sockaddr_in)                  // size of address to socket pointer
                    );
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
        for (size_t i=0; i<cinfo->pdata.agent[0].ifcnt; ++i)
        {
            CLOSE_SOCKET(cinfo->pdata.agent[0].pub[i].cudp);
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
    int32_t Agent::subscribe(NetworkType type, char *address, uint16_t port, uint32_t usectimeo)
    {
        int32_t iretn = 0;

        // ?? this is preventing from running socket_open if
        // for some reason cinfo->pdata.agent[0].sub.cport was ill initialized
#ifndef COSMOS_WIN_BUILD_MSVC
        if (cinfo->pdata.agent[0].sub.cport)
            return 0;
#endif
        if ((iretn=socket_open(&cinfo->pdata.agent[0].sub,type,address,port,SOCKET_LISTEN,SOCKET_BLOCKING, usectimeo)) < 0)
        {
            return (iretn);
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
    int32_t Agent::subscribe(NetworkType type, char *address, uint16_t port)
    {
        int32_t iretn = 0;

        if ((iretn=Agent::subscribe(type, address, port, 100)) < 0)
        {
            return (iretn);
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
            CLOSE_SOCKET(cinfo->pdata.agent[0].sub.cudp);
        }
        return 0;
    }

    //! Listen for string message
    /*! Poll the subscription channel for the requested amount of time. Return as soon as a single message
 * comes in, or the timer runs out.
 * \param meta ::pollstruc for storing meta information.
    \param message String for storing incoming message.
    \param type Type of message to look for, taken from ::AGENT_MESSAGE.
    \param waitsec Number of seconds in timer.
    \return If a message comes in, return its type. If none comes in, return zero, otherwise negative error.
*/
    //int32_t Agent::poll(pollstruc &meta, string &message, uint8_t type, float waitsec)
    //{
    //    vector <uint8_t> bytes;
    //    int32_t iretn;

    //    iretn = poll(meta, bytes, type, waitsec);
    //    if (iretn > 0)
    //    {
    //        bytes.push_back(0);
    //        message = (char *)bytes.data();
    //    }
    //    return iretn;
    //}

    //! Listen for binary message
    /*! Poll the subscription channel for the requested amount of time. Return as soon as a single message
 * comes in, or the timer runs out.
 * \param meta ::pollstruc for storing meta information.
    \param mess ::messstruc for storing incoming message.
    \param type Type of message to look for, taken from ::AGENT_MESSAGE.
    \param waitsec Number of seconds in timer.
    \return If a message comes in, return its type. If none comes in, return zero, otherwise negative error.
*/
    int32_t Agent::poll(messstruc &mess, uint8_t type, float waitsec)
    {
        int nbytes;
        uint8_t input[AGENTMAXBUFFER+1];

        if (cinfo == nullptr)
        {
            return AGENT_ERROR_NULL;
        }

        if (!cinfo->pdata.agent[0].sub.cport)
            return (AGENT_ERROR_CHANNEL);

        ElapsedTime ep;
        ep.start();
        do
        {
            nbytes = 0;
            switch (cinfo->pdata.agent[0].sub.type)
            {
            case NetworkType::MULTICAST:
            case NetworkType::UDP:

                nbytes = recvfrom(cinfo->pdata.agent[0].sub.cudp, (char *)input,AGENTMAXBUFFER, 0, (struct sockaddr *)&cinfo->pdata.agent[0].sub.caddr, (socklen_t *)&cinfo->pdata.agent[0].sub.addrlen);

                // Return if port and address are our own
                for (uint16_t i=0; i<cinfo->pdata.agent[0].ifcnt; ++i)
                {
                    if (cinfo->pdata.agent[0].sub.caddr.sin_port == cinfo->pdata.agent[0].pub[i].caddr.sin_port &&
                        cinfo->pdata.agent[0].sub.caddr.sin_addr.s_addr == cinfo->pdata.agent[0].pub[i].caddr.sin_addr.s_addr)
                    {
                        return 0;
                        break;
                    }
                }
                break;
            default:
                break;
            }

            if (nbytes > 0)
            {
                if (type == AGENT_MESSAGE_ALL || type == input[0])
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
                        mess.meta.type = (uint16_t)input[0];
                        mess.meta.jlength = input[1] + 256 * input[2];
                    }
                    else
                    {
                        mess.meta.type = (uint16_t)input[0] + 1;
                        mess.meta.jlength = nbytes;
                    }

                    // Copy message parts to ring, placing in appropriate buffers.
                    // First: JSON header
                    mess.jdata.assign((const char *)&input[start_byte], mess.meta.jlength);

                    // Next: ASCII or BINARY message, depending on message type.
                    if (mess.meta.type < AGENT_MESSAGE_BINARY)
                    {
                        mess.adata.assign((const char *)&input[start_byte+mess.meta.jlength], nbytes - (start_byte + mess.meta.jlength));
                    }
                    else
                    {
                        mess.bdata.resize(nbytes - (start_byte + mess.meta.jlength));
                        memcpy(mess.bdata.data(), &input[start_byte + mess.meta.jlength], nbytes - (start_byte + mess.meta.jlength));
                    }

                    // Extract meta data
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

                    return ((int)mess.meta.type);
                }
            }
            if (ep.split() >= waitsec)
            {
                nbytes = 0;
            }
        } while (nbytes != 0);

        return 0;
    }

    //! Check Ring for message
    /*! Check the message ring for the requested amount of time. Return as soon as a message of the right type
 * is available, or the timer runs out.
    \param message Vector for storing incoming message.
    \param type Type of message to look for, taken from ::AGENT_MESSAGE.
    \param waitsec Number of seconds in timer. If 0, return last message in ring immediatelly.
    \return If a message comes in, return its type. If none comes in, return zero, otherwise negative error.
*/
    int32_t Agent::readring(messstruc &message, uint8_t type, float waitsec, Where where)
    {
        if (waitsec < 0.)
        {
            waitsec = 0.;
        }

        if (cinfo == nullptr)
        {
            return AGENT_ERROR_NULL;
        }

        if (where == Where::HEAD)
        {
            message_tail = message_head - 1;
            if (message_tail >= message_ring.size())
            {
                message_tail = message_ring.size() - 1;
            }
        }
        ElapsedTime ep;
        ep.start();
        do
        {
            if (message_head != message_tail || waitsec == 0.)
            {
                ++message_tail;
                if (message_tail >= message_ring.size())
                {
                    message_tail = 0;
                }

                if (type == AGENT_MESSAGE_ALL || type == message_ring[message_tail].meta.type)
                {
                    // Copy message.
                    message = message_ring[message_tail];
                    return ((int)message.meta.type);
                }
            }

            if (ep.split() < waitsec)
            {
                COSMOS_SLEEP(.1);
            }
        } while (ep.split() < waitsec);

        return 0;
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
    //! in the message ring. This has the effect of emptying the message ring as far as ::readring
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
    beatstruc Agent::poll_beat(float waitsec)
    {
        int32_t iretn;
        beatstruc beat;
        messstruc mess;

        iretn = Agent::poll(mess, AGENT_MESSAGE_BEAT, waitsec);

        beat.utc = 0.;
        if (iretn == AGENT_MESSAGE_BEAT)
        {
            beat = mess.meta.beat;
        }

        return (beat);
    }

    //! Listen for Time
    /*! Poll the subscription channel until you receive a time message, or the timer runs out.
    \param waitsec Number of seconds to wait before timing out.
    \return ::timestruc with acquired time. The UTC will be set to 0 if no heartbeat was
    acquired.
*/
    timestruc Agent::poll_time(float waitsec)
    {
        int32_t iretn;
        timestruc time;
        messstruc mess;

        iretn = Agent::poll(mess, AGENT_MESSAGE_TIME, waitsec);

        if (iretn == AGENT_MESSAGE_TIME)
        {
            iretn = json_parse(mess.adata, cinfo->meta, cinfo->sdata);
            if (iretn >= 0)
            {
                time.mjd = cinfo->sdata.node.loc.utc;
            }
        }

        return (time);
    }

    //! Listen for Location
    /*! Poll the subscription channel until you receive a location message, or the timer runs out.
    \param waitsec Number of seconds to wait before timing out.
    \return ::locstruc with acquired location. The UTC will be set to 0 if no heartbeat was
    acquired.
*/
    locstruc Agent::poll_location(float waitsec)
    {
        int32_t iretn;
        locstruc loc;
        messstruc mess;

        iretn = Agent::poll(mess, AGENT_MESSAGE_LOCATION, waitsec);

        if (iretn == AGENT_MESSAGE_LOCATION)
        {
            iretn = json_parse(mess.adata, cinfo->meta, cinfo->sdata);
            if (iretn >= 0)
            {
                loc = cinfo->sdata.node.loc;
            }
        }

        return (loc);
    }

    //! Listen for Beacon
    /*! Poll the subscription channel until you receive a info message, or the timer runs out.
    \param waitsec Number of seconds to wait before timing out.
    \return ::infostruc with acquired info. The UTC will be set to 0 if no info was
    acquired.
*/
    nodestruc Agent::poll_info(float waitsec)
    {
        int32_t iretn;
        //summarystruc info;
        nodestruc info;
        messstruc mess;

        iretn = Agent::poll(mess, AGENT_MESSAGE_TRACK, waitsec);

        if (iretn == AGENT_MESSAGE_TRACK)
        {
            iretn = json_parse(mess.adata, cinfo->meta, cinfo->sdata);
            if (iretn >= 0)
            {
                strcpy(info.name,cinfo->sdata.node.name);
                //			info.utc = cinfo->pdata.node.loc.utc;
                info.loc = cinfo->sdata.node.loc;
                info.powgen = cinfo->sdata.node.powgen;
                info.powuse = cinfo->sdata.node.powuse;
                info.battlev = cinfo->sdata.node.battlev;
            }
        }
        else
            info.loc.utc = 0.;

        return (info);
    }

    //! Listen for IMU device
    /*! Poll the subscription channel until you receive a IMU device message, or the timer runs out.
    \param waitsec Number of seconds to wait before timing out.
    \return ::beatstruc with acquired heartbeat. The UTC will be set to 0 if no heartbeat was
    acquired.
*/
    imustruc Agent::poll_imu(float waitsec)
    {
        int32_t iretn;
        imustruc imu;
        messstruc mess;

        iretn = Agent::poll(mess, AGENT_MESSAGE_IMU, waitsec);

        if (iretn == AGENT_MESSAGE_IMU)
        {
            iretn = json_parse(mess.adata, cinfo->meta, cinfo->sdata);
            if (iretn >= 0)
            {
                imu = *cinfo->sdata.devspec.imu[0];
            }
        }

        return (imu);
    }

    std::string Agent::getNode()
    {
        return nodeName;
    }

    std::string Agent::getAgent()
    {
        return agentName;
    }

} // end namespace Cosmos

//! @}
