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

#ifndef _COSMOS_ERRORCLASS_H
#define _COSMOS_ERRORCLASS_H 1

#include "support/configCosmos.h"
#include "support/cosmos-errno.h"
#include <errno.h>

namespace Cosmos {
    namespace Support {
        class Error {
        public:
            Error();

            typedef  std::map<int16_t, string> ErrorMap;
            ErrorMap ErrorStrings;

            enum LogType
                {
                LOG_NONE = 0,
                LOG_STDOUT_FAST = 1,
                LOG_STDOUT_FFLUSH = 2,
                LOG_FILE_FAST = 3,
                LOG_FILE_FFLUSH = 4,
                LOG_FILE_CLOSE = 5
                };

            string ErrorString(int16_t number);
            int32_t Set(uint16_t type=0, string ipathname="", double iinterval=1800., string iextension="log");
            int32_t Type(uint16_t type);
            int32_t Type();
            FILE *Open();
            int32_t Close();
            int32_t Printf(string output);
            int32_t Printf(const char *fmt, ...);


        private:
            double interval = 30. / 1440.;
            double oldmjd = 0.;
            uint16_t type = 0;
            string pathName;
            string Extension = "log";
            FILE *log_fd = nullptr;
        };

        //! \ingroup error
        //! \defgroup error_functions Errno support functions
        //! @{
        string cosmos_error_string(int32_t cosmos_errno);
        int32_t set_cosmos_error_level(uint16_t level=0, string pathname="./");
        FILE* get_cosmos_error_fd(double mjd=0.);
        int32_t close_cosmos_error_fd();

#define cosmos_error_printf(mjd, ...) \
    do { \
    if ((FILE* fd = get_cosmos_error_fd(mjd, path) != nullptr) \
        { \
    char _buf[512]; \
    fprintf(fd, __VA_ARGS__); \
    } \
    } while(0);


        //! @}
    }
}

#endif
