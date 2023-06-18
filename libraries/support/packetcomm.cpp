#include "packetcomm.h"
namespace Cosmos {
    namespace Support {
        PacketComm::PacketComm(uint16_t size)
        {
            if (size >= 4)
            {
                data.resize(size);
            }
            else
            {
                data.resize(4);
            }
            uint32to(decisec(), &data[0], ByteOrder::LITTLEENDIAN);
            RawPacketize();
        }

        void PacketComm::Invert(vector<uint8_t> &data)
        {
            vector<uint8_t> result;
            uint8from(data, result, ByteOrder::BIGENDIAN);
            data = result;
            return;
        }

        void PacketComm::CalcCRC()
        {
            crc = calc_crc.calc(data);
        }

        bool PacketComm::CheckCRC()
        {
            return true;
        }

        int32_t PacketComm::Unwrap(bool checkcrc)
        {
            return Unwrap(checkcrc, false);
        }

        //! \param checkcrc Perform crc check validation
        //! \return Data size on success, negative on error
        int32_t PacketComm::Unwrap(bool checkcrc, bool minimal_header)
        {
            style = PacketStyle::None;
            if (wrapped.size() <= 0)
            {
                return GENERAL_ERROR_BAD_SIZE;
            }

            switch(minimal_header)
            {
            case true:
                {
                    // Short packet
                    header.data_size = wrapped.size() - 1;
                    data.clear();
                    data.insert(data.begin(), &wrapped[1], &wrapped[header.data_size+1]);
                    uint8_t cs = 0;
                    for (uint8_t byte : data)
                    {
                        cs += byte;
                    }
                    if (cs != wrapped[0] >> 4)
                    {
                        return GENERAL_ERROR_CRC;
                    }
                    header.type = static_cast<PacketComm::TypeId>(wrapped[0] & 0x0f);
                    style = PacketStyle::Minimal;
                    header.nodeorig = 254;
                    header.nodedest = 255;
                    header.chanin = 0;
                    header.chanout = 0;
                    return data.size();
                }
            case false:
                {
                    // V2 packet
                    if (wrapped.size() >= COSMOS_SIZEOF(Header))
                    {
                        memcpy(&header, &wrapped[0], COSMOS_SIZEOF(Header));
                        header.data_size = uint16from(reinterpret_cast<uint8_t *>(&header.data_size));
                        uint32_t wrapsize = header.data_size + COSMOS_SIZEOF(Header) + 2;
                        if (wrapped.size() >= wrapsize)
                        {
                            if (checkcrc)
                            {
                                uint16_t crcin = uint16from(&wrapped[header.data_size+COSMOS_SIZEOF(Header)], ByteOrder::LITTLEENDIAN);
                                crc = calc_crc.calc(wrapped.data(), wrapsize-2);
                                if (crc != crcin)
                                {
                                    checkcrc = false;
                                    wrapped.resize(wrapsize);
                                    style = PacketStyle::V2;
                                }
                            }
                            else
                            {
                                checkcrc = true;
                            }
                            if (checkcrc)
                            {
                                data.clear();
                                data.insert(data.begin(), &wrapped[COSMOS_SIZEOF(Header)], &wrapped[header.data_size+COSMOS_SIZEOF(Header)]);
                                style = PacketStyle::V2;
                                return data.size();
                            }
                        }
                    }
                }
            }
            return DATA_ERROR_CRC;
        }

        int32_t PacketComm::RawUnPacketize(bool invert, bool checkcrc)
        {
            if (invert)
            {
                uint8from(packetized, wrapped, ByteOrder::BIGENDIAN);
            }
            else
            {
                wrapped = packetized;
            }
            return Unwrap(checkcrc);
        }

        int32_t PacketComm::RawUnPacketize(bool invert, bool checkcrc, bool minimal_header)
        {
            if (invert)
            {
                uint8from(packetized, wrapped, ByteOrder::BIGENDIAN);
            }
            else
            {
                wrapped = packetized;
            }
            return Unwrap(checkcrc, minimal_header);
        }

        bool PacketComm::SLIPUnPacketize(bool checkcrc)
        {
            int32_t iretn = slip_unpack(packetized, wrapped);
            if (iretn <= 0)
            {
                return false;
            }
            return Unwrap(checkcrc);
        }

        bool PacketComm::ASMUnPacketize()
        {
            return (ASMUnPacketize(true, false));
        }

        bool PacketComm::ASMUnPacketize(bool checkcrc, bool descramble)
        {
            wrapped.clear();
            if (atsm[0] == packetized[0] && atsm[1] == packetized[1] && atsm[2] == packetized[2] && atsm[3] == packetized[3])
            {
                wrapped.insert(wrapped.begin(), &packetized[4], &packetized[packetized.size()]);
            }
            else if (atsmr[0] == packetized[0] && atsmr[1] == packetized[1] && atsmr[2] == packetized[2] && atsmr[3] == packetized[3])
            {
                vector<uint8_t> input;
                input.insert(input.begin(), &packetized[4], &packetized[packetized.size()]);
                uint8from(input, wrapped, ByteOrder::BIGENDIAN);
            }
            else
            {
                return false;
            }
            // Apply Galois LFSR (de)scrambling to avoid long contiguous sequences of 0s or 1s
            if (descramble)
            {
                uint16_t lfsr = 0xACE1u;
                const uint16_t poly = 0x8016;
                for (size_t i=0; i<wrapped.size(); ++i)
                {
                    uint16_t lsb = lfsr & 0x1;
                    lfsr >>= 1;
                    if (lsb)
                    {
                        lfsr ^= poly;
                    }
                    wrapped[i] ^= lfsr;
                }
            }
            return (Unwrap(checkcrc) >= 0);
        }

        bool PacketComm::HDLCUnPacketize(bool checkcrc)
        {
            Ax25Handle axhandle;
            axhandle.unstuff(packetized);
            wrapped = axhandle.ax25_packet;
            return Unwrap(checkcrc);
        }

        bool PacketComm::AX25UnPacketize(bool checkcrc)
        {
            int32_t iretn = 0;
            Ax25Handle axhandle;
            axhandle.set_ax25_packet(packetized);
            iretn = axhandle.unload(checkcrc);
            if (iretn >= 0)
            {
                wrapped = axhandle.ax25_packet;
                return Unwrap(checkcrc);
            }
            else
            {
                wrapped.clear();
                return false;
            }
//            if (packetized.size() > 18)
//            {
//                axhandle.ax25_packet.clear();
//                axhandle.ax25_packet.insert(axhandle.ax25_packet.begin(), packetized.begin() + 16, packetized.end() - 2);
//                packetized = axhandle.ax25_packet;
//                uint8from(packetized, wrapped, ByteOrder::BIGENDIAN);
//            }
//            else
//            {
//                packetized.clear();
//            }
        }

        //! \brief Wrap up header and payload
        //! Merge ::Cosmos::Support::PacketComm::data and ::Cosmos::Support::PacketComm::header
        //! into ::Cosmos::Support::PacketComm::wrapped
        //! \return Boolean success
        bool PacketComm::Wrap()
        {
            return Wrap(true);
        }

        //! \brief Wrap up header and payload
        //! Merge ::Cosmos::Support::PacketComm::data and ::Cosmos::Support::PacketComm::header
        //! into ::Cosmos::Support::PacketComm::wrapped
        //! \param calc_checksum If false, skips crc calculation
        //! \return Boolean success
        bool PacketComm::Wrap(bool calc_checksum)
        {
            switch (style)
            {
            case PacketStyle::Minimal:
                {
                    header.data_size = data.size();
                    uint8_t cs = 0;
                    for (uint8_t byte : data)
                    {
                        cs += byte;
                    }
                    wrapped.resize(1);
                    wrapped[0] = ((cs & 0x0f) << 4) + (static_cast<uint8_t>(header.type) & 0x0f);
                    wrapped.insert(wrapped.end(), data.begin(), data.end());
                }
                break;
//            case PacketStyle::V1:
//                {
//                    header.data_size = data.size();
//                    headerv1.data_size = header.data_size;
//                    headerv1.type = static_cast<TypeIdV1>(header.type);
//                    headerv1.nodeorig = header.nodeorig;
//                    headerv1.nodedest = header.nodedest;
//                    headerv1.chanin = header.chanin;
//                    wrapped.resize(COSMOS_SIZEOF(HeaderV1));
//                    memcpy(&wrapped[0], &headerv1, COSMOS_SIZEOF(HeaderV1));
//                    wrapped.insert(wrapped.end(), data.begin(), data.end());
//                    crc = calc_crc.calc(wrapped);
//                    wrapped.resize(wrapped.size()+2);
//                    wrapped[wrapped.size()-2] = crc & 0xff;
//                    wrapped[wrapped.size()-1] = crc >> 8;
//                }
//                break;
            case PacketStyle::V2:
                {
                    header.data_size = data.size();
                    wrapped.resize(COSMOS_SIZEOF(Header));
                    memcpy(&wrapped[0], &header, COSMOS_SIZEOF(Header));
                    wrapped.insert(wrapped.end(), data.begin(), data.end());
                    crc = calc_checksum ? calc_crc.calc(wrapped) : 0;
                    wrapped.resize(wrapped.size()+2);
                    wrapped[wrapped.size()-2] = crc & 0xff;
                    wrapped[wrapped.size()-1] = crc >> 8;
                }
                break;
            default:
                {
                    return false;
                }
                break;
            }

            return true;
        }

        bool PacketComm::RawPacketize()
        {
            if (!Wrap())
            {
                return false;
            }
            packetized.clear();
            packetized.insert(packetized.begin(), wrapped.begin(), wrapped.end());
            return true;
        }

        bool PacketComm::SLIPPacketize()
        {
            if (!Wrap())
            {
                return false;
            }
            if (slip_pack(wrapped, packetized) < 0)
            {
                return false;
            }
            return true;
        }

        bool PacketComm::ASMPacketize()
        {
            // Default call has no padding at the end
            return ASMPacketize(data.size() + sizeof(PacketComm::header) + 2 + atsm.size(), false);
        }

        //! @param packet_wrapped_size Size to stuff a packet up to for fixed-sized requirements
        bool PacketComm::ASMPacketize(uint16_t packet_wrapped_size, bool scramble)
        {
            // 2 is crc size
            if (data.size() + sizeof(PacketComm::header) + 2 + atsm.size() > packet_wrapped_size)
            {
                return false;
            }
            if (!Wrap())
            {
                return false;
            }
            
            packetized.clear();
            packetized.insert(packetized.begin(), atsm.begin(), atsm.end());
            packetized.insert(packetized.end(), wrapped.begin(), wrapped.end());
            // Adjust packet size to specified padded size
            // XBand seems to ignore packets if it's all just 0 or 1 (though it seems to like other numbers), so just fill with a sequence
            packetized.resize(packet_wrapped_size, 0);
            // Apply Galois LFSR scrambling to avoid long contiguous sequences of 0s or 1s
            if (scramble)
            {
                uint16_t lfsr = 0xACE1u;
                const uint16_t poly = 0x8016;
                for (size_t i=atsm.size(); i<packetized.size(); ++i)
                {
                    uint16_t lsb = lfsr & 0x1;
                    lfsr >>= 1;
                    if (lsb)
                    {
                        lfsr ^= poly;
                    }
                    packetized[i] ^= lfsr;
                }
            }
            return true;
        }

        bool PacketComm::AX25Packetize(string dest_call, string sour_call, uint8_t flagcount, uint8_t dest_stat, uint8_t sour_stat, uint8_t cont, uint8_t prot)
        {
            if (!Wrap())
            {
                return false;
            }
            Ax25Handle axhandle(dest_call, sour_call, dest_stat, sour_stat, cont, prot);
            axhandle.load(wrapped);
            axhandle.stuff({}, flagcount);
            vector<uint8_t> ax25packet = axhandle.get_hdlc_packet();
            packetized.clear();
            packetized.insert(packetized.begin(), ax25packet.begin(), ax25packet.end());
            return true;
        }

        bool PacketComm::HDLCPacketize(uint8_t flagcount)
        {
            if (!Wrap())
            {
                return false;
            }
            Ax25Handle axhandle;
            axhandle.stuff(wrapped, flagcount);
            packetized = axhandle.get_hdlc_packet();
            return true;
        }
    }
}
