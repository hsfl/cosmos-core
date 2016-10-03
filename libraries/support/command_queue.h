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

#ifndef COSMOS_COMMAND_QUEUE_H
#define COSMOS_COMMAND_QUEUE_H

/*! \file command_queue.h
*	\brief Command Queue Class
*/

//! \ingroup support
//! \defgroup agentlib Agent Server and Client Library
//! %Agent Server and Client.
//!

#include "configCosmos.h"
#include "jsonlib.h"
#include "agent/agentclass.h"
#include "command.h"

namespace Cosmos {



// Class to manage information about a list of commands
class command_queue
{
private:
    std::list<Command> commands;
    bool queue_changed = false;

public:
    size_t get_size()
    {
        return commands.size();
    }

    Command& get_command(int i)
    {
        std::list<Command>::iterator ii = commands.begin();
        std::advance(ii,i);
        return *ii;
    }
    void load_commands(std::string incoming_dir, Agent *agent);
    void save_commands(std::__1::string temp_dir);
    void run_command(Command &cmd, std::__1::string nodename, double logdate_exec);
    void run_commands(Agent *agent, std::__1::string nodename, double logdate_exec);
    void add_command(Command& c);
    int del_command(Command& c);
    void sort()	{ commands.sort([](Command & c1, Command & c2) { return c1.get_utc() < c2.get_utc(); });	}
    friend std::ostream& operator<<(std::ostream& out, command_queue& cmd);

//    bool compare_command_times(Command command1, Command command2);
//    std::string incoming_dir;
//    std::string outgoing_dir;
//    std::string temp_dir;

}; // end of Command Queue Class

} // end of namepsace Cosmos

#endif // COSMOS_COMMAND_QUEUE_H
