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

#ifndef _COSMOS_LOG_H
#define _COSMOS_LOG_H 1

#include "support/configCosmos.h"
#include "support/elapsedtime.h"
#include "support/timelib.h"
#include "support/stringlib.h"
#include "support/datalib.h"
#include "support/socketlib.h"
//#include <stdarg.h>
//#include <errno.h>

namespace Cosmos {
    namespace Support {
        namespace Log {

            enum Level {
                TELEMETRY, DATA, INFO, WARNING, CRITICAL
            };

            static unordered_map<Level, string> LevelString = {
                {Level::TELEMETRY, "Telemetry"},
                {Level::DATA, "Data"},
                {Level::INFO, "Info"},
                {Level::WARNING, "Warning"},
                {Level::CRITICAL, "Critical"}
            };

            class Monitor
            {
            public:

                Monitor() {}

                int32_t Init(const string address, const uint16_t port);

                /**
                 * @brief
                 *
                 * @param level Severity level of the message
                 * @param location Where the message is from
                 * @param msg The actual string message to log
                 * @return 0 on success, negative on error
                 */
                int32_t log_msg(Level level, string location, std::string &msg);
                int32_t log_msg(Level level, string location, json11::Json &msg);

            private:
                //! IP Address of logging database
                static string address;
                //! Port of logging database
                static uint16_t port;
                static socket_bus sock;
                //! True if Init() was successful
                static bool ready;
            };

            enum LogType
                {
                LOG_NONE = 0,
                LOG_STDOUT_FAST = 1,
                LOG_STDOUT_FFLUSH = 2,
                LOG_FILE_FAST = 3,
                LOG_FILE_FFLUSH = 4,
                LOG_FILE_CLOSE = 5
                };

            class Logger {
            public:
                Logger() {}

                int32_t Set(uint16_t type, string ipathname="", double iinterval=1800., string iextension="log");
                int32_t Set(uint16_t type, bool timestamped, string ipathname="", double iinterval=1800., string iextension="log");
                int32_t Type();
                FILE *Open();
                int32_t Close();
                int32_t Printf(string output);
                int32_t Printf(const char *fmt, ...);


            private:
                bool timestamped = false;
                double interval = 30. / 1440.;
                double oldmjd = 0.;
                uint16_t type = 0;
                string pathName;
                string Extension = "log";
                FILE *log_fd = nullptr;
                ElapsedTime et;
            };

        }
    }
}

#endif
