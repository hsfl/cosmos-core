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

#include "support/configCosmosKernel.h"
#include "support/ax25class.h"

namespace Cosmos {
    namespace Support {
        Ax25Handle::Ax25Handle(string dest_call, string sour_call, uint8_t dest_stat, uint8_t sour_stat, uint8_t cont, uint8_t prot)
        {
            set_destination_callsign(dest_call);
            set_destination_stationID(dest_stat);
            set_source_callsign(sour_call);
            set_source_stationID(sour_stat);
            set_control(cont);
            set_protocolID(prot);
            calc_crc.set("hdlc");
        }

        Ax25Handle::Ax25Handle()
        {
            set_destination_callsign("");
            set_destination_stationID(0x60);
            set_source_callsign("");
            set_source_stationID(0x61);
            set_control(0x03);
            set_protocolID(0xf0);
            calc_crc.set("hdlc");
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

        Ax25Handle::packet_header Ax25Handle::get_header()
        {
            return header;
        }

        vector <uint8_t> Ax25Handle::get_data()
        {
            return data;
        }

        vector <uint8_t> Ax25Handle::get_ax25_packet()
        {
            return ax25_packet;
        }

        vector <uint8_t> Ax25Handle::get_hdlc_packet()
        {
            return hdlc_packet;
        }

        int32_t Ax25Handle::set_data(vector<uint8_t> input)
        {
            data = input;
            return 0;
        }

        int32_t Ax25Handle::set_ax25_packet(vector <uint8_t> packet)
        {
            ax25_packet = packet;
            return 0;
        }

        int32_t Ax25Handle::set_hdlc_packet(vector <uint8_t> packet)
        {
            hdlc_packet = packet;
            return 0;
        }

        int32_t Ax25Handle::load(vector<uint8_t> data)
        {
            //    if (newdata.size())
            //    {
            //        data = newdata;
            //    }
            int32_t tsize = 18 + data.size();
            ax25_packet.resize(tsize);
            memcpy(&ax25_packet[0], &header, 16);
            memcpy(&ax25_packet[16], &data[0], data.size());
            crccalc = calc_crc.calc(&ax25_packet[0], ax25_packet.size()-2);
            ax25_packet[ax25_packet.size()-1] = (crccalc>>8);
            ax25_packet[ax25_packet.size()-2] = (crccalc&0xff);

            return tsize;
        }

        int32_t Ax25Handle::unload()
        {
            memcpy(&header, &ax25_packet[0], 16);
            data.resize(ax25_packet.size() - 18);
            memcpy(&data[0], &ax25_packet[16], data.size());
            crc = ax25_packet[ax25_packet.size()-1];
            crc = crc << 8;
            crc = crc + ax25_packet[ax25_packet.size()-2];
            crccalc = calc_crc.calc(&ax25_packet[0], ax25_packet.size()-2);
            return 0;
        }

        int32_t Ax25Handle::stuff(vector<uint8_t> ax25data, uint8_t flagcount, uint8_t flag)
        {
            if (ax25data.size())
            {
                ax25_packet = ax25data;
            }

            uint8_t bit_count = 7;
            uint8_t hdlcbyte = 0;
            uint8_t run_count = 0;

            hdlc_packet.resize(flagcount);
            for (uint8_t i=0; i<flagcount; ++i)
            {
                hdlc_packet[i] = flag;
            }
            for (uint8_t ax25byte : ax25_packet)
            {
                for (uint8_t bit_num=0; bit_num<8; ++bit_num)
                {
                    uint8_t bit = (ax25byte >> bit_num) & 1U;
                    if (bit)
                    {
                        hdlcbyte |= 1UL << bit_count;
                        ++run_count;
                        if (run_count == 5)
                        {
                            //                    hdlcbyte &= ~(1UL << bit_count);
                            if (--bit_count > 7)
                            {
                                hdlc_packet.push_back(hdlcbyte);
                                hdlcbyte = 0;
                                bit_count = 7;
                            }
                            run_count = 0;
                        }
                    }
                    else
                    {
                        run_count = 0;
                    }
                    if (--bit_count > 7)
                    {
                        hdlc_packet.push_back(hdlcbyte);
                        hdlcbyte = 0;
                        bit_count = 7;
                    }
                }
            }
            for (uint8_t i=0; i<flagcount; ++i)
            {
                for (uint8_t bit_num=0; bit_num<8; ++bit_num)
                {
                    uint8_t bit = (flag >> bit_num) & 1U;
                    if (bit)
                    {
                        hdlcbyte |= 1UL << bit_count;
                    }
                    if (--bit_count > 7)
                    {
                        hdlc_packet.push_back(hdlcbyte);
                        hdlcbyte = 0;
                        bit_count = 7;
                    }
                }
            }
            return hdlc_packet.size();
        }

        int32_t Ax25Handle::unstuff(vector<uint8_t> hdlcdata, uint8_t flag)
        {
            if (hdlcdata.size())
            {
                hdlc_packet = hdlcdata;
            }

            ax25_packet.clear();
            uint8_t bit_count = 0;
            uint8_t ax25byte = 0;
            uint8_t run_count = 0;
            for (uint8_t hdlcbyte : hdlc_packet)
            {
                if (hdlcbyte == flag)
                {
                    continue;
                }
                for (uint8_t bit_num=7; bit_num<8; --bit_num)
                {
                    uint8_t bit = (hdlcbyte >> bit_num) & 1U;
                    if (bit)
                    {
                        ++run_count;
                        ax25byte |= 1UL << bit_count;
                    }
                    else
                    {
                        if (run_count == 5)
                        {
                            --bit_count;
                        }
                        run_count = 0;
                    }

                    if (++bit_count > 7)
                    {
                        if (ax25byte == flag)
                        {
                            break;
                        }
                        ax25_packet.push_back(ax25byte);
                        ax25byte = 0;
                        bit_count = 0;
                    }
                }
                if (ax25byte == flag)
                {
                    break;
                }
            }
            return ax25_packet.size();
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
    }
}

