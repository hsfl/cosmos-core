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

#ifndef COSMOS_CommandQueue_H
#define COSMOS_CommandQueue_H

/*! \file CommandQueue.h
*	\brief Command Queue Class
*/

//! \ingroup support
//! \defgroup agentlib Agent Server and Client Library
//! %Agent Server and Client.
//!

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "agent/agentclass.h"
#include "support/event.h"

namespace Cosmos {

/// Class to manage information about a queue of Events
class CommandQueue
{
private:
	/**	An std::list of members of the Event class	*/
	std::list<Event> commands;
	/** A boolean indicator that the queue has changed	*/
	bool queue_changed = false;

public:

	///	Retrieve the size of the queue
	/**
		\return	The size of the queue
	*/
	size_t get_size() { return commands.size(); }

	///	Retrieve an Event by its position in the queue
	/**
		\param	i	Integer representing the position in the queue	
		\return	Reference to the ith Event
	*/
    Event& get_command(int i)
	{
        std::list<Event>::iterator ii = commands.begin();
		std::advance(ii,i);
		return *ii;
	}

	///	Load queue of Events from a file
	/**

		Reads new Events from *.command files in the incoming directory,
		adds them to the queue of Events, and deletes the *.command files.
		Events in the queue are then sorted by their execution time.

		\param	incoming_dir	Directory where the .queue file will be read from
		\param	agent	Pointer to Agent object (needed to parse JSON input)
		
	*/
	void load_commands(string incoming_dir, Agent *agent);

	///	Save the queue of Events to a file
	/**
		Save the queue of Events to the file temp_dir/.queue

		\param	temp_dir	Directory where the .queue file will be written
	*/
	void save_commands(string temp_dir);

	/// Run the given Event
	/**
		Execute an event using ford().  For each event run, the time of 
		execution (utcexec) is set, the flag EVENT_FLAG_ACTUAL is set to true,
		and this updated command information is logged to the OUTPUT directory.

		\param	cmd	Reference to event to run
		\param	nodename	Name of node
		\param	logdate_exec	Time of execution (for logging purposes)
	*/
    void run_command(Event &cmd, string nodename, double logdate_exec);

	///	Traverse the entire queue of Events, and run those which qualify.
	/**

		An %Event only qualifies to run if the current time is greater than or equal to
		the execution time of the %Event.  Further, if the %Event is conditional, then the
		%Event condition must be true.

		\param	agent	Pointer to Agent object (for call to condition_true(..))
		\param	nodename	Name of the node
		\param	logdate_exec	Time of execution (for logging purposes)
	*/
	void run_commands(Agent *agent, string nodename, double logdate_exec);

	///	Add Event to the queue
	/**
		\param	c	 Event to add

		JIMNOTE:	this only adds given Event to the queue if the Event has flag for EVENT_TYPE_COMMAND set to true
	*/
    void add_command(Event& c);

	///	Remove Event from the queue
	/**
		\param	c	Event to remove
		\return	The number of Events removed

		This function only removes events from the queue if the are exactly equal to the given Event.
	*/
    int del_command(Event& c);

	///	Sort the Events in the queue by Event exectution time
	/**
		This function is called after new Events are loaded.
	*/
    void sort()	{ commands.sort([](Event & c1, Event & c2) { return c1.getTime() < c2.getTime(); });	}
	///	Extraction operator
	/**
		\param	out	Reference to ostream
		\param	cmd	Reference to CommandQueue (JIMNOTE: should be const, ya?)
		\return	Reference to modified ostream

		Writes the given CommandQueue to the given output stream (in JSON format) and returns a reference to the modified ostream.
		
	*/
    friend std::ostream& operator<<(std::ostream& out, CommandQueue& cmd);

//	bool compare_command_times(Event command1, Event command2);
//	string incoming_dir;
//	string outgoing_dir;
//	string temp_dir;

}; // end of Command Queue Class

} // end of namepsace Cosmos

#endif // COSMOS_CommandQueue_H
