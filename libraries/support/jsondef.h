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

#ifndef _JSONDEF_H
#define _JSONDEF_H 1

/*! \file jsonlib.h
* \brief JSON Support definitions
*/

#include "configCosmos.h"

#include "mathlib.h"
#include "convertdef.h"
#include "physicsdef.h"
#include "socketlib.h"

//! \ingroup jsonlib
//! \defgroup jsonlib_type JSON Name Space variable type constants
//! @{

//! JSON Unit conversion type
enum
	{
	//! Identity
	JSON_UNIT_TYPE_IDENTITY,
	//! Polynomial
	JSON_UNIT_TYPE_POLY,
	//! Logarithm
	JSON_UNIT_TYPE_LOG
	};

//! JSON Unit type
enum
	{
	//! No units
	JSON_UNIT_NONE,
	//! Length
	JSON_UNIT_LENGTH,
	//! Mass
	JSON_UNIT_MASS,
	//! Time
	JSON_UNIT_TIME,
	//! Current
	JSON_UNIT_CURRENT,
	//! Temperature
	JSON_UNIT_TEMPERATURE,
	//! Intensity
	JSON_UNIT_INTENSITY,
	//! Area
	JSON_UNIT_AREA,
	//! Volume
	JSON_UNIT_VOLUME,
	//! Speed
	JSON_UNIT_SPEED,
	//! Acceleration
	JSON_UNIT_ACCELERATION,
	//! Density
	JSON_UNIT_DENSITY,
	//! Magnetic Field Strengh
	JSON_UNIT_MAGFIELD,
	//! Luminance
	JSON_UNIT_LUMINANCE,
	//! Angle
	JSON_UNIT_ANGLE,
	//! Solid Angle
	JSON_UNIT_SOLIDANGLE,
	//! Frequency
	JSON_UNIT_FREQUENCY,
	//! Force
	JSON_UNIT_FORCE,
	//! Pressure
	JSON_UNIT_PRESSURE,
	//! Energy
	JSON_UNIT_ENERGY,
	//! Charge
	JSON_UNIT_CHARGE,
	//! Electric Potential
	JSON_UNIT_VOLTAGE,
	//! Capacitance
	JSON_UNIT_CAPACITANCE,
	//! Electrical Resistance
	JSON_UNIT_RESISTANCE,
	//! Magnetic FLux
	JSON_UNIT_MAGFLUX,
	//! Magnetic Flux Density
	JSON_UNIT_MAGDENSITY,
	//! Specific Impulse
	JSON_UNIT_ISP,
	//! Holder for number of entries
	JSON_UNIT_COUNT
	} ;

//! JSON Namelist Group
enum
	{
	//! Absolute pointer
	JSON_GROUP_ABSOLUTE,
	//! ::nodestruc
	JSON_GROUP_NODE,
	//! ::agentstruc
	JSON_GROUP_AGENT,
	//! ::devicestruc
	JSON_GROUP_DEVICE,
	//! ::devspecstruc
	JSON_GROUP_DEVSPEC,
	//! ::physstruc
	JSON_GROUP_PHYSICS,
	//! ::eventstruc
	JSON_GROUP_EVENT,
	//! ::piecestruc
	JSON_GROUP_PIECE,
	//! ::targetstrunc
	JSON_GROUP_TARGET,
	//! ::userstruc
	JSON_GROUP_USER,
	//! ::portstruc
	JSON_GROUP_PORT,
	//! ::glossarystruc
	JSON_GROUP_GLOSSARY,
	//! ::tlestruc
	JSON_GROUP_TLE,
	//! ::aliasstruc
	JSON_GROUP_ALIAS,
	//! ::equationstruc
	JSON_GROUP_EQUATION
	};

//! Constants defining the data types supported in the \ref jsonlib_namespace.
enum
	{
	//! JSON 8 bit unsigned integer type
	JSON_TYPE_UINT8=1,
	//! JSON 8 bit signed integer type
	JSON_TYPE_INT8,
	//! JSON 32 bit unsigned integer type
	JSON_TYPE_UINT32,
	//! JSON 16 bit unsigned integer type
	JSON_TYPE_UINT16,
	//! JSON 16 bit integer type
	JSON_TYPE_INT16,
	//! JSON 32 bit integer type
	JSON_TYPE_INT32,
	//! JSON single precision floating point type
	JSON_TYPE_FLOAT,
	//! JSON double precision floating point type
	JSON_TYPE_DOUBLE,
	//! JSON string type
	JSON_TYPE_STRING,
	//! JSON Name type
	JSON_TYPE_NAME,
	//! JSON ::rvector
	JSON_TYPE_RVECTOR,
	//! JSON 3 element ::rvector
	JSON_TYPE_TVECTOR,
	//! JSON ::cvector
	JSON_TYPE_CVECTOR,
	//! JSON ::quaternion type
	JSON_TYPE_QUATERNION,
	//! JSON ::gvector
	JSON_TYPE_GVECTOR,
	//! JSON ::svector
	JSON_TYPE_SVECTOR,
	//! JSON 3x3 ::rmatrix
	JSON_TYPE_DCM,
	//! JSON ::rmatrix
	JSON_TYPE_RMATRIX,
	//! JSON ::cartpos
	JSON_TYPE_CARTPOS,
	//! JSON ::qatt
	JSON_TYPE_QATT,
	//! JSON ::dcmatt
	JSON_TYPE_DCMATT,
	//! JSON ::posstruc
	JSON_TYPE_POSSTRUC,
	//! JSON ::attstruc
	JSON_TYPE_ATTSTRUC,
	//! JSON Agent Heartbeat
	JSON_TYPE_HBEAT,
	//! JSON Solar Barycentric Position
	JSON_TYPE_POS_BARYC,
	//! JSON Earth Centered Inertial Position
	JSON_TYPE_POS_ECI,
	//! JSON Lunar Centered Inertial Position
	JSON_TYPE_POS_SCI,
	//! JSON Geocentric Position
	JSON_TYPE_POS_GEOC,
	//! JSON Selenocentric Position
	JSON_TYPE_POS_SELC,
	//! JSON Geodetic Position
	JSON_TYPE_POS_GEOD,
	//! JSON Geocentric Spherical
	JSON_TYPE_POS_GEOS,
	//! JSON Selenographic Position
	JSON_TYPE_POS_SELG,
	//! JSON Topocentric Attitude
	JSON_TYPE_QATT_TOPO,
	//! JSON Geocentric Attitude
	JSON_TYPE_QATT_GEOC,
	//! JSON Lunar Centered Inertial Attitude
	JSON_TYPE_QATT_SELC,
	//! JSON Earth Centered Inertial Attitude
	JSON_TYPE_QATT_ICRF,
	//! JSON LVLH Attitude
	JSON_TYPE_QATT_LVLH,
	//! JSON Satellite Position Structure
	JSON_TYPE_LOC_POS,
	//! JSON Satellite Attitude Structure
	JSON_TYPE_LOC_ATT,
	//! JSON ::locstruc type
	JSON_TYPE_LOC,
	//! JSON Timestamp
	JSON_TYPE_TIMESTAMP,
	//! JSON Equation
	JSON_TYPE_EQUATION,
	//! JSON Alias
	JSON_TYPE_ALIAS
	};

//! Types of equation operands
enum
	{
	//! Nothing at all
	JSON_OPERAND_NULL,
	//! A simple constant
	JSON_OPERAND_CONSTANT,
	//! A Namespace name
	JSON_OPERAND_NAME,
	//! Another equation
	JSON_OPERAND_EQUATION
	};

//! Types of equation operations
enum
	{
	//! Addition
	JSON_OPERATION_ADD,
	//! Subtraction
	JSON_OPERATION_SUBTRACT,
	//! Multiplication
	JSON_OPERATION_MULTIPLY,
	//! Division
	JSON_OPERATION_DIVIDE,
	//! Modulo
	JSON_OPERATION_MOD,
	//! Boolean And
	JSON_OPERATION_AND,
	//! Boolean Or
	JSON_OPERATION_OR,
	//! Boolean Greater Than
	JSON_OPERATION_GT,
	//! Boolean Less Than
	JSON_OPERATION_LT,
	//! Boolean Equal
	JSON_OPERATION_EQ,
	//! Logical Not
	JSON_OPERATION_NOT,
	//! Complement
	JSON_OPERATION_COMPLEMENT,
	//! Power
	JSON_OPERATION_POWER
	};

#define HCAP 800.

#define MAREA (.0027)
#define MWIDTH .2167
#define MHEIGHT .8255
#define MRADIUS .2616

#define LEO1_BOTTOM 16
#define LEO1_TOP 17

//! Maximum number of structures
#define MAXPART 256
//! Maximum number of PV strings
#define MAXSTRING 20
//! maximum number of batteries
#define MAXBATT 15
//! Maximum number of points in a structure
#define MAXPNT 8
//! Maximum number of components
#define MAXCOMP 128
//! Maximum number of Ground Stations
#define MAXTRACK 30
//! Maximum number of temperature sensors
#define MAXTSEN 64
//! Maximum number of Sun Sensors
#define MAXSS 2
//! Maximum number of Inertial Measurement Units
#define MAXIMU 2
//! Maximum number of Star Trackers
#define MAXSTT 2
//! Maximum number of Reaction Wheels
#define MAXRW 2
//! Maximum number of Magnetic Torque Rods
#define MAXMTR 3
//! Maximum number of Processors
#define MAXCPU 6
//! Maximum number of GPS units
#define MAXGPS 2
//! Maximum number of Payloads
#define MAXPLOAD 6
//! Maximum number of Power Buses
#define MAXBUS 40
//! Maximum number of Propellent Tanks
#define MAXPROP 5
//! Maximum number of Thrusters
#define MAXTHST 5
//! Maximum number of Motors
#define MAXMOTR 8
//! Maximum number of Switches
#define MAXSWCH 2
//! Maximum number of antennas
#define MAXANT 5
//! Maximum number of transceivers
#define MAXTCV 2
//! Maximum number of Motion Capture Cameras
#define MAXMCC 3
//! Maximum number of Special Payload Keys
#define MAXPLOADKEYCNT 10
//! Maximum size of a Payload Key
#define MAXPLOADKEYSIZE COSMOS_MAX_NAME

//! @}

//! \ingroup jsonlib
//! \defgroup jsonlib_constants JSON Constants
//! @{

//! Largest JSON data
#define JSON_MAX_DATA (COSMOS_MAX_DATA)
//! Largest JSON name
#define COSMOS_MAX_NAME 40
//! Maximum value of JSON HASH
#define JSON_MAX_HASH (COSMOS_MAX_NAME*37)
//! Maximum JSON buffer
#define JSTRINGMAXBUFFER (AGENTMAXBUFFER-2)
//! Maximum number of ::cosmosstruc elements
#define MAX_COSMOSSTRUC 20

//! Entire ::cosmosstruc
#define JSON_MAP_ALL 0
//! ::agentstruc part of ::cosmosstruc
#define JSON_MAP_AGENT 1
//! ::beaconstruc part of ::cosmosstruc
#define JSON_MAP_BEACON 2
//! ::nodestruc part of ::cosmosstruc
#define JSON_MAP_NODESTATIC 3
//! ::nodestruc part of ::cosmosstruc
#define JSON_MAP_NODEDYNAMIC 4
//! ::eventstruc part of ::cosmosstruc
#define JSON_MAP_EVENT 5
//! ::cosmosstruc_s part of ::cosmosstruc
#define JSON_MAP_STATIC 7
//! ::cosmosstruc_d part of ::cosmosstruc
#define JSON_MAP_DYNAMIC 8

//! Maximum AGENT transfer buffer size
#define AGENTMAXBUFFER 60000
//! Maximum number of supported publication interfaces
#define AGENTMAXIF 7
//! Maximum number of builtin AGENT requests
#define AGENTMAXBUILTINCOUNT 6
//! Maximum number of user defined AGENT requests
#define AGENTMAXUSERCOUNT 40
//! Maximum number of AGENT requests
#define AGENTMAXREQUESTCOUNT (AGENTMAXBUILTINCOUNT+AGENTMAXUSERCOUNT)

//! @}
//! \ingroup defs
//! \defgroup defs_comp Constants defining Components.

//! \ingroup defs_comp
//! \defgroup defs_comp_type Type of Component.
//! @{
enum
	{
	//! Payload
	DEVICE_TYPE_PLOAD=0,
	//! Elevation and Azimuth Sun Sensor
	DEVICE_TYPE_SSEN=1,
	//! Inertial Measurement Unit
	DEVICE_TYPE_IMU=2,
	//! Reaction Wheel
	DEVICE_TYPE_RW=3,
	//! Magnetic Torque Rod
	DEVICE_TYPE_MTR=4,
	//! Processing Unit
	DEVICE_TYPE_CPU=5,
	//! GPS Unit
	DEVICE_TYPE_GPS=6,
	//! Antenna
	DEVICE_TYPE_ANT=7,
	//! Radio Receiver
	DEVICE_TYPE_RXR=8,
	//! Radio Transmitter
	DEVICE_TYPE_TXR=9,
	//! Radio Transceiver
	DEVICE_TYPE_TCV=10,
	//! Photo Voltaic String
	DEVICE_TYPE_STRG=11,
	//! Battery
	DEVICE_TYPE_BATT=12,
	//! Heater
	DEVICE_TYPE_HTR=13,
	//! Motor
	DEVICE_TYPE_MOTR=14,
	//! Temperature Sensor
	DEVICE_TYPE_TSEN=15,
	//! Thruster
	DEVICE_TYPE_THST=16,
	//! Propellant Tank
	DEVICE_TYPE_PROP=17,
	//! Switch
	DEVICE_TYPE_SWCH=18,
	//! Rotor
	DEVICE_TYPE_ROT=19,
	//! Star Tracker
	DEVICE_TYPE_STT=20,
	//! Motion Capture Camera
	DEVICE_TYPE_MCC=21,
	//! Torque rod Control Unit
	DEVICE_TYPE_TCU=22,
	//! Power Bus
	DEVICE_TYPE_BUS=23,
	//! Pressure sensor
	DEVICE_TYPE_PSEN=24,
	//! SUCHI
	DEVICE_TYPE_SUCHI=25,
	//! Camera
	DEVICE_TYPE_CAM=26,
	//! Telemetry
	DEVICE_TYPE_TELEM=27,
	//! List count
	DEVICE_TYPE_COUNT,
	//! Not a Component
	DEVICE_TYPE_NONE=65535
	};

enum
	{
	TELEM_TYPE_UINT8,
	TELEM_TYPE_INT8,
	TELEM_TYPE_UINT16,
	TELEM_TYPE_INT16,
	TELEM_TYPE_UINT32,
	TELEM_TYPE_INT32,
	TELEM_TYPE_FLOAT,
	TELEM_TYPE_DOUBLE,
	TELEM_TYPE_STRING
	};

//! @}

//! \ingroup defs_comp
//! \defgroup defs_comp_model Model of Device.
//! @{
//! Undefined
#define COMP_MODEL_UNDEFINED 0
//! VN100 IMU
#define COMP_MODEL_IMU_VN100 1
//! Microstrain IMU
#define COMP_MODEL_IMU_MICROSTRAIN 2
//! HCC IMU
#define COMP_MODEL_IMU_HCC 3
//! @}

//! \ingroup defs_comp
//! \defgroup defs_comp_flag Component flags.
//! @{
#define DEVICE_FLAG_OFF			0x0000
#define DEVICE_FLAG_ON			0x0001
#define DEVICE_FLAG_SIMULATED	0x0002
#define DEVICE_FLAG_ACTIVE      0x0004
#define DEVICE_FLAG_CONNECTED   0x0008
//! @}

//! \ingroup defs_comp
//! \defgroup defs_comp_port IO Port types.
//! @{
//!
enum
	{
	PORT_TYPE_RS232 = 0,
	PORT_TYPE_RS422 = 1,
	PORT_TYPE_ETHERNET = 2,
	PORT_TYPE_USB = 3,
	PORT_TYPE_COUNT,
	PORT_TYPE_NONE = 65535
	};

//! @}
//! @}

//! \ingroup jsonlib
//! \defgroup jsonlib_typedefs JSON typedefs
//! @{

//! JSON unit type entry
/*! To be used in the ::json_map table of units. Each entry represents
 * one specific variant of one type of unit. Each entry includes:
 * - a name for the unit
 * - a type for the unit conversion: 0 = identity, 1 = linear, 2 = log
 * - 0th, 1st and 2nd derivative terms for any conversion
 */
struct unitstruc
{
	//! JSON Unit Name
	string name;
	//! JSON Unit conversion type
	uint16_t type;
	//! 0th derivative term
	float p0;
	//! 1th derivative term
	float p1;
	//! 2th derivative term
	float p2;
};

//! JSON map offset entry
/*! Single entry in a JSON offset map. Ties together a single JSON name and a offset
 * to a single object, along with its data type.
 * - index: Index of this entry in the ::json_map.
 * - data: Offset to appropriate storage for this data type.
*/
struct jsonentry
{
	//! JSON Data Type
	uint16_t type;
	//! JSON Data Group
	uint16_t group;
	//! Name of entry
	string name;
	//! offset to data storage
	ptrdiff_t offset;
	//! size of data storage
	size_t size;
	//! vector of actual data
	vector <uint8_t> data;
	//! Index to JSON Unit Type
	uint16_t unit_index;
	//! Index to alert condition in Data Dictionary
	uint16_t alert_index;
	//! Index to alarm condition in Data Dictionary
	uint16_t alarm_index;
	//! Index to maximum condition in Data Dictionary
	uint16_t maximum_index;
	//! Index to minimum condition in Data Dictionary
	uint16_t minimum_index;
	//! Index to subsystem
	uint16_t subsystem;
};

//! JSON handle
/*! Structure representing the location of a single JSON Equation or Name in its respective
	hash table.
*/
struct jsonhandle
{
	// Hash of equation or name
	uint16_t hash;
	// Index within that hash entry
	uint16_t index;
};

//! JSON token
/*! Tokenized version of a single JSON object. The token contains a pointer
 * into the JSON map, and a binary representation of the JSON value.
 */
struct json_token
{
	jsonentry entry;
	vector <uint8_t> data;
};

//! JSON equation operand
/*! Structure representing a single operand of a JSON equation. Each operand can be one
	of: JSON_OPERAND_NULL, no operand; JSON_OPERAND_EQUATION, another equation;
	JSON_OPERAND_CONSTANT, a constant value; or JSON_OPERAND_NAME, a Namespace name.
*/
struct jsonoperand
{
	//! JSON Operand Type
	uint16_t type;
	//! JSON Operand data: CONSTANT uses value, EQUATION and NAME use ::jsonhandle.
	union
	{
		double value;
		jsonhandle data;
	};
};

//! JSON equation entry
/*! Single entry in a JSON equation map. Ties together a JSON equation and its
 * preparsed form.
 * - index: Index of this entry in the ::json_map.
 * - data: Offset to appropriate storage for this data type.
*/
struct jsonequation
{
	//! JSON equation text
	char *text;
	//! Index to JSON Unit Type
	uint16_t unit_index;
	//! JSON equation operation
	uint16_t operation;
	//! JSON equation operands
	jsonoperand operand[2];
};

//! JSON pointer map
/*! The complete JSON offset map consists of an array of ::jsonentry elements, along
 * with their count. It also provides a dynamically sized char string, used by
 * the JSON output functions, and an index of its length.
*/
struct jsonmap
{
	//! Array of entries
	vector<vector<jsonentry> > entry;
};

//! Agent Request Function
//! Format of a user supplied function to handle a given request
typedef int32_t (*agent_request_function)(char* request_string, char* output_string, void *root);

//! Agent Request Entry
//! Structure representing a single Agent request.
struct agent_request_entry
{
	//! Character token for request
	char token[COSMOS_MAX_NAME];
	//! Pointer to function to call with request string as argument and returning any error
	agent_request_function function;
	string synopsis;
	string description;
};

//! Channel structure
//! This structure stores the information about an open COSMOS network channel.
struct agent_channel
{
	// Channel type
	int32_t type;
	// Channel UDP socket handle
	int32_t cudp;
	// Channel UDP INET4 address
	struct sockaddr_in caddr;
	// Channel UDP INET4 broadcast address
	struct sockaddr_in baddr;
	// Channel UDP INET6 address
	struct sockaddr_in6 caddr6;
	// Length for chosen address
	int addrlen;
	// Channel port
	uint16_t cport;
	// Channel's maximum message size
	uint16_t msgsize;
	// Channel's protocol address in string form
	char address[17];
	// Channel's broadcast address in string form
	char baddress[17];
	// Channel's interface name
	char name[COSMOS_MAX_NAME];
};

//! Process heartbeat.
//! Detailed elements of a single heartbeat of a single process.
struct beatstruc
{
	// Heartbeat timestamp
	double utc;
	// Heartbeat Node Name
	char node[COSMOS_MAX_NAME+1];
	//! Heartbeat Agent Name
	char proc[COSMOS_MAX_NAME+1];
	// Type of address protocol
	uint16_t ntype;
	//! Protocol Address
	char addr[17];
	//! AGENT port
	uint16_t port;
	//! Transfer buffer size
	uint32_t bsz;
	//! Heartbeat period in seconds
	double bprd;
	//! Agent User Name
	char user[COSMOS_MAX_NAME+1];
};

//! Agent control structure
struct agentstruc
{
	//! Client initialized?
	bool client;
	//! Subscription channel (for Client)
	socket_channel sub;
	//! Server initialized?
	bool server;
	//! Number of network interfaces
	int ifcnt;
	//! Publication channels for each interface (for Server)
	socket_channel pub[AGENTMAXIF];
	//! Request channel (for Server)
	socket_channel req;
	//! Agent process ID
	int32_t pid;
	//! Activity period in seconds
	double aprd;
	//! Agent Running State Flag
	uint16_t stateflag;
	//! State of Health report string
	//	char sohstring[AGENTMAXBUFFER];
	//! Agent request list
	vector <agent_request_entry> reqs;
	//! Heartbeat
	beatstruc beat;
	//! State of Health element vector
	vector<jsonentry*> sohtable;
};

//! Long COSMOS Event structure.
/*! This is the structure that holds each Event, along with associated
 * resources.
 */
struct longeventstruc
{
	//! Time event is to start.
	double utc;
	//! Time event was executed.
	double utcexec;
	//! Node for event
	char node[COSMOS_MAX_NAME];
	//! Name of event.
	char name[COSMOS_MAX_NAME];
	//! User of event.
	char user[COSMOS_MAX_NAME];
	//! Event flags.
	uint32_t flag;
	//! Event type.
	uint32_t type;
	//! Value of condition
	double value;
	//! Event initial time consumed.
	double dtime;
	//! Event continuous time consumed.
	double ctime;
	//! Event initial energy consumed.
	float denergy;
	//! Event continuous energy consumed.
	float cenergy;
	//! Event initial mass consumed.
	float dmass;
	//! Event continuous mass consumed.
	float cmass;
	//! Event initial bytes consumed.
	float dbytes;
	//! Event continuous bytes consumed.
	float cbytes;
	//! Handle of condition that caused event, NULL if timed event.
	jsonhandle handle;
	//! Event specific data.
	char data[JSON_MAX_DATA];
	//! Condition that caused event, NULL if timed event.
	char condition[JSON_MAX_DATA];
};

//! Shortened COSMOS Event structure
/*! This is the structure that holds each Event, along with associated
 * resources, in a shorter format.
 */
struct shorteventstruc
{
	//! Time event is to start.
	double utc;
	//! Time event was executed.
	double utcexec;
	//! Node for event
	char node[COSMOS_MAX_NAME];
	//! Name of event.
	char name[COSMOS_MAX_NAME];
	//! User of event.
	char user[COSMOS_MAX_NAME];
	//! Event flags.
	uint32_t flag;
	//! Event type.
	uint32_t type;
	//! Value of condition
	double value;
	//! Event initial time consumed.
	double dtime;
	//! Event continuous time consumed.
	double ctime;
	//! Event initial energy consumed.
	float denergy;
	//! Event continuous energy consumed.
	float cenergy;
	//! Event initial mass consumed.
	float dmass;
	//! Event continuous mass consumed.
	float cmass;
	//! Event initial bytes consumed.
	float dbytes;
	//! Event continuous bytes consumed.
	float cbytes;
	//! Handle of condition that caused event, NULL if timed event.
	jsonhandle handle;
	//! Event specific data.
	char data[COSMOS_MAX_NAME];
};

//! Full COSMOS Event structure
/*! This is the union that holds each Event, along with associated
 * resources, in both formats.
 */
union eventstruc
{
	shorteventstruc s;
	longeventstruc l;
};

//! User structure
/*! Containing entries unique to each User
 */
struct userstruc
{
	char name[COSMOS_MAX_NAME];
	char node[COSMOS_MAX_NAME];
	char tool[COSMOS_MAX_NAME];
	char cpu[COSMOS_MAX_NAME];
};

//! Glossary structure
/*! Contains entries for a glossary of names and matching descriptions
and types.
*/
struct glossarystruc
{
	// Glossary entry name.
	string name;
	// Glossary entry description, to be interpreted based on its type.
	string description;
	// Glossary entry ::namespace type.
	uint16_t type;
};

//! Alias structure
/*! Contains the name of an alias and the ::jsonmap entry it points to,
 * stored as a ::jsonhandle.
*/
struct aliasstruc
{
	// Alias name
	string name;
	// Namespace handle
	jsonhandle handle;
};

//! Equation structure
/*! Contains the name of an equation and the Equation string it points to.
*/
struct equationstruc
{
	// Equation name
	string name;
	// Equation string
	string value;
};


//! Target structure
/*! Contains entries unique to each Target
*/
struct targetstruc
{
	double utc;
	char name[COSMOS_MAX_NAME];
	uint16_t type;
	float azfrom;
	float elfrom;
	float azto;
	float elto;
	double range;
	double close;
	float min;
	locstruc loc;
};

//! Port structure
/*! Contains information about I/O ports available to devices. The
 * ::portidx entry in each device structure indicates which of these
 * ports a device will use.
 */
struct portstruc
{
	//! Type of I/O as listed in ::def_comp_port.
	uint16_t type;
	//! Name information for port.
	char name[COSMOS_MAX_NAME];
};

//! Part structure: physical information for each piece of Node
struct piecestruc
{
	//! Name of piece
	char name[COSMOS_MAX_NAME];
	//! Type of piece from \ref defs_piece.
	uint16_t type;
	//! Component index: -1 if not a Component
	uint16_t cidx;
	//! Mass in kg
	float mass;
	//! Emissivity: 0-1
	float emi;
	//! Absorptivity: 0-1
	float abs;
	//! Heat capacity in joules per kelvin
	float hcap;
	//! Heat conductivity in Watts per meter per kelvin
	float hcon;
	//! Dimension in meters: effect is dependent on Part type
	float dim;
	//! Area in square meters
	float area;
	//! Number of vertices/points
	uint16_t pnt_cnt;
	//! Array of vertices/points
	rvector points[MAXPNT];
	//! Normal to panel if external
	rvector normal;
	//! Centroid of piece
	rvector centroid;
	//! Contribution of piece to linear forces
	rvector shove;
	//! Contribution of piece to angular forces
	rvector twist;
	//! Stored thermal energy
	float heat;
	//! Temperature in Kelvins
	float temp;
	//! Insolation in Watts/sq m
	float insol;
};

// Beginning of Device General structures

//! General Component structure
/*! These are the elements that are repeated in all devices. Each
device specific structure has these as its first elements, followed by
any specific elements. They are all then combined together in one grand
union as a ::devicestruc.
*/
struct genstruc
{
	//! Component Type
	uint16_t type;
	//! Device Model
	uint16_t model;
	//! Device flag
	uint16_t flag;
	//! Component Index
	uint16_t cidx;
	//! Device specific index
	uint16_t didx;
	//! Piece index
	uint16_t pidx;
	//! Power Bus index
	uint16_t bidx;
	//! Connection information for device.
	uint16_t portidx;
	//! Nominal Amperage
	float namp;
	//! Nominal Voltage
	float nvolt;
	//! Current Amperage
	float amp;
	//! Current Voltage
	float volt;
	//! Current Temperature
	float temp;
	//! Device information time stamp
	double utc;
};

//! All structures.
/*! This structure can be used for all devices. It contains only a ::genstruc
 */
struct allstruc
{
	genstruc gen;
};

// End of Device General structures

// Beginning of Device Specific structures

//! Telemetry (TELEM) structure.
/*! This provides access to a union of a variety of possible data types, along
 * with an optional indicator of what type the data actually is. The same data
 * is then accessible as all supported data types, both in the structure and the
 * Namespace.
 */
struct telemstruc
{
	//! Generic info
	genstruc gen;
	//! Data type
	uint16_t type;
	//! Union of data
	union
	{
		uint8_t vuint8;
		int8_t vint8;
		uint16_t vuint16;
		int16_t vint16;
		uint32_t vuint32;
		int32_t vint32;
		float vfloat;
		double vdouble;
		char vstring[COSMOS_MAX_NAME];
	};
} ;

//! Payload (PLOAD) structure.
/*! You can define up to ::MAXPLOADKEYCNT keys by giving them unique
 * names. You can then set double precision values for these keys in
 * the dynamic structure.
 */
struct ploadstruc
{
	//! Generic info
	genstruc gen;
	//! Number of keys being used.
	uint16_t key_cnt;
	//! Name for each key.
	uint16_t keyidx[MAXPLOADKEYCNT];
	//! Rate at which the payload generates data.
	uint32_t bps;
	//! Value for each key.
	float keyval[MAXPLOADKEYCNT];
};

//! Sun Sensor (SSEN) Sructure
struct ssenstruc
{
	//! Generic info
	genstruc gen;
	//! Sensor alignment quaternion.
	quaternion align;
	float qva;
	float qvb;
	float qvc;
	float qvd;
	float azimuth;
	float elevation;
};

//! Inertial Measurement Unit (IMU) structure
struct imustruc
{
	//! Generic info
	genstruc gen;
	//! alignment quaternion
	quaternion align;
	//! Position acceleration vector
	rvector accel;
	//! Attitude rate vector
	rvector omega;
	//! Attitude acceleration vector
	rvector alpha;
	//! Magnetic field in sensor frame
	rvector mag;
	//! Magnetic field rate change in sensor frame
	rvector bdot;
};

//! Reaction Wheel structure: z axis is aligned with axis of rotation.
struct rwstruc
{
	//! Generic info
	genstruc gen;
	//! Rotates vectors from RW frame (axis of rotation = z) to body frame.
	quaternion align;
	//! Moments of inertia in RW frame.
	rvector mom;
	//! Maximum omega in radians/second.
	float mxomg;
	//! Maximum alpha in radians/second/second.
	float mxalp;
	//! Acceleration Time Constant
	float tc;
	//! Current angular velocity
	float omg;
	//! Current angular acceleration
	float alp;
	//! Requested angular velocity
	float romg;
	//! Requested angular acceleration
	float ralp;
};

//! Magnetic Torque Rod structure: z axis is aligned with rod.
struct mtrstruc
{
	//! Generic info
	genstruc gen;
	//! Rotates vectors from MTR frame to Body frame.
	quaternion align;
	//! Terms of 6th order polynomial converting negative moment to current
	float npoly[7];
	//! Terms of 6th order polynomial converting positive moment to current
	float ppoly[7];
	//! Maxiumum field strength
	float mxmom;
	//! Field Change Time Constant
	float tc;
	//! Requested Magnetic Moment.
	float rmom;
	//! Actual Magnetic Moment.
	float mom;
};

struct cpustruc
{
	//! Generic info
	genstruc gen;
	//! Seconds CPU has been up
	uint32_t uptime;
	//! Maximum disk capacity in GiB
	float maxdisk;
	//! Maximum memory capacity in GiB
	float maxmem;
	//! Maximu load
	float maxload;
	//! Current disk usage in GiB
	float disk;
	//! Current memory usage in GiB
	float mem;
	//! Current load
	float load;
	//! Number of reboots
	uint32_t boot_count;
};


struct gpsstruc
{
	//! Generic info
	genstruc gen;
	//! UTC time error
	double dutc;
	//! Geocentric position: x, y, z
	rvector geocs;
	//! Geocentric velocity: x, y, z
	rvector geocv;
	//! Geocentric position error: x, y, z
	rvector dgeocs;
	//! Geocentric velocity error: x, y, z
	rvector dgeocv;
	//! Geodetic position: longitude, latitude, altitude
	gvector geods;
	//! Geodetic velocity: longitude, latitude, altitude
	gvector geodv;
	//! Geodetic position error: longitude, latitude, altitude
	gvector dgeods;
	//! Geodetic velocity error: longitude, latitude, altitude
	gvector dgeodv;
	//! GPS heading
	float heading;
	//! number of satellites used by GPS receiver
	uint16_t n_sats_used;
	//! number of satellites visible by GPS receiver
	uint16_t n_sats_visible;
	//! Time Status
	uint16_t time_status;
	//! Position Type
	uint16_t position_type;
	//! Solution Status
	uint16_t solution_status;
};

//! Antenna information
/*! This structure holds the information concerning antennas.
*/
struct antstruc
{
	//! Generic info
	genstruc gen;
	//! Alignment
	quaternion align;
	//! Minimum elevation
	float minelev;
};

//! Receiver information
/*! This structure holds the information concerning receivers.
*/
struct rxrstruc
{
	//! Generic info
	genstruc gen;
	//! Frequency
	float freq;
};

//! Transmitter information
/*! This structure holds the information concerning transmitters.
*/
struct txrstruc
{
	//! Generic info
	genstruc gen;
	//! Frequency
	float freq;
};

//! Transceiver information
/*! This structure holds the information concerning transceivers.
*/
struct tcvstruc
{
	//! Generic info
	genstruc gen;
	//! Input Frequency
	float freqin;
	//! Output Frequency
	float freqout;
};

//! PV String (STRG) structure.
/*! Efficiency is goven as effbase + effslope * Tkelvin.
*/
struct strgstruc
{
	//! Generic info
	genstruc gen;
	//! Efficiency 0th order term
	float effbase;
	//! Efficiency 1st order term
	float effslope;
	//! Maximum power generation in Watts
	float maxpower;
	//! Current power being generated in Watts.
	float power;
};

//! Battery (BATT) structure.
struct battstruc
{
	//! Generic info
	genstruc gen;
	//! Capacity in amp hours
	float capacity;
	//! Charge conversion efficiency
	float efficiency;
	//! Charge in amp hours
	float charge;
};

//! Heater Structure definition
/*! This structure holds the description of a heaters.
*/
struct htrstruc
{
	//! Generic info
	genstruc gen;
	//! Temperature set point
	float setpoint;
};

struct motrstruc
{
	//! Generic info
	genstruc gen;
	//! Maximum speed in revolutions per second
	float max;
	//!
	float rat;
	float spd;
};

struct tsenstruc
{
	//! Generic info
	genstruc gen;
};

//! Thruster (THST) dynamic structure
struct thststruc
{
	//! Generic info
	genstruc gen;
	//! Flow
	quaternion align;
	//! Specific Impulse in dynes per kg per second
	float flw;
	//! Rotation of thrust vector (+z) in to node frame.
	float isp;
};

//! Propellant Tank (PROP) structure.
struct propstruc
{
	//! Generic info
	genstruc gen;
	//! Propellant capacity in kg
	float cap;
	//! Propellant level in kg
	float lev;
};

//! Switch Structure definition
/*! This structure holds the description of a switches.
*/
struct swchstruc
{
	//! Generic info
	genstruc gen;
};

//! Rotor Structure definition
/*! This structure holds the description of a rotors.
*/
struct rotstruc
{
	//! Generic info
	genstruc gen;
	//! Angular position
	float angle;
};

//! Star Tracker (STT) Sructure
struct sttstruc
{
	//! Generic info
	genstruc gen;
	//! alignment quaternion
	quaternion align;
	//! includes 0 and 1st order derivative
	quaternion att;
	rvector omega;
	rvector alpha;
	uint16_t retcode;
	uint32_t status;
};

//! Motion Capture Camera (MCC) Structure
struct mccstruc
{
	//! Generic info
	genstruc gen;
	//! Sensor alignment quaternion.
	quaternion align;
	//! attitude
	quaternion q;
};

//! Torque Rod Control Unit
struct tcustruc
{
	//! Generic info
	genstruc gen;
	//! Torque Rod count
	uint16_t mcnt;
	//! Torque Rod Component indices
	uint16_t mcidx[3];
};

struct busstruc
{
	//! Generic info
	genstruc gen;
	//! Total energy usage
	float energy;
	//! Watch Dog Timer (MJD)
	float wdt;
};

struct psenstruc
{
	//! Generic info
	genstruc gen;
	//! Current Pressure
	float press;
};

//! SUCHI Sructure
struct suchistruc
{
	//! Generic info
	genstruc gen;
	//! alignment quaternion
	quaternion align;
	//! Internal pressure
	float press;
	//! Internal temperatures
	float temps[8];
};

struct camstruc
{
	//! Generic info
	genstruc gen;
	uint16_t pwidth;
	uint16_t pheight;
	float width;
	float height;
	float flength;
};

// End of Device Specific structures

//! Node Structure
//! Structure for storing all information about a Node that never changes, or only
//! changes slowly. The information for initializing this should be in node.ini.
struct nodestruc
{
	//! Node Name.
	char name[COSMOS_MAX_NAME];
	//! Node Type as listed in \ref defs_node.
	uint16_t type;
	//! Operational state
	uint16_t state;
	uint16_t piece_cnt;
	uint16_t device_cnt;
	uint16_t port_cnt;
	uint16_t target_cnt;
	uint16_t glossary_cnt;
	uint16_t charging;
	//! Total Heat Capacity
	float hcap;
	//! Total Mass
	float mass;
	float area;
	float battcap;
	float powgen;
	float powuse;
	float battlev;
	//! Alt/Az/Range info
	float azfrom;
	float elfrom;
	float azto;
	float elto;
	float range;
	//! MJD Offset between system UT and simulated UT
	double utcoffset;
	//! Overall Node time
	double utc;
	//! Mission start time
	double utcstart;
	//! Location structure
	locstruc loc;
};

//! Device structure
/*! Complete details of each Device. It is a union of all the
possible device types, with a generic type for looking up basic
information.
*/
struct devicestruc
{
	union
	{
		allstruc all;
		antstruc ant;
		battstruc batt;
		busstruc bus;
		camstruc cam;
		cpustruc cpu;
		gpsstruc gps;
		htrstruc htr;
		imustruc imu;
		mccstruc mcc;
		motrstruc motr;
		mtrstruc mtr;
		tcustruc tcu;
		ploadstruc pload;
		propstruc prop;
		psenstruc psen;
		rotstruc rot;
		rwstruc rw;
		ssenstruc ssen;
		strgstruc strg;
		sttstruc stt;
		suchistruc suchi;
		swchstruc swch;
		tcvstruc tcv;
		telemstruc telem;
		txrstruc txr;
		rxrstruc rxr;
		thststruc thst;
		tsenstruc tsen;
	};
};

//! Specific Device structure
/*! Counts and arrays of pointers to each type of device, ordered by type.
*/
struct devspecstruc
{
	uint16_t ant_cnt;
	uint16_t batt_cnt;
	uint16_t bus_cnt;
	uint16_t cam_cnt;
	uint16_t cpu_cnt;
	uint16_t gps_cnt;
	uint16_t htr_cnt;
	uint16_t imu_cnt;
	uint16_t mcc_cnt;
	uint16_t motr_cnt;
	uint16_t mtr_cnt;
	uint16_t pload_cnt;
	uint16_t prop_cnt;
	uint16_t psen_cnt;
	uint16_t rot_cnt;
	uint16_t rw_cnt;
	uint16_t rxr_cnt;
	uint16_t ssen_cnt;
	uint16_t strg_cnt;
	uint16_t stt_cnt;
	uint16_t suchi_cnt;
	uint16_t swch_cnt;
	uint16_t tcu_cnt;
	uint16_t tcv_cnt;
	uint16_t telem_cnt;
	uint16_t txr_cnt;
	uint16_t thst_cnt;
	uint16_t tsen_cnt;
	vector<allstruc *>all;
	vector<antstruc *>ant;
	vector<battstruc *>batt;
	vector<busstruc *>bus;
	vector<camstruc *>cam;
	vector<cpustruc *>cpu;
	vector<gpsstruc *>gps;
	vector<htrstruc *>htr;
	vector<imustruc *>imu;
	vector<mccstruc *>mcc;
	vector<motrstruc *>motr;
	vector<mtrstruc *>mtr;
	vector<tcustruc *>tcu;
	vector<ploadstruc *>pload;
	vector<propstruc *>prop;
	vector<psenstruc *>psen;
	vector<rotstruc *>rot;
	vector<rwstruc *>rw;
	vector<ssenstruc *>ssen;
	vector<strgstruc *>strg;
	vector<sttstruc *>stt;
	vector<suchistruc *>suchi;
	vector<swchstruc *>swch;
	vector<telemstruc *>telem;
	vector<tcvstruc *>tcv;
	vector<txrstruc *>txr;
	vector<rxrstruc *>rxr;
	vector<thststruc *>thst;
	vector<tsenstruc *>tsen;
};

//! JSON Name Space structure
/*! A structure containing an element for every unique name in the COSMOS Name
 * Space. The static and dynamic components of this can then be mapped
 * separately to the name space using ::json_map_cosmosstruc.
*/
struct cosmosstruc
{
	//! Structure for summary information in node
	nodestruc node;
	//! Vector of all pieces in node.
	vector<piecestruc> piece;
	//! Vector of all general (common) information for devices (components) in node.
	vector<devicestruc> device;
	//! Structure for devices (components) special data in node, by type.
	devspecstruc devspec;
	//! Vector of all ports known to node.
	vector<portstruc> port;
	//! Structure for physics modelling.
	physicsstruc physics;
	//! Single entry vector for agent information.
	vector<agentstruc> agent;
	//! Single entry vector for event information.
	vector<eventstruc> event;
	//! Vector of all targets known to node.
	vector<targetstruc> target;
	//! Single entry vector for user information.
	vector<userstruc> user;
	//! Vector of glossary terms for node.
	vector<glossarystruc> glossary;
	//! Whether JSON map has been created.
	uint16_t jmapped;
	//! JSON Namespace Map matrix.
	vector<vector<jsonentry> > jmap;
	//! JSON Equation Map matrix.
	vector<vector<jsonequation> > emap;
	//! JSON Unit Map matrix: first level is for type, second level is for variant.
	vector<vector<unitstruc> > unit;
	//! Array of Two Line Elements
	vector<tlestruc> tle;
	//! Array of Aliases
	vector<aliasstruc> alias;
	//! Vector of Equations
	vector<equationstruc> equation;
};

//! @}

#endif
