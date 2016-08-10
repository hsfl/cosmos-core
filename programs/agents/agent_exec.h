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

#ifndef AGENT_EXEC_H
#define AGENT_EXEC_H

/*! \file agent_exec.h
* \brief Executive Agent header file
*/
#include "configCosmos.h"
#include "jsonlib.h"


// *************************************************************************
// Class: command
// *************************************************************************

// Class to manage information about a single command event
class command
{
private:
    double	utc;
    double	utcexec;
    std::string name;
    uint32_t type;
    uint32_t flag;
    std::string data;
    std::string condition;

public:
    command();
    void set_command(std::string line);
    std::string get_json();
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

    //seems to return nan from json_equation...  how to use?
    bool condition_true(cosmosstruc *cinfo)
    {
        const char *cp = (char *)condition.c_str();
        if (cinfo != nullptr)
        {
            double d = json_equation(cp, cinfo->meta, cinfo->pdata);
            return d;
        }
        else
        {
            return false;
        }
    }

    const std::string get_name()
    {
        return name;
    }
    friend std::ostream& operator<<(std::ostream& out, const command& cmd);
    friend bool operator==(const command& cmd1, const command& cmd2);
};





// *************************************************************************
// Class: command_queue
// *************************************************************************


// Predicate function for comparing command objects, used by command_queue.sort()
bool compare_command_times(command command1, command command2)
{
    return command1.get_utc()<command2.get_utc();
}


// Class to manage information about a list of commands
class command_queue
{
private:
    std::list<command> commands;

public:
    size_t get_size()
    {
        return commands.size();
    }
    command& get_command(int i)
    {
        std::list<command>::iterator ii = commands.begin();
        std::advance(ii,i);
        return *ii;
    }
    void load_commands();
    void save_commands();
    void run_commands();
    void add_command(command& c);
    int del_command(command& c);
    void sort()	{ commands.sort(compare_command_times);	}
    friend std::ostream& operator<<(std::ostream& out, command_queue& cmd);
};


#endif // AGENT_EXEC_H
