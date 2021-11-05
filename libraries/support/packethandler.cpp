#include "packethandler.h"

namespace Cosmos {
    namespace Support {
        PacketHandler::PacketHandler() { }

        int32_t PacketHandler::register_response(RespCallback f)
        {
            if (get_next_packet_id()) {
                response_packets[packet_idx].set_callback(f);
                return static_cast<int32_t>(packet_idx);
            }
            // TODO: return proper error code
            cout << "Error in PacketHandler::register_response(), no available packet_id" << endl;
            return -1;
        }

        /** Insert data of PacketComm packet of type Response into appropriate
         *  response_packets slot. Assumes packet is valid, and a proper response
         *  packet type.
         */
        int32_t PacketHandler::receive_response_packet(const PacketComm &packet)
        {
            uint16_t response_id = packet.data[1] | packet.data[2] << 8;
            if(response_packets[response_id].is_available()) {
                cout << "Error in PacketHandler, no response registered for response_id: " << response_id << endl;
                // TODO: return proper error code
                return -1;
            }
            response_packets[response_id].insert(packet.data);
            response_packets[response_id].attempt_resolution(60.);
        }

        bool PacketHandler::get_next_packet_id()
        {
            for (int i = 0; i < 256; ++i) {
                packet_idx += i;
                if (packet_idx >= 256) {
                    packet_idx %= 256;
                }
                if (response_packets[packet_idx].is_available()) {
                    return true;
                }
            }

            return false;
        }

        /**
         * Create a response type packet to be addressed to the
         * packet id contained within the addressee packet
         */
        vector<PacketComm> PacketHandler::create_response_packets(const PacketComm &addressee, const vector<uint8_t> &data) {
            vector<PacketComm> packets;
            uint16_t packet_id = addressee.data[0] | addressee.data[1] << 8;
            // TODO: check data length and break up if necessary
            uint8_t total_chunks = 1;
            packets.resize(total_chunks);
            uint8_t chunk_id = 0;
            vector<uint8_t> resp_data(5 + data.size());
            resp_data[0] = addressee.type;
            resp_data[1] = packet_id & 0xff;
            resp_data[2] = packet_id >> 8;
            resp_data[3] = chunk_id;
            resp_data[4] = total_chunks;
            std::copy(data.begin(), data.end(), &resp_data[5]);
            packets[chunk_id].type = packets[chunk_id].TypeId["Response"];
            packets[chunk_id].data = resp_data;
            return packets;
        }

        //////////////////////////////////////////////////////////////
        //                     Response Handler                     //    
        //////////////////////////////////////////////////////////////

        ResponseHandler::ResponseHandler()
        {
            callback = nullptr;
        }

        bool ResponseHandler::is_available()
        {
            return callback == nullptr;
        }

        void ResponseHandler::set_callback(const RespCallback f) {
            callback = f;
        }

        int32_t ResponseHandler::insert(const vector<uint8_t> &bytes)
        {
            uint8_t chunk_id = bytes[3];
            uint8_t total_chunks = bytes[4];
            // Resize vector to expected response size
            if (data.size() != total_chunks) {
                data.resize(total_chunks);
            }
            // Check if packet index has already been receieved
            if (data[chunk_id].size() > 0) {
                return 0;
            }
            // starting index of response data
            int data_start = 5;
            data[chunk_id] = vector<uint8_t>(bytes.begin() + data_start, bytes.end());
            last_receieved_time = currentmjd();

            return 0;
        }

        int32_t ResponseHandler::attempt_resolution(const double lapse) {
            double lapse_mjd = lapse / 86400;
            bool all_packets_received = true;
            // Check if all expected packets have been receieved
            for (auto p : data) {
                if (p.size() == 0) {
                    all_packets_received = false;
                    break;
                }
            }
            // Only continue if lapse time has passed or all expected packets received
            if (currentmjd() < last_receieved_time + lapse_mjd && !all_packets_received) {
                return 0;
            }

            // Proceed with response resolution since conditions are cleared
            vector<uint8_t> full_response;
            for (auto p : data) {
                full_response.insert(full_response.end(), p.begin(), p.end());
            }

            // Resolve resposne
            callback(full_response);
            // Reset to initial, make this available for reuse
            reset();
        }

        void ResponseHandler::reset() {
            callback = nullptr;
            last_receieved_time = 0;
            vector<vector<uint8_t>>().swap(data);
        }
    }
}