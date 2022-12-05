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
            add_func(PacketComm::TypeId::CommandExternalTask, ExternalTask);
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
            add_func(PacketComm::TypeId::CommandSetOpsMode, ExecForward);
            add_func(PacketComm::TypeId::CommandAdcsCommunicate, AdcsForward);
            add_func(PacketComm::TypeId::CommandAdcsState, AdcsForward);
            add_func(PacketComm::TypeId::CommandAdcsGetAdcsState, AdcsForward);
            add_func(PacketComm::TypeId::CommandAdcsOrbitParameters, AdcsForward);
            add_func(PacketComm::TypeId::CommandEpsCommunicate, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsSwitchName, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsSwitchNumber, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsReset, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsState, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsWatchdog, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsSetTime, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsMinimumPower, EpsForward);
            add_func(PacketComm::TypeId::CommandEpsSwitchNames, EpsForward);
            add_func(PacketComm::TypeId::CommandExecLoadCommand, ExecForward);
            add_func(PacketComm::TypeId::CommandExecAddCommand, ExecForward);

            // Telemetry
            add_func(PacketComm::TypeId::DataBeacon, DecodeBeacon);
            add_func(PacketComm::TypeId::DataPong, DecodePong);
            add_func(PacketComm::TypeId::DataAdcsResponse, DecodeAdcsResponse);
            add_func(PacketComm::TypeId::DataEpsResponse, DecodeEpsResponse);
            add_func(PacketComm::TypeId::DataResponse, DecodeResponse);
            add_func(PacketComm::TypeId::DataTest, DecodeTest);
            add_func(PacketComm::TypeId::DataTime, DecodeTime);

            return 0;
        }

        /**
         * Create a response type packet to be addressed to the
         * packet id contained within the addressee packet
         */
        vector<PacketComm> PacketHandler::create_response_packets(uint32_t response_id, uint16_t data_size, const vector<uint8_t> &response)
        {
            vector<PacketComm> packets;
            if (response.size())
            {
                PacketComm::ResponseHeader header;
                header.response_id = response_id;
                header.deci = decisec();
                PacketComm packet;
                packet.header.type = PacketComm::TypeId::DataResponse;
                uint8_t chunk_size = (data_size-COSMOS_SIZEOF(PacketComm::ResponseHeader));
                if (response.size() / chunk_size > 254)
                {
                    header.chunks = 255;
                }
                else
                {
                    header.chunks = (response.size() - 1) / chunk_size + 1;
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

        int32_t PacketHandler::DecodeBeacon(PacketComm& packet, string &response, Agent* agent)
        {
            Beacon beacon;
            beacon.Init();
            int32_t iretn = beacon.Decode(packet.data, agent->cinfo);
            if (iretn < 0)
            {
                return iretn;
            }

            // Create Response
            beacon.EncodeJson(beacon.type, agent->cinfo, response);

            // Log beacon
            string orig_node = agent->nodeData.lookup_node_id_name(packet.header.orig);
            if (!orig_node.empty())
            {
                log_write(orig_node, "beacon", agent->get_timeStart(), "", "beacon", response, "incoming");
            }

            return 0;
        }

        int32_t PacketHandler::DecodePong(PacketComm& packet, string &response, Agent *agent)
        {
            int32_t iretn = 0;
            static uint32_t last_response_id = 0;
            if (packet.data.size() > 4)
            {
                uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
                uint16_t pong_size = packet.data.size() - 4;
                response = to_unsigned(response_id) + " ";
                response.insert(response.end(), packet.data.begin()+4, packet.data.end());
                filestruc file = data_name_struc(agent->nodeName, "temp", agent->agentName, 0., "pong_"+to_unsigned(response_id));
                if (file.path.size())
                {
                    FILE *tf;
                    tf = fopen(file.path.c_str(), "a");
                    if (tf != nullptr)
                    {
                        iretn = fwrite(packet.data.data()+4, pong_size, 1, tf);
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
                        response = "Error: " + cosmos_error_string(iretn);
                    }
                }
                else
                {
                    iretn = GENERAL_ERROR_NAME;
                    response = "Error: " + cosmos_error_string(iretn);
                }
            }
            else
            {
                iretn = GENERAL_ERROR_BAD_SIZE;
                response = "Error: " + cosmos_error_string(iretn);
            }
            return iretn;
        }

        int32_t PacketHandler::DecodeEpsResponse(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;

            if (packet.data.size() > COSMOS_SIZEOF(PacketComm::EpsResponseHeader))
            {
                uint16_t header_size;
                uint16_t chunk_size;
                uint16_t this_chunk_size;
                uint16_t chunk_id;
                uint16_t chunks;
                filestruc file;

                iretn = agent->channel_push(agent->channel_number("EPS"), packet);

                PacketComm::EpsResponseHeader header;
                header_size = sizeof(header);
                memcpy(&header, packet.data.data(), header_size);
                this_chunk_size = packet.data.size() - header_size;
                chunk_size = agent->channel_datasize(packet.header.radio) - header_size;
                chunk_id = header.chunk_id;
                chunks = header.chunks;
                file = data_name_struc(NodeData::lookup_node_id_name(packet.header.orig), "temp", "eps", decisec2mjd(header.deci), data_name(decisec2mjd(header.deci), "eresp", NodeData::lookup_node_id_name(packet.header.orig), "eps", to_unsigned(header.unit)+"_"+to_unsigned(header.command)));

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
                            iretn = fwrite(packet.data.data()+header_size, this_chunk_size, 1, tf);
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
//                        response = "chunks=" + std::to_string(chunks) + " chunk_id=" + std::to_string(chunk_id) + "chunk_size=" + std::to_string(this_chunk_size);
                        response = "[" + to_unsigned(chunk_id) + ":" + to_unsigned(chunks) + "]" + to_hex_string(packet.data, false, header_size);

                        // Check if all chunks received
                        if (chunk_id == chunks - 1 && data_isfile(file.path, chunk_id*chunk_size+this_chunk_size))
                        {
                            iretn = data_move(file, "incoming", false);
                            response += " Complete";
                        }
                    }
                    else
                    {
                        iretn = GENERAL_ERROR_BAD_FD;
                        response = "Error: " + cosmos_error_string(iretn);
                    }
                }
                else
                {
                    iretn = GENERAL_ERROR_NAME;
                    response = "Error: " + cosmos_error_string(iretn);
                }
            }
            else
            {
                iretn = GENERAL_ERROR_BAD_SIZE;
                response = "Error: " + cosmos_error_string(iretn);
            }

            return iretn;
        }

        int32_t PacketHandler::DecodeAdcsResponse(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;

            if (packet.data.size() > COSMOS_SIZEOF(PacketComm::AdcsResponseHeader))
            {
                uint16_t header_size;
                uint16_t chunk_size;
                uint16_t this_chunk_size;
                uint16_t chunk_id;
                uint16_t chunks;
                filestruc file;

                iretn = agent->channel_push(agent->channel_number("ADCS"), packet);

                PacketComm::AdcsResponseHeader header;
                header_size = sizeof(header);
                memcpy(&header, packet.data.data(), header_size);
                this_chunk_size = packet.data.size() - header_size;
                chunk_size = agent->channel_datasize(packet.header.radio) - header_size;
                chunk_id = header.chunk_id;
                chunks = header.chunks;
                file = data_name_struc(NodeData::lookup_node_id_name(packet.header.orig), "temp", "adcs", decisec2mjd(header.deci), data_name(decisec2mjd(header.deci), "aresp", NodeData::lookup_node_id_name(packet.header.orig), "adcs", to_unsigned(header.command)));

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
                            iretn = fwrite(packet.data.data()+header_size, this_chunk_size, 1, tf);
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
//                        response = "chunks=" + std::to_string(chunks) + " chunk_id=" + std::to_string(chunk_id) + "chunk_size=" + std::to_string(this_chunk_size);
                        response = "[" + to_unsigned(chunk_id) + ":" + to_unsigned(chunks) + ":" + to_unsigned(this_chunk_size) + "]" + to_hex_string(packet.data, false, header_size);

                        // Check if all chunks received
                        if (chunk_id == chunks - 1 && data_isfile(file.path, chunk_id*chunk_size+this_chunk_size))
                        {
                            iretn = data_move(file, "incoming", false);
                            response += " Complete";
                        }
                    }
                    else
                    {
                        iretn = GENERAL_ERROR_BAD_FD;
                        response = "Error: " + cosmos_error_string(iretn);
                    }
                }
                else
                {
                    iretn = GENERAL_ERROR_NAME;
                    response = "Error: " + cosmos_error_string(iretn);
                }
            }
            else
            {
                iretn = GENERAL_ERROR_BAD_SIZE;
                response = "Error: " + cosmos_error_string(iretn);
            }

            return iretn;
        }

        int32_t PacketHandler::DecodeResponse(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;

            if (packet.data.size() > COSMOS_SIZEOF(PacketComm::ResponseHeader))
            {
                uint16_t header_size;
                uint16_t chunk_size;
                uint16_t this_chunk_size;
                uint16_t chunk_id;
                uint16_t chunks;
                filestruc file;


                PacketComm::ResponseHeader header;
                header_size = COSMOS_SIZEOF(PacketComm::ResponseHeader);
                memcpy(&header, packet.data.data(), header_size);
                this_chunk_size = packet.data.size() - header_size;
                chunk_size = agent->channel_datasize(packet.header.radio) - header_size;
                chunk_id = header.chunk_id;
                chunks = header.chunks;
                file = data_name_struc(NodeData::lookup_node_id_name(packet.header.orig), "temp", "main", decisec2mjd(header.deci), data_name(decisec2mjd(header.deci), "gresp", NodeData::lookup_node_id_name(packet.header.orig), "main", to_unsigned(header.response_id)));

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
                            iretn = fwrite(packet.data.data()+header_size, this_chunk_size, 1, tf);
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
                        response = "chunks=" + to_unsigned(chunks) + " chunk_id=" + to_unsigned(chunk_id) + " chunk_size=" + to_unsigned(this_chunk_size) + " data=" + to_hex_string(packet.data, true, header_size);

                        if (chunk_id == chunks - 1 && data_isfile(file.path, chunk_id*chunk_size+this_chunk_size))
                        {
                            iretn = data_move(file, "incoming", false);
                            response += " Complete";
                        }
                    }
                    else
                    {
                        iretn = GENERAL_ERROR_BAD_FD;
                        response = "Error: " + cosmos_error_string(iretn);
                    }
                }
                else
                {
                    iretn = GENERAL_ERROR_NAME;
                    response = "Error: " + cosmos_error_string(iretn);
                }
            }
            else
            {
                iretn = GENERAL_ERROR_BAD_SIZE;
                response = "Error: " + cosmos_error_string(iretn);
            }
            return iretn;
        }

        int32_t PacketHandler::DecodeTest(PacketComm& packet, string &response, Agent *agent)
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
            response.clear();

            if (packet.data.size() > COSMOS_SIZEOF(PacketComm::TestHeader))
            {
                PacketComm::TestHeader header;
                uint16_t header_size = COSMOS_SIZEOF(PacketComm::TestHeader);
                memcpy(&header, packet.data.data(), header_size);
                uint16_t data_size = packet.data.size() - header_size;

                if (tests.find(header.test_id) == tests.end())
                {
                    // New Test
                    if (tests.find(last_test_id) != tests.end())
                    {
                        // Finish off existing test
                        response += to_label("MET", (currentmjd() - agent->cinfo->node.utcstart));
                        response += to_label(" Test_Id", last_test_id);
                        response +=  to_label(" Packet_Id", tests[last_test_id].last_packet_id);
                        response += " Good: " + to_unsigned(tests[last_test_id].good_count);
                        response += " Skip: " + to_unsigned(tests[last_test_id].skip_count);
                        response += " Size: " + to_unsigned(tests[last_test_id].size_count);
                        response += " Crc: " + to_unsigned(tests[last_test_id].crc_count);
                        response += to_label(" Bytes", tests[last_test_id].total_bytes);
                        response += to_label(" Count", tests[last_test_id].total_count);
                        response += to_label(" Seconds", tests[last_test_id].et.split());
                        response += to_label(" Speed", tests[last_test_id].total_bytes / tests[last_test_id].et.split());
                        response += " Abort: \n";
                    }
                    tests[header.test_id].path = data_name_path(agent->nodeName, "incoming", agent->agentName, 0., "test_"+to_unsigned(header.test_id));
                    tests[header.test_id].et.reset();
                }

                tests[header.test_id].total_count = tests[header.test_id].good_count + tests[header.test_id].crc_count + tests[header.test_id].size_count;
                if (header.packet_id - tests[header.test_id].last_packet_id > 1 && header.packet_id != ((uint32_t)-1))
                {
                    tests[header.test_id].skip_count += (header.packet_id - tests[header.test_id].last_packet_id) - 1;
                }

                uint16_t crccalc = calc_crc.calc(&packet.data[0], packet.data.size()-2);
                uint16_t crcdata = 256 * packet.data[packet.data.size()-1] + packet.data[packet.data.size()-2];
                if (crccalc != crcdata)
                {
                    ++tests[header.test_id].crc_count;
                }
                else
                {
                    FILE *tf = fopen(tests[header.test_id].path.c_str(), "a");
                    iretn = fwrite(packet.data.data(), packet.data.size(), 1, tf);
                    fclose(tf);
                    tests[header.test_id].total_bytes += data_size;
                    ++tests[header.test_id].good_count;
                }
                tests[header.test_id].total_count = tests[header.test_id].good_count + tests[header.test_id].crc_count + tests[header.test_id].size_count + tests[header.test_id].skip_count;
                response += to_label("MET", (currentmjd() - agent->cinfo->node.utcstart));
                response += to_label(" Test_Id", header.test_id);
                if (header.packet_id == ((uint32_t)-1))
                {
                    response +=  to_label(" Packet_Id", tests[header.test_id].last_packet_id+1);
                }
                else
                {
                    response +=  to_label(" Packet_Id", header.packet_id);
                }
                response += " Good: " + to_unsigned(tests[header.test_id].good_count);
                response += " Skip: " + to_unsigned(tests[header.test_id].skip_count);
                response += " Size: " + to_unsigned(tests[header.test_id].size_count);
                response += " Crc: " + to_unsigned(tests[header.test_id].crc_count);
                response += to_label(" Bytes", tests[header.test_id].total_bytes);
                response += to_label(" Count", tests[header.test_id].total_count);
                response += to_label(" Seconds", tests[header.test_id].et.split());
                response += to_label(" Speed", tests[header.test_id].total_bytes / tests[header.test_id].et.split());
                if (header.packet_id == ((uint32_t)-1))
                {
                    response += " Complete: ";
                    tests.erase(header.test_id);
                }
                else if (last_test_id < header.test_id)
                {
                    response += " Start: ";
                }

                tests[header.test_id].last_packet_id = header.packet_id;
                last_test_id = header.test_id;
            }
            else
            {
                iretn = GENERAL_ERROR_BAD_SIZE;
                response = "Error: " + cosmos_error_string(iretn);
            }

            return iretn;
        }

        int32_t PacketHandler::DecodeTime(PacketComm& packet, string &response, Agent *agent)
        {
            int32_t iretn = 0;
            if (packet.data.size() == 12)
            {
                double mjd = doublefrom(&packet.data[0], ByteOrder::LITTLEENDIAN);
                double met = doublefrom(&packet.data[8], ByteOrder::LITTLEENDIAN);
                response = to_mjd(mjd) + " " + utc2iso8601(mjd) + " " + to_floating(met);
            }
            else
            {
                iretn = GENERAL_ERROR_BAD_SIZE;
                response = "Error: " + cosmos_error_string(iretn);
            }

            return iretn;
        }

        // Incoming Commands
        int32_t PacketHandler::Reset(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
            verification_check = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->check_verification(verification_check);
            if (iretn < 0)
            {
                response = "Verification Failed: [" + to_hex(verification_check) + ":" + to_hex(agent->get_verification()) + "] " + cosmos_error_string(iretn);
                return iretn;
            }
            uint16_t seconds;
            seconds = uint16from(&packet.data[4], ByteOrder::LITTLEENDIAN);

            response = "Resetting Power in " + to_unsigned(seconds) + " seconds";

            iretn = QueueEpsReset(seconds, agent);
            return iretn;
        }

        int32_t PacketHandler::Reboot(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
            verification_check = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->check_verification(verification_check);
            if (iretn < 0)
            {
                response = "Verification Failed: [" + to_hex(verification_check) + ":" + to_hex(agent->get_verification()) + "] " + cosmos_error_string(iretn);
                return iretn;
            }
            response = "Rebooting";
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
                iretn = agent->channel_push(packet.header.radio, packet);
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
            iretn = agent->channel_clear(packet.data[0]);
            return iretn;
        }

        int32_t PacketHandler::ExternalCommand(PacketComm& packet, string &response, Agent* agent)
        {
            // Run command, return response
            int32_t iretn = data_execute(string(packet.data.begin()+4, packet.data.end()), response);
            uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_response(packet.header.radio, packet.header.orig, response_id, string(response.begin(), response.end()));
            response.clear();
            return iretn;
        }

        int32_t PacketHandler::ExternalTask(PacketComm& packet, string &response, Agent* agent)
        {
            // Run command, return response
            int32_t iretn = agent->task_add(string(packet.data.begin()+4, packet.data.end()));
            response = "Running: " + agent->task_command(iretn) + " in " + agent->task_path(iretn) + " #" + to_unsigned(agent->task_size());
            uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_response(packet.header.radio, packet.header.orig, response_id, string(response.begin(), response.end()));
            response.clear();
            return iretn;
        }

        int32_t PacketHandler::TestRadio(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn = agent->channel_push(packet.header.radio, packet);
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

        int32_t PacketHandler::InternalRequest(PacketComm& packet, string &response, Agent* agent)
        {
            // Run request, return response
            string erequest = string(packet.data.begin()+4, packet.data.end());
            int32_t iretn = agent->process_request(erequest, response);
            uint32_t response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->push_response(packet.header.radio, packet.header.orig, response_id, response);
            return iretn;
        }

        int32_t PacketHandler::Ping(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;

            packet.header.type = PacketComm::TypeId::DataPong;
            NodeData::NODE_ID_TYPE temp = packet.header.dest;
            packet.header.dest = packet.header.orig;
            packet.header.orig = temp;
            iretn = agent->channel_push(packet.header.radio, packet);
            return iretn;
        }

        int32_t PacketHandler::SetTime(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            double mjd = doublefrom(packet.data.data(), ByteOrder::LITTLEENDIAN);
            double delta = set_local_clock(mjd, packet.data[8]);
            response = to_label("Delta Seconds", delta);

            iretn = QueueEpsSetTime(currentmjd(), agent);
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
            iretn = agent->channel_push(packet.header.radio, packet);
            return iretn;
        }

        int32_t PacketHandler::AdcsForward(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->channel_push(agent->channel_number("ADCS"), packet);
            return iretn;
        }

        int32_t PacketHandler::EpsForward(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::ExecForward(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->channel_push(agent->channel_number("EXEC"), packet);
            return iretn;
        }

        int32_t PacketHandler::FileForward(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->channel_push(agent->channel_number("FILE"), packet);
            return iretn;
        }


        // Queue Packetcomm packets

        int32_t PacketHandler::QueueReset(uint16_t seconds, uint32_t verification_check, Agent* agent, string channel, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandReset;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(6);
            uint32to(verification_check, &packet.data[0], ByteOrder::LITTLEENDIAN);
            uint16to(seconds, &packet.data[4], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueReboot(uint32_t verification_check, Agent* agent, string channel, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandReboot;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(6);
            uint32to(verification_check, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueSendBeacon(uint8_t btype, uint8_t bcount, Agent* agent, string channel, NodeData::NODE_ID_TYPE orig, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandSendBeacon;
            packet.header.orig = dest;
            packet.header.dest = orig;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(2);
            packet.data[0] = btype;
            packet.data[1] = bcount;
            iretn = agent->channel_push(agent->channel_number(channel), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueAdcsCommunicate(uint8_t unit, uint8_t command, uint16_t rcount, vector<uint8_t> data, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm::CommunicateHeader header;

            header.unit = unit;
            header.command = command;
            header.responsecount = rcount;

            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandAdcsCommunicate;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(sizeof(header));
            memcpy(packet.data.data(), &header, sizeof(header));
            packet.data.insert(packet.data.end(), data.begin(), data.end());
            iretn = agent->channel_push(agent->channel_number("ADCS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueAdcsState(uint8_t state, vector<uint8_t>  data, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
//            PacketComm::StateHeader header;

//            header.unit = unit;
//            header.command = command;
//            header.responsecount = rcount;

            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandAdcsState;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(1);
            packet.data[0] = state;
//            packet.data.resize(sizeof(header));
//            memcpy(packet.data.data(), &header, sizeof(header));
            packet.data.insert(packet.data.end(), data.begin(), data.end());
            iretn = agent->channel_push(agent->channel_number("ADCS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsCommunicate(uint8_t unit, uint8_t command, uint16_t rcount, vector<uint8_t> data, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm::CommunicateHeader header;

            header.unit = unit;
            header.command = command;
            header.responsecount = rcount;

            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsCommunicate;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(sizeof(header));
            memcpy(packet.data.data(), &header, sizeof(header));
            packet.data.insert(packet.data.end(), data.begin(), data.end());
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsSwitchName(string name, uint16_t seconds, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSwitchName;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(2);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            packet.data.insert(packet.data.end(), name.begin(), name.end());
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsSwitchNames(vector<string> names, vector<uint16_t> seconds, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            if (names.size() != seconds.size())
            {
                return GENERAL_ERROR_MISMATCH;
            }

            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSwitchNames;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(1);
            packet.data[0] = static_cast<uint8_t>(names.size());
            for (uint16_t i=0; i<names.size(); ++i)
            {
                packet.data.resize(packet.data.size() + 3);
                uint16to(seconds[i], &packet.data[packet.data.size() - 3], ByteOrder::LITTLEENDIAN);
                packet.data[packet.data.size()-1] = names[i].size();
                packet.data.insert(packet.data.end(), names[i].begin(), names[i].end());
            }
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsSwitchNumber(uint16_t number, uint16_t seconds, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSwitchName;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(4);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            uint16to(number, &packet.data[2], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsReset(uint16_t seconds, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsReset;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(2);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsState(uint8_t state, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsState;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(1);
            packet.data[0] = state;
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsWatchdog(uint16_t seconds, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsWatchdog;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(2);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsSetTime(double mjd, Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSetTime;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(8);
            doubleto(mjd, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsMinimumPower(Agent* agent, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsMinimumPower;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            iretn = agent->channel_push(agent->channel_number("EPS"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueTransferRadio(uint8_t use_radio, bool availability, Agent* agent, NodeData::NODE_ID_TYPE dest)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandTransferRadio;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = 0;
            packet.data.resize(2);
            packet.data[0] = use_radio;
            packet.data[1] = availability;
            iretn = agent->channel_push(agent->channel_number("FILE"), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueTestRadio(uint8_t start, uint8_t step, uint8_t stop, uint32_t count, Agent* agent, string testradio, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandTestRadio;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(11);
            packet.data[0] = start;
            packet.data[1] = step;
            packet.data[2] = stop;
            uint32to(decisec(), &packet.data[3], ByteOrder::LITTLEENDIAN);
            uint32to(count, &packet.data[7], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(agent->channel_number(testradio), packet);
            return iretn;
        }

        int32_t PacketHandler::QueueSetTime(double mjd, int8_t direction , Agent* agent, string channel, NodeData::NODE_ID_TYPE dest, string radio)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandSetTime;
            packet.header.orig = agent->nodeId;
            packet.header.dest = dest;
            packet.header.radio = agent->channel_number(radio);
            packet.data.resize(9);
            doubleto(mjd, &packet.data[0], ByteOrder::LITTLEENDIAN);
            packet.data[8] = direction;
            iretn = agent->channel_push(agent->channel_number(channel), packet);
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
