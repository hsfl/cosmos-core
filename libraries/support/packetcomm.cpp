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

        void PacketComm::CalcCRC()
        {
            crc = calc_crc.calc(data);
        }

        bool PacketComm::CheckCRC()
        {
            return true;
        }

        //! \param checkcrc Perform crc check validation
        //! \return Data size on success, negative on error
        int32_t PacketComm::Unwrap(bool checkcrc)
        {
            style = PacketStyle::None;
            if (wrapped.size() <= 0)
            {
                return GENERAL_ERROR_BAD_SIZE;
            }

            // First: Try Long packet
            if (wrapped.size() >= COSMOS_SIZEOF(Header))
            {
                memcpy(&header, &wrapped[0], COSMOS_SIZEOF(Header));
                header.data_size = uint16from(reinterpret_cast<uint8_t *>(&header.data_size));
                uint32_t wrapsize = header.data_size + COSMOS_SIZEOF(Header) + 2;
                if (wrapped.size() >= wrapsize)
                {
                    uint16_t crcin = uint16from(&wrapped[header.data_size+COSMOS_SIZEOF(Header)], ByteOrder::LITTLEENDIAN);
                    crc = calc_crc.calc(wrapped.data(), wrapsize-2);
                    if (!checkcrc || crc == crcin)
                    {
                        wrapped.resize(wrapsize);
                        style = PacketStyle::V2;
                    }
                }
                data.clear();
                data.insert(data.begin(), &wrapped[COSMOS_SIZEOF(Header)], &wrapped[header.data_size+COSMOS_SIZEOF(Header)]);
                style = PacketStyle::V2;
                return data.size();
            }

            // Second: Try Medium packet
            if (wrapped.size() >= COSMOS_SIZEOF(HeaderV1))
            {
                memcpy(&headerv1, &wrapped[0], COSMOS_SIZEOF(HeaderV1));
                headerv1.data_size = uint16from(&headerv1.data_size, ByteOrder::BIGENDIAN);
                uint32_t wrapsize = headerv1.data_size + COSMOS_SIZEOF(HeaderV1) + 2;
                if (wrapped.size() >= wrapsize)
                {
                    uint16_t crcin = uint16from(&wrapped[headerv1.data_size+COSMOS_SIZEOF(HeaderV1)], ByteOrder::LITTLEENDIAN);
                    crc = calc_crc.calc(wrapped.data(), wrapsize-2);
                    if (!checkcrc || crc == crcin)
                    {
                        wrapped.resize(wrapsize);
                        style = PacketStyle::V2;
                    }
                }
                data.clear();
                data.insert(data.begin(), &wrapped[COSMOS_SIZEOF(HeaderV1)], &wrapped[headerv1.data_size+COSMOS_SIZEOF(HeaderV1)]);
                style = PacketStyle::V1;
                header.type = static_cast<TypeId>(headerv1.type);
                header.data_size = headerv1.data_size;
                header.nodeorig = headerv1.nodeorig;
                header.nodedest = headerv1.nodedest;
                header.chanorig = headerv1.chanorig;
                header.chandest = headerv1.chanorig;
                return data.size();
            }

            // If we get here: Assume Short packet
            // Short packet
            header.type = static_cast<PacketComm::TypeId>(wrapped[0]);
            header.data_size = wrapped.size() - 1;
            data.clear();
            data.insert(data.begin(), &wrapped[1], &wrapped[header.data_size+1]);
            style = PacketStyle::Minimal;
            header.nodeorig = 254;
            header.nodedest = 255;
            header.chanorig = 0;
            header.chandest = 0;
            return data.size();
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

        bool PacketComm::SLIPUnPacketize(bool checkcrc)
        {
            int32_t iretn = slip_unpack(packetized, wrapped);
            if (iretn <= 0)
            {
                return false;
            }
            return Unwrap(checkcrc);
        }

        bool PacketComm::ASMUnPacketize(bool checkcrc)
        {
            wrapped.clear();
            if (atsm[0] == packetized[0] && atsm[1] == packetized[1] && atsm[2] == packetized[2] && atsm[3] == packetized[3])
            {
                wrapped.insert(wrapped.begin(), &packetized[4], &packetized[packetized.size()]);
            }
            else if (atsmr[0] == packetized[0] && atsmr[1] == packetized[1] && atsmr[2] == packetized[2] && atsmr[3] == packetized[3])
            {
                vector<uint8_t> input;
                input.insert(wrapped.begin(), &packetized[4], &packetized[packetized.size()]);
                uint8from(input, wrapped, ByteOrder::BIGENDIAN);
            }
            return Unwrap(checkcrc);
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
            Ax25Handle axhandle;
            //            axhandle.unstuff(packetized);
            axhandle.unload();
            wrapped = axhandle.ax25_packet;
            return Unwrap(checkcrc);
        }

        //! \brief Wrap up header and payload
        //! Merge ::Cosmos::Support::PacketComm::data and ::Cosmos::Support::PacketComm::header
        //! into ::Cosmos::Support::PacketComm::wrapped
        //! \return Boolean success
        bool PacketComm::Wrap()
        {
            switch (style)
            {
            case PacketStyle::Minimal:
                {
                    header.data_size = data.size();
                    wrapped.resize(1);
                    wrapped[0] = static_cast<uint8_t>(header.type);
                    wrapped.insert(wrapped.end(), data.begin(), data.end());
                }
                break;
            case PacketStyle::V1:
                {
                    header.data_size = data.size();
                    headerv1.data_size = header.data_size;
                    headerv1.type = static_cast<TypeIdV1>(header.type);
                    headerv1.nodeorig = header.nodeorig;
                    headerv1.nodedest = header.nodedest;
                    headerv1.chanorig = header.chanorig;
                    wrapped.resize(COSMOS_SIZEOF(HeaderV1));
                    memcpy(&wrapped[0], &headerv1, COSMOS_SIZEOF(HeaderV1));
                    wrapped.insert(wrapped.end(), data.begin(), data.end());
                    crc = calc_crc.calc(wrapped);
                    wrapped.resize(wrapped.size()+2);
                    wrapped[wrapped.size()-2] = crc & 0xff;
                    wrapped[wrapped.size()-1] = crc >> 8;
                }
                break;
            case PacketStyle::V2:
                {
                    header.data_size = data.size();
                    wrapped.resize(COSMOS_SIZEOF(Header));
                    memcpy(&wrapped[0], &header, COSMOS_SIZEOF(Header));
                    wrapped.insert(wrapped.end(), data.begin(), data.end());
                    crc = calc_crc.calc(wrapped);
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
            if (!Wrap())
            {
                return false;
            }
            packetized.clear();
            packetized.insert(packetized.begin(), atsm.begin(), atsm.end());
            packetized.insert(packetized.end(), wrapped.begin(), wrapped.end());
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
