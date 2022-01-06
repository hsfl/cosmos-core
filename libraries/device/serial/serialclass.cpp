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

// TODO: rename to serial.cpp only
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
    //! \param dbaud Baud rate. Will be rounded to nearest of 75, 110, 150, 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200.
    //! \param dbits Number of data bits.
    //! \param dparity 0 = none, 1 = odd, 2 = even.
    //! \param dstop Number of stop bits.
    Serial::Serial(string dname, size_t dbaud, size_t dbits, string dparity, size_t dstop)
    {
        if (Parity.count(dparity))
        {
            parity = Parity[dparity];
        }
        else
        {
            parity = Parity["none"];
        }
        name = dname;
        baud = dbaud;
        bits = dbits;
        stop = dstop;

        error = open_device();
    }

    int32_t Serial::open_device()
        {
        if (get_open())
        {
            error = SERIAL_ERROR_OPEN;
        }

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS)
        fd=open(name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1)
        {
            error = -errno;
            return error;
        }
        fcntl(fd, F_SETFL, FNDELAY);
#endif

#if  defined(COSMOS_MAC_OS)
        fd=open(name.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
        if (fd == -1)
        {
            error = -errno;
            return error;
        }
#endif

#if defined(COSMOS_WIN_OS)
        // using TCHAR we can directly send COM1 to the string
        // rather than \\\\.\\COM1
        TCHAR *port=new TCHAR[name.size()+1];
        port[name.size()]=0;
        std::copy(name.begin(),name.end(),port);

        handle = CreateFileA(port,
                             GENERIC_READ|GENERIC_WRITE,
                             0,     //  must be opened with exclusive-access
                             NULL,  //  default security attributes
                             OPEN_EXISTING,  //  must use OPEN_EXISTING
                             0,     //  not overlapped I/O, no threads
                             NULL); //  hTemplate must be NULL for comm devices

        if (handle == INVALID_HANDLE_VALUE) {
            cout << "CreateFile failed with error " << GetLastError() << endl;
            //cout << "error opening serial port" << endl;
        } else {
            // sucess opening serial port
            fd = _open_osfhandle((intptr_t)handle, _O_RDONLY); // flag = 0
            //        fd=open(name.c_str(), O_RDWR | O_NOCTTY);
            if (fd == -1)
            {
                error = -WSAGetLastError();
                return error;
            }
        }

#endif

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

        tcgetattr(fd,&(oldtio));
#endif

        error = set_params(baud, bits, parity, stop);
        return error;
    }

    int32_t Serial::close_device()
    {
        if (fd >= 0)
        {
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
            tcflush(fd,TCOFLUSH);
            tcflush(fd,TCIFLUSH);
#endif

            if (restoreonclose)
            {
                /* then we restore old settings */
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
                tcsetattr(fd,TCSANOW,&(oldtio));
#else
                SetCommState(handle, &(dcb));
#endif
            }

            /* and close the file */
            close(fd);
            fd = -1;
        }
        error = 0;
        return error;
    }

    Serial::~Serial()
    {
        close_device();
    }

    bool Serial::get_open() const
    {
        if (fd >= 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int32_t Serial::get_error()
    {
        return error;
    }

    int32_t Serial::set_restoreonclose(bool argument)
    {
        restoreonclose = argument;
        return 0;
    }

    int32_t Serial::set_params(size_t dbaud, size_t dbits, size_t dparity, size_t dstop)
    {
        baud = dbaud;
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
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
        uint32_t baud_index;
        bool baud_speed_index = 0;
        if (dbaud > (38400+57600)/2)
        {
            baud_speed_index = 1;
            baud_index = static_cast<uint32_t>((log10f(dbaud) - 4.7604F) / .118F + .5F);
        }
        else
        {
            baud_index = static_cast<uint32_t>((log10f(dbaud) - 1.699F) / .2025F + .5F);
        }

        bool baud_adjust = false;
        do
        {
            baud_adjust = false;
            int32_t baud_diff = std::abs(static_cast<int32_t>(dbaud) - static_cast<int32_t>(baud_speed[baud_speed_index][baud_index]));
            if (baud_index > 0)
            {
                int32_t new_baud_diff = std::abs(static_cast<int32_t>(dbaud) - static_cast<int32_t>(baud_speed[baud_speed_index][baud_index-1]));
                if (new_baud_diff < baud_diff)
                {
                    --baud_index;
                    baud_diff = new_baud_diff;
                    baud_adjust = true;
                }
            }
            if (baud_index < baud_speed[baud_speed_index].size()-1)
            {
                int32_t new_baud_diff = std::abs(static_cast<int32_t>(dbaud) - static_cast<int32_t>(baud_speed[baud_speed_index][baud_index+1]));
                if (new_baud_diff < baud_diff)
                {
                    ++baud_index;
                    baud_diff = new_baud_diff;
                    baud_adjust = true;
                }
            }
        } while (baud_adjust);
        baud = baud_speed[baud_speed_index][baud_index];
        set_timeout(20. / baud);

        if (baud_speed_index)
        {
            trate = B57600 + baud_index;
        }
        else
        {
            trate = baud_index;
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
        tio.c_cc[VMIN] = 1;
        tio.c_cc[VTIME] = 0;

        /* we flush the port */
        tcflush(fd,TCOFLUSH);
        tcflush(fd,TCIFLUSH);

        /* we send new config to the port */
        tcsetattr(fd,TCSANOW,&(tio));
#else
        // windows

        //  Initialize the DCB structure.
        SecureZeroMemory(&dcb, sizeof(DCB));
        dcb.DCBlength = sizeof(DCB);

        //  Build on the current configuration by first retrieving all current
        //  settings.
        BOOL fSuccess = GetCommState(handle, &dcb);
        if (!fSuccess)
        {
            //  Handle the error.
            cout << "GetCommState failed with error " << GetLastError() << endl;
            //return (2);
        }

        //PrintCommState(dcb);       //  Output to console

        printf("\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n",
               dcb.BaudRate,
               dcb.ByteSize,
               dcb.Parity,
               dcb.StopBits);

        //  Fill in DCB values and set the com state:
        //  Example: 115,200 bps, 8 data bits, no parity, and 1 stop bit.

        dcb.fBinary = true; // Windows does not support nonbinary mode transfers, so this member must be TRUE.
        dcb.BaudRate = dbaud; // baud rate
        dcb.ByteSize = dbits; // number of data bits must be between 5 to 8
        dcb.Parity = dparity; // If this member is TRUE, parity checking is performed and errors are reported.
        if (dstop == 1) {
            dcb.StopBits = ONESTOPBIT; // 0 = 1 stop bit, see https://msdn.microsoft.com/en-us/library/windows/desktop/aa363214(v=vs.85).aspx
        }
        if (dstop == 1.5) {
            dcb.StopBits = ONE5STOPBITS; // 1 = 1.5 stop bits, see https://msdn.microsoft.com/en-us/library/windows/desktop/aa363214(v=vs.85).aspx
        }
        if (dstop == 2) {
            dcb.StopBits = TWOSTOPBITS; // 2 = 2 stop bits, see https://msdn.microsoft.com/en-us/library/windows/desktop/aa363214(v=vs.85).aspx
        }

        fSuccess = SetCommState(handle, &dcb);

        if (!fSuccess)
        {
            //  Handle the error.
            cout << "SetCommState failed with error " << GetLastError();
            return (3);
        }


        //    COMMTIMEOUTS Cptimeouts;

        //    Cptimeouts.ReadIntervalTimeout         = MAXDWORD;
        //    Cptimeouts.ReadTotalTimeoutMultiplier  = 0;
        //    Cptimeouts.ReadTotalTimeoutConstant    = 0;
        //    Cptimeouts.WriteTotalTimeoutMultiplier = 0;
        //    Cptimeouts.WriteTotalTimeoutConstant   = 0;

        //    if(!SetCommTimeouts(handle, &Cptimeouts))
        //    {
        //        //printf("unable to set comport time-out settings\n");
        //        //CloseHandle(Cport[comport_number]);
        //        //return(1);
        //    }


        //  Get the comm config again.
        fSuccess = GetCommState(handle, &dcb);

        if (!fSuccess)
        {
            //  Handle the error.
            cout << "GetCommState failed with error " << GetLastError();
            //       return (2);
        }

        cout << "Serial port sucessfully configured" << endl;

#endif // windows

        return 0;
    }

    int32_t Serial::set_flowcontrol(bool rtscts, bool xonxoff)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

#if defined(COSMOS_WIN_OS)
        GetCommState(handle, &(dcb));
        if (rtscts)
        {
            dcb.fRtsControl	= RTS_CONTROL_HANDSHAKE;
            dcb.fOutxCtsFlow = TRUE;

        }
        else
        {
            dcb.fRtsControl	= RTS_CONTROL_DISABLE;
            dcb.fOutxCtsFlow = FALSE;
        }
#else
        /* We setup rts/cts (hardware) flow control */
        if (rtscts)
        {
            tio.c_cflag |= CRTSCTS;
        } else
        {
            tio.c_cflag &= ~CRTSCTS;
        }
#endif

#if defined(COSMOS_WIN_OS)
        if (xonxoff)
        {
            dcb.fInX = dcb.fOutX = TRUE;
        }
        else
        {
            dcb.fInX = dcb.fOutX = FALSE;

        }
        SetCommState(handle, &(dcb));
#else
        /* We setup xon/xoff (soft) flow control */
        if (xonxoff)
        {
            tio.c_iflag |= (IXON|IXOFF);
        }
        else
        {
            tio.c_iflag &= ~(IXON|IXOFF);
        }

        tcsetattr(fd,TCSANOW,&(tio));
#endif

        error = 0;
        return error;
    }

    int32_t Serial::set_dtr(bool state)
    {
#if defined(COSMOS_WIN_OS)
        GetCommState(handle, &(dcb));
        if (state)
        {
            dcb.fDtrControl	= DTR_CONTROL_ENABLE;
        }
        else
        {
            dcb.fDtrControl	= DTR_CONTROL_DISABLE;
        }
#elif defined(COSMOS_LINUX_OS)
        int flag = TIOCM_DTR;

        if (state)
        {
            ioctl(fd, TIOCMBIS, &flag);
        }
        else
        {
            ioctl(fd, TIOCMBIC, &flag);
        }
#endif
        return 0;
    }

    int32_t Serial::set_rts(bool state)
    {
#if defined(COSMOS_WIN_OS)
        GetCommState(handle, &(dcb));
        if (state)
        {
            dcb.fDtrControl	= RTS_CONTROL_ENABLE;
        }
        else
        {
            dcb.fDtrControl	= RTS_CONTROL_DISABLE;
        }
#elif defined(COSMOS_LINUX_OS)
        int flag = TIOCM_RTS;

        if (state)
        {
            ioctl(fd, TIOCMBIS, &flag);
        }
        else
        {
            ioctl(fd, TIOCMBIC, &flag);
        }
#endif
        return 0;
    }

    bool Serial::get_cts()
    {
#if defined(COSMOS_WIN_OS)

#elif defined(COSMOS_LINUX_OS)
        int s;
        ioctl(fd, TIOCMGET, &s);
        return (s & TIOCM_DSR) != 0;
#endif
    }

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
    int32_t Serial::set_timeout(int minchar, double timeout)
#else // Windows
    int32_t Serial::set_timeout(int, double timeout)
#endif
    {
        return set_timeout(timeout);
    }

    int32_t Serial::set_wtimeout(double timeout)
    {
        if (fd < 0)
        {
            return SERIAL_ERROR_OPEN;
        }

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
        wictimeout = timeout;
#else // windows
        _COMMTIMEOUTS timeouts;
        timeouts.WriteTotalTimeoutMultiplier = timeout * 1000.;
        SetCommTimeouts(handle, &timeouts);
#endif

        error=0;
        return 0;
    }

    int32_t Serial::set_rtimeout(double timeout)
    {
        if (fd < 0)
        {
            return SERIAL_ERROR_OPEN;
        }

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
        rictimeout = timeout;
#else // windows
        _COMMTIMEOUTS timeouts;
        timeouts.ReadIntervalTimeout = timeout * 1000.;
        SetCommTimeouts(handle, &timeouts);
#endif

        error=0;
        return 0;
    }

    int32_t Serial::set_timeout(double timeout)
    {
        if (fd < 0)
        {
            return SERIAL_ERROR_OPEN;
        }

//#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
//        ictimeout = timeout;
//#else // windows
//        _COMMTIMEOUTS timeouts;
//        timeouts.ReadIntervalTimeout = timeout * 1000.;
//        SetCommTimeouts(handle, &timeouts);
//#endif

        if ((error = set_rtimeout(timeout)) < 0)
        {
            return error;
        }
        if ((error = set_wtimeout(timeout)) < 0)
        {
            return error;
        }
        return 0;
    }

    // equivalent to put_char
    // this function sends one byte only
    // to send more bytes at once please use 'SendBuffer'
    int32_t Serial::SendByte(uint8_t byte)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

#ifdef COSMOS_WIN_OS
        int n=0;
        // write to port
        WriteFile(handle, &byte, 1, (LPDWORD)((void *)&n), NULL);

        if(n<0)
        {
            return(-errno);
        }

#else
        if (write(fd, &byte, 1) < 0)
        {
            return (-errno);
        }
#endif

        // if all goes well, 0 = sucess
        return 0;
    }



    int32_t Serial::SendBuffer(uint8_t *buffer, int size)
    {
        int32_t n=0;

        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

#ifdef COSMOS_WIN_OS
        // write to port
        WriteFile(handle, buffer, size, (LPDWORD)((void *)&n), NULL);

        if(n<0)
        {
            return(-errno);
        }

#else
        if ((n = write(fd, &buffer, size)) < 0)
        {
            return (-errno);
        }
#endif

        // if all goes well, return the number of bytes sent
        return n;
    }


    int32_t Serial::put_char(uint8_t c)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        int result;

#ifdef COSMOS_WIN_OS
        int n=0;
        WriteFile(handle, &c, 1, (LPDWORD)((void *)&n), NULL);
        if(n<0)
        {
            error = -errno;
        }

#else
        ElapsedTime et;
        do
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(fd, &set);
            timeval timeout;
            double rtimeout = wictimeout - et.split();
            if (rtimeout >= 0.)
            {
                timeout.tv_sec = static_cast<int32_t>(rtimeout);
                timeout.tv_usec = static_cast<int32_t>(1000000. * (rtimeout - timeout.tv_sec));
                int rv = select(fd+1, nullptr, &set, nullptr, &timeout);
                if (rv == -1)
                {
                    error = -errno;
                }
                else if (rv == 0)
                {
                    error = SERIAL_ERROR_TIMEOUT;
                }
                else
                {
                    result = write(fd, &c, 1);
                    if (result > 0)
                    {
                        error = result;
                        break;
                    }
                    else
                    {
                        if (result < 0)
                        {
                            error = -errno;
                        }
                        else
                        {
                            error = SERIAL_ERROR_BUFFER_SIZE_EXCEEDED;
                        }
                    }
                }
            }
        } while (et.split() < wictimeout);
#endif

        // These sleeps are necessary to keep from overrunning the serial output buffer
        // Windows seem to have trouble with anything shorter than 1/100 second (we should explore this)
        // Unix is set to sleep just a little longer than it would take to send a character (could be shorter?)
#ifdef COSMOS_WIN_OS
        secondsleep(0.010);
#else
//        secondsleep(10. / baud - et.split());
#endif
        //

        return error;
    }

    int32_t Serial::put_string(string data)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        int nbytes = write(fd, data.data(), data.size());
        if (nbytes < 0)
        {
            error = -errno;
            return error;
        }
        else
        {
            error = 0;
            return nbytes;
        }
    }

    int32_t Serial::put_data(vector<uint8_t> data)
    {
        return put_data(data.data(), data.size());
//        if (fd < 0)
//        {
//            error = SERIAL_ERROR_OPEN;
//            return (error);
//        }

//        int nbytes = write(fd, data.data(), data.size());
//        if (nbytes < 0)
//        {
//            error = -errno;
//            return error;
//        }
//        else
//        {
//            error = 0;
//            return nbytes;
//        }
    }

    int32_t Serial::put_data(const uint8_t *data, size_t size)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

//        ElapsedTime et;
//        int32_t tbytes = 0;
//        error = 0;
//        double timeout = ictimeout * size;
//        if (timeout > 5.)
//        {
//            timeout = 5.;
//        }
//        do
//        {
//        int nbytes = write(fd, &data[tbytes], size);
        int nbytes = write(fd, data, size);
            if (nbytes < 0)
            {
                error = -errno;
                return error;
            }
            else {
                error = 0;
                return nbytes;
            }
//            tbytes += nbytes;
//        } while (tbytes < size && et.split() < timeout);
//        return tbytes;
    }

    int32_t Serial::put_slip(vector<uint8_t> data)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        size_t i = 0;
        error = put_char(SLIP_FEND);
        if (error < 0)
        {
            return error;
        }
        for (size_t j=0; j<data.size(); j++)
        {
            switch (data[j])
            {
            case SLIP_FEND:
                error = put_char(SLIP_FESC);
                if (error < 0)
                {
                    return error;
                }
                error = put_char(SLIP_TFEND);
                if (error < 0)
                {
                    return error;
                }
                i+=2;
                break;
            case SLIP_FESC:
                error = put_char(SLIP_FESC);
                if (error < 0)
                {
                    return error;
                }
                error = put_char(SLIP_TFESC);
                if (error < 0)
                {
                    return error;
                }
                i+=2;
                break;
            default:
                error = put_char(data[j]);
                if (error < 0)
                {
                    return error;
                }
                i++;
                break;
            }
        }
        error = put_char(SLIP_FEND);
        if (error < 0)
        {
            return error;
        }
        return (i);
    }

    int32_t Serial::put_slip(const uint8_t *data, size_t size)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        size_t i = 0;
        error = put_char(SLIP_FEND);
        if (error < 0)
        {
            return error;
        }
        for (size_t j=0; j<size; j++)
        {
            switch (data[j])
            {
            case SLIP_FEND:
                error = put_char(SLIP_FESC);
                if (error < 0)
                {
                    return error;
                }
                error = put_char(SLIP_TFEND);
                if (error < 0)
                {
                    return error;
                }
                i+=2;
                break;
            case SLIP_FESC:
                error = put_char(SLIP_FESC);
                if (error < 0)
                {
                    return error;
                }
                error = put_char(SLIP_TFESC);
                if (error < 0)
                {
                    return error;
                }
                i+=2;
                break;
            default:
                error = put_char(data[j]);
                if (error < 0)
                {
                    return error;
                }
                i++;
                break;
            }
        }
        error = put_char(SLIP_FEND);
        if (error < 0)
        {
            return error;
        }
        return (i);
    }

    int32_t Serial::put_nmea(vector<uint8_t> data)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        uint8_t cs_in, digit1, digit2;
        string message_sent; // for debugging

        cs_in = 0;

        // All commands start with a dollar sign, followed by a
        // character command, a comma, command specific parameters,
        // an asterisk, a checksum, and a newline character.
        // An example for VN-100 command is shown below.
        // $VNRRG,11*73

        // start command '$'
        error = put_char('$');
        message_sent = '$';

        // iterate through the buffer to send each charcter to serial port
        size_t j;
        for (j=0; j<data.size(); j++)
        {
            error = put_char(data[j]);
            message_sent += data[j];
            // check sum (xor?)
            cs_in ^= static_cast<uint8_t>(data[j]);
        }
        // end of command '*'
        error = put_char('*');
        if (error < 0)
        {
            return error;
        }
        message_sent += '*';

        if (cs_in > 16)
        {
            digit1 = cs_in/16;
            if (digit1 < 10)
            {
                error = put_char( '0'+digit1);
                if (error < 0)
                {
                    return error;
                }
                message_sent += '0'+digit1;
            }
            else
            {
                error = put_char( 'A'+digit1-10);
                if (error < 0)
                {
                    return error;
                }
                message_sent += 'A'+digit1-10;
            }
        }
        else
        {
            error = put_char( '0');
            if (error < 0)
            {
                return error;
            }
            message_sent += '0';
        }

        ++j;
        digit2 = cs_in%16;
        if (digit2 <10 )
        {
            error = put_char( '0'+digit2);
            if (error < 0)
            {
                return error;
            }
            message_sent += '0'+digit2;
        }
        else
        {
            error = put_char( 'A'+digit2-10);
            if (error < 0)
            {
                return error;
            }
            message_sent += 'A'+digit2-10;
        }
        ++j;
        error = put_char( '\n');
        if (error < 0)
        {
            return error;
        }
        message_sent += "<CR><LF>";
        return (j+3);
    }

    int32_t Serial::drain()
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
        //	tcdrain(fd);
        //	tcflush(fd,TCOFLUSH);
        //	tcflush(fd,TCIFLUSH);
        tcflush(fd, TCIOFLUSH);
#else // windows
        PurgeComm(handle, PURGE_RXCLEAR|PURGE_TXCLEAR);
#endif
        return 0;
    }

    int32_t Serial::get_char(uint8_t &buffer)
    {
        // if file descirptor return error (<0) then fail
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        error = get_char();
        if (error >= 0)
        {
            buffer = error;
        }
        return error;
    }

    int32_t Serial::ReceiveByte(uint8_t &buf) {
        int n = 0;
        buf = '\0'; // emtpy buffer
        //n = ReceiveBuffer(&buf, size);
#ifdef COSMOS_WIN_OS
        ReadFile(handle, &buf, 1, (LPDWORD)((void *)&n), NULL);
#endif

        return(n);
    }

    int32_t Serial::ReceiveBuffer(uint8_t *buf, int size) {
        int n = 0;

        if(size>4096)  size = 4096;

        //uint8_t *data_rx;
#ifdef COSMOS_WIN_OS
        ReadFile(handle, buf, size, (LPDWORD)((void *)&n), NULL);
#endif

        return(n);
    }

    int32_t Serial::get_char()
    {
        // if file descirptor return error (<0) then fail
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        int result;
        uint8_t c;

#ifdef COSMOS_WIN_OS
        ElapsedTime et;
        do
        {
            int n=0;
            result = ReadFile(handle, &c, 1, (LPDWORD)((void *)&n), NULL);
            if (result < 0)
            {
                result = -WSAGetLastError();
            }
            else
            {
                result = SERIAL_ERROR_TIMEOUT;
            }
            if (result > 0)
            {
                error = c;
                break;
            }
            else
            {
                error = result;
            }
            secondsleep(rictimeout < 1. ? rictimeout/10. : .1);
        } while (error == SERIAL_ERROR_TIMEOUT && et.split() < rictimeout);
#else
        ElapsedTime et;
        do
        {
            fd_set set;
            FD_ZERO(&set);
            FD_SET(fd, &set);
            timeval timeout;
            timeout.tv_sec = static_cast<int32_t>(rictimeout);
            timeout.tv_usec = static_cast<int32_t>(1000000. * (rictimeout - timeout.tv_sec));
            int rv = select(fd+1, &set, nullptr, nullptr, &timeout);
            if (rv == -1)
            {
                error = -errno;
            }
            else if (rv == 0)
            {
                error = SERIAL_ERROR_TIMEOUT;
            }
            else
            {
                if (FD_ISSET(fd, &set))
                {
                    result = read(fd, &c, 1);
                    if (result > 0)
                    {
                        error = c;
                        break;
                    }
                    else
                    {
                        if (result < 0)
                        {
                            error = -errno;
                        }
                        else
                        {
                            error = SERIAL_ERROR_EOT;
                        }
                    }
                }
            }
        } while (et.split() < rictimeout);
#endif

        return error;
    }

    int32_t Serial::poll_char()
    {
        // if file descirptor return error (<0) then fail
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        int result;
        uint8_t c;

#ifdef COSMOS_WIN_OS
        ElapsedTime et;
        do
        {
            int n=0;
            result = ReadFile(handle, &c, 1, (LPDWORD)((void *)&n), NULL);
            if (result < 0)
            {
                result = -WSAGetLastError();
            }
            else
            {
                result = SERIAL_ERROR_TIMEOUT;
            }
            if (result > 0)
            {
                error = c;
                break;
            }
            else
            {
                error = result;
            }
            secondsleep(rictimeout < 1. ? rictimeout/10. : .1);
        } while (error == SERIAL_ERROR_TIMEOUT && et.split() < rictimeout);
#else
        ElapsedTime et;
        do
        {
            result = read(fd, &c, 1);
            if (result > 0)
            {
                error = c;
                break;
            }
            else
            {
                if (result < 0)
                {
                    error = -errno;
                }
            }
        } while (et.split() < rictimeout);
#endif

//        printf("{%.5f}", et.split());

        if (et.split() > rictimeout)
        {
            return SERIAL_ERROR_TIMEOUT;
        }
        else
        {
            return error;
        }
    }

    int32_t Serial::get_string(string &data, size_t size)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        data.clear();
        for (uint16_t i=0; i<size; ++i)
        {
            if ((error=get_char()) < 0)
            {
                if (error == SERIAL_ERROR_TIMEOUT)
                {
                    return(i);
                }
                else
                {
                    return error;
                }
            }
            else
            {
                data.append(1, (char)error);
            }
        }
        return (static_cast<int32_t>(size));
    }

    int32_t Serial::get_string(string &data, char endc)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        data.clear();
        do
        {
            if ((error=get_char()) < 0)
            {
                if (error == SERIAL_ERROR_TIMEOUT)
                {
                    return(data.size());
                }
                else
                {
                    return error;
                }
            }
            else
            {
                data.append(1, (char)error);
            }
        } while (error != endc);

        return (static_cast<int32_t>(data.size()));
    }

    int32_t Serial::get_data(vector <uint8_t> &data, size_t size)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        for (uint16_t i=0; i<size; ++i)
        {
            if ((error=get_char()) < 0)
            {
                if (error == SERIAL_ERROR_TIMEOUT)
                {
                    return(i);
                }
                else
                {
                    return error;
                }
            }
            else
            {
                data.push_back(static_cast<uint8_t>(error));
            }
        }
        return (static_cast<int32_t>(size));
    }

    int32_t Serial::get_data(uint8_t *data, size_t size)
    {
        if (fd < 0)
        {
            error = SERIAL_ERROR_OPEN;
            return (error);
        }

        for (uint16_t i=0; i<size; ++i)
        {
            if ((error=get_char()) < 0)
            {
                if (error == SERIAL_ERROR_TIMEOUT)
                {
                    return(i);
                }
                else
                {
                    return error;
                }
            }
            else
            {
                data[i] = static_cast<uint8_t>(error);
            }
        }
        return (static_cast<int32_t>(size));
    }

    //! Read Xmodem frame.
    /*! Read one Xmodem block (frame) of data, removing control characters
     * and calculating checksum. Supplied buffer is assumed to be at least 128
     * bytes.
        \param data Byte array to store incoming data.
        \param size Size , in bytes, of byte array.
        \return Packet number or negative error.
    */
    int32_t Serial::get_xmodem(vector <uint8_t> &data, size_t size)
    {
        int32_t ch;

        ch = get_char();
        if (ch != XMODEM_SOH)
        {
            if (ch == XMODEM_EOT)
            {
                return SERIAL_ERROR_EOT;
            }
            else
            {
                return SERIAL_ERROR_READ;
            }
        }
        uint8_t blocknum = static_cast<uint8_t>(get_char());
        ch = get_char();
        if (255-blocknum != ch)
        {
            return SERIAL_ERROR_READ;
        }

        uint16_t i = 0;
        uint8_t csum = 0;
        do
        {
            ch = get_char();
            if (ch < 0)
            {
                return (ch);
            }
            data.push_back(static_cast<uint8_t>(ch));
            ++i;
            csum += ch;
        } while (i<128);

        ch = get_char();
        if (ch != csum)
        {
            return SERIAL_ERROR_CHECKSUM;
        }

        return blocknum;
    }

    //! Read SLIP frame.
    /*! Read an entire frame of SLIP encoded data from the serial port.
     * Special SLIP characters are removed on the fly. Will stop early if
     * supplied buffer size is exceeded.
        \param data Byte array to store incoming data.
        \param size Size , in bytes, of byte array.
        \return Number of bytes read, up to maximum.
    */
    int32_t Serial::get_slip(vector <uint8_t> &data, size_t size)
    {
        int32_t ch;

        data.clear();
        do
        {
            ch = get_char();
            if (ch < 0)
            {
                if (ch == SERIAL_ERROR_TIMEOUT)
                {
                    return (SERIAL_ERROR_SLIPIN);
                }
                else
                {
                    return (ch);
                }
            }
        } while (ch != SLIP_FEND);

        do
        {
            ch = get_char();
            if (ch < 0)
            {
                if (ch == SERIAL_ERROR_TIMEOUT)
                {
                    return (SERIAL_ERROR_SLIPOUT);
                }
                else
                {
                    return (ch);
                }
            }
            if (data.size() < size)
            {
                switch (ch)
                {
                case SLIP_FESC:
                    ch = get_char();
                    switch (ch)
                    {
                    case SLIP_TFEND:
                        data.push_back(SLIP_FEND);
                        break;
                    case SLIP_TFESC:
                        data.push_back(SLIP_FESC);
                        break;
                    }
                    break;
                case SLIP_FEND:
                    break;
                default:
                    data.push_back(static_cast<uint8_t>(ch));
                    break;
                }
            }
        } while (ch != SLIP_FEND);

        return data.size();
    }

    int32_t Serial::get_slip(uint8_t *data, size_t size)
    {
        int32_t ch;
        uint16_t i;

        do
        {
            ch = get_char();
            if (ch < 0)
            {
                if (ch == SERIAL_ERROR_TIMEOUT)
                {
                    return (SERIAL_ERROR_SLIPIN);
                }
                else
                {
                    return (ch);
                }
            }
        } while (ch != SLIP_FEND);

        i = 0;
        do
        {
            ch = get_char();
            if (ch < 0)
            {
                if (ch == SERIAL_ERROR_TIMEOUT)
                {
                    return (SERIAL_ERROR_SLIPOUT);
                }
                else
                {
                    return (ch);
                }
            }
            if (i < size)
            {
                switch (ch)
                {
                case SLIP_FESC:
                    ch = get_char();
                    switch (ch)
                    {
                    case SLIP_TFEND:
                        data[i] = SLIP_FEND;
                        break;
                    case SLIP_TFESC:
                        data[i] = SLIP_FESC;
                        break;
                    }
                    ++i;
                    break;
                case SLIP_FEND:
                    break;
                default:
                    data[i] = static_cast<uint8_t>(ch);
                    ++i;
                    break;
                }
            }
        } while (ch != SLIP_FEND);

        return (i);
    }

    //! Read NMEA response.
    /*! Read an entire NMEA response from the serial port.
     * The leading $ and trailing * and checksum are removed, and only the
     * payload of the response is returned. Will stop early if
     * supplied buffer size is exceeded.
        \param data Byte array to store incoming data.
        \param size Size , in bytes, of byte array.
        \return Number of bytes read, up to maximum.
    */
    int32_t Serial::get_nmea(vector <uint8_t> &data, size_t size)
    {
        int32_t ch;
        uint16_t i;
        uint8_t cs_in, cs_out;
        string input;

        do
        {
            ch = get_char();
            input += static_cast<char>(ch);
            if (ch < 0) return (ch);
        } while (ch != '$');

        i = 0;
        cs_in = 0;
        do
        {
            ch = get_char();
            input += static_cast<char>(ch);
            if (ch < 0) return (ch);
            if (i < size)
            {
                switch (ch)
                {
                case '*':
                    break;
                default:
                    cs_in ^= ch;
                    data.push_back(static_cast<uint8_t>(ch));
                    ++i;
                    break;
                }
            }
        } while (ch != '*');
        ch = get_char();
        input += static_cast<char>(ch);
        if (ch < 0) return (ch);
        if (ch > '9')
        {
            if (ch > 'F')
            {
                cs_out = (static_cast<uint8_t>(ch) - 'a' + 10) * 16;
            }
            else
            {
                cs_out = (static_cast<uint8_t>(ch) - 'A' + 10) * 16;
            }
        }
        else
        {
            cs_out = (static_cast<uint8_t>(ch) - '0') * 16;
        }
        ch = get_char();
        input += static_cast<char>(ch);
        if (ch < 0) return (ch);
        if (ch > '9')
        {
            if (ch > 'F')
            {
                cs_out += (ch - 'a' + 10);
            }
            else
            {
                cs_out += (ch - 'A' + 10);
            }
        }
        else
        {
            cs_out += (ch - '0');
        }
        if (cs_in != cs_out)
            return (SERIAL_ERROR_CHECKSUM);
        ch = get_char();
        input += static_cast<char>(ch);

        return (i);
    }

} // end of namepsace Cosmos
