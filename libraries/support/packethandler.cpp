#include "packethandler.h"
#include "beacon.h"

namespace Cosmos {
    namespace Support {
        PacketHandler::PacketHandler() { }

        int32_t PacketHandler::init(Agent *calling_agent, uint16_t secret)
        {
            this->agent = calling_agent;
            this->secret = secret;
            // Commands
            add_func(PacketComm::TypeId::Reset, Reset);
            add_func(PacketComm::TypeId::Reboot, Reboot);
            add_func(PacketComm::TypeId::SendBeacon, SendBeacon);
            add_func(PacketComm::TypeId::ClearRadioQueue, ClearRadioQueue);
            add_func(PacketComm::TypeId::ExternalCommand, ExternalCommand);
            add_func(PacketComm::TypeId::ListDirectory, ListDirectory);
//            add_func(PacketComm::TypeId::TestRadio, TestRadio);

            // Telemetry
            add_func(PacketComm::TypeId::Test, Test);
            add_func(PacketComm::TypeId::Response, Response);
            for (uint8_t ti=(uint8_t)PacketComm::TypeId::BeaconStart; ti<=(uint8_t)PacketComm::TypeId::BeaconEnd; ++ti)
            {
                add_func((PacketComm::TypeId)ti, DecodeBeacon);
            }
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
            uint32_t response_id = uint32from((uint8_t*)&packet.data[0], ByteOrder::LITTLEENDIAN);
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
        vector<PacketComm> PacketHandler::create_response_packets(uint32_t response_id, uint16_t data_size, const vector<uint8_t> &response)
        {
            vector<PacketComm> packets;
            PacketComm::ResponseHeader header;
            header.response_id = response_id;
            header.met = 86400 * agent->cinfo->node.met;
            PacketComm packet;
            packet.type = PacketComm::TypeId::Response;
            uint8_t chunk_size = (data_size-COSMOS_SIZEOF(PacketComm::ResponseHeader));
            if (response.size() / chunk_size > 254)
            {
                header.chunks = 255;
            }
            else
            {
                header.chunks = response.size() / chunk_size + 1;
            }
            for (header.chunk_id=0; header.chunk_id<header.chunks; ++header.chunk_id)
            {
                uint16_t chunk_begin = header.chunk_id * chunk_size;
                uint16_t chunk_end = chunk_begin + chunk_size;
                if (chunk_end > response.size())
                {
                    chunk_end = response.size();
                }
                packet.data.resize(10);
                memcpy(packet.data.data(), &header, 10);
                packet.data.insert(packet.data.end(), &response[chunk_begin], &response[chunk_end]);
                packets.push_back(packet);
            }
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
            FuncEntry &fentry = Funcs[(uint8_t)packet.type];
            if (fentry.efunction != nullptr)
            {
                iretn = fentry.efunction(packet, response, this->agent);
            }
            return iretn;
        }

        int32_t PacketHandler::add_func(PacketComm::TypeId type, PacketHandler::ExternalFunc function)
        {
            FuncEntry tentry;
            tentry.type = type;
            tentry.efunction = function;
            Funcs[(uint8_t)type] = tentry;
            return 0;
        }

        // Incoming Packets
        int32_t PacketHandler::Test(PacketComm& packet, vector<uint8_t>& response, Agent *agent)
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

            uint32_t test_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            uint32_t packet_id = uint32from(&packet.data[4], ByteOrder::LITTLEENDIAN);
            string sresponse = to_label("MET", agent->cinfo->node.met) + to_label(" Test_Id", test_id) + to_label(" Packet_Id", packet_id);
            tests[test_id].total_count = tests[test_id].good_count + tests[test_id].crc_count + tests[test_id].size_count;
            if (packet_id == ((uint32_t)-1))
            {
                sresponse += " Finish: " + to_unsigned(tests[test_id].good_count);
                sresponse += to_label(" Bytes", tests[test_id].total_bytes) + to_label(" Count", tests[test_id].total_count);
                tests.erase(test_id);
            }
            else
            {
                if (packet_id - tests[test_id].last_packet_id > 1)
                {
                    tests[test_id].skip_count;
                    tests[test_id].skip_total += (packet_id - tests[test_id].last_packet_id) - 1;
                }
                tests[test_id].last_packet_id = packet_id;
                uint32_t data_size = uint32from(&packet.data[8], ByteOrder::LITTLEENDIAN);
                if (data_size != packet.data.size() - 14)
                {
                    ++tests[test_id].size_count;
                    sresponse += " Size_Error: " + to_unsigned(tests[test_id].size_count) + " " + to_unsigned(data_size) + " " + to_unsigned(packet.data.size()-14);
                }
                else
                {
                    CRC16 calc_crc;
                    uint16_t crccalc = calc_crc.calc(&packet.data[0], packet.data.size()-2);
                    uint16_t crcdata = 256 * packet.data[packet.data.size()-1] + packet.data[packet.data.size()-2];
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
            }
            response.clear();
            response.insert(response.end(), sresponse.begin(), sresponse.end());
            return iretn;
        }

        int32_t PacketHandler::Response(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            struct response_control
            {
                uint32_t response_id;
                uint32_t met;
                string response;
                ElapsedTime et;
            };
            static map<uint32_t, response_control> responses;

            uint32_t response_id = uint32from(&packet.data[2], ByteOrder::LITTLEENDIAN);
            responses[response_id].met = uint32from(&packet.data[6], ByteOrder::LITTLEENDIAN);
            string sresponse = to_label("MET", responses[response_id].met) + to_label(" Response_Id", responses[response_id].response_id) + to_label(" Chunk_Id", packet.data[1]) + to_label(" Chunks", packet.data[0]);
            responses[response_id].response.insert(responses[response_id].response.end(), packet.data.begin()+8, packet.data.end());
            sresponse += to_label(" Response", string(responses[response_id].response));
            response.clear();
            response.insert(response.end(), sresponse.begin(), sresponse.end());
            return iretn;
        }

        int32_t PacketHandler::DecodeBeacon(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            Beacon beacon;
            string decoded_beacon;
            beacon.Decode(packet, decoded_beacon);
            response.insert(response.begin(), decoded_beacon.begin(), decoded_beacon.end());
            return response.size();
        }

        // Incoming Commands
        int32_t PacketHandler::Reset(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
//            uint32_t secret_check;
//            memcpy(&secret_check, &packet.data[0], 4);
//            if (secret_check != secret)
//            {
//                continue;
//            }
            // We will need some way to reset the EPS here
            return iretn;
        }

        int32_t PacketHandler::Reboot(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
//            uint32_t secret_check;
//            memcpy(&secret_check, &packet.data[0], 4);
//            if (secret_check != secret)
//            {
//                continue;
//            }
            data_execute("shutdown -r");
            return iretn;
        }

        int32_t PacketHandler::SendBeacon(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            packet.type = (PacketComm::TypeId)packet.data[0];
            uint8_t count = packet.data[1];
            uint8_t radio = packet.data[2];
            Beacon beacon;
            beacon.Init(agent);
            beacon.Generate(packet);
            packet.RawPacketize();
//            for (uint16_t i=0; i<count; ++i)
//            {
//                switch (radio)
//                {
//                case 0:
//                    push_packet(txs_queue, txs_mtx, packet);
//                    push_packet(net_queue, net_mtx, packet);
//                    break;
//                case 1:
//                    push_packet(txs_queue, txs_mtx, packet);
//                    break;
//                case 2:
//                    push_packet(net_queue, net_mtx, packet);
//                    break;
//                }
//                secondsleep(.01);
//            }
            return iretn;
        }

        int32_t PacketHandler::ClearRadioQueue(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
//            switch (packet.data[2])
//            {
//            case 0:
//                clear_queue(txs_queue, txs_mtx);
//                clear_queue(net_queue, net_mtx);
//                break;
//            case 1:
//                clear_queue(txs_queue, txs_mtx);
//                break;
//            case 2:
//                clear_queue(net_queue, net_mtx);
//                break;
//            }
            return iretn;
        }

        int32_t PacketHandler::ExternalCommand(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            // Run command, return response
            string eresponse;
            int32_t iretn = data_execute(string(packet.data.begin()+5, packet.data.end()), eresponse);
            response.clear();
            response.insert(response.begin(),eresponse.begin(), eresponse.end());
            return iretn;
        }

//        int32_t PacketHandler::TestRadio(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
//        {
//            int32_t iretn=0;
//            return iretn;
//        }

        int32_t PacketHandler::ListDirectory(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            string node;
//            node.resize(packet.data[5]);
            node.insert(node.begin(), packet.data.begin()+6, packet.data.begin()+6+packet.data[5]);
            string agentname;
//            agentname.resize(packet.data[node.size()+1]);
            agentname.insert(agentname.begin(), packet.data.begin()+node.size()+7, packet.data.begin()+node.size()+7+packet.data[5+node.size()]);

            response.clear();
            for (filestruc file : data_list_files(node, "outgoing", agentname))
            {
                response.insert(response.end(), file.name.begin(), file.name.end());
                response.push_back(' ');
            }
            return iretn;
        }

        int32_t PacketHandler::TransferFile(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            return iretn;
        }

        // Support
        int32_t PacketHandler::GenerateBeacon(PacketComm &packet)
        {
            int32_t iretn;
            Beacon beacon;
            beacon.Init(agent);
            iretn = beacon.Generate(packet);
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
