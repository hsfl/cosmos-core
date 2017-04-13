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

//#define I2C_ADDRESS 0x57 // CubeADCS
#define I2C_BUFFER_LIMIT 32

namespace Cosmos {

//! \ingroup i2c
//! \ingroup i2c
//! \defgroup i2clib_functions I2C functions
//! @{

//! Create i2c port instance.
//! Create a i2c port object to be used for reading and writing to a physical port.
//! \param dname Name of physical serial port.


I2C::I2C(string i2c_bus, uint8_t i2c_address)
{
    int fh;
    char buff[I2C_BUFFER_LIMIT + 4];
    int len, sent, rcvd;
    //i2c_address = 0x10;

    fh = open(i2c_bus.c_str(), O_RDWR);

    if (fh < 0) {
        perror("open");
        //return 1;
    }

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
    if (ioctl(fh, I2C_SLAVE, i2c_address) < 0) {
        perror("ioctl");
        //return 1;
    }
#endif

}

I2C::~I2C()
{

}

int I2C::send(std::string data)
{

    int fh;
    char buff[I2C_BUFFER_LIMIT + 4];
    strcpy(buff, "hello");

    int len, sent, rcvd;
    len = strlen(buff);

    sent = write(fh, buff, len);

    if (sent != len) {
        perror("write");
        return 1;
    }
}

int I2C::receive(std::string data)
{
    int fh;
    char buff[I2C_BUFFER_LIMIT + 4];
    int len, sent, rcvd;
    printf("Sent: %s\n", buff);

    memset(buff, 0, sizeof(buff));
    rcvd = read(fh, buff, sent);

    while (rcvd < sent) {
        usleep(50000);
        len = read(fh, buff + rcvd, sent - rcvd);

        if (len <= 0) {
            if (len < 0)
                perror("read");

            break;
        }

        rcvd += len;
    }

    if (rcvd > 0)
        printf("Received: %s\n", buff);
}

int I2C::test(std::string data)
{
    cout << data << endl;
}



} // end of namepsace Cosmos
