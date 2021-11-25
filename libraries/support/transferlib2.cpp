#include "support/transferlib2.h"

namespace Cosmos {
    namespace Support {
        bool filestruc_smaller_by_size(const filestruc& a, const filestruc& b)
        {
            return a.size < b.size;
        }

        bool lower_chunk(file_progress i,file_progress j)
        {
            return (i.chunk_start<j.chunk_start);
        }

        //! Create a QUEUE-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_name Name of the sending node
        //! \param queue A vector with QUEUE data
        //! \return n/a
        void serialize_queue(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, string node_name, const vector<PACKET_TX_ID_TYPE>& queue)
        {
            // TODO: remove magic numbers for packet type for all of these
            packet.type = 79;
            packet.data.resize(PACKET_QUEUE_OFFSET_TOTAL);
            memset(&packet.data[0], 0, PACKET_QUEUE_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_QUEUE_OFFSET_NODE_ID, &node_id, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_QUEUE_OFFSET_NODE_NAME, node_name.c_str(), node_name.size());
            memmove(&packet.data[0]+PACKET_QUEUE_OFFSET_TX_ID, &queue[0], COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
        }

        //! Extracts the necessary fields from a received QUEUE packet.
        //! \param packet An incoming QUEUE-type PacketComm packet
        //! \param queue Reference to a packet_struct_queue to fill
        //! \return n/a
        void deserialize_queue(const PacketComm& packet, packet_struct_queue& queue)
        {
            memmove(&queue.node_id, &packet.data[0]+PACKET_QUEUE_OFFSET_NODE_ID, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&queue.node_name, &packet.data[0]+PACKET_QUEUE_OFFSET_NODE_NAME, COSMOS_MAX_NAME);
            memmove(&queue.tx_id, &packet.data[0]+PACKET_QUEUE_OFFSET_TX_ID, COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
        }

        void serialize_cancel(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id)
        {
            packet.type = 80;
            packet.data.resize(PACKET_CANCEL_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_CANCEL_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_CANCEL_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
        }

        void serialize_complete(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id)
        {
            packet.type = 81;
            packet.data.resize(PACKET_COMPLETE_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_COMPLETE_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_COMPLETE_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
        }

        void serialize_reqmeta(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, string node_name, vector<PACKET_TX_ID_TYPE> reqmeta)
        {
            packet.type = 82;
            packet.data.resize(PACKET_REQMETA_OFFSET_TOTAL);
            memset(&packet.data[0], 0, PACKET_REQMETA_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_REQMETA_OFFSET_NODE_ID, &node_id, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_REQMETA_OFFSET_NODE_NAME, node_name.c_str(), node_name.size());
            memmove(&packet.data[0]+PACKET_REQMETA_OFFSET_TX_ID, &reqmeta[0], COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
        }

        void serialize_reqdata(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_SIZE_TYPE hole_start, PACKET_FILE_SIZE_TYPE hole_end)
        {
            packet.type = 83;
            packet.data.resize(PACKET_REQDATA_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_REQDATA_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_REQDATA_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
            memmove(&packet.data[0]+PACKET_REQDATA_OFFSET_HOLE_START, &hole_start, sizeof(PACKET_FILE_SIZE_TYPE));
            memmove(&packet.data[0]+PACKET_REQDATA_OFFSET_HOLE_END, &hole_end, sizeof(PACKET_FILE_SIZE_TYPE));
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
            packet.type = 84;
            packet.data.resize(PACKET_METALONG_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_FILE_NAME, file_name, TRANSFER_MAX_FILENAME);
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_FILE_SIZE, &file_size, sizeof(file_size));
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_NODE_NAME, node_name, COSMOS_MAX_NAME);
            memmove(&packet.data[0]+PACKET_METALONG_OFFSET_AGENT_NAME, agent_name, COSMOS_MAX_NAME);
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
            packet.type = 84;
            packet.data.resize(PACKET_METASHORT_OFFSET_TOTAL);
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_FILE_NAME, file_name, TRANSFER_MAX_FILENAME);
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_FILE_SIZE, &file_size, sizeof(file_size));
            memmove(&packet.data[0]+PACKET_METASHORT_OFFSET_AGENT_NAME, agent_name, COSMOS_MAX_NAME);
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
            packet.type = 85;
            packet.data.resize(PACKET_DATA_OFFSET_HEADER_TOTAL+byte_count);
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_BYTE_COUNT, &byte_count, sizeof(PACKET_CHUNK_SIZE_TYPE));
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_CHUNK_START, &chunk_start, sizeof(chunk_start));
            memmove(&packet.data[0]+PACKET_DATA_OFFSET_CHUNK, chunk, byte_count);
        }

        PACKET_FILE_SIZE_TYPE merge_chunks_overlap(tx_progress& tx)
        {
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

        vector<file_progress> find_chunks_missing(tx_progress& tx)
        {
            vector<file_progress> missing;
            file_progress tp;

            if (!tx.havemeta)
            {
                return missing;
            }

            if (tx.file_info.size() == 0)
            {
                tp.chunk_start = 0;
                tp.chunk_end = tx.file_size - 1;
                missing.push_back(tp);
            }
            else
            {
                merge_chunks_overlap(tx);
                sort(tx.file_info.begin(), tx.file_info.end(), lower_chunk);

                // Check missing before first chunk
                if (tx.file_info[0].chunk_start)
                {
                    tp.chunk_start = 0;
                    tp.chunk_end = tx.file_info[0].chunk_start - 1;
                    missing.push_back(tp);
                }

                // Check missing between chunks
                for (uint32_t i=1; i<tx.file_info.size(); ++i)
                {
                    if (tx.file_info[i-1].chunk_end+1 != tx.file_info[i].chunk_start)
                    {
                        tp.chunk_start = tx.file_info[i-1].chunk_end + 1;
                        tp.chunk_end = tx.file_info[i].chunk_start - 1;
                        missing.push_back(tp);
                    }
                }

                // Check missing after last chunk
                if (tx.file_info[tx.file_info.size()-1].chunk_end + 1 != tx.file_size)
                {
                    tp.chunk_start = tx.file_info[tx.file_info.size()-1].chunk_end + 1;
                    tp.chunk_end = tx.file_size - 1;
                    missing.push_back(tp);
                }
            }

            // calculate bytes so far
            tx.total_bytes = 0;
            for (file_progress prog : tx.file_info)
            {
                tx.total_bytes += (prog.chunk_end - prog.chunk_start) + 1;
            }
            if (tx.total_bytes == tx.file_size)
            {
                tx.complete = true;
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