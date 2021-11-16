#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include "math/mathlib.h"
#include "support/configCosmos.h"
#include "support/packetcomm.h"
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

            int32_t init(cosmosstruc* cinfo, uint16_t secret);

            typedef int32_t (*RespCallback)(const vector<uint8_t>&);
            int32_t register_response(const RespCallback f);
            int32_t clear_response(const uint16_t packet_id);
            int32_t receive_response_packet(const PacketComm &packet);
            vector<PacketComm> create_response_packets(const PacketComm &addressee, const vector<uint8_t> &data);

            typedef int32_t (*ExternalFunc)(vector<uint8_t>& data, vector<uint8_t> &response, cosmosstruc* cinfo);
            struct FuncEntry
            {
                uint8_t type;
                //! Pointer to function to call with request vector as argument and returning any error
                ExternalFunc efunction;
            };
            FuncEntry Funcs[256];
            int32_t add_func(uint8_t index, ExternalFunc function);
            int32_t process(PacketComm &packet, vector<uint8_t> &response);
            int32_t process(PacketComm &packet);

            static int32_t Test(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);
            static int32_t FileMeta(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);
            static int32_t FileChunk(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);

            static int32_t Reset(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);
            static int32_t Reboot(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);
            static int32_t SendBeacon(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);
            static int32_t ClearRadioQueue(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);
            static int32_t ExternalCommand(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);
            static int32_t TestRadio(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);
            static int32_t Transfer(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo);
        private:
            cosmosstruc* cinfo;
            /// Register response callbacks in this array
            ResponseHandler response_packets[256];
            /// Current packet id index, to index response_packets
            uint16_t packet_idx;

            /// Set packet_idx to index of next available slot
            bool get_next_packet_id();
            uint32_t secret;

            mutex mtx;
        };
    }
}


#endif // PACKETHANDLER_H
