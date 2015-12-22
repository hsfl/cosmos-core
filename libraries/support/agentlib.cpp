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

/*! \file agentlib.cpp
    \brief Agent support functions
*/

#include "agentlib.h"
#include "socketlib.h"
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

//! \ingroup agentlib
//! \defgroup agentlib_statics Agent Server and Client static variables
//! @{

std::string hbjstring;
static std::vector<beatstruc> slist;
std::vector <agent_request_entry> ireqs;

#include<iostream>
//using namespace std; // don't use this as it may cause conflicts with other namespaces

//! Handle for request thread
static std::thread cthread;
//! Handle for heartbeat thread
static std::thread hthread;
//! @}

void heartbeat_loop(cosmosstruc *cdata);
void request_loop(cosmosstruc *cdata);
char * agent_parse_request(char *input);

//! \ingroup agentlib
//! \defgroup agentlib_functions Agent Server and Client functions
//! @{

//! Add request to Agent request list
/*! Adds access to the indicated function by way of the given token. The provided
* function will be called with the request string as its argument. Any result will then
* be returned in the output pointer.
    \param cdata Pointer to ::cosmosstruc to use.
    \param token A string of maximum length ::COSMOS_MAX_NAME containing the token
    for the request. This should be the first word in the request.
    \param function The user supplied function to parse the specified request.
    \return Error, if any, otherwise zero.
*/
int32_t agent_add_request(cosmosstruc *cdata, std::string token, agent_request_function function)
{
    int32_t iretn;

    iretn = agent_add_request(cdata, token, function, "", "");

    return iretn;
}

//! Add request to Agent request list with description
/*! Adds access to the indicated function by way of the given token. The provided
* function will be called with the request string as its argument. Any result will then
* be returned in the output pointer.
    \param cdata Pointer to ::cosmosstruc to use.
    \param token A string of maximum length ::COSMOS_MAX_NAME containing the token
    for the request. This should be the first word in the request.
    \param function The user supplied function to parse the specified request.
    \param description A brief description of the function performed.
    \return Error, if any, otherwise zero.
*/
int32_t agent_add_request(cosmosstruc *cdata, std::string token, agent_request_function function, std::string description)
{
    int32_t iretn;

    iretn = agent_add_request(cdata, token, function, "", description);

    return iretn;
}

//! Add request to Agent request list with description and synopsis.
/*! Adds access to the indicated function by way of the given token. The provided
* function will be called with the request string as its argument. Any result will then
* be returned in the output pointer.
    \param cdata Pointer to ::cosmosstruc to use.
    \param token A string of maximum length ::COSMOS_MAX_NAME containing the token
    for the request. This should be the first word in the request.
    \param function The user supplied function to parse the specified request.
    \param description A brief description of the function performed.
    \param synopsis A usage synopsis for the request.
    \return Error, if any, otherwise zero.
*/
int32_t agent_add_request(cosmosstruc *cdata, std::string token, agent_request_function function, std::string synopsis, std::string description)
{
    if (cdata[0].agent[0].reqs.size() > AGENTMAXREQUESTCOUNT)
        return (AGENT_ERROR_REQ_COUNT);

    agent_request_entry tentry;
    if (token.size() > COSMOS_MAX_NAME)
    {
        token.resize(COSMOS_MAX_NAME);
    }
    tentry.token = token;
    tentry.function = function;
    tentry.synopsis = synopsis;
    tentry.description = description;
    cdata[0].agent[0].reqs.push_back(tentry);
    return 0;
}

//! Start Agent Request and Heartbeat loops
/*!	Starts the request and heartbeat threads for an Agent server initialized with
 * ::agent_setup_server. The Agent will open its request and heartbeat channels using the
 * address and port supplied in cdata. The heartbeat will cycle with the period requested in cdata.
    \param cdata Pointer to ::cosmosstruc to use.
    \return value returned by request thread create
*/
int32_t agent_start(cosmosstruc *cdata)
{

    // start heartbeat thread
    hthread = std::thread(heartbeat_loop, cdata);
    cthread = std::thread(request_loop, cdata);
    return 0;
}

//! Prepare Agent client
/*! This is the first function to call when setting up a program as an Agent client. It establishes the
 * Subscription channel for collecting messages and sets up the Name Space for the requested Node.
    \param ntype Type of network channel to use, taken from ::AGENT_TYPE.
    \param node Name of Node.
    \param usectimeo Blocking read timeout in micro seconds.
    \return Pointer to ::cosmosstruc to be used for all other calls, otherwise NULL.
*/
cosmosstruc *agent_setup_client(NetworkType ntype, std::string node, uint32_t usectimeo)
{
    int32_t iretn;
    cosmosstruc *cdata;

    // Initialize COSMOS data space
    if ((cdata = json_create()) == nullptr)
    {
        return(nullptr);
    }

    //! Next, set up node.
    if (!node.empty() && (iretn=json_setup_node(node, cdata)) != 0)
    {
        json_destroy(cdata);
        return nullptr;
    }

    if (node.empty())
    {
        strcpy(cdata[0].node.name,"null");
    }

    // Establish subscribe channel
    iretn = agent_subscribe(cdata, ntype, (char *)AGENTMCAST, AGENTSENDPORT, usectimeo);
    if (iretn)
    {
        json_destroy(cdata);
        return nullptr;
    }

    // Finally, make copies
    if ((iretn=json_clone(cdata)) != 0)
    {
        json_destroy(cdata);
        return(NULL);
    }

    cdata[0].agent[0].client = 1;
    strncpy(cdata[0].agent[0].beat.node, cdata[0].node.name ,COSMOS_MAX_NAME);
    cdata[0].agent[0].beat.ntype = ntype;
    cdata[1].agent[0].client = 1;
    strncpy(cdata[1].agent[0].beat.node, cdata[0].node.name ,COSMOS_MAX_NAME);
    cdata[1].agent[0].beat.ntype = ntype;
    return (cdata);
}

//! Prepare Agent client with 1 msec timeout.
/*! This is the first function to call when setting up a program as an Agent client. It establishes the
 * Subscription channel for collecting messages and sets up the Name Space for the requested Node. This
 * version sets the response timeout to 1 msec.
    \param ntype Type of network channel to use, taken from ::AGENT_TYPE.
    \param node Name of Node.
    \return Pointer to ::cosmosstruc to be used for all other calls, otherwise NULL.
*/
cosmosstruc *agent_setup_client(NetworkType ntype, std::string node)
{
    cosmosstruc *cdata;

    if ((cdata=agent_setup_client(ntype, node, 1000)) == nullptr)
    {
        return (nullptr);
    }

    return (cdata);
}

//! Prepare Agent server
/*! This is the first function to call when setting up an Agent. It first sets the Agent up as
 * a client, and checks that no other copies of the Agent are running. It then establishes the name
 * and connection information for the Agent, the frequency of the heartbeat, and the
 * size of the largest block that can be transferred. It initializes the built in
 * requests and makes it so the user can add their own requests.
    \param ntype The type of network medium.
    \param node The Node this Agent is associated with.
    \param name The name of the Agent.
    \param bprd The period of the heartbeat loop in sec.
    \param port The port number for contacting the Agent.
    \param bsize Size of transfer buffer.
    \return Pointer to ::cosmosstruc, otherwise NULL.
*/
cosmosstruc *agent_setup_server(NetworkType ntype, std::string node, std::string name, double bprd, int32_t port, uint32_t bsize)
{
    return (agent_setup_server(ntype, node, name, bprd, port, bsize, AGENT_SINGLE));
}

//! Prepare Agent server
/*! This is the first function to call when setting up an Agent. It first sets the Agent up as
 * a client, and checks that no other copies of the Agent are running. It then establishes the name
 * and connection information for the Agent, the frequency of the heartbeat, and the
 * size of the largest block that can be transferred. It initializes the built in
 * requests and makes it so the user can add their own requests.
    \param ntype The type of network medium.
    \param node The Node this Agent is associated with.
    \param name The name of the Agent.
    \param bprd The period of the heartbeat loop in sec.
    \param port The port number for contacting the Agent.
    \param bsize Size of transfer buffer.
    \param multiflag Boolean for whether to start multiple copies.
    \return Pointer to ::cosmosstruc, otherwise NULL.
*/
cosmosstruc *agent_setup_server(NetworkType ntype, std::string node, std::string name, double bprd, int32_t port, uint32_t bsize, bool multiflag)
{
    cosmosstruc *cdata;

    //! First, see if we can become a Client, as all Servers are also Clients.
    if ((cdata = agent_setup_client(ntype, node, 1000)) == NULL)
    {
        return nullptr;
    }

    return agent_setup_server(cdata, name, bprd, port, bsize, multiflag, 4);
}

//! Prepare Agent server with default values (overloaded)
/*! This is the first function to call when setting up an Agent. It first sets the Agent up as
 * a client, and checks that no other copies of the Agent are running. It then establishes the name
 * and connection information for the Agent, the frequency of the heartbeat, and the
 * size of the largest block that can be transferred. It initializes the built in
 * requests and makes it so the user can add their own requests.
    \param nodename The Node this Agent is associated with.
    \param agentname The name of the Agent.
    \return Pointer to ::cosmosstruc, otherwise NULL.
*/
cosmosstruc *agent_setup_server(std::string nodename, std::string agentname)
{
    cosmosstruc *cdata;

    //! First, see if we can become a Client, as all Servers are also Clients.
    if ((cdata = agent_setup_client(NetworkType::UDP, nodename, 1000)) == NULL)
    {
        return nullptr;
    }

    // run setup server with default values
    double   beat_period = 1;
    int32_t  port        = 0;
    uint32_t buffer_size = 1000;
    bool     multiflag   = 0;
    float    timeoutSec  = 1;

    return agent_setup_server(cdata, agentname, beat_period, port, buffer_size, multiflag, timeoutSec);
}

//! Prepare Agent server
/*! This is the first function to call when setting up an Agent. It establishes the name
 * and connection information for the Agent, the frequency of the heartbeat, and the
 * size of the largest block that can be transferred. It also initializes the buit in
 * requests and makes it so the user can add their own requests. It
 * also initializes a ::cosmosstruc, and the Data Space associated with
 * it. If the multiflag is set to false, then if another Agent of the same Node
 * and Name is discovered, the Agent will  not start and an error will be
 * returned. If multiflag is set to true, a 3 digit number of the form
 * "_ddd" will be appended to the name. The routine will then keep
 * trying until it finds no conflict.
    \param cdata Pointer to ::cosmosstruc previously created with call to ::agent_setup_client.
    \param name The name of the Agent.
    \param bprd The period of the heartbeat loop in sec.
    \param port The port number for contacting the Agent.
    \param bsize Size of transfer buffer.
    \param multiflag Boolean for whether to start multiple copies.
    \param timeoutSec Time, in seconds, to listen for other instances of same agent.
    \return Pointer to ::cosmosstruc, otherwise NULL.
*/
cosmosstruc* agent_setup_server(cosmosstruc* cdata, std::string name, double bprd, int32_t port, uint32_t bsize, bool multiflag, float timeoutSec = 4)
{
    int32_t iretn;
    char tname[COSMOS_MAX_NAME+1];

    //! Next, check if this Agent is already running
    if (!multiflag)
    {
        if (strlen(cdata[0].node.name)>COSMOS_MAX_NAME || name.size()>COSMOS_MAX_NAME || agent_get_server(cdata, cdata[0].node.name, name, timeoutSec, (beatstruc *)NULL))
        {
            json_destroy(cdata);
            return nullptr;
        }
        strcpy(tname,name.c_str());
    }
    else // then there is an agent running with the given name, so let's make the name unique
    {
        if (strlen(cdata[0].node.name)>COSMOS_MAX_NAME-4 || name.size()>COSMOS_MAX_NAME-4)
        {
            json_destroy(cdata);
            return nullptr;
        }

        uint32_t i=0;
        do
        {
            sprintf(tname,"%s_%03d",name.c_str(),i);
            if (!agent_get_server(cdata, cdata[0].node.name, tname, timeoutSec, (beatstruc *)NULL))
            {
                break;
            }
        } while (++i<100);
    }

    // Initialize important server variables

    strncpy(cdata[0].agent[0].beat.node, cdata[0].node.name, COSMOS_MAX_NAME);
    strncpy(cdata[0].agent[0].beat.proc, tname, COSMOS_MAX_NAME);
    //	cdata[0].agent[0].beat.ntype = ntype;
    if (bprd >= .1)
        cdata[0].agent[0].beat.bprd = bprd;
    else
        cdata[0].agent[0].beat.bprd = .1;
    cdata[0].agent[0].stateflag = (uint16_t)AGENT_STATE_INIT;
    cdata[0].agent[0].beat.port = (uint16_t)port;
    cdata[0].agent[0].beat.bsz = (bsize<=AGENTMAXBUFFER-4?bsize:AGENTMAXBUFFER-4);

#ifdef COSMOS_WIN_BUILD_MSVC
    cdata[0].agent[0].pid = _getpid();
#else
    cdata[0].agent[0].pid = getpid();
#endif
    cdata[0].agent[0].aprd = 1.;
    strncpy(cdata[0].agent[0].beat.user, "cosmos", COSMOS_MAX_NAME);
    //	cdata[0].agent[0].sohstring[0] = 0;

    // Establish publish channel
    iretn = agent_publish(cdata, cdata[0].agent[0].beat.ntype, AGENTSENDPORT);
    if (iretn)
    {
        agent_unsubscribe(cdata);
        json_destroy(cdata);
        return nullptr;
    }

    // Start the heartbeat and request threads running
    iretn = agent_start(cdata);
    if (iretn)
    {
        agent_unsubscribe(cdata);
        agent_unpublish(cdata);
        json_destroy(cdata);
        return nullptr;
    }

    //! Set up initial requests
    {
        agent_request_entry tentry = {"help",agent_req_help,"","list of available requests for this agent"};
        cdata[0].agent[0].reqs.push_back(tentry);
    }
    {
        agent_request_entry tentry = {"shutdown",agent_req_shutdown,"","request to shutdown this agent"};
        cdata[0].agent[0].reqs.push_back(tentry);
    }
    {
        agent_request_entry tentry = {"idle",agent_req_idle,"","request to transition this agent to idle state"};
        cdata[0].agent[0].reqs.push_back(tentry);
    }
    {
        agent_request_entry tentry = {"run",agent_req_run,"","request to transition this agent to run state"};
        cdata[0].agent[0].reqs.push_back(tentry);
    }
    {
        agent_request_entry tentry = {"status",agent_req_status,"","request the status of this agent"};
        cdata[0].agent[0].reqs.push_back(tentry);
    }
    {
        agent_request_entry tentry = {"getvalue",agent_req_getvalue,"{\"name1\",\"name2\",...}","get specified value(s) from agent"};
        cdata[0].agent[0].reqs.push_back(tentry);
    }
    {
        agent_request_entry tentry = {"setvalue",agent_req_setvalue,"{\"name1\":value},{\"name2\":value},...}","set specified value(s) in agent"};
        cdata[0].agent[0].reqs.push_back(tentry);
    }
    {
        agent_request_entry tentry = {"listnames",agent_req_listnames,"","list the Namespace of the agent"};
        cdata[0].agent[0].reqs.push_back(tentry);
    }
    {
        agent_request_entry tentry = {"forward",agent_req_forward,"nbytes packet","Broadcast JSON packet to the default SEND port on local network"};
        cdata[0].agent[0].reqs.push_back(tentry);
    }
    {
        agent_request_entry tentry = {"echo",agent_req_echo,"utc crc nbytes bytes","echo array of nbytes bytes, sent at time utc, with CRC crc."};
        cdata[0].agent[0].reqs.push_back(tentry);
    }

    cdata[0].agent[0].server = 1;
    cdata[0].agent[0].stateflag = (uint16_t)AGENT_STATE_RUN;
    return (cdata);
}

//! Shutdown server gracefully
/*! Waits for heartbeat and request loops to stop running before pulling the rug out from under them.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0 or negative error.
 */
int32_t agent_shutdown_server(cosmosstruc *cdata)
{
    cdata[0].agent[0].stateflag = AGENT_STATE_SHUTDOWN;;
    hthread.join();
    cthread.join();
    agent_unsubscribe(cdata);
    agent_unpublish(cdata);
    json_destroy(cdata);
    return 0;
}

//! Shutdown client gracefully
/*! Closes an open network connections and frees up memory so that
 * you can setup again as a different Node.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0 or negative error.
 */
int32_t agent_shutdown_client(cosmosstruc *cdata)
{
    agent_unsubscribe(cdata);
    json_destroy(cdata);
    return 0;
}

//! Check if we're supposed to be running
/*!	Returns the value of the internal variable that indicates that
 * the threads are running.
    \param cdata Pointer to ::cosmosstruc to use.
    \return Value of internal state variable, as enumerated in ::AGENT_STATE.
*/
uint16_t agent_running(cosmosstruc *cdata)
{
    return (cdata[0].agent[0].stateflag);
}

//! Send a request over AGENT
/*! Send a request string to the process at the provided address
    \param hbeat The agent ::beatstruc
    \param request the request and its arguments
    \param output any output returned by the request
    \param clen the size of the return buffer
    \param waitsec Maximum number of seconds to wait
    \return Either the number of bytes returned, or an error number.
*/
int32_t agent_send_request(beatstruc hbeat, std::string request, char* output, uint32_t clen, float waitsec)
{
    static socket_channel sendchan;
    int32_t iretn;
    int32_t nbytes;
    output[0] = '\0'; // reset the output

    if (hbeat.utc == 0. || hbeat.addr == 0 || hbeat.port == 0)
        return (AGENT_ERROR_SOCKET);

    ElapsedTime ep;
    ep.start();

    if ((iretn=socket_open(&sendchan, NetworkType::UDP, hbeat.addr, hbeat.port, AGENT_TALK, AGENT_BLOCKING, AGENTRCVTIMEO)) < 0)
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

        nbytes = recvfrom(sendchan.cudp,output,clen,0,(struct sockaddr *)NULL,(socklen_t *)NULL);

    } while ( (nbytes <= 0) && (ep.split() <= waitsec) );

    output[nbytes] = 0;

    CLOSE_SOCKET(sendchan.cudp);

    return (nbytes);
}

//! Get specific server.
/*! Listen to the multicast/broadcast traffic for a set amount of time,
 * waiting for a specific named server to appear, then return its
 * heartbeat.
    \param cdata Pointer to ::cosmosstruc to use.
    \param node Node for the server.
    \param name Name of the server.
    \param waitsec Maximum number of seconds to wait
    \param rbeat pointer to a location to store the heartbeat
    \return 1 if found, otherwise 0, or an error number
*/
int32_t agent_get_server(cosmosstruc *cdata, std::string node, std::string name, float waitsec, beatstruc *rbeat)
{
    beatstruc cbeat;

    //! 3. Loop for ::waitsec seconds, filling list with any discovered heartbeats.

    ElapsedTime ep;
    ep.start();

    do
    {
        cbeat = agent_poll_beat(cdata, 1);

        if (!strcmp(cbeat.proc,name.c_str()) && !strcmp(cbeat.node,node.c_str()))
        {
            if (rbeat != NULL)
                *rbeat = cbeat;
            return (1);
        }

        if (!strcmp(cbeat.proc,name.c_str()) && !strcmp(cbeat.node,node.c_str()))
        {
            if (rbeat != NULL)
                *rbeat = cbeat;
            return (1);
        }

    } while (ep.split() <= waitsec);

    return(0);
}

//! Find single server
/*! Listen to the local subnet for a set amount of time,
 * collecting heartbeats, searching for a particular agent.
    \param cdata Pointer to ::cosmosstruc to use.
    \param node Node that agent is in.
    \param proc Name of agent.
    \param waitsec Maximum number of seconds to wait.
    \return ::beatstruc of located agent, otherwise empty ::beatstruc.
 */
beatstruc agent_find_server(cosmosstruc *cdata, std::string node, std::string proc, float waitsec)
{
    beatstruc cbeat = {0.,"","",NetworkType::MULTICAST,"",0,0,0.,"",0.,0.,0.};

    //! Loop for ::waitsec seconds, looking for desired agent.

    ElapsedTime ep;
    ep.start();

    do
    {
        cbeat = agent_poll_beat(cdata, 1);
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
    \param cdata Pointer to ::cosmosstruc to use.
    \param waitsec Maximum number of seconds to wait.
    \return A vector of ::beatstruc entries listing the unique servers found.
*/
std::vector<beatstruc> agent_find_servers(cosmosstruc *cdata, float waitsec)
{
    beatstruc cbeat, tbeat;
    uint32_t i, j;

    //! Loop for ::waitsec seconds, filling list with any discovered heartbeats.

    ElapsedTime ep;
    ep.start();

    do
    {
        cbeat = agent_poll_beat(cdata, 1);
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
    \param cdata Pointer to ::cosmosstruc to use.
    \param list Properly formatted list of JSON names.
    \return 0, otherwise a negative error.
*/
int32_t agent_set_sohstring(cosmosstruc *cdata, std::string list)
{

    if (!cdata[0].agent[0].sohtable.empty())
    {
        cdata[0].agent[0].sohtable.clear();
    }

    json_table_of_list(cdata[0].agent[0].sohtable, list, cdata);
    return 0;
}

//! Return Agent ::cosmosstruc
/*! Return a pointer to the Agent's internal copy of the ::cosmosstruc.
    \param cdata Pointer to ::cosmosstruc to use.
    \return A pointer to the ::cosmosstruc, otherwise NULL.
*/
cosmosstruc *agent_get_cosmosstruc(cosmosstruc *cdata)
{
    return (cdata);
}

//! Heartbeat Loop
/*! This function is run as a thread to provide the Heartbeat for the Agent. The Heartbeat will
 * consist of the contents of ::AGENT_MESSAGE_BEAT in ::agent_poll, plus the contents of the
 * ::sohstring. It will come every ::bprd seconds.
 * \param cdata Pointer to ::cosmosstruc to use.
 */
void heartbeat_loop(cosmosstruc *cdata)
{
    ElapsedTime ep;

    while (((cosmosstruc *)cdata)->agent[0].stateflag)
    {

        // compute the jitter
        ((cosmosstruc *)cdata)->agent[0].beat.jitter = ep.split() - ((cosmosstruc *)cdata)->agent[0].beat.bprd;
        ((cosmosstruc *)cdata)->agent[0].beat.cpu = 1;
        ((cosmosstruc *)cdata)->agent[0].beat.memory = 2;

        ep.start();

        ((cosmosstruc *)cdata)->agent[0].beat.utc = currentmjd(0.);
        if (((cosmosstruc*)cdata)->agent[0].stateflag != AGENT_STATE_IDLE && !((cosmosstruc*)cdata)->agent[0].sohtable.empty())
        {
            agent_post(((cosmosstruc *)cdata), AGENT_MESSAGE_BEAT, json_of_table(hbjstring, ((cosmosstruc *)cdata)->agent[0].sohtable, ((cosmosstruc *)cdata)));
        }
        else
        {
            agent_post(((cosmosstruc *)cdata), AGENT_MESSAGE_BEAT,"");
        }

        if (((cosmosstruc *)cdata)->agent[0].beat.bprd < .1)
        {
            ((cosmosstruc *)cdata)->agent[0].beat.bprd = .1;
        }

        if (ep.split() <= ((cosmosstruc *)cdata)->agent[0].beat.bprd)
        {
            COSMOS_SLEEP(((cosmosstruc *)cdata)->agent[0].beat.bprd - ep.split());
        }
    }
    agent_unpublish(((cosmosstruc *)cdata));
}

//! Request Loop
/*! This function is run as a thread to service requests to the Agent. It receives requests on
 * it assigned port number, matches the first word of the request against its set of requests,
 * and then either performs the matched function, or returns [NOK].
 * \param cdata Pointer to ::cosmosstruc to use.
 */
void request_loop(cosmosstruc *cdata)
{
    char ebuffer[6]="[NOK]";
    int32_t iretn, nbytes;
    char *bufferin, *bufferout;
    char request[AGENTMAXBUFFER+1];
    uint32_t i;

    if ((iretn=socket_open(&((cosmosstruc *)cdata)->agent[0].req,NetworkType::UDP,(char *)"",((cosmosstruc *)cdata)->agent[0].beat.port,AGENT_LISTEN,AGENT_BLOCKING,2000000)) < 0)
    {
        return;
    }

    ((cosmosstruc *)cdata)->agent[0].beat.port = ((cosmosstruc *)cdata)->agent[0].req.cport;

    if ((bufferin=(char *)calloc(1,((cosmosstruc *)cdata)->agent[0].beat.bsz)) == NULL)
    {
        iretn = -errno;
        return;
    }
    //	((cosmosstruc *)cdata)->agent[0].req.addrlen = sizeof(struct sockaddr_in);

    while (((cosmosstruc *)cdata)->agent[0].stateflag)
    {
        iretn = recvfrom(((cosmosstruc *)cdata)->agent[0].req.cudp,bufferin,((cosmosstruc *)cdata)->agent[0].beat.bsz,0,(struct sockaddr *)&((cosmosstruc *)cdata)->agent[0].req.caddr,(socklen_t *)&((cosmosstruc *)cdata)->agent[0].req.addrlen);

        if (iretn > 0)
        {
            bufferin[iretn] = 0;

            if (((cosmosstruc *)cdata)->agent[0].stateflag == AGENT_STATE_DEBUG)
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

            for (i=0; i<((cosmosstruc *)cdata)->agent[0].reqs.size(); i++)
            {
                if (!strcmp(request,((cosmosstruc *)cdata)->agent[0].reqs[i].token.c_str()))
                    break;
            }

            if (i < ((cosmosstruc *)cdata)->agent[0].reqs.size())
            {
                iretn = ((cosmosstruc *)cdata)->agent[0].reqs[i].function(bufferin,request,((cosmosstruc *)cdata));
                if (iretn >= 0)
                    bufferout = (char *)&request;
                else
                    bufferout = NULL;
            }
            else
            {
                iretn = AGENT_ERROR_NULL;
                bufferout = NULL;
            }

            if (bufferout == NULL)
            {
                bufferout = ebuffer;
            }
            else
            {
                strcat(bufferout,"[OK]");
                bufferout[((cosmosstruc *)cdata)->agent[0].beat.bsz+3] = 0;
            }
            nbytes = sendto(((cosmosstruc *)cdata)->agent[0].req.cudp,bufferout,strlen(bufferout),0,(struct sockaddr *)&((cosmosstruc *)cdata)->agent[0].req.caddr,sizeof(struct sockaddr_in));
            if (((cosmosstruc *)cdata)->agent[0].stateflag == AGENT_STATE_DEBUG)
            {
                printf("[%d] %s\n",nbytes,bufferout);
            }
        }
    }
    free(bufferin);
    return;
}

//! Built-in Forward request
/*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_forward(char* request, char* output, void *cdata)
{
    uint16_t count;
    int32_t iretn=-1;

    sscanf(request,"%*s %hu",&count);
    for (uint16_t i=0; i<((cosmosstruc *)cdata)->agent[0].ifcnt; ++i)
    {
        iretn = sendto(((cosmosstruc *)cdata)->agent[0].pub[i].cudp,(const char *)&request[strlen(request)-count],count,0,(struct sockaddr *)&((cosmosstruc *)cdata)->agent[0].pub[i].baddr,sizeof(struct sockaddr_in));
    }
    sprintf(output,"%.17g %d ",currentmjd(0),iretn);
    return(0);
}

//! Built-in Echo request
/*! Returns the received packet, reaclculating the CRC, and adding the time.
 * \param request Text of echo packet.
 * \param output Text of echoed packet.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_echo(char* request, char* output, void *)
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
 * \param output Text of response to request.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_help(char*, char* output, void *cdata)
{
    std::string help_string;
    help_string += "\n";
    for(uint32_t i = 0; i < ((cosmosstruc *)cdata)->agent[0].reqs.size(); ++i)
    {
        help_string += "        ";
        help_string += ((cosmosstruc *)cdata)->agent[0].reqs[i].token;
        help_string += " ";
        help_string += ((cosmosstruc *)cdata)->agent[0].reqs[i].synopsis;
        help_string += "\n";
        //size_t blanks = (20 - (signed int)strlen(((cosmosstruc *)cdata)->agent[0].reqs[i].token)) > 0 ? 20 - strlen(((cosmosstruc *)cdata)->agent[0].reqs[i].token) : 4;
        //string blank(blanks,' ');
        //help_string += blank;
        help_string += "                ";
        help_string += ((cosmosstruc *)cdata)->agent[0].reqs[i].description;
        help_string += "\n\n";
    }
    help_string += "\n";
    strcpy(output, (char*)help_string.c_str());
    return 0;
}

//! Built-in Set state to Run request
/*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param output Text of response to request.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_run(char*, char* output, void *cdata)
{
    ((cosmosstruc *)cdata)->agent[0].stateflag = AGENT_STATE_RUN;
    output[0] = 0;
    return(0);
}

//! Built-in Set state to Idle request
/*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param output Text of response to request.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_idle(char*, char* output, void *cdata)
{
    ((cosmosstruc *)cdata)->agent[0].stateflag = AGENT_STATE_IDLE;
    output[0] = 0;
    return(0);
}

//! Built-in Set state to Shutdown request
/*! Resends the received request, less count bytes, to all Publication channels of the Agent.
 * \param output Text of response to request.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_shutdown(char*, char* output, void *cdata)
{
    ((cosmosstruc *)cdata)->agent[0].stateflag = AGENT_STATE_SHUTDOWN;
    output[0] = 0;
    return(0);
}

// TODO: add a line break (\n) when printing the data
// this makes it easier to read
//! Built-in Status request
/*! Returns agent status.
 * \param output Text of response to request.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_status(char*, char* output, void *cdata)
{
    std::string jstring;

    if (json_of_agent(jstring, (cosmosstruc*)cdata) != NULL)
    {
        strncpy(output, jstring.c_str(),((cosmosstruc *)cdata)->agent[0].beat.bsz);
        output[((cosmosstruc *)cdata)->agent[0].beat.bsz-1] = 0;
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
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_getvalue(char *request, char* output, void *cdata)
{
    std::string jstring;

    if (json_of_list(jstring, request, ((cosmosstruc *)cdata)) != NULL)
    {
        strncpy(output, jstring.c_str(), ((cosmosstruc *)cdata)->agent[0].beat.bsz);
        output[((cosmosstruc *)cdata)->agent[0].beat.bsz-1] = 0;
        return 0;
    }
    else
        return (JSON_ERROR_EOS);
}

//! Built-in Set Internal Value request
/*! Sets the current value of the requested Name Space values. Names and values are expressed as a JSON object.
 * \param request Text of request.
 * \param output Text of response to request.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_setvalue(char *request, char* output, void *cdata)
{
    int32_t iretn;
    iretn = json_parse(request,((cosmosstruc *)cdata));

    sprintf(output,"%d",iretn);

    return(iretn);
}

//! Built-in List Name Space Names request
/*! Returns a list of all names in the JSON Name Space.
 * \param output Text of response to request.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \return 0, or negative error.
 */
int32_t agent_req_listnames(char *, char* output, void *cdata)
{
    //    UNUSED_VARIABLE_LOCALDEF(request);  // Unused: Assumed already checked by calling function, no parameters

    std::string result = json_list_of_all((cosmosstruc *)cdata);
    strncpy(output, result.c_str(), ((cosmosstruc *)cdata)->agent[0].beat.bsz);
    output[((cosmosstruc *)cdata)->agent[0].beat.bsz-1] = 0;
    return 0;
}

//! Open COSMOS output channel
/*! Establish a multicast socket for publishing COSMOS messages using the specified address and
 * port.
 * \param cdata Pointer to ::cosmosstruc to use.
 * \param type One of ::NetworkType.
 * \param port Port number to publish on.
 * \return 0, otherwise negative error.
*/
int32_t agent_publish(cosmosstruc *cdata, NetworkType type, uint16_t port)
{
#ifdef COSMOS_WIN_OS
#else
#endif // COSMOS_WIN_OS
    int32_t iretn;
    int on = 1;
    //uint32_t ip, net, bcast;

    // Return immediately if we've already done this
    if (cdata[0].agent[0].pub[0].cport)
        return 0;

    switch (type)
    {
    case NetworkType::MULTICAST:
    case NetworkType::UDP:
        {
            for (uint32_t i=0; i<AGENTMAXIF; i++)
                cdata[0].agent[0].pub[i].cudp = -1;

            if ((cdata[0].agent[0].pub[0].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
            {
                return (AGENT_ERROR_SOCKET);
            }

            // Use above socket to find available interfaces and establish
            // publication on each.
            cdata[0].agent[0].ifcnt = 0;

#if defined(COSMOS_WIN_OS)
            struct sockaddr_storage ss;
            int sslen;
            INTERFACE_INFO ilist[20];
            unsigned long nbytes;
            uint32_t nif;
            if (WSAIoctl(cdata[0].agent[0].pub[0].cudp, SIO_GET_INTERFACE_LIST, 0, 0, &ilist,sizeof(ilist), &nbytes, 0, 0) == SOCKET_ERROR)
            {
                CLOSE_SOCKET(cdata[0].agent[0].pub[0].cudp);
                return (AGENT_ERROR_DISCOVERY);
            }

            nif = nbytes / sizeof(INTERFACE_INFO);
            for (uint32_t i=0; i<nif; i++)
            {
                inet_ntop(ilist[i].iiAddress.AddressIn.sin_family,&ilist[i].iiAddress.AddressIn.sin_addr,cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].address,sizeof(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].address));
                //            strcpy(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].address,inet_ntoa(((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr));
                if (!strcmp(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].address,"127.0.0.1"))
                {
                    if (cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp >= 0)
                    {
                        CLOSE_SOCKET(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp);
                        cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp = -1;
                    }
                    continue;
                }
                // No need to open first socket again
                if (cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp < 0)
                {
                    if ((cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                    {
                        continue;
                    }
                }

                memset(&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr,0,sizeof(struct sockaddr_in));
                cdata[0].agent[0].pub[i].caddr.sin_family = AF_INET;
                cdata[0].agent[0].pub[i].baddr.sin_family = AF_INET;
                if (type == NetworkType::MULTICAST)
                {
                    sslen = sizeof(ss);
                    WSAStringToAddressA((char *)AGENTMCAST,AF_INET,NULL,(struct sockaddr*)&ss,&sslen);
                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_addr = ((struct sockaddr_in *)&ss)->sin_addr;
                }
                else
                {
                    if ((iretn = setsockopt(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                    {
                        CLOSE_SOCKET(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp);
                        continue;
                    }

                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;
                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_addr = ((struct sockaddr_in *)&ilist[i].iiAddress)->sin_addr;

                    uint32_t ip, net, bcast;
                    ip = ((struct sockaddr_in*)&(ilist[i].iiAddress))->sin_addr.S_un.S_addr;
                    net = ((struct sockaddr_in*)&(ilist[i].iiNetmask))->sin_addr.S_un.S_addr;
                    bcast = ip | (~net);
                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_addr.S_un.S_addr = bcast;
                }
                cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr.sin_port = htons(port);
                cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_port = htons(port);
                cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].type = type;
                cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cport = port;
                cdata[0].agent[0].ifcnt++;
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
                    inet_ntop(if_addr->ifa_addr->sa_family,&((struct sockaddr_in*)if_addr->ifa_addr)->sin_addr,cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].address,sizeof(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].address));
                    memcpy((char *)&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr, (char *)if_addr->ifa_addr, sizeof(if_addr->ifa_addr));

                    if ((if_addr->ifa_flags & IFF_POINTOPOINT) || (if_addr->ifa_flags & IFF_UP) == 0 || (if_addr->ifa_flags & IFF_LOOPBACK) || (if_addr->ifa_flags & (IFF_BROADCAST)) == 0)
                    {
                        continue;
                    }

                    // No need to open first socket again
                    if (cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp < 0)
                    {
                        if ((cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                        {
                            continue;
                        }
                    }

                    if (type == NetworkType::MULTICAST)
                    {
                        inet_pton(AF_INET,AGENTMCAST,&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr.sin_addr);
                        inet_pton(AF_INET,AGENTMCAST,&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_addr);
                    }
                    else
                    {
                        if ((iretn = setsockopt(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                        {
                            CLOSE_SOCKET(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp);
                            continue;
                        }

                        //                    if (ioctl(cdata[0].agent[0].pub[0].cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                        //                    {
                        //                        continue;
                        //                    }
                        //                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr = cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr;
                        memcpy((char *)&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr, (char *)if_addr->ifa_netmask, sizeof(if_addr->ifa_netmask));

                        uint32_t ip, net, bcast;
                        ip = cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr.sin_addr.s_addr;
                        net = cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_addr.s_addr;
                        bcast = ip | (~net);
                        cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_addr.s_addr = bcast;
                        inet_ntop(if_addr->ifa_netmask->sa_family,&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_addr,cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddress,sizeof(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddress));
                    }
                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr.sin_port = htons(port);
                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_port = htons(port);
                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].type = type;
                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cport = port;
                    cdata[0].agent[0].ifcnt++;
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
            if (ioctl(cdata[0].agent[0].pub[0].cudp,SIOCGIFCONF,&confa) < 0)
            {
                CLOSE_SOCKET(cdata[0].agent[0].pub[0].cudp);
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
                inet_ntop(ifra->ifr_addr.sa_family,&((struct sockaddr_in*)&ifra->ifr_addr)->sin_addr,cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].address,sizeof(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].address));
                memcpy((char *)&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr, (char *)&ifra->ifr_addr, sizeof(ifra->ifr_addr));

                if (ioctl(cdata[0].agent[0].pub[0].cudp,SIOCGIFFLAGS, (char *)ifra) < 0) continue;

                if ((ifra->ifr_flags & IFF_POINTOPOINT) || (ifra->ifr_flags & IFF_UP) == 0 || (ifra->ifr_flags & IFF_LOOPBACK) || (ifra->ifr_flags & (IFF_BROADCAST)) == 0)
                {
                    continue;
                }

                // No need to open first socket again
                if (cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp < 0)
                {
                    if ((cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp=socket(AF_INET,SOCK_DGRAM,0)) < 0)
                    {
                        continue;
                    }
                }

                if (type == NetworkType::MULTICAST)
                {
                    inet_pton(AF_INET,AGENTMCAST,&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr.sin_addr);
                    inet_pton(AF_INET,AGENTMCAST,&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_addr);
                }
                else
                {
                    if ((iretn = setsockopt(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp,SOL_SOCKET,SO_BROADCAST,(char*)&on,sizeof(on))) < 0)
                    {
                        CLOSE_SOCKET(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cudp);
                        continue;
                    }

                    if (ioctl(cdata[0].agent[0].pub[0].cudp,SIOCGIFBRDADDR,(char *)ifra) < 0)
                    {
                        continue;
                    }
                    cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr = cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr;
                    inet_ntop(ifra->ifr_broadaddr.sa_family,&((struct sockaddr_in*)&ifra->ifr_broadaddr)->sin_addr,cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddress,sizeof(cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddress));
                    inet_pton(AF_INET,cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddress,&cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_addr);
                }
                cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].caddr.sin_port = htons(port);
                cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].baddr.sin_port = htons(port);
                cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].type = type;
                cdata[0].agent[0].pub[cdata[0].agent[0].ifcnt].cport = port;
                cdata[0].agent[0].ifcnt++;
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
std::vector<socket_channel> agent_find_addresses(NetworkType ntype)
{
    std::vector<socket_channel> iface;
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

//! Post a JSON message
/*! Post a JSON string on the previously opened publication channel.
 * \param cdata Pointer to ::cosmosstruc to use.
    \param type A byte indicating the type of message.
    \param message A NULL terminated JSON text string to post.
    \return 0, otherwise negative error.
*/
int32_t agent_post(cosmosstruc *cdata, uint8_t type, std::string message)
{
    size_t nbytes;
    int32_t i, iretn=0;
    uint8_t post[AGENTMAXBUFFER];

    cdata[0].agent[0].beat.utc = cdata[0].agent[0].beat.utc;
    post[0] = type;
    // this will broadcast messages to all external interfaces (ifcnt = interface count)
    for (i=0; i<cdata[0].agent[0].ifcnt; i++)
    {
        sprintf((char *)&post[3],"{\"agent_utc\":%.15g}{\"agent_node\":\"%s\"}{\"agent_proc\":\"%s\"}{\"agent_addr\":\"%s\"}{\"agent_port\":%u}{\"agent_bsz\":%u}{\"agent_cpu\":%f}{\"agent_memory\":%f}{\"agent_jitter\":%f}{\"node_utcoffset\":%.15g}",
                cdata[0].agent[0].beat.utc,
                cdata[0].agent[0].beat.node,
                cdata[0].agent[0].beat.proc,
                cdata[0].agent[0].pub[i].address,
                cdata[0].agent[0].beat.port,
                cdata[0].agent[0].beat.bsz,
                cdata[0].agent[0].beat.cpu,
                cdata[0].agent[0].beat.memory,
                cdata[0].agent[0].beat.jitter,
                cdata[0].node.utcoffset);
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
        iretn = sendto(cdata[0].agent[0].pub[i].cudp,       // socket
                (const char *)post,                         // buffer to send
                nbytes+message.size(),                      // size of buffer
                0,                                          // flags
                (struct sockaddr *)&cdata[0].agent[0].pub[i].baddr, // socket address
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
 * \param cdata Pointer to ::cosmosstruc to use.
    \return 0, otherwise negative error.
    */
int32_t agent_unpublish(cosmosstruc *cdata)
{
    int i;

    for (i=0; i<cdata[0].agent[0].ifcnt; i++)
        CLOSE_SOCKET(cdata[0].agent[0].pub[i].cudp);
    return 0;
}

//! Open COSMOS channel for polling
/*! This establishes a multicast channel for subscribing to COSMOS messages.
    \param cdata Pointer to ::cosmosstruc to use.
    \param type 0=Multicast, 1=Broadcast UDP, 2=Broadcast CSP.
    \param address The IP Multicast address of the channel.
    \param port The port to use for the channel.
    \param usectimeo Blocking read timeout in micro seconds.
    \return 0, otherwise negative error.
*/
int32_t agent_subscribe(cosmosstruc *cdata, NetworkType type, char *address, uint16_t port, uint32_t usectimeo)
{
    int32_t iretn = 0;

    // ?? this is preventing from running socket_open if
    // for some reason cdata[0].agent[0].sub.cport was ill initialized
#ifndef COSMOS_WIN_BUILD_MSVC
    if (cdata[0].agent[0].sub.cport)
        return 0;
#endif
    if ((iretn=socket_open(&cdata[0].agent[0].sub,type,address,port,AGENT_LISTEN,AGENT_BLOCKING, usectimeo)) < 0)
    {
        return (iretn);
    }

    return 0;
}

//! Open COSMOS channel for polling with 100 usec timeout.
/*! This establishes a multicast channel for subscribing to COSMOS messages. The timeout is set to
 * 100 usec.
    \param cdata Pointer to ::cosmosstruc to use.
    \param type 0=Multicast, 1=Broadcast UDP, 2=Broadcast CSP.
    \param address The IP Multicast address of the channel.
    \param port The port to use for the channel.
    \return 0, otherwise negative error.
*/
int32_t agent_subscribe(cosmosstruc *cdata, NetworkType type, char *address, uint16_t port)
{
    int32_t iretn = 0;

    if ((iretn=agent_subscribe(cdata, type, address, port, 100)) < 0)
    {
        return (iretn);
    }

    return 0;
}
//! Close COSMOS subscription channel
/*! Close channel previously opened for polling for messages and recover resources.
    \param cdata Pointer to ::cosmosstruc to use.
    \return 0, otherwise negative error.
*/
int32_t agent_unsubscribe(cosmosstruc *cdata)
{
    CLOSE_SOCKET(cdata[0].agent[0].sub.cudp);
    return 0;
}

//! Listen for message
/*! Poll the subscription channel for the requested amount of time. Return as soon as a single message
 * comes in, or the timer runs out.
    \param cdata Pointer to ::cosmosstruc to use.
    \param message String for storing incoming message.
    \param type Type of message to look for, taken from ::AGENT_MESSAGE.
    \param waitsec Number of seconds in timer.
    \return If a message comes in, return its type. If none comes in, return zero, otherwise negative error.
*/
// TODO: instead of type (AGENT_MESSAGE_ALL) replace by header ex: 0xBB
int32_t agent_poll(cosmosstruc *cdata, pollstruc &meta, std::string &message, uint8_t type, float waitsec)
{
    int nbytes;
    uint8_t input[AGENTMAXBUFFER+1];

    if (!cdata[0].agent[0].sub.cport)
        return (AGENT_ERROR_CHANNEL);

    ElapsedTime ep;
    ep.start();
    do
    {
        nbytes = 0;
        switch (cdata[0].agent[0].sub.type)
        {
        case NetworkType::MULTICAST:
        case NetworkType::UDP:

            nbytes = recvfrom(cdata[0].agent[0].sub.cudp,
                    (char *)input,AGENTMAXBUFFER,
                    0,
                    (struct sockaddr *)&cdata[0].agent[0].sub.caddr,
                    (socklen_t *)&cdata[0].agent[0].sub.addrlen);

            // Return if port and address are our own
            for (uint16_t i=0; i<cdata[0].agent[0].ifcnt; ++i)
            {
                if (cdata[0].agent[0].sub.caddr.sin_port == cdata[0].agent[0].pub[i].caddr.sin_port &&
                    cdata[0].agent[0].sub.caddr.sin_addr.s_addr == cdata[0].agent[0].pub[i].caddr.sin_addr.s_addr)
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
                    meta.type = (uint16_t)input[0];
                    meta.jlength = input[1] + 256 * input[2];
                }
                else
                {
                    meta.type = (uint16_t)input[0] + 1;
                    meta.jlength = nbytes;
                }

                // Copy message. Could have binary data, so copy safe way
                if (nbytes > 0)
                {
                    input[nbytes] = 0;
                }
                message.resize(nbytes+1-start_byte);
                memcpy(&message[0], &input[start_byte], nbytes+1-start_byte);

                // Extract meta data
                sscanf(message.c_str(), "{\"agent_utc\":%lg}{\"agent_node\":\"%40[^\"]\"}{\"agent_proc\":\"%40[^\"]\"}{\"agent_addr\":\"%17[^\"]\"}{\"agent_port\":%hu}{\"agent_bsz\":%u}",
                       &meta.beat.utc, meta.beat.node, meta.beat.proc, meta.beat.addr, &meta.beat.port, &meta.beat.bsz);
                return ((int)meta.type);
            }
        }
        if (ep.split() >= waitsec)
        {
            nbytes = 0;
        }
    } while (nbytes != 0);

    return 0;
}

//! Listen for heartbeat
/*! Poll the subscription channel until you receive a heartbeat message, or the timer runs out.
    \param cdata Pointer to ::cosmosstruc to use.
    \param waitsec Number of seconds to wait before timing out.
    \return ::beatstruc with acquired heartbeat. The UTC will be set to 0 if no heartbeat was
    acquired.
*/
beatstruc agent_poll_beat(cosmosstruc *cdata, float waitsec)
{
    int32_t iretn;
    beatstruc beat;
    pollstruc meta;
    std::string message;

    iretn = agent_poll(cdata, meta, message, AGENT_MESSAGE_BEAT, waitsec);

    beat.utc = 0.;
    if (iretn == AGENT_MESSAGE_BEAT)
    {
        beat = meta.beat;
    }

    return (beat);
}

//! Listen for Time
/*! Poll the subscription channel until you receive a time message, or the timer runs out.
    \param cdata Pointer to ::cosmosstruc to use.
    \param waitsec Number of seconds to wait before timing out.
    \return ::timestruc with acquired time. The UTC will be set to 0 if no heartbeat was
    acquired.
*/
timestruc agent_poll_time(cosmosstruc *cdata, float waitsec)
{
    int32_t iretn;
    timestruc time;
    pollstruc meta;
    std::string message;

    iretn = agent_poll(cdata, meta, message, AGENT_MESSAGE_TIME, waitsec);

    if (iretn == AGENT_MESSAGE_TIME)
    {
        iretn = json_parse(message, &cdata[1]);
        if (iretn >= 0)
        {
            time.mjd = cdata[1].node.loc.utc;
        }
    }

    return (time);
}

//! Listen for Location
/*! Poll the subscription channel until you receive a location message, or the timer runs out.
    \param cdata Pointer to ::cosmosstruc to use.
    \param waitsec Number of seconds to wait before timing out.
    \return ::locstruc with acquired location. The UTC will be set to 0 if no heartbeat was
    acquired.
*/
locstruc agent_poll_location(cosmosstruc *cdata, float waitsec)
{
    int32_t iretn;
    locstruc loc;
    pollstruc meta;
    std::string message;

    iretn = agent_poll(cdata, meta, message, AGENT_MESSAGE_LOCATION, waitsec);

    if (iretn == AGENT_MESSAGE_LOCATION)
    {
        iretn = json_parse(message, &cdata[1]);
        if (iretn >= 0)
        {
            loc = cdata[1].node.loc;
        }
    }

    return (loc);
}

//! Listen for Beacon
/*! Poll the subscription channel until you receive a info message, or the timer runs out.
    \param cdata Pointer to ::cosmosstruc to use.
    \param waitsec Number of seconds to wait before timing out.
    \return ::infostruc with acquired info. The UTC will be set to 0 if no info was
    acquired.
*/
nodestruc agent_poll_info(cosmosstruc *cdata, float waitsec)
{
    int32_t iretn;
    //summarystruc info;
    nodestruc info;
    pollstruc meta;
    std::string message;

    iretn = agent_poll(cdata, meta, message, AGENT_MESSAGE_TRACK, waitsec);

    if (iretn == AGENT_MESSAGE_TRACK)
    {
        iretn = json_parse(message, &cdata[1]);
        if (iretn >= 0)
        {
            strcpy(info.name,cdata[1].node.name);
            //			info.utc = cdata[0].node.loc.utc;
            info.loc = cdata[1].node.loc;
            info.powgen = cdata[1].node.powgen;
            info.powuse = cdata[1].node.powuse;
            info.battlev = cdata[1].node.battlev;
        }
    }
    else
        info.loc.utc = 0.;

    return (info);
}

//! Listen for IMU device
/*! Poll the subscription channel until you receive a IMU device message, or the timer runs out.
    \param cdata Pointer to ::cosmosstruc to use.
    \param waitsec Number of seconds to wait before timing out.
    \return ::beatstruc with acquired heartbeat. The UTC will be set to 0 if no heartbeat was
    acquired.
*/
imustruc agent_poll_imu(cosmosstruc *cdata, float waitsec)
{
    int32_t iretn;
    imustruc imu;
    pollstruc meta;
    std::string message;

    iretn = agent_poll(cdata, meta, message, AGENT_MESSAGE_IMU, waitsec);

    if (iretn == AGENT_MESSAGE_IMU)
    {
        iretn = json_parse(message, &cdata[1]);
        if (iretn >= 0)
        {
            imu = *cdata[1].devspec.imu[0];
        }
    }

    return (imu);
}

// default constructor
Agent::Agent()
{

}

// overloaded constructor
Agent::Agent(std::string nodename, std::string agentname)
{
    nodeName = nodename;
    name = agentname;
}

Agent::~Agent()
{
    shutdown();
}

// this function assumes you have initialized the node and agent names
// with Agent::nodename and Agent::agentname
bool Agent::setupServer()
{
    //! First, see if we can become a Client, as all Servers are also Clients.
    if ((cdata = agent_setup_client(NetworkType::UDP, nodeName, 1000)) == NULL)
    {
        std::cout << "Agent setup client failed" << std::endl;
        return nullptr;
    }

    //cdata = agent_setup_server(nodeName, name);
    //cdata = agent_setup_server(NetworkType::UDP, nodeName, beat_period, 0, AGENTSVR_MAXBUF_BYTES, (bool)false);
    cdata = agent_setup_server(cdata, name, beat_period, port, buffer_size, multiflag, timeoutSec);

    // if setup server was not sucessfull
    if (cdata == NULL)
    {
        // TODO: improve error message with a more detailed description
        // of the reason for failure
        std::cout << "Agent setup server failed" << std::endl;
        return false;
    }

    //std::cout << "Agent server is on for " << nodeName << ":" << name << std::endl;

    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "COSMOS AGENT [" <<  name << " on node " << nodeName << "]" << std::endl;
    std::cout << "Version " << version << " built on " <<  __DATE__ << " " << __TIME__ << std::endl;
    //std::cout << "------------------------------------------------------" << std::endl;

    //std::cout << "================================================" << std::endl;
    std::cout << "Agent server is running" << std::endl;

    //std::cout << "================================================" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    // if setup server was sucessfull
    return true;
}

bool Agent::setupServer(std::string nodename, std::string agentname)
{
    nodeName = nodename;
    name = agentname;

    return setupServer();
}


bool Agent::setupClient(std::string nodename)
{
    nodeName = nodename;

    if (!(cdata=agent_setup_client(NetworkType::UDP, nodeName.c_str(), 1000)))
    {
        std::cout << "Couldn't establish client for Node " << nodename << std::endl;
        exit (AGENT_ERROR_NULL);
    }

    return true;
}

//! Check if we're supposed to be running
/*!	Returns the value of the internal variable that indicates that
 * the threads are running.
    \return Value of internal state flag, as taken from ::AGENT_STATE.
*/
// replica of agent_running
uint16_t Agent::isRunning()
{
    return (cdata[0].agent[0].stateflag);
}

// replica of agent_send_request
int32_t Agent::sendRequest(beatstruc beat, std::string request, std::string &response)
{
    char response_c_str[300];
    int32_t iretn = agent_send_request(beat, request.c_str(), response_c_str, 512, 2 );

    response = std::string(response_c_str);

    return iretn;
}

// replica of agent_add_request
int32_t Agent::addRequest(std::string request, agent_request_function function)
{

    int32_t iretn = agent_add_request(cdata, request, function);

    return iretn;
}


//! Shutdown server gracefully
/*! Waits for heartbeat and request loops to stop running before pulling the rug out from under them.
 */
// replica from agent_shutdown_server
int32_t Agent::shutdown()
{
    cdata[0].agent[0].stateflag = AGENT_STATE_SHUTDOWN;;
    hthread.join();
    cthread.join();
    agent_unsubscribe(cdata);
    agent_unpublish(cdata);
    json_destroy(cdata);

    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "Agent is down" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    return 0;
}


int32_t Agent::post(uint8_t type, std::string message)
{
    return agent_post(cdata, type, message);
}

int32_t Agent::send(uint8_t address, std::string message)
{
    return post(address, message);
}



int32_t Agent::poll(pollstruc &meta, std::string &message, uint8_t type, float waitsec)
{
    return agent_poll(cdata, meta, message, type, waitsec);
}

int32_t Agent::poll(uint8_t type, std::string &message)
{
    float waitsec = 1; // by default
    return agent_poll(cdata, metaRx, message, type, waitsec);
}

int32_t Agent::receive(uint8_t address, std::string &message)
{
    return poll(address, message);
}



int32_t Agent::pollParse(std::string &message)
{
    // collect the header
    metaHeader = message.substr(0, metaRx.jlength);
    // collect the data
    message.erase(0, metaRx.jlength);

    return 0;
}

//beatstruc Agent::findAgent(std::string agent)
//{
//    return findServer(agent);
//}

beatstruc Agent::find(std::string agent)
{
    float timeout = 1.0;

    beatstruc beat_agent = agent_find_server(cdata, nodeName, agent, timeout);

    // TODO: improve the way we find the agent server
    if (beat_agent.utc == 0)
    {
        if (printMessages) {
            std::cout << "agent " << agent << " : not found" << std::endl;
        }
    }
    else
    {
        if (printMessages) {
            std::cout << "agent " << agent << " : found" << std::endl;
        }
    }

    return beat_agent;
}

//! @}
