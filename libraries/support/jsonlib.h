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

#ifndef _JSONLIB_H
#define _JSONLIB_H 1

/*! \file jsonlib.h
* \brief JSON Support
*/

//! \ingroup support
//! \defgroup jsonlib JSON Library
//! A library providing functions in support of the COSMOS specific JSON format and
//! the associated Nodal Namespace concept.
//!
//! COSMOS JSON is a special application of JavaScript Object Notation. It has been
//! designed to fill the needs of COSMOS for a mechanism by which it can share data
//! between the various Agent and Programs, both internally, and cross platform.
//!
//! COSMOS JSON is defined by the following key elements:
//! - implements all elements of JSON
//! - defines base types of double, float, int32, uint32, int16, uint16 and string
//! - allows for the creation of complex types as combinations of base types
//! - defines a unified Name Space, in which each name is tied to a base or complex data
//! type.
//!
//! The \ref jsonlib_functions provide mechanisms for mapping names from the \ref jsonlib_namespace to
//! internal data structures. JSON strings can then be parsed directly in to memory.
//! Similarly, JSON strings can be created directly from memory.
//!
//! The COSMOS Nodal Namespace is the unique application of COSMOS JSON to manage data
//! within COSMOS.
//!
//! The Node is the highest level physical entity in the COSMOS system. Every COSMOS
//! installation will consist of a number of Nodes, all interacting through their Agents
//! (as described in the \ref agentlib). Each Node has a detailed description, stored both as
//! structures in memory, and as elements of the \ref jsonlib_namespace. These descriptions
//! are broadly separated into the following categories:
//! - Node: summary information about the Node, including counts of various other categories
//! - Piece: physical pieces in Node
//! - Device General: that subset of pieces that have some electronic nature
//! - Device Specific: the subset of General Devices that have qualities unique to that specific device
//!
//! Every physical thing in COSMOS is part of a Node. All Nodal actions
//! are implemented through Agents, as described in the \ref agentlib.
//! Interactions between Nodes are always expressed as interactions
//! between Node:Agent pairings. Thus, Messages and Requests will be
//! addressed to names expressed as Node:Agent (though wildcards are in
//! some case supported eg. Node:* or *:Agent).
//!
//! Each Node will also have a number of associated tables and/or dictionaries. These will be lists
//! of either straight JSON, providing additional information about the Node, or JSON Equations, providing
//! conditional information about the Node.
//!
//! The first type currently supports:
//! - ports.ini: information about the specific port for any device that requires one
//! - target.ini: a list of other locations of possible interest, to be used with targetting
//! - state.ini: the most recent state vector for the Node
//! - node_utcstart.ini: the mission start time
//!
//! The second type currently supports:
//! - events.dict: table of Physical Events, to be matched against current data to determine events.
//! - commands.dict: table of Command Events
//!
//! Targetting
//!
//! Information about alternate targets is supported in memory through the use of a table of type
//! ::targetstruc, stored within the ::CosmosClass. These values can be accessed externally through
//! names of type "target_*". This table is initialized using
//! ::load_target, and updated with values in the current ::CosmosClass using ::update_target. Information
//! calculated includes the Azimuth and Elevation to and from each target, as well as its range.
//!
//! Events
//!
//! Unlike Targetting, information about Physical Events is not stored directly in the ::CosmosClass. Instead,
//! the user keeps a table of type ::shorteventstruc, initialized with ::load_dictionary, that provides templates
//! for all possible events. These templates can be matched against current conditions using ::calc_events.
//! This will return a second table of type ::shorteventstruc that includes an entry for each event that:
//! - had its condition calculate non zero
//! - did not have its condition calculate non zero previously
//!

//#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "support/configCosmos.h"

#include "support/jsondef.h"
//#include "support/datalib.h"
#include "support/stringlib.h"
#include <limits>
#include <string>
using std::numeric_limits;
using std::string;

enum class HandlerDataType : uint16_t{
	INTEIGHT=1,
	INTSIXTEEN=2,
	INTTHIRTYTWO=3,
	UINTEIGHT=4,
	UINTSIXTEEN=5,
	UINTTHIRTYTWO=6,
	FLOAT=7,
	DOUBLE=8,
	// STRING=9,
	RVECTOR=10
	
};

typedef struct handlerstruc{
	string name;
    HandlerDataType datatype;
	void* value;	
	
}handlerstruc;

class cosmoshandler{
	public:
	vector<handlerstruc> entries;
    vector<handlerstruc> beats;
	
	const char * json_out(string &jstring);		//cyt: use case:switch to call json_out with correct pointer type
	const char * json_beat_out(string &jstring);//cyt: use to create beat or use to set a vector that contains the beat entries
	
	
};


//! \ingroup jsonlib
//! \defgroup jsonlib_functions JSON functions
//! @{

int32_t json_out_entry(string &jstring, handlerstruc entry);
int32_t json_out_int8(string &jstring, void* value);
int32_t json_out_int16(string &jstring, void* value);
int32_t json_out_int32(string &jstring, void* value);
int32_t json_out_uint8(string &jstring, void* value);
int32_t json_out_uint16(string &jstring, void* value);
int32_t json_out_uint32(string &jstring, void* value);
int32_t json_out_float(string &jstring, void* value);
int32_t json_out_double(string &jstring, void* value);
int32_t json_out_rvector(string &jstring, void* value);

int32_t json_create(CosmosClass* cinfo);
//void json_destroy(CosmosClass *cinfo);

int32_t json_append(string &jstring, const char *tstring);
int32_t json_out_character(string &jstring,char character);
int32_t json_out_name(string &jstring, string name);
int32_t json_out_int8(string &jstring,int8_t value);
int32_t json_out_int16(string &jstring,int16_t value);
int32_t json_out_int32(string &jstring,int32_t value);
int32_t json_out_uint8(string &jstring, uint8_t value);
int32_t json_out_uint16(string &jstring,uint16_t value);
int32_t json_out_uint32(string &jstring,uint32_t value);
int32_t json_out_float(string &jstring,float value);
int32_t json_out_double(string &jstring,double value);
int32_t json_out_string(string &jstring, string ostring, uint16_t len);
int32_t json_out_gvector(string &jstring,gvector value);
int32_t json_out_svector(string &jstring,svector value);
int32_t json_out_avector(string &jstring, avector value);
int32_t json_out_vector(string &jstring,Vector value);
int32_t json_out_rvector(string &jstring,rvector value);
int32_t json_out_quaternion(string &jstring,quaternion value);
int32_t json_out_cvector(string &jstring,cvector value);
int32_t json_out_cartpos(string &jstring,cartpos value);
int32_t json_out_geoidpos(string &jstring,geoidpos value);
int32_t json_out_spherpos(string &jstring,spherpos value);
int32_t json_out_node(string &jstring, string value);
int32_t json_out_utcstart(string &jstring, double value);
int32_t json_out_locstruc(string &jstring,locstruc value);
//int32_t json_out_commandevent(string &jstring,longeventstruc value);
//int32_t json_out_beatstruc(string &jstring,beatstruc value);
string json_extract_namedobject(string json, string token);
int32_t json_extract_value(const char *&ptr, string &value);
string json_convert_string(string object);
double json_convert_double(string object);
//int32_t json_tokenize(string jstring, CosmosClass *cinfo, vector<jsontoken> &tokens);
//int32_t json_tokenize_namedobject(const char* &ptr, CosmosClass *cinfo, jsontoken &token);
//int32_t json_parse(string jstring, CosmosClass *cinfo);
//int32_t json_parse_namedobject(const char* &ptr, CosmosClass *cinfo);
int32_t json_skip_character(const char* &ptr, const char character);
int32_t json_parse_name(const char* &ptr, string& ostring);
int32_t json_extract_string(const char* &ptr, string &ostring);
int32_t json_parse_number(const char* &ptr, double *number);
int32_t json_skip_white(const char* &ptr);
int32_t json_skip_value(const char* &ptr);


//! @}

#endif
