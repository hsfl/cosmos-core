#ifndef PACKETCOMM_H
#define PACKETCOMM_H

#include "support/configCosmos.h"
#include "math/mathlib.h"
#include "support/datalib.h"
#include "support/enumlib.h"

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
            bool RawUnPacketize(bool invert=false, bool checkcrc=true);
//            bool RXSUnPacketize();
            bool ASMUnPacketize();
//            bool TXSUnPacketize();
            bool SLIPUnPacketize();
            bool Pack();
            bool RawPacketize();
            bool ASMPacketize();
//            bool TXSPacketize();
            bool AX25Packetize(string dest_call="", string sour_call="", uint8_t dest_stat=0x60, uint8_t sour_stat=0x61, uint8_t cont=0x03, uint8_t prot=0xf0);
            bool SLIPPacketize();

            int32_t PushQueue(queue<PacketComm> &queue, mutex &mtx);
            int32_t PullQueue(queue<PacketComm> &queue, mutex &mtx);

            enum class TypeId : uint8_t
                {
                None = 0,
                Beacon = 10,
                Pong = 41,
                EPSResponse = 42,
                Response = 61,
                IP = 62,
                Test = 63,
                FileInfo = 70,
                FileCommand = 71,
                FileMessage = 72,
                FileHeartbeat = 73,
				FileQueue = 79,
                FileCancel = 80,
				FileComplete = 81,
				FileReqMeta = 82,
				FileReqData = 83,
                FileMetaData = 84,
                FileChunkData = 85,
                FileReqComplete = 86,
                Reset = 128,
                Reboot = 129,
                SendBeacon = 130,
                ClearRadioQueue = 131,
                ExternalCommand = 132,
                TestRadio = 133,
                ListDirectory = 134,
                TransferSingleFile = 135,
                TransferAllFiles = 136,
                StopTransfer = 137,
                Request = 140,
                Ping = 141,
                EPSCommand = 142,
                EPSSwitch = 143,
                };

            map<TypeId, string> TypeString = {
                {TypeId::Beacon, "Beacon"},
                {TypeId::Response, "Response"},
                {TypeId::IP, "IP"},
                {TypeId::Test, "Test"},
                {TypeId::FileInfo, "FileInfo"},
                {TypeId::FileCommand, "FileCommand"},
                {TypeId::FileMessage, "FileMessage"},
                {TypeId::FileHeartbeat, "FileHeartbeat"},
                {TypeId::FileChunkData, "FileChunkData"},
                {TypeId::FileMetaData, "FileMetaData"},
                {TypeId::Reset, "Reset"},
                {TypeId::Reboot, "Reboot"},
                {TypeId::SendBeacon, "SendBeacon"},
                {TypeId::ClearRadioQueue, "ClearRadioQueue"},
                {TypeId::ExternalCommand, "ExternalCommand"},
                {TypeId::TestRadio, "TestRadio"},
                {TypeId::ListDirectory, "ListDirectory"},
                {TypeId::TransferSingleFile, "TransferSingleFile"},
                {TypeId::TransferAllFiles, "TransferAllFiles"},
                {TypeId::StopTransfer, "StopTransfer"},
                {TypeId::Request, "Request"},
                {TypeId::Ping, "Ping"},
                {TypeId::EPSSwitch, "EPSSwitch"},
            };

            map<string, TypeId> StringType = {
                {"Beacon", TypeId::Beacon},
                {"Response", TypeId::Response},
                {"IP", TypeId::IP},
                {"Test", TypeId::Test},
                {"FileInfo", TypeId::FileInfo},
                {"FileCommand", TypeId::FileCommand},
                {"FileMessage", TypeId::FileMessage},
                {"FileHeartbeat", TypeId::FileHeartbeat},
                {"FileChunkData", TypeId::FileChunkData},
                {"FileMetaData", TypeId::FileMetaData},
                {"Reset", TypeId::Reset},
                {"Reboot", TypeId::Reboot},
                {"SendBeacon", TypeId::SendBeacon},
                {"ClearRadioQueue", TypeId::ClearRadioQueue},
                {"ExternalCommand", TypeId::ExternalCommand},
                {"TestRadio", TypeId::TestRadio},
                {"ListDirectory", TypeId::ListDirectory},
                {"TransferSingleFile", TypeId::TransferSingleFile},
                {"TransferAllFiles", TypeId::TransferAllFiles},
                {"StopTransfer", TypeId::StopTransfer},
                {"Request", TypeId::Request},
                {"Ping", TypeId::Ping},
                {"EPSSwitch", TypeId::EPSSwitch},
            };


            struct __attribute__ ((packed))  ResponseHeader
            {
                uint8_t chunks;
                uint8_t chunk_id;
                uint32_t response_id;
                uint32_t met;
            };

            struct __attribute__ ((packed))  TestHeader
            {
                uint32_t test_id;
                uint32_t size;
                uint32_t packet_id;
            };

            struct __attribute__ ((packed))  Header
            {
                uint16_t data_size;
                TypeId type;
                NodeData::NODE_ID_TYPE orig = NodeData::NODEIDORIG;
                NodeData::NODE_ID_TYPE dest = NodeData::NODEIDDEST;
            } header;

            CCSDS_Header ccsds_header;
            vector<uint8_t> packetized;
            vector<uint8_t> packed;
			/// Data of interest
            vector<uint8_t> data;
            uint16_t crc;

            struct __attribute__ ((packed)) FileChunkData
            {
                uint16_t size;
                uint32_t txid;
            };

            struct FileMeta
            {

            };

            vector<uint8_t> atsm = {0x1a, 0xcf, 0xfc, 0x1d};
            vector<uint8_t> atsmr = {0x58, 0xf3, 0x3f, 0xb8};

        private:
            CRC16 calc_crc;

//            Transfer ttransfer;
//            int32_t close_transfer();

        };
    }
}

#endif // PACKETCOMM_H
