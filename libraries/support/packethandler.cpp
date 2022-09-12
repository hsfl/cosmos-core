#include "packethandler.h"
#include "beacon.h"

namespace Cosmos {
    namespace Support {
        PacketHandler::PacketHandler() { }

        int32_t PacketHandler::init(Agent *calling_agent)
        {
            this->agent = calling_agent;

            // File Transfer
            add_func(PacketComm::TypeId::DataFileCommand, FileForward);
            add_func(PacketComm::TypeId::DataFileMessage, FileForward);
            add_func(PacketComm::TypeId::DataFileQueue, FileForward);
            add_func(PacketComm::TypeId::DataFileCancel, FileForward);
            add_func(PacketComm::TypeId::DataFileComplete, FileForward);
            add_func(PacketComm::TypeId::DataFileReqMeta, FileForward);
            add_func(PacketComm::TypeId::DataFileReqData, FileForward);
            add_func(PacketComm::TypeId::DataFileMetaData, FileForward);
            add_func(PacketComm::TypeId::DataFileChunkData, FileForward);
            add_func(PacketComm::TypeId::DataFileReqComplete, FileForward);

            // Commands
            add_func(PacketComm::TypeId::CommandReset, Reset);
            add_func(PacketComm::TypeId::CommandReboot, Reboot);
            add_func(PacketComm::TypeId::CommandSendBeacon, SendBeacon);
            add_func(PacketComm::TypeId::CommandClearQueue, ClearQueue);
            add_func(PacketComm::TypeId::CommandExternalCommand, ExternalCommand);
            add_func(PacketComm::TypeId::CommandTestRadio, TestRadio);
            add_func(PacketComm::TypeId::CommandListDirectory, ListDirectory);
            add_func(PacketComm::TypeId::CommandTransferFile, FileForward);
            add_func(PacketComm::TypeId::CommandTransferNode, FileForward);
            add_func(PacketComm::TypeId::CommandTransferRadio, FileForward);
            add_func(PacketComm::TypeId::CommandTransferList, FileForward);
            add_func(PacketComm::TypeId::CommandInternalRequest, InternalRequest);
            add_func(PacketComm::TypeId::CommandPing, Ping);
            add_func(PacketComm::TypeId::CommandSetTime, SetTime);
            add_func(PacketComm::TypeId::CommandGetTimeHuman, GetTimeHuman);
            add_func(PacketComm::TypeId::CommandGetTimeBinary, GetTimeBinary);
            add_func(PacketComm::TypeId::CommandAdcsCommunicate, AdcsForward);
            add_func(PacketComm::TypeId::CommandAdcsState, AdcsForward);
            add_func(PacketComm::TypeId::CommandEpsCommunicate, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsSwitchName, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsSwitchNumber, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsReset, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsState, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsWatchdog, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsSetTime, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsMinimumPower, EpsForward);
            add_func(PacketComm::TypeId::CommandExecLoadCommand, ExecForward);
            add_func(PacketComm::TypeId::CommandExecAddCommand, ExecForward);

            // Telemetry
            add_func(PacketComm::TypeId::DataBeacon, DecodeBeacon);
            add_func(PacketComm::TypeId::DataAdcsResponse, AdcsResponse);
            add_func(PacketComm::TypeId::DataEpsResponse, EpsResponse);
            add_func(PacketComm::TypeId::DataResponse, Response);
            add_func(PacketComm::TypeId::DataTest, Test);

            return 0;
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
            header.met = (currentmjd() - agent->cinfo->node.utcstart);
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
                packet.data.resize(sizeof(header));
                memcpy(packet.data.data(), &header, sizeof(header));
                packet.data.insert(packet.data.end(), &response[chunk_begin], &response[chunk_end]);
                packets.push_back(packet);
            }
            return packets;
        }

        int32_t PacketHandler::process(PacketComm& packet)
        {
            string response;
            return process(packet, response);
        }

        int32_t PacketHandler::process(PacketComm& packet, string &response)
        {
            int32_t iretn = 0;
            response.clear();
            FuncEntry &fentry = Funcs[(uint8_t)packet.header.type];
            if (fentry.efunction != nullptr)
            {
                iretn = fentry.efunction(packet, response, this->agent);
            }
            else
            {
                iretn = GENERAL_ERROR_OUTOFRANGE;
            }
            // Let the handler do something with the string response

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
        int32_t PacketHandler::Pong(PacketComm& packet, string &response, Agent *agent)
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

        int32_t PacketHandler::Test(PacketComm& packet, string &response, Agent *agent)
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
                    sresponse = to_label("TestMET", (currentmjd() - agent->cinfo->node.utcstart)) + to_label(" Test_Id", last_test_id);
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
            sresponse += to_label("TestMET", (currentmjd() - agent->cinfo->node.utcstart)) + to_label(" Test_Id", test_id);
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

            response = sresponse;
            tests[test_id].last_packet_id = packet_id;
            last_test_id = test_id;
            return iretn;
        }

        int32_t PacketHandler::Response(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint16_t header_size;
            uint16_t chunk_size;
            uint16_t chunk_id;
            uint16_t chunks;
            filestruc file;


            PacketComm::ResponseHeader header;
            header_size = COSMOS_SIZEOF(PacketComm::ResponseHeader);
            chunk_size = packet.data.size() - header_size;
            memcpy(&header, packet.data.data(), header_size);
            chunk_id = header.chunk_id;
            chunks = header.chunks;
            file = data_name_struc(NodeData::lookup_node_id_name(packet.header.orig), "temp", "main", header.met, data_name(agent->cinfo->node.utcstart + header.met, "gresp", NodeData::lookup_node_id_name(packet.header.orig), "main", to_unsigned(header.response_id)));

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
                    iretn = fseek(tf, chunk_id*chunk_size, SEEK_SET);
                    if (iretn >= 0)
                    {
                        iretn = fwrite(packet.data.data()+header_size, chunk_size, 1, tf);
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
                    response = "chunks=" + to_unsigned(chunks) + " chunk_id=" + to_unsigned(chunk_id) + " chunk_size=" + to_unsigned(chunk_size) + " data=" + to_hex_string(packet.data, true);

                    if (data_isfile(file.path, chunks*chunk_size))
                    {
                        iretn = data_move(file, "incoming", false);
                        response += " | Response fully received";
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

        int32_t PacketHandler::DecodeBeacon(PacketComm& packet, string &response, Agent* agent)
        {
            Beacon beacon;
            beacon.Init();
            int32_t iretn = beacon.Decode(packet.data, agent->cinfo);
//            printf("Beacon Type=%u Size=%lu %d ", packet.data[0], packet.data.size(), iretn);
            if (iretn < 0)
            {
                return iretn;
            }

            // Log beacon
            string orig_node = agent->nodeData.lookup_node_id_name(packet.header.orig);
            if (!orig_node.empty())
            {
                string jstr;
                beacon.EncodeJson(beacon.type, agent->cinfo, jstr);
                log_write(orig_node, "beacon", agent->get_timeStart(), "", "beacon", jstr, "incoming");
            }

            return 0;
        }

        // Incoming Commands
        int32_t PacketHandler::Reset(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
//            memcpy(&verification_check, &packet.data[0], 4);
            verification_check = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->check_verification(verification_check);
            if (iretn < 0)
            {
                return iretn;
            }
            uint16_t seconds;
            seconds = uint16from(&packet.data[4], ByteOrder::LITTLEENDIAN);

            response = "Resetting Power in " + to_unsigned(seconds) + " seconds";

            // Command the EPS
//            packet.header.type = PacketComm::TypeId::CommandEpsReset;
//            packet.data.resize(2);
//            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
//            agent->push_unwrapped(agent->channel_number("EPS"), packet);
            iretn = QueueEpsReset(seconds, agent);
            return iretn;
        }

        int32_t PacketHandler::Reboot(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
//            memcpy(&verification_check, &packet.data[0], 4);
            verification_check = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->check_verification(verification_check);
            if (iretn < 0)
            {
                return iretn;
            }
            data_execute("reboot");
            return iretn;
        }

        int32_t PacketHandler::SendBeacon(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint8_t count = packet.data[1];
            Beacon beacon;
            vector<uint8_t> bytes;
            beacon.Init();
            beacon.EncodeBinary((Beacon::TypeId)packet.data[0], agent->cinfo, bytes);
            packet.header.type = PacketComm::TypeId::DataBeacon;
            packet.header.dest = packet.header.orig;
            packet.header.orig = agent->nodeId;
            packet.data.clear();
            packet.data.insert(packet.data.end(), bytes.begin(), bytes.end());
            for (uint16_t i=0; i<count; ++i)
            {
                iretn = agent->push_unwrapped(packet.header.radio, packet);
            }
            return iretn;
        }

        int32_t PacketHandler::ClearQueue(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
            memcpy(&verification_check, &packet.data[0], 4);
            iretn = agent->check_verification(verification_check);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = agent->clear_channel(packet.data[0]);
            return iretn;
        }

        int32_t PacketHandler::ExternalCommand(PacketComm& packet, string &response, Agent* agent)
        {
            // Run command, return response
            string eresponse;
            int32_t iretn = data_execute(string(packet.data.begin()+4, packet.data.end()), response);
            uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_response(packet.header.radio, packet.header.orig, response_id, string(response.begin(), response.end()));
            response.clear();
            return iretn;
        }

        int32_t PacketHandler::TestRadio(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn = agent->push_unwrapped(packet.header.radio, packet);
            return iretn;
        }

        int32_t PacketHandler::ListDirectory(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            string node;
            node.insert(node.begin(), packet.data.begin()+6, packet.data.begin()+6+packet.data[5]);
            string agentname;
            agentname.insert(agentname.begin(), packet.data.begin()+node.size()+7, packet.data.begin()+node.size()+7+packet.data[5+node.size()]);

            response.clear();
            for (filestruc file : data_list_files(node, "outgoing", agentname))
            {
                response += file.name + " ";
            }
            uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_response(packet.header.radio, packet.header.orig, response_id, response);
            response.clear();
            return iretn;
        }

        //        int32_t PacketHandler::TransferNode(PacketComm &packet, string &response, Agent* agent)
        //        {
        //            int32_t iretn = agent->push_unwrapped(packet.header.radio, packet);
        //            return iretn;
        //        }

        //        int32_t PacketHandler::TransferRadio(PacketComm &packet, string &response, Agent* agent)
        //        {
        //            int32_t iretn = agent->push_unwrapped(packet.header.radio, packet);
        //            return iretn;
        //        }

        //        int32_t PacketHandler::TransferList(PacketComm &packet, string &response, Agent* agent)
        //        {
        //            int32_t iretn = agent->push_unwrapped(packet.header.radio, packet);
        //            return iretn;
        //        }

        int32_t PacketHandler::InternalRequest(PacketComm& packet, string &response, Agent* agent)
        {
            // Run request, return response
            string erequest = string(packet.data.begin()+4, packet.data.end());
            int32_t iretn = agent->process_request(erequest, response);
            printf("InternalRequest: %s Response: %s\n", erequest.c_str(), response.c_str());
            fflush(stdout);
            uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_response(packet.header.radio, packet.header.orig, response_id, response);
            return iretn;
        }

        int32_t PacketHandler::Ping(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;

            // response.clear();
            // response.insert(response.begin(), packet.data.begin()+4, packet.data.end());
            packet.header.type = PacketComm::TypeId::DataPong;
            NodeData::NODE_ID_TYPE temp = packet.header.dest;
            packet.header.dest = packet.header.orig;
            packet.header.orig = temp;
            iretn = agent->push_unwrapped(packet.header.radio, packet);
            return iretn;
        }

        int32_t PacketHandler::SetTime(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            double mjd = doublefrom(packet.data.data(), ByteOrder::LITTLEENDIAN);
            double delta = set_local_clock(mjd);
            response = to_label("Delta Seconds", delta);

//            packet.header.type = PacketComm::TypeId::CommandEpsSetTime;
//            packet.data.resize(8);
//            doubleto(mjd, &packet.data[0], ByteOrder::LITTLEENDIAN);
//            iretn = agent->push_unwrapped(agent->channel_number("EPS"), packet);
            iretn = QueueEpsSetTime(mjd, agent);
            return iretn;
        }

        int32_t PacketHandler::GetTimeHuman(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            response = mjd2iso8601(currentmjd());
            response += " " + to_label("MET", currentmjd() - agent->cinfo->node.utcstart);
            iretn = agent->push_response(packet.header.radio, packet.header.orig, centisec(), response);
            return iretn;
        }

        int32_t PacketHandler::GetTimeBinary(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            packet.header.type = PacketComm::TypeId::DataTime;
            packet.header.dest = packet.header.orig;
            packet.header.orig = agent->nodeId;
            packet.data.resize(16);
            double mjd = currentmjd();
            memcpy(&packet.data[0], &mjd, 8);
            double met = currentmjd() - agent->cinfo->node.utcstart;
            memcpy(&packet.data[8], &met, 8);
            iretn = agent->push_unwrapped(packet.header.radio, packet);
            return iretn;
        }

        int32_t PacketHandler::AdcsResponse(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint16_t header_size;
            uint16_t chunk_size;
            uint16_t chunk_id;
            uint16_t chunks;
            filestruc file;

            iretn = agent->push_unwrapped(agent->channel_number("ADCS"), packet);

            PacketComm::AdcsResponseHeader header;
            header_size = COSMOS_SIZEOF(PacketComm::AdcsResponseHeader);
            chunk_size = packet.data.size() - header_size;
            memcpy(&header, packet.data.data(), header_size);
            chunk_id = header.chunk_id;
            chunks = header.chunks;
            printf("ADCSResp [%u/%u] %u ", chunk_id, chunks, packet.data[header_size]);
            file = data_name_struc(NodeData::lookup_node_id_name(packet.header.orig), "temp", "adcs", header.met, data_name(agent->cinfo->node.utcstart + header.met, "aresp", NodeData::lookup_node_id_name(packet.header.orig), "adcs", to_unsigned(header.command)));
            // Rebuild response with chunks
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
                    iretn = fseek(tf, chunk_id*chunk_size, SEEK_SET);
                    if (iretn >= 0)
                    {
                        iretn = fwrite(packet.data.data()+header_size, chunk_size, 1, tf);
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
                    response = "chunks=" + std::to_string(chunks) + " chunk_id=" + std::to_string(chunk_id) + "chunk_size=" + std::to_string(chunk_size);

                    // Check if all chunks received
                    if (data_isfile(file.path, chunks*chunk_size))
                    {
                        iretn = data_move(file, "incoming", false);
                        response += " | AdcsResponse fully received";
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

        int32_t PacketHandler::AdcsForward(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->push_unwrapped(agent->channel_number("ADCS"), packet);
            return iretn;
        }

        int32_t PacketHandler::EpsResponse(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint16_t header_size;
            uint16_t chunk_size;
            uint16_t chunk_id;
            uint16_t chunks;
            filestruc file;

            iretn = agent->push_unwrapped(agent->channel_number("EPS"), packet);

            PacketComm::EpsResponseHeader header;
            header_size = COSMOS_SIZEOF(PacketComm::EpsResponseHeader);
            chunk_size = packet.data.size() - header_size;
            memcpy(&header, packet.data.data(), header_size);
            chunk_id = header.chunk_id;
            chunks = header.chunks;
            printf("EPSResp [%u/%u] %u ", chunk_id, chunks, packet.data[header_size]);
            file = data_name_struc(NodeData::lookup_node_id_name(packet.header.orig), "temp", "eps", header.met, data_name(agent->cinfo->node.utcstart + header.met, "eresp", NodeData::lookup_node_id_name(packet.header.orig), "eps", to_unsigned(header.sbid)));
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
                    iretn = fseek(tf, chunk_id*chunk_size, SEEK_SET);
                    if (iretn >= 0)
                    {
                        iretn = fwrite(packet.data.data()+header_size, chunk_size, 1, tf);
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
                    response = "chunks=" + std::to_string(chunks) + " chunk_id=" + std::to_string(chunk_id) + "chunk_size=" + std::to_string(chunk_size);

                    // Check if all chunks received
                    if (data_isfile(file.path, chunks*chunk_size))
                    {
                        iretn = data_move(file, "incoming", false);
                        response += " | EpsResponse fully received";
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

        int32_t PacketHandler::EpsForward(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->push_unwrapped(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::ExecForward(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->push_unwrapped(agent->channel_number("EXEC"), packet);
            return iretn;
        }

        int32_t PacketHandler::FileForward(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->push_unwrapped(agent->channel_number("FILE"), packet);
            return iretn;
        }


        // Queue Packetcomm packets

        int32_t PacketHandler::QueueReset(uint16_t seconds, uint32_t verification_check, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandReset;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(6);
            uint32to(verification_check, &packet.data[0], ByteOrder::LITTLEENDIAN);
            uint16to(seconds, &packet.data[4], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueReboot(uint32_t verification_check, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandReboot;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(6);
            uint32to(verification_check, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueSendBeacon(uint8_t btype, uint8_t bcount, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandSendBeacon;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(2);
            packet.data[0] = btype;
            packet.data[1] = bcount;
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsCommunicate(uint8_t unit, uint8_t command, uint16_t rcount, vector<uint8_t> data, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm::CommunicateHeader header;

            header.unit = unit;
            header.command = command;
            header.responsecount = rcount;

            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsCommunicate;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(sizeof(header));
            memcpy(packet.data.data(), &header, sizeof(header));
            packet.data.insert(packet.data.end(), data.begin(), data.end());
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsSwitchName(string name, uint16_t seconds, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSwitchName;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(2);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            packet.data.insert(packet.data.end(), name.begin(), name.end());
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            printf("QueueEpsSwitchName: %s %d %u\n", name.c_str(), iretn, agent->channel_size(channel));
            return iretn;
        }

        int32_t PacketHandler::QueueEpsSwitchNumber(uint16_t number, uint16_t seconds, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSwitchName;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(4);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            uint16to(number, &packet.data[2], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsReset(uint16_t seconds, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsReset;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(2);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsState(uint8_t state, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsState;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(1);
            packet.data[0] = state;
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsWatchdog(uint16_t seconds, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsWatchdog;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(2);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsSetTime(double mjd, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSetTime;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(8);
            doubleto(mjd, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsMinimumPower(Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsMinimumPower;
            packet.header.orig = orig;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            iretn = agent->push_unwrapped(agent->channel_number(channel), packet);
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
