#ifndef _NODELIB_H
#define _NODELIB_H 1

#include "configCosmos.h"

#include "cosmos-defs.h"
#include "nodedef.h"
#include "jsondef.h"
#include "datalib.h"

/*! \file nodelib.h
*	\brief Node Library header file
*/

//! \ingroup support
//! \defgroup nodelib Node Library
//! Node support library.
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
//! \ingroup nodelib
//! \defgroup nodelib_functions Node Library functions
//! @{

int32_t node_init(const char *name, cosmosstruc *data);
int32_t node_calc(cosmosstruc *data);
void create_databases(cosmosstruc *root);
void load_databases(char *name, uint16_t type, cosmosstruc *root);
int load_dictionary(vector<shorteventstruc> &dict, cosmosstruc *root, char *file);
int load_target(cosmosstruc *root);
int update_target(cosmosstruc *root);
uint32_t calc_events(vector<shorteventstruc> &dictionary, cosmosstruc *root, vector<shorteventstruc> &events);

//! @}

#endif
