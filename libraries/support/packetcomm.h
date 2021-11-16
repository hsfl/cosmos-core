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
            struct __attribute__ ((packed)) CCSDS_Header {
                uint8_t tf_version:2;
                uint16_t spacecraft_id:10;
                uint8_t virtual_channel_id:3;
                uint8_t ocf_flag:1;
                uint8_t master_frame_cnt;
                uint8_t virtual_frame_cnt; // Start frame, will increment
                uint16_t tf_data_field_status;
            };

            PacketComm();
            void CalcCRC();
            bool CheckCRC();
            bool Unpack(bool checkcrc=true);
            bool UnpackForward();
            bool RawIn(bool invert=false, bool checkcrc=true);
            bool CCSDSIn();
            bool ASMIn();
            bool SLIPIn();
            bool Pack();
            bool RawOut();
            bool ASMOut();
            bool AX25Out(string dest_call="", string sour_call="", uint8_t dest_stat=0x60, uint8_t sour_stat=0x61, uint8_t cont=0x03, uint8_t prot=0xf0);
            bool SLIPOut();
//            int32_t Generate(string args="");

            static constexpr uint16_t INTERNAL_BEACON = 0;
            static constexpr uint16_t INTERNAL_COMMAND = 0;
            static constexpr uint16_t EXTERNAL_COMMAND = 14;

            enum class TypeId
                {
                Transfer = 20,
                FileMeta = 21,
                FileChunk = 22,
                ShortCPUBeacon1 = 30,
                ShortCPUBeacon2 = 31,
                ShortDuplexBeacon1 = 32,
                ShortDuplexBeacon2 = 33,
                ShortDuplexBeacon3 = 34,
                ShortDuplexBeacon4 = 35,
                ShortTempBeacon = 36,
                LongCPUBeacon = 50,
                LongTempBeacon = 51,
                LongEPSBeacon = 52,
                LongADCSBeacon = 53,
                LongSolarPanelsBeacon = 54,
                LongOBCBeacon = 55,
                LongRadioBeacon = 56,
                LongBinaryBeacon = 59,
                Forward = 60,
                Response = 61,
                IP = 62,
                Test = 63,
                FileNodeInfo = 70,
                Reset = 128,
                Reboot = 129,
                SendBeacon = 130,
                ClearRadioQueue = 131,
                ExternalCommand = 132,
                TestRadio = 133,
                };

            struct __attribute__ ((packed))  ResponseHeader
            {
                uint8_t chunks;
                uint8_t chunk_id;
                uint16_t chunk_size;
                uint16_t response_id;
                uint32_t met;
            };

            CCSDS_Header ccsds_header;
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

            //Transfer ttransfer;
            int32_t close_transfer();

            bool PackForward();

        };
    }
}

#endif // PACKETCOMM_H
