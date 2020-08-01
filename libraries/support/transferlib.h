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

#ifndef FILETRANSFER_H
#define FILETRANSFER_H

/*! \file transferlib.h
*	\brief File Transfer Support header file
*/

//! \ingroup support
//! \defgroup transferlib File Transfer Library
//! File Transfer Protocol.
//!
//! COSMOS provides a basic File Transfer Protocol for direct transfer of
//! files between Nodes and Agents. Each file transferred is targeted at
//! a specific Node and Agent, and always flows from the outgoing folder
//! at the sending location to the incoming folder of the receiving location.
//!
//! The protocol is designed to be robust in the face of intermittent connections,
//! and to work within a network MTU of 250 bytes. Transfers are supported through
//! the exchange of 8 packet types. Each packet starts with 1 byte indicating the
//! type and 1 byte representing a Transaction Id. The packets differ from each
//! other in the following way:
//! - PACKET_METADATA: file_name, file_size, node_name, agent_name
//! - PACKET_DATA: byte_count, chunk_start, chunk_data
//! - PACKET_REQDATA: hole_start, hole_end
//! - PACKET_REQMETA:
//! - PACKET_COMPLETE:
//! - PACKET_CANCEL:
//! - PACKET_QUEUE: node_name,tx_ids[]

// cosmos includes
#include "support/configCosmos.h"
#include "support/datalib.h"

// c++ includes
//#include <cstring>
//#include <algorithm>
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <sys/stat.h>

//! \ingroup transferlib
//! \defgroup transferlib_constants File Transfer constants
//! @{

#define PACKET_MAX_LENGTH 60000
// Radios are only allowing 253 byte packet. IP/UDP header is 28 bytes.
#define TRANSFER_MAX_PROTOCOL_PACKET 225
#define TRANSFER_MAX_FILENAME 128
#ifndef COSMOS_WIN_BUILD_MSVC
#define TRANSFER_QUEUE_LIMIT ((TRANSFER_MAX_PROTOCOL_PACKET-(COSMOS_SIZEOF(PACKET_TYPE)+COSMOS_SIZEOF(PACKET_NODE_ID_TYPE)+COSMOS_SIZEOF(PACKET_TX_ID_TYPE)+COSMOS_MAX_NAME))/COSMOS_SIZEOF(PACKET_TX_ID_TYPE))
#else
#define TRANSFER_QUEUE_LIMIT 100
#endif
//#define MESSAGE_MAX_PACKET (TRANSFER_MAX_PROTOCOL_PACKET-(COSMOS_SIZEOF(PACKET_TYPE)+COSMOS_SIZEOF(PACKET_TX_ID_TYPE)))

namespace STATE_TYPE
{
    static const unsigned char STATE_NONE = 0x00;
    static const unsigned char STATE_REQQUEUE = 0x01;
    static const unsigned char STATE_QUEUE = 0x02;
    static const unsigned char STATE_REQMETA = 0x03;
    static const unsigned char STATE_METADATA = 0x04;
    static const unsigned char STATE_REQDATA = 0x05;
    static const unsigned char STATE_DATA = 0x06;
    static const unsigned char STATE_CANCEL = 0x07;
    static const unsigned char STATE_COMPLETE = 0x08;
    static const unsigned char STATE_SIZE = 0x09;
}
using namespace STATE_TYPE;

namespace PACKET_TYPE_STUFF	{
    //these bits reserved for PACKET_TYPES
    static const unsigned char PACKET_METADATA = 0xf;
    static const unsigned char PACKET_DATA = 0xe;
    static const unsigned char PACKET_REQDATA =	0xd;
    static const unsigned char PACKET_REQMETA =	0xc;
    static const unsigned char PACKET_COMPLETE = 0xb;
    static const unsigned char PACKET_CANCEL = 0xa;
    static const unsigned char PACKET_QUEUE = 0x9;
    static const unsigned char PACKET_REQQUEUE = 0x8;
    static const unsigned char PACKET_HEARTBEAT = 0x7;
    static const unsigned char PACKET_MESSAGE = 0x6;
}

using namespace PACKET_TYPE_STUFF;

namespace STATUS_TYPE	{
    //these bits reserved for PACKET_TYPES
    static const unsigned char STATUS_ALL_GOOD		= 128;  //10000000
    static const unsigned char STATUS_BAD_TX_ID		=  64;  //01000000
    static const unsigned char STATUS_NO_FILE_FOUND =  32;  //00100000
    static const unsigned char STATUS_HOLES_TO_FILL =  16;  //00010000
    static const unsigned char STATUS_SUCCESS		=   8;  //00001000
    static const unsigned char STATUS_NEED_METADATA =   4;	//00000100

    //these bits reserved for future STATUS_TYPES
    //static const unsigned char STATUS_1 =	4;	  //00000100
    static const unsigned char STATUS_2 =	2;	  //00000010
    static const unsigned char STATUS_3 =	 1;	  //00000001
}

using namespace STATUS_TYPE;
//! @}

//! \ingroup transferlib
//! \defgroup transferlib_typedefs File Transfer typedefs
//! @{

typedef uint8_t PACKET_BYTE;
typedef uint8_t PACKET_TYPE;
typedef uint16_t PACKET_CRC;
typedef uint8_t PACKET_NODE_ID_TYPE;
typedef uint8_t PACKET_TX_ID_TYPE;
typedef uint16_t PACKET_CHUNK_SIZE_TYPE;
typedef int32_t PACKET_FILE_SIZE_TYPE;
typedef uint32_t PACKET_UNIXTIME_TYPE;

#define PACKET_HEADER_OFFSET_TYPE 0
#define PACKET_HEADER_OFFSET_CRC (PACKET_HEADER_OFFSET_TYPE + COSMOS_SIZEOF(PACKET_TYPE))
#define PACKET_HEADER_OFFSET_TOTAL (PACKET_HEADER_OFFSET_CRC + COSMOS_SIZEOF(PACKET_CRC))
#define PACKET_HEADER_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_HEADER_OFFSET_NODE_NAME (PACKET_HEADER_OFFSET_TOTAL + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    char node_name[COSMOS_MAX_NAME+1];
    PACKET_BYTE beat_period;
    PACKET_UNIXTIME_TYPE throughput;
    PACKET_UNIXTIME_TYPE funixtime;
} packet_struct_heartbeat;

#define PACKET_HEARTBEAT_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_HEARTBEAT_OFFSET_NODE_NAME (PACKET_HEADER_OFFSET_NODE_NAME)
#define PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD (PACKET_HEARTBEAT_OFFSET_NODE_NAME + COSMOS_MAX_NAME)
#define PACKET_HEARTBEAT_OFFSET_THROUGHPUT (PACKET_HEARTBEAT_OFFSET_BEAT_PERIOD + 1)
#define PACKET_HEARTBEAT_OFFSET_FUNIXTIME (PACKET_HEARTBEAT_OFFSET_THROUGHPUT + 4)
#define PACKET_HEARTBEAT_OFFSET_TOTAL (PACKET_HEARTBEAT_OFFSET_FUNIXTIME + 8)

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    PACKET_BYTE length;
    PACKET_BYTE bytes[TRANSFER_MAX_PROTOCOL_PACKET-2];
} packet_struct_message;

#define PACKET_MESSAGE_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_MESSAGE_OFFSET_LENGTH (PACKET_MESSAGE_OFFSET_NODE_ID + 1)
#define PACKET_MESSAGE_OFFSET_BYTES (PACKET_MESSAGE_OFFSET_LENGTH + 1)
#define PACKET_MESSAGE_OFFSET_TOTAL (PACKET_MESSAGE_OFFSET_BYTES + TRANSFER_MAX_PROTOCOL_PACKET - 2)

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    char node_name[COSMOS_MAX_NAME+1];
} packet_struct_reqqueue;

#define PACKET_REQQUEUE_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_REQQUEUE_OFFSET_NODE_NAME (PACKET_HEADER_OFFSET_NODE_NAME)
#define PACKET_REQQUEUE_OFFSET_TOTAL (PACKET_REQQUEUE_OFFSET_NODE_ID + COSMOS_MAX_NAME)

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    char node_name[COSMOS_MAX_NAME+1];
    PACKET_TX_ID_TYPE tx_id[TRANSFER_QUEUE_LIMIT];
} packet_struct_queue;

#define PACKET_QUEUE_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_QUEUE_OFFSET_NODE_NAME (PACKET_HEADER_OFFSET_NODE_NAME)
#define PACKET_QUEUE_OFFSET_TX_ID (PACKET_QUEUE_OFFSET_NODE_NAME + COSMOS_MAX_NAME)
#define PACKET_QUEUE_OFFSET_TOTAL (PACKET_QUEUE_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE) * TRANSFER_QUEUE_LIMIT)

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    char node_name[COSMOS_MAX_NAME+1];
    PACKET_TX_ID_TYPE tx_id[TRANSFER_QUEUE_LIMIT];
} packet_struct_reqmeta;

#define PACKET_REQMETA_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_REQMETA_OFFSET_NODE_NAME (PACKET_HEADER_OFFSET_NODE_NAME)
#define PACKET_REQMETA_OFFSET_TX_ID (PACKET_REQMETA_OFFSET_NODE_NAME + COSMOS_MAX_NAME)
#define PACKET_REQMETA_OFFSET_TOTAL (PACKET_REQMETA_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE) * TRANSFER_QUEUE_LIMIT)

typedef struct
{
    char node_name[COSMOS_MAX_NAME+1];
    PACKET_TX_ID_TYPE tx_id;
    char agent_name[COSMOS_MAX_NAME+1];
    char file_name[TRANSFER_MAX_FILENAME];
    PACKET_FILE_SIZE_TYPE file_size;
} packet_struct_metalong;

#define PACKET_METALONG_OFFSET_NODE_NAME (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_METALONG_OFFSET_TX_ID (PACKET_METALONG_OFFSET_NODE_NAME + COSMOS_MAX_NAME)
#define PACKET_METALONG_OFFSET_AGENT_NAME (PACKET_METALONG_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))
#define PACKET_METALONG_OFFSET_FILE_NAME (PACKET_METALONG_OFFSET_AGENT_NAME + COSMOS_MAX_NAME)
#define PACKET_METALONG_OFFSET_FILE_SIZE (PACKET_METALONG_OFFSET_FILE_NAME + TRANSFER_MAX_FILENAME)
#define PACKET_METALONG_OFFSET_TOTAL (PACKET_METALONG_OFFSET_FILE_SIZE + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    PACKET_TX_ID_TYPE tx_id;
    char agent_name[COSMOS_MAX_NAME+1];
    char file_name[TRANSFER_MAX_FILENAME];
    PACKET_FILE_SIZE_TYPE file_size;
} packet_struct_metashort;

#define PACKET_METASHORT_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_METASHORT_OFFSET_TX_ID (PACKET_METASHORT_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
#define PACKET_METASHORT_OFFSET_AGENT_NAME (PACKET_METASHORT_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))
#define PACKET_METASHORT_OFFSET_FILE_NAME (PACKET_METASHORT_OFFSET_AGENT_NAME + COSMOS_MAX_NAME)
#define PACKET_METASHORT_OFFSET_FILE_SIZE (PACKET_METASHORT_OFFSET_FILE_NAME + TRANSFER_MAX_FILENAME)
#define PACKET_METASHORT_OFFSET_TOTAL (PACKET_METASHORT_OFFSET_FILE_SIZE + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    PACKET_TX_ID_TYPE tx_id;
    PACKET_FILE_SIZE_TYPE hole_start;
    PACKET_FILE_SIZE_TYPE hole_end;
} packet_struct_reqdata;

#define PACKET_REQDATA_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_REQDATA_OFFSET_TX_ID (PACKET_REQDATA_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
#define PACKET_REQDATA_OFFSET_HOLE_START (PACKET_REQDATA_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))
#define PACKET_REQDATA_OFFSET_HOLE_END (PACKET_REQDATA_OFFSET_HOLE_START + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))
#define PACKET_REQDATA_OFFSET_TOTAL (PACKET_REQDATA_OFFSET_HOLE_END + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    PACKET_TX_ID_TYPE tx_id;
    PACKET_CHUNK_SIZE_TYPE byte_count;
    PACKET_FILE_SIZE_TYPE chunk_start;
    PACKET_BYTE chunk[PACKET_MAX_LENGTH];
} packet_struct_data;

#define PACKET_DATA_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_DATA_OFFSET_TX_ID (PACKET_DATA_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
#define PACKET_DATA_OFFSET_BYTE_COUNT (PACKET_DATA_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))
#define PACKET_DATA_OFFSET_CHUNK_START (PACKET_DATA_OFFSET_BYTE_COUNT + COSMOS_SIZEOF(PACKET_CHUNK_SIZE_TYPE))
#define PACKET_DATA_OFFSET_CHUNK (PACKET_DATA_OFFSET_CHUNK_START + COSMOS_SIZEOF(PACKET_FILE_SIZE_TYPE))
#define PACKET_DATA_OFFSET_HEADER_TOTAL (PACKET_DATA_OFFSET_CHUNK)

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    PACKET_TX_ID_TYPE tx_id;
} packet_struct_complete;

#define PACKET_COMPLETE_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_COMPLETE_OFFSET_TX_ID (PACKET_COMPLETE_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
#define PACKET_COMPLETE_OFFSET_TOTAL (PACKET_COMPLETE_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    PACKET_TX_ID_TYPE tx_id;
} packet_struct_cancel;

#define PACKET_CANCEL_OFFSET_NODE_ID (PACKET_HEADER_OFFSET_TOTAL)
#define PACKET_CANCEL_OFFSET_TX_ID (PACKET_CANCEL_OFFSET_NODE_ID + COSMOS_SIZEOF(PACKET_NODE_ID_TYPE))
#define PACKET_CANCEL_OFFSET_TOTAL (PACKET_CANCEL_OFFSET_TX_ID + COSMOS_SIZEOF(PACKET_TX_ID_TYPE))

typedef struct
{
    PACKET_NODE_ID_TYPE node_id;
    PACKET_TX_ID_TYPE tx_id;
} packet_struct_raw;

typedef union
{
    packet_struct_heartbeat heartbeat;
    packet_struct_reqqueue reqqueue;
    packet_struct_queue queue;
    packet_struct_complete complete;
    packet_struct_cancel cancel;
    packet_struct_data data;
    packet_struct_metalong metalong;
    packet_struct_metashort metashort;
    packet_struct_reqdata reqdata;
    packet_struct_reqmeta reqmeta;
    packet_struct_raw raw;
} packet_struct_all;

typedef struct
{
    PACKET_FILE_SIZE_TYPE	chunk_start;
    PACKET_FILE_SIZE_TYPE	chunk_end;
} file_progress;

typedef struct
{
    PACKET_TX_ID_TYPE tx_id=0;
    bool havemeta;
    bool sendmeta;
    bool sentmeta;
    bool senddata;
    bool sentdata;
    bool complete;
    std::string node_name="";
    std::string agent_name="";
    std::string	file_name="";
    std::string filepath="";
    std::string temppath="";
    double savetime;
    double datatime=0.;
    PACKET_FILE_SIZE_TYPE file_size;
    PACKET_FILE_SIZE_TYPE total_bytes;
    deque<file_progress> file_info;
    FILE * fp;
} tx_progress;

//! @}

//! \ingroup transferlib
//! \defgroup transferlib_functions File Transfer functions
//! @{

//functions to verify whether a packet is a particular type
bool IS_MESSAGE(const unsigned char);
bool IS_REQUEST(const unsigned char);
bool IS_METADATA(const unsigned char);
bool IS_DATA(const unsigned char);
bool IS_REQDATA(const unsigned char);
bool IS_REQMETA(const unsigned char);
bool IS_COMPLETE(const unsigned char);

//functions to verify whether a request is of a particular type
bool IS_GET_REQUEST(const unsigned char);
bool IS_PUT_REQUEST(const unsigned char);
bool IS_DELETE_REQUEST(const unsigned char);

void make_metadata_packet(vector<PACKET_BYTE>& packet, packet_struct_metalong meta);
void make_metadata_packet(vector<PACKET_BYTE>& packet, PACKET_TX_ID_TYPE tx_id, char* file_name, PACKET_FILE_SIZE_TYPE file_size, char* node_name, char* agent_name);
void make_metadata_packet(vector<PACKET_BYTE>& packet, packet_struct_metashort meta);
void make_metadata_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, char* file_name, PACKET_FILE_SIZE_TYPE file_size, char* agent_name);
void extract_metadata(vector<PACKET_BYTE>& packet, packet_struct_metalong& meta);
void extract_metadata(vector<PACKET_BYTE>& packet, PACKET_TX_ID_TYPE& tx_id, char* file_name, PACKET_FILE_SIZE_TYPE& file_size, char* node_name, char* agent_name);
void extract_metadata(vector<PACKET_BYTE>& packet, packet_struct_metashort& meta);
void extract_metadata(vector<PACKET_BYTE>& packet, PACKET_TX_ID_TYPE& tx_id, char* file_name, PACKET_FILE_SIZE_TYPE& file_size, PACKET_NODE_ID_TYPE& node_id, char* agent_name);

void make_data_packet(vector<PACKET_BYTE>& packet, packet_struct_data data);
void make_data_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_CHUNK_SIZE_TYPE byte_count, PACKET_FILE_SIZE_TYPE chunk_start, PACKET_BYTE* chunk);
void extract_data(vector<PACKET_BYTE>& packet, packet_struct_data& data);
void extract_data(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE& node_id, PACKET_TX_ID_TYPE& tx_id, PACKET_CHUNK_SIZE_TYPE& byte_count, PACKET_FILE_SIZE_TYPE& chunk_start, PACKET_BYTE* chunk);

void make_reqdata_packet(vector<PACKET_BYTE>& packet, packet_struct_reqdata reqdata);
void make_reqdata_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id, PACKET_FILE_SIZE_TYPE hole_start, PACKET_FILE_SIZE_TYPE hole_end);
void extract_reqdata(vector<PACKET_BYTE>& packet, packet_struct_reqdata& reqdata);
void extract_reqdata(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE &node_id, PACKET_TX_ID_TYPE& tx_id, PACKET_FILE_SIZE_TYPE& hole_start, PACKET_FILE_SIZE_TYPE& hole_end);

void make_reqmeta_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string node_name, vector<PACKET_TX_ID_TYPE> reqmeta);
void extract_reqmeta(vector<PACKET_BYTE>& packet, packet_struct_reqmeta& reqmeta);

void make_complete_packet(vector<PACKET_BYTE>& packet, packet_struct_complete complete);
void make_complete_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id);
void extract_complete(vector<PACKET_BYTE>& packet, packet_struct_complete& complete);
void extract_complete(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE &node_id, PACKET_TX_ID_TYPE& tx_id);

void make_cancel_packet(vector<PACKET_BYTE>& packet, packet_struct_cancel cancel);
void make_cancel_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, PACKET_TX_ID_TYPE tx_id);
void extract_cancel(vector<PACKET_BYTE>& packet, packet_struct_cancel& cancel);
void extract_cancel(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE &node_id, PACKET_TX_ID_TYPE& tx_id);

void make_reqqueue_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string node_name);
void extract_reqqueue(vector<PACKET_BYTE>& packet, packet_struct_reqqueue& reqqueue);

void make_queue_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string node_name, vector<PACKET_TX_ID_TYPE> queue);
void extract_queue(vector<PACKET_BYTE>& packet, packet_struct_queue& queue);

void make_heartbeat_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string node_name, uint8_t beat_period, uint32_t throughput, uint32_t funixtime);
void extract_heartbeat(vector<PACKET_BYTE>& packet, packet_struct_heartbeat& heartbeat);

void make_message_packet(vector<PACKET_BYTE>& packet, PACKET_NODE_ID_TYPE node_id, std::string message);
void extract_message(vector<PACKET_BYTE>& packet, packet_struct_message& message);

int32_t check_node_id(PACKET_NODE_ID_TYPE node_id);
//int32_t lookup_node_id(PACKET_NODE_ID_TYPE node_id);
int32_t lookup_node_id(string node_name);
int32_t set_node_id(PACKET_NODE_ID_TYPE node_id, string node_name);
string lookup_node_id_name(PACKET_NODE_ID_TYPE node_id);
int32_t load_nodeids();

//void make_message_packet(vector<PACKET_BYTE>& packet, packet_struct_message message);
//void make_message_packet(vector<PACKET_BYTE>& packet, PACKET_TX_ID_TYPE tx_id);
//void extract_message(vector<PACKET_BYTE>& packet, packet_struct_message& message);
//void extract_message(vector<PACKET_BYTE>& packet, PACKET_TX_ID_TYPE& tx_id);

//void make_send_packet(vector<PACKET_BYTE>& packet, PACKET_TX_ID_TYPE tx_id);
//void extract_send(vector<PACKET_BYTE>& packet, PACKET_TX_ID_TYPE &tx_id);


///  --------------- code above this line has be checked and de-Jonny-fied

//Function to extract necessary fields from a received request packet
/*
void extract_request(uint8_t* packet, PACKET_TX_ID_TYPE tx_id, char* filename, char* to_node, char* to_agent, char* from_node, char* from_agent);
*/


void show_fstream_state(std::ifstream& out);
//Function which gets the size of a file
int32_t get_file_size(std::string filename);
int32_t get_file_size(const char* filename);

void print_cstring(uint8_t* buf, int siz);
void print_cstring_with_index(uint8_t* buf, int siz);
void print_cstring_hex(uint8_t* buf, int siz);
void print_cstring_hex_with_index(uint8_t* buf, int siz);

void unable_to_remove(std::string filename);
PACKET_TYPE salt_type(PACKET_TYPE type);
//! @}
#endif
