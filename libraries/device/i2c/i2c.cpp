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
    //! \param dname Name of physical serial port.


    I2C::I2C(string bus, uint8_t address, double delay)
    {
        handle.bus = bus;
        handle.fh = open(handle.bus.c_str(), O_RDWR);

        if (handle.fh < 0)
        {
            handle.fh = -1;
            return;
        }

        if (ioctl(handle.fh, I2C_FUNCS, &handle.funcs) < 0)
        {
            close(handle.fh);
            handle.fh = -1;
            return;
        }

        handle.address = address;
        handle.delay = delay;
        return;
    }

    I2C::~I2C()
    {
        if (handle.fh >= 0)
        {
            close(handle.fh);
        }
    }

    int32_t I2C::connect()
    {
        int32_t iretn = 0;

        if (ioctl(handle.fh, I2C_SLAVE, handle.address) < 0)
        {
            handle.connected = false;
            iretn = - errno;
            return iretn;
        }

        handle.connected = true;
        return iretn;
    }

    int32_t I2C::send(uint8_t *data, size_t len)
    {

        int32_t iretn = ::write(handle.fh, data, len);

        if (iretn < 0)
        {
            iretn = -errno;
        }

        return iretn;
    }

    int32_t I2C::receive(uint8_t *data, size_t len)
    {
        int32_t iretn;
        size_t count = 0;

        ElapsedTime et;
        do
        {
            int32_t rcvd = ::read(handle.fh, data, len - count);
            if (rcvd < 0)
            {
                iretn = -errno;
                return iretn;
            }
            else if (rcvd == 0)
            {
                if (et.split() > handle.delay)
                {
                    iretn = count;
                    return iretn;
                }
            }
            else
            {
                et.reset();
                count += rcvd;
            }
        } while(count < len);
        return count;
    }
} // end of namepsace Cosmos
