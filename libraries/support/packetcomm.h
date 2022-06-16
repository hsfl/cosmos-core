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
            bool Unwrap(bool checkcrc=true);
            bool RawUnPacketize(bool invert=false, bool checkcrc=true);
            bool ASMUnPacketize();
            bool SLIPUnPacketize();
            bool Wrap();
            bool RawPacketize();
            bool ASMPacketize();
            bool AX25Packetize(string dest_call="", string sour_call="", uint8_t dest_stat=0x60, uint8_t sour_stat=0x61, uint8_t cont=0x03, uint8_t prot=0xf0);
            bool SLIPPacketize();
//            void SetSecret(uint32_t secretnumber);

            int32_t PushQueue(queue<PacketComm> &queue, mutex &mtx);
            int32_t PullQueue(queue<PacketComm> &queue, mutex &mtx);

            enum class TypeId : uint8_t {
                None = 0,
                DataBeacon = 10,
                DataPong = 41,
                DataEPSSingleResponse = 42,
                DataEPSMultiResponse = 43,
                DataADCSSingleResponse = 44,
                DataADCSMultiResponse = 45,
                DataResponse = 61,
                DataIP = 62,
                DataTest = 63,
                DataTime = 64,
                DataFileInfo = 70,
                DataFileCommand = 71,
                DataFileMessage = 72,
                DataFileHeartbeat = 73,
                DataFileQueue = 79,
                DataFileCancel = 80,
                DataFileComplete = 81,
                DataFileReqMeta = 82,
                DataFileReqData = 83,
                DataFileMetaData = 84,
                DataFileChunkData = 85,
                DataFileReqComplete = 86,
                CommandReset = 128,
                CommandReboot = 129,
                CommandSendBeacon = 130,
                CommandClearQueue = 131,
                CommandExternalCommand = 132,
                CommandTestRadio = 133,
                CommandListDirectory = 134,
                CommandTransferFile = 135,
                CommandTransferNode = 136,
                CommandTransferRadio = 137,
                CommandTransferList = 138,
                CommandInternalRequest = 140,
                CommandPing = 141,
                CommandSetTime = 142,
                CommandGetTimeHuman = 143,
                CommandGetTimeBinary = 144,
                CommandAdcsCommunicate = 150,
                CommandAdcsCommission = 151,
                CommandAdcsSetRunMode = 152,
                CommandEpsCommunicate = 160,
                CommandEpsSwitchName = 161,
                CommandEpsSwitchNumber = 162,
                CommandEpsReset = 163,
                CommandEpsState = 164,
                CommandEpsWatchdog = 165,
                CommandEpsSetTime = 166,
                CommandExecLoadCommand = 170,
                CommandExecAddCommand = 171,
                };

            map<TypeId, string> TypeString = {
                {TypeId::DataBeacon, "Beacon"},
                {TypeId::DataPong, "Pong"},
                {TypeId::DataEPSSingleResponse, "EPSSingleResponse"},
                {TypeId::DataEPSMultiResponse, "EPSMultiResponse"},
                {TypeId::DataADCSSingleResponse, "ADCSSingleResponse"},
                {TypeId::DataADCSMultiResponse, "ADCSMultiResponse"},
                {TypeId::DataResponse, "Response"},
                {TypeId::DataIP, "IP"},
                {TypeId::DataTest, "Test"},
                {TypeId::DataTime, "Time"},
                {TypeId::DataFileInfo, "FileInfo"},
                {TypeId::DataFileCommand, "FileCommand"},
                {TypeId::DataFileMessage, "FileMessage"},
                {TypeId::DataFileHeartbeat, "FileHeartbeat"},
                {TypeId::DataFileChunkData, "FileChunkData"},
                {TypeId::DataFileMetaData, "FileMetaData"},
                {TypeId::CommandReset, "Reset"},
                {TypeId::CommandReboot, "Reboot"},
                {TypeId::CommandSendBeacon, "SendBeacon"},
                {TypeId::CommandClearQueue, "ClearQueue"},
                {TypeId::CommandExternalCommand, "ExternalCommand"},
                {TypeId::CommandTestRadio, "TestRadio"},
                {TypeId::CommandListDirectory, "ListDirectory"},
                {TypeId::CommandTransferFile, "TransferFile"},
                {TypeId::CommandTransferNode, "TransferNode"},
                {TypeId::CommandTransferRadio, "TransferRadio"},
                {TypeId::CommandTransferList, "TransferList"},
                {TypeId::CommandInternalRequest, "InternalRequest"},
                {TypeId::CommandPing, "Ping"},
                {TypeId::CommandSetTime, "SetTime"},
                {TypeId::CommandGetTimeHuman, "GetTimeHuman"},
                {TypeId::CommandGetTimeBinary, "GetTimeBinary"},
                {TypeId::CommandAdcsCommunicate, "AdcsCommunicate"},
                {TypeId::CommandAdcsCommission, "AdcsCommision"},
                {TypeId::CommandAdcsSetRunMode, "AdcsSetRunMode"},
                {TypeId::CommandEpsCommunicate, "EpsCommunicate"},
                {TypeId::CommandEpsSwitchName, "EpsSwitchName"},
                {TypeId::CommandEpsSwitchNumber, "EpsSwitchNumber"},
                {TypeId::CommandEpsReset, "EpsReset"},
                {TypeId::CommandEpsState, "EpsState"},
                {TypeId::CommandEpsWatchdog, "EpsWatchdog"},
                {TypeId::CommandEpsSetTime, "EpsSetTime"},
                {TypeId::CommandExecLoadCommand, "ExecLoadCommand"},
                {TypeId::CommandExecAddCommand, "ExecAddCommand"},
            };

            map<string, TypeId> StringType = {
                {"Beacon", TypeId::DataBeacon},
                {"Pong", TypeId::DataPong},
                {"EPSSingleResponse", TypeId::DataEPSSingleResponse},
                {"EPSMultiResponse", TypeId::DataEPSMultiResponse},
                {"ADCSSingleResponse", TypeId::DataADCSSingleResponse},
                {"ADCSMultiResponse", TypeId::DataADCSMultiResponse},
                {"Response", TypeId::DataResponse},
                {"IP", TypeId::DataIP},
                {"Test", TypeId::DataTest},
                {"Time", TypeId::DataTime},
                {"FileInfo", TypeId::DataFileInfo},
                {"FileCommand", TypeId::DataFileCommand},
                {"FileMessage", TypeId::DataFileMessage},
                {"FileHeartbeat", TypeId::DataFileHeartbeat},
                {"FileChunkData", TypeId::DataFileChunkData},
                {"FileMetaData", TypeId::DataFileMetaData},
                {"Reset", TypeId::CommandReset},
                {"Reboot", TypeId::CommandReboot},
                {"SendBeacon", TypeId::CommandSendBeacon},
                {"ClearQueue", TypeId::CommandClearQueue},
                {"ExternalCommand", TypeId::CommandExternalCommand},
                {"TestRadio", TypeId::CommandTestRadio},
                {"ListDirectory", TypeId::CommandListDirectory},
                {"TransferFile", TypeId::CommandTransferFile},
                {"TransferNode", TypeId::CommandTransferNode},
                {"TransferRadio", TypeId::CommandTransferRadio},
                {"TransferList", TypeId::CommandTransferList},
                {"InternalRequest", TypeId::CommandInternalRequest},
                {"Ping", TypeId::CommandPing},
                {"SetTime", TypeId::CommandSetTime},
                {"GetTimeHuman", TypeId::CommandGetTimeHuman},
                {"GetTimeBinary", TypeId::CommandGetTimeBinary},
                {"EpsCommunicate", TypeId::CommandEpsCommunicate},
                {"EpsSwitchName", TypeId::CommandEpsSwitchName},
                {"EpsSwitchNumber", TypeId::CommandEpsSwitchNumber},
                {"EpsReset", TypeId::CommandEpsReset},
                {"EpsState", TypeId::CommandEpsState},
                {"EpsWatchdog", TypeId::CommandEpsWatchdog},
                {"EpsSetTime", TypeId::CommandEpsSetTime},
                {"AdcsCommunicate", TypeId::CommandAdcsCommunicate},
                {"AdcsCommision", TypeId::CommandAdcsCommission},
                {"AdcsSetRunMode", TypeId::CommandAdcsSetRunMode},
                {"ExecLoadCommand", TypeId::CommandExecLoadCommand},
                {"ExecAddCommand", TypeId::CommandExecAddCommand},
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
                uint8_t radio;
                NodeData::NODE_ID_TYPE orig = NodeData::NODEIDORIG;
                NodeData::NODE_ID_TYPE dest = NodeData::NODEIDDEST;
            } header;

            CCSDS_Header ccsds_header;
            vector<uint8_t> packetized;
            vector<uint8_t> wrapped;
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
//            uint32_t secret;

        private:
            CRC16 calc_crc;

//            Transfer ttransfer;
//            int32_t close_transfer();

        };
    }
}

#endif // PACKETCOMM_H
