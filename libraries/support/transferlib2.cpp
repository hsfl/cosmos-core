#include "support/transferlib2.h"

namespace Cosmos {
    namespace Support {
        //! For sorting files by size in ascending order
        bool filestruc_smaller_by_size(const filestruc& a, const filestruc& b)
        {
            return a.size < b.size;
        }

        //! For sorting chunks in ascending order
        bool lower_chunk(file_progress i,file_progress j)
        {
            return (i.chunk_start<j.chunk_start);
        }

        //! Extracts the necessary fields from a received COMMAND packet.
        //! \param pdata An incoming COMMAND-type packet
        //! \param command Reference to a packet_struct_command to fill
        //! \return n/a
        void deserialize_command(const vector<PACKET_BYTE>& pdata, packet_struct_command& command)
        {
            memmove(&command.node_id,  &pdata[0]+PACKET_COMMAND_OFFSET_NODE_ID, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&command.length,   &pdata[0]+PACKET_COMMAND_OFFSET_LENGTH,  1);
            memmove(&command.bytes[0], &pdata[0]+PACKET_COMMAND_OFFSET_BYTES,   command.length);
        }

        //! Extracts the necessary fields from a received MESSAGE packet.
        //! \param pdata An incoming MESSAGE-type packet
        //! \param message Reference to a packet_struct_message to fill
        //! \return n/a
        void deserialize_message(const vector<PACKET_BYTE>& pdata, packet_struct_message& message)
        {
            memmove(&message.node_id,  &pdata[0]+PACKET_MESSAGE_OFFSET_NODE_ID, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&message.length,   &pdata[0]+PACKET_MESSAGE_OFFSET_LENGTH,  1);
            memmove(&message.bytes[0], &pdata[0]+PACKET_MESSAGE_OFFSET_BYTES,   message.length);
        }

        //! Extracts the necessary fields from a received HEARTBEAT packet.
        //! \param pdata An incoming HEARTBEAT-type packet
        //! \param heartbeat Reference to a packet_struct_heartbeat to fill
        //! \return n/a
        void deserialize_heartbeat(const vector<PACKET_BYTE>& pdata, packet_struct_heartbeat& heartbeat)
        {
            memmove(&heartbeat.node_id,     &pdata[0]+PACKET_HEARTBEAT_OFFSET_NODE_ID,     COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&heartbeat.node_name,   &pdata[0]+PACKET_HEARTBEAT_OFFSET_NODE_NAME,   COSMOS_MAX_NAME);
            memmove(&heartbeat.beat_period, &pdata[0]+PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD, 1);
            memmove(&heartbeat.throughput,  &pdata[0]+PACKET_HEARTBEAT_OFFSET_THROUGHPUT,  4);
            memmove(&heartbeat.funixtime,   &pdata[0]+PACKET_HEARTBEAT_OFFSET_FUNIXTIME,   4);
        }

        //! Create a REQQUEUE-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the sending node in the node table
        //! \param node_name Name of the sending node
        //! \return n/a
        void serialize_reqqueue(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, string node_name)
        {
            packet.header.type = PacketComm::TypeId::FileReqQueue;
            packet.data.resize(PACKET_REQQUEUE_OFFSET_TOTAL);
            memset(&packet.data[0], 0, PACKET_REQQUEUE_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_REQQUEUE_OFFSET_NODE_ID, &node_id, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_REQQUEUE_OFFSET_NODE_NAME, node_name.c_str(), node_name.size());
        }

        //! Extracts the necessary fields from a received REQQUEUE packet.
        //! \param pdata An incoming REQQUEUE-type packet
        //! \param reqqueue Reference to a packet_struct_reqqueue to fill
        //! \return n/a
        void deserialize_reqqueue(const vector<PACKET_BYTE>& pdata, packet_struct_reqqueue& reqqueue)
        {
            memmove(&reqqueue.node_id,   &pdata[0]+PACKET_REQQUEUE_OFFSET_NODE_ID,   COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&reqqueue.node_name, &pdata[0]+PACKET_REQQUEUE_OFFSET_NODE_NAME, COSMOS_MAX_NAME);
        }

        //! Create a QUEUE-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the sending node in the node table
        //! \param node_name Name of the sending node
        //! \param queue A vector with QUEUE data
        //! \return n/a
        void serialize_queue(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, string node_name, const vector<PACKET_TX_ID_TYPE>& queue)
        {
            // TODO: remove magic numbers for packet type for all of these
            packet.header.type = PacketComm::TypeId::FileQueue;
            packet.data.resize(PACKET_QUEUE_OFFSET_TOTAL);
            memset(&packet.data[0], 0, PACKET_QUEUE_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_QUEUE_OFFSET_NODE_ID,   &node_id,          COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_QUEUE_OFFSET_NODE_NAME, node_name.c_str(), node_name.size());
            memmove(&packet.data[0]+PACKET_QUEUE_OFFSET_TX_ID,     &queue[0],         COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
        }

        //! Extracts the necessary fields from a received QUEUE packet.
        //! \param pdata An incoming QUEUE-type packet
        //! \param queue Reference to a packet_struct_queue to fill
        //! \return n/a
        void deserialize_queue(const vector<PACKET_BYTE>& pdata, packet_struct_queue& queue)
        {
            memmove(&queue.node_id,   &pdata[0]+PACKET_QUEUE_OFFSET_NODE_ID,   COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&queue.node_name, &pdata[0]+PACKET_QUEUE_OFFSET_NODE_NAME, COSMOS_MAX_NAME);
            memmove(&queue.tx_id,     &pdata[0]+PACKET_QUEUE_OFFSET_TX_ID,     COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
        }

        //! Create a CANCEL-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the sending node in the node table
        //! \param tx_id ID of the transaction
        //! \return n/a
        void serialize_cancel(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id)
        {
            packet.header.type = PacketComm::TypeId::FileCancel;
            packet.data.resize(PACKET_CANCEL_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_CANCEL_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_CANCEL_OFFSET_TX_ID,   &tx_id,   sizeof(PACKET_TX_ID_TYPE));
        }

        //! Extracts the necessary fields from a received CANCEL packet.
        //! \param pdata An incoming CANCEL-type packet
        //! \param cancel Reference to a packet_struct_cancel to fill
        //! \return n/a
        void deserialize_cancel(const vector<PACKET_BYTE>& pdata, packet_struct_cancel &cancel)
        {
            memmove(&cancel.node_id, &pdata[0]+PACKET_CANCEL_OFFSET_NODE_ID, sizeof(PACKET_NODE_ID_TYPE));
            memmove(&cancel.tx_id,   &pdata[0]+PACKET_CANCEL_OFFSET_TX_ID,   sizeof(PACKET_TX_ID_TYPE));
        }

        //! Create a CCOMPLETE-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the sending node in the node table
        //! \param tx_id ID of the transaction
        //! \return n/a
        void serialize_complete(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id)
        {
            packet.header.type = PacketComm::TypeId::FileComplete;
            packet.data.resize(PACKET_COMPLETE_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_COMPLETE_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_COMPLETE_OFFSET_TX_ID,   &tx_id,   sizeof(PACKET_TX_ID_TYPE));
        }

        //! Extracts the necessary fields from a received COMPLETE packet.
        //! \param pdata An incoming COMPLETE-type packet
        //! \param complete Reference to a packet_struct_complete to fill
        //! \return n/a
        void deserialize_complete(const vector<PACKET_BYTE>& pdata, packet_struct_complete &complete)
        {
            memmove(&complete.node_id, &pdata[0]+PACKET_COMPLETE_OFFSET_NODE_ID, sizeof(PACKET_NODE_ID_TYPE));
            memmove(&complete.tx_id,   &pdata[0]+PACKET_COMPLETE_OFFSET_TX_ID,   sizeof(PACKET_TX_ID_TYPE));
        }

        //! Create a REQMETA-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the sending node in the node table
        //! \param node_name Name of the sending node
        //! \param reqmeta A vector with REQMETA data
        //! \return n/a
        void serialize_reqmeta(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, string node_name, vector<PACKET_TX_ID_TYPE> reqmeta)
        {
            packet.header.type = PacketComm::TypeId::FileReqMeta;
            packet.data.resize(PACKET_REQMETA_OFFSET_TOTAL);
            memset(&packet.data[0], 0, PACKET_REQMETA_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_REQMETA_OFFSET_NODE_ID,   &node_id,          COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_REQMETA_OFFSET_NODE_NAME, node_name.c_str(), node_name.size());
            memmove(&packet.data[0]+PACKET_REQMETA_OFFSET_TX_ID,     &reqmeta[0],       COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
        }

        //! Extracts the necessary fields from a received REQMETA packet.
        //! \param pdata An incoming REQMETA-type packet
        //! \param reqmeta Reference to a packet_struct_reqmeta to fill
        //! \return n/a
        void deserialize_reqmeta(const vector<PACKET_BYTE>& pdata, packet_struct_reqmeta& reqmeta)
        {
            memmove(&reqmeta.node_id,   &pdata[0]+PACKET_REQMETA_OFFSET_NODE_ID,   COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&reqmeta.node_name, &pdata[0]+PACKET_REQMETA_OFFSET_NODE_NAME, COSMOS_MAX_NAME);
            memmove(&reqmeta.tx_id,     &pdata[0]+PACKET_REQMETA_OFFSET_TX_ID,     COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
        }

        //! Create a REQDATA-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the sending node in the node table
        //! \param tx_id ID of the transaction
        //! \param hole_start Index of byte start of data chunk
        //! \param hole_end Index of byte end of data chunk
        //! \return n/a
        void serialize_reqdata(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_SIZE_TYPE hole_start, PACKET_FILE_SIZE_TYPE hole_end)
        {
            packet.header.type = PacketComm::TypeId::FileReqData;
            packet.data.resize(PACKET_REQDATA_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_REQDATA_OFFSET_NODE_ID,    &node_id,    sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_REQDATA_OFFSET_TX_ID,      &tx_id,      sizeof(PACKET_TX_ID_TYPE));
            memmove(&packet.data[0]+PACKET_REQDATA_OFFSET_HOLE_START, &hole_start, sizeof(PACKET_FILE_SIZE_TYPE));
            memmove(&packet.data[0]+PACKET_REQDATA_OFFSET_HOLE_END,   &hole_end,   sizeof(PACKET_FILE_SIZE_TYPE));
        }

        //! Extracts the necessary fields from a received REQDATA packet.
        //! \param pdata An incoming REQDATA-type packet
        //! \param reqdata Reference to a packet_struct_reqdata to fill
        //! \return n/a
        void deserialize_reqdata(const vector<PACKET_BYTE>& pdata, packet_struct_reqdata &reqdata)
        {   
            memmove(&reqdata.node_id,    &pdata[0]+PACKET_REQDATA_OFFSET_NODE_ID,    sizeof(PACKET_NODE_ID_TYPE));
            memmove(&reqdata.tx_id,      &pdata[0]+PACKET_REQDATA_OFFSET_TX_ID,      sizeof(PACKET_TX_ID_TYPE));
            memmove(&reqdata.hole_start, &pdata[0]+PACKET_REQDATA_OFFSET_HOLE_START, sizeof(reqdata.hole_start));
            memmove(&reqdata.hole_end,   &pdata[0]+PACKET_REQDATA_OFFSET_HOLE_END,   sizeof(reqdata.hole_end));
        }

        //! Create a long METADATA-type PacketComm packet.
        //! Includes node_name, omits node_id information.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param tx_id Transaction ID of file transfer
        //! \param file_name Name of the file
        //! \param file_size Size of the file
        //! \param node_name Name of the sending node
        //! \param agent_name Name of the sending agent
        //! \return n/a
        void serialize_metadata(PacketComm& packet, PACKET_TX_ID_TYPE tx_id, char* file_name, PACKET_FILE_SIZE_TYPE file_size, char* node_name, char* agent_name)
        {
            packet.header.type = PacketComm::TypeId::FileMetaData;
            packet.data.resize(PACKET_METALONG_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_TX_ID,      &tx_id,     sizeof(PACKET_TX_ID_TYPE));
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_FILE_NAME,  file_name,  TRANSFER_MAX_FILENAME);
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_FILE_SIZE,  &file_size, sizeof(file_size));
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_NODE_NAME,  node_name,  COSMOS_MAX_NAME);
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_AGENT_NAME, agent_name, COSMOS_MAX_NAME);
        }

        //! Extracts the necessary fields from a received long META packet.
        //! \param pdata An incoming long META-type packet
        //! \param meta Reference to a packet_struct_long to fill
        //! \return n/a
        void deserialize_metadata(const vector<PACKET_BYTE>& pdata, packet_struct_metalong &meta)
        {
            memmove(&meta.tx_id,     &pdata[0]+PACKET_METALONG_OFFSET_TX_ID,      sizeof(PACKET_TX_ID_TYPE));
            memmove(meta.node_name,  &pdata[0]+PACKET_METALONG_OFFSET_NODE_NAME,  COSMOS_MAX_NAME);
            memmove(meta.file_name,  &pdata[0]+PACKET_METALONG_OFFSET_FILE_NAME,  TRANSFER_MAX_FILENAME);
            memmove(&meta.file_size, &pdata[0]+PACKET_METALONG_OFFSET_FILE_SIZE,  sizeof(meta.file_size));
            memmove(meta.agent_name, &pdata[0]+PACKET_METALONG_OFFSET_AGENT_NAME, COSMOS_MAX_NAME);
        }

        //! Create a short METADATA-type PacketComm packet.
        //! Includes node_id, omits node_name information.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id Node's ID in the node table
        //! \param tx_id Transaction ID of file transfer
        //! \param file_name Name of the file
        //! \param file_size Size of the file
        //! \param agent_name Name of the sending agent
        //! \return n/a
        void serialize_metadata(PacketComm& packet, PACKET_NODE_ID_TYPE node_id , PACKET_TX_ID_TYPE tx_id, char* file_name, PACKET_FILE_SIZE_TYPE file_size, char* agent_name)
        {
            packet.header.type = PacketComm::TypeId::FileMetaData;
            packet.data.resize(PACKET_METASHORT_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_NODE_ID,    &node_id,   sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_TX_ID,      &tx_id,     sizeof(PACKET_TX_ID_TYPE));
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_FILE_NAME,  file_name,  TRANSFER_MAX_FILENAME);
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_FILE_SIZE,  &file_size, sizeof(file_size));
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_AGENT_NAME, agent_name, COSMOS_MAX_NAME);
        }

        //! Extracts the necessary fields from a received short META packet.
        //! \param pdata An incoming short META-type packet
        //! \param meta Reference to a packet_struct_metashort to fill
        //! \return n/a
        void deserialize_metadata(const vector<PACKET_BYTE>& pdata, packet_struct_metashort &meta)
        {
            memmove(&meta.tx_id,     &pdata[0]+PACKET_METASHORT_OFFSET_TX_ID,      sizeof(PACKET_TX_ID_TYPE));
            memmove(meta.file_name,  &pdata[0]+PACKET_METASHORT_OFFSET_FILE_NAME,  TRANSFER_MAX_FILENAME);
            memmove(&meta.file_size, &pdata[0]+PACKET_METASHORT_OFFSET_FILE_SIZE,  sizeof(meta.file_size));
            memmove(&meta.node_id,   &pdata[0]+PACKET_METASHORT_OFFSET_NODE_ID,    COSMOS_MAX_NAME);
            memmove(meta.agent_name, &pdata[0]+PACKET_METASHORT_OFFSET_AGENT_NAME, COSMOS_MAX_NAME);
        }

        //! Create a DATA-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id Node's ID in the node table
        //! \param byte_count Number of bytes of data data
        //! \param chunk_start Byte of index of start of this chunk in the file
        //! \param chunk Array of bytes defining this chunk
        //! \return n/a
        void serialize_data(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_CHUNK_SIZE_TYPE byte_count, PACKET_FILE_SIZE_TYPE chunk_start, PACKET_BYTE* chunk)
        {
            packet.header.type = PacketComm::TypeId::FileChunkData;
            packet.data.resize(PACKET_DATA_OFFSET_HEADER_TOTAL+byte_count);
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_NODE_ID,     &node_id,     sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_TX_ID,       &tx_id,       sizeof(PACKET_TX_ID_TYPE));
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_BYTE_COUNT,  &byte_count,  sizeof(PACKET_CHUNK_SIZE_TYPE));
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_CHUNK_START, &chunk_start, sizeof(chunk_start));
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_CHUNK,       chunk,        byte_count);
        }

        //! Extracts the necessary fields from a received DATA packet.
        //! \param pdata An incoming DATA-type packet
        //! \param data Reference to a packet_struct_data to fill
        //! \return n/a
        void deserialize_data(const vector<PACKET_BYTE>& pdata, packet_struct_data &data)
        {
            memmove(&data.node_id,     &pdata[0]+PACKET_DATA_OFFSET_NODE_ID,     sizeof(PACKET_NODE_ID_TYPE));
            memmove(&data.tx_id,       &pdata[0]+PACKET_DATA_OFFSET_TX_ID,       sizeof(PACKET_TX_ID_TYPE));
            memmove(&data.byte_count,  &pdata[0]+PACKET_DATA_OFFSET_BYTE_COUNT,  sizeof(data.byte_count));
            memmove(&data.chunk_start, &pdata[0]+PACKET_DATA_OFFSET_CHUNK_START, sizeof(data.chunk_start));
            memmove(data.chunk,        &pdata[0]+PACKET_DATA_OFFSET_CHUNK,       data.byte_count);
        }

        //! Merges any overlapping chunks in the tx.file_info deque.
        //! \param tx A tx_progress to merge chunks for
        //! \return Sum of bytes of the chunks in tx
        PACKET_FILE_SIZE_TYPE merge_chunks_overlap(tx_progress& tx)
        {
            // Remove any chunks that go beyond the file size
            for (uint16_t i=tx.file_info.size()-1; i<tx.file_info.size(); --i)
            {
                if (tx.file_info[i].chunk_end >= tx.file_size)
                {
                    tx.file_info.pop_back();
                }
            }
            switch (tx.file_info.size())
            {
            case 0:
                {
                    tx.total_bytes = 0;
                    break;
                }
            case 1:
                {
                    tx.total_bytes = (tx.file_info[0].chunk_end - tx.file_info[0].chunk_start) + 1;
                    break;
                }
            default:
                {
                    tx.total_bytes = 0;
                    sort(tx.file_info.begin(), tx.file_info.end(), lower_chunk);
                    // Merge chunks
                    for (uint32_t i=0; i<tx.file_info.size(); ++i)
                    {
                        for (uint32_t j=i+1; j<tx.file_info.size(); ++j)
                        {
                            while (j < tx.file_info.size() && tx.file_info[j].chunk_start <= tx.file_info[i].chunk_end+1)
                            {
                                if (tx.file_info[j].chunk_end > tx.file_info[i].chunk_end)
                                {
                                    tx.file_info[i].chunk_end = tx.file_info[j].chunk_end;
                                }
                                tx.file_info.erase(tx.file_info.begin()+j);
                            }
                        }
                        tx.total_bytes += (tx.file_info[i].chunk_end - tx.file_info[i].chunk_start) + 1;
                    }
                    break;
                }
            }
            return tx.total_bytes;
        }

        //! Determines which chunks are missing for an incoming file.
        //! Used by the receiving side to request missing data with REQDATA packets.
        //! \param tx_in The tx_progress of a file in the incoming queue
        //! \return A vector of missing chunks
        vector<file_progress> find_chunks_missing(tx_progress& tx_in)
        {
            vector<file_progress> missing;
            file_progress tp;

            if (!tx_in.havemeta)
            {
                return missing;
            }

            if (tx_in.file_info.size() == 0)
            {
                tp.chunk_start = 0;
                tp.chunk_end = tx_in.file_size - 1;
                missing.push_back(tp);
            }
            else
            {
                merge_chunks_overlap(tx_in);
                sort(tx_in.file_info.begin(), tx_in.file_info.end(), lower_chunk);

                // Check missing before first chunk
                if (tx_in.file_info[0].chunk_start)
                {
                    tp.chunk_start = 0;
                    tp.chunk_end = tx_in.file_info[0].chunk_start - 1;
                    missing.push_back(tp);
                }

                // Check missing between chunks
                for (uint32_t i=1; i<tx_in.file_info.size(); ++i)
                {
                    if (tx_in.file_info[i-1].chunk_end+1 != tx_in.file_info[i].chunk_start)
                    {
                        tp.chunk_start = tx_in.file_info[i-1].chunk_end + 1;
                        tp.chunk_end = tx_in.file_info[i].chunk_start - 1;
                        missing.push_back(tp);
                    }
                }

                // Check missing after last chunk
                if (tx_in.file_info[tx_in.file_info.size()-1].chunk_end + 1 != tx_in.file_size)
                {
                    tp.chunk_start = tx_in.file_info[tx_in.file_info.size()-1].chunk_end + 1;
                    tp.chunk_end = tx_in.file_size - 1;
                    missing.push_back(tp);
                }
            }

            // calculate bytes so far
            tx_in.total_bytes = 0;
            for (file_progress prog : tx_in.file_info)
            {
                tx_in.total_bytes += (prog.chunk_end - prog.chunk_start) + 1;
            }
            if (tx_in.total_bytes == tx_in.file_size)
            {
                tx_in.complete = true;
            }

            return (missing);
        }

        //! Gets the size of a file.
        /*! Looks up the size of the file on the filesystem. This returns a 32 bit signed
        * integer so that it works for most files we want to transfer. If the file is larger
        * than 2^32/2, then it will turn negative and be treated as an error.
        * \param filename Full path to file
        * \return Size, or negative error.
        */
        int32_t get_file_size(string filename)
        {
            int32_t iretn;
            struct stat stat_buf;

            if ((stat(filename.c_str(), &stat_buf)) == 0)
            {
                iretn = stat_buf.st_size;
                return  iretn;
            }
            else
            {
                return -errno;
            }
        }

        int32_t get_file_size(const char* filename)
        {
            string sfilename = filename;
            return get_file_size(sfilename);
        }
    }
}