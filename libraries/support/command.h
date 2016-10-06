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

#ifndef COSMOS_COMMAND_H
#define COSMOS_COMMAND_H

/*! \file commanding.h
*	\brief Commanding Class
*/

//! \ingroup support
//! \defgroup agentlib Agent Server and Client Library
//! %Agent Server and Client.
//!


#include "configCosmos.h"
#include "jsonlib.h"
#include "agent/agentclass.h" // TODO: remove dependency

using std::string;

#define MAXCOMMANDWORD 20

namespace Cosmos {

// *************************************************************************
// Class: command
// *************************************************************************

// Class to manage information about a single command event
class Command
{
private:
    double	utc;
    double	utcexec;
    string name;
    uint32_t type;
    uint32_t flag;
    string data;
    string condition;

public:
    Command();
    ~Command();

    void set_command(string line, Agent *agent);
    string get_json();
    void set_utcexec()	{	utcexec = currentmjd(0.);	}
    void set_actual()	{	flag |= EVENT_FLAG_ACTUAL;	}
    double get_utc()	{	return utc;	}
    double get_utcexec()	{	return utcexec;	}
    char*  get_data()	{	return (char*)data.c_str();	}
    bool is_ready()		{	return (utc <= currentmjd(0.)); }
    bool is_repeat()	{	return (flag & EVENT_FLAG_REPEAT);	}
    bool is_command()	{	return (type & EVENT_TYPE_COMMAND);	}
    bool is_conditional()	{	return (flag & EVENT_FLAG_CONDITIONAL);	}
    bool already_ran;

    string generator(string name,
                   string data,
                   double utc,
                   string condition,
                   uint32_t flag);
    string command_string;

    //seems to return nan from json_equation...  how to use?
    bool condition_true(cosmosstruc *cinfo)
    {
        const char *cp = (char *)condition.c_str();
        if (cinfo != nullptr)
        {
            // TODO: remove from this class, to keep it modular
//            double d = json_equation(cp, agent->cinfo->meta, agent->cinfo->pdata);
            double d;
            return d;
        }
        else
        {
            return false;
        }
    }

    const string get_name()
    {
        return name;
    }
    friend std::ostream& operator<<(std::ostream& out, const Command& cmd);
    friend bool operator==(const Command& cmd1, const Command& cmd2);


}; // end of Commanding Class

} // end of namepsace Cosmos

#endif // COSMOS_COMMAND_H
