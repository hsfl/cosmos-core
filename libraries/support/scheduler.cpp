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

/*! \file scheduler.cpp
    \brief Agent support functions
*/

#include "scheduler.h"


namespace Cosmos {

Scheduler::Scheduler(std::string node_name) {

    agent = new Agent(NetworkType::UDP, node_name, "test");
    agent_exec_soh = agent->find_agent("kauaicc_sim","execsoh");

}

Scheduler::~Scheduler() {

}

void Scheduler::addCommand(std::string name,
                           std::string data,
                           double utc,
                           std::string condition,
                           uint32_t flag) {

    Command command;
    command.generator(name, data, utc, condition, flag);

    //com.set_command(line);

    using std::cout;
    using std::endl;

    if (!agent_exec_soh.exists) { // TODO: change the way to find if another beat exists (no utc)
        std::cout << "could not find agent execsoh" << std::endl;
    }

    std::string out;
    agent->send_request(agent_exec_soh, "add_queue_entry "+ command.command_string, out, 0);

    cout << "command set: " << out << endl;

}

void Scheduler::addCommand(longeventstruc command) {
    addCommand(command.name, command.data, command.utc, command.condition, command.flag);
}

void Scheduler::deleteCommand(std::string name,
                           std::string data,
                           double utc,
                           std::string condition,
                           uint32_t flag) {

    Command command;
    command.generator(name, data, utc, condition, flag);

    //com.set_command(line);

    using std::cout;
    using std::endl;

    if (!agent_exec_soh.exists) { // TODO: change the way to find if another beat exists (no utc)
        std::cout << "could not find agent execsoh" << std::endl;
    }

    std::string out;
    agent->send_request(agent_exec_soh, "del_queue_entry "+ command.command_string, out, 0);

    cout << "command deleted: " << out << endl;

}


} // end namespace Cosmos

//! @}
