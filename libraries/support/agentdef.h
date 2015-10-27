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

//#include <stdio.h>
//#include <stdlib.h>
//#include <errno.h>

using namespace std;

//! \ingroup agentlib
//! \defgroup agentlib_constants Agent Server and Client Library constants
//! @{

//! Agent socket using Multicast UDP
#define AGENT_TYPE_MULTICAST 0
//! Agent socket using Broadcast UDP
#define AGENT_TYPE_BROADCAST 2
//! Agent socket using Unicast UDP
#define AGENT_TYPE_UDP 2
//! Agent socket using Broadcast CSP
#define AGENT_TYPE_CSP 3

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

//! All Message types
#define AGENT_MESSAGE_ALL 1
//! Heartbeat Messages
#define AGENT_MESSAGE_BEAT 2
//! State of Health Messages
#define AGENT_MESSAGE_SOH 3
//! Generic Mesages
#define AGENT_MESSAGE_GENERIC 4
#define AGENT_MESSAGE_TIME 5
#define AGENT_MESSAGE_LOCATION 6
#define AGENT_MESSAGE_TRACK 7
#define AGENT_MESSAGE_IMU 8
//! Event Messsages
#define AGENT_MESSAGE_EVENT 9

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
