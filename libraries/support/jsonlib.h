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

#include "configCosmos.h"

#include "jsondef.h"
//#include "agentdef.h"
#include "datalib.h"
#include "stringlib.h"

//#include <stdlib.h>
//#include <stdio.h>
//#include <cstring>
//#include <stdarg.h>
//#include <stddef.h>

//! \ingroup jsonlib
//! \defgroup jsonlib_functions JSON functions
//! @{

cosmosstruc *json_create();
int32_t json_clone(cosmosstruc *cdata);
void json_destroy(cosmosstruc *cdata);

//uint16_t json_addequation(const char *text, cosmosstruc *cdata, uint16_t unit);
uint16_t json_addentry(const char *name, uint16_t d1, uint16_t d2, ptrdiff_t offset, size_t size, uint16_t type, uint16_t group, cosmosstruc *cdata, uint16_t unit);
uint16_t json_addentry(const char *name, uint16_t d1, uint16_t d2, ptrdiff_t offset, size_t size, uint16_t type, uint16_t group, cosmosstruc *cdata);
uint16_t json_addbaseentry(cosmosstruc *cdata);
uint16_t json_addpieceentry(uint16_t i, cosmosstruc *cdata);
uint16_t json_addcompentry(uint16_t i, cosmosstruc *cdata);
uint16_t json_adddeviceentry(uint16_t i, cosmosstruc *cdata);
size_t json_count_hash(uint16_t hash, cosmosstruc *cdata);
size_t json_count_total(cosmosstruc *cdata);

uint8_t *json_ptr_of_offset(ptrdiff_t offset, uint16_t group, cosmosstruc *cdata);
jsonentry *json_entry_of_ptr(uint8_t *ptr, cosmosstruc *cdata);
jsonentry *json_entry_of_name(string token, cosmosstruc *cdata);
int32_t json_table_of_list(vector<jsonentry*> &entry, const char *tokens, cosmosstruc *cdata);
uint16_t json_type_of_name(string token, cosmosstruc *cdata);

int32_t json_append(string &jstring, const char *tstring);

int32_t json_out_value(string &jstring, string name, uint8_t *data, uint16_t type, cosmosstruc *cdata);
int32_t json_out_handle(string &jstring, jsonhandle handle,cosmosstruc *cdata);
int32_t json_out_entry(string &jstring, jsonentry* entry, cosmosstruc* cdata);
int32_t json_out(string &jstring, string token,cosmosstruc *cdata);
int32_t json_out_list(string &jstring,const char *tokens,cosmosstruc *cdata);
int32_t json_out_1d(string &jstring, const char *token, uint16_t col, cosmosstruc *cdata);
int32_t json_out_2d(string &jstring,const char *token, uint16_t row, uint16_t col,cosmosstruc *cdata);
int32_t json_out_name(string &jstring, string name);
int32_t json_out_character(string &jstring,char character);
int32_t json_out_int16(string &jstring,int16_t value);
int32_t json_out_int32(string &jstring,int32_t value);
int32_t json_out_uint16(string &jstring,uint16_t value);
int32_t json_out_uint32(string &jstring,uint32_t value);
int32_t json_out_float(string &jstring,float value);
int32_t json_out_double(string &jstring,double value);
int32_t json_out_string(string &jstring, string ostring, uint16_t len);
int32_t json_out_svector(string &jstring,svector value);
int32_t json_out_gvector(string &jstring,gvector value);
int32_t json_out_cvector(string &jstring,cvector value);
int32_t json_out_rvector(string &jstring,rvector value);
int32_t json_out_tvector(string &jstring,rvector value);
int32_t json_out_quaternion(string &jstring,quaternion value);
int32_t json_out_cartpos(string &jstring,cartpos value);
int32_t json_out_geoidpos(string &jstring,geoidpos value);
int32_t json_out_spherpos(string &jstring,spherpos value);
int32_t json_out_dcmatt(string &jstring,dcmatt value);
int32_t json_out_qatt(string &jstring,qatt value);
int32_t json_out_dcm(string &jstring,rmatrix value);
int32_t json_out_rmatrix(string &jstring,rmatrix value);
int32_t json_out_beatstruc(string &jstring,beatstruc value);
int32_t json_out_ecipos(string &jstring,cartpos value);
int32_t json_out_posstruc(string &jstring,posstruc value);
int32_t json_out_attstruc(string &jstring,attstruc value);
int32_t json_out_locstruc(string &jstring,locstruc value);
int32_t json_out_commandevent(string &jstring, longeventstruc event);

uint8_t *json_ptrto(string token, cosmosstruc *cdata);
uint8_t *json_ptrto_1d(const char *token, uint16_t index1, cosmosstruc *cdata);
uint8_t *json_ptrto_2d(const char *token, uint16_t index1, uint16_t index2, cosmosstruc *cdata);

double json_equation(const char** pointer, cosmosstruc *cdata);
double json_equation_entry(jsonequation *ptr, cosmosstruc *cdata);
double json_equation_handle(jsonhandle *handle, cosmosstruc *cdata);

//uint8_t json_get_byte_name(const char *token, cosmosstruc *cdata);
int32_t json_get_int_name(string token, cosmosstruc *cdata);
uint32_t json_get_uint_name(string token, cosmosstruc *cdata);
double json_get_double_pointer(uint16_t type, uint8_t *pointer);
double json_get_double_entry(jsonentry *entry, cosmosstruc *cdata);
double json_get_double_name(string token, cosmosstruc *cdata);
string json_get_string_name(string token, cosmosstruc *cdata);
int32_t json_get_int_name_1d(string token, uint16_t index1, cosmosstruc *cdata);
uint32_t json_get_uint_name_1d(string token, uint16_t index1, cosmosstruc *cdata);
double json_get_double_name_1d(string token, uint16_t index1, cosmosstruc *cdata);
int32_t json_get_int_name_2d(string token, uint16_t index1, uint16_t index2, cosmosstruc *cdata);
uint32_t json_get_uint_name_2d(string token, uint16_t index1, uint16_t index2, cosmosstruc *cdata);
double json_get_double_name_2d(string token, uint16_t index1, uint16_t index2, cosmosstruc *cdata);

int32_t json_set_int_name(int32_t value,char *token, cosmosstruc *cdata);
int32_t json_set_uint_name(uint32_t value,char *token, cosmosstruc *cdata);
int32_t json_set_double_name(double value,char *token, cosmosstruc *cdata);
int32_t json_set_int_name_1d(int32_t value,char *token, uint16_t index1, cosmosstruc *cdata);
int32_t json_set_uint_name_1d(uint32_t value,char *token, uint16_t index1, cosmosstruc *cdata);
int32_t json_set_double_name_1d(double value,char *token, uint16_t index1, cosmosstruc *cdata);
int32_t json_set_int_name_2d(int32_t value,char *token, uint16_t index1, uint16_t index2, cosmosstruc *cdata);
int32_t json_set_uint_name_2d(uint32_t value,char *token, uint16_t index1, uint16_t index2, cosmosstruc *cdata);
int32_t json_set_double_name_2d(double value,char *token, uint16_t index1, uint16_t index2, cosmosstruc *cdata);

int32_t json_scan(char *istring);

int32_t json_tokenize(string jstring, cosmosstruc *cdata, vector <jsonentry> &tokens);
int32_t json_tokenize_namedobject(const char** pointer, cosmosstruc *cdata, jsonentry &token);
int32_t json_parse(string json, cosmosstruc *cdata);
int32_t json_parse_namedobject(const char** pointer, cosmosstruc *cdata);
int32_t json_parse_value(const char **pointer, uint16_t type, ptrdiff_t offset, uint16_t group, cosmosstruc *cdata);
int32_t json_parse_equation(const char **pointer, string &equation);
int32_t json_parse_operand(const char **pointer, jsonoperand *operand, cosmosstruc *cdata);
int32_t json_parse_string(const char **pointer, string &ostring);
int32_t json_parse_character(const char **pointer, char character);
int32_t json_parse_name(const char **pointer, string &ostring);

string json_extract_namedobject(string json, string token);
string json_convert_string(string object);
int32_t json_convert_int32(string object);
uint32_t json_convert_uint32(string object);
int16_t json_convert_int16(string object);
uint16_t json_convert_uint16(string object);
float json_convert_float(string object);
double json_convert_double(string object);

int32_t json_parse_number(const char **pointer, double *value);
int32_t json_skip_value(const char ** pointer);
int32_t json_skip_white(const char ** pointer);
int32_t json_clear_cosmosstruc(int32_t type, cosmosstruc *cdata);
int32_t json_setup(cosmosstruc *cdata);
int32_t json_setup_node(string node, cosmosstruc *cdata);
int32_t json_setup_node(string node, cosmosstruc *cdata, bool create_flag);
int32_t json_dump_node(cosmosstruc *cdata);

const char *json_of_wildcard(string &jstring, string wildcard, cosmosstruc *cdata);
const char *json_of_list(string &jstring, const char *tokens, cosmosstruc *cdata);
const char *json_of_table(string &jstring,vector<jsonentry*> entries,cosmosstruc *cdata);
const char *json_of_node(string &jstring, cosmosstruc *cdata);
const char *json_of_agent(string &jstring, cosmosstruc *cdata);
const char *json_of_target(string &jstring, cosmosstruc *cdata, uint16_t num);
const char *json_of_time(string &jstring, cosmosstruc *cdata);
const char *json_of_beat(string &jstring, cosmosstruc *cdata);
const char *json_of_beacon(string &jstring, cosmosstruc *cdata);
const char *json_of_groundcontact(cosmosstruc *cdata);
const char *json_of_soh(string &jstring, cosmosstruc *cdata);
string json_list_of_soh(cosmosstruc* cdata);
const char *json_of_utc(string &jstring, cosmosstruc *cdata);
const char *json_of_statevec(string &jstring, cosmosstruc *cdata);
const char *json_of_imu(string &jstring,uint16_t num, cosmosstruc *cdata);
const char *json_of_rw(string &jstring,uint16_t index, cosmosstruc *cdata);
const char *json_of_mtr(string &jstring,uint16_t index, cosmosstruc *cdata);
const char *json_of_ephemeris(string &jstring, cosmosstruc *cdata);
const char *json_of_groundstation(string &jstring, cosmosstruc *cdata);
const char *json_of_event(string &jstring, cosmosstruc *cdata);
const char *json_of_log(string &jstring, cosmosstruc *cdata);
//char* json_of_command(string &jstring, string node, string name, string user, string type, double utc);
const char *json_node(string &jstring, cosmosstruc *cdata);
const char *json_pieces(string &jstring, cosmosstruc *cdata);
const char *json_devices_general(string &jstring, cosmosstruc *cdata);
const char *json_devices_specific(string &jstring, cosmosstruc *cdata);
const char *json_ports(string &jstring, cosmosstruc *cdata);

void json_test();

uint16_t json_hash(string hstring);
//uint16_t json_hash2(const char *string);
//json_name *json_get_name_list();
uint32_t json_get_name_list_count();
uint32_t json_get_name_list_count(cosmosstruc *cdata);
int32_t json_name_map(string name, cosmosstruc *cdata, jsonhandle *handle);
int32_t json_equation_map(string equation, cosmosstruc *cdata, jsonhandle *handle);

bool json_static(char* json_extended_name);
bool json_dynamic(char* json_extended_name);

int32_t node_init(string name, cosmosstruc *data);
int32_t node_calc(cosmosstruc *data);
void create_databases(cosmosstruc *root);
void load_databases(char *name, uint16_t type, cosmosstruc *root);
size_t load_dictionary(vector<shorteventstruc> &dict, cosmosstruc *root, char *file);
int load_target(cosmosstruc *root);
int update_target(cosmosstruc *root);
size_t calc_events(vector<shorteventstruc> &dictionary, cosmosstruc *root, vector<shorteventstruc> &events);

string device_type_name(uint32_t type);
string port_type_name(uint32_t type);


//! @}


#endif
