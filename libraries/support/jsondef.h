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

#include "support/configCosmos.h"

#include "math/mathlib.h"
#include "support/convertdef.h"
#include "physics/physicsdef.h"
#include "support/socketlib.h"
#include "support/objlib.h"


//! \ingroup jsonlib
//! \defgroup jsonlib_type JSON Name Space variable type constants
//! @{

//! JSON Unit conversion type
enum {
    //! Identity
    JSON_UNIT_TYPE_IDENTITY,
    //! Polynomial
    JSON_UNIT_TYPE_POLY,
    //! Logarithm
    JSON_UNIT_TYPE_LOG
};

//! JSON Unit type
enum {
    //! No units
    JSON_UNIT_NONE,
    //! Length
    JSON_UNIT_LENGTH,
    //! Mass
    JSON_UNIT_MASS,
    //! Moment of Inertia
    JSON_UNIT_MOI,
    //! Time
    JSON_UNIT_TIME,
    //! Date
    JSON_UNIT_DATE,
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
    //! Luminance
    JSON_UNIT_LUMINANCE,
    //! Angle
    JSON_UNIT_ANGLE,
    //! Angular rate
    JSON_UNIT_ANGULAR_RATE,
    //! Angular acceleration
    JSON_UNIT_ANGULAR_ACCELERATION,
    //! Solid Angle
    JSON_UNIT_SOLIDANGLE,
    //! Frequency
    JSON_UNIT_FREQUENCY,
    //! Force
    JSON_UNIT_FORCE,
    //! Torque
    JSON_UNIT_TORQUE,
    //! Pressure
    JSON_UNIT_PRESSURE,
    //! Energy
    JSON_UNIT_ENERGY,
    //! Power
    JSON_UNIT_POWER,
    //! Charge
    JSON_UNIT_CHARGE,
    //! Electric Potential
    JSON_UNIT_VOLTAGE,
    //! Capacitance
    JSON_UNIT_CAPACITANCE,
    //! Electrical Resistance
    JSON_UNIT_RESISTANCE,
    //! Magnetic Field Strengh
    JSON_UNIT_MAGFIELD,
    //! Magnetic FLux
    JSON_UNIT_MAGFLUX,
    //! Magnetic Flux Density
    JSON_UNIT_MAGDENSITY,
    //! Magnetic Moment
    JSON_UNIT_MAGMOMENT,
    //! Specific Impulse
    JSON_UNIT_ISP,
    //! Random Access Memory and Disk Storage
    JSON_UNIT_BYTES,
    //! Fraction
    JSON_UNIT_FRACTION,
    //! Holder for number of entries (CLEVER!)
    JSON_UNIT_COUNT
};

//! JSON Namelist Group
enum {
    //! Absolute pointer
    JSON_STRUCT_ABSOLUTE,
    //! ::nodestruc
    JSON_STRUCT_NODE,
    //! ::agentstruc
    JSON_STRUCT_AGENT,
    //! ::devicestruc
    JSON_STRUCT_DEVICE,
    //! ::devspecstruc
    JSON_STRUCT_DEVSPEC,
    //! ::physicsstruc
    JSON_STRUCT_PHYSICS,
    //! ::eventstruc
    JSON_STRUCT_EVENT,
    //! ::piecestruc
    JSON_STRUCT_PIECE,
    //! ::targetstruc
    JSON_STRUCT_TARGET,
    //! ::userstruc
    JSON_STRUCT_USER,
    //! ::portstruc
    JSON_STRUCT_PORT,
    //! ::glossarystruc
    JSON_STRUCT_GLOSSARY,
    //! ::tlestruc
    JSON_STRUCT_TLE,
    //! ::aliasstruc
    JSON_STRUCT_ALIAS,
    //! ::equationstruc
    JSON_STRUCT_EQUATION,
    //! ::vertexstruc
    JSON_STRUCT_POINT,
    //! ::facestruc
    JSON_STRUCT_FACE,
    JSON_STRUCT_PTR,
    //! entirety
    JSON_STRUCT_ALL
};

//! Constants defining the data types supported in the \ref jsonlib_namespace.
enum {
    //! JSON 8 bit unsigned integer type
    JSON_TYPE_UINT8=1,
    //! JSON 8 bit signed integer type
    JSON_TYPE_INT8,
    //! JSON char type
    JSON_TYPE_CHAR,
    //! JSON Boolean
    JSON_TYPE_BOOL,
    //! JSON 32 bit unsigned integer type
    JSON_TYPE_UINT32,
    //! JSON 16 bit unsigned integer type
    JSON_TYPE_UINT16,
    //! JSON 16 bit integer type
    JSON_TYPE_INT16,
    //! JSON 32 bit integer type
    JSON_TYPE_INT32,
    //! JSON single precision floating vertex type
    JSON_TYPE_FLOAT,
    //! JSON double precision floating vertex type
    JSON_TYPE_DOUBLE,
    //! JSON char* type
    JSON_TYPE_CHARP,
    //! JSON string type
    JSON_TYPE_STRING,
    //! JSON Name type
    JSON_TYPE_NAME,
    //! JSON ::Vector
    JSON_TYPE_VECTOR,
    //! JSON ::rvector
    JSON_TYPE_RVECTOR,
    //! JSON ::rvector
    JSON_TYPE_AVECTOR,
    //! JSON ::cvector
    JSON_TYPE_CVECTOR,
    //! JSON ::quaternion type
    JSON_TYPE_QUATERNION,
    //! JSON ::gvector
    JSON_TYPE_GVECTOR,
    //! JSON ::svector
    JSON_TYPE_SVECTOR,
    //! JSON ::rmatrix
    JSON_TYPE_RMATRIX,
    //! JSON ::cartpos
    JSON_TYPE_CARTPOS,
    //! JSON ::cartpos
    JSON_TYPE_GEOIDPOS,
    //! JSON ::cartpos
    JSON_TYPE_SPHERPOS,
    //! JSON ::extrapos
    JSON_TYPE_EXTRAPOS,
    //! JSON ::qatt
    JSON_TYPE_QATT,
    //! JSON ::dcmatt
    JSON_TYPE_DCMATT,
    //! JSON ::extraatt
    JSON_TYPE_EXTRAATT,
    //! JSON ::posstruc
    JSON_TYPE_POSSTRUC,
    //! JSON ::attstruc
    JSON_TYPE_ATTSTRUC,
    //! JSON Agent Heartbeat
    JSON_TYPE_HBEAT,
    //! JSON Solar Barycentric Position
    JSON_TYPE_POS_ICRF,
    JSON_TYPE_POS_FIRST = JSON_TYPE_POS_ICRF,
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
    JSON_TYPE_POS_LAST = JSON_TYPE_POS_SELG,
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
    JSON_TYPE_LOCSTRUC,
    JSON_TYPE_NODESTRUC,
    JSON_TYPE_VERTEXSTRUC,
    JSON_TYPE_FACESTRUC,
    JSON_TYPE_PIECESTRUC,
    JSON_TYPE_DEVICESTRUC,
    JSON_TYPE_DEVSPECSTRUC,
    JSON_TYPE_PORTSTRUC,
    JSON_TYPE_PHYSICSSTRUC,
    JSON_TYPE_AGENTSTRUC,
    JSON_TYPE_EVENTSTRUC,
    JSON_TYPE_TARGETSTRUC,
    JSON_TYPE_USERSTRUC,
    JSON_TYPE_GLOSSARYSTRUC,
    JSON_TYPE_TLESTRUC,
    //! JSON Timestamp
    JSON_TYPE_TIMESTAMP,
    //! JSON Equation
    JSON_TYPE_EQUATION,
    //! JSON Alias
    JSON_TYPE_ALIAS,
    //! Not defined
    JSON_TYPE_NONE=UINT16_MAX
};

//! Things to update
enum class JSON_UPDATE : int32_t {
    NONE,
    POS,
    ATT
};

//! Types of equation operands
enum {
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
enum {
    //! Addition +
    JSON_OPERATION_ADD,
    //! Subtraction -
    JSON_OPERATION_SUBTRACT,
    //! Multiplication *
    JSON_OPERATION_MULTIPLY,
    //! Division /
    JSON_OPERATION_DIVIDE,
    //! Modulo %
    JSON_OPERATION_MOD,
    //! Boolean And &
    JSON_OPERATION_AND,
    //! Boolean Or |
    JSON_OPERATION_OR,
    //! Boolean Greater Than >
    JSON_OPERATION_GT,
    //! Boolean Less Than <
    JSON_OPERATION_LT,
    //! Boolean Equal =
    JSON_OPERATION_EQ,
    //! Logical Not !
    JSON_OPERATION_NOT,
    //! Complement ~
    JSON_OPERATION_COMPLEMENT,
    //! Power ^
    JSON_OPERATION_POWER,
    //! Bitwise AND @
    JSON_OPERATION_BITWISEAND,
    //! Bitwise OR #
    JSON_OPERATION_BITWISEOR
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
//! Maximum number of vertexs in a structure
#define MAXPNT 8
//! Maximum number of faces in a Piece
#define MAXFACE 20
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

//! Maximum AGENT transfer buffer size
#define AGENTMAXBUFFER 60000
//! Maximum number of supported publication interfaces
#define AGENTMAXIF 7
//! Maximum number of builtin AGENT requests
#define AGENTMAXBUILTINCOUNT 6
//! Maximum number of user defined AGENT requests
#define AGENTMAXUSERCOUNT 200    // 20150629JC: Increased count from 40 to 200 (to support new software)
//! Maximum number of AGENT requests
#define AGENTMAXREQUESTCOUNT (AGENTMAXBUILTINCOUNT+AGENTMAXUSERCOUNT)

//! @}
//! \ingroup defs
//! \defgroup defs_comp Constants defining Components.

//! \ingroup defs_comp
//! \defgroup defs_comp_type Type of Component.
//! @{

enum DeviceType : uint16_t {
    //! Payload
    PLOAD=0,
    //! Elevation and Azimuth Sun Sensor
    SSEN=1,
    //! Inertial Measurement Unit
    IMU=2,
    //! Reaction Wheel
    RW=3,
    //! Magnetic Torque Rod
    MTR=4,
    //! Processing Unit
    CPU=5,
    //! GPS Unit
    GPS=6,
    //! Antenna
    ANT=7,
    //! Radio Receiver
    RXR=8,
    //! Radio Transmitter
    TXR=9,
    //! Radio Transceiver
    TCV=10,
    //! Photo Voltaic String
    PVSTRG=11,
    //! Battery
    BATT=12,
    //! Heater
    HTR=13,
    //! Motor
    MOTR=14,
    //! Temperature Sensor
    TSEN=15,
    //! Thruster
    THST=16,
    //! Propellant Tank
    PROP=17,
    //! Switch
    SWCH=18,
    //! Rotor
    ROT=19,
    //! Star Tracker
    STT=20,
    //! Motion Capture Camera
    MCC=21,
    //! Torque rod Control Unit
    TCU=22,
    //! Power Bus
    BUS=23,
    //! Pressure sensor
    PSEN=24,
    //! SUCHI
    SUCHI=25,
    //! Camera
    CAM=26,
    //! Telemetry
    TELEM=27,
    //! Disk Drive
    DISK=28,
    //! TNC
    TNC=29,
    //! BCREG
    BCREG=30,
    //! List count
    COUNT,
    //! Not a Component
    NONE=UINT16_MAX
};

enum {
    DEVICE_MODEL_ASTRODEV=0,
    DEVICE_MODEL_TS2000=1,
    DEVICE_MODEL_IC9100=2,
    DEVICE_MODEL_GS232B=3,
    DEVICE_MODEL_PRKX2SU=4,
    DEVICE_MODEL_LOOPBACK=5,
    DEVICE_MODEL_PROPAGATOR=6,
    DEVICE_MODEL_USRP=7,
    DEVICE_MODEL_DIRECT=8
};

enum {
    DEVICE_RADIO_MODE_AM,
    DEVICE_RADIO_MODE_AMD,
    DEVICE_RADIO_MODE_FM,
    DEVICE_RADIO_MODE_FMD,
    DEVICE_RADIO_MODE_LSB,
    DEVICE_RADIO_MODE_LSBD,
    DEVICE_RADIO_MODE_USB,
    DEVICE_RADIO_MODE_USBD,
    DEVICE_RADIO_MODE_DV,
    DEVICE_RADIO_MODE_DVD,
    DEVICE_RADIO_MODE_CW,
    DEVICE_RADIO_MODE_CWR,
    DEVICE_RADIO_MODE_RTTY,
    DEVICE_RADIO_MODE_RTTYR,
    DEVICE_RADIO_MODE_UNDEF
};

enum {
    DEVICE_RADIO_MODULATION_ASK,
    DEVICE_RADIO_MODULATION_BPSK1200,
    DEVICE_RADIO_MODULATION_BPSK2400,
    DEVICE_RADIO_MODULATION_BPSK4800,
    DEVICE_RADIO_MODULATION_BPSK9600,
    DEVICE_RADIO_MODULATION_AFSK,
    DEVICE_RADIO_MODULATION_GFSK1200,
    DEVICE_RADIO_MODULATION_GFSK2400,
    DEVICE_RADIO_MODULATION_GFSK4800,
    DEVICE_RADIO_MODULATION_GFSK9600,
    DEVICE_RADIO_MODULATION_UNDEF
    };

enum {
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
#define DEVICE_FLAG_ACTIVE      0x0004 // TODO: define device active
#define DEVICE_FLAG_CONNECTED   0x0008 // TODO: define device connected
//! @}

//! \ingroup defs_comp
//! \defgroup defs_comp_port IO Port types.
//! @{
//!
enum PORT_TYPE {
    PORT_TYPE_RS232 = 0,
    PORT_TYPE_RS422 = 1,
    PORT_TYPE_ETHERNET = 2,
    PORT_TYPE_USB = 3,
    PORT_TYPE_DRIVE = 4,
    PORT_TYPE_SIM = 5,
    PORT_TYPE_UDP = 6,
    PORT_TYPE_TCP = 7,
    PORT_TYPE_PROPAGATOR = 8,
    PORT_TYPE_COUNT,
    PORT_TYPE_NONE = UINT16_MAX
};

//! @}
//! @}

//! \ingroup jsonlib
//! \defgroup jsonlib_typedefs JSON typedefs
//! @{

//! JSON unit type entry
/*! To be used in the ::cosmosstruc table of units. Each entry represents
 * one specific variant of one type of unit. Each entry includes:
 * - a name for the unit
 * - a type for the unit conversion: 0 = identity, 1 = linear, 2 = log
 * - 0th, 1st and 2nd derivative terms for any conversion
 */
struct unitstruc
{
    //! JSON Unit Name
    string name = "";
    //! JSON Unit conversion type
    uint16_t type = JSON_UNIT_TYPE_IDENTITY;
    //! 0th derivative term
    float p0 = 0.f;
    //! 1th derivative term
    float p1 = 0.f;
    //! 2th derivative term
    float p2 = 0.f;

	// needed to get class contents as JSON object (internal to json11)
	json11::Json to_json() const {
		return json11::Json::object {
			{ "name", name },
			{ "type", type },
			{ "p0", p0 },
			{ "p1", p1 },
			{ "p2", p2 }
		};
	}

	// needed to set class contents from JSON string
	void from_json(const string& s)	{
		string error;
		json11::Json parsed = json11::Json::parse(s,error);
		if(error.empty())	{
			if(!parsed["name"].is_null()) name = parsed["name"].string_value();
			if(!parsed["type"].is_null()) type = parsed["type"].number_value();
			if(!parsed["p0"].is_null()) p0 = parsed["p0"].number_value();
			if(!parsed["p1"].is_null()) p1 = parsed["p1"].number_value();
			if(!parsed["p2"].is_null()) p2 = parsed["p2"].number_value();
		} else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
		return;
	}
};

//! JSON Node description strings
//! Structure containing the JSON strings that describe the various aspects of a node. These
//! can be loaded from disk, or transferred from another agent.
struct jsonnode
{
    string name = "";
    string node = "";
    string state = "";
    string utcstart = "";
    string vertexs = "";
    string faces = "";
    string pieces = "";
    string devgen = "";
    string devspec = "";
    string ports = "";
    string targets = "";
    string aliases = "";

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "name" , name },
            { "node" , node },
            { "state" , state },
            { "utcstart" , utcstart },
            { "vertexs" , vertexs },
            { "faces" , faces },
            { "pieces" , pieces },
            { "devgen" , devgen },
            { "devspec" , devspec },
            { "ports" , ports },
            { "targets" , targets },
            { "aliases" , aliases }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["name"].is_null())		name = parsed["name"].string_value();
            if(!parsed["node"].is_null())		node = parsed["node"].string_value();
            if(!parsed["state"].is_null())		state = parsed["state"].string_value();
            if(!parsed["utcstart"].is_null())	utcstart = parsed["utcstart"].string_value();
            if(!parsed["vertexs"].is_null())	vertexs = parsed["vertexs"].string_value();
            if(!parsed["faces"].is_null())		faces = parsed["faces"].string_value();
            if(!parsed["pieces"].is_null())		pieces = parsed["pieces"].string_value();
            if(!parsed["devgen"].is_null())		devgen = parsed["devgen"].string_value();
            if(!parsed["devspec"].is_null())	devspec = parsed["devspec"].string_value();
            if(!parsed["ports"].is_null())		ports = parsed["ports"].string_value();
            if(!parsed["targets"].is_null())	targets = parsed["targets"].string_value();
            if(!parsed["aliases"].is_null())	aliases = parsed["aliases"].string_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
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
/*! Tokenized version of a single JSON object. The token is a handle to the location
 * in the JSON map represented by the string portion, and the value portion stored as a string.
 */
struct jsontoken
{
    jsonhandle handle;
    string value;
    double utc;
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
 * - index: Index of this entry in the ::cosmosstruc::jmap.
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

//! Agent Request Function
//! Format of a user supplied function to handle a given request
typedef int32_t (*agent_request_function)(char* request_string, char* output_string, void *root);

//! Agent Request Entry
//! Structure representing a single Agent request.
struct agent_request_entry
{
    //! Character token for request
    string token;
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
    char address[18];
    // Channel's broadcast address in string form
    char baddress[18];
    // Channel's interface name
    char name[COSMOS_MAX_NAME+1];
};

//! Process heartbeat.
//! Detailed elements of a single heartbeat of a single process.
struct beatstruc
{
    /** Heartbeat timestamp */
    double utc = 0.;
    /** Heartbeat Node Name */
    char node[COSMOS_MAX_NAME+1] = {};  // TODO: change to string
    //! Heartbeat Agent Name
    char proc[COSMOS_MAX_NAME+1] = {}; // TODO: change to string
    // Type of address protocol
    NetworkType ntype = NetworkType::MULTICAST;
    //! Protocol Address
    char addr[18] = {};
	//! AGENT port
    uint16_t port = 0;
    //! Transfer buffer size
    uint32_t bsz = 0;
    //! Heartbeat period in seconds
    double bprd = 0.;
    //! Agent User Name
    char user[COSMOS_MAX_NAME+1] = {};
    //! Agent % CPU
    float cpu = 0.;
    //! Agent % memory
    float memory = 0.;
    //! Agent heartbeat jitter [sec]
    double jitter = 0.;
    //! Existence Flag (if beat exists then flag is set to true, false otherwise)
    bool exists = false;
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
    size_t ifcnt;
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
    //! Agent request list
    vector <agent_request_entry> reqs;
    //! Heartbeat
    beatstruc beat;
};

//! Full COSMOS Event structure
/*! This is the struct that holds each Event, along with associated
 * resources and conditions.
 */
struct eventstruc
{
    //! Time event is to start.
    double utc = 0.; //TODO: replace for mjd
    //! Time event was executed.
    double utcexec = 0.;
    //! Node for event
    char node[COSMOS_MAX_NAME+1] = "";
    //! Name of event.
    // TODO: change char to string
    char name[COSMOS_MAX_NAME+1] = "";
    //! User of event.
    char user[COSMOS_MAX_NAME+1] = "";
    //! Event flags.
    uint32_t flag = 0;
    //! Event type.
    uint32_t type = 0;
    //! Value of condition
    double value = 0.;
    //! Event initial time consumed.
    double dtime = 0.;
    //! Event continuous time consumed.
    double ctime = 0.;
    //! Event initial energy consumed.
    float denergy = 0.f;
    //! Event continuous energy consumed.
    float cenergy = 0.f;
    //! Event initial mass consumed.
    float dmass = 0.f;
    //! Event continuous mass consumed.
    float cmass = 0.f;
    //! Event initial bytes consumed.
    float dbytes = 0.f;
    //! Event continuous bytes consumed.
    float cbytes = 0.f;
    //! Handle of condition that caused event, NULL if timed event.
    jsonhandle handle;
    //! Event specific data.
    char data[JSON_MAX_DATA] = "";
    //! Condition that caused event, NULL if timed event.
    char condition[JSON_MAX_DATA] = "";
};

struct userstruc
{
	//to enforce a std::string name length maximum (if necessary) use this code to truncate:
	//
	//constexpr std::string::size_type MAX_CHARS = 20 ;
	//if(whatever_string.size() > MAX_CHARS)	{
	//	whatever_string = whatever_string.substr(0, MAX_CHARS);
	//}
	string name;
	string node;
	string tool;
	string cpu;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "name" , name },
            { "node", node },
            { "tool", tool },
            { "cpu", cpu }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["name"].is_null())	name = parsed["name"].string_value();
            if(!parsed["node"].is_null())	node = parsed["node"].string_value();
            if(!parsed["tool"].is_null())	tool = parsed["tool"].string_value();
            if(!parsed["cpu"].is_null())	cpu = parsed["cpu"].string_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
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
    // Glossary entry ::jsonlib_namespace type.
    uint16_t type;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "name" , name },
            { "description", description },
            { "type", type }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["name"].is_null())			name = parsed["name"].string_value();
            if(!parsed["description"].is_null())	description = parsed["description"].string_value();
            if(!parsed["type"].is_null())			type = parsed["type"].int_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Alias structure
/*! Contains the name of an alias and the ::jsonmap entry it vertexs to,
 * stored as a ::jsonhandle.
*/
struct aliasstruc
{
    // Alias name
    string name;
    // Namespace handle
    jsonhandle handle;
    //! JSON Data Type
    uint16_t type;
};

//! Equation structure
/*! Contains the name of an equation and the Equation string it vertexs to.
*/
struct equationstruc
{
    // Equation name
    string name;
    // Equation string
    string value;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "name" , name },
            { "value", value }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["name"].is_null()) name = parsed["name"].string_value();
            if(!parsed["value"].is_null()) value = parsed["value"].string_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};


//! Target structure
/*! Contains entries unique to each Target
*/
struct targetstruc
{
    double utc;
    char name[COSMOS_MAX_NAME+1];
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
 * ::allstruc::portidx entry in each device structure indicates which of these
 * ports a device will use.
 */
struct portstruc
{
    //! Type of I/O as listed in ::PORT_TYPE.
	//JIMNOTE: should this be of PORT_TYPE not uint16_t?
    uint16_t type;
    //! Name information for port.
    //!!! Do not make this string
    char name[COSMOS_MAX_DATA+1];
};

//! Point structure: information on each vertex in a face
typedef Vector vertexstruc;

//! Face structure: information on each face of a piece
struct facestruc
{
    uint16_t vertex_cnt;
    vector <uint16_t> vertex_idx;
    Vector com;
    Vector normal;
    double area=0.;
};

//! Part structure: physical information for each piece of Node
struct piecestruc
{
    //! Name of piece
    char name[COSMOS_MAX_NAME+1];
    //! Enabled?
    bool enabled;
    //! Type of piece from \ref defs_piece.
    //	uint16_t type;
    //! Component index: -1 if not a Component
    uint16_t cidx;
    //! Density in kg/cu m
    float density;
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
    //! Volume in cubic meters
    float volume;
    //! Number of faces
    uint16_t face_cnt;
    //! Array of vertices/vertexs
    vector <uint16_t> face_idx;
    //! Centroid of piece
    Vector com;
    //! Contribution of piece to linear forces
    Vector shove;
    //! Contribution of piece to angular forces
    Vector twist;
    //! Stored thermal energy
    float heat;
    //! Temperature in Kelvins
    float temp;
    //! Insolation in Watts/sq m
    float insol;
    //! Material density (1. - transparency)
    float material_density;
    //! Material ambient reflective qualities
    Vector material_ambient;
    //! Material diffuse reflective qualities
    Vector material_diffuse;
    //! Material specular reflective qualities
    Vector material_specular;
};

// Beginning of Device General structures

//! All Component structure
/*! These are the elements that are repeated in all devices. Each
device specific structure has these as its first elements, followed by
any specific elements. They are all then combined together in one grand
union as a ::devicestruc.
*/
struct allstruc
{
    //! Enabled?
    bool enabled;
    //! Component Type
    uint16_t type;
    //! Device Model
    uint16_t model;
    //! Device flag - catch all for any small piece of information that might be device specific
    uint32_t flag;
    //! Device specific address
    uint16_t addr;
    //! Component Index
    uint16_t cidx;
    //! Device specific index
    uint16_t didx;
    //! Piece index
    uint16_t pidx;
    //! Power Bus index
    uint16_t bidx;
    //! Connection information for device.
    uint16_t portidx; // TODO: rename to port_id or port_index
    //! Nominal Amperage
    float namp; // TODO: rename to nominal current
    //! Nominal Voltage
    float nvolt; // TODO: rename to nominal voltage
    //! Current Amperage
    float amp; // TODO: rename to current
    //! Current Voltage
    float volt; // TODO: rename to voltage
    //! Current Power
    float power; // TODO: rename to voltage
    //! Total energy usage
    float energy;
    //! Current data rate
    float drate;
    //! Current Temperature
    float temp; // TODO: rename to temperature
    //! Device information time stamp
    double utc;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "enabled" , enabled },
            { "type"    , type },
            { "model"   , model },
            { "flag"    , static_cast<double>(flag) },
            { "addr"    , addr },
            { "cidx"    , cidx },
            { "didx"    , didx },
            { "pidx"    , pidx },
            { "bidx"    , bidx },
            { "portidx" , portidx },
            { "namp"    , namp},
            { "nvolt"   , nvolt },
            { "amp"     , amp },
            { "volt"    , volt },
            { "power"   , power },
            { "energy"  , energy },
            { "drate"   , drate },
            { "temp"    , temp },
            { "utc"     , utc },
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["enabled"].is_null()) enabled = parsed["enabled"].bool_value();
            if(!parsed["type"].is_null()) type = parsed["type"].int_value();
            if(!parsed["model"].is_null()) model = parsed["model"].int_value();
            if(!parsed["flag"].is_null()) flag = parsed["flag"].int_value();
            if(!parsed["addr"].is_null()) addr = parsed["addr"].int_value();
            if(!parsed["cidx"].is_null()) cidx = parsed["cidx"].int_value();
            if(!parsed["didx"].is_null()) didx = parsed["didx"].int_value();
            if(!parsed["pidx"].is_null()) pidx = parsed["pidx"].int_value();
            if(!parsed["bidx"].is_null()) bidx = parsed["bidx"].int_value();
            if(!parsed["portidx"].is_null()) portidx = parsed["portidx"].int_value();
            if(!parsed["namp"].is_null()) namp = parsed["namp"].number_value();
            if(!parsed["nvolt"].is_null()) nvolt = parsed["nvolt"].number_value();
            if(!parsed["amp"].is_null()) amp = parsed["amp"].number_value();
            if(!parsed["volt"].is_null()) volt = parsed["volt"].number_value();
            if(!parsed["power"].is_null()) power = parsed["power"].number_value();
            if(!parsed["energy"].is_null()) energy = parsed["energy"].number_value();
            if(!parsed["drate"].is_null()) drate = parsed["drate"].number_value();
            if(!parsed["temp"].is_null()) temp = parsed["temp"].number_value();
            if(!parsed["utc"].is_null()) utc = parsed["utc"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

// End of Device General structures

// Beginning of Device Specific structures

//! Telemetry (TELEM) structure.
/*! This provides access to a union of a variety of possible data types, along
 * with an optional indicator of what type the data actually is. The same data
 * is then accessible as all supported data types, both in the structure and the
 * Namespace.
 */

 //JIMNOTE:  this one needs some JSON work... hmm....
struct telemstruc : public allstruc
{
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
        char vstring[COSMOS_MAX_NAME+1];
    };

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "type" , type },
            { "vstring", vstring }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["type"].is_null()) type = parsed["type"].number_value();
//  TODO:          vstring = parsed["vstring"].string_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
} ;

//! Payload (PLOAD) structure.
/*! You can define up to ::MAXPLOADKEYCNT keys by giving them unique
 * names. You can then set double precision values for these keys in
 * the dynamic structure.
 */
struct ploadstruc : public allstruc
{
    //! Number of keys being used.
    uint16_t key_cnt;
    //! Name for each key.
    uint16_t keyidx[MAXPLOADKEYCNT];
    //! Value for each key.
    float keyval[MAXPLOADKEYCNT];

    // Convert class contents to JSON object
    json11::Json to_json() const {
        vector<uint16_t> v_keyidx = vector<uint16_t>(keyidx, keyidx+MAXPLOADKEYCNT);
        vector<float> v_keyval = vector<float>(keyval, keyval+MAXPLOADKEYCNT);
        return json11::Json::object {
            { "key_cnt", key_cnt },
            { "keyidx" , v_keyidx },
            { "keyval" , v_keyval }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["key_cnt"].is_null()) key_cnt = parsed["key_cnt"].number_value();
            if(!parsed["keyidx"].is_null()) {
                auto p_keyidx = parsed["keyidx"].array_items();
                for(size_t i = 0; i != p_keyidx.size(); ++i) {
                    if(!parsed["keyidx"][i].is_null()) keyidx[i] = p_keyidx[i].int_value();
                }
            }
            if(!parsed["keyval"].is_null()) {
                auto p_keyval = parsed["keyval"].array_items();
                for(size_t i = 0; i != p_keyval.size(); ++i) {
                    if(!parsed["keyval"][i].is_null()) keyval[i] = p_keyval[i].number_value();
                }
            }
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Sun Sensor (SSEN) Sructure
struct ssenstruc : public allstruc
{
    //! Sensor alignment quaternion.
    quaternion align;
    float qva;
    float qvb;
    float qvc;
    float qvd;
    float azimuth;
    float elevation;

// Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "align" , align },
            { "qva", qva },
            { "qvb", qvb },
            { "qvc", qvc },
            { "qvd", qvd },
            { "azimuth", azimuth },
            { "elevation", elevation },
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["align"].is_null()) align.from_json(parsed["align"].dump());
            if(!parsed["qva"].is_null()) qva = parsed["qva"].number_value();
            if(!parsed["qvb"].is_null()) qvb = parsed["qvb"].number_value();
            if(!parsed["qvc"].is_null()) qvc = parsed["qvc"].number_value();
            if(!parsed["qvd"].is_null()) qvd = parsed["qvd"].number_value();
            if(!parsed["azimuth"].is_null()) azimuth = parsed["azimuth"].number_value();
            if(!parsed["elevation"].is_null()) elevation = parsed["elevation"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Inertial Measurement Unit (IMU) structure
struct imustruc : public allstruc
{
    //! alignment quaternion
    quaternion align;
    //! Position acceleration vector
    rvector accel;
    //! Attitude quaternion
    quaternion theta;
    //! Attitude Euler angles
    avector euler;
    //! Attitude rate vector
    rvector omega;
    //! Attitude acceleration vector
    rvector alpha;
    //! Magnetic field in sensor frame
    rvector mag;
    //! Magnetic field rate change in sensor frame
    rvector bdot;
    
    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "align" , align },
            { "accel" , accel },
            { "theta" , theta },
            { "euler" , euler },
            { "omega" , omega },
            { "alpha" , alpha },
            { "mag"   , mag },
            { "bdot"  , bdot }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["align"].is_null()) align.from_json(parsed["align"].dump());
            if(!parsed["accel"].is_null()) accel.from_json(parsed["accel"].dump());
            if(!parsed["theta"].is_null()) theta.from_json(parsed["theta"].dump());
            if(!parsed["euler"].is_null()) euler.from_json(parsed["euler"].dump());
            if(!parsed["omega"].is_null()) omega.from_json(parsed["omega"].dump());
            if(!parsed["alpha"].is_null()) alpha.from_json(parsed["alpha"].dump());
            if(!parsed["mag"].is_null()) mag.from_json(parsed["mag"].dump());
            if(!parsed["bdot"].is_null()) bdot.from_json(parsed["bdot"].dump());
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Reaction Wheel structure: z axis is aligned with axis of rotation.
struct rwstruc : public allstruc
{
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

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "align" , align },
            { "mom"   , mom },
            { "mxomg" , mxomg },
            { "mxalp" , mxalp },
            { "tc"    , tc },
            { "omg"   , omg },
            { "alp"   , alp },
            { "romg"  , romg },
            { "ralp"  , ralp },
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["align"].is_null()) align.from_json(parsed["align"].dump());
            if(!parsed["mom"].is_null()) mom.from_json(parsed["mom"].dump());
            if(!parsed["mxomg"].is_null()) mxomg = parsed["mxomg"].number_value();
            if(!parsed["bdmxalpot"].is_null()) mxalp = parsed["mxalp"].number_value();
            if(!parsed["tc"].is_null()) tc = parsed["tc"].number_value();
            if(!parsed["omg"].is_null()) omg = parsed["omg"].number_value();
            if(!parsed["alp"].is_null()) alp = parsed["alp"].number_value();
            if(!parsed["romg"].is_null()) romg = parsed["romg"].number_value();
            if(!parsed["ralp"].is_null()) ralp = parsed["ralp"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Magnetic Torque Rod structure: z axis is aligned with rod.
struct mtrstruc : public allstruc
{
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

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "align" , align },
            { "npoly"   , json11::Json::carray_to_vector(npoly, sizeof(npoly)/sizeof(npoly[0])) },
            { "ppoly"   , json11::Json::carray_to_vector(ppoly, sizeof(ppoly)/sizeof(ppoly[0])) },
            { "mxmom"   , mxmom },
            { "tc"      , tc },
            { "rmom"    , rmom },
            { "mom"     , mom }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["align"].is_null()) align.from_json(parsed["align"].dump());
            if(!parsed["npoly"].is_null()) {
                auto p_npoly = parsed["npoly"].array_items();
                for(size_t i = 0; i != p_npoly.size(); ++i) {
					// SCOTTNOTE:  check with is_null()
                    npoly[i] = p_npoly[i].number_value();
                }
            }
            if(!parsed["ppoly"].is_null()) {
                auto p_ppoly = parsed["ppoly"].array_items();
                for(size_t i = 0; i != p_ppoly.size(); ++i) {
					// SCOTTNOTE:  check with is_null()
                    ppoly[i] = p_ppoly[i].number_value();
                }
            }
            if(!parsed["mxmom"].is_null()) mxmom = parsed["mxmom"].number_value();
            if(!parsed["tc"].is_null()) tc = parsed["tc"].number_value();
            if(!parsed["rmom"].is_null()) rmom = parsed["rmom"].number_value();
            if(!parsed["mom"].is_null()) mom = parsed["mom"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! CPU information
struct cpustruc : public allstruc
{
    // cpu
    //! Seconds CPU has been up
    uint32_t uptime;
    //! Current load
    float load;
    //! Maximum load
    float maxload;

    // memory
    //! Maximum memory capacity in GiB
    float maxgib;
    //! Current memory usage in GiB
    float gib;

    //! Number of reboots
    uint32_t boot_count;

        // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "uptime" , static_cast<int>(uptime) },
            { "load"   , load },
            { "maxload", maxload },
            { "maxgib" , maxgib },
            { "gib"    , gib },
            { "boot_count", static_cast<int>(boot_count) },
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["uptime"].is_null()) uptime = static_cast<uint32_t>(parsed["uptime"].int_value());
            if(!parsed["load"].is_null()) load = parsed["load"].number_value();
            if(!parsed["maxload"].is_null()) maxload = parsed["maxload"].number_value();
            if(!parsed["maxgib"].is_null()) maxgib = parsed["maxgib"].number_value();
            if(!parsed["gib"].is_null()) gib = parsed["gib"].number_value();
            if(!parsed["boot_count"].is_null()) boot_count = static_cast<uint32_t>(parsed["boot_count"].int_value());
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Disk information
struct diskstruc : public allstruc
{
    // disk
    //! Maximum disk capacity in GiB
    float maxgib;  // TODO: rename to diskSize, consider bytes?
    //! Current disk usage in GiB
    float gib; // TODO: rename to diskUsed, consider bytes?
    //! Path
    char path[COSMOS_MAX_NAME];

        // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "maxgib" , maxgib },
            { "gib"    , gib },
			{ "path"   , path }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["maxgib"].is_null()) maxgib = parsed["maxgib"].number_value();
            if(!parsed["gib"].is_null()) gib = parsed["gib"].number_value();
            if(!parsed["path"].is_null()) strcpy(path, parsed["gib"].string_value().c_str());
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

// TODO: rename to GpsData
struct gpsstruc : public allstruc
{
    //! UTC time error
    double dutc;
    //! Geocentric position: x, y, z
    //! ECEF coordinates
    // TODO: rename variables to more meaningfull names like position, velocity
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
    uint16_t sats_used;
    //! number of satellites visible by GPS receiver
    uint16_t sats_visible;
    //! Time Status
    uint16_t time_status;
    //! Position Type
    uint16_t position_type;
    //! Solution Status
    uint16_t solution_status;

        // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "dutc"   , dutc },
            { "geocs"  , geocs },
            { "geocv"  , geocv },
            { "dgeocs" , dgeocs },
            { "dgeocv" , dgeocv },
            { "geods"  , geods },
            { "geodv"  , geodv },
            { "dgeods" , dgeods },
            { "dgeodv" , dgeodv },
            { "heading", heading },
            { "sats_used"    , sats_used },
            { "sats_visible" , sats_visible },
            { "time_status"  , time_status },
            { "position_type", position_type },
            { "solution_status", solution_status }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["dutc"].is_null()) dutc = parsed["dutc"].number_value();
            if(!parsed["geocs"].is_null()) geocs.from_json(parsed["geocs"].dump());
            if(!parsed["geocv"].is_null()) geocv.from_json(parsed["geocv"].dump());
            if(!parsed["dgeocs"].is_null()) dgeocs.from_json(parsed["dgeocs"].dump());
            if(!parsed["dgeocv"].is_null()) dgeocv.from_json(parsed["dgeocv"].dump());
            if(!parsed["geods"].is_null()) geods.from_json(parsed["geods"].dump());
            if(!parsed["geodv"].is_null()) geodv.from_json(parsed["geodv"].dump());
            if(!parsed["dgeods"].is_null()) dgeods.from_json(parsed["dgeods"].dump());
            if(!parsed["dgeodv"].is_null()) dgeodv.from_json(parsed["dgeodv"].dump());
            if(!parsed["heading"].is_null()) heading = parsed["heading"].number_value();
            if(!parsed["sats_used"].is_null()) sats_used = parsed["sats_used"].int_value();
            if(!parsed["sats_visible"].is_null()) sats_visible = parsed["sats_visible"].int_value();
            if(!parsed["time_status"].is_null()) time_status = parsed["time_status"].int_value();
            if(!parsed["position_type"].is_null()) position_type = parsed["position_type"].int_value();
            if(!parsed["solution_status"].is_null()) solution_status = parsed["solution_status"].int_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Antenna information
/*! This structure holds the information concerning antennas.
*/
struct antstruc : public allstruc
{
    //! Alignment
    quaternion align;
    //! Azimuth;
    float azim;
    //! Elevation
    float elev;
    //! Minimum elevation
    float minelev;
    //! Maximum elevation
    float maxelev;
    //! Minimum azimuth
    float minazim;
    //! Maximum azimuth
    float maxazim;
    //! Elevation threshold
    float threshelev;

        // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "align" , align },
            { "azim"  , azim },
            { "elev"  , elev },
            { "minelev" , minelev },
            { "maxelev" , maxelev },
            { "minazim" , minazim },
            { "maxazim" , maxazim },
            { "threshelev" , threshelev }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["align"].is_null()) align.from_json(parsed["align"].dump());
            if(!parsed["azim"].is_null()) azim = parsed["azim"].number_value();
            if(!parsed["elev"].is_null()) elev = parsed["elev"].number_value();
            if(!parsed["minelev"].is_null()) minelev = parsed["minelev"].number_value();
            if(!parsed["maxelev"].is_null()) maxelev = parsed["maxelev"].number_value();
            if(!parsed["minazim"].is_null()) minazim = parsed["minazim"].number_value();
            if(!parsed["maxazim"].is_null()) maxazim = parsed["maxazim"].number_value();
            if(!parsed["threshelev"].is_null()) threshelev = parsed["threshelev"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Receiver information
/*! This structure holds the information concerning receivers.
*/
struct rxrstruc : public allstruc
{
    //! Operating mode
    uint16_t opmode;
    //! Data modulation
    uint16_t modulation;
    //! RSSI
    uint16_t rssi;
    //! Packet Size
    uint16_t pktsize;
    //! Input Frequency
    double freq;
    //! Maximum frequency allowed
    double maxfreq;
    //! Minimum frequency allowed
    double minfreq;
    //! Current RX Power
    float powerin;
    //! Current TX Power
    float powerout;
    //! Output Power limit
    float maxpower;
    //! Input Filter bandpass
    float band;
    //! repeater squelch tone frequency
    float squelch_tone;
    //! Good Packet Percentage
    double goodratio;
    //! Last RX time
    double rxutc;
    //! Connection Uptime
    double uptime;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "opmode" , opmode },
            { "modulation" , modulation },
            { "rssi"   , rssi },
            { "pktsize", pktsize },
            { "freq"   , freq },
            { "maxfreq", maxfreq },
            { "minfreq", minfreq },
            { "powerin", powerin },
            { "powerout" , powerout },
            { "maxpower" , maxpower },
            { "band"     , band },
            { "squelch_tone", squelch_tone },
            { "goodratio", goodratio },
            { "rxutc"  , rxutc },
            { "uptime" , uptime }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["opmode"].is_null()) opmode = parsed["opmode"].int_value();
            if(!parsed["modulation"].is_null()) modulation = parsed["modulation"].int_value();
            if(!parsed["rssi"].is_null()) rssi = parsed["rssi"].int_value();
            if(!parsed["pktsize"].is_null()) pktsize = parsed["pktsize"].int_value();
            if(!parsed["freq"].is_null()) freq = parsed["freq"].number_value();
            if(!parsed["maxfreq"].is_null()) maxfreq = parsed["maxfreq"].number_value();
            if(!parsed["minfreq"].is_null()) minfreq = parsed["minfreq"].number_value();
            if(!parsed["powerin"].is_null()) powerin = parsed["powerin"].number_value();
            if(!parsed["powerout"].is_null()) powerout = parsed["powerout"].number_value();
            if(!parsed["maxpower"].is_null()) maxpower = parsed["maxpower"].number_value();
            if(!parsed["band"].is_null()) band = parsed["band"].number_value();
            if(!parsed["squelch_tone"].is_null()) squelch_tone = parsed["squelch_tone"].number_value();
            if(!parsed["goodratio"].is_null()) goodratio = parsed["goodratio"].number_value();
            if(!parsed["rxutc"].is_null()) rxutc = parsed["rxutc"].number_value();
            if(!parsed["uptime"].is_null()) uptime = parsed["uptime"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Transmitter information
/*! This structure holds the information concerning transmitters.
*/
struct txrstruc : public allstruc
{
    //! Operating mode
    uint16_t opmode;
    //! Data modulation
    uint16_t modulation;
    //! RSSI
    uint16_t rssi;
    //! Packet Size
    uint16_t pktsize;
    //! Input Frequency
    double freq;
    //! Maximum frequency allowed
    double maxfreq;
    //! Minimum frequency allowed
    double minfreq;
    //! Current RX Power
    float powerin;
    //! Current TX Power
    float powerout;
    //! Output Power limit
    float maxpower;
    //! Input Filter bandpass
    float band;
    //! repeater squelch tone frequency
    float squelch_tone;
    //! Good Packet Percentage
    double  goodratio;
    //! Last TX time
    double txutc;
    //! Connection Uptime
    double uptime;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "opmode" , opmode },
            { "modulation" , modulation },
            { "rssi"   , rssi },
            { "pktsize", pktsize },
            { "freq"   , freq },
            { "maxfreq", maxfreq },
            { "minfreq", minfreq },
            { "powerin", powerin },
            { "powerout" , powerout },
            { "maxpower" , maxpower },
            { "band"     , band },
            { "squelch_tone", squelch_tone },
            { "goodratio", goodratio },
            { "txutc"  , txutc },
            { "uptime" , uptime }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["opmode"].is_null()) opmode = parsed["opmode"].int_value();
            if(!parsed["modulation"].is_null()) modulation = parsed["modulation"].int_value();
            if(!parsed["rssi"].is_null()) rssi = parsed["rssi"].int_value();
            if(!parsed["pktsize"].is_null()) pktsize = parsed["pktsize"].int_value();
            if(!parsed["freq"].is_null()) freq = parsed["freq"].number_value();
            if(!parsed["maxfreq"].is_null()) maxfreq = parsed["maxfreq"].number_value();
            if(!parsed["minfreq"].is_null()) minfreq = parsed["minfreq"].number_value();
            if(!parsed["powerin"].is_null()) powerin = parsed["powerin"].number_value();
            if(!parsed["powerout"].is_null()) powerout = parsed["powerout"].number_value();
            if(!parsed["maxpower"].is_null()) maxpower = parsed["maxpower"].number_value();
            if(!parsed["band"].is_null()) band = parsed["band"].number_value();
            if(!parsed["squelch_tone"].is_null()) squelch_tone = parsed["squelch_tone"].number_value();
            if(!parsed["goodratio"].is_null()) goodratio = parsed["goodratio"].number_value();
            if(!parsed["txutc"].is_null()) txutc = parsed["txutc"].number_value();
            if(!parsed["uptime"].is_null()) uptime = parsed["uptime"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Transceiver information
/*! This structure holds the information concerning transceivers.
*/
struct tcvstruc : public allstruc
{
    //! Operating mode
    uint16_t opmode;
    //! Data modulation
    uint16_t modulation;
    //! RSSI
    uint16_t rssi;
    //! Packet Size
    uint16_t pktsize;
    //! Input Frequency
    double freq;
    //! Maximum frequency allowed
    double maxfreq;
    //! Minimum frequency allowed
    double minfreq;
    //! Current RX Power
    float powerin;
    //! Current TX Power
    float powerout;
    //! Output Power limit
    float maxpower;
    //! Input Filter bandpass
    float band;
    //! repeater squelch tone frequency
    float squelch_tone;
    //! Good Packet Percentage
    double  goodratio;
    //! Last TX time
    double txutc;
    //! Last RX time
    double rxutc;
    //! Connection Uptime
    double uptime;

        // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "opmode" , opmode },
            { "modulation" , modulation },
            { "rssi"   , rssi },
            { "pktsize", pktsize },
            { "freq"   , freq },
            { "maxfreq", maxfreq },
            { "minfreq", minfreq },
            { "powerin", powerin },
            { "powerout" , powerout },
            { "maxpower" , maxpower },
            { "band"     , band },
            { "squelch_tone", squelch_tone },
            { "goodratio", goodratio },
            { "txutc"  , txutc },
            { "rxutc"  , rxutc },
            { "uptime" , uptime }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["opmode"].is_null()) opmode = parsed["opmode"].int_value();
            if(!parsed["modulation"].is_null()) modulation = parsed["modulation"].int_value();
            if(!parsed["rssi"].is_null()) rssi = parsed["rssi"].int_value();
            if(!parsed["pktsize"].is_null()) pktsize = parsed["pktsize"].int_value();
            if(!parsed["freq"].is_null()) freq = parsed["freq"].number_value();
            if(!parsed["maxfreq"].is_null()) maxfreq = parsed["maxfreq"].number_value();
            if(!parsed["minfreq"].is_null()) minfreq = parsed["minfreq"].number_value();
            if(!parsed["powerin"].is_null()) powerin = parsed["powerin"].number_value();
            if(!parsed["powerout"].is_null()) powerout = parsed["powerout"].number_value();
            if(!parsed["maxpower"].is_null()) maxpower = parsed["maxpower"].number_value();
            if(!parsed["band"].is_null()) band = parsed["band"].number_value();
            if(!parsed["squelch_tone"].is_null()) squelch_tone = parsed["squelch_tone"].number_value();
            if(!parsed["goodratio"].is_null()) goodratio = parsed["goodratio"].number_value();
            if(!parsed["txutc"].is_null()) txutc = parsed["txutc"].number_value();
            if(!parsed["rxutc"].is_null()) rxutc = parsed["rxutc"].number_value();
            if(!parsed["uptime"].is_null()) uptime = parsed["uptime"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! PV String (STRG) structure.
/*! Efficiency is goven as effbase + effslope * Tkelvin.
*/
struct pvstrgstruc : public allstruc
{
    //! BCREG index
    uint16_t bcidx;
    //! Efficiency 0th order term
    float effbase;
    //! Efficiency 1st order term
    float effslope;
    //! Maximum power generation in Watts
    float maxpower;
    //! Current power being generated in Watts.
    float power;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "bcidx"  , bcidx },
            { "effbase", effbase },
            { "effslope", effslope },
            { "maxpower", maxpower },
            { "power"  , power}
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["bcidx"].is_null()) bcidx = parsed["bcidx"].int_value();
            if(!parsed["effbase"].is_null()) effbase = parsed["effbase"].number_value();
            if(!parsed["effslope"].is_null()) effslope = parsed["effslope"].number_value();
            if(!parsed["maxpower"].is_null()) maxpower = parsed["maxpower"].number_value();
            if(!parsed["power"].is_null()) power = parsed["power"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Battery (BATT) structure.
struct battstruc : public allstruc
{
    //! Capacity in amp hours
    float capacity;
    //! Charge conversion efficiency
    float efficiency;
    //! Charge in amp hours
    float charge;
    //! Resistance in
    float r_in;
    //! Resistance out
    float r_out;
    //! Battery Percentage Remaining
    float percentage;
    //! Time Remaining
    float time_remaining;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "capacity"  , capacity },
            { "efficiency", efficiency },
            { "charge"    , charge },
            { "r_in"  , r_in },
            { "r_out" , r_out },
            { "percentage", percentage },
            { "time_remaining" , time_remaining }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["capacity"].is_null()) capacity = parsed["capacity"].number_value();
            if(!parsed["efficiency"].is_null()) efficiency = parsed["efficiency"].number_value();
            if(!parsed["charge"].is_null()) charge = parsed["charge"].number_value();
            if(!parsed["r_in"].is_null()) r_in = parsed["r_in"].number_value();
            if(!parsed["r_out"].is_null()) r_out = parsed["r_out"].number_value();
            if(!parsed["percentage"].is_null()) percentage = parsed["percentage"].number_value();
            if(!parsed["time_remaining"].is_null()) time_remaining = parsed["time_remaining"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Heater Structure definition
/*! This structure holds the description of a heaters.
*/
struct htrstruc : public allstruc
{
    bool state;
    //! Temperature set vertex
    float setvertex;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "state" , state },
            { "setvertex" , setvertex }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["state"].is_null()) state = parsed["state"].bool_value();
            if(!parsed["setvertex"].is_null()) setvertex = parsed["setvertex"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

struct motrstruc : public allstruc
{
    //! Maximum speed in revolutions per second
    float max;
    //!
    float rat;
    float spd;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "max" , max },
            { "rat" , rat },
            { "spd" , spd }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["max"].is_null()) max = parsed["max"].number_value();
            if(!parsed["rat"].is_null()) rat = parsed["rat"].number_value();
            if(!parsed["spd"].is_null()) spd = parsed["spd"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

struct tsenstruc : public allstruc {};

//! Thruster (THST) dynamic structure
struct thststruc : public allstruc
{
    //! Flow
    quaternion align;
    //! Specific Impulse in dynes per kg per second
    float flw;
    //! Rotation of thrust vector (+z) in to node frame.
    float isp;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "align" , align },
            { "flw"   , flw },
            { "isp"   , isp }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["align"].is_null()) align.from_json(parsed["align"].dump());
            if(!parsed["flw"].is_null()) flw = parsed["flw"].number_value();
            if(!parsed["isp"].is_null()) isp = parsed["isp"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Propellant Tank (PROP) structure.
struct propstruc : public allstruc
{
    //! Propellant capacity in kg
    float cap;
    //! Propellant level in kg
    float lev;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "cap" , cap },
            { "lev" , lev }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["cap"].is_null()) cap = parsed["cap"].number_value();
            if(!parsed["lev"].is_null()) lev = parsed["lev"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Switch Structure definition
/*! This structure holds the description of a switches.
*/
struct swchstruc : public allstruc {};

//! Rotor Structure definition
/*! This structure holds the description of a rotors.
*/
struct rotstruc : public allstruc
{
    //! Angular position
    float angle;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "angle" , angle }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["angle"].is_null()) angle = parsed["angle"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Star Tracker (STT) Sructure
// TODO: rename to ST
struct sttstruc : public allstruc
{
    //! alignment quaternion
    quaternion align;
    //! includes 0 and 1st order derivative
    quaternion att; // TODO: rename to q
    rvector omega;
    rvector alpha;
    //! return code for
    uint16_t retcode;
    uint32_t status;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "align" , align },
            { "att"   , att },
            { "omega" , omega },
            { "alpha" , alpha },
            { "retcode", retcode },
            { "status" , static_cast<int>(status) }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["align"].is_null()) align.from_json(parsed["align"].dump());
            if(!parsed["att"].is_null()) att.from_json(parsed["att"].dump());
            if(!parsed["omega"].is_null()) omega.from_json(parsed["omega"].dump());
            if(!parsed["alpha"].is_null()) alpha.from_json(parsed["alpha"].dump());
            if(!parsed["retcode"].is_null()) retcode = parsed["retcode"].int_value();
            if(!parsed["status"].is_null()) status = parsed["status"].int_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Motion Capture Camera (MCC) Structure
struct mccstruc : public allstruc
{
    //! Sensor alignment quaternion.
    quaternion align;
    //! attitude
    quaternion q;
    rvector o;
    rvector a;

        // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "align" , align },
            { "q"     , q },
            { "o"     , o },
            { "a"     , a }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["align"].is_null()) align.from_json(parsed["align"].dump());
            if(!parsed["q"].is_null()) q.from_json(parsed["q"].dump());
            if(!parsed["o"].is_null()) o.from_json(parsed["o"].dump());
            if(!parsed["a"].is_null()) a.from_json(parsed["a"].dump());
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Torque Rod Control Unit
struct tcustruc : public allstruc
{
    //! Torque Rod count
    uint16_t mcnt;
    //! Torque Rod Component indices
    uint16_t mcidx[3];

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "mcnt" , mcnt },
            { "mcidx", json11::Json::carray_to_vector(mcidx, sizeof(mcidx)/sizeof(mcidx[0])) }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["mcnt"].is_null()) mcnt = parsed["mcnt"].int_value();
            if(!parsed["mcidx"].is_null()) {
                auto p_mcidx = parsed["mcidx"].array_items();
                for(size_t i = 0; i != p_mcidx.size(); ++i) {
					// SCOTTNOTE: add is_null check
                    mcidx[i] = p_mcidx[i].int_value();
                }
            }
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

struct busstruc : public allstruc
{
    //! Watch Dog Timer (MJD)
    float wdt;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "wdt" , wdt }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["wdt"].is_null()) wdt = parsed["wdt"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

struct psenstruc : public allstruc
{
    //! Current Pressure
    float press;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "press" , press }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["press"].is_null()) press = parsed["press"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! SUCHI Sructure
struct suchistruc : public allstruc
{
    //! alignment quaternion
    quaternion align;
    //! Internal pressure
    float press;
    //! Internal temperatures
    float temps[8];

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "align" , align },
            { "press" , press },
            { "temps" , json11::Json::carray_to_vector(temps, sizeof(temps)/sizeof(temps[0])) }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["align"].is_null()) align.from_json(parsed["align"].dump());
            if(!parsed["press"].is_null()) press = parsed["press"].number_value();
            if(!parsed["temps"].is_null()) {
                auto p_temps = parsed["temps"].array_items();
                for(size_t i = 0; i != p_temps.size(); ++i) {
					// SCOTTNOTE:  add is_null check
                    temps[i] = p_temps[i].number_value();
                }
            }
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

struct camstruc : public allstruc
{
    uint16_t pwidth;
    uint16_t pheight;
    float width;
    float height;
    float flength;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "pwidth" , pwidth },
            { "pheight", pheight },
            { "width"  , width },
            { "height" , height },
            { "flength", flength }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["pwidth"].is_null()) pwidth = parsed["pwidth"].int_value();
            if(!parsed["pheight"].is_null()) pheight = parsed["pheight"].int_value();
            if(!parsed["width"].is_null()) width = parsed["width"].number_value();
            if(!parsed["height"].is_null()) height = parsed["height"].number_value();
            if(!parsed["flength"].is_null()) flength = parsed["flength"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! TNC Structure definition
/*! This structure holds the description of a TNC.
*/
struct tncstruc : public allstruc {};

//! BCREG (PV Regulator) Structure definition
/*! This structure holds the description of a BCREG.
*/
struct bcregstruc : public allstruc {};

// End of Device Specific structures

//! Finite Triangle Element
//! Holds minimum information necessary to use smallest possible triangular element
//! of a larger piece.
struct trianglestruc
{
    //! External facing?
    bool external = true;
    //! center of mass
    Vector com;
    //! outward facing normal
    Vector normal;
    //! Contribution of triangle to linear forces
    Vector shove;
    //! Contribution of triangle to angular forces
    Vector twist;
    //! Index to parent piece
    uint16_t pidx;
    uint16_t tidx[3];
    //! Energy content in Joules
    float heat;
    //! Heat Capacity in Joules / (Kg Kelvin)
    float hcap = 900.;
    //! Emissivity: 0-1
    float emi = .9f;
    //! Absorptivity: 0-1
    float abs = .9f;
    //! mass in Kg
    float mass = 1.;
    //! Temperature in Kelvin
    float temp;
    //! Area
    float area;
    //! perimeter
    float perimeter;
    //! Insolation in Watts/sq m
    float irradiation;
    //! Solar cell coverage
    float pcell = 0.;
    //! Solar cell base efficiency
    float ecellbase = .25;
    //! Solar cell efficiency with temp
    float ecellslope = 0.;
    vector<vector<size_t>> triangleindex;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "external" , external },
            { "com" , com },
            { "normal" , normal },
            { "shove" , shove },
            { "twist" , twist },
            { "pidx"  , pidx },
            { "tidx"  , json11::Json::carray_to_vector(tidx, sizeof(tidx)/sizeof(tidx[0])) },
            { "heat"  , heat },
            { "hcap"  , hcap },
            { "emi"   , emi },
            { "abs"   , abs },
            { "mass"  , mass },
            { "temp"  , temp },
            { "area"  , area },
            { "perimeter"   , perimeter },
            { "irradiation" , irradiation },
            { "pcell" , pcell },
            { "ecellbase"   , ecellbase },
            { "ecellslope"  , ecellslope },
//  TODO:          { "triangleindex" , triangleindex }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
// TODO:
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Physics Simulation Structure
/*! Holds parameters used specifically for the physical simulation of the
 * environment and hardware of a Node.
*/
struct physicsstruc
{
    //! Time step in seconds
    double dt = 0.;
    //! Time step in Julian days
    double dtj = 0.;
    //! Simulated starting time in MJD
    double utc = 0.;
    //! Acceleration factor for simulated time
    double mjdaccel;
    //! Offset factor for simulated time (simtime = mjdaccel * realtime + mjddiff)
    double mjddiff;

    float hcap = 900.;
    float mass = 1.;
    float temp = 300.;
    float heat = 300. * 900. * 1.;
    float area = .001f;
    float battcap = 36000.;
    float battlev;
    float powgen;
    float powuse;

    //! Simulation mode as listed in \def defs_physics
    int32_t mode;
    Vector ftorque;
    Vector atorque;
    Vector rtorque;
    Vector gtorque;
    Vector htorque;
    Vector hmomentum;
    Vector ctorque;
    Vector fdrag;
    Vector adrag;
    Vector rdrag;
    Vector thrust;
    Vector moi = Vector(1.,1.,1.);
    Vector com;

    vector <Vector> vertices;
    vector <trianglestruc> triangles;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "dt" , dt },
            { "dtj" , dtj },
            { "utc" , utc },
            { "mjdaccel" , mjdaccel },
            { "mjddiff" , mjddiff },
            { "hcap" , hcap },
            { "mass" , mass },
            { "temp" , temp },
            { "heat" , heat },
            { "area" , area },
            { "battcap" , battcap },
            { "battlev" , battlev },
            { "powgen" , powgen },
            { "powuse" , powuse },
            { "mode" , mode }
		};
	}

    // Set class contents from JSON string
    void from_json(const string& js) {
        string error;
        json11::Json parsed = json11::Json::parse(js,error);
        if(error.empty()) {
            if(!parsed["dt"].is_null())	dt = parsed["dt"].number_value();
            if(!parsed["dtj"].is_null())	dtj = parsed["dtj"].number_value();
            if(!parsed["utc"].is_null())	utc = parsed["utc"].number_value();
            if(!parsed["mjdaccel"].is_null())	mjdaccel = parsed["mjdaccel"].number_value();
            if(!parsed["mjddiff"].is_null())	mjddiff = parsed["mjddiff"].number_value();
            if(!parsed["hcap"].is_null())	hcap = parsed["hcap"].number_value();
            if(!parsed["mass"].is_null())	mass = parsed["mass"].number_value();
            if(!parsed["temp"].is_null())	temp = parsed["temp"].number_value();
            if(!parsed["heat"].is_null())	heat = parsed["heat"].number_value();
            if(!parsed["area"].is_null())	area = parsed["area"].number_value();
            if(!parsed["battcap"].is_null())	battcap = parsed["battcap"].number_value();
            if(!parsed["battlev"].is_null())	battlev = parsed["battlev"].number_value();
            if(!parsed["powgen"].is_null())	powgen = parsed["powgen"].number_value();
            if(!parsed["powuse"].is_null())	powuse = parsed["powuse"].number_value();
            if(!parsed["mode"].is_null())	mode = parsed["mode"].int_value();
        } else {
            cerr<<"ERROR = "<<error<<endl;
        }
        return;
    }


};

//! Node Structure
//! Structure for storing all information about a Node that never changes, or only
//! changes slowly. The information for initializing this should be in node.ini.
struct nodestruc
{
    //! Node Name.
    char name[COSMOS_MAX_NAME+1];
    //! Last event
    char lastevent[COSMOS_MAX_NAME+1];
    //! Last event UTC
    double lasteventutc;
    //! Node Type as listed in \ref NODE_TYPE.
    uint16_t type;
    //! Operational state
    uint16_t state;

	// actually these are cosmosstruc counts...
    uint16_t vertex_cnt = 0;
    uint16_t normal_cnt = 0;
    uint16_t face_cnt = 0;
    uint16_t piece_cnt = 0;
    uint16_t device_cnt = 0;
    uint16_t port_cnt = 0;
    uint16_t agent_cnt = 0;
    uint16_t event_cnt = 0;
    uint16_t target_cnt = 0;
    uint16_t user_cnt = 0;
    uint16_t glossary_cnt = 0;
    uint16_t tle_cnt = 0;

    uint16_t flags;
    int16_t powmode;
    //! Seconds Node will be down
    uint32_t downtime;
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
    physicsstruc phys;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "name" , name },
            { "lastevent" , lastevent },
            { "lasteventutc" , lasteventutc },
            { "type" , type },
            { "state" , state },

            { "vertex_cnt" , vertex_cnt },
            { "normal_cnt" , normal_cnt },
            { "face_cnt" , face_cnt },
            { "piece_cnt" , piece_cnt },
            { "device_cnt" , device_cnt },
            { "port_cnt" , port_cnt },
            { "agent_cnt" , agent_cnt },
            { "event_cnt" , event_cnt },
            { "target_cnt" , target_cnt },
            { "user_cnt" , user_cnt },
            { "glossary_cnt" , glossary_cnt },
            { "tle_cnt" , tle_cnt },

            { "flags" , flags },
            { "powmode" , powmode },
            { "downtime" , static_cast<int>(downtime) },
            { "azfrom" , azfrom },
            { "elfrom" , elfrom },
            { "azto" , azto },
            { "elto" , elto },
            { "range" , range },
            { "utcoffset" , utcoffset },
            { "utc" , utc },
            { "utcstart" , utcstart },
			{ "loc" , loc },
			{ "phys" , phys }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& js) {
        string error;
        json11::Json parsed = json11::Json::parse(js,error);
        if(error.empty()) {
            if(!parsed["name"].is_null())			strcpy(name, parsed["name"].string_value().c_str());
            if(!parsed["lastevent"].is_null())		strcpy(lastevent, parsed["lastevent"].string_value().c_str());
            if(!parsed["lasteventutc"].is_null())	lasteventutc = parsed["lasteventutc"].number_value();
            if(!parsed["type"].is_null())	type = parsed["type"].int_value();
            if(!parsed["state"].is_null())	state = parsed["state"].int_value();

            if(!parsed["vertex_cnt"].is_null())    vertex_cnt = parsed["vertex_cnt"].int_value();
            if(!parsed["normal_cnt"].is_null())    normal_cnt = parsed["normal_cnt"].int_value();
            if(!parsed["face_cnt"].is_null())    face_cnt = parsed["face_cnt"].int_value();
            if(!parsed["piece_cnt"].is_null())    piece_cnt = parsed["piece_cnt"].int_value();
            if(!parsed["device_cnt"].is_null())    device_cnt = parsed["device_cnt"].int_value();
            if(!parsed["port_cnt"].is_null())    port_cnt = parsed["port_cnt"].int_value();
            if(!parsed["agent_cnt"].is_null())    agent_cnt = parsed["agent_cnt"].int_value();
            if(!parsed["event_cnt"].is_null())    event_cnt = parsed["event_cnt"].int_value();
            if(!parsed["target_cnt"].is_null())    target_cnt = parsed["target_cnt"].int_value();
            if(!parsed["user_cnt"].is_null())    user_cnt = parsed["user_cnt"].int_value();
            if(!parsed["glossary_cnt"].is_null())    glossary_cnt = parsed["glossary_cnt"].int_value();
            if(!parsed["tle_cnt"].is_null())    tle_cnt = parsed["tle_cnt"].int_value();

            if(!parsed["flags"].is_null())    flags = parsed["flags"].int_value();
            if(!parsed["powmode"].is_null())    powmode = parsed["powmode"].int_value();
            if(!parsed["downtime"].is_null())    downtime = parsed["downtime"].int_value();
            if(!parsed["azfrom"].is_null())    azfrom = parsed["azfrom"].number_value();
            if(!parsed["elfrom"].is_null())    elfrom = parsed["elfrom"].number_value();
            if(!parsed["azto"].is_null())    azto = parsed["azto"].number_value();
            if(!parsed["elto"].is_null())    elto = parsed["elto"].number_value();
            if(!parsed["range"].is_null())    range = parsed["range"].number_value();
            if(!parsed["utcoffset"].is_null())    utc = parsed["utcoffset"].number_value();
            if(!parsed["utc"].is_null())    utc = parsed["utc"].number_value();
            if(!parsed["utcstart"].is_null())    utc = parsed["utcstart"].number_value();
            if(!parsed["loc"].is_null())    loc.from_json(parsed["loc"].dump());
            if(!parsed["phys"].is_null())    phys.from_json(parsed["phys"].dump());
        } else {
            cerr<<"ERROR = "<<error<<endl;
        }
        return;
    }
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
        bcregstruc bcreg;
        busstruc bus;
        camstruc cam;
        cpustruc cpu;
        diskstruc disk;
        gpsstruc gps;
        htrstruc htr;
        imustruc imu;
        mccstruc mcc;
        motrstruc motr;
        mtrstruc mtr;
        ploadstruc pload;
        propstruc prop;
        psenstruc psen;
        pvstrgstruc pvstrg;
        rotstruc rot;
        rwstruc rw;
        rxrstruc rxr;
        ssenstruc ssen;
        sttstruc stt;
        suchistruc suchi;
        swchstruc swch;
        tcustruc tcu;
        tcvstruc tcv;
        telemstruc telem;
        thststruc thst;
        tncstruc tnc;
        tsenstruc tsen;
        txrstruc txr;
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
    uint16_t disk_cnt;
    uint16_t gps_cnt;
    uint16_t htr_cnt;
    uint16_t imu_cnt;
    uint16_t mcc_cnt;
    uint16_t motr_cnt;
    uint16_t mtr_cnt;
    uint16_t pload_cnt;
    uint16_t prop_cnt;
    uint16_t psen_cnt;
    uint16_t bcreg_cnt;
    uint16_t rot_cnt;
    uint16_t rw_cnt;
    uint16_t rxr_cnt;
    uint16_t ssen_cnt;
    uint16_t pvstrg_cnt;
    uint16_t stt_cnt;
    uint16_t suchi_cnt;
    uint16_t swch_cnt;
    uint16_t tcu_cnt;
    uint16_t tcv_cnt;
    uint16_t telem_cnt;
    uint16_t thst_cnt;
    uint16_t tsen_cnt;
    uint16_t tnc_cnt;
    uint16_t txr_cnt;
    vector<uint16_t>all;
    vector<uint16_t>ant;
    vector<uint16_t>batt;
    vector<uint16_t>bcreg;
    vector<uint16_t>bus;
    vector<uint16_t>cam;
    vector<uint16_t>cpu;
    vector<uint16_t>disk;
    vector<uint16_t>gps;
    vector<uint16_t>htr;
    vector<uint16_t>imu;
    vector<uint16_t>mcc;
    vector<uint16_t>motr;
    vector<uint16_t>mtr;
    vector<uint16_t>pload;
    vector<uint16_t>prop;
    vector<uint16_t>psen;
    vector<uint16_t>pvstrg;
    vector<uint16_t>rot;
    vector<uint16_t>rw;
    vector<uint16_t>rxr;
    vector<uint16_t>ssen;
    vector<uint16_t>stt;
    vector<uint16_t>suchi;
    vector<uint16_t>swch;
    vector<uint16_t>tcu;
    vector<uint16_t>tcv;
    vector<uint16_t>telem;
    vector<uint16_t>thst;
    vector<uint16_t>tnc;
    vector<uint16_t>tsen;
    vector<uint16_t>txr;
};

//! JSON map offset entry
/*! Single entry in a JSON offset map. Ties together a single JSON name and a offset
 * to a single object, along with its data type.
 * - index: Index of this entry in the ::cosmosstruc::jmap.
 * - data: Offset to appropriate storage for this data type.
*/
struct jsonentry
{
    //! Enabled?
    bool enabled;
    //! JSON Data Type
    uint16_t type;
    //! JSON Data Group
    uint16_t group;
    //! Name of entry
    string name;
    //! offset to data storage
    ptrdiff_t offset;
    //! pointer to data storage
    uint8_t* ptr;
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

void replace(std::string& str, const std::string& from, const std::string& to);
vector<size_t> find_newlines(const string& sample);
void pretty_form(string& js);

struct cosmosstruc
{
    //! Timestamp for last change to data
    double timestamp = 0.;

    //! Whether JSON map has been created.
    uint16_t jmapped = 0;

    //! JSON Namespace Map matrix. first entry hash, second is items with that hash
    vector<vector<jsonentry> > jmap;

    //! JSON Equation Map matrix.
    vector<vector<jsonequation> > emap;

    //! JSON Unit Map matrix: first level is for unit type, second level is for all variants (starting with primary).
    vector<vector<unitstruc> > unit; // works

    //! Vector of Equations
    vector<equationstruc> equation;

    //! Array of Aliases
    vector<aliasstruc> alias;

    //! Structure for summary information in node
    nodestruc node;

    //! Vector of all vertexs in node.
    vector <vertexstruc> vertexs;

    //! Vector of all vertexs in node.
    vector <vertexstruc> normals;

    //! Vector of all faces in node.
    vector <facestruc> faces;

    //! Vector of all pieces in node.
    vector<piecestruc> pieces;

    //! Wavefront obj structure
    wavefront obj;

    //! Vector of all general (common) information for devices (components) in node.
    vector<devicestruc> device;

    //! Structure for devices (components) special data in node, by type.
    devspecstruc devspec;

    //! Vector of all ports known to node.
    vector<portstruc> port;

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

    //! Array of Two Line Elements
    vector<tlestruc> tle;

    //! JSON descriptive information
    jsonnode json;


	/// Support for Namespace 2.0

	using name_map = map<string,void*>;
	using name_mapping = pair<string,void*>;

	name_map names;

	bool name_exists(const string& s)	{ return (names.find(s) == names.end()) ? false : true; }

	void print_all_names() const	{
		name_map::const_iterator it = names.begin();
		while(it != names.end())	{ cout<<(it++)->first<<endl; }
	}

	void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };
	//TODO:   add remove_name(..), change_name(..) functions
	
	string get_name(void* v)	{
		name_map::const_iterator it = names.begin();
		while(it->second != v && it != names.end())	{ it++; }
		if(it == names.end())	{	cerr<<"address <"<<v<<"> not found!"<<endl; return "";	}
		return it->first;
	}

	template<class T>
	T* get_pointer(const string& s) const	{
		name_map::const_iterator it = names.find(s);
		if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return nullptr;	}
		return (T*)(it->second);
	}

	template<class T>
	T get_value(const string& s) const	{
		// change to static null object?
		T dummy = T();
		name_map::const_iterator it = names.find(s);
		if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return dummy;	}
		return *get_pointer<T>(s);
	}

	template<class T>
	void set_value(const string& s, const T& value) const	{
		// maybe if not found should be inserted??  hmmm....  ask Eric
		name_map::const_iterator it = names.find(s);
		if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return;	}
		*get_pointer<T>(s) = value;
	}

	template<class T>
	void set_json_value(const string& s, const string& json) const 	{
		get_pointer<T>(s)->from_json(json);
	}

	template<class T>
	string get_json(const string& s)	{
		if(name_exists(s))	{
			json11::Json json = json11::Json::object { { s, this->get_value<T>(s) } };
			return json.dump();
		} else {
			return "";
		}
	}

	void replace(std::string& str, const std::string& from, const std::string& to) {
    	if(from.empty()) return;
    	size_t start_pos = 0;
    	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        	str.replace(start_pos, from.length(), to);
        	start_pos += to.length();
    	}
    	return;
	}
	
	void pretty_form(string& js)    {
	
    	replace(js, ", ", ",\n");
    	replace(js, "{", "{\n");
    	replace(js, "[", "[\n");
    	replace(js, "}", "\n}");
    	replace(js, "]", "\n]");
	
    	// create vector[char position] = # of indents
    	int indent = 0;
    	vector<size_t> indents;
    	for(size_t i = 0; i < js.size(); ++i)   {
        	if(js[i]=='['){ ++indent;}
        	if(js[i]=='{'){ ++indent;}
        	if(js[i]==']'){ --indent; indents[i-1]--;}
        	if(js[i]=='}'){ --indent; indents[i-1]--;}
        	indents.push_back(indent);
    	}
		
    	// find position of all '\n' characters
    	vector<size_t> newlines;
    	for(size_t i =0; i < js.size(); i++) if(js[i] == '\n') newlines.push_back(i);
	
    	// insert the appropriate # of indents after the '\n' char
    	for(size_t i = newlines.size(); i!=0; ) {
        	--i;
        	string indent_string;
        	for(size_t j = 0; j < indents[newlines[i]]; ++j)    indent_string += "  ";
        	js.insert(newlines[i]+1, indent_string);
    	}
    	return;
	}

	template<class T>
	string get_json_pretty(const string& s)	{
		if(name_exists(s))	{
			json11::Json json = json11::Json::object { { s, this->get_value<T>(s) } };
			string pretty = json.dump();
			pretty_form(pretty);
			return pretty;
		} else {
			return "";
		}
	}


    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "timestamp" , timestamp },
            { "jmapped" , jmapped },
            { "unit" , unit },
            { "equation" , equation },
            { "node" , node },
            { "vertexs" , vertexs },
            { "normals" , normals },
			// ... MORE ... //
			{ "user" , user },
			{ "tle" , tle },
			{ "json" , json }
		};
	}

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
			string obj(p.object_items().begin()->first);
 			if(!p[obj]["timestamp"].is_null())	timestamp = p[obj]["timestamp"].number_value();
 			if(!p[obj]["jmapped"].is_null())	jmapped = p[obj]["jmapped"].number_value();
 			for(size_t i = 0; i < unit.size(); ++i)	{
 				for(size_t j = 0; j < unit[i].size(); ++j)	{
 					if(!p[obj]["unit"][i][j].is_null())	unit[i][j].from_json(p[obj]["unit"][i][j].dump());
 				}
 			}
 			for(size_t i = 0; i < equation.size(); ++i)	{
 				if(!p[obj]["equation"][i].is_null())	equation[i].from_json(p[obj]["equation"][i].dump());
			}
 			if(!p[obj]["node"].is_null())	node.from_json(p[obj]["node"].dump());
 			for(size_t i = 0; i < vertexs.size(); ++i)	{
 				if(!p[obj]["vertexs"][i].is_null())	vertexs[i].from_json(p[obj]["vertexs"][i].dump());
			}
 			for(size_t i = 0; i < normals.size(); ++i)	{
 				if(!p[obj]["normals"][i].is_null())	normals[i].from_json(p[obj]["normals"][i].dump());
			}

			// ... MORE ... //


 			for(size_t i = 0; i < user.size(); ++i)	{
 				if(!p[obj]["user"][i].is_null())	user[i].from_json(p[obj]["user"][i].dump());
			}
 			for(size_t i = 0; i < glossary.size(); ++i)	{
 				if(!p[obj]["glossary"][i].is_null())	glossary[i].from_json(p[obj]["glossary"][i].dump());
			}
 			for(size_t i = 0; i < tle.size(); ++i)	{
 				if(!p[obj]["tle"][i].is_null())	tle[i].from_json(p[obj]["tle"][i].dump());
			}
 			if(!p[obj]["json"].is_null())	json.from_json(p[obj]["json"].dump());
		} else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }

	// other namespace member functions??
	// maybe set_json for use with namespace names (calls from_json...)
};

//! @}

#endif
