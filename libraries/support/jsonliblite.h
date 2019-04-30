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
//! ::targetstruc, stored within the ::cosmosclass. These values can be accessed externally through
//! names of type "target_*". This table is initialized using
//! ::load_target, and updated with values in the current ::cosmosclass using ::update_target. Information
//! calculated includes the Azimuth and Elevation to and from each target, as well as its range.
//!
//! Events
//!
//! Unlike Targetting, information about Physical Events is not stored directly in the ::cosmosclass. Instead,
//! the user keeps a table of type ::shorteventstruc, initialized with ::load_dictionary, that provides templates
//! for all possible events. These templates can be matched against current conditions using ::calc_events.
//! This will return a second table of type ::shorteventstruc that includes an entry for each event that:
//! - had its condition calculate non zero
//! - did not have its condition calculate non zero previously
//!

//#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "support/configCosmos.h"

#include "support/jsondeflite.h"
#include "support/datalib.h"
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

cosmosclass *json_create();
int32_t json_clone(cosmosclass *cinfo);
int32_t json_clone(cosmosclass *cinfo1, cosmosclass *cinfo2);
int32_t json_repoint(cosmosclass *cinfo);
void json_destroy(cosmosclass *cinfo);
//int32_t json_pushdevspec(uint16_t cidx, cosmosclass *cinfo);
      //cyt: section built on jsonnode/jsonentries
//uint16_t json_addequation(const char *text, cosmosclass *cinfo, uint16_t unit);
int32_t json_addpiece(cosmosclass *cinfo, string name, uint16_t type=PIECE_TYPE_DIMENSIONLESS, uint16_t cidx=UINT16_MAX, double emi=.8, double abs=.88, double hcap=800., double hcon=237., double density=1000.);
int32_t json_addentry(string name, string value, cosmosclass *cinfo);
int32_t json_addentry(jsonentry entry, cosmosclass *cinfo);
int32_t json_addentry(string name, uint16_t d1, uint16_t d2, ptrdiff_t offset, uint16_t type, uint16_t group, cosmosclass *cinfo, uint16_t unit=0);
int32_t json_addentry(string name, uint16_t d1, uint16_t d2, uint8_t *ptr, uint16_t type, cosmosclass *cinfo, uint16_t unit=0);
//int32_t json_addentry(string name, uint16_t d1, uint16_t d2, const cosmosdatastrucVoid &ptr, uint16_t type, cosmosclass *cinfo, uint16_t unit=0);
int32_t json_toggleentry(string name, uint16_t d1, uint16_t d2, cosmosclass *cinfo, bool state);
bool json_checkentry(string name, uint16_t d1, uint16_t d2, cosmosclass *cinfo);
int32_t json_mapentries(cosmosclass *cinfo);
int32_t json_mapbaseentries(cosmosclass *cinfo);
int32_t json_mappieceentry(uint16_t pidx, cosmosclass *cinfo);
int32_t json_togglepieceentry(uint16_t pidx, cosmosclass *cinfo, bool state);
int32_t json_mapcompentry(uint16_t cidx, cosmosclass *cinfo);
int32_t json_togglecompentry(uint16_t cidx, cosmosclass *cinfo, bool state);
uint16_t json_mapdeviceentry(const devicestruc &device, cosmosclass *cinfo);
int32_t json_toggledeviceentry(uint16_t didx, uint16_t type, cosmosclass *cinfo, bool state);
uint16_t json_mapportentry(uint16_t portidx, cosmosclass *cinfo);
int32_t json_toggleportentry(uint16_t portidx, cosmosclass *cinfo, bool state);
int32_t json_mapvertexentry(uint16_t vidx, cosmosclass *cinfo);
int32_t json_mapfaceentry(uint16_t fidx, cosmosclass *cinfo);
size_t json_count_hash(uint16_t hash, cosmosclass *cinfo);
size_t json_count_total(cosmosclass *cinfo);

//uint8_t *json_ptr_of_ptm(cosmosdatastrucVoid ptm, uint16_t type, cosmosclass *cinfo);
uint8_t *json_ptr_of_offset(ptrdiff_t offset, uint16_t group, cosmosclass *cinfo);
uint8_t *json_ptr_of_entry(const jsonentry &entry, cosmosclass *cinfo);
jsonentry *json_entry_of(uint8_t *ptr, cosmosclass *cinfo);
jsonentry *json_entry_of(string token, cosmosclass *cinfo);
jsonentry *json_entry_of(jsonhandle handle, cosmosclass *cinfo);
jsonequation *json_equation_of(jsonhandle handle, cosmosclass *cinfo);
int32_t json_table_of_list(vector<jsonentry*> &entry, string tokens, cosmosclass *cinfo);
uint16_t json_type_of_name(string token, cosmosclass *cinfo);

//cyt: need a json_table_of_list?
int32_t json_append(string &jstring, const char *tstring);

int32_t json_out_entry(string &jstring, handlerstruc entry);
int32_t json_out_int8(string &jstring, void* value);
int32_t json_out_int16(string &jstring, void* value);
int32_t json_out_int32(string &jstring, void* value);
int32_t json_out_uint8(string &jstring, void* value);
int32_t json_out_uint16(string &jstring, void* value);
int32_t json_out_uint32(string &jstring, void* value);
int32_t json_out_float(string &jstring, void* value);
int32_t json_out_double(string &jstring, void* value);
// int32_t json_out_string(string &jstring, void* value);
int32_t json_out_rvector(string &jstring, void* value);

int32_t json_out_value(string &jstring, string name, uint8_t *data, uint16_t type, cosmosclass *cinfo);
int32_t json_out_handle(string &jstring, jsonhandle handle,cosmosclass *cinfo);
int32_t json_out_entry(string &jstring, const jsonentry &entry, cosmosclass *cinfo);
int32_t json_out(string &jstring, string token,cosmosclass *cinfo);
int32_t json_out_list(string &jstring, string tokens, cosmosclass *cinfo);
int32_t json_out_1d(string &jstring, const char *token, uint16_t col, cosmosclass *cinfo);
int32_t json_out_2d(string &jstring,const char *token, uint16_t row, uint16_t col,cosmosclass *cinfo);
int32_t json_out_name(string &jstring, string name);
int32_t json_out_character(string &jstring,char character);
//int32_t json_out_type(string &jstring, uint8_t *data, uint16_t type, cosmosclass *cinfo);
int32_t json_out_int8(string &jstring,int8_t value);
int32_t json_out_int16(string &jstring,int16_t value);
int32_t json_out_int32(string &jstring,int32_t value);
int32_t json_out_uint8(string &jstring,uint8_t value);
int32_t json_out_uint16(string &jstring,uint16_t value);
int32_t json_out_uint32(string &jstring,uint32_t value);
int32_t json_out_float(string &jstring,float value);
int32_t json_out_double(string &jstring,double value);
int32_t json_out_string(string &jstring, string ostring, uint16_t len);
int32_t json_out_vector(string &jstring,Vector value);
int32_t json_out_svector(string &jstring,svector value);
int32_t json_out_avector(string &jstring,avector value);
int32_t json_out_gvector(string &jstring,gvector value);
int32_t json_out_cvector(string &jstring,cvector value);
int32_t json_out_rvector(string &jstring,rvector value);
int32_t json_out_tvector(string &jstring,rvector value);
int32_t json_out_quaternion(string &jstring,quaternion value);
int32_t json_out_cartpos(string &jstring,cartpos value);
int32_t json_out_geoidpos(string &jstring,geoidpos value);
int32_t json_out_spherpos(string &jstring,spherpos value);
int32_t json_out_dcmatt(string &jstring, dcmatt value);
int32_t json_out_qatt(string &jstring,qatt value);
int32_t json_out_dcm(string &jstring,rmatrix value);
int32_t json_out_rmatrix(string &jstring,rmatrix value);
int32_t json_out_beatstruc(string &jstring,beatstruc value);
int32_t json_out_node(string &jstring, string value);
int32_t json_out_utcstart(string &jstring, double value);
int32_t json_out_ecipos(string &jstring,cartpos value);
int32_t json_out_posstruc(string &jstring,posstruc value);
int32_t json_out_attstruc(string &jstring,attstruc value);
int32_t json_out_locstruc(string &jstring,locstruc value);
int32_t json_out_commandevent(string &jstring, longeventstruc event);

uint8_t *json_ptrto(string token, cosmosclass *cinfo);
uint8_t *json_ptrto_1d(const char *token, uint16_t index1, cosmosclass *cinfo);
uint8_t *json_ptrto_2d(const char *token, uint16_t index1, uint16_t index2, cosmosclass *cinfo);

double json_equation(const char *&ptr, cosmosclass *cinfo);
double json_equation(jsonequation *ptr, cosmosclass *cinfo); // TODO: overload with json_equation
double json_equation(jsonhandle *handle, cosmosclass *cinfo); // TODO: overload with json_equation

int32_t json_get_int(jsonhandle &handle, cosmosclass *cinfo);
int32_t json_get_int(string token, cosmosclass *cinfo);
int32_t json_get_int(string token, uint16_t index1, cosmosclass *cinfo);
int32_t json_get_int(string token, uint16_t index1, uint16_t index2, cosmosclass *cinfo);
int32_t json_get_int(const jsonentry &entry, cosmosclass *cinfo);

uint32_t json_get_uint(jsonhandle &handle, cosmosclass *cinfo);
uint32_t json_get_uint(string token, cosmosclass *cinfo);
uint32_t json_get_uint(string token, uint16_t index1, cosmosclass *cinfo);
uint32_t json_get_uint(string token, uint16_t index1, uint16_t index2, cosmosclass *cinfo);
uint32_t json_get_uint(const jsonentry &entry, cosmosclass *cinfo);

double json_get_double(jsonhandle &handle, cosmosclass *cinfo);
double json_get_double(string token, cosmosclass *cinfo);
double json_get_double(string token, uint16_t index1, cosmosclass *cinfo);
double json_get_double(string token, uint16_t index1, uint16_t index2, cosmosclass *cinfo);
double json_get_double(const jsonentry &entry, cosmosclass *cinfo);

rvector json_get_rvector(const jsonentry &entry, cosmosclass *cinfo);
quaternion json_get_quaternion(const jsonentry &entry, cosmosclass *cinfo);

string json_get_string(string token, cosmosclass *cinfo);
string json_get_string(const jsonentry &entry, cosmosclass *cinfo);

posstruc json_get_posstruc(const jsonentry &entry, cosmosclass *cinfo);

int32_t json_set_number(double val, const jsonentry &entry, cosmosclass *cinfo);
int32_t json_set_string(string val, const jsonentry &entry, cosmosclass *cinfo);

int32_t json_scan(char *istring);

int32_t json_tokenize(string jstring, cosmosclass *cinfo, vector <jsontoken> &tokens);
int32_t json_tokenize_namedobject(const char *&pointer, cosmosclass *cinfo, jsontoken &token);
int32_t json_parse(string json, cosmosclass *cinfo);
int32_t json_parse_namedobject(const char *&ptr, cosmosclass *cinfo);
int32_t json_parse_value(const char* &ptr, uint16_t type, ptrdiff_t offset, uint16_t group, cosmosclass *cinfo);
int32_t json_parse_value(const char* &ptr, const jsonentry &entry, cosmosclass *cinfo);
int32_t json_parse_value(const char *&ptr, uint16_t type, uint8_t *data, cosmosclass *cinfo);
int32_t json_parse_equation(const char* &ptr, string &equation);
int32_t json_parse_operand(const char* &ptr, jsonoperand *operand, cosmosclass *cinfo);
int32_t json_extract_string(const char* &ptr, string &ostring);
int32_t json_parse_name(const char* &ptr, string &ostring);

string json_extract_namedobject(string json, string token);
int32_t json_extract_value(const char *&ptr, string &value);
string json_convert_string(string object);
int32_t json_convert_int32(string object);
uint32_t json_convert_uint32(string object);
int16_t json_convert_int16(string object);
uint16_t json_convert_uint16(string object);
float json_convert_float(string object);
double json_convert_double(string object);

int32_t json_parse_number(const char* &ptr, double *value);
int32_t json_skip_character(const char* &ptr, const char character);
int32_t json_skip_value(const char* &ptr);
int32_t json_skip_white(const char* &ptr);
int32_t json_clear_cosmosclass(int32_t type, cosmosclass *cinfo);
int32_t json_setup_node(jsonnode json, cosmosclass *cinfo, bool create_flag = false);
int32_t json_setup_node(string node, cosmosclass *cinfo);
int32_t json_load_node(string node, jsonnode &json);
int32_t json_dump_node(cosmosclass *cinfo);
int32_t json_recenter_node(cosmosclass *cinfo);

const char *json_of_wildcard(string &jstring, string wildcard, cosmosclass *cinfo);
const char *json_of_list(string &jstring, string tokens, cosmosclass *cinfo);
const char *json_of_table(string &jstring,vector<jsonentry*> entries,cosmosclass *cinfo);
const char *json_of_node(string &jstring, cosmosclass *cinfo);
const char *json_of_agent(string &jstring, cosmosclass *cinfo);
const char *json_of_target(string &jstring, cosmosclass *cinfo, uint16_t num);
const char *json_of_time(string &jstring, cosmosclass *cinfo);
const char *json_of_beat(string &jstring, cosmosclass *cinfo);
const char *json_of_beacon(string &jstring, cosmosclass *cinfo);
const char *json_of_groundcontact(cosmosclass *cinfo);
const char *json_of_soh(string &jstring, cosmosclass *cinfo);
string json_list_of_soh(cosmosclass *cinfo);
string json_list_of_all(cosmosclass *cinfo);
const char *json_of_utc(string &jstring, cosmosclass *cinfo);
const char *json_of_state_eci(string &jstring, cosmosclass *cinfo);
const char *json_of_state_geoc(string &jstring, cosmosclass *cinfo);
const char *json_of_imu(string &jstring,uint16_t num, cosmosclass *cinfo);
const char *json_of_rw(string &jstring,uint16_t index, cosmosclass *cinfo);
const char *json_of_mtr(string &jstring,uint16_t index, cosmosclass *cinfo);
const char *json_of_ephemeris(string &jstring, cosmosclass *cinfo);
const char *json_of_groundstation(string &jstring, cosmosclass *cinfo);
const char *json_of_event(string &jstring, cosmosclass *cinfo);
const char *json_of_log(string &jstring, cosmosclass *cinfo);
//char* json_of_command(string &jstring, string node, string name, string user, string type, double utc);
const char *json_node(string &jstring, cosmosclass *cinfo);
const char *json_vertices(string &jstring, cosmosclass *cinfo);
const char *json_faces(string &jstring, cosmosclass *cinfo);
const char *json_pieces(string &jstring, cosmosclass *cinfo);
const char *json_devices_general(string &jstring, cosmosclass *cinfo);
const char *json_devices_specific(string &jstring, cosmosclass *cinfo);
const char *json_ports(string &jstring, cosmosclass *cinfo);

void json_test(cosmosclass *cinfo);

uint16_t json_hash(string hstring);
//uint16_t json_hash2(const char *string);
//json_name *json_get_name_list();
uint32_t json_get_name_list_count(cosmosclass *cinfo);
int32_t json_name_map(string name, cosmosclass *cinfo, jsonhandle &handle);
int32_t json_equation_map(string equation, cosmosclass *cinfo, jsonhandle *handle);

bool json_static(char* json_extended_name);
bool json_dynamic(char* json_extended_name);

int32_t node_init(string name, cosmosclass *cinfo);
int32_t node_calc(cosmosclass *cinfo);
void create_databases(cosmosclass *cinfo);
//void load_databases(char *name, uint16_t type, cosmosclass *cinfo);
size_t load_dictionary(vector<shorteventstruc> &dict, cosmosclass *cinfo, const char *file);
int32_t load_target(cosmosclass *cinfo);
int32_t update_target(cosmosclass *cinfo);
int32_t update_target(locstruc source, targetstruc &target);
size_t calc_events(vector<shorteventstruc> &dictionary, cosmosclass *cinfo, vector<shorteventstruc> &events);

string device_type_name(uint32_t type);
string port_type_name(uint32_t type);


//! @}


#endif
