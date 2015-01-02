#ifndef _DATALIB_H
#define _DATALIB_H 1

#include "configCosmos.h"
//#include "cosmos-errno.h"
#include "datadef.h"
#include "jsondef.h"
//#include "timelib.h"

//#include <stdio.h>
//#include <dirent.h>
//#include <unistd.h>
//#include <sys/stat.h>
//#include <iostream>
//#include <fstream>

/*! \file datalib.h
*	\brief Data Management header file
*/

//! \ingroup support
//! \defgroup datalib Data Management support library
//! Data Management support library.
//!
//! Data within COSMOS is managed in a heirarchical structure that
//! mirrors the Node:Agent arrangement laid down in the \ref nodelib.
//! Directories are defined for storage of general use information, as
//! well as information specific to each Node. Separate directories are
//! also defined for Incoming data, Outgoing data, and Archival data.
//! The top level directory of this Node information is defined as the
//! "cosmosnodes". "cosmosnodes" contains all data specific to each Node. Within
//! "nodes", there is a directory named after each Node. This directory
//! can be discovered automatically by the software if it is within 4 levels
//! above the current directory, or if an environment variable, "NODEBASE",
//! is set. The environment variable takes precedence.
//!
//! Within each Node directory is a
//! set of initialization files covering different aspects of the Node, as well as sub-directories
//! for "incoming", "outgoing", "temp", and "data" archive files.
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
//! Each nodes sub-directories contain sub-directories of their own for each Agent. The "data"
//! directories Agent directories are further subdivided by first
//! year, then day.
//!
//! Resources for COSMOS are stored in a second heirarchy of directories.
//! The highest level is a top level directory, "cosmosresources". This
//! directory can also be discovered by the software, if it is within 4
//! levels above the current directory. It can also be set through use
//! of the environment variable, "COSMOSBASE".
//!
//! "cosmosresources" contains directories of files for use in different
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
//! \defgroup datalib_functions Data Management support library function declarations
//! @{

void log_reopen();
void log_write(string node, int type, double utc, const char* data);
void log_write(string node, string agent, double utc, string type, const char *data);
void log_write(string node, string agent, double utc, string extra, string type, string record);
void log_move(string node, string agent);
int check_events(eventstruc* events, int max, cosmosstruc* data);
int32_t data_get_nodes(vector<cosmosstruc> &data);
vector<string> data_list_nodes();
int32_t data_list_nodes(vector<string>& nodes);
vector<filestruc> data_list_files(string node, string location, string agent);
vector<filestruc> data_list_archive(string node, string agent, double utc);
int32_t data_list_files(string node, string location, string agent, vector<filestruc>& files);
FILE* data_open(string path, char* mode);
string data_name(string node, double mjd, string extra, string type);
string data_name(string node, double mjd, string type);
string data_base_path(string node);
string data_base_path(string node, string location);
string data_base_path(string node, string location, string agent);
string data_base_path(string node, string location, string agent, string filename);
string data_archive_path(string node, string agent, double mjd);
string data_archive_path(string node, string agent, double mjd, string filename);
string data_type_path(string node, string location, string agent, double mjd, string type);
string data_extra_type_path(string node, string location, string agent, double mjd, string extra, string type);
string data_name_path(string node, string location, string agent, double mjd, string name);
bool data_exists(string& path);
void set_cosmosresources(string name);
string get_cosmosresources();
void set_cosmosnodes(string name);
string get_cosmosnodes();
string get_resdir();
string get_nodedir();
string get_cnodedir(string node);
string set_cnodedir(string node);
int32_t data_load_archive(double mjd, vector<string> &telem, vector<string> &event, cosmosstruc* root);
double findlastday(string node);
double findfirstday(string node);
int32_t kml_write(cosmosstruc* cdata);

//! @}

#endif
