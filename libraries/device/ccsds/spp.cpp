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

// TODO: rename to serial.cpp only
#include "spp.h"

namespace Cosmos {
    namespace Protocols {
        namespace Ccsds {

            Spp::Spp(uint16_t apid, bool telecommand, bool secondary_header, uint8_t version)
            {
                frame.primary_header_fields.version = version;
                frame.primary_header_fields.apid_msb = apid / 256;
                frame.primary_header_fields.apid_lsb = apid % 256;
                if (telecommand)
                {
                    frame.primary_header_fields.type = 1;
                }
                else
                {
                    frame.primary_header_fields.type = 0;
                }
                if (secondary_header)
                {
                    frame.primary_header_fields.secondary_header_flag = 1;
                }
                else
                {
                    frame.primary_header_fields.secondary_header_flag = 0;
                }
            }

            int32_t Spp::setHeaderByte(uint8_t number, uint8_t value)
            {
                if (number > 6)
                {
                    return GENERAL_ERROR_OUTOFRANGE;
                }
                frame.primary_header_bytes[number] = value;
                return number;
            }

            int32_t Spp::setVersion(uint8_t version)
            {
                if (version < 8)
                {
                    frame.primary_header_fields.version = version;
                    return 0;
                }
                else
                {
                    return GENERAL_ERROR_OUTOFRANGE;
                }
            }

            int32_t Spp::setApid(uint16_t apid)
            {
                if (apid < 2048)
                {
                    frame.primary_header_fields.apid_msb = apid / 256;
                    frame.primary_header_fields.apid_lsb = apid % 256;
                    return 0;
                }
                else
                {
                    return GENERAL_ERROR_OUTOFRANGE;
                }
            }

            int32_t Spp::setApidIdle()
            {
                frame.primary_header_fields.apid_lsb = 255;
                frame.primary_header_fields.apid_msb = 7;
                return 0;
            }

            int32_t Spp::setType(PacketType type)
            {
                if (type == PacketType::Telecommand || type == PacketType::Telemetry)
                {
                    frame.primary_header_fields.type = (unsigned)type;
                    return 0;
                }
                else
                {
                    return GENERAL_ERROR_OUTOFRANGE;
                }
            }

            int32_t Spp::setSecondaryHeaderFlag(bool present)
            {
                if (present)
                {
                    frame.primary_header_fields.secondary_header_flag = 1;
                }
                else
                {
                    frame.primary_header_fields.secondary_header_flag = 0;
                }
                return 0;
            }

            int32_t Spp::setSequenceCount(uint16_t count)
            {
                if (count < 16384)
                {
                    frame.primary_header_fields.sequence_count_lsb = count % 256;
                    frame.primary_header_fields.sequence_count_msb = count / 256;
                    return 0;
                }
                else
                {
                    return GENERAL_ERROR_OUTOFRANGE;
                }
            }

            int32_t Spp::setSequenceFlags(SequenceFlags flag)
            {
                if (flag == SequenceFlags::ContinuationSegment || flag == SequenceFlags::FirstSegment || flag == SequenceFlags::LastSegment || flag == SequenceFlags::UnSegmented)
                {
                    frame.primary_header_fields.sequence_flags = (unsigned)flag;
                    return 0;
                }
                else
                {
                    return GENERAL_ERROR_OUTOFRANGE;
                }
            }

            int32_t Spp::setDataLength(uint32_t length)
            {
                if (length < 65537 && length > 0)
                {
                    length--;
                    frame.primary_header_fields.data_length_lsb = length % 256;
                    frame.primary_header_fields.data_length_msb = length / 256;
                    return 0;
                }
                else
                {
                    return GENERAL_ERROR_OUTOFRANGE;
                }
            }

            uint8_t Spp::getVersion()
            {
                return frame.primary_header_fields.version;
            }

            uint16_t Spp::getApid()
            {
                return 16 * frame.primary_header_fields.apid_lsb + frame.primary_header_fields.apid_msb;
            }

            Spp::PacketType Spp::getType()
            {
                return (PacketType)frame.primary_header_fields.type;
            }

            bool Spp::getSecondaryHeaderFlag()
            {
                return frame.primary_header_fields.secondary_header_flag;
            }

            Spp::SequenceFlags Spp::getSequenceFlags()
            {
                return (SequenceFlags)frame.primary_header_fields.sequence_flags;
            }

            uint16_t Spp::getSequenceCount()
            {
                return frame.primary_header_fields.sequence_count_msb * 256L + frame.primary_header_fields.sequence_count_lsb;
            }

            uint32_t Spp::getDataLength()
            {
                return frame.primary_header_fields.data_length_msb * 256L + frame.primary_header_fields.data_length_lsb + 1;
            }

            int32_t Spp::clearPacket()
            {
                stage = PacketStage::Start;
                frame.data_bytes.clear();
                return 0;
            }

            int32_t Spp::clearDataBytes()
            {
                frame.data_bytes.clear();
                return 0;
            }

            int32_t Spp::addByte(uint8_t byte)
            {
                switch (stage)
                {
                case PacketStage::Start:
                case PacketStage::HeaderByte0:
                case PacketStage::HeaderByte1:
                case PacketStage::HeaderByte2:
                case PacketStage::HeaderByte3:
                case PacketStage::HeaderByte4:
                    frame.primary_header_bytes[static_cast<uint16_t>(stage)] = byte;
                    stage = static_cast<PacketStage>(static_cast<uint16_t>(stage) + 1);
                    return static_cast<int32_t>(stage);
                case PacketStage::HeaderByte5:
                    frame.data_bytes.push_back(byte);
                    stage = PacketStage::DataBytes;
                    return static_cast<int32_t>(stage);
                case PacketStage::DataBytes:
                    if (frame.data_bytes.size() < 65536)
                    {
                        frame.data_bytes.push_back(byte);
                        return static_cast<int32_t>(stage);
                    }
                    else
                    {
                        return GENERAL_ERROR_OVERSIZE;
                    }
                }
                return GENERAL_ERROR_UNDEFINED;
            }

            int32_t Spp::addDataByte(uint8_t dbyte)
            {
                if (frame.data_bytes.size() < 65536)
                {
                    frame.data_bytes.push_back(dbyte);
                    return frame.data_bytes.size();
                }
                else
                {
                    return GENERAL_ERROR_OVERSIZE;
                }
            }

            int32_t Spp::getDataBytes(vector <uint8_t> &dbytes)
            {
                if (dbytes.size() < 65536)
                {
                    frame.data_bytes = dbytes;
                    return 0;
                }
                else
                {
                    return GENERAL_ERROR_OVERSIZE;
                }
            }

            int32_t Spp::setDataBytes(vector <uint8_t> &dbytes)
            {
                    dbytes = frame.data_bytes;
                    return 0;
            }

        }
    }
} // end of namepsace Cosmos
