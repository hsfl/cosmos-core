/*!******************************************************************
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

/*! \file command_queue.h
*	\brief Command Queue Class
*/

//! \ingroup support
//! \defgroup agentlib Agent Server and Client Library
//! %Agent Server and Client.
//!

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "agent/agentclass.h"
#include "agent/event.h"

namespace Cosmos
{
    namespace Support
    {
        namespace Command
        {
            int32_t Shell(string command_line="", string outpath="", string inpath="", string errpath="");
        }

        //! Class to manage information about a queue of Events
        class CommandQueue
        {
        private:
            /**	An std::list of members of the Event class to be run	*/
            std::list<Event> commands;
            /**	An std::queue of members of the Event class that have run	*/
            std::deque<Event> events;
            /** A vector of all threads spawned to run events  */
            vector<std::thread> event_threads;
            /** A boolean indicator that the queue has changed	*/
            bool queue_changed = false;

            bool queue_blocked = false;

        public:
            //! Ensure all threads are joined before destruction.
            ~CommandQueue();

            //! Join all threads spawn and empty our vector.
            size_t join_event_threads();

            //!	Retrieve the size of the queue
            /*!
                \return	The size of the queue
            */
            size_t get_event_size() { return events.size(); }

            //!	Retrieve an Event by its position in the queue
            /*!
                \param	i	Integer representing the position in the queue
                \return	Reference to the ith Event
            */
            Event& get_event(int i)
            {
                std::deque<Event>::iterator ii = events.begin();
                std::advance(ii,i);
                return *ii;
            }

            //!	Retrieve the size of the queue
            /*!
                \return	The size of the queue
            */
            size_t get_command_size() { return commands.size(); }

            //!	Retrieve an Event by its position in the queue
            /*!
                \param	i	Integer representing the position in the queue
                \return	Reference to the ith Event
            */
            Event& get_command(int i)
            {
                std::list<Event>::iterator ii = commands.begin();
                std::advance(ii,i);
                return *ii;
            }

            //!	Load queue of Events from a file
            /*!

                Reads new Events from *.command files in the incoming directory,
                adds them to the queue of Events, and deletes the *.command files.
                Events in the queue are then sorted by their execution time.

                \param	incoming_dir	Directory where the .queue file will be read from

            */
            int32_t load_commands(string incoming_dir);

            //!	Save the queue of Events to a file
            /*!
            Save the queue of Events to the file temp_dir/.queue

                \param	temp_dir	Directory where the .queue file will be written
            */
            int32_t save_commands(string temp_dir, string name=".queue");

            //!	Restore the queue of Events from a file
            /*!
            Save the queue of Events to the file temp_dir/.queue

                \param	temp_dir	Directory where the .queue file will be written
            */
            int32_t restore_commands(string temp_dir, string name=".queue");

            //! Run the given Event
            /*!
            Execute an event using ford().  For each event run, the time of
            execution (utcexec) is set, the flag EVENT_FLAG_ACTUAL is set to true,
            and this updated command information is logged to the OUTPUT directory.

                \param	cmd	Reference to event to run
                \param	nodename	Name of node
                \param	logdate_exec	Time of execution (for logging purposes)
            */
            int32_t run_command(Event &cmd, string nodename, double logdate_exec);

            //!	Traverse the entire queue of Events, clearing those that have finished.
            /*!

            An %Event qualifies to be cleared if its thread could be joined, or if it has been
            running for more than 1 minute.

                \param	agent	Pointer to Agent object (for call to condition_true(..))
                \param	nodename	Name of the node
                \param	logdate_exec	Time of execution (for logging purposes)
            */
//            int32_t flush_commands();

            //!	Traverse the entire queue of Events, and run those which qualify.
            /*!

            An %Event only qualifies to run if the current time is greater than or equal to
            the execution time of the %Event.  Further, if the %Event is conditional, then the
            %Event condition must be true.

                \param	agent	Pointer to Agent object (for call to condition_true(..))
                \param	nodename	Name of the node
                \param	logdate_exec	Time of execution (for logging purposes)
            */
            int32_t run_commands(Agent *agent, string nodename, double logdate_exec);

            //!	Remove **all** matching Event from the queue
            /*!
                \param	c	Event to remove
                \return	The number of Events removed

                JIMNOTE:	this only adds given Event to the queue if the Event has flag for EVENT_TYPE_COMMAND set to true
            */
            int32_t add_command(Event& c);

            ///	Remove **all** matching Event from the queue
            /**
                \param	c	Event to remove
                \return	The number of Events removed

                This function only removes events from the queue if the are exactly equal to the given Event.
            */
            int32_t del_command(Event& c);

            //! Remove Event from the queue based on position
            /*!
                 \param  pos  Position of event to remove
                 \return The number of Events removed

                 This function removes events based on their queue position (0-indexed).
            */
            int32_t del_command(int pos);

            //!	Sort the Events in the queue by Event exectution time
            /*!
                This function is called after new Events are loaded.
            */
            void sort()	{ commands.sort([](Event & c1, Event & c2) { return c1.getTime() < c2.getTime(); });	}
            //!	Extraction operator
            /*!
                \param	out	Reference to ostream
                \param	cmdq	Reference to CommandQueue (JIMNOTE: should be const, ya?)
                \return	Reference to modified ostream

                Writes the given CommandQueue to the given output stream (in JSON format) and returns a reference to the modified ostream.

            */
            friend std::ostream& operator<<(std::ostream& out, CommandQueue& cmdq);


        }; // end of Command Queue Class
    } // end of namespace Support
} // end of namepsace Cosmos

#endif // COSMOS_CommandQueue_H
