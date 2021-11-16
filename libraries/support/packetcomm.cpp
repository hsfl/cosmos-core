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
//            TypeId.Init({"ShortReset", "ShortReboot","ShortSendBeacon"}, {0});
//            TypeId.Extend({"LongReset","LongReboot","LongSendBeacon"}, {10});
//            TypeId.Extend({"FileMeta","FileChunk"}, {20});

//            TypeId.Extend({"ShortCPUBeacon","ShortTempBeacon"}, {30});
//            TypeId.Extend({"LongCPUBeacon","LongTempBeacon"}, {40});
//            TypeId.Extend({"Forward", "Response"}, {60});
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
            if (datain.size() <= 0) {
                return false;
            }
            type = datain[0];

            // Unpack as forwarding-type packet instead
//            if (type == TypeId["Forward"]) {
            if (type == (uint8_t)PacketComm::TypeId::Forward)
            {
                if (UnpackForward()) {
                    return true;
                }
                return false;
            }

            // Unpack as a regular packet
            size_t size = datain[1] + 256 * datain[2];
            if (datain.size() < size + 5)
            {
                return false;
            }
            data.clear();
            data.insert(data.begin(), &datain[3], &datain[size+3]);
            uint16_t crcin = datain[size+3] + 256 * datain[size+4];
            crc = calc_crc.calc(datain.data(), datain.size()-2);
            if (checkcrc && crc != crcin)
            {
                return false;
            }

            return true;
        }

        bool PacketComm::UnpackForward() {
            fdest.clear();
            type = datain[0];
            size_t size = datain[1] + 256 * datain[2];
            if (datain.size() < size + 5)
            {
                return false;
            }
            // Extract node:agent address string
            uint8_t addr_len = datain[3];
            fdest.assign(&datain[4], &datain[4+addr_len]);
            // Extract inner data
            data.clear();
            data.insert(data.begin(), &datain[4+addr_len], &datain[size+3]);
            uint16_t crcin = datain[size+3] + 256 * datain[size+4];
            crc = calc_crc.calc(datain.data(), datain.size()-2);
            if (crc != crcin)
            {
                return false;
            }
            datain = data;
            return true;
        }

        bool PacketComm::RawIn(bool invert, bool checkcrc)
        {
            if (invert)
            {
                uint8from(dataout, datain, ByteOrder::BIGENDIAN);
            }
            else
            {
                datain = dataout;
            }
            return Unpack(checkcrc);
        }

        bool PacketComm::CCSDSIn()
        {
            memcpy(&ccsds_header, dataout.data(), 6);
            datain.clear();
            datain.insert(datain.begin(), &dataout[6], &dataout[dataout.size()-(dataout.size()<189?6:194-dataout.size())]);
            return Unpack();
        }

        bool PacketComm::SLIPIn()
        {
            slip_unpack(dataout, datain);
            return Unpack();
        }

        bool PacketComm::ASMIn()
        {
            datain.clear();
            if (atsm[0] == dataout[0] && atsm[1] == dataout[1] && atsm[2] == dataout[2] && atsm[3] == dataout[3])
            {
                datain.insert(datain.begin(), &dataout[4], &dataout[dataout.size()]);
            }
            else if (atsmr[0] == dataout[0] && atsmr[1] == dataout[1] && atsmr[2] == dataout[2] && atsmr[3] == dataout[3])
            {
                vector<uint8_t> input;
                input.insert(datain.begin(), &dataout[4], &dataout[dataout.size()]);
                uint8from(input, datain, ByteOrder::BIGENDIAN);
            }
            return Unpack();
        }

        bool PacketComm::Pack()
        {
            datain.resize(3);
            datain[0] = type;
            datain[1] = data.size() & 0xff;
            datain[2] = data.size() >> 8;
            datain.insert(datain.end(), data.begin(), data.end());
            crc = calc_crc.calc(datain);
            datain.resize(datain.size()+2);
            datain[datain.size()-2] = crc & 0xff;
            datain[datain.size()-1] = crc >> 8;

            // Repack into Forwarding-type packet if necessary
            if (!fdest.empty() && !PackForward()) {
                return false;
            }
            return true;
        }

        bool PacketComm::PackForward()
        {
            data = datain;
            datain.resize(4);
            datain[0] = (uint8_t)PacketComm::TypeId::Forward; //TypeId["Forward"];
            // Data size = data size + addr length + 1 byte to specify addr length
            datain[1] = (data.size() + fdest.size() + 1) & 0xff;
            datain[2] = (data.size() + fdest.size() + 1) >> 8;
            // Insert addr length and string
            datain[3] = fdest.size();
            datain.insert(datain.end(), fdest.begin(), fdest.end());
            // Insert data
            datain.insert(datain.end(), data.begin(), data.end());
            crc = calc_crc.calc(datain);
            datain.resize(datain.size()+2);
            datain[datain.size()-2] = crc & 0xff;
            datain[datain.size()-1] = crc >> 8;
            return true;
        }

        bool PacketComm::RawOut()
        {
            if (!Pack())
            {
                return false;
            }
            dataout.clear();
            dataout.insert(dataout.begin(), datain.begin(), datain.end());
            return true;
        }

        bool PacketComm::SLIPOut()
        {
            if (!Pack())
            {
                return false;
            }
            if (slip_pack(datain, dataout) < 0)
            {
                return false;
            }
            return true;
        }

        bool PacketComm::ASMOut()
        {
            if (!Pack())
            {
                return false;
            }
            dataout.clear();
            dataout.insert(dataout.begin(), atsm.begin(), atsm.end());
            dataout.insert(dataout.end(), datain.begin(), datain.end());
            return true;
        }

        bool PacketComm::AX25Out(string dest_call, string sour_call, uint8_t dest_stat, uint8_t sour_stat, uint8_t cont, uint8_t prot)
        {
            if (!Pack())
            {
                return false;
            }
            Ax25Handle axhandle(dest_call, sour_call, dest_stat, sour_stat, cont, prot);
            datain.resize(datain.size()+6);
            axhandle.load(datain);
            axhandle.stuff();
            vector<uint8_t> ax25packet = axhandle.get_hdlc_packet();
            dataout.clear();
            dataout.insert(dataout.begin(), ax25packet.begin(), ax25packet.end());
            return true;
        }

        /*int32_t PacketComm::close_transfer()
        {
            int32_t missing = 0;
            FILE *ofp;
            if (ttransfer.name.size())
            {
                ofp = fopen(ttransfer.name.c_str(), "w");
                if (ofp == nullptr)
                {
                    return -errno;
                }
//                printf("CLosing File: %u %s %lu bytes\n", ttransfer.txid, ttransfer.name.c_str(), ttransfer.size);
            }
            else
            {
                ofp = fopen(("txid_"+to_unsigned(ttransfer.txid)).c_str(), "w");
                if (ofp == nullptr)
                {
                    return -errno;
                }
//                printf("Missing Meta: %u\n", ttransfer.txid);
            }
            for (uint16_t idx=0; idx<ttransfer.data.size(); ++idx)
            {
                if (ttransfer.data[idx].size())
                {
                    if (fseek(ofp,idx*ttransfer.chunk_size, SEEK_SET) < 0)
                    {
                        missing = -errno;
                        fclose(ofp);
                        return missing;
                    }
                    if (ttransfer.size - idx*ttransfer.chunk_size < ttransfer.chunk_size)
                    {
                        if (fwrite(ttransfer.data[idx].data(), ttransfer.size - idx*ttransfer.chunk_size, 1, ofp) < 1)
                        {
                            missing = -errno;
                            fclose(ofp);
                            return missing;
                        }
                    }
                    else
                    {
                        if (fwrite(ttransfer.data[idx].data(), ttransfer.data[idx].size(), 1, ofp) < 1)
                        {
                            missing = -errno;
                            fclose(ofp);
                            return missing;
                        }
                    }
                }
                else
                {
//                    printf("Missing Chunk: %u %u\n", ttransfer.txid, idx);
                    ++missing;
                }
            }
            if (fclose(ofp) != 0)
            {
                return -errno;
            }
            ttransfer.txid = 0;
            ttransfer.name.clear();
            ttransfer.meta.clear();
            ttransfer.data.clear();
            ttransfer.json.clear();
            return missing;
        }*/

    }
}
