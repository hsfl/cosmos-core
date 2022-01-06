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

#include "device/i2c/i2c.h"

#define ARDUINO_I2C_ADDRESS 0x10
#define ARDUINO_I2C_BUFFER_LIMIT 32

namespace Cosmos {

    //! \ingroup i2c
    //! \ingroup i2c
    //! \defgroup seriallib_functions Serial Port functions
    //! @{

    //! Create i2c port instance.
    //! Create a i2c port object to be used for reading and writing to a physical port.
    //! \param bus Name of physical I2C device.
    //! \param address Address of physical I2C port.
    //! \param delay Time in seconds to wait for reading after writing.


    I2C::I2C(string bus, uint8_t address, double delay, bool probe)
    {
        handle.bus = bus;
#if !defined(COSMOS_WIN_OS)
        handle.fh = open(handle.bus.c_str(), O_RDWR|O_NONBLOCK);
#endif

        if (handle.fh < 0)
        {
            error = - errno;
            handle.fh = -1;
            return;
        }

        // only works for linux for now
        // TODO: expand to mac and windows
#if defined(COSMOS_LINUX_OS)
        if (ioctl(handle.fh, I2C_FUNCS, &handle.funcs) < 0)
        {
            error = - errno;
            close(handle.fh);
            handle.fh = -1;
            return;
        }

        handle.probe = probe;
        handle.address = address;
        handle.delay = delay;

        error = connect();

#endif
        handle.connected = true;
        return;
    }

    I2C::~I2C()
    {
        if (handle.fh >= 0)
        {
            close(handle.fh);
        }
    }

    int32_t I2C::set_address(uint64_t address)
    {
        if (handle.fh >= 0)
        {
            handle.address = address;
            error = connect();
        }
        else
        {
            error = COSMOS_GENERAL_ERROR_BAD_FD;
        }
        return error;
    }

    int32_t I2C::set_delay(double seconds)
    {
        if (handle.fh >= 0)
        {
            handle.delay = seconds;
            error = 0;
        }
        else
        {
            error = COSMOS_GENERAL_ERROR_BAD_FD;
        }
        return error;
    }

    int32_t I2C::connect()
    {
        if (handle.fh >= 0)
        {
            error = 0;

#if defined(COSMOS_LINUX_OS)
            if (ioctl(handle.fh, I2C_SLAVE, handle.address) < 0)
            {
                error = - errno;
                handle.connected = false;
                return error;
            }

            if (handle.probe && (error = i2c_smbus_read_byte(handle.fh)) < 0)
            {
                error = - errno;
                handle.connected = false;
                return error;
            }
#endif
            handle.connected = true;
        }
        else
        {
            error = COSMOS_GENERAL_ERROR_BAD_FD;
            handle.connected = false;
        }
        return error;
    }

    int32_t I2C::communicate(string data, string &response, size_t bytes)
    {
        int32_t iretn=0;

        std::lock_guard<mutex> lock(mtx);
        iretn = send(data);
        if (iretn < 0)
        {
            return iretn;
        }

        if (bytes)
        {
            iretn = receive(response, bytes);
        }
        return iretn;
    }

    int32_t I2C::communicate(vector <uint8_t> data, vector <uint8_t> &response, size_t bytes)
    {
        int32_t iretn=0;

        std::lock_guard<mutex> lock(mtx);
        iretn = send(data);
        if (iretn < 0)
        {
            return iretn;
        }

        if (bytes)
        {
            iretn = receive(response, bytes);
        }
        return iretn;
    }

    int32_t I2C::communicate(uint8_t *data, size_t len, uint8_t *response, size_t bytes)
    {
        int32_t iretn=0;

        std::lock_guard<mutex> lock(mtx);
        iretn = send(data, len);
        if (iretn < 0)
        {
            return iretn;
        }

        if (bytes)
        {
            iretn = receive(response, bytes);
        }
        return iretn;
    }

    int32_t I2C::send(string data)
    {
        return send((uint8_t *)data.data(), data.length());
    }

    int32_t I2C::send(vector <uint8_t> data)
    {
        return send(data.data(), data.size());
    }


    int32_t I2C::send(uint8_t *data, size_t len)
    {
        if (handle.fh < 0)
        {
            error = COSMOS_GENERAL_ERROR_BAD_FD;
            return error;
        }

        if (!handle.connected && (error=connect()) < 0)
        {
            return error;
        }

        error = ::write(handle.fh, data, len);

        if (error < 0)
        {
            error = -errno;
        }

        return error;
    }

    int32_t I2C::receive(string &data, size_t bytes)
    {
        data.resize(bytes);
        return receive((uint8_t *)data.data(), bytes);
    }

    int32_t I2C::receive(vector <uint8_t> &data, size_t bytes)
    {
        data.resize(bytes);
        return receive(data.data(), bytes);
    }

    int32_t I2C::receive(uint8_t *data, size_t bytes)
    {
        if (handle.fh < 0)
        {
            error = COSMOS_GENERAL_ERROR_BAD_FD;
            return error;
        }

        if (!handle.connected && (error=connect()) < 0)
        {
            return error;
        }

        size_t count = 0;

        secondsleep(handle.delay);

        if (bytes)
        {
            ElapsedTime et;
            do
            {
                int32_t rcvd = 0;
                if (data == nullptr)
                {
                    vector <uint8_t> tbuf;
                    tbuf.resize(bytes - count);
                    rcvd = ::read(handle.fh, tbuf.data(), bytes - count);
                }
                else
                {
                    rcvd = ::read(handle.fh, data, bytes - count);
                }

                if (rcvd < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
                {
                    error = -errno;
                    return error;
                }
                else if (rcvd <= 0)
                {
                    if (et.split() > bytes * .0001)
                    {
                        error = count;
                        return error;
                    }
                }
                else
                {
                    count += rcvd;
                }
            } while(count < bytes);
        }
        return count;
    }

    int32_t I2C::poll(uint8_t *data, size_t len, uint8_t markchar, double timeout)
    {
        if (handle.fh < 0)
        {
            error = COSMOS_GENERAL_ERROR_BAD_FD;
            return error;
        }

        if (data == nullptr)
        {
            return COSMOS_GENERAL_ERROR_MEMORY;
        }

        size_t count = 0;
        if (timeout >10.)
        {
            timeout = 10.;
        }

        if (len)
        {
            ElapsedTime et;
            do
            {
                int32_t rcvd = ::read(handle.fh, &data[0], 1);
                if (rcvd < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
                {
                    error = -errno;
                    return error;
                }
                if (rcvd == 1 && data[0] != markchar)
                {
                    break;
                }
                microsleep(5000);
            } while(et.split() < timeout);
            if (et.split() >= timeout)
            {
                error = COSMOS_GENERAL_ERROR_TIMEOUT;
                return error;
            }
            count = 1;
            do
            {
                int32_t rcvd = ::read(handle.fh, &data[count], 1);

                if (rcvd < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
                {
                    error = -errno;
                    return error;
                }
                if (rcvd == 1)
                {
                    ++count;
                }
            } while(count < len && et.split() < timeout);
        }
        return count;
    }

    int32_t I2C::get_error()
    {
        return error;
    }

    int32_t I2C::get_fh()
    {
        return handle.fh;
    }

    bool I2C::get_connected()
    {
        return handle.connected;
    }


} // end of namepsace Cosmos
