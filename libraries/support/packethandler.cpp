#include "packethandler.h"

namespace Cosmos {
    namespace Support {
        PacketHandler::PacketHandler() { }

        int32_t PacketHandler::init(cosmosstruc *cinfo, uint16_t secret)
        {
            this->cinfo = cinfo;
            this->secret = secret;
            add_func((uint8_t)PacketComm::TypeId::Reset, Reset);
            add_func((uint8_t)PacketComm::TypeId::Reboot, Reboot);
            add_func((uint8_t)PacketComm::TypeId::ExternalCommand, ExternalCommand);
            add_func((uint8_t)PacketComm::TypeId::SendBeacon, SendBeacon);
            add_func((uint8_t)PacketComm::TypeId::ClearRadioQueue, ClearRadioQueue);
            add_func((uint8_t)PacketComm::TypeId::TestRadio, TestRadio);
            add_func((uint8_t)PacketComm::TypeId::Test, Test);
            return 0;
        }

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

            return 0;
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
            packets[chunk_id].type = (uint8_t)PacketComm::TypeId::Response; //packets[chunk_id].TypeId["Response"];
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

            return 0;
        }

        void ResponseHandler::reset() {
            callback = nullptr;
            last_receieved_time = 0;
            vector<vector<uint8_t>>().swap(data);
        }

        int32_t PacketHandler::process(PacketComm& packet)
        {
            vector<uint8_t> response;
            return process(packet, response);
        }

        int32_t PacketHandler::process(PacketComm& packet, vector<uint8_t>& response)
        {
            int32_t iretn;
            FuncEntry &fentry = Funcs[packet.type];
            if (fentry.efunction != nullptr)
            {
                iretn = fentry.efunction(packet.data, response, this->cinfo);
            }
            return iretn;
        }

        int32_t PacketHandler::add_func(uint8_t index, PacketHandler::ExternalFunc function)
        {
            FuncEntry tentry;
            tentry.type = index;
            tentry.efunction = function;
            Funcs[index] = tentry;
            return 0;
        }

        // Incoming Packets
        int32_t PacketHandler::Test(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo)
        {
            int32_t iretn=0;
            struct test_control
            {
                uint32_t total_bytes;
                uint32_t total_count;
                uint32_t good_count = 0;
                uint32_t crc_count = 0;
                uint32_t size_count = 0;
                uint32_t skip_total = 0;
                uint32_t skip_count = 0;
                uint32_t last_packet_id = 0;
                ElapsedTime et;
            };
            static map<uint32_t, test_control> tests;

            uint32_t test_id = uint32from(&data[0], ByteOrder::LITTLEENDIAN);
            uint32_t packet_id = uint32from(&data[4], ByteOrder::LITTLEENDIAN);
            string sresponse = to_label("MET:", cinfo->node.met) + to_label(" Test_Id", test_id) + to_label(" Packet_Id", packet_id);
            tests[test_id].total_count = tests[test_id].good_count + tests[test_id].crc_count + tests[test_id].size_count;
            if (tests[test_id].et.lap() > 10.)
            {
                    sresponse += " Finish: " + to_unsigned(tests[test_id].good_count);
                    tests.erase(test_id);
            }
            if (packet_id - tests[test_id].last_packet_id > 1)
            {
                tests[test_id].skip_count;
                tests[test_id].skip_total += (packet_id - tests[test_id].last_packet_id) - 1;
            }
            tests[test_id].last_packet_id = packet_id;
            uint32_t data_size = uint32from(&data[8], ByteOrder::LITTLEENDIAN);
            if (data_size != data.size() - 14)
            {
                ++tests[test_id].size_count;
                sresponse += " Size_Error: " + to_unsigned(tests[test_id].size_count) + " " + to_unsigned(data_size) + " " + to_unsigned(data.size()-14);
            }
            else
            {
                CRC16 calc_crc;
                uint16_t crccalc = calc_crc.calc(&data[0], data.size()-2);
                uint16_t crcdata = 256 * data[data.size()-1] + data[data.size()-2];
                if (crccalc != crcdata)
                {
                    ++tests[test_id].crc_count;
                    sresponse += " Crc_Error: " + to_unsigned(tests[test_id].crc_count) + " " + to_unsigned(crccalc) + " " + to_unsigned(crcdata);
                }
                else
                {
                    tests[test_id].total_bytes += data_size;
                    ++tests[test_id].good_count;
                    sresponse += " Good: " + to_unsigned(tests[test_id].good_count);
                }
            }
            tests[test_id].total_count = tests[test_id].good_count + tests[test_id].crc_count + tests[test_id].size_count;
            sresponse += to_label(" Bytes", tests[test_id].total_bytes) + to_label(" Count", tests[test_id].total_count);
            response.clear();
            response.insert(response.end(), sresponse.begin(), sresponse.end());
            return iretn;
        }

        // Incoming Commands
        int32_t PacketHandler::Reset(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo)
        {
            int32_t iretn=0;
            return iretn;
        }

        int32_t PacketHandler::Reboot(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo)
        {
            int32_t iretn=0;
            return iretn;
        }

        int32_t PacketHandler::SendBeacon(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo)
        {
            int32_t iretn=0;
            return iretn;
        }

        int32_t PacketHandler::ClearRadioQueue(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc* cinfo)
        {
            int32_t iretn=0;
            return iretn;
        }

        int32_t PacketHandler::ExternalCommand(vector<uint8_t>& data, vector<uint8_t>& response, cosmosstruc *cinfo)
        {
            // Run command, return response
            string eresponse;
            int32_t iretn = data_execute(string(data.begin(), data.end()), eresponse);
            response.clear();
            response.insert(response.begin(),eresponse.begin(), eresponse.end());
            return iretn;
        }

        int32_t PacketHandler::TestRadio(vector<uint8_t> &data, vector<uint8_t>& response, cosmosstruc* cinfo)
        {
            int32_t iretn=0;
            return iretn;
        }
    }
}

// TODO:
// - add mutexes in critical sections
// - reconsider the vector return types?
// - janitor thread or function to clean up old responses
// - response_packet size could stand to be variable. Satellite should be fine with 256,
//    ground-side programs will need the full 2 bytes
