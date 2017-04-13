
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

//#define I2C_ADDRESS 0x57 // CubeADCS
#define I2C_BUFFER_LIMIT 32

namespace Cosmos {

//! \ingroup i2c
//! \ingroup i2c
//! \defgroup seriallib_functions Serial Port functions
//! @{

    //! Create i2c port instance.
    //! Create a i2c port object to be used for reading and writing to a physical port.
    //! \param dname Name of physical serial port.


I2C::I2C(string dname, size_t dbaud, size_t dbits, size_t dparity, size_t dstop)
{
    int fh;
    char buff[ARDUINO_I2C_BUFFER_LIMIT + 4];
    int len, sent, rcvd;
I2C::I2C(string i2c_bus, uint8_t i2c_address)
{
    int fh;
    char buff[I2C_BUFFER_LIMIT + 4];
    int len, sent, rcvd;
    //i2c_address = 0x10;

    fh = open("/dev/i2c-2", O_RDWR);
    fh = open(i2c_bus.c_str(), O_RDWR);

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

#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
    if (ioctl(fh, I2C_SLAVE, ARDUINO_I2C_ADDRESS) < 0) {
        perror("ioctl");
        //return 1;
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)
    if (ioctl(fh, I2C_SLAVE, i2c_address) < 0) {
        perror("ioctl");
        //return 1;
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

}

int I2C::send(std::string data)
{

    int fh;
    char buff[ARDUINO_I2C_BUFFER_LIMIT + 4];
    strcpy(buff, "hello");

    int len, sent, rcvd;
    len = strlen(buff);

    sent = write(fh, buff, len);

    if (sent != len) {
        perror("write");
        return 1;
        handle.connected = true;
        return iretn;
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

int I2C::receive(std::string data)
{
    int fh;
    char buff[ARDUINO_I2C_BUFFER_LIMIT + 4];
    int len, sent, rcvd;
    printf("Sent: %s\n", buff);
    int32_t I2C::write(uint8_t *data, size_t len)
    {
int I2C::receive(std::string data)
{
    int fh;
    char buff[I2C_BUFFER_LIMIT + 4];
    int len, sent, rcvd;
    printf("Sent: %s\n", buff);

        int32_t iretn = ::write(handle.fh, data, len);

        if (iretn < 0)
        {
            iretn = -errno;
        }

        return iretn;
    }

    int32_t I2C::read(uint8_t *data, size_t len)
    {
        int32_t iretn;
        size_t count = 0;




int I2C::test(std::string data)
{
    cout << data << endl;
}


} // end of namepsace Cosmos
