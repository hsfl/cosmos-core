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

#ifndef TASK_H
#define TASK_H

/*! \file task.h
*	\brief Task Class
*/

//! \ingroup support
//! \defgroup agentlib Agent Server and Client Library
//! %Agent Server and Client.
//!

#include "support/configCosmos.h"
#include "support/datalib.h"
#include "support/timelib.h"
#include <future>

namespace Cosmos
{
    namespace Support
    {

        class Task
        {
        public:
            Task(string node="", string agent="");
            ~Task();
            void Runner();
            int32_t Add(string command);
            int32_t Del(uint32_t deci);
            int32_t Iretn(uint16_t number);
            uint32_t Deci(uint16_t number);
            double Startmjd(uint16_t number);
            uint8_t State(uint16_t number);
            string Command(uint16_t number);
            string Path(uint16_t number);
            uint16_t Size();

            struct Running
            {
                std::future<int32_t> result;
                double startmjd = 0.;
                uint8_t state = 0;
                string command = "";
//                thread mythread;
                int32_t iretn = 0;
                string path = "";
//                string input;
                double runtime = 0.;
                uint32_t outsize = 0;
            };

        private:
            string NodeName;
            string AgentName;
            vector<Running> tasks;
            mutex mtx;
            uint8_t state = 0;
            thread mythread;
        };
    }
}

#endif // TASK_H
