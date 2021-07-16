#include "packetcomm.h"
#include "math/mathlib.h"
#include "support/sliplib.h"

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

        }

        bool PacketComm::PacketIn()
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

        bool PacketComm::PacketInRaw(bool invert)
        {
            if (invert)
            {
                uint8from(dataout, datain, ByteOrder::BIGENDIAN);
            }
            else
            {
                datain = dataout;
            }
            return PacketIn();
        }

        bool PacketComm::PacketInSLIP()
        {
            slip_unpack(dataout, datain);
            return PacketIn();
        }

        bool PacketComm::PacketInASM()
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
            return PacketIn();
        }

        bool PacketComm::PacketOut()
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

        bool PacketComm::PacketOutRaw()
        {
            if (!PacketOut())
            {
                return false;
            }
            dataout.clear();
            dataout.insert(dataout.begin(), datain.begin(), datain.end());
            return true;
        }

        bool PacketComm::PacketOutSLIP()
        {
            if (!PacketOut())
            {
                return false;
            }
            if (slip_pack(datain, dataout) < 0)
            {
                return false;
            }
            return true;
        }

        bool PacketComm::PacketOutASM()
        {
            if (!PacketOut())
            {
                return false;
            }
            dataout.clear();
            dataout.insert(dataout.begin(), atsm.begin(), atsm.end());
            dataout.insert(dataout.end(), datain.begin(), datain.end());
            return true;
        }
    }
}
