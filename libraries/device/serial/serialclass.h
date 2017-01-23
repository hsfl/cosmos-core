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

#ifndef SERIALCLASS_H
#define SERIALCLASS_H

#include "support/configCosmos.h"
#include "support/sliplib.h"


namespace Cosmos {
    class Serial
    {
    public:
        Serial(string dname, int32_t dbaud, size_t dbits, size_t dparity, size_t dstop);
        ~Serial();
        int32_t set_params(int32_t dbaud, size_t dbits, size_t dparity, size_t dstop);
        int32_t set_flowcontrol(bool rtscts, bool xonxoff);
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
        int32_t set_timeout(int minchar, double timeout);
#else // Windows
        int32_t set_timeout(int, double timeout);
#endif
        int32_t put_char(uint8_t c);
        int32_t put_string(string data);
        int32_t put_data(vector <uint8_t> data);
        int32_t put_slip(vector <uint8_t> data);
        int32_t put_nmea(vector <uint8_t> data);
        int32_t drain();
        int32_t get_char();
        int32_t get_data(vector <uint8_t> &data, size_t size);
        int32_t get_slip(vector <uint8_t> &data, size_t size);
        int32_t get_nmea(vector <uint8_t> &data, size_t size);
        int32_t get_xmodem(vector <uint8_t> &data, size_t size);
        int32_t get_error();


    private:
        int fd = -1;                   /* tty file descriptor */
        int32_t error;
        vector <int32_t> baud_speed[2] = {
            {0, 50, 75, 110, 134, 150, 200, 300, 500, 1200, 1800, 2400, 4800, 9600, 19200, 38400},
            {57600, 115200, 230400, 460800, 500000, 576000, 921600, 1000000, 1152000, 1500000, 2000000, 2500000, 3000000, 3500000, 4000000}
        };

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
        struct termios tio;       /* termios structure for the port */
        struct termios oldtio;    /* old termios structure */
#else // windows
        struct _DCB dcb;
        struct _DCB olddcb;
        HANDLE handle;
#endif

        string name;
        size_t baud;
        size_t bits;
        size_t parity;
        size_t stop;

#define XMODEM_SOH 0x01
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18

    };


} // end of namepsace Cosmos
#endif // SERIALCLASS_H
