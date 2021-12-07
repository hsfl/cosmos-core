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
            if (packed.size() <= 0) {
                return false;
            }
            type = (TypeId)packed[0];

            // Unpack as forwarding-type packet instead
//            if (type == TypeId["Forward"]) {
            if (type == PacketComm::TypeId::Forward)
            {
                if (UnpackForward()) {
                    return true;
                }
                return false;
            }

            // Unpack as a regular packet
            size_t size = packed[1] + 256 * packed[2];
            if (packed.size() < size + 5)
            {
                return false;
            }
            data.clear();
            data.insert(data.begin(), &packed[3], &packed[size+3]);
            uint16_t crcin = packed[size+3] + 256 * packed[size+4];
            crc = calc_crc.calc(packed.data(), packed.size()-2);
            if (checkcrc && crc != crcin)
            {
                return false;
            }

            return true;
        }

        bool PacketComm::UnpackForward() {
            fdest.clear();
            type = (TypeId)packed[0];
            size_t size = packed[1] + 256 * packed[2];
            if (packed.size() < size + 5)
            {
                return false;
            }
            // Extract node:agent address string
            uint8_t addr_len = packed[3];
            fdest.assign(&packed[4], &packed[4+addr_len]);
            // Extract inner data
            data.clear();
            data.insert(data.begin(), &packed[4+addr_len], &packed[size+3]);
            uint16_t crcin = packed[size+3] + 256 * packed[size+4];
            crc = calc_crc.calc(packed.data(), packed.size()-2);
            if (crc != crcin)
            {
                return false;
            }
            packed = data;
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

        bool PacketComm::RXSUnPacketize()
        {
            memcpy(&ccsds_header, packetized.data(), 6);
            packed.clear();
            packed.insert(packed.begin(), &packetized[6], &packetized[packetized.size()-(packetized.size()<189?6:194-packetized.size())]);
            return Unpack();
        }

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
            packed.resize(3);
            packed[0] = (uint8_t)type;
            packed[1] = data.size() & 0xff;
            packed[2] = data.size() >> 8;
            packed.insert(packed.end(), data.begin(), data.end());
            crc = calc_crc.calc(packed);
            packed.resize(packed.size()+2);
            packed[packed.size()-2] = crc & 0xff;
            packed[packed.size()-1] = crc >> 8;

            // Repack into Forwarding-type packet if necessary
            if (!fdest.empty() && !PackForward()) {
                return false;
            }
            return true;
        }

        bool PacketComm::PackForward()
        {
            data = packed;
            packed.resize(4);
            packed[0] = (uint8_t)PacketComm::TypeId::Forward; //TypeId["Forward"];
            // Data size = data size + addr length + 1 byte to specify addr length
            packed[1] = (data.size() + fdest.size() + 1) & 0xff;
            packed[2] = (data.size() + fdest.size() + 1) >> 8;
            // Insert addr length and string
            packed[3] = fdest.size();
            packed.insert(packed.end(), fdest.begin(), fdest.end());
            // Insert data
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
    }
}
