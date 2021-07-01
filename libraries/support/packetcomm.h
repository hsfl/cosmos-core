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
            int32_t PacketIn();
            int32_t PacketOut();

			/// Slip encoded packet
            vector<uint8_t> slipdata;
			/// COSMOS protocol packet
            vector<uint8_t> rawdata;
			/// Packet type ID, refer to relevant documentation
            uint8_t type;
			/// Data of interest
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
