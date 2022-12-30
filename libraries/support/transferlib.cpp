#include "support/transferlib.h"
#include "math/bytelib.h"

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

        //! Create a QUEUE-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param orig_node_id ID of the origin node in the node table
        //! \param node_name Name of the origin node
        //! \param queue A vector with QUEUE data
        //! \return n/a
        void serialize_queue(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id, string node_name, const vector<PACKET_TX_ID_TYPE>& queue)
        {
            const uint8_t MAXSTRLEN = sizeof(uint8_t);
            packet.header.type = PacketComm::TypeId::DataFileQueue;
            packet.data.resize(
                sizeof(PACKET_NODE_ID_TYPE)             // node_id
                + MAXSTRLEN                             // node_name_len
                + node_name.size()                      // node_name
                + sizeof(packet_struct_queue::tx_ids)); // tx_ids
            size_t offset = 0;
            packet.data[offset] = orig_node_id;
            offset += sizeof(PACKET_NODE_ID_TYPE);
            packet.data[offset] = node_name.size();
            offset += MAXSTRLEN;
            std::copy(node_name.begin(), node_name.end(), &packet.data[offset]);
            offset += node_name.size();

            vector<PACKET_QUEUE_FLAGS_TYPE> row(PACKET_QUEUE_FLAGS_LIMIT, 0);
            for (PACKET_TX_ID_TYPE tx_id : queue)
            {
                // Note: this logic assumes that PACKET_QUEUE_FLAGS_TYPE is a uint8_t
                uint8_t flags = tx_id >> 3;
                uint8_t lshift = tx_id & 7; // 0b111
                PACKET_QUEUE_FLAGS_TYPE mask = 1 << lshift;
                row[flags] |= mask;
            }
            std::copy(row.begin(), row.end(), packet.data.begin()+offset);
        }

        //! Extracts the necessary fields from a received QUEUE packet.
        //! \param pdata An incoming QUEUE-type packet
        //! \param queue Reference to a packet_struct_queue to fill
        //! \return n/a
        void deserialize_queue(const vector<PACKET_BYTE>& pdata, packet_struct_queue& queue)
        {
            const uint8_t MAXSTRLEN = sizeof(uint8_t);
            size_t offset = 0;
            queue.node_id = pdata[offset];
            offset += sizeof(PACKET_NODE_ID_TYPE);
            queue.node_name_len = pdata[offset];
            offset += MAXSTRLEN;
            queue.node_name.resize(queue.node_name_len);
            std::copy_n(pdata.begin()+offset, queue.node_name_len, queue.node_name.begin());
            offset += queue.node_name.size();
            std::copy(pdata.begin()+offset, pdata.end(), queue.tx_ids);
        }

        //! Create a CANCEL-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param orig_node_id ID of the origin node in the node table
        //! \param tx_id ID of the transaction
        //! \return n/a
        void serialize_cancel(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id, PACKET_TX_ID_TYPE tx_id)
        {
            packet.header.type = PacketComm::TypeId::DataFileCancel;
            packet.data.resize(sizeof(packet_struct_cancel));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_cancel, node_id), &orig_node_id,  sizeof(PACKET_NODE_ID_TYPE));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_cancel, tx_id),   &tx_id,         sizeof(PACKET_TX_ID_TYPE));
        }

        //! Extracts the necessary fields from a received CANCEL packet.
        //! \param pdata An incoming CANCEL-type packet
        //! \param cancel Reference to a packet_struct_cancel to fill
        //! \return n/a
        void deserialize_cancel(const vector<PACKET_BYTE>& pdata, packet_struct_cancel &cancel)
        {
            memcpy(&cancel, pdata.data(), sizeof(packet_struct_cancel));
        }

        //! Create a REQCOMPLETE-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param orig_node_id ID of the origin node in the node table
        //! \param tx_id ID of the transaction
        //! \return n/a
        void serialize_reqcomplete(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id, PACKET_TX_ID_TYPE tx_id)
        {
            packet.header.type = PacketComm::TypeId::DataFileReqComplete;
            packet.data.resize(sizeof(packet_struct_reqcomplete));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_reqcomplete, node_id), &orig_node_id,   sizeof(PACKET_NODE_ID_TYPE));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_reqcomplete, tx_id),   &tx_id,          sizeof(PACKET_TX_ID_TYPE));
        }

        //! Extracts the necessary fields from a received REQCOMPLETE packet.
        //! \param pdata An incoming COMPLETE-type packet
        //! \param reqcomplete Reference to a packet_struct_reqcomplete to fill
        //! \return n/a
        void deserialize_reqcomplete(const vector<PACKET_BYTE>& pdata, packet_struct_reqcomplete &reqcomplete)
        {
            memcpy(&reqcomplete, pdata.data(), sizeof(packet_struct_reqcomplete));
        }

        //! Create a COMPLETE-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the receiver node in the node table
        //! \param tx_id ID of the transaction
        //! \return n/a
        void serialize_complete(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id)
        {
            packet.header.type = PacketComm::TypeId::DataFileComplete;
            packet.data.resize(sizeof(packet_struct_complete));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_complete, node_id), &node_id, sizeof(PACKET_NODE_ID_TYPE));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_complete, tx_id),   &tx_id,   sizeof(PACKET_TX_ID_TYPE));
        }

        //! Extracts the necessary fields from a received COMPLETE packet.
        //! \param pdata An incoming COMPLETE-type packet
        //! \param complete Reference to a packet_struct_complete to fill
        //! \return n/a
        void deserialize_complete(const vector<PACKET_BYTE>& pdata, packet_struct_complete &complete)
        {
            memcpy(&complete, pdata.data(), sizeof(packet_struct_complete));
        }

        //! Create a REQMETA-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the requester node in the node table
        //! \param node_name Name of the origin node
        //! \param reqmeta A vector with REQMETA data
        //! \return n/a
        void serialize_reqmeta(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, string node_name, const vector<PACKET_TX_ID_TYPE>& reqmeta)
        {
            serialize_queue(packet, node_id, node_name, reqmeta);
            packet.header.type = PacketComm::TypeId::DataFileReqMeta;
        }

        //! Extracts the necessary fields from a received REQMETA packet.
        //! \param pdata An incoming REQMETA-type packet
        //! \param reqmeta Reference to a packet_struct_reqmeta to fill
        //! \return n/a
        void deserialize_reqmeta(const vector<PACKET_BYTE>& pdata, packet_struct_reqmeta& reqmeta)
        {
            deserialize_queue(pdata, reqmeta);
        }

        //! Create a REQDATA-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the requester node in the node table
        //! \param tx_id ID of the transaction
        //! \param hole_start Index of byte start of data chunk
        //! \param hole_end Index of byte end of data chunk
        //! \return n/a
        void serialize_reqdata(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_SIZE_TYPE hole_start, PACKET_FILE_SIZE_TYPE hole_end)
        {
            packet.header.type = PacketComm::TypeId::DataFileReqData;
            packet.data.resize(sizeof(packet_struct_reqdata));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_reqdata, node_id),    &node_id,    sizeof(PACKET_NODE_ID_TYPE));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_reqdata, tx_id),      &tx_id,      sizeof(PACKET_TX_ID_TYPE));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_reqdata, hole_start), &hole_start, sizeof(PACKET_FILE_SIZE_TYPE));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_reqdata, hole_end),   &hole_end,   sizeof(PACKET_FILE_SIZE_TYPE));
        }

        //! Extracts the necessary fields from a received REQDATA packet.
        //! \param pdata An incoming REQDATA-type packet
        //! \param reqdata Reference to a packet_struct_reqdata to fill
        //! \return n/a
        void deserialize_reqdata(const vector<PACKET_BYTE>& pdata, packet_struct_reqdata &reqdata)
        {   
            memcpy(&reqdata, pdata.data(), sizeof(packet_struct_reqdata));
        }

        //! Create a long METADATA-type PacketComm packet.
        //! Includes node_name, omits node_id information.
        //! Used by write_meta()
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param tx_id Transaction ID of file transfer
        //! \param file_name Name of the file
        //! \param file_size Size of the file
        //! \param node_name Name of the receiving node
        //! \param agent_name Name of the receiving agent
        //! \return n/a
        void serialize_metadata(PacketComm& packet, PACKET_TX_ID_TYPE tx_id, const string& file_name, PACKET_FILE_SIZE_TYPE file_size, const string& node_name, const string& agent_name)
        {
            const uint8_t MAXSTRLEN = sizeof(uint8_t);
            packet.header.type = PacketComm::TypeId::DataFileMetaData;
            packet.data.resize(
                MAXSTRLEN                           // node_name_len
                + node_name.size()                  // node_name
                + MAXSTRLEN                         // tx_id
                + MAXSTRLEN                         // agent_name_len
                + agent_name.size()                 // agent_name
                + MAXSTRLEN                         // file_name_len
                + file_name.size()                  // file_name
                + sizeof(PACKET_FILE_SIZE_TYPE));   // file_size
            size_t offset = 0;
            packet.data[offset] = node_name.size();
            offset += MAXSTRLEN;
            std::copy_n(node_name.begin(), node_name.size(), &packet.data[offset]);
            offset += node_name.size();
            packet.data[offset] = tx_id;
            offset += sizeof(PACKET_TX_ID_TYPE);
            packet.data[offset] = agent_name.size();
            offset += MAXSTRLEN;
            std::copy_n(agent_name.begin(), agent_name.size(), &packet.data[offset]);
            offset += agent_name.size();
            packet.data[offset] = file_name.size();
            offset += MAXSTRLEN;
            std::copy_n(file_name.begin(), file_name.size(), &packet.data[offset]);
            offset += file_name.size();
            uint32to(file_size, &packet.data[offset], ByteOrder::LITTLEENDIAN);
        }

        //! Extracts the necessary fields from a received long META packet.
        //! \param pdata An incoming long META-type packet
        //! \param meta Reference to a packet_struct_long to fill
        //! \return n/a
        void deserialize_metadata(const vector<PACKET_BYTE>& pdata, packet_struct_metalong &meta)
        {
            const uint8_t MAXSTRLEN = sizeof(uint8_t);
            size_t offset = 0;
            meta.node_name_len = pdata[offset];
            offset += MAXSTRLEN;
            meta.node_name.resize(meta.node_name_len);
            std::copy_n(pdata.begin()+offset, meta.node_name_len, meta.node_name.begin());
            offset += meta.node_name.size();
            meta.tx_id = pdata[offset];
            offset += sizeof(PACKET_TX_ID_TYPE);
            meta.agent_name_len = pdata[offset];
            offset += MAXSTRLEN;
            meta.agent_name.resize(meta.agent_name_len);
            std::copy_n(pdata.begin()+offset, meta.agent_name_len, meta.agent_name.begin());
            offset += meta.agent_name.size();
            meta.file_name_len = pdata[offset];
            offset += MAXSTRLEN;
            meta.file_name.resize(meta.file_name_len);
            std::copy_n(pdata.begin()+offset, meta.file_name_len, meta.file_name.begin());
            offset += meta.file_name.size();
            meta.file_size = uint32from(&pdata[offset], ByteOrder::LITTLEENDIAN);
        }

        //! Create a short METADATA-type PacketComm packet.
        //! Includes node_id, omits node_name information.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param orig_node_id Node's ID in the node table
        //! \param tx_id Transaction ID of file transfer
        //! \param file_name Name of the file
        //! \param file_size Size of the file
        //! \param agent_name Name of the sending agent
        //! \return n/a
        void serialize_metadata(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id , PACKET_TX_ID_TYPE tx_id, const string& file_name, PACKET_FILE_SIZE_TYPE file_size, const string& agent_name)
        {
            const uint8_t MAXSTRLEN = sizeof(uint8_t);
            packet.header.type = PacketComm::TypeId::DataFileMetaData;
            packet.data.resize(
                sizeof(PACKET_NODE_ID_TYPE)         // orig_node_id
                + sizeof(PACKET_TX_ID_TYPE)         // tx_id
                + MAXSTRLEN                         // agent_name_len
                + agent_name.size()                 // agent_name
                + MAXSTRLEN                         // file_name_len
                + file_name.size()                  // file_name
                + sizeof(PACKET_FILE_SIZE_TYPE));   // file_size
            size_t offset = 0;
            packet.data[offset] = orig_node_id;
            offset += sizeof(PACKET_NODE_ID_TYPE);
            packet.data[offset] = tx_id;
            offset += sizeof(PACKET_TX_ID_TYPE);
            packet.data[offset] = agent_name.size();
            offset += MAXSTRLEN;
            std::copy_n(agent_name.begin(), agent_name.size(), &packet.data[offset]);
            offset += agent_name.size();
            packet.data[offset] = file_name.size();
            offset += MAXSTRLEN;
            std::copy_n(file_name.begin(), file_name.size(), &packet.data[offset]);
            offset += file_name.size();
            uint32to(file_size, &packet.data[offset], ByteOrder::LITTLEENDIAN);
        }

        //! Extracts the necessary fields from a received short META packet.
        //! \param pdata An incoming short META-type packet
        //! \param meta Reference to a packet_struct_metashort to fill
        //! \return n/a
        void deserialize_metadata(const vector<PACKET_BYTE>& pdata, packet_struct_metashort &meta)
        {
            const uint8_t MAXSTRLEN = sizeof(uint8_t);
            size_t offset = 0;
            meta.node_id = pdata[offset];
            offset += sizeof(PACKET_NODE_ID_TYPE);
            meta.tx_id = pdata[offset];
            offset += sizeof(PACKET_TX_ID_TYPE);
            meta.agent_name_len = pdata[offset];
            offset += MAXSTRLEN;
            meta.agent_name.resize(meta.agent_name_len);
            std::copy_n(pdata.begin()+offset, meta.agent_name_len, meta.agent_name.begin());
            offset += meta.agent_name.size();
            meta.file_name_len = pdata[offset];
            offset += MAXSTRLEN;
            meta.file_name.resize(meta.file_name_len);
            std::copy_n(pdata.begin()+offset, meta.file_name_len, meta.file_name.begin());
            offset += meta.file_name.size();
            meta.file_size = uint32from(&pdata[offset], ByteOrder::LITTLEENDIAN);
        }

        //! Create a DATA-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param orig_node_id Node's ID in the node table
        //! \param byte_count Number of bytes of data data
        //! \param chunk_start Byte of index of start of this chunk in the file
        //! \param chunk Array of bytes defining this chunk
        //! \return n/a
        void serialize_data(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id, PACKET_TX_ID_TYPE tx_id, PACKET_CHUNK_SIZE_TYPE byte_count, PACKET_FILE_SIZE_TYPE chunk_start, PACKET_BYTE* chunk)
        {
            packet.header.type = PacketComm::TypeId::DataFileChunkData;
            packet.data.resize(offsetof(struct packet_struct_data, chunk) + byte_count);
            memcpy(&packet.data[0]+offsetof(struct packet_struct_data, node_id),     &orig_node_id,  sizeof(PACKET_NODE_ID_TYPE));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_data, tx_id),       &tx_id,         sizeof(PACKET_TX_ID_TYPE));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_data, byte_count),  &byte_count,    sizeof(PACKET_CHUNK_SIZE_TYPE));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_data, chunk_start), &chunk_start,   sizeof(chunk_start));
            memcpy(&packet.data[0]+offsetof(struct packet_struct_data, chunk),       chunk,          byte_count);
        }

        //! Extracts the necessary fields from a received DATA packet.
        //! \param pdata An incoming DATA-type packet
        //! \param data Reference to a packet_struct_data to fill
        //! \return n/a
        void deserialize_data(const vector<PACKET_BYTE>& pdata, packet_struct_data &data)
        {
            memcpy(&data.node_id,     &pdata[0]+offsetof(struct packet_struct_data, node_id),     sizeof(PACKET_NODE_ID_TYPE));
            memcpy(&data.tx_id,       &pdata[0]+offsetof(struct packet_struct_data, tx_id),       sizeof(PACKET_TX_ID_TYPE));
            memcpy(&data.byte_count,  &pdata[0]+offsetof(struct packet_struct_data, byte_count),  sizeof(data.byte_count));
            memcpy(&data.chunk_start, &pdata[0]+offsetof(struct packet_struct_data, chunk_start), sizeof(data.chunk_start));
            data.chunk.resize(data.byte_count);
            memcpy(data.chunk.data(), pdata.data()+offsetof(struct packet_struct_data, chunk), data.byte_count);
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

            if (!tx_in.sentmeta)
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

        //! Adds a chunk to the tx_progress vector.
        //! Used when a DATA or REQDATA packet is received.
        //! \param tx The tx_progress of a file in the incoming queue
        //! \param tp Data chunk to add
        //! \return true if tx_in was updated
        bool add_chunk(tx_progress& tx, file_progress& tp)
        {
            uint32_t check=0;
            bool duplicate = false;
            bool updated = false;
            PACKET_CHUNK_SIZE_TYPE byte_count = tp.chunk_end - tp.chunk_start + 1;

            // Do we have any data yet?
            if (!tx.file_info.size())
            {
                // Add first entry, then write data
                tx.file_info.push_back(tp);
                tx.total_bytes += byte_count;
                updated = true;
            }
            else
            {
                // Check against existing data
                for (uint32_t j=0; j<tx.file_info.size(); ++j)
                {
                    // Check for duplicate
                    if (tp.chunk_start >= tx.file_info[j].chunk_start && tp.chunk_end <= tx.file_info[j].chunk_end)
                    {
                        duplicate = true;
                        break;
                    }
                    // If we start before this entry
                    if (tp.chunk_start < tx.file_info[j].chunk_start)
                    {
                        // If we end before this entry (at least one byte between), insert
                        if (tp.chunk_end + 1 < tx.file_info[j].chunk_start)
                        {
                            tx.file_info.insert(tx.file_info.begin()+j, tp);
                            tx.total_bytes += byte_count;
                            updated = true;
                            break;
                        }
                        // Otherwise, extend the near end
                        else
                        {
                            tp.chunk_end = tx.file_info[j].chunk_start - 1;
                            tx.file_info[j].chunk_start = tp.chunk_start;
                            byte_count = (tp.chunk_end - tp.chunk_start) + 1;
                            tx.total_bytes += byte_count;
                            updated = true;
                            break;
                        }
                    }
                    else
                    {
                        // If we overlap on the end, extend the far end
                        if (tp.chunk_start <= tx.file_info[j].chunk_end + 1)
                        {
                            if (tp.chunk_end > tx.file_info[j].chunk_end)
                            {
                                byte_count = tp.chunk_end - tx.file_info[j].chunk_end;
                                tp.chunk_start = tx.file_info[j].chunk_end + 1;
                                tx.file_info[j].chunk_end = tp.chunk_end;
                                tx.total_bytes += byte_count;
                                updated = true;
                                break;
                            }
                        }
                    }
                    check = j + 1;
                }


                // If we are higher than everything currently in the list, then append
                if (!duplicate && check == tx.file_info.size())
                {
                    tx.file_info.push_back(tp);
                    tx.total_bytes += byte_count;
                    updated = true;
                }
            }

            return updated;
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
            int32_t iretn = 0;
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
