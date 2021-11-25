#ifndef TRANSFERLIB2_H
#define TRANSFERLIB2_H

// cosmos includes
#include "support/configCosmos.h"
#include "support/datalib.h"
#include "support/packetcomm.h"

#define PROGRESS_QUEUE_SIZE 256

#define PACKET_MAX_LENGTH 1500
// Radios are only allowing 253 byte packet. IP/UDP header is 28 bytes.
#define TRANSFER_MAX_PROTOCOL_PACKET 225
#define TRANSFER_MAX_FILENAME 128
#ifndef COSMOS_WIN_BUILD_MSVC
#define TRANSFER_QUEUE_LIMIT ((TRANSFER_MAX_PROTOCOL_PACKET-(COSMOS_SIZEOF(PACKET_TYPE)+COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)+COSMOS_SIZEOF(PACKET_TX_ID_TYPE)+COSMOS_MAX_NAME))/(COSMOS_SIZEOF(PACKET_TX_ID_TYPE)))
#else
#define TRANSFER_QUEUE_LIMIT 100
#endif

namespace Cosmos {
    namespace Support {
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

        typedef uint8_t PACKET_BYTE;
        typedef uint8_t PACKET_TYPE;
        typedef uint16_t PACKET_CRC;
        typedef uint8_t PACKET_NODE_ID_TYPE;
        typedef uint8_t PACKET_TX_ID_TYPE;
        typedef uint16_t PACKET_CHUNK_SIZE_TYPE;
        typedef int32_t PACKET_FILE_SIZE_TYPE;
        typedef uint32_t PACKET_UNIXTIME_TYPE;

        struct packet_struct_heartbeat
        {
            PACKET_NODE_ID_TYPE node_id;
            char node_name[COSMOS_MAX_NAME+1];
            PACKET_BYTE beat_period;
            PACKET_UNIXTIME_TYPE throughput;
            PACKET_UNIXTIME_TYPE funixtime;
        };

        #define PACKET_HEARTBEAT_OFFSET_NODE_ID 0
        #define PACKET_HEARTBEAT_OFFSET_NODE_NAME (COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
        #define PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD (PACKET_HEARTBEAT_OFFSET_NODE_NAME + COSMOS_MAX_NAME)
        #define PACKET_HEARTBEAT_OFFSET_THROUGHPUT (PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD + 1)
        #define PACKET_HEARTBEAT_OFFSET_FUNIXTIME (PACKET_HEARTBEAT_OFFSET_THROUGHPUT + 4)
        #define PACKET_HEARTBEAT_OFFSET_TOTAL (PACKET_HEARTBEAT_OFFSET_FUNIXTIME + 8)

        struct packet_struct_message
        {
            PACKET_NODE_ID_TYPE node_id;
            PACKET_BYTE length;
            PACKET_BYTE bytes[TRANSFER_MAX_PROTOCOL_PACKET-2];
        };

        #define PACKET_MESSAGE_OFFSET_NODE_ID 0
        #define PACKET_MESSAGE_OFFSET_LENGTH (PACKET_MESSAGE_OFFSET_NODE_ID + 1)
        #define PACKET_MESSAGE_OFFSET_BYTES (PACKET_MESSAGE_OFFSET_LENGTH + 1)
        #define PACKET_MESSAGE_OFFSET_TOTAL (PACKET_MESSAGE_OFFSET_BYTES + TRANSFER_MAX_PROTOCOL_PACKET - 2)

        struct packet_struct_command
        {
            PACKET_NODE_ID_TYPE node_id;
            PACKET_BYTE length;
            PACKET_BYTE bytes[TRANSFER_MAX_PROTOCOL_PACKET-2];
        };

        #define PACKET_COMMAND_OFFSET_NODE_ID 0
        #define PACKET_COMMAND_OFFSET_LENGTH (PACKET_COMMAND_OFFSET_NODE_ID + 1)
        #define PACKET_COMMAND_OFFSET_BYTES (PACKET_COMMAND_OFFSET_LENGTH + 1)
        #define PACKET_COMMAND_OFFSET_TOTAL (PACKET_COMMAND_OFFSET_BYTES + TRANSFER_MAX_PROTOCOL_PACKET - 2)

        struct packet_struct_reqqueue
        {
            PACKET_NODE_ID_TYPE node_id;
            char node_name[COSMOS_MAX_NAME+1];
        };

        #define PACKET_REQQUEUE_OFFSET_NODE_ID 0
        #define PACKET_REQQUEUE_OFFSET_NODE_NAME (COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
        #define PACKET_REQQUEUE_OFFSET_TOTAL (PACKET_REQQUEUE_OFFSET_NODE_ID + COSMOS_MAX_NAME)

        struct packet_struct_queue
        {
            PACKET_NODE_ID_TYPE node_id;
            char node_name[COSMOS_MAX_NAME+1];
            PACKET_TX_ID_TYPE tx_id[TRANSFER_QUEUE_LIMIT];
        };

        #define PACKET_QUEUE_OFFSET_NODE_ID 0
        #define PACKET_QUEUE_OFFSET_NODE_NAME (COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
        #define PACKET_QUEUE_OFFSET_TX_ID (PACKET_QUEUE_OFFSET_NODE_NAME + COSMOS_MAX_NAME)
        #define PACKET_QUEUE_OFFSET_TOTAL (PACKET_QUEUE_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE) * TRANSFER_QUEUE_LIMIT)

        struct packet_struct_reqmeta
        {
            PACKET_NODE_ID_TYPE node_id;
            char node_name[COSMOS_MAX_NAME+1];
            PACKET_TX_ID_TYPE tx_id[TRANSFER_QUEUE_LIMIT];
        };

        #define PACKET_REQMETA_OFFSET_NODE_ID 0
        #define PACKET_REQMETA_OFFSET_NODE_NAME (COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
        #define PACKET_REQMETA_OFFSET_TX_ID (PACKET_REQMETA_OFFSET_NODE_NAME + COSMOS_MAX_NAME)
        #define PACKET_REQMETA_OFFSET_TOTAL (PACKET_REQMETA_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE) * TRANSFER_QUEUE_LIMIT)

        struct packet_struct_metalong
        {
            char node_name[COSMOS_MAX_NAME+1];
            PACKET_TX_ID_TYPE tx_id;
            char agent_name[COSMOS_MAX_NAME+1];
            char file_name[TRANSFER_MAX_FILENAME];
            PACKET_FILE_SIZE_TYPE file_size;
        };

        #define PACKET_METALONG_OFFSET_NODE_NAME 0
        #define PACKET_METALONG_OFFSET_TX_ID (PACKET_METALONG_OFFSET_NODE_NAME + COSMOS_MAX_NAME)
        #define PACKET_METALONG_OFFSET_AGENT_NAME (PACKET_METALONG_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))
        #define PACKET_METALONG_OFFSET_FILE_NAME (PACKET_METALONG_OFFSET_AGENT_NAME + COSMOS_MAX_NAME)
        #define PACKET_METALONG_OFFSET_FILE_SIZE (PACKET_METALONG_OFFSET_FILE_NAME + TRANSFER_MAX_FILENAME)
        #define PACKET_METALONG_OFFSET_TOTAL (PACKET_METALONG_OFFSET_FILE_SIZE + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))

        struct packet_struct_metashort
        {
            PACKET_NODE_ID_TYPE node_id;
            PACKET_TX_ID_TYPE tx_id;
            char agent_name[COSMOS_MAX_NAME+1];
            char file_name[TRANSFER_MAX_FILENAME];
            PACKET_FILE_SIZE_TYPE file_size;
        };

        #define PACKET_METASHORT_OFFSET_NODE_ID 0
        #define PACKET_METASHORT_OFFSET_TX_ID (PACKET_METASHORT_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
        #define PACKET_METASHORT_OFFSET_AGENT_NAME (PACKET_METASHORT_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))
        #define PACKET_METASHORT_OFFSET_FILE_NAME (PACKET_METASHORT_OFFSET_AGENT_NAME + COSMOS_MAX_NAME)
        #define PACKET_METASHORT_OFFSET_FILE_SIZE (PACKET_METASHORT_OFFSET_FILE_NAME + TRANSFER_MAX_FILENAME)
        #define PACKET_METASHORT_OFFSET_TOTAL (PACKET_METASHORT_OFFSET_FILE_SIZE + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))

        struct packet_struct_reqdata
        {
            PACKET_NODE_ID_TYPE node_id;
            PACKET_TX_ID_TYPE tx_id;
            PACKET_FILE_SIZE_TYPE hole_start;
            PACKET_FILE_SIZE_TYPE hole_end;
        };

        #define PACKET_REQDATA_OFFSET_NODE_ID 0
        #define PACKET_REQDATA_OFFSET_TX_ID (PACKET_REQDATA_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
        #define PACKET_REQDATA_OFFSET_HOLE_START (PACKET_REQDATA_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))
        #define PACKET_REQDATA_OFFSET_HOLE_END (PACKET_REQDATA_OFFSET_HOLE_START + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))
        #define PACKET_REQDATA_OFFSET_TOTAL (PACKET_REQDATA_OFFSET_HOLE_END + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))

        struct packet_struct_data
        {
            PACKET_NODE_ID_TYPE node_id;
            PACKET_TX_ID_TYPE tx_id;
            PACKET_CHUNK_SIZE_TYPE byte_count;
            PACKET_FILE_SIZE_TYPE chunk_start;
            PACKET_BYTE chunk[PACKET_MAX_LENGTH];
        };

        #define PACKET_DATA_OFFSET_NODE_ID 0
        #define PACKET_DATA_OFFSET_TX_ID (PACKET_DATA_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
        #define PACKET_DATA_OFFSET_BYTE_COUNT (PACKET_DATA_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))
        #define PACKET_DATA_OFFSET_CHUNK_START (PACKET_DATA_OFFSET_BYTE_COUNT + COSMOS_SIZEOF(PACKET_CHUNK_SIZE_TYPE))
        #define PACKET_DATA_OFFSET_CHUNK (PACKET_DATA_OFFSET_CHUNK_START + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))
        #define PACKET_DATA_OFFSET_HEADER_TOTAL (PACKET_DATA_OFFSET_CHUNK)

        struct packet_struct_complete
        {
            PACKET_NODE_ID_TYPE node_id;
            PACKET_TX_ID_TYPE tx_id;
        };

        #define PACKET_COMPLETE_OFFSET_NODE_ID 0
        #define PACKET_COMPLETE_OFFSET_TX_ID (PACKET_COMPLETE_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
        #define PACKET_COMPLETE_OFFSET_TOTAL (PACKET_COMPLETE_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))

        struct packet_struct_cancel
        {
            PACKET_NODE_ID_TYPE node_id;
            PACKET_TX_ID_TYPE tx_id;
        };

        #define PACKET_CANCEL_OFFSET_NODE_ID 0
        #define PACKET_CANCEL_OFFSET_TX_ID (PACKET_CANCEL_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
        #define PACKET_CANCEL_OFFSET_TOTAL (PACKET_CANCEL_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))

        struct packet_struct_raw
        {
            PACKET_NODE_ID_TYPE node_id;
            PACKET_TX_ID_TYPE tx_id;
        };

        /// Chunk start and end.
        struct file_progress
        {
            PACKET_FILE_SIZE_TYPE	chunk_start;
            PACKET_FILE_SIZE_TYPE	chunk_end;
        };

        /// Holds info about the transfer progress of a single file.
        struct tx_progress
        {
            PACKET_TX_ID_TYPE tx_id=0;
            bool enabled=false;
            bool havemeta=false;
            bool havedata=false;
            bool sendmeta=false;
            bool sentmeta=false;
            bool senddata=false;
            bool sentdata=false;
            bool complete=false;
            string node_name="";
            string agent_name="";
            string	file_name="";
            string filepath="";
            string temppath="";
            double savetime;
            double datatime=0.;
            PACKET_FILE_SIZE_TYPE file_size=0;
            PACKET_FILE_SIZE_TYPE total_bytes=0;
            deque<file_progress> file_info;
            FILE * fp;
        };

        /// Holds info about the queue of file transfers in progress.
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

        /// Holds the incoming and outgoing queues for a single node.
        struct tx_queue
        {
            string node_name="";
            PACKET_NODE_ID_TYPE node_id;
            tx_entry incoming;
            tx_entry outgoing;
        };

        //Function which gets the size of a file
        int32_t get_file_size(string filename);
        int32_t get_file_size(const char* filename);

        // Converts packet types to and from byte arrays
        void serialize_queue(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, string node_name, const vector<PACKET_TX_ID_TYPE>& queue);
        void deserialize_queue(const PacketComm& packet, packet_struct_queue& queue);
        void serialize_cancel(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id);
        void serialize_complete(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id);
        void serialize_reqmeta(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, string node_name, vector<PACKET_TX_ID_TYPE> reqmeta);
        void serialize_reqdata(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_SIZE_TYPE hole_start, PACKET_FILE_SIZE_TYPE hole_end);
        void serialize_metadata(PacketComm& packet, PACKET_TX_ID_TYPE tx_id, char* file_name, PACKET_FILE_SIZE_TYPE file_size, char* node_name, char* agent_name);
        void serialize_metadata(PacketComm& packet, PACKET_NODE_ID_TYPE node_id , PACKET_TX_ID_TYPE tx_id, char* file_name, PACKET_FILE_SIZE_TYPE file_size, char* agent_name);
        void serialize_data(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_CHUNK_SIZE_TYPE byte_count, PACKET_FILE_SIZE_TYPE chunk_start, PACKET_BYTE* chunk);

        // Accumulate and manage chunks
        PACKET_FILE_SIZE_TYPE merge_chunks_overlap(tx_progress& tx);
        vector<file_progress> find_chunks_missing(tx_progress& tx);

        // Random utility functions
        bool filestruc_smaller_by_size(const filestruc& a, const filestruc& b);
        bool lower_chunk(file_progress i,file_progress j);
    }
}

#endif