/**
 * @file event.cpp
 * @brief 
 * 
 * Copyright (C) 2024 by Interstel Technologies, Inc. and Hawaii Space Flight
 * Laboratory.
 * 
 * This file is part of the COSMOS/core that is the central module for COSMOS.
 * For more information on COSMOS go to <http://cosmos-project.com>
 * 
 * The COSMOS/core software is licenced under the GNU Lesser General Public
 * License (LGPL) version 3 licence.
 * 
 * You should have received a copy of the GNU Lesser General Public License. If
 * not, go to <http://www.gnu.org/licenses/>
 * 
 * COSMOS/core is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 * 
 * COSMOS/core is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * Refer to the "licences" folder for further information on the conditions and
 * terms to use this software.
 */

#include "agent/event.h"

namespace Cosmos {

/**
 * @brief Construct a new Event:: Event object
 * 
 * Default constructor
 */
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

/**
 * @brief Destroy the Event:: Event object
 * 
 * Destructor
 */
Event::~Event() {}

/**
 * @brief 
 * 
 * @param name 
 * @param data 
 * @param mjd 
 * @param condition 
 * @param flag 
 * @param type 
 * @return string 
 * 
 * @todo Document this.
 */
string Event::generator(string name,
    string data,
    double mjd,
    string condition,
    uint32_t flag
, uint16_t type) {

    this->name = name;
    this->data = data;
    this->mjd  = mjd;
    this->condition = condition;
    this->flag = flag;
    this->utcexec = 0.;
    this->type = type;
    event_string = "";
    event_string = this->get_event_string();

    return event_string;
}

/**
 * @brief 
 * 
 * @param event 
 * @return string 
 * 
 * @todo Document this.
 */
string Event::generator(eventstruc event) {

    // returns a string with the event and also puts the string in "event_string"
    return generator(event.name, event.data, event.utc, event.condition, event.flag, event.type);;
}

/**
 * @brief Extraction operator
 * 
 * Copies the current event object to the output stream using JSON format
 * 
 * Writes the given Event to the given output stream (in JSON format) and 
 * returns a reference to the modified ostream.
 * 
 * @param out Reference to ostream
 * @param cmd Reference to const Event
 * @return ::std::ostream& Reference to modified ostream
 */
::std::ostream& operator<<(::std::ostream& out, const Event& cmd)
{
    JSONObject jobj;
    jobj.addElement("utc", JSONValue(cmd.mjd));
    if (cmd.utcexec != 0.)
    {
        jobj.addElement("utcexec", JSONValue(cmd.utcexec));
    }
    jobj.addElement("name", JSONValue(cmd.name));
    jobj.addElement("type", JSONValue(cmd.type));
    jobj.addElement("flag", JSONValue(cmd.flag));
    jobj.addElement("data", JSONValue(cmd.data));
    if (cmd.flag & EVENT_FLAG_CONDITIONAL)
    {
        jobj.addElement("condition", JSONValue(cmd.condition));
    }
    out	<< std::setprecision(15) << jobj.to_json_string();
    return out;
}

/**
 * @brief Equality Operator for command objects
 * 
 * @param cmd1 First event
 * @param cmd2 Second event
 * @return true if events are exactly the same
 * @return false otherwise
 */
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

/**
 * @brief Sets %Event information from a JSON formatted string
 * 
 * This function copies all Event information (from a JSON formatted string) 
 * into the current Event object
 * 
 * void Event::set_command(string jstring, Agent *agent)
 * 
 * JIMNOTE: trying to take out the Agent dependency for the Event class (because
 * an Event and a queue of command Events has nothing to do with the concept of 
 * Agents)
 * 
 * @param jstring Event information string formatted as JSON
 */
void Event::set_command(string jstring)
{
    eventstruc dummy;

    // load Event information (from jstring)
    dummy.from_json(jstring);

    mjd = dummy.utc;
    utcexec = dummy.utcexec;
    name = dummy.name;
    type = dummy.type;
    flag = dummy.flag;
    data = dummy.data;
    condition = dummy.condition;
}

/**
 * @brief Retrieves Event information
 * 
 * @return string representing Event information (as a JSON formatted string)
 */
string Event::get_event_string()
{
    JSONObject jobj;
    jobj.addElement("utc", JSONValue(mjd));
    if (utcexec != 0.)
    {
        jobj.addElement("utcexec", JSONValue(utcexec));
    }
    jobj.addElement("name", JSONValue(name));
    jobj.addElement("type", JSONValue(type));
    jobj.addElement("flag", JSONValue(flag));
    jobj.addElement("data", JSONValue(data));
    if (flag & EVENT_FLAG_CONDITIONAL)
    {
        jobj.addElement("condition", JSONValue(condition));
    }

    return jobj.to_json_string();
}

/**
 * @brief 
 * 
 * JIMNOTE: this function (condition_true)  needs a look at....
 * OLDNOTE: seems to return nan from json_equation...  how to use?
 * 
 * @param cinfo 
 * @return true 
 * @return false 
 * 
 * @todo Document this.
 */
bool Event::condition_true(cosmosstruc *cinfo)
{
    const char *cp = condition.c_str();
    if (cinfo != nullptr)
    { // Note: an equation must be enclosed by parentheses.
        double r =  json_equation(cp, cinfo);
        if (fabs(r - 1.0) < std::numeric_limits<double>::epsilon())
        {
            ++true_count;
            if (true_count >= 5 * (flag&EVENT_FLAG_PRIORITY)/32)
            {
                return true;
            }
        }
    }
    return false;
}
} // end namespace Cosmos

//! @}
