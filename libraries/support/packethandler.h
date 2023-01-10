#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include "math/mathlib.h"
#include "support/configCosmos.h"
#include "support/packetcomm.h"
#include "support/beacon.h"
#include "support/timelib.h"
#include "agent/agentclass.h"

// Class to create PacketComm packets and maintain response callbacks

namespace Cosmos {
    namespace Support {
        class PacketHandler
        {
        public:
            PacketHandler();

            int32_t init(Agent* calling_agent);

            typedef int32_t (*RespCallback)(const vector<uint8_t>&);
            int32_t register_response(const RespCallback f);
            int32_t clear_response(const uint16_t packet_id);
            int32_t receive_response_packet(const PacketComm &packet);
            vector<PacketComm> create_response_packets(uint32_t response_id, uint16_t data_size, const vector<uint8_t> &response);

            typedef int32_t (*ExternalFunc)(PacketComm &packet, string &response, Agent* agent);
            struct FuncEntry
            {
                PacketComm::TypeId type;
                //! Pointer to function to call with request vector as argument and returning any error
                ExternalFunc efunction=nullptr;
            };
            FuncEntry Funcs[256];
            int32_t add_func(PacketComm::TypeId type, ExternalFunc function);
            int32_t process(PacketComm &packet, string &response);
            int32_t process(PacketComm &packet);
            int32_t generate(PacketComm &packet);

//            int32_t GenerateBeacon(PacketComm &packet);

            // Telemetry
            static int32_t DecodeResponse(PacketComm &packet, string &response, Agent* agent);
            static int32_t DecodeAdcsResponse(PacketComm &packet, string &response, Agent* agent);
            static int32_t DecodeEpsResponse(PacketComm &packet, string &response, Agent* agent);
            static int32_t DecodeBeacon(PacketComm &packet, string &response, Agent* agent);
            static int32_t DecodePong(PacketComm &packet, string &response, Agent* agent);
            static int32_t DecodeTime(PacketComm &packet, string &response, Agent* agent);
            static int32_t DecodeTest(PacketComm &packet, string &response, Agent* agent);
            static int32_t FileMeta(PacketComm &packet, string &response, Agent* agent);
            static int32_t FileChunk(PacketComm &packet, string &response, Agent* agent);

            // Commands
            static int32_t Reset(PacketComm &packet, string &response, Agent* agent);
            static int32_t Reboot(PacketComm &packet, string &response, Agent* agent);
            static int32_t SendBeacon(PacketComm &packet, string &response, Agent* agent);
            static int32_t ClearQueue(PacketComm &packet, string &response, Agent* agent);
            static int32_t ExternalCommand(PacketComm &packet, string &response, Agent* agent);
            static int32_t ExternalTask(PacketComm &packet, string &response, Agent* agent);
            static int32_t TestRadio(PacketComm &packet, string &response, Agent* agent);
            static int32_t ListDirectory(PacketComm &packet, string &response, Agent* agent);
//            static int32_t TransferNode(PacketComm &packet, string &response, Agent* agent);
//            static int32_t TransferRadio(PacketComm &packet, string &response, Agent* agent);
//            static int32_t TransferList(PacketComm &packet, string &response, Agent* agent);
            static int32_t InternalRequest(PacketComm &packet, string &response, Agent* agent);
            static int32_t Ping(PacketComm &packet, string &response, Agent* agent);
            static int32_t SetTime(PacketComm &packet, string &response, Agent* agent);
            static int32_t GetTimeHuman(PacketComm &packet, string &response, Agent* agent);
            static int32_t GetTimeBinary(PacketComm &packet, string &response, Agent* agent);
            static int32_t AdcsForward(PacketComm &packet, string &response, Agent* agent);
            static int32_t EpsLog(PacketComm &packet, string &response, Agent* agent);
            static int32_t EpsForward(PacketComm &packet, string &response, Agent* agent);
            static int32_t ExecForward(PacketComm &packet, string &response, Agent* agent);
            static int32_t FileForward(PacketComm &packet, string &response, Agent* agent);
            static int32_t RadioForward(PacketComm &packet, string &response, Agent* agent);
            static int32_t EnableChannel(PacketComm &packet, string &response, Agent* agent);
//            static int32_t EpsSwitchName(PacketComm &packet, string &response, Agent* agent);
//            static int32_t EpsSwitchNumber(PacketComm &packet, string &response, Agent* agent);
//            static int32_t EpsReset(PacketComm &packet, string &response, Agent* agent);
//            static int32_t EpsState(PacketComm &packet, string &response, Agent* agent);
            static int32_t request_command(string &, string &response, Agent *agent);

            static int32_t QueueReset(uint16_t seconds, uint32_t verification_check, Agent* agent, string channel="EXEC", NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueReboot(uint32_t verification_check, Agent* agent, string channel="EXEC", NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueSendBeacon(uint8_t btype, uint8_t bcount, Agent* agent, string channel="EXEC", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueBeacon(uint8_t btype, uint8_t bcount, Agent* agent, NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");

            static int32_t QueueAdcsCommunicate(uint8_t unit, uint8_t command, uint16_t rcount, vector<uint8_t> data, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueAdcsState(uint8_t state, vector<uint8_t> data, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");

            static int32_t QueueEpsCommunicate(uint8_t unit, uint8_t command, uint16_t rcount, vector<uint8_t> data, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEpsSwitchName(const string& name, uint8_t state, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEpsSwitchNames(vector<string>& names, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEpsSwitchNames(const string& names, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEpsSwitchNumber(uint16_t number, uint8_t state, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEpsReset(uint16_t seconds, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEpsState(uint8_t state, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEpsWatchdog(uint16_t seconds, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEpsSetTime(double mjd, Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEpsMinimumPower(Agent* agent, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueTransferRadio(uint8_t use_radio, bool availability, Agent* agent, NodeData::NODE_ID_TYPE dest);
            static int32_t QueueTestRadio(uint8_t start, uint8_t step, uint8_t stop, uint32_t count, Agent* agent, string testradio="", NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueSetTime(double mjd, int8_t direction, Agent* agent, string channel="", NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, string radio="");
            static int32_t QueueEnableChannel(const string& name, uint8_t enable, Agent* agent, const std::string &channel="", NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, const string& radio="");
            static int32_t QueueEnableChannel(uint8_t number, uint8_t enable, Agent* agent, const string& channel="", NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, const string& radio="");
        private:
            Agent* agent;
            /// Current packet id index, to index response_packets
            uint16_t packet_idx;

            /// Set packet_idx to index of next available slot
            bool get_next_packet_id();
            Error *errorlog;

        };
    }
}


#endif // PACKETHANDLER_H
