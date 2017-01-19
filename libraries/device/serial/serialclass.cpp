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
    Serial::Serial(string dname, size_t dbaud, size_t dbits, size_t dparity, size_t dstop)
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

    int32_t Serial::configure(size_t dbaud, size_t dbits, size_t dparity, size_t dstop)
    {
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
    tcflag_t baudrate;
    tcflag_t databits;
    tcflag_t stopbits;
    tcflag_t checkparity;
#endif

    if (fd < 0)
    {
        return SERIAL_ERROR_OPEN;
    }

    // Set baud rate
//    serial->baud = baud;
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
    size_t baud_index;
    bool baud_high_flag = false;
    if (baud > (38400+57600)/2))
    {
        baud_high_flag = true;
        baud_index = (log10f((float)baud) - 4.7604) / .118 + .5;
    }
    else
    {
        baud_index = (log10f((float)baud) - 1.699) / .2025 + .5;
    }

    do
    {

    } while

    switch (baud) {
    case 75:
        baudrate=B75;
        break;
    case 110:
        baudrate=B110;
        break;
    case 150:
        baudrate=B150;
        break;
    case 300:
        baudrate=B300;
        break;
    case 600:
        baudrate=B600;
        break;
    case 1200:
        baudrate=B1200;
        break;
    case 2400:
        baudrate=B2400;
        break;
    case 4800:
        baudrate=B4800;
        break;
    case 9600:
        baudrate=B9600;
        break;
    case 19200:
        baudrate=B19200;
        break;
    case 38400:
        baudrate=B38400;
        break;
    case 57600:
        baudrate=B57600;
        break;
    case 115200:
        baudrate=B115200;
        break;
    default:
        baudrate=B9600;
    }

    /* databits */
    switch (bits) {
    case 7:
        databits=CS7;
        break;
    case 8:
        databits=CS8;
        break;
    default:
        databits=CS8;
    }

    /* parity, */
    switch (parity) {
    case 0:
        checkparity=0;
        break;
    case 1:   /* odd */
        checkparity=PARENB|PARODD;
        break;
    case 2:
        checkparity=PARENB;
        break;
    default:
        checkparity=0;
    }

    /* and stop bits */
    switch (stop) {
    case 1:
        stopbits=0;
        break;
    case 2:
        stopbits=CSTOPB;
        break;
    default:
        stopbits=0;
    }

    /* now we setup the values in port's termios */
    tio.c_cflag=baudrate|databits|checkparity|stopbits|CLOCAL|CREAD;
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
