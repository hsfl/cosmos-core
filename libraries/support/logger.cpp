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

/*! \file logger.cpp
        \brief COSMOS Logging support library source file
*/

#include "support/logger.h"

namespace Cosmos {
    namespace Support {
        namespace Log {

            // Static vars
            string Monitor::address;
            uint16_t Monitor::port;
            socket_bus Monitor::sock;
            bool Monitor::ready = false;
            // End static vars

            int32_t Monitor::Init(const string address, const uint16_t port)
            {
                if (address.empty())
                {
                    return COSMOS_GENERAL_ERROR_ARGS;
                }

            //    int32_t iretn = socket_open(&sock, NetworkType::UDP, address.c_str(), port, SOCKET_TALK, SOCKET_BLOCKING, 2000000);
                int32_t iretn = socket_publish(sock, port);
                if ((iretn) < 0)
                {
                    return iretn;
                }

                ready = true;
                return 0;
            }

            int32_t Monitor::log_msg(Level level, string location, string& msg)
            {
                if (!ready)
                {
                    return SOCKET_ERROR_OPEN;
                }

                string slevel;
                switch(level)
                {
                case Level::INFO:
                    slevel = "info";
                    break;
                case Level::WARNING:
                    slevel = "warning";
                    break;
                case Level::CRITICAL:
                    slevel = "critical";
                    break;
                default:
                    return COSMOS_GENERAL_ERROR_ARGS;
                }
                json11::Json log = json11::Json::object {
                    { "level", slevel },
                    { "location", location },
                    { "msg", msg },
                };

                // Send to cosmos web logging database
                socket_post(sock, log.dump());
                return 0;
            }

            int32_t Monitor::log_msg(Level level, string location, json11::Json& msg)
            {
                if (!ready)
                {
                    return SOCKET_ERROR_OPEN;
                }

                string slevel;
                if (LevelString.find(level) != LevelString.end())
                {
                    slevel = LevelString[level];
                }
                else
            //    switch(level)
            //    {
            //    case Level::INFO:
            //        slevel = "info";
            //        break;
            //    case Level::WARNING:
            //        slevel = "warning";
            //        break;
            //    case Level::CRITICAL:
            //        slevel = "critical";
            //        break;
            //    default:
                {
                    return COSMOS_GENERAL_ERROR_ARGS;
                }
                json11::Json log = json11::Json::object {
                    { "level", slevel },
                    { "location", location },
                    { "msg", msg },
                };

                // Send to cosmos web logging database
                socket_post(sock, log.dump());
                return 0;
            }


            int32_t Logger::Type()
            {
                return type;
            }

            int32_t Logger::Set(uint16_t itype, string ipathname, double iinterval, string iextension)
            {
                return Set(itype, true, ipathname, iinterval, iextension);
            }

            int32_t Logger::Set(uint16_t itype, bool itimestamped, string ipathname, double iinterval, string iextension)
            {
                timestamped = itimestamped;
                interval = iinterval;
                Extension = iextension;
                type = itype;
                switch (type)
                {
                case LOG_NONE:
                    if (log_fd != nullptr && log_fd != stdout) {
                        fclose(log_fd);
                    }
                    log_fd = nullptr;
                    pathName.clear();
                    break;
                case LOG_STDOUT_FAST:
                case LOG_STDOUT_FFLUSH:
                    if (log_fd != nullptr && log_fd != stdout) {
                        fclose(log_fd);
                    }
                    log_fd = stdout;
                    pathName.clear();
                    break;
                case LOG_FILE_CLOSE:
                case LOG_FILE_FAST:
                case LOG_FILE_FFLUSH:
                default:
                    if (ipathname.empty())
                    {
                        pathName = data_base_path("", "temp", "log");
                    }
                    else
                    {
                        pathName = ipathname;
                    }
                    break;
                }
                return type;
            }

            FILE* Logger::Open()
            {
                switch (type)
                {
                case LOG_NONE:
                    log_fd = nullptr;
                    pathName.clear();
                    break;
                case LOG_STDOUT_FAST:
                case LOG_STDOUT_FFLUSH:
                    if (log_fd != stdout) {
                        if (log_fd != nullptr) {
                            fclose(log_fd);
                        }
                        log_fd = stdout;
                        pathName.clear();
                    }
                    break;
                default:
                    double mjd = currentmjd();
                    mjd -= fmod(mjd - floor(mjd), interval);
                    if (fabs(mjd - oldmjd) > interval / 2.)
                    {
                        string npathName;
                        if (Extension == "log")
                        {
                            npathName = pathName + data_name(mjd, Extension + to_unsigned(type));
                        }
                        else {
                            npathName = pathName + data_name(mjd, Extension);
                        }

                        if (log_fd != nullptr) {
                            if (npathName != pathName) {
                                FILE *fd = fopen(npathName.c_str(), "a");
                                if (fd != nullptr) {
                                    if (log_fd != stdout) {
                                        fclose(log_fd);
                                    }
                                    log_fd = fd;
                                    pathName = npathName;
                                }
                            }
                        }
                        else {
                            FILE *fd = fopen(npathName.c_str(), "a");
                            if (fd != nullptr) {
                                log_fd = fd;
                                pathName = npathName;
                            }
                        }
                        oldmjd = mjd;
                    }
                    break;
                }
                return log_fd;
            }

            int32_t Logger::Close()
            {
                int32_t iretn = 0;
                if (log_fd != nullptr && log_fd != stdout)
                {
                    iretn = fclose(log_fd);
                    if (iretn != 0) { return -errno; }
                    log_fd = nullptr;
                }
                return 0;
            }

            int32_t Logger::Printf(string output)
            {
                return Printf(output.c_str());
            }

            int32_t Logger::Printf(const char* fmt, ...)
            {
                int32_t iretn = 0;

                Open();
                if (log_fd != nullptr)
                {
                    if (timestamped)
                    {
                        fprintf(log_fd, "[%.3f] ", et.split());
                    }
                    va_list args;
                    va_start(args, fmt);
                    iretn = vfprintf(log_fd, fmt, args);
                    va_end(args);
                    switch (type)
                    {
                    case LOG_STDOUT_FFLUSH:
                    case LOG_FILE_FFLUSH:
                        fflush(log_fd);
                        break;
                    case LOG_FILE_CLOSE:
                        fclose(log_fd);
                        break;
                    default:
                        break;
                    }
                }

                return iretn;
            }

        }
    }
}


//! @}
