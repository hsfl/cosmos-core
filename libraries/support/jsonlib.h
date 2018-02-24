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
//! ::targetstruc, stored within the ::cosmosstruc. These values can be accessed externally through
//! names of type "target_*". This table is initialized using
//! ::load_target, and updated with values in the current ::cosmosstruc using ::update_target. Information
//! calculated includes the Azimuth and Elevation to and from each target, as well as its range.
//!
//! Events
//!
//! Unlike Targetting, information about Physical Events is not stored directly in the ::cosmosstruc. Instead,
//! the user keeps a table of type ::shorteventstruc, initialized with ::load_dictionary, that provides templates
//! for all possible events. These templates can be matched against current conditions using ::calc_events.
//! This will return a second table of type ::shorteventstruc that includes an entry for each event that:
//! - had its condition calculate non zero
//! - did not have its condition calculate non zero previously
//!

//#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "support/configCosmos.h"

#include "support/jsondef.h"
#include "support/datalib.h"
#include "support/stringlib.h"
#include <limits>
using std::numeric_limits;

//! \ingroup jsonlib
//! \defgroup jsonlib_functions JSON functions
//! @{

cosmosstruc *json_create();
int32_t json_clone(cosmosstruc *cinfo);
int32_t json_clone(cosmosstruc *cinfo1, cosmosstruc *cinfo2);
int32_t json_repoint(cosmosstruc *cinfo);
void json_destroy(cosmosstruc *cinfo);
int32_t json_pushdevspec(uint16_t cidx, cosmosstruc *cinfo);

//uint16_t json_addequation(const char *text, cosmosstruc *cinfo, uint16_t unit);
int32_t json_addpiece(cosmosstruc *cinfo, string name, uint16_t type=PIECE_TYPE_DIMENSIONLESS, uint16_t cidx=UINT16_MAX, double emi=.8, double abs=.88, double hcap=800., double hcon=237., double density=1000.);
int32_t json_addentry(string name, string value, cosmosstruc *cinfo);
int32_t json_addentry(jsonentry entry, cosmosstruc *cinfo);
int32_t json_addentry(string name, uint16_t d1, uint16_t d2, ptrdiff_t offset, uint16_t type, uint16_t group, cosmosstruc *cinfo, uint16_t unit=0);
int32_t json_addentry(string name, uint16_t d1, uint16_t d2, uint8_t *ptr, uint16_t type, cosmosstruc *cinfo, uint16_t unit=0);
//int32_t json_addentry(string name, uint16_t d1, uint16_t d2, const cosmosdatastrucVoid &ptr, uint16_t type, cosmosstruc *cinfo, uint16_t unit=0);
int32_t json_toggleentry(string name, uint16_t d1, uint16_t d2, cosmosstruc *cinfo, bool state);
bool json_checkentry(string name, uint16_t d1, uint16_t d2, cosmosstruc *cinfo);
int32_t json_mapentries(cosmosstruc *cinfo);
int32_t json_mapbaseentries(cosmosstruc *cinfo);
int32_t json_mappieceentry(uint16_t pidx, cosmosstruc *cinfo);
int32_t json_togglepieceentry(uint16_t pidx, cosmosstruc *cinfo, bool state);
int32_t json_mapcompentry(uint16_t cidx, cosmosstruc *cinfo);
int32_t json_togglecompentry(uint16_t cidx, cosmosstruc *cinfo, bool state);
uint16_t json_mapdeviceentry(const devicestruc &device, cosmosstruc *cinfo);
int32_t json_toggledeviceentry(uint16_t didx, uint16_t type, cosmosstruc *cinfo, bool state);
uint16_t json_mapportentry(uint16_t portidx, cosmosstruc *cinfo);
int32_t json_toggleportentry(uint16_t portidx, cosmosstruc *cinfo, bool state);
int32_t json_mapvertexentry(uint16_t vidx, cosmosstruc *cinfo);
int32_t json_mapfaceentry(uint16_t fidx, cosmosstruc *cinfo);
size_t json_count_hash(uint16_t hash, cosmosstruc *cinfo);
size_t json_count_total(cosmosstruc *cinfo);

//uint8_t *json_ptr_of_ptm(cosmosdatastrucVoid ptm, uint16_t type, cosmosstruc *cinfo);
uint8_t *json_ptr_of_offset(ptrdiff_t offset, uint16_t group, cosmosstruc *cinfo);
uint8_t *json_ptr_of_entry(const jsonentry &entry, cosmosstruc *cinfo);
jsonentry *json_entry_of(uint8_t *ptr, cosmosstruc *cinfo);
jsonentry *json_entry_of(string token, cosmosstruc *cinfo);
jsonentry *json_entry_of(jsonhandle handle, cosmosstruc *cinfo);
jsonequation *json_equation_of(jsonhandle handle, cosmosstruc *cinfo);
int32_t json_table_of_list(vector<jsonentry*> &entry, string tokens, cosmosstruc *cinfo);
uint16_t json_type_of_name(string token, cosmosstruc *cinfo);

int32_t json_append(string &jstring, const char *tstring);

int32_t json_out_value(string &jstring, string name, uint8_t *data, uint16_t type, cosmosstruc *cinfo);
int32_t json_out_handle(string &jstring, jsonhandle handle,cosmosstruc *cinfo);
int32_t json_out_entry(string &jstring, const jsonentry &entry, cosmosstruc *cinfo);
int32_t json_out(string &jstring, string token,cosmosstruc *cinfo);
int32_t json_out_list(string &jstring, string tokens, cosmosstruc *cinfo);
int32_t json_out_1d(string &jstring, const char *token, uint16_t col, cosmosstruc *cinfo);
int32_t json_out_2d(string &jstring,const char *token, uint16_t row, uint16_t col,cosmosstruc *cinfo);
int32_t json_out_name(string &jstring, string name);
int32_t json_out_character(string &jstring,char character);
int32_t json_out_type(string &jstring, uint8_t *data, uint16_t type, cosmosstruc *cinfo);
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

uint8_t *json_ptrto(string token, cosmosstruc *cinfo);
uint8_t *json_ptrto_1d(const char *token, uint16_t index1, cosmosstruc *cinfo);
uint8_t *json_ptrto_2d(const char *token, uint16_t index1, uint16_t index2, cosmosstruc *cinfo);

double json_equation(const char *&ptr, cosmosstruc *cinfo);
double json_equation(jsonequation *ptr, cosmosstruc *cinfo); // TODO: overload with json_equation
double json_equation(jsonhandle *handle, cosmosstruc *cinfo); // TODO: overload with json_equation

int32_t json_get_int(jsonhandle &handle, cosmosstruc *cinfo);
int32_t json_get_int(string token, cosmosstruc *cinfo);
int32_t json_get_int(string token, uint16_t index1, cosmosstruc *cinfo);
int32_t json_get_int(string token, uint16_t index1, uint16_t index2, cosmosstruc *cinfo);
int32_t json_get_int(const jsonentry &entry, cosmosstruc *cinfo);

uint32_t json_get_uint(jsonhandle &handle, cosmosstruc *cinfo);
uint32_t json_get_uint(string token, cosmosstruc *cinfo);
uint32_t json_get_uint(string token, uint16_t index1, cosmosstruc *cinfo);
uint32_t json_get_uint(string token, uint16_t index1, uint16_t index2, cosmosstruc *cinfo);
uint32_t json_get_uint(const jsonentry &entry, cosmosstruc *cinfo);

double json_get_double(jsonhandle &handle, cosmosstruc *cinfo);
double json_get_double(string token, cosmosstruc *cinfo);
double json_get_double(string token, uint16_t index1, cosmosstruc *cinfo);
double json_get_double(string token, uint16_t index1, uint16_t index2, cosmosstruc *cinfo);
double json_get_double(const jsonentry &entry, cosmosstruc *cinfo);

rvector json_get_rvector(const jsonentry &entry, cosmosstruc *cinfo);
quaternion json_get_quaternion(const jsonentry &entry, cosmosstruc *cinfo);

string json_get_string(string token, cosmosstruc *cinfo);
string json_get_string(const jsonentry &entry, cosmosstruc *cinfo);

posstruc json_get_posstruc(const jsonentry &entry, cosmosstruc *cinfo);

int32_t json_set_number(double val, const jsonentry &entry, cosmosstruc *cinfo);
int32_t json_set_string(string val, const jsonentry &entry, cosmosstruc *cinfo);

int32_t json_scan(char *istring);

int32_t json_tokenize(string jstring, cosmosstruc *cinfo, vector <jsontoken> &tokens);
int32_t json_tokenize_namedobject(const char *&pointer, cosmosstruc *cinfo, jsontoken &token);
int32_t json_parse(string json, cosmosstruc *cinfo);
int32_t json_parse_namedobject(const char *&ptr, cosmosstruc *cinfo);
int32_t json_parse_value(const char* &ptr, uint16_t type, ptrdiff_t offset, uint16_t group, cosmosstruc *cinfo);
int32_t json_parse_value(const char* &ptr, const jsonentry &entry, cosmosstruc *cinfo);
int32_t json_parse_value(const char *&ptr, uint16_t type, uint8_t *data, cosmosstruc *cinfo);
int32_t json_parse_equation(const char* &ptr, string &equation);
int32_t json_parse_operand(const char* &ptr, jsonoperand *operand, cosmosstruc *cinfo);
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
int32_t json_clear_cosmosstruc(int32_t type, cosmosstruc *cinfo);
int32_t json_setup_node(jsonnode json, cosmosstruc *cinfo, bool create_flag = false);
int32_t json_setup_node(string node, cosmosstruc *cinfo);
int32_t json_load_node(string node, jsonnode &json);
int32_t json_dump_node(cosmosstruc *cinfo);
int32_t json_recenter_node(cosmosstruc *cinfo);

const char *json_of_wildcard(string &jstring, string wildcard, cosmosstruc *cinfo);
const char *json_of_list(string &jstring, string tokens, cosmosstruc *cinfo);
const char *json_of_table(string &jstring,vector<jsonentry*> entries,cosmosstruc *cinfo);
const char *json_of_node(string &jstring, cosmosstruc *cinfo);
const char *json_of_agent(string &jstring, cosmosstruc *cinfo);
const char *json_of_target(string &jstring, cosmosstruc *cinfo, uint16_t num);
const char *json_of_time(string &jstring, cosmosstruc *cinfo);
const char *json_of_beat(string &jstring, cosmosstruc *cinfo);
const char *json_of_beacon(string &jstring, cosmosstruc *cinfo);
const char *json_of_groundcontact(cosmosstruc *cinfo);
const char *json_of_soh(string &jstring, cosmosstruc *cinfo);
string json_list_of_soh(cosmosstruc *cinfo);
string json_list_of_all(cosmosstruc *cinfo);
const char *json_of_utc(string &jstring, cosmosstruc *cinfo);
const char *json_of_state_eci(string &jstring, cosmosstruc *cinfo);
const char *json_of_state_geoc(string &jstring, cosmosstruc *cinfo);
const char *json_of_imu(string &jstring,uint16_t num, cosmosstruc *cinfo);
const char *json_of_rw(string &jstring,uint16_t index, cosmosstruc *cinfo);
const char *json_of_mtr(string &jstring,uint16_t index, cosmosstruc *cinfo);
const char *json_of_ephemeris(string &jstring, cosmosstruc *cinfo);
const char *json_of_groundstation(string &jstring, cosmosstruc *cinfo);
const char *json_of_event(string &jstring, cosmosstruc *cinfo);
const char *json_of_log(string &jstring, cosmosstruc *cinfo);
//char* json_of_command(string &jstring, string node, string name, string user, string type, double utc);
const char *json_node(string &jstring, cosmosstruc *cinfo);
const char *json_vertices(string &jstring, cosmosstruc *cinfo);
const char *json_faces(string &jstring, cosmosstruc *cinfo);
const char *json_pieces(string &jstring, cosmosstruc *cinfo);
const char *json_devices_general(string &jstring, cosmosstruc *cinfo);
const char *json_devices_specific(string &jstring, cosmosstruc *cinfo);
const char *json_ports(string &jstring, cosmosstruc *cinfo);

void json_test(cosmosstruc *cinfo);

uint16_t json_hash(string hstring);
//uint16_t json_hash2(const char *string);
//json_name *json_get_name_list();
uint32_t json_get_name_list_count(cosmosstruc *cinfo);
int32_t json_name_map(string name, cosmosstruc *cinfo, jsonhandle &handle);
int32_t json_equation_map(string equation, cosmosstruc *cinfo, jsonhandle *handle);

bool json_static(char* json_extended_name);
bool json_dynamic(char* json_extended_name);

int32_t node_init(string name, cosmosstruc *cinfo);
int32_t node_calc(cosmosstruc *cinfo);
void create_databases(cosmosstruc *cinfo);
//void load_databases(char *name, uint16_t type, cosmosstruc *cinfo);
size_t load_dictionary(vector<shorteventstruc> &dict, cosmosstruc *cinfo, const char *file);
int32_t load_target(cosmosstruc *cinfo);
int32_t update_target(cosmosstruc *cinfo);
int32_t update_target(locstruc source, targetstruc &target);
size_t calc_events(vector<shorteventstruc> &dictionary, cosmosstruc *cinfo, vector<shorteventstruc> &events);

string device_type_name(uint32_t type);
string port_type_name(uint32_t type);


//! @}


#endif
