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
//Namespace 1.0
enum {
    //! Identity
    JSON_UNIT_TYPE_IDENTITY,
    //! Polynomial
    JSON_UNIT_TYPE_POLY,
    //! Logarithm
    JSON_UNIT_TYPE_LOG
};

//! JSON Unit type
//Namespace 1.0
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
//Namespace 1.0
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
    //! ::tlestruc
    JSON_STRUCT_TLE,
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
//Namespace 1.0
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
//Namespace 1.0
enum class JSON_UPDATE : int32_t {
    NONE,
    POS,
    ATT
};

//! Types of equation operands
//Namespace 1.0
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
//Namespace 1.0
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

// Maximums for pre-allocated (and never reallocated) vector storage inside cosmosstruc

//* Maximum number of equations
#define MAX_NUMBER_OF_EQUATIONS 25

//* Maximum number of ports
#define MAX_NUMBER_OF_PORTS 5

//* Maximum number of events
#define MAX_NUMBER_OF_EVENTS 20

//* Maximum number of targets
#define MAX_NUMBER_OF_TARGETS 15

//* Maximum number of users
#define MAX_NUMBER_OF_USERS 10

//* Maximum number of TLES
#define MAX_NUMBER_OF_TLES 50

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

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // string name
        add_name("name", &name);
        
        // uint16_t type
        add_name("type", &type);
        
        // float p0
        add_name("p0", &p0);
        
        // float p1
        add_name("p1", &p1);
        
        // float p2
        add_name("p2", &p2);
    }

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
                        if(!parsed["type"].is_null()) type = parsed["type"].int_value();
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

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // string name
        add_name("name", &name);

        // string node
        add_name("node", &node);

        // string state
        add_name("state", &state);

        // string utcstart
        add_name("utcstart", &utcstart);

        // string vertexs
        add_name("vertexs", &vertexs);

        // string faces
        add_name("faces", &faces);

        // string pieces
        add_name("pieces", &pieces);

        // string devgen
        add_name("devgen", &devgen);

        // string devspec
        add_name("devspec", &devspec);

        // string ports
        add_name("ports", &ports);

        // string targets
        add_name("targets", &targets);

    }

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
//Namespace 1.0
struct jsonhandle
{
    // Hash of equation or name
    uint16_t hash;  // NOTE: Initializing this
    // Index within that hash entry
    uint16_t index;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "hash" , hash },
            { "index", index }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!p["hash"].is_null()) hash = p["hash"].int_value();
            if(!p["index"].is_null()) index = p["index"].int_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! JSON token
/*! Tokenized version of a single JSON object. The token is a handle to the location
 * in the JSON map represented by the string portion, and the value portion stored as a string.
 */
//Namespace 1.0
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
//Namespace 1.0
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
//Namespace 1.0
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
    bool exists = true;

        // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "utc"   , utc },
            { "node"  , node },
            { "proc"  , proc },
            { "ntype" , static_cast<int>(ntype) },
            { "addr"  , addr },
            { "port"  , port },
            { "bsz"   , static_cast<int>(bsz) },
            { "bprd"  , bprd },
            { "user"  , user },
            { "cpu"   , cpu },
            { "memory", memory },
            { "jitter", jitter },
            { "exists", exists }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!p["utc"].is_null()) utc = p["utc"].number_value();
            if(!p["node"].is_null()) strcpy(node, p["node"].string_value().c_str());
            if(!p["proc"].is_null()) strcpy(proc, p["proc"].string_value().c_str());
            if(!p["ntype"].is_null()) ntype = static_cast<NetworkType>(p["ntype"].int_value());
            if(!p["addr"].is_null()) strcpy(addr, p["addr"].string_value().c_str());
            if(!p["port"].is_null()) port = p["port"].int_value();
            if(!p["bsz"].is_null()) bsz = p["bsz"].int_value();
            if(!p["bprd"].is_null()) bprd = p["bprd"].number_value();
            if(!p["user"].is_null()) strcpy(user, p["user"].string_value().c_str());
            if(!p["cpu"].is_null()) cpu = p["cpu"].number_value();
            if(!p["memory"].is_null()) memory = p["memory"].number_value();
            if(!p["jitter"].is_null()) jitter = p["jitter"].number_value();
            if(!p["exists"].is_null()) exists = p["exists"].bool_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Agent control structure
// JIMNOTE: take a look at this mess
struct agentstruc
{
    //! Client initialized?
    bool client = true;
    //! Subscription channel (for Client)
    socket_channel sub;
    //! Server initialized?
    bool server = true;
    //! Number of network interfaces
    size_t ifcnt = 0;
    //! Publication channels for each interface (for Server)
    socket_channel pub[AGENTMAXIF];
    //! Request channel (for Server)
    socket_channel req;
    //! Agent process ID
    int32_t pid = 0;
    //! Activity period in seconds
    double aprd = 0.;
    //! Agent Running State Flag
    uint16_t stateflag = 0;
    //! Agent request list
    vector <agent_request_entry> reqs;
    //! Heartbeat
    beatstruc beat;

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // bool client
        add_name("client", &client);

        // socket_channel sub
        add_name("sub", &sub);
        
        // bool server
        add_name("server", &server);
        
        // size_t ifcnt
        add_name("ifcnt", &ifcnt);
        
        // socket_channel pub[AGENTMAXIF]
        add_name("pub", &pub);
        for(size_t i = 0; i < sizeof(pub)/sizeof(pub[0]); ++i) {
            string basename = "pub[" + std::to_string(i) + "]";
            add_name(basename, &pub[i]);
        }
        
        // socket_channel req
        add_name("req", &req);
        
        // int32_t pid
        add_name("pid", &pid);
        
        // double aprd
        add_name("aprd", &aprd);
        
        // uint16_t stateflag
        add_name("stateflag", &stateflag);
        
        // vector <agent_request_entry> reqs
        add_name("reqs", &reqs);
        for(size_t i = 0; i < reqs.capacity(); ++i) {
            string basename = "reqs[" + std::to_string(i) + "]";
            add_name(basename, &reqs[i]);
        }
        
        // beatstruc beat
        add_name("beat", &beat);
        
    }

    // Convert class contents to JSON object
    json11::Json to_json() const {
//        vector<uint16_t> v_pub = vector<uint16_t>(pub, pub+AGENTMAXIF);
        return json11::Json::object {
            { "client" , client },
//TODO?            { "sub"    , sub },
            { "server" , server },
            { "ifcnt"  , static_cast<int>(ifcnt) },
//            { "pub"    , v_pub },
//            { "req"    , req },
            { "pid"    , pid },
            { "aprd"   , aprd },
            { "stateflag" , stateflag },
//            { "reqs"   , reqs },
            { "beat"   , beat }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!p["client"].is_null()) client = p["client"].bool_value();
//TODO?            if(!p["sub"].is_null()) sub.from_json(p["sub"].dump());
            if(!p["server"].is_null()) server = p["server"].bool_value();
            if(!p["ifcnt"].is_null()) ifcnt = static_cast<size_t>(p["ifcnt"].number_value());
            auto p_pub = p["pub"].array_items();
//            if(!p["pub"].is_null()) {
//                for(size_t i = 0; i < p_pub.size(); ++i)	{
// 				    if(!p["pub"][i].is_null())	pub[i].from_json(p["pub"][i].dump());
//			    }
//            }
//            if(!p["req"].is_null()) req.from_json(p["req"].dump());
            if(!p["pid"].is_null()) pid = p["pid"].int_value();
            if(!p["aprd"].is_null()) aprd = p["aprd"].number_value();
            if(!p["stateflag"].is_null()) stateflag = p["stateflag"].int_value();
//            if(!p["reqs"].is_null()) {
//                for(size_t i = 0; i < reqs.size(); ++i) {
//                    if(!p["reqs"][i].is_null()) reqs[i].from_json(p["reqs"][i].dump());
//                }
//            }
            if(!p["beat"].is_null()) beat.from_json(p["beat"].dump());
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
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

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // double utc
        add_name("utc", &utc);

        // double utcexec
        add_name("utcexec", &utcexec);
        
        // char node[COSMOS_MAX_NAME+1]
        add_name("node", &node);
        for(size_t i = 0; i < sizeof(node)/sizeof(node[0]); ++i) {
            string basename = "node[" + std::to_string(i) + "]";
            add_name(basename, &node[i]);
        }

        // char name[COSMOS_MAX_NAME+1]
        add_name("name", &name);
        for(size_t i = 0; i < sizeof(name)/sizeof(name[0]); ++i) {
            string basename = "name[" + std::to_string(i) + "]";
            add_name(basename, &name[i]);
        }

        // char user[COSMOS_MAX_NAME+1]
        add_name("user", &user);
        for(size_t i = 0; i < sizeof(user)/sizeof(user[0]); ++i) {
            string basename = "user[" + std::to_string(i) + "]";
            add_name(basename, &user[i]);
        }

        // uint32_t flag
        add_name("flag", &flag);
        
        // uint32_t type
        add_name("type", &type);
        
        // double value
        add_name("value", &value);
        
        // double dtime
        add_name("dtime", &dtime);
        
        // double ctime
        add_name("ctime", &ctime);
        
        // float denergy
        add_name("denergy", &denergy);
        
        // float cenergy
        add_name("cenergy", &cenergy);
        
        // float dmass
        add_name("dmass", &dmass);
        
        // float cmass
        add_name("cmass", &cmass);
        
        // float dbytes
        add_name("dbytes", &dbytes);
        
        // float cbytes
        add_name("cbytes", &cbytes);
        
        // jsonhandle handle
        add_name("handle", &handle);
        
        // char data[JSON_MAX_DATA]
        add_name("data", &data);
        for(size_t i = 0; i < sizeof(data)/sizeof(data[0]); ++i) {
            string basename = "data[" + std::to_string(i) + "]";
            add_name(basename, &data[i]);
        }

        // char condition[JSON_MAX_DATA]
        add_name("condition", &condition);
        for(size_t i = 0; i < sizeof(condition)/sizeof(condition[0]); ++i) {
            string basename = "condition[" + std::to_string(i) + "]";
            add_name(basename, &condition[i]);
        }

    }

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "utc"   , utc },
            { "utcexec" , utcexec },
            { "node"  , node },
            { "name"  , name },
            { "user"  , user },
            { "flag"  , static_cast<int>(flag) },
            { "type"  , static_cast<int>(type) },
            { "value" , value },
            { "dtime" , dtime },
            { "ctime" , ctime },
            { "denergy" , denergy },
            { "cenergy" , cenergy },
            { "dmass"   , dmass },
            { "cmass"   , cmass },
            { "dbytes"  , dbytes },
            { "cbytes"  , cbytes },
            { "handle"  , handle },
            { "data"    , data },
            { "condition" , condition }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!p["utc"].is_null()) utc = p["utc"].number_value();
            if(!p["utcexec"].is_null()) utcexec = p["utcexec"].number_value();
            if(!p["node"].is_null()) strcpy(node, p["node"].string_value().c_str());
            if(!p["name"].is_null()) strcpy(name, p["name"].string_value().c_str());
            if(!p["user"].is_null()) strcpy(user, p["user"].string_value().c_str());
            if(!p["flag"].is_null()) flag = p["flag"].int_value();
            if(!p["type"].is_null()) type = p["type"].int_value();
            if(!p["value"].is_null()) value = p["value"].number_value();
            if(!p["dtime"].is_null()) dtime = p["dtime"].number_value();
            if(!p["ctime"].is_null()) ctime = p["ctime"].number_value();
            if(!p["denergy"].is_null()) denergy = p["denergy"].number_value();
            if(!p["cenergy"].is_null()) cenergy = p["cenergy"].number_value();
            if(!p["dmass"].is_null()) dmass = p["dmass"].number_value();
            if(!p["cmass"].is_null()) cmass = p["cmass"].number_value();
            if(!p["dbytes"].is_null()) dbytes = p["dbytes"].number_value();
            if(!p["cbytes"].is_null()) cbytes = p["cbytes"].number_value();
            if(!p["handle"].is_null()) handle.from_json(p["handle"].dump());
            if(!p["data"].is_null()) strcpy(data, p["data"].string_value().c_str());
            if(!p["condition"].is_null()) strcpy(condition, p["condition"].string_value().c_str());

        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

struct userstruc
{
        //to enforce a std::string name length maximum (if necessary) use this code to truncate:
        //
        //constexpr std::string::size_type MAX_CHARS = 20 ;
        //if(whatever_string.size() > MAX_CHARS)	{
        //	whatever_string = whatever_string.substr(0, MAX_CHARS);
        //}
        string name = "";
        string node = "";
        string tool = "";
        string cpu = "";
    
    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // string name
        add_name("name", &name);

        // string node
        add_name("node", &node);

        // string tool
        add_name("tool", &tool);

        // string cpu
        add_name("cpu", &cpu);
    }

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

//! Equation structure
/*! Contains the name of an equation and the Equation string it vertexs to.
*/
struct equationstruc
{
    // Equation name
    string name;
    // Equation string
    string value;

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // string name
        add_name("name", &name);

        // string value
        add_name("value", &value);
    }

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
    double utc = 0.;
    char name[COSMOS_MAX_NAME+1] = "";
    uint16_t type = 0;
    float azfrom = 0.f;
    float elfrom = 0.f;
    float azto = 0.f;
    float elto = 0.f;
    double range = 0.;
    double close = 0.;
    float min = 0.f;
    locstruc loc;

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // double utc
        add_name("utc", &utc);

        // char name[COSMOS_MAX_NAME+1]
        add_name("name", &name);
        for(size_t i = 0; i < sizeof(name)/sizeof(name[0]); ++i) {
            string basename = "name[" + std::to_string(i) + "]";
            add_name(basename, &name[i]);
        }

        // uint16_t type
        add_name("type", &type);
        
        // float azfrom
        add_name("azfrom", &azfrom);
        
        // float elfrom
        add_name("elfrom", &elfrom);
        
        // float azto
        add_name("azto", &azto);
        
        // float elto
        add_name("elto", &elto);
        
        // double range
        add_name("range", &range);
        
        // double close
        add_name("close", &close);
        
        // float min
        add_name("min", &min);
        
        // locstruc loc
        add_name("loc", &loc);
        
    }

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "utc"    , utc },
            { "name"   , name },
            { "type"   , type },
            { "azfrom" , azfrom },
            { "elfrom" , elfrom },
            { "azto"   , azto },
            { "elto"   , elto },
            { "range"  , range },
            { "close"  , close },
            { "min"    , min },
            { "loc"    , loc }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!p["utc"].is_null()) utc = p["utc"].number_value();
            if(!p["name"].is_null()) strcpy(name, p["name"].string_value().c_str());
            if(!p["type"].is_null()) type = p["type"].int_value();
            if(!p["azfrom"].is_null()) azfrom = p["azfrom"].number_value();
            if(!p["elfrom"].is_null()) elfrom = p["elfrom"].number_value();
            if(!p["azto"].is_null()) azto = p["azto"].number_value();
            if(!p["elto"].is_null()) elto = p["elto"].number_value();
            if(!p["range"].is_null()) range = p["range"].number_value();
            if(!p["close"].is_null()) close = p["close"].number_value();
            if(!p["min"].is_null()) min = p["min"].number_value();
            if(!p["loc"].is_null()) loc.from_json(p["loc"].dump());
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Port structure
/*! Contains information about I/O ports available to devices. The
 * ::allstruc::portidx entry in each device structure indicates which of these
 * ports a device will use.
 */
struct portstruc
{
    //! Type of I/O as listed in ::PORT_TYPE.
    PORT_TYPE type = PORT_TYPE_NONE;
    //! Name information for port.
    //!!! Do not make this string
    char name[COSMOS_MAX_DATA+1] = "";

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // PORT_TYPE type
        add_name("type", &type);

        // char name[COSMOS_MAX_NAME+1]
        add_name("name", &name);
        for(size_t i = 0; i < sizeof(name)/sizeof(name[0]); ++i) {
            string basename = "name[" + std::to_string(i) + "]";
            add_name(basename, &name[i]);
        }
    }

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "type" , type },
            { "name" , name }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!p["type"].is_null()) type = static_cast<PORT_TYPE>(p["type"].int_value());
            if(!p["name"].is_null()) strcpy(name, p["name"].string_value().c_str());
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Point structure: information on each vertex in a face
typedef Vector vertexstruc;

//! Face structure: information on each face of a piece
struct facestruc
{
    uint16_t vertex_cnt = 0;
    vector <uint16_t> vertex_idx;
    Vector com;
    Vector normal;
    double area=0.;

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // uint16_t vertex_cnt
            add_name("vertex_cnt", &vertex_cnt);

        // vector <uint16_t> vertex_idx
            add_name("vertex_idx", &vertex_idx);
            
        // Vector com
            add_name("com", &com);
            
        // Vector normal
            add_name("normal", &normal);
            
        // double area
            add_name("area", &area);
            
    }

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "vertex_cnt" , vertex_cnt},
            { "vertex_idx" , vertex_idx },
            { "com"    , com },
            { "normal" , normal },
            { "area"   , area }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["vertex_cnt"].is_null()) vertex_cnt = parsed["vertex_cnt"].int_value();
            for(size_t i = 0; i < vertex_idx.size(); ++i)	{
                                if(!parsed["vertex_idx"][i].is_null())	vertex_idx[i] = parsed["vertex_idx"][i].int_value();
                        }
            if(!parsed["com"].is_null()) com.from_json(parsed["com"].dump());
            if(!parsed["normal"].is_null()) normal.from_json(parsed["normal"].dump());
            if(!parsed["area"].is_null()) area = parsed["area"].number_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Part structure: physical information for each piece of Node
struct piecestruc
{
    //! Name of piece
    char name[COSMOS_MAX_NAME+1] = "";
    //! Enabled?
    bool enabled = true;
    //! Component index: -1 if not a Component
    uint16_t cidx = 0;
    //! Density in kg/cu m
    float density = 0.f;
    //! Mass in kg
    float mass = 0.f;
    //! Emissivity: 0-1
    float emi = 0.f;
    //! Absorptivity: 0-1
    float abs = 0.f;
    //! Heat capacity in joules per kelvin
    float hcap = 0.f;
    //! Heat conductivity in Watts per meter per kelvin
    float hcon = 0.f;
    //! Dimension in meters: effect is dependent on Part type
    float dim = 0.f;
    //! Area in square meters
    float area = 0.f;
    //! Volume in cubic meters
    float volume = 0.f;
    //! Number of faces
    uint16_t face_cnt = 0;
    //! Array of vertices/vertexs
    vector <uint16_t> face_idx;
    //! Centroid of piece
    Vector com;
    //! Contribution of piece to linear forces
    Vector shove;
    //! Contribution of piece to angular forces
    Vector twist;
    //! Stored thermal energy
    float heat = 0.f;
    //! Temperature in Kelvins
    float temp = 0.f;
    //! Insolation in Watts/sq m
    float insol = 0.f;
    //! Material density (1. - transparency)
    float material_density = 0.f;
    //! Material ambient reflective qualities
    Vector material_ambient;
    //! Material diffuse reflective qualities
    Vector material_diffuse;
    //! Material specular reflective qualities
    Vector material_specular;

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // char name[COSMOS_MAX_NAME+1]
        add_name("name", &name);
        for(size_t i = 0; i < sizeof(name)/sizeof(name[0]); ++i) {
            string basename = "name[" + std::to_string(i) + "]";
            add_name(basename, &name[i]);
        }

        // bool enabled
        add_name("enabled", &enabled);

        // uint16_t cidx
        add_name("cidx", &cidx);
        
        // float density
        add_name("density", &density);
        
        // float mass
        add_name("mass", &mass);
        
        // float emi
        add_name("emi", &emi);
        
        // float abs
        add_name("abs", &abs);
        
        // float hcap
        add_name("hcap", &hcap);
        
        // float hcon
        add_name("hcon", &hcon);
        
        // float dim
        add_name("dim", &dim);
        
        // float area
        add_name("area", &area);
        
        // float volume
        add_name("volume", &volume);
        
        // uint16_t face_cnt
        add_name("face_cnt", &face_cnt);
        
        // vector <uint16_t> face_idx
        add_name("face_idx", &face_idx);
        for(size_t i = 0; i < face_idx.capacity(); ++i) {
            string basename = "face_idx[" + std::to_string(i) + "]";
            add_name(basename, &face_idx[i]);
        }

        // Vector com
        add_name("com", &com);
        
        // Vector shove
        add_name("shove", &shove);
        
        // Vector twist
        add_name("twist", &twist);
        
        // float heat
        add_name("heat", &heat);
        
        // float temp
        add_name("temp", &temp);
        
        // float insol
        add_name("insol", &insol);
        
        // float material_density
        add_name("material_density", &material_density);
        
        // Vector material_ambient
        add_name("material_ambient", &material_ambient);
        
        // Vector material_diffuse
        add_name("material_diffuse", &material_diffuse);
        
        // Vector material_specular
        add_name("material_specular", &material_specular);
        
    }

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "name"    , name },
            { "enabled" , enabled },
            { "cidx"    , cidx },
            { "density" , density },
            { "mass"    , mass },
            { "emi"     , emi },
            { "abs"     , abs },
            { "hcap"    , hcap },
            { "hcon"    , hcon },
            { "dim"     , dim },
            { "area"    , area },
            { "volume"  , volume },
            { "face_cnt", face_cnt },
            { "face_idx", face_idx },
            { "com"     , com },
            { "shove"   , shove },
            { "twist"   , twist },
            { "heat"    , heat },
            { "temp"    , temp },
            { "insol"   , insol },
            { "material_density" , material_density },
            { "material_ambient" , material_ambient },
            { "material_diffuse" , material_diffuse },
            { "material_specular", material_specular }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!p["name"].is_null()) strcpy(name, p["name"].string_value().c_str());
            if(!p["enabled"].is_null()) enabled = p["enabled"].bool_value();
            if(!p["cidx"].is_null()) cidx = p["cidx"].int_value();
            if(!p["density"].is_null()) density = p["density"].number_value();
            if(!p["mass"].is_null()) mass = p["mass"].number_value();
            if(!p["emi"].is_null()) emi = p["emi"].number_value();
            if(!p["abs"].is_null()) abs = p["abs"].number_value();
            if(!p["hcap"].is_null()) hcap = p["hcap"].number_value();
            if(!p["hcon"].is_null()) hcon = p["hcon"].number_value();
            if(!p["dim"].is_null()) dim = p["dim"].number_value();
            if(!p["area"].is_null()) area = p["area"].number_value();
            if(!p["volume"].is_null()) volume = p["volume"].number_value();
            if(!p["face_cnt"].is_null()) face_cnt = p["face_cnt"].int_value();
            if(!p["face_idx"].is_null()) {
                auto p_face_idx = p["face_idx"].array_items();
                for(size_t i = 0; i != p_face_idx.size(); ++i) {
                    if(!p_face_idx[i].is_null()) face_idx[i] = p_face_idx[i].int_value();
                }
            }
            if(!p["com"].is_null()) com.from_json(p["com"].dump());
            if(!p["shove"].is_null()) shove.from_json(p["shove"].dump());
            if(!p["twist"].is_null()) twist.from_json(p["twist"].dump());
            if(!p["heat"].is_null()) heat = p["heat"].number_value();
            if(!p["temp"].is_null()) temp = p["temp"].number_value();
            if(!p["insol"].is_null()) insol = p["insol"].number_value();
            if(!p["material_density"].is_null()) material_density = p["material_density"].number_value();
            if(!p["material_ambient"].is_null()) material_ambient.from_json(p["material_ambient"].dump());
            if(!p["material_diffuse"].is_null()) material_diffuse.from_json(p["material_diffuse"].dump());
            if(!p["material_specular"].is_null()) material_specular.from_json(p["material_specular"].dump());
        }
        return;
    }
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
    bool enabled = true;
    //! Component Type
    uint16_t type = 0;
    //! Device Model
    uint16_t model = 0;
    //! Device flag - catch all for any small piece of information that might be device specific
    uint32_t flag = 0;
    //! Device specific address
    uint16_t addr = 0;
    //! Component Index
    uint16_t cidx = 0;
    //! Device specific index
    uint16_t didx = 0;
    //! Piece index
    uint16_t pidx = 0;
    //! Power Bus index
    uint16_t bidx = 0;
    //! Connection information for device.
    uint16_t portidx = 0; // TODO: rename to port_id or port_index
    //! Nominal Amperage
    float namp = 0.f; // TODO: rename to nominal current
    //! Nominal Voltage
    float nvolt = 0.f; // TODO: rename to nominal voltage
    //! Current Amperage
    float amp = 0.f; // TODO: rename to current
    //! Current Voltage
    float volt = 0.f; // TODO: rename to voltage
    //! Current Power
    float power = 0.f; // TODO: rename to voltage
    //! Total energy usage
    float energy = 0.f;
    //! Current data rate
    float drate = 0.f;
    //! Current Temperature
    float temp = 0.f; // TODO: rename to temperature
    //! Device information time stamp
    double utc = 0.;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "enabled" , enabled },
            { "type"    , type },
            { "model"   , model },
            { "flag"    , static_cast<int>(flag) },
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

 //JIMNOTE:  this one needs some JSON work... hmm....  what to do about that union....
struct telemstruc
{
    //! Data type
    uint16_t type = 0;
    //! Union of data
    //union
    //{
        uint8_t vuint8 = 0;
        int8_t vint8 = 0;
        uint16_t vuint16 = 0;
        int16_t vint16 = 0;
        uint32_t vuint32 = 0;
        int32_t vint32 = 0;
        float vfloat = 0.f;
        double vdouble = 0.f;
        char vstring[COSMOS_MAX_NAME+1] = "";
    //};

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
struct ploadstruc
{
    //! Number of keys being used.
    uint16_t key_cnt = 0;
    //! Name for each key.
    uint16_t keyidx[MAXPLOADKEYCNT] = {0};
    //! Value for each key.
    float keyval[MAXPLOADKEYCNT] = {0.f};

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
struct ssenstruc
{
    //! Sensor alignment quaternion.
    quaternion align;
    float qva = 0.f;
    float qvb = 0.f;
    float qvc = 0.f;
    float qvd = 0.f;
    float azimuth = 0.f;
    float elevation = 0.f;

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
struct imustruc
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
struct rwstruc
{
    //! Rotates vectors from RW frame (axis of rotation = z) to body frame.
    quaternion align;
    //! Moments of inertia in RW frame.
    rvector mom;
    //! Maximum omega in radians/second.
    float mxomg = 0.f;
    //! Maximum alpha in radians/second/second.
    float mxalp = 0.f;
    //! Acceleration Time Constant
    float tc = 0.f;
    //! Current angular velocity
    float omg = 0.f;
    //! Current angular acceleration
    float alp = 0.f;
    //! Requested angular velocity
    float romg = 0.f;
    //! Requested angular acceleration
    float ralp = 0.f;

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
struct mtrstruc
{
    //! Rotates vectors from MTR frame to Body frame.
    quaternion align;
    //! Terms of 6th order polynomial converting negative moment to current
    float npoly[7] = {0.f};
    //! Terms of 6th order polynomial converting positive moment to current
    float ppoly[7] = {0.f};
    //! Maxiumum field strength
    float mxmom = 0.f;
    //! Field Change Time Constant
    float tc = 0.f;
    //! Requested Magnetic Moment.
    float rmom = 0.f;
    //! Actual Magnetic Moment.
    float mom = 0.f;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        vector<float> v_npoly = vector<float>(npoly, npoly+sizeof(npoly)/sizeof(npoly[0]));
        vector<float> v_ppoly = vector<float>(ppoly, npoly+sizeof(ppoly)/sizeof(ppoly[0]));
        return json11::Json::object {
            { "align" , align },
            { "npoly"   , v_npoly },
            { "ppoly"   , v_ppoly },
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
                    if(!p_npoly[i].is_null()) npoly[i] = p_npoly[i].number_value();
                }
            }
            if(!parsed["ppoly"].is_null()) {
                auto p_ppoly = parsed["ppoly"].array_items();
                for(size_t i = 0; i != p_ppoly.size(); ++i) {
                    if(!p_ppoly[i].is_null()) ppoly[i] = p_ppoly[i].number_value();
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
struct cpustruc
{
    // cpu
    //! Seconds CPU has been up
    uint32_t uptime = 0;
    //! Current load
    float load = 0.f;
    //! Maximum load
    float maxload = 0.f;

    // memory
    //! Maximum memory capacity in GiB
    float maxgib = 0.f;
    //! Current memory usage in GiB
    float gib = 0.f;

    //! Number of reboots
    uint32_t boot_count = 0;

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
            if(!parsed["uptime"].is_null()) uptime = parsed["uptime"].int_value();
            if(!parsed["load"].is_null()) load = parsed["load"].number_value();
            if(!parsed["maxload"].is_null()) maxload = parsed["maxload"].number_value();
            if(!parsed["maxgib"].is_null()) maxgib = parsed["maxgib"].number_value();
            if(!parsed["gib"].is_null()) gib = parsed["gib"].number_value();
            if(!parsed["boot_count"].is_null()) boot_count = parsed["boot_count"].int_value();
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Disk information
struct diskstruc
{
    // disk
    //! Maximum disk capacity in GiB
    float maxgib = 0.f;  // TODO: rename to diskSize, consider bytes?
    //! Current disk usage in GiB
    float gib = 0.f; // TODO: rename to diskUsed, consider bytes?
    //! Path
    char path[COSMOS_MAX_NAME] = "";

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
struct gpsstruc
{
    //! UTC time error
    double dutc = 0.;
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
    float heading = 0.f;
    //! number of satellites used by GPS receiver
    uint16_t sats_used = 0;
    //! number of satellites visible by GPS receiver
    uint16_t sats_visible = 0;
    //! Time Status
    uint16_t time_status = 0;
    //! Position Type
    uint16_t position_type = 0;
    //! Solution Status
    uint16_t solution_status = 0;

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
struct antstruc
{
    //! Alignment
    quaternion align;
    //! Azimuth;
    float azim = 0.f;
    //! Elevation
    float elev = 0.f;
    //! Minimum elevation
    float minelev = 0.f;
    //! Maximum elevation
    float maxelev = 0.f;
    //! Minimum azimuth
    float minazim = 0.f;
    //! Maximum azimuth
    float maxazim = 0.f;
    //! Elevation threshold
    float threshelev = 0.f;

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
struct rxrstruc
{
    //! Operating mode
    uint16_t opmode = 0;
    //! Data modulation
    uint16_t modulation = 0;
    //! RSSI
    uint16_t rssi = 0;
    //! Packet Size
    uint16_t pktsize = 0;
    //! Input Frequency
    double freq = 0.;
    //! Maximum frequency allowed
    double maxfreq = 0.;
    //! Minimum frequency allowed
    double minfreq = 0.;
    //! Current RX Power
    float powerin = 0.f;
    //! Current TX Power
    float powerout = 0.f;
    //! Output Power limit
    float maxpower = 0.f;
    //! Input Filter bandpass
    float band = 0.f;
    //! repeater squelch tone frequency
    float squelch_tone = 0.f;
    //! Good Packet Percentage
    double goodratio = 0.;
    //! Last RX time
    double rxutc = 0.;
    //! Connection Uptime
    double uptime = 0.;

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
struct txrstruc
{
    //! Operating mode
    uint16_t opmode = 0;
    //! Data modulation
    uint16_t modulation = 0;
    //! RSSI
    uint16_t rssi = 0;
    //! Packet Size
    uint16_t pktsize = 0;
    //! Input Frequency
    double freq = 0.;
    //! Maximum frequency allowed
    double maxfreq = 0.;
    //! Minimum frequency allowed
    double minfreq = 0.;
    //! Current RX Power
    float powerin = 0.f;
    //! Current TX Power
    float powerout = 0.f;
    //! Output Power limit
    float maxpower = 0.f;
    //! Input Filter bandpass
    float band = 0.f;
    //! repeater squelch tone frequency
    float squelch_tone = 0.f;
    //! Good Packet Percentage
    double  goodratio = 0.;
    //! Last TX time
    double txutc = 0.;
    //! Connection Uptime
    double uptime = 10.;

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
struct tcvstruc
{
    //! Operating mode
    uint16_t opmode = 0;
    //! Data modulation
    uint16_t modulation = 0;
    //! RSSI
    uint16_t rssi = 0;
    //! Packet Size
    uint16_t pktsize = 0;
    //! Input Frequency
    double freq = 0.;
    //! Maximum frequency allowed
    double maxfreq = 0.;
    //! Minimum frequency allowed
    double minfreq = 0.;
    //! Current RX Power
    float powerin = 0.f;
    //! Current TX Power
    float powerout = 0.f;
    //! Output Power limit
    float maxpower = 0.f;
    //! Input Filter bandpass
    float band = 0.f;
    //! repeater squelch tone frequency
    float squelch_tone = 0.f;
    //! Good Packet Percentage
    double  goodratio = 0.;
    //! Last TX time
    double txutc = 0.;
    //! Last RX time
    double rxutc = 0.;
    //! Connection Uptime
    double uptime = 0.;

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
struct pvstrgstruc
{
    //! BCREG index
    uint16_t bcidx = 0;
    //! Efficiency 0th order term
    float effbase = 0.f;
    //! Efficiency 1st order term
    float effslope = 0.f;
    //! Maximum power generation in Watts
    float maxpower = 0.f;
    //! Current power being generated in Watts.
    float power = 0.f;

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
struct battstruc
{
    //! Capacity in amp hours
    float capacity = 0.f;
    //! Charge conversion efficiency
    float efficiency = 0.f;
    //! Charge in amp hours
    float charge = 0.f;
    //! Resistance in
    float r_in = 0.f;
    //! Resistance out
    float r_out = 0.f;
    //! Battery Percentage Remaining
    float percentage = 0.f;
    //! Time Remaining
    float time_remaining = 0.f;

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
struct htrstruc
{
    bool state = true;
    //! Temperature set vertex
    float setvertex = 0.f;

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

struct motrstruc
{
    //! Maximum speed in revolutions per second
    float max = 0.f;
    //!
    float rat = 0.f;
    float spd = 0.f;

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

struct tsenstruc  {
        // Convert class contents to JSON object
    json11::Json to_json() const { return json11::Json::object {}; }

    // Set class contents from JSON string
    void from_json(const string& s) {};
};

//! Thruster (THST) dynamic structure
struct thststruc
{
    //! Flow
    quaternion align;
    //! Specific Impulse in dynes per kg per second
    float flw = 0.f;
    //! Rotation of thrust vector (+z) in to node frame.
    float isp = 0.f;

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
struct propstruc
{
    //! Propellant capacity in kg
    float cap = 0.f;
    //! Propellant level in kg
    float lev = 0.f;

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
struct swchstruc  {
    // Convert class contents to JSON object
    json11::Json to_json() const { return json11::Json::object {}; }

    // Set class contents from JSON string
    void from_json(const string& s) {};
};

//! Rotor Structure definition
/*! This structure holds the description of a rotors.
*/
struct rotstruc
{
    //! Angular position
    float angle = 0.f;

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
struct sttstruc
{
    //! alignment quaternion
    quaternion align;
    //! includes 0 and 1st order derivative
    quaternion att; // TODO: rename to q
    rvector omega;
    rvector alpha;
    //! return code for
    uint16_t retcode = 0;
    uint32_t status = 0;

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
struct mccstruc
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
struct tcustruc
{
    //! Torque Rod count
    uint16_t mcnt = 0;
    //! Torque Rod Component indices
    uint16_t mcidx[3] = {0};

    // Convert class contents to JSON object
    json11::Json to_json() const {
        vector<uint16_t> v_mcidx = vector<uint16_t>(mcidx, mcidx+sizeof(mcidx)/sizeof(mcidx[0]));
        return json11::Json::object {
            { "mcnt" , mcnt },
            { "mcidx", v_mcidx }
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
                    if(!p_mcidx[i].is_null()) mcidx[i] = p_mcidx[i].int_value();
                }
            }
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

struct busstruc
{
    //! Watch Dog Timer (MJD)
    float wdt = 0.f;

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

struct psenstruc
{
    //! Current Pressure
    float press = 0.f;

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
struct suchistruc
{
    //! alignment quaternion
    quaternion align;
    //! Internal pressure
    float press = 0.f;
    //! Internal temperatures
    float temps[8] = {0.f};

    // Convert class contents to JSON object
    json11::Json to_json() const {
        vector<float> v_temps = vector<float>(temps, temps+sizeof(temps)/sizeof(temps[0]));
        return json11::Json::object {
            { "align" , align },
            { "press" , press },
            { "temps" , v_temps }
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
                    if(!p_temps[i].is_null()) temps[i] = p_temps[i].number_value();
                }
            }
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

struct camstruc
{
    uint16_t pwidth = 0;
    uint16_t pheight = 0;
    float width = 0.f;
    float height = 0.f;
    float flength = 0.f;

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
struct tncstruc  {
    // Convert class contents to JSON object
    json11::Json to_json() const { return json11::Json::object {}; }

    // Set class contents from JSON string
    void from_json(const string& s) {};
};

//! BCREG (PV Regulator) Structure definition
/*! This structure holds the description of a BCREG.
*/
struct bcregstruc  {
    // Convert class contents to JSON object
    json11::Json to_json() const { return json11::Json::object {}; }

    // Set class contents from JSON string
    void from_json(const string& s) {};
};

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
    uint16_t pidx = 0;
    uint16_t tidx[3] = {0};
    //! Energy content in Joules
    float heat = 0.f;
    //! Heat Capacity in Joules / (Kg Kelvin)
    float hcap = 900.f;
    //! Emissivity: 0-1
    float emi = .9f;
    //! Absorptivity: 0-1
    float abs = .9f;
    //! mass in Kg
    float mass = 1.f;
    //! Temperature in Kelvin
    float temp = 0.f;
    //! Area
    float area = 0.f;
    //! perimeter
    float perimeter = 0.f;
    //! Insolation in Watts/sq m
    float irradiation = 0.f;
    //! Solar cell coverage
    float pcell = 0.f;
    //! Solar cell base efficiency
    float ecellbase = .25f;
    //! Solar cell efficiency with temp
    float ecellslope = 0.f;
    vector<vector<uint16_t>> triangleindex;

    // Convert class contents to JSON object
    json11::Json to_json() const {
        vector<uint16_t> v_tidx = vector<uint16_t>(tidx, tidx+sizeof(tidx)/sizeof(tidx[0]));
        return json11::Json::object {
            { "external" , external },
            { "com" , com },
            { "normal" , normal },
            { "shove" , shove },
            { "twist" , twist },
            { "pidx"  , pidx },
            { "tidx"  , v_tidx },
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
                        { "triangleindex" , triangleindex }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json parsed = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!parsed["external"].is_null())	external = parsed["external"].bool_value();
            if(!parsed["com"].is_null())	com.from_json(parsed["com"].dump());
            if(!parsed["normal"].is_null())	normal.from_json(parsed["normal"].dump());
            if(!parsed["shove"].is_null())	shove.from_json(parsed["shove"].dump());
            if(!parsed["twist"].is_null())	twist.from_json(parsed["twist"].dump());
            if(!parsed["pidx"].is_null())	pidx = parsed["pidx"].int_value();

                        // array

            if(!parsed["heat"].is_null())	heat = parsed["heat"].number_value();
            if(!parsed["hcap"].is_null())	hcap = parsed["hcap"].number_value();
            if(!parsed["emi"].is_null())	emi = parsed["emi"].number_value();
            if(!parsed["abs"].is_null())	abs = parsed["abs"].number_value();
            if(!parsed["mass"].is_null())	mass = parsed["mass"].number_value();
            if(!parsed["temp"].is_null())	temp = parsed["temp"].number_value();
            if(!parsed["area"].is_null())	area = parsed["area"].number_value();
            if(!parsed["perimeter"].is_null())	perimeter = parsed["perimeter"].number_value();
            if(!parsed["irradiation"].is_null())	irradiation = parsed["irradiation"].number_value();
            if(!parsed["pcell"].is_null())	pcell = parsed["pcell"].number_value();
            if(!parsed["ecellbase"].is_null())	ecellbase = parsed["ecellbase"].number_value();
            if(!parsed["ecellslope"].is_null())	ecellslope = parsed["ecellslope"].number_value();
            for(size_t i = 0; i < triangleindex.size(); ++i) {
                for(size_t j = 0; j < triangleindex[i].size(); ++j)  {
                    if(!parsed["triangleindex"][i][j].is_null()) triangleindex[i][j] = parsed["triangleindex"][i][j].number_value();
                }
            }
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
    double mjdaccel = 0.;
    //! Offset factor for simulated time (simtime = mjdaccel * realtime + mjddiff)
    double mjddiff = 0.;

    float hcap = 900.f;
    float mass = 1.f;
    float temp = 300.f;
    float heat = 300. * 900. * 1.;
    float area = .001f;
    float battcap = 36000.f;
    float battlev = 0.f;
    float powgen = 0.f;
    float powuse = 0.f;

    //! Simulation mode as listed in \def defs_physics
    int32_t mode = 0;
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
            { "mode" , mode },

            { "ftorque" , ftorque },
            { "atorque" , atorque },
            { "rtorque" , rtorque },
            { "gtorque" , gtorque },
            { "htorque" , htorque },
            { "hmomentum" , hmomentum },
            { "ctorque" , ctorque },
            { "fdrag" , fdrag },
            { "adrag" , adrag },
            { "rdrag" , rdrag },
            { "thrust" , thrust },
            { "moi" , moi },
            { "com" , com },
            { "vertices" , vertices },
            { "triangles" , triangles }
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

            if(!parsed["ftorque"].is_null())	ftorque.from_json(parsed["ftorque"].dump());
            if(!parsed["atorque"].is_null())	atorque.from_json(parsed["atorque"].dump());
            if(!parsed["rtorque"].is_null())	rtorque.from_json(parsed["rtorque"].dump());
            if(!parsed["gtorque"].is_null())	gtorque.from_json(parsed["gtorque"].dump());
            if(!parsed["htorque"].is_null())	htorque.from_json(parsed["htorque"].dump());
            if(!parsed["hmomentum"].is_null())	hmomentum.from_json(parsed["hmomentum"].dump());
            if(!parsed["ctorque"].is_null())	ctorque.from_json(parsed["ctorque"].dump());
            if(!parsed["fdrag"].is_null())	fdrag.from_json(parsed["fdrag"].dump());
            if(!parsed["adrag"].is_null())	adrag.from_json(parsed["adrag"].dump());
            if(!parsed["rdrag"].is_null())	rdrag.from_json(parsed["rdrag"].dump());
            if(!parsed["thrust"].is_null())	thrust.from_json(parsed["thrust"].dump());
            if(!parsed["moi"].is_null())	moi.from_json(parsed["moi"].dump());
            if(!parsed["com"].is_null())	com.from_json(parsed["com"].dump());
                        for(size_t i = 0; i < vertices.size(); ++i)	{
                                if(!parsed["vertices"][i].is_null())	vertices[i].from_json(parsed["vertices"][i].dump());
                        }
                        for(size_t i = 0; i < triangles.size(); ++i)	{
                                if(!parsed["triangles"][i].is_null())	triangles[i].from_json(parsed["triangles"][i].dump());
                        }
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
    char name[COSMOS_MAX_NAME+1] = "";
    //! Last event
    char lastevent[COSMOS_MAX_NAME+1] = "";
    //! Last event UTC
    double lasteventutc = 0.;
    //! Node Type as listed in \ref NODE_TYPE.
    uint16_t type = 0;
    //! Operational state
    uint16_t state = 0;

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
    uint16_t tle_cnt = 0;

    uint16_t flags = 0;
    int16_t powmode = 0;
    //! Seconds Node will be down
    uint32_t downtime = 0;
    //! Alt/Az/Range info
    float azfrom = 0.f;
    float elfrom = 0.f;
    float azto = 0.f;
    float elto = 0.f;
    float range = 0.f;
    //! MJD Offset between system UT and simulated UT
    double utcoffset = 0.;
    //! Overall Node time
    double utc = 0.;
    //! Mission start time
    double utcstart = 0.;
    //! Location structure
    locstruc loc;
    physicsstruc phys;

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // char name[COSMOS_MAX_NAME+1]
        add_name("name", &name);
        for(size_t i = 0; i < sizeof(name)/sizeof(name[0]); ++i) {
            string basename = "name[" + std::to_string(i) + "]";
            add_name(basename, &name[i]);
        }

        // char lastevent[COSMOS_MAX_NAME+1]
        add_name("lastevent", &lastevent);
        for(size_t i = 0; i < sizeof(lastevent)/sizeof(lastevent[0]); ++i) {
            string basename = "lastevent[" + std::to_string(i) + "]";
            add_name(basename, &lastevent[i]);
        }

        // double lasteventutc
        add_name("type", &type);

        // uint16_t type
        add_name("type", &type);

        // uint16_t state
        add_name("state", &state);

        // uint16_t vertex_cnt
        add_name("vertex_cnt", &vertex_cnt);
        
        // uint16_t normal_cnt
        add_name("normal_cnt", &normal_cnt);
        
        // uint16_t face_cnt
        add_name("face_cnt", &face_cnt);

        // uint16_t piece_cnt
        add_name("piece_cnt", &piece_cnt);
        
        // uint16_t device_cnt
        add_name("device_cnt", &device_cnt);
        
        // uint16_t port_cnt
        add_name("port_cnt", &port_cnt);
        
        // uint16_t agent_cnt
        add_name("agent_cnt", &agent_cnt);
        
        // uint16_t event_cnt
        add_name("event_cnt", &event_cnt);
        
        // uint16_t target_cnt
        add_name("target_cnt", &target_cnt);
        
        // uint16_t user_cnt
        add_name("user_cnt", &user_cnt);
        
        // uint16_t tle_cnt
        add_name("tle_cnt", &tle_cnt);
        

        // uint16_t flags
        add_name("flags", &flags);
        
        // int16_t powmode
        add_name("powmode", &powmode);
        
        // uint32_t downtime
        add_name("downtime", &downtime);
        
        // float azfrom
        add_name("azfrom", &azfrom);
        
        // float elfrom
        add_name("elfrom", &elfrom);
        
        // float azto
        add_name("azto", &azto);
        
        // float elto
        add_name("elto", &elto);
        
        // float range
        add_name("range", &range);
        
        // double utcoffset
        add_name("utcoffset", &utcoffset);
        
        // double utc
        add_name("utc", &utc);
        
        // double utcstart
        add_name("utcstart", &utcstart);
        
        // locstruc loc
        add_name("loc", &loc);

        // physicsstruc phys
        add_name("phys", &phys);
    }

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
struct devicestruc : public allstruc
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
    
    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // allstruc all
        add_name("all", &all);

        // antstruc ant
        add_name("ant", &ant);
        
        // battstruc batt
        add_name("batt", &batt);
        
        // bcregstruc bcreg
        add_name("bcreg", &bcreg);
        
        // busstruc bus
        add_name("bus", &bus);
        
        // camstruc cam
        add_name("cam", &cam);
        
        // cpustruc cpu
        add_name("cpu", &cpu);
        
        // diskstruc disk
        add_name("disk", &disk);
        
        // gpsstruc gps
        add_name("gps", &gps);
        
        // htrstruc htr
        add_name("htr", &htr);
        
        // imustruc imu
        add_name("imu", &imu);
        
        // mccstruc mcc
        add_name("mcc", &mcc);
        
        // motrstruc motr
        add_name("motr", &motr);
        
        // mtrstruc mtr
        add_name("mtr", &mtr);
        
        // ploadstruc pload
        add_name("pload", &pload);
        
        // propstruc prop
        add_name("prop", &prop);
        
        // psenstruc psen
        add_name("psen", &psen);
        
        // pvstrgstruc pvstrg
        add_name("pvstrg", &pvstrg);
        
        // rotstruc rot
        add_name("rot", &rot);
        
        // rwstruc rw
        add_name("rw", &rw);
        
        // rxrstruc rxr
        add_name("rxr", &rxr);
        
        // ssenstruc ssen
        add_name("ssen", &ssen);
        
        // sttstruc stt
        add_name("stt", &stt);
        
        // suchistruc suchi
        add_name("suchi", &suchi);
        
        // swchstruc swch
        add_name("swch", &swch);
        
        // tcustruc tcu
        add_name("tcu", &tcu);
        
        // tcvstruc tcv
        add_name("tcv", &tcv);
        
        // telemstruc telem
        add_name("telem", &telem);
        
        // thststruc thst
        add_name("thst", &thst);
        
        // tncstruc tnc
        add_name("tnc", &tnc);
        
        // tsenstruc tsen
        add_name("tsen", &tsen);
        
        // txrstruc txr
        add_name("txr", &txr);
        
    }

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "enabled" , enabled },
            { "type"    , type },
            { "model"   , model },
            { "flag"    , static_cast<int>(flag) },
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

            { "all" , all },
            { "ant" , ant },
            { "batt" , batt },
            { "bcreg" , bcreg },
            { "bus" , bus },
            { "cam" , cam },
            { "cpu" , cpu },
            { "disk" , disk },
            { "gps" , gps },
            { "htr" , htr },
            { "imu" , imu },
            { "mcc" , mcc },
            { "motr" , motr },
            { "mtr" , mtr },
            { "pload" , pload },
            { "prop" , prop },
            { "psen" , psen },
            { "pvstrg" , pvstrg },
            { "rot" , rot },
            { "rw" , rw },
            { "rxr" , rxr },
            { "ssen" , ssen },
            { "stt" , stt },
            { "suchi" , suchi },
            { "swch" , swch },
            { "tcu" , tcu },
            { "tcv" , tcv },
            { "telem" , telem },
            { "thst" , thst },
            { "tnc" , tnc },
            { "tsen" , tsen },
            { "txr" , txr }
                };
        }

    // Set class contents from JSON string
    void from_json(const string& js) {
        string error;
        json11::Json p = json11::Json::parse(js,error);
        if(error.empty()) {
           if(!p["enabled"].is_null()) enabled = p["enabled"].bool_value();
            if(!p["type"].is_null()) type = p["type"].int_value();
            if(!p["model"].is_null()) model = p["model"].int_value();
            if(!p["flag"].is_null()) flag = p["flag"].int_value();
            if(!p["addr"].is_null()) addr = p["addr"].int_value();
            if(!p["cidx"].is_null()) cidx = p["cidx"].int_value();
            if(!p["didx"].is_null()) didx = p["didx"].int_value();
            if(!p["pidx"].is_null()) pidx = p["pidx"].int_value();
            if(!p["bidx"].is_null()) bidx = p["bidx"].int_value();
            if(!p["portidx"].is_null()) portidx = p["portidx"].int_value();
            if(!p["namp"].is_null()) namp = p["namp"].number_value();
            if(!p["nvolt"].is_null()) nvolt = p["nvolt"].number_value();
            if(!p["amp"].is_null()) amp = p["amp"].number_value();
            if(!p["volt"].is_null()) volt = p["volt"].number_value();
            if(!p["power"].is_null()) power = p["power"].number_value();
            if(!p["energy"].is_null()) energy = p["energy"].number_value();
            if(!p["drate"].is_null()) drate = p["drate"].number_value();
            if(!p["temp"].is_null()) temp = p["temp"].number_value();
            if(!p["utc"].is_null()) utc = p["utc"].number_value();

            if(!p["all"].is_null())		all.from_json(p["all"].dump());
            if(!p["ant"].is_null())		ant.from_json(p["ant"].dump());
            if(!p["batt"].is_null())	batt.from_json(p["batt"].dump());
            if(!p["bcreg"].is_null())	bcreg.from_json(p["bcreg"].dump());
            if(!p["bus"].is_null())		bus.from_json(p["bus"].dump());
            if(!p["cam"].is_null())		cam.from_json(p["cam"].dump());
            if(!p["cpu"].is_null())		cpu.from_json(p["cpu"].dump());
            if(!p["disk"].is_null())	disk.from_json(p["disk"].dump());
            if(!p["gps"].is_null())		gps.from_json(p["gps"].dump());
            if(!p["htr"].is_null())		htr.from_json(p["htr"].dump());
            if(!p["imu"].is_null())		imu.from_json(p["imu"].dump());
            if(!p["mcc"].is_null())		mcc.from_json(p["mcc"].dump());
            if(!p["motr"].is_null())	motr.from_json(p["motr"].dump());
            if(!p["mtr"].is_null())		mtr.from_json(p["mtr"].dump());
            if(!p["pload"].is_null())	pload.from_json(p["pload"].dump());
            if(!p["prop"].is_null())	prop.from_json(p["prop"].dump());
            if(!p["psen"].is_null())	psen.from_json(p["psen"].dump());
            if(!p["pvstrg"].is_null())	pvstrg.from_json(p["pvstrg"].dump());
            if(!p["rot"].is_null())		rot.from_json(p["rot"].dump());
            if(!p["rw"].is_null())		rw.from_json(p["rw"].dump());
            if(!p["rxr"].is_null())		rxr.from_json(p["rxr"].dump());
            if(!p["ssen"].is_null())	ssen.from_json(p["ssen"].dump());
            if(!p["stt"].is_null())		stt.from_json(p["stt"].dump());
            if(!p["suchi"].is_null())	suchi.from_json(p["suchi"].dump());
            if(!p["swch"].is_null())	swch.from_json(p["swch"].dump());
            if(!p["tcu"].is_null())		tcu.from_json(p["tcu"].dump());
            if(!p["tcv"].is_null())		tcv.from_json(p["tcv"].dump());
            if(!p["telem"].is_null())	telem.from_json(p["telem"].dump());
            if(!p["thst"].is_null())	thst.from_json(p["thst"].dump());
            if(!p["tnc"].is_null())		tnc.from_json(p["tnc"].dump());
            if(!p["tsen"].is_null())	tsen.from_json(p["tsen"].dump());
            if(!p["txr"].is_null())		txr.from_json(p["txr"].dump());
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
};

//! Specific Device structure
/*! Counts and arrays of pointers to each type of device, ordered by type.
*/
struct devspecstruc
{
    uint16_t all_cnt = 0;
    uint16_t ant_cnt = 0;
    uint16_t batt_cnt = 0;
    uint16_t bus_cnt = 0;
    uint16_t cam_cnt = 0;
    uint16_t cpu_cnt = 0;
    uint16_t disk_cnt = 0;
    uint16_t gps_cnt = 0;
    uint16_t htr_cnt = 0;
    uint16_t imu_cnt = 0;
    uint16_t mcc_cnt = 0;
    uint16_t motr_cnt = 0;
    uint16_t mtr_cnt = 0;
    uint16_t pload_cnt = 0;
    uint16_t prop_cnt = 0;
    uint16_t psen_cnt = 0;
    uint16_t bcreg_cnt = 0;
    uint16_t rot_cnt = 0;
    uint16_t rw_cnt = 0;
    uint16_t rxr_cnt = 0;
    uint16_t ssen_cnt = 0;
    uint16_t pvstrg_cnt = 0;
    uint16_t stt_cnt = 0;
    uint16_t suchi_cnt = 0;
    uint16_t swch_cnt = 0;
    uint16_t tcu_cnt = 0;
    uint16_t tcv_cnt = 0;
    uint16_t telem_cnt = 0;
    uint16_t thst_cnt = 0;
    uint16_t tsen_cnt = 0;
    uint16_t tnc_cnt = 0;
    uint16_t txr_cnt = 0;
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

    /// Support for Namespace 2.0
    using name_map = map<string,void*>;
    using name_mapping = pair<string,void*>;
    name_map names;

    void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

    void add_default_names() {
        // uint16_t all_cnt
        add_name("all_cnt", &all_cnt);

        // uint16_t ant_cnt
        add_name("ant_cnt", &ant_cnt);
        
        // uint16_t batt_cnt
        add_name("batt_cnt", &batt_cnt);
        
        // uint16_t bus_cnt
        add_name("bus_cnt", &bus_cnt);
        
        // uint16_t cam_cnt
        add_name("cam_cnt", &cam_cnt);
        
        // uint16_t cpu_cnt
        add_name("cpu_cnt", &cpu_cnt);
        
        // uint16_t disk_cnt
        add_name("disk_cnt", &disk_cnt);
        
        // uint16_t gps_cnt
        add_name("gps_cnt", &gps_cnt);
        
        // uint16_t htr_cnt
        add_name("htr_cnt", &htr_cnt);
        
        // uint16_t imu_cnt
        add_name("imu_cnt", &imu_cnt);
        
        // uint16_t mcc_cnt
        add_name("mcc_cnt", &mcc_cnt);
        
        // uint16_t motr_cnt
        add_name("motr_cnt", &motr_cnt);
        
        // uint16_t mtr_cnt
        add_name("mtr_cnt", &mtr_cnt);
        
        // uint16_t pload_cnt
        add_name("pload_cnt", &pload_cnt);
        
        // uint16_t prop_cnt
        add_name("prop_cnt", &prop_cnt);
        
        // uint16_t psen_cnt
        add_name("psen_cnt", &psen_cnt);
        
        // uint16_t bcreg_cnt
        add_name("bcreg_cnt", &bcreg_cnt);
        
        // uint16_t rot_cnt
        add_name("rot_cnt", &rot_cnt);
        
        // uint16_t rw_cnt
        add_name("rw_cnt", &rw_cnt);
        
        // uint16_t rxr_cnt
        add_name("rxr_cnt", &rxr_cnt);
        
        // uint16_t ssen_cnt
        add_name("ssen_cnt", &ssen_cnt);
        
        // uint16_t pvstrg_cnt
        add_name("pvstrg_cnt", &pvstrg_cnt);
        
        // uint16_t stt_cnt
        add_name("stt_cnt", &stt_cnt);
        
        // uint16_t suchi_cnt
        add_name("suchi_cnt", &suchi_cnt);
        
        // uint16_t swch_cnt
        add_name("swch_cnt", &swch_cnt);
        
        // uint16_t tcu_cnt
        add_name("tcu_cnt", &tcu_cnt);
        
        // uint16_t tcv_cnt
        add_name("tcv_cnt", &tcv_cnt);
        
        // uint16_t telem_cnt
        add_name("telem_cnt", &telem_cnt);
        
        // uint16_t thst_cnt
        add_name("thst_cnt", &thst_cnt);
        
        // uint16_t tsen_cnt
        add_name("tsen_cnt", &tsen_cnt);
        
        // uint16_t tnc_cnt
        add_name("tnc_cnt", &tnc_cnt);
        
        // uint16_t txr_cnt
        add_name("txr_cnt", &txr_cnt);
        
        // vector<uint16_t>all
        add_name("all", &all);
        for(size_t i = 0; i < all.capacity(); ++i) {
            string basename = "all[" + std::to_string(i) + "]";
            add_name(basename, &all[i]);
        }

        // vector<uint16_t>ant
        add_name("ant", &ant);
        for(size_t i = 0; i < ant.capacity(); ++i) {
            string basename = "ant[" + std::to_string(i) + "]";
            add_name(basename, &ant[i]);
        }
        // vector<uint16_t>batt
        add_name("batt", &batt);
        for(size_t i = 0; i < batt.capacity(); ++i) {
            string basename = "batt[" + std::to_string(i) + "]";
            add_name(basename, &batt[i]);
        }
        // vector<uint16_t>bcreg
        add_name("bcreg", &bcreg);
        for(size_t i = 0; i < bcreg.capacity(); ++i) {
            string basename = "bcreg[" + std::to_string(i) + "]";
            add_name(basename, &bcreg[i]);
        }
        // vector<uint16_t>bus
        add_name("bus", &bus);
        for(size_t i = 0; i < bus.capacity(); ++i) {
            string basename = "bus[" + std::to_string(i) + "]";
            add_name(basename, &bus[i]);
        }
        // vector<uint16_t>cam
        add_name("cam", &cam);
        for(size_t i = 0; i < cam.capacity(); ++i) {
            string basename = "cam[" + std::to_string(i) + "]";
            add_name(basename, &cam[i]);
        }
        // vector<uint16_t>cpu
        add_name("cpu", &cpu);
        for(size_t i = 0; i < cpu.capacity(); ++i) {
            string basename = "cpu[" + std::to_string(i) + "]";
            add_name(basename, &cpu[i]);
        }
        // vector<uint16_t>disk
        add_name("disk", &disk);
        for(size_t i = 0; i < disk.capacity(); ++i) {
            string basename = "disk[" + std::to_string(i) + "]";
            add_name(basename, &disk[i]);
        }
        // vector<uint16_t>gps
        add_name("gps", &gps);
        for(size_t i = 0; i < gps.capacity(); ++i) {
            string basename = "gps[" + std::to_string(i) + "]";
            add_name(basename, &gps[i]);
        }
        // vector<uint16_t>htr
        add_name("htr", &htr);
        for(size_t i = 0; i < htr.capacity(); ++i) {
            string basename = "htr[" + std::to_string(i) + "]";
            add_name(basename, &htr[i]);
        }
        // vector<uint16_t>imu
        add_name("imu", &imu);
        for(size_t i = 0; i < imu.capacity(); ++i) {
            string basename = "imu[" + std::to_string(i) + "]";
            add_name(basename, &imu[i]);
        }
        // vector<uint16_t>mcc
        add_name("mcc", &mcc);
        for(size_t i = 0; i < mcc.capacity(); ++i) {
            string basename = "mcc[" + std::to_string(i) + "]";
            add_name(basename, &mcc[i]);
        }
        // vector<uint16_t>motr
        add_name("motr", &motr);
        for(size_t i = 0; i < motr.capacity(); ++i) {
            string basename = "motr[" + std::to_string(i) + "]";
            add_name(basename, &motr[i]);
        }
        // vector<uint16_t>mtr
        add_name("mtr", &mtr);
        for(size_t i = 0; i < mtr.capacity(); ++i) {
            string basename = "mtr[" + std::to_string(i) + "]";
            add_name(basename, &mtr[i]);
        }
        // vector<uint16_t>pload
        add_name("pload", &pload);
        for(size_t i = 0; i < pload.capacity(); ++i) {
            string basename = "pload[" + std::to_string(i) + "]";
            add_name(basename, &pload[i]);
        }
        // vector<uint16_t>prop
        add_name("prop", &prop);
        for(size_t i = 0; i < prop.capacity(); ++i) {
            string basename = "prop[" + std::to_string(i) + "]";
            add_name(basename, &prop[i]);
        }
        // vector<uint16_t>psen
        add_name("psen", &psen);
        for(size_t i = 0; i < psen.capacity(); ++i) {
            string basename = "psen[" + std::to_string(i) + "]";
            add_name(basename, &psen[i]);
        }
        // vector<uint16_t>pvstrg
        add_name("pvstrg", &pvstrg);
        for(size_t i = 0; i < pvstrg.capacity(); ++i) {
            string basename = "pvstrg[" + std::to_string(i) + "]";
            add_name(basename, &pvstrg[i]);
        }
        // vector<uint16_t>rot
        add_name("rot", &rot);
        for(size_t i = 0; i < rot.capacity(); ++i) {
            string basename = "rot[" + std::to_string(i) + "]";
            add_name(basename, &rot[i]);
        }
        // vector<uint16_t>rw
        add_name("rw", &rw);
        for(size_t i = 0; i < rw.capacity(); ++i) {
            string basename = "rw[" + std::to_string(i) + "]";
            add_name(basename, &rw[i]);
        }
        // vector<uint16_t>rxr
        add_name("rxr", &rxr);
        for(size_t i = 0; i < rxr.capacity(); ++i) {
            string basename = "rxr[" + std::to_string(i) + "]";
            add_name(basename, &rxr[i]);
        }
        // vector<uint16_t>ssen
        add_name("ssen", &ssen);
        for(size_t i = 0; i < ssen.capacity(); ++i) {
            string basename = "ssen[" + std::to_string(i) + "]";
            add_name(basename, &ssen[i]);
        }
        // vector<uint16_t>stt
        add_name("stt", &stt);
        for(size_t i = 0; i < stt.capacity(); ++i) {
            string basename = "stt[" + std::to_string(i) + "]";
            add_name(basename, &stt[i]);
        }
        // vector<uint16_t>suchi
        add_name("suchi", &suchi);
        for(size_t i = 0; i < suchi.capacity(); ++i) {
            string basename = "suchi[" + std::to_string(i) + "]";
            add_name(basename, &suchi[i]);
        }
        // vector<uint16_t>swch
        add_name("swch", &swch);
        for(size_t i = 0; i < swch.capacity(); ++i) {
            string basename = "swch[" + std::to_string(i) + "]";
            add_name(basename, &swch[i]);
        }
        // vector<uint16_t>tcu
        add_name("tcu", &tcu);
        for(size_t i = 0; i < tcu.capacity(); ++i) {
            string basename = "tcu[" + std::to_string(i) + "]";
            add_name(basename, &tcu[i]);
        }
        // vector<uint16_t>tcv
        add_name("tcv", &tcv);
        for(size_t i = 0; i < tcv.capacity(); ++i) {
            string basename = "tcv[" + std::to_string(i) + "]";
            add_name(basename, &tcv[i]);
        }
        // vector<uint16_t>telem
        add_name("telem", &telem);
        for(size_t i = 0; i < telem.capacity(); ++i) {
            string basename = "telem[" + std::to_string(i) + "]";
            add_name(basename, &telem[i]);
        }
        // vector<uint16_t>thst
        add_name("thst", &thst);
        for(size_t i = 0; i < thst.capacity(); ++i) {
            string basename = "thst[" + std::to_string(i) + "]";
            add_name(basename, &thst[i]);
        }
        // vector<uint16_t>tnc
        add_name("tnc", &tnc);
        for(size_t i = 0; i < tnc.capacity(); ++i) {
            string basename = "tnc[" + std::to_string(i) + "]";
            add_name(basename, &tnc[i]);
        }
        // vector<uint16_t>tsen
        add_name("tsen", &tsen);
        for(size_t i = 0; i < tsen.capacity(); ++i) {
            string basename = "tsen[" + std::to_string(i) + "]";
            add_name(basename, &tsen[i]);
        }
        // vector<uint16_t>txr
        add_name("txr", &txr);
        for(size_t i = 0; i < txr.capacity(); ++i) {
            string basename = "txr[" + std::to_string(i) + "]";
            add_name(basename, &txr[i]);
        }
    }

    // Convert class contents to JSON object
    json11::Json to_json() const {
        return json11::Json::object {
            { "all_cnt", all_cnt },
            { "ant_cnt", ant_cnt },
            { "batt_cnt", batt_cnt },
            { "bus_cnt", bus_cnt },
            { "cam_cnt", cam_cnt },
            { "cpu_cnt", cpu_cnt },
            { "disk_cnt", disk_cnt },
            { "gps_cnt", gps_cnt },
            { "htr_cnt", htr_cnt },
            { "imu_cnt", imu_cnt },
            { "mcc_cnt", mcc_cnt },
            { "motr_cnt", motr_cnt },
            { "mtr_cnt", mtr_cnt },
            { "pload_cnt", pload_cnt },
            { "prop_cnt", prop_cnt },
            { "psen_cnt", psen_cnt },
            { "bcreg_cnt", bcreg_cnt },
            { "rot_cnt", rot_cnt },
            { "rw_cnt", rw_cnt },
            { "rxr_cnt", rxr_cnt },
            { "ssen_cnt", ssen_cnt },
            { "pvstrg_cnt", pvstrg_cnt },
            { "stt_cnt", stt_cnt },
            { "suchi_cnt", suchi_cnt },
            { "swch_cnt", swch_cnt },
            { "tcu_cnt", tcu_cnt },
            { "tcv_cnt", tcv_cnt },
            { "telem_cnt", telem_cnt },
            { "thst_cnt", thst_cnt },
            { "tsen_cnt", tsen_cnt },
            { "tnc_cnt", tnc_cnt },
            { "txr_cnt", txr_cnt },
            { "all", all },
            { "ant", ant },
            { "ant", batt },
            { "bcreg", bcreg },
            { "bus", bus },
            { "cam", cam },
            { "cpu", cpu },
            { "disk", disk },
            { "gps", gps },
            { "htr", htr },
            { "imu", imu },
            { "mcc", mcc },
            { "motr", motr },
            { "mtr", mtr },
            { "pload", pload },
            { "prop", prop },
            { "psen", psen },
            { "pvstrg", pvstrg },
            { "rot", rot },
            { "rw", rw },
            { "rxr", rxr },
            { "ssen", ssen },
            { "stt", stt },
            { "suchi", suchi },
            { "swch", swch },
            { "tcu", tcu },
            { "tcv", tcv },
            { "telem", telem },
            { "thst", thst },
            { "tnc", tnc },
            { "tsen", tsen },
            { "txr", txr }
        };
    }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
            if(!p["all_cnt"].is_null()) all_cnt = p["all_cnt"].int_value();
            if(!p["ant_cnt"].is_null()) ant_cnt = p["ant_cnt"].int_value();
            if(!p["batt_cnt"].is_null()) batt_cnt = p["batt_cnt"].int_value();
            if(!p["bus_cnt"].is_null()) bus_cnt = p["bus_cnt"].int_value();
            if(!p["cam_cnt"].is_null()) cam_cnt = p["cam_cnt"].int_value();

            if(!p["cpu_cnt"].is_null()) cpu_cnt = p["cpu_cnt"].int_value();
            if(!p["disk_cnt"].is_null()) disk_cnt = p["disk_cnt"].int_value();
            if(!p["gps_cnt"].is_null()) gps_cnt = p["gps_cnt"].int_value();
            if(!p["htr_cnt"].is_null()) htr_cnt = p["htr_cnt"].int_value();
            if(!p["imu_cnt"].is_null()) imu_cnt = p["imu_cnt"].int_value();

            if(!p["mcc_cnt"].is_null()) mcc_cnt = p["mcc_cnt"].int_value();
            if(!p["motr_cnt"].is_null()) motr_cnt = p["motr_cnt"].int_value();
            if(!p["mtr_cnt"].is_null()) mtr_cnt = p["mtr_cnt"].int_value();
            if(!p["pload_cnt"].is_null()) pload_cnt = p["pload_cnt"].int_value();
            if(!p["prop_cnt"].is_null()) prop_cnt = p["prop_cnt"].int_value();

            if(!p["psen_cnt"].is_null()) psen_cnt = p["psen_cnt"].int_value();
            if(!p["bcreg_cnt"].is_null()) bcreg_cnt = p["bcreg_cnt"].int_value();
            if(!p["rot_cnt"].is_null()) rot_cnt = p["rot_cnt"].int_value();
            if(!p["rw_cnt"].is_null()) rw_cnt = p["rw_cnt"].int_value();
            if(!p["rxr_cnt"].is_null()) rxr_cnt = p["rxr_cnt"].int_value();

            if(!p["ssen_cnt"].is_null()) ssen_cnt = p["ssen_cnt"].int_value();
            if(!p["pvstrg_cnt"].is_null()) pvstrg_cnt = p["pvstrg_cnt"].int_value();
            if(!p["stt_cnt"].is_null()) stt_cnt = p["stt_cnt"].int_value();
            if(!p["suchi_cnt"].is_null()) suchi_cnt = p["suchi_cnt"].int_value();
            if(!p["swch_cnt"].is_null()) swch_cnt = p["swch_cnt"].int_value();

            if(!p["tcu_cnt"].is_null()) tcu_cnt = p["tcu_cnt"].int_value();
            if(!p["tcv_cnt"].is_null()) tcv_cnt = p["tcv_cnt"].int_value();
            if(!p["telem_cnt"].is_null()) telem_cnt = p["telem_cnt"].int_value();
            if(!p["thst_cnt"].is_null()) thst_cnt = p["thst_cnt"].int_value();
            if(!p["tsen_cnt"].is_null()) tsen_cnt = p["tsen_cnt"].int_value();

            if(!p["tnc_cnt"].is_null()) tnc_cnt = p["tnc_cnt"].int_value();
            if(!p["txr_cnt"].is_null()) txr_cnt = p["txr_cnt"].int_value();

            for(size_t i = 0; i < all.size(); ++i)
                if(!p["all"][i].is_null()) all[i] = p["all"][i].int_value();
            for(size_t i = 0; i < ant.size(); ++i)
                if(!p["ant"][i].is_null()) ant[i] = p["ant"][i].int_value();
            for(size_t i = 0; i < batt.size(); ++i)
                if(!p["batt"][i].is_null()) batt[i] = p["batt"][i].int_value();
            for(size_t i = 0; i < bcreg.size(); ++i)
                if(!p["bcreg"][i].is_null()) bcreg[i] = p["bcreg"][i].int_value();
            for(size_t i = 0; i < bus.size(); ++i)
                if(!p["bus"][i].is_null()) bus[i] = p["bus"][i].int_value();
            
            for(size_t i = 0; i < cam.size(); ++i)
                if(!p["cam"][i].is_null()) cam[i] = p["cam"][i].int_value();
            for(size_t i = 0; i < cpu.size(); ++i)
                if(!p["cpu"][i].is_null()) cpu[i] = p["cpu"][i].int_value();
            for(size_t i = 0; i < disk.size(); ++i)
                if(!p["disk"][i].is_null()) disk[i] = p["disk"][i].int_value();
            for(size_t i = 0; i < gps.size(); ++i)
                if(!p["gps"][i].is_null()) gps[i] = p["gps"][i].int_value();
            for(size_t i = 0; i < htr.size(); ++i)
                if(!p["htr"][i].is_null()) htr[i] = p["htr"][i].int_value();
            
            for(size_t i = 0; i < imu.size(); ++i)
                if(!p["imu"][i].is_null()) imu[i] = p["imu"][i].int_value();
            for(size_t i = 0; i < mcc.size(); ++i)
                if(!p["mcc"][i].is_null()) mcc[i] = p["mcc"][i].int_value();
            for(size_t i = 0; i < motr.size(); ++i)
                if(!p["motr"][i].is_null()) motr[i] = p["motr"][i].int_value();
            for(size_t i = 0; i < mtr.size(); ++i)
                if(!p["mtr"][i].is_null()) mtr[i] = p["mtr"][i].int_value();
            for(size_t i = 0; i < pload.size(); ++i)
                if(!p["pload"][i].is_null()) pload[i] = p["pload"][i].int_value();
            
            for(size_t i = 0; i < prop.size(); ++i)
                if(!p["prop"][i].is_null()) prop[i] = p["prop"][i].int_value();
            for(size_t i = 0; i < psen.size(); ++i)
                if(!p["psen"][i].is_null()) psen[i] = p["psen"][i].int_value();
            for(size_t i = 0; i < pvstrg.size(); ++i)
                if(!p["pvstrg"][i].is_null()) pvstrg[i] = p["pvstrg"][i].int_value();
            for(size_t i = 0; i < rot.size(); ++i)
                if(!p["rot"][i].is_null()) rot[i] = p["rot"][i].int_value();
            for(size_t i = 0; i < rw.size(); ++i)
                if(!p["rw"][i].is_null()) rw[i] = p["rw"][i].int_value();
            
            for(size_t i = 0; i < rxr.size(); ++i)
                if(!p["rxr"][i].is_null()) rxr[i] = p["rxr"][i].int_value();
            for(size_t i = 0; i < ssen.size(); ++i)
                if(!p["ssen"][i].is_null()) ssen[i] = p["ssen"][i].int_value();
            for(size_t i = 0; i < stt.size(); ++i)
                if(!p["stt"][i].is_null()) stt[i] = p["stt"][i].int_value();
            for(size_t i = 0; i < suchi.size(); ++i)
                if(!p["suchi"][i].is_null()) suchi[i] = p["suchi"][i].int_value();
            for(size_t i = 0; i < swch.size(); ++i)
                if(!p["swch"][i].is_null()) swch[i] = p["swch"][i].int_value();
            
            for(size_t i = 0; i < tcu.size(); ++i)
                if(!p["tcu"][i].is_null()) tcu[i] = p["tcu"][i].int_value();
            for(size_t i = 0; i < tcv.size(); ++i)
                if(!p["tcv"][i].is_null()) tcv[i] = p["tcv"][i].int_value();
            for(size_t i = 0; i < telem.size(); ++i)
                if(!p["telem"][i].is_null()) telem[i] = p["telem"][i].int_value();
            for(size_t i = 0; i < thst.size(); ++i)
                if(!p["thst"][i].is_null()) thst[i] = p["thst"][i].int_value();
            for(size_t i = 0; i < tnc.size(); ++i)
                if(!p["tnc"][i].is_null()) tnc[i] = p["tnc"][i].int_value();
            
            for(size_t i = 0; i < tsen.size(); ++i)
                if(!p["tsen"][i].is_null()) tsen[i] = p["tsen"][i].int_value();
            for(size_t i = 0; i < txr.size(); ++i)
                if(!p["txr"][i].is_null()) txr[i] = p["txr"][i].int_value();
            
        } else {
            cerr<<"ERROR: <"<<error<<">"<<endl;
        }
        return;
    }
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
    bool enabled = true;
    //! JSON Data Type
    uint16_t type = 0;
    //! JSON Data Group
    uint16_t group = 0;
    //! Name of entry
    string name = "";
    //! offset to data storage
    ptrdiff_t offset = 0;
    //! pointer to data storage
    uint8_t* ptr = 0;
    //! vector of actual data
    vector <uint8_t> data;
    //! Index to JSON Unit Type
    uint16_t unit_index = 0;
    //! Index to alert condition in Data Dictionary
    uint16_t alert_index = 0;
    //! Index to alarm condition in Data Dictionary
    uint16_t alarm_index = 0;
    //! Index to maximum condition in Data Dictionary
    uint16_t maximum_index = 0;
    //! Index to minimum condition in Data Dictionary
    uint16_t minimum_index = 0;
    //! Index to subsystem
    uint16_t subsystem = 0;
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
    vector<vector<jsonentry> > jmap; // depricate me!

    //! JSON Equation Map matrix.
    vector<vector<jsonequation> > emap; // depricate me?

    //! JSON Unit Map matrix: first level is for unit type, second level is for all variants (starting with primary).
    vector<vector<unitstruc> > unit;

    //! Vector of Equations
    vector<equationstruc> equation;

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

    //! Array of Two Line Elements
    vector<tlestruc> tle;

    //! JSON descriptive information
    jsonnode json;


        /// Support for Namespace 2.0

        using name_map = map<string,void*>;
        using name_mapping = pair<string,void*>;

        name_map names;

        bool name_exists(const string& s)	{ return (names.find(s) == names.end()) ? false : true; }

		size_t size()	{	return names.size();	}

        void print_all_names() const	{
                name_map::const_iterator it = names.begin();
                while(it != names.end())	{ cout<<(it++)->first<<endl; }
        }

        void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };
        //TODO:   add remove_name(..), change_name(..) functions, match_name(), find_aliases(), etc

        void add_default_names()	{

        // double timestamp
                add_name("timestamp", &timestamp);

        // uint16_t jmapped
                add_name("jmapped", &jmapped);

        // vector<vector<unitstruc>> unit
                add_name("unit", &unit);
                for(size_t i = 0; i < unit.size(); ++i)	{
                        string basename = "unit[" + std::to_string(i) + "]";
                        add_name(basename, &unit[i]);
                        for(size_t j = 0; j < unit[i].size(); ++j)	{
                                string rebasename = basename + "[" + std::to_string(j) + "]";
                                add_name(rebasename, &unit[i][j]);
                                add_name(rebasename+".name", &unit[i][j].name);
                                add_name(rebasename+".type", &unit[i][j].type);
                                add_name(rebasename+".p0", &unit[i][j].p0);
                                add_name(rebasename+".p1", &unit[i][j].p1);
                                add_name(rebasename+".p2", &unit[i][j].p2);
                        }
                }

        // vector<equationstruc> equation
                add_name("equation", &equation);
                for(size_t i = 0; i < equation.capacity(); ++i)	{
                        string basename = "equation[" + std::to_string(i) + "]";
                        add_name(basename, &equation[i]);
                        add_name(basename+".name", &equation[i].name);
                        add_name(basename+".value", &equation[i].value);
                }

        // nodestruc node
                add_name("node", &node);
                add_name("node.name", &node.name);
                add_name("node.lastevent", &node.lastevent);
                add_name("node.lasteventutc", &node.lasteventutc);
                add_name("node.type", &node.type);
                add_name("node.state", &node.state);
                add_name("node.vertex_cnt", &node.vertex_cnt);
                add_name("node.normal_cnt", &node.normal_cnt);
                add_name("node.face_cnt", &node.face_cnt);
                add_name("node.piece_cnt", &node.piece_cnt);
                add_name("node.device_cnt", &node.device_cnt);
                add_name("node.port_cnt", &node.port_cnt);
                add_name("node.agent_cnt", &node.agent_cnt);
                add_name("node.event_cnt", &node.event_cnt);
                add_name("node.target_cnt", &node.target_cnt);
                add_name("node.user_cnt", &node.user_cnt);
                add_name("node.tle_cnt", &node.tle_cnt);
                add_name("node.flags", &node.flags);
                add_name("node.powmode", &node.powmode);
                add_name("node.downtime", &node.downtime);
                add_name("node.azfrom", &node.azfrom);
                add_name("node.elfrom", &node.elfrom);
                add_name("node.azto", &node.azto);
                add_name("node.elto", &node.elto);
                add_name("node.range", &node.range);
                add_name("node.utcoffset", &node.utcoffset);
                add_name("node.utc", &node.utc);
                add_name("node.utcstart", &node.utcstart);
                add_name("node.loc", &node.loc);
                add_name("node.phys", &node.phys);

        // vector<portstruc> port
                add_name("port", &port);
                for(size_t i = 0; i < port.capacity(); ++i)	{
                        string basename = "port[" + std::to_string(i) + "]";
                        add_name(basename, &port[i]);
                        add_name(basename+".type", &port[i].type);
                        add_name(basename+".name", &port[i].name);
                }

        // vector<eventstruc> event
                add_name("event", &event);
                for(size_t i = 0; i < event.capacity(); ++i)	{
                        string basename = "event[" + std::to_string(i) + "]";
                        add_name(basename, &event[i]);
                        add_name(basename+".utc", &event[i].utc);
                        add_name(basename+".utcexec", &event[i].utcexec);
                        add_name(basename+".node", &event[i].node);
                        add_name(basename+".name", &event[i].name);
                        add_name(basename+".user", &event[i].user);
                        add_name(basename+".flag", &event[i].flag);
                        add_name(basename+".type", &event[i].type);
                        add_name(basename+".value", &event[i].value);
                        add_name(basename+".dtime", &event[i].dtime);
                        add_name(basename+".ctime", &event[i].ctime);
                        add_name(basename+".denergy", &event[i].denergy);
                        add_name(basename+".cenergy", &event[i].cenergy);
                        add_name(basename+".dmass", &event[i].dmass);
                        add_name(basename+".cmass", &event[i].cmass);
                        add_name(basename+".dbytes", &event[i].dbytes);
                        add_name(basename+".cbytes", &event[i].cbytes);
                        add_name(basename+".handle", &event[i].handle);
                        add_name(basename+".data", &event[i].data);
                        add_name(basename+".condition", &event[i].condition);
                }

        // vector<targetstruc> target
                add_name("target", &target);
                for(size_t i = 0; i < target.capacity(); ++i)	{
                        string basename = "target[" + std::to_string(i) + "]";
                        add_name(basename, &target[i]);
                        add_name(basename+".utc", &target[i].utc);
                        add_name(basename+".name", &target[i].name);
                        add_name(basename+".type", &target[i].type);
                        add_name(basename+".azfrom", &target[i].azfrom);
                        add_name(basename+".elfrom", &target[i].elfrom);
                        add_name(basename+".azto", &target[i].azto);
                        add_name(basename+".elto", &target[i].elto);
                        add_name(basename+".range", &target[i].range);
                        add_name(basename+".close", &target[i].close);
                        add_name(basename+".min", &target[i].min);
                        add_name(basename+".loc", &target[i].loc);
                }

        // vector<userstruc> user
                add_name("user", &user);
                for(size_t i = 0; i < user.capacity(); ++i)	{
                        string basename = "user[" + std::to_string(i) + "]";
                        add_name(basename, &user[i]);
                        add_name(basename+".name", &user[i].name);
                        add_name(basename+".node", &user[i].node);
                        add_name(basename+".tool", &user[i].tool);
                        add_name(basename+".cpu", &user[i].cpu);
                }

        // vector<tlestruc> tle
                add_name("tle", &tle);
                for(size_t i = 0; i < tle.capacity(); ++i)	{
                        string basename = "tle[" + std::to_string(i) + "]";
                        add_name(basename, &tle[i]);
                        add_name(basename+".utc", &tle[i].utc);
                        add_name(basename+".name", &tle[i].name);
                        add_name(basename+".snumber", &tle[i].snumber);
                        add_name(basename+".id", &tle[i].id);
                        add_name(basename+".bstar", &tle[i].bstar);
                        add_name(basename+".i", &tle[i].i);
                        add_name(basename+".raan", &tle[i].raan);
                        add_name(basename+".e", &tle[i].e);
                        add_name(basename+".ap", &tle[i].ap);
                        add_name(basename+".ma", &tle[i].ma);
                        add_name(basename+".mm", &tle[i].mm);
                        add_name(basename+".orbit", &tle[i].orbit);
                }
        }





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

// JIMNOTE: fix bug with splitting lines over array indices [#]

        replace(js, ", ", ",\n");
        replace(js, "[]", "E M P T Y   V E C T O R");
        replace(js, "{}", "E M P T Y   O B J E C T");
        replace(js, "{", "{\n");
        replace(js, "[", "[\n");
        replace(js, "}", "\n}");
        replace(js, "]", "\n]");
        replace(js, "E M P T Y   O B J E C T", "{}");
        replace(js, "E M P T Y   V E C T O R", "[]");

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
            { "faces" , faces },
            { "pieces" , pieces },
            { "obj" , obj },
            { "device" , device },
            { "devspec" , devspec },
            { "port" , port },
            { "agent" , agent },
            { "event" , event },
            { "target" , target },
                        { "user" , user },
                        { "tle" , tle },
                        //{ "json" , json }
                };
        }

    // Set class contents from JSON string
    void from_json(const string& s) {
        string error;
        json11::Json p = json11::Json::parse(s,error);
        if(error.empty()) {
                        string obj(p.object_items().begin()->first); // NOTE: Should we rename this to something else? We already have a wavefront obj member var
                        if(!p[obj]["timestamp"].is_null())	timestamp = p[obj]["timestamp"].number_value();
                        if(!p[obj]["jmapped"].is_null())	jmapped = p[obj]["jmapped"].number_value();
                        for(size_t i = 0; i < unit.size(); ++i)	{
                                for(size_t j = 0; j < unit[i].size(); ++j)	{
                    if(!p[obj]["unit"][i][j].is_null())
                                            unit[i][j].from_json(p[obj]["unit"][i][j].dump());
                                }
                        }
                        for(size_t i = 0; i < equation.size(); ++i)	{
                            if(!p[obj]["equation"][i].is_null())
                                    equation[i].from_json(p[obj]["equation"][i].dump());
                        }
                        if(!p[obj]["node"].is_null())	node.from_json(p[obj]["node"].dump());
                        for(size_t i = 0; i < vertexs.size(); ++i)	{
                                if(!p[obj]["vertexs"][i].is_null())	vertexs[i].from_json(p[obj]["vertexs"][i].dump());
                        }
                        for(size_t i = 0; i < normals.size(); ++i)	{
                                if(!p[obj]["normals"][i].is_null())	normals[i].from_json(p[obj]["normals"][i].dump());
                        }
            for(size_t i = 0; i < faces.size(); ++i)	{
                                if(!p[obj]["faces"][i].is_null())	faces[i].from_json(p[obj]["faces"][i].dump());
                        }
            if(!p["obj"].is_null()) cosmosstruc::obj.from_json(p[obj]["obj"].dump());
            for(size_t i = 0; i < device.size(); ++i)	{
                                if(!p[obj]["device"][i].is_null())	device[i].from_json(p[obj]["device"][i].dump());
                        }
            if(!p[obj]["devspec"].is_null()) devspec.from_json(p[obj]["devspec"].dump());
            for(size_t i = 0; i < port.size(); ++i)	{
                                if(!p[obj]["port"][i].is_null())	port[i].from_json(p[obj]["port"][i].dump());
                        }
            for(size_t i = 0; i < agent.size(); ++i)	{
                                if(!p[obj]["agent"][i].is_null())	agent[i].from_json(p[obj]["agent"][i].dump());
                        }
            for(size_t i = 0; i < event.size(); ++i)	{
                                if(!p[obj]["event"][i].is_null())	event[i].from_json(p[obj]["event"][i].dump());
                        }
            for(size_t i = 0; i < target.size(); ++i)	{
                                if(!p[obj]["target"][i].is_null())	target[i].from_json(p[obj]["target"][i].dump());
                        }
                        for(size_t i = 0; i < user.size(); ++i)	{
                                if(!p[obj]["user"][i].is_null())	user[i].from_json(p[obj]["user"][i].dump());
                        }
                        for(size_t i = 0; i < tle.size(); ++i)	{
                                if(!p[obj]["tle"][i].is_null())	tle[i].from_json(p[obj]["tle"][i].dump());
                        }
                        //if(!p[obj]["json"].is_null())	json.from_json(p[obj]["json"].dump());
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
