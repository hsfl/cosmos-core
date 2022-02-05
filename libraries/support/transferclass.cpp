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
         * \param calling_node_name Name of node using this instance of transferclass
         * \return 0 if success
         */
        int32_t Transfer::Init(string calling_node_name)
        {
            int32_t iretn = Init(calling_node_name, nullptr);
            return iretn;
        }

        /** 
         * Initialize the transfer class
         * \param calling_node_name Name of node using this instance of transferclass
         * \param debug_error Pointer to an agent's debug_error to use to print error messages.
         * \return 0 if success
         */
        int32_t Transfer::Init(string calling_node_name, Error* debug_error)
        {
            int32_t iretn;
            this->debug_error = debug_error;

            packet_size = 217;

            // Initialize Transfer Queue
            if ((iretn = NodeData::load_node_ids()) < 2)
            {
                if (this->debug_error != nullptr)
                {
                    this->debug_error->Printf("%.4f Couldn't load node lookup table\n", tet.split());
                }
                return iretn;
            }
            txq.resize(iretn);
            for (uint16_t i=1; i<iretn; ++i)
            {
                if (NodeData::check_node_id(i) > 0)
                {
                    txq[i].node_id = i;
                    txq[i].node_name = NodeData::lookup_node_id_name(i);
                }
            }

            // Identify and store calling node's node_id
            iretn = NodeData::lookup_node_id(calling_node_name);
            if (iretn < 0)
            {
                if (this->debug_error != nullptr)
                {
                    this->debug_error->Printf("%.4f Could not find node in node table!\n", tet.split());
                }
                return iretn;
            }
            self_node_id = iretn;

            // Restore in progress transfers from previous run
            for (string node_name : data_list_nodes())
            {
                for(filestruc file : data_list_files(node_name, "temp", "file"))
                {
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
                            }
                        }

                        // Outgoing
                        if (!file.name.compare(0,4,"out_"))
                        {
                            tx_progress tx_out;
                            tx_out.temppath = file.path.substr(0,file.path.find(".meta"));
                            if (read_meta(tx_out) >= 0)
                            {
                                merge_chunks_overlap(tx_out);
                                iretn = outgoing_tx_add(tx_out);
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
            int32_t iretn = 0;

            // Go through outgoing queues for all nodes
            for (uint8_t node_id = 0; node_id < txq.size(); ++node_id) {
                if (node_id == self_node_id)
                {
                    continue;
                }
                iretn += outgoing_tx_load(node_id);
            }
            return iretn;
        }

        //! Scan the outgoing directory of the specified node in txq.
        //! Enqueues new files in its outgoing queue.
        //! Not thread safe.
        //! \return Number of files in the outgoing queue
        int32_t Transfer::outgoing_tx_load(string node_name)
        {
            int32_t iretn = NodeData::lookup_node_id(node_name);
            if (iretn < 0)
            {
                return iretn;
            }
            return outgoing_tx_load(iretn);
        }

        //! Scan the outgoing directory of the specified node in txq.
        //! Enqueues new files in its outgoing queue.
        //! Not thread safe.
        //! \return Number of files in this node's outgoing queue
        int32_t Transfer::outgoing_tx_load(uint8_t node_id)
        {
            // Go through outgoing queues, removing files that no longer exist
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[node_id].outgoing.progress[i].tx_id != 0 && !data_isfile(txq[node_id].outgoing.progress[i].filepath))
                {
                    outgoing_tx_del(node_id, txq[(node_id)].outgoing.progress[i].tx_id);
                }
            }
            // Go through outgoing directories, adding files not already in queue
            if (txq[(node_id)].outgoing.size < PROGRESS_QUEUE_SIZE-1)
            {
                vector<filestruc> file_names;
                for (filestruc file : data_list_files(txq[(node_id)].node_name, "outgoing", ""))
                {
                    if (file.type == "directory")
                    {
                        data_list_files(txq[(node_id)].node_name, "outgoing", file.name, file_names);
                    }
                }

                // Sort list by size, then go through list of files found, adding to queue.
                sort(file_names.begin(), file_names.end(), filestruc_smaller_by_size);
                for(uint16_t i=0; i<file_names.size(); ++i)
                {
                    filestruc file = file_names[i];
                    if (txq[(node_id)].outgoing.size == PROGRESS_QUEUE_SIZE - 1)
                    {
                        break;
                    }

                    //Ignore sub-directories
                    if (file.type == "directory")
                    {
                        continue;
                    }

                    // TODO: consider how to handle zero-size file
                    // Ignore zero length files (may still be being formed)
                    if (file.size == 0)
                    {
                        continue;
                    }

                    outgoing_tx_add(file.node, file.agent, file.name);
                }
            }

            return outgoing_tx_recount(node_id);
        }

        //! Look through the outgoing and incoming queues of all nodes and generate any necessary packets.
        //! Note that this gets outgoing packets for every node, no distinction is made.
        //! Not thread safe.
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        int32_t Transfer::get_outgoing_lpackets(vector<PacketComm> &packets)
        {
            for (uint8_t node_id = 0; node_id < txq.size(); ++node_id) {
                get_outgoing_lpackets(node_id, packets);
            }
            
            return 0;
        }

        //! Look through the outgoing and incoming queues of the specified node and generate any necessary packets.
        //! Not thread safe.
        //! \param node_name Name of the node
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        int32_t Transfer::get_outgoing_lpackets(string node_name, vector<PacketComm> &packets)
        {
            if (node_name.empty())
            {
                if (debug_error != nullptr)
                {
                    debug_error->Printf("%.4f %.4f Main: get_outgoing_packets: TRANSFER_ERROR_FILENAME\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_FILENAME;
            }

            // Find corresponding node_id in the node table
            uint8_t node_id = NodeData::lookup_node_id(node_name);
            if (node_id == 0)
            {
                return TRANSFER_ERROR_NODE;
            }

            int32_t iretn;
            iretn = get_outgoing_lpackets(node_id, packets);

            return iretn;
        }

        //! Look through the outgoing and incoming queues of the specified node and generate any necessary packets.
        //! Not thread safe.
        //! \param node_id ID of node in txq
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        int32_t Transfer::get_outgoing_lpackets(uint8_t node_id, vector<PacketComm> &packets)
        {
            // Check that node exists in the node table
            if (NodeData::check_node_id(node_id) <= 0)
            {
                return TRANSFER_ERROR_NODE;
            }

            // Hold QUEUE packet tx_id's in here
            vector<PACKET_TX_ID_TYPE> tqueue;

            // Iterate over all files in outgoing queue, push necessary packets
            for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
            {
                // Check if this is a valid outgoing transaction
                if (txq[(node_id)].outgoing.progress[tx_id].tx_id != tx_id) {
                    continue;
                }

                // **************************************************************
                // ** METADATA **************************************************
                // **************************************************************
                if (!txq[(node_id)].outgoing.progress[tx_id].sentmeta)
                {
                    tx_progress tx = txq[(node_id)].outgoing.progress[tx_id];
                    PacketComm packet;
                    serialize_metadata(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), tx.tx_id, (char *)tx.file_name.c_str(), tx.file_size, (char *)tx.agent_name.c_str());
                    packets.push_back(packet);
                    txq[(node_id)].outgoing.progress[tx_id].sentmeta = true;
                }

                // ** enabled check *****************
                // Check if this file is enabled
                // Note that the file's QUEUE status and METADATA is always sent,
                // regardless of whether it's enabled or not.

                // Check if all data for this file has been sent
                if (txq[(node_id)].outgoing.progress[tx_id].enabled
                && !txq[(node_id)].outgoing.progress[tx_id].sentdata)
                {
                // **************************************************************
                // ** DATA ******************************************************
                // **************************************************************
                    if (txq[(node_id)].outgoing.progress[tx_id].file_size)
                    {
                        // Check if there is any more data to send
                        if (txq[(node_id)].outgoing.progress[tx_id].total_bytes == 0)
                        {
                            txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                        }
                        // Grab next chunk of file's data and push DATA packet
                        else
                        {
                            // Attempt to open the outgoing progress file
                            if (txq[(node_id)].outgoing.progress[tx_id].fp == nullptr)
                            {
                                txq[(node_id)].outgoing.progress[tx_id].fp = fopen(txq[(node_id)].outgoing.progress[tx_id].filepath.c_str(), "r");
                            }

                            // If we're good, continue with the process
                            if (txq[(node_id)].outgoing.progress[tx_id].fp != nullptr)
                            {
                                file_progress tp;
                                tp = txq[(node_id)].outgoing.progress[tx_id].file_info[0];

                                PACKET_FILE_SIZE_TYPE byte_count = (tp.chunk_end - tp.chunk_start) + 1;
                                if (byte_count > packet_size)
                                {
                                    byte_count = packet_size;
                                }

                                tp.chunk_end = tp.chunk_start + byte_count - 1;

                                // Read the packet and send it
                                int32_t nbytes;
                                PACKET_BYTE* chunk = new PACKET_BYTE[byte_count]();
                                if (!(nbytes = fseek(txq[(node_id)].outgoing.progress[tx_id].fp, tp.chunk_start, SEEK_SET)))
                                {
                                    nbytes = fread(chunk, 1, byte_count, txq[(node_id)].outgoing.progress[tx_id].fp);
                                }
                                if (nbytes == byte_count)
                                {
                                    PacketComm packet;
                                    serialize_data(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].outgoing.progress[tx_id].tx_id, byte_count, tp.chunk_start, chunk);
                                    packets.push_back(packet);
                                    txq[(node_id)].outgoing.progress[tx_id].file_info[0].chunk_start = tp.chunk_end + 1;
                                }
                                else
                                {
                                    // Some problem with this transmission, ask other end to dequeue it
                                    // Remove transaction
                                    txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                                    txq[(node_id)].outgoing.progress[tx_id].complete = true;
                                }
                                delete[] chunk;

                                if (txq[(node_id)].outgoing.progress[tx_id].file_info[0].chunk_start > txq[(node_id)].outgoing.progress[tx_id].file_info[0].chunk_end)
                                {
                                    // All done with this file_info entry. Close file and remove entry.
                                    fclose(txq[(node_id)].outgoing.progress[tx_id].fp);
                                    txq[(node_id)].outgoing.progress[tx_id].fp = nullptr;
                                    txq[(node_id)].outgoing.progress[tx_id].file_info.pop_front();
                                    // Update total_bytes
                                    merge_chunks_overlap(txq[(node_id)].outgoing.progress[tx_id]);
                                }

                                write_meta(txq[(node_id)].outgoing.progress[tx_id]);
                            }
                            else
                            {
                                // Some problem with this transmission, ask other end to dequeue it
                                txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                                txq[(node_id)].outgoing.progress[tx_id].complete = true;
                            }
                        }
                    }
                    // Zero length file, ask other end to dequeue it
                    else
                    {
                        txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                        txq[(node_id)].outgoing.progress[tx_id].complete = true;
                    }
                }
                // All data for this file has been sent
                else
                {
                    // Check if a COMPLETE packet response was received from the receiver
                    // or if the complete flag was set because of an error
                    if (txq[(node_id)].outgoing.progress[tx_id].complete)
                    {
                // **************************************************************
                // ** CANCEL ****************************************************
                // **************************************************************
                        // Remove from queue
                        outgoing_tx_del(node_id, tx_id);

                        // Send a CANCEL packet
                        PacketComm packet;
                        serialize_cancel(packet, static_cast<PACKET_NODE_ID_TYPE>(node_id), tx_id);
                        packets.push_back(packet);
                    }
                    // A COMPLETE packet was not received yet
                    else
                    {
                        // Check if it's not too soon to send another REQCOMPLETE packet
                        if (currentmjd() > txq[(node_id)].outgoing.progress[tx_id].next_response)
                        {
                // **************************************************************
                // ** REQCOMPLETE ***********************************************
                // **************************************************************
                            txq[(node_id)].outgoing.progress[tx_id].next_response = currentmjd() + txq[(node_id)].outgoing.waittime;
                            PacketComm packet;
                            serialize_reqcomplete(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), tx_id);
                            packets.push_back(packet);
                        }

                        // If it's too soon, don't send a REQCOMPLETE packet yet.
                        // This is to prevent sending redundant requests, the receiver
                        // only needs to respond once.
                    }
                }

                // **************************************************************
                // ** QUEUE *****************************************************
                // **************************************************************
                if (!txq[(node_id)].outgoing.sentqueue
                // Check that this transaction is still happening
                && txq[(node_id)].outgoing.progress[tx_id].tx_id == tx_id)
                {
                    tqueue.push_back(tx_id);
                }
            }

            // Push QUEUE packet if it needs to be sent
            if (tqueue.size())
            {
                PacketComm packet;
                serialize_queue(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].node_name, tqueue);
                packets.push_back(packet);
                txq[(node_id)].outgoing.sentqueue = true;
            }

            return 0;
        }

        //! Look through the outgoing and incoming queues of all nodes and generate any necessary packets.
        //! Note that this gets outgoing packets for every node, no distinction is made.
        //! Not thread safe.
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        int32_t Transfer::get_outgoing_rpackets(vector<PacketComm> &packets)
        {
            for (uint8_t node_id = 0; node_id < txq.size(); ++node_id) {
                get_outgoing_rpackets(node_id, packets);
            }
            
            return 0;
        }

        //! Look through the outgoing and incoming queues of the specified node and generate any necessary packets.
        //! Not thread safe.
        //! \param node_name Name of the node
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        int32_t Transfer::get_outgoing_rpackets(string node_name, vector<PacketComm> &packets)
        {
            if (node_name.empty())
            {
                if (debug_error != nullptr)
                {
                    debug_error->Printf("%.4f %.4f Main: get_outgoing_packets: TRANSFER_ERROR_FILENAME\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_FILENAME;
            }

            // Find corresponding node_id in the node table
            uint8_t node_id = NodeData::lookup_node_id(node_name);
            if (node_id == 0)
            {
                return TRANSFER_ERROR_NODE;
            }

            int32_t iretn;
            iretn = get_outgoing_rpackets(node_id, packets);

            return iretn;
        }

        //! Look through the outgoing and incoming queues of the specified node and generate any necessary packets.
        //! These are response-type file transfer packets, to signal to the sender that something is wrong, or going right.
        //! Not thread safe.
        //! \param node_id ID of node in txq
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        int32_t Transfer::get_outgoing_rpackets(uint8_t node_id, vector<PacketComm> &packets)
        {
            // Check that node exists in the node table
            if (NodeData::check_node_id(node_id) <= 0)
            {
                return TRANSFER_ERROR_NODE;
            }

            // Hold REQMETA bits in here
            vector<PACKET_TX_ID_TYPE> treqmeta (TRANSFER_QUEUE_LIMIT, 0);
            PACKET_TX_ID_TYPE iq = 0; // TODO: fix this stuff

            // Iterate over tx_id's requiring a response
            for (PACKET_TX_ID_TYPE tx_id : txq[(node_id)].incoming.respond)
            {
                // Sanity check if this tx_id is valid
                if (txq[(node_id)].incoming.progress[tx_id].tx_id != tx_id)
                {
                    continue;
                }

                // **************************************************************
                // ** REQMETA ***************************************************
                // **************************************************************
                if (!txq[(node_id)].incoming.progress[tx_id].sentmeta)
                {
                    treqmeta[iq++] = tx_id;
                }

                // **************************************************************
                // ** REQDATA ***************************************************
                // **************************************************************
                if (!txq[(node_id)].incoming.progress[tx_id].sentdata)
                {
                    // Request missing data
                    vector<file_progress> missing;
                    missing = find_chunks_missing(txq[(node_id)].incoming.progress[tx_id]);
                    for (uint32_t j=0; j<missing.size(); ++j)
                    {
                        PacketComm packet;
                        serialize_reqdata(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].incoming.progress[tx_id].tx_id, missing[j].chunk_start, missing[j].chunk_end);
                        packets.push_back(packet);
                    }
                }
                // **************************************************************
                // ** COMPLETE **************************************************
                // **************************************************************
                else
                {
                    PacketComm packet;
                    serialize_complete(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), tx_id);
                    packets.push_back(packet);
                }
            }

            txq[(node_id)].incoming.respond.clear();
            
            // Send REQMETA packet if needed
            if (iq)
            {
                PacketComm packet;
                serialize_reqmeta(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].node_name, treqmeta);
                packets.push_back(packet);
            }

            return 0;
        }

        //! Creates the metadata for a new file to queue.
        //! After performing basic checks regarding file creation and ensuring uniqueness in the
        //! outgoing queue, writes the meta data to disk and then calls outgoing_tx_add(tx_progress &tx_out).
        //! \param node_name Name of node
        //! \param agent_name Name of agent
        //! \param file_name Name of file to add to outgoing queue
        //! \return Number of files in node's outgoing queue if non-error
        int32_t Transfer::outgoing_tx_add(string node_name, string agent_name, string file_name)
        {
            if (node_name.empty() || agent_name.empty() || file_name.empty())
            {
                if (debug_error != nullptr)
                {
                    debug_error->Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_FILENAME\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_FILENAME;
            }

            // BEGIN GATHERING THE METADATA
            tx_progress tx_out;

            uint8_t node_id = NodeData::lookup_node_id(node_name);
            if (node_id == 0)
            {
                return TRANSFER_ERROR_NODE;
            }

            // Only add if we have room
            if (txq[(node_id)].outgoing.size == PROGRESS_QUEUE_SIZE-1)
            {
                return TRANSFER_ERROR_QUEUEFULL;
            }

            // Get the file path and size
            string filepath = data_base_path(node_name, "outgoing", agent_name, file_name);
            int32_t file_size = get_file_size(filepath);

            // Go through existing queue
            // - if it is already there and the size is different, remove it
            // - if it is already there, and the size is the same, enable it
            // - if it is not already there and there is room to add it, go on to next step
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[(node_id)].outgoing.progress[i].filepath == filepath)
                {
                    if (txq[(node_id)].outgoing.progress[i].file_size == file_size)
                    {
                        // This file transaction already exists and doesn't need to be added
                        if (!txq[(node_id)].outgoing.progress[i].enabled)
                        {
                            txq[(node_id)].outgoing.progress[i].enabled = true;
                            if (debug_error != nullptr)
                            {
                                debug_error->Printf("%.4f %.4f Main: outgoing_tx_add: Enable %u %s %s %s %d\n", tet.split(), dt.lap(), txq[(node_id)].outgoing.progress[i].tx_id, txq[(node_id)].outgoing.progress[i].node_name.c_str(), txq[(node_id)].outgoing.progress[i].agent_name.c_str(), txq[(node_id)].outgoing.progress[i].filepath.c_str(), PROGRESS_QUEUE_SIZE);
                            }
                        }
                        return outgoing_tx_recount(node_id);
                    }
                    else
                    {
                        outgoing_tx_del(node_id, i, false);
                        return TRANSFER_ERROR_FILESIZE;
                    }
                    if (txq[(node_id)].outgoing.progress[i].enabled && txq[(node_id)].outgoing.progress[i].file_size == 0)
                    {
                        outgoing_tx_del(node_id, i, true);
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
                if (txq[(node_id)].outgoing.progress[id].tx_id == 0)
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
                tx_out.node_name = node_name;
                tx_out.agent_name = agent_name;
                tx_out.file_name = file_name;
                tx_out.temppath = data_base_path(tx_out.node_name, "temp", "file", "out_"+std::to_string(tx_out.tx_id));
                tx_out.filepath = filepath;
                tx_out.savetime = 0.;

                std::ifstream filename;

                // get the file size
                tx_out.file_size = file_size;

                if(tx_out.file_size < 0)
                {
                    if (debug_error != nullptr)
                    {
                        debug_error->Printf("%.4f %.4f Main: outgoing_tx_add: DATA_ERROR_SIZE_MISMATCH\n", tet.split(), dt.lap());
                    }
                    return DATA_ERROR_SIZE_MISMATCH;
                }

                // see if file can be opened
                filename.open(tx_out.filepath, std::ios::in|std::ios::binary);
                if(!filename.is_open())
                {
                    if (debug_error != nullptr)
                    {
                        debug_error->Printf("%.4f %.4f Main: outgoing_tx_add: %s\n", tet.split(), dt.lap(), cosmos_error_string(-errno).c_str());
                    }
                    return -errno;
                }
                filename.close();

                file_progress tp;
                tp.chunk_start = 0;
                tp.chunk_end = tx_out.file_size - 1;
                tx_out.file_info.push_back(tp);

                write_meta(tx_out);

                int32_t iretn = outgoing_tx_add(tx_out);
                return iretn;
            }
            else
            {
                return TRANSFER_ERROR_MATCH;
            }
        }

        //! Adds new file to outgoing queue.
        //! Called from other functions that create a tx_progress to pass into this.
        //! \param tx_out A tx_progress containing meta information about file to add to outgoing queue.
        //! \return Number of files in node's outgoing queue if non-error
        int32_t Transfer::outgoing_tx_add(tx_progress &tx_out)
        {
            if (debug_error != nullptr)
            {
                debug_error->Printf("%.4f %.4f Main: outgoing_tx_add: ", tet.split(), dt.lap());
            }

            int32_t node_id = NodeData::lookup_node_id(tx_out.node_name);
            if (node_id <= 0)
            {
                if (debug_error != nullptr)
                {
                    debug_error->Printf("TRANSFER_ERROR_NODE\n");
                }
                if (node_id == 0)
                {
                    return TRANSFER_ERROR_NODE;
                }
                else
                {
                    return node_id;
                }
            }

            // Check for duplicate tx_id
            if (txq[(node_id)].outgoing.progress[tx_out.tx_id].tx_id)
            {
                return TRANSFER_ERROR_DUPLICATE;
            }

            if (tx_out.file_name.size())
            {
                tx_out.filepath = data_base_path(tx_out.node_name, "outgoing", tx_out.agent_name, tx_out.file_name);
            }
            else
            {
                if (debug_error != nullptr)
                {
                    debug_error->Printf("TRANSFER_ERROR_FILENAME\n");
                }
                tx_out.filepath = "";
                return TRANSFER_ERROR_FILENAME;
            }

            tx_out.temppath = data_base_path(tx_out.node_name, "temp", "file", "out_"+std::to_string(tx_out.tx_id));

            // Check for a duplicate file name of something already in queue
            uint16_t minindex = 255 - static_cast<uint16_t>(pow(pow(TRANSFER_QUEUE_LIMIT,1.f/3.f), (3.f - log10f(tx_out.file_size) / 2.f)));
            for (uint16_t i=minindex; i<256; ++i)
            {
                if (!txq[(node_id)].outgoing.progress[i].filepath.empty() && tx_out.filepath == txq[(node_id)].outgoing.progress[i].filepath)
                {
                    // Remove the META file
                    if (debug_error != nullptr)
                    {
                        debug_error->Printf("%u %s %s %s TRANSFER_ERROR_DUPLICATE\n", tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.filepath.c_str());
                    }
                    string filepath = tx_out.temppath + ".meta";
                    remove(filepath.c_str());
                    return TRANSFER_ERROR_DUPLICATE;
                }
            }

            tx_out.fp = nullptr;
            //get the file size
            tx_out.file_size = get_file_size(tx_out.filepath);
            tx_out.savetime = 0.;

            // save and queue metadata packet
            tx_out.sentmeta = false;
            tx_out.sentdata = false;
            tx_out.complete = false;

            if (debug_error != nullptr)
            {
                debug_error->Printf("%u %s %s %s %d ", tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.filepath.c_str(), PROGRESS_QUEUE_SIZE);
            }

            // Good to go. Add it to queue.
            txq[(node_id)].outgoing.progress[tx_out.tx_id].tx_id = tx_out.tx_id;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].sentmeta = tx_out.sentmeta;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].sentdata = tx_out.sentdata;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].complete = tx_out.complete;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].enabled  = tx_out.enabled;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].node_name = tx_out.node_name;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].agent_name = tx_out.agent_name;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].file_name = tx_out.file_name;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].filepath = tx_out.filepath;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].temppath = tx_out.temppath;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].savetime = tx_out.savetime;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].datatime = tx_out.datatime;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].file_size = tx_out.file_size;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].total_bytes = tx_out.total_bytes;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].file_info.clear();
            for (file_progress filep : tx_out.file_info)
            {
                txq[(node_id)].outgoing.progress[tx_out.tx_id].file_info.push_back(filep);
            }
            txq[(node_id)].outgoing.progress[tx_out.tx_id].fp = tx_out.fp;
            ++txq[(node_id)].outgoing.size;

            // Update total_bytes
            merge_chunks_overlap(txq[(node_id)].outgoing.progress[tx_out.tx_id]);

            // Set QUEUE to be sent out again
            txq[(node_id)].outgoing.sentqueue = false;

            if (debug_error != nullptr)
            {
                debug_error->Printf(" %u\n", txq[(node_id)].outgoing.size);
            }

            return outgoing_tx_recount(node_id);
        }

        //! Removes file from outgoing queue.
        //! \param node_id ID of node
        //! \param tx_id ID of the file transfer transaction
        //! \param remove_file If set to true, deletes file from the outgoing directory as well
        //! \return Number of files in node's outgoing queue if non-error
        int32_t Transfer::outgoing_tx_del(uint8_t node_id, uint16_t tx_id, bool remove_file)
        {
            if (node_id == 0 || node_id >= txq.size())
            {
                return TRANSFER_ERROR_INDEX;
            }

            if (txq[(node_id)].outgoing.progress[tx_id].tx_id == 0)
            {
                return TRANSFER_ERROR_MATCH;
            }

            if (tx_id >= PROGRESS_QUEUE_SIZE)
            {
                for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
                {
                    outgoing_tx_del(node_id, i);
                }
            }
            else
            {
                tx_progress tx_out = txq[(node_id)].outgoing.progress[tx_id];

                // erase the transaction
                txq[(node_id)].outgoing.progress[tx_id].fp = nullptr;
                txq[(node_id)].outgoing.progress[tx_id].enabled = false;
                txq[(node_id)].outgoing.progress[tx_id].tx_id = 0;
                txq[(node_id)].outgoing.progress[tx_id].complete = false;
                txq[(node_id)].outgoing.progress[tx_id].filepath = "";
                txq[(node_id)].outgoing.progress[tx_id].savetime = 0;
                txq[(node_id)].outgoing.progress[tx_id].temppath = "";
                txq[(node_id)].outgoing.progress[tx_id].file_name = "";
                txq[(node_id)].outgoing.progress[tx_id].file_size = 0;
                txq[(node_id)].outgoing.progress[tx_id].node_name = "";
                txq[(node_id)].outgoing.progress[tx_id].agent_name = "";
                txq[(node_id)].outgoing.progress[tx_id].total_bytes = 0;
                txq[(node_id)].outgoing.progress[tx_id].file_info.clear();

                if (txq[(node_id)].outgoing.size)
                {
                    --txq[(node_id)].outgoing.size;
                }

                // Set QUEUE to be sent out again
                txq[node_id].outgoing.sentqueue = false;

                // Remove the file
                if(remove_file && remove(tx_out.filepath.c_str()))
                {
                    if (debug_error != nullptr)
                    {
                        debug_error->Printf("%.4f %.4f Main/Outgoing: Del outgoing: %u %s %s %s - Unable to remove file\n", tet.split(), dt.lap(), tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str());
                    }
                }

                // Remove the META file
                string meta_filepath = tx_out.temppath + ".meta";
                remove(meta_filepath.c_str());

                if (debug_error != nullptr)
                {
                    debug_error->Printf("%.4f %.4f Main/Outgoing: Del outgoing: %u %s %s %s\n", tet.split(), dt.lap(), tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str());
                }
            }

            return outgoing_tx_recount(node_id);
        }

        //! Recount number of files in outgoing queue
        //! \param node_name Name of node to check
        //! \return Number of files in node's outgoing queue
        int32_t Transfer::outgoing_tx_recount(string node_name)
        {
            return outgoing_tx_load(NodeData::lookup_node_id(node_name));
        }

        //! Recount number of files in outgoing queue
        //! \param node_id ID of node to check
        //! \return Number of files in node's outgoing queue
        int32_t Transfer::outgoing_tx_recount(uint8_t node_id)
        {
            if (node_id == 0 || node_id >= txq.size())
            {
                return TRANSFER_ERROR_INDEX;
            }

            txq[(node_id)].outgoing.size = 0;
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[(node_id)].outgoing.progress[i].tx_id)
                {
                    ++txq[(node_id)].outgoing.size;
                }
            }

            return txq[(node_id)].outgoing.size;
        }

        //! Choose a file to send out.
        //! File is chosen on various metrics, such as the smallest remaining size.
        //! \param node_id ID of the node to send to
        //! \return tx_id of a valid file or 0 if nothing is chosen
        PACKET_TX_ID_TYPE Transfer::choose_outgoing_tx_id(uint8_t node_id)
        {
            PACKET_TX_ID_TYPE tx_id = 0;

            if (node_id > 0 && node_id < txq.size())
            {
                // Choose file with least data left to send
                PACKET_FILE_SIZE_TYPE nsize = INT32_MAX;
                for (PACKET_FILE_SIZE_TYPE i=1; i < PROGRESS_QUEUE_SIZE; ++i)
                {
                    if (txq[(node_id)].outgoing.progress[i].tx_id && txq[(node_id)].outgoing.progress[i].enabled && txq[(node_id)].outgoing.progress[i].sentmeta && !txq[(node_id)].outgoing.progress[i].sentdata)
                    {
                        // Remove anything file_size == 0
                        if (txq[(node_id)].outgoing.progress[i].file_size == 0)
                        {
                            outgoing_tx_del(node_id, txq[(node_id)].outgoing.progress[i].tx_id);
                        }
                        else
                        {
                            // calculate bytes so far
                            merge_chunks_overlap(txq[(node_id)].outgoing.progress[i]);

                            // Remove anything suspicious: file_size < total_bytes
                            if (txq[(node_id)].outgoing.progress[i].file_size < txq[(node_id)].outgoing.progress[i].total_bytes)
                            {
                                outgoing_tx_del(node_id, txq[(node_id)].outgoing.progress[i].tx_id, false);
                            }
                            // Choose unfinished transactions for which bytes remaining is minimized
                            else if (txq[(node_id)].outgoing.progress[i].total_bytes < nsize)
                            {
                                nsize = txq[(node_id)].outgoing.progress[i].total_bytes;
                                tx_id = txq[(node_id)].outgoing.progress[i].tx_id;
                            }
                        }
                    }
                }
            }

            if (tx_id)
            {
                merge_chunks_overlap(txq[(node_id)].outgoing.progress[tx_id]);
                txq[(node_id)].outgoing.progress[tx_id].sentdata = false;
            }
            return tx_id;
        }

        PACKET_TX_ID_TYPE Transfer::check_tx_id(tx_entry &txentry, PACKET_TX_ID_TYPE tx_id)
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

        int32_t Transfer::incoming_tx_add(string node_name, PACKET_TX_ID_TYPE tx_id)
        {
            tx_progress tx_in;

            tx_in.tx_id = tx_id;
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

        //! Adds a new file to incoming queue
        //! \return Number of files in incoming queue, or error value
        int32_t Transfer::incoming_tx_add(tx_progress &tx_in)
        {
            uint8_t node_id = NodeData::lookup_node_id(tx_in.node_name);
            if (node_id == 0)
            {
                if (debug_error != nullptr)
                {
                    debug_error->Printf("%.4f %.4f Main: incoming_tx_add: TRANSFER_ERROR_NODE\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_NODE;
            }

            // Check for an actual file name
            if (tx_in.file_name.size())
            {
                tx_in.filepath = data_base_path(tx_in.node_name, "incoming", tx_in.agent_name, tx_in.file_name);
            }
            else
            {
                tx_in.filepath = "";
            }

            string tx_name = "in_"+std::to_string(tx_in.tx_id);
            tx_in.temppath = data_base_path(tx_in.node_name, "temp", "file", tx_name);

            // Check for a duplicate file name of something already in queue
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (!txq[(node_id)].incoming.progress[i].filepath.empty() && tx_in.filepath == txq[(node_id)].incoming.progress[i].filepath)
                {
                    if (debug_error != nullptr)
                    {
                        debug_error->Printf("%u %s %s %s TRANSFER_ERROR_DUPLICATE\n", tx_in.tx_id, tx_in.node_name.c_str(), tx_in.agent_name.c_str(), tx_in.filepath.c_str());
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
            txq[(node_id)].incoming.progress[tx_in.tx_id].tx_id = tx_in.tx_id;
            txq[(node_id)].incoming.progress[tx_in.tx_id].sentmeta = tx_in.sentmeta;
            txq[(node_id)].incoming.progress[tx_in.tx_id].sentdata = tx_in.sentdata;
            txq[(node_id)].incoming.progress[tx_in.tx_id].complete = tx_in.complete;
            txq[(node_id)].incoming.progress[tx_in.tx_id].node_name = tx_in.node_name;
            txq[(node_id)].incoming.progress[tx_in.tx_id].agent_name = tx_in.agent_name;
            txq[(node_id)].incoming.progress[tx_in.tx_id].file_name = tx_in.file_name;
            txq[(node_id)].incoming.progress[tx_in.tx_id].filepath = tx_in.filepath;
            txq[(node_id)].incoming.progress[tx_in.tx_id].temppath = tx_in.temppath;
            txq[(node_id)].incoming.progress[tx_in.tx_id].savetime = tx_in.savetime;
            txq[(node_id)].incoming.progress[tx_in.tx_id].file_size = tx_in.file_size;
            txq[(node_id)].incoming.progress[tx_in.tx_id].total_bytes = tx_in.total_bytes;
            txq[(node_id)].incoming.progress[tx_in.tx_id].file_info.clear();
            for (file_progress filep : tx_in.file_info)
            {
                txq[(node_id)].incoming.progress[tx_in.tx_id].file_info.push_back(filep);
            }
            txq[(node_id)].incoming.progress[tx_in.tx_id].fp = tx_in.fp;
            ++txq[(node_id)].incoming.size;

            if (debug_error != nullptr)
            {
                debug_error->Printf("%.4f %.4f Main/Incoming: Add incoming: %u %s %s %s %d\n", tet.split(), dt.lap(), tx_in.tx_id, tx_in.node_name.c_str(), tx_in.agent_name.c_str(), tx_in.filepath.c_str(), PROGRESS_QUEUE_SIZE);
            }

            return incoming_tx_recount(node_id);
        }

        int32_t Transfer::incoming_tx_update(packet_struct_metashort meta)
        {
            int32_t node_id = NodeData::check_node_id(meta.node_id);
            if (node_id <= 0)
            {
                if (node_id < 0)
                {
                    return node_id;
                }
                else
                {
                    return TRANSFER_ERROR_NODE;
                }
            }

            if (meta.tx_id)
            {
                // This will run if META is received first
                // If so, add the file to the incoming queue.
                if (txq[(node_id)].incoming.progress[meta.tx_id].tx_id != meta.tx_id)
                {
                    txq[(node_id)].incoming.progress[meta.tx_id].tx_id = meta.tx_id;
                    string node_name = NodeData::lookup_node_id_name(node_id);
                    if (node_name.empty()) {
                        return TRANSFER_ERROR_INDEX;
                    }
                    int32_t iretn = incoming_tx_add(node_name, meta.tx_id);
                    if (iretn <= 0) {
                        return iretn;
                    }
                }

                txq[(node_id)].incoming.progress[meta.tx_id].datatime = currentmjd();

                // This will run if either META is received for the first time or if requested by REQMETA
                if (!txq[(node_id)].incoming.progress[meta.tx_id].sentmeta)
                {
                    // Core META information
                    txq[(node_id)].incoming.progress[meta.tx_id].node_name = txq[(node_id)].node_name;
                    txq[(node_id)].incoming.progress[meta.tx_id].agent_name = meta.agent_name;
                    txq[(node_id)].incoming.progress[meta.tx_id].file_name = meta.file_name;
                    txq[(node_id)].incoming.progress[meta.tx_id].file_size = meta.file_size;
                    txq[(node_id)].incoming.progress[meta.tx_id].filepath = data_base_path(txq[(node_id)].incoming.progress[meta.tx_id].node_name, "incoming", txq[(node_id)].incoming.progress[meta.tx_id].agent_name, txq[(node_id)].incoming.progress[meta.tx_id].file_name);

                    // This section will be skipped if DATA is written before META was received
                    if (!txq[(node_id)].incoming.progress[meta.tx_id].file_info.size()) {
                        string tx_name = "in_"+std::to_string(txq[(node_id)].incoming.progress[meta.tx_id].tx_id);
                        txq[(node_id)].incoming.progress[meta.tx_id].temppath = data_base_path(txq[(node_id)].incoming.progress[meta.tx_id].node_name, "temp", "file", tx_name);
                        
                        // Derivative META information
                        txq[(node_id)].incoming.progress[meta.tx_id].savetime = 0.;
                        txq[(node_id)].incoming.progress[meta.tx_id].complete = false;
                        txq[(node_id)].incoming.progress[meta.tx_id].total_bytes = 0;
                        txq[(node_id)].incoming.progress[meta.tx_id].fp = nullptr;
                    }

                    // Save it to disk
                    write_meta(txq[(node_id)].incoming.progress[meta.tx_id]);
                }

                txq[(node_id)].incoming.progress[meta.tx_id].sentmeta = true;

                if (debug_error != nullptr)
                {
                    debug_error->Printf("%.4f %.4f Incoming: Update incoming: %u %s %s %s\n", tet.split(), dt.lap(), txq[(node_id)].incoming.progress[meta.tx_id].tx_id, txq[(node_id)].incoming.progress[meta.tx_id].node_name.c_str(), txq[(node_id)].incoming.progress[meta.tx_id].agent_name.c_str(), txq[(node_id)].incoming.progress[meta.tx_id].file_name.c_str());
                }

                return meta.tx_id;
            }
            else
            {
                return TRANSFER_ERROR_INDEX;
            }
        }

        //! Resets a file transfer tx_id
        //! Used when a CANCEL packet is received.
        //! \return Number of files remaining in incoming queue
        int32_t Transfer::incoming_tx_del(uint8_t node_id, uint16_t tx_id)
        {
            node_id = NodeData::check_node_id(node_id);
            if (node_id == 0)
            {
                return TRANSFER_ERROR_NODE;
            }

            if (tx_id >= PROGRESS_QUEUE_SIZE)
            {
                for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
                {
                    incoming_tx_del(node_id, i);
                }
            }
            else
            {
                if (txq[(node_id)].incoming.progress[tx_id].tx_id == 0)
                {
                    return TRANSFER_ERROR_MATCH;
                }

                if (txq[(node_id)].incoming.size)
                {
                    --txq[(node_id)].incoming.size;
                }

                // Move file to its final location
                if (!txq[(node_id)].incoming.progress[tx_id].complete)
                {
                    incoming_tx_complete(node_id, tx_id);
                }

                if (debug_error != nullptr)
                {
                    debug_error->Printf("%.4f %.4f Incoming: Del incoming: %u %s\n", tet.split(), dt.lap(), txq[(node_id)].incoming.progress[tx_id].tx_id, txq[(node_id)].incoming.progress[tx_id].node_name.c_str());
                }

                txq[(node_id)].incoming.progress[tx_id].tx_id = 0;
                txq[(node_id)].incoming.progress[tx_id].complete = false;
                txq[(node_id)].incoming.progress[tx_id].file_info.clear();
            }

            return incoming_tx_recount(node_id);
        }

        //! Completes an incoming file transfer
        //! Similar to incoming_tx_del() but doesn't reset the tx_id.
        //! Used when a file's data is fully received.
        //! \return 0 on success
        int32_t Transfer::incoming_tx_complete(uint8_t node_id, uint16_t tx_id)
        {
            node_id = NodeData::check_node_id(node_id);
            if (node_id == 0)
            {
                return TRANSFER_ERROR_NODE;
            }

            if (tx_id < PROGRESS_QUEUE_SIZE)
            {
                if (txq[(node_id)].incoming.progress[tx_id].tx_id == 0)
                {
                    return TRANSFER_ERROR_MATCH;
                }

                if (txq[(node_id)].incoming.size)
                {
                    --txq[(node_id)].incoming.size;
                }

                // Move file to its final location
                // Close the DATA file
                if (txq[(node_id)].incoming.progress[tx_id].fp != nullptr)
                {
                    fclose(txq[(node_id)].incoming.progress[tx_id].fp);
                    txq[(node_id)].incoming.progress[tx_id].fp = nullptr;
                }
                string final_filepath = txq[(node_id)].incoming.progress[tx_id].temppath + ".file";
                int iret = rename(final_filepath.c_str(), txq[(node_id)].incoming.progress[tx_id].filepath.c_str());
                // Make sure metadata is recorded
                write_meta(txq[(node_id)].incoming.progress[tx_id], 0.);
                if (!iret && debug_error != nullptr)
                {
                    debug_error->Printf("%.4f %.4f Incoming: Renamed/Data: %d %s\n", tet.split(), dt.lap(), iret, txq[(node_id)].incoming.progress[tx_id].filepath.c_str());
                }

                if (debug_error != nullptr)
                {
                    debug_error->Printf("%.4f %.4f Incoming: Complete incoming: %u %s\n", tet.split(), dt.lap(), txq[(node_id)].incoming.progress[tx_id].tx_id, txq[(node_id)].incoming.progress[tx_id].node_name.c_str());
                }

                string filepath;
                // Remove the DATA file
                filepath = txq[(node_id)].incoming.progress[tx_id].temppath + ".file";
                remove(filepath.c_str());

                // Remove the META file
                filepath = txq[(node_id)].incoming.progress[tx_id].temppath + ".meta";
                remove(filepath.c_str());

                txq[(node_id)].incoming.progress[tx_id].complete = true;
            }

            return 0;
        }

        //! Handles receiving of incoming file transfer-type packet.
        //! Make sure to process the PacketComm packet before calling this (with SLIPIn() or equivalent).
        //! \param packet PacketComm packet containing file transfer-type data
        //! \return 0 on success, RESPONSE_REQUIRED if get_outgoing_packets() needs to be called
        int32_t Transfer::receive_packet(const PacketComm& packet)
        {
            string node_name = NodeData::lookup_node_id_name(packet.data[0]);
            int32_t node_id = NodeData::check_node_id(packet.data[0]);
            int32_t iretn = 0;

            if (node_id <= 0 || node_name.empty())
            {
                return TRANSFER_ERROR_INDEX;
            }

            // Respond appropriately according to type of packet
            switch (packet.header.type)
            {
            case PacketComm::TypeId::FileCommand:
                {
                    packet_struct_command command;

                    deserialize_command(packet.data, command);

                    string node_name = NodeData::lookup_node_id_name(self_node_id);

                    FILE *fp = data_open(("/cosmos/nodes/" + node_name + "/incoming/exec/file.command").c_str(), "w");
                    if (fp)
                    {
                        fwrite(&command.bytes[0], 1, command.length, fp);
                        fclose(fp);
                    }
                }
                break;
            case PacketComm::TypeId::FileMessage:
                {
                    packet_struct_message message;

                    deserialize_message(packet.data, message);

                    string imessage;
                    imessage.resize(message.length);
                    memcpy(&imessage[0], message.bytes, message.length);
                    log_write(NodeData::lookup_node_id_name(message.node_id), "file", tet.split(), "", "message", imessage, "incoming");
                }
                break;
            case PacketComm::TypeId::FileQueue:
                {
                    packet_struct_queue queue;

                    deserialize_queue(packet.data, queue);

                    txq[node_id].incoming.sentqueue = true;

                    size_t tx_id = 0;

                    // Iterate over every group of flags
                    for (auto flags : queue.tx_ids)
                    {
                        for (size_t i = 0; i < sizeof(flags) * 8; ++i)
                        {
                            bool flag = flags & 1;
                            // If a file is in the QUEUE packet but not in our local queue, add it
                            if (flag && !(txq[node_id].incoming.progress[tx_id].tx_id))
                            {
                                incoming_tx_add(queue.node_name, tx_id);
                            }
                            // If a file is in our local queue but not in the QUEUE packet, delete it
                            // TODO: I think this logic fails for multiple senders
                            else if (!flag && txq[node_id].incoming.progress[tx_id].tx_id)
                            {
                                incoming_tx_del(node_id, tx_id);
                            }

                            flags = flags >> 1;
                            if (++tx_id >= PROGRESS_QUEUE_SIZE)
                            {
                                break;
                            }
                        }
                    }
                }
                break;
            case PacketComm::TypeId::FileReqMeta:
                {
                    packet_struct_reqmeta reqmeta;

                    deserialize_reqmeta(packet.data, reqmeta);

                    // Mark QUEUE for resending, clarify to receiver what it ought to be receiving
                    txq[node_id].outgoing.sentqueue = false;

                    // Send requested META packets
                    if (txq[node_id].node_id > 0)
                    {
                        for (uint16_t i=0; i<TRANSFER_QUEUE_LIMIT; ++i)
                        {
                            PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].outgoing, reqmeta.tx_id[i]);
                            if (tx_id > 0)
                            {
                                txq[node_id].outgoing.progress[tx_id].sentmeta = false;
                            }
                        }
                    }
                    break;
                }
            case PacketComm::TypeId::FileMetaData:
                {
                    packet_struct_metashort meta;

                    deserialize_metadata(packet.data, meta);

                    incoming_tx_update(meta);

                    break;
                }
            case PacketComm::TypeId::FileReqData:
                {
                    packet_struct_reqdata reqdata;

                    deserialize_reqdata(packet.data, reqdata);

                    // Simple validity check
                    if (reqdata.hole_end < reqdata.hole_start)
                    {
                        break;
                    }

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].outgoing, reqdata.tx_id);
                    
                    // tx_id now points to the valid entry to which we should add the data
                    if (tx_id > 0)
                    {
                        // Add this chunk to the queue
                        file_progress tp;
                        tp.chunk_start = reqdata.hole_start;
                        tp.chunk_end = reqdata.hole_end;
                        bool updated = add_chunk(txq[node_id].outgoing.progress[tx_id], tp);

                        // Save meta to disk
                        if (updated)
                        {
                            write_meta(txq[node_id].outgoing.progress[tx_id]);
                            txq[node_id].outgoing.progress[tx_id].sentdata = false;
                        }
                    }
                    break;
                }
            case PacketComm::TypeId::FileChunkData:
                {
                    packet_struct_data data;

                    deserialize_data(packet.data, data);

                    // create transaction entry if new, and then add data

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].incoming, data.tx_id);

                    // If transaction does not exist in incoming queue, then request META
                    if (tx_id <= 0)
                    {
                        tx_id = data.tx_id;
                        string node_name = NodeData::lookup_node_id_name(node_id);
                        if (node_name.empty()) {
                            iretn = TRANSFER_ERROR_INDEX;
                            break;
                        }
                        iretn = incoming_tx_add(node_name, data.tx_id);
                        if (iretn <= 0) {
                            break;
                        }
                    }

                    // Request META if it hasn't been received yet and it's been awhile since we last requested it
                    if (!txq[node_id].incoming.progress[data.tx_id].sentmeta
                    && currentmjd() > txq[(node_id)].incoming.progress[tx_id].next_response)
                    {
                        txq[(node_id)].incoming.progress[tx_id].next_response = currentmjd() + txq[(node_id)].incoming.waittime;
                        iretn = RESPONSE_REQUIRED;
                    }

                    // Regardless, receive the DATA anyway
                    txq[node_id].incoming.progress[tx_id].datatime = currentmjd();

                    // tx_id now points to the valid entry to which we should add the data
                    file_progress tp;
                    tp.chunk_start = data.chunk_start;
                    tp.chunk_end = data.chunk_start + data.byte_count - 1;

                    // Attempt to add chunk to incoming queue
                    bool updated = add_chunk(txq[node_id].incoming.progress[tx_id], tp);

                    // Write to disk if this is new data
                    if (updated)
                    {
                        string partial_filepath;
                        // Write incoming data to disk
                        if (txq[node_id].incoming.progress[tx_id].fp == nullptr)
                        {
                            partial_filepath = txq[node_id].incoming.progress[tx_id].temppath + ".file";
                            if (data_exists(partial_filepath))
                            {
                                txq[node_id].incoming.progress[tx_id].fp = fopen(partial_filepath.c_str(), "r+");
                            }
                            else
                            {
                                txq[node_id].incoming.progress[tx_id].fp = fopen(partial_filepath.c_str(), "w");
                            }
                        }

                        if (txq[node_id].incoming.progress[tx_id].fp == nullptr)
                        {
                            if (debug_error != nullptr)
                            {
                                debug_error->Printf("%.4f %.4f Incoming: File Error: %s %s on ID: %u Chunk: %u\n", tet.split(), dt.lap(), partial_filepath.c_str(), cosmos_error_string(-errno).c_str(), tx_id, tp.chunk_start);
                            }
                        }
                        else
                        {
                            fseek(txq[node_id].incoming.progress[tx_id].fp, tp.chunk_start, SEEK_SET);
                            fwrite(data.chunk, data.byte_count, 1, txq[node_id].incoming.progress[tx_id].fp);
                            fflush(txq[node_id].incoming.progress[tx_id].fp);
                            // Write latest meta data to disk
                            write_meta(txq[node_id].incoming.progress[tx_id]);
                            if (debug_error != nullptr)
                            {
                                uint32_t total = 0;
                                for (uint16_t i=0; i<data.byte_count; ++i)
                                {
                                    total += data.chunk[i];
                                }
                                //debug_error->Printf("%.4f %.4f Incoming: Received DATA/Write: %u bytes for tx_id: %u\n", tet.split(), dt.lap(), data.byte_count, tx_id);
                            }

                            // If all bytes have been received, mark as all data sent over
                            if (txq[node_id].incoming.progress[tx_id].file_size == txq[node_id].incoming.progress[tx_id].total_bytes)
                            {
                                txq[(node_id)].incoming.progress[tx_id].sentdata = true;
                                // Move file over to final destination
                                incoming_tx_complete(node_id, tx_id);
                                iretn = RESPONSE_REQUIRED;
                            }
                        }
                    }
                    if (iretn == RESPONSE_REQUIRED)
                    {
                        txq[node_id].incoming.respond.push_back(tx_id);
                    }

                    break;
                }
            case PacketComm::TypeId::FileReqComplete:
                {
                    packet_struct_reqcomplete reqcomplete;

                    deserialize_reqcomplete(packet.data, reqcomplete);

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].incoming, reqcomplete.tx_id);

                    // No transaction ID, so metadata needs to be requested first
                    if (tx_id <= 0)
                    {
                        // Trigger REQMETA sending check
                        // Technically this line is redundant and pointless
                        txq[node_id].incoming.progress[reqcomplete.tx_id].sentmeta = false;
                    }
                    txq[node_id].incoming.respond.push_back(reqcomplete.tx_id);
                    iretn = RESPONSE_REQUIRED;

                    break;
                }
            case PacketComm::TypeId::FileComplete:
                {
                    packet_struct_complete complete;

                    deserialize_complete(packet.data, complete);

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].outgoing, complete.tx_id);
                    if (tx_id > 0)
                    {
                        txq[node_id].outgoing.progress[tx_id].complete = true;
                        txq[node_id].outgoing.progress[tx_id].sentdata = true;
                        txq[node_id].outgoing.progress[tx_id].sentmeta = true;
                    }

                    break;
                }
            case PacketComm::TypeId::FileCancel:
                {
                    packet_struct_cancel cancel;
                    deserialize_cancel(packet.data, cancel);

                    PACKET_TX_ID_TYPE tx_id = check_tx_id(txq[node_id].incoming, cancel.tx_id);
                    if (tx_id > 0)
                    {
                        // Remove the transaction
                        incoming_tx_del(node_id, tx_id);
                    }

                    break;
                }
            default:
                return COSMOS_PACKET_TYPE_MISMATCH;
                break;
            }

            return iretn;
        }

        int32_t Transfer::incoming_tx_recount(string node_name)
        {
            return incoming_tx_recount(NodeData::lookup_node_id(node_name));
        }

        int32_t Transfer::incoming_tx_recount(uint8_t node_id)
        {
            if (node_id == 0 || node_id >= txq.size())
            {
                return TRANSFER_ERROR_INDEX;
            }

            txq[(node_id)].incoming.size = 0;
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[(node_id)].incoming.progress[i].tx_id)
                {
                    ++txq[(node_id)].incoming.size;
                }
            }
            return txq[(node_id)].incoming.size;
        }

        int32_t Transfer::write_meta(tx_progress& tx, double interval)
        {
            PacketComm packet;
            std::ofstream file_name;

            if (currentmjd(0.) - tx.savetime > interval/86400.)
            {
                tx.savetime = currentmjd(0.);
                serialize_metadata(packet, tx.tx_id, (char *)tx.file_name.c_str(), tx.file_size, (char *)tx.node_name.c_str(), (char *)tx.agent_name.c_str());
                file_name.open(tx.temppath + ".meta", std::ios::out|std::ios::binary);
                if(!file_name.is_open())
                {
                    return (-errno);
                }

                uint16_t crc;
                CRC16 calc_crc;
                file_name.write((char *)&packet.data[0], PACKET_METALONG_OFFSET_TOTAL);
                crc = calc_crc.calc(packet.data);
                file_name.write((char *)&crc, 2);
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

        int32_t Transfer::read_meta(tx_progress& tx)
        {
            vector<PACKET_BYTE> packet(PACKET_METALONG_OFFSET_TOTAL,0);
            std::ifstream file_name;
            packet_struct_metalong meta;

            struct stat statbuf;
            if (!stat((tx.temppath + ".meta").c_str(), &statbuf) && statbuf.st_size >= COSMOS_SIZEOF(file_progress))
            {
                file_name.open(tx.temppath + ".meta", std::ios::out|std::ios::binary);
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


            // load metadata

            file_name.read((char *)&packet[0], PACKET_METALONG_OFFSET_TOTAL);
            if (file_name.eof())
            {
                return DATA_ERROR_SIZE_MISMATCH;
            }
            uint16_t crc;
            CRC16 calc_crc;
            file_name.read((char *)&crc, 2);
            if (file_name.eof())
            {
                return DATA_ERROR_SIZE_MISMATCH;
            }
            if (crc != calc_crc.calc((uint8_t *)&packet[0], PACKET_METALONG_OFFSET_TOTAL))
            {
                file_name.close();
                return DATA_ERROR_CRC;
            }
            deserialize_metadata(packet, meta);
            tx.tx_id = meta.tx_id;
            tx.node_name = meta.node_name;
            tx.agent_name = meta.agent_name;
            tx.file_name = meta.file_name;
            tx.filepath = data_base_path(tx.node_name, "outgoing", tx.agent_name, tx.file_name);
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
                    return DATA_ERROR_SIZE_MISMATCH;
                }
                if (crc != calc_crc.calc((uint8_t *)&progress_info, sizeof(progress_info)))
                {
                    file_name.close();
                    return DATA_ERROR_CRC;
                }

                tx.file_info.push_back(progress_info);
            } while(!file_name.eof());
            file_name.close();
            if (debug_error != nullptr)
            {
                debug_error->Printf("%.4f %.4f Main: read_meta: %s tx_id: %d chunks: %lu\n", tet.split(), dt.lap(), (tx.temppath + ".meta").c_str(), tx.tx_id, tx.file_info.size());
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
int32_t Transfer::set_enabled(uint8_t node_id, PACKET_TX_ID_TYPE tx_id, bool enabled)
{
    if (NodeData::check_node_id(node_id) <= 0)
    {
        return TRANSFER_ERROR_NODE;
    }
    if (check_tx_id(txq[(node_id)].outgoing, tx_id) <= 0)
    {
        return TRANSFER_ERROR_MATCH;
    }

    txq[(node_id)].outgoing.progress[tx_id].enabled = enabled;

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
int32_t Transfer::enable_single(string node_name, string file_name)
{
    if (node_name.empty())
    {
        return TRANSFER_ERROR_NODE;
    }
    uint8_t node_id = NodeData::lookup_node_id(node_name);
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
int32_t Transfer::enable_single(uint8_t node_id, string file_name)
{
    if (NodeData::check_node_id(node_id) <= 0)
    {
        return TRANSFER_ERROR_NODE;
    }
    if (file_name.empty())
    {
        return TRANSFER_ERROR_FILENAME;
    }

    bool file_found = false;

    // Attempt to find the file first
    for (auto &tx_out : txq[(node_id)].outgoing.progress)
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
    for (auto &tx_out : txq[(node_id)].outgoing.progress)
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
int32_t Transfer::enable_all(string node_name)
{
    if (node_name.empty())
    {
        return TRANSFER_ERROR_NODE;
    }
    uint8_t node_id = NodeData::lookup_node_id(node_name);
    return enable_all(node_id);
}

//! Enable all outgoing files
//! Sets the enabled status of all outgoing files to true.
//! \param node_id ID of receiving node
//! \return Number of files enabled
int32_t Transfer::enable_all(uint8_t node_id)
{
    if (NodeData::check_node_id(node_id) <= 0)
    {
        return TRANSFER_ERROR_NODE;
    }

    int32_t iretn = 0;

    for (auto &tx_out : txq[(node_id)].outgoing.progress)
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
    for (uint16_t node_id=0; node_id<txq.size(); ++node_id)
    {
        if ((NodeData::check_node_id(node_id)) > 0)
        {
            for(tx_progress tx : txq[(node_id)].outgoing.progress)
            {
                if (tx.tx_id)
                {
                    jlist.push_back(json11::Json::object {
                        { "tx_id", tx.tx_id },
                        { "file_name", tx.file_name },
                        { "file_size", tx.file_size },
                        { "node_name", tx.node_name },
                        { "enabled", tx.enabled }
                    });
                }
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
    for (uint16_t node_id=0; node_id<txq.size(); ++node_id)
    {
        if ((NodeData::check_node_id(node_id)) > 0)
        {
            for(tx_progress tx : txq[(node_id)].incoming.progress)
            {
                if (tx.tx_id)
                {
                    jlist.push_back(json11::Json::object {
                        { "tx_id", tx.tx_id },
                        { "file_name", tx.file_name },
                        { "file_size", tx.file_size },
                        { "total_bytes", tx.total_bytes },
                        { "sent_meta", tx.sentmeta }
                    });
                }
            }
        }
    }
    string s = json11::Json(jlist).dump();

    return s;
}