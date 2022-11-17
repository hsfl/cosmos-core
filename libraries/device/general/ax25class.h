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

#ifndef AX25LIB
#define AX25LIB

#include "support/configCosmosKernel.h"
#include "support/cosmos-errno.h"
#include "math/bytelib.h"
#include "math/crclib.h"

#include <cstring>
#include <iostream>

#define PACKETMAX 1024


class Ax25Handle
{

public:
    struct __attribute__ ((packed)) packet_header
    {
        uint8_t destination_callsign[6];
        uint8_t destination_stationID;
        uint8_t source_callsign[6];
        uint8_t source_stationID;
        uint8_t control = 0x03;
        uint8_t protocolID = 0xf0;
    };

    struct packet_content
    {
        packet_header header;
        vector <uint8_t> data;
    };

    Ax25Handle(string dest_call="", string sour_call="", uint8_t dest_stat=0x60, uint8_t sour_stat=0x61, uint8_t cont=0x03, uint8_t prot=0xf0);

    void set_destination_callsign(string destination);
    string get_destination_callsign();
    void set_destination_stationID(uint8_t ID);
    uint8_t get_destination_stationID();
    void set_source_callsign(string source);
    string get_source_callsign();
    void set_source_stationID(uint8_t ID);
    uint8_t get_source_stationID();
    void set_control(uint8_t control_number);
    uint8_t get_control();
    void set_protocolID(uint8_t protocol);
    uint8_t get_protocolID();
    packet_header get_header();
    vector <uint8_t> get_data();
    vector <uint8_t> get_ax25_packet();
    vector <uint8_t> get_hdlc_packet();
    int32_t set_data(vector <uint8_t> input);
    int32_t set_ax25_packet(vector <uint8_t> packet);
    int32_t set_hdlc_packet(vector <uint8_t> packet);
    int32_t unload();
    int32_t load(vector<uint8_t> newdata={});
    int32_t stuff(vector<uint8_t> ax25data={});
    int32_t unstuff(vector<uint8_t> hdlcdata={});

private:
    packet_header header;
    uint16_t crc;
    uint16_t crccalc;
    vector <uint8_t> data;
    vector<uint8_t> hdlc_packet;
    vector<uint8_t> ax25_packet;
    int32_t error;
//    vector<uint8_t> flags = {0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e, 0x7e};
    vector<uint8_t> flags = {0x7e, 0x7e};
    CRC16 calc_crc;


    friend ::std::ostream& operator<<(::std::ostream& out, Ax25Handle& K);
};

//Functions to print packets
void print_ascii(unsigned char* packet, unsigned int count);
void print_hex(unsigned char* packet, unsigned int count);

#endif
