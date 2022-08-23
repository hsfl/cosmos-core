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
        class ResponseHandler
        {
        public:
            ResponseHandler();
            /// Insert indexed response packet's data into data vector
            int32_t insert(const vector<uint8_t> &bytes);
            /// Attempt callback execution on currently received data.
            /// Will succeed if last indexed packet has been receieved
            /// or if a specified amount of time has passed (in seconds).
            int32_t attempt_resolution(const double lapse);
            /// Check if this response packet is available for use
            bool is_available();
            typedef int32_t (*RespCallback)(const vector<uint8_t>&);
            /// Setter
            void set_callback(RespCallback f);
        private:
            // Holds indexed packet response data
            vector<vector<uint8_t>> data;
            // Execute this callback function when data is all receieved
            RespCallback callback;
            /// Timestamp of last receieved packet, use when cleaning up lost responses
            double last_receieved_time;

            // Make this response handler available for reuse after resolution
            void reset();
        };

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

            typedef int32_t (*ExternalFunc)(PacketComm &packet, vector<uint8_t> &response, Agent* agent);
            struct FuncEntry
            {
                PacketComm::TypeId type;
                //! Pointer to function to call with request vector as argument and returning any error
                ExternalFunc efunction;
            };
            FuncEntry Funcs[256];
            int32_t add_func(PacketComm::TypeId type, ExternalFunc function);
            int32_t process(PacketComm &packet, vector<uint8_t> &response);
            int32_t process(PacketComm &packet);
            int32_t generate(PacketComm &packet);

//            int32_t GenerateBeacon(PacketComm &packet);

            // Telemetry
            static int32_t Response(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t AdcsResponse(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t EpsResponse(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t DecodeBeacon(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t Pong(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t Test(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t FileMeta(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t FileChunk(PacketComm &packet, vector<uint8_t>& response, Agent* agent);

            // Commands
            static int32_t Reset(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t Reboot(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t SendBeacon(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t ClearQueue(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t ExternalCommand(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t TestRadio(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t ListDirectory(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
//            static int32_t TransferNode(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
//            static int32_t TransferRadio(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
//            static int32_t TransferList(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t InternalRequest(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t Ping(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t SetTime(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t GetTimeHuman(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t GetTimeBinary(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t AdcsForward(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t EpsLog(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t EpsForward(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t ExecForward(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t FileForward(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
//            static int32_t EpsSwitchName(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
//            static int32_t EpsSwitchNumber(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
//            static int32_t EpsReset(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
//            static int32_t EpsState(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t request_command(string &, string &response, Agent *agent);

            static int32_t QueueReset(uint16_t seconds, uint32_t verification_check, Agent* agent, string channel="EXEC", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);
            static int32_t QueueReboot(uint32_t verification_check, Agent* agent, string channel="EXEC", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);
            static int32_t QueueSendBeacon(uint8_t btype, uint8_t bcount, Agent* agent, string channel="EXEC", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);

            static int32_t QueueEpsCommunicate(uint8_t unit, uint8_t command, uint16_t rcount, vector<uint8_t> data, Agent* agent, string channel="EPS", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);
            static int32_t QueueEpsSwitchName(string name, uint16_t seconds, Agent* agent, string channel="EPS", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);
            static int32_t QueueEpsSwitchNumber(uint16_t number, uint16_t seconds, Agent* agent, string channel="EPS", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);
            static int32_t QueueEpsReset(uint16_t seconds, Agent* agent, string channel="EPS", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);
            static int32_t QueueEpsState(uint8_t state, Agent* agent, string channel="EPS", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);
            static int32_t QueueEpsWatchdog(uint16_t seconds, Agent* agent, string channel="EPS", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);
            static int32_t QueueEpsSetTime(double mjd, Agent* agent, string channel="EPS", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);
            static int32_t QueueEpsMinimumPower(Agent* agent, string channel="EPS", NodeData::NODE_ID_TYPE orig=NodeData::NODEIDORIG, NodeData::NODE_ID_TYPE dest=NodeData::NODEIDDEST, uint8_t radio=0);

        private:
            Agent* agent;
            /// Register response callbacks in this array
            ResponseHandler response_packets[256];
            /// Current packet id index, to index response_packets
            uint16_t packet_idx;

            /// Set packet_idx to index of next available slot
            bool get_next_packet_id();
            Error *errorlog;

            mutex mtx;
        };
    }
}


#endif // PACKETHANDLER_H
