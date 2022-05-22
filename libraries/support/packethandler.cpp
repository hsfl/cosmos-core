#include "packethandler.h"
#include "beacon.h"

namespace Cosmos {
    namespace Support {
        PacketHandler::PacketHandler() { }

        int32_t PacketHandler::init(Agent *calling_agent)
        {
            this->agent = calling_agent;
            // Commands
            add_func(PacketComm::TypeId::CommandReset, Reset);
            add_func(PacketComm::TypeId::CommandReboot, Reboot);
            add_func(PacketComm::TypeId::CommandSendBeacon, SendBeacon);
            add_func(PacketComm::TypeId::CommandClearQueue, ClearQueue);
            add_func(PacketComm::TypeId::CommandExternalCommand, ExternalCommand);
            add_func(PacketComm::TypeId::CommandTestRadio, TestRadio);
            add_func(PacketComm::TypeId::CommandListDirectory, ListDirectory);
            add_func(PacketComm::TypeId::CommandTransferFile, TransferForward);
            add_func(PacketComm::TypeId::CommandTransferNode, TransferForward);
            add_func(PacketComm::TypeId::CommandTransferRadio, TransferForward);
            add_func(PacketComm::TypeId::CommandTransferList, TransferForward);
            add_func(PacketComm::TypeId::CommandInternalRequest, InternalRequest);
            add_func(PacketComm::TypeId::CommandPing, Ping);
            add_func(PacketComm::TypeId::CommandSetTime, SetTime);
            add_func(PacketComm::TypeId::CommandAdcsCommunicate, AdcsForward);
            add_func(PacketComm::TypeId::CommandEpsCommunicate, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsSwitchName, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsSwitchNumber, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsReset, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsState, EpsForward);
            add_func(PacketComm::TypeId::CommandExecLoadCommand, ExecForward);
            add_func(PacketComm::TypeId::CommandExecAddCommand, ExecForward);

            // Telemetry
            add_func(PacketComm::TypeId::DataBeacon, DecodeBeacon);
            add_func(PacketComm::TypeId::DataResponse, Response);
            add_func(PacketComm::TypeId::DataTest, Test);

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
            header.met = 86400 * (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart);
            PacketComm packet;
            packet.header.type = PacketComm::TypeId::DataResponse;
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
            FuncEntry &fentry = Funcs[(uint8_t)packet.header.type];
            if (fentry.efunction != nullptr)
            {
                iretn = fentry.efunction(packet, response, this->agent);
            }
            agent->debug_error.Printf("[Type=%hu, Return=%d]%s\n", packet.header.type, iretn, string(response.begin(), response.end()).c_str());
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
        int32_t PacketHandler::Pong(PacketComm& packet, vector<uint8_t>& response, Agent *agent)
        {
            int32_t iretn = 0;
            static uint32_t last_response_id = 0;
            uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            uint16_t pong_size = packet.data.size() - 4;
            filestruc file = data_name_struc(agent->nodeName, "temp", agent->agentName, 0., "pong_"+to_unsigned(response_id));
            if (file.path.size())
            {
                FILE *tf;
                tf = fopen(file.path.c_str(), "a");
                if (tf != nullptr)
                {
                    iretn = fwrite(packet.data.data()+pong_size, pong_size, 1, tf);
                    if (iretn < 0)
                    {
                        iretn = -errno;
                    }
                    fclose(tf);
                    if (data_isfile(file.path) && last_response_id != response_id)
                    {
                        iretn = data_move(file, "incoming", false);
                        last_response_id = response_id;
                    }
                }
                else
                {
                    iretn = GENERAL_ERROR_BAD_FD;
                }
            }
            else
            {
                iretn = GENERAL_ERROR_NAME;
            }
            return iretn;
        }

        int32_t PacketHandler::Test(PacketComm& packet, vector<uint8_t>& response, Agent *agent)
        {
            static CRC16 calc_crc;
            static uint32_t last_test_id = 0;
            int32_t iretn=0;
            struct test_control
            {
                string path;
                uint32_t total_bytes;
                uint32_t total_count;
                uint32_t good_count = 0;
                uint32_t crc_count = 0;
                uint32_t size_count = 0;
                uint32_t skip_count = 0;
                uint32_t last_packet_id = 0;
                ElapsedTime et;
            };
            static map<uint32_t, test_control> tests;
            string sresponse;

            uint32_t test_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            uint32_t packet_id = uint32from(&packet.data[8], ByteOrder::LITTLEENDIAN);
            if (tests.find(test_id) == tests.end())
            {
                if (tests.find(last_test_id) != tests.end())
                {
                    sresponse = to_label("TestMET", (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart)) + to_label(" Test_Id", last_test_id);
                    sresponse +=  to_label(" Packet_Id", tests[last_test_id].last_packet_id);
                    sresponse += " Good: " + to_unsigned(tests[last_test_id].good_count);
                    sresponse += " Skip: " + to_unsigned(tests[last_test_id].skip_count);
                    sresponse += " Size: " + to_unsigned(tests[last_test_id].size_count);
                    sresponse += " Crc: " + to_unsigned(tests[last_test_id].crc_count);
                    sresponse += to_label(" Bytes", tests[last_test_id].total_bytes) + to_label(" Count", tests[last_test_id].total_count);
                    sresponse += " Complete: \n";
                }
                tests[test_id].path = data_name_path(agent->nodeName, "incoming", agent->agentName, 0., "test_"+to_unsigned(test_id));
                tests[test_id].et.reset();
            }
            tests[test_id].total_count = tests[test_id].good_count + tests[test_id].crc_count + tests[test_id].size_count;
            tests[test_id].et.reset();
            if (packet_id - tests[test_id].last_packet_id > 1 && packet_id != ((uint32_t)-1))
            {
                tests[test_id].skip_count += (packet_id - tests[test_id].last_packet_id) - 1;
            }
            uint32_t data_size = uint32from(&packet.data[4], ByteOrder::LITTLEENDIAN);
            if (data_size != packet.data.size() - 14)
            {
                ++tests[test_id].size_count;
            }
            else
            {
                uint16_t crccalc = calc_crc.calc(&packet.data[0], packet.data.size()-2);
                uint16_t crcdata = 256 * packet.data[packet.data.size()-1] + packet.data[packet.data.size()-2];
                if (crccalc != crcdata)
                {
                    ++tests[test_id].crc_count;
                }
                else
                {
                    FILE *tf = fopen(tests[test_id].path.c_str(), "a");
                    iretn = fwrite(packet.data.data(), packet.data.size(), 1, tf);
                    fclose(tf);
                    tests[test_id].total_bytes += data_size;
                    ++tests[test_id].good_count;
                }
            }
            tests[test_id].total_count = tests[test_id].good_count + tests[test_id].crc_count + tests[test_id].size_count + tests[test_id].skip_count;
            sresponse += to_label("TestMET", (utc2unixseconds(currentmjd()) - agent->cinfo->node.utcstart)) + to_label(" Test_Id", test_id);
            if (packet_id == ((uint32_t)-1))
            {
                sresponse +=  to_label(" Packet_Id", tests[test_id].last_packet_id+1);
            }
            else
            {
                sresponse +=  to_label(" Packet_Id", packet_id);
            }
            sresponse += " Good: " + to_unsigned(tests[test_id].good_count);
            sresponse += " Skip: " + to_unsigned(tests[test_id].skip_count);
            sresponse += " Size: " + to_unsigned(tests[test_id].size_count);
            sresponse += " Crc: " + to_unsigned(tests[test_id].crc_count);
            sresponse += to_label(" Bytes", tests[test_id].total_bytes) + to_label(" Count", tests[test_id].total_count);
            if (packet_id == ((uint32_t)-1))
            {
                sresponse += " Complete: ";
                tests.erase(test_id);
            }
            else if (last_test_id < test_id)
            {
                sresponse += " Start: ";
            }
            response.clear();
            response.insert(response.end(), sresponse.begin(), sresponse.end());
            tests[test_id].last_packet_id = packet_id;
            last_test_id = test_id;
            return iretn;
        }

        int32_t PacketHandler::Response(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            PacketComm::ResponseHeader header;
            uint16_t chunk_size = packet.data.size() - COSMOS_SIZEOF(PacketComm::ResponseHeader);
            memcpy(&header, packet.data.data(), COSMOS_SIZEOF(PacketComm::ResponseHeader));
            filestruc file = data_name_struc(agent->nodeName, "incoming", agent->agentName, 0., "response_"+to_unsigned(header.response_id)+"_"+to_unsigned(header.met));
            if (file.path.size())
            {
                FILE *tf;
                if (data_exists(file.path))
                {
                    tf = fopen(file.path.c_str(), "r+");
                }
                else
                {
                    tf = fopen(file.path.c_str(), "w");
                }
                if (tf != nullptr)
                {
                    iretn = fseek(tf, header.chunk_id*chunk_size, SEEK_SET);
                    if (iretn >= 0)
                    {
                        iretn = fwrite(packet.data.data()+chunk_size, chunk_size, 1, tf);
                        if (iretn < 0)
                        {
                            iretn = -errno;
                        }
                    }
                    else
                    {
                        iretn = -errno;
                    }
                    fclose(tf);
                    if (data_isfile(file.path, header.chunks*chunk_size))
                    {
                        iretn = data_move(file, "incoming", false);
                    }
                }
                else
                {
                    iretn = GENERAL_ERROR_BAD_FD;
                }
            }
            else
            {
                iretn = GENERAL_ERROR_NAME;
            }
            return iretn;
        }

        int32_t PacketHandler::DecodeBeacon(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            Beacon beacon;
            string decoded_beacon;
            beacon.Init();
            int32_t iretn = beacon.Decode(packet, decoded_beacon);
            if (iretn < 0)
            {
                response.clear();
                return iretn;
            }
            response.insert(response.begin(), decoded_beacon.begin(), decoded_beacon.end());
            return response.size();
        }

        // Incoming Commands
        int32_t PacketHandler::Reset(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
            memcpy(&verification_check, &packet.data[0], 4);
            if (verification_check != agent->channels.verification)
            {
                return GENERAL_ERROR_ARGS;
            }
            uint16_t seconds;
            seconds = uint16from(&packet.data[4], ByteOrder::LITTLEENDIAN);

            // Send a response
            agent->push_response(packet.header.radio, centisec(), string("Resetting Power"));

            // Command the EPS
            packet.header.type = PacketComm::TypeId::CommandEpsReset;
            packet.data.resize(2);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            agent->push_unwrapped(agent->find_channel("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::Reboot(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
            memcpy(&verification_check, &packet.data[0], 4);
            if (verification_check != agent->channels.verification)
            {
                return GENERAL_ERROR_ARGS;
            }
            data_execute("shutdown -r");
            return iretn;
        }

        int32_t PacketHandler::SendBeacon(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            uint8_t count = packet.data[1];
            Beacon beacon;
            beacon.Init();
            beacon.Encode((Beacon::TypeId)packet.data[0], agent->cinfo);
            response = beacon.data;
            packet.header.type = PacketComm::TypeId::DataBeacon;
            packet.header.dest = packet.header.orig;
            packet.header.orig = agent->nodeId;
            packet.data.clear();
            packet.data.insert(packet.data.end(), beacon.data.begin(), beacon.data.end());
            for (uint16_t i=0; i<count; ++i)
            {
                iretn = agent->push_unwrapped(packet.header.radio, packet);
            }
            return iretn;
        }

        int32_t PacketHandler::ClearQueue(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
            memcpy(&verification_check, &packet.data[0], 4);
            if (verification_check != agent->channels.verification)
            {
                return GENERAL_ERROR_ARGS;
            }
            iretn = agent->clear_channel(packet.data[0]);
            return iretn;
        }

        int32_t PacketHandler::ExternalCommand(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            // Run command, return response
            string eresponse;
            int32_t iretn = data_execute(string(packet.data.begin()+4, packet.data.end()), eresponse);
            response.clear();
            response.insert(response.begin(),eresponse.begin(), eresponse.end());
            uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_response(packet.header.radio, response_id, string(response.begin(), response.end()));
            return iretn;
        }

        int32_t PacketHandler::TestRadio(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn = agent->push_unwrapped(packet.header.radio, packet);
            return iretn;
        }

        int32_t PacketHandler::ListDirectory(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            string node;
            node.insert(node.begin(), packet.data.begin()+6, packet.data.begin()+6+packet.data[5]);
            string agentname;
            agentname.insert(agentname.begin(), packet.data.begin()+node.size()+7, packet.data.begin()+node.size()+7+packet.data[5+node.size()]);

            response.clear();
            for (filestruc file : data_list_files(node, "outgoing", agentname))
            {
                response.insert(response.end(), file.name.begin(), file.name.end());
                response.push_back(' ');
            }
            uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_response(packet.header.radio, response_id, string(response.begin(), response.end()));
            return iretn;
        }

        int32_t PacketHandler::TransferForward(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn = agent->push_unwrapped(agent->find_channel("FILE"), packet);
            return iretn;
        }

        //        int32_t PacketHandler::TransferNode(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        //        {
        //            int32_t iretn = agent->push_unwrapped(packet.header.radio, packet);
        //            return iretn;
        //        }

        //        int32_t PacketHandler::TransferRadio(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        //        {
        //            int32_t iretn = agent->push_unwrapped(packet.header.radio, packet);
        //            return iretn;
        //        }

        //        int32_t PacketHandler::TransferList(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        //        {
        //            int32_t iretn = agent->push_unwrapped(packet.header.radio, packet);
        //            return iretn;
        //        }

        int32_t PacketHandler::InternalRequest(PacketComm& packet, vector<uint8_t>& response, Agent* agent)
        {
            // Run request, return response
            string eresponse;
            string erequest = string(packet.data.begin()+5, packet.data.end());
            int32_t iretn = agent->process_request(erequest, eresponse);
            response.clear();
            response.insert(response.begin(),eresponse.begin(), eresponse.end());
            return iretn;
        }

        int32_t PacketHandler::Ping(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            response.clear();
            response.insert(response.begin(), packet.data.begin()+5, packet.data.end());
            return iretn;
        }

        int32_t PacketHandler::SetTime(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            response.clear();
            response.insert(response.begin(), packet.data.begin()+5, packet.data.end());
            return iretn;
        }

        int32_t PacketHandler::AdcsForward(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->push_unwrapped(agent->find_channel("ADCS"), packet);
            return iretn;
        }

        int32_t PacketHandler::EpsForward(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->push_unwrapped(agent->find_channel("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::ExecForward(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->push_unwrapped(agent->find_channel("EXEC"), packet);
            return iretn;
        }

        //        int32_t PacketHandler::EpsSwitchName(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        //        {
        //            int32_t iretn=0;
        //            iretn = agent->push_unwrapped(agent->find_channel("EPS"), packet);
        //            return iretn;
        //        }

        //        int32_t PacketHandler::EpsSwitchNumber(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        //        {
        //            int32_t iretn=0;
        //            iretn = agent->push_unwrapped(agent->find_channel("EPS"), packet);
        //            return iretn;
        //        }

        //        int32_t PacketHandler::EpsReset(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        //        {
        //            int32_t iretn=0;
        //            iretn = agent->push_unwrapped(agent->find_channel("EPS"), packet);
        //            return iretn;
        //        }

        //        int32_t PacketHandler::EpsState(PacketComm &packet, vector<uint8_t>& response, Agent* agent)
        //        {
        //            int32_t iretn=0;
        //            iretn = agent->push_unwrapped(agent->find_channel("EPS"), packet);
        //            return iretn;
        //        }

    }
}

// TODO:
// - add mutexes in critical sections
// - reconsider the vector return types?
// - janitor thread or function to clean up old responses
// - response_packet size could stand to be variable. Satellite should be fine with 256,
//    ground-side programs will need the full 2 bytes
