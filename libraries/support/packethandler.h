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

            int32_t init(Agent* calling_agent, uint16_t secret=0x1111);

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
            static int32_t DecodeBeacon(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t Test(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t FileMeta(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t FileChunk(PacketComm &packet, vector<uint8_t>& response, Agent* agent);

            // Commands
            uint32_t secret;
            static int32_t Reset(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t Reboot(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            struct sendbeacon_packet
            {
                uint8_t radio = 0;
                uint8_t type = (uint8_t)Beacon::TypeId::CPU1BeaconS;
                uint8_t count = 1;
            };
            static int32_t SendBeacon(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            struct clearradioqueue_packet
            {
                uint8_t radio = 0;
            };
            static int32_t ClearRadioQueue(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            struct externalcommand_packet
            {
                uint8_t radio = 0;
                uint32_t response_id = 0;
                string command;
            };
            static int32_t ExternalCommand(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            struct request_packet
            {
                uint8_t radio = 0;
                uint32_t response_id = 0;
                string request;
            };
            static int32_t InternalRequest(PacketComm &packet, vector<uint8_t>& response, Agent* agent);

            struct testradio_packet
            {
                uint8_t radio = 0;
                uint8_t start = 0;
                uint8_t step = 1;
                uint8_t count = 255;
                uint32_t total = 10000;
            };

//            static int32_t TestRadio(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            struct listdirectory_packet
            {
                uint8_t radio = 0;
                string node;
                string agent;
            };
            static int32_t ListDirectory(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t TransferFile(PacketComm &packet, vector<uint8_t>& response, Agent* agent);
            static int32_t Ping(PacketComm &packet, vector<uint8_t>& response, Agent* agent);

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
