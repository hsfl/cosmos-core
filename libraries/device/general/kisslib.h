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

#include <cstring>
#include <iostream>

//using namespace std;

#define FEND  0xC0
#define FESC  0xDB
#define TFEND 0xDC
#define TFESC 0xDD

#define PACKETMAX 1024


class KissHandle
{

public:
    KissHandle(); // default constructor
    KissHandle( 		int port,
				int comm,
				const char dest_call[],
				char dest_stat,
				const char sour_call[],
				char sour_stat,
				char cont,
				char prot);

	void set_port_number(unsigned int P);
	unsigned int get_port_number();
	void set_command(unsigned int C);
	unsigned int get_command();
	void set_destination_callsign(const char destination[]);
	unsigned char* get_destination_callsign();	
	void set_destination_stationID(unsigned int ID);
	unsigned int get_destination_stationID();
	void set_source_callsign(const char source[]);
	unsigned char* get_source_callsign();	
	void set_source_stationID(unsigned int ID);
	unsigned int get_source_stationID();
	void set_control(unsigned int control_number);
	unsigned int get_control();
	void set_protocolID(unsigned int protocol);
	unsigned int get_protocolID();
	
private:
	unsigned int  port_number;
	unsigned int  command;
	unsigned char destination_callsign[6];
	unsigned char destination_stationID;
	unsigned char source_callsign[6];
	unsigned char source_stationID;
	unsigned char control;
	unsigned char protocolID;

    friend std::ostream& operator<<(std::ostream& out, KissHandle& K);
};

//old encode (to be deleted)**********************************************
int kissEncode(uint8_t *input, uint32_t count, uint8_t *encoded_packet);
//new encode
int kissEncode(uint8_t *input, uint32_t count, uint8_t *encoded_packet, KissHandle* handle);

//int kissDecode(uint8_t *input, uint32_t count, uint8_t *decoded_packet);

int kissDecode(uint8_t* kissed_input, uint32_t count, uint8_t* decoded_payload);

KissHandle kissInspect(const unsigned char* input);

//Functions to print packets
void print_ascii(unsigned char* packet, unsigned int count);
void print_hex(unsigned char* packet, unsigned int count);

#endif
