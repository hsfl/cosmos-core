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
#include "support/transferlib.h"

#define PROGRESS_QUEUE_SIZE 256

namespace Cosmos {
    namespace Support {
        class Transfer {
        public:
            struct chunk
            {
                uint32_t start;
                uint32_t end;
            };

            Transfer();
            // int32_t Init(string node, string agent, uint16_t chunk_size);
            int32_t Init(Agent *calling_agent);
            //int32_t Load(string filename, vector<chunk> &chunks);
            int32_t Add(size_t chunkidx, vector<uint8_t> chunk);
            int32_t outgoing_tx_load();

            static const uint8_t PACKET_METADATA = 0xf;
            static const uint8_t PACKET_DATA = 0xe;
            static const uint8_t PACKET_REQDATA =	0xd;
            static const uint8_t PACKET_REQMETA =	0xc;
            static const uint8_t PACKET_COMPLETE = 0xb;
            static const uint8_t PACKET_CANCEL = 0xa;
            static const uint8_t PACKET_QUEUE = 0x9;
            static const uint8_t PACKET_REQQUEUE = 0x8;
            static const uint8_t PACKET_HEARTBEAT = 0x7;
            static const uint8_t PACKET_MESSAGE = 0x6;
            static const uint8_t PACKET_COMMAND = 0x5;
            static const uint8_t PACKET_INFO = 0x4;

            struct PacketInfo
            {
                uint8_t type = PACKET_INFO;
                uint32_t date = 0;
                uint32_t node_hash = 0;
                uint32_t agent_hash = 0;
                uint16_t node_size = 0;
                string node = "";
            };

            struct PacketMetaData
            {
                uint8_t type = PACKET_METADATA;
                uint32_t date = 0;
                uint32_t node_hash = 0;
                uint32_t agent_hash = 0;
                uint16_t tx_id = UINT16_MAX;
            };

            struct PacketData
            {
                uint8_t type = PACKET_DATA;
                uint32_t date = 0;
                uint32_t node_hash = 0;
                uint32_t agent_hash = 0;
                uint16_t tx_id = UINT16_MAX;
                vector<uint8_t> chunk;
            };

            // vector<uint8_t> meta;
            // vector<vector<uint8_t>> data;
            // uint32_t txid;
            // string name;
            // size_t size;
            // string node;
            // string agent;
            // string json;
            // size_t throughput;
            // size_t chunk_size;
        private:
            struct tx_entry
            {
                bool activity = false;
                bool sendqueue = false;
                bool sentqueue = false;
                bool rcvdqueue = false;
                bool rcvdmeta = false;
                bool rcvddata = false;
                PACKET_TX_ID_TYPE size;
                PACKET_TX_ID_TYPE next_id;
                string node_name="";
                tx_progress progress[PROGRESS_QUEUE_SIZE];
                double heartbeatclock = 0.;
                double reqmetaclock = 0.;
                double reqdataclock = 0.;
            };

            //! Holds the incoming and outgoing queues for a single node
            struct tx_queue
            {
                string node_name="";
                PACKET_NODE_ID_TYPE node_id;
                tx_entry incoming;
                tx_entry outgoing;
            };

            //! The calling agent of this class
            Agent *agent;

            //! The heart of the file transfer manager
            vector<tx_queue> txq;

            // Timers for debug messages
            ElapsedTime dt, tet;

            // Private queue manipulation functions
            int32_t outgoing_tx_add(tx_progress &tx_out);
            int32_t outgoing_tx_add(string node_name, string agent_name, string file_name);
            int32_t outgoing_tx_recount(uint8_t node_id);
        };
    }
}

#endif
