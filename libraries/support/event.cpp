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

#include "support/event.h"

namespace Cosmos {

/// Default constructor
Event::Event() :
    mjd(0),
	utcexec(0),
	name(""),
	type(0),
	flag(0),
	data(""),
	condition(""),
	already_ran(false)
{}

/// Destructor
Event::~Event() {}

string Event::generator(
	string name,
	string data,
	double mjd,
	string condition,
	uint32_t flag
) {

    this->name = name;
    this->data = data;
    this->mjd  = mjd;
    this->condition = condition;
    this->flag = flag;

    // TODO: this is temporary, later will only use Event class
    // no more longeventstruc

    longeventstruc event;

    // reset
    event_string = "";
    event.type = EVENT_TYPE_COMMAND;
    event.flag = 0;
    event.data[0] = 0;
    event.condition[0] = 0;
    event.utc = 0;
    event.utcexec = 0.;

    strcpy(event.name, name.c_str());
    strcpy(event.data, data.c_str());
    strcpy(event.condition, condition.c_str());
    event.utc  = mjd;
    event.flag = flag;

	// set event_string
    json_out_commandevent(event_string, event);

    return event_string;
}

string Event::generator(longeventstruc event) {

    // returns a string with the event and also puts the string in "event_string"
    return generator(event.name, event.data, event.utc, event.condition, event.flag);;
}


// Copies the current event object to the output stream using JSON format
std::ostream& operator<<(std::ostream& out, const Event& cmd)
{
    out	<< std::setprecision(15) <<"{\"event_utc\":"<< cmd.mjd
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
bool operator==(const Event& cmd1, const Event& cmd2)
{
	return (	cmd1.name==cmd2.name &&
                cmd1.mjd==cmd2.mjd &&
				cmd1.utcexec==cmd2.utcexec &&
				cmd1.type==cmd2.type &&
				cmd1.flag==cmd2.flag &&
				cmd1.data==cmd2.data &&
				cmd1.condition==cmd2.condition);
}

// JIMNOTE:  trying to take out the Agent dependency for the Event class
//				(because an Event and a queue of command Events has nothing
//				to do with the concept of Agents)
//void Event::set_command(string jstring, Agent *agent)
void Event::set_command(string jstring)
{
	// clear Event information in agent
	cosmosstruc * dummy = json_create();
    json_mapbaseentries(dummy);

    json_clear_cosmosstruc(JSON_STRUCT_EVENT, dummy);

	// load Event information (from jstring) into agent
    json_parse(jstring, dummy);

    mjd = dummy->event[0].l.utc;
    utcexec = dummy->event[0].l.utcexec;
    name = dummy->event[0].l.name;
    type = dummy->event[0].l.type;
    flag = dummy->event[0].l.flag;
    data = dummy->event[0].l.data;
    condition = dummy->event[0].l.condition;
}

string Event::get_event_string()
{
	longeventstruc event;

	// set Event members
    event.utc = mjd;
	event.utcexec = utcexec;
	strcpy(event.name, name.c_str());
	event.type = type;
	event.flag = flag;
	strcpy(event.data, data.c_str());
	strcpy(event.condition, condition.c_str());

	// return Event data as JSON string
	string jsp;
	json_out_commandevent(jsp, event);
	return jsp;
}

bool Event::condition_true(cosmosstruc *cinfo)
{
    const char *cp = condition.c_str();
    if (cinfo != nullptr) { // Note: an equation must be enclosed by parentheses.
        double r =  json_equation(cp, cinfo);
        return fabs(r - 1.0) < std::numeric_limits<double>::epsilon();
    }
    return false;
}

} // end namespace Cosmos

//! @}
