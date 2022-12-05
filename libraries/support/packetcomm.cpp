#include "packetcomm.h"
namespace Cosmos {
    namespace Support {
        //PacketComm::PacketComm()
        //{
        //}

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
            if (wrapped.size() <= 0)
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            memcpy(&header, &wrapped[0], COSMOS_SIZEOF(Header));
            uint32_t wrapsize = header.data_size + COSMOS_SIZEOF(Header) + 2;
            if (wrapped.size() < wrapsize)
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            wrapped.resize(wrapsize);
            if (checkcrc)
            {
                uint16_t crcin = uint16from(&wrapped[header.data_size+COSMOS_SIZEOF(Header)], ByteOrder::LITTLEENDIAN);
                crc = calc_crc.calc(wrapped.data(), wrapped.size()-2);
                if (crc != crcin)
                {
                    return GENERAL_ERROR_CRC;
                }
            }

            data.clear();
            data.insert(data.begin(), &wrapped[COSMOS_SIZEOF(Header)], &wrapped[header.data_size+COSMOS_SIZEOF(Header)]);

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

        bool PacketComm::SLIPUnPacketize()
        {
            int32_t iretn = slip_unpack(packetized, wrapped);
            if (iretn <= 0)
            {
                return false;
            }
            return Unwrap();
        }

        bool PacketComm::ASMUnPacketize()
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
            return Unwrap();
        }

        bool PacketComm::Wrap()
        {
            header.data_size = data.size();
            wrapped.resize(COSMOS_SIZEOF(Header));
            memcpy(&wrapped[0], &header, COSMOS_SIZEOF(Header));
            wrapped.insert(wrapped.end(), data.begin(), data.end());
            crc = calc_crc.calc(wrapped);
            wrapped.resize(wrapped.size()+2);
            wrapped[wrapped.size()-2] = crc & 0xff;
            wrapped[wrapped.size()-1] = crc >> 8;

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

//        bool PacketComm::TXSPacketize()
//        {
//            if (!Wrap())
//            {
//                return false;
//            }
//            packetized.clear();
//            packetized.insert(packetized.begin(), satsm.begin(), satsm.end());
//            packetized.insert(packetized.begin(), wrapped.begin(), wrapped.end());
//            return true;
//        }

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

        bool PacketComm::AX25Packetize(string dest_call, string sour_call, uint8_t dest_stat, uint8_t sour_stat, uint8_t cont, uint8_t prot)
        {
            if (!Wrap())
            {
                return false;
            }
            Ax25Handle axhandle(dest_call, sour_call, dest_stat, sour_stat, cont, prot);
//            wrapped.resize(wrapped.size()+6);
            axhandle.load(wrapped);
            axhandle.stuff();
            vector<uint8_t> ax25packet = axhandle.get_hdlc_packet();
            packetized.clear();
            packetized.insert(packetized.begin(), ax25packet.begin(), ax25packet.end());
            return true;
        }

//        void PacketComm::SetSecret(uint32_t secretnumber)
//        {
//            secret = secretnumber;
//            return;
//        }
    }
}
