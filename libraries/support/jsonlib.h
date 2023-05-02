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
//! the user keeps a table of type ::eventstruc, initialized with ::load_dictionary, that provides templates
//! for all possible events. These templates can be matched against current conditions using ::calc_events.
//! This will return a second table of type ::eventstruc that includes an entry for each event that:
//! - had its condition calculate non zero
//! - did not have its condition calculate non zero previously
//!

//#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "support/configCosmos.h"

#include "support/jsondef.h"
#include "support/datalib.h"
#include "support/stringlib.h"

//! \ingroup jsonlib
//! \defgroup jsonlib_functions JSON functions
//! @{

void json_init_unit(cosmosstruc* cinfo);
void json_init_device_type_string();
void json_init_node(cosmosstruc* cinfo);
void json_init_reserve(cosmosstruc* cinfo);
cosmosstruc *json_init(cosmosstruc *cinfo);
cosmosstruc *json_init();
int32_t json_create_node(cosmosstruc *cinfo, string &node_name, uint16_t node_type=NODE_TYPE_COMPUTER);
int32_t json_shrink(cosmosstruc *cinfo);
//int32_t json_clone(cosmosstruc *cinfo1, cosmosstruc *cinfo2);
int32_t json_repoint(cosmosstruc *cinfo);
void json_destroy(cosmosstruc *cinfo);
//int32_t json_pushdevspec(uint16_t cidx, cosmosstruc *cinfo);
int32_t json_updatecosmosstruc(cosmosstruc *cinfo);

int32_t json_createpiece(cosmosstruc *cinfo, string name, DeviceType ctype, double emi=1.0, double abs=1.0, double hcap=0.9, double hcon=205., double density=2710.);
int32_t json_addpiece(cosmosstruc *cinfo, string name, DeviceType ctype, double emi=1.0, double abs=1.0, double hcap=0.9, double hcon=205., double density=2710.);
int32_t json_adddevice(cosmosstruc *cinfo, uint16_t pidx, DeviceType ctype);
int32_t json_findpiece(cosmosstruc *cinfo, string name);
int32_t json_findcomp(cosmosstruc *cinfo, string name);
int32_t json_finddev(cosmosstruc *cinfo, string name);
int32_t json_addentry(string alias, string value, cosmosstruc *cinfo);
int32_t json_addentry(string name, uint16_t d1, uint16_t d2, uint8_t *ptr, uint16_t type, cosmosstruc *cinfo, uint16_t unit=0);

jsonequation *json_equation_of(jsonhandle handle, cosmosstruc *cinfo);

double json_equation(const char *&ptr, cosmosstruc *cinfo);
double json_equation(jsonequation *ptr, cosmosstruc *cinfo); // TODO: overload with json_equation
double json_equation(jsonhandle *handle, cosmosstruc *cinfo); // TODO: overload with json_equation

int32_t json_get_int(const jsonentry &entry, cosmosstruc *cinfo);

double json_get_double(string token, cosmosstruc *cinfo);
double json_get_double(const jsonentry &entry, cosmosstruc *cinfo);

rvector json_get_rvector(const jsonentry &entry, cosmosstruc *cinfo);
quaternion json_get_quaternion(const jsonentry &entry, cosmosstruc *cinfo);

int32_t json_clear_cosmosstruc(int32_t type, cosmosstruc *cinfo);
int32_t json_setup_node(jsonnode json, cosmosstruc *cinfo, bool create_flag = false);
int32_t json_setup_node(string &node, cosmosstruc *cinfo);
int32_t json_load_node(string node, jsonnode &json);
int32_t load_tle(cosmosstruc *cinfo);
int32_t json_dump_node(cosmosstruc *cinfo);

uint16_t json_hash(string hstring);
int32_t json_equation_map(string equation, cosmosstruc *cinfo, jsonhandle *handle);

//void load_databases(char *name, uint16_t type, cosmosstruc *cinfo);
size_t load_dictionary(vector<eventstruc> &dict, cosmosstruc *cinfo, const char *file);
int32_t load_target(cosmosstruc *cinfo);
int32_t update_target(cosmosstruc *cinfo);
int32_t update_target(Convert::locstruc source, targetstruc &target);
size_t calc_events(vector<eventstruc> &dictionary, cosmosstruc *cinfo, vector<eventstruc> &events);
uint16_t device_type_index(string name);
string device_type_name(uint32_t type);
string port_type_name(uint32_t type);
int32_t device_index(cosmosstruc* cinfo, string name);
bool device_has_property(uint16_t deviceType, string prop);
string json_memory_usage();
int32_t kml_write(cosmosstruc* cinfo);

void rebase_node(cosmosstruc *cinfo);
void update_faces(const vector<Vector>& vertices, vector<facestruc>& faces);
Vector update_pieces(cosmosstruc* cinfo);
int32_t node_calc(cosmosstruc *cinfo);

//! @}

::std::ostream& operator<<(::std::ostream& out, const beatstruc& b);




#endif
