#include "file_module.h"

namespace Cosmos
{
    namespace Module
    {
        FileModule::FileModule() {}

        int32_t FileModule::Init(Agent *parent_agent, const vector<string> file_transfer_contact_nodes)
        {
            agent = parent_agent;
            
            // Initialize Transfer class
            int32_t iretn = transfer.Init(agent->cinfo, &agent->debug_log, keep_errored_files);
            if (iretn < 0)
            {
                agent->debug_log.Printf("%.4f Error initializing transfer class!: %s\n", agent->uptime.split(), cosmos_error_string(iretn).c_str());
                return iretn;
            }

            // List of nodes to handle files for
            contact_nodes.clear();
            for (const string& node : file_transfer_contact_nodes)
            {
                iretn = lookup_node_id(agent->cinfo, node);
                if (iretn <= 0)
                {
                    agent->debug_log.Printf("%.4f Could not find node ID for node name: %s\n", agent->uptime.split(), node.c_str());
                    continue;
                }
                contact_nodes.push_back(static_cast<uint8_t>(iretn));
            }

            return 0;
        }

        // This thread handles file transfers
        void FileModule::Loop()
        {
            int32_t iretn = 0;

            PacketComm packet;
            mychannel = agent->channel_number("FILE");

            agent->debug_log.Printf("Starting File Loop\n");

            // Perform initial load
            double diskcheckwait = 30.;
            ElapsedTime disk_check_timer;
            disk_check_timer.set(diskcheckwait);

            while(agent->running())
            {
                if (agent->running() != (uint16_t)Agent::State::IDLE)
                {

                    // Process packets from channel
                    ElapsedTime cet;
                    while (cet.split() < 1.)
                    {
                        if ((iretn = agent->channel_pull(mychannel, packet)) <= 0)
                        {
                            break;
                        }
                        // Packets for us
                        switch (packet.header.type)
                        {
                        case PacketComm::TypeId::DataFileCommand:
                        case PacketComm::TypeId::DataFileMessage:
                        case PacketComm::TypeId::DataFileQueue:
                        case PacketComm::TypeId::DataFileCancel:
                        case PacketComm::TypeId::DataFileComplete:
                        case PacketComm::TypeId::DataFileReqMeta:
                        case PacketComm::TypeId::DataFileReqData:
                        case PacketComm::TypeId::DataFileMetaData:
                        case PacketComm::TypeId::DataFileChunkData:
                        case PacketComm::TypeId::DataFileReqComplete:
                            {
                                transfer.print_file_packet(packet, 0, "Incoming", &agent->debug_log);
                                iretn = transfer.receive_packet(packet);
                                if (iretn == transfer.RESPONSE_REQUIRED)
                                {
                                    file_transfer_respond = true;
                                }
                                else if (iretn < 0)
                                {
                                    if (agent->get_debug_level())
                                    {
                                        agent->debug_log.Printf("%.4f Main: Node: %s Agent: %s - Error in receive_packet(): %d\n", agent->uptime.split(), agent->cinfo->node.name.c_str(), agent->cinfo->agent0.name.c_str(), iretn);
                                    }
                                }
                            }
                            break;
                        case PacketComm::TypeId::CommandFileTransferFile:
                            {
                                string node_name;
                                size_t nn_len = packet.data[0];
                                node_name.insert(node_name.begin(), packet.data.begin()+1, packet.data.begin()+1+nn_len);
                                string agent_name;
                                size_t an_len = *(packet.data.begin()+1+nn_len);
                                agent_name.insert(agent_name.begin(), packet.data.begin()+2+nn_len, packet.data.begin()+2+nn_len+an_len);
                                string file_name;
                                size_t fn_len = *(packet.data.begin()+2+nn_len+an_len);
                                file_name.insert(file_name.begin(), packet.data.begin()+2+nn_len+an_len, packet.data.begin()+3+nn_len+an_len+fn_len);

                                iretn = transfer.enable_single(node_name, file_name);
                                file_transfer_enabled = true;

                                string s = std::to_string(iretn) + " files enabled";
                            }
                            break;
                        case PacketComm::TypeId::CommandFileTransferNode:
                            {
                                string node_name;
                                size_t nn_len = packet.data[0];
                                node_name.insert(node_name.begin(), packet.data.begin()+1, packet.data.begin()+1+nn_len);

                                iretn = transfer.outgoing_tx_load(node_name);
                                iretn = transfer.enable_all(node_name);
                                file_transfer_enabled = true;

                                string s = std::to_string(iretn) + " files enabled";
                            }
                            break;
                        case PacketComm::TypeId::CommandFileTransferRadio:
                            {
                                uint8_t current_out_radio = out_radio;
                                set_radio_availability(packet.data[0], packet.data[1]);
                                if (out_radio == 0)
                                {
                                    file_transfer_enabled = false;
                                }
                                else
                                {
                                    // Enable transfer and rescan outgoing directory
                                    file_transfer_enabled = true;
                                    // Scan directories if scanning interval passed or if radio changed
                                    if (disk_check_timer.timer() > 0 && out_radio == current_out_radio)
                                    {
                                        break;
                                    }
                                    for (size_t i = 0; i < contact_nodes.size(); ++i)
                                    {
                                        iretn = transfer.outgoing_tx_load(contact_nodes[i]);
                                    }
                                    disk_check_timer.set(diskcheckwait);
                                }
                            }
                            break;
                        case PacketComm::TypeId::CommandFileTransferList:
                            {
                                if (out_radio)
                                {
                                    string s = transfer.list_outgoing();
                                    agent->push_response(out_radio, packet.header.nodeorig, mychannel, centisec(), s);
                                }
                            }
                            break;
                        case PacketComm::TypeId::CommandFileResetQueue:
                            {
                                // Arg 0: node id of contact node to clear queue of
                                // Arg 1: direction to clear (0 incoming, 1 outgoing, 2 both)
                                if (packet.data.size() < 2)
                                {
                                    break;
                                }
                                for (auto node : contact_nodes)
                                {
                                    if (node == packet.data[0])
                                    {
                                        transfer.reset_queue(node, packet.data[1]);
                                    }
                                }
                            }
                            break;
                        case PacketComm::TypeId::CommandFileStopTransfer:
                            {
                                out_radio = 0;
                                file_transfer_enabled = false;
                                for (auto node : contact_nodes)
                                {
                                    transfer.close_file_pointers(node, 2);
                                }
                            }
                            break;
                        default:
                            break;
                        }
                    }

                    // No outgoing radio connection established,
                    // Or the channel is not in active mode
                    if (!out_radio || agent->channel_enabled(out_radio) != 1)
                    {
                        secondsleep(1.);
                        continue;
                    }

                    // Check if any response-type packets need to be pushed
                    if (file_transfer_respond)
                    {
                        for (size_t i = 0; i < contact_nodes.size(); ++i)
                        {
                            iretn = transfer.send_outgoing_rpackets(contact_nodes[i], agent, out_radio, continual_stream_time);
                            if (iretn < 0)
                            {
                                agent->debug_log.Printf("%16.10f Error in get_outgoing_rpackets: %d\n", currentmjd(), cosmos_error_string(iretn).c_str());
                            }
                        }
                        file_transfer_respond = false;
                    }

                    // Get our own files' transfer packets if transfer is enabled
                    if (file_transfer_enabled)
                    {
                        // Scan directories if scanning interval passed
                        if (disk_check_timer.timer() < 0)
                        {
                            for (size_t i = 0; i < contact_nodes.size(); ++i)
                            {
                                iretn = transfer.outgoing_tx_load(contact_nodes[i]);
                            }
                            disk_check_timer.set(diskcheckwait);
                        }

                        // Perform runs of file packet grabbing
                        for (size_t i = 0; i < contact_nodes.size(); ++i)
                        {
                            // Consider current queue fullness, this decreases the effective time we have to queue and transmit
                            double time_to_flush_current_queue = agent->channel_size(out_radio)/packet_rate;
                            // Calculated as the amount of time we would like to transmit continually for
                            double effective_time = continual_stream_time - time_to_flush_current_queue;
                            if (effective_time <= 0)
                            {
                                // Wait until queue is less full
                                continue;
                            }
                            // The amount of packets we can feasibly queue up without any issue
                            uint32_t max_packets = effective_time * packet_rate;

                            // Queue up outgoing file packets
                            queueing_timer.reset();
                            iretn = transfer.send_outgoing_lpackets(contact_nodes[i], agent, out_radio, max_packets, continual_stream_time);
                            double queueing_time = queueing_timer.split();
                            
                            if (iretn < 0)
                            {
                                agent->debug_log.Printf("%16.10f Error in get_outgoing_lpackets: %s\n", currentmjd(), cosmos_error_string(iretn).c_str());
                            }
                            else if (queueing_time > 0 && iretn > 0)
                            {
                                // Use the number of packets queued and the elapsed time
                                // to calculate the queuing speed of the system.
                                // Adds in some arbitrary inefficiency.
                                // Keep the rate above some arbitrary lower bound.
                                packet_rate = std::max((iretn / queueing_time)*0.75, PACKET_RATE_LOWER_BOUND);
                            }
                        }
                    }
                }
                std::this_thread::yield();
            }

            return;
        }

        void FileModule::set_radios(vector<uint8_t> radios)
        {
            // radios_channel_number and radios_available are always the same size, and indexes match
            radios_channel_number = radios;
            radios_available.resize(radios.size(), false);
        }

        void FileModule::set_radio_availability(uint8_t radio, bool availability)
        {
            // Turn on or off specified radio
            for (size_t i=0; i < radios_channel_number.size(); ++i)
            {
                if (radios_channel_number[i] == radio)
                {
                    // Pointless to do the rest if no change is being made
                    if (radios_available[i] == availability && out_radio == radio)
                    {
                        return;
                    }
                    radios_available[i] = availability;
                }
            }
            // Set the out_radio to the newest available and highest priority radio
            determine_out_radio();
        }

        void FileModule::determine_out_radio()
        {
            // 0 is no radio
            uint8_t new_out_radio = 0;
            // Iterate over available radios, which are sorted by priority, highest to lowest
            for (size_t i=0; i < radios_available.size(); ++i)
            {
                // If it's on, use it
                if (radios_available[i])
                {
                    new_out_radio = radios_channel_number[i];
                    break;
                }
            }
            // Set transfer class to use file packet sizes of the new channel
            if (new_out_radio)
            {
                int32_t channel_datasize = agent->channel_datasize(new_out_radio);
                if (channel_datasize <= 0)
                {
                    return;
                }
                transfer.set_packet_size(channel_datasize);
            }
            bool radio_changed = (out_radio != new_out_radio);
            out_radio = new_out_radio;

            // If no radios for files to transfer over,
            // close and flush all open file pointers.
            // E.g., if a ground station pass is over
            if (out_radio == 0 && radio_changed)
            {
                agent->debug_log.Printf("%.4f No radios for file transfer, flushing all file pointers\n", agent->uptime.split());
                for (auto node : contact_nodes)
                {
                    transfer.close_file_pointers(node, 2);
                }
            }
        }
    }
}
