#ifndef PACKETCOMM_H
#define PACKETCOMM_H

#include "support/configCosmos.h"
#include "math/mathlib.h"
#include "support/datalib.h"
#include "support/sliplib.h"
#include "device/general/ax25class.h"

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

            //PacketComm();
            void CalcCRC();
            bool CheckCRC();
            int32_t Unwrap(bool checkcrc=true);
            int32_t RawUnPacketize(bool invert=false, bool checkcrc=true);
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
                DataNop = 15,
                DataPong = 41,
                DataEpsResponse = 43,
                DataRadioResponse = 44,
                DataAdcsResponse = 45,
                DataResponse = 61,
                DataIP = 62,
                DataTest = 63,
                DataTime = 64,
                DataFileCommand = 71,
                DataFileMessage = 72,
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
                CommandExternalTask = 139,
                CommandInternalRequest = 140,
                CommandPing = 141,
                CommandSetTime = 142,
                CommandGetTimeHuman = 143,
                CommandGetTimeBinary = 144,
                CommandAdcsCommunicate = 150,
                CommandAdcsState = 151,
                CommandAdcsSetRunMode = 152,
				CommandAdcsGetAdcsState = 155,
                CommandEpsCommunicate = 160,
                CommandEpsSwitchName = 161,
                CommandEpsSwitchNumber = 162,
                CommandEpsReset = 163,
                CommandEpsState = 164,
                CommandEpsWatchdog = 165,
                CommandEpsSetTime = 166,
                CommandEpsMinimumPower = 167,
                CommandEpsSwitchNames = 168,
                CommandEpsSwitchStatus = 169,
                CommandExecLoadCommand = 170,
                CommandExecAddCommand = 171,
                };

            map<TypeId, string> TypeString = {
                {TypeId::DataBeacon, "Beacon"},
                {TypeId::DataNop, "Nop"},
                {TypeId::DataPong, "Pong"},
                {TypeId::DataEpsResponse, "EpsResponse"},
                {TypeId::DataRadioResponse, "RadioResponse"},
                {TypeId::DataAdcsResponse, "AdcsResponse"},
                {TypeId::DataResponse, "Response"},
                {TypeId::DataIP, "IP"},
                {TypeId::DataTest, "Test"},
                {TypeId::DataTime, "Time"},
                {TypeId::DataFileCommand, "FileCommand"},
                {TypeId::DataFileMessage, "FileMessage"},
                {TypeId::DataFileQueue, "FileQueue"},
                {TypeId::DataFileCancel, "FileCancel"},
                {TypeId::DataFileComplete, "FileComplete"},
                {TypeId::DataFileReqMeta, "FileReqMeta"},
                {TypeId::DataFileReqData, "FileReqData"},
                {TypeId::DataFileMetaData, "FileMetaData"},
                {TypeId::DataFileChunkData, "FileChunkData"},
                {TypeId::DataFileReqComplete, "FileReqComplete"},
                {TypeId::CommandReset, "Reset"},
                {TypeId::CommandReboot, "Reboot"},
                {TypeId::CommandSendBeacon, "SendBeacon"},
                {TypeId::CommandClearQueue, "ClearQueue"},
                {TypeId::CommandExternalCommand, "ExternalCommand"},
                {TypeId::CommandExternalTask, "ExternalTask"},
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
                {TypeId::CommandAdcsState, "AdcsState"},
                {TypeId::CommandAdcsSetRunMode, "AdcsSetRunMode"},
                {TypeId::CommandAdcsGetAdcsState, "AdcsGetAdcsState"},
                {TypeId::CommandEpsCommunicate, "EpsCommunicate"},
                {TypeId::CommandEpsSwitchName, "EpsSwitchName"},
                {TypeId::CommandEpsSwitchNumber, "EpsSwitchNumber"},
                {TypeId::CommandEpsReset, "EpsReset"},
                {TypeId::CommandEpsState, "EpsState"},
                {TypeId::CommandEpsWatchdog, "EpsWatchdog"},
                {TypeId::CommandEpsSetTime, "EpsSetTime"},
                {TypeId::CommandEpsMinimumPower, "EpsMinimumPower"},
                {TypeId::CommandEpsSwitchNames, "EpsSwitchNames"},
                {TypeId::CommandEpsSwitchStatus, "EpsSwitchStatus"},
                {TypeId::CommandExecLoadCommand, "ExecLoadCommand"},
                {TypeId::CommandExecAddCommand, "ExecAddCommand"},
            };

            map<string, TypeId> StringType = {
                {"Beacon", TypeId::DataBeacon},
                {"Nop", TypeId::DataNop},
                {"Pong", TypeId::DataPong},
                {"EpsResponse", TypeId::DataEpsResponse},
                {"RadioResponse", TypeId::DataRadioResponse},
                {"AdcsResponse", TypeId::DataAdcsResponse},
                {"Response", TypeId::DataResponse},
                {"IP", TypeId::DataIP},
                {"Test", TypeId::DataTest},
                {"Time", TypeId::DataTime},

                {"FileCommand", TypeId::DataFileCommand},
                {"FileMessage", TypeId::DataFileMessage},
                {"FileQueue", TypeId::DataFileQueue},
                {"FileCancel", TypeId::DataFileCancel},
                {"FileComplete", TypeId::DataFileComplete},
                {"FileReqMeta", TypeId::DataFileReqMeta},
                {"FileReqData", TypeId::DataFileReqData},
                {"FileMetaData", TypeId::DataFileMetaData},
                {"FileChunkData", TypeId::DataFileChunkData},
                {"FileReqComplete", TypeId::DataFileReqComplete},

                {"Reset", TypeId::CommandReset},
                {"Reboot", TypeId::CommandReboot},
                {"SendBeacon", TypeId::CommandSendBeacon},
                {"ClearQueue", TypeId::CommandClearQueue},
                {"ExternalCommand", TypeId::CommandExternalCommand},
                {"ExternalTask", TypeId::CommandExternalTask},
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
                {"EpsMinimumPower", TypeId::CommandEpsMinimumPower},
                {"EpsSwitchNames", TypeId::CommandEpsSwitchNames},
                {"EpsSwitchStatus", TypeId::CommandEpsSwitchStatus},
                {"AdcsCommunicate", TypeId::CommandAdcsCommunicate},
                {"AdcsState", TypeId::CommandAdcsState},
                {"AdcsSetRunMode", TypeId::CommandAdcsSetRunMode},
                {"AdcsGetAdcsState", TypeId::CommandAdcsGetAdcsState},
                {"ExecLoadCommand", TypeId::CommandExecLoadCommand},
                {"ExecAddCommand", TypeId::CommandExecAddCommand},
            };

            struct __attribute__ ((packed)) CommunicateHeader
            {
                uint8_t unit;
                uint8_t command;
                uint16_t responsecount;
            };

            struct __attribute__ ((packed))  CommunicateResponseHeader
            {
                uint32_t deci;
                uint8_t chunks;
                uint8_t chunk_id;
                uint8_t unit;
                uint8_t command;
            };

            // Structure is the same, but type aliases are provided for user convenience
            using AdcsResponseHeader = CommunicateResponseHeader;
            using EpsResponseHeader = CommunicateResponseHeader;
            using RadioResponseHeader = CommunicateResponseHeader;

            struct __attribute__ ((packed))  ResponseHeader
            {
                uint32_t deci;
                uint8_t chunks;
                uint8_t chunk_id;
                uint32_t response_id;
            };

            struct __attribute__ ((packed))  TestHeader
            {
                uint32_t test_id = 0;
                uint32_t size = 0;
                uint32_t packet_id = 0;
            };

            struct __attribute__ ((packed))  Header
            {
                uint16_t data_size = 0;
                TypeId type = TypeId::CommandPing;
                uint8_t radio = 0;
                NodeData::NODE_ID_TYPE orig = NodeData::NODEIDORIG;
                NodeData::NODE_ID_TYPE dest = NodeData::NODEIDDEST;
                // Maybe response_id here
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
