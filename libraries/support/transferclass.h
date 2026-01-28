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

#ifndef TRANSFERCLASS_H
#define TRANSFERCLASS_H

// cosmos includes
#include "agent/agentclass.h"
#include "support/configCosmos.h"
#include "support/datalib.h"
#include "support/packetcomm.h"
#include "support/stringlib.h"
#include "support/timelib.h"
#include "support/transferlib.h"

// This class handles the file transfer protocol
// A flowchart can be found at https://app.diagrams.net/#G19nJZLP2bCVitW9KCbkADq6HpnASu0Ddl

namespace Cosmos {
    namespace Support {
        enum class SendRetVal : int32_t {
            QUIT = 1,           //!< Success, but stop is requested
            SUCCESS = 0,        //!< Success, continue as normal
            ERROR_ABORT = -1,   //!< Error occurred, return from the current function
        };
        /**
         * @brief Virtual interface to send packets.
         */
        class Sender {
        public:
            virtual ~Sender() = default;
            /**
             * @brief Send a packet.
             * 
             * @param packet The packet to send. It is expected for implementations to handle the packet's serialization.
             * @return int32_t Returns 0 on success, or an error code on failure that signals to the transfer process what to do next.
             */
            virtual SendRetVal send(PacketComm& packet) = 0;

            /**
             * @brief Get the number of packets sent out of this sender object.
             * @return size_t
             */
            size_t get_number_of_packets_sent() const { return num_packets_sent_; }

        private:
            //! Internal counter for number of packets sent. It is expected for implementations to increment this.
            size_t num_packets_sent_ = 0;
        };
        class Transfer {
        public:
            // For specifying what types of packets to get from get_outgoing_packets()
            typedef uint8_t GET_OUTGOING_MODE;
            enum {
                GET_OUTGOING_ALL       = 0,
                GET_OUTGOING_RESPONSES = 1,
            };
            //! Returned by receive_packet() if there was something of concern requring the receive to send out a response-type packet of some sort (e.g., REQMETA, REQDATA, etc.)
            static const int32_t RESPONSE_REQUIRED = 1;

            Transfer(Sender* sender) : sender_(sender) {}
            // int32_t Init(string node, string agent, uint16_t chunk_size);
            int32_t Init(cosmosstruc *cinfo, bool keep_errored_files);
            int32_t Init(cosmosstruc *cinfo, Log::Logger* debug_log, bool keep_errored_files);
            //int32_t Load(string filename, vector<chunk> &chunks);
            int32_t outgoing_tx_add(tx_progress &tx_out, const string dest_node_name);
            int32_t outgoing_tx_add(const string dest_node, const string dest_agent, const string file_name);
            int32_t outgoing_tx_load();
            int32_t outgoing_tx_load(const string node_name, const string& specific_directory="");
            int32_t outgoing_tx_load(const uint8_t node_id, const string& specific_directory="");
            int32_t outgoing_tx_recount(const string node_name);
            int32_t outgoing_tx_recount(const uint8_t node_id);
            int32_t incoming_tx_recount(const size_t orig_node_idx);
            int32_t send_outgoing_lpackets(const uint8_t dest_node_id);
            int32_t send_outgoing_rpackets(const uint8_t orig_node_id);
            int32_t receive_packet(const PacketComm& packet);

            // Various publicly available requests
            int32_t set_enabled(const uint8_t node_id, const PACKET_TX_ID_TYPE tx_id, bool enabled);
            int32_t enable_single(const string node_name, const string file_name);
            int32_t enable_single(const uint8_t node_id, const string file_name);
            int32_t enable_all(const string node_name);
            int32_t enable_all(const uint8_t node_id);
            string list_outgoing();
            string list_incoming();
            int32_t set_waittime(const uint8_t node_id, const uint8_t direction, const double waittime);
            int32_t set_waittime(const string node_name, const uint8_t direction, const double waittime);
            int32_t reset_queue();
            int32_t reset_queue(string node_name);
            int32_t reset_queue(uint8_t node_id, uint8_t direction);
            int32_t close_file_pointers(uint8_t node_id, uint8_t direction);
            int32_t save_progress(uint8_t node_id);

            // Getters/setters
            PACKET_CHUNK_SIZE_TYPE get_packet_size();
            int32_t set_packet_size(const PACKET_CHUNK_SIZE_TYPE size);

            const Sender* get_sender() { return sender_; }

            // Print out packets for debugging
            void print_file_packet(const PacketComm& packet, uint8_t direction, string type, Log::Logger* debug_log);


        private:
            //! Associated cosmosstruc
            cosmosstruc *cinfo;
            /// The node_name of the calling node
            string self_node_name;
            /// The node_id of the calling node
            PACKET_NODE_ID_TYPE self_node_id;
            /// Maps a node_id to an index into txq. <node_id, txq_index>. Use node_id_to_txq_idx instead of trying to access the map directly with []operator.
            map<PACKET_NODE_ID_TYPE, size_t> node_id_to_txq_map;

            /// Sender interface for sending packets
            Sender* sender_;

            /// Vector of nodes to transfer files in/out. Heart of the file transfer manager.
            vector<tx_queue> txq;

            // Timers for debug messages
            ElapsedTime dt, tet;
            // Disable to surpress DATA type printing
            bool verbose_log = false;

            // Byte size limit of a packet
            PACKET_CHUNK_SIZE_TYPE packet_size = 217;

            // The maximum number of files to scan in each outgoing/* folder
            uint16_t files_to_scan_per_dir = 50;

            // Pointer to calling agent's debug_log
            Log::Logger* debug_log = nullptr;

            // Private queue manipulation functions
            int32_t outgoing_tx_del(const uint8_t node_id, const PACKET_TX_ID_TYPE tx_id=PROGRESS_QUEUE_SIZE-1, const bool remove_file=true);
            SendRetVal outgoing_tx_data(const uint8_t dest_node_id, const size_t dest_node_idx, const PACKET_TX_ID_TYPE tx_id);
            int32_t outgoing_tx_reqdata(const uint8_t orig_node_id, const size_t orig_node_idx, const PACKET_TX_ID_TYPE tx_id);
            PACKET_TX_ID_TYPE check_tx_id(const tx_entry &txentry, const PACKET_TX_ID_TYPE tx_id);
            int32_t incoming_tx_add(const string node_name, const PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc);
            int32_t incoming_tx_add(const string node_name, const packet_struct_metadata& meta);
            int32_t incoming_tx_add(tx_progress &tx_in);
            int32_t incoming_tx_update(const packet_struct_metadata &meta);
            int32_t incoming_tx_del(const uint8_t orig_node_idx, const PACKET_TX_ID_TYPE tx_id=PROGRESS_QUEUE_SIZE-1);
            int32_t incoming_tx_complete(const size_t orig_node_idx, const PACKET_TX_ID_TYPE tx_id=PROGRESS_QUEUE_SIZE-1, bool delete_file=false);
            int32_t incoming_tx_data(packet_struct_data& data, size_t orig_node_idx);

            // Reuse to write the META
            vector<uint8_t> meta_file_data;
            // Reuse to create outgoing lpackets
            PacketComm outgoing_packet;
            int32_t write_meta(tx_progress& tx, double interval=5.);
            int32_t read_meta(tx_progress& tx);

            static const size_t INVALID_TXQ_IDX = (size_t)-1;
            size_t node_id_to_txq_idx(const uint8_t node_id);
            PACKET_NODE_ID_TYPE txq_idx_to_node_id(const size_t node_idx);

            // Whether to keep copies of files that encountered errors and were cancelled
            bool keep_errored_files = false;

            // Reuse crc class
            CRC16 calc_crc;
        };
    }
}

#endif
