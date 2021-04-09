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

/*! \file agentclass2.cpp
    \brief Agent support functions
*/

#include "agent/agentclass2.h"

namespace Cosmos
{
namespace Support
{
	//! \ingroup agentclass2
    //! \ingroup agentclass2
    //! \defgroup agentclass2_functions Agent Server and Client functions
    //! @{

    //! Add COSMOS awareness.
    //! Sets up minimum framework for COSMOS awareness. The minimum call makes a nodeless client, setting up the
    //! message ring buffer thread, and a main thread of execution. Additional parameters are related to making
    //! the program a true Agent by tieing it to a node, and starting the request and heartbeat threads.
    //! \param ntype Transport Layer protocol to be used, taken from ::NetworkType. Defaults to UDP Broadcast.
    //! \param node_name Node name. Defaults to empty.
    //! \param agent_name Agent name. Defaults to empty. If this is defined, the full Agent code will be started.
    //! \param bprd Period, in seconds, for heartbeat. Defaults to 1.
    //! \param bsize Size of interagent communication buffer. Defaults to ::AGENTMAXBUFFER.
    //! \param mflag Boolean controlling whether or not multiple instances of the same Agent can start. If true, then Agent names
    //! will have an index number appended (eg: myname_001). If false, agent will listen for 5 seconds and terminate if it senses
    //! the Agent already running.
    //! \param portnum The network port to listen on for requests. Defaults to 0 whereupon it will use whatever th OS assigns.
    //! \param dlevel debug level. Defaults to 1 so that if there is an error the user can immediately see it. also initialized in the namespace variables
    Agent2::Agent2(
		const string &node_name,
		const string &agent_name,
		double bprd,
		uint32_t bsize,
		bool mflag,
		int32_t portnum,
		NetworkType ntype,
		uint16_t dlevel)
		: Agent{node_name, agent_name, bprd, bsize, mflag, portnum, ntype, dlevel}
	{
        add_request("soh",req_soh,"","Get Limited SOH string");
	}

    //! \ingroup agentclass2
    //! \ingroup agentclass2
    //! \defgroup agentclass2_functions Agent Server and Client functions
    //! @{

	//! Set SOH string
	/*! Set the SOH string to a json list of \ref namespace 2.0 names.
		\param list Vector of strings of namespace 2.0 names.
		\return 0, otherwise a negative error.
	*/
	int32_t Agent2::set_sohstring(vector<std::string> list) {
		sohstring = list;
		
		return 0;
	}

	int32_t Agent2::req_soh(string &, string &response, Agent *agent) {
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
		return 0;
	}
} // end of namespace Support
} // end namespace Cosmos

//! @}
