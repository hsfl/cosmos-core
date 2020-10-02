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

#ifndef KISSLIB
#define KISSLIB

#include "support/configCosmos.h"
#include "support/cosmos-errno.h"
#include "support/sliplib.h"
#include "device/serial/serialclass.h"

#include <cstring>
#include <iostream>

#define FEND  0xC0
#define FESC  0xDB
#define TFEND 0xDC
#define TFESC 0xDD

#define PACKETMAX 1024


class KissHandle
{

public:
    KissHandle(const string &device="", string dest_call="", string sour_call="", uint8_t port=0, uint8_t comm=0, uint8_t dest_stat=0x60, uint8_t sour_stat=0x61, uint8_t cont=0x03, uint8_t prot=0xf0);

    void set_port_number(uint8_t P);
    uint8_t get_port_number();
    void set_command(uint8_t C);
    uint8_t get_command();
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
    int set_data(vector <uint8_t> input);
    vector <uint8_t> get_data();
    int32_t load_packet();
    int32_t unload_packet();
    vector <uint8_t> get_packet();
    bool get_open();
    int32_t get_error();
    Serial *get_serial();

    struct __attribute__ ((packed)) packet_header
    {
        unsigned command : 4;
        unsigned port_number : 4;
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
        uint8_t data;
    };
	
private:
    packet_header header;
    vector <uint8_t> data;
    vector <uint8_t> packet;
    Serial *handle;
    int32_t error;

//	unsigned int  port_number;
//	unsigned int  command;
//	unsigned char destination_callsign[6];
//	unsigned char destination_stationID;
//	unsigned char source_callsign[6];
//	unsigned char source_stationID;
//	unsigned char control;
//	unsigned char protocolID;

    friend std::ostream& operator<<(std::ostream& out, KissHandle& K);
};

//old load_packet (to be deleted)**********************************************
int kissEncode(uint8_t *input, uint32_t count, uint8_t *packet);

//int kissDecode(uint8_t *input, uint32_t count, uint8_t *unload_packetd_packet);

int kissDecode(uint8_t* kissed_input, uint32_t count, uint8_t* unload_packetd_payload);
int kissDecode(vector <uint8_t> kissed_input, KissHandle &handle);

KissHandle kissInspect(const unsigned char* input);

//Functions to print packets
void print_ascii(unsigned char* packet, unsigned int count);
void print_hex(unsigned char* packet, unsigned int count);

#endif
