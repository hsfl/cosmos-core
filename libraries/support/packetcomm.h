#ifndef PACKETCOMM_H
#define PACKETCOMM_H

#include "support/configCosmos.h"
#include "math/mathlib.h"

namespace Cosmos {
    namespace Support {
        class PacketComm
        {
        public:
            PacketComm();
            void CalcCRC();
            bool CheckCRC();
            bool PacketIn();
            bool PacketInRaw(bool invert=false);
            bool PacketInASM();
            bool PacketInSLIP();
            bool PacketOut();
            bool PacketOutRaw();
            bool PacketOutASM();
            bool PacketOutSLIP();

            vector<uint8_t> dataout;
            vector<uint8_t> datain;
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
            vector<uint8_t> atsm = {0x1a, 0xcf, 0xfc, 0x1d};
            vector<uint8_t> atsmr = {0x58, 0xf3, 0x3f, 0xb8};
            CRC16 calc_crc;
        };
    }
}

#endif // PACKETCOMM_H
