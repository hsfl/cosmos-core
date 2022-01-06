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
            uint16_t crcin = uint16from(&packed[header.data_size+COSMOS_SIZEOF(Header)], ByteOrder::LITTLEENDIAN);
            crc = calc_crc.calc(packed.data(), packed.size()-2);
            if (checkcrc && crc != crcin)
            {
                return false;
            }

            // Unpack as forwarding-type packet if required
            fdest.clear();
            if (header.type == PacketComm::TypeId::Forward)
            {
                if (UnpackForward()) {
                    return true;
                }
                return false;
            }

            // Otherwise Unpack as a regular packet
            data.clear();
            data.insert(data.begin(), &packed[COSMOS_SIZEOF(Header)], &packed[header.data_size+COSMOS_SIZEOF(Header)]);

            return true;
        }

        bool PacketComm::UnpackForward()
        {
            // Extract node:agent address string
            fdest.clear();
            uint8_t addr_len = packed[COSMOS_SIZEOF(Header)];
            fdest.assign(&packed[COSMOS_SIZEOF(Header)+1], &packed[COSMOS_SIZEOF(Header)+1+addr_len]);
            // Extract inner data
            vector<uint8_t> oldpacked = packed;
            packed.clear();
            packed.insert(packed.begin(), &oldpacked[COSMOS_SIZEOF(Header)+1+addr_len], &oldpacked[packed.size()-2]);
            return Unpack();

//            memcpy(&header, &packed[COSMOS_SIZEOF(Header)+1+addr_len], COSMOS_SIZEOF(Header));
//            if (packed.size() < header.data_size + 5)
//            {
//                return false;
//            }
//            data.clear();
//            data.insert(data.begin(), &packed[8+addr_len], &packed[header.data_size+8+addr_len]);
//            uint16_t crcin = uint16from(&packed[header.data_size+COSMOS_SIZEOF(Header)+1+addr_len], ByteOrder::LITTLEENDIAN);
//            crc = calc_crc.calc(packed.data(), packed.size()-2);
//            if (crc != crcin)
//            {
//                return false;
//            }
//            packed = data;
//            return true;
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

//        bool PacketComm::TXSUnPacketize()
//        {
//            static uint16_t byte_slip = 0;
//            static vector<uint8_t> cpacketized;
//            static vector<uint8_t> ppacketized;
//            vector<uint8_t> npacketized;
//            if (atsm[0] == packetized[0] && atsm[1] == packetized[1] && atsm[2] == packetized[2] && atsm[3] == packetized[3])
//            {
//                if (packetized[5])
//                {
//                    //                    if ((packetized[5] & 0xf) == 14 || (packetized[5] & 0xf) == 2)
//                    {
//                        packed.clear();
//                        packed.insert(packed.begin(), &packetized[10], &packetized[packetized.size()]);
//                        cpacketized = packed;
//                        if (!byte_slip)
//                        {
//                            if (satsm[0] == cpacketized[4] && satsm[1] == cpacketized[5] && satsm[2] == cpacketized[6] && satsm[3] == cpacketized[7])
//                            {
//                                npacketized = cpacketized;
//                            }
//                            else if (satsm[0] == cpacketized[packed.size()-(byte_slip+8)] && satsm[1] == cpacketized[packed.size()-(byte_slip+7)] && satsm[2] == cpacketized[packed.size()-(byte_slip+6)] && satsm[3] == cpacketized[packed.size()-(byte_slip+5)])
//                                //                                if ((cpacketized[packed.size()-(byte_slip+8)] == 0 || cpacketized[packed.size()-(byte_slip+8)] == 0x3f) && cpacketized[(packed.size()-(byte_slip+8))+1] == 0xd4 && cpacketized[(packed.size()-(byte_slip+8))+2] == 0)
//                            {
//                                byte_slip += 8;
//                            }
//                        }
//                        else
//                        {
//                            if (satsm[0] == cpacketized[4] && satsm[1] == cpacketized[5] && satsm[2] == cpacketized[6] && satsm[3] == cpacketized[7])
//                            {
//                                byte_slip = 0;
//                                npacketized = cpacketized;
//                            }
//                            else if (satsm[0] == cpacketized[packed.size()-(byte_slip)] && satsm[1] == cpacketized[packed.size()-(byte_slip-1)] && satsm[2] == cpacketized[packed.size()-(byte_slip-2)] && satsm[3] == cpacketized[packed.size()-(byte_slip-3)])
//                            {
//                                npacketized.insert(npacketized.begin(), &ppacketized[packed.size()-byte_slip], &ppacketized[packed.size()]);
//                                npacketized.insert(npacketized.end(), &cpacketized[0], &cpacketized[packed.size()-byte_slip]);
//                            }
//                            else if (satsm[0] == cpacketized[packed.size()-(byte_slip+8)] && satsm[1] == cpacketized[packed.size()-(byte_slip+7)] && satsm[2] == cpacketized[packed.size()-(byte_slip+6)] && satsm[3] == cpacketized[packed.size()-(byte_slip+5)])
//                            {
//                                byte_slip += 8;
//                            }
//                        }
//                        packed = npacketized;
//                        ppacketized = cpacketized;
//                        return Unpack();
//                        //                        return Unpack(byte_slip==0);
//                    }
//                }
//            }
//            return false;
//        }

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

            // Repack into Forwarding-type packet if necessary
            if (!fdest.empty() && !PackForward()) {
                return false;
            }
            return true;
        }

        bool PacketComm::PackForward()
        {
            data = packed;
            header.data_size = data.size() + fdest.size() + 1;
            header.type = TypeId::Forward;
            packed.resize(COSMOS_SIZEOF(Header));
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
    }
}
