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
            add_func(PacketComm::TypeId::CommandObcReset, Reset);
            add_func(PacketComm::TypeId::CommandObcReboot, Reboot);
            add_func(PacketComm::TypeId::CommandObcSendBeacon, SendBeacon);
            add_func(PacketComm::TypeId::CommandObcHalt, Reboot);
            add_func(PacketComm::TypeId::CommandObcInternalRequest, InternalRequest);
            add_func(PacketComm::TypeId::CommandObcPing, Ping);
            add_func(PacketComm::TypeId::CommandObcSetTime, SetTime);
            add_func(PacketComm::TypeId::CommandObcGetTimeHuman, GetTimeHuman);
            add_func(PacketComm::TypeId::CommandObcGetTimeBinary, GetTimeBinary);
            add_func(PacketComm::TypeId::CommandObcExternalCommand, ExternalCommand);
            add_func(PacketComm::TypeId::CommandObcExternalTask, ExternalTask);
            add_func(PacketComm::TypeId::CommandRadioTest, TestRadio);
            add_func(PacketComm::TypeId::CommandFileListDirectory, ListDirectory);
            add_func(PacketComm::TypeId::CommandFileTransferFile, FileForward);
            add_func(PacketComm::TypeId::CommandFileTransferNode, FileForward);
            add_func(PacketComm::TypeId::CommandFileTransferRadio, FileForward);
            add_func(PacketComm::TypeId::CommandFileTransferList, FileForward);
            add_func(PacketComm::TypeId::CommandExecClearQueue, ClearQueue);
            add_func(PacketComm::TypeId::CommandExecSetOpsMode, ExecForward);
            add_func(PacketComm::TypeId::CommandExecEnableChannel, EnableChannel);
            add_func(PacketComm::TypeId::CommandExecLoadCommand, ExecForward);
            add_func(PacketComm::TypeId::CommandExecAddCommand, ExecForward);
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
            add_func(PacketComm::TypeId::CommandCameraCapture, ImageForward);

            // Telemetry
            add_func(PacketComm::TypeId::DataObcBeacon, DecodeBeacon);
            add_func(PacketComm::TypeId::DataObcPong, DecodePong);
            add_func(PacketComm::TypeId::DataAdcsResponse, DecodeAdcsResponse);
            add_func(PacketComm::TypeId::DataEpsResponse, DecodeEpsResponse);
            add_func(PacketComm::TypeId::DataObcResponse, DecodeResponse);
            add_func(PacketComm::TypeId::DataRadioTest, DecodeTest);
            add_func(PacketComm::TypeId::DataObcTime, DecodeTime);

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
                packet.header.type = PacketComm::TypeId::DataObcResponse;
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
            FuncEntry &fentry = Funcs[(uint16_t)packet.header.type];
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
            Funcs[(uint16_t)type] = tentry;
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
            //            string orig_node = lookup_node_id_name(agent->cinfo, packet.header.nodeorig);
            //            if (!orig_node.empty())
            //            {
            //                log_write(orig_node, "beacon", agent->get_timeStart(), "", "beacon", response, "incoming");
            //            }

            return 0;
        }

        int32_t PacketHandler::DecodePong(PacketComm& packet, string &response, Agent *agent)
        {
            int32_t iretn = 0;
            static uint32_t last_response_id = 0;
            if (packet.data.size() > 4)
            {
                packet.response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
                uint16_t pong_size = packet.data.size() - 4;
                response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
                response += "Pong\n";
                response += to_unsigned(packet.response_id) + " ";
                response.insert(response.end(), packet.data.begin()+4, packet.data.end());
                filestruc file = data_name_struc(agent->cinfo->node.name, "temp", agent->cinfo->agent0.name, 0., "pong_"+to_unsigned(packet.response_id));
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
                        if (data_isfile(file.path) && last_response_id != packet.response_id)
                        {
                            iretn = data_move_file(file, "incoming", false);
                            last_response_id = packet.response_id;
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

                iretn = agent->channel_push(packet);

                PacketComm::EpsResponseHeader header;
                header_size = sizeof(header);
                memcpy(&header, packet.data.data(), header_size);
                this_chunk_size = packet.data.size() - header_size;
                chunk_size = agent->channel_datasize(packet.header.chanin) - header_size;
                chunk_id = header.chunk_id;
                chunks = header.chunks;
                file = data_name_struc(lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "temp", "eps", decisec2mjd(header.deci), data_name(decisec2mjd(header.deci), "eresp", lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "eps", to_unsigned(header.unit)+"_"+to_unsigned(header.command)));

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
                        response = "[" + to_unsigned(chunk_id) + ":" + to_unsigned(chunks) + "]" + to_hex_string(packet.data, false, header_size);

                        // Check if all chunks received
                        if (chunk_id == chunks - 1 && data_isfile(file.path, chunk_id*chunk_size+this_chunk_size))
                        {
                            iretn = data_move_file(file, "incoming", false);
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
                chunk_size = agent->channel_datasize(packet.header.chanin) - header_size;
                chunk_id = header.chunk_id;
                chunks = header.chunks;
                file = data_name_struc(lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "temp", "adcs", decisec2mjd(header.deci), data_name(decisec2mjd(header.deci), "aresp", lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "adcs", to_unsigned(header.command)));

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
                            iretn = data_move_file(file, "incoming", false);
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
                chunk_size = agent->channel_datasize(packet.header.chanin) - header_size;
                chunk_id = header.chunk_id;
                chunks = header.chunks;
                file = data_name_struc(lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "temp", "main", decisec2mjd(header.deci), data_name(decisec2mjd(header.deci), "gresp", lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "main", to_unsigned(header.response_id)));

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
                        response = "{\"Channel\":" + to_unsigned(header.source_id);
                        response += ",\"Chunks\":" + to_unsigned(chunks);
                        response += ",\"Chunk_id\":";
                        response += to_unsigned(chunk_id);
                        response += ",\"Chunk_size\":" + to_unsigned(this_chunk_size);
                        response += ",\"Data\":\"" + to_hex_string(packet.data, true, header_size) + "\"}";

                        if (chunk_id == chunks - 1 && data_isfile(file.path, chunk_id*chunk_size+this_chunk_size))
                        {
                            iretn = data_move_file(file, "incoming", false);
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
            static FILE *tf = nullptr;
            static CRC16 calc_crc;
            // static uint32_t last_test_id = 0;
            int32_t iretn=0;
            struct test_control
            {
                uint32_t test_id = 0;
                uint32_t total_bytes = 0;
                uint32_t total_count = 0;
                uint32_t good_count = 0;
                uint32_t repeat_count = 0;
                uint32_t crc_count = 0;
                uint32_t size_count = 0;
                uint32_t skip_count = 0;
                uint32_t packet_id = 0;
                ElapsedTime et;
                string path;
            };
            static test_control test;
            //            static map<uint32_t, test_control> tests;
            response.clear();

            if (packet.data.size() > COSMOS_SIZEOF(PacketComm::TestHeader))
            {
                uint16_t crccalc = calc_crc.calc(&packet.data[0], packet.data.size()-2);
                uint16_t crcdata = 256 * packet.data[packet.data.size()-1] + packet.data[packet.data.size()-2];
                if (crccalc != crcdata)
                {
                    ++test.crc_count;
                    return GENERAL_ERROR_CRC;
                }
                PacketComm::TestHeader header;
                uint16_t header_size = COSMOS_SIZEOF(PacketComm::TestHeader);
                memcpy(&header, packet.data.data(), header_size);
                uint16_t data_size = packet.data.size() - header_size;

                if (tf == nullptr)
                {
//                    test.path = data_name_path(lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "temp", agent->cinfo->agent0.name, 0., "test_"+to_unsigned(header.test_id));
                    test.path = data_base_path(lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "temp", agent->cinfo->agent0.name, data_name(currentmjd(), "test", lookup_node_id_name(agent->cinfo, packet.header.nodeorig), agent->cinfo->agent0.name));
                    tf = fopen(test.path.c_str(), "a");
                }

                if (header.test_id != test.test_id)
                {
                    if (tf != nullptr)
                    {
                        fclose(tf);
                        tf = nullptr;
                        log_move_file(test.path, string_replace(test.path, "temp", "incoming"), true);
                        test.path.clear();
                    }

                    test.good_count = 0;
                    test.crc_count = 0;
                    test.size_count = 0;
                    test.skip_count = 0;
                    test.packet_id = 0;
                    test.et.reset();
                    test.total_bytes = 0;
                }

                test.total_bytes += data_size;
                if (header.packet_id == ((uint32_t)-1))
                {
                    ++test.good_count;
                    response = "Complete: ";
                }
                else if (header.packet_id < test.packet_id)
                {
                    ++test.repeat_count;
                    response = "Repeat: ";
                }
                else if (header.packet_id > test.packet_id + 1)
                {
                    test.skip_count += (header.packet_id - test.packet_id) - 1;
                    ++test.good_count;
                    response = "Skip: ";
                }
                else
                {
                    ++test.good_count;
                    response = "Good: ";
                }

                test.total_count = test.good_count + test.crc_count + test.size_count + test.skip_count;
                response += to_label("MET", (currentmjd() - agent->cinfo->node.utcstart));
                response += to_label(" Test_Id", header.test_id);
                if (header.packet_id == ((uint32_t)-1))
                {
                    response +=  to_label(" Packet_Id", test.packet_id+1);
                }
                else
                {
                    response +=  to_label(" Packet_Id", header.packet_id);
                }
                response += " Good: " + to_unsigned(test.good_count);
                response += " Skip: " + to_unsigned(test.skip_count);
                response += " Size: " + to_unsigned(test.size_count);
                response += " Crc: " + to_unsigned(test.crc_count);
                response += to_label(" Bytes", test.total_bytes);
                response += to_label(" Count", test.total_count);
                response += to_label(" Seconds", test.et.split());
                response += to_label(" Speed", test.total_bytes / test.et.split());

                if (tf == nullptr)
                {
//                    test.path = data_name_path(lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "temp", agent->cinfo->agent0.name, 0., "test_"+to_unsigned(header.test_id));
                    test.path = data_base_path(lookup_node_id_name(agent->cinfo, packet.header.nodeorig), "temp", agent->cinfo->agent0.name, data_name(currentmjd(), "test", lookup_node_id_name(agent->cinfo, packet.header.nodeorig), agent->cinfo->agent0.name));
                    tf = fopen(test.path.c_str(), "a");
                    if (tf == nullptr)
                    {
                        test.path.clear();
                    }
                }

                if (tf != nullptr)
                {
                    iretn = fprintf(tf, "%s\n", response.c_str());
                    if (header.packet_id == ((uint32_t)-1))
                    {
                        fclose(tf);
                        tf = nullptr;
                        log_move_file(test.path, string_replace(test.path, "temp", "incoming"), true);
                        test.path.clear();
                    }
                }

                test.packet_id = header.packet_id;
                test.test_id = header.test_id;
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
            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            response += "Reset\n";
            if (iretn < 0)
            {
                response += "Verification Failed: [" + to_hex(verification_check) + ":" + to_hex(agent->get_verification()) + "] " + cosmos_error_string(iretn);
                return iretn;
            }
            uint16_t seconds = 10;
            if (packet.data.size() >5)
            {
                seconds = uint16from(&packet.data[4], ByteOrder::LITTLEENDIAN);
            }

            response +="Resetting Power in " + to_unsigned(seconds) + " seconds";

            iretn = QueueEpsReset(seconds, agent);
            return iretn;
        }

        int32_t PacketHandler::Reboot(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
            verification_check = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->check_verification(verification_check);
            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            response += "Reboot\n";
            if (iretn < 0)
            {
                response +="Verification Failed: [" + to_hex(verification_check) + ":" + to_hex(agent->get_verification()) + "] " + cosmos_error_string(iretn);
                return iretn;
            }
            uint16_t seconds = 0;
            if (packet.data.size() >5)
            {
                seconds = uint16from(&packet.data[4], ByteOrder::LITTLEENDIAN);
            }

            response +="Rebooting";
            data_execute("sleep " + to_unsigned(seconds) + ";" + "reboot");
            return iretn;
        }

        int32_t PacketHandler::Halt(PacketComm& packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            uint32_t verification_check;
            verification_check = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->check_verification(verification_check);
            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            response += "Halt\n";
            if (iretn < 0)
            {
                response +="Verification Failed: [" + to_hex(verification_check) + ":" + to_hex(agent->get_verification()) + "] " + cosmos_error_string(iretn);
                return iretn;
            }
            uint16_t seconds = 0;
            if (packet.data.size() >5)
            {
                seconds = uint16from(&packet.data[4], ByteOrder::LITTLEENDIAN);
            }

            response +="Halting";
            data_execute("halt " + to_unsigned(seconds) + ";" + "reboot");
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
            packet.header.type = PacketComm::TypeId::DataObcBeacon;
            packet.header.nodedest = packet.header.nodeorig;
            packet.header.nodeorig = agent->nodeId;
            packet.data.clear();
            packet.data.insert(packet.data.end(), bytes.begin(), bytes.end());
            for (uint16_t i=0; i<count; ++i)
            {
                iretn = agent->channel_push(packet.header.chanin, packet);
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
            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            // Run command, return response
            response += string(packet.data.begin()+4, packet.data.end()) + "\n";
            int32_t iretn = data_execute(string(packet.data.begin()+4, packet.data.end()), response);
            packet.response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
//            iretn = agent->push_response(packet.header.chanin, 0, packet.header.nodeorig, response_id, string(response.begin(), response.end()));
//            response.clear();
            return iretn;
        }

        int32_t PacketHandler::ExternalTask(PacketComm& packet, string &response, Agent* agent)
        {
            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            // Run command, return response
            string source = lookup_node_id_name(agent->cinfo, packet.header.nodeorig);
            int32_t iretn = agent->task_add(string(packet.data.begin()+4, packet.data.end()), source);
            response += "Running: " + agent->task_command(iretn) + " in " + agent->task_path(iretn) + " #" + to_unsigned(agent->task_size()) + "\n";
            packet.response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
//            iretn = agent->push_response(packet.header.chanin, 0, packet.header.nodeorig, response_id, string(response.begin(), response.end()));
//            response.clear();
            return iretn;
        }

        int32_t PacketHandler::TestRadio(PacketComm& packet, string &response, Agent* agent)
        {
            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            response += "Test Radio\n";
            int32_t iretn = agent->channel_push(packet.header.chanin, packet);
            return iretn;
        }

        int32_t PacketHandler::ListDirectory(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            string node;
            node.insert(node.begin(), packet.data.begin()+6, packet.data.begin()+6+packet.data[5]);
            string agentname;
            agentname.insert(agentname.begin(), packet.data.begin()+node.size()+7, packet.data.begin()+node.size()+7+packet.data[5+node.size()]);

            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            response += "List Directory " + node + " " + agentname + "\n";
            for (filestruc file : data_list_files(node, "outgoing", agentname))
            {
                response += file.name + " ";
            }
            packet.response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
//            iretn = agent->push_response(packet.header.chanin, 0, packet.header.nodeorig, response_id, response);
//            response.clear();
            return iretn;
        }

        int32_t PacketHandler::InternalRequest(PacketComm& packet, string &response, Agent* agent)
        {
            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            // Run request, return response
            string erequest = string(packet.data.begin()+4, packet.data.end());
            response += "Internal Request " + erequest + "\n";
            int32_t iretn = agent->process_request(erequest, response);
            packet.response_id = uint32from(&packet.data[0], ByteOrder::LITTLEENDIAN);
//            iretn = agent->push_response(packet.header.chanin, 0, packet.header.nodeorig, response_id, "Internal Request " + erequest + "\n" + response);
            return iretn;
        }

        int32_t PacketHandler::Ping(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;

            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            response += "Ping\n";
            packet.header.type = PacketComm::TypeId::DataObcPong;
            NODE_ID_TYPE temp = packet.header.nodedest;
            packet.header.nodedest = packet.header.nodeorig;
            packet.header.nodeorig = temp;
            iretn = agent->channel_push(packet.header.chanin, packet);
            return iretn;
        }

        int32_t PacketHandler::SetTime(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            double mjd = doublefrom(packet.data.data(), ByteOrder::LITTLEENDIAN);
            double delta = set_local_clock(mjd, packet.data[8]);
            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            response += "Set Time\n";
            response += to_label("Delta Seconds", delta) + to_label(" Direction", packet.data[8]) + "\n";

            iretn = QueueEpsSetTime(currentmjd(), agent);
            return iretn;
        }

        int32_t PacketHandler::GetTimeHuman(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            packet.response_id = centisec();
            response = to_unsigned(centisec(), 10) + " " + mjd2iso8601(currentmjd()) + " ";
            response += "Get Time\n";
            response += to_label("MET", currentmjd() - agent->cinfo->node.utcstart);
//            iretn = agent->push_response(packet.header.chanin, 0, packet.header.nodeorig, centisec(), response);
            return iretn;
        }

        int32_t PacketHandler::GetTimeBinary(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            packet.header.type = PacketComm::TypeId::DataObcTime;
            packet.header.nodedest = packet.header.nodeorig;
            packet.header.nodeorig = agent->nodeId;
            packet.data.resize(16);
            double mjd = currentmjd();
            memcpy(&packet.data[0], &mjd, 8);
            memcpy(&packet.data[8], &agent->cinfo->node.utcstart, 8);
            iretn = agent->channel_push(packet.header.chanin, packet);
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

        int32_t PacketHandler::ImageForward(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            iretn = agent->channel_push(agent->channel_number("IMAGE"), packet);
            return iretn;
        }

        int32_t PacketHandler::EnableChannel(PacketComm &packet, string &response, Agent* agent)
        {
            int32_t iretn=0;
            if (packet.data[1] < 32)
            {
                iretn = agent->channel_enable(packet.data[1], packet.data[0]);
            }
            else
            {
                string name(&packet.data[1], &packet.data[1]+(packet.data.size()-1));
                iretn = agent->channel_number(name);
                if (iretn >= 0)
                {
                    iretn = agent->channel_enable(name, packet.data[0]);
                }
            }
            return iretn;
        }


        // Queue Packetcomm packets

        int32_t PacketHandler::QueueReset(uint16_t seconds, uint32_t verification_check, Agent* agent, NODE_ID_TYPE dest, const string& channelout, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandObcReset;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number(channelout);
            packet.data.resize(6);
            uint32to(verification_check, &packet.data[0], ByteOrder::LITTLEENDIAN);
            uint16to(seconds, &packet.data[4], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueReboot(uint32_t verification_check, Agent* agent, NODE_ID_TYPE dest, const string& channelout, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandObcReboot;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number(channelout);
            packet.data.resize(6);
            uint32to(verification_check, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueSendBeacon(uint8_t btype, uint8_t bcount, Agent* agent, NODE_ID_TYPE orig, NODE_ID_TYPE dest, const string& channelout, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandObcSendBeacon;
            packet.header.nodeorig = dest;
            packet.header.nodedest = orig;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number(channelout);
            packet.data.resize(2);
            packet.data[0] = btype;
            packet.data[1] = bcount;
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::CreateBeacon(PacketComm &packet, uint8_t btype, Agent* agent, NODE_ID_TYPE dest, const string& channelout, const string& radioin)
        {
            int32_t iretn=0;

            Beacon beacon;
            vector<uint8_t> bytes;
            beacon.Init();
            beacon.EncodeBinary((Beacon::TypeId)btype, agent->cinfo, bytes);
            packet.header.type = PacketComm::TypeId::DataObcBeacon;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number(channelout);
            packet.data.clear();
            packet.data.insert(packet.data.end(), bytes.begin(), bytes.end());
            packet.Wrap();
            return iretn;
        }

        int32_t PacketHandler::QueueBeacon(uint8_t btype, uint8_t bcount, Agent* agent, NODE_ID_TYPE dest, const string& channelout, const string& radioin)
        {
            int32_t iretn=0;
            PacketComm packet;

            Beacon beacon;
            vector<uint8_t> bytes;
            beacon.Init();
            beacon.EncodeBinary((Beacon::TypeId)btype, agent->cinfo, bytes);
            packet.header.type = PacketComm::TypeId::DataObcBeacon;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number(channelout);
            packet.data.clear();
            packet.data.insert(packet.data.end(), bytes.begin(), bytes.end());
            for (uint16_t i=0; i<bcount; ++i)
            {
                iretn = agent->channel_push(packet);
            }
            return iretn;
        }

        int32_t PacketHandler::QueueAdcsCommunicate(uint8_t unit, uint8_t command, uint16_t rcount, vector<uint8_t> data, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm::CommunicateHeader header;

            header.unit = unit;
            header.command = command;
            header.responsecount = rcount;

            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandAdcsCommunicate;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("ADCS");
            packet.data.resize(sizeof(header));
            memcpy(packet.data.data(), &header, sizeof(header));
            packet.data.insert(packet.data.end(), data.begin(), data.end());
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueAdcsState(uint8_t state, vector<uint8_t>  data, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;

            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandAdcsState;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("ADCS");
            packet.data.resize(1);
            packet.data[0] = state;
            packet.data.insert(packet.data.end(), data.begin(), data.end());
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsCommunicate(uint8_t unit, uint8_t command, uint16_t rcount, vector<uint8_t> data, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm::CommunicateHeader header;

            header.unit = unit;
            header.command = command;
            header.responsecount = rcount;

            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsCommunicate;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("EPS");
            packet.data.resize(sizeof(header));
            memcpy(packet.data.data(), &header, sizeof(header));
            packet.data.insert(packet.data.end(), data.begin(), data.end());
            iretn = agent->channel_push(packet);
            return iretn;
        }

        //! \brief Queue EpsSwitchName packet
        //! Queue packet to change switch by name; on, of, or toggle, as indicated.
        //! \param name Name of switch.
        //! \param state 0 = off, 1 = on, 2 = toggle.
        //! \param agent Pointer to ::Cosmos::Support::Agent.
        //! \param dest Destination Node number.
        //! \param radio Radio name.
        //! \return Zero or negative error.
        int32_t PacketHandler::QueueEpsSwitchName(const std::string &name, uint8_t state, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSwitchName;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("EPS");
            packet.data.resize(1);
            packet.data[0] = state;
            packet.data.insert(packet.data.end(), name.begin(), name.end());
            iretn = agent->channel_push(packet);
            return iretn;
        }

        //! \brief Queue EpsSwitchNames packet
        //! Queue packet to switch names in list on, of, or toggle, as indicated.
        //! \param names ::vector of names as ::string.
        //! \param state 0 = off, 1 = on, 2 = toggle.
        //! \param agent Pointer to ::Cosmos::Support::Agent.
        //! \param dest Destination Node number.
        //! \param radio Radio name.
        //! \return Zero or negative error.
        int32_t PacketHandler::QueueEpsSwitchNames(vector<std::string> &names, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            return QueueEpsSwitchNames(string_join(names), agent, dest, radioin);
        }

        //! \brief Queue EpsSwitchNames packet
        //! Queue packet to switch names in list on, of, or toggle, as indicated.
        //! \param names ::string of space separated names.
        //! \param state 0 = off, 1 = on, 2 = toggle.
        //! \param agent Pointer to ::Cosmos::Support::Agent.
        //! \param dest Destination Node number.
        //! \param radio Radio name.
        //! \return Zero or negative error.
        int32_t PacketHandler::QueueEpsSwitchNames(const string& names, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSwitchNames;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("EPS");
            packet.data.clear();
            packet.data.insert(packet.data.end(), names.begin(), names.end());
            iretn = agent->channel_push(packet);
            return iretn;
        }

        //! \brief Queue EpsSwitchNumber packet
        //! Queue packet to change switch by number; on, of, or toggle, as indicated.
        //! \param number Number of switch.
        //! \param state 0 = off, 1 = on, 2 = toggle.
        //! \param agent Pointer to ::Cosmos::Support::Agent.
        //! \param dest Destination Node number.
        //! \param radio Radio name.
        //! \return Zero or negative error.
        int32_t PacketHandler::QueueEpsSwitchNumber(uint16_t number, uint8_t state, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSwitchName;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("EPS");
            packet.data.resize(3);
            packet.data[0] = state;
            uint16to(number, &packet.data[2], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsReset(uint16_t seconds, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsReset;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("EPS");
            packet.data.resize(2);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsState(uint8_t state, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsState;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("EPS");
            packet.data.resize(1);
            packet.data[0] = state;
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsWatchdog(uint16_t seconds, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsWatchdog;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("EPS");
            packet.data.resize(2);
            uint16to(seconds, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsSetTime(double mjd, Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsSetTime;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("EPS");
            packet.data.resize(8);
            doubleto(mjd, &packet.data[0], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEpsMinimumPower(Agent* agent, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandEpsMinimumPower;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number("EPS");
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueTransferRadio(uint8_t use_radio, bool availability, Agent* agent, NODE_ID_TYPE dest)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandFileTransferRadio;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = 0;
            packet.header.chanout = agent->channel_number("FILE");
            packet.data.resize(2);
            packet.data[0] = use_radio;
            packet.data[1] = availability;
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueTestRadio(uint8_t start, uint8_t step, uint8_t stop, uint32_t count, Agent* agent, string testradio, NODE_ID_TYPE dest, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandRadioTest;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number(testradio);
            packet.data.resize(11);
            packet.data[0] = start;
            packet.data[1] = step;
            packet.data[2] = stop;
            uint32to(decisec(), &packet.data[3], ByteOrder::LITTLEENDIAN);
            uint32to(count, &packet.data[7], ByteOrder::LITTLEENDIAN);
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueSetTime(double mjd, int8_t direction , Agent* agent, NODE_ID_TYPE dest, const string& channelout, const string& radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandObcSetTime;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number(channelout);
            packet.data.resize(9);
            doubleto(mjd, &packet.data[0], ByteOrder::LITTLEENDIAN);
            packet.data[8] = direction;
            iretn = agent->channel_push(packet);
            return iretn;
        }

        int32_t PacketHandler::QueueEnableChannel(const string &name, uint8_t enable,  Agent* agent, NODE_ID_TYPE dest, const string& channelout, const string &radioin)
        {
            int32_t iretn = agent->channel_number(name);
            if (iretn >= 0)
            {
                PacketComm packet;

                packet.header.type = PacketComm::TypeId::CommandExecEnableChannel;
                packet.header.nodeorig = agent->nodeId;
                packet.header.nodedest = dest;
                packet.header.chanin = agent->channel_number(radioin);
                packet.header.chanout = agent->channel_number(channelout);
                packet.data.resize(1);
                packet.data[0] = enable;
                packet.data.insert(packet.data.end(), name.begin(), name.end());
                iretn = agent->channel_push(packet);
            }
            return iretn;
        }

        int32_t PacketHandler::QueueEnableChannel(uint8_t number, uint8_t enable,  Agent* agent, NODE_ID_TYPE dest, const std::string &channelout, const string &radioin)
        {
            int32_t iretn = 0;
            PacketComm packet;

            packet.header.type = PacketComm::TypeId::CommandExecEnableChannel;
            packet.header.nodeorig = agent->nodeId;
            packet.header.nodedest = dest;
            packet.header.chanin = agent->channel_number(radioin);
            packet.header.chanout = agent->channel_number(channelout);
            packet.data.resize(2);
            packet.data[0] = enable;
            packet.data[1] = number;
            iretn = agent->channel_push(packet);
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
