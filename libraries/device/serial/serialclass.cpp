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

#include "support/configCosmos.h"
#include "device/serial/serialclass.h"

namespace Cosmos {

    //! \ingroup seriallib
    //! \ingroup seriallib
    //! \defgroup seriallib_functions Serial Port functions
    //! @{

    //! Create serial port instance.
    //! Create a serial port object to be used for reading and writing to a physical serial port.
    //! \param dname Name of physical serial port.
    //! \param baud Baud rate. Will be rounded to nearest of 75, 110, 150, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200.
    //! \param bits Number of data bits.
    //! \param parity 0 = even, 1 = odd.
    //! \param stop Number of stop bits.
    Serial::Serial(string dname, int32_t dbaud, size_t dbits, size_t dparity, size_t dstop)
    {
        int32_t iretn;

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS)
        fd=open(dname.c_str(), O_RDWR | O_NOCTTY);
#endif

#if  defined(COSMOS_MAC_OS)
        fd=open(dname.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
#endif

#if defined(COSMOS_WIN_OS)
        handle = CreateFileA(dname.c_str(), GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

        if(handle != INVALID_HANDLE_VALUE)
        {
            fd = _open_osfhandle((intptr_t)handle, 0);
        }

#endif

        if (fd == -1)
        {
            error = SERIAL_ERROR_OPEN;
        }

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
        tcgetattr(fd,&(oldtio));
#endif

        error = configure(dbaud, dbits, dparity, dstop);
        name = dname;
    }

    Serial::~Serial()
    {
        if (fd >= 0)
        {
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
            tcflush(fd,TCOFLUSH);
            tcflush(fd,TCIFLUSH);
#endif

            /* then we restore old settings */
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
            tcsetattr(fd,TCSANOW,&(oldtio));
#else
            SetCommState(handle, &(dcb));
#endif

            /* and close the file */
            close(fd);
        }
    }

    int32_t Serial::get_error()
    {
        return error;
    }

    int32_t Serial::configure(int32_t dbaud, size_t dbits, size_t dparity, size_t dstop)
    {
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
        tcflag_t trate;
        tcflag_t tbits;
        tcflag_t tstop;
        tcflag_t tparity;
#endif

        if (fd < 0)
        {
            return SERIAL_ERROR_OPEN;
        }

        // Set baud rate
        //    serial->baud = baud;
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
        size_t baud_index;
        bool baud_speed_index = 0;
        if (dbaud > (38400+57600)/2))
        {
            baud_speed_index = 1;
            baud_index = (log10f((float)dbaud) - 4.7604) / .118 + .5;
        }
        else
        {
            baud_index = (log10f((float)dbaud) - 1.699) / .2025 + .5;
        }

        do
        {
            bool baud_adjust = false;
            int32_t baud_diff = abs(dbaud - baud_speed[baud_speed_index][baud_index]);
            if (baud_index > 0)
            {
                int32_t new_baud_diff = abs(dbaud - baud_speed[baud_speed_index][baud_index-1]);
                if (new_baud_diff < baud_diff)
                {
                    --baud_index;
                    baud_diff = new_baud_diff;
                    baud_adjust = true;
                }
            }
            if (baud_index < baud_speed.size()-1)
            {
                int32_t new_baud_diff = abs(dbaud - baud_speed[baud_speed_index][baud_index+1]);
                if (new_baud_diff < baud_diff)
                {
                    ++baud_index;
                    baud_diff = new_baud_diff;
                    baud_adjust = true;
                }
            }
        }
    } while (baud_adjust);

    switch (baud_index)
    {
        case 1:
            trate = B57600 + baud_index;
            break;
        default:
            trate = baud_index;
            break;
    }

    /* databits */
    switch (dbits) {
        case 7:
            tbits=CS7;
            break;
        case 8:
            tbits=CS8;
            break;
        default:
            tbits=CS8;
    }

    /* parity, */
    switch (dparity) {
        case 0:
            tparity=0;
            break;
        case 1:   /* odd */
            tparity=PARENB|PARODD;
            break;
        case 2:
            tparity=PARENB;
            break;
        default:
            tparity=0;
    }

    /* and stop bits */
    switch (dstop) {
        case 1:
            tstop=0;
            break;
        case 2:
            tstop=CSTOPB;
            break;
        default:
            tstop=0;
    }

    /* now we setup the values in port's termios */
    tio.c_cflag=trate|tbits|tparity|tstop|CLOCAL|CREAD;
    tio.c_iflag=IGNPAR;
    tio.c_oflag=0;
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=0;

    /* we flush the port */
    tcflush(fd,TCOFLUSH);
    tcflush(fd,TCIFLUSH);

    /* we send new config to the port */
    tcsetattr(fd,TCSANOW,&(tio));
#else // windows
        dcb.BaudRate = baud;
        dcb.Parity = parity;
        dcb.StopBits = stop;
        dcb.ByteSize = bits;
        dcb.DCBlength = sizeof(DCB);
        SetCommState(handle, &(dcb));
#endif


    }

} // end of namepsace Cosmos
