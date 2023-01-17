#ifndef PACKETCOMM_H
#define PACKETCOMM_H

#include "support/configCosmosKernel.h"
#include "math/crclib.h"
#include "support/sliplib.h"
#include "support/ax25class.h"
#include "support/timelib.h"

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

            PacketComm(uint16_t size=4);

//            enum class ShortTypeId : uint8_t
//                {
//                Blank = 0,
//                Type1,
//                Type2,
//                Type3,
//                Type4,
//                Type5,
//                Type6,
//                Type7,
//                Type8,
//                Type9,
//                Type10,
//                Type11,
//                Type12,
//                Type13,
//                Type14,
//                Type15,
//                };

//            std::map<ShortTypeId, string> ShortTypeString = {
//                {ShortTypeId::Type1, "Type1"},
//                {ShortTypeId::Type2, "Type2"},
//                {ShortTypeId::Type3, "Type3"},
//                {ShortTypeId::Type4, "Type4"},
//                {ShortTypeId::Type5, "Type5"},
//                {ShortTypeId::Type6, "Type6"},
//                {ShortTypeId::Type7, "Type7"},
//                {ShortTypeId::Type8, "Type8"},
//                {ShortTypeId::Type9, "Type9"},
//                {ShortTypeId::Type10, "Type10"},
//                {ShortTypeId::Type11, "Type11"},
//                {ShortTypeId::Type12, "Type12"},
//                {ShortTypeId::Type13, "Type13"},
//                {ShortTypeId::Type14, "Type14"},
//                {ShortTypeId::Type15, "Type15"},
//            };

//            std::map<string, ShortTypeId> StringShortType = {
//                {"Type1", ShortTypeId::Type1},
//                {"Type2", ShortTypeId::Type2},
//                {"Type3", ShortTypeId::Type3},
//                {"Type4", ShortTypeId::Type4},
//                {"Type5", ShortTypeId::Type5},
//                {"Type6", ShortTypeId::Type6},
//                {"Type7", ShortTypeId::Type7},
//                {"Type8", ShortTypeId::Type8},
//                {"Type9", ShortTypeId::Type9},
//                {"Type10", ShortTypeId::Type10},
//                {"Type11", ShortTypeId::Type11},
//                {"Type12", ShortTypeId::Type12},
//                {"Type13", ShortTypeId::Type13},
//                {"Type14", ShortTypeId::Type14},
//                {"Type15", ShortTypeId::Type15},
//            };

            enum class TypeIdV1 : uint8_t {
                Blank = 0,
                DataObcBeacon = 10,
                DataObcPong = 41,
                DataEpsResponse = 43,
                DataRadioResponse = 44,
                DataAdcsResponse = 45,
                DataObcResponse = 61,
                DataIP = 62,
                DataRadioTest = 63,
                DataObcTime = 64,
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
                DataObcNop = 0x7f,

                Command = 0x80,
                CommandObcReset = 128,
                CommandObcReboot = 129,
                CommandObcSendBeacon = 130,
                CommandExecClearQueue = 131,
                CommandObcExternalCommand = 132,
                CommandRadioTest = 133,
                CommandFileListDirectory = 134,
                CommandFileTransferFile = 135,
                CommandFileTransferNode = 136,
                CommandFileTransferRadio = 137,
                CommandFileTransferList = 138,
                CommandObcExternalTask = 139,
                CommandObcInternalRequest = 140,
                CommandObcPing = 141,
                CommandObcSetTime = 142,
                CommandObcGetTimeHuman = 143,
                CommandObcGetTimeBinary = 144,
                CommandExecSetOpsMode = 145,
                CommandExecEnableChannel = 146,
                CommandAdcsCommunicate = 150,
                CommandAdcsState = 151,
                CommandAdcsSetRunMode = 152,
                CommandAdcsGetAdcsState = 155,
                CommandAdcsOrbitParameters = 156,
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
                CommandRadioCommunicate = 180,
                CommandObcNop = 255,
            };

            enum class TypeId : uint16_t {
                Blank = 0,
//                DataObcBeacon = 10,
//                DataObcPong = 41,
//                DataEpsResponse = 43,
//                DataAdcsResponse = 45,
//                DataObcResponse = 61,
//                DataIP = 62,
//                DataRadioTest = 63,
//                DataObcTime = 64,
//                DataObcNop = 0x7f,

//                Command = 0x80,
//                CommandObcReset = 128,
//                CommandObcReboot = 129,
//                CommandObcSendBeacon = 130,
//                CommandExecClearQueue = 131,
//                CommandObcExternalCommand = 132,
//                CommandObcExternalTask = 139,
//                CommandObcInternalRequest = 140,
//                CommandObcPing = 141,
//                CommandObcSetTime = 142,
//                CommandObcGetTimeHuman = 143,
//                CommandObcGetTimeBinary = 144,
//                CommandObcNop = 255,

//                // 0x0100 NET COMMANDS

//                // 0x0200 EPS COMMANDS
//                CommandExecEnableChannel = 146,
//                CommandEpsCommunicate = 160,
//                CommandEpsSwitchName = 161,
//                CommandEpsSwitchNumber = 162,
//                CommandEpsReset = 163,
//                CommandEpsState = 164,
//                CommandEpsWatchdog = 165,
//                CommandEpsSetTime = 166,
//                CommandEpsMinimumPower = 167,
//                CommandEpsSwitchNames = 168,
//                CommandEpsSwitchStatus = 169,

//                // 0x0300 ADCS COMMANDS
//                CommandAdcsCommunicate = 150,
//                CommandAdcsState = 151,
//                CommandAdcsSetRunMode = 152,
//                CommandAdcsGetAdcsState = 155,
//                CommandAdcsOrbitParameters = 156,

//                // 0x0400 FILE COMMANDS
//                DataFileCommand = 71,
//                DataFileMessage = 72,
//                DataFileQueue = 79,
//                DataFileCancel = 80,
//                DataFileComplete = 81,
//                DataFileReqMeta = 82,
//                DataFileReqData = 83,
//                DataFileMetaData = 84,
//                DataFileChunkData = 85,
//                DataFileReqComplete = 86,
//                CommandFileListDirectory = 134,
//                CommandFileTransferFile = 135,
//                CommandFileTransferNode = 136,
//                CommandFileTransferRadio = 137,
//                CommandFileTransferList = 138,

//                // 0x0500 EXEC COMMANDS
//                CommandExecSetOpsMode = 145,
//                CommandExecLoadCommand = 170,
//                CommandExecAddCommand = 171,

//                // 0x0600 LOG PACKETS

//                // 0x0800 RADIO PACKETS
//                CommandRadioCommunicate = 2048,
//                DataRadioResponse = 2049,
//                CommandRadioTest = 2050,
//                CommandRadioAstrodevCommunicate = 2064,

                DataObc = 0x100,
                DataObcBeacon = 0x101,
                DataObcPong = 0x102,
                DataObcResponse = 0x103,
                DataObcTime = 0x104,
                DataObcNop = 0x17f,
                CommandObc = 0x180,
                CommandObcReset = 0x181,
                CommandObcReboot = 0x182,
                CommandObcPing = 0x183,
                CommandObcSetTime = 0x184,
                CommandObcGetTimeHuman = 0x185,
                CommandObcGetTimeBinary = 0x186,
                CommandObcExternalCommand = 0x187,
                CommandObcExternalTask = 0x188,
                CommandObcInternalRequest = 0x189,
                CommandObcSendBeacon = 0x18a,
                CommandObcNop = 0x1ff,

                DataEps = 0x200,
                DataEpsResponse = 0x201,
                CommandEps = 0x280,
                CommandEpsCommunicate = 0x281,
                CommandEpsSwitchName = 0x282,
                CommandEpsSwitchNumber = 0x283,
                CommandEpsReset = 0x284,
                CommandEpsState = 0x285,
                CommandEpsWatchdog = 0x286,
                CommandEpsSetTime = 0x287,
                CommandEpsMinimumPower = 0x288,
                CommandEpsSwitchNames = 0x289,
                CommandEpsSwitchStatus = 0x28a,

                DataAdcs = 0x300,
                DataAdcsResponse = 0x301,
                CommandAdcs = 0x380,
                CommandAdcsCommunicate = 0x381,
                CommandAdcsState = 0x382,
                CommandAdcsSetRunMode = 0x383,
                CommandAdcsGetAdcsState = 0x384,
                CommandAdcsOrbitParameters = 0x385,

                DataFile = 0x400,
                DataFileCommand = 0x401,
                DataFileMessage = 0x402,
                DataFileQueue = 0x403,
                DataFileCancel = 0x404,
                DataFileComplete = 0x405,
                DataFileReqMeta = 0x406,
                DataFileReqData = 0x407,
                DataFileMetaData = 0x408,
                DataFileChunkData = 0x409,
                DataFileReqComplete = 0x40a,
                CommandFile = 0x480,
                CommandFileListDirectory = 0x481,
                CommandFileTransferFile = 0x482,
                CommandFileTransferNode = 0x483,
                CommandFileTransferRadio = 0x484,
                CommandFileTransferList = 0x485,

                DataExec = 0x500,
                CommandExec = 0x580,
                CommandExecClearQueue = 0x582,
                CommandExecSetOpsMode = 0x583,
                CommandExecEnableChannel = 0x584,
                CommandExecLoadCommand = 170,
                CommandExecAddCommand = 171,

                DataRadio = 0x700,
                DataRadioTest = 0x701,
                DataRadioResponse = 0x701,
                CommandRadio = 0x780,
                CommandRadioTest = 0x781,
                CommandRadioCommunicate = 0x782,
                CommandRadioAstrodevCommunicate = 0x783,
            };

            std::map<TypeId, string> TypeString = {
//                {TypeId::DataObcBeacon, "Beacon"},
//                {TypeId::DataObcNop, "Nop"},
//                {TypeId::DataObcPong, "Pong"},
//                {TypeId::DataEpsResponse, "EpsResponse"},
//                {TypeId::DataRadioResponse, "RadioResponse"},
//                {TypeId::DataAdcsResponse, "AdcsResponse"},
//                {TypeId::DataObcResponse, "Response"},
//                {TypeId::DataIP, "IP"},
//                {TypeId::DataRadioTest, "Test"},
//                {TypeId::DataObcTime, "Time"},
//                {TypeId::DataFileCommand, "FileCommand"},
//                {TypeId::DataFileMessage, "FileMessage"},
//                {TypeId::DataFileQueue, "FileQueue"},
//                {TypeId::DataFileCancel, "FileCancel"},
//                {TypeId::DataFileComplete, "FileComplete"},
//                {TypeId::DataFileReqMeta, "FileReqMeta"},
//                {TypeId::DataFileReqData, "FileReqData"},
//                {TypeId::DataFileMetaData, "FileMetaData"},
//                {TypeId::DataFileChunkData, "FileChunkData"},
//                {TypeId::DataFileReqComplete, "FileReqComplete"},
//                {TypeId::CommandObcReset, "Reset"},
//                {TypeId::CommandObcReboot, "Reboot"},
//                {TypeId::CommandObcSendBeacon, "SendBeacon"},
//                {TypeId::CommandExecClearQueue, "ClearQueue"},
//                {TypeId::CommandObcExternalCommand, "ExternalCommand"},
//                {TypeId::CommandObcExternalTask, "ExternalTask"},
//                {TypeId::CommandRadioTest, "TestRadio"},
//                {TypeId::CommandFileListDirectory, "ListDirectory"},
//                {TypeId::CommandFileTransferFile, "TransferFile"},
//                {TypeId::CommandFileTransferNode, "TransferNode"},
//                {TypeId::CommandFileTransferRadio, "TransferRadio"},
//                {TypeId::CommandFileTransferList, "TransferList"},
//                {TypeId::CommandObcInternalRequest, "InternalRequest"},
//                {TypeId::CommandObcPing, "Ping"},
//                {TypeId::CommandObcSetTime, "SetTime"},
//                {TypeId::CommandObcGetTimeHuman, "GetTimeHuman"},
//                {TypeId::CommandObcGetTimeBinary, "GetTimeBinary"},
//                {TypeId::CommandExecSetOpsMode, "SetOpsMode"},
//                {TypeId::CommandExecEnableChannel, "EnableChannel"},
                {TypeId::DataObcBeacon, "Beacon"},
                {TypeId::DataObcNop, "Nop"},
                {TypeId::DataObcPong, "Pong"},
                {TypeId::DataEpsResponse, "EpsResponse"},
                {TypeId::DataRadioResponse, "RadioResponse"},
                {TypeId::DataAdcsResponse, "AdcsResponse"},
                {TypeId::DataObcResponse, "Response"},
                {TypeId::DataRadioTest, "Test"},
                {TypeId::DataObcTime, "Time"},
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
                {TypeId::CommandObcReset, "Reset"},
                {TypeId::CommandObcReboot, "Reboot"},
                {TypeId::CommandObcSendBeacon, "SendBeacon"},
                {TypeId::CommandExecClearQueue, "ClearQueue"},
                {TypeId::CommandObcExternalCommand, "ExternalCommand"},
                {TypeId::CommandObcExternalTask, "ExternalTask"},
                {TypeId::CommandRadioTest, "TestRadio"},
                {TypeId::CommandFileListDirectory, "ListDirectory"},
                {TypeId::CommandFileTransferFile, "TransferFile"},
                {TypeId::CommandFileTransferNode, "TransferNode"},
                {TypeId::CommandFileTransferRadio, "TransferRadio"},
                {TypeId::CommandFileTransferList, "TransferList"},
                {TypeId::CommandObcInternalRequest, "InternalRequest"},
                {TypeId::CommandObcPing, "Ping"},
                {TypeId::CommandObcSetTime, "SetTime"},
                {TypeId::CommandObcGetTimeHuman, "GetTimeHuman"},
                {TypeId::CommandObcGetTimeBinary, "GetTimeBinary"},
                {TypeId::CommandExecSetOpsMode, "SetOpsMode"},
                {TypeId::CommandExecEnableChannel, "EnableChannel"},
                {TypeId::CommandAdcsCommunicate, "AdcsCommunicate"},
                {TypeId::CommandAdcsState, "AdcsState"},
                {TypeId::CommandAdcsSetRunMode, "AdcsSetRunMode"},
                {TypeId::CommandAdcsGetAdcsState, "AdcsGetAdcsState"},
                {TypeId::CommandAdcsOrbitParameters, "AdcsOrbitParameters"},
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
                {TypeId::CommandRadioCommunicate, "RadioCommunicate"},
                {TypeId::CommandRadioAstrodevCommunicate, "RadioAstrodevCommunicate"},
            };

            std::map<string, TypeId> StringType = {
                {"Beacon", TypeId::DataObcBeacon},
                {"Nop", TypeId::DataObcNop},
                {"Pong", TypeId::DataObcPong},
                {"EpsResponse", TypeId::DataEpsResponse},
                {"RadioResponse", TypeId::DataRadioResponse},
                {"AdcsResponse", TypeId::DataAdcsResponse},
                {"Response", TypeId::DataObcResponse},
                {"Test", TypeId::DataRadioTest},
                {"Time", TypeId::DataObcTime},

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

                {"Reset", TypeId::CommandObcReset},
                {"Reboot", TypeId::CommandObcReboot},
                {"SendBeacon", TypeId::CommandObcSendBeacon},
                {"ClearQueue", TypeId::CommandExecClearQueue},
                {"ExternalCommand", TypeId::CommandObcExternalCommand},
                {"ExternalTask", TypeId::CommandObcExternalTask},
                {"TestRadio", TypeId::CommandRadioTest},
                {"ListDirectory", TypeId::CommandFileListDirectory},
                {"TransferFile", TypeId::CommandFileTransferFile},
                {"TransferNode", TypeId::CommandFileTransferNode},
                {"TransferRadio", TypeId::CommandFileTransferRadio},
                {"TransferList", TypeId::CommandFileTransferList},
                {"InternalRequest", TypeId::CommandObcInternalRequest},
                {"Ping", TypeId::CommandObcPing},
                {"SetTime", TypeId::CommandObcSetTime},
                {"GetTimeHuman", TypeId::CommandObcGetTimeHuman},
                {"GetTimeBinary", TypeId::CommandObcGetTimeBinary},
                {"SetOpsMode", TypeId::CommandExecSetOpsMode},
                {"EnableChannel", TypeId::CommandExecEnableChannel},
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
                {"AdcsOrbitParameters", TypeId::CommandAdcsOrbitParameters},
                {"ExecLoadCommand", TypeId::CommandExecLoadCommand},
                {"ExecAddCommand", TypeId::CommandExecAddCommand},
                {"RadioCommunicate", TypeId::CommandRadioCommunicate},
                {"RadioAstrodevCommunicate", TypeId::CommandRadioAstrodevCommunicate},
            };

//            enum class TypeId : uint16_t {
//                Blank = 0x100,
//                Data = 0x100,
//                DataObcBeacon = 0x100 + 10,
//                DataObcPong = 0x100 + 41,
//                DataEpsResponse = 0x100 + 43,
//                DataRadioResponse = 0x100 + 44,
//                DataAdcsResponse = 0x100 + 45,
//                DataObcResponse = 0x100 + 61,
//                DataIP = 0x100 + 62,
//                DataRadioTest = 0x100 + 63,
//                DataObcTime = 0x100 + 64,
//                DataFileCommand = 0x100 + 71,
//                DataFileMessage = 0x100 + 72,
//                DataFileQueue = 0x100 + 79,
//                DataFileCancel = 0x100 + 80,
//                DataFileComplete = 0x100 + 81,
//                DataFileReqMeta = 0x100 + 82,
//                DataFileReqData = 0x100 + 83,
//                DataFileMetaData = 0x100 + 84,
//                DataFileChunkData = 0x100 + 85,
//                DataFileReqComplete = 0x100 + 86,
//                DataObcNop = 0x7fff,

//                Command = 0x8000,
//                CommandObcReset = 0x8000 + 128,
//                CommandObcReboot = 0x8000 + 129,
//                CommandObcSendBeacon = 0x8000 + 130,
//                CommandExecClearQueue = 0x8000 + 131,
//                CommandObcExternalCommand = 0x8000 + 132,
//                CommandRadioTest = 0x8000 + 133,
//                CommandFileListDirectory = 0x8000 + 134,
//                CommandFileTransferFile = 0x8000 + 135,
//                CommandFileTransferNode = 0x8000 + 136,
//                CommandFileTransferRadio = 0x8000 + 137,
//                CommandFileTransferList = 0x8000 + 138,
//                CommandObcExternalTask = 0x8000 + 139,
//                CommandObcInternalRequest = 0x8000 + 140,
//                CommandObcPing = 0x8000 + 141,
//                CommandObcSetTime = 0x8000 + 142,
//                CommandObcGetTimeHuman = 0x8000 + 143,
//                CommandObcGetTimeBinary = 0x8000 + 144,
//                CommandExecSetOpsMode = 0x8000 + 145,
//                CommandExecEnableChannel = 0x8000 + 146,
//                CommandAdcsCommunicate = 0x8000 + 150,
//                CommandAdcsState = 0x8000 + 151,
//                CommandAdcsSetRunMode = 0x8000 + 152,
//                CommandAdcsGetAdcsState = 0x8000 + 155,
//                CommandAdcsOrbitParameters = 0x8000 + 156,
//                CommandEpsCommunicate = 0x8000 + 160,
//                CommandEpsSwitchName = 0x8000 + 161,
//                CommandEpsSwitchNumber = 0x8000 + 162,
//                CommandEpsReset = 0x8000 + 163,
//                CommandEpsState = 0x8000 + 164,
//                CommandEpsWatchdog = 0x8000 + 165,
//                CommandEpsSetTime = 0x8000 + 166,
//                CommandEpsMinimumPower = 0x8000 + 167,
//                CommandEpsSwitchNames = 0x8000 + 168,
//                CommandEpsSwitchStatus = 0x8000 + 169,
//                CommandExecLoadCommand = 0x8000 + 170,
//                CommandExecAddCommand = 0x8000 + 171,
//                CommandRadioCommunicate = 0x8000 + 180,
//                CommandObcNop = 0x8000 + 255,
//                };

//            std::map<TypeId, string> TypeString = {
//                {TypeId::DataObcBeacon, "Beacon"},
//                {TypeId::DataObcNop, "Nop"},
//                {TypeId::DataObcPong, "Pong"},
//                {TypeId::DataEpsResponse, "EpsResponse"},
//                {TypeId::DataRadioResponse, "RadioResponse"},
//                {TypeId::DataAdcsResponse, "AdcsResponse"},
//                {TypeId::DataObcResponse, "Response"},
//                {TypeId::DataIP, "IP"},
//                {TypeId::DataRadioTest, "Test"},
//                {TypeId::DataObcTime, "Time"},
//                {TypeId::DataFileCommand, "FileCommand"},
//                {TypeId::DataFileMessage, "FileMessage"},
//                {TypeId::DataFileQueue, "FileQueue"},
//                {TypeId::DataFileCancel, "FileCancel"},
//                {TypeId::DataFileComplete, "FileComplete"},
//                {TypeId::DataFileReqMeta, "FileReqMeta"},
//                {TypeId::DataFileReqData, "FileReqData"},
//                {TypeId::DataFileMetaData, "FileMetaData"},
//                {TypeId::DataFileChunkData, "FileChunkData"},
//                {TypeId::DataFileReqComplete, "FileReqComplete"},
//                {TypeId::CommandObcReset, "Reset"},
//                {TypeId::CommandObcReboot, "Reboot"},
//                {TypeId::CommandObcSendBeacon, "SendBeacon"},
//                {TypeId::CommandExecClearQueue, "ClearQueue"},
//                {TypeId::CommandObcExternalCommand, "ExternalCommand"},
//                {TypeId::CommandObcExternalTask, "ExternalTask"},
//                {TypeId::CommandRadioTest, "TestRadio"},
//                {TypeId::CommandFileListDirectory, "ListDirectory"},
//                {TypeId::CommandFileTransferFile, "TransferFile"},
//                {TypeId::CommandFileTransferNode, "TransferNode"},
//                {TypeId::CommandFileTransferRadio, "TransferRadio"},
//                {TypeId::CommandFileTransferList, "TransferList"},
//                {TypeId::CommandObcInternalRequest, "InternalRequest"},
//                {TypeId::CommandObcPing, "Ping"},
//                {TypeId::CommandObcSetTime, "SetTime"},
//                {TypeId::CommandObcGetTimeHuman, "GetTimeHuman"},
//                {TypeId::CommandObcGetTimeBinary, "GetTimeBinary"},
//                {TypeId::CommandExecSetOpsMode, "SetOpsMode"},
//                {TypeId::CommandExecEnableChannel, "EnableChannel"},
//                {TypeId::CommandAdcsCommunicate, "AdcsCommunicate"},
//                {TypeId::CommandAdcsState, "AdcsState"},
//                {TypeId::CommandAdcsSetRunMode, "AdcsSetRunMode"},
//                {TypeId::CommandAdcsGetAdcsState, "AdcsGetAdcsState"},
//                {TypeId::CommandAdcsOrbitParameters, "AdcsOrbitParameters"},
//                {TypeId::CommandEpsCommunicate, "EpsCommunicate"},
//                {TypeId::CommandEpsSwitchName, "EpsSwitchName"},
//                {TypeId::CommandEpsSwitchNumber, "EpsSwitchNumber"},
//                {TypeId::CommandEpsReset, "EpsReset"},
//                {TypeId::CommandEpsState, "EpsState"},
//                {TypeId::CommandEpsWatchdog, "EpsWatchdog"},
//                {TypeId::CommandEpsSetTime, "EpsSetTime"},
//                {TypeId::CommandEpsMinimumPower, "EpsMinimumPower"},
//                {TypeId::CommandEpsSwitchNames, "EpsSwitchNames"},
//                {TypeId::CommandEpsSwitchStatus, "EpsSwitchStatus"},
//                {TypeId::CommandExecLoadCommand, "ExecLoadCommand"},
//                {TypeId::CommandExecAddCommand, "ExecAddCommand"},
//                {TypeId::CommandRadioCommunicate, "RadioCommunicate"},
//            };

//            std::map<string, TypeId> StringType = {
//                {"Beacon", TypeId::DataObcBeacon},
//                {"Nop", TypeId::DataObcNop},
//                {"Pong", TypeId::DataObcPong},
//                {"EpsResponse", TypeId::DataEpsResponse},
//                {"RadioResponse", TypeId::DataRadioResponse},
//                {"AdcsResponse", TypeId::DataAdcsResponse},
//                {"Response", TypeId::DataObcResponse},
//                {"IP", TypeId::DataIP},
//                {"Test", TypeId::DataRadioTest},
//                {"Time", TypeId::DataObcTime},

//                {"FileCommand", TypeId::DataFileCommand},
//                {"FileMessage", TypeId::DataFileMessage},
//                {"FileQueue", TypeId::DataFileQueue},
//                {"FileCancel", TypeId::DataFileCancel},
//                {"FileComplete", TypeId::DataFileComplete},
//                {"FileReqMeta", TypeId::DataFileReqMeta},
//                {"FileReqData", TypeId::DataFileReqData},
//                {"FileMetaData", TypeId::DataFileMetaData},
//                {"FileChunkData", TypeId::DataFileChunkData},
//                {"FileReqComplete", TypeId::DataFileReqComplete},

//                {"Reset", TypeId::CommandObcReset},
//                {"Reboot", TypeId::CommandObcReboot},
//                {"SendBeacon", TypeId::CommandObcSendBeacon},
//                {"ClearQueue", TypeId::CommandExecClearQueue},
//                {"ExternalCommand", TypeId::CommandObcExternalCommand},
//                {"ExternalTask", TypeId::CommandObcExternalTask},
//                {"TestRadio", TypeId::CommandRadioTest},
//                {"ListDirectory", TypeId::CommandFileListDirectory},
//                {"TransferFile", TypeId::CommandFileTransferFile},
//                {"TransferNode", TypeId::CommandFileTransferNode},
//                {"TransferRadio", TypeId::CommandFileTransferRadio},
//                {"TransferList", TypeId::CommandFileTransferList},
//                {"InternalRequest", TypeId::CommandObcInternalRequest},
//                {"Ping", TypeId::CommandObcPing},
//                {"SetTime", TypeId::CommandObcSetTime},
//                {"GetTimeHuman", TypeId::CommandObcGetTimeHuman},
//                {"GetTimeBinary", TypeId::CommandObcGetTimeBinary},
//                {"SetOpsMode", TypeId::CommandExecSetOpsMode},
//                {"EnableChannel", TypeId::CommandExecEnableChannel},
//                {"EpsCommunicate", TypeId::CommandEpsCommunicate},
//                {"EpsSwitchName", TypeId::CommandEpsSwitchName},
//                {"EpsSwitchNumber", TypeId::CommandEpsSwitchNumber},
//                {"EpsReset", TypeId::CommandEpsReset},
//                {"EpsState", TypeId::CommandEpsState},
//                {"EpsWatchdog", TypeId::CommandEpsWatchdog},
//                {"EpsSetTime", TypeId::CommandEpsSetTime},
//                {"EpsMinimumPower", TypeId::CommandEpsMinimumPower},
//                {"EpsSwitchNames", TypeId::CommandEpsSwitchNames},
//                {"EpsSwitchStatus", TypeId::CommandEpsSwitchStatus},
//                {"AdcsCommunicate", TypeId::CommandAdcsCommunicate},
//                {"AdcsState", TypeId::CommandAdcsState},
//                {"AdcsSetRunMode", TypeId::CommandAdcsSetRunMode},
//                {"AdcsGetAdcsState", TypeId::CommandAdcsGetAdcsState},
//                {"AdcsOrbitParameters", TypeId::CommandAdcsOrbitParameters},
//                {"ExecLoadCommand", TypeId::CommandExecLoadCommand},
//                {"ExecAddCommand", TypeId::CommandExecAddCommand},
//                {"RadioCommunicate", TypeId::CommandRadioCommunicate},
//            };

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
                uint32_t response_id;
                uint8_t source_id;
                uint8_t chunks;
                uint8_t chunk_id;
            };

            struct __attribute__ ((packed))  TestHeader
            {
                uint32_t test_id = 0;
                uint32_t size = 0;
                uint32_t packet_id = 0;
            };

//            struct __attribute__ ((packed))  ShortHeader
//            {
//                ShortTypeId type = ShortTypeId::Blank;
//                uint8_t data_size;
//            } sheader;

            struct __attribute__ ((packed))  HeaderV1
            {
                TypeIdV1 type = TypeIdV1::Blank;
                uint8_t data_size;
                uint8_t nodeorig = 254; // refer to NodeData::NODEIDORIG;
                uint8_t nodedest = 255; // refer to NodeData::NODEIDDEST;
                uint8_t chanorig = 0;
            } headerv1;

            struct __attribute__ ((packed))  Header
            {
                TypeId type = TypeId::Blank;
                uint16_t data_size;
                uint8_t nodeorig = 254; // refer to NodeData::NODEIDORIG;
                uint8_t nodedest = 255; // refer to NodeData::NODEIDDEST;
                uint8_t chanorig = 0;
                uint8_t chandest = 0;
            } header;

            CCSDS_Header ccsds_header;
            vector<uint8_t> packetized;
            vector<uint8_t> wrapped;
			/// Data of interest
            vector<uint8_t> data;
            uint16_t crc;
            enum class PacketStyle : uint8_t
                {
                None,
                Minimal,
                V1,
                V2
                };

            PacketStyle style = PacketStyle::V2;

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
            vector<uint8_t> satsm = {0x35, 0x2e, 0xf8, 0x53};
            CRC16 calc_crc;
            void CalcCRC();
            bool CheckCRC();
            int32_t Unwrap(bool checkcrc=true);
            int32_t Unwrap(bool checkcrc, bool minimal_header);
            int32_t RawUnPacketize(bool invert=false, bool checkcrc=true);
            int32_t RawUnPacketize(bool invert, bool checkcrc, bool minimal_header);
            bool ASMUnPacketize(bool checkcrc=true);
            bool SLIPUnPacketize(bool checkcrc=true);
            bool HDLCUnPacketize(bool checkcrc=true);
            bool AX25UnPacketize(bool checkcrc=true);
            bool Wrap();
            bool RawPacketize();
            bool ASMPacketize();
            bool AX25Packetize(string dest_call="", string sour_call="", uint8_t flagcount=2, uint8_t dest_stat=0x60, uint8_t sour_stat=0x61, uint8_t cont=0x03, uint8_t prot=0xf0);
            bool HDLCPacketize(uint8_t flagcount=10);
            bool SLIPPacketize();

        private:

//            Transfer ttransfer;
//            int32_t close_transfer();

        };
    }
}

#endif // PACKETCOMM_H
