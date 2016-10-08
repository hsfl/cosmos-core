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

#ifndef _DATALIB_H
#define _DATALIB_H 1

// COSMOS libs
#include "support/configCosmos.h"
#include "support/cosmos-errno.h"
#include "support/datadef.h"
#include "support/jsondef.h"
#include "support/jsonlib.h"
#include "thirdparty/zlib/zlib.h"
#include "support/timelib.h"

// C libs
#include <sys/stat.h>
#include <fstream>

#ifdef _MSC_BUILD
#include "dirent/dirent.h"
#else
#include <dirent.h>
#endif

/*! \file datalib.h
*	\brief Data Management header file
*/

//! \ingroup support
//! \defgroup datalib Data Management
//! Data Management.
//!
//! Data within COSMOS is managed in a heirarchical structure. At the highest level, separate
//! directories are provided for Nodal information (nodes), and for general software Resources (resources).
//! These two directories can be together in a single COSMOS directory, or in separate locations. Their locations can
//! be defined through the use of environment variables, or through default values. The order of assignment is as follows:
//! - If the environment variables COSMOSNODES or COSMOSRESOURCES exist, then the
//! directories specified by these variables will be used for nodes or resources.
//! - If either COSMOSNODES and/or COSMOSRESOURCES is not set, but the environment variable COSMOS is, then nodes or
//! resources will be located in ${COSMOS}/nodes or ${COSMOS}/resources.
//! - If no environment variable is found, default locations are used, specific to the OS
//! + Windows: c:/cosmos/nodes and c:/cosmos/resources
//! + Unix: /usr/local/cosmos/nodes and /usr/local/cosmos/resources
//! + MacOS: /Applications/cosmos/nodes and /Applications/cosmos/resources
//! - If these directories are not found, then the software will look up to four levels above the directory it is running
//! in for directories called "nodes" or "cosmosnodes" and "resources" or "cosmosresources".
//! Once this search is completed successfully, the nodes folder for the current Node will be stored internally as
//! "cosmosnodes" and the resources folder as "cosmosresources".
//!
//! Within "nodes", there is a directory specific to each Node. Each Node specific directory mirrors the Node:Agent
//! arrangement laid down in the \ref jsonlib. A series of initialization files are incldued, as well as a set of directories
//! for Temporary data (temp), Incoming data (incoming), Outgoing data (outgoing), and Archival data (data).
//!
//! The initialization files include:
//! - node.ini: Describes the Node type, number of pieces, number of Devices, and number of ports.
//! - pieces.ini: Describes each Piece, including the number of any related Device.
//! - devices_general.ini: Describes common elements of each Device.
//! - devices_specific.ini: Describes any unique of each Device.
//! - ports.ini: Describes Ports available to each device.
//! - target.ini: Provides a list of any targets associated with this Node.
//! - state.ini: Provides a complete state vector for this Node at some specific time.
//!
//! Each of temp, incoming and outgoing contain sub-directories named after each active Agent. Each agent sub-directory
//! in the "data" directory is further subdivided by first year, then day.
//!
//! The software resources directory contains directories of files for use in different
//! aspects of COSMOS. The directory "general" contains files of
//! coefficients used for the various models used in COSMOS simulations.
//! The directory "logo" contains any special images used by COSMOS
//! software. The directory "mapping" contains all the DEM's and
//! ancillary files used by the \ref demlib. Finally. the directory "qt"
//! contains files, such as UI forms, used by the various Qt based Tools
//! withing COSMOS.
//!
//! The functions in this library support path discovery and creation, the automatic generation
//! of standard names, and the automatic creation of log files.

//! \ingroup datalib
//! \defgroup datalib_functions Data Management function declarations
//! @{

void log_reopen();
void log_write(std::string node, int type, double utc, const char* data);
void log_write(std::string node, std::string agent, double utc, std::string type, const char *data);
void log_write(std::string node, std::string agent, double utc, std::string extra, std::string type, std::string record);
void log_write(std::string node, std::string agent, std::string location, double utc, std::string extra, std::string type, std::string record);
void log_move(std::string node, std::string agent, std::string srclocation, std::string dstlocation, bool compress);
void log_move(std::string node, std::string agent);
int check_events(eventstruc* events, int max, cosmosstruc* data);
int32_t data_get_nodes(std::vector<cosmosstruc> &data);
std::vector<std::string> data_list_nodes();
int32_t data_list_nodes(std::vector<std::string>& nodes);
std::vector<filestruc> data_list_files(std::string node, std::string location, std::string agent);
size_t data_list_files(std::string node, std::string location, std::string agent, std::vector<filestruc>& files);
std::vector<filestruc> data_list_archive(std::string node, std::string agent, double utc, std::string type);
std::vector<filestruc> data_list_archive(std::string node, std::string agent, double utc);
std::vector <double> data_list_archive_days(std::string node, std::string agent);
FILE* data_open(std::string path, char* mode);
int32_t data_name_date(std::string node, std::string filename, uint16_t &year, uint16_t &jday, uint32_t &seconds);
int32_t data_name_date(std::string node, std::string filename, double &utc);
std::string data_name(std::string node, double mjd, std::string extra, std::string type);
std::string data_name(std::string node, double mjd, std::string type);
std::string data_base_path(std::string node);
std::string data_base_path(std::string node, std::string location);
std::string data_base_path(std::string node, std::string location, std::string agent);
std::string data_base_path(std::string node, std::string location, std::string agent, std::string filename);
std::string data_archive_path(std::string node, std::string agent, double mjd);
std::string data_type_path(std::string node, std::string location, std::string agent, double mjd, std::string type);
std::string data_type_path(std::string node, std::string location, std::string agent, double mjd, std::string extra, std::string type);
std::string data_name_path(std::string node, std::string location, std::string agent, double mjd, std::string name);
bool data_exists(std::string& path);
int32_t set_cosmosresources(std::string name);
int32_t set_cosmosresources();
int32_t get_cosmosresources(std::string &result);
int32_t setEnvCosmosResources(std::string path);
int32_t setEnvCosmosNodes(std::string path);
int32_t setEnv(std::string var, std::string path);
int32_t setEnvCosmos(std::string path);
int32_t set_cosmosnodes(std::string name);
int32_t set_cosmosnodes();
int32_t get_cosmosnodes(std::string &result);
std::string get_nodedir(std::string node, bool create_flag=false);
int32_t data_load_archive(std::string node, std::string agent, double utcbegin, double utcend, std::string type, std::vector<std::string> &result);
int32_t data_load_archive(std::string node, std::string agent, double mjd, std::string type, std::vector<std::string> &result);
int32_t data_load_archive(double mjd, std::vector<std::string> &telem, std::vector<std::string> &event, cosmosstruc* root);
double findlastday(std::string node);
double findfirstday(std::string node);
int32_t kml_write(cosmosstruc* cinfo);
bool data_isdir(std::string path);

//! @}

#endif
