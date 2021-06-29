#ifndef PACKETCOMM_H
#define PACKETCOMM_H

#include "support/configCosmos.h"

namespace Cosmos {
    namespace Support {
        class PacketComm
        {
        public:
            PacketComm();
            void CalcCRC();
            bool CheckCRC();
            bool PacketIn();
            bool PacketOut();

            vector<uint8_t> slipdata;
            vector<uint8_t> rawdata;
            uint8_t type;
            vector<uint8_t> data;
            uint16_t crc;

            struct __attribute__ ((packed)) FileData
            {
                uint16_t size;
                uint32_t txid;
            };

            struct FileMeta
            {

            };

        private:
        };
    }
}

#endif // PACKETCOMM_H
