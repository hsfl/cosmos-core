#ifndef _JSONLIB_H
#define _JSONLIB_H 1

/*! \file jsonlib.h
* \brief JSON Support
*/

//! \ingroup support
//! \defgroup jsonlib JSON Library
//! A library providing functions in support of the COSMOS specific JSON format.
//!
//! JSON is a special application of JavaScript Object Notation. It has been
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

//#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "configCosmos.h"

#include "agentdef.h"
#include "cosmos-errno.h"
#include "mathlib.h"
#include "convertlib.h"
#include "memlib.h"
#include "jsondef.h"
#include "stringlib.h"

#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <stdarg.h>
//#include <sys/types.h>
#include <stddef.h>

//! \ingroup jsonlib
//! \defgroup jsonlib_functions JSON functions
//! @{

cosmosstruc *json_create();
int32_t json_clone(cosmosstruc *cdata);
void json_destroy(cosmosstruc *cdata);

//uint16_t json_addequation(const char *text, cosmosstruc *cdata, uint16_t unit);
uint16_t json_addentry(const char *name, int16_t d1, int16_t d2, ptrdiff_t offset, uint16_t type, uint16_t group, cosmosstruc *cdata, uint16_t unit);
uint16_t json_addentry(const char *name, int16_t d1, int16_t d2, ptrdiff_t offset, uint16_t type, uint16_t group, cosmosstruc *cdata);
uint16_t json_addbaseentry(cosmosstruc *cdata);
uint16_t json_addpieceentry(uint16_t i, cosmosstruc *cdata);
uint16_t json_addcompentry(uint16_t i, cosmosstruc *cdata);
uint16_t json_adddeviceentry(uint16_t i, cosmosstruc *cdata);
uint32_t json_length(jstring *jstring);
uint32_t json_index(jstring *jstring);
uint32_t json_count_hash(uint16_t hash, cosmosstruc *cdata);
uint32_t json_count_total(cosmosstruc *cdata);

int32_t json_startout(jstring *jstring);
int32_t json_stopout(jstring *jstring);

uint8_t *json_ptr_of_offset(ptrdiff_t offset, uint16_t group, cosmosstruc *cdata);
jsonentry *json_entry_of_ptr(uint8_t *ptr, cosmosstruc *cdata);
jsonentry *json_entry_of_name(string token, cosmosstruc *cdata);
int32_t json_table_of_list(vector<jsonentry*> &entry, const char *tokens, cosmosstruc *cdata);
uint16_t json_type_of_name(string token, cosmosstruc *cdata);

int32_t json_append(jstring *jstring,char *tstring);

int32_t json_out_value(jstring *jstring, char *name, uint8_t *data, uint16_t type, cosmosstruc *cdata);
int32_t json_out_handle(jstring *jstring, jsonhandle handle,cosmosstruc *cdata);
int32_t json_out_entry(jstring* jstring, jsonentry* entry, cosmosstruc* cdata);
int32_t json_out(jstring *jstring, string token,cosmosstruc *cdata);
int32_t json_out_list(jstring *jstring,const char *tokens,cosmosstruc *cdata);
int32_t json_out_1d(jstring *jstring, const char *token, uint16_t col, cosmosstruc *cdata);
int32_t json_out_2d(jstring *jstring,const char *token, uint16_t row, uint16_t col,cosmosstruc *cdata);
int32_t json_out_name(jstring *jstring,char *name);
int32_t json_out_character(jstring *jstring,char character);
int32_t json_out_int16(jstring *jstring,int16_t value);
int32_t json_out_int32(jstring *jstring,int32_t value);
int32_t json_out_uint16(jstring *jstring,uint16_t value);
int32_t json_out_uint32(jstring *jstring,uint32_t value);
int32_t json_out_float(jstring *jstring,float value);
int32_t json_out_double(jstring *jstring,double value);
int32_t json_out_string(jstring *jstring,char *string, uint16_t len);
int32_t json_out_svector(jstring *jstring,svector value);
int32_t json_out_gvector(jstring *jstring,gvector value);
int32_t json_out_cvector(jstring *jstring,cvector value);
int32_t json_out_rvector(jstring *jstring,rvector value);
int32_t json_out_tvector(jstring *jstring,rvector value);
int32_t json_out_quaternion(jstring *jstring,quaternion value);
int32_t json_out_cartpos(jstring *jstring,cartpos value);
int32_t json_out_geoidpos(jstring *jstring,geoidpos value);
int32_t json_out_spherpos(jstring *jstring,spherpos value);
int32_t json_out_dcmatt(jstring *jstring,dcmatt value);
int32_t json_out_qatt(jstring *jstring,qatt value);
int32_t json_out_dcm(jstring *jstring,rmatrix value);
int32_t json_out_rmatrix(jstring *jstring,rmatrix value);
int32_t json_out_beatstruc(jstring *jstring,beatstruc value);
int32_t json_out_posstruc(jstring *jstring,posstruc value);
int32_t json_out_attstruc(jstring *jstring,attstruc value);
int32_t json_out_locstruc(jstring *jstring,locstruc value);
int32_t json_out_commandevent(jstring *jstring, longeventstruc event);

uint8_t *json_ptrto(string token, cosmosstruc *cdata);
uint8_t *json_ptrto_1d(const char *token, uint16_t index1, cosmosstruc *cdata);
uint8_t *json_ptrto_2d(const char *token, uint16_t index1, uint16_t index2, cosmosstruc *cdata);

double json_equation(const char** pointer, cosmosstruc *cdata);
double json_equation_entry(jsonequation *ptr, cosmosstruc *cdata);
double json_equation_handle(jsonhandle *handle, cosmosstruc *cdata);

//uint8_t json_get_byte_name(const char *token, cosmosstruc *cdata);
int32_t json_get_int_name(string token, cosmosstruc *cdata);
uint32_t json_get_uint_name(string token, cosmosstruc *cdata);
double json_get_double_entry(jsonentry *entry, cosmosstruc *cdata);
double json_get_double_name(string token, cosmosstruc *cdata);
string json_get_string_name(string token, cosmosstruc *cdata);
int32_t json_get_int_name_1d(string token, int index1, cosmosstruc *cdata);
uint32_t json_get_uint_name_1d(string token, int index1, cosmosstruc *cdata);
double json_get_double_name_1d(string token, int index1, cosmosstruc *cdata);
int32_t json_get_int_name_2d(string token, int index1, int index2, cosmosstruc *cdata);
uint32_t json_get_uint_name_2d(string token, int index1, int index2, cosmosstruc *cdata);
double json_get_double_name_2d(string token, int index1, int index2, cosmosstruc *cdata);

int32_t json_set_int_name(int32_t value,char *token, cosmosstruc *cdata);
int32_t json_set_uint_name(uint32_t value,char *token, cosmosstruc *cdata);
int32_t json_set_double_name(double value,char *token, cosmosstruc *cdata);
int32_t json_set_int_name_1d(int32_t value,char *token, int index1, cosmosstruc *cdata);
int32_t json_set_uint_name_1d(uint32_t value,char *token, int index1, cosmosstruc *cdata);
int32_t json_set_double_name_1d(double value,char *token, int index1, cosmosstruc *cdata);
int32_t json_set_int_name_2d(int32_t value,char *token, int index1, int index2, cosmosstruc *cdata);
int32_t json_set_uint_name_2d(uint32_t value,char *token, int index1, int index2, cosmosstruc *cdata);
int32_t json_set_double_name_2d(double value,char *token, int index1, int index2, cosmosstruc *cdata);

int32_t json_scan(char *istring);

int32_t json_parse(string json, cosmosstruc *cdata);
int32_t json_parse_namedobject(const char** pointer, cosmosstruc *cdata);
int32_t json_parse_value(const char **pointer, uint8_t type, ptrdiff_t offset, uint16_t group, cosmosstruc *cdata);
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
int32_t json_clear_cosmosstruc(int type, cosmosstruc *cdata);
int32_t json_setup(cosmosstruc *cdata);
int32_t json_setup_node(const char *node, cosmosstruc *cdata);
int32_t json_dump_node(cosmosstruc *cdata);

char *json_of_wildcard(jstring *jstring, char *wildcard, cosmosstruc *cdata);
char *json_of_list(jstring *jstring, const char *tokens, cosmosstruc *cdata);
char *json_of_table(jstring *jstring,vector<jsonentry*> entries,cosmosstruc *cdata);
char *json_of_node(jstring *jstring, cosmosstruc *cdata);
char *json_of_agent(jstring *jstring, cosmosstruc *cdata);
char *json_of_target(jstring *jstring, cosmosstruc *cdata, int num);
char *json_of_time(jstring *jstring, cosmosstruc *cdata);
char *json_of_beat(jstring *jstring, cosmosstruc *cdata);
char *json_of_beacon(jstring *jstring, cosmosstruc *cdata);
char *json_of_groundcontact(cosmosstruc *cdata);
char *json_of_soh(jstring *jstring, cosmosstruc *cdata);
string json_list_of_soh(cosmosstruc* cdata);
char *json_of_utc(jstring *jstring, cosmosstruc *cdata);
char *json_of_statevec(jstring *jstring, cosmosstruc *cdata);
char *json_of_imu(jstring *jstring,uint16_t num, cosmosstruc *cdata);
char *json_of_rw(jstring *jstring,uint16_t index, cosmosstruc *cdata);
char *json_of_mtr(jstring *jstring,uint16_t index, cosmosstruc *cdata);
char *json_of_ephemeris(jstring *jstring, cosmosstruc *cdata);
char *json_of_groundstation(jstring *jstring, cosmosstruc *cdata);
char *json_of_event(jstring *jstring, cosmosstruc *cdata);
char *json_of_log(jstring *jstring, cosmosstruc *cdata);
//char* json_of_command(jstring *jstring, string node, string name, string user, string type, double utc);
char *json_node(jstring *jstring, cosmosstruc *cdata);
char *json_pieces(jstring *jstring, cosmosstruc *cdata);
char *json_devices_general(jstring *jstring, cosmosstruc *cdata);
char *json_devices_specific(jstring *jstring, cosmosstruc *cdata);
char *json_ports(jstring *jstring, cosmosstruc *cdata);

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

//! @}


#endif
