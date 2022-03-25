#include "packetcomm.h"
#include "math/mathlib.h"
#include "support/sliplib.h"
#include "support/datalib.h"
#include "support/stringlib.h"
#include "device/general/ax25class.h"

namespace Cosmos {
    namespace Support {
        PacketComm::PacketComm()
        {
        }

        void PacketComm::CalcCRC()
        {
            crc = calc_crc.calc(data);
        }

        bool PacketComm::CheckCRC()
        {
            return true;
        }

        bool PacketComm::Unpack(bool checkcrc)
        {
            if (packed.size() <= 0)
            {
                return false;
            }
            memcpy(&header, &packed[0], COSMOS_SIZEOF(Header));
            if (packed.size() < uint32_t(header.data_size + COSMOS_SIZEOF(Header) + 2))
            {
                return false;
            }
            packed.resize(header.data_size + COSMOS_SIZEOF(Header) + 2);
            if (checkcrc)
            {
                uint16_t crcin = uint16from(&packed[header.data_size+COSMOS_SIZEOF(Header)], ByteOrder::LITTLEENDIAN);
                crc = calc_crc.calc(packed.data(), packed.size()-2);
                if (crc != crcin)
                {
                    return false;
                }
            }

            data.clear();
            data.insert(data.begin(), &packed[COSMOS_SIZEOF(Header)], &packed[header.data_size+COSMOS_SIZEOF(Header)]);

            return true;
        }

        bool PacketComm::RawUnPacketize(bool invert, bool checkcrc)
        {
            if (invert)
            {
                uint8from(packetized, packed, ByteOrder::BIGENDIAN);
            }
            else
            {
                packed = packetized;
            }
            return Unpack(checkcrc);
        }

//        bool PacketComm::RXSUnPacketize()
//        {
//            memcpy(&ccsds_header, packetized.data(), 6);
//            packed.clear();
//            packed.insert(packed.begin(), &packetized[6], &packetized[packetized.size()-(packetized.size()<189?6:194-packetized.size())]);
//            return Unpack();
//        }

        bool PacketComm::SLIPUnPacketize()
        {
            slip_unpack(packetized, packed);
            return Unpack();
        }

        bool PacketComm::ASMUnPacketize()
        {
            packed.clear();
            if (atsm[0] == packetized[0] && atsm[1] == packetized[1] && atsm[2] == packetized[2] && atsm[3] == packetized[3])
            {
                packed.insert(packed.begin(), &packetized[4], &packetized[packetized.size()]);
            }
            else if (atsmr[0] == packetized[0] && atsmr[1] == packetized[1] && atsmr[2] == packetized[2] && atsmr[3] == packetized[3])
            {
                vector<uint8_t> input;
                input.insert(packed.begin(), &packetized[4], &packetized[packetized.size()]);
                uint8from(input, packed, ByteOrder::BIGENDIAN);
            }
            return Unpack();
        }

        bool PacketComm::Pack()
        {
            header.data_size = data.size();
            packed.resize(COSMOS_SIZEOF(Header));
            memcpy(&packed[0], &header, COSMOS_SIZEOF(Header));
            packed.insert(packed.end(), data.begin(), data.end());
            crc = calc_crc.calc(packed);
            packed.resize(packed.size()+2);
            packed[packed.size()-2] = crc & 0xff;
            packed[packed.size()-1] = crc >> 8;

            return true;
        }

        bool PacketComm::RawPacketize()
        {
            if (!Pack())
            {
                return false;
            }
            packetized.clear();
            packetized.insert(packetized.begin(), packed.begin(), packed.end());
            return true;
        }

//        bool PacketComm::TXSPacketize()
//        {
//            if (!Pack())
//            {
//                return false;
//            }
//            packetized.clear();
//            packetized.insert(packetized.begin(), satsm.begin(), satsm.end());
//            packetized.insert(packetized.begin(), packed.begin(), packed.end());
//            return true;
//        }

        bool PacketComm::SLIPPacketize()
        {
            if (!Pack())
            {
                return false;
            }
            if (slip_pack(packed, packetized) < 0)
            {
                return false;
            }
            return true;
        }

        bool PacketComm::ASMPacketize()
        {
            if (!Pack())
            {
                return false;
            }
            packetized.clear();
            packetized.insert(packetized.begin(), atsm.begin(), atsm.end());
            packetized.insert(packetized.end(), packed.begin(), packed.end());
            return true;
        }

        bool PacketComm::AX25Packetize(string dest_call, string sour_call, uint8_t dest_stat, uint8_t sour_stat, uint8_t cont, uint8_t prot)
        {
            if (!Pack())
            {
                return false;
            }
            Ax25Handle axhandle(dest_call, sour_call, dest_stat, sour_stat, cont, prot);
            packed.resize(packed.size()+6);
            axhandle.load(packed);
            axhandle.stuff();
            vector<uint8_t> ax25packet = axhandle.get_hdlc_packet();
            packetized.clear();
            packetized.insert(packetized.begin(), ax25packet.begin(), ax25packet.end());
            return true;
        }

        // Thread-safe way of pushing onto the packet queue
        int32_t PacketComm::PushQueue(queue<PacketComm> &queue, mutex &mtx)
        {
            std::lock_guard<mutex> lock(mtx);
            queue.push(*this);
            return 1;
        }
        // Thread-safe way of pulling from the packet queue
        int32_t PacketComm::PullQueue(queue<PacketComm> &queue, mutex &mtx)
        {
            std::lock_guard<mutex> lock(mtx);
            if (queue.size())
            {
                *this = queue.front();
                queue.pop();
                return 1;
            }
            else
            {
                return 0;
            }
        }

    }
}
