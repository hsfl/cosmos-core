#ifndef PACKETCOMM_H
#define PACKETCOMM_H

#include "support/configCosmos.h"

namespace Cosmos {
    namespace Support {
        class PacketComm
        {
        public:
            PacketComm();
            CalcCRC();
            bool CheckCRC();
            bool PacketIn();
            bool PacketOut();

            vector<uint8_t> rawdata;
            uint8_t type;
            vector<uint8_t> data;
            uint16_t crc;

        private:
        };
    }
}

#endif // PACKETCOMM_H
