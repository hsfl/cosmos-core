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

#include "command.h"

namespace Cosmos {


// Default Constructor for command objects
Command::Command() : utc(0), utcexec(0), name(""), type(0), flag(0), data(""), condition(""), already_ran(false)
{
}

Command::~Command()
{

}

std::string Command::generator(std::string name,
                        std::string data,
                        double utc,
                        std::string condition,
                        uint32_t flag){

    longeventstruc command;

    command.type = EVENT_TYPE_COMMAND;
    command.flag = 0;
    command.data[0] = 0;
    command.condition[0] = 0;
    command.utc = 0;
    command.utcexec = 0.;

    // submit_command name command_string [time [condition [repeat_flag]]]

    strcpy(command.name, name.c_str());
    // TODO: command.name = name.c_str();

    strcpy(command.data, data.c_str());
    // TODO: command.data = data;

    command.utc  = utc;

    strcpy(command.condition, condition.c_str());
    // TODO: command.condition = condition;
    command.flag = flag;

    //std::string jsp;

    json_out_commandevent(command_string, command);

    return command_string;
    //printf("%s\n", jsp.c_str());

}


// *************************************************************************
// Class: command
// *************************************************************************

// Copies the current command object to the output stream using JSON format
std::ostream& operator<<(std::ostream& out, const Command& cmd)
{
    out	<< std::setprecision(15) <<"{\"event_utc\":"<< cmd.utc
        << "}{\"event_utcexec\":" << cmd.utcexec
        << "}{\"event_name\":\"" << cmd.name
        << "\"}{\"event_type\":" << cmd.type
        << "}{\"event_flag\":" << cmd.flag
        << "}{\"event_data\":\"" << cmd.data
        << "\"}{\"event_condition\":\"" << cmd.condition
        << "\"}";
    return out;
}

// Equality Operator for command objects
bool operator==(const Command& cmd1, const Command& cmd2)
{
    return (	cmd1.name==cmd2.name &&
                cmd1.utc==cmd2.utc &&
                cmd1.utcexec==cmd2.utcexec &&
                cmd1.type==cmd2.type &&
                cmd1.flag==cmd2.flag &&
                cmd1.data==cmd2.data &&
                cmd1.condition==cmd2.condition);
}




// Copies the command information stored in the local copy
// agent->cinfo->pdata.event[0].l into the current command object
void Command::set_command(std::string line, Agent *agent)
{
    json_clear_cosmosstruc(JSON_STRUCT_EVENT, agent->cinfo->meta, agent->cinfo->sdata);
    json_parse(line, agent->cinfo->meta, agent->cinfo->sdata);
    utc = agent->cinfo->sdata.event[0].l.utc;
    utcexec = agent->cinfo->sdata.event[0].l.utcexec;
    name = agent->cinfo->sdata.event[0].l.name;
    type = agent->cinfo->sdata.event[0].l.type;
    flag = agent->cinfo->sdata.event[0].l.flag;
    data = agent->cinfo->sdata.event[0].l.data;
    condition = agent->cinfo->sdata.event[0].l.condition;
}

std::string Command::get_json()
{
    std::string jsp;

    longeventstruc event;

    event.utc = utc;
    event.utcexec = utcexec;
    strcpy(event.name, name.c_str());
    event.type = type;
    event.flag = flag;
    strcpy(event.data, data.c_str());
    strcpy(event.condition, condition.c_str());

    json_out_commandevent(jsp, event);
    return jsp;
}



} // end namespace Cosmos

//! @}
