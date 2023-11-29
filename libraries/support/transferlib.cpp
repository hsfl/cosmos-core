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
        //! \param queue A vector with QUEUE data
        //! \return n/a
        void serialize_queue(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id, const vector<PACKET_TX_ID_TYPE>& queue)
        {
            packet.header.type = PacketComm::TypeId::DataFileQueue;
            packet.data.resize(
                sizeof(file_packet_header)              // header
                + sizeof(packet_struct_queue::tx_ids)); // tx_ids
            size_t offset = 0;
            file_packet_header header;
            header.version = FILE_TRANSFER_PROTOCOL_VERSION;
            header.node_id = orig_node_id;
            header.tx_id = 0;
            header.file_crc = 0;
            memcpy(&packet.data[offset], &header, sizeof(file_packet_header));
            offset += sizeof(file_packet_header);

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
        //! \return 0 on success, negative on error
        int32_t deserialize_queue(const vector<PACKET_BYTE>& pdata, packet_struct_queue& queue)
        {
            if (pdata.empty())
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            // Reject version mismatches
            if (pdata[0] != FILE_TRANSFER_PROTOCOL_VERSION)
            {
                return TRANSFER_ERROR_VERSION;
            }
            size_t offset = 0;
            memcpy(&queue.header, &pdata[offset], sizeof(file_packet_header));
            offset += sizeof(file_packet_header);
            std::copy(pdata.begin()+offset, pdata.end(), queue.tx_ids);

            return 0;
        }

        //! Create a CANCEL-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param orig_node_id ID of the origin node in the node table
        //! \param tx_id ID of the transaction
        //! \return n/a
        void serialize_cancel(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc)
        {
            packet.header.type = PacketComm::TypeId::DataFileCancel;
            packet.data.resize(sizeof(packet_struct_cancel));
            file_packet_header header;
            header.version = FILE_TRANSFER_PROTOCOL_VERSION;
            header.node_id = orig_node_id;
            header.tx_id = tx_id;
            header.file_crc = file_crc;
            memcpy(&packet.data[0], &header, sizeof(file_packet_header));
        }

        //! Extracts the necessary fields from a received CANCEL packet.
        //! \param pdata An incoming CANCEL-type packet
        //! \param cancel Reference to a packet_struct_cancel to fill
        //! \return 0 on success, negative on error
        int32_t deserialize_cancel(const vector<PACKET_BYTE>& pdata, packet_struct_cancel &cancel)
        {
            if (pdata.empty())
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            // Reject version mismatches
            if (pdata[0] != FILE_TRANSFER_PROTOCOL_VERSION)
            {
                return TRANSFER_ERROR_VERSION;
            }
            memcpy(&cancel, pdata.data(), sizeof(packet_struct_cancel));
            return 0;
        }

        //! Create a REQCOMPLETE-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param orig_node_id ID of the origin node in the node table
        //! \param tx_id ID of the transaction
        //! \return n/a
        void serialize_reqcomplete(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc)
        {
            packet.header.type = PacketComm::TypeId::DataFileReqComplete;
            packet.data.resize(sizeof(file_packet_header));
            file_packet_header header;
            header.version = FILE_TRANSFER_PROTOCOL_VERSION;
            header.node_id = orig_node_id;
            header.tx_id = tx_id;
            header.file_crc = file_crc;
            memcpy(&packet.data[0], &header, sizeof(file_packet_header));
        }

        //! Extracts the necessary fields from a received REQCOMPLETE packet.
        //! \param pdata An incoming COMPLETE-type packet
        //! \param reqcomplete Reference to a packet_struct_reqcomplete to fill
        //! \return 0 on success, negative on error
        int32_t deserialize_reqcomplete(const vector<PACKET_BYTE>& pdata, packet_struct_reqcomplete &reqcomplete)
        {
            if (pdata.empty())
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            // Reject version mismatches
            if (pdata[0] != FILE_TRANSFER_PROTOCOL_VERSION)
            {
                return TRANSFER_ERROR_VERSION;
            }
            memcpy(&reqcomplete, pdata.data(), sizeof(packet_struct_reqcomplete));
            return 0;
        }

        //! Create a COMPLETE-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the receiver node in the node table
        //! \param tx_id ID of the transaction
        //! \return n/a
        void serialize_complete(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc)
        {
            packet.header.type = PacketComm::TypeId::DataFileComplete;
            packet.data.resize(sizeof(file_packet_header));
            file_packet_header header;
            header.version = FILE_TRANSFER_PROTOCOL_VERSION;
            header.node_id = node_id;
            header.tx_id = tx_id;
            header.file_crc = file_crc;
            memcpy(&packet.data[0], &header, sizeof(file_packet_header));
        }

        //! Extracts the necessary fields from a received COMPLETE packet.
        //! \param pdata An incoming COMPLETE-type packet
        //! \param complete Reference to a packet_struct_complete to fill
        //! \return 0 on success, negative on error
        int32_t deserialize_complete(const vector<PACKET_BYTE>& pdata, packet_struct_complete &complete)
        {
            if (pdata.empty())
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            // Reject version mismatches
            if (pdata[0] != FILE_TRANSFER_PROTOCOL_VERSION)
            {
                return TRANSFER_ERROR_VERSION;
            }
            memcpy(&complete, pdata.data(), sizeof(file_packet_header));
            return 0;
        }

        //! Create a REQMETA-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param node_id ID of the requester node in the node table
        //! \param node_name Name of the origin node
        //! \param reqmeta A vector with REQMETA data
        //! \return n/a
        void serialize_reqmeta(PacketComm& packet, PACKET_NODE_ID_TYPE node_id, string node_name, const vector<PACKET_TX_ID_TYPE>& reqmeta)
        {
            serialize_queue(packet, node_id, reqmeta);
            packet.header.type = PacketComm::TypeId::DataFileReqMeta;
        }

        //! Extracts the necessary fields from a received REQMETA packet.
        //! \param pdata An incoming REQMETA-type packet
        //! \param reqmeta Reference to a packet_struct_reqmeta to fill
        //! \return 0 on success, negative on error
        int32_t deserialize_reqmeta(const vector<PACKET_BYTE>& pdata, packet_struct_reqmeta& reqmeta)
        {
            return deserialize_queue(pdata, reqmeta);
        }

        //! Create a REQDATA-type PacketComm packet.
        //! \param packets Reference to a vector of PacketComm packets to push to
        //! \param self_node_id ID of the requester node in the node table
        //! \param orig_node_id ID of the origin node in the node table
        //! \param tx_id ID of the transaction
        //! \param holes Vector of file_progress holes
        //! \param packet_data_size Limit of packet data size
        //! \return n/a
        void serialize_reqdata(vector<PacketComm>& packets, PACKET_NODE_ID_TYPE self_node_id, PACKET_NODE_ID_TYPE orig_node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc, vector<file_progress>& holes, PACKET_CHUNK_SIZE_TYPE packet_data_size)
        {
            PacketComm packet;
            packet.header.nodeorig = self_node_id;
            packet.header.nodedest = orig_node_id;
            packet.header.type = PacketComm::TypeId::DataFileReqData;
            packet.data.reserve(packet_data_size);


            uint16_t num_holes = 0;
            constexpr size_t num_holes_offset   = sizeof(file_packet_header);
            constexpr size_t holes_offset       = sizeof(file_packet_header) + sizeof(num_holes);
            packet.data.resize(holes_offset);
            file_packet_header header;
            header.version = FILE_TRANSFER_PROTOCOL_VERSION;
            header.node_id = self_node_id;
            header.tx_id = tx_id;
            header.file_crc = file_crc;
            memcpy(&packet.data[0], &header, sizeof(file_packet_header));


            // Add as many holes as possible to each reqdata packet
            constexpr uint8_t has_first = 1;
            constexpr uint8_t has_last = 2;
            // Apply has_first or has_last bit masking to this to signify whether the current
            // reqdata packet contains the first and/or last hole
            uint8_t contains_first_or_last = has_first;
            for (size_t i=0; i < holes.size(); ++i)
            {
                // If i==0, mark first. if i == holes.size()-1, mark end
                // If neither, mark nothing
                // If no more holes can be fit into this packet.
                // (save 1 byte at the end for signifying first and/or last hole)
                if (packet.data.size() + sizeof(holes[i]) > static_cast<size_t>(packet_data_size-1))
                {
                    // Copy in num_holes into place
                    memcpy(&packet.data[0]+num_holes_offset, &num_holes, sizeof(num_holes));
                    // Last element is first/last signifier
                    packet.data.push_back(contains_first_or_last);
                    packets.push_back(packet);
                    // Resize back to reqdata header to create next reqdata packet
                    packet.data.resize(holes_offset);
                    num_holes = 0;
                    contains_first_or_last = 0;
                }
                // Append holes to holes vector
                packet.data.insert(packet.data.end(), (uint8_t*)&holes[i], (uint8_t*)((&holes[i])+1));
                ++num_holes;
            }
            // Mark last packet as containing the last hole
            contains_first_or_last |= has_last;
            packet.data.push_back(contains_first_or_last);
            memcpy(&packet.data[0]+num_holes_offset, &num_holes, sizeof(num_holes));
            packets.push_back(packet);
        }

        //! Extracts the necessary fields from a received REQDATA packet.
        //! \param pdata An incoming REQDATA-type packet
        //! \param reqdata Reference to a packet_struct_reqdata to fill
        //! \return non-negative on success: 0 if this reqdata packet has neither first nor last
        //! hole, 1 if this contains the first hole, 2 if this contains the last hole,
        //! and 3 if both. Since holes are assumed to be sorted, first is firster, last is laster.
        //! Negative on error.
        int32_t deserialize_reqdata(const vector<PACKET_BYTE>& pdata, packet_struct_reqdata &reqdata)
        {   
            if (pdata.empty())
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            // Reject version mismatches
            reqdata.header.version = pdata[0];
            if (pdata[0] != FILE_TRANSFER_PROTOCOL_VERSION)
            {
                return TRANSFER_ERROR_VERSION;
            }

            constexpr size_t num_holes_offset   = sizeof(file_packet_header);
            constexpr size_t holes_offset       = sizeof(file_packet_header) + sizeof(uint16_t);
            if (pdata.size() < holes_offset)
            {
                return COSMOS_DATA_ERROR_SIZE_MISMATCH;
            }
            memcpy(&reqdata.header,     &pdata[0],     sizeof(packet_struct_reqdata::header));
            memcpy(&reqdata.num_holes,  &pdata[0]+num_holes_offset,  sizeof(packet_struct_reqdata::num_holes));
            size_t packet_size = holes_offset + reqdata.num_holes * sizeof(file_progress);
            // Last byte must be the first/last hole signifier
            if (pdata.size() != packet_size + 1)
            {
                return COSMOS_DATA_ERROR_SIZE_MISMATCH;
            }
            reqdata.holes.resize(reqdata.num_holes);
            memcpy(reqdata.holes.data(), pdata.data()+holes_offset, reqdata.num_holes*sizeof(file_progress));
            // Return first/last hole signifier
            return pdata[pdata.size()-1];
        }

        //! Serialize data to write to a .meta file.
        //! Includes node_name, omits node_id information.
        //! Used by write_meta()
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param tx_id Transaction ID of file transfer
        //! \param file_name Name of the file
        //! \param file_size Size of the file
        //! \param node_name Name of the receiving node
        //! \param agent_name Name of the receiving agent
        //! \return n/a
        void serialize_metafile(vector<PACKET_BYTE>& pdata, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc, const string& file_name, PACKET_FILE_SIZE_TYPE file_size, const string& node_name, const string& agent_name)
        {
            constexpr uint8_t MAXSTRLEN = sizeof(uint8_t);
            pdata.resize(
                MAXSTRLEN                           // node_name_len
                + node_name.size()                  // node_name
                + sizeof(PACKET_TX_ID_TYPE)         // tx_id
                + sizeof(PACKET_FILE_CRC_TYPE)      // file_crc
                + MAXSTRLEN                         // agent_name_len
                + agent_name.size()                 // agent_name
                + MAXSTRLEN                         // file_name_len
                + file_name.size()                  // file_name
                + sizeof(PACKET_FILE_SIZE_TYPE));   // file_size
            size_t offset = 0;
            pdata[offset] = node_name.size();
            offset += MAXSTRLEN;
            std::copy_n(node_name.begin(), node_name.size(), &pdata[offset]);
            offset += node_name.size();
            pdata[offset] = tx_id;
            offset += sizeof(PACKET_TX_ID_TYPE);
            memcpy(&pdata[offset], &file_crc, sizeof(PACKET_FILE_CRC_TYPE));
            offset += sizeof(PACKET_FILE_CRC_TYPE);
            pdata[offset] = agent_name.size();
            offset += MAXSTRLEN;
            std::copy_n(agent_name.begin(), agent_name.size(), &pdata[offset]);
            offset += agent_name.size();
            pdata[offset] = file_name.size();
            offset += MAXSTRLEN;
            std::copy_n(file_name.begin(), file_name.size(), &pdata[offset]);
            offset += file_name.size();
            uint32to(file_size, &pdata[offset], ByteOrder::LITTLEENDIAN);
        }

        //! Extracts the necessary fields from a .meta file.
        //! Used by read_meta()
        //! \param pdata An incoming long META-type packet
        //! \param meta Reference to a packet_struct_long to fill
        //! \return 0 on success, negative on error
        int32_t deserialize_metafile(const vector<PACKET_BYTE>& pdata, packet_struct_metafile &meta)
        {
            constexpr uint8_t MAXSTRLEN = sizeof(uint8_t);
            size_t offset = 0;
            meta.node_name_len = pdata[offset];
            offset += MAXSTRLEN;
            meta.node_name.resize(meta.node_name_len);
            std::copy_n(pdata.begin()+offset, meta.node_name_len, meta.node_name.begin());
            offset += meta.node_name.size();
            meta.tx_id = pdata[offset];
            offset += sizeof(PACKET_TX_ID_TYPE);
            memcpy(&meta.file_crc, &pdata[offset], sizeof(PACKET_FILE_CRC_TYPE));
            offset += sizeof(PACKET_FILE_CRC_TYPE);
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
            return 0;
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
        void serialize_metadata(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id , PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc, const string& file_name, PACKET_FILE_SIZE_TYPE file_size, const string& agent_name)
        {
            constexpr uint8_t MAXSTRLEN = sizeof(uint8_t);
            packet.header.type = PacketComm::TypeId::DataFileMetaData;
            packet.data.resize(
                sizeof(file_packet_header)          // header
                + MAXSTRLEN                         // agent_name_len
                + agent_name.size()                 // agent_name
                + MAXSTRLEN                         // file_name_len
                + file_name.size()                  // file_name
                + sizeof(PACKET_FILE_SIZE_TYPE));   // file_size
            size_t offset = 0;
            file_packet_header header;
            header.version = FILE_TRANSFER_PROTOCOL_VERSION;
            header.node_id = orig_node_id;
            header.tx_id = tx_id;
            header.file_crc = file_crc;
            memcpy(&packet.data[offset], &header, sizeof(file_packet_header));
            offset += sizeof(file_packet_header);
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
        //! \param meta Reference to a packet_struct_metadata to fill
        //! \return 0 on success, negative on error
        int32_t deserialize_metadata(const vector<PACKET_BYTE>& pdata, packet_struct_metadata &meta)
        {
            if (pdata.empty())
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            // Reject version mismatches
            meta.header.version = pdata[0];
            if (pdata[0] != FILE_TRANSFER_PROTOCOL_VERSION)
            {
                return TRANSFER_ERROR_VERSION;
            }
            constexpr uint8_t MAXSTRLEN = sizeof(uint8_t);
            size_t offset = 0;
            memcpy(&meta.header, &pdata[offset], sizeof(file_packet_header));
            offset += sizeof(file_packet_header);
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
            return 0;
        }

        //! Create a DATA-type PacketComm packet.
        //! \param packet Reference to a PacketComm packet to fill in
        //! \param orig_node_id Node's ID in the node table
        //! \param byte_count Number of bytes of data data
        //! \param chunk_start Byte of index of start of this chunk in the file
        //! \param chunk Array of bytes defining this chunk
        //! \return n/a
        void serialize_data(PacketComm& packet, PACKET_NODE_ID_TYPE orig_node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_CRC_TYPE file_crc, PACKET_CHUNK_SIZE_TYPE byte_count, PACKET_FILE_SIZE_TYPE chunk_start, PACKET_BYTE* chunk)
        {
            packet.header.type = PacketComm::TypeId::DataFileChunkData;
            constexpr size_t byte_count_offset  = sizeof(file_packet_header);
            constexpr size_t chunk_start_offset = byte_count_offset     + sizeof(PACKET_CHUNK_SIZE_TYPE);
            constexpr size_t chunk_offset       = chunk_start_offset    + sizeof(PACKET_FILE_SIZE_TYPE);

            packet.data.resize(chunk_offset + byte_count);
            file_packet_header header;
            header.version = FILE_TRANSFER_PROTOCOL_VERSION;
            header.node_id = orig_node_id;
            header.tx_id = tx_id;
            header.file_crc = file_crc;
            memcpy(&packet.data[0],                    &header,        sizeof(file_packet_header));
            memcpy(&packet.data[0]+byte_count_offset,  &byte_count,    sizeof(PACKET_CHUNK_SIZE_TYPE));
            memcpy(&packet.data[0]+chunk_start_offset, &chunk_start,   sizeof(chunk_start));
            memcpy(&packet.data[0]+chunk_offset,       chunk,          byte_count);
        }

        //! Extracts the necessary fields from a received DATA packet.
        //! \param pdata An incoming DATA-type packet
        //! \param data Reference to a packet_struct_data to fill
        //! \return 0 on success, negative on error
        int32_t deserialize_data(const vector<PACKET_BYTE>& pdata, packet_struct_data &data)
        {
            if (pdata.empty())
            {
                return GENERAL_ERROR_BAD_SIZE;
            }
            // Reject version mismatches
            if (pdata[0] != FILE_TRANSFER_PROTOCOL_VERSION)
            {
                return TRANSFER_ERROR_VERSION;
            }
            constexpr size_t byte_count_offset  = sizeof(file_packet_header);
            constexpr size_t chunk_start_offset = byte_count_offset     + sizeof(PACKET_CHUNK_SIZE_TYPE);
            constexpr size_t chunk_offset       = chunk_start_offset    + sizeof(PACKET_FILE_SIZE_TYPE);

            memcpy(&data.header,      &pdata[0],      sizeof(file_packet_header));
            memcpy(&data.byte_count,  &pdata[0]+byte_count_offset,  sizeof(data.byte_count));
            memcpy(&data.chunk_start, &pdata[0]+chunk_start_offset, sizeof(data.chunk_start));
            data.chunk.resize(data.byte_count);
            memcpy(data.chunk.data(), pdata.data()+chunk_offset, data.byte_count);
            return 0;
        }

        //! Merges any overlapping chunks in the tx.file_info deque.
        //! \param tx A tx_progress to merge chunks for
        //! \return Sum of bytes of the chunks in tx
        PACKET_FILE_SIZE_TYPE merge_chunks_overlap(tx_progress& tx)
        {
            // Remove any chunks that go beyond the file size
            auto it = tx.file_info.begin();
            // If the METADATA has yet to be received, then file_size will be 0
            if (tx.file_size > 0)
            {
                while (it != tx.file_info.end())
                {
                    if (it->chunk_end >= tx.file_size)
                    {
                        // TODO: consider if another data structure may be better
                        it = tx.file_info.erase(it);
                    }
                    else {
                        ++it;
                    }
                }
            }
            switch (tx.file_info.size())
            {
            case 0:
                {
                    tx.total_bytes = 0;
                }
                break;
            case 1:
                {
                    tx.total_bytes = (tx.file_info[0].chunk_end - tx.file_info[0].chunk_start) + 1;
                }
                break;
            default:
                {
                    tx.total_bytes = 0;
                    sort(tx.file_info.begin(), tx.file_info.end(), lower_chunk);
                    // Merge chunks
                    for (auto it = tx.file_info.begin(); it != tx.file_info.end(); ++it)
                    {
                        tx.total_bytes += (it->chunk_end - it->chunk_start) + 1;
                        auto it_next = it+1;
                        while (it_next != tx.file_info.end())
                        {
                            // Merge if the next chunk's start overlaps with the previous chunk's end
                            if (it_next->chunk_start <= it->chunk_end+1)
                            {
                                // Sanity check in case the next chunk's end was less than the previous chunk's end
                                if (it_next->chunk_end > it->chunk_end)
                                {
                                    tx.total_bytes += it_next->chunk_end - it->chunk_end;
                                    it->chunk_end = it_next->chunk_end;
                                }
                                it_next = tx.file_info.erase(it_next);
                            }
                            else
                            {
                                ++it_next;
                            }
                        } // End while
                    } // End for
                } // End default
                break;
            } // End switch

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
                tx_in.sentdata = true;
                tx_in.complete = true;
            }

            return (missing);
        }

        //! Adds a vector chunk to the tx_progress vector.
        //! Used when sender receives a REQDATA packet.
        //! \param tx The tx_progress of a file in the incoming queue
        //! \param holes Data chunks to add
        //! \param start_end_signifier The return of deserialize_reqdata(), signifies whether this sorted vector contains the first or last hole
        //! \return true if tx was updated
        bool add_chunks(tx_progress& tx, const vector<file_progress>& holes, uint8_t start_end_signifier)
        {
            bool updated = false;
            bool has_start = start_end_signifier & 0x1; // has start if bit 0 is set
            bool has_end = start_end_signifier & 0x2; // has end if bit 1 is set
            for (size_t i=0; i<holes.size(); ++i)
            {
                // Simple validity check
                if (holes[i].chunk_end < holes[i].chunk_start)
                {
                    continue;
                }

                bool is_start = false;
                bool is_end = false;

                if (has_start && i == 0)
                {
                    is_start = true;
                }
                if (has_end && (i == holes.size() - 1))
                {
                    is_end = true;
                }
                // Add this chunk to the queue
                updated = add_chunk(tx, holes[i], is_start, is_end) || updated;
            }
            return updated;
        }

        //! Adds a chunk to the tx_progress vector.
        //! Used when a DATA packet is received.
        //! \param tx The tx_progress of a file in the incoming queue
        //! \param tp Data chunk to add
        //! \return true if tx_in was updated
        bool add_chunk(tx_progress& tx, const file_progress& tp)
        {
            return add_chunk(tx, tp, false, false);
        }

        //! Adds a chunk to the tx_progress vector.
        //! Called by the other add_chunk or add_chunks.
        //! \param tx The tx_progress of a file in the incoming queue
        //! \param tp Data chunk to add
        //! \param is_start If this chunk's start is the holes vector's start
        //! \param is_end If this chunk's end is the holes vector's end
        //! \return true if tx_in was updated
        bool add_chunk(tx_progress& tx, const file_progress& tp, bool is_start, bool is_end)
        {
            // Sort first before truncating either end
            if (is_start || is_end)
            {
                sort(tx.file_info.begin(), tx.file_info.end(), lower_chunk);
            }
            // Bytes in hole
            const PACKET_CHUNK_SIZE_TYPE byte_count = tp.chunk_end - tp.chunk_start + 1;

            // Do we have any data yet?
            if (!tx.file_info.size())
            {
                // Add first entry, then write data
                tx.file_info.push_back(tp);
                tx.total_bytes += byte_count;
                return true;
            }
            // Check against existing data
            for (uint32_t j=0; j<tx.file_info.size(); ++j)
            {
                // If we start before this entry
                if (tp.chunk_start < tx.file_info[j].chunk_start)
                {
                    
                    // If the new chunk is the start hole, then truncate anything before
                    if (is_start)
                    {
                        tx.file_info.erase(tx.file_info.begin(), tx.file_info.begin()+j);
                        j = 0;
                    }
                    // If we end before this entry starts (at least one byte between)
                    if (tp.chunk_end + 1 < tx.file_info[j].chunk_start)
                    {
                        // If the new chunk is the end hole, then truncate anything beyond
                        if (is_end)
                        {
                            tx.file_info.resize(j);
                        }
                        // Insert the new chunk
                        tx.file_info.insert(tx.file_info.begin()+j, tp);
                        tx.total_bytes += byte_count;

                        if (is_start || is_end)
                        {
                            // Recalculate total_bytes
                            merge_chunks_overlap(tx);
                        }
                        return true;
                    }
                    // We end beyond the start of this chunk
                    else
                    {
                        // If the new chunk is the end hole, then truncate anything beyond
                        // and set the end to the new chunk's end
                        if (is_end)
                        {
                            tx.file_info.resize(j+1);
                            tx.file_info[j].chunk_end = tp.chunk_end;
                        }
                        // If we completely encompass this chunk, have this chunk match start and end
                        if (tp.chunk_end > tx.file_info[j].chunk_end)
                        {
                            tx.total_bytes += (tx.file_info[j].chunk_start - tp.chunk_start);
                            tx.total_bytes += (tp.chunk_end - tx.file_info[j].chunk_end);
                            tx.file_info[j].chunk_start = tp.chunk_start;
                            tx.file_info[j].chunk_end = tp.chunk_end;
                            if (is_start || is_end)
                            {
                                // Recalculate total_bytes
                                merge_chunks_overlap(tx);
                            }
                            return true;
                        }
                        // We end before the end of this chunk, then extend the front of this chunk
                        tx.total_bytes += (tx.file_info[j].chunk_start - tp.chunk_start);
                        tx.file_info[j].chunk_start = tp.chunk_start;
                        if (is_start || is_end)
                        {
                            // Recalculate total_bytes
                            merge_chunks_overlap(tx);
                        }
                        return true;
                    }
                }
                // If we start somewhere between the start and end of this chunk
                else if (tp.chunk_start <= tx.file_info[j].chunk_end + 1)
                {
                    // If the new chunk is the end hole, then truncate anything beyond
                    // and set the end to the new chunk's end
                    if (is_end)
                    {
                        tx.file_info.resize(j+1);
                        tx.file_info[j].chunk_end = tp.chunk_end;
                    }
                    // If the new chunk is the start hole, then truncate anything before
                    if (is_start)
                    {
                        tx.file_info.erase(tx.file_info.begin(), tx.file_info.begin()+j);
                        j = 0;
                        tx.file_info[j].chunk_start = tp.chunk_start;
                    }
                    // If we end beyond the end of this chunk, then extend the end of this chunk
                    if (tp.chunk_end > tx.file_info[j].chunk_end)
                    {
                        tx.total_bytes += (tp.chunk_end - tx.file_info[j].chunk_end);
                        tx.file_info[j].chunk_end = tp.chunk_end;
                        if (is_start || is_end)
                        {
                            // Recalculate total_bytes
                            merge_chunks_overlap(tx);
                        }
                        return true;
                    }
                    if (is_start || is_end)
                    {
                        // Recalculate total_bytes
                        merge_chunks_overlap(tx);
                    }
                    // New chunk is completely inside this chunk, just discard it.
                    // Unless, we had to truncate anything before, then we did in fact update.
                    return is_start || is_end;
                }
                // We were somewhere beyond the end of this chunk
            } // End for

            // If we are higher than everything currently in the list
            // If the new chunk is the start hole, then clear the whole vector
            if (is_start)
            {
                tx.file_info.clear();
                tx.total_bytes = 0;
            }
            // Append
            tx.file_info.push_back(tp);
            tx.total_bytes += byte_count;

            return true;
        }

        //! Gets the size of a file.
        /*! Looks up the size of the file on the filesystem.
        * On success, sets the size param to the size of the file.
        * Performs a conversion from off_t to uint32_t, which we will say is large enough for our purposes.
        * \param filename Full path to file
        * \param size On success, this is set to the size of the file
        * \return 0 on success, or negative error.
        */
        int32_t get_file_size(string filename, PACKET_FILE_SIZE_TYPE& size)
        {
            struct stat stat_buf;

            if ((stat(filename.c_str(), &stat_buf)) == 0)
            {
                size = static_cast<PACKET_FILE_SIZE_TYPE>(stat_buf.st_size);
                return 0;
            }
            else
            {
                return -errno;
            }
        }

        int32_t get_file_size(const char* filename, PACKET_FILE_SIZE_TYPE& size)
        {
            string sfilename = filename;
            return get_file_size(sfilename, size);
        }

        /**
         * @brief Clears out the incoming or outgoing tx_entry.
         * 
         *  Resets the queue to a clean state. If there are temp files, then those
         *  are cleared out too.
         * 
         * @param tx The incoming or outgoing tx_entry to clear out
         * @return int32_t 0 on success, negative on error
         */
        int32_t clear_tx_entry(tx_entry& tx)
        {
            // Check file stuff first
            for (size_t i=0; i < PROGRESS_QUEUE_SIZE; ++i)
            {
                // Close file pointer
                if (tx.progress[i].fp != nullptr)
                {
                    fclose(tx.progress[i].fp);
                    tx.progress[i].fp = nullptr;
                }
                // Clear out temporary files
                if (tx.progress[i].temppath.size())
                {
                    remove((tx.progress[i].temppath + ".meta").c_str());
                    remove((tx.progress[i].temppath + ".file").c_str());
                }
            }
            // Reset everything to default
            tx = tx_entry();
            return 0;
        }

        /**
         * @brief Closes file pointers for the incoming or outgoing tx_entry.
         * 
         * Use to flush current progress.
         * 
         * @param tx The incoming or outgoing tx_entry
         * @return int32_t 0 on success, negative on error
         */
        int32_t flush_tx_entry(tx_entry& tx)
        {
            // Check file stuff first
            for (size_t i=0; i < PROGRESS_QUEUE_SIZE; ++i)
            {
                // Close file pointer
                if (tx.progress[i].fp != nullptr)
                {
                    fclose(tx.progress[i].fp);
                    tx.progress[i].fp = nullptr;
                }
            }
            return 0;
        }
    }
}
