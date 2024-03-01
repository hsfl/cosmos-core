/**
 * @file command_queue.h
 * @brief Command Queue Class
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

#ifndef COSMOS_CommandQueue_H
#define COSMOS_CommandQueue_H

/**
 * @ingroup support
 * @defgroup agentlib Agent Server and Client Library
 * %Agent Server and Client.
 */

#include "support/configCosmos.h"
#include "support/stringlib.h"
#include "support/jsonlib.h"
#include "agentclass.h"
#include "event.h"

namespace Cosmos
{
    namespace Support
    {
        /** @todo Document this. */
        namespace Command
        {
            int32_t Shell(string command_line="", string outpath="", string inpath="", string errpath="");
        }

        /** @brief Class to manage information about a queue of Events */
        class CommandQueue
        {
        private:
            /**	An std::list of members of the Event class to be run */
            std::list<Event> commands;
            /**	An std::queue of members of the Event class that have run */
            std::deque<Event> events;
            /** A vector of all threads spawned to run events */
            vector<thread> event_threads;
            /** A boolean indicator that the queue has changed */
            bool queue_changed = false;
            /** @todo Document this. */
            bool queue_blocked = false;

        public:
            ~CommandQueue();

            size_t join_event_threads();
            int32_t run_request(Agent *agent, Event &cmd, string nodename, double logdate_exec);
            int32_t run_command(Event &cmd, string nodename, double logdate_exec);
            int32_t run_commands(Agent *agent, string nodename, double logdate_exec);
            int32_t save_commands(string temp_dir, string name=".queue");
            int32_t restore_commands(string temp_dir, string name=".queue");
            int32_t load_commands(string incoming_dir);
            int32_t del_command(Event& c);
            int32_t del_command(int pos);
            int32_t add_command(Event& c);
            friend ::std::ostream& operator<<(::std::ostream& out, CommandQueue& cmdq);

            /**
             * @brief Get the event size object
             * 
             * Retrieve the size of the queue
             * 
             * @return size_t The size of the queue
             * 
             * @todo Move to .cpp file
             */
            size_t get_event_size() { return events.size(); }

            /**
             * @brief Get the event object
             * 
             * Retrieve an Event by its position in the queue
             * 
             * @param i Integer representing the position in the queue
             * @return Event& Reference to the ith Event
             * 
             * @todo Move to .cpp file
             */
            Event& get_event(int i)
            {
                std::deque<Event>::iterator ii = events.begin();
                std::advance(ii,i);
                return *ii;
            }

            /**
             * @brief Get the command size object
             * 
             * Retrieve the size of the queue
             * 
             * @return size_t The size of the queue
             * 
             * @todo Move to .cpp file
             */
            size_t get_command_size() { return commands.size(); }

            /**
             * @brief Get the command object
             * 
             * Retrieve an Event by its position in the queue
             * 
             * @param i Integer representing the position in the queue
             * @return Event& Reference to the ith Event
             * 
             * @todo Move to .cpp file
             */
            Event& get_command(int i)
            {
                std::list<Event>::iterator ii = commands.begin();
                std::advance(ii,i);
                return *ii;
            }

            /**
             * @brief Sort the Events in the queue by Event execution time
             * 
             * This function is called after new Events are loaded.
             * 
             * @todo Move to .cpp file
             */
            void sort()	{ commands.sort([](Event & c1, Event & c2) { return c1.getTime() < c2.getTime(); }); }

            
            /** @todo Remove commented-out code? */
            //!	Traverse the entire queue of Events, clearing those that have finished.
            /*!

            An %Event qualifies to be cleared if its thread could be joined, or if it has been
            running for more than 1 minute.

                \param	agent	Pointer to Agent object (for call to condition_true(..))
                \param	nodename	Name of the node
                \param	logdate_exec	Time of execution (for logging purposes)
            */
//            int32_t flush_commands();

        
            

            

            
            


        }; // end of Command Queue Class
    } // end of namespace Support
} // end of namespace Cosmos

#endif // COSMOS_CommandQueue_H
