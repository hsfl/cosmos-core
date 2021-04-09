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

#ifndef COSMOSAGENT2_H
#define COSMOSAGENT2_H

/*! \file agentclass2.h
*	\brief Agent Server and Client header file, with certain functionalities implemented using Namespace 2.0
*
*/

//! \ingroup support
//! \defgroup agentclass2 Agent Server and Client Library
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

#include "agent/agentclass.h"

namespace Cosmos
{
	namespace Support
	{
		class Agent2 : public Agent
		{
		public:
			Agent2(
				const string &node_name = "",
				const string &agent_name = "",
				double bprd = 0.,
				uint32_t bsize = AGENTMAXBUFFER,
				bool mflag = false,
				int32_t portnum = 0,
				NetworkType ntype = NetworkType::UDP,
				uint16_t dlevel = 0
			);

            //!s
            // agent functions
            int32_t set_sohstring(vector<string> list);

		protected:
		private:
            static int32_t req_soh(string &, string &response, Agent *agent);
		};
	}
}

#endif