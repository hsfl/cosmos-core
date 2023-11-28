/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

#include "support/transferclass.h"
namespace Cosmos {
    namespace Support {
        Transfer::Transfer()
        {
            // calstruc cal = mjd2cal(currentmjd());
            // txid = 100 * (utc2unixseconds(currentmjd()) - utc2unixseconds(cal2mjd(cal.year, 1.))) - 1;
        }

        // int32_t Transfer::Init(string node, string agent, uint16_t chunk_size)
        // {
        //     this->node = node;
        //     this->agent = agent;
        //     this->chunk_size = chunk_size;
        //     return 0;
        // }

        /** 
         * Initialize the transfer class
         * \param calling_node_realm Name of node using this instance of transferclass
         * \return 0 if success
         */
        int32_t Transfer::Init(cosmosstruc *cinfo)
        {
            int32_t iretn = Init(cinfo, nullptr);
            return iretn;
        }

        /** 
         * Initialize the transfer class
         * \param calling_node_realm Realm of node using this instance of transferclass
         * \param debug_log Pointer to an agent's debug_log to use to print error messages.
         * \return 0 if success
         */
        int32_t Transfer::Init(cosmosstruc *cinfo, Log::Logger* debug_log)
        {

            int32_t iretn = 0;
            this->debug_log = debug_log;
            this->cinfo = cinfo;

            // if (this->debug_log != nullptr)
            // {
            //     this->debug_log->Printf("Transfer: step 1\n");
            // }

            // Transfer: Queue
            const int32_t node_ids_size = cinfo->realm.node_ids.size();
            if (node_ids_size < 2)
            {
                if (this->debug_log != nullptr)
                {
                    this->debug_log->Printf("%.4f Couldn't load node lookup table\n", tet.split());
                }
                return COSMOS_TRANSFER_ERROR_NODE;
            }
            // if (this->debug_log != nullptr)
            // {
            //     this->debug_log->Printf("Transfer: step 2\n");
            // }

            // Identify and store calling node's node_id
            iretn = lookup_node_id(cinfo, cinfo->node.name);
            if (iretn == NODEIDUNKNOWN)
            {
                if (this->debug_log != nullptr)
                {
                    this->debug_log->Printf("%.4f Could not find node %s in node table!\n", tet.split(), cinfo->node.name.c_str());
                }
                return COSMOS_TRANSFER_ERROR_NODE;
            }
            self_node_id = iretn;
            self_node_name = cinfo->node.name;
            // if (this->debug_log != nullptr)
            // {
            //     this->debug_log->Printf("Transfer: step 3\n");
            // }

            // Create map of node_ids to indexes into txq
            txq.clear();
            txq.resize(node_ids_size);
            node_id_to_txq_map.clear();
            size_t tidx = 0;
            for (auto it = cinfo->realm.node_ids.begin(); it != cinfo->realm.node_ids.end(); ++it)
            {
                if (it->second == self_node_id || it->second == NODEIDUNKNOWN)
                {
                    // Don't add self or unknown to txq
                    continue;
                }
                node_id_to_txq_map.insert(std::make_pair(it->second, tidx));
                txq[tidx].node_name = it->first;
                txq[tidx].node_id = it->second;
                ++tidx;
            }
            txq.resize(tidx);
            // if (this->debug_log != nullptr)
            // {
            //     this->debug_log->Printf("Transfer: step 4\n");
            // }

            // Restore in progress transfers from previous run
            for (auto& transfer_node : txq)
            {
                for(filestruc file : data_list_files(transfer_node.node_name, "temp", "file"))
                {
                    if (this->debug_log != nullptr)
                    {
                        this->debug_log->Printf("Transfer: Restore %s %s\n", transfer_node.node_name.c_str(), file.name.c_str());
                    }
                    // Add entry for each meta file
                    if (file.type == "meta")
                    {
                        // Incoming
                        if (!file.name.compare(0,3,"in_"))
                        {
                            tx_progress tx_in;
                            tx_in.temppath = file.path.substr(0,file.path.find(".meta"));
                            if (read_meta(tx_in) >= 0)
                            {
                                merge_chunks_overlap(tx_in);
                                iretn = incoming_tx_add(tx_in);
                                // Error readd-ing incoming meta, move what was there, delete meta files
                                if (iretn < 0)
                                {
                                    if (debug_log != nullptr)
                                    {
                                        debug_log->Printf("%.4f %.4f Init: Error readd-ing incoming meta. Moving: %u %s %s %s %s\n", tet.split(), dt.lap(), tx_in.tx_id, tx_in.node_name.c_str(), tx_in.agent_name.c_str(), tx_in.file_name.c_str(), tx_in.filepath.c_str());
                                    }
                                    rename((tx_in.temppath + ".file").c_str(), (tx_in.filepath + "_INITADD_ERR").c_str());
                                    remove((tx_in.temppath + ".meta").c_str());
                                    remove((tx_in.temppath + ".file").c_str());
                                }
                            }
                            else
                            {
                                if (debug_log != nullptr)
                                {
                                    debug_log->Printf("%.4f %.4f Init: Error reading incoming meta, version mismatch. Moving: %u %s %s %s %s\n", tet.split(), dt.lap(), tx_in.tx_id, tx_in.node_name.c_str(), tx_in.agent_name.c_str(), tx_in.file_name.c_str(), tx_in.filepath.c_str());
                                }
                                rename((tx_in.temppath + ".file").c_str(), (tx_in.filepath + "_INITADD_VER_ERR").c_str());
                                remove((tx_in.temppath + ".meta").c_str());
                                remove((tx_in.temppath + ".file").c_str());
                            }
                        }

                        // Outgoing
                        if (!file.name.compare(0,4,"out_"))
                        {
                            tx_progress tx_out;
                            tx_out.enabled = true;
                            tx_out.temppath = file.path.substr(0,file.path.find(".meta"));
                            if (read_meta(tx_out) >= 0)
                            {
                                merge_chunks_overlap(tx_out);
                                iretn = outgoing_tx_add(tx_out, transfer_node.node_name);
                                // Error readd-ing outgoing meta, delete meta files
                                if (iretn < 0)
                                {
                                    if (debug_log != nullptr)
                                    {
                                        debug_log->Printf("%.4f %.4f Init: Error readd-ing outgoing meta. Deleting: %s\n", tet.split(), dt.lap(), tx_out.filepath.c_str());
                                    }
                                    remove((tx_out.temppath + ".meta").c_str());
                                }
                            }
                            else
                            {
                                if (debug_log != nullptr)
                                {
                                    debug_log->Printf("%.4f %.4f Init: Error reading outgoing meta, version mismatch. Deleting: %s\n", tet.split(), dt.lap(), tx_out.filepath.c_str());
                                }
                                remove((tx_out.temppath + ".meta").c_str());
                            }
                        }
                    }
                }
            }

            return 0;
        }

        //! Scan the outgoing directory of every node in txq.
        //! Enqueues new files in txq's outgoing queue.
        //! Not thread safe.
        //! \return Number of files in the outgoing queue
        int32_t Transfer::outgoing_tx_load()
        {
            int32_t num_files = 0;

            // Go through outgoing queues for all nodes
            for (auto it = node_id_to_txq_map.begin(); it != node_id_to_txq_map.end(); ++it)
            {
                if (it->first == self_node_id)
                {
                    continue;
                }
                int32_t iretn = outgoing_tx_load(it->first);
                if (iretn >= 0)
                {
                    num_files += iretn;
                }
            }
            return num_files;
        }

        //! Scan the outgoing directory of the specified node in txq.
        //! Enqueues new files in its outgoing queue.
        //! Not thread safe.
        //! \return Number of files in the outgoing queue, negative on error
        int32_t Transfer::outgoing_tx_load(const string node_name)
        {
            if (node_name.empty())
            {
                return outgoing_tx_load();
            }
            int32_t iretn = lookup_node_id(cinfo, node_name);
            if (iretn == NODEIDUNKNOWN)
            {
                return TRANSFER_ERROR_NODE;
            }
            return outgoing_tx_load(iretn);
        }

        //! Scan the outgoing directory of the specified node in txq.
        //! Enqueues new files in its outgoing queue.
        //! Not thread safe.
        //! \param dest_node_id ID of destination node
        //! \return Number of files in this node's outgoing queue, negative on error
        int32_t Transfer::outgoing_tx_load(const uint8_t dest_node_id)
        {
            const size_t dest_node_idx = node_id_to_txq_idx(dest_node_id);
            if (dest_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_NODE;
            }
            
            // Go through outgoing queues, removing files that no longer exist
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[dest_node_idx].outgoing.progress[i].tx_id != 0 && !data_isfile(txq[dest_node_idx].outgoing.progress[i].filepath))
                {
                    outgoing_tx_del(dest_node_id, txq[dest_node_idx].outgoing.progress[i].tx_id);
                }
            }
            // Go through outgoing directories, adding files not already in queue
            if (txq[dest_node_idx].outgoing.size < PROGRESS_QUEUE_SIZE-1)
            {
                string dest_node = lookup_node_id_name(cinfo, dest_node_id);
                if (dest_node.empty())
                {
                    return TRANSFER_ERROR_NODE;
                }
                vector<filestruc> file_names;
                // dest_node/outgoing/
                for (filestruc file : data_list_files(txq[dest_node_idx].node_name, "outgoing"))
                {
                    // dest_node/outgoing/dest_agents
                    if (file.type == "directory")
                    {
                        data_list_files(txq[dest_node_idx].node_name, "outgoing", file.name, file_names);
                    }
                }

                // Sort list by size, then go through list of files found, adding to queue.
                sort(file_names.begin(), file_names.end(), filestruc_smaller_by_size);
                for(uint16_t i=0; i<file_names.size(); ++i)
                {
                    if (txq[dest_node_idx].outgoing.size == PROGRESS_QUEUE_SIZE - 1)
                    {
                        break;
                    }

                    //Ignore sub-directories
                    if (file_names[i].type == "directory")
                    {
                        continue;
                    }

                    // Remove zero-length files
                    if (file_names[i].size == 0)
                    {
                        remove(file_names[i].path.c_str());
                        continue;
                    }

                    outgoing_tx_add(file_names[i].node, file_names[i].agent, file_names[i].name);
                }
            }

            return outgoing_tx_recount(dest_node_id);
        }

        //! Look through the outgoing and incoming queues of all nodes and generate any necessary packets.
        //! Note that this gets outgoing packets for every node, no distinction is made.
        //! Not thread safe.
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        // int32_t Transfer::get_outgoing_lpackets(vector<PacketComm> &packets)
        // {
        //     for (auto it = node_id_to_txq_map.begin(); it != node_id_to_txq_map.end(); ++it)
        //     {
        //         int32_t iretn = get_outgoing_lpackets(it->first, packets);
        //         if (iretn < 0)
        //         {
        //             if (debug_log != nullptr)
        //             {
        //                 debug_log->Printf("%.4f %.4f Main: get_outgoing_lpackets: %s at node %u\n", tet.split(), dt.lap(), cosmos_error_string(iretn).c_str(), it->first);
        //             }
        //             return iretn;
        //         }
        //     }
            
        //     return 0;
        // }

        //! Look through the outgoing and incoming queues of the specified node and generate any necessary packets.
        //! Not thread safe.
        //! \param node_name Name of the node
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        // int32_t Transfer::get_outgoing_lpackets(const string node_name, vector<PacketComm> &packets)
        // {
        //     if (node_name.empty())
        //     {
        //         if (debug_log != nullptr)
        //         {
        //             debug_log->Printf("%.4f %.4f Main: get_outgoing_lpackets: node_name is empty\n", tet.split(), dt.lap());
        //         }
        //         return TRANSFER_ERROR_FILENAME;
        //     }

        //     // Find corresponding node_id in the node table
        //     uint8_t node_id = lookup_node_id(cinfo, node_name);
        //     if (node_id == NODEIDUNKNOWN)
        //     {
        //         if (debug_log != nullptr)
        //         {
        //             debug_log->Printf("%.4f %.4f Main: get_outgoing_lpackets: node_id is unknown\n", tet.split(), dt.lap());
        //         }
        //         return TRANSFER_ERROR_NODE;
        //     }

            // int32_t iretn = 0;
            // iretn = get_outgoing_lpackets(node_id, packets);

            // return iretn;
        // }

        //! Look through the outgoing and incoming queues of the specified node and generate any necessary packets.
        //! Not thread safe.
        //! \param dest_node_id ID of dest node
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return Number of total packets queued on success, negative on error
        int32_t Transfer::send_outgoing_lpackets(const uint8_t dest_node_id, Agent* agent, const uint8_t channel_id, uint32_t max_packets_to_queue, double timeout)
        {
            // Check that node exists in the node table
            const size_t dest_node_idx = node_id_to_txq_idx(dest_node_id);
            if (dest_node_idx == INVALID_TXQ_IDX)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: get_outgoing_lpackets: invalid dest_node_idx %u\n", tet.split(), dt.lap(), dest_node_id);
                }
                return TRANSFER_ERROR_NODE;
            }

            // Hold QUEUE packet tx_id's in here
            vector<PACKET_TX_ID_TYPE> tqueue;

            // Store separately to not be affected by sentqueue being toggled mid-run, which would be bad.
            bool sendqueue = !txq[dest_node_idx].outgoing.sentqueue;

            // Each file can queue up to a certain fraction of the max packets that can be queued up
            uint32_t packets_per_file = 1;
            uint8_t outgoing_file_count = txq[dest_node_idx].outgoing.size;
            if (!outgoing_file_count)
            {
                return 0;
            }
            else
            {
                packets_per_file = std::max(max_packets_to_queue / outgoing_file_count, 1u);
            }
            // If a previous file did not use all its allotted packet count, then the next file(s) can queue up however many went unused
            size_t extra_packets = 0;
            // Keep track of total queued packets, don't exceed max
            uint32_t total_queued_packets = 0;
            // Since it's possible to queue too fast for the other threads to pull quickly enough,
            // stop queueing up if we're edging up on the max size of the channel buffer.
            int32_t channel_buffer_limit = agent->channel_maximum(channel_id) * 0.9;
            if (channel_buffer_limit <= 0)
            {
                return GENERAL_ERROR_UNDERSIZE;
            }

            // Break out of chunk streaming if timeout time has elapsed
            ElapsedTime file_stream_timeout;

            // Iterate over all files in outgoing queue, push necessary packets
            // Since higher tx_ids have higher file sizes, let extra_packets accumulate for bigger files to use
            for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
            {
                // Check if this is a valid outgoing transaction
                // Note: the semantic of this equality comparison necessarily implies the unavailability of tx_id == 0
                // This check is entered if REQMETA is received for an invalid tx_id
                if (txq[dest_node_idx].outgoing.progress[tx_id].tx_id != tx_id) {
                    if (!txq[dest_node_idx].outgoing.progress[tx_id].enabled)
                    {
                        // Not a transaction to consider, skip
                        continue;
                    }
                    // The other side thinks this is a valid transaction, respond that it isn't
                    txq[dest_node_idx].outgoing.progress[tx_id].enabled = false;
                    outgoing_packet.header.nodeorig = self_node_id;
                    outgoing_packet.header.nodedest = dest_node_id;
                    serialize_cancel(outgoing_packet, static_cast<PACKET_NODE_ID_TYPE>(self_node_id), tx_id, PACKET_FILE_CRC_FORCE);
                    agent->channel_push(channel_id, outgoing_packet);
                    print_file_packet(outgoing_packet, 1, "Outgoing", &agent->debug_log);
                    ++total_queued_packets;
                    continue;
                }

                // Files can queue up a number of extra packets that went unused so far
                size_t packets_left_to_queue_for_this_file = packets_per_file + extra_packets;
                extra_packets = 0;

                // **************************************************************
                // ** METADATA **************************************************
                // **************************************************************
                if (!txq[dest_node_idx].outgoing.progress[tx_id].sentmeta)
                {
                    tx_progress& tx = txq[dest_node_idx].outgoing.progress[tx_id];
                    outgoing_packet.header.nodeorig = self_node_id;
                    outgoing_packet.header.nodedest = dest_node_id;
                    serialize_metadata(outgoing_packet, static_cast <PACKET_NODE_ID_TYPE>(self_node_id), tx.tx_id, tx.file_crc, tx.file_name, tx.file_size, tx.agent_name);
                    agent->channel_push(channel_id, outgoing_packet);
                    print_file_packet(outgoing_packet, 1, "Outgoing", &agent->debug_log);
                    ++total_queued_packets;
                    txq[dest_node_idx].outgoing.progress[tx_id].sentmeta = true;
                }

                // ** enabled check *****************
                // Check if this file is enabled
                // Note that the file's QUEUE status and METADATA is always sent,
                // regardless of whether it's enabled or not.

                // Check if all data for this file has been sent
                if (txq[dest_node_idx].outgoing.progress[tx_id].enabled
                && !txq[dest_node_idx].outgoing.progress[tx_id].sentdata)
                {
                // **************************************************************
                // ** DATA ******************************************************
                // **************************************************************
                    if (txq[dest_node_idx].outgoing.progress[tx_id].file_size)
                    {
                        // Check if there is any more data to send
                        if (txq[dest_node_idx].outgoing.progress[tx_id].total_bytes == 0)
                        {
                            // This sections runs, for example, after a read_meta of a previous run where all data was already sent
                            txq[dest_node_idx].outgoing.progress[tx_id].sentdata = true;
                            write_meta(txq[dest_node_idx].outgoing.progress[tx_id], 0.);
                        }
                        // Grab next chunk of file's data and push DATA packet
                        else
                        {
                            // Attempt to open the outgoing progress file
                            if (txq[dest_node_idx].outgoing.progress[tx_id].fp == nullptr)
                            {
                                txq[dest_node_idx].outgoing.progress[tx_id].fp = fopen(txq[dest_node_idx].outgoing.progress[tx_id].filepath.c_str(), "r");
                            }
                            // Check if file still exists
                            else if (!data_exists(txq[dest_node_idx].outgoing.progress[tx_id].filepath))
                            {
                                // Some problem with this transmission, ask other end to dequeue it
                                txq[dest_node_idx].outgoing.progress[tx_id].sentdata = true;
                                txq[dest_node_idx].outgoing.progress[tx_id].complete = true;
                            }

                            // Some problem with this transmission, ask other end to dequeue it
                            if (txq[dest_node_idx].outgoing.progress[tx_id].fp == nullptr)
                            {
                                txq[dest_node_idx].outgoing.progress[tx_id].sentdata = true;
                                txq[dest_node_idx].outgoing.progress[tx_id].complete = true;
                            }

                            // If we're good, continue with the process, queue up as many data packets as allotted
                            const PACKET_FILE_SIZE_TYPE packet_data_size_limit = packet_size - offsetof(struct packet_struct_data, chunk);
                            PACKET_BYTE* chunk = new PACKET_BYTE[packet_data_size_limit]();
                            while (txq[dest_node_idx].outgoing.progress[tx_id].fp != nullptr // Check file is still open
                            && packets_left_to_queue_for_this_file > 0                       // Queue up to max per file
                            && total_queued_packets < max_packets_to_queue)                  // Don't exceed max
                            {
                                file_progress tp;
                                tp = txq[dest_node_idx].outgoing.progress[tx_id].file_info.back();

                                PACKET_FILE_SIZE_TYPE byte_count = (tp.chunk_end - tp.chunk_start) + 1;
                                if (byte_count > packet_data_size_limit)
                                {
                                    byte_count = packet_data_size_limit;
                                }

                                tp.chunk_end = tp.chunk_start + byte_count - 1;

                                // Read the packet and send it
                                size_t nbytes = 0;
                                // Read bytes into chunk
                                if (!fseek(txq[dest_node_idx].outgoing.progress[tx_id].fp, tp.chunk_start, SEEK_SET))
                                {
                                    nbytes = fread(chunk, 1, byte_count, txq[dest_node_idx].outgoing.progress[tx_id].fp);
                                }
                                if (nbytes == static_cast<size_t>(byte_count))
                                {
                                    outgoing_packet.header.nodeorig = self_node_id;
                                    outgoing_packet.header.nodedest = dest_node_id;
                                    serialize_data(outgoing_packet, static_cast <PACKET_NODE_ID_TYPE>(self_node_id), txq[dest_node_idx].outgoing.progress[tx_id].tx_id, txq[dest_node_idx].outgoing.progress[tx_id].file_crc, byte_count, tp.chunk_start, chunk);
                                    int32_t current_channel_buffer_size = agent->channel_push(channel_id, outgoing_packet);
                                    print_file_packet(outgoing_packet, 1, "Outgoing", &agent->debug_log);
                                    --packets_left_to_queue_for_this_file;
                                    ++total_queued_packets;
                                    txq[dest_node_idx].outgoing.progress[tx_id].file_info.back().chunk_start = tp.chunk_end + 1;
                                    // Break out if channel is getting too full
                                    if (current_channel_buffer_size < 0 || current_channel_buffer_size > channel_buffer_limit)
                                    {
                                        break;
                                    }
                                }
                                else
                                {
                                    // Some problem with this transmission, ask other end to dequeue it
                                    // Remove transaction
                                    txq[dest_node_idx].outgoing.progress[tx_id].sentdata = true;
                                    txq[dest_node_idx].outgoing.progress[tx_id].complete = true;
                                    break;
                                }

                                // Check if last chunk is finished yet
                                if (txq[dest_node_idx].outgoing.progress[tx_id].file_info.back().chunk_start > txq[dest_node_idx].outgoing.progress[tx_id].file_info.back().chunk_end)
                                {
                                    // All done with this file_info entry. Close file and remove entry.
                                    fclose(txq[dest_node_idx].outgoing.progress[tx_id].fp);
                                    txq[dest_node_idx].outgoing.progress[tx_id].fp = nullptr;
                                    txq[dest_node_idx].outgoing.progress[tx_id].file_info.pop_back();
                                    // Update total_bytes
                                    merge_chunks_overlap(txq[dest_node_idx].outgoing.progress[tx_id]);

                                    // Check if there is any more data to send
                                    if (txq[dest_node_idx].outgoing.progress[tx_id].total_bytes == 0)
                                    {
                                        txq[dest_node_idx].outgoing.progress[tx_id].sentdata = true;
                                        write_meta(txq[dest_node_idx].outgoing.progress[tx_id], 0.);
                                    }
                                    break;
                                }
                                else
                                {
                                    write_meta(txq[dest_node_idx].outgoing.progress[tx_id]);
                                }
                                // Break out if timeout has elapsed.
                                // Put this at the end of the while loop so that each
                                // file can send out at least 1 data packet per run.
                                if (file_stream_timeout.split() > timeout)
                                {
                                    break;
                                }
                            }
                            delete[] chunk;
                        }
                    }
                    // Zero length file, ask other end to dequeue it
                    else
                    {
                        txq[dest_node_idx].outgoing.progress[tx_id].sentdata = true;
                        txq[dest_node_idx].outgoing.progress[tx_id].complete = true;
                    }
                }
                // All data for this file has been sent
                else if (txq[dest_node_idx].outgoing.progress[tx_id].enabled)
                {
                    // Check if a COMPLETE packet response was received from the receiver
                    // or if the complete flag was set because of an error
                    if (txq[dest_node_idx].outgoing.progress[tx_id].complete)
                    {
                // **************************************************************
                // ** CANCEL ****************************************************
                // **************************************************************
                        // Remove from queue
                        PACKET_FILE_CRC_TYPE file_crc = txq[dest_node_idx].outgoing.progress[tx_id].file_crc;
                        outgoing_tx_del(dest_node_id, tx_id);

                        // Send a CANCEL packet
                        outgoing_packet.header.nodeorig = self_node_id;
                        outgoing_packet.header.nodedest = dest_node_id;
                        serialize_cancel(outgoing_packet, static_cast<PACKET_NODE_ID_TYPE>(self_node_id), tx_id, file_crc);
                        agent->channel_push(channel_id, outgoing_packet);
                        print_file_packet(outgoing_packet, 1, "Outgoing", &agent->debug_log);
                        ++total_queued_packets;
                    }
                    // A COMPLETE packet was not received yet
                    else
                    {
                        // Check if it's not too soon to send another REQCOMPLETE packet
                        if (currentmjd() > txq[dest_node_idx].outgoing.progress[tx_id].next_response)
                        {
                // **************************************************************
                // ** REQCOMPLETE ***********************************************
                // **************************************************************
                            txq[dest_node_idx].outgoing.progress[tx_id].next_response = currentmjd() + txq[dest_node_idx].outgoing.waittime;
                            outgoing_packet.header.nodeorig = self_node_id;
                            outgoing_packet.header.nodedest = dest_node_id;
                            serialize_reqcomplete(outgoing_packet, static_cast <PACKET_NODE_ID_TYPE>(self_node_id), tx_id, txq[dest_node_idx].outgoing.progress[tx_id].file_crc);
                            agent->channel_push(channel_id, outgoing_packet);
                            print_file_packet(outgoing_packet, 1, "Outgoing", &agent->debug_log);
                            ++total_queued_packets;
                        }

                        // If it's too soon, don't send a REQCOMPLETE packet yet.
                        // This is to prevent sending redundant requests, the receiver
                        // only needs to respond once.
                    }
                }

                // **************************************************************
                // ** QUEUE *****************************************************
                // **************************************************************
                if (sendqueue
                // Check that this transaction is still happening
                && txq[dest_node_idx].outgoing.progress[tx_id].tx_id == tx_id)
                {
                    tqueue.push_back(tx_id);
                }

                // Let next file queue up more if this file didn't have much
                extra_packets = packets_left_to_queue_for_this_file;
            }

            // Push QUEUE packet if it needs to be sent
            if (sendqueue)
            {
                outgoing_packet.header.nodeorig = self_node_id;
                outgoing_packet.header.nodedest = dest_node_id;
                serialize_queue(outgoing_packet, static_cast<PACKET_NODE_ID_TYPE>(self_node_id), tqueue);
                agent->channel_push(channel_id, outgoing_packet);
                print_file_packet(outgoing_packet, 1, "Outgoing", &agent->debug_log);
                ++total_queued_packets;
                txq[dest_node_idx].outgoing.sentqueue = true;
            }

            return total_queued_packets;
        }

        //! Look through the outgoing and incoming queues of the specified node and generate any necessary packets.
        //! These are response-type file transfer packets, to signal to the sender that something is wrong, or going right.
        //! Not thread safe.
        //! \param node_id ID of origin node in txq
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        int32_t Transfer::send_outgoing_rpackets(const uint8_t orig_node_id, Agent* agent, const uint8_t channel_id, double timeout)
        {
            // Check that node exists in the node table
            const size_t orig_node_idx = node_id_to_txq_idx(orig_node_id);
            if (orig_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_NODE;
            }

            // Hold REQMETA bits in here
            vector<PACKET_TX_ID_TYPE> treqmeta;
            // Hold REQDATA packets here
            vector<PacketComm> reqdata_packets;
            // Since it's possible to queue too fast for the other threads to pull quickly enough,
            // stop queueing up if we're edging up on the max size of the channel buffer.
            int32_t channel_buffer_limit = agent->channel_maximum(channel_id) * 0.9;
            if (channel_buffer_limit <= 0)
            {
                return GENERAL_ERROR_UNDERSIZE;
            }
            // Break out of chunk streaming if timeout time has elapsed
            ElapsedTime file_stream_timeout;

            // Iterate over tx_id's requiring a response
            for (PACKET_TX_ID_TYPE tx_id : txq[orig_node_idx].incoming.respond)
            {
                // **************************************************************
                // ** REQMETA ***************************************************
                // **************************************************************
                if (!txq[orig_node_idx].incoming.progress[tx_id].sentmeta)
                {
                    treqmeta.push_back(tx_id);
                }
                // **************************************************************
                // ** REQDATA ***************************************************
                // **************************************************************
                else if (!txq[orig_node_idx].incoming.progress[tx_id].sentdata)
                {
                    // Request missing data
                    vector<file_progress> missing;
                    missing = find_chunks_missing(txq[orig_node_idx].incoming.progress[tx_id]);
                    if (missing.size())
                    {
                        serialize_reqdata(reqdata_packets, static_cast <PACKET_NODE_ID_TYPE>(self_node_id), orig_node_id, txq[orig_node_idx].incoming.progress[tx_id].tx_id, txq[orig_node_idx].incoming.progress[tx_id].file_crc, missing, packet_size);
                        for (auto& packet: reqdata_packets)
                        {
                            int32_t current_channel_buffer_size = agent->channel_push(channel_id, packet);
                            print_file_packet(packet, 1, "Outgoing", &agent->debug_log);
                            // Break out if channel is getting too full
                            if (current_channel_buffer_size < 0 || current_channel_buffer_size > channel_buffer_limit)
                            {
                                break;
                            }
                            // Break out if timeout has elapsed
                            if (file_stream_timeout.split() > timeout)
                            {
                                break;
                            }
                        }
                    }
                    else
                    {
                        // This code block to run under this scenario:
                        // METADATA is missed
                        // But all DATA is received
                        // Then METADATA is received (after a )
                        // And then REQCOMPLETE was received

                        // Now check the file_crc
                        int32_t crcret = calc_crc.calc_file(txq[orig_node_idx].incoming.progress[tx_id].temppath + ".file");
                        if (crcret < 0 || txq[orig_node_idx].incoming.progress[tx_id].file_crc != crcret)
                        {
                            if (debug_log != nullptr)
                            {
                                debug_log->Printf("%.4f %.4f rpacket/ReqData: Error in final crc check. Was %d expected %u\n", tet.split(), dt.lap(), crcret, txq[orig_node_idx].incoming.progress[tx_id].file_crc);
                            }
                            txq[orig_node_idx].incoming.progress[tx_id].complete = false;
                            txq[orig_node_idx].incoming.progress[tx_id].filepath += "_FILECRC_ERR";
                            incoming_tx_del(orig_node_id, tx_id);
                            continue;
                        }

                        // Received file's checksum matches sender's checksum, file is succesfully received
                        incoming_tx_complete(orig_node_id, tx_id);
                    }
                }
                // **************************************************************
                // ** COMPLETE **************************************************
                // **************************************************************
                if (txq[orig_node_idx].incoming.progress[tx_id].sentmeta
                && txq[orig_node_idx].incoming.progress[tx_id].sentdata)
                {
                    PacketComm packet;
                    packet.header.nodeorig = self_node_id;
                    packet.header.nodedest = orig_node_id;
                    serialize_complete(packet, static_cast <PACKET_NODE_ID_TYPE>(self_node_id), tx_id, txq[orig_node_idx].incoming.progress[tx_id].file_crc);
                    agent->channel_push(channel_id, packet);
                    print_file_packet(packet, 1, "Outgoing", &agent->debug_log);
                }

                txq[orig_node_idx].incoming.progress[tx_id].next_response = currentmjd() + txq[orig_node_idx].incoming.waittime;
            }

            txq[orig_node_idx].incoming.respond.clear();
            
            // Send REQMETA packet if needed
            if (treqmeta.size())
            {
                PacketComm packet;
                packet.header.nodeorig = self_node_id;
                packet.header.nodedest = orig_node_id;
                serialize_reqmeta(packet, static_cast <PACKET_NODE_ID_TYPE>(self_node_id), txq[orig_node_idx].node_name, treqmeta);
                agent->channel_push(channel_id, packet);
                print_file_packet(packet, 1, "Outgoing", &agent->debug_log);
            }

            return 0;
        }

        //! Creates the metadata for a new file to queue.
        //! After performing basic checks regarding file creation and ensuring unqidxueness in the
        //! outgoing queue, writes the meta data to disk and then calls outgoing_tx_add(tx_progress &tx_out).
        //! \param dest_node Name of the destination node
        //! \param dest_agent Name of destination agent
        //! \param file_name Name of file to add to outgoing queue
        //! \return Number of files in node's outgoing queue if non-error
        int32_t Transfer::outgoing_tx_add(const string dest_node, const string dest_agent, const string file_name)
        {
            if (self_node_name.empty() || dest_node.empty() || dest_agent.empty() || file_name.empty())
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_FILENAME\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_FILENAME;
            }

            // BEGIN GATHERING THE METADATA
            tx_progress tx_out;

            const uint8_t dest_node_id = lookup_node_id(cinfo, dest_node);
            if (dest_node_id == NODEIDUNKNOWN)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_NODENAME %s\n", tet.split(), dt.lap(), dest_node.c_str());
                }
                return TRANSFER_ERROR_NODE;
            }
            const size_t dest_node_idx = node_id_to_txq_idx(dest_node_id);
            if (dest_node_idx == INVALID_TXQ_IDX)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_NODE %s %u\n", tet.split(), dt.lap(), dest_node.c_str(), dest_node_id);
                }
                return TRANSFER_ERROR_NODE;
            }

            // Only add if we have room
            if (txq[dest_node_idx].outgoing.size == PROGRESS_QUEUE_SIZE-1)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_QUEUEFULL\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_QUEUEFULL;
            }

            // Get the file path and size
            string filepath = data_base_path(dest_node, "outgoing", dest_agent, file_name);
            PACKET_FILE_SIZE_TYPE file_size = 0;
            int32_t iretn = get_file_size(filepath, file_size);
            if (iretn < 0)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_FILESIZE\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_FILESIZE;
            }

            // Go through existing queue
            // - if it is already there and the size is different, remove it
            // - if it is already there, and the size is the same, enable it
            // - if it is not already there and there is room to add it, go on to next step
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[dest_node_idx].outgoing.progress[i].filepath == filepath)
                {
                    if (txq[dest_node_idx].outgoing.progress[i].file_size == file_size)
                    {
                        // This file transaction already exists and doesn't need to be added
                        // if (!txq[dest_node_idx].outgoing.progress[i].enabled)
                        // {
                        //     txq[dest_node_idx].outgoing.progress[i].enabled = true;
                        //     if (debug_log != nullptr)
                        //     {
                        //         debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: Enable %u %s %s %s %d\n", tet.split(), dt.lap(), txq[dest_node_idx].outgoing.progress[i].tx_id, txq[dest_node_idx].outgoing.progress[i].node_name.c_str(), txq[dest_node_idx].outgoing.progress[i].agent_name.c_str(), txq[dest_node_idx].outgoing.progress[i].filepath.c_str(), PROGRESS_QUEUE_SIZE);
                        //     }
                        // }
                        return outgoing_tx_recount(dest_node_id);
                    }
                    else
                    {
                        if (debug_log != nullptr)
                        {
                            debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_FILESIZE\n", tet.split(), dt.lap());
                        }
                        outgoing_tx_del(dest_node_id, i, false);
                        return TRANSFER_ERROR_FILESIZE;
                    }
                    if (txq[dest_node_idx].outgoing.progress[i].enabled && txq[dest_node_idx].outgoing.progress[i].file_size == 0)
                    {
                        if (debug_log != nullptr)
                        {
                            debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_FILEZERO\n", tet.split(), dt.lap());
                        }
                        outgoing_tx_del(dest_node_id, i, true);
                        return TRANSFER_ERROR_FILEZERO;
                    }
                }
            }

            // That we made it this far, the file is ok to add to the queue

            // Generate a new tx_id using the file size as a hash key
            uint16_t minindex = TRANSFER_QUEUE_LIMIT - static_cast<uint16_t>(pow(pow(TRANSFER_QUEUE_LIMIT,1.f/3.f), (3.f - log10f(file_size) / 2.f)));
            tx_out.tx_id = 0;
            for (uint16_t id=minindex; id<PROGRESS_QUEUE_SIZE; ++id)
            {
                if (txq[dest_node_idx].outgoing.progress[id].tx_id == 0)
                {
                    tx_out.tx_id = id;
                    break;
                }
            }

            if (tx_out.tx_id > 0)
            {
                // Initialize file transfer settings
                tx_out.sentmeta = false;
                tx_out.sentdata = false;
                tx_out.complete = false;
                tx_out.enabled = true;
                tx_out.total_bytes = 0;
                tx_out.node_name = self_node_name; // i.e., the origin node
                tx_out.agent_name = dest_agent;
                tx_out.file_name = file_name;
                tx_out.temppath = data_base_path(dest_node, "temp", "file", "out_"+std::to_string(tx_out.tx_id));
                tx_out.filepath = filepath;
                tx_out.savetime = 0.;

                std::ifstream filename;

                // get the file size
                tx_out.file_size = file_size;

                // Calculate the file_crc
                int32_t iretn = calc_crc.calc_file(tx_out.filepath);
                if (iretn < 0)
                {
                    if (debug_log != nullptr)
                    {
                        debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: Error opening file. %s\n", tet.split(), dt.lap(), cosmos_error_string(iretn).c_str());
                    }
                    return iretn;
                }
                tx_out.file_crc = iretn;

                file_progress tp;
                tp.chunk_start = 0;
                tp.chunk_end = tx_out.file_size - 1;
                tx_out.file_info.push_back(tp);

                write_meta(tx_out, 0.);

                iretn = outgoing_tx_add(tx_out, dest_node);
                return iretn;
            }
            else
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_MATCH\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_MATCH;
            }
        }

        //! Adds new file to outgoing queue.
        //! Called from other functions that create a tx_progress to pass into this.
        //! \param tx_out A tx_progress containing meta information about file to add to outgoing queue.
        //! \param dest_node_name Name of destination node
        //! \return Number of files in node's outgoing queue if non-error
        int32_t Transfer::outgoing_tx_add(tx_progress &tx_out, const string dest_node_name)
        {
            if (debug_log != nullptr)
            {
                debug_log->Printf("%.4f %.4f Main: outgoing_tx_add: ", tet.split(), dt.lap());
            }

            const int32_t dest_node_id = lookup_node_id(cinfo, dest_node_name);
            if (dest_node_id <= 0)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("dest_node_id not found for node_name %s\n", dest_node_name.c_str());
                }
                if (dest_node_id == NODEIDUNKNOWN)
                {
                    return TRANSFER_ERROR_NODE;
                }
                else
                {
                    return dest_node_id;
                }
            }
            const size_t dest_node_idx = node_id_to_txq_idx(dest_node_id);
            if (dest_node_idx == INVALID_TXQ_IDX)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("Error getting txq_idx for node_id %u\n", dest_node_id);
                }
                return TRANSFER_ERROR_NODE;
            }

            // Check for duplicate tx_id
            if (txq[dest_node_idx].outgoing.progress[tx_out.tx_id].tx_id)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("TRANSFER_ERROR_DUPLICATE\n");
                }
                return TRANSFER_ERROR_DUPLICATE;
            }

            if (!tx_out.file_name.size() || !tx_out.agent_name.size() || !tx_out.temppath.size())
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("TRANSFER_ERROR_FILENAME %d %d %d\n", tx_out.file_name.size(), tx_out.agent_name.size(), tx_out.temppath.size());
                }
                tx_out.filepath = "";
                return TRANSFER_ERROR_FILENAME;
            }
            if (!tx_out.filepath.size())
            {
                tx_out.filepath = data_base_path(dest_node_name, "outgoing", tx_out.agent_name, tx_out.file_name);
            }

            // Check for a duplicate file name of something already in queue
            uint16_t minindex = 255 - static_cast<uint16_t>(pow(pow(TRANSFER_QUEUE_LIMIT,1.f/3.f), (3.f - log10f(tx_out.file_size) / 2.f)));
            for (uint16_t i=minindex; i<256; ++i)
            {
                if (!txq[dest_node_idx].outgoing.progress[i].filepath.empty() && tx_out.filepath == txq[dest_node_idx].outgoing.progress[i].filepath)
                {
                    // Remove the META file
                    if (debug_log != nullptr)
                    {
                        debug_log->Printf("%u %u %s %s %s TRANSFER_ERROR_DUPLICATE\n", tx_out.tx_id, tx_out.file_crc, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.filepath.c_str());
                    }
                    string filepath = tx_out.temppath + ".meta";
                    remove(filepath.c_str());
                    return TRANSFER_ERROR_DUPLICATE;
                }
            }

            tx_out.fp = nullptr;
            //get the file size
            int32_t iretn = get_file_size(tx_out.filepath, tx_out.file_size);
            if (iretn < 0)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%u %s %s %s TRANSFER_ERROR_FILESIZE\n", tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.filepath.c_str());
                }
                return TRANSFER_ERROR_FILESIZE;
            }
            tx_out.savetime = 0.;

            // save and queue metadata packet
            tx_out.sentmeta = false;
            tx_out.sentdata = false;
            tx_out.complete = false;

            if (debug_log != nullptr)
            {
                debug_log->Printf("%u %u %s %s %s %d ", tx_out.tx_id, tx_out.file_crc, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.filepath.c_str(), PROGRESS_QUEUE_SIZE);
            }

            // Good to go. Add it to queue.
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].tx_id = tx_out.tx_id;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].file_crc = tx_out.file_crc;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].sentmeta = tx_out.sentmeta;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].sentdata = tx_out.sentdata;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].complete = tx_out.complete;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].enabled  = tx_out.enabled;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].node_name = tx_out.node_name;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].agent_name = tx_out.agent_name;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].file_name = tx_out.file_name;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].filepath = tx_out.filepath;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].temppath = tx_out.temppath;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].savetime = tx_out.savetime;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].next_response = 0;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].file_size = tx_out.file_size;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].total_bytes = tx_out.total_bytes;
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].file_info.clear();
            for (file_progress filep : tx_out.file_info)
            {
                txq[dest_node_idx].outgoing.progress[tx_out.tx_id].file_info.push_back(filep);
            }
            txq[dest_node_idx].outgoing.progress[tx_out.tx_id].fp = tx_out.fp;
            ++txq[dest_node_idx].outgoing.size;

            // Update total_bytes
            merge_chunks_overlap(txq[dest_node_idx].outgoing.progress[tx_out.tx_id]);

            // Set QUEUE to be sent out again
            txq[dest_node_idx].outgoing.sentqueue = false;

            if (debug_log != nullptr)
            {
                debug_log->Printf(" %u\n", txq[dest_node_idx].outgoing.size);
            }

            return outgoing_tx_recount(dest_node_id);
        }

        //! Removes file from outgoing queue.
        //! \param node_id ID of destination node
        //! \param tx_id ID of the file transfer transaction
        //! \param remove_file If set to true, deletes file from the outgoing directory as well
        //! \return Number of files in node's outgoing queue if non-error
        int32_t Transfer::outgoing_tx_del(const uint8_t node_id, const PACKET_TX_ID_TYPE tx_id, const bool remove_file)
        {
            const size_t dest_node_idx = node_id_to_txq_idx(node_id);
            if (node_id == NODEIDUNKNOWN || dest_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_INDEX;
            }


            if (txq[dest_node_idx].outgoing.progress[tx_id].tx_id == NODEIDUNKNOWN)
            {
                return TRANSFER_ERROR_MATCH;
            }

            tx_progress tx_out = txq[dest_node_idx].outgoing.progress[tx_id];

            // erase the transaction
            if (txq[dest_node_idx].outgoing.progress[tx_id].fp != nullptr)
            {
                fclose(txq[dest_node_idx].outgoing.progress[tx_id].fp);
            }
            txq[dest_node_idx].outgoing.progress[tx_id].fp = nullptr;
            txq[dest_node_idx].outgoing.progress[tx_id].enabled = false;
            txq[dest_node_idx].outgoing.progress[tx_id].tx_id = 0;
            txq[dest_node_idx].outgoing.progress[tx_id].file_crc = 0;
            txq[dest_node_idx].outgoing.progress[tx_id].complete = false;
            txq[dest_node_idx].outgoing.progress[tx_id].filepath = "";
            txq[dest_node_idx].outgoing.progress[tx_id].savetime = 0;
            txq[dest_node_idx].outgoing.progress[tx_id].temppath = "";
            txq[dest_node_idx].outgoing.progress[tx_id].file_name = "";
            txq[dest_node_idx].outgoing.progress[tx_id].file_size = 0;
            txq[dest_node_idx].outgoing.progress[tx_id].node_name = "";
            txq[dest_node_idx].outgoing.progress[tx_id].agent_name = "";
            txq[dest_node_idx].outgoing.progress[tx_id].total_bytes = 0;
            txq[dest_node_idx].outgoing.progress[tx_id].file_info.clear();

            if (txq[dest_node_idx].outgoing.size)
            {
                --txq[dest_node_idx].outgoing.size;
            }

            // Set QUEUE to be sent out again
            txq[dest_node_idx].outgoing.sentqueue = false;

            bool error = false;

            // Remove the file
            if(remove_file && remove(tx_out.filepath.c_str()) && debug_log != nullptr)
            {
                debug_log->Printf("%.4f %.4f Main/Outgoing: Del outgoing: %u %s %s %s %s - Unable to remove file\n", tet.split(), dt.lap(), tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str(), tx_out.filepath.c_str());
                error = true;
            }

            // Remove the META file
            string meta_filepath = tx_out.temppath + ".meta";
            if (remove(meta_filepath.c_str()) && debug_log != nullptr)
            {
                debug_log->Printf("%.4f %.4f Main/Outgoing: Del outgoing: %u %s %s %s %s - Unable to remove meta file\n", tet.split(), dt.lap(), tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str(), meta_filepath.c_str());
                error = true;
            }

            if (!error && debug_log != nullptr)
            {
                debug_log->Printf("%.4f %.4f Main/Outgoing: Del outgoing: %u %s %s %s\n", tet.split(), dt.lap(), tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str());
            }

            return outgoing_tx_recount(node_id);
        }

        //! Recount number of files in outgoing queue
        //! \param node_name Name of node to check
        //! \return Number of files in node's outgoing queue
        int32_t Transfer::outgoing_tx_recount(const string node_name)
        {
            return outgoing_tx_recount(lookup_node_id(cinfo, node_name));
        }

        //! Recount number of files in outgoing queue
        //! \param node_id ID of node to check
        //! \return Number of files in node's outgoing queue
        int32_t Transfer::outgoing_tx_recount(const uint8_t node_id)
        {
            const size_t dest_node_idx = node_id_to_txq_idx(node_id);
            if (node_id == NODEIDUNKNOWN || dest_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_INDEX;
            }

            txq[dest_node_idx].outgoing.size = 0;
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[dest_node_idx].outgoing.progress[i].tx_id)
                {
                    ++txq[dest_node_idx].outgoing.size;
                }
            }

            return txq[dest_node_idx].outgoing.size;
        }

        /// Checks if tx_id is valid
        /// \return tx_id on success, 0 on failure
        PACKET_TX_ID_TYPE Transfer::check_tx_id(const tx_entry &txentry, const PACKET_TX_ID_TYPE tx_id)
        {
            if (tx_id != 0 && txentry.progress[tx_id].tx_id == tx_id)
            {
                return tx_id;
            }
            else
            {
                return 0;
            }
        }

        //! Adds a new incoming transaction.
        //! Called when knowledge of this transaction is first gained from a DATA or QUEUE packet.
        //! i.e., no META data is acquired yet.
        int32_t Transfer::incoming_tx_add(const string node_name, const PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc)
        {
            tx_progress tx_in;

            tx_in.tx_id = tx_id;
            tx_in.file_crc = file_crc;
            tx_in.sentmeta = false;
            tx_in.sentdata = false;
            tx_in.complete = false;
            tx_in.node_name = node_name;
            tx_in.agent_name = "";
            tx_in.file_name = "";
            tx_in.savetime = 0.;
            tx_in.file_size = 0;
            tx_in.total_bytes = 0;
            tx_in.file_info.clear();

            int32_t iretn = incoming_tx_add(tx_in);

            return iretn;
        }

        //! Adds a new incoming file.
        //! Called when a METADATA packet is the first to arrive
        int32_t Transfer::incoming_tx_add(const string node_name, const packet_struct_metadata& meta)
        {
            tx_progress tx_in;

            tx_in.tx_id = meta.header.tx_id;
            tx_in.file_crc = meta.header.file_crc;
            tx_in.sentmeta = true;
            tx_in.sentdata = false;
            tx_in.complete = false;
            tx_in.node_name = node_name;
            tx_in.agent_name = meta.agent_name;
            tx_in.file_name = meta.file_name;
            tx_in.savetime = 0.;
            tx_in.file_size = meta.file_size;
            tx_in.total_bytes = 0;
            tx_in.file_info.clear();

            int32_t iretn = incoming_tx_add(tx_in);

            return iretn;
        }

        //! Adds a new file to incoming queue
        //! \return Number of files in incoming queue, or error value
        int32_t Transfer::incoming_tx_add(tx_progress &tx_in)
        {
            uint8_t orig_node_id = lookup_node_id(cinfo, tx_in.node_name);
            if (orig_node_id == NODEIDUNKNOWN)
            {
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: incoming_tx_add: TRANSFER_ERROR_NODE\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_NODE;
            }
            const size_t orig_node_idx = node_id_to_txq_idx(orig_node_id);
            if (orig_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_NODE;
            }


            // Check for an actual file name
            if (tx_in.file_name.size() && tx_in.agent_name.size())
            {
                tx_in.filepath = data_base_path(tx_in.node_name, "incoming", tx_in.agent_name, tx_in.file_name);
            }
            else
            {
                tx_in.filepath = "";
            }

            // Set path to temporary files for new incoming files
            if (!tx_in.temppath.size())
            {
                string tx_name = "in_"+std::to_string(tx_in.tx_id);
                tx_in.temppath = data_base_path(tx_in.node_name, "temp", "file", tx_name);
            }

            // Check for a duplicate file name of something already in queue
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (!txq[orig_node_idx].incoming.progress[i].filepath.empty() && tx_in.filepath == txq[orig_node_idx].incoming.progress[i].filepath)
                {
                    if (debug_log != nullptr)
                    {
                        debug_log->Printf("%u %s %s %s TRANSFER_ERROR_DUPLICATE\n", tx_in.tx_id, tx_in.node_name.c_str(), tx_in.agent_name.c_str(), tx_in.filepath.c_str());
                    }
                    // Remove the META file
                    string filepath = tx_in.temppath + ".meta";
                    remove(filepath.c_str());
                    return TRANSFER_ERROR_DUPLICATE;
                }
            }

            tx_in.savetime = 0.;
            tx_in.fp = nullptr;

            // Put it in list
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].tx_id = tx_in.tx_id;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].file_crc = tx_in.file_crc;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].sentmeta = tx_in.sentmeta;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].sentdata = tx_in.sentdata;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].complete = tx_in.complete;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].node_name = tx_in.node_name;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].agent_name = tx_in.agent_name;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].file_name = tx_in.file_name;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].filepath = tx_in.filepath;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].temppath = tx_in.temppath;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].savetime = tx_in.savetime;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].next_response = currentmjd() + txq[orig_node_idx].incoming.waittime;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].file_size = tx_in.file_size;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].total_bytes = tx_in.total_bytes;
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].file_info.clear();
            for (file_progress filep : tx_in.file_info)
            {
                txq[orig_node_idx].incoming.progress[tx_in.tx_id].file_info.push_back(filep);
            }
            txq[orig_node_idx].incoming.progress[tx_in.tx_id].fp = tx_in.fp;
            ++txq[orig_node_idx].incoming.size;

            if (debug_log != nullptr)
            {
                debug_log->Printf("%.4f %.4f Main/Incoming: Add incoming: %u %u %s %s %s %d\n", tet.split(), dt.lap(), tx_in.tx_id, tx_in.file_crc, tx_in.node_name.c_str(), tx_in.agent_name.c_str(), tx_in.filepath.c_str(), PROGRESS_QUEUE_SIZE);
            }

            return incoming_tx_recount(orig_node_id);
        }

        /**
         * @brief Updates internal structures from a received META-type packet
         * 
         * @param meta Incoming META packet
         * @return tx_id on success, negative on failure
         */
        int32_t Transfer::incoming_tx_update(const packet_struct_metadata &meta)
        {
            int32_t orig_node_id = check_node_id(cinfo, meta.header.node_id);
            if (orig_node_id == NODEIDUNKNOWN)
            {
                return TRANSFER_ERROR_NODE;
            }
            if (orig_node_id < 0)
            {
                return orig_node_id;
            }
            const size_t orig_node_idx = node_id_to_txq_idx(orig_node_id);
            if (orig_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_NODE;
            }

            if (meta.header.tx_id)
            {
                // Check if file_crc is the same. If they differ, this is a new transaction. If so, cancel current entry and move
                // If QUEUE is received first, then file_crc will not be known. Only delete the current transaction
                // if the meta has already been received but the file_crc differs.
                if (txq[orig_node_idx].incoming.progress[meta.header.tx_id].sentmeta
                && txq[orig_node_idx].incoming.progress[meta.header.tx_id].file_crc != meta.header.file_crc)
                {
                    // This renaming would only occur in error states.
                    // A file completed correctly would already be renamed and moved.
                    if (debug_log != nullptr)
                    {
                        debug_log->Printf("%.4f %.4f Incoming: MetaData: File crc differs, closing existing transaction: %u %u %u %u %s %s %s\n", tet.split(), dt.lap(), txq[orig_node_idx].incoming.progress[meta.header.tx_id].tx_id, txq[orig_node_idx].incoming.progress[meta.header.tx_id].file_crc, meta.header.tx_id, meta.header.file_crc, txq[orig_node_idx].incoming.progress[meta.header.tx_id].node_name.c_str(), txq[orig_node_idx].incoming.progress[meta.header.tx_id].agent_name.c_str(), txq[orig_node_idx].incoming.progress[meta.header.tx_id].file_name.c_str());
                    }
                    txq[orig_node_idx].incoming.progress[meta.header.tx_id].filepath += "_METATXID_ERR";
                    incoming_tx_del(orig_node_id, meta.header.tx_id);
                    // Fallthrough to META received for the first time code
                }

                // This will run if META is received first
                // If so, add the file to the incoming queue.
                if (txq[orig_node_idx].incoming.progress[meta.header.tx_id].tx_id != meta.header.tx_id)
                {
                    txq[orig_node_idx].incoming.progress[meta.header.tx_id].tx_id = meta.header.tx_id;
                    string node_name = lookup_node_id_name(cinfo, orig_node_id);
                    if (node_name.empty())
                    {
                        return TRANSFER_ERROR_INDEX;
                    }
                    int32_t iretn = incoming_tx_add(node_name, meta);
                    if (iretn <= 0) {
                        return iretn;
                    }
                }


                // This will run if either META is received for the first time or if requested by REQMETA
                if (!txq[orig_node_idx].incoming.progress[meta.header.tx_id].sentmeta)
                {
                    // Core META information
                    txq[orig_node_idx].incoming.progress[meta.header.tx_id].tx_id = meta.header.tx_id;
                    txq[orig_node_idx].incoming.progress[meta.header.tx_id].file_crc = meta.header.file_crc;
                    txq[orig_node_idx].incoming.progress[meta.header.tx_id].node_name = txq[orig_node_idx].node_name;
                    txq[orig_node_idx].incoming.progress[meta.header.tx_id].agent_name = meta.agent_name;
                    txq[orig_node_idx].incoming.progress[meta.header.tx_id].file_name = meta.file_name;
                    txq[orig_node_idx].incoming.progress[meta.header.tx_id].file_size = meta.file_size;
                    txq[orig_node_idx].incoming.progress[meta.header.tx_id].filepath = data_base_path(txq[orig_node_idx].incoming.progress[meta.header.tx_id].node_name, "incoming", txq[orig_node_idx].incoming.progress[meta.header.tx_id].agent_name, txq[orig_node_idx].incoming.progress[meta.header.tx_id].file_name);

                    // This section will be skipped if DATA is written before META was received
                    if (!txq[orig_node_idx].incoming.progress[meta.header.tx_id].file_info.size()) {
                        string tx_name = "in_"+std::to_string(txq[orig_node_idx].incoming.progress[meta.header.tx_id].tx_id);
                        txq[orig_node_idx].incoming.progress[meta.header.tx_id].temppath = data_base_path(txq[orig_node_idx].incoming.progress[meta.header.tx_id].node_name, "temp", "file", tx_name);
                        
                        // Derivative META information
                        txq[orig_node_idx].incoming.progress[meta.header.tx_id].savetime = 0.;
                        txq[orig_node_idx].incoming.progress[meta.header.tx_id].complete = false;
                        txq[orig_node_idx].incoming.progress[meta.header.tx_id].total_bytes = 0;
                        txq[orig_node_idx].incoming.progress[meta.header.tx_id].fp = nullptr;
                    }

                    // Save it to disk
                    write_meta(txq[orig_node_idx].incoming.progress[meta.header.tx_id]);
                }

                txq[orig_node_idx].incoming.progress[meta.header.tx_id].sentmeta = true;

                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Incoming: Update incoming: %u %s %s %s\n", tet.split(), dt.lap(), txq[orig_node_idx].incoming.progress[meta.header.tx_id].tx_id, txq[orig_node_idx].incoming.progress[meta.header.tx_id].node_name.c_str(), txq[orig_node_idx].incoming.progress[meta.header.tx_id].agent_name.c_str(), txq[orig_node_idx].incoming.progress[meta.header.tx_id].file_name.c_str());
                }

                return meta.header.tx_id;
            }
            else
            {
                return TRANSFER_ERROR_INDEX;
            }
        }

        //! Resets a file transfer tx_id
        //! Used when a CANCEL packet is received.
        //! \return Number of files remaining in incoming queue
        int32_t Transfer::incoming_tx_del(const uint8_t orig_node_id, const PACKET_TX_ID_TYPE tx_id)
        {
            const size_t orig_node_idx = node_id_to_txq_idx(orig_node_id);
            if (orig_node_id == NODEIDUNKNOWN || orig_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_NODE;
            }

            if (txq[orig_node_idx].incoming.progress[tx_id].tx_id == NODEIDUNKNOWN)
            {
                return TRANSFER_ERROR_MATCH;
            }

            if (txq[orig_node_idx].incoming.size)
            {
                --txq[orig_node_idx].incoming.size;
            }

            // Move file to its final location
            if (!txq[orig_node_idx].incoming.progress[tx_id].complete)
            {
                incoming_tx_complete(orig_node_id, tx_id);
            }

            if (debug_log != nullptr)
            {
                debug_log->Printf("%.4f %.4f Incoming: Delete incoming: %u %s %s\n", tet.split(), dt.lap(), txq[orig_node_idx].incoming.progress[tx_id].tx_id, txq[orig_node_idx].incoming.progress[tx_id].node_name.c_str(), txq[orig_node_idx].incoming.progress[tx_id].node_name.c_str());
            }

            txq[orig_node_idx].incoming.progress[tx_id].tx_id = 0;
            txq[orig_node_idx].incoming.progress[tx_id].file_crc = 0;
            txq[orig_node_idx].incoming.progress[tx_id].enabled = false;
            txq[orig_node_idx].incoming.progress[tx_id].sentmeta = false;
            txq[orig_node_idx].incoming.progress[tx_id].sentdata = false;
            txq[orig_node_idx].incoming.progress[tx_id].complete = false;
            txq[orig_node_idx].incoming.progress[tx_id].node_name.clear();
            txq[orig_node_idx].incoming.progress[tx_id].agent_name.clear();
            txq[orig_node_idx].incoming.progress[tx_id].file_name.clear();
            txq[orig_node_idx].incoming.progress[tx_id].filepath.clear();
            txq[orig_node_idx].incoming.progress[tx_id].temppath.clear();
            txq[orig_node_idx].incoming.progress[tx_id].savetime = 0.;
            txq[orig_node_idx].incoming.progress[tx_id].next_response = 0.;
            txq[orig_node_idx].incoming.progress[tx_id].file_size = 0.;
            txq[orig_node_idx].incoming.progress[tx_id].total_bytes = 0.;
            txq[orig_node_idx].incoming.progress[tx_id].file_info.clear();

            return incoming_tx_recount(orig_node_id);
        }

        //! Completes an incoming file transfer
        //! Similar to incoming_tx_del() but doesn't reset the tx_id.
        //! Used when a file's data is fully received.
        //! \return 0 on success
        int32_t Transfer::incoming_tx_complete(const uint8_t orig_node_id, const PACKET_TX_ID_TYPE tx_id)
        {
            const size_t orig_node_idx = node_id_to_txq_idx(orig_node_id);
            if (orig_node_id == NODEIDUNKNOWN || orig_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_NODE;
            }

            if (txq[orig_node_idx].incoming.progress[tx_id].tx_id == NODEIDUNKNOWN)
            {
                return TRANSFER_ERROR_MATCH;
            }

            if (txq[orig_node_idx].incoming.size)
            {
                --txq[orig_node_idx].incoming.size;
            }

            // Move file to its final location
            // Close the DATA file
            if (txq[orig_node_idx].incoming.progress[tx_id].fp != nullptr)
            {
                fclose(txq[orig_node_idx].incoming.progress[tx_id].fp);
                txq[orig_node_idx].incoming.progress[tx_id].fp = nullptr;
            }
            string completed_filepath = txq[orig_node_idx].incoming.progress[tx_id].temppath + ".file";
            int iret = rename(completed_filepath.c_str(), txq[orig_node_idx].incoming.progress[tx_id].filepath.c_str());
            // Make sure metadata is recorded
            write_meta(txq[orig_node_idx].incoming.progress[tx_id], 0.);
            if (!iret && debug_log != nullptr)
            {
                debug_log->Printf("%.4f %.4f Incoming: Renamed/Data: %d %s\n", tet.split(), dt.lap(), iret, txq[orig_node_idx].incoming.progress[tx_id].filepath.c_str());
            }

            if (debug_log != nullptr)
            {
                debug_log->Printf("%.4f %.4f Incoming: Complete incoming: %u %s %s\n", tet.split(), dt.lap(), txq[orig_node_idx].incoming.progress[tx_id].tx_id, txq[orig_node_idx].incoming.progress[tx_id].node_name.c_str(), txq[orig_node_idx].incoming.progress[tx_id].file_name.c_str());
            }

            string filepath;
            // Remove the DATA file
            filepath = txq[orig_node_idx].incoming.progress[tx_id].temppath + ".file";
            remove(filepath.c_str());

            // Remove the META file
            filepath = txq[orig_node_idx].incoming.progress[tx_id].temppath + ".meta";
            remove(filepath.c_str());

            txq[orig_node_idx].incoming.progress[tx_id].complete = true;

            return 0;
        }

        //! Receive a chunk of file data
        //! Called by receive_packet() when a DATA packet is received.
        //! \param data Deserialized DATA packet
        //! \param node_id Origin node_id
        //! \param orig_node_idx Index of origin node id in the txq
        //! \return 0 on success, negative on error
        int32_t Transfer::incoming_tx_data(packet_struct_data& data, uint8_t node_id, size_t orig_node_idx)
        {
            int32_t iretn = 0;
            // create transaction entry if new, and then add data
            PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[orig_node_idx].incoming, data.header.tx_id);

            // If tx_id matches, but file_crc does not match, then cancel current transaction and begin a new one
            // Note, tx_id must be > 0
            if (txq[orig_node_idx].incoming.progress[tx_id].sentmeta && data.header.tx_id
            && txq[orig_node_idx].incoming.progress[tx_id].tx_id == data.header.tx_id
            && txq[orig_node_idx].incoming.progress[tx_id].file_crc != data.header.file_crc)
            {
                // This renaming would only occur in error states.
                // A file completed correctly would already be renamed and moved.
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Incoming: Data: File crc differs, closing existing transaction: %u %u %u %u %s %s %s\n", tet.split(), dt.lap(), txq[orig_node_idx].incoming.progress[data.header.tx_id].tx_id, txq[orig_node_idx].incoming.progress[data.header.tx_id].file_crc, data.header.tx_id, data.header.file_crc, txq[orig_node_idx].incoming.progress[data.header.tx_id].node_name.c_str(), txq[orig_node_idx].incoming.progress[data.header.tx_id].agent_name.c_str(), txq[orig_node_idx].incoming.progress[data.header.tx_id].file_name.c_str());
                }
                txq[orig_node_idx].incoming.progress[data.header.tx_id].filepath += "_DATATXID_ERR";
                incoming_tx_del(node_id, data.header.tx_id);
                tx_id = 0;
                // Fallthrough to DATA received for the first time code
            }

            // If transaction does not exist in incoming queue, then request META
            if (!tx_id)
            {
                tx_id = data.header.tx_id;
                string node_name = lookup_node_id_name(cinfo, node_id);
                if (node_name.empty())
                {
                    return TRANSFER_ERROR_INDEX;
                }
                iretn = incoming_tx_add(node_name, data.header.tx_id, data.header.file_crc);
                if (iretn <= 0) {
                    return iretn;
                }
            }

            // Request META if it hasn't been received yet and it's been awhile since we last requested it
            // Also to periodically keep the sender up-to-date on holes
            if (currentmjd() > txq[orig_node_idx].incoming.progress[tx_id].next_response)
            {
                txq[orig_node_idx].incoming.progress[tx_id].next_response = currentmjd() + txq[orig_node_idx].incoming.waittime;
                iretn = RESPONSE_REQUIRED;
            }

            // Regardless, receive the DATA anyway

            // tx_id now points to the valid entry to which we should add the data
            file_progress tp;
            tp.chunk_start = data.chunk_start;
            tp.chunk_end = data.chunk_start + data.byte_count - 1;

            // Attempt to add chunk to incoming queue
            bool updated = add_chunk(txq[orig_node_idx].incoming.progress[tx_id], tp);

            // Write to disk if this is new data
            if (updated)
            {
                string partial_filepath;
                // Write incoming data to disk
                if (txq[orig_node_idx].incoming.progress[tx_id].fp == nullptr)
                {
                    partial_filepath = txq[orig_node_idx].incoming.progress[tx_id].temppath + ".file";
                    if (data_exists(partial_filepath))
                    {
                        txq[orig_node_idx].incoming.progress[tx_id].fp = fopen(partial_filepath.c_str(), "r+");
                    }
                    else
                    {
                        txq[orig_node_idx].incoming.progress[tx_id].fp = fopen(partial_filepath.c_str(), "w");
                    }
                }

                // Check fp open success
                if (txq[orig_node_idx].incoming.progress[tx_id].fp == nullptr)
                {
                    if (debug_log != nullptr)
                    {
                        debug_log->Printf("%.4f %.4f Incoming: File Error: %s %s on ID: %u Chunk: %u\n", tet.split(), dt.lap(), partial_filepath.c_str(), cosmos_error_string(-errno).c_str(), tx_id, tp.chunk_start);
                    }
                    // TODO: Actually, what would you do here?
                    iretn = GENERAL_ERROR_NULLPOINTER;
                }
                else
                {
                    fseek(txq[orig_node_idx].incoming.progress[tx_id].fp, tp.chunk_start, SEEK_SET);
                    fwrite(data.chunk.data(), data.byte_count, 1, txq[orig_node_idx].incoming.progress[tx_id].fp);
                    fflush(txq[orig_node_idx].incoming.progress[tx_id].fp);
                    // Recalculate total periodically, if it's time to resend a REQDATA to the sender
                    if (iretn == RESPONSE_REQUIRED)
                    {
                        merge_chunks_overlap(txq[orig_node_idx].incoming.progress[tx_id]);
                    }
                    // Write latest meta data to disk
                    write_meta(txq[orig_node_idx].incoming.progress[tx_id]);
                    if (debug_log != nullptr)
                    {
                        // Leave this commented out since there are a lot of DATA packets flowing
                        //debug_log->Printf("%.4f %.4f Incoming: Received DATA/Write: %u bytes for tx_id: %u\n", tet.split(), dt.lap(), data.byte_count, tx_id);
                    }

                    // Check if all data has been received (we must have the METADATA)
                    // incoming total_bytes updated by add_chunks() or merge_chunks_overlap()
                    if (txq[orig_node_idx].incoming.progress[tx_id].sentmeta && txq[orig_node_idx].incoming.progress[tx_id].total_bytes >= txq[orig_node_idx].incoming.progress[tx_id].file_size)
                    {
                        // Merge chunks and recalculate actual total correctly
                        merge_chunks_overlap(txq[orig_node_idx].incoming.progress[tx_id]);
                        if (txq[orig_node_idx].incoming.progress[tx_id].total_bytes == txq[orig_node_idx].incoming.progress[tx_id].file_size)
                        {
                            // Now check the file_crc
                            int32_t crcret = calc_crc.calc_file(txq[orig_node_idx].incoming.progress[tx_id].temppath + ".file");
                            if (crcret < 0 || txq[orig_node_idx].incoming.progress[tx_id].file_crc != crcret)
                            {
                                if (debug_log != nullptr)
                                {
                                    debug_log->Printf("%.4f %.4f Incoming/Data: Error in final crc check. Was %d expected %u\n", tet.split(), dt.lap(), crcret, txq[orig_node_idx].incoming.progress[tx_id].file_crc);
                                }
                                txq[orig_node_idx].incoming.progress[tx_id].filepath += "_FILECRC_ERR";
                                incoming_tx_del(node_id, tx_id);
                            }
                            else
                            {
                                // Received file's checksum matches sender's checksum, file is succesfully received
                                // Mark as all data sent over
                                txq[orig_node_idx].incoming.progress[tx_id].sentdata = true;
                                // Move file over to final destination
                                incoming_tx_complete(node_id, tx_id);
                                iretn = RESPONSE_REQUIRED;
                            }
                        }
                    }
                }
            }
            if (iretn == RESPONSE_REQUIRED)
            {
                if (std::find(txq[orig_node_idx].incoming.respond.begin(), txq[orig_node_idx].incoming.respond.end(), tx_id) == txq[orig_node_idx].incoming.respond.end())
                {
                    txq[orig_node_idx].incoming.respond.push_back(tx_id);
                }
            }

            return iretn;
        }

        //! Handles receiving of incoming file transfer-type packet.
        //! Make sure to process the PacketComm packet before calling this (with SLIPIn() or equivalent).
        //! \param packet PacketComm packet containing file transfer-type data
        //! \param response_queue Any l-type responses (e.g., CANCEL) are pushed onto this vector.
        //! \return 0 on success, RESPONSE_REQUIRED if get_outgoing_packets() needs to be called
        int32_t Transfer::receive_packet(const PacketComm& packet)
        {
            int32_t iretn = 0;
            if (!packet.data.size())
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            // Reject if versions don't match
            if (packet.data[0] != FILE_TRANSFER_PROTOCOL_VERSION)
            {
                return TRANSFER_ERROR_VERSION;
            }
            const uint8_t node_id = packet.data[1];
            const size_t orig_node_idx = node_id_to_txq_idx(node_id);
            if (orig_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_INDEX;
            }

            // Respond appropriately according to type of packet
            switch (packet.header.type)
            {
            case PacketComm::TypeId::DataFileQueue:
                {
                    packet_struct_queue queue;

                    deserialize_queue(packet.data, queue);

                    txq[orig_node_idx].incoming.sentqueue = true;

                    PACKET_TX_ID_TYPE tx_id = 0;

                    // Iterate over every group of flags
                    for (auto flags : queue.tx_ids)
                    {
                        for (size_t i = 0; i < sizeof(flags) * 8; ++i)
                        {
                            bool flag = flags & 1;
                            // If a file is in the QUEUE packet but not in our local queue, add it
                            if (flag && !(txq[orig_node_idx].incoming.progress[tx_id].tx_id))
                            {
                                string node_name = lookup_node_id_name(cinfo, queue.header.node_id);
                                if (node_name.empty())
                                {
                                    continue;
                                }
                                incoming_tx_add(node_name, tx_id, 0);
                            }
                            // If a file is in our local queue but not in the QUEUE packet, delete it
                            // TODO: I think this logic fails for multiple senders
                            else if (!flag && txq[orig_node_idx].incoming.progress[tx_id].tx_id)
                            {
                                // This renaming would only occur in error states.
                                // A file completed correctly would already be renamed and moved.
                                txq[orig_node_idx].incoming.progress[tx_id].filepath += "_QUEUE_ERR";
                                incoming_tx_del(node_id, tx_id);
                            }

                            flags = flags >> 1;
                            ++tx_id;
                        }
                    }
                }
                break;
            case PacketComm::TypeId::DataFileReqMeta:
                {
                    packet_struct_reqmeta reqmeta;

                    deserialize_reqmeta(packet.data, reqmeta);

                    // Mark QUEUE for resending, clarify to receiver what it ought to be receiving
                    txq[orig_node_idx].outgoing.sentqueue = false;

                    // Send requested META packets
                    PACKET_TX_ID_TYPE tx_id = 0;
                    // Iterate over every group of flags
                    for (auto flags : reqmeta.tx_ids)
                    {
                        for (size_t i = 0; i < sizeof(flags) * 8; ++i)
                        {
                            bool flag = flags & 1;
                            // If this tx_id's METADATA is being requested and the tx_id is valid, set sentmeta flag to false to resend
                            if (flag)
                            {
                                // This is a valid outgoing tx_id, resend META
                                if (check_tx_id(txq[orig_node_idx].outgoing, tx_id) > 0)
                                {
                                    txq[orig_node_idx].outgoing.progress[tx_id].sentmeta = false;
                                }
                                // This is no longer a valid outgoing tx_id, cancel the transaction
                                else
                                {
                                    // Set to enable for get_outgoing_lpackets() to handle,
                                    // but tx_id = 0 indicates the erroneous state of the transaction
                                    txq[orig_node_idx].outgoing.progress[tx_id].enabled = true;
                                    txq[orig_node_idx].outgoing.progress[tx_id].tx_id = 0;
                                }
                            }

                            flags = flags >> 1;
                            ++tx_id;
                        }
                    }
                }
                break;
            case PacketComm::TypeId::DataFileMetaData:
                {
                    packet_struct_metadata meta;

                    deserialize_metadata(packet.data, meta);

                    iretn = incoming_tx_update(meta);
                    if (iretn > 0)
                    {
                        iretn = 0;
                    }
                }
                break;
            case PacketComm::TypeId::DataFileReqData:
                {
                    packet_struct_reqdata reqdata;

                    int32_t start_end_signifier = deserialize_reqdata(packet.data, reqdata);
                    if (start_end_signifier < 0)
                    {
                        break;
                    }

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[orig_node_idx].outgoing, reqdata.header.tx_id);
                    
                    if (tx_id <= 0)
                    {
                        break;
                    }
                    // tx_id now points to the valid entry to which we should add the data
                    // Add this chunk to the queue
                    bool updated = add_chunks(txq[orig_node_idx].outgoing.progress[tx_id], reqdata.holes, start_end_signifier);
                    
                    // Recalculate chunks
                    merge_chunks_overlap(txq[orig_node_idx].outgoing.progress[tx_id]);

                    // Save meta to disk
                    if (updated)
                    {
                        write_meta(txq[orig_node_idx].outgoing.progress[tx_id]);
                        txq[orig_node_idx].outgoing.progress[tx_id].sentdata = false;
                    }
                }
                break;
            case PacketComm::TypeId::DataFileChunkData:
                {
                    packet_struct_data data;

                    deserialize_data(packet.data, data);

                    iretn = incoming_tx_data(data, node_id, orig_node_idx);
                }
                break;
            case PacketComm::TypeId::DataFileReqComplete:
                {
                    packet_struct_reqcomplete reqcomplete;

                    deserialize_reqcomplete(packet.data, reqcomplete);

                    // Check if file_crc is the same. If they differ, this is a new transaction. If so, cancel current entry and move
                    if (txq[orig_node_idx].incoming.progress[reqcomplete.header.tx_id].sentmeta && reqcomplete.header.tx_id
                    && txq[orig_node_idx].incoming.progress[reqcomplete.header.tx_id].tx_id == reqcomplete.header.tx_id
                    && txq[orig_node_idx].incoming.progress[reqcomplete.header.tx_id].file_crc != reqcomplete.header.file_crc)
                    {
                        // This renaming would only occur in error states.
                        // A file completed correctly would already be renamed and moved.
                        if (debug_log != nullptr)
                        {
                            debug_log->Printf("%.4f %.4f Incoming: ReqComplete: File crc differs, closing existing transaction: %u %u %u %u %s %s %s\n", tet.split(), dt.lap(), txq[orig_node_idx].incoming.progress[reqcomplete.header.tx_id].tx_id, txq[orig_node_idx].incoming.progress[reqcomplete.header.tx_id].file_crc, reqcomplete.header.tx_id, reqcomplete.header.file_crc, txq[orig_node_idx].incoming.progress[reqcomplete.header.tx_id].node_name.c_str(), txq[orig_node_idx].incoming.progress[reqcomplete.header.tx_id].agent_name.c_str(), txq[orig_node_idx].incoming.progress[reqcomplete.header.tx_id].file_name.c_str());
                        }
                        txq[orig_node_idx].incoming.progress[reqcomplete.header.tx_id].filepath += "_REQCTXID_ERR";
                        incoming_tx_del(node_id, reqcomplete.header.tx_id);
                        // Fallthrough to REQCOMPLETE received for the first time code
                    }

                    // No transaction ID, so metadata needs to be requested first
                    if (std::find(txq[orig_node_idx].incoming.respond.begin(), txq[orig_node_idx].incoming.respond.end(), reqcomplete.header.tx_id) == txq[orig_node_idx].incoming.respond.end())
                    {
                        // cout << "REQCOMPLETE for unknown txid encountered: " << unsigned(reqcomplete.header.tx_id) << endl;
                        txq[orig_node_idx].incoming.respond.push_back(reqcomplete.header.tx_id);
                    }
                    iretn = RESPONSE_REQUIRED;
                }
                break;
            case PacketComm::TypeId::DataFileComplete:
                {
                    packet_struct_complete complete;

                    deserialize_complete(packet.data, complete);

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[orig_node_idx].outgoing, complete.header.tx_id);
                    if (tx_id > 0)
                    {
                        txq[orig_node_idx].outgoing.progress[tx_id].complete = true;
                        txq[orig_node_idx].outgoing.progress[tx_id].sentdata = true;
                        txq[orig_node_idx].outgoing.progress[tx_id].sentmeta = true;
                    }
                }
                break;
            case PacketComm::TypeId::DataFileCancel:
                {
                    packet_struct_cancel cancel;
                    deserialize_cancel(packet.data, cancel);

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[orig_node_idx].incoming, cancel.header.tx_id);
                    if (tx_id > 0)
                    {
                        // Remove the transaction
                        // This renaming would only occur in error states.
                        // A file completed correctly would already be renamed and moved.
                        txq[orig_node_idx].incoming.progress[tx_id].filepath += "_CANCEL_ERR";
                        incoming_tx_del(node_id, tx_id);
                    }
                }
                break;
            default:
                return COSMOS_PACKET_TYPE_MISMATCH;
                break;
            }

            return iretn;
        }

        int32_t Transfer::incoming_tx_recount(const string node_name)
        {
            return incoming_tx_recount(lookup_node_id(cinfo, node_name));
        }

        int32_t Transfer::incoming_tx_recount(const uint8_t node_id)
        {
            const size_t orig_node_idx = node_id_to_txq_idx(node_id);
            if (node_id == NODEIDUNKNOWN || orig_node_idx == INVALID_TXQ_IDX)
            {
                return TRANSFER_ERROR_INDEX;
            }

            txq[orig_node_idx].incoming.size = 0;
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[orig_node_idx].incoming.progress[i].tx_id)
                {
                    ++txq[orig_node_idx].incoming.size;
                }
            }
            return txq[orig_node_idx].incoming.size;
        }

        //! Save current progress information to disk
        //! Stores metadata of current progress status to a .meta file.
        //! Note that this function isn't called on a crash or agent stop, so transfer progress will resume from last call.
        //! \param tx A tx_progress to save
        //! \param interval How long (in MJD) from previous call it must be before log file is updated
        //! \return 0 on success, negative on error
        int32_t Transfer::write_meta(tx_progress& tx, const double interval)
        {
            std::ofstream file_name;

            if (currentmjd(0.) - tx.savetime > interval/86400.)
            {
                tx.savetime = currentmjd(0.);
                serialize_metafile(meta_file_data, tx.tx_id, tx.file_crc, tx.file_name, tx.file_size, tx.node_name, tx.agent_name);
                file_name.open(tx.temppath + ".meta", std::ios::out|std::ios::binary); // Note: truncs by default
                if(!file_name.is_open())
                {
                    return (-errno);
                }

                // Structure of a .meta file:
                // Byte 0: FILE_TRANSFER_METAFILE_VERSION
                // Byte 1-4: packet size (n)
                // Byte 5-6: packet crc
                // Byte 7-(7+n): packet metalong struct
                // Byte (7+n)-end: file_progress structs[]

                // where file_progress structs[] are a consecutive list of the following:
                // Byte 0-7: file_progress
                // Byte 8-9: crc
                // ... repeat above structure per hole
                file_name.write((char *)&FILE_TRANSFER_METAFILE_VERSION, sizeof(uint8_t));
                uint16_t crc = calc_crc.calc(meta_file_data);
                size_t packet_size = meta_file_data.size();
                file_name.write((char *)&packet_size, sizeof(packet_size));
                file_name.write((char *)&crc, sizeof(crc));
                file_name.write((char *)&meta_file_data[0], meta_file_data.size());
                for (file_progress progress_info : tx.file_info)
                {
                    file_name.write((const char *)&progress_info, sizeof(progress_info));
                    crc = calc_crc.calc((uint8_t *)&progress_info, sizeof(progress_info));
                    file_name.write((char *)&crc, 2);
                }
                file_name.close();
            }

            return 0;
        }

        //! Read in-progress file from a previous run
        //! \return 0 on success; if error, will remove the .meta and .file files and return a negative error value
        int32_t Transfer::read_meta(tx_progress& tx)
        {
            std::ifstream file_name;

            struct stat statbuf;
            if (!stat((tx.temppath + ".meta").c_str(), &statbuf) && statbuf.st_size >= COSMOS_SIZEOF(file_progress))
            {
                file_name.open(tx.temppath + ".meta", std::ios::in|std::ios::binary);
                if(!file_name.is_open())
                {
                    return (-errno);
                }
            }
            else
            {
                return DATA_ERROR_SIZE_MISMATCH;
            }

            tx.fp = nullptr;
            tx.savetime = 0.;
            tx.complete = false;

            // Structure of a .meta file:
            // Byte 0: FILE_TRANSFER_METAFILE_VERSION
                // Byte 1-4: packet size (n)
                // Byte 5-6: packet crc
                // Byte 7-(7+n): packet metalong struct
                // Byte (7+n)-end: file_progress structs[]

            // where file_progress structs[] are a consecutive list of the following:
            // Byte 0-7: file_progress
            // Byte 8-9: crc
            // ... repeat above structure per hole

            // Load metadata
            // Reject if versions don't match
            uint8_t metafile_version;
            file_name.read((char *)&metafile_version, sizeof(uint8_t));
            if (metafile_version != FILE_TRANSFER_METAFILE_VERSION)
            {
                file_name.close();
                remove((tx.temppath + ".meta").c_str());
                remove((tx.temppath + ".file").c_str());
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: read_meta: %s Error wrong metafile version. Was %u expected %u\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str(), unsigned(metafile_version), unsigned(FILE_TRANSFER_METAFILE_VERSION));
                }
                return TRANSFER_ERROR_VERSION;
            }
            size_t packet_size;
            file_name.read((char *)&packet_size, sizeof(packet_size));
            // Check for file OoB or an absurd packet_size
            if (file_name.eof() || packet_size > 9999)
            {
                file_name.close();
                remove((tx.temppath + ".meta").c_str());
                remove((tx.temppath + ".file").c_str());
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: read_meta: %s Error in packet_size read. Removing meta\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str());
                }
                return DATA_ERROR_SIZE_MISMATCH;
            }
            uint16_t crc;
            file_name.read((char *)&crc, sizeof(crc));
            if (file_name.eof())
            {
                file_name.close();
                remove((tx.temppath + ".meta").c_str());
                remove((tx.temppath + ".file").c_str());
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: read_meta: %s Error in crc read. Removing meta\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str());
                }
                return DATA_ERROR_SIZE_MISMATCH;
            }
            vector<PACKET_BYTE> bytes;
            // Handle packet_size being exceptionally large enough to trigger a bad_alloc exception
            try {
                bytes.resize(packet_size);
            }
            catch (std::bad_alloc& e)
            {
                file_name.close();
                remove((tx.temppath + ".meta").c_str());
                remove((tx.temppath + ".file").c_str());
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: read_meta: %s bad_alloc exception. Removing meta\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str());
                }
                return DATA_ERROR_SIZE_MISMATCH;
            }
            file_name.read((char *)bytes.data(), packet_size);
            if (file_name.eof())
            {
                file_name.close();
                remove((tx.temppath + ".meta").c_str());
                remove((tx.temppath + ".file").c_str());
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: read_meta: %s Error in packet data read. Removing meta\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str());
                }
                return DATA_ERROR_SIZE_MISMATCH;
            }
            if (crc != calc_crc.calc(bytes.data(), bytes.size()))
            {
                file_name.close();
                remove((tx.temppath + ".meta").c_str());
                remove((tx.temppath + ".file").c_str());
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: read_meta: %s Error in crc check. Removing meta\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str());
                }
                return DATA_ERROR_CRC;
            }
            packet_struct_metafile meta;
            deserialize_metafile(bytes, meta);

            // Check for absurd data sizes that would happen on attempting to
            // ingest outdated .meta formats or file corruption
            if (meta.node_name_len > COSMOS_MAX_NAME || meta.agent_name_len > COSMOS_MAX_NAME || meta.file_name_len > TRANSFER_MAX_FILENAME)
            {
                file_name.close();
                remove((tx.temppath + ".meta").c_str());
                remove((tx.temppath + ".file").c_str());
                if (debug_log != nullptr)
                {
                    debug_log->Printf("%.4f %.4f Main: read_meta: %s Error in deserialize. Removing meta\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str());
                }
                return DATA_ERROR_SIZE_MISMATCH;
            }

            tx.tx_id = meta.tx_id;
            tx.file_crc = meta.file_crc;
            tx.sentmeta = !!meta.file_name.size(); // META was obtained if file_name is known
            // Origin node name
            tx.node_name = meta.node_name;
            // Destination agent name
            tx.agent_name = meta.agent_name;
            tx.file_name = meta.file_name;
            // filepath is immediately overwritten by subsequent functions, so don't worry about this
            tx.filepath = "";
            tx.file_size = meta.file_size;

            // load file progress
            file_progress progress_info;
            do
            {
                file_name.read((char *)&progress_info, sizeof(progress_info));
                if (file_name.eof())
                {
                    break;
                }
                uint16_t crc;
                file_name.read((char *)&crc, 2);
                if (file_name.eof())
                {
                    file_name.close();
                    remove((tx.temppath + ".meta").c_str());
                    remove((tx.temppath + ".file").c_str());
                    if (debug_log != nullptr)
                    {
                        debug_log->Printf("%.4f %.4f Main: read_meta: %s Error in progress crc read. Removing meta\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str());
                    }
                    return DATA_ERROR_SIZE_MISMATCH;
                }
                if (crc != calc_crc.calc((uint8_t *)&progress_info, sizeof(progress_info)))
                {
                    file_name.close();
                    remove((tx.temppath + ".meta").c_str());
                    remove((tx.temppath + ".file").c_str());
                    if (debug_log != nullptr)
                    {
                        debug_log->Printf("%.4f %.4f Main: read_meta: %s Error in progress crc check. Removing meta\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str());
                    }
                    return DATA_ERROR_CRC;
                }
                tx.file_info.push_back(progress_info);
            } while(!file_name.eof());
            file_name.close();
            if (debug_log != nullptr)
            {
                debug_log->Printf("%.4f %.4f Main: read_meta: %s tx_id: %d file_crc: %u chunks: %lu\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str(), tx.tx_id, tx.file_crc, tx.file_info.size());
            }

            return 0;
        }

    }
}

//! Set the enabled status of an outgoing tx_id
//! \param node_id ID of receiving node in the node table
//! \param tx_id Transfer ID of file
//! \param enabled Set to mark file for transfer
//! \return 0 on success
int32_t Transfer::set_enabled(const uint8_t node_id, const PACKET_TX_ID_TYPE tx_id, const bool enabled)
{
    const size_t node_idx = node_id_to_txq_idx(node_id);
    if (node_id == NODEIDUNKNOWN || node_idx == INVALID_TXQ_IDX)
    {
        return TRANSFER_ERROR_INDEX;
    }
    if (check_tx_id(txq[node_idx].outgoing, tx_id) <= 0)
    {
        return TRANSFER_ERROR_MATCH;
    }

    txq[node_idx].outgoing.progress[tx_id].enabled = enabled;

    return 0;
}

//! Enable a single outgoing file
//! Sets the enabled status of a single outgoing file to true,
//! and sets the enabled status of all other outgoing files of the
//! node_id to false.
//! If the file is not found, no changes are made.
//! \param node_name Name of receiving node
//! \param file_name Name of file
//! \return Number of files enabled
int32_t Transfer::enable_single(const string node_name, const string file_name)
{
    if (node_name.empty())
    {
        return TRANSFER_ERROR_NODE;
    }
    uint8_t node_id = lookup_node_id(cinfo, node_name);
    return enable_single(node_id, file_name);
}

//! Enable a single outgoing file
//! Sets the enabled status of a single outgoing file to true,
//! and sets the enabled status of all other outgoing files of the
//! node_id to false.
//! If the file is not found, no changes are made.
//! \param node_id ID of receiving node
//! \param file_name Name of file
//! \return Number of files enabled
int32_t Transfer::enable_single(const uint8_t node_id, const string file_name)
{
    const size_t node_idx = node_id_to_txq_idx(node_id);
    if (node_id == NODEIDUNKNOWN || node_idx == INVALID_TXQ_IDX)
    {
        return TRANSFER_ERROR_INDEX;
    }
    if (file_name.empty())
    {
        return TRANSFER_ERROR_FILENAME;
    }

    bool file_found = false;

    // Attempt to find the file first
    for (auto &tx_out : txq[node_idx].outgoing.progress)
    {
        if (!tx_out.tx_id)
        {
            continue;
        }

        if (tx_out.file_name.size() && tx_out.file_name == file_name)
        {
            file_found = true;
            break;
        }
    }
    // Return error if not found
    if (!file_found)
    {
        return TRANSFER_ERROR_FILENAME;
    }

    // If the file was found, go through the outgoing queue again,
    // this time actually making changes.
    for (auto &tx_out : txq[node_idx].outgoing.progress)
    {
        if (!tx_out.tx_id)
        {
            continue;
        }

        if (tx_out.file_name.size() && tx_out.file_name == file_name)
        {
            tx_out.enabled = true;
        }
        else
        {
            tx_out.enabled = false;
        }
    }

    return 1;
}

//! Enable all outgoing files
//! Sets the enabled status of all outgoing files to true.
//! \param node_name Name of the receiving node
//! \return Number of files enabled
int32_t Transfer::enable_all(const string node_name)
{
    if (node_name.empty())
    {
        return TRANSFER_ERROR_NODE;
    }
    uint8_t node_id = lookup_node_id(cinfo, node_name);
    return enable_all(node_id);
}

//! Enable all outgoing files
//! Sets the enabled status of all outgoing files to true.
//! \param node_id ID of receiving node
//! \return Number of files enabled
int32_t Transfer::enable_all(const uint8_t node_id)
{
    const size_t node_idx = node_id_to_txq_idx(node_id);
    if (node_id == NODEIDUNKNOWN || node_idx == INVALID_TXQ_IDX)
    {
        return TRANSFER_ERROR_INDEX;
    }

    int32_t iretn = 0;

    for (auto &tx_out : txq[node_idx].outgoing.progress)
    {
        // Check if this file is valid
        if (!tx_out.tx_id)
        {
            continue;
        }

        tx_out.enabled = true;
        ++iretn;
    }

    return iretn;
}

//! Get a list of all files in the outgoing queue.
//! Returns a json string list of outgoing files, with the following keys:
//! - tx_id: The transaction ID
//! - file_name: Name of the file
//! - file_size: Size of the file (in bytes)
//! - node_name: The name of the node to send to
//! - enabled: Whether the file is marked for transfer
//! \return json string list of outgoing files
string Transfer::list_outgoing()
{
    vector<json11::Json> jlist;
    for (auto it = node_id_to_txq_map.begin(); it != node_id_to_txq_map.end(); ++it)
    {
        for(tx_progress tx : txq[it->second].outgoing.progress)
        {
            if (tx.tx_id)
            {
                jlist.push_back(json11::Json::object {
                    { "tx_id", tx.tx_id },
                    { "file_crc", tx.file_crc },
                    { "file_name", tx.file_name },
                    { "file_size", static_cast<int>(tx.file_size) },
                    { "node_name", tx.node_name },
                    { "enabled", tx.enabled }
                });
            }
        }
    }
    string s = json11::Json(jlist).dump();

    return s;
}

//! Get a list of all files in the incoming queue.
//! Returns a json string list of incoming files, with the following keys:
//! - tx_id: The transaction ID
//! - file_name: Name of the file
//! - file_size: Size of the full file (in bytes)
//! - total_bytes: Total bytes received so far
//! - sent_meta: Whether the meta has been received for this file
//! \return json string list of incoming files
string Transfer::list_incoming()
{
    vector<json11::Json> jlist;
    for (auto it = node_id_to_txq_map.begin(); it != node_id_to_txq_map.end(); ++it)
    {
        for(tx_progress tx : txq[it->second].incoming.progress)
        {
            if (tx.tx_id)
            {
                jlist.push_back(json11::Json::object {
                    { "tx_id", tx.tx_id },
                    { "file_crc", tx.file_crc },
                    { "file_name", tx.file_name },
                    { "file_size", static_cast<int>(tx.file_size) },
                    { "total_bytes", static_cast<int>(tx.total_bytes) },
                    { "sent_meta", tx.sentmeta }
                });
            }
        }
    }
    string s = json11::Json(jlist).dump();

    return s;
}

//! Set the waittime for a transfer
//! \param node_id ID in the node table
//! \param direction 0 for incoming, 1 for outgoing
//! \param waittime Time interval to wait before sending another response or request-type packet, in seconds
//! \return 0 on success, negative on error
int32_t Transfer::set_waittime(const uint8_t node_id, const uint8_t direction, const double waittime)
{
    const size_t node_idx = node_id_to_txq_idx(node_id);
    if (node_id == NODEIDUNKNOWN || node_idx == INVALID_TXQ_IDX)
    {
        return TRANSFER_ERROR_INDEX;
    }

    switch(direction)
    {
    case 0:
        txq[node_idx].incoming.waittime = waittime/86400.;
        break;
    case 1:
        txq[node_idx].outgoing.waittime = waittime/86400.;
        break;
    default:
        txq[node_idx].incoming.waittime = waittime/86400.;
        txq[node_idx].outgoing.waittime = waittime/86400.;
        break;
    }
    
    return 0;
}

int32_t Transfer::set_waittime(const string node_name, const uint8_t direction, const double waittime)
{
    return set_waittime(lookup_node_id(cinfo, node_name), direction, waittime);
}

//! Get the currently configured size of the packet that transferclass is using
PACKET_CHUNK_SIZE_TYPE Transfer::get_packet_size()
{
    return packet_size;
}

//! Sets the size of the packet to stuff. Should be set to the size of packet the channel supports that file packets will be sent out on.
int32_t Transfer::set_packet_size(const PACKET_CHUNK_SIZE_TYPE size)
{
    packet_size = size;
    return 0;
}

/// Obtains the corresponding txq_idx mapped from a node_id.
/// If an out_of_range exception was raised (node_id was not found),
/// then the value INVALID_TXQ_IDX will be returned.
size_t Transfer::node_id_to_txq_idx(const uint8_t node_id)
{
    try
    {
        return node_id_to_txq_map.at(node_id);
    }
    catch (const std::out_of_range& oor)
    {
        return INVALID_TXQ_IDX;
    }
}

/**
 * @brief Resets queues, clearing out everything inside to the defaults.
 * 
 * Also clears out all temporary files for specified nodes
 * 
 * @param node_id ID of node
 * @param direction 0 to clear incoming, 1 to clear outgoing, 2 to clear both
 * @return int32_t 
 */
int32_t Transfer::reset_queue(uint8_t node_id, uint8_t direction)
{
    const size_t txq_idx = node_id_to_txq_idx(node_id);
    if (txq_idx == INVALID_TXQ_IDX)
    {
        return TRANSFER_ERROR_NODE;
    }

    int32_t iretn = 0;
    switch(direction)
    {
    // Clear incoming
    case 0:
        iretn = clear_tx_entry(txq[txq_idx].incoming);
        break;
    // Clear outgoing
    case 1:
        iretn = clear_tx_entry(txq[txq_idx].outgoing);
        break;
    // Clear both
    default:
        {
            string node_name = txq[txq_idx].node_name;
            clear_tx_entry(txq[txq_idx].incoming);
            clear_tx_entry(txq[txq_idx].outgoing);
            // Also clear out the entire temp folder for the node
            string nodes_folder_path;
            if (get_cosmosnodes(nodes_folder_path) < 0 || nodes_folder_path.empty())
            {
                break;
            }
            rmdir_contents(nodes_folder_path + "/" + node_name + "/temp/file");
        }
        break;
    }
    return iretn;
}

void Transfer::print_file_packet(PacketComm packet, uint8_t direction, string type, Log::Logger* debug_log)
{
    if (packet.header.type == PacketComm::TypeId::DataFileChunkData)
    {
        // For DATA-type packets, print only the first time it comes in. Comment out these lines to skip all DATA-packet logging
        // PACKET_FILE_SIZE_TYPE chunk_start;
        // memmove(&chunk_start, &packet.data[0]+PACKET_DATA_OFFSET_CHUNK_START, sizeof(chunk_start));
        // if (chunk_start != 0)
        if (!verbose_log)
        {
            return;
        }
    }

    if (debug_log->Type())
    {
        string node_name = lookup_node_id_name(cinfo, packet.data[0]);
        uint8_t node_id = check_node_id(cinfo, packet.data[0]);

        if (direction == 0)
        {
            debug_log->Printf("%.4f %.4f RECV L %u R %u %s [%s] Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), type.c_str(), packet.data.size());
        }
        else if (direction == 1)
        {
            debug_log->Printf("%.4f %.4f SEND L %u R %u %s [%s] Size: %lu ", tet.split(), dt.lap(), node_id, node_id, node_name.c_str(), type.c_str(), packet.data.size());
        }

        switch (packet.header.type)
        {
        case PacketComm::TypeId::DataFileMetaData:
            {
                packet_struct_metadata meta;
                deserialize_metadata(packet.data, meta);
                debug_log->Printf("[METADATA] v%u %u %u %s ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_metadata, header.tx_id)], meta.file_name.c_str());
                break;
            }
        case PacketComm::TypeId::DataFileChunkData:
            {
                debug_log->Printf("[DATA] v%u %u %u %u %u ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_data, header.tx_id)], packet.data[offsetof(packet_struct_data, chunk_start)]+256U*(packet.data[offsetof(packet_struct_data, chunk_start)+1]+256U*(packet.data[offsetof(packet_struct_data, chunk_start)+2]+256U*packet.data[offsetof(packet_struct_data, chunk_start)+3])), packet.data[offsetof(packet_struct_data, byte_count)]+256U*packet.data[offsetof(packet_struct_data, byte_count)+1]);
                break;
            }
        case PacketComm::TypeId::DataFileReqData:
            {
                debug_log->Printf("[REQDATA] v");
                for (auto& byte : packet.data)
                {
                    debug_log->Printf("%u ", unsigned(byte));
                }
                break;
            }
        case PacketComm::TypeId::DataFileReqComplete:
            {
                debug_log->Printf("[REQCOMPLETE] v%u %u %u ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_reqcomplete, header.tx_id)]);
                break;
            }
        case PacketComm::TypeId::DataFileComplete:
            {
                debug_log->Printf("[COMPLETE] v%u %u %u ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_complete, header.tx_id)]);
                break;
            }
        case PacketComm::TypeId::DataFileCancel:
            {
                debug_log->Printf("[CANCEL] v%u %u %u ", packet.data[offsetof(packet_struct_metadata, header.version)], node_id, packet.data[offsetof(packet_struct_cancel, header.tx_id)]);
                break;
            }
        case PacketComm::TypeId::DataFileReqMeta:
        case PacketComm::TypeId::DataFileQueue:
            {
                packet_struct_queue queue;
                deserialize_queue(packet.data, queue);
                string label = packet.header.type == PacketComm::TypeId::DataFileReqMeta ? "REQMETA" : "QUEUE";
                debug_log->Printf("[%s] v%u %u ", label.c_str(), packet.data[offsetof(packet_struct_metadata, header.version)], node_id);
                // Note: this assumes that PACKET_QUEUE_FLAGS_TYPE is a uint8_t type
                for (PACKET_QUEUE_FLAGS_TYPE i=0; i<PACKET_QUEUE_FLAGS_LIMIT; ++i)
                {
                    PACKET_QUEUE_FLAGS_TYPE flags = queue.tx_ids[i];
                    //debug_log->Printf("[%u] ", flags);
                    PACKET_TX_ID_TYPE hi = i << 3;
                    for (size_t bit = 0; bit < sizeof(PACKET_QUEUE_FLAGS_TYPE)*8; ++bit)
                    {
                        uint8_t flag = (flags >> bit) & 1;
                        if (!flag)
                        {
                            continue;
                        }
                        PACKET_TX_ID_TYPE tx_id = hi | bit;
                        debug_log->Printf("%u ", unsigned(tx_id));
                    }
                }
            }
            break;
        default:
            {
                debug_log->Printf("[OTHER] %u %s", node_id, "Non-file transfer type in packet.header.type");
            }
        }
        debug_log->Printf("\n");
    }

    return;
}
