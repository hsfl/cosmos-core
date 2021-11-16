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

#include "support/configCosmos.h"
#include "support/transferclass.h"
#include "support/timelib.h"
#include "support/stringlib.h"

bool filestruc_smaller_by_size(const filestruc& a, const filestruc& b)
{
    return a.size < b.size;
}


namespace Cosmos {
    namespace Support {
        Transfer::Transfer()
        {
            calstruc cal = mjd2cal(currentmjd());
            txid = 100 * (utc2unixseconds(currentmjd()) - utc2unixseconds(cal2mjd(cal.year, 1.))) - 1;
        }

        int32_t Transfer::Init(string node, string agent, uint16_t chunk_size)
        {
            this->node = node;
            this->agent = agent;
            this->chunk_size = chunk_size;
            return 0;
        }

        int32_t Transfer::Init()
        {
            int32_t iretn;

            // if (static_cast<string>(argv[0]).find("slow") != string::npos)
            // {
            //     default_throughput = THROUGHPUT_LO;
            //     default_packet_size = PACKET_SIZE_LO;
            // }

            // Initialize Transfer Queue
            if ((iretn = load_nodeids()) < 2)
            {
                // agent->debug_error.Printf("%.4f Couldn't load node lookup table\n", tet.split());
                // agent->shutdown();
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
        //! Enqueues new file in outgoing_tx_queue.
        //! \return 0 if success
        int32_t Transfer::outgoing_tx_load()
        {
            int32_t iretn=0;

            // Go through outgoing queues, removing files that no longer exist
            for (size_t node_id = 0; node_id < txq.size(); ++node_id) {
                for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
                {
                    if (txq[node_id].outgoing.progress[i].tx_id != 0 && !data_isfile(txq[node_id].outgoing.progress[i].filepath))
                    {
                        //outgoing_tx_del(node_id, txq[(node_id)].outgoing.progress[i].tx_id);
                        cout << "outgoing_tx_del("<<node_id<<", txq["<<node_id<<"].outgoing.progress["<<i<<"].tx_id)"<<endl;
                    }
                }
            }
            // Go through outgoing directories, adding files not already in queue
            for (size_t node_id = 0; node_id < txq.size(); ++node_id) {
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
                        outgoing_tx_lock.lock();
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
                                        // if (agent->get_debug_level())
                                        // {
                                        //     debug_fd_lock.lock();
                                        //     agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: Enable %u %s %s %s %d\n", tet.split(), dt.lap(), txq[(node_id)].outgoing.progress[i].tx_id, txq[(node_id)].outgoing.progress[i].node_name.c_str(), txq[(node_id)].outgoing.progress[i].agent_name.c_str(), txq[(node_id)].outgoing.progress[i].filepath.c_str(), PROGRESS_QUEUE_SIZE);
                                        //     // fflush(agent->get_debug_fd());
                                        //     debug_fd_lock.unlock();

                                        // }
                                    }
                                    iretn = 0;
                                }
                                else
                                {
                                    //outgoing_tx_del(node_id, i, false);
                                    cout << "outgoing_tx_del("<<node_id<<", "<<i<<", false)"<<endl;
                                    addtoqueue = false;
                                    iretn = TRANSFER_ERROR_FILESIZE;
                                }
                                if (txq[(node_id)].outgoing.progress[i].enabled && txq[(node_id)].outgoing.progress[i].file_size == 0)
                                {
                                    //outgoing_tx_del(node_id, i, true);
                                    cout << "outgoing_tx_del("<<node_id<<", "<<i<<", true)"<<endl;
                                    addtoqueue = false;
                                    iretn = TRANSFER_ERROR_FILEZERO;
                                }
                            }
                        }

                        outgoing_tx_lock.unlock();

                        if (addtoqueue)
                        {
                            iretn = outgoing_tx_add(file.node, file.agent, file.name);
                            // if (agent->get_debug_level() && iretn != -471)
                            // {
                            //     debug_fd_lock.lock();
                            //     agent->debug_error.Printf("%.4f %.4f Main/Load: outgoing_tx_add: %s [%d]\n", tet.split(), dt.lap(), file.path.c_str(), iretn);
                            //     // fflush(agent->get_debug_fd());
                            //     debug_fd_lock.unlock();
                            // }
                        }
                    }
                }
            }

            return iretn;
        }

        int32_t Transfer::outgoing_tx_add(string node_name, string agent_name, string file_name)
        {
            if (node_name.empty() || agent_name.empty() || file_name.empty())
            {
                // if (agent->get_debug_level())
                // {
                //     debug_fd_lock.lock();
                //     agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: TRANSFER_ERROR_FILENAME\n", tet.split(), dt.lap());
                //     // fflush(agent->get_debug_fd());
                //     debug_fd_lock.unlock();
                // }
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
            outgoing_tx_lock.lock();
            string filepath = data_base_path(node_name, "outgoing", agent_name, file_name);
            int32_t file_size = get_file_size(filepath);

            // Go through existing queue
            // - if it is already there and the size is different, remove it
            // - if it is already there, and the size is the same, set enable it
            // - if it is not already there and there is room to add is, go on to next step
            for (uint16_t i=1; i<PROGRESS_QUEUE_SIZE; ++i)
            {
                if (txq[(node_id)].outgoing.progress[i].filepath == filepath)
                {
                    if (txq[(node_id)].outgoing.progress[i].file_size == file_size)
                    {
                        txq[(node_id)].outgoing.progress[i].enabled = true;
                        // if (agent->get_debug_level())
                        // {
                        //     debug_fd_lock.lock();
                        //     agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: Enable %u %s %s %s %d ", tet.split(), dt.lap(), txq[(node_id)].outgoing.progress[i].tx_id, txq[(node_id)].outgoing.progress[i].node_name.c_str(), txq[(node_id)].outgoing.progress[i].agent_name.c_str(), txq[(node_id)].outgoing.progress[i].filepath.c_str(), PROGRESS_QUEUE_SIZE);
                        //     // fflush(agent->get_debug_fd());
                        //     debug_fd_lock.unlock();
                        // }
                        return node_id;
                    }
                    else
                    {
                        //outgoing_tx_del(node_id, i, false);
                        cout << "outgoing_tx_del("<<node_id<<", "<<i<<", false)"<<endl;
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
            outgoing_tx_lock.unlock();

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

                //get the file size
        //        tx_out.file_size = get_file_size(tx_out.filepath);
                tx_out.file_size = file_size;

                if(tx_out.file_size < 0)
                {
                    // if (agent->get_debug_level())
                    // {
                    //     debug_fd_lock.lock();
                    //     agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: DATA_ERROR_SIZE_MISMATCH\n", tet.split(), dt.lap());
                    //     // fflush(agent->get_debug_fd());
                    //     debug_fd_lock.unlock();
                    // }
                    return DATA_ERROR_SIZE_MISMATCH;
                }

                // see if file can be opened
                filename.open(tx_out.filepath, std::ios::in|std::ios::binary);
                if(!filename.is_open())
                {
                    // if (agent->get_debug_level())
                    // {
                    //     debug_fd_lock.lock();
                    //     agent->debug_error.Printf("%.4f %.4f Main: outgoing_tx_add: %s\n", tet.split(), dt.lap(), cosmos_error_string(-errno).c_str());
                    //     // fflush(agent->get_debug_fd());
                    //     debug_fd_lock.unlock();
                    // }
                    return -errno;
                }
                filename.close();

                file_progress tp;
                tp.chunk_start = 0;
                tp.chunk_end = tx_out.file_size - 1;
                tx_out.file_info.push_back(tp);

                //write_meta(tx_out);

                //int32_t iretn = outgoing_tx_add(tx_out);
                cout << "outgoing_tx_add(tx_out); filepath: "<<tx_out.filepath<<endl;
                return 0;//iretn;
            }
            else
            {
                return TRANSFER_ERROR_MATCH;
            }
            return 0;
        }
    }
}
