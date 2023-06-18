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
#include "device/general/kisslib.h"

KissHandle::KissHandle(string dest_call, string sour_call, uint8_t port, uint8_t comm, uint8_t dest_stat, uint8_t sour_stat, uint8_t cont, uint8_t prot)
{
    set_port_number(port);
    set_command(comm);
    set_destination_callsign(dest_call);
    set_destination_stationID(dest_stat);
    set_source_callsign(sour_call);
    set_source_stationID(sour_stat);
    set_control(cont);
    set_protocolID(prot);
}

//Set and get functions for all members of the KissHandle class
void KissHandle::set_port_number(uint8_t P)
{
    header.port_number = P;
	return;
}

uint8_t KissHandle::get_port_number()
{
    return header.port_number;
}

void KissHandle::set_command(uint8_t C)
{
    header.command = C;
	return;	
}

uint8_t KissHandle::get_command()
{
    return header.command;

}

void KissHandle::set_destination_callsign(string destination)
{
    for (uint16_t i=0; i<6; ++i)
    {
        if (i >= destination.length())
        {
            header.destination_callsign[i] = 0x20 << 1;
        }
        else
        {
            header.destination_callsign[i] = destination[i] << 1;
        }
    }
	return;
}

string KissHandle::get_destination_callsign()
{
    string callsign;
    for (uint8_t byte : header.destination_callsign)
    {
        callsign.push_back(byte >> 1);
    }
    return callsign;
}

void KissHandle::set_destination_stationID(uint8_t ID)
{
    header.destination_stationID = ID;
	return;
}

uint8_t KissHandle::get_destination_stationID()
{
    return header.destination_stationID;
}

void KissHandle::set_source_callsign(string source)
{
    for (uint16_t i=0; i<6; ++i)
    {
        if (i >= source.length())
        {
            header.source_callsign[i] = 0x20 << 1;
        }
        else
        {
            header.source_callsign[i] = source[i] << 1;
        }
    }
    return;
}

string KissHandle::get_source_callsign()
{
    string callsign;
    for (uint8_t byte : header.source_callsign)
    {
        callsign.push_back(byte >> 1);
    }
    return callsign;
}

void KissHandle::set_source_stationID(uint8_t ID)
{
    header.source_stationID = ID;
	return;
}

uint8_t KissHandle::get_source_stationID()
{
    return header.source_stationID;
}

void KissHandle::set_control(uint8_t control_number)
{
    header.control = control_number;
	return;
}

uint8_t KissHandle::get_control()
{
    return header.control;
}

void KissHandle::set_protocolID(uint8_t protocol)
{
    header.protocolID = protocol;
	return;
}

uint8_t KissHandle::get_protocolID()
{
    return header.protocolID;
}

::std::ostream& operator<< (::std::ostream& out, KissHandle& K)	{
    out<<"port_number= "<< K.header.port_number<<std::endl;
    out<<"command="<< K.header.command<<std::endl;
    out<<"destination callsign=<"<< K.header.destination_callsign<<">"<<std::endl;
    out<<"destination station ID="<< K.header.destination_stationID<<std::endl;
    out<<"source callsign=<"<< K.header.source_callsign<<">"<<std::endl;
    out<<"source station ID="<< K.header.source_stationID<<std::endl;
    out<<"control="<< K.header.control<<std::endl;
    out<<"protocol ID="<< K.header.protocolID<<std::endl;

	return out;
}

// Encodes input (using KISS protocol) into a packet ready for transmission
// returns bytes in payload of packet



int kissEncode(uint8_t *input, uint32_t count, uint8_t *packet)
{
	uint32_t payload_bytes = 0;

	//If payload is over 255 bytes indicate an error
	if(count > 255)
	{
		printf("Error: Limit input to 255 8-bit character bytes\n");
		return -1;
	}

//Build Packet Header

	//Frame End (1 Byte)
    *packet = SLIP_FEND;
	//Port number and command type (1Byte)
    *(packet+1) = 0x10;
	//Destination call sign(6Bytes)
    *(packet+2) = ('W' << 1);
    *(packet+3) = ('H' << 1);
    *(packet+4) = ('7' << 1);
    *(packet+5) = ('L' << 1);
    *(packet+6) = ('G' << 1);
    *(packet+7) = (0x20 << 1);
	//Destination station ID (1Byte)
    *(packet+8) = 0X60;
	//Source call sign (6Bytes)
    *(packet+9) = ('W' << 1);
    *(packet+10) = ('H' << 1);
    *(packet+11) = ('7' << 1);
    *(packet+12) = ('L' << 1);
    *(packet+13) = ('E' << 1);
    *(packet+14) = (0x20 << 1);
	//Source station ID (1Byte)
    *(packet+15) = 0x61;
	//Control(1Byte)
    *(packet+16) = 0X31;
	//Protocol ID (1Byte)
    *(packet+17) = 0xF0; // 0xF0 = No layer 3 protocol

    packet += 18;

	for(uint32_t i=0; i<count; i++)
	{
		// Replace FEND with FESC TFEND
        if (input[i] == SLIP_FEND)
		{
            *packet++ = SLIP_FESC;
            *packet++ = SLIP_TFEND;
			payload_bytes += 2;	// Advance Payload Data Count 2 Bytes
		} 
		// Replace FESC with FESC TFESC
        else if (input[i] == SLIP_FESC)
		{
            *packet++ = SLIP_FESC;
            *packet++ = SLIP_TFESC;
			payload_bytes += 2;	// Advance Payload Data Count 2 Bytes
		}
		// Copy Input
		else
		{
            *packet++ = input[i];
			payload_bytes++;	// Advance Payload Data Count 1 Byte
		}
	}

	//Placing final frame-end
    *packet= SLIP_FEND;

	//Keeping track of the count (payload bytes + header bytes + trailer)
	payload_bytes += 19;

	return payload_bytes;
}

int32_t KissHandle::set_data(vector<uint8_t> input)
{
    data = input;
    return 0;
}

int32_t KissHandle::set_raw_packet(vector<uint8_t> packet)
{
    raw_packet = packet;
    return 0;
}

int32_t KissHandle::set_slip_packet(vector<uint8_t> packet)
{
    slip_packet = packet;
    return 0;
}

vector <uint8_t> KissHandle::get_data()
{
    return data;
}

int32_t KissHandle::load_packet()
{
    int32_t tsize = 17 + data.size();
    raw_packet.resize(tsize);
    memcpy(&raw_packet[0], &header, 17);
    memcpy(&raw_packet[17], &data[0], data.size());
    slip_encode(raw_packet, slip_packet);

    return tsize;
}

int32_t KissHandle::unload_packet()
{
    slip_decode(slip_packet, raw_packet);
    memcpy(&header, &raw_packet[0], 17);
    data.resize(raw_packet.size() - 17);
    memcpy(&data[0], &raw_packet[17], data.size());
    return 0;
}

int32_t KissHandle::get_error()
{
    return error;
}

vector <uint8_t> KissHandle::get_raw_packet()
{
    return raw_packet;
}

vector <uint8_t> KissHandle::get_slip_packet()
{
    return slip_packet;
}

KissHandle kissInspect(const unsigned char* input)	{

	//fill me up with goodness
	KissHandle KKK;

//	uint8_t port_command;
	unsigned char destination_call_sign[6];
	uint8_t destination_station_id;//Changed this to an array of [1]
	unsigned char source_call_sign[6];
	uint8_t source_station_id;//changed this to an array of [1]
	uint8_t control_id;//changed this to an array of [1]
	uint8_t protocol_id;//changed this to an array of [1]

	
	// First character of input is always FEND
	// The next 17 characters we are interested in...
//	port_command = input[1];
	destination_station_id = input[8];
	source_station_id = input[15];
	control_id = input[16];
	protocol_id = input[17];
	
		
	//Extract destination call sign
	//printf("Destination Call Sign = ");
	for(int i=2;i<=7;i++)
	{
		destination_call_sign[i-2] = input[i] >> 1;
		//printf("%c", destination_call_sign[i-2]);
	}
	//Copying destination callsign into KissHandle
    KKK.set_destination_callsign((const char*) destination_call_sign);
    //strcpy((char *)KKK.header.destination_callsign, (const char *) destination_call_sign);
	
	//Copying destination callsign ID into KissHandle
    KKK.set_destination_stationID((uint8_t) destination_station_id);
    //KKK.header.destination_stationID = destination_station_id;

	//printf("\n");
	
	//Extract source call sign
	//printf("Source Call Sign = ");
	for(int i=9;i<=14;i++)
	{
		source_call_sign[i-9] = input[i] >> 1;
		//printf("%c", source_call_sign[i-9]);
	}
	//Copying source callsign into KissHandle
    KKK.set_source_callsign((const char*) source_call_sign);
    //strcpy((char *)KKK.header.source_callsign, (const char *) source_call_sign);
	
	//Copying source station ID into KissHandle
    KKK.set_source_stationID((uint8_t) source_station_id);
    //KKK.header.source_stationID = source_station_id;
	
	//Copying control into KissHandle
    KKK.set_control((uint8_t) control_id);
    //KKK.header.control = control_id;

	//Copying protocol ID into KissHandle
    KKK.set_protocolID((uint8_t) protocol_id);
    //KKK.header.protocolID = protocol_id;
	
	
	//printf("\n");

	//printf("control id: %d", control_id[0]);
	//printf("protocol id: %d", protocol_id[0]);

	return KKK;
}


// Decodes input (using KISS protocol) into payload
// returns bytes in payload of packet
int kissDecode(uint8_t* kissed_input, uint32_t, uint8_t* unload_packetd_payload)
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint8_t load_packetd_payload[PACKETMAX];

    ///Encoded payload will act as an intermediate packet holding the load_packetd data.
    //It will be used to hold the load_packetd data while being decoding.
    for(i=18; kissed_input[i] != SLIP_FEND; i++)	// FEND = FEND
    {
        j = i - 18; // pointer to 0 in intermediate
        load_packetd_payload[j] = kissed_input[i];
    }

    ///Frame end appended to packet after all the header is removed.
    //We do this to pinpoint where we can stop the decoding process.
    load_packetd_payload[j+1]= SLIP_FEND;


    //check the read in data
    i = 0;
    j = 0;

    for(;j <= PACKETMAX;)
    {
        //Function will return i once the appended FEND is read in
        if(load_packetd_payload[j] == SLIP_FEND)
            return i;

        if(load_packetd_payload[j] == SLIP_FESC) //if FESC
        {
            if(load_packetd_payload[j+1] == SLIP_TFEND) // Check for TFEND
            {
                load_packetd_payload[j] = SLIP_FEND;
                unload_packetd_payload[i] = load_packetd_payload[j];
                j+=2;
                i++;
            }

            else if(load_packetd_payload[j+1] == SLIP_TFESC)	// Check for TFESC
            {
                load_packetd_payload[j] = SLIP_FESC;
                unload_packetd_payload[i] = load_packetd_payload[j];
                j+=2;
                i++;
            }

            else
            {
                printf("Invalid Transpose Error!!!\n"); //Error occurred if TFEND or TFESC does not follow a FESC
            }
        }

        else
        {
            unload_packetd_payload[i] = load_packetd_payload[j];
            j++;
            i++;
        }
    }

    return 0;
}


//Packet printing functions

void print_ascii(unsigned char* packet, uint8_t count)
{

    std::cout<<"<";
    for(uint8_t i=0; i<count; i++)
		printf("%c", packet[i]);	
    std::cout<<">"<<std::endl;

	return;
}


void print_hex(unsigned char* packet, uint8_t count)
{
    std::cout<<"<";
    for(uint8_t i=0; i<count; i++)
		printf("%02x", packet[i]);	
    std::cout<<">"<<std::endl;

	return;
}
