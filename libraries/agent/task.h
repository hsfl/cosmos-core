/**
 * @file task.h
 * @brief Task class
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

#ifndef TASK_H
#define TASK_H

/**
 * @ingroup support
 * @defgroup agentlib Agent Server and Client Library
 * %Agent Server and Client.
 */

#include "support/configCosmos.h"
#include "support/datalib.h"
#include "support/timelib.h"
#include <future>
#include <mutex>
#include <thread>

namespace Cosmos
{
    namespace Support
    {
        /** @todo Document this. */
        class Task
        {
        public:
            Task(string node="", string agent="");
            ~Task();
            void Start();
            void Runner();
            int32_t Add(string command, string node="");
            int32_t Del(uint32_t deci);
            int32_t Iretn(uint16_t number);
            uint32_t Deci(uint16_t number);
            double Startmjd(uint16_t number);
            uint8_t State(uint16_t number);
            string Command(uint16_t number);
            string Path(uint16_t number);
            uint16_t Size();

            /** @todo Document this. */
            struct Running
            {
                /** @todo Document this. */
                std::future<int32_t> result;
                /** @todo Document this. */
                double startmjd = 0.;
                /** @todo Document this. */
                uint8_t state = 0;
                /** @todo Document this. */
                string command = "";
                /** @todo Remove commented-out code? */
//                thread mythread;
                /** @todo Document this. */
                int32_t iretn = 0;
                /** @todo Document this. */
                string path = "";
                /** @todo Remove commented-out code? */
//                string input;
                /** @todo Document this. */
                uint32_t outsize = 0;
            };

        private:
            /** @todo Document this. */
            string NodeName;
            /** @todo Document this. */
            string AgentName;
            /** @todo Document this. */
            vector<Running> tasks;
            /** @todo Document this. */
            mutex mtx;
            /** @todo Document this. */
            uint8_t state = 0;
            /** @todo Document this. */
            thread mythread;
        };
    }
}

#endif // TASK_H
