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
#include "device/general/ax25class.h"

Ax25Handle::Ax25Handle(string dest_call, string sour_call, uint8_t dest_stat, uint8_t sour_stat, uint8_t cont, uint8_t prot)
{
    set_destination_callsign(dest_call);
    set_destination_stationID(dest_stat);
    set_source_callsign(sour_call);
    set_source_stationID(sour_stat);
    set_control(cont);
    set_protocolID(prot);
}

//Set and get functions for all members of the Ax25Handle class
void Ax25Handle::set_destination_callsign(string destination)
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

string Ax25Handle::get_destination_callsign()
{
    string callsign;
    for (uint8_t byte : header.destination_callsign)
    {
        callsign.push_back(byte >> 1);
    }
    return callsign;
}

void Ax25Handle::set_destination_stationID(uint8_t ID)
{
    header.destination_stationID = ID;
    return;
}

uint8_t Ax25Handle::get_destination_stationID()
{
    return header.destination_stationID;
}

void Ax25Handle::set_source_callsign(string source)
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

string Ax25Handle::get_source_callsign()
{
    string callsign;
    for (uint8_t byte : header.source_callsign)
    {
        callsign.push_back(byte >> 1);
    }
    return callsign;
}

void Ax25Handle::set_source_stationID(uint8_t ID)
{
    header.source_stationID = ID;
    return;
}

uint8_t Ax25Handle::get_source_stationID()
{
    return header.source_stationID;
}

void Ax25Handle::set_control(uint8_t control_number)
{
    header.control = control_number;
    return;
}

uint8_t Ax25Handle::get_control()
{
    return header.control;
}

void Ax25Handle::set_protocolID(uint8_t protocol)
{
    header.protocolID = protocol;
    return;
}

uint8_t Ax25Handle::get_protocolID()
{
    return header.protocolID;
}

Ax25Handle::packet_header Ax25Handle::get_packetHeader()
{
    return header;
}

vector <uint8_t> Ax25Handle::get_packetData()
{
    return data;
}

int32_t Ax25Handle::set_packetData(vector<uint8_t> input)
{
    data = input;
    return 0;
}

int32_t Ax25Handle::set_raw_packet(vector <uint8_t> packet)
{
    raw_packet = packet;
    return 0;
}

int32_t Ax25Handle::load_packet()
{
    int32_t tsize = 16 + data.size();
    raw_packet.resize(tsize);
    memcpy(&raw_packet[0], &header, 16);
    memcpy(&raw_packet[16], &data[0], data.size());

    return tsize;
}

int32_t Ax25Handle::unload_packet()
{
    memcpy(&header, &raw_packet[0], 16);
    data.resize(raw_packet.size() - 16);
    memcpy(&data[0], &raw_packet[16], data.size());
    return 0;
}

::std::ostream& operator<< (::std::ostream& out, Ax25Handle& K)	{
    out<<"destination callsign=<"<< K.header.destination_callsign<<">"<<std::endl;
    out<<"destination station ID="<< K.header.destination_stationID<<std::endl;
    out<<"source callsign=<"<< K.header.source_callsign<<">"<<std::endl;
    out<<"source station ID="<< K.header.source_stationID<<std::endl;
    out<<"control="<< K.header.control<<std::endl;
    out<<"protocol ID="<< K.header.protocolID<<std::endl;

    return out;
}

