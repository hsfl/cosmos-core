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

#include "support/transferlib.h"
#include "support/timelib.h"
#include <iostream>
#include <sys/stat.h>

//#define DA_BUG

//using namespace PACKET_TYPE;

//bool IS_MESSAGE(const unsigned char P_TYPE)	{ return P_TYPE & PACKET_MESSAGE; }
//bool IS_REQUEST(const unsigned char P_TYPE) { return P_TYPE & PACKET_REQUEST; }
bool IS_METADATA(const unsigned char P_TYPE){ return (((P_TYPE & 0x0f) & PACKET_METADATA) != 0); }
bool IS_DATA(const unsigned char P_TYPE)	{ return (((P_TYPE & 0x0f) & PACKET_DATA) != 0); }
bool IS_REQDATA(const unsigned char P_TYPE)	{ return (((P_TYPE & 0x0f) & PACKET_REQDATA) != 0); }
bool IS_REQMETA(const unsigned char P_TYPE)	{ return (((P_TYPE & 0x0f) & PACKET_REQMETA) != 0); }
bool IS_COMPLETE(const unsigned char P_TYPE)	{ return (((P_TYPE & 0x0f) & PACKET_COMPLETE) != 0); }
bool IS_CANCEL(const unsigned char P_TYPE)	{ return (((P_TYPE & 0x0f) & PACKET_CANCEL) != 0); }
bool IS_QUEUE(const unsigned char P_TYPE)	{ return (((P_TYPE & 0x0f) & PACKET_QUEUE) != 0); }

/** the Node ID lookup table */
static vector <string> nodeids;
//uint8_t lookup_node_id(PACKET_NODE_ID_TYPE node_id);
//uint8_t lookup_node_id(string node_name);

void make_complete_packet(vector<PACKET_BYTE>& packet, packet_struct_complete complete)
{
    make_complete_packet(packet, complete.node_id, complete.tx_id);
}

void make_complete_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id)
{
    PACKET_TYPE type = salt_type(PACKET_COMPLETE);

    packet.resize(PACKET_COMPLETE_OFFSET_TOTAL);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_COMPLETE_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
    memmove(&packet[0]+PACKET_COMPLETE_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

void extract_complete(vector<PACKET_BYTE>& packet, packet_struct_complete &complete)
{
    extract_complete(packet, complete.node_id, complete.tx_id);
}

void extract_complete(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE& node_id, PACKET_TX_ID_TYPE &tx_id)
{
    memmove(&node_id, &packet[0]+PACKET_COMPLETE_OFFSET_NODE_ID, sizeof(PACKET_NODE_ID_TYPE));
    memmove(&tx_id, &packet[0]+PACKET_COMPLETE_OFFSET_TX_ID, sizeof(PACKET_TX_ID_TYPE));
}

void make_cancel_packet(vector<PACKET_BYTE>& packet, packet_struct_cancel cancel)
{
    make_cancel_packet(packet, cancel.node_id, cancel.tx_id);
}

void make_cancel_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id)
{
    PACKET_TYPE type = salt_type(PACKET_CANCEL);

    packet.resize(PACKET_CANCEL_OFFSET_TOTAL);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_CANCEL_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
    memmove(&packet[0]+PACKET_CANCEL_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

void extract_cancel(vector<PACKET_BYTE>& packet, packet_struct_cancel &cancel)
{
    extract_cancel(packet, cancel.node_id, cancel.tx_id);
}

void extract_cancel(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE& node_id, PACKET_TX_ID_TYPE &tx_id)
{
    memmove(&node_id, &packet[0]+PACKET_CANCEL_OFFSET_NODE_ID, sizeof(PACKET_NODE_ID_TYPE));
    memmove(&tx_id, &packet[0]+PACKET_CANCEL_OFFSET_TX_ID, sizeof(PACKET_TX_ID_TYPE));
}

void make_reqmeta_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string node_name, vector<PACKET_TX_ID_TYPE> reqmeta)
{
    PACKET_TYPE type = salt_type(PACKET_REQMETA);

    packet.resize(PACKET_REQMETA_OFFSET_TOTAL);
    memset(&packet[0], 0, PACKET_REQMETA_OFFSET_TOTAL);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, COSMOS_SIZEOF(PACKET_TYPE));
    memmove(&packet[0]+PACKET_REQMETA_OFFSET_NODE_ID, &node_id, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    memmove(&packet[0]+PACKET_REQMETA_OFFSET_NODE_NAME, node_name.c_str(), node_name.size());
    memmove(&packet[0]+PACKET_REQMETA_OFFSET_TX_ID, &reqmeta[0], COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

void extract_reqmeta(vector<PACKET_BYTE>& packet, packet_struct_reqmeta& reqmeta)
{
    memmove(&reqmeta.node_id, &packet[0]+PACKET_REQMETA_OFFSET_NODE_ID, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    memmove(&reqmeta.node_name, &packet[0]+PACKET_REQMETA_OFFSET_NODE_NAME, COSMOS_MAX_NAME);
    memmove(&reqmeta.tx_id, &packet[0]+PACKET_REQMETA_OFFSET_TX_ID, COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
}

void make_reqdata_packet(vector<PACKET_BYTE>& packet, packet_struct_reqdata reqdata)
{
    make_reqdata_packet(packet, reqdata.node_id, reqdata.tx_id, reqdata.hole_start, reqdata.hole_end);
}

void make_reqdata_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_SIZE_TYPE hole_start, PACKET_FILE_SIZE_TYPE hole_end)
{
    PACKET_TYPE type = salt_type(PACKET_REQDATA);

    packet.resize(PACKET_REQDATA_OFFSET_TOTAL);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_REQDATA_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
    memmove(&packet[0]+PACKET_REQDATA_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
    memmove(&packet[0]+PACKET_REQDATA_OFFSET_HOLE_START, &hole_start, sizeof(PACKET_FILE_SIZE_TYPE));
    memmove(&packet[0]+PACKET_REQDATA_OFFSET_HOLE_END, &hole_end, sizeof(PACKET_FILE_SIZE_TYPE));
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

void extract_reqdata(vector<PACKET_BYTE>& packet, packet_struct_reqdata &reqdata)
{
    extract_reqdata(packet, reqdata.node_id, reqdata.tx_id, reqdata.hole_start, reqdata.hole_end);
}

void extract_reqdata(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE& node_id, PACKET_TX_ID_TYPE &tx_id, PACKET_FILE_SIZE_TYPE &hole_start, PACKET_FILE_SIZE_TYPE &hole_end)
{
    memmove(&node_id, &packet[0]+PACKET_REQDATA_OFFSET_NODE_ID, sizeof(PACKET_NODE_ID_TYPE));
    memmove(&tx_id, &packet[0]+PACKET_REQDATA_OFFSET_TX_ID, sizeof(PACKET_TX_ID_TYPE));
    memmove(&hole_start, &packet[0]+PACKET_REQDATA_OFFSET_HOLE_START, sizeof(hole_start));
    memmove(&hole_end, &packet[0]+PACKET_REQDATA_OFFSET_HOLE_END, sizeof(hole_end));
}

void make_metadata_packet(vector<PACKET_BYTE>& packet , packet_struct_metalong meta)
{
    make_metadata_packet(packet, meta.tx_id, meta.file_name, meta.file_size, meta.node_name, meta.agent_name);
}

void make_metadata_packet(vector<PACKET_BYTE>& packet , PACKET_TX_ID_TYPE tx_id, char* file_name, PACKET_FILE_SIZE_TYPE file_size, char* node_name, char* agent_name)
{
    PACKET_TYPE type = salt_type(PACKET_METADATA);

    packet.resize(PACKET_METALONG_OFFSET_TOTAL);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_METALONG_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
    memmove(&packet[0]+PACKET_METALONG_OFFSET_FILE_NAME, file_name, TRANSFER_MAX_FILENAME);
    memmove(&packet[0]+PACKET_METALONG_OFFSET_FILE_SIZE, &file_size, sizeof(file_size));
    memmove(&packet[0]+PACKET_METALONG_OFFSET_NODE_NAME, node_name, COSMOS_MAX_NAME);
    memmove(&packet[0]+PACKET_METALONG_OFFSET_AGENT_NAME, agent_name, COSMOS_MAX_NAME);
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

void make_metadata_packet(vector<PACKET_BYTE>& packet , packet_struct_metashort meta)
{
    make_metadata_packet(packet, meta.node_id, meta.tx_id, meta.file_name, meta.file_size, meta.agent_name);
}

void make_metadata_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id , PACKET_TX_ID_TYPE tx_id, char* file_name, PACKET_FILE_SIZE_TYPE file_size, char* agent_name)
{
    PACKET_TYPE type = salt_type(PACKET_METADATA);

    packet.resize(PACKET_METASHORT_OFFSET_TOTAL);

    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_METASHORT_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
    memmove(&packet[0]+PACKET_METASHORT_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
    memmove(&packet[0]+PACKET_METASHORT_OFFSET_FILE_NAME, file_name, TRANSFER_MAX_FILENAME);
    memmove(&packet[0]+PACKET_METASHORT_OFFSET_FILE_SIZE, &file_size, sizeof(file_size));
    memmove(&packet[0]+PACKET_METASHORT_OFFSET_AGENT_NAME, agent_name, COSMOS_MAX_NAME);
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

void extract_metadata(vector<PACKET_BYTE>& packet, packet_struct_metalong &meta)
{
    extract_metadata(packet, meta.tx_id, meta.file_name, meta.file_size, meta.node_name, meta.agent_name);
}

void extract_metadata(vector<PACKET_BYTE>& packet, PACKET_TX_ID_TYPE &tx_id, char* file_name, PACKET_FILE_SIZE_TYPE& file_size, char* node_name, char* agent_name)
{
    memmove(&tx_id, &packet[0]+PACKET_METALONG_OFFSET_TX_ID, sizeof(PACKET_TX_ID_TYPE));
    memmove(node_name, &packet[0]+PACKET_METALONG_OFFSET_NODE_NAME, COSMOS_MAX_NAME);
    memmove(file_name, &packet[0]+PACKET_METALONG_OFFSET_FILE_NAME, TRANSFER_MAX_FILENAME);
    memmove(&file_size, &packet[0]+PACKET_METALONG_OFFSET_FILE_SIZE, sizeof(file_size));
    memmove(agent_name, &packet[0]+PACKET_METALONG_OFFSET_AGENT_NAME, COSMOS_MAX_NAME);
}

void extract_metadata(vector<PACKET_BYTE>& packet, packet_struct_metashort &meta)
{
    extract_metadata(packet, meta.tx_id, meta.file_name, meta.file_size, meta.node_id, meta.agent_name);
}

void extract_metadata(vector<PACKET_BYTE>& packet, PACKET_TX_ID_TYPE &tx_id, char* file_name, PACKET_FILE_SIZE_TYPE& file_size, PACKET_NODE_ID_TYPE& node_id, char* agent_name)
{
    memmove(&tx_id, &packet[0]+PACKET_METASHORT_OFFSET_TX_ID, sizeof(PACKET_TX_ID_TYPE));
    memmove(file_name, &packet[0]+PACKET_METASHORT_OFFSET_FILE_NAME, TRANSFER_MAX_FILENAME);
    memmove(&file_size, &packet[0]+PACKET_METASHORT_OFFSET_FILE_SIZE, sizeof(file_size));
    memmove(&node_id, &packet[0]+PACKET_METASHORT_OFFSET_NODE_ID, COSMOS_MAX_NAME);
    memmove(agent_name, &packet[0]+PACKET_METASHORT_OFFSET_AGENT_NAME, COSMOS_MAX_NAME);
}

void make_data_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_CHUNK_SIZE_TYPE byte_count, PACKET_FILE_SIZE_TYPE chunk_start, PACKET_BYTE* chunk)
{
    PACKET_TYPE type = salt_type(PACKET_DATA);

    packet.resize(PACKET_DATA_OFFSET_HEADER_TOTAL+byte_count);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_DATA_OFFSET_NODE_ID, &node_id, sizeof(PACKET_NODE_ID_TYPE));
    memmove(&packet[0]+PACKET_DATA_OFFSET_TX_ID, &tx_id, sizeof(PACKET_TX_ID_TYPE));
    memmove(&packet[0]+PACKET_DATA_OFFSET_BYTE_COUNT, &byte_count, sizeof(PACKET_CHUNK_SIZE_TYPE));
    memmove(&packet[0]+PACKET_DATA_OFFSET_CHUNK_START, &chunk_start, sizeof(chunk_start));
    memmove(&packet[0]+PACKET_DATA_OFFSET_CHUNK, chunk, byte_count);
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

//Function to extract necessary fileds from a received data packet
void extract_data(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE& node_id, PACKET_TX_ID_TYPE& tx_id, PACKET_CHUNK_SIZE_TYPE& byte_count, PACKET_FILE_SIZE_TYPE& chunk_start, PACKET_BYTE* chunk)
{
    memmove(&node_id, &packet[0]+PACKET_DATA_OFFSET_NODE_ID, sizeof(PACKET_NODE_ID_TYPE));
    memmove(&tx_id, &packet[0]+PACKET_DATA_OFFSET_TX_ID, sizeof(PACKET_TX_ID_TYPE));
    memmove(&byte_count, &packet[0]+PACKET_DATA_OFFSET_BYTE_COUNT, sizeof(byte_count));
    memmove(&chunk_start, &packet[0]+PACKET_DATA_OFFSET_CHUNK_START, sizeof(chunk_start));
    memmove(chunk, &packet[0]+PACKET_DATA_OFFSET_CHUNK, byte_count);
}

void make_reqqueue_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string node_name)
{
    PACKET_TYPE type = salt_type(PACKET_REQQUEUE);

    packet.resize(PACKET_REQQUEUE_OFFSET_TOTAL);
    memset(&packet[0], 0, PACKET_REQQUEUE_OFFSET_TOTAL);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_REQQUEUE_OFFSET_NODE_ID, &node_id, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    memmove(&packet[0]+PACKET_REQQUEUE_OFFSET_NODE_NAME, node_name.c_str(), node_name.size());
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

//Function to extract necessary fileds from a received reqqueue packet
void extract_reqqueue(vector<PACKET_BYTE>& packet, packet_struct_reqqueue& reqqueue)
{
    memmove(&reqqueue.node_id, &packet[0]+PACKET_REQQUEUE_OFFSET_NODE_ID, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    memmove(&reqqueue.node_name, &packet[0]+PACKET_REQQUEUE_OFFSET_NODE_NAME, COSMOS_MAX_NAME);
}

void make_queue_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string node_name, vector<PACKET_TX_ID_TYPE> queue)
{
    PACKET_TYPE type = salt_type(PACKET_QUEUE);

    packet.resize(PACKET_QUEUE_OFFSET_TOTAL);
    memset(&packet[0], 0, PACKET_QUEUE_OFFSET_TOTAL);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_QUEUE_OFFSET_NODE_ID, &node_id, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    memmove(&packet[0]+PACKET_QUEUE_OFFSET_NODE_NAME, node_name.c_str(), node_name.size());
    memmove(&packet[0]+PACKET_QUEUE_OFFSET_TX_ID, &queue[0], COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

//Function to extract necessary fileds from a received queue packet
void extract_queue(vector<PACKET_BYTE>& packet, packet_struct_queue& queue)
{
    memmove(&queue.node_id, &packet[0]+PACKET_QUEUE_OFFSET_NODE_ID, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    memmove(&queue.node_name, &packet[0]+PACKET_QUEUE_OFFSET_NODE_NAME, COSMOS_MAX_NAME);
    memmove(&queue.tx_id, &packet[0]+PACKET_QUEUE_OFFSET_TX_ID, COSMOS_SIZEOF(PACKET_TX_ID_TYPE)*TRANSFER_QUEUE_LIMIT);
}

void make_heartbeat_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string node_name, PACKET_BYTE beat_period, PACKET_UNIXTIME_TYPE throughput, PACKET_UNIXTIME_TYPE funixtime)
{
    PACKET_TYPE type = salt_type(PACKET_HEARTBEAT);

    packet.resize(PACKET_HEARTBEAT_OFFSET_TOTAL);
    memset(&packet[0], 0, PACKET_HEARTBEAT_OFFSET_TOTAL);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_HEARTBEAT_OFFSET_NODE_ID, &node_id, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    memmove(&packet[0]+PACKET_HEARTBEAT_OFFSET_NODE_NAME, node_name.c_str(), node_name.size());
    memmove(&packet[0]+PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD, &beat_period, 1);
    memmove(&packet[0]+PACKET_HEARTBEAT_OFFSET_THROUGHPUT, &throughput, 4);
    memmove(&packet[0]+PACKET_HEARTBEAT_OFFSET_FUNIXTIME, &funixtime, 4);
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

//Function to extract necessary fileds from a received heartbeat packet
void extract_heartbeat(vector<PACKET_BYTE>& packet, packet_struct_heartbeat& heartbeat)
{
    memmove(&heartbeat.node_id, &packet[0]+PACKET_HEARTBEAT_OFFSET_NODE_ID, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    memmove(&heartbeat.node_name, &packet[0]+PACKET_HEARTBEAT_OFFSET_NODE_NAME, COSMOS_MAX_NAME);
    memmove(&heartbeat.beat_period, &packet[0]+PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD, 1);
    memmove(&heartbeat.throughput, &packet[0]+PACKET_HEARTBEAT_OFFSET_THROUGHPUT, 4);
    memmove(&heartbeat.funixtime, &packet[0]+PACKET_HEARTBEAT_OFFSET_FUNIXTIME, 4);
}

void make_message_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string message)
{
    PACKET_TYPE type = salt_type(PACKET_MESSAGE);

    packet.resize(PACKET_MESSAGE_OFFSET_TOTAL);
    memset(&packet[0], 0, PACKET_MESSAGE_OFFSET_TOTAL);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_TYPE, &type, sizeof(PACKET_TYPE));
    memmove(&packet[0]+PACKET_MESSAGE_OFFSET_NODE_ID, &node_id, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    PACKET_BYTE length = message.size();
    memmove(&packet[0]+PACKET_MESSAGE_OFFSET_LENGTH, &length, 1);
    memmove(&packet[0]+PACKET_MESSAGE_OFFSET_BYTES, &message[0], TRANSFER_MAX_PROTOCOL_PACKET - 2);
    uint16_t crc = calc_crc16ccitt(&packet[3], packet.size()-3);
    memmove(&packet[0]+PACKET_HEADER_OFFSET_CRC, &crc, sizeof(PACKET_CRC));
}

//Function to extract necessary fileds from a received message packet
void extract_message(vector<PACKET_BYTE>& packet, packet_struct_message& message)
{
    memmove(&message.node_id, &packet[0]+PACKET_MESSAGE_OFFSET_NODE_ID, COSMOS_SIZEOF(PACKET_NODE_ID_TYPE));
    memmove(&message.length, &packet[0]+PACKET_MESSAGE_OFFSET_LENGTH, 1);
    memmove(&message.bytes[0], &packet[0]+PACKET_MESSAGE_OFFSET_BYTES, message.length);
}

void show_fstream_state(std::ifstream& )  {
    std::cout<<"eobit =\t"<<std::ios_base::eofbit<<std::endl;
    std::cout<<"failbit =\t"<<std::ios_base::failbit<<std::endl;
    std::cout<<"badbit =\t"<<std::ios_base::badbit<<std::endl;
    std::cout<<"goodbit =\t"<<std::ios_base::goodbit<<std::endl;
    return;
}

// Function to get age of a file in seconds
time_t get_file_age(std::string filename)
{
    struct stat stat_buf;
    if (stat(filename.c_str(), &stat_buf) != 0)
    {
        return 0;
    }
    else
    {
        struct timeval cmjd = utc2unix(currentmjd(0.));
        return (cmjd.tv_sec-stat_buf.st_mtime);
    }
}

//Function which gets the size of a file
//! Get size of file
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
    std::string sfilename = filename;
    return get_file_size(sfilename);
}

void print_cstring(uint8_t* buf, int siz)
{
    for(int i=0; i<siz; ++i)
        printf("%c", buf[i]);
    printf("\n");
    printf("\n");
    return;
}

void print_cstring_with_index(uint8_t* buf, int siz)
{
    using std::min;
    int start = 0;
    int linesize = 40;
    while(start < siz)	{
        for(int i=start; i<min(start+linesize,siz); ++i)
            printf("%2c", buf[i]);
        printf("\n");
        for(int i=start; i<min(start+linesize,siz); ++i)
            printf("%02d", i%100);
        printf("\n");
        printf("\n");

        start+=linesize;
    }
    return;
}

void print_cstring_hex(uint8_t* buf, int siz)
{
    for(int i=0; i<siz; ++i)
        printf("%02x", buf[i]);
    printf("\n");
    printf("\n");
    return;
}

void print_cstring_hex_with_index(uint8_t* buf, int siz)
{
    using std::min;
    int start = 0;
    int linesize = 40;
    while(start < siz)	{
        for(int i=start; i<min(start+linesize,siz); ++i)
            printf("%02x", buf[i]);
        printf("\n");
        for(int i=start; i<min(start+linesize,siz); ++i)
            printf("%02d", i%100);
        printf("\n");
        printf("\n");

        start+=linesize;
    }
    return;
}

void unable_to_remove(std::string filename)  {
    std::cout<<"ERROR:\tunable to remove file <"<<filename<<">"<<std::endl;
    return;
}

PACKET_TYPE salt_type(PACKET_TYPE type)
{
    double cmjd = currentmjd();
    uint8_t time_salt = (uint32_t)((cmjd-(int)cmjd) * 86400.) % 16;
    type &= 0xf;
    type |= time_salt << 4;
    return type;
}

int32_t check_node_id(PACKET_NODE_ID_TYPE node_id)
{
    int32_t iretn;

    if ((iretn=load_nodeids()) <= 0)
    {
        return iretn;
    }


    if (node_id > 0 && nodeids[node_id].size())
    {
        return node_id;
    }
    else
    {
        return 0;
    }
}

int32_t lookup_node_id(string node_name)
{
    int32_t iretn;

    if ((iretn=load_nodeids()) <= 0)
    {
        return iretn;
    }

    uint8_t node_id = 0;
    for (uint8_t i=1; i<nodeids.size(); ++i)
    {
        if (nodeids[i] == node_name)
        {
            node_id = i;
            break;
        }
    }

    return node_id;
}

string lookup_node_id_name(PACKET_NODE_ID_TYPE node_id)
{
    string name;
    if (load_nodeids() > 0 && node_id > 0 && nodeids[node_id].size())
    {
        return nodeids[node_id];
    }
    else
    {
        return "";
    }
}

int32_t load_nodeids()
{
    //    int32_t iretn;
    //    char name[100];
    char buf[103];
    if (nodeids.size() == 0)
    {
        FILE *fp = data_open(get_cosmosnodes()+"/nodeids.ini", "rb");
        if (fp)
        {
            nodeids.resize(256);
            uint16_t index;
            while (fgets(buf, 102, fp) != nullptr)
            {
                if (buf[strlen(buf)-1] == '\n')
                {
                    buf[strlen(buf)-1] = 0;
                }
                if (buf[1] == ' ')
                {
                    buf[1] = 0;
                    index = atoi(buf);
                    nodeids[index] = &buf[2];
                }
                else if (buf[2] == ' ')
                {
                    buf[2] = 0;
                    index = atoi(buf);
                    nodeids[index] = &buf[3];
                }
                else if (buf[3] == ' ')
                {
                    buf[3] = 0;
                    index = atoi(buf);
                    nodeids[index] = &buf[4];
                }
                else
                {
                    index = 0;
                }
                //                sscanf(buf, "%u %s\n", &index, name);
                //                nodeids[index] = name;
            }
        }
        else
        {
            return -errno;
        }
    }

    return nodeids.size();
}
