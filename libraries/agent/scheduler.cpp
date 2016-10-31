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

#include "agent/scheduler.h"

namespace Cosmos {

Scheduler::Scheduler(string node_name) {

    agent = new Agent(node_name, "test");
    agent_exec_soh = agent->find_agent("exec", node_name);

}

Scheduler::~Scheduler() {

}

void Scheduler::addEvent(string name,
                           string data,
                           double mjd,
                           string condition,
                           uint32_t flag) {

    Event event;
    event.generator(name, data, mjd, condition, flag);

    //com.set_command(line);

    if (!agent_exec_soh.exists) { // TODO: change the way to find if another beat exists (no mjd)
        cout << "could not find agent exec" << endl;
        return;
    }

    string out;
    agent->send_request(agent_exec_soh, "add_event "+ event.event_string, out, 0);

    cout << "event set: " << endl;
    cout << out << endl;

}

void Scheduler::addEvent(Event event) {
    addEvent(event.name, event.data, event.mjd, event.condition, event.flag);
}

void Scheduler::deleteEvent(string name,
                              string data,
                              double mjd,
                              string condition,
                              uint32_t flag) {

    Event event;

	// set the event_string
	// JIMNOTE: this could be done in a constructor
    event.generator(name, data, mjd, condition, flag);

    if (!agent_exec_soh.exists) {
        cout << "could not find agent exec" << endl;
        return;
    }

    string out;
    agent->send_request(agent_exec_soh, "del_event "+ event.event_string, out, 0);

    cout << "event deleted: " << out << endl;

}

void Scheduler::deleteEvent(Event event) {
    deleteEvent(event.name,event.data, event.mjd, event.condition, event.flag);
}

int Scheduler::getEventQueueSize() {
    if (!agent_exec_soh.exists) {
        cout << "could not find agent exec" << endl;
        return 0 ;
    }

    string out;
    agent->send_request(agent_exec_soh, "get_queue_size", out, 0);

    StringParser str(out,'[');
    int queue_size = str.getFieldNumberAsInteger(1);
    cout << "Event queue size: " << queue_size << endl;

    return queue_size;
}

void Scheduler::getEventQueue() {
    if (!agent_exec_soh.exists) {
        cout << "could not find agent exec" << endl;
        return ;
    }

    string out;
    agent->send_request(agent_exec_soh, "get_event", out, 0);

    //StringParser str(out,'[');
    //int queue_size = str.getFieldNumberAsInteger(1);
    cout << endl << "queue list: " << endl << out << endl;

//    return queue_size;
}

} // end namespace Cosmos

//! @}
