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

            Transfer();
            // int32_t Init(string node, string agent, uint16_t chunk_size);
            int32_t Init(const string calling_node_name);
            int32_t Init(const string calling_node_name, Log::Logger* debug_log);
            //int32_t Load(string filename, vector<chunk> &chunks);
            int32_t outgoing_tx_add(tx_progress &tx_out, const string dest_node_name);
            int32_t outgoing_tx_add(const string dest_node, const string dest_agent, const string file_name);
            int32_t outgoing_tx_load();
            int32_t outgoing_tx_load(const string node_name);
            int32_t outgoing_tx_load(const uint8_t node_id);
            int32_t outgoing_tx_recount(const string node_name);
            int32_t outgoing_tx_recount(const uint8_t node_id);
            int32_t incoming_tx_recount(const string node_name);
            int32_t incoming_tx_recount(const uint8_t node_id);
            int32_t get_outgoing_lpackets(vector<PacketComm> &packets);
            int32_t get_outgoing_lpackets(const string node_name, vector<PacketComm> &packets);
            int32_t get_outgoing_rpackets(vector<PacketComm> &packets);
            int32_t get_outgoing_rpackets(const string node_name, vector<PacketComm> &packets);
            int32_t get_outgoing_lpackets(const uint8_t node_id, vector<PacketComm> &packets);
            int32_t get_outgoing_rpackets(const uint8_t node_id, vector<PacketComm> &packets);
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

            // Getters/setters
            PACKET_CHUNK_SIZE_TYPE get_packet_size();
            int32_t set_packet_size(const PACKET_CHUNK_SIZE_TYPE size);

        private:
            /// The node_name of the calling node
            string self_node_name;
            /// The node_id of the calling node
            PACKET_NODE_ID_TYPE self_node_id;
            /// Maps a node_id to an index into txq. <node_id, txq_index>. Use node_id_to_txq_idx instead of trying to access the map directly with []operator.
            map<PACKET_NODE_ID_TYPE, size_t> node_id_to_txq_map;

            /// Vector of nodes to transfer files in/out. Heart of the file transfer manager.
            vector<tx_queue> txq;

            // Timers for debug messages
            ElapsedTime dt, tet;

            // Byte size limit of a packet
            PACKET_CHUNK_SIZE_TYPE packet_size = 217;

            // Pointer to calling agent's debug_log
            Log::Logger* debug_log = nullptr;

            // Private queue manipulation functions
            int32_t outgoing_tx_del(const uint8_t node_id, const PACKET_TX_ID_TYPE tx_id=PROGRESS_QUEUE_SIZE-1, const bool remove_file=true);
            PACKET_TX_ID_TYPE check_tx_id(const tx_entry &txentry, const PACKET_TX_ID_TYPE tx_id);
            int32_t incoming_tx_add(const string node_name, const PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc);
            int32_t incoming_tx_add(const string node_name, const packet_struct_metadata& meta);
            int32_t incoming_tx_add(tx_progress &tx_in);
            int32_t incoming_tx_update(const packet_struct_metadata &meta);
            int32_t incoming_tx_del(const uint8_t node_id, const PACKET_TX_ID_TYPE tx_id=PROGRESS_QUEUE_SIZE-1);
            int32_t incoming_tx_complete(const uint8_t node_id, const PACKET_TX_ID_TYPE tx_id=PROGRESS_QUEUE_SIZE-1);
            int32_t incoming_tx_data(packet_struct_data& data, uint8_t node_id, size_t orig_node_idx);

            // Reuse to write the META
            vector<uint8_t> meta_file_data;
            // Reuse to create outgoing lpackets
            PacketComm outgoing_packet;
            int32_t write_meta(tx_progress& tx, double interval=5.);
            int32_t read_meta(tx_progress& tx);

            static const size_t INVALID_TXQ_IDX = (size_t)-1;
            size_t node_id_to_txq_idx(const uint8_t node_id);

            // Reuse crc class
            CRC16 calc_crc;
        };
    }
}

#endif
