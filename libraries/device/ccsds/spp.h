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

#ifndef SPP_H
#define SPP_H

#include "support/configCosmos.h"
#include "support/sliplib.h"

namespace Cosmos {
    namespace Protocols {
        namespace Ccsds {

            class Spp
            {
            public:
                Spp(uint16_t apid=0, bool telecommand=static_cast<bool>(PacketType::Telemetry), bool secondary_header=false, uint8_t version=0);
//                ~Spp();


                enum class PacketType : uint8_t
                    {
                    Telemetry,
                    Telecommand
                    };

                enum class SequenceFlags : uint8_t
                    {
                    ContinuationSegment,
                    FirstSegment,
                    LastSegment,
                    UnSegmented
                    };

                enum class PacketStage : uint16_t
                    {
                    Start,
                    HeaderByte0,
                    HeaderByte1,
                    HeaderByte2,
                    HeaderByte3,
                    HeaderByte4,
                    HeaderByte5,
                    DataBytes
                    };

                int32_t setHeaderByte(uint8_t number, uint8_t value);
                int32_t setVersion(uint8_t version);
                int32_t setApid(uint16_t apid);
                int32_t setApidIdle();
                int32_t setType(PacketType type);
                int32_t setSecondaryHeaderFlag(bool present);
                int32_t setSequenceFlags(SequenceFlags flag);
                int32_t setSequenceCount(uint16_t count);
                int32_t setDataLength(uint32_t length);

                uint8_t getVersion();
                uint16_t getApid();
                PacketType getType();
                bool getSecondaryHeaderFlag();
                SequenceFlags getSequenceFlags();
                uint16_t getSequenceCount();
                uint32_t getDataLength();

                int32_t clearPacket();
                int32_t clearHeaderBytes();
                int32_t clearDataBytes();
                int32_t getDataBytes(vector <uint8_t> &dbytes);
                int32_t addByte(uint8_t dbyte);
                int32_t addDataByte(uint8_t dbyte);
                int32_t setDataBytes(vector <uint8_t> &dbytes);

                struct primary_header
                {
                    unsigned apid_msb : 3;
                    unsigned secondary_header_flag : 1;
                    unsigned type : 1;
                    unsigned version : 3;
                    unsigned apid_lsb : 8;
                    unsigned sequence_count_msb : 6;
                    unsigned sequence_flags : 2;
                    unsigned sequence_count_lsb : 8;
                    unsigned data_length_msb : 8;
                    unsigned data_length_lsb : 8;
                };

                struct packet
                {
                    union
                    {
                        uint8_t primary_header_bytes[6];
                        primary_header primary_header_fields;
                    };
                    vector <uint8_t> data_bytes;
                };

            protected:
                packet frame;
                int32_t error;
                PacketStage stage=PacketStage::Start;
            };
        }
    }

} // end of namepsace Cosmos
#endif // SPP_H
