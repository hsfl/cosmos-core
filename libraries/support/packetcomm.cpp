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
            crc = calc_crc16ccitt_lsb(data);
        }

        bool PacketComm::CheckCRC()
        {

        }

        bool PacketComm::PacketIn()
        {
            slip_unpack(slipdata, rawdata);
            type = rawdata[0];
            uint16_t size = rawdata[1] + 256 * rawdata[2];
            if (rawdata.size() < size + 5)
            {
                return false;
            }
            uint16_t crcin = rawdata[size+3] + 256 * rawdata[size+4];
            data.resize(size);
            memcpy(data.data(), &rawdata[3], size);
            crc = calc_crc16ccitt_lsb(data);
            if (crc != crcin)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        bool PacketComm::PacketOut()
        {
            uint16_t size = data.size();
            rawdata.resize(size+5);
            rawdata[0] = type;
            rawdata[1] = size & 0xff;
            rawdata[2] = size >> 8;
            memcpy(&rawdata[3], data.data(), size);
            rawdata[size+3] = crc & 0xff;
            rawdata[size+4] = crc >> 8;
            slip_pack(rawdata, slipdata);
            return true;
        }
    }
}
