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
#include "support/transferlib2.h"

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
            int32_t Init(Agent *calling_agent);
            //int32_t Load(string filename, vector<chunk> &chunks);
            int32_t outgoing_tx_load();
            int32_t outgoing_tx_load(uint8_t node_id);
            int32_t get_outgoing_packets(vector<PacketComm> &packets, GET_OUTGOING_MODE mode = GET_OUTGOING_ALL);
            int32_t get_outgoing_packets(string node_name, vector<PacketComm> &packets, GET_OUTGOING_MODE mode = GET_OUTGOING_ALL);
            int32_t get_outgoing_packets(uint8_t node_id, vector<PacketComm> &packets, GET_OUTGOING_MODE mode = GET_OUTGOING_ALL);
            int32_t receive_packet(const PacketComm& packet);

            // Various checks
            int32_t check_node_id(PACKET_NODE_ID_TYPE node_id);
            int32_t lookup_node_id(string node_name);
            string lookup_node_id_name(PACKET_NODE_ID_TYPE node_id);


        private:
            /// The calling agent of this class.
            Agent *agent;

            /// The heart of the file transfer manager.
            vector<tx_queue> txq;

            /// The Node ID lookup table
            vector<string> nodeids;

            // Timers for debug messages
            ElapsedTime dt, tet;

            // Byte size limit of a packet
            PACKET_CHUNK_SIZE_TYPE packet_size;

            // Private queue manipulation functions
            int32_t outgoing_tx_add(tx_progress &tx_out);
            int32_t outgoing_tx_add(string node_name, string agent_name, string file_name);
            int32_t outgoing_tx_del(uint8_t node_id, uint16_t tx_id=PROGRESS_QUEUE_SIZE, bool remove_file=true);
            int32_t outgoing_tx_recount(uint8_t node_id);
            PACKET_TX_ID_TYPE choose_outgoing_tx_id(uint8_t node_id);
            PACKET_TX_ID_TYPE check_tx_id(tx_entry &txentry, PACKET_TX_ID_TYPE tx_id);
            int32_t incoming_tx_add(string node_name, PACKET_TX_ID_TYPE tx_id);
            int32_t incoming_tx_add(tx_progress &tx_in);
            int32_t incoming_tx_update(packet_struct_metashort meta);
            int32_t incoming_tx_del(uint8_t node, uint16_t tx_id=PROGRESS_QUEUE_SIZE);
            int32_t incoming_tx_recount(uint8_t node_id);

            int32_t write_meta(tx_progress& tx, double interval=5.);
            int32_t read_meta(tx_progress& tx);

            int32_t load_nodeids();
        };
    }
}

#endif
