/**
 * @file scheduler.h
 * @brief Scheduler Class
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

#ifndef COSMOS_SCHEDULER_QUEUE_H
#define COSMOS_SCHEDULER_QUEUE_H

/**
 * @ingroup support
 * @defgroup agentlib Agent Server and Client Library
 * %Agent Server and Client.
 */

#include "support/configCosmos.h"
#include "agentclass.h"
#include "event.h"

using std::cout;
using std::endl;

namespace Cosmos
{
    namespace Support
    {
        /**
         * @brief 
         * 
         * @todo Document this.
         */
        class Scheduler
        {
        private:
            /** @todo Document this. */
            Agent *agent;
            /** @todo Document this. */
            beatstruc agent_exec_soh;
        public:
            Scheduler(string node_name);
            ~Scheduler();

            void addEvent(string name, string data, double utc, string condition, uint32_t flag);
            void addEvent(Event event);
            void deleteEvent(string name, string data, double utc, string condition, uint32_t flag);
            void deleteEvent(Event event);
            int getEventQueueSize();
            void getEventQueue();
        }; // end of Scheduler Class
    } // end of namespace Support
} // end of namespace Cosmos

#endif // COSMOS_SCHEDULER_QUEUE_H
