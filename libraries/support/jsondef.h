#ifndef _JSONDEF_H
#define _JSONDEF_H 1

/*! \file jsonlib.h
* \brief JSON Support definitions
*/

#include "configCosmos.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <stddef.h>

#include "mathlib.h"
#include "convertdef.h"
#include "nodedef.h"
#include "physicsdef.h"
//#include "agentdef.h"

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
	JSON_GROUP_PORT
	};

//! Constants defining the data types supported in the \ref jsonlib_namespace.
enum
	{
	//! JSON 32 bit unsigned integer type
	JSON_TYPE_UINT32=1,
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
	JSON_TYPE_EQUATION
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
	JSON_OPERATION_COMPLEMENT
	};

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
//! Extra value added to Dynamic offsets to distinguis them from Static offsets
//#define JSON_MAP_DYNAMIC_OFFSET 1000000L

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
	//! List count
	DEVICE_TYPE_COUNT,
	//! Not a Component
	DEVICE_TYPE_NONE=65535
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
#define COMP_PORT_TYPE_RS232 0
#define COMP_PORT_TYPE_RS422 1
#define COMP_PORT_TYPE_ETHERNET 2
#define COMP_PORT_TYPE_USB 3
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
typedef struct
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
} unitstruc;

//! JSON map offset entry
/*! Single entry in a JSON offset map. Ties together a single JSON name and a offset
 * to a single object, along with its data type.
 * - index: Index of this entry in the ::json_map.
 * - data: Offset to appropriate storage for this data type.
*/
typedef struct
{
	//! JSON Data Type
	uint16_t type;
	//! JSON Data Group
	uint16_t group;
	//! Name of entry
	char *name;
	//! offset to data storage
	ptrdiff_t offset;
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
} jsonentry;

//! JSON handle
/*! Structure representing the location of a single JSON Equation or Name in its respective
	hash table.
*/
typedef struct
{
	// Hash of equation or name
	uint32_t hash;
	// Index within that hash entry
	uint32_t index;
} jsonhandle;

//! JSON equation operand
/*! Structure representing a single operand of a JSON equation. Each operand can be one
	of: JSON_OPERAND_NULL, no operand; JSON_OPERAND_EQUATION, another equation;
	JSON_OPERAND_CONSTANT, a constant value; or JSON_OPERAND_NAME, a Namespace name.
*/
typedef struct
{
	//! JSON Operand Type
	uint16_t type;
	//! JSON Operand data: CONSTANT uses value, EQUATION and NAME use ::jsonhandle.
	union
	{
		double value;
		jsonhandle data;
	};
} jsonoperand;

//! JSON equation entry
/*! Single entry in a JSON equation map. Ties together a JSON equation and its
 * preparsed form.
 * - index: Index of this entry in the ::json_map.
 * - data: Offset to appropriate storage for this data type.
*/
typedef struct
{
	//! JSON equation text
	char *text;
	//! Index to JSON Unit Type
	uint16_t unit_index;
	//! JSON equation operation
	uint16_t operation;
	//! JSON equation operands
	jsonoperand operand[2];
} jsonequation;

//! JSON pointer map
/*! The complete JSON offset map consists of an array of ::jsonentry elements, along
 * with their count. It also provides a dynamically sized char string, used by
 * the JSON output functions, and an index of its length.
*/
typedef struct
{
	//! Array of entries
	vector<vector<jsonentry> > entry;
} jsonmap;

//! JSON string storage
/*! A structure to be used for building a JSON string. Contains a pointer to the growing string and a size
 * indicator. As the string fills, it is automatically grown by another 1024 bytes. A record is kept
 * of both its' total and used length.
*/
typedef struct
{
	//! Growing JSON string
	char *string;
	//! Current total length
	uint32_t length;
	//! Current used length
	uint32_t index;
} jstring;

//! Agent Request Function
//! Format of a user supplied function to handle a given request
typedef int32_t (*agent_request_function)(char* request_string, char* output_string, void *root);

//! Agent Request Entry
//! Structure representing a single Agent request.
typedef struct
{
	//! Character token for request
	char token[COSMOS_MAX_NAME];
	//! Pointer to function to call with request string as argument and returning any error
	agent_request_function function;
	string synopsis;
	string description;
} agent_request_entry;

//! Agent Request Structure
//! Structure for storing Agent request handling information
typedef struct
{
	uint32_t count;
	agent_request_entry request[AGENTMAXREQUESTCOUNT];
} agent_request_structure;

//! Channel structure
//! This structure stores the information about an open COSMOS network channel.
typedef struct
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
} agent_channel;

//! Process heartbeat.
//! Detailed elements of a single heartbeat of a single process.
typedef struct
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
} beatstruc;

//! Agent control structure
typedef struct
{
	//! Client initialized?
	bool client;
	//! Subscription channel (for Client)
	agent_channel sub;
	//! Server initialized?
	bool server;
	//! Number of network interfaces
	int ifcnt;
	//! Publication channels for each interface (for Server)
	agent_channel pub[AGENTMAXIF];
	//! Request channel (for Server)
	agent_channel req;
	//! Agent process ID
	int32_t pid;
	//! Activity period in seconds
	double aprd;
	//! Agent Running State Flag
	uint16_t stateflag;
	//! State of Health report string
	//	char sohstring[AGENTMAXBUFFER];
	//! Agent request list
	agent_request_structure reqs;
	//! Heartbeat
	beatstruc beat;
	//! State of Health element vector
	vector<jsonentry*> sohtable;
} agentstruc;

//! Long COSMOS Event structure.
/*! This is the structure that holds each Event, along with associated
 * resources.
 */
typedef struct
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
} longeventstruc;

//! Shortened COSMOS Event structure
/*! This is the structure that holds each Event, along with associated
 * resources, in a shorter format.
 */
typedef struct
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
} shorteventstruc;

//! Full COSMOS Event structure
/*! This is the union that holds each Event, along with associated
 * resources, in both formats.
 */
typedef union
{
	shorteventstruc s;
	longeventstruc l;
} eventstruc;

//! User structure
/*! Containing entries unique to each User
 */
typedef struct
{
	char name[COSMOS_MAX_NAME];
	char node[COSMOS_MAX_NAME];
	char tool[COSMOS_MAX_NAME];
	char cpu[COSMOS_MAX_NAME];
} userstruc;

//! Glossary structure
/*! Contains entries for a glossary of names and matching descriptions
and types.
*/
typedef struct
{
	// Glossary entry name.
	string name;
	// Glossary entry description, to be interpreted based on its type.
	string description;
	// Glossary entry ::namespace type.
	uint16_t type;
} glossarystruc;

//! Target structure
/*! Contains entries unique to each Target
*/
typedef struct
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
} targetstruc;

//! Port structure
/*! Contains information about I/O ports available to devices. The
 * ::portidx entry in each device structure indicates which of these
 * ports a device will use.
 */
typedef struct
{
	//! Type of I/O as listed in ::def_comp_port.
	uint16_t type;
	//! Name information for port.
	char name[COSMOS_MAX_NAME];
} portstruc;

//! JSON Name Space structure
/*! A structure containing an element for every unique name in the COSMOS Name
 * Space. The static and dynamic components of this can then be mapped
 * separately to the name space using ::json_map_cosmosstruc.
*/
typedef struct
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
	//! Pointer to JSON Namespace Map.
	vector<vector<jsonentry> > jmap;
	//! Pointer to JSON Equation Map.
	vector<vector<jsonequation> > emap;
	//! Array of units: first level is for type, second level is for variant.
	vector<vector<unitstruc> > unit;
	//! Array of Two Line Elements
	vector<linestruc> tle;
} cosmosstruc;

typedef uint32_t cosmoshandle;

//! @}

#endif
