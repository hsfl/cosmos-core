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
         * \param calling_agent Pointer to the agent creating this class
         * \return 0 if success
         */
        int32_t Transfer::Init(Agent *calling_agent)
        {
            int32_t iretn;
            agent = calling_agent;

            packet_size = 217;

            // if (static_cast<string>(argv[0]).find("slow") != string::npos)
            // {
            //     default_throughput = THROUGHPUT_LO;
            //     default_packet_size = PACKET_SIZE_LO;
            // }

            // Initialize Transfer Queue
            if ((iretn = load_nodeids()) < 2)
            {
                agent->debug_error.Printf("%.4f Couldn't load node lookup table\n", tet.split());
                agent->shutdown();
                exit (iretn);
            }
            txq.resize(iretn);
            for (uint16_t i=1; i<iretn; ++i)
            {
                if (check_node_id(i) > 0)
                {
                    txq[i].node_id = i;
                    txq[i].node_name = lookup_node_id_name(i);
                }
            }

        //     // Restore in progress transfers from previous run
        //     for (string node_name : data_list_nodes())
        //     {
        // //        int32_t node_id = add_node_name(node_name);

        //         for(filestruc file : data_list_files(node_name, "temp", "file"))
        //         {
        //             // Add entry for each meta file
        //             if (file.type == "meta")
        //             {
        //                 // Incoming
        //                 if (!file.name.compare(0,3,"in_"))
        //                 {
        //                     tx_progress tx_in;
        //                     tx_in.temppath = file.path.substr(0,file.path.find(".meta"));
        //                     if (read_meta(tx_in) >= 0)
        //                     {
        //                         merge_chunks_overlap(tx_in);
        //                         iretn = incoming_tx_add(tx_in);
        //                     }
        //                 }

        //                 // Outgoing
        //                 if (!file.name.compare(0,4,"out_"))
        //                 {
        //                     tx_progress tx_out;
        //                     tx_out.temppath = file.path.substr(0,file.path.find(".meta"));
        //                     if (read_meta(tx_out) >= 0)
        //                     {
        //                         find_chunks_togo(tx_out);
        //                         iretn = outgoing_tx_add(tx_out);
        //                     }
        //                 }
        //             }
        //         }
        //     }

            return 0;
        }


        /*int32_t Transfer::Load(string filename, vector<chunk> &chunks)
        {
            int32_t iretn;
            FILE *fp = fopen(filename.c_str(), "r");
            if (filename.find(".xfr") != string::npos)
            {
                json.resize(data_size(filename));
                fgets((char *)json.data(), json.size(), fp);
                string estring;
                json11::Json jmeta = json11::Json::parse(json.data(), estring);
                name = jmeta["name"].string_value();
                size = jmeta["size"].number_value();
                node = jmeta["node"].string_value();
                agent = jmeta["agent"].string_value();
                txid = jmeta["txid"].number_value();
                chunk_size = jmeta["chunksize"].number_value();
            }
            else
            {
                ++txid;
                name = filename;
                size = data_size(name);
                if (!size)
                {
                    return (GENERAL_ERROR_BAD_SIZE);
                }
                if (!chunks.size())
                {
                    chunk tchunk;
                    tchunk.start = 0;
                    tchunk.end = ((size - 1) / chunk_size);
                    chunks.push_back(tchunk);
                }
                json = "{";
                json += to_json("txid", txid);
                json += "," + to_json("name", name);
                json += "," + to_json("size", size);
                json += "," + to_json("node", node);
                json += "," + to_json("agent", agent);
                json += "," + to_json("chunksize", chunk_size);
                json += "}";
            }
            meta = vector<uint8_t>(json.begin(), json.end());
            data.resize(1+((size-1) / chunk_size));
            for (chunk tchunk : chunks)
            {
                for (uint32_t chunkidx=tchunk.start; chunkidx <= tchunk.end; ++chunkidx)
                {
                    data[chunkidx].resize(chunk_size);
                    if (fseek(fp, chunkidx*chunk_size, SEEK_SET) == 0 && (iretn=fread(data[chunkidx].data(), 1, chunk_size, fp)) > 0)
                    {
                        if ((size_t)iretn < chunk_size)
                        {
//                            data[chunkidx].resize(iretn);
                            break;
                        }
                    }
                }
            }
            fclose(fp);
            return size;
        }*/

        //! Scan the outgoing directory of every node in txq.
        //! Enqueues new files in txq's outgoing queue.
        //! Not thread safe.
        //! \return 0 if success
        int32_t Transfer::outgoing_tx_load()
        {
            int32_t iretn=0;

            // Go through outgoing queues for all nodes
            for (uint8_t node_id = 0; node_id < txq.size(); ++node_id) {
                iretn = outgoing_tx_load(node_id);
            }
            return iretn;
        }

        //! Scan the outgoing directory of specified node in txq.
        //! Enqueues new files in its outgoing queue.
        //! Not thread safe.
        //! \return 0 if success
        int32_t Transfer::outgoing_tx_load(uint8_t node_id)
        {
            int32_t iretn=0;

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
                        iretn = data_list_files(txq[(node_id)].node_name, "outgoing", file.name, file_names);
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

                    // Ignore zero length files (may still be being formed)
                    // if (file.size == 0)
                    // {
                    //     continue;
                    // }

                    // Go through existing queue
                    // - if it is already there and the size is different, remove it from queue
                    // - if it is already there, and the size is the same, enable it
                    // - if it is enabled and the size is zero, remove it from queue and remove file
                    bool addtoqueue = true;
                    for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
                    {
                        if (txq[(node_id)].outgoing.progress[i].filepath == file.path)
                        {
                            if (txq[(node_id)].outgoing.progress[i].file_size == file.size)
                            {
                                addtoqueue = false;
                                if (!txq[(node_id)].outgoing.progress[i].enabled)
                                {
                                    txq[(node_id)].outgoing.progress[i].enabled = true;
                                    if (agent->get_debug_level())
                                    {
                                        agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: Enable %u %s %s %s %d\n", tet.split(), dt.lap(), txq[(node_id)].outgoing.progress[i].tx_id, txq[(node_id)].outgoing.progress[i].node_name.c_str(), txq[(node_id)].outgoing.progress[i].agent_name.c_str(), txq[(node_id)].outgoing.progress[i].filepath.c_str(), PROGRESS_QUEUE_SIZE);
                                    }
                                }
                                iretn = 0;
                            }
                            else
                            {
                                outgoing_tx_del(node_id, i, false);
                                addtoqueue = false;
                                iretn = TRANSFER_ERROR_FILESIZE;
                            }
                            if (txq[(node_id)].outgoing.progress[i].enabled && txq[(node_id)].outgoing.progress[i].file_size == 0)
                            {
                                outgoing_tx_del(node_id, i, true);
                                addtoqueue = false;
                                iretn = TRANSFER_ERROR_FILEZERO;
                            }
                        }
                    }

                    if (addtoqueue)
                    {
                        iretn = outgoing_tx_add(file.node, file.agent, file.name);
                        if (agent->get_debug_level() && iretn != -471)
                        {
                            agent->debug_error.Printf("%.4f %.4f Main/Load: outgoing_tx_add: %s [%d]\n", tet.split(), dt.lap(), file.path.c_str(), iretn);
                        }
                    }
                }
            }

            return iretn;
        }

        //! Look through the outgoing and incoming queues of all nodes and generate any necessary packets.
        //! Not thread safe.
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        int32_t Transfer::get_outgoing_packets(vector<PacketComm> &packets)
        {
            for (uint8_t node_id = 0; node_id < txq.size(); ++node_id) {
                get_outgoing_packets(node_id, packets);
            }
            
            return 0;
        }

        //! Look through the outgoing and incoming queues of specified node and generate any necessary packets.
        //! Not thread safe.
        //! \param packets Outgoing packets will be pushed onto this vector of PacketComm packets.
        //! \return
        int32_t Transfer::get_outgoing_packets(uint8_t node_id, vector<PacketComm> &packets) {
            // Send Queue packet, if anything needs to be queued
            if (/*txq[(node_id)].outgoing.sendqueue &&*/ !txq[(node_id)].outgoing.sentqueue)
            {
                vector<PACKET_TX_ID_TYPE> tqueue(TRANSFER_QUEUE_LIMIT, 0);
                PACKET_TX_ID_TYPE iq = 0;
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].outgoing.progress[tx_id].tx_id == tx_id)
                    {
                        tqueue[iq++] = txq[(node_id)].outgoing.progress[tx_id].tx_id;
                    }
                    if (iq == TRANSFER_QUEUE_LIMIT)
                    {
                        break;
                    }
                }
                if (iq)
                {
                    PacketComm packet;
                    serialize_queue(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].node_name, tqueue);
                    packets.push_back(packet);
                    txq[(node_id)].outgoing.sendqueue = false;
                    txq[(node_id)].outgoing.sentqueue = true;
                    txq[(node_id)].outgoing.activity = true;
                }
            }

            // Send any pending Metadata packets
            if (txq[(node_id)].outgoing.sentqueue)
            {
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].outgoing.progress[tx_id].tx_id == tx_id && txq[(node_id)].outgoing.progress[tx_id].sendmeta && !txq[(node_id)].outgoing.progress[tx_id].sentmeta)
                    {
                        tx_progress tx = txq[(node_id)].outgoing.progress[tx_id];
                        PacketComm packet;
                        serialize_metadata(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), tx.tx_id, (char *)tx.file_name.c_str(), tx.file_size, (char *)tx.agent_name.c_str());
                        packets.push_back(packet);
                        txq[(node_id)].outgoing.activity = true;
                        txq[(node_id)].outgoing.progress[tx_id].sendmeta = false;
                        txq[(node_id)].outgoing.progress[tx_id].havemeta = true;
                        txq[(node_id)].outgoing.progress[tx_id].sentmeta = true;
                        break;
                    }
                }
            }

            // Send Data packets if we have data to send and we didn't do anything above
            if (txq[(node_id)].outgoing.sentqueue)
            {
                uint16_t tx_id = choose_outgoing_tx_id((node_id));
                if (txq[(node_id)].outgoing.progress[tx_id].tx_id == tx_id && txq[(node_id)].outgoing.progress[tx_id].sentmeta && txq[(node_id)].outgoing.progress[tx_id].senddata)
                {
                    if (txq[(node_id)].outgoing.progress[tx_id].file_size)
                    {
                        // Check if there is any more data to send
                        if (txq[(node_id)].outgoing.progress[tx_id].total_bytes == 0)
                        {
                            txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                            txq[(node_id)].outgoing.progress[tx_id].senddata = false;
                        }
                        else
                        {
                            // Attempt to open the outgoing progress file
                            if (txq[(node_id)].outgoing.progress[tx_id].fp == nullptr)
                            {
                                txq[(node_id)].outgoing.progress[tx_id].fp = fopen(txq[(node_id)].outgoing.progress[tx_id].filepath.c_str(), "r");
                            }

                            // If we're good, continue with the process
                            if(txq[(node_id)].outgoing.progress[tx_id].fp != nullptr)
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
                                    txq[(node_id)].outgoing.activity = true;
                                }
                                else
                                {
                                    // Some problem with this transmission, ask other end to dequeue it
                                    // Remove transaction
                                    txq[(node_id)].outgoing.progress[tx_id].senddata = false;
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
                                }

                                // write_meta(txq[(node_id)].outgoing.progress[tx_id]);
                            }
                            else
                            {
                                // Some problem with this transmission, ask other end to dequeue it

                                txq[(node_id)].outgoing.progress[tx_id].senddata = false;
                                txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                                txq[(node_id)].outgoing.progress[tx_id].complete = true;
                            }
                        }
                    }
                    // Zero length file, ask other end to dequeue it
                    else
                    {
                        txq[(node_id)].outgoing.progress[tx_id].senddata = false;
                        txq[(node_id)].outgoing.progress[tx_id].sentdata = true;
                        txq[(node_id)].outgoing.progress[tx_id].complete = true;
                    }
                }
            }

            // Send Cancel packets if required
            if (txq[(node_id)].outgoing.sentqueue)
            {
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].outgoing.progress[tx_id].tx_id == tx_id && txq[(node_id)].outgoing.progress[tx_id].complete)
                    {
                        // Remove from queue
                        outgoing_tx_del(node_id, tx_id);

                        // Send a CANCEL packet
                        PacketComm packet;
                        serialize_cancel(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), tx_id);
                        packets.push_back(packet);
                        txq[(node_id)].outgoing.activity = true;
                    }
                }
            }

            // Send Reqmeta packet if needed and it has been otherwise quiet
            if (!txq[(node_id)].incoming.rcvdqueue && !txq[(node_id)].incoming.rcvdmeta && txq[(node_id)].incoming.reqmetaclock < currentmjd())
            {
                vector<PACKET_TX_ID_TYPE> tqueue (TRANSFER_QUEUE_LIMIT, 0);
                PACKET_TX_ID_TYPE iq = 0;
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].incoming.progress[tx_id].tx_id && !txq[(node_id)].incoming.progress[tx_id].sentmeta)
                    {
                        tqueue[iq++] = tx_id;
                        txq[(node_id)].incoming.progress[tx_id].sendmeta = true;
                    }
                    if (iq == TRANSFER_QUEUE_LIMIT)
                    {
                        break;
                    }
                }
                if (iq)
                {
                    PacketComm packet;
                    serialize_reqmeta(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].node_name, tqueue);
                    packets.push_back(packet);
                }
                txq[(node_id)].incoming.reqmetaclock = currentmjd() + 5. / 86400.;
            }

            // Send Reqdata packet if there is still data to be gotten and it has been otherwise quiet
            if (!txq[(node_id)].incoming.rcvdqueue && !txq[(node_id)].incoming.rcvdmeta && !txq[(node_id)].incoming.rcvddata && txq[(node_id)].incoming.reqdataclock < currentmjd())
            {
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].incoming.progress[tx_id].tx_id)
                    {
                        // Ask for missing data
                        vector<file_progress> missing;
                        missing = find_chunks_missing(txq[(node_id)].incoming.progress[tx_id]);
                        for (uint32_t j=0; j<missing.size(); ++j)
                        {
                            PacketComm packet;
                            serialize_reqdata(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), txq[(node_id)].incoming.progress[tx_id].tx_id, missing[j].chunk_start, missing[j].chunk_end);
                            packets.push_back(packet);
                        }
                    }
                }
                txq[(node_id)].incoming.reqdataclock = currentmjd() + 5. / 86400.;
            }

            // Send Complete packets if required
            if (!txq[(node_id)].incoming.rcvdqueue && !txq[(node_id)].incoming.rcvdmeta && !txq[(node_id)].incoming.rcvddata)
            {
                for (uint16_t tx_id=1; tx_id<PROGRESS_QUEUE_SIZE; ++tx_id)
                {
                    if (txq[(node_id)].incoming.progress[tx_id].tx_id == tx_id && txq[node_id].incoming.progress[tx_id].file_size == txq[node_id].incoming.progress[tx_id].total_bytes)
                    {
                        // Remove from queue
                        txq[(node_id)].incoming.progress[tx_id].complete = true;
                        incoming_tx_del(node_id, tx_id);

                        // Send a COMPLETE packet
                        PacketComm packet;
                        serialize_complete(packet, static_cast <PACKET_NODE_ID_TYPE>(node_id), tx_id);
                        packets.push_back(packet);
                        txq[(node_id)].incoming.activity = true;
                    }
                }
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
                if (agent->get_debug_level())
                {
                    agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_FILENAME\n", tet.split(), dt.lap());
                }
                return TRANSFER_ERROR_FILENAME;
            }

            // BEGIN GATHERING THE METADATA
            tx_progress tx_out;

            uint8_t node_id = lookup_node_id(node_name);
            if (node_id == 0)
            {
                return TRANSFER_ERROR_NODE;
            }

            // Only add if we have room
            if (txq[(node_id)].outgoing.size == PROGRESS_QUEUE_SIZE-1)
            {
                return TRANSFER_ERROR_QUEUEFULL;
            }

            // Locate next empty space
            //get the file size
            string filepath = data_base_path(node_name, "outgoing", agent_name, file_name);
            int32_t file_size = get_file_size(filepath);

            // Go through existing queue
            // - if it is already there and the size is different, remove it
            // - if it is already there, and the size is the same, set enable it
            // - if it is not already there and there is room to add it, go on to next step
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[(node_id)].outgoing.progress[i].filepath == filepath)
                {
                    if (txq[(node_id)].outgoing.progress[i].file_size == file_size)
                    {
                        txq[(node_id)].outgoing.progress[i].enabled = true;
                        if (agent->get_debug_level())
                        {
                            agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: Enable %u %s %s %s %d ", tet.split(), dt.lap(), txq[(node_id)].outgoing.progress[i].tx_id, txq[(node_id)].outgoing.progress[i].node_name.c_str(), txq[(node_id)].outgoing.progress[i].agent_name.c_str(), txq[(node_id)].outgoing.progress[i].filepath.c_str(), PROGRESS_QUEUE_SIZE);
                        }
                        return node_id;
                    }
                    else
                    {
                        outgoing_tx_del(node_id, i, false);
                        return TRANSFER_ERROR_FILESIZE;
                    }
                }
            }

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
                tx_out.havemeta = true;
                tx_out.sendmeta = true;
                tx_out.sentmeta = false;
                tx_out.senddata = false;
                tx_out.sentdata = false;
                tx_out.complete = false;
                tx_out.total_bytes = 0;
                tx_out.node_name = node_name;
                tx_out.agent_name = agent_name;
                tx_out.file_name = file_name;
                tx_out.temppath = data_base_path(tx_out.node_name, "temp", "file", "out_"+std::to_string(tx_out.tx_id));
                tx_out.filepath = filepath;
        //        tx_out.filepath = data_base_path(tx_out.node_name, "outgoing", tx_out.agent_name, tx_out.file_name);
                tx_out.savetime = 0.;

                std::ifstream filename;

                // get the file size
        //        tx_out.file_size = get_file_size(tx_out.filepath);
                tx_out.file_size = file_size;

                if(tx_out.file_size < 0)
                {
                    if (agent->get_debug_level())
                    {
                        agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: DATA_ERROR_SIZE_MISMATCH\n", tet.split(), dt.lap());
                    }
                    return DATA_ERROR_SIZE_MISMATCH;
                }

                // see if file can be opened
                filename.open(tx_out.filepath, std::ios::in|std::ios::binary);
                if(!filename.is_open())
                {
                    if (agent->get_debug_level())
                    {
                        agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: %s\n", tet.split(), dt.lap(), cosmos_error_string(-errno).c_str());
                    }
                    return -errno;
                }
                filename.close();

                file_progress tp;
                tp.chunk_start = 0;
                tp.chunk_end = tx_out.file_size - 1;
                tx_out.file_info.push_back(tp);

                //write_meta(tx_out);

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
            if (agent->get_debug_level())
            {
                agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: ", tet.split(), dt.lap());
            }

            int32_t node_id = lookup_node_id(tx_out.node_name);
            if (node_id <= 0)
            {
                if (agent->get_debug_level())
                {
                    agent->debug_error.Printf("TRANSFER_ERROR_NODE\n");
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
            // Only add if we have room
        //    if (txq[(node_id)].outgoing.size == PROGRESS_QUEUE_SIZE)
        //    {
        //        if (agent->get_debug_level())
        //        {
        //            agent->debug_error.Printf("TRANSFER_ERROR_QUEUEFULL\n");
        //        }
        //        return TRANSFER_ERROR_QUEUEFULL;
        //    }

            //    tx_out.state = STATE_QUEUE;
            if (tx_out.file_name.size())
            {
                tx_out.filepath = data_base_path(tx_out.node_name, "outgoing", tx_out.agent_name, tx_out.file_name);
            }
            else
            {
                if (agent->get_debug_level())
                {
                    agent->debug_error.Printf("TRANSFER_ERROR_FILENAME\n");
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
                    if (agent->get_debug_level())
                    {
                        agent->debug_error.Printf("%u %s %s %s TRANSFER_ERROR_DUPLICATE\n", tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.filepath.c_str());
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
            tx_out.havemeta = true;
            tx_out.sendmeta = true;
            tx_out.sentmeta = false;
            tx_out.senddata = false;
            tx_out.sentdata = false;
            tx_out.complete = false;

            if (agent->get_debug_level())
            {
                agent->debug_error.Printf("%u %s %s %s %d ", tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.filepath.c_str(), PROGRESS_QUEUE_SIZE);
            }

            // Good to go. Add it to queue.

            txq[(node_id)].outgoing.progress[tx_out.tx_id].tx_id = tx_out.tx_id;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].havemeta = tx_out.havemeta;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].sendmeta = tx_out.sendmeta;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].sentmeta = tx_out.sentmeta;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].senddata = tx_out.senddata;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].sentdata = tx_out.sentdata;
            txq[(node_id)].outgoing.progress[tx_out.tx_id].complete = tx_out.complete;
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

            if (agent->get_debug_level())
            {
                agent->debug_error.Printf(" %u\n", txq[(node_id)].outgoing.size);
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
                txq[(node_id)].outgoing.progress[tx_id].havemeta = false;
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

                // Remove the file
                if(remove_file && remove(tx_out.filepath.c_str()))
                {
                    if (agent->get_debug_level())
                    {
                        agent->debug_error.Printf("%.4f %.4f Main/Outgoing: Del outgoing: %u %s %s %s - Unable to remove file\n", tet.split(), dt.lap(), tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str());
                    }
                }

                // Remove the META file
                string meta_filepath = tx_out.temppath + ".meta";
                remove(meta_filepath.c_str());

                if (agent->get_debug_level())
                {
                    agent->debug_error.Printf("%.4f %.4f Main/Outgoing: Del outgoing: %u %s %s %s\n", tet.split(), dt.lap(), tx_out.tx_id, tx_out.node_name.c_str(), tx_out.agent_name.c_str(), tx_out.file_name.c_str());
                }
            }

            return outgoing_tx_recount(node_id);
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
                txq[(node_id)].outgoing.progress[tx_id].senddata = true;
            }
            return tx_id;
        }

        int32_t Transfer::incoming_tx_del(uint8_t node_id, uint16_t tx_id)
        {
            node_id = check_node_id(node_id);
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

                tx_progress tx_in = txq[(node_id)].incoming.progress[tx_id];

                txq[(node_id)].incoming.progress[tx_id].tx_id = 0;
                txq[(node_id)].incoming.progress[tx_id].havemeta = false;
                txq[(node_id)].incoming.progress[tx_id].havedata = false;
                txq[(node_id)].incoming.progress[tx_id].complete = false;
                if (txq[(node_id)].incoming.size)
                {
                    --txq[(node_id)].incoming.size;
                }

                // Move file to its final location
                if (tx_in.complete)
                {
                    // Close the DATA file
                    if (tx_in.fp != nullptr)
                    {
                        fclose(tx_in.fp);
                        tx_in.fp = nullptr;
                    }
                    string final_filepath = tx_in.temppath + ".file";
                    int iret = rename(final_filepath.c_str(), tx_in.filepath.c_str());
                    // Make sure metadata is recorded
                    //write_meta(tx_in, 0.);
                    if (agent->get_debug_level())
                    {
                        agent->debug_error.Printf("%.4f %.4f Incoming: Renamed/Data: %d %s\n", tet.split(), dt.lap(), iret, tx_in.filepath.c_str());
                    }

                    // Mark complete
                    tx_in.complete = true;
                    tx_in.senddata = false;
                    tx_in.sentdata = true;
                }

                string filepath;
                // Remove the DATA file
                filepath = tx_in.temppath + ".file";
                remove(filepath.c_str());

                // Remove the META file
                filepath = tx_in.temppath + ".meta";
                remove(filepath.c_str());

                if (agent->get_debug_level())
                {
                    agent->debug_error.Printf("%.4f %.4f Incoming: Del incoming: %u %s\n", tet.split(), dt.lap(), tx_in.tx_id, tx_in.node_name.c_str());
                }
            }

            return incoming_tx_recount(node_id);
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

        int32_t Transfer::check_node_id(PACKET_NODE_ID_TYPE node_id)
        {
            int32_t iretn;

            if ((iretn=load_nodeids()) <= 0)
            {
                return iretn;
            }


            if (node_id > 0 && nodeids[node_id].size())
            {
                return node_id;
            }
            else
            {
                return 0;
            }
        }

        int32_t Transfer::lookup_node_id(string node_name)
        {
            int32_t iretn;

            if ((iretn=load_nodeids()) <= 0)
            {
                return iretn;
            }

            uint8_t node_id = 0;
            for (uint8_t i=1; i<nodeids.size(); ++i)
            {
                if (nodeids[i] == node_name)
                {
                    node_id = i;
                    break;
                }
            }

            return node_id;
        }

        string Transfer::lookup_node_id_name(PACKET_NODE_ID_TYPE node_id)
        {
            string name;
            if (load_nodeids() > 0 && node_id > 0 && nodeids[node_id].size())
            {
                return nodeids[node_id];
            }
            else
            {
                return "";
            }
        }

        int32_t Transfer::load_nodeids()
        {
            char buf[103];
            if (nodeids.size() == 0)
            {
                FILE *fp = data_open(get_cosmosnodes()+"/nodeids.ini", "rb");
                if (fp)
                {
                    uint16_t max_index = 0;
                    vector<uint16_t> tindex;
                    vector<string> tnodeid;
                    while (fgets(buf, 102, fp) != nullptr)
                    {
                        uint16_t index = 0;
                        string nodeid;
                        if (buf[strlen(buf)-1] == '\n')
                        {
                            buf[strlen(buf)-1] = 0;
                        }
                        if (buf[1] == ' ')
                        {
                            buf[1] = 0;
                            index = atoi(buf);
                            nodeid = &buf[2];
                        }
                        else if (buf[2] == ' ')
                        {
                            buf[2] = 0;
                            index = atoi(buf);
                            nodeid = &buf[3];
                        }
                        else if (buf[3] == ' ')
                        {
                            buf[3] = 0;
                            index = atoi(buf);
                            nodeid = &buf[4];
                        }
                        else
                        {
                            index = 0;
                        }
                        if (index)
                        {
                            if (index > max_index)
                            {
                                max_index = index;
                            }
                            tindex.push_back(index);
                            tnodeid.push_back(nodeid);
                        }
                    }
                    fclose(fp);
                    nodeids.resize(max_index+1);
                    for (uint16_t i=0; i<tindex.size(); ++i)
                    {
                        nodeids[tindex[i]] = tnodeid[i];
                    }
                }
                else
                {
                    return -errno;
                }
            }

            return nodeids.size();
        }

    }
}
