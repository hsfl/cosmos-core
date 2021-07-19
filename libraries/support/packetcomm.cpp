#include "packetcomm.h"
#include "math/mathlib.h"
#include "support/sliplib.h"
#include "support/datalib.h"
#include "support/stringlib.h"

namespace Cosmos {
    namespace Support {
        PacketComm::PacketComm()
        {
            TypeId.Init({"ShortReset", "ShortReboot","ShortSendBeacon"}, {0});
            TypeId.Extend({"LongReset","LongReboot","LongSendBeacon"}, {10});
            TypeId.Extend({"FileMeta","FileChunk"}, {20});

            TypeId.Extend({"ShortCPUBeacon","ShortTempBeacon"}, {30});
            TypeId.Extend({"LongCPUBeacon","LongTempBeacon"}, {40});
        }

        void PacketComm::CalcCRC()
        {
            crc = calc_crc.calc(data);
        }

        bool PacketComm::CheckCRC()
        {

        }

        bool PacketComm::Unpack()
        {
            type = datain[0];
            uint16_t size = datain[1] + 256 * datain[2];
            if (datain.size() < size + 5)
            {
                return COSMOS_GENERAL_ERROR_UNDERSIZE;
            }
            data.clear();
            data.insert(data.begin(), &datain[3], &datain[size+3]);
            uint16_t crcin = datain[size+3] + 256 * datain[size+4];
            crc = calc_crc.calc(datain.data(), datain.size()-2);
//            if (crc != crcin)
//            {
//                return false;
//            }
            return true;
        }

        bool PacketComm::RawIn(bool invert)
        {
            if (invert)
            {
                uint8from(dataout, datain, ByteOrder::BIGENDIAN);
            }
            else
            {
                datain = dataout;
            }
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

        int32_t PacketComm::Process()
        {
            int32_t iretn;
            string args;
            if (Funcs[type] != nullptr)
            {
                iretn = (*(Funcs[type]))(this, args);
            }
        }

        int32_t PacketComm::Generate(string args)
        {
            int32_t iretn;
            if (Funcs[type] != nullptr)
            {
                iretn = (*(Funcs[type]))(this, args);
            }
        }

        int32_t PacketComm::ShortReset(PacketComm *packet, string args)
        {
            // Code to tell EPS to reset
            return 0;
        }

        int32_t PacketComm::ShortReboot(PacketComm *packet, string args)
        {
            int32_t iretn;
            string result;
            iretn = data_execute("/sbin/reboot -f", result);
            return iretn;
        }

        int32_t PacketComm::ShortSendBeacon(PacketComm *packet, string args)
        {
            vector<string>sargs = string_split(args, " ");
            packet->data[0] = stoi(sargs[0]);
            packet->data[1] = stoi(sargs[1]);
//            float value;
//            value = stof(sargs[0]);
//            memcpy(&packet->data[0], &value, 4);
            return 0;
        }

        int32_t PacketComm::LongReset(PacketComm *packet, string args)
        {
            return ShortReset(packet, args);
        }

        int32_t PacketComm::LongReboot(PacketComm *packet, string args)
        {
            return ShortReboot(packet, args);
        }

        int32_t PacketComm::LongSendBeacon(PacketComm *packet, string args)
        {
            return ShortSendBeacon(packet, args);
        }

        int32_t PacketComm::FileMeta(PacketComm *packet, string args)
        {
            uint32_t txid = uint32from(&packet->data[0], ByteOrder::LITTLEENDIAN);
            if (txid != packet->ttransfer.txid)
            {
                packet->close_transfer();
                packet->ttransfer.txid = txid;
                packet->ttransfer.chunk_size = packet->data.size() - 10;
            }
            uint32_t chunkidx = uint32from(&packet->data[4], ByteOrder::LITTLEENDIAN);
            if (chunkidx >= packet->ttransfer.data.size())
            {
                packet->ttransfer.data.resize(chunkidx+1);
            }
            packet->ttransfer.data[chunkidx].clear();
            packet->ttransfer.data[chunkidx].insert(packet->ttransfer.data[chunkidx].begin(), &packet->data[8], &packet->data[packet->data.size()-2]);
            return packet->ttransfer.txid;
        }

        int32_t PacketComm::FileChunk(PacketComm *packet, string args)
        {
            if (packet->ttransfer.data.size())
            {
                packet->close_transfer();
            }
            packet->ttransfer.json.assign(packet->data.begin(), packet->data.end());
            string estring;
            json11::Json jmeta = json11::Json::parse(packet->ttransfer.json.c_str(), estring);
            packet->ttransfer.name = jmeta["name"].string_value();
            packet->ttransfer.size = jmeta["size"].number_value();
            packet->ttransfer.node = jmeta["node"].string_value();
            packet->ttransfer.agent = jmeta["agent"].string_value();
            packet->ttransfer.txid = jmeta["txid"].number_value();
            packet->ttransfer.chunk_size = jmeta["chunksize"].number_value();
            packet->ttransfer.data.resize(1+((packet->ttransfer.size-1)/packet->ttransfer.chunk_size));
            return packet->ttransfer.txid;
        }

        int32_t PacketComm::close_transfer()
        {

            FILE *ofp;
            if (ttransfer.name.size())
            {
                ofp = fopen(ttransfer.name.c_str(), "w");
                printf("CLosing File: %u %s %u bytes\n", ttransfer.txid, ttransfer.name.c_str(), ttransfer.size);
            }
            else
            {
                ofp = fopen(("txid_"+to_unsigned(ttransfer.txid)).c_str(), "w");
                printf("Missing Meta: %u\n", ttransfer.txid);
            }
            for (uint16_t idx=0; idx<ttransfer.data.size(); ++idx)
            {
                if (ttransfer.data[idx].size())
                {
                    fseek(ofp,idx*ttransfer.chunk_size, SEEK_SET);
                    if (ttransfer.size - idx*ttransfer.chunk_size < ttransfer.chunk_size)
                    {
                        fwrite(ttransfer.data[idx].data(), ttransfer.size - idx*ttransfer.chunk_size, 1, ofp);
                    }
                    else
                    {
                        fwrite(ttransfer.data[idx].data(), ttransfer.data[idx].size(), 1, ofp);
                    }
                }
                else
                {
                    printf("Missing Chunk: %u %u\n", ttransfer.txid, idx);
                }
            }
            fclose(ofp);
            ttransfer.txid = 0;
            ttransfer.name.clear();
            ttransfer.meta.clear();
            ttransfer.data.clear();
            ttransfer.json.clear();
        }

    }
}
