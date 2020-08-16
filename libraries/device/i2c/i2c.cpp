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
#include "device/i2c/i2c.h"
#include "support/elapsedtime.h"

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


    I2C::I2C(string bus, uint8_t address, double delay)
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

        handle.address = address;
        handle.delay = delay;

        if (ioctl(handle.fh, I2C_SLAVE, handle.address) < 0)
        {
            error = - errno;
            handle.connected = false;
            close(handle.fh);
            handle.fh = -1;
            return;
        }

        if ((error = i2c_smbus_read_byte(handle.fh)) < 0)
        {
            error = - errno;
            handle.connected = false;
            close(handle.fh);
            handle.fh = -1;
            return;
        }
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

    //int32_t I2C::connect()
    //{
    //    error = 0;

    //    if (ioctl(handle.fh, I2C_SLAVE, handle.address) < 0)
    //    {
    //        handle.connected = false;
    //        error = - errno;
    //        return error;
    //    }

    //    handle.connected = true;
    //    return error;
    //}

    int32_t I2C::send(string data)
    {
        //uint8_t * c = data.c_str();
        uint8_t * buff = new uint8_t[data.size() + 1];
        memset(buff, 0, sizeof(data.size())); // reset buffer
        std::copy(data.begin(), data.end(), buff);
        error = this->send(buff, data.size());

        return error;
    }

    int32_t I2C::send(uint8_t *data, size_t len)
    {

        error = ::write(handle.fh, data, len);

        if (error < 0)
        {
            error = -errno;
        }

        return error;
    }

    int32_t I2C::send(vector <uint8_t> data)
    {

        error = ::write(handle.fh, data.data(), data.size());

        if (error < 0)
        {
            error = -errno;
        }

        return error;
    }


    int32_t I2C::receive(string &data)
    {
        // work in progress
        //    uint8_t buff[0];
        //    int32_t count = 0;
        //    int32_t rcvd = 0;

        //    do {
        //        rcvd = this->receive(buff,1);
        //        printf("%02x - %c\n", buff[0], buff[0]);
        //        count ++;
        //    }while (buff[0] != 0x00); //end tranmission with null byte

        return 0;
    }

    int32_t I2C::receive(uint8_t *data, size_t len)
    {
        size_t count = 0;

        COSMOS_SLEEP(handle.delay);

        if (len)
        {
            ElapsedTime et;
            do
            {
                int32_t rcvd = 0;
                if (data == nullptr)
                {
                    vector <uint8_t> tbuf;
                    tbuf.resize(len - count);
                    rcvd = ::read(handle.fh, tbuf.data(), len - count);
                }
                else
                {
                    rcvd = ::read(handle.fh, data, len - count);
                }

                if (rcvd < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
                {
                    error = -errno;
                    return error;
                }
                else if (rcvd <= 0)
                {
                    if (et.split() > len * .0001)
                    {
                        error = count;
                        return error;
                    }
                }
                else
                {
                    //                    et.reset();
                    count += rcvd;
                }
            } while(count < len);
        }
        return count;
    }

    int32_t I2C::receive(vector <uint8_t> &data)
    {
        uint8_t tbuf[256];
        data.clear();

        COSMOS_SLEEP(handle.delay);

        ElapsedTime et;
        do
        {
            int32_t rcvd = 0;
            rcvd = ::read(handle.fh, tbuf, 256);

            if (rcvd < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
            {
                error = -errno;
                return error;
            }
            else if (rcvd > 0)
            {
                for (int32_t i=0; i<rcvd; ++i)
                {
                    data.push_back(tbuf[i]);
                }
                et.reset();
            }
        } while(et.split() <= .0001 * (data.size()+1));

        return data.size();
    }

    int32_t I2C::get_error()
    {
        return error;
    }

    int32_t I2C::get_fh()
    {
        return handle.fh;
    }


} // end of namepsace Cosmos
