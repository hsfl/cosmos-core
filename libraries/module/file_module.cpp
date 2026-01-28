#include "file_module.h"

namespace Cosmos
{
    namespace Module
    {

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
            is_running = true;

            ElapsedTime active_mode_timer;

            while(is_running.load(std::memory_order_relaxed))
            {
                std::this_thread::yield();
                bool received_packets = false;
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
                        received_packets = true;
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
                                if (packet.data.size() < 3)
                                {
                                    agent->debug_log.Printf("%16.10f Error: CommandFileTransferFile packet too short\n", currentmjd());
                                    continue;
                                }
                                size_t nn_len = packet.data[0];
                                size_t an_len = packet.data[1];
                                size_t fn_len = packet.data[2];
                                if (nn_len + an_len + fn_len + 3 > packet.data.size())
                                {
                                    agent->debug_log.Printf("%16.10f Error: CommandFileTransferFile packet too short for node, agent, and file names\n", currentmjd());
                                    continue;
                                }
                                string node_name = "";
                                string agent_name = "";
                                string file_name = "";
                                node_name.insert(node_name.begin(), packet.data.begin()+3, packet.data.begin()+3+nn_len);
                                agent_name.insert(agent_name.begin(), packet.data.begin()+3+nn_len, packet.data.begin()+3+nn_len+an_len);
                                file_name.insert(file_name.begin(), packet.data.begin()+3+nn_len+an_len, packet.data.begin()+3+nn_len+an_len+fn_len);
                                iretn = transfer.outgoing_tx_add(node_name, agent_name, file_name);
                                if (iretn < 0)
                                {
                                    agent->debug_log.Printf("%16.10f Error (%s) adding file %s/outgoing/%s/%s\n", currentmjd(), cosmos_error_string(iretn).c_str(), node_name.c_str(), agent_name.c_str(), file_name.c_str());
                                    continue;
                                }
                                else
                                {
                                    file_transfer_enabled = true;
                                    agent->debug_log.Printf("%16.10f Enabling transfer of %s/outgoing/%s/%s\n", currentmjd(), node_name.c_str(), agent_name.c_str(), file_name.c_str());
                                }
                            }
                            break;
                        case PacketComm::TypeId::CommandFileTransferNode:
                            {
                                string node_name;
                                size_t nn_len = packet.data[0];
                                node_name.insert(node_name.begin(), packet.data.begin()+1, packet.data.begin()+1+nn_len);

                                iretn = transfer.enable_all(node_name);
                                file_transfer_enabled.store(true, std::memory_order_relaxed);

                                string s = std::to_string(iretn) + " files enabled";
                            }
                            break;
                        case PacketComm::TypeId::CommandFileTransferRadio:
                            {
                                if (packet.data[1])
                                {
                                    file_transfer_enabled.store(true, std::memory_order_relaxed);
                                }
                                else
                                {
                                    file_transfer_enabled.store(false, std::memory_order_relaxed);
                                }
                            }
                            break;
                        case PacketComm::TypeId::CommandFileTransferList:
                            {
                                // if (out_radio)
                                // {
                                //     string s = transfer.list_outgoing();
                                //     agent->push_response(out_radio, packet.header.nodeorig, mychannel, centisec(), s);
                                // }
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
                                file_transfer_enabled.store(false, std::memory_order_relaxed);
                                for (auto node : contact_nodes)
                                {
                                    transfer.close_file_pointers(node, 2);
                                }
                            }
                            break;
                        case PacketComm::TypeId::CommandFileSendFileResponses:
                            {
                                // Check if any response-type packets need to be pushed
                                for (size_t i = 0; i < contact_nodes.size(); ++i)
                                {
                                    iretn = transfer.send_outgoing_rpackets(contact_nodes[i]);
                                    if (iretn < 0)
                                    {
                                        agent->debug_log.Printf("%16.10f Error in get_outgoing_rpackets: %d\n", currentmjd(), cosmos_error_string(iretn).c_str());
                                    }
                                }
                                file_transfer_respond = false;
                            }
                            break;
                        case PacketComm::TypeId::CommandFileSaveFileProgress:
                            {
                                for (size_t i = 0; i < contact_nodes.size(); ++i)
                                {
                                    iretn = transfer.save_progress(contact_nodes[i]);
                                    if (iretn < 0)
                                    {
                                        agent->debug_log.Printf("%16.10f Error in save_progress for node %u: %s\n", currentmjd(), (unsigned)contact_nodes[i], cosmos_error_string(iretn).c_str());
                                    }
                                }
                            }
                            break;
                        case PacketComm::TypeId::CommandFileTransferDirectory:
                            {
                                if (packet.data.size() < 2)
                                {
                                    agent->debug_log.Printf("%16.10f Error: CommandFileTransferDirectory packet too short\n", currentmjd());
                                    continue;
                                }
                                size_t nn_len = packet.data[0];
                                size_t dn_len = packet.data[1];
                                if (nn_len + dn_len + 2 > packet.data.size())
                                {
                                    agent->debug_log.Printf("%16.10f Error: CommandFileTransferDirectory packet too short for node, agent, and file names\n", currentmjd());
                                    continue;
                                }
                                string node_name = "";
                                string outgoing_subdirectory = "";
                                node_name.insert(node_name.begin(), packet.data.begin()+2, packet.data.begin()+2+nn_len);
                                outgoing_subdirectory.insert(outgoing_subdirectory.begin(), packet.data.begin()+2+nn_len, packet.data.begin()+2+nn_len+dn_len);
                                iretn = transfer.outgoing_tx_load(node_name, outgoing_subdirectory);
                                if (iretn < 0)
                                {
                                    agent->debug_log.Printf("%16.10f Error (%s) adding directory %s/outgoing/%s\n", currentmjd(), cosmos_error_string(iretn).c_str(), node_name.c_str(), outgoing_subdirectory.c_str());
                                    continue;
                                }
                                else
                                {
                                    file_transfer_enabled.store(true, std::memory_order_relaxed);
                                    agent->debug_log.Printf("%16.10f Enabling transfer of directory %s/outgoing/%s\n", currentmjd(), node_name.c_str(), outgoing_subdirectory.c_str());
                                }
                            }
                            break;
                        default:
                            break;
                        }
                    }

                    if (!file_transfer_enabled.load(std::memory_order_relaxed))
                    {
                        secondsleep(1.);
                        continue;
                    }

                    auto packets_sent = transfer.get_sender()->get_number_of_packets_sent();

                    // Get our own files' transfer packets if transfer is enabled
                    // Perform runs of file packet grabbing
                    for (size_t i = 0; i < contact_nodes.size(); ++i)
                    {
                        // Queue up outgoing file packets
                        iretn = transfer.send_outgoing_lpackets(contact_nodes[i]);
                        
                        if (iretn < 0)
                        {
                            agent->debug_log.Printf("%16.10f Error in get_outgoing_lpackets: %s\n", currentmjd(), cosmos_error_string(iretn).c_str());
                        }
                    }
                    // Check activity on RX or TX
                    if (received_packets || packets_sent != transfer.get_sender()->get_number_of_packets_sent())
                    {
                        active_mode_timer.reset();
                    }
                    else if (active_mode_timer.split() > inactivity_time_threshold)
                    {
                        // Transition out of busy wait back into an idle mode
                        file_transfer_enabled.store(false, std::memory_order_relaxed);
                    }
                }
            }

            return;
        }

        void FileModule::soft_shutdown()
        {
            file_transfer_enabled.store(false, std::memory_order_relaxed);
            is_running.store(false, std::memory_order_relaxed);
        }

        void FileModule::shutdown()
        {
            file_transfer_enabled.store(false, std::memory_order_relaxed);
            for (auto node : contact_nodes)
            {
                transfer.close_file_pointers(node, 2);
            }
            is_running.store(false, std::memory_order_relaxed);
        }
    }
}

// TODO: Investigate channel overflow (packet discards) for low maximums (try with maximum=100)
