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

#ifndef _AGENTDEF_H
#define _AGENTDEF_H 1

/*! \file agentdef.h
*	\brief Agent Server and Client definitions file
*/

#include "configCosmos.h"
#include "jsondef.h"

//! \ingroup agentlib
//! \defgroup agentlib_constants Agent Server and Client Library constants
//! @{

//! Type of network channel for agent to use.
//enum class AgentType : std::uint16_t
//    {
//    //! Agent socket using Multicast UDP
//    MULTICAST=0,
//    //! Agent socket using Broadcast UDP
//    BROADCAST=2,
//    //! Agent socket using Unicast UDP
//    UDP=2,
//    //! Agent socket using Broadcast CSP
//    CSP=3
//    } ;

enum AGENT_STATE
    {
    //! Shut down Agent
    AGENT_STATE_SHUTDOWN=0,
    //! Agent Initializing
    AGENT_STATE_INIT,
    //! Do minimal necessary to run
    AGENT_STATE_IDLE,
    //! Run without monitoring
    AGENT_STATE_RUN,
    //! Run with monitoring
    AGENT_STATE_MONITOR,
    //! Agent in Safe State
    AGENT_STATE_SAFE,
    //! Agent in Debug State
    AGENT_STATE_DEBUG
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

//! @}

#endif
