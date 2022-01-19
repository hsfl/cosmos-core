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

#ifndef COSMOS_DEVICE_I2C_CLASS_H
#define COSMOS_DEVICE_I2C_CLASS_H

#include "support/cosmos-errno.h"
#include "support/timelib.h"
#include "support/elapsedtime.h"
//#include <linux/i2c-dev.h> /* for I2C_SLAVE */
#if !defined(COSMOS_WIN_OS)
#include "device/i2c/i2c-dev-smbus.h"
#endif

using std::cout;

namespace Cosmos {
    class I2C
    {
    public:
        I2C(string bus, uint8_t address, double delay=2e-4, bool probe=false);
        ~I2C();
        //        int32_t set_params(size_t dbaud, size_t dbits, size_t dparity, size_t dstop);

        //        int32_t put_char(uint8_t c);
        //        int32_t put_string(string data);
        //        int32_t put_data(vector <uint8_t> data);
        //        int32_t get_char();
        //        int32_t get_data(vector <uint8_t> &data, size_t size);

        //        string name;
        //        int32_t connect();
        int32_t get_funcs();
        int32_t set_address(uint64_t address);
        int32_t set_delay(double seconds);
        int32_t connect();
        int32_t communicate(string data, string &response, size_t bytes=0);
        int32_t communicate(vector <uint8_t> data, vector <uint8_t> &response, size_t bytes=0);
        int32_t communicate(uint8_t *data, size_t len, uint8_t *response, size_t bytes=0);
        int32_t send(string data);
        int32_t send(uint8_t *data, size_t len);
        int32_t send(vector <uint8_t> data);
        int32_t receive(string &data, size_t bytes);
        int32_t receive(uint8_t *data, size_t bytes);
        int32_t receive(vector <uint8_t> &data, size_t bytes);
        int32_t poll(uint8_t *data, size_t len, uint8_t markchar=0xff, double timeout=0.);
        int32_t get_error();
        int32_t get_fh();
        bool get_connected();


    private:
        struct
        {
            bool connected = false;
            bool probe = true;
            string bus;
            uint8_t address;
            int fh = -1;
            uint64_t funcs;
            double delay = 1e-4;
        } handle;

        mutex mtx;
        int32_t error;

    };


} // end of namepsace Cosmos
#endif // COSMOS_DEVICE_I2C_CLASS_H
