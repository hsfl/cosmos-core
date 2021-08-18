#ifndef PACKETCOMM_H
#define PACKETCOMM_H

#include "support/configCosmos.h"
#include "math/mathlib.h"
#include "support/enumlib.h"
#include "support/transferclass.h"

namespace Cosmos {
    namespace Support {
        class PacketComm
        {
        public:
            PacketComm();
            void CalcCRC();
            bool CheckCRC();
            bool Unpack();
            bool UnpackForward();
            bool RawIn(bool invert=false);
            bool ASMIn();
            bool SLIPIn();
            bool Pack();
            bool RawOut();
            bool ASMOut();
            bool SLIPOut();
            int32_t Process();
            int32_t Generate(string args="");

            static constexpr uint16_t INTERNAL_BEACON = 0;
            static constexpr uint16_t INTERNAL_COMMAND = 60;
            static constexpr uint16_t EXTERNAL_COMMAND = 128;

            Enum TypeId;
            typedef int32_t (*Func)(PacketComm *packet, string args);
            Func Funcs[256];

            vector<uint8_t> dataout;
            vector<uint8_t> datain;
            uint8_t type;
			/// Data of interest
            vector<uint8_t> data;
            uint16_t crc;
            // Destination for forward type packets
            string fdest;

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

            Transfer ttransfer;
            int32_t close_transfer();

            bool PackForward();


            static int32_t ShortReset(PacketComm *packet, string args="");
            static int32_t ShortReboot(PacketComm *packet, string args="");
            static int32_t ShortSendBeacon(PacketComm *packet, string args="");
            static int32_t LongReset(PacketComm *packet, string args="");
            static int32_t LongReboot(PacketComm *packet, string args="");
            static int32_t LongSendBeacon(PacketComm *packet, string args="");
            static int32_t FileMeta(PacketComm *packet, string args="");
            static int32_t FileChunk(PacketComm *packet, string args="");

            static int32_t ShortCPUBeacon(PacketComm *packet, string args="");
            static int32_t ShortTempBeacon(PacketComm *packet, string args="");
            static int32_t LongCPUBeacon(PacketComm *packet, string args="");
            static int32_t LongTempBeacon(PacketComm *packet, string args="");
        };
    }
}

#endif // PACKETCOMM_H
