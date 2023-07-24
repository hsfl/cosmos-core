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
#define _JSONDEF_H 

/*! \file jsonlib.h
* \brief JSON Support definitions
*/

#include "support/configCosmos.h"

#include "math/mathlib.h"
#include "support/convertdef.h"
#include "physics/physicsdef.h"
#include "support/socketlib.h"
#include "support/objlib.h"

namespace Cosmos
{
    namespace Support
    {

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
        // Namespace 2.0
        static map<string, uint16_t> JsonUnitValue = {
            {"none", 0},
            {"length", 1},
            {"mass", 2},
            {"moi", 3},
            {"time", 4},
            {"data", 5},
            {"current", 6},
            {"temperature", 7},
            {"area", 8},
            {"volume", 9},
            {"speed", 10},
            {"acceleration", 11},
            {"density", 12},
            {"luminance", 13},
            {"angle", 14},
            {"omega", 15},
            {"alpha", 16},
            {"solidangle", 17},
            {"frequency", 18},
            {"force", 19},
            {"torque", 20},
            {"pressure", 21},
            {"energy", 22},
            {"power", 23},
            {"charge", 24},
            {"voltage", 25},
            {"capacitance", 26},
            {"resistance", 27},
            {"magfield", 28},
            {"magflux", 29},
            {"magdensity", 30},
            {"magmoment", 31},
            {"isp", 32},
            {"bytes", 33},
            {"fraction", 34}
        };

        static map<uint16_t, string> JsonUnitName = {
            {0, "none"},
            {1, "length"},
            {2, "mass"},
            {3, "moi"},
            {4, "time"},
            {5, "data"},
            {6, "current"},
            {7, "temperature"},
            {8, "area"},
            {9, "volume"},
            {10, "speed"},
            {11, "acceleration"},
            {12, "density"},
            {13, "luminance"},
            {14, "angle"},
            {15, "omega"},
            {16, "alpha"},
            {17, "solidangle"},
            {18, "frequency"},
            {19, "force"},
            {20, "torque"},
            {21, "pressure"},
            {22, "energy"},
            {23, "power"},
            {24, "charge"},
            {25, "voltage"},
            {26, "capacitance"},
            {27, "resistance"},
            {28, "magfield"},
            {29, "magflux"},
            {30, "magdensity"},
            {31, "magmoment"},
            {32, "isp"},
            {33, "bytes"},
            {34, "fraction"}
        };

        //! JSON Namelist Group
        //Namespace 1.0
        enum {
            //! Absolute pointer
            JSON_STRUCT_ABSOLUTE,
            //! Cosmos::Support::nodestruc
            JSON_STRUCT_NODE,
            //! Cosmos::Support::agentstruc
            JSON_STRUCT_AGENT,
            //! Cosmos::Support::devicestruc
            JSON_STRUCT_DEVICE,
            //! Cosmos::Support::devspecstruc
            JSON_STRUCT_DEVSPEC,
            //! Cosmos::Support::physicsstruc
            JSON_STRUCT_PHYSICS,
            //! Cosmos::Support::eventstruc
            JSON_STRUCT_EVENT,
            //! Cosmos::Support::piecestruc
            JSON_STRUCT_PIECE,
            //! Cosmos::Support::targetstruc
            JSON_STRUCT_TARGET,
            //! Cosmos::Support::userstruc
            JSON_STRUCT_USER,
            //! Cosmos::Support::portstruc
            JSON_STRUCT_PORT,
            //! Cosmos::Support::tlestruc
            JSON_STRUCT_TLE,
            //! Cosmos::Support::aliasstruc
            JSON_STRUCT_ALIAS,
            //! Cosmos::Support::equationstruc
            JSON_STRUCT_EQUATION,
            //! Cosmos::Support::vertexstruc
            JSON_STRUCT_POINT,
            //! Cosmos::Support::facestruc
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
#define AGENTMAXIF 10
        //! Maximum number of builtin AGENT requests
#define AGENTMAXBUILTINCOUNT 6
        //! Maximum number of user defined AGENT requests
#define AGENTMAXUSERCOUNT 200	// 20150629JC: Increased count from 40 to 200 (to support new software)
        //! Maximum number of AGENT requests
#define AGENTMAXREQUESTCOUNT (AGENTMAXBUILTINCOUNT+AGENTMAXUSERCOUNT)

        // Maximums for pre-allocated (and never reallocated) vector storage inside cosmosstruc
        //* Maximum number of vertices - Cube = 14
#define MAX_NUMBER_OF_VERTICES 28

        //* Maximum number of triangles - Cube = 24
#define MAX_NUMBER_OF_TRIANGLES 48

        //* Maximum number of vertices
#define MAX_NUMBER_OF_VERTEXS 5

        //* Maximum number of normals
#define MAX_NUMBER_OF_NORMALS 5

        //* Maximum number of equations
#define MAX_NUMBER_OF_EQUATIONS 10

        //* Maximum number of agents
#define MAX_NUMBER_OF_AGENTS 5

        //* Maximum number of ports
#define MAX_NUMBER_OF_PORTS 10

        //* Maximum number of pieces
#define MAX_NUMBER_OF_PIECES 100

        //* Maximum number of devices
#define MAX_NUMBER_OF_DEVICES MAX_NUMBER_OF_PIECES

        //* Maximum number of parameter sets
#define MAX_NUMBER_OF_PARAMETER_SETS 1

        //* Maximum number of satellites
#define MAX_NUMBER_OF_SATELLITES 5

        //* Maximum number of events
#define MAX_NUMBER_OF_EVENTS 10

        //* Maximum number of targets
#define MAX_NUMBER_OF_TARGETS 10

        //* Maximum number of users
#define MAX_NUMBER_OF_USERS 3

        //* Maximum number of TLES
#define MAX_NUMBER_OF_TLES 5

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
            //! Gyroscope
            GYRO=31,
            //! Magnetometer
            MAG=32,
            //! Earth Sensor
            XYZSEN=33,
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
#define DEVICE_FLAG_ACTIVE	  0x0004 // TODO: define device active
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

        //! STTR Simulation Parameter Class
        /** Yes it is.
*/

class sim_param	{
	public:

	sim_param() : r_ijs(MAX_NUMBER_OF_SATELLITES, 0.0) {};

	// MAC Parameters

	/** half-kernel radius for inter-agent control (interaction radius), m */
    double	h = 1.0;
	/** half-kernel radius for inter-agent control (interaction radius) for attractor, m */
    double	h_attractor = 10.0;
	/** Reynolds number, controls viscosity, dimensionless */
	double	Re = 20.0;
	/** Maximum inter-agent relative velocity, m/s */
	double	v_max = 15.0;
	/** Maximum acceleration, m/s^2 */
    double	a_max = 0.1;
	/** Mach number, dimensionless */
	double	M = 1.0;
	/** exponent on a term in the pressure function */
	double	gamma = 1.0;
	/** reference density, kg/m^3 */
	double	rho_0 = 1.0;
	/** hydrodynamics force weighting, repulsion force */
	double	inter_agent_w = 1.0;
	/** attractor force weighing */
	double	attractor_w = 1.0;
	/** damping coefficient */
	double	damping_coeff = 0.01;
    /** attractor point for MAC (ECI) - both position and velocity */
    double	x_attractor = 0.0;
    double	y_attractor = 0.0;
    double	z_attractor = 0.0;
    double	vx_attractor = 0.0;
    double	vy_attractor = 0.0;
    double	vz_attractor = 0.0;

	// Computed MAC (i.e. dependent) parameters
	/** mass (fictional) */
	double m = 1.0;
	/** viscousity coefficient */
	double mu = 1.0;
	/** square of information speed*/
	double c_squared = 1.0;
	/** speed of sound associated with attraction point */
	double c_a = 1.0;
	/** pressure (fictional)*/
	double P = 1.0;
	/** density */
	double	rho = 0.0;
	/** Pairwise inter-node distances */
	vector<double> r_ijs; //(MAX_NUMBER_OF_SATELLITES, 0.0);

	// HCL Parameters
    /** Diagonal values of covariance matrix of accelerometer */
	double covar_acc_diag0 = 1.;
	double covar_acc_diag1 = 1.;
	double covar_acc_diag2 = 1.;
	/** Diagonal values of covariance matrix of gps pos and vel */
	double covar_gps_diag0 = 1.;
	double covar_gps_diag1 = 1.;
	double covar_gps_diag2 = 1.;
	double covar_gps_diag3 = 1.;
	double covar_gps_diag4 = 1.;
	double covar_gps_diag5 = 1.;
    /** Diagonal values of covariance matrix of relative pos */
	double covar_rel_pos_diag0 = 1.;
	double covar_rel_pos_diag1 = 1.;
	double covar_rel_pos_diag2 = 1.;


	/// Convert class contents to JSON object
	/** Returns a json11 JSON object of the class
		@return A json11 JSON object containing every member variable within the class
	*/
	json11::Json to_json() const {
		return json11::Json::object {
			{ "h"   , h },
			{ "Re"  , Re },
			{ "v_max"   , v_max },
			{ "a_max"   , a_max },
			{ "M"  	, M },
			{ "gamma"   , gamma },
			{ "rho_0" 	, rho_0 },
			//{ "rho" 	, rho },
			{ "inter_agent_w"	, inter_agent_w },
			{ "attractor_w"		, attractor_w },
			{ "damping_coeff"		, damping_coeff },
            { "x_attractor"		, x_attractor },
            { "y_attractor"		, y_attractor },
            { "z_attractor"		, z_attractor },
            { "vx_attractor"		, vx_attractor },
            { "vy_attractor"		, vy_attractor },
            { "vz_attractor"		, vz_attractor },
            { "h_attractor"		, h_attractor },
            { "covar_acc_diag0"	, covar_acc_diag0 },
            { "covar_acc_diag1"	, covar_acc_diag1 },
            { "covar_acc_diag2"	, covar_acc_diag2 },
            { "covar_gps_diag0"	, covar_gps_diag0 },
            { "covar_gps_diag1"	, covar_gps_diag1 },
            { "covar_gps_diag2"	, covar_gps_diag2 },
            { "covar_gps_diag3"	, covar_gps_diag3 },
            { "covar_gps_diag4"	, covar_gps_diag4 },
            { "covar_gps_diag5"	, covar_gps_diag5 },
            { "covar_rel_pos_diag0"	, covar_rel_pos_diag0 },
            { "covar_rel_pos_diag1"	, covar_rel_pos_diag1 },
            { "covar_rel_pos_diag2"	, covar_rel_pos_diag2 },
            //{ "m"	, m },
            //{ "mu"	, mu },
            //{ "c_squared"	, c_squared },
            //{ "c_a"	, c_a },
            //{ "P"	, P },
            //{ "r_ijs"	, r_ijs },
		};
	}

	/// Set class contents from JSON string
	/** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
		@param	s	JSON-formatted string to set class contents to
		@return n/a
	*/
	void from_json(const string& s) {
		string error;
		json11::Json p = json11::Json::parse(s,error);

		if(error.empty()) {
			if(!p["h"].is_null()) { h = p["h"].number_value(); }
			if(!p["Re"].is_null()) { Re = p["Re"].number_value(); }
			if(!p["v_max"].is_null()) { v_max = p["v_max"].number_value(); }
			if(!p["a_max"].is_null()) { a_max = p["a_max"].number_value(); }
			if(!p["M"].is_null()) { M = p["M"].number_value(); }
			if(!p["gamma"].is_null()) { gamma = p["gamma"].number_value(); }
			if(!p["rho_0"].is_null()) { rho_0 = p["rho_0"].number_value(); }
			//if(!p["rho"].is_null()) { rho = p["rho"].number_value(); }
			if(!p["inter_agent_w"].is_null()) { inter_agent_w = p["inter_agent_w"].number_value(); }
			if(!p["attractor_w"].is_null()) { attractor_w = p["attractor_w"].number_value(); }
			if(!p["damping_coeff"].is_null()) { damping_coeff = p["damping_coeff"].number_value(); }
            if(!p["x_attractor"].is_null()) { x_attractor = p["x_attractor"].number_value(); }
            if(!p["y_attractor"].is_null()) { y_attractor = p["y_attractor"].number_value(); }
            if(!p["z_attractor"].is_null()) { z_attractor = p["z_attractor"].number_value(); }
            if(!p["vx_attractor"].is_null()) { vx_attractor = p["vx_attractor"].number_value(); }
            if(!p["vy_attractor"].is_null()) { vy_attractor = p["vy_attractor"].number_value(); }
            if(!p["vz_attractor"].is_null()) { vz_attractor = p["vz_attractor"].number_value(); }
            if(!p["h_attractor"].is_null()) { h_attractor = p["h_attractor"].number_value(); }
            if(!p["m"].is_null()) { m = p["m"].number_value(); }
            if(!p["mu"].is_null()) { mu = p["mu"].number_value(); }
            if(!p["c_squared"].is_null()) { c_squared = p["c_squared"].number_value(); }
            if(!p["c_a"].is_null()) { c_a = p["c_a"].number_value(); }
            if(!p["P"].is_null()) { P = p["P"].number_value(); }
            if(!p["covar_acc_diag0"].is_null()) { covar_acc_diag0 = p["covar_acc_diag0"].number_value(); }
            if(!p["covar_acc_diag1"].is_null()) { covar_acc_diag1 = p["covar_acc_diag1"].number_value(); }
            if(!p["covar_acc_diag2"].is_null()) { covar_acc_diag2 = p["covar_acc_diag2"].number_value(); }
            if(!p["covar_gps_diag0"].is_null()) { covar_gps_diag0 = p["covar_gps_diag0"].number_value(); }
            if(!p["covar_gps_diag1"].is_null()) { covar_gps_diag1 = p["covar_gps_diag1"].number_value(); }
            if(!p["covar_gps_diag2"].is_null()) { covar_gps_diag2 = p["covar_gps_diag2"].number_value(); }
            if(!p["covar_gps_diag3"].is_null()) { covar_gps_diag3 = p["covar_gps_diag3"].number_value(); }
            if(!p["covar_gps_diag4"].is_null()) { covar_gps_diag4 = p["covar_gps_diag4"].number_value(); }
            if(!p["covar_gps_diag5"].is_null()) { covar_gps_diag5 = p["covar_gps_diag5"].number_value(); }
            if(!p["covar_rel_pos_diag0"].is_null()) { covar_rel_pos_diag0 = p["covar_rel_pos_diag0"].number_value(); }
            if(!p["covar_rel_pos_diag1"].is_null()) { covar_rel_pos_diag1 = p["covar_rel_pos_diag1"].number_value(); }
            if(!p["covar_rel_pos_diag2"].is_null()) { covar_rel_pos_diag2 = p["covar_rel_pos_diag2"].number_value(); }
			for(size_t i = 0; i < r_ijs.size(); ++i) {
                if(!p["r_ijs"][i].is_null()) { r_ijs[i] = p["r_ijs"][i].number_value(); }
            }
		} else {
			cerr<<"ERROR: <"<<error<<">"<<endl;
		}
		return;
	}
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
        class unitstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(unitstruc);
                total += name.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
            }

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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "name", name },
                    { "type", type },
                    { "p0", p0 },
                    { "p1", p1 },
                    { "p2", p2 }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s)	{
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty())	{
                    if(!parsed["name"].is_null()) { name = parsed["name"].string_value(); }
                    if(!parsed["type"].is_null()) { type = parsed["type"].int_value(); }
                    if(!parsed["p0"].is_null()) { p0 = parsed["p0"].number_value(); }
                    if(!parsed["p1"].is_null()) { p1 = parsed["p1"].number_value(); }
                    if(!parsed["p2"].is_null()) { p2 = parsed["p2"].number_value(); }
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
            string vertexs = "";
            string faces = "";
            string pieces = "";
            string devgen = "";
            string devspec = "";
            string ports = "";
            string targets = "";
            string tles = "";
            string aliases = "";

            // Convert class contents to JSON object
            json11::Json to_json() const {
                return json11::Json::object {
                    { "name" , name },
                    { "node" , node },
                    { "state" , state },
                    { "vertexs" , vertexs },
                    { "faces" , faces },
                    { "pieces" , pieces },
                    { "devgen" , devgen },
                    { "devspec" , devspec },
                    { "ports" , ports },
                    { "targets" , targets },
                    { "tles" , tles },
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
                    if(!parsed["vertexs"].is_null())	vertexs = parsed["vertexs"].string_value();
                    if(!parsed["faces"].is_null())		faces = parsed["faces"].string_value();
                    if(!parsed["pieces"].is_null())		pieces = parsed["pieces"].string_value();
                    if(!parsed["devgen"].is_null())		devgen = parsed["devgen"].string_value();
                    if(!parsed["devspec"].is_null())	devspec = parsed["devspec"].string_value();
                    if(!parsed["ports"].is_null())		ports = parsed["ports"].string_value();
                    if(!parsed["targets"].is_null())	targets = parsed["targets"].string_value();
                    if(!parsed["tles"].is_null())	tles = parsed["tles"].string_value();
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
        //Namespace 1.0
        struct jsonhandle
        {
            // Hash of equation or name
            uint16_t hash;  // NOTE: Initializing this
            // Index within that hash entry
            uint16_t index;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "hash" , hash },
                    { "index", index }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["hash"].is_null()) { hash = p["hash"].int_value(); }
                    if(!p["index"].is_null()) { index = p["index"].int_value(); }
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
        class jsonequation
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(jsonequation);
                total += text.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(text).swap(text);
            }

            //! JSON equation text
            //            char *text;
            string text;
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
        class agent_request_entry
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(agent_request_entry);
                total += token.capacity();
                total += synopsis.capacity();
                total += description.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(token).swap(token);
                string(synopsis).swap(synopsis);
                string(description).swap(description);
            }

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
        class beatstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(beatstruc);
                total += node.capacity();
                total += proc.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(node).swap(node);
                string(proc).swap(proc);
            }

            /** Heartbeat timestamp */
            double utc = 0.;
            /** Heartbeat Node Name */
            string node = "";
            //! Heartbeat Agent Name
            string proc = "";
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
            string user;
            //! Agent % CPU
            float cpu = 0.;
            //! Agent % memory
            float memory = 0.;
            //! Agent heartbeat jitter [sec]
            double jitter = 0.;
            //! Agent Duty Cycle
            float dcycle = 1.;
            //! Existence Flag (if beat exists then flag is set to true, false otherwise)
            bool exists = true;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
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
                    { "dcycle", dcycle },
                    { "exists", exists }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["utc"].is_null()) { utc = p["utc"].number_value(); }
                    if(!p["node"].is_null()) { node = p["node"].string_value(); }
                    if(!p["proc"].is_null()) { proc = p["proc"].string_value(); }
                    if(!p["ntype"].is_null()) { ntype = static_cast<NetworkType>(p["ntype"].int_value()); }
                    if(!p["addr"].is_null()) { strcpy(addr, p["addr"].string_value().c_str()); }
                    if(!p["port"].is_null()) { port = p["port"].int_value(); }
                    if(!p["bsz"].is_null()) { bsz = p["bsz"].int_value(); }
                    if(!p["bprd"].is_null()) { bprd = p["bprd"].number_value(); }
                    if(!p["user"].is_null()) { user = p["user"].string_value(); }
                    if(!p["cpu"].is_null()) { cpu = p["cpu"].number_value(); }
                    if(!p["memory"].is_null()) { memory = p["memory"].number_value(); }
                    if(!p["jitter"].is_null()) { jitter = p["jitter"].number_value(); }
                    if(!p["dcycle"].is_null()) { dcycle = p["dcycle"].number_value(); }
                    if(!p["exists"].is_null()) { exists = p["exists"].bool_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Agent control structure
        // JIMNOTE: take a look at this mess
        class agentstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(agentstruc);
                for (size_t i=0; i<reqs.size(); ++i)
                {
                    total += reqs[i].memoryusage();
                }
                total += beat.memoryusage();
                return total;
            }

            void shrinkusage()
            {
                for (size_t i=0; i<reqs.size(); ++i)
                {
                    reqs[i].shrinkusage();
                }
                vector<agent_request_entry>(reqs).swap(reqs);
                beat.shrinkusage();
            }

            //! Agent Name
            string name;
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                //		vector<uint16_t> v_pub = vector<uint16_t>(pub, pub+AGENTMAXIF);
                return json11::Json::object {
                    { "name" , name },
                    { "client" , client },
                    //TODO?			{ "sub"	, sub },
                    { "server" , server },
                    { "ifcnt"  , static_cast<int>(ifcnt) },
                    //			{ "pub"	, v_pub },
                    //			{ "req"	, req },
                    { "pid"	, pid },
                    { "aprd"   , aprd },
                    { "stateflag" , stateflag },
                    //			{ "reqs"   , reqs },
                    { "beat"   , beat }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["name"].is_null()) { name = p["name"].string_value(); }
                    if(!p["client"].is_null()) { client = p["client"].bool_value(); }
                    //TODO?			if(!p["sub"].is_null()) sub.from_json(p["sub"].dump());
                    if(!p["server"].is_null()) { server = p["server"].bool_value(); }
                    if(!p["ifcnt"].is_null()) { ifcnt = static_cast<size_t>(p["ifcnt"].number_value()); }
                    auto p_pub = p["pub"].array_items();
                    //			if(!p["pub"].is_null()) {
                    //				for(size_t i = 0; i < p_pub.size(); ++i)	{
                    // 					if(!p["pub"][i].is_null())	pub[i].from_json(p["pub"][i].dump());
                    //				}
                    //			}
                    //			if(!p["req"].is_null()) req.from_json(p["req"].dump());
                    if(!p["pid"].is_null()) { pid = p["pid"].int_value(); }
                    if(!p["aprd"].is_null()) { aprd = p["aprd"].number_value(); }
                    if(!p["stateflag"].is_null()) { stateflag = p["stateflag"].int_value(); }
                    //			if(!p["reqs"].is_null()) {
                    //				for(size_t i = 0; i < reqs.size(); ++i) {
                    //					if(!p["reqs"][i].is_null()) reqs[i].from_json(p["reqs"][i].dump());
                    //				}
                    //			}
                    if(!p["beat"].is_null()) { beat.from_json(p["beat"].dump()); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! STTR Simulation State Class
        /** Yes it is.
*/
        class sim_state	{
        public:

            string	node_name;
            string	agent_name;

            /** position */
            double	t_pos = 0.0;
            double	x_pos = 0.0;
            double	y_pos = 0.0;
            double	z_pos = 0.0;

            /** velocity */
            double	t_vel = 0.0;
            double	x_vel = 0.0;
            double	y_vel = 0.0;
            double	z_vel = 0.0;

            /** acceleration */
            double	t_acc = 0.0;
            double	x_acc = 0.0;
            double	y_acc = 0.0;
            double	z_acc = 0.0;

            /** attitude ( quaternion q = a + bi + cj + dk ) */
            double	t_att = 0.0;
            double	a_att = 0.0;
            double	b_att = 0.0;
            double	c_att = 0.0;
            double	d_att = 0.0;

            /** target attitude ( quaternion q = a + bi + cj + dk ) */
            double	t_att_target = 0.0;
            double	a_att_target = 0.0;
            double	b_att_target = 0.0;
            double	c_att_target = 0.0;
            double	d_att_target = 0.0;

            /** angular velocity */
			double t_omega = 0.0;
            double x_omega = 0.0;
            double y_omega = 0.0;
            double z_omega = 0.0;

            /** angular acceleration **/
			double t_alpha = 0.0;
            double x_alpha = 0.0;
            double y_alpha = 0.0;
            double z_alpha = 0.0;

            /** thrust */
			double t_thrust = 0.0;
            double x_thrust = 0.0;
            double y_thrust = 0.0;
            double z_thrust = 0.0;

            /** torque */
			double t_torque = 0.0;
            double x_torque = 0.0;
            double y_torque = 0.0;
            double z_torque = 0.0;

            /** acceleration differentials */
			double t_acc_diff = 0.0;
            double x_acc_diff = 0.0;
            double y_acc_diff = 0.0;
            double z_acc_diff = 0.0;

            /** target */
            double	target_latitude = 0.0;
            double	target_longitude = 0.0;
            double	target_altitude = 0.0;

            //  double	etc...

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "node_name"   , node_name },
                    { "agent_name"  , agent_name },

                    { "t_pos"   , t_pos },
                    { "x_pos"   , x_pos },
                    { "y_pos"   , y_pos },
                    { "z_pos"   , z_pos },

                    { "t_vel"   , t_vel },
                    { "x_vel"   , x_vel },
                    { "y_vel"   , y_vel },
                    { "z_vel"   , z_vel },

                    { "t_acc"   , t_acc },
                    { "x_acc"   , x_acc },
                    { "y_acc"   , y_acc },
                    { "z_acc"   , z_acc },

                    { "t_att"   , t_att },
                    { "a_att"   , a_att },
                    { "b_att"   , b_att },
                    { "c_att"   , c_att },
                    { "d_att"   , d_att },

                    { "t_att_target"   , t_att_target },
                    { "a_att_target"   , a_att_target },
                    { "b_att_target"   , b_att_target },
                    { "c_att_target"   , c_att_target },
                    { "d_att_target"   , d_att_target },

                    { "t_omega" , t_omega },
                    { "x_omega" , x_omega },
                    { "y_omega" , y_omega },
                    { "z_omega" , z_omega },

                    { "t_alpha" , t_alpha },
                    { "x_alpha" , x_alpha },
                    { "y_alpha" , y_alpha },
                    { "z_alpha" , z_alpha },

                    { "t_thrust", t_thrust },
                    { "x_thrust", x_thrust },
                    { "y_thrust", y_thrust },
                    { "z_thrust", z_thrust },

                    { "t_torque", t_torque },
                    { "x_torque", x_torque },
                    { "y_torque", y_torque },
                    { "z_torque", z_torque },

                    { "t_acc_diff"   , t_acc_diff },
                    { "x_acc_diff"   , x_acc_diff },
                    { "y_acc_diff"   , y_acc_diff },
                    { "z_acc_diff"   , z_acc_diff },

                    { "target_latitude"   , target_latitude },
                    { "target_longitude"   , target_longitude },
                    { "target_altitude"	  , target_altitude }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);

                if(error.empty()) {
                    if(!p["node_name"].is_null()) { node_name = p["node_name"].string_value(); }
                    if(!p["agent_name"].is_null()) { agent_name = p["agent_name"].string_value(); }

                    if(!p["t_pos"].is_null()) { t_pos = p["t_pos"].number_value(); }
                    if(!p["x_pos"].is_null()) { x_pos = p["x_pos"].number_value(); }
                    if(!p["y_pos"].is_null()) { y_pos = p["y_pos"].number_value(); }
                    if(!p["z_pos"].is_null()) { z_pos = p["z_pos"].number_value(); }

                    if(!p["t_vel"].is_null()) { t_vel = p["t_vel"].number_value(); }
                    if(!p["x_vel"].is_null()) { x_vel = p["x_vel"].number_value(); }
                    if(!p["y_vel"].is_null()) { y_vel = p["y_vel"].number_value(); }
                    if(!p["z_vel"].is_null()) { z_vel = p["z_vel"].number_value(); }

                    if(!p["t_acc"].is_null()) { t_acc = p["t_acc"].number_value(); }
                    if(!p["x_acc"].is_null()) { x_acc = p["x_acc"].number_value(); }
                    if(!p["y_acc"].is_null()) { y_acc = p["y_acc"].number_value(); }
                    if(!p["z_acc"].is_null()) { z_acc = p["z_acc"].number_value(); }

                    if(!p["t_att"].is_null()) { t_att = p["t_att"].number_value(); }
                    if(!p["a_att"].is_null()) { a_att = p["a_att"].number_value(); }
                    if(!p["b_att"].is_null()) { b_att = p["b_att"].number_value(); }
                    if(!p["c_att"].is_null()) { c_att = p["c_att"].number_value(); }
                    if(!p["d_att"].is_null()) { d_att = p["d_att"].number_value(); }

                    if(!p["t_att_target"].is_null()) { t_att_target = p["t_att_target"].number_value(); }
                    if(!p["a_att_target"].is_null()) { a_att_target = p["a_att_target"].number_value(); }
                    if(!p["b_att_target"].is_null()) { b_att_target = p["b_att_target"].number_value(); }
                    if(!p["c_att_target"].is_null()) { c_att_target = p["c_att_target"].number_value(); }
                    if(!p["d_att_target"].is_null()) { d_att_target = p["d_att_target"].number_value(); }

                    if(!p["t_omega"].is_null()) { t_omega = p["t_omega"].number_value(); }
                    if(!p["x_omega"].is_null()) { x_omega = p["x_omega"].number_value(); }
                    if(!p["y_omega"].is_null()) { y_omega = p["y_omega"].number_value(); }
                    if(!p["z_omega"].is_null()) { z_omega = p["z_omega"].number_value(); }

                    if(!p["t_alpha"].is_null()) { t_alpha = p["t_alpha"].number_value(); }
                    if(!p["x_alpha"].is_null()) { x_alpha = p["x_alpha"].number_value(); }
                    if(!p["y_alpha"].is_null()) { y_alpha = p["y_alpha"].number_value(); }
                    if(!p["z_alpha"].is_null()) { z_alpha = p["z_alpha"].number_value(); }

                    if(!p["t_thrust"].is_null()) { t_thrust = p["t_thrust"].number_value(); }
                    if(!p["x_thrust"].is_null()) { x_thrust = p["x_thrust"].number_value(); }
                    if(!p["y_thrust"].is_null()) { y_thrust = p["y_thrust"].number_value(); }
                    if(!p["z_thrust"].is_null()) { z_thrust = p["z_thrust"].number_value(); }

                    if(!p["t_torque"].is_null()) { t_torque = p["t_torque"].number_value(); }
                    if(!p["x_torque"].is_null()) { x_torque = p["x_torque"].number_value(); }
                    if(!p["y_torque"].is_null()) { y_torque = p["y_torque"].number_value(); }
                    if(!p["z_torque"].is_null()) { z_torque = p["z_torque"].number_value(); }

                    if(!p["t_acc_diff"].is_null()) { t_acc_diff = p["t_acc_diff"].number_value(); }
                    if(!p["x_acc_diff"].is_null()) { x_acc_diff = p["x_acc_diff"].number_value(); }
                    if(!p["y_acc_diff"].is_null()) { y_acc_diff = p["y_acc_diff"].number_value(); }
                    if(!p["z_acc_diff"].is_null()) { z_acc_diff = p["z_acc_diff"].number_value(); }

                    if(!p["target_latitude"].is_null()) { target_latitude = p["target_latitude"].number_value(); }
                    if(!p["target_longitude"].is_null()) { target_longitude = p["target_longitude"].number_value(); }
                    if(!p["target_altitude"].is_null()) { target_altitude = p["target_altitude"].number_value(); }

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
        class eventstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(eventstruc);
                total += node.capacity();
                total += name.capacity();
                total += user.capacity();
                total += data.capacity();
                total += condition.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
                string(node).swap(node);
                string(user).swap(user);
                string(data).swap(data);
                string(condition).swap(condition);
            }

            //! Time event is to start.
            double utc = 0.; //TODO: replace for mjd
            //! Time event was executed.
            double utcexec = 0.;
            //! Node for event
            //            char node[COSMOS_MAX_NAME+1] = "";
            string node;
            //! Name of event.
            // TODO: change char to string
            //            char name[COSMOS_MAX_NAME+1] = "";
            string name;
            //! User of event.
            //            char user[COSMOS_MAX_NAME+1] = "";
            string user;
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
            //	char data[JSON_MAX_DATA] = "";
            string data;
            //! Condition that caused event, NULL if timed event.
            //	char condition[JSON_MAX_DATA] = "";
            string condition;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
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
                    { "data"	, data },
                    { "condition" , condition }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["utc"].is_null()) { utc = p["utc"].number_value(); }
                    if(!p["utcexec"].is_null()) { utcexec = p["utcexec"].number_value(); }
                    if(!p["node"].is_null()) { node = p["node"].string_value(); }
                    if(!p["name"].is_null()) { name = p["name"].string_value(); }
                    if(!p["user"].is_null()) { user = p["user"].string_value(); }
                    if(!p["flag"].is_null()) { flag = p["flag"].int_value(); }
                    if(!p["type"].is_null()) { type = p["type"].int_value(); }
                    if(!p["value"].is_null()) { value = p["value"].number_value(); }
                    if(!p["dtime"].is_null()) { dtime = p["dtime"].number_value(); }
                    if(!p["ctime"].is_null()) { ctime = p["ctime"].number_value(); }
                    if(!p["denergy"].is_null()) { denergy = p["denergy"].number_value(); }
                    if(!p["cenergy"].is_null()) { cenergy = p["cenergy"].number_value(); }
                    if(!p["dmass"].is_null()) { dmass = p["dmass"].number_value(); }
                    if(!p["cmass"].is_null()) { cmass = p["cmass"].number_value(); }
                    if(!p["dbytes"].is_null()) { dbytes = p["dbytes"].number_value(); }
                    if(!p["cbytes"].is_null()) { cbytes = p["cbytes"].number_value(); }
                    if(!p["handle"].is_null()) { handle.from_json(p["handle"].dump()); }
                    //			if(!p["data"].is_null()) { strcpy(data, p["data"].string_value().c_str()); }
                    //			if(!p["condition"].is_null()) { strcpy(condition, p["condition"].string_value().c_str()); }
                    if(!p["data"].is_null()) { data = p["data"].string_value(); }
                    if(!p["condition"].is_null()) { condition = p["condition"].string_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        // TODO: add struct description
        class userstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(userstruc);
                total += name.capacity();
                total += node.capacity();
                total += tool.capacity();
                total += cpu.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
                string(node).swap(node);
                string(tool).swap(tool);
                string(cpu).swap(cpu);
            }

            //to enforce a string name length maximum (if necessary) use this code to truncate:
            //
            //constexpr string::size_type MAX_CHARS = 20 ;
            //if(whatever_string.size() > MAX_CHARS)	{
            //	whatever_string = whatever_string.substr(0, MAX_CHARS);
            //}
            string name = "";
            string node = "";
            string tool = "";
            string cpu = "";

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "name" , name },
                    { "node", node },
                    { "tool", tool },
                    { "cpu", cpu }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["name"].is_null())	{ name = parsed["name"].string_value(); }
                    if(!parsed["node"].is_null())	{ node = parsed["node"].string_value(); }
                    if(!parsed["tool"].is_null())	{ tool = parsed["tool"].string_value(); }
                    if(!parsed["cpu"].is_null())	{ cpu = parsed["cpu"].string_value(); }
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
        class aliasstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(aliasstruc);
                total += name.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
            }

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
        class equationstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(equationstruc);
                total += name.capacity();
                total += value.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
                string(value).swap(value);
            }

            // Equation name
            string name;
            // Equation string
            string value;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "name" , name },
                    { "value", value }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["name"].is_null()) { name = parsed["name"].string_value(); }
                    if(!parsed["value"].is_null()) { value = parsed["value"].string_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };


        //! Target structure
        /*! Contains entries unique to each Target
*/
        class targetstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(targetstruc);
                total += name.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
            }

            double utc = 0.;
            string name;
            uint16_t type = 0;
            float azfrom = 0.f;
            float elfrom = 0.f;
            float azto = 0.f;
            float elto = 0.f;
            float maxelto = 0.f;
            double range = 0.;
            double close = 0.;
            float min = 0.f;
            float bearing;
            float distance;
            Convert::locstruc loc;
            Convert::locstruc cloc;
            gvector size = {0., 0., 0.};
            double area;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc"	, utc },
                    { "name"   , name },
                    { "type"   , type },
                    { "azfrom" , azfrom },
                    { "elfrom" , elfrom },
                    { "azto"   , azto },
                    { "elto"   , elto },
                    { "maxelto"   , maxelto },
                    { "range"  , range },
                    { "close"  , close },
                    { "min"	, min },
                    { "bearing"	, bearing },
                    { "distance"	, distance },
                    { "loc"	, loc },
                    { "cloc"	, cloc },
                    { "size", size},
                    { "area", area},
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["utc"].is_null()) { utc = p["utc"].number_value(); }
                    if(!p["name"].is_null()) { name = p["name"].string_value(); }
                    if(!p["type"].is_null()) { type = p["type"].int_value(); }
                    if(!p["azfrom"].is_null()) { azfrom = p["azfrom"].number_value(); }
                    if(!p["elfrom"].is_null()) { elfrom = p["elfrom"].number_value(); }
                    if(!p["azto"].is_null()) { azto = p["azto"].number_value(); }
                    if(!p["elto"].is_null()) { elto = p["elto"].number_value(); }
                    if(!p["maxelto"].is_null()) { maxelto = p["maxelto"].number_value(); }
                    if(!p["range"].is_null()) { range = p["range"].number_value(); }
                    if(!p["close"].is_null()) { close = p["close"].number_value(); }
                    if(!p["min"].is_null()) { min = p["min"].number_value(); }
                    if(!p["bearing"].is_null()) { bearing = p["bearing"].number_value(); }
                    if(!p["distance"].is_null()) { distance = p["distance"].number_value(); }
                    if(!p["loc"].is_null()) { loc.from_json(p["loc"].dump()); }
                    if(!p["cloc"].is_null()) { cloc.from_json(p["cloc"].dump()); }
                    if(!p["size"].is_null()) { size.from_json(p["size"].dump()); }
                    if(!p["area"].is_null()) { area = p["area"].number_value(); }
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
        class portstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(portstruc);
                total += name.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
            }

            //! Type of I/O as listed in ::PORT_TYPE.
            PORT_TYPE type = PORT_TYPE_NONE;
            //! Name information for port.
            // TODO: double check this comment not to make this a string
            //!!! Do not make this string
            //            char name[COSMOS_MAX_DATA+1] = "";
            string name;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "type" , type },
                    { "name" , name }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["type"].is_null()) { type = static_cast<PORT_TYPE>(p["type"].int_value()); }
                    if(!p["name"].is_null()) { name = p["name"].string_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Point structure: information on each vertex in a face
        typedef Vector vertexstruc;

        //! Face structure: information on each face of a piece
        class facestruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(facestruc);
                total += vertex_idx.capacity() * sizeof(uint16_t);
                return total;
            }

            void shrinkusage()
            {
                vector<uint16_t>(vertex_idx).swap(vertex_idx);
            }

            uint16_t vertex_cnt = 0;
            vector <uint16_t> vertex_idx;
            Vector com;
            Vector normal;
            double area=0.;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "vertex_cnt" , vertex_cnt},
                    { "vertex_idx" , vertex_idx },
                    { "com"	, com },
                    { "normal" , normal },
                    { "area"   , area }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["vertex_cnt"].is_null()) { vertex_cnt = parsed["vertex_cnt"].int_value(); }
                    for(size_t i = 0; i < vertex_idx.size(); ++i)	{
                        if(!parsed["vertex_idx"][i].is_null())	{ vertex_idx[i] = parsed["vertex_idx"][i].int_value(); }
                    }
                    if(!parsed["com"].is_null()) { com.from_json(parsed["com"].dump()); }
                    if(!parsed["normal"].is_null()) { normal.from_json(parsed["normal"].dump()); }
                    if(!parsed["area"].is_null()) { area = parsed["area"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Part structure: physical information for each piece of Node
        class piecestruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(piecestruc);
                total += name.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
                vector<uint16_t>(face_idx).swap(face_idx);
            }

            //! Name of piece
            //            char name[COSMOS_MAX_NAME+1] = "";
            string name;
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "name"	, name },
                    { "enabled" , enabled },
                    { "cidx"	, cidx },
                    { "density" , density },
                    { "mass"	, mass },
                    { "emi"	 , emi },
                    { "abs"	 , abs },
                    { "hcap"	, hcap },
                    { "hcon"	, hcon },
                    { "dim"	 , dim },
                    { "area"	, area },
                    { "volume"  , volume },
                    { "face_cnt", face_cnt },
                    { "face_idx", face_idx },
                    { "com"	 , com },
                    { "shove"   , shove },
                    { "twist"   , twist },
                    { "heat"	, heat },
                    { "temp"	, temp },
                    { "insol"   , insol },
                    { "material_density" , material_density },
                    { "material_ambient" , material_ambient },
                    { "material_diffuse" , material_diffuse },
                    { "material_specular", material_specular }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!p["name"].is_null()) { name = p["name"].string_value(); }
                    if(!p["enabled"].is_null()) { enabled = p["enabled"].bool_value(); }
                    if(!p["cidx"].is_null()) { cidx = p["cidx"].int_value(); }
                    if(!p["density"].is_null()) { density = p["density"].number_value(); }
                    if(!p["mass"].is_null()) { mass = p["mass"].number_value(); }
                    if(!p["emi"].is_null()) { emi = p["emi"].number_value(); }
                    if(!p["abs"].is_null()) { abs = p["abs"].number_value(); }
                    if(!p["hcap"].is_null()) { hcap = p["hcap"].number_value(); }
                    if(!p["hcon"].is_null()) { hcon = p["hcon"].number_value(); }
                    if(!p["dim"].is_null()) { dim = p["dim"].number_value(); }
                    if(!p["area"].is_null()) { area = p["area"].number_value(); }
                    if(!p["volume"].is_null()) { volume = p["volume"].number_value(); }
                    if(!p["face_cnt"].is_null()) { face_cnt = p["face_cnt"].int_value(); }
                    if(!p["face_idx"].is_null()) {
                        auto p_face_idx = p["face_idx"].array_items();
                        for(size_t i = 0; i != p_face_idx.size(); ++i) {
                            if(!p_face_idx[i].is_null()) { face_idx[i] = p_face_idx[i].int_value(); }
                        }
                    }
                    if(!p["com"].is_null()) { com.from_json(p["com"].dump()); }
                    if(!p["shove"].is_null()) { shove.from_json(p["shove"].dump()); }
                    if(!p["twist"].is_null()) { twist.from_json(p["twist"].dump()); }
                    if(!p["heat"].is_null()) { heat = p["heat"].number_value(); }
                    if(!p["temp"].is_null()) { temp = p["temp"].number_value(); }
                    if(!p["insol"].is_null()) { insol = p["insol"].number_value(); }
                    if(!p["material_density"].is_null()) { material_density = p["material_density"].number_value(); }
                    if(!p["material_ambient"].is_null()) { material_ambient.from_json(p["material_ambient"].dump()); }
                    if(!p["material_diffuse"].is_null()) { material_diffuse.from_json(p["material_diffuse"].dump()); }
                    if(!p["material_specular"].is_null()) { material_specular.from_json(p["material_specular"].dump()); }
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
        //        struct allstruc
        class devicestruc
        {
        public:
            //! Device name
            string name;
            //! Enabled?
            uint8_t state = 0;
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
            uint16_t portidx = PORT_TYPE_NONE; // TODO: rename to port_id or port_index
            //! Nominal Amperage
            float namp = 0.f; // TODO: rename to nominal current
            //! Nominal Voltage
            float nvolt = 0.f; // TODO: rename to nominal voltage
            //! Current Amperage
            float amp = 0.f; // TODO: rename to current
            //! Current Voltage
            float volt = 0.f; // TODO: rename to voltage
            //! Current Power
            float power = 0.f;
            //! Total energy usage
            float energy = 0.f;
            //! Current data rate
            float drate = 0.f;
            //! Current Temperature
            float temp = 0.f; // TODO: rename to temperature
            //! Device information time stamp
            double utc = 0.;
            //! Device turn off time stamp
            double expiration = 0.;
            //! Pointer to Specific Device
            void *ptr = nullptr;


            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "state" , state },
                    { "type"	, type },
                    { "model"   , model },
                    { "flag"	, static_cast<int>(flag) },
                    { "addr"	, addr },
                    { "cidx"	, cidx },
                    { "didx"	, didx },
                    { "pidx"	, pidx },
                    { "bidx"	, bidx },
                    { "portidx" , portidx },
                    { "namp"	, namp},
                    { "nvolt"   , nvolt },
                    { "amp"	 , amp },
                    { "volt"	, volt },
                    { "power"   , power },
                    { "energy"  , energy },
                    { "drate"   , drate },
                    { "temp"	, temp },
                    { "utc"	 , utc },
                    { "expiration"	 , expiration },
                    { "name"	 , name },
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["state"].is_null()) { state = parsed["state"].int_value(); }
                    if(!parsed["type"].is_null()) { type = parsed["type"].int_value(); }
                    if(!parsed["model"].is_null()) { model = parsed["model"].int_value(); }
                    if(!parsed["flag"].is_null()) { flag = parsed["flag"].int_value(); }
                    if(!parsed["addr"].is_null()) { addr = parsed["addr"].int_value(); }
                    if(!parsed["cidx"].is_null()) { cidx = parsed["cidx"].int_value(); }
                    if(!parsed["didx"].is_null()) { didx = parsed["didx"].int_value(); }
                    if(!parsed["pidx"].is_null()) { pidx = parsed["pidx"].int_value(); }
                    if(!parsed["bidx"].is_null()) { bidx = parsed["bidx"].int_value(); }
                    if(!parsed["portidx"].is_null()) { portidx = parsed["portidx"].int_value(); }
                    if(!parsed["namp"].is_null()) { namp = parsed["namp"].number_value(); }
                    if(!parsed["nvolt"].is_null()) { nvolt = parsed["nvolt"].number_value(); }
                    if(!parsed["amp"].is_null()) { amp = parsed["amp"].number_value(); }
                    if(!parsed["volt"].is_null()) { volt = parsed["volt"].number_value(); }
                    if(!parsed["power"].is_null()) { power = parsed["power"].number_value(); }
                    if(!parsed["energy"].is_null()) { energy = parsed["energy"].number_value(); }
                    if(!parsed["drate"].is_null()) { drate = parsed["drate"].number_value(); }
                    if(!parsed["temp"].is_null()) { temp = parsed["temp"].number_value(); }
                    if(!parsed["utc"].is_null()) { utc = parsed["utc"].number_value(); }
                    if(!parsed["expiration"].is_null()) { expiration = parsed["expiration"].number_value(); }
                    if(!parsed["name"].is_null()) { name = parsed["name"].string_value(); }
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
        static map<string, uint16_t> TelemTypeValue = {
            {"vuint8", 0},
            {"vint8", 1},
            {"vuint16", 2},
            {"vint16", 3},
            {"vuint32", 4},
            {"vint32", 5},
            {"vfloat", 6},
            {"vdouble", 7},
            {"vstring", 8}
        };

        static map<uint16_t, string> TelemTypeName = {
            {0, "vuint8"},
            {1, "vint8"},
            {2, "vuint16"},
            {3, "vint16"},
            {4, "vuint32"},
            {5, "vint32"},
            {6, "vfloat"},
            {7, "vdouble"},
            {8, "vstring"}
        };

        static map<uint16_t, uint16_t> TelemTypeSize = {
            {0, 1},
            {1, 1},
            {2, 2},
            {3, 2},
            {4, 4},
            {5, 4},
            {6, 4},
            {7, 8},
            {8, 0}
        };

        struct telemstruc : public devicestruc
        {
            //! Data type
            uint16_t vtype = 0;
            uint8_t vuint8 = 0;
            int8_t vint8 = 0;
            uint16_t vuint16 = 0;
            int16_t vint16 = 0;
            uint32_t vuint32 = 0;
            int32_t vint32 = 0;
            float vfloat = 0.f;
            double vdouble = 0.f;
            string vstring;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "vtype" , vtype },
                    { "vuint8", static_cast<int>(vuint8) },
                    { "vint8", vint8 },
                    { "vuint16", vuint16 },
                    { "vint16", vint16 },
                    { "vuint32", static_cast<int>(vuint32) },
                    { "vint32", vint32 },
                    { "vfloat", vfloat },
                    { "vdouble", vdouble },
                    { "vstring", vstring }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["type"].is_null()) { type = parsed["type"].number_value(); }
                    //  TODO:		  vstring = parsed["vstring"].string_value();
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
        struct ploadstruc : public devicestruc
        {
            //! Number of keys being used.
            uint16_t key_cnt = 0;
            //! Name for each key.
            uint16_t keyidx[MAXPLOADKEYCNT] = {0};
            //! Value for each key.
            float keyval[MAXPLOADKEYCNT] = {0.f};

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                vector<uint16_t> v_keyidx = vector<uint16_t>(keyidx, keyidx+MAXPLOADKEYCNT);
                vector<float> v_keyval = vector<float>(keyval, keyval+MAXPLOADKEYCNT);
                return json11::Json::object {
                    { "key_cnt", key_cnt },
                    { "keyidx" , v_keyidx },
                    { "keyval" , v_keyval }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["key_cnt"].is_null()) { key_cnt = parsed["key_cnt"].number_value(); }
                    if(!parsed["keyidx"].is_null()) {
                        auto p_keyidx = parsed["keyidx"].array_items();
                        for(size_t i = 0; i != p_keyidx.size(); ++i) {
                            if(!parsed["keyidx"][i].is_null()) { keyidx[i] = p_keyidx[i].int_value(); }
                        }
                    }
                    if(!parsed["keyval"].is_null()) {
                        auto p_keyval = parsed["keyval"].array_items();
                        for(size_t i = 0; i != p_keyval.size(); ++i) {
                            if(!parsed["keyval"][i].is_null()) { keyval[i] = p_keyval[i].number_value(); }
                        }
                    }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Sun Sensor (SSEN) Sructure
        struct ssenstruc : public devicestruc
        {
            //! Sensor alignment quaternion.
            quaternion align;
            float qva = 0.f;
            float qvb = 0.f;
            float qvc = 0.f;
            float qvd = 0.f;
            float azimuth = 0.f;
            float elevation = 0.f;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
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

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["qva"].is_null()) { qva = parsed["qva"].number_value(); }
                    if(!parsed["qvb"].is_null()) { qvb = parsed["qvb"].number_value(); }
                    if(!parsed["qvc"].is_null()) { qvc = parsed["qvc"].number_value(); }
                    if(!parsed["qvd"].is_null()) { qvd = parsed["qvd"].number_value(); }
                    if(!parsed["azimuth"].is_null()) { azimuth = parsed["azimuth"].number_value(); }
                    if(!parsed["elevation"].is_null()) { elevation = parsed["elevation"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! XYZ Directional Sensor (XYZSEN) Sructure
        struct xyzsenstruc : public devicestruc
        {
            rvector direction;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "direction" , direction },
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["direction"].is_null()) { direction.from_json(parsed["direction"].dump()); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Inertial Measurement Unit (IMU) structure
        struct imustruc : public devicestruc
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
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

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["accel"].is_null()) { accel.from_json(parsed["accel"].dump()); }
                    if(!parsed["theta"].is_null()) { theta.from_json(parsed["theta"].dump()); }
                    if(!parsed["euler"].is_null()) { euler.from_json(parsed["euler"].dump()); }
                    if(!parsed["omega"].is_null()) { omega.from_json(parsed["omega"].dump()); }
                    if(!parsed["alpha"].is_null()) { alpha.from_json(parsed["alpha"].dump()); }
                    if(!parsed["mag"].is_null()) { mag.from_json(parsed["mag"].dump()); }
                    if(!parsed["bdot"].is_null()) { bdot.from_json(parsed["bdot"].dump()); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Magnetometer structure
        struct magstruc : public devicestruc
        {
            //! alignment quaternion
            quaternion align;
            //! Magnetic field in sensor frame
            rvector mag;
            //! Magnetic field rate change in sensor frame
            rvector bdot;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "align" , align },
                    { "mag"   , mag },
                    { "bdot"  , bdot }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["mag"].is_null()) { mag.from_json(parsed["mag"].dump()); }
                    if(!parsed["bdot"].is_null()) { bdot.from_json(parsed["bdot"].dump()); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Inertial Measurement Unit (IMU) structure
        struct gyrostruc : public devicestruc
        {
            //! alignment quaternion
            quaternion align;
            //! Attitude rate vector
            float omega;
            //! Attitude acceleration vector
            float alpha;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "align" , align },
                    { "omega" , omega },
                    { "alpha" , alpha },
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["omega"].is_null()) { omega = parsed["omega"].number_value(); }
                    if(!parsed["alpha"].is_null()) { alpha = parsed["alpha"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Reaction Wheel structure: z axis is aligned with axis of rotation.
        struct rwstruc : public devicestruc
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "align" , align },
                    { "mom"   , mom },
                    { "mxomg" , mxomg },
                    { "mxalp" , mxalp },
                    { "tc"	, tc },
                    { "omg"   , omg },
                    { "alp"   , alp },
                    { "romg"  , romg },
                    { "ralp"  , ralp },
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["mom"].is_null()) { mom.from_json(parsed["mom"].dump()); }
                    if(!parsed["mxomg"].is_null()) { mxomg = parsed["mxomg"].number_value(); }
                    if(!parsed["bdmxalpot"].is_null()) { mxalp = parsed["mxalp"].number_value(); }
                    if(!parsed["tc"].is_null()) { tc = parsed["tc"].number_value(); }
                    if(!parsed["omg"].is_null()) { omg = parsed["omg"].number_value(); }
                    if(!parsed["alp"].is_null()) { alp = parsed["alp"].number_value(); }
                    if(!parsed["romg"].is_null()) { romg = parsed["romg"].number_value(); }
                    if(!parsed["ralp"].is_null()) { ralp = parsed["ralp"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Magnetic Torque Rod structure: z axis is aligned with rod.
        struct mtrstruc : public devicestruc
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                vector<float> v_npoly = vector<float>(npoly, npoly+sizeof(npoly)/sizeof(npoly[0]));
                vector<float> v_ppoly = vector<float>(ppoly, npoly+sizeof(ppoly)/sizeof(ppoly[0]));
                return json11::Json::object {
                    { "align" , align },
                    { "npoly"   , v_npoly },
                    { "ppoly"   , v_ppoly },
                    { "mxmom"   , mxmom },
                    { "tc"	  , tc },
                    { "rmom"	, rmom },
                    { "mom"	 , mom }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["npoly"].is_null()) {
                        auto p_npoly = parsed["npoly"].array_items();
                        for(size_t i = 0; i != p_npoly.size(); ++i) {
                            if(!p_npoly[i].is_null()) { npoly[i] = p_npoly[i].number_value(); }
                        }
                    }
                    if(!parsed["ppoly"].is_null()) {
                        auto p_ppoly = parsed["ppoly"].array_items();
                        for(size_t i = 0; i != p_ppoly.size(); ++i) {
                            if(!p_ppoly[i].is_null()) { ppoly[i] = p_ppoly[i].number_value(); }
                        }
                    }
                    if(!parsed["mxmom"].is_null()) { mxmom = parsed["mxmom"].number_value(); }
                    if(!parsed["tc"].is_null()) { tc = parsed["tc"].number_value(); }
                    if(!parsed["rmom"].is_null()) { rmom = parsed["rmom"].number_value(); }
                    if(!parsed["mom"].is_null()) { mom = parsed["mom"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! CPU information
        struct cpustruc : public devicestruc
        {
            // cpu
            //! Seconds CPU has been up
            uint32_t uptime = 0;
            //! Current load
            float load = 0.f;
            //! Maximum load
            float maxload = 1.f;

            // memory
            //! Maximum memory capacity in GiB
            float maxgib = 1.f;
            //! Current memory usage in GiB
            float gib = 0.f;

            //! Number of reboots
            uint32_t boot_count = 0;

            //! Root storage
            float storage = 0.f;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "uptime" , static_cast<int>(uptime) },
                    { "load"   , load },
                    { "maxload", maxload },
                    { "maxgib" , maxgib },
                    { "gib"	, gib },
                    { "storage"	, storage },
                    { "boot_count", static_cast<int>(boot_count) },
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["uptime"].is_null()) { uptime = parsed["uptime"].int_value(); }
                    if(!parsed["load"].is_null()) { load = parsed["load"].number_value(); }
                    if(!parsed["maxload"].is_null()) { maxload = parsed["maxload"].number_value(); }
                    if(!parsed["maxgib"].is_null()) { maxgib = parsed["maxgib"].number_value(); }
                    if(!parsed["gib"].is_null()) { gib = parsed["gib"].number_value(); }
                    if(!parsed["storage"].is_null()) { storage = parsed["storage"].number_value(); }
                    if(!parsed["boot_count"].is_null()) { boot_count = parsed["boot_count"].int_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Disk information
        struct diskstruc : public devicestruc
        {
            size_t memoryusage()
            {
                size_t total = sizeof(diskstruc);
                total += path.capacity();
                return total;
            }
            // disk
            //! Maximum disk capacity in GiB
            float maxgib = 100.f;  // TODO: rename to diskSize, consider bytes?
            //! Current disk usage in GiB
            float gib = 0.f; // TODO: rename to diskUsed, consider bytes?
            //! Path
            //            char path[COSMOS_MAX_NAME] = "";
            string path;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "maxgib" , maxgib },
                    { "gib"	, gib },
                    { "path"   , path }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["maxgib"].is_null()) { maxgib = parsed["maxgib"].number_value(); }
                    if(!parsed["gib"].is_null()) { gib = parsed["gib"].number_value(); }
                    if(!parsed["path"].is_null()) { path = parsed["path"].string_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        // TODO: rename to GpsData
        struct gpsstruc : public devicestruc
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
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
                    { "sats_used"	, sats_used },
                    { "sats_visible" , sats_visible },
                    { "time_status"  , time_status },
                    { "position_type", position_type },
                    { "solution_status", solution_status }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["dutc"].is_null()) { dutc = parsed["dutc"].number_value(); }
                    if(!parsed["geocs"].is_null()) { geocs.from_json(parsed["geocs"].dump()); }
                    if(!parsed["geocv"].is_null()) { geocv.from_json(parsed["geocv"].dump()); }
                    if(!parsed["dgeocs"].is_null()) { dgeocs.from_json(parsed["dgeocs"].dump()); }
                    if(!parsed["dgeocv"].is_null()) { dgeocv.from_json(parsed["dgeocv"].dump()); }
                    if(!parsed["geods"].is_null()) { geods.from_json(parsed["geods"].dump()); }
                    if(!parsed["geodv"].is_null()) { geodv.from_json(parsed["geodv"].dump()); }
                    if(!parsed["dgeods"].is_null()) { dgeods.from_json(parsed["dgeods"].dump()); }
                    if(!parsed["dgeodv"].is_null()) { dgeodv.from_json(parsed["dgeodv"].dump()); }
                    if(!parsed["heading"].is_null()) { heading = parsed["heading"].number_value(); }
                    if(!parsed["sats_used"].is_null()) { sats_used = parsed["sats_used"].int_value(); }
                    if(!parsed["sats_visible"].is_null()) { sats_visible = parsed["sats_visible"].int_value(); }
                    if(!parsed["time_status"].is_null()) { time_status = parsed["time_status"].int_value(); }
                    if(!parsed["position_type"].is_null()) { position_type = parsed["position_type"].int_value(); }
                    if(!parsed["solution_status"].is_null()) { solution_status = parsed["solution_status"].int_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Antenna information
        /*! This structure holds the information concerning antennas.
*/
        struct antstruc : public devicestruc
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
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

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["azim"].is_null()) { azim = parsed["azim"].number_value(); }
                    if(!parsed["elev"].is_null()) { elev = parsed["elev"].number_value(); }
                    if(!parsed["minelev"].is_null()) { minelev = parsed["minelev"].number_value(); }
                    if(!parsed["maxelev"].is_null()) { maxelev = parsed["maxelev"].number_value(); }
                    if(!parsed["minazim"].is_null()) { minazim = parsed["minazim"].number_value(); }
                    if(!parsed["maxazim"].is_null()) { maxazim = parsed["maxazim"].number_value(); }
                    if(!parsed["threshelev"].is_null()) { threshelev = parsed["threshelev"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Receiver information
        /*! This structure holds the information concerning receivers.
*/
        struct rxrstruc : public devicestruc
        {
            //! Operating mode
            uint16_t opmode = 0;
            //! Data modulation
            uint16_t modulation = 0;
            //! RSSI
            uint16_t rssi = 0;
            //! Packet Size
            uint16_t pktsize = 0;
            //! Rate in Bytes/Second
            float byte_rate = 0.f;
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
            //! Input Frequency
            double freq = 0.;
            //! Maximum frequency allowed
            double maxfreq = 0.;
            //! Minimum frequency allowed
            double minfreq = 0.;
            //! Good Packet Percentage
            double goodratio = 0.;
            //! Last RX time
            double utcin = 0.;
            //! Connection Uptime
            double uptime = 0.;
            //! Total bytes in
            int32_t bytesin = 0;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "opmode" , opmode },
                    { "modulation" , modulation },
                    { "rssi"   , rssi },
                    { "pktsize", pktsize },
                    { "freq"   , freq },
                    { "maxfreq", maxfreq },
                    { "minfreq", minfreq },
                    { "byte_rate", byte_rate },
                    { "powerin", powerin },
                    { "powerout" , powerout },
                    { "maxpower" , maxpower },
                    { "band"	 , band },
                    { "squelch_tone", squelch_tone },
                    { "goodratio", goodratio },
                    { "utcin"  , utcin },
                    { "uptime" , uptime },
                    { "bytesin", bytesin}
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["opmode"].is_null()) { opmode = parsed["opmode"].int_value(); }
                    if(!parsed["modulation"].is_null()) { modulation = parsed["modulation"].int_value(); }
                    if(!parsed["rssi"].is_null()) { rssi = parsed["rssi"].int_value(); }
                    if(!parsed["pktsize"].is_null()) { pktsize = parsed["pktsize"].int_value(); }
                    if(!parsed["freq"].is_null()) { freq = parsed["freq"].number_value(); }
                    if(!parsed["maxfreq"].is_null()) { maxfreq = parsed["maxfreq"].number_value(); }
                    if(!parsed["minfreq"].is_null()) { minfreq = parsed["minfreq"].number_value(); }
                    if(!parsed["byte_rate"].is_null()) { byte_rate = parsed["byte_rate"].number_value(); }
                    if(!parsed["powerin"].is_null()) { powerin = parsed["powerin"].number_value(); }
                    if(!parsed["powerout"].is_null()) { powerout = parsed["powerout"].number_value(); }
                    if(!parsed["maxpower"].is_null()) { maxpower = parsed["maxpower"].number_value(); }
                    if(!parsed["band"].is_null()) { band = parsed["band"].number_value(); }
                    if(!parsed["squelch_tone"].is_null()) { squelch_tone = parsed["squelch_tone"].number_value(); }
                    if(!parsed["goodratio"].is_null()) { goodratio = parsed["goodratio"].number_value(); }
                    if(!parsed["utcin"].is_null()) { utcin = parsed["utcin"].number_value(); }
                    if(!parsed["uptime"].is_null()) { uptime = parsed["uptime"].number_value(); }
                    if(!parsed["bytesin"].is_null()) { bytesin = parsed["bytesin"].int_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Transmitter information
        /*! This structure holds the information concerning transmitters.
*/
        struct txrstruc : public devicestruc
        {
            //! Operating mode
            uint16_t opmode = 0;
            //! Data modulation
            uint16_t modulation = 0;
            //! RSSI
            uint16_t rssi = 0;
            //! Packet Size
            uint16_t pktsize = 0;
            //! Rate in Bytes/Second
            float byte_rate = 0.f;
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
            //! Input Frequency
            double freq = 0.;
            //! Maximum frequency allowed
            double maxfreq = 0.;
            //! Minimum frequency allowed
            double minfreq = 0.;
            //! Good Packet Percentage
            double  goodratio = 0.;
            //! Last TX time
            double utcout = 0.;
            //! Connection Uptime
            double uptime = 0.;
            //! Total bytes out
            int32_t bytesout = 0;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "opmode" , opmode },
                    { "modulation" , modulation },
                    { "rssi"   , rssi },
                    { "pktsize", pktsize },
                    { "freq"   , freq },
                    { "maxfreq", maxfreq },
                    { "minfreq", minfreq },
                    { "byte_rate", byte_rate },
                    { "powerin", powerin },
                    { "powerout" , powerout },
                    { "maxpower" , maxpower },
                    { "band"	 , band },
                    { "squelch_tone", squelch_tone },
                    { "goodratio", goodratio },
                    { "utcout"  , utcout },
                    { "uptime" , uptime },
                    { "bytesout", bytesout}
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["opmode"].is_null()) { opmode = parsed["opmode"].int_value(); }
                    if(!parsed["modulation"].is_null()) { modulation = parsed["modulation"].int_value(); }
                    if(!parsed["rssi"].is_null()) { rssi = parsed["rssi"].int_value(); }
                    if(!parsed["pktsize"].is_null()) { pktsize = parsed["pktsize"].int_value(); }
                    if(!parsed["freq"].is_null()) { freq = parsed["freq"].number_value(); }
                    if(!parsed["maxfreq"].is_null()) { maxfreq = parsed["maxfreq"].number_value(); }
                    if(!parsed["minfreq"].is_null()) { minfreq = parsed["minfreq"].number_value(); }
                    if(!parsed["byte_rate"].is_null()) { byte_rate = parsed["byte_rate"].number_value(); }
                    if(!parsed["powerin"].is_null()) { powerin = parsed["powerin"].number_value(); }
                    if(!parsed["powerout"].is_null()) { powerout = parsed["powerout"].number_value(); }
                    if(!parsed["maxpower"].is_null()) { maxpower = parsed["maxpower"].number_value(); }
                    if(!parsed["band"].is_null()) { band = parsed["band"].number_value(); }
                    if(!parsed["squelch_tone"].is_null()) { squelch_tone = parsed["squelch_tone"].number_value(); }
                    if(!parsed["goodratio"].is_null()) { goodratio = parsed["goodratio"].number_value(); }
                    if(!parsed["utcout"].is_null()) { utcout = parsed["utcout"].number_value(); }
                    if(!parsed["uptime"].is_null()) { uptime = parsed["uptime"].number_value(); }
                    if(!parsed["bytesout"].is_null()) { bytesout = parsed["bytesout"].int_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Transceiver information
        /*! This structure holds the information concerning transceivers.
*/
        struct tcvstruc : public devicestruc
        {
            //! Operating mode
            uint16_t opmode = 0;
            //! Data modulation
            uint16_t modulation = 0;
            //! RSSI
            uint16_t rssi = 0;
            //! Packet Size
            uint16_t pktsize = 0;
            //! Rate in Bytes/Second
            float byte_rate = 0.f;
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
            //! Input Frequency
            double freq = 0.;
            //! Maximum frequency allowed
            double maxfreq = 0.;
            //! Minimum frequency allowed
            double minfreq = 0.;
            //! Good Packet Percentage
            double  goodratio = 0.;
            //! Last TX time
            double utcout = 0.;
            //! Last RX time
            double utcin = 0.;
            //! Connection Uptime
            double uptime = 0.;
            //! Total bytes out
            int32_t bytesout = 0;
            //! Total bytes in
            int32_t bytesin = 0;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "opmode" , opmode },
                    { "modulation" , modulation },
                    { "rssi"   , rssi },
                    { "pktsize", pktsize },
                    { "freq"   , freq },
                    { "maxfreq", maxfreq },
                    { "minfreq", minfreq },
                    { "byte_rate", byte_rate },
                    { "powerin", powerin },
                    { "powerout" , powerout },
                    { "maxpower" , maxpower },
                    { "band"	 , band },
                    { "squelch_tone", squelch_tone },
                    { "goodratio", goodratio },
                    { "utcout"  , utcout },
                    { "utcin"  , utcin },
                    { "uptime" , uptime },
                    { "bytesout", bytesout},
                    { "bytesin", bytesin}
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["opmode"].is_null()) { opmode = parsed["opmode"].int_value(); }
                    if(!parsed["modulation"].is_null()) { modulation = parsed["modulation"].int_value(); }
                    if(!parsed["rssi"].is_null()) { rssi = parsed["rssi"].int_value(); }
                    if(!parsed["pktsize"].is_null()) { pktsize = parsed["pktsize"].int_value(); }
                    if(!parsed["freq"].is_null()) { freq = parsed["freq"].number_value(); }
                    if(!parsed["maxfreq"].is_null()) { maxfreq = parsed["maxfreq"].number_value(); }
                    if(!parsed["minfreq"].is_null()) { minfreq = parsed["minfreq"].number_value(); }
                    if(!parsed["byte_rate"].is_null()) { byte_rate = parsed["byte_rate"].number_value(); }
                    if(!parsed["powerin"].is_null()) { powerin = parsed["powerin"].number_value(); }
                    if(!parsed["powerout"].is_null()) { powerout = parsed["powerout"].number_value(); }
                    if(!parsed["maxpower"].is_null()) { maxpower = parsed["maxpower"].number_value(); }
                    if(!parsed["band"].is_null()) { band = parsed["band"].number_value(); }
                    if(!parsed["squelch_tone"].is_null()) { squelch_tone = parsed["squelch_tone"].number_value(); }
                    if(!parsed["goodratio"].is_null()) { goodratio = parsed["goodratio"].number_value(); }
                    if(!parsed["utcout"].is_null()) { utcout = parsed["utcout"].number_value(); }
                    if(!parsed["utcin"].is_null()) { utcin = parsed["utcin"].number_value(); }
                    if(!parsed["uptime"].is_null()) { uptime = parsed["uptime"].number_value(); }
                    if(!parsed["bytesin"].is_null()) { bytesin = parsed["bytesin"].int_value(); }
                    if(!parsed["bytesout"].is_null()) { bytesout = parsed["bytesout"].int_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! PV String (STRG) structure.
        /*! Efficiency is goven as effbase + effslope * Tkelvin.
*/
        struct pvstrgstruc : public devicestruc
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "bcidx"  , bcidx },
                    { "effbase", effbase },
                    { "effslope", effslope },
                    { "maxpower", maxpower },
                    { "power"  , power}
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["bcidx"].is_null()) { bcidx = parsed["bcidx"].int_value(); }
                    if(!parsed["effbase"].is_null()) { effbase = parsed["effbase"].number_value(); }
                    if(!parsed["effslope"].is_null()) { effslope = parsed["effslope"].number_value(); }
                    if(!parsed["maxpower"].is_null()) { maxpower = parsed["maxpower"].number_value(); }
                    if(!parsed["power"].is_null()) { power = parsed["power"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Battery (BATT) structure.
        struct battstruc : public devicestruc
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "cap"  , capacity },
                    { "eff", efficiency },
                    { "charge"	, charge },
                    { "r_in"  , r_in },
                    { "r_out" , r_out },
                    { "percentage", percentage },
                    { "time_remaining" , time_remaining }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["cap"].is_null()) { capacity = parsed["cap"].number_value(); }
                    if(!parsed["eff"].is_null()) { efficiency = parsed["eff"].number_value(); }
                    if(!parsed["charge"].is_null()) { charge = parsed["charge"].number_value(); }
                    if(!parsed["r_in"].is_null()) { r_in = parsed["r_in"].number_value(); }
                    if(!parsed["r_out"].is_null()) { r_out = parsed["r_out"].number_value(); }
                    if(!parsed["percentage"].is_null()) { percentage = parsed["percentage"].number_value(); }
                    if(!parsed["time_remaining"].is_null()) { time_remaining = parsed["time_remaining"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Heater Structure definition
        /*! This structure holds the description of a heaters.
*/
        struct htrstruc : public devicestruc
        {
            bool state = true;
            //! Temperature set vertex
            float setvertex = 0.f;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "state" , state },
                    { "setvertex" , setvertex }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["state"].is_null()) { state = parsed["state"].bool_value(); }
                    if(!parsed["setvertex"].is_null()) { setvertex = parsed["setvertex"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        // TODO: add struct description
        struct motrstruc : public devicestruc
        {
            //! Maximum speed in revolutions per second
            float max = 0.f;
            //!
            float rat = 0.f;
            float spd = 0.f;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "max" , max },
                    { "rat" , rat },
                    { "spd" , spd }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["max"].is_null()) { max = parsed["max"].number_value(); }
                    if(!parsed["rat"].is_null()) { rat = parsed["rat"].number_value(); }
                    if(!parsed["spd"].is_null()) { spd = parsed["spd"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        // TODO: add struct description
        struct tsenstruc : public devicestruc
        {
            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const { return json11::Json::object {}; }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {};
        };

        //! Thruster (THST) dynamic structure
        struct thststruc : public devicestruc
        {
            //! Flow
            quaternion align;
            //! Specific Impulse in dynes per kg per second
            float flw = 0.f;
            //! Rotation of thrust vector (+z) in to node frame.
            float isp = 0.f;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "align" , align },
                    { "flw"   , flw },
                    { "isp"   , isp }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["flw"].is_null()) { flw = parsed["flw"].number_value(); }
                    if(!parsed["isp"].is_null()) { isp = parsed["isp"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Propellant Tank (PROP) structure.
        struct propstruc : public devicestruc
        {
            //! Propellant capacity in kg
            float cap = 0.f;
            //! Propellant level in kg
            float lev = 0.f;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "cap" , cap },
                    { "lev" , lev }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["cap"].is_null()) { cap = parsed["cap"].number_value(); }
                    if(!parsed["lev"].is_null()) { lev = parsed["lev"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Switch Structure definition
        /*! This structure holds the description of a switches.
*/
        struct swchstruc : public devicestruc
        {
            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const { return json11::Json::object {}; }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {};
        };

        //! Rotor Structure definition
        /*! This structure holds the description of a rotors.
*/
        struct rotstruc : public devicestruc
        {
            //! Angular position
            float angle = 0.f;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "angle" , angle }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["angle"].is_null()) { angle = parsed["angle"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Star Tracker (STT) Sructure
        // TODO: rename to ST
        struct sttstruc : public devicestruc
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

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
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

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["att"].is_null()) { att.from_json(parsed["att"].dump()); }
                    if(!parsed["omega"].is_null()) { omega.from_json(parsed["omega"].dump()); }
                    if(!parsed["alpha"].is_null()) { alpha.from_json(parsed["alpha"].dump()); }
                    if(!parsed["retcode"].is_null()) { retcode = parsed["retcode"].int_value(); }
                    if(!parsed["status"].is_null()) { status = parsed["status"].int_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Motion Capture Camera (MCC) Structure
        struct mccstruc : public devicestruc
        {
            //! Sensor alignment quaternion.
            quaternion align;
            //! attitude
            quaternion q;
            rvector o;
            rvector a;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "align" , align },
                    { "q"	 , q },
                    { "o"	 , o },
                    { "a"	 , a }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["q"].is_null()) { q.from_json(parsed["q"].dump()); }
                    if(!parsed["o"].is_null()) { o.from_json(parsed["o"].dump()); }
                    if(!parsed["a"].is_null()) { a.from_json(parsed["a"].dump()); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! Torque Rod Control Unit
        struct tcustruc : public devicestruc
        {
            //! Torque Rod count
            uint16_t mcnt = 0;
            //! Torque Rod Component indices
            uint16_t mcidx[3] = {0};

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                vector<uint16_t> v_mcidx = vector<uint16_t>(mcidx, mcidx+sizeof(mcidx)/sizeof(mcidx[0]));
                return json11::Json::object {
                    { "mcnt" , mcnt },
                    { "mcidx", v_mcidx }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["mcnt"].is_null()) { mcnt = parsed["mcnt"].int_value(); }
                    if(!parsed["mcidx"].is_null()) {
                        auto p_mcidx = parsed["mcidx"].array_items();
                        for(size_t i = 0; i != p_mcidx.size(); ++i) {
                            if(!p_mcidx[i].is_null()) { mcidx[i] = p_mcidx[i].int_value(); }
                        }
                    }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        // TODO: add struct description
        struct busstruc : public devicestruc
        {
            //! Watch Dog Timer (MJD)
            float wdt = 0.f;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "wdt" , wdt }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["wdt"].is_null()) { wdt = parsed["wdt"].number_value();}
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        // TODO: add struct description
        struct psenstruc : public devicestruc
        {
            //! Current Pressure
            float press = 0.f;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "press" , press }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["press"].is_null()) { press = parsed["press"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! SUCHI Sructure
        struct suchistruc : public devicestruc
        {
            //! alignment quaternion
            quaternion align;
            //! Internal pressure
            float press = 0.f;
            //! Internal temperatures
            float temps[8] = {0.f};

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                vector<float> v_temps = vector<float>(temps, temps+sizeof(temps)/sizeof(temps[0]));
                return json11::Json::object {
                    { "align" , align },
                    { "press" , press },
                    { "temps" , v_temps }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["align"].is_null()) { align.from_json(parsed["align"].dump()); }
                    if(!parsed["press"].is_null()) { press = parsed["press"].number_value(); }
                    if(!parsed["temps"].is_null()) {
                        if(!parsed["press"].is_null()) { press = parsed["press"].number_value(); }
                        auto p_temps = parsed["temps"].array_items();
                        for(size_t i = 0; i != p_temps.size(); ++i) {
                            if(!p_temps[i].is_null()) { temps[i] = p_temps[i].number_value(); }
                        }
                    }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        // TODO: add struct description
        struct camstruc : public devicestruc
        {
            //! lens Step
            int16_t lstep;
            uint16_t pwidth = 0;
            uint16_t pheight = 0;
            float width = 0.f;
            float height = 0.f;
            float flength = 0.f;
            //! DN noise
            float noise = 0.f;
            //! lens temperature
            float ltemp = 0.f;
            //! Target temperature
            float ttemp = 0.f;
            //! Field of view
            float fov = 0.001;
            //! Instantaneous field of view
            float ifov = 0.00001;
            //! Minimum spectral wavelength
            float specmin = 300e-9;
            //! Maximum spectral wavelength
            float specmax = 1e-6;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "lstep" , lstep },
                    { "pwidth" , pwidth },
                    { "pheight", pheight },
                    { "width"  , width },
                    { "height" , height },
                    { "flength", flength },
                    { "ltemp", ltemp},
                    { "ttemp", ttemp},
                    { "fov", fov },
                    { "ifov", ifov },
                    { "specmin", specmin },
                    { "specmax", specmax }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["lstep"].is_null()) { lstep = parsed["lstep"].int_value(); }
                    if(!parsed["pwidth"].is_null()) { pwidth = parsed["pwidth"].int_value(); }
                    if(!parsed["pheight"].is_null()) { pheight = parsed["pheight"].int_value(); }
                    if(!parsed["width"].is_null()) { width = parsed["width"].number_value(); }
                    if(!parsed["height"].is_null()) { height = parsed["height"].number_value(); }
                    if(!parsed["flength"].is_null()) { flength = parsed["flength"].number_value(); }
                    if(!parsed["ltemp"].is_null()) { ltemp = parsed["ltemp"].number_value(); }
                    if(!parsed["ttemp"].is_null()) { ttemp = parsed["ttemp"].number_value(); }
                    if(!parsed["fov"].is_null()) { fov = parsed["fov"].number_value(); }
                    if(!parsed["ifov"].is_null()) { ifov = parsed["ifov"].number_value(); }
                    if(!parsed["specmin"].is_null()) { specmin = parsed["specmin"].number_value(); }
                    if(!parsed["specmax"].is_null()) { specmax = parsed["specmax"].number_value(); }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        //! TNC Structure definition
        /*! This structure holds the description of a TNC.
*/
        struct tncstruc : public devicestruc
        {
            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const { return json11::Json::object {}; }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {};
        };

        //! BCREG (PV Regulator) Structure definition
        /*! This structure holds the description of a BCREG.
*/
        struct bcregstruc : public devicestruc
        {
            //! MPPT Input Voltage
            float mpptin_volt;
            //! MPPT Input Current
            float mpptin_amp;
            //! MPPT Output Voltage
            float mpptout_volt;
            //! MPPT Output Current
            float mpptout_amp;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */

            json11::Json to_json() const {
                return json11::Json::object {
                    { "mpptin_volt", mpptin_volt },
                    { "mpptin_amp", mpptin_amp },
                    { "mpptout_volt", mpptout_volt },
                    { "mpptout_amp", mpptout_amp }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["mpptin_volt"].is_null()) { mpptin_volt = parsed["mpptin_volt"].number_value(); }
                    if(!parsed["mpptin_amp"].is_null()) { mpptin_amp = parsed["mpptin_amp"].number_value(); }
                    if(!parsed["mpptout_volt"].is_null()) { mpptout_volt = parsed["mpptout_volt"].number_value(); }
                    if(!parsed["mpptout_amp"].is_null()) { mpptout_amp = parsed["mpptout_amp"].number_value(); }
                }
                else
                {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        // End of Device Specific structures

        //! Finite Triangle Element
        //! Holds minimum information necessary to use smallest possible triangular element
        //! of a larger piece.
        class trianglestruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(trianglestruc);
                for (size_t i=0; i<triangleindex.size(); ++i)
                {
                    total += triangleindex[i].size() * sizeof(uint16_t);
                }
                return total;
            }

            void shrinkusage()
            {
                for (size_t i=0; i<triangleindex.size(); ++i)
                {
                    vector<uint16_t>(triangleindex[i]).swap(triangleindex[i]);
                }
                vector<vector<uint16_t>>(triangleindex).swap(triangleindex);
            }

            //! External facing sides
            uint8_t external = 1;
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
            float emi = .88f;
            //! Absorptivity: 0-1
            float abs = .18f;
            //! Internal Emissivity: 0-1
            float iemi = .88f;
            //! Internal Absorptivity: 0-1
            float iabs = .88f;
            //! mass in Kg
            float mass = 1.f;
            //! density in kg/m3
            float density =  2710.;
            //! Temperature in Kelvin
            float temp = 0.f;
            //! Area
            float area = 0.f;
            //! Depth
            float depth = 0.f;
            //! perimeter
            float perimeter = 0.f;
            //! Irradiation from Sun in Watts/sq m
            float sirradiation = 0.f;
            //! Irradiation from Earth in Watts/sq m
            float eirradiation = 0.f;
            //! Solar cell coverage
            float pcell = 0.f;
            //! Solar cell base efficiency
            float ecellbase = .25f;
            //! Solar cell efficiency with temp
            float ecellslope = 0.f;
            //! Nominal voltage of Solar cell
            float vcell = 10.;
            //! Power generated in watts
            float power = 0.f;
            //! Voltage generated in volts
            float volt = 0.f;
            //! Current generated in amps
            float amp = 0.f;
            vector<vector<uint16_t>> triangleindex;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
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
                    { "iemi"   , iemi },
                    { "iabs"   , iabs },
                    { "mass"  , mass },
                    { "density"  , density },
                    { "temp"  , temp },
                    { "area"  , area },
                    { "depth" , depth },
                    { "perimeter"   , perimeter },
                    { "sirradiation" , sirradiation },
                    { "eirradiation" , eirradiation },
                    { "pcell" , pcell },
                    { "ecellbase"   , ecellbase },
                    { "ecellslope"  , ecellslope },
                    { "triangleindex" , triangleindex }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json parsed = json11::Json::parse(s,error);
                if(error.empty()) {
                    if(!parsed["external"].is_null())	{ external = parsed["external"].int_value(); }
                    if(!parsed["com"].is_null())	{ com.from_json(parsed["com"].dump()); }
                    if(!parsed["normal"].is_null())	{ normal.from_json(parsed["normal"].dump()); }
                    if(!parsed["shove"].is_null())	{ shove.from_json(parsed["shove"].dump()); }
                    if(!parsed["twist"].is_null())	{ twist.from_json(parsed["twist"].dump()); }
                    if(!parsed["pidx"].is_null())	{ pidx = parsed["pidx"].int_value(); }

                    // Array
                    if(!parsed["heat"].is_null())	{ heat = parsed["heat"].number_value(); }
                    if(!parsed["hcap"].is_null())	{ hcap = parsed["hcap"].number_value(); }
                    if(!parsed["emi"].is_null())	{ emi = parsed["emi"].number_value(); }
                    if(!parsed["abs"].is_null())	{ abs = parsed["abs"].number_value(); }
                    if(!parsed["iemi"].is_null())	{ emi = parsed["iemi"].number_value(); }
                    if(!parsed["iabs"].is_null())	{ abs = parsed["iabs"].number_value(); }
                    if(!parsed["mass"].is_null())	{ mass = parsed["mass"].number_value(); }
                    if(!parsed["density"].is_null())	{ density = parsed["density"].number_value(); }
                    if(!parsed["temp"].is_null())	{ temp = parsed["temp"].number_value(); }
                    if(!parsed["area"].is_null())	{ area = parsed["area"].number_value(); }
                    if(!parsed["perimeter"].is_null())	{ perimeter = parsed["perimeter"].number_value(); }
                    if(!parsed["sirradiation"].is_null())	{ sirradiation = parsed["sirradiation"].number_value(); }
                    if(!parsed["eirradiation"].is_null())	{ eirradiation = parsed["eirradiation"].number_value(); }
                    if(!parsed["pcell"].is_null())	{ pcell = parsed["pcell"].number_value(); }
                    if(!parsed["ecellbase"].is_null())	{ ecellbase = parsed["ecellbase"].number_value(); }
                    if(!parsed["ecellslope"].is_null())	{ ecellslope = parsed["ecellslope"].number_value(); }
                    for(size_t i = 0; i < triangleindex.size(); ++i) {
                        for(size_t j = 0; j < triangleindex[i].size(); ++j)  {
                            if(!parsed["triangleindex"][i][j].is_null()) { triangleindex[i][j] = parsed["triangleindex"][i][j].number_value(); }
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
        class physicsstruc
        {
        public:
            //! Time step in seconds
            size_t memoryusage()
            {
                size_t total = 5 * sizeof(double) + 10 * sizeof(float) + 13 * sizeof(Vector);
                total += vertices.size() * sizeof(Vector);
                return total;
            }

            void shrinkusage()
            {
                vector<Vector>(vertices).swap(vertices);
                for (size_t i=0; i<triangles.size(); ++i)
                {
                    triangles[i].shrinkusage();
                }
                vector<trianglestruc>(triangles).swap(triangles);
            }

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
            float temp = 600.f;
            float heat = 600. * 900. * 1.;
            float radiation;
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
            Vector fpush;
            Vector moi = Vector(1.,1.,1.);
            Vector com;

            vector <Vector> vertices;
            vector <trianglestruc> triangles;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
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
                    { "radiation" , radiation },
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

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& js) {
                string error;
                json11::Json parsed = json11::Json::parse(js,error);
                if(error.empty()) {
                    if(!parsed["dt"].is_null())	{ dt = parsed["dt"].number_value(); }
                    if(!parsed["dtj"].is_null())	{ dtj = parsed["dtj"].number_value(); }
                    if(!parsed["utc"].is_null())	{ utc = parsed["utc"].number_value(); }
                    if(!parsed["mjdaccel"].is_null())	{ mjdaccel = parsed["mjdaccel"].number_value(); }
                    if(!parsed["mjddiff"].is_null())	{ mjddiff = parsed["mjddiff"].number_value(); }
                    if(!parsed["hcap"].is_null())	{ hcap = parsed["hcap"].number_value(); }
                    if(!parsed["mass"].is_null())	{ mass = parsed["mass"].number_value(); }
                    if(!parsed["temp"].is_null())	{ temp = parsed["temp"].number_value(); }
                    if(!parsed["heat"].is_null())	{ heat = parsed["heat"].number_value(); }
                    if(!parsed["radiation"].is_null())	{ radiation = parsed["radiation"].number_value(); }
                    if(!parsed["area"].is_null())	{ area = parsed["area"].number_value(); }
                    if(!parsed["battcap"].is_null())	{ battcap = parsed["battcap"].number_value(); }
                    if(!parsed["battlev"].is_null())	{ battlev = parsed["battlev"].number_value(); }
                    if(!parsed["powgen"].is_null())	{ powgen = parsed["powgen"].number_value(); }
                    if(!parsed["powuse"].is_null())	{ powuse = parsed["powuse"].number_value(); }
                    if(!parsed["mode"].is_null())	{ mode = parsed["mode"].int_value(); }

                    if(!parsed["ftorque"].is_null())	{ ftorque.from_json(parsed["ftorque"].dump()); }
                    if(!parsed["atorque"].is_null())	{ atorque.from_json(parsed["atorque"].dump()); }
                    if(!parsed["rtorque"].is_null())	{ rtorque.from_json(parsed["rtorque"].dump()); }
                    if(!parsed["gtorque"].is_null())	{ gtorque.from_json(parsed["gtorque"].dump()); }
                    if(!parsed["htorque"].is_null())	{ htorque.from_json(parsed["htorque"].dump()); }
                    if(!parsed["hmomentum"].is_null())	{ hmomentum.from_json(parsed["hmomentum"].dump()); }
                    if(!parsed["ctorque"].is_null())	{ ctorque.from_json(parsed["ctorque"].dump()); }
                    if(!parsed["fdrag"].is_null())	{ fdrag.from_json(parsed["fdrag"].dump()); }
                    if(!parsed["adrag"].is_null())	{ adrag.from_json(parsed["adrag"].dump()); }
                    if(!parsed["rdrag"].is_null())	{ rdrag.from_json(parsed["rdrag"].dump()); }
                    if(!parsed["thrust"].is_null())	{ thrust.from_json(parsed["thrust"].dump()); }
                    if(!parsed["moi"].is_null())	{ moi.from_json(parsed["moi"].dump()); }
                    if(!parsed["com"].is_null())	{ com.from_json(parsed["com"].dump()); }
                    for(size_t i = 0; i < vertices.size(); ++i)	{
                        if(!parsed["vertices"][i].is_null())	{ vertices[i].from_json(parsed["vertices"][i].dump()); }
                    }
                    for(size_t i = 0; i < triangles.size(); ++i)	{
                        if(!parsed["triangles"][i].is_null())	{ triangles[i].from_json(parsed["triangles"][i].dump()); }
                    }
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }


        };

        typedef uint8_t NODE_ID_TYPE;
        static constexpr uint8_t NODEIDUNKNOWN = 0;
        static constexpr uint8_t NODEIDORIG = 254;
        static constexpr uint8_t NODEIDDEST = 255;

        //! \brief Realm Structure
        //! Structure for storing all information shared by Nodes within a Realm.
        class realmstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(realmstruc);
                total += name.capacity();
                total += node_ids.size() * 4;
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
                map<string, uint8_t>(node_ids).swap(node_ids);
            }

            string name;
            map<string, NODE_ID_TYPE> node_ids = {{"unkown", NODEIDUNKNOWN}, {"origin", NODEIDORIG}, {"destination", NODEIDDEST}};
        };

        //! \brief Node Structure
        //! Structure for storing all information about a Node that never changes, or only
        //! changes slowly. The information for initializing this should be in node.ini.
        class nodestruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(nodestruc);
                total += name.capacity();
//                total += agent.capacity();
                total += lastevent.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
//                string(agent).swap(agent);
                string(lastevent).swap(lastevent);
                phys.shrinkusage();
            }

            //! Node Name.
            //    char name[COSMOS_MAX_NAME+1] = "";
            string name="";
//            //! Agent Name.
//            //    char agent[COSMOS_MAX_NAME+1] = "";
//            string agent="";
            //! Last event
            //	char lastevent[COSMOS_MAX_NAME+1] = "";
            string lastevent="";
            //! Last event UTC
            double lasteventutc = 0.;
            //! Node Type as listed in \ref NODE_TYPE.
            uint16_t type = 0;
            //! Operational state
            uint16_t state = 0;

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
            //! Mission Deci Seconds
            uint32_t deci = 0;
            //! Location structure
            Convert::locstruc loc;
			//! Estimated location structure
            Convert::locstruc loc_est;
			//! Standard Deviation for location estimates
            Convert::locstruc loc_std;
            physicsstruc phys;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "name" , name },
//                    { "agent" , agent },
                    { "lastevent" , lastevent },
                    { "lasteventutc" , lasteventutc },
                    { "type" , type },
                    { "state" , state },

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
                    { "deci" , static_cast<int>(deci) },
                    { "loc" , loc },
					{ "loc_est", loc_est },
					{ "loc_std", loc_std },
                    { "phys" , phys }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& js) {
                string error;
                json11::Json parsed = json11::Json::parse(js,error);
                if(error.empty()) {
                    //            if(!parsed["name"].is_null())			{ strcpy(name, parsed["name"].string_value().c_str()); }
                    //            if(!parsed["agent"].is_null())			{ strcpy(agent, parsed["agent"].string_value().c_str()); }
                    //			if(!parsed["lastevent"].is_null())		{ strcpy(lastevent, parsed["lastevent"].string_value().c_str()); }
                    if(!parsed["name"].is_null())			{ name = parsed["name"].string_value(); }
//                    if(!parsed["agent"].is_null())			{ agent = parsed["agent"].string_value(); }
                    if(!parsed["lastevent"].is_null())		{ lastevent = parsed["lastevent"].string_value(); }
                    if(!parsed["lasteventutc"].is_null())	{ lasteventutc = parsed["lasteventutc"].number_value(); }
                    if(!parsed["type"].is_null())	{ type = parsed["type"].int_value(); }
                    if(!parsed["state"].is_null())	{ state = parsed["state"].int_value(); }

                    if(!parsed["flags"].is_null())	{ flags = parsed["flags"].int_value(); }
                    if(!parsed["powmode"].is_null())	{ powmode = parsed["powmode"].int_value(); }
                    if(!parsed["downtime"].is_null())	{ downtime = parsed["downtime"].int_value(); }
                    if(!parsed["azfrom"].is_null())	{ azfrom = parsed["azfrom"].number_value(); }
                    if(!parsed["elfrom"].is_null())	{ elfrom = parsed["elfrom"].number_value(); }
                    if(!parsed["azto"].is_null())	{ azto = parsed["azto"].number_value(); }
                    if(!parsed["elto"].is_null())	{ elto = parsed["elto"].number_value(); }
                    if(!parsed["range"].is_null())	{ range = parsed["range"].number_value(); }
                    if(!parsed["utcoffset"].is_null())	{ utcoffset = parsed["utcoffset"].number_value(); }
                    if(!parsed["utc"].is_null())	{ utc = parsed["utc"].number_value(); }
                    if(!parsed["utcstart"].is_null())	{ utcstart = parsed["utcstart"].number_value(); }
                    if(!parsed["deci"].is_null())	{ deci = parsed["deci"].int_value(); }
                    if(!parsed["loc"].is_null())	{ loc.from_json(parsed["loc"].dump()); }
					if(!parsed["loc_est"].is_null()){ loc_est.from_json(parsed["loc_est"].dump()); }
					if(!parsed["loc_std"].is_null()){ loc_std.from_json(parsed["loc_std"].dump()); }
                    if(!parsed["phys"].is_null())	{ phys.from_json(parsed["phys"].dump()); }
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        //! Specific Device structure
        /*! Counts and arrays of pointers to each type of device, ordered by type.
*/
        class devspecstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(devspecstruc);
                total += ant.capacity() * sizeof(antstruc);
                total += batt.capacity() * sizeof(battstruc);
                total += bcreg.capacity() * sizeof(bcregstruc);
                total += bus.capacity() * sizeof(busstruc);
                total += cam.capacity() * sizeof(camstruc);
                total += cpu.capacity() * sizeof(cpustruc);
                total += disk.capacity() * sizeof(diskstruc);
                total += gps.capacity() * sizeof(gpsstruc);
                total += gyro.capacity() * sizeof(gyrostruc);
                total += htr.capacity() * sizeof(htrstruc);
                total += imu.capacity() * sizeof(imustruc);
                total += mag.capacity() * sizeof(magstruc);
                total += mcc.capacity() * sizeof(mccstruc);
                total += motr.capacity() * sizeof(motrstruc);
                total += mtr.capacity() * sizeof(mtrstruc);
                total += pload.capacity() * sizeof(ploadstruc);
                total += prop.capacity() * sizeof(propstruc);
                total += psen.capacity() * sizeof(psenstruc);
                total += pvstrg.capacity() * sizeof(pvstrgstruc);
                total += rot.capacity() * sizeof(rotstruc);
                total += rw.capacity() * sizeof(rwstruc);
                total += rxr.capacity() * sizeof(rxrstruc);
                total += ssen.capacity() * sizeof(ssenstruc);
                total += stt.capacity() * sizeof(sttstruc);
                total += suchi.capacity() * sizeof(suchistruc);
                total += swch.capacity() * sizeof(swchstruc);
                total += tcu.capacity() * sizeof(tcustruc);
                total += tcv.capacity() * sizeof(tcvstruc);
                total += telem.capacity() * sizeof(telemstruc);
                total += thst.capacity() * sizeof(thststruc);
                total += tnc.capacity() * sizeof(tncstruc);
                total += tsen.capacity() * sizeof(tsenstruc);
                total += txr.capacity() * sizeof(tcvstruc);
                total += xyzsen.capacity() * sizeof(xyzsenstruc);
                return total;
            }

            void shrinkusage()
            {
                vector<antstruc>(ant).swap(ant);
                vector<battstruc>(batt).swap(batt);
                vector<bcregstruc>(bcreg).swap(bcreg);
                vector<busstruc>(bus).swap(bus);
                vector<camstruc>(cam).swap(cam);
                vector<cpustruc>(cpu).swap(cpu);
                vector<diskstruc>(disk).swap(disk);
                vector<gpsstruc>(gps).swap(gps);
                vector<gyrostruc>(gyro).swap(gyro);
                vector<htrstruc>(htr).swap(htr);
                vector<imustruc>(imu).swap(imu);
                vector<magstruc>(mag).swap(mag);
                vector<mccstruc>(mcc).swap(mcc);
                vector<motrstruc>(motr).swap(motr);
                vector<mtrstruc>(mtr).swap(mtr);
                vector<ploadstruc>(pload).swap(pload);
                vector<propstruc>(prop).swap(prop);
                vector<psenstruc>(psen).swap(psen);
                vector<pvstrgstruc>(pvstrg).swap(pvstrg);
                vector<rotstruc>(rot).swap(rot);
                vector<rwstruc>(rw).swap(rw);
                vector<rxrstruc>(rxr).swap(rxr);
                vector<ssenstruc>(ssen).swap(ssen);
                vector<sttstruc>(stt).swap(stt);
                vector<suchistruc>(suchi).swap(suchi);
                vector<swchstruc>(swch).swap(swch);
                vector<tcustruc>(tcu).swap(tcu);
                vector<tcvstruc>(tcv).swap(tcv);
                vector<telemstruc>(telem).swap(telem);
                vector<thststruc>(thst).swap(thst);
                vector<tncstruc>(tnc).swap(tnc);
                vector<tsenstruc>(tsen).swap(tsen);
                vector<txrstruc>(txr).swap(txr);
                vector<xyzsenstruc>(xyzsen).swap(xyzsen);
            }

            //            uint16_t all_cnt = 0;
            uint16_t ant_cnt = 0;
            uint16_t batt_cnt = 0;
            uint16_t bus_cnt = 0;
            uint16_t cam_cnt = 0;
            uint16_t cpu_cnt = 0;
            uint16_t disk_cnt = 0;
            uint16_t gps_cnt = 0;
            uint16_t gyro_cnt = 0;
            uint16_t htr_cnt = 0;
            uint16_t imu_cnt = 0;
            uint16_t mag_cnt = 0;
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
            uint16_t xyzsen_cnt = 0;

            vector<antstruc>ant;
            vector<battstruc>batt;
            vector<bcregstruc>bcreg;
            vector<busstruc>bus;
            vector<camstruc>cam;
            vector<cpustruc>cpu;
            vector<diskstruc>disk;
            vector<gpsstruc>gps;
            vector<gyrostruc>gyro;
            vector<htrstruc>htr;
            vector<imustruc>imu;
            vector<magstruc>mag;
            vector<mccstruc>mcc;
            vector<motrstruc>motr;
            vector<mtrstruc>mtr;
            vector<ploadstruc>pload;
            vector<propstruc>prop;
            vector<psenstruc>psen;
            vector<pvstrgstruc>pvstrg;
            vector<rotstruc>rot;
            vector<rwstruc>rw;
            vector<rxrstruc>rxr;
            vector<ssenstruc>ssen;
            vector<sttstruc>stt;
            vector<suchistruc>suchi;
            vector<swchstruc>swch;
            vector<tcustruc>tcu;
            vector<tcvstruc>tcv;
            vector<telemstruc>telem;
            vector<thststruc>thst;
            vector<tncstruc>tnc;
            vector<tsenstruc>tsen;
            vector<txrstruc>txr;
            vector<xyzsenstruc>xyzsen;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    //                    { "all_cnt", all_cnt },
                    { "ant_cnt", ant_cnt },
                    { "batt_cnt", batt_cnt },
                    { "bus_cnt", bus_cnt },
                    { "cam_cnt", cam_cnt },
                    { "cpu_cnt", cpu_cnt },
                    { "disk_cnt", disk_cnt },
                    { "gps_cnt", gps_cnt },
                    { "gyro_cnt", gyro_cnt },
                    { "htr_cnt", htr_cnt },
                    { "imu_cnt", imu_cnt },
                    { "mag_cnt", mag_cnt },
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
                    { "xyzsen_cnt", xyzsen_cnt },
                    //                    { "all", all },
                    { "ant", ant },
                    { "batt", batt },
                    { "bcreg", bcreg },
                    { "bus", bus },
                    { "cam", cam },
                    { "cpu", cpu },
                    { "disk", disk },
                    { "gps", gps },
                    { "gyro", gyro },
                    { "htr", htr },
                    { "imu", imu },
                    { "mag", mag },
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
                    { "txr", txr },
                    { "xyzsen", xyzsen },
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    //                    if(!p["all_cnt"].is_null()) { all_cnt = p["all_cnt"].int_value(); }
                    if(!p["ant_cnt"].is_null()) { ant_cnt = p["ant_cnt"].int_value(); }
                    if(!p["batt_cnt"].is_null()) { batt_cnt = p["batt_cnt"].int_value(); }
                    if(!p["bus_cnt"].is_null()) { bus_cnt = p["bus_cnt"].int_value(); }
                    if(!p["cam_cnt"].is_null()) { cam_cnt = p["cam_cnt"].int_value(); }

                    if(!p["cpu_cnt"].is_null()) { cpu_cnt = p["cpu_cnt"].int_value(); }
                    if(!p["disk_cnt"].is_null()) { disk_cnt = p["disk_cnt"].int_value(); }
                    if(!p["gps_cnt"].is_null()) { gps_cnt = p["gps_cnt"].int_value(); }
                    if(!p["gyro_cnt"].is_null()) { gyro_cnt = p["gyro_cnt"].int_value(); }
                    if(!p["htr_cnt"].is_null()) { htr_cnt = p["htr_cnt"].int_value(); }
                    if(!p["imu_cnt"].is_null()) { imu_cnt = p["imu_cnt"].int_value(); }
                    if(!p["mag_cnt"].is_null()) { mag_cnt = p["mag_cnt"].int_value(); }

                    if(!p["mcc_cnt"].is_null()) { mcc_cnt = p["mcc_cnt"].int_value(); }
                    if(!p["motr_cnt"].is_null()) { motr_cnt = p["motr_cnt"].int_value(); }
                    if(!p["mtr_cnt"].is_null()) { mtr_cnt = p["mtr_cnt"].int_value(); }
                    if(!p["pload_cnt"].is_null()) { pload_cnt = p["pload_cnt"].int_value(); }
                    if(!p["prop_cnt"].is_null()) { prop_cnt = p["prop_cnt"].int_value(); }

                    if(!p["psen_cnt"].is_null()) { psen_cnt = p["psen_cnt"].int_value(); }
                    if(!p["bcreg_cnt"].is_null()) { bcreg_cnt = p["bcreg_cnt"].int_value(); }
                    if(!p["rot_cnt"].is_null()) { rot_cnt = p["rot_cnt"].int_value(); }
                    if(!p["rw_cnt"].is_null()) { rw_cnt = p["rw_cnt"].int_value(); }
                    if(!p["rxr_cnt"].is_null()) { rxr_cnt = p["rxr_cnt"].int_value(); }

                    if(!p["ssen_cnt"].is_null()) { ssen_cnt = p["ssen_cnt"].int_value(); }
                    if(!p["pvstrg_cnt"].is_null()) { pvstrg_cnt = p["pvstrg_cnt"].int_value(); }
                    if(!p["stt_cnt"].is_null()) { stt_cnt = p["stt_cnt"].int_value(); }
                    if(!p["suchi_cnt"].is_null()) { suchi_cnt = p["suchi_cnt"].int_value(); }
                    if(!p["swch_cnt"].is_null()) { swch_cnt = p["swch_cnt"].int_value(); }

                    if(!p["tcu_cnt"].is_null()) { tcu_cnt = p["tcu_cnt"].int_value(); }
                    if(!p["tcv_cnt"].is_null()) { tcv_cnt = p["tcv_cnt"].int_value(); }
                    if(!p["telem_cnt"].is_null()) { telem_cnt = p["telem_cnt"].int_value(); }
                    if(!p["thst_cnt"].is_null()) { thst_cnt = p["thst_cnt"].int_value(); }
                    if(!p["tsen_cnt"].is_null()) { tsen_cnt = p["tsen_cnt"].int_value(); }

                    if(!p["tnc_cnt"].is_null()) { tnc_cnt = p["tnc_cnt"].int_value(); }
                    if(!p["txr_cnt"].is_null()) { txr_cnt = p["txr_cnt"].int_value(); }
                    if(!p["xyzsen_cnt"].is_null()) { xyzsen_cnt = p["xyzsen_cnt"].int_value(); }

                    //                    for(size_t i = 0; i < all.size(); ++i) {
                    //                        if(!p["all"][i].is_null()) { all[i] = p["all"][i].int_value(); }
                    //                    }
                    for(size_t i = 0; i < ant.size(); ++i) {
                        //                        if(!p["ant"][i].is_null()) { ant[i] = p["ant"][i].int_value(); }
                        if(!p["ant"][i].is_null()) { ant[i].from_json(p["ant"][i].dump()); }
                    }
                    for(size_t i = 0; i < batt.size(); ++i) {
                        if(!p["batt"][i].is_null()) { batt[i].from_json(p["batt"][i].dump()); }
                    }
                    for(size_t i = 0; i < bcreg.size(); ++i) {
                        if(!p["bcreg"][i].is_null()) { bcreg[i].from_json(p["bcreg"][i].dump()); }
                    }
                    for(size_t i = 0; i < bus.size(); ++i) {
                        if(!p["bus"][i].is_null()) { bus[i].from_json(p["bus"][i].dump()); }
                    }

                    for(size_t i = 0; i < cam.size(); ++i) {
                        if(!p["cam"][i].is_null()) { cam[i].from_json(p["cam"][i].dump()); }
                    }
                    for(size_t i = 0; i < cpu.size(); ++i) {
                        if(!p["cpu"][i].is_null()) { cpu[i].from_json(p["cpu"][i].dump()); }
                    }
                    for(size_t i = 0; i < disk.size(); ++i) {
                        if(!p["disk"][i].is_null()) { disk[i].from_json(p["disk"][i].dump()); }
                    }
                    for(size_t i = 0; i < gps.size(); ++i) {
                        if(!p["gps"][i].is_null()) { gps[i].from_json(p["gps"][i].dump()); }
                    }
                    for(size_t i = 0; i < gyro.size(); ++i) {
                        if(!p["gyro"][i].is_null()) { gyro[i].from_json(p["gyro"][i].dump()); }
                    }
                    for(size_t i = 0; i < htr.size(); ++i) {
                        if(!p["htr"][i].is_null()) { htr[i].from_json(p["htr"][i].dump()); }
                    }

                    for(size_t i = 0; i < imu.size(); ++i) {
                        if(!p["imu"][i].is_null()) { imu[i].from_json(p["imu"][i].dump()); }
                    }
                    for(size_t i = 0; i < mag.size(); ++i) {
                        if(!p["mag"][i].is_null()) { mag[i].from_json(p["mag"][i].dump()); }
                    }
                    for(size_t i = 0; i < mcc.size(); ++i) {
                        if(!p["mcc"][i].is_null()) { mcc[i].from_json(p["mcc"][i].dump()); }
                    }
                    for(size_t i = 0; i < motr.size(); ++i) {
                        if(!p["motr"][i].is_null()) { motr[i].from_json(p["motr"][i].dump()); }
                    }
                    for(size_t i = 0; i < mtr.size(); ++i) {
                        if(!p["mtr"][i].is_null()) { mtr[i].from_json(p["mtr"][i].dump()); }
                    }
                    for(size_t i = 0; i < pload.size(); ++i) {
                        if(!p["pload"][i].is_null()) { pload[i].from_json(p["pload"][i].dump()); }
                    }

                    for(size_t i = 0; i < prop.size(); ++i) {
                        if(!p["prop"][i].is_null()) { prop[i].from_json(p["prop"][i].dump()); }
                    }
                    for(size_t i = 0; i < psen.size(); ++i) {
                        if(!p["psen"][i].is_null()) { psen[i].from_json(p["psen"][i].dump()); }
                    }
                    for(size_t i = 0; i < pvstrg.size(); ++i) {
                        if(!p["pvstrg"][i].is_null()) { pvstrg[i].from_json(p["pvstrg"][i].dump()); }
                    }
                    for(size_t i = 0; i < rot.size(); ++i) {
                        if(!p["rot"][i].is_null()) { rot[i].from_json(p["rot"][i].dump()); }
                    }
                    for(size_t i = 0; i < rw.size(); ++i) {
                        if(!p["rw"][i].is_null()) { rw[i].from_json(p["rw"][i].dump()); }
                    }

                    for(size_t i = 0; i < rxr.size(); ++i) {
                        if(!p["rxr"][i].is_null()) { rxr[i].from_json(p["rxr"][i].dump()); }
                    }
                    for(size_t i = 0; i < ssen.size(); ++i) {
                        if(!p["ssen"][i].is_null()) { ssen[i].from_json(p["ssen"][i].dump()); }
                    }
                    for(size_t i = 0; i < stt.size(); ++i) {
                        if(!p["stt"][i].is_null()) { stt[i].from_json(p["stt"][i].dump()); }
                    }
                    for(size_t i = 0; i < suchi.size(); ++i) {
                        if(!p["suchi"][i].is_null()) { suchi[i].from_json(p["suchi"][i].dump()); }
                    }
                    for(size_t i = 0; i < swch.size(); ++i) {
                        if(!p["swch"][i].is_null()) { swch[i].from_json(p["swch"][i].dump()); }
                    }

                    for(size_t i = 0; i < tcu.size(); ++i) {
                        if(!p["tcu"][i].is_null()) { tcu[i].from_json(p["tcu"][i].dump()); }
                    }
                    for(size_t i = 0; i < tcv.size(); ++i) {
                        if(!p["tcv"][i].is_null()) { tcv[i].from_json(p["tcv"][i].dump()); }
                    }
                    for(size_t i = 0; i < telem.size(); ++i) {
                        if(!p["telem"][i].is_null()) { telem[i].from_json(p["telem"][i].dump()); }
                    }
                    for(size_t i = 0; i < thst.size(); ++i) {
                        if(!p["thst"][i].is_null()) { thst[i].from_json(p["thst"][i].dump()); }
                    }
                    for(size_t i = 0; i < tnc.size(); ++i) {
                        if(!p["tnc"][i].is_null()) { tnc[i].from_json(p["tnc"][i].dump()); }
                    }

                    for(size_t i = 0; i < tsen.size(); ++i) {
                        if(!p["tsen"][i].is_null()) { tsen[i].from_json(p["tsen"][i].dump()); }
                    }
                    for(size_t i = 0; i < txr.size(); ++i) {
                        if(!p["txr"][i].is_null()) { txr[i].from_json(p["txr"][i].dump()); }
                    }
                    for(size_t i = 0; i < xyzsen.size(); ++i) {
                        if(!p["xyzsen"][i].is_null()) { xyzsen[i].from_json(p["xyzsen"][i].dump()); }
                    }
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
        class jsonentry
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(jsonentry);
                total += name.capacity();
                total += data.capacity();
                return total;
            }

            void shrinkusage()
            {
                string(name).swap(name);
            }

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

        //void replace(string& str, const string& from, const string& to);
        //vector<size_t> find_newlines(const string& sample);
        //void pretty_form(string& js);

        // TODO: add struct description in Doxygen
        class cosmosstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(cosmosstruc);
                for (size_t i=0; i<jmap.size(); ++i)
                {
                    for (size_t j=0; j<jmap[i].size(); ++j)
                    {
                        total += jmap[i][j].memoryusage();
                    }
                }
                for (auto &entry : ujmap)
                {
                    total += entry.second.memoryusage();
                }
                for (size_t i=0; i<emap.size(); ++i)
                {
                    for (size_t j=0; j<emap[i].size(); ++j)
                    {
                        total += emap[i][j].memoryusage();
                    }
                }
                for (size_t i=0; i<unit.size(); ++i)
                {
                    for (size_t j=0; j<unit[i].size(); ++j)
                    {
                        total += unit[i][j].memoryusage();
                    }
                }
                for (size_t i=0; i<equation.size(); ++i)
                {
                    total += equation[i].memoryusage();
                }
                for (size_t i=0; i<alias.size(); ++i)
                {
                    total += alias[i].memoryusage();
                }
                total += node.memoryusage();
                for (size_t i=0; i<vertexs.size(); ++i)
                {
                    total += sizeof(vertexstruc);
                }
                for (size_t i=0; i<normals.size(); ++i)
                {
                    total += sizeof(vertexstruc);
                }
                for (size_t i=0; i<faces.size(); ++i)
                {
                    total += faces[i].memoryusage();
                }
                for (size_t i=0; i<pieces.size(); ++i)
                {
                    total += pieces[i].memoryusage();
                }
                total += sizeof(obj);
                for (size_t i=0; i<device.size(); ++i)
                {
                    total += sizeof(device[i]);
                }
                total += devspec.memoryusage();
                for (size_t i=0; i<port.size(); ++i)
                {
                    total += port[i].memoryusage();
                }
//                for (size_t i=0; i<agent.size(); ++i)
//                {
//                    total += agent[i].memoryusage();
//                }
                total += agent0.memoryusage();
                for (size_t i=0; i<sim_states.size(); ++i)
                {
                    total += sizeof(sim_states[i]);
                }
                for (size_t i=0; i<event.size(); ++i)
                {
                    total += event[i].memoryusage();
                }
                for (size_t i=0; i<target.size(); ++i)
                {
                    total += target[i].memoryusage();
                }
                for (size_t i=0; i<user.size(); ++i)
                {
                    total += user[i].memoryusage();
                }
                for (size_t i=0; i<tle.size(); ++i)
                {
                    total += sizeof(tle[i]);
                }

                return total;
            }

            void shrinkusage()
            {
                for (size_t i=0; i<jmap.size(); ++i)
                {
                    for (size_t j=0; j<jmap[i].size(); ++j)
                    {
                        jmap[i][j].shrinkusage();
                    }
                }
                for (auto &entry : ujmap)
                {
                    entry.second.shrinkusage();
                }
                for (size_t i=0; i<emap.size(); ++i)
                {
                    for (size_t j=0; j<emap[i].size(); ++j)
                    {
                        emap[i][j].shrinkusage();
                    }
                }
                for (size_t i=0; i<unit.size(); ++i)
                {
                    for (size_t j=0; j<unit[i].size(); ++j)
                    {
                        unit[i][j].shrinkusage();
                    }
                }
                for (size_t i=0; i<equation.size(); ++i)
                {
                    equation[i].shrinkusage();
                }
                for (size_t i=0; i<alias.size(); ++i)
                {
                    alias[i].shrinkusage();
                }
                node.shrinkusage();
                for (size_t i=0; i<faces.size(); ++i)
                {
                    faces[i].shrinkusage();
                }
                for (size_t i=0; i<pieces.size(); ++i)
                {
                    pieces[i].shrinkusage();
                }
                devspec.shrinkusage();
                for (size_t i=0; i<port.size(); ++i)
                {
                    port[i].shrinkusage();
                }
//                for (size_t i=0; i<agent.size(); ++i)
//                {
//                    agent[i].shrinkusage();
//                }
                agent0.shrinkusage();
                for (size_t i=0; i<event.size(); ++i)
                {
                    event[i].shrinkusage();
                }
                for (size_t i=0; i<target.size(); ++i)
                {
                    target[i].shrinkusage();
                }
                for (size_t i=0; i<user.size(); ++i)
                {
                    user[i].shrinkusage();
                }
                name_map(names).swap(names);
                type_map(types).swap(types);
//                for (auto &name : names)
//                {
//                    name_mapping(name).swap(name);
//                }
//                for (auto &type : types)
//                {
//                    type_mapping(type).swap(type);
//                }
            }

            cosmosstruc();

            //! Timestamp for last change to data
            double timestamp = 0.;

            //! Whether JSON map has been created.
            uint16_t jmapped = 0;
            uint16_t ujmapped = 0;

            //! JSON Namespace Map matrix. first entry hash, second is items with that hash
            vector<vector<jsonentry> > jmap; // depricate me!
            std::unordered_map<string, jsonentry> ujmap;

            //! JSON Equation Map matrix.
            vector<vector<jsonequation> > emap; // depricate me?

            //! JSON Unit Map matrix: first level is for unit type, second level is for all variants (starting with primary).
            vector<vector<unitstruc> > unit;

            //! Vector of Equations
            vector<equationstruc> equation;

            //! Array of Aliases
            vector<aliasstruc> alias;

            //! Structure for summary information in realm
            realmstruc realm;

            //! Structure for summary information in node
            nodestruc node;

            //! Vector of all vertexs in node.
            vector <vertexstruc> vertexs;
            uint16_t vertex_cnt = 0;

            //! Vector of all vertexs in node.
            vector <vertexstruc> normals;
            uint16_t normal_cnt = 0;

            //! Vector of all faces in node.
            vector <facestruc> faces;
            uint16_t face_cnt = 0;

            //! Vector of all pieces in node.
            vector<piecestruc> pieces;
            uint16_t piece_cnt = 0;

            //! Wavefront obj structure
            wavefront obj;

            //! Vector of all general (common) information for devices (components) in node.
            vector<devicestruc*> device;
            uint16_t device_cnt = 0;

            //! Structure for devices (components) special data in node, by type.
            devspecstruc devspec;

            //! Vector of all ports known to node.
            vector<portstruc> port;
            uint16_t port_cnt = 0;

            //! Single entry vector for agent information.
            agentstruc agent0;
//            uint16_t agent_cnt = 0;

            //! Vector of State information for all nodes
            vector<sim_state> sim_states;

			sim_state	prev_sim_state;
			sim_state	prev_prev_sim_state;

            //! Single entry vector for event information.
            vector<eventstruc> event;
            uint16_t event_cnt = 0;

            //! Vector of all targets known to node.
            vector<targetstruc> target;
            uint16_t target_cnt = 0;

            //! Single entry vector for user information.
            vector<userstruc> user;
            uint16_t user_cnt = 0;

            //! Array of Two Line Elements
            vector<Convert::tlestruc> tle;
            uint16_t tle_cnt = 0;

            //! JSON descriptive information
            jsonnode json;

            /// Support for Simulation (just for testing and integration)

            // fictional mass and density
            double mass = 1.0;
            double dens = 1.0;

            // position
            double	t_pos = 0.0;
            double	x_pos = 0.0;
            double	y_pos = 0.0;
            double	z_pos = 0.0;

            // velocity
            double	t_vel = 0.0;
            double	x_vel = 0.0;
            double	y_vel = 0.0;
            double	z_vel = 0.0;

            // acceleration
            double	t_acc = 0.0;
            double	x_acc = 0.0;
            double	y_acc = 0.0;
            double	z_acc = 0.0;

            // attitude
            double	t_att = 0.0;
            double	pitch = 0.0;
            double	roll =  0.0;
            double	yaw =   0.0;

            // orbital elements

            // epoch (do i wanna do it this way?)

            // initial position
            //double	x_0_pos = 0.0;
            //double	y_0_pos = 0.0;
            //double	z_0_pos = 0.0;

            // initial velocity
            //double	x_0_vel = 0.0;
            //double	y_0_vel = 0.0;
            //double	z_0_vel = 0.0;

            // t0 = time of epoch
            //double	t0 = 0.0;
            // v0 = true anomaly of epoch
            //double	v0 = 0.0;

            // a = semi-major axis (m)
            double	a = 0.0;
            // e = eccentricity
            double	e = 0.0;

            // BONUS ROUND

            // b = semi-minor axis (m)
            double b = a*sqrt(1.0 - pow(e,2.0));

            // l = semi-latus rectum = h^2/mu = b^2/a = a*(1-e^2) = wow!
            double l = a*(1.0-pow(e,2.0));


            // i = inclination (radians)
            double	i = 0.0;
            // O = longitude of the right ascending node (radians)
            double	O = 0.0;
            // w = argument of the periapsis (radians)
            double	w = 0.0;
            // tau = time of periapsis passage (seconds)
            double	tau = 0.0;


            // G = gravitational constant = 6.67430(15)*10^(−11) m3⋅kg–1⋅s–2
            double G = 6.6743015e-11;

            // M = Mass of the Earth = 5.9722*10^24 kg
            double mass_of_Earth = 5.9722e24;

            // mu = standard gravitational parameter (m3/s2)
            double mu = G * mass_of_Earth;

            // n = mean angular motion (rad/s) [ used to find a in TLEs ]
            double n = pow( (mu / pow(a,3.0) ), (0.5) );

            // T = period of orbit (seconds)
            double T = ( 2.0 * M_PI ) / n;

            // t = current time (should this be seconds?  or days?)
            double t = 0.0;

            // M = mean anamoly
            //double M = n * (t - tau);
            double M = fmod(n * (t - tau), 2*M_PI);

            // v = true anomaly (Fourier approximation)
            double v = M + (2.0*e-0.25*pow(e,3.0))*sin(M) + 1.25*pow(e,2.0)*sin(2.0*M) + (13.0/12.0)*pow(e,3.0)*sin(3.0*M);

            // r(v(t)) = radius (distance from focus of attraction to orbiting body)
            double r = l / (1.0 + e*cos(v));

            // position at time t in perifocal co-ords (as function of v(t))
            double P_pos_t = r * cos(v);
            double Q_pos_t = r * sin(v);
            double W_pos_t = 0.0;

            // velocity at time t in perifocal co-ords (as function of v(t))
            double P_vel_t = sqrt(mu/l) * -sin(v);
            double Q_vel_t = sqrt(mu/l) * (e+cos(v));
            double W_vel_t = 0.0;

            // acceleration at time t in perifocal co-ords (as function of v(t))
            double P_acc_t = 0.0;
            double Q_acc_t = 0.0;
            double W_acc_t = 0.0;

            // rotation matrix from perifocal to equitorial coordinates (R_row_col)

            //	|R_0_0	R_0_1	R_0_2|	[ P ]     [I]
            //	|R_1_0	R_1_1	R_1_2|* | Q | ==> |J|
            //	|R_2_0	R_2_1	R_2_2|	[ W ]     [K]

            // (inverse transform is the transpose of R)

            double R_0_0 = 0.0;
            double R_1_0 = 0.0;
            double R_2_0 = 0.0;

            double R_0_1 = 0.0;
            double R_1_1 = 0.0;
            double R_2_1 = 0.0;

            double R_0_2 = 0.0;
            double R_1_2 = 0.0;
            double R_2_2 = 0.0;

            /// Support for Simulation (just for testing and integration)
            void set_up_rotation_matrix ()	{
                R_0_0 =	-sin(O)*cos(i)*sin(w) + cos(O)*cos(w);
                R_0_1 =	-sin(O)*cos(i)*cos(w) - cos(O)*sin(w);
                R_0_2 =	        sin(O)*sin(i);

                R_1_0 =	 cos(O)*cos(i)*sin(w) + sin(O)*cos(w);
                R_1_1 =	 cos(O)*cos(i)*cos(w) - sin(O)*sin(w);
                R_1_2 =	-cos(O)*sin(i);

                R_2_0 =	        sin(i)*sin(w);
                R_2_1 =	        sin(i)*cos(w);
                R_2_2 =	        cos(i);
                return;
            }

            // equatorial co-ordinates (ECI)
            double I_pos_t = 0.0;
            double J_pos_t = 0.0;
            double K_pos_t = 0.0;

            double I_vel_t = 0.0;
            double J_vel_t = 0.0;
            double K_vel_t = 0.0;

            double I_acc_t = 0.0;
            double J_acc_t = 0.0;
            double K_acc_t = 0.0;

            // set position, velocity, acceleration at given time (in MJD)
            void set_PQW(double time)	{

                // to find position and velocity at time t
                // 0    Make sure all necessary orbital elements are set
                t = time; // in MJD
                double t_secs = fmod(time*24.*60.*60.,T); // in seconds of orbit
                l = a*(1.0-pow(e,2.0));
                // 1    Calculate mean anamoly (M)
                M = fmod(n * (t_secs - tau), 2*M_PI);
                // 2    Calculate true anamoly (v)
                v = M + (2.0*e-0.25*pow(e,3.0))*sin(M) + 1.25*pow(e,2.0)*sin(2.0*M) + (13.0/12.0)*pow(e,3.0)*sin(3.0*M);
                // 3    Calculate radius (r)
                r = l / (1.0 + e*cos(v));
                // 4    Calculate position vector <P_pos_t, Q_pos_t, W_pos_t>
                P_pos_t = r * cos(v);
                Q_pos_t = r * sin(v);
                W_pos_t = 0.0;

                // 4    Calculate velocity vector <P_pos_t, Q_pos_t, W_pos_t>
                P_vel_t = sqrt(mu/l) * -sin(v);
                Q_vel_t = sqrt(mu/l) * (e+cos(v));
                W_vel_t = 0.0;


                // 5	Calculate acceleration vector <P_acc_t, Q_acc_t, W_acc_t>
                // a nanosecond apart
                double dt = t_secs + 0.000000001;
                double M_dt = fmod(n * (dt - tau), 2*M_PI);
                double v_dt = M_dt + (2.0*e-0.25*pow(e,3.0))*sin(M_dt) + 1.25*pow(e,2.0)*sin(2.0*M_dt) + (13.0/12.0)*pow(e,3.0)*sin(3.0*M_dt);
                //double r_dt = l / (1.0 + e*cos(v_dt));

                double P_vel_dt = sqrt(mu/l) * -sin(v_dt);
                double Q_vel_dt = sqrt(mu/l) * (e+cos(v_dt));
                double W_vel_dt = 0.0;

                P_acc_t = (P_vel_t - P_vel_dt) / (t_secs-dt);
                Q_acc_t = (Q_vel_t - Q_vel_dt) / (t_secs-dt);
                W_acc_t = (W_vel_t - W_vel_dt) / (t_secs-dt);

                return;

            }

            // perifocal to geocentric equatorial co-ordinate conversion
            void set_IJK_from_PQW ()	{
                set_up_rotation_matrix();
                I_pos_t = R_0_0 * P_pos_t + R_0_1 * Q_pos_t + R_0_2 * W_pos_t;
                J_pos_t = R_1_0 * P_pos_t + R_1_1 * Q_pos_t + R_1_2 * W_pos_t;
                K_pos_t = R_2_0 * P_pos_t + R_2_1 * Q_pos_t + R_2_2 * W_pos_t;

                I_vel_t = R_0_0 * P_vel_t + R_0_1 * Q_vel_t + R_0_2 * W_vel_t;
                J_vel_t = R_1_0 * P_vel_t + R_1_1 * Q_vel_t + R_1_2 * W_vel_t;
                K_vel_t = R_2_0 * P_vel_t + R_2_1 * Q_vel_t + R_2_2 * W_vel_t;

                I_acc_t = R_0_0 * P_acc_t + R_0_1 * Q_acc_t + R_0_2 * W_acc_t;
                J_acc_t = R_1_0 * P_acc_t + R_1_1 * Q_acc_t + R_1_2 * W_acc_t;
                K_acc_t = R_2_0 * P_acc_t + R_2_1 * Q_acc_t + R_2_2 * W_acc_t;

                return;
            }


            // placeholder for state updater from Eric's propagator
            // time t is in MJD
            void update_sim_state(double t)	{
                set_PQW(t);
                set_IJK_from_PQW();

                // update the state for the calling agent
                // find index of calling agent in sim_states[]
                for(size_t i = 0; i < sim_states.size(); ++i)	{
                    string node_name(agent0.beat.node);
                    string agent_name(agent0.beat.proc);
                    if(sim_states[i].node_name == node_name && sim_states[i].agent_name == agent_name)	{

                        // update to most recent values
                        sim_states[i].t_pos = t;
                        sim_states[i].x_pos = I_pos_t;
                        sim_states[i].y_pos = J_pos_t;
                        sim_states[i].z_pos = K_pos_t;

                        sim_states[i].t_vel = t;
                        sim_states[i].x_vel = I_vel_t;
                        sim_states[i].y_vel = J_vel_t;
                        sim_states[i].z_vel = K_vel_t;

                        sim_states[i].t_acc = t;
                        sim_states[i].x_acc = I_acc_t;
                        sim_states[i].y_acc = J_acc_t;
                        sim_states[i].z_acc = K_acc_t;

                        break;
                    }
                }
                return;
            }


            // geocentric equatorial to perifocal co-ordinate conversion
            void set_PQW_from_IJK ()	{
                set_up_rotation_matrix();
                P_pos_t = R_0_0 * I_pos_t + R_1_0 * J_pos_t + R_2_0 * K_pos_t;
                Q_pos_t = R_0_1 * I_pos_t + R_1_1 * J_pos_t + R_2_1 * K_pos_t;
                W_pos_t = R_0_2 * I_pos_t + R_1_2 * J_pos_t + R_2_2 * K_pos_t;

                P_vel_t = R_0_0 * I_vel_t + R_1_0 * J_vel_t + R_2_0 * K_vel_t;
                Q_vel_t = R_0_1 * I_vel_t + R_1_1 * J_vel_t + R_2_1 * K_vel_t;
                W_vel_t = R_0_2 * I_vel_t + R_1_2 * J_vel_t + R_2_2 * K_vel_t;

                P_acc_t = R_0_0 * I_acc_t + R_1_0 * J_acc_t + R_2_0 * K_acc_t;
                Q_acc_t = R_0_1 * I_acc_t + R_1_1 * J_acc_t + R_2_1 * K_acc_t;
                W_acc_t = R_0_2 * I_acc_t + R_1_2 * J_acc_t + R_2_2 * K_acc_t;
                return;
            }


            // orbital equations

            // to find position and velocity at time t
            // 0	Make sure all orbital elements are set
            // 1	Calculate mean anamoly (M)
            // 2	Calculate true anamoly (v)
            // 3	Calculate radius (r)
            // 4	Calculate position vector <P_pos_t, Q_pos_t, R_pos_t>
            // 5	Calculate velocity vector <P_vel_t, Q_vel_t, R_vel_t>
            // 6	Transform perifocal (PQW) co-ords to geocentric equatorial (IJK) co-ords




            /// Support for Namespace 2.0

            using name_map = map<string,void*>;
            using name_mapping = pair<string,void*>;

            using type_map = map<string,string>;
            using type_mapping = pair<string,string>;

            name_map names;
            type_map types;

            /// Checks if provided name exists within Namespace 2.0
            /**
        @param	s	string representing name to search for
        @return Returns true if name exists. Returns false if not.
    */
            bool name_exists(const string& s)
            {
                return (names.find(s) == names.end()) ? false : true;
            }

            /// Returns the length of the map used to represent Namespace 2.0
            /**
        @return Size size_t of the map used to represent Namespace 2.0
    */
            size_t size()	{	return names.size();	}

            /// Prints every name within Namespace 2.0
            /**	For debug use.
        @return n/a
    */
            void print_all_names() const	{
                name_map::const_iterator it = names.begin();
                while(it != names.end())
                {
                    cout<<(it++)->first<<endl;
                }
            }

            /// Prints every name and the associated type within Namespace 2.0
            /**	For debug use.
        @return n/a
    */
            void print_all_names_types() const	{
                name_map::const_iterator n = names.begin();
                while(n != names.end())	{
                    type_map::const_iterator t = types.find(n->first);
                    if(t == types.end())	{
                        cout<<left<<setw(40)<<(n++)->first<<endl;
                    } else {
                        cout<<left<<setw(40)<<(n++)->first<<t->second<<endl;
                    }
                }
                return;
            }

            /// Prints every name and the associated value and type within Namespace 2.0
            /**	For debug use.
        @return n/a
    */
            void print_all_names_types_values() const	{
                name_map::const_iterator n = names.begin();
                while(n != names.end())	{
                    type_map::const_iterator t = types.find(n->first);
                    if(t == types.end())	{
                        cout<<left<<setw(40)<<(n++)->first<<endl;
                    } else {
                        cout<<left<<setw(40)<<(n++)->first<<left<<setw(16)<<t->second;
                        if(t->second=="double")	{
                            cout<<left<<setw(32)<<setprecision(std::numeric_limits<double>::digits10)<<fixed<<get_value<double>(t->first)<<endl;
                        } else if(t->second=="float")	{
                            cout<<left<<setw(32)<<setprecision(std::numeric_limits<float>::digits10)<<fixed<<get_value<float>(t->first)<<endl;
                        } else if(t->second=="int")	{
                            cout<<left<<setw(32)<<get_value<int>(t->first)<<endl;
                        } else if(t->second=="uint32_t")	{
                            cout<<left<<setw(32)<<get_value<uint32_t>(t->first)<<endl;
                        } else if(t->second=="int32_t")	{
                            cout<<left<<setw(32)<<get_value<int32_t>(t->first)<<endl;
                        } else if(t->second=="uint16_t")	{
                            cout<<left<<setw(32)<<get_value<uint16_t>(t->first)<<endl;
                        } else if(t->second=="int16_t")	{
                            cout<<left<<setw(32)<<get_value<int16_t>(t->first)<<endl;
                        } else if(t->second=="uint8_t")	{
                            cout<<left<<setw(32)<<get_value<uint8_t>(t->first)<<endl;
                        } else if(t->second=="int8_t")	{
                            cout<<left<<setw(32)<<get_value<int8_t>(t->first)<<endl;
                        } else if(t->second=="size_t")	{
                            cout<<left<<setw(32)<<get_value<size_t>(t->first)<<endl;
                        } else if(t->second=="bool")	{
                            cout<<left<<setw(32)<<get_value<bool>(t->first)<<endl;
                        } else if(t->second=="string")	{
                            cout<<left<<setw(32)<<get_value<string>(t->first)<<endl;
                        } else	{
                            cout<<endl;
                        }
                    }
                }
                return;
            }

            /// Get every name within Namespace 2.0
            /** Returns a vector of strings containing every name within Namespace 2.0.
        @return Vector of strings of all names within Namespace 2.0.
    */
            vector<string> get_all_names() const	{
                vector<string> all_names;
                name_map::const_iterator it = names.begin();
                while(it != names.end())	{ all_names.push_back((it++)->first); }
                return all_names;
            }

            // deprecate for now to force types to be known for all names
            //void add_name(const string& s, void* v)	{ names.insert(name_mapping(s,v)); };

            /// Adds a name, memory location, and datatype to Namespace 2.0
            /** Associates a name with a specific memory location of the COSMOS Data Structure (::cosmosstruc) and a specific data type.  Multiple names may point to the same memory address.  Names may point to primitive datatypes, user-defined objects, vectors of primitive datatypes, or vectors of user-defined objects.  Whitespace and all punctuation besides unescaped double quotes are supported for inclusion in names.
        @param	s	string representing name of data
        @param	v	void* representing memory address of data
        @param	t	string representing datatype of data
        @return	n/a

    */
            void add_name(const string& s, void* v, string t)	{
//                names.insert(name_mapping(s,v));
                names[s] = v;
                types.insert(type_mapping(s,t));
            };
            //TODO:   change_name(..) functions, match_name(), find_aliases(), etc

            /// Removes a name, memory location, and datatype from Namespace 2.0.
            /** Removes a single entry added into Namespace 2.0 with #add_name().
        @param	s	string representing name of data to remove
        @return n/a
    */
            void remove_name(const string& s) {
                names.erase(s);
                types.erase(s);
            }

            /// Removes names from Namespace 2.0 recursively.
            /** Removes reference to memory location and data type associated with the provided name, then searches through Namespace 2.0 removing entries for all children of the provided name. E.g., if provided the name of an array, will remove the entry associated with the array and also the entries of every one of its elements.
        @param	s	string representing name of data (and its children) to remove
        @return n/a
    */
            void remove_name_recursive(const string& s) {
                auto p = names.lower_bound(s);
                auto q = names.end();
                const string sbracket = s + "[";
                const string sdot = s + ".";

                // search through name map for names of the object/collection and its members
                while (p != q) {
                    if (p->first.compare(s) == 0 ||								// if exact match s is found. eg: "jmap" but not "jmapped"
                        p->first.compare(0, sbracket.size(), sbracket) == 0 ||	// if search string s + [ is found. eg: "jmap[0]"
                        p->first.compare(0, sdot.size(), sdot) == 0) {			// if search string s + . is found. eg: "node.name"
                        names.erase(p++);
                    } else {
                        ++p;
                    }
                }

                auto p2 = types.lower_bound(s);
                auto q2 = types.end();
                while (p2 != q2) {
                    if (p2->first.compare(s) == 0 ||							// if exact match s is found. eg: "jmap" but not "jmapped"
                        p2->first.compare(0, sbracket.size(), sbracket) == 0 ||	// if search string s + [ is found. eg: "jmap[0]"
                        p2->first.compare(0, sdot.size(), sdot) == 0) {			// if search string s + . is found. eg: "node.name"
                        types.erase(p2++);
                    } else {
                        ++p2;
                    }
                }

            }

            /// Add default names for every accessible memory location within the COSMOS Data Structure (Cosmos::Support::cosmosstruc) to Namespace 2.0.
            /** Provide a default name for every accessible memory location within the COSMOS Data Structure (Cosmos::Support::cosmosstruc) for Namespace 2.0. Naming convention follows the exact representation of the object in code. E.g., default name for `equation[0].name` is `"equation[0].name"`.
        @param none
        @return n/a
    */
            void add_default_names();

            /// Gets the name associated with the provided memory address in Namespace 2.0.
            /** Searches through Namespace 2.0 and returns the first name associated with the provided memory address.
        @param	v	void pointer to memory address of an entry within Namespace 2.0 to search for
        @return	string name of memory address associated with \p v. Returns empty string if address is not found.
    */
            string get_name(void* v)	{
                name_map::const_iterator it = names.begin();
                while(it->second != v && it != names.end())	{ it++; }
                if(it == names.end())	{	cerr<<"address <"<<v<<"> not found!"<<endl; return "";	}
                return it->first;
            }

            /// Gets the data type associated with the provided name in Namespace 2.0.
            /** Finds the data type associated with the provided name in Namespace 2.0.
        @param	s	string representing name to search for
        @return	string representing the data type associated with the provided name. Returns empty string if name is not found.
    */
            string get_type(const string& s) const	{
                type_map::const_iterator it = types.find(s);
                if(it == types.end())	{	/*cerr<<"type for <"<<s<<"> not found!"<<endl;*/ return "";	}
                return it->second;
            }

            /// Gets the pointer to the memory address associated with the provided name in Namespace 2.0.
            /** Searches through Namespace 2.0 and returns a pointer to the associated memory address casted into the data type given to the template.
        @param	s	string representing name to search for
        @return	type casted pointer to associated memory address. Returns `nullptr` if name is not found.
    */
            template<class T>
            T* get_pointer(const string& s) const	{
                name_map::const_iterator it = names.find(s);
                if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return nullptr;	}
                return (T*)(it->second);
            }

            /// Gets the pointer to the memory address associated with the provided name in Namespace 2.0.
            /** Searches through Namespace 2.0 and returns a void pointer to the associated memory address.
            @param	s	string representing name to search for
            @return	void pointer to associated memory address. Returns `nullptr` if name is not found.
            */
            void* get_pointer(const string& s) const	{
                name_map::const_iterator it = names.find(s);
                if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return nullptr;	}
                return it->second;
            }

            /// Gets the value of the data associated with the provided name in Namespace 2.0.
            /** Searches through Namespace 2.0 and returns the value of the dereferenced pointer to the associated memory address.
        @param	s	string representing name to search for
        @return	value of the data pointed to by the pointer of the associated name. Returns a new instance of type `T` if name is not found.
    */
            template<class T>
            T get_value(const string& s) const	{
                // change to static null object?
                T dummy = T();
                name_map::const_iterator it = names.find(s);
                if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return dummy;	}
                return *get_pointer<T>(s);
            }

            template<class T>
            T get_ivalue(const string& s) const	{
                // change to static null object?
                T dummy = T();
                name_map::const_iterator it = names.find(s);
                if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return dummy;	}
                return **get_pointer<T*>(s);
            }

            /// Sets the value of the data associated with the provided name in Namespace 2.0.
            /** Searches through Namespace 2.0 and sets the value of the data pointed to by the pointer to the memory address associated with the provided name.
        @param	s	string representing name to search for
        @param	value	new value to set the data to
        @return	n/a
    */
            template<class T>
            void set_value(const string& s, const T& value) const	{
                // maybe if not found should be inserted??  hmmm....  ask Eric
                name_map::const_iterator it = names.find(s);
                if(it == names.end())	{	cerr<<"name <"<<s<<"> not found!"<<endl; return;	}
                *get_pointer<T>(s) = value;
            }

            /// Sets the data in Namespace 2.0 with a JSON-formatted string.
            /** Searches through Namespace 2.0 and sets the value of the appropriate data by parsing the provided JSON-formatted string. The name used to search Namespace 2.0 is the key of the first entry in the JSON-formatted string.
        @param	json	JSON formatted string to set the data to
        @return	n/a
    */
            void set_json(const string& json) 	{
                //cout<<"\tJSON received = <"<<json<<">"<<endl;
                string error;
                // parses multiple JSON strings
                vector<json11::Json> vp = json11::Json::parse_multi(json,error);
                //cout<<"\tJSON error    = <"<<error<<">"<<endl;
                for(size_t i = 0; i < vp.size(); ++i)	{
                    json11::Json p = vp[i];

                    string name(p.object_items().begin()->first);
                    //cout<<"\tJSON name     = <"<<name<<">"<<endl;
                    if(error.empty()) {
                        if(!p[name].is_null())	{
                            if(name_exists(name))  {
                                string type = get_type(name);
                                // base data type
                                if(type == "string")	{
                                    set_value<string>(name, p[name].string_value());
                                } else if(type == "double")	{
                                    set_value<double>(name, p[name].number_value());
                                } else if (type == "float")	{
                                    set_value<float>(name, p[name].number_value());
                                } else if (type == "int")	{
                                    set_value<int>(name, p[name].int_value());
                                } else if (type == "bool")	{
                                    set_value<bool>(name, p[name].bool_value());
                                } else if (type == "uint32_t")	{
                                    set_value<uint32_t>(name, p[name].int_value());
                                } else if (type == "int32_t")	{
                                    set_value<int32_t>(name, p[name].int_value());
                                } else if (type == "uint16_t")	{
                                    set_value<uint16_t>(name, p[name].int_value());
                                } else if (type == "int16_t")	{
                                    set_value<int16_t>(name, p[name].int_value());
                                } else if (type == "uint8_t")	{
                                    set_value<uint8_t>(name, p[name].int_value());
                                } else if (type == "int8_t")	{
                                    set_value<int8_t>(name, p[name].int_value());
                                } else if (type == "size_t") {
                                    set_value<size_t>(name, p[name].int_value());

                                    // user-defined types

                                } else if (type == "agent_request_entry") {
                                    //get_pointer<agent_request_entry>(name)->from_json(json);
                                } else if (type == "agentstruc") {
                                    get_pointer<agentstruc>(name)->from_json(p[name].dump());
                                } else if (type == "attstruc") {
                                    get_pointer<Convert::attstruc>(name)->from_json(p[name].dump());
                                } else if (type == "beatstruc") {
                                    get_pointer<beatstruc>(name)->from_json(p[name].dump());
                                } else if (type == "cartpos") {
                                    get_pointer<Convert::cartpos>(name)->from_json(p[name].dump());
                                } else if (type == "cosmosstruc") {
                                    get_pointer<cosmosstruc>(name)->from_json(p[name].dump());
                                } else if (type == "cvector") {
                                    get_pointer<cvector>(name)->from_json(p[name].dump());
                                } else if (type == "devicestruc") {
                                    get_pointer<devicestruc>(name)->from_json(p[name].dump());
                                } else if (type == "devspecstruc") {
                                    get_pointer<devspecstruc>(name)->from_json(p[name].dump());
                                } else if (type == "equationstruc") {
                                    get_pointer<equationstruc>(name)->from_json(p[name].dump());
                                } else if (type == "sim_param") {
                                    get_pointer<sim_param>(name)->from_json(p[name].dump());
                                } else if (type == "sim_state") {
                                    get_pointer<sim_state>(name)->from_json(p[name].dump());
                                } else if (type == "eventstruc") {
                                    get_pointer<eventstruc>(name)->from_json(p[name].dump());
                                } else if (type == "extraatt") {
                                    get_pointer<Convert::extraatt>(name)->from_json(p[name].dump());
                                } else if (type == "extrapos") {
                                    get_pointer<Convert::extrapos>(name)->from_json(p[name].dump());
                                } else if (type == "face") {
                                    get_pointer<Cosmos::wavefront::face>(name)->from_json(p[name].dump());
                                } else if (type == "facestruc") {
                                    get_pointer<facestruc>(name)->from_json(p[name].dump());
                                } else if (type == "geoidpos") {
                                    get_pointer<Convert::geoidpos>(name)->from_json(p[name].dump());
                                } else if (type == "group") {
                                    get_pointer<Cosmos::wavefront::group>(name)->from_json(p[name].dump());
                                } else if (type == "gvector") {
                                    get_pointer<gvector>(name)->from_json(p[name].dump());
                                } else if (type == "jsonhandle") {
                                    get_pointer<jsonhandle>(name)->from_json(p[name].dump());
                                } else if (type == "jsonnode") {
                                    get_pointer<jsonnode>(name)->from_json(p[name].dump());
                                } else if (type == "line") {
                                    get_pointer<Cosmos::wavefront::line>(name)->from_json(p[name].dump());
                                } else if (type == "locstruc") {
                                    get_pointer<Convert::locstruc>(name)->from_json(p[name].dump());
                                } else if (type == "material") {
                                    get_pointer<Cosmos::wavefront::material>(name)->from_json(p[name].dump());
                                } else if (type == "NetworkType") {
                                    //get_pointer<NetworkType>(name)->from_json(p[name].dump());
                                } else if (type == "nodestruc") {
                                    get_pointer<nodestruc>(name)->from_json(p[name].dump());
                                } else if (type == "physicsstruc") {
                                    get_pointer<physicsstruc>(name)->from_json(p[name].dump());
                                } else if (type == "piecestruc") {
                                    get_pointer<piecestruc>(name)->from_json(p[name].dump());
                                } else if (type == "point") {
                                    get_pointer<Cosmos::wavefront::point>(name)->from_json(p[name].dump());
                                } else if (type == "portstruc") {
                                    get_pointer<portstruc>(name)->from_json(p[name].dump());
                                } else if (type == "PORT_TYPE") {
                                    set_value<PORT_TYPE>(name, static_cast<PORT_TYPE>(p[name].int_value()));
                                } else if (type == "posstruc") {
                                    get_pointer<Convert::posstruc>(name)->from_json(p[name].dump());
                                } else if (type == "qatt") {
                                    get_pointer<Convert::qatt>(name)->from_json(p[name].dump());
                                } else if (type == "quaternion") {
                                    get_pointer<quaternion>(name)->from_json(p[name].dump());
                                } else if (type == "rmatrix") {
                                    get_pointer<rmatrix>(name)->from_json(p[name].dump());
                                } else if (type == "rvector") {
                                    get_pointer<rvector>(name)->from_json(p[name].dump());
                                } else if (type == "socket_channel") {
                                    //get_pointer<socket_channel>(name)->from_json(p[name].dump());
                                } else if (type == "spherpos") {
                                    get_pointer<Convert::spherpos>(name)->from_json(p[name].dump());
                                } else if (type == "svector") {
                                    get_pointer<svector>(name)->from_json(p[name].dump());
                                } else if (type == "targetstruc") {
                                    get_pointer<targetstruc>(name)->from_json(p[name].dump());
                                } else if (type == "tlestruc") {
                                    get_pointer<Convert::tlestruc>(name)->from_json(p[name].dump());
                                } else if (type == "trianglestruc") {
                                    get_pointer<trianglestruc>(name)->from_json(p[name].dump());
                                } else if (type == "unitstruc") {
                                    get_pointer<unitstruc>(name)->from_json(p[name].dump());
                                } else if (type == "userstruc") {
                                    get_pointer<userstruc>(name)->from_json(p[name].dump());
                                } else if (type == "Vector") {
                                    get_pointer<Vector>(name)->from_json(p[name].dump());
                                } else if (type == "vertexstruc") {
                                    get_pointer<vertexstruc>(name)->from_json(p[name].dump());
                                } else if (type == "wavefront") {
                                    get_pointer<wavefront>(name)->from_json(p[name].dump());
                                } else if (type == "sim_param") {
                                    get_pointer<sim_param>(name)->from_json(p[name].dump());
                                } else if (type == "sim_state") {
                                    get_pointer<sim_state>(name)->from_json(p[name].dump());

                                    // vector of base types
                                } else if (type == "vector<uint32_t>") {
                                    for(size_t i = 0; i < get_pointer<vector<uint32_t>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<uint32_t>>(name)->at(i) = p[name][i].int_value();
                                        }
                                    }
                                } else if (type == "vector<vector<uint32_t>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<uint32_t>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<uint32_t>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<uint32_t>>>(name)->at(i).at(j) = p[name][i][j].int_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<int32_t>") {
                                    for(size_t i = 0; i < get_pointer<vector<int32_t>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<int32_t>>(name)->at(i) = p[name][i].int_value();
                                        }
                                    }
                                } else if (type == "vector<vector<int32_t>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<int32_t>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<int32_t>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<int32_t>>>(name)->at(i).at(j) = p[name][i][j].int_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<uint16_t>") {
                                    for(size_t i = 0; i < get_pointer<vector<uint16_t>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<uint16_t>>(name)->at(i) = p[name][i].int_value();
                                        }
                                    }
                                } else if (type == "vector<vector<uint16_t>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<uint16_t>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<uint16_t>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<uint16_t>>>(name)->at(i).at(j) = p[name][i][j].int_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<int16_t>") {
                                    for(size_t i = 0; i < get_pointer<vector<int16_t>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<int16_t>>(name)->at(i) = p[name][i].int_value();
                                        }
                                    }
                                } else if (type == "vector<vector<int16_t>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<int16_t>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<int16_t>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<int16_t>>>(name)->at(i).at(j) = p[name][i][j].int_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<uint8_t>") {
                                    for(size_t i = 0; i < get_pointer<vector<uint8_t>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<uint8_t>>(name)->at(i) = p[name][i].int_value();
                                        }
                                    }
                                } else if (type == "vector<vector<uint8_t>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<uint8_t>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<uint8_t>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<uint8_t>>>(name)->at(i).at(j) = p[name][i][j].int_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<int8_t>") {
                                    for(size_t i = 0; i < get_pointer<vector<int8_t>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<int8_t>>(name)->at(i) = p[name][i].int_value();
                                        }
                                    }
                                } else if (type == "vector<vector<int8_t>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<int8_t>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<int8_t>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<int8_t>>>(name)->at(i).at(j) = p[name][i][j].int_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<int>") {
                                    for(size_t i = 0; i < get_pointer<vector<int>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<int>>(name)->at(i) = p[name][i].int_value();
                                        }
                                    }
                                } else if (type == "vector<vector<int>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<int>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<int>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<int>>>(name)->at(i).at(j) = p[name][i][j].int_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<size_t>") {
                                    for(size_t i = 0; i < get_pointer<vector<size_t>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<size_t>>(name)->at(i) = p[name][i].int_value();
                                        }
                                    }
                                } else if (type == "vector<vector<size_t>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<size_t>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<size_t>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<size_t>>>(name)->at(i).at(j) = p[name][i][j].int_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<bool>") {
                                    for(size_t i = 0; i < get_pointer<vector<bool>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<bool>>(name)->at(i) = p[name][i].bool_value();
                                        }
                                    }
                                } else if (type == "vector<vector<bool>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<bool>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<bool>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<bool>>>(name)->at(i).at(j) = p[name][i][j].bool_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<string>") {
                                    for(size_t i = 0; i < get_pointer<vector<string>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<string>>(name)->at(i) = p[name][i].string_value();
                                        }
                                    }
                                } else if (type == "vector<vector<string>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<string>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<string>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<string>>>(name)->at(i).at(j) = p[name][i][j].string_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<float>") {
                                    for(size_t i = 0; i < get_pointer<vector<float>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<float>>(name)->at(i) = p[name][i].number_value();
                                        }
                                    }
                                } else if (type == "vector<vector<float>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<float>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<float>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<float>>>(name)->at(i).at(j) = p[name][i][j].number_value();
                                            }
                                        }
                                    }
                                } else if (type == "vector<double>") {
                                    for(size_t i = 0; i < get_pointer<vector<double>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<double>>(name)->at(i) = p[name][i].number_value();
                                        }
                                    }
                                } else if (type == "vector<vector<double>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<double>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<double>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<double>>>(name)->at(i).at(j) = p[name][i][j].number_value();
                                            }
                                        }
                                    }

                                    // vector of user-defined types
                                    //} else if (type == "vector<agent_request_entry>") {
                                    //	for(size_t i = 0; i < get_pointer<vector<agent_request_entry>>(name)->size(); ++i) {
                                    //		if(!p[name][i].is_null()) {
                                    //			get_pointer<vector<agent_request_entry>>(name)->at(i).from_json(p[name][i].dump());
                                    //		}
                                    //	}
                                } else if (type == "vector<agentstruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<agentstruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<agentstruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<devicestruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<devicestruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<devicestruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<devicestruc*>") {
                                    for(size_t i = 0; i < get_pointer<vector<devicestruc*>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<devicestruc*>>(name)->at(i)->from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<equationstruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<equationstruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<equationstruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<sim_param>") {
                                    for(size_t i = 0; i < get_pointer<vector<sim_param>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<sim_param>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<sim_state>") {
                                    for(size_t i = 0; i < get_pointer<vector<sim_state>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<sim_state>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<eventstruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<eventstruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<eventstruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<face>") {
                                    for(size_t i = 0; i < get_pointer<vector<Cosmos::wavefront::face>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<Cosmos::wavefront::face>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<facestruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<facestruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<facestruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<group>") {
                                    for(size_t i = 0; i < get_pointer<vector<Cosmos::wavefront::group>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<Cosmos::wavefront::group>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<line>") {
                                    for(size_t i = 0; i < get_pointer<vector<Cosmos::wavefront::line>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<Cosmos::wavefront::line>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<material>") {
                                    for(size_t i = 0; i < get_pointer<vector<Cosmos::wavefront::material>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<Cosmos::wavefront::material>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<piecestruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<piecestruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<piecestruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<point>") {
                                    for(size_t i = 0; i < get_pointer<vector<Cosmos::wavefront::point>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<Cosmos::wavefront::point>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<portstruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<portstruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<portstruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<targetstruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<targetstruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<targetstruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<Convert::tlestruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<Convert::tlestruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<Convert::tlestruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<trianglestruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<trianglestruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<trianglestruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<unitstruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<unitstruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<unitstruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<userstruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<userstruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<userstruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<Vector>") {
                                    for(size_t i = 0; i < get_pointer<vector<Vector>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<Vector>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<unitstruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<unitstruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<unitstruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<vector<unitstruc>>") {
                                    for(size_t i = 0; i < get_pointer<vector<vector<unitstruc>>>(name)->size(); ++i) {
                                        for(size_t j = 0; j < get_pointer<vector<vector<unitstruc>>>(name)->at(i).size(); ++j) {
                                            if(!p[name][i][j].is_null()) {
                                                get_pointer<vector<vector<unitstruc>>>(name)->at(i).at(j).from_json(p[name][i][j].dump());
                                            }
                                        }
                                    }
                                } else if (type == "vector<vertexstruc>") {
                                    for(size_t i = 0; i < get_pointer<vector<vertexstruc>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<vertexstruc>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<sim_param>") {
                                    for(size_t i = 0; i < get_pointer<vector<sim_param>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<sim_param>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else if (type == "vector<sim_state>") {
                                    for(size_t i = 0; i < get_pointer<vector<sim_state>>(name)->size(); ++i) {
                                        if(!p[name][i].is_null()) {
                                            get_pointer<vector<sim_state>>(name)->at(i).from_json(p[name][i].dump());
                                        }
                                    }
                                } else	{
                                    // I guess this block means the type is not supported!
                                    // could re-add templated version of set_json so user
                                    // can explicitly call for given unsupported datatype
                                    //get_pointer<T>(name)->from_json(json);
                                    return;
                                }
                            }
                        }
                    }
                }// end for loop
            }

            /// Gets a JSON-formatted string of the data associated with the provided name in Namespace 2.0.
            /** Searches through Namespace 2.0 and gets the JSON-formatted string of the data pointed to by the pointer to the memory address associated with the provided name.
        @param	s	string representing name to search for
        @return	JSON-formatted string of data. Returns empty string if name is not found.
    */
            template<class T>
            string get_json(const string& s)	{
                if(name_exists(s))	{
                    json11::Json json = json11::Json::object { { s, this->get_value<T>(s) } };
                    return json.dump();
                } else {
                    return "";
                }
            }

            // get called from agents (no template)
            /// Gets a JSON-formatted string of the data associated with the provided name in Namespace 2.0. (Non-template version)
            /** Searches through Namespace 2.0 and gets the JSON-formatted string of the data pointed to by the pointer to the memory address associated with the provided name.
        @param	s	string representing name to search for
        @return	JSON-formatted string of data. Returns empty string if name is not found.
    */
            string get_json(const string& s)	{
                if(name_exists(s))  {
                    json11::Json json;
                    string type = get_type(s);
                    // base data types
                    if (type == "uint32_t") {
                        json = json11::Json::object { { s, get_value<int>(s) } };
                    } else if (type == "int32_t") {
                        json = json11::Json::object { { s, get_value<int32_t>(s) } };
                    } else if (type == "uint16_t") {
                        json = json11::Json::object { { s, get_value<uint16_t>(s) } };
                    } else if (type == "int16_t") {
                        json = json11::Json::object { { s, get_value<int16_t>(s) } };
                    } else if (type == "uint8_t") {
                        json = json11::Json::object { { s, get_value<uint8_t>(s) } };
                    } else if (type == "int8_t") {
                        json = json11::Json::object { { s, get_value<int8_t>(s) } };
                    } else if (type == "int") {
                        json = json11::Json::object { { s, get_value<int>(s) } };
                    } else if (type == "size_t") {
                        json = json11::Json::object { { s, get_value<int>(s) } };
                    } else if (type == "bool") {
                        json = json11::Json::object { { s, get_value<bool>(s) } };
                    } else if (type == "string") {
                        json = json11::Json::object { { s, get_value<string>(s) } };
                    } else if (type == "float") {
                        json = json11::Json::object { { s, get_value<float>(s) } };
                    } else if (type == "double") {
                        json = json11::Json::object { { s, get_value<double>(s) } };

                        // user-defined types
                    } else if (type == "agent_request_entry") {
                        //	json = json11::Json::object { { s, get_value<agent_request_entry>(s) } };
                    } else if (type == "agentstruc") {
                        json = json11::Json::object { { s, get_value<agentstruc>(s) } };
                    } else if (type == "attstruc") {
                        json = json11::Json::object { { s, get_value<Convert::attstruc>(s) } };
                    } else if (type == "beatstruc") {
                        json = json11::Json::object { { s, get_value<beatstruc>(s) } };
                    } else if (type == "cartpos") {
                        json = json11::Json::object { { s, get_value<Convert::cartpos>(s) } };
                    } else if (type == "cosmosstruc") {
                        json = json11::Json::object { { s, get_value<cosmosstruc>(s) } };
                    } else if (type == "cvector") {
                        json = json11::Json::object { { s, get_value<cvector>(s) } };
                    } else if (type == "devicestruc") {
                        json = json11::Json::object { { s, get_value<devicestruc>(s) } };
                    } else if (type == "devspecstruc") {
                        json = json11::Json::object { { s, get_value<devspecstruc>(s) } };
                    } else if (type == "equationstruc") {
                        json = json11::Json::object { { s, get_value<equationstruc>(s) } };
                    } else if (type == "sim_param") {
                        json = json11::Json::object { { s, get_value<sim_param>(s) } };
                    } else if (type == "sim_state") {
                        json = json11::Json::object { { s, get_value<sim_state>(s) } };
                    } else if (type == "eventstruc") {
                        json = json11::Json::object { { s, get_value<eventstruc>(s) } };
                    } else if (type == "extraatt") {
                        json = json11::Json::object { { s, get_value<Convert::extraatt>(s) } };
                    } else if (type == "extrapos") {
                        json = json11::Json::object { { s, get_value<Convert::extrapos>(s) } };
                    } else if (type == "face") {
                        json = json11::Json::object { { s, get_value<Cosmos::wavefront::face>(s) } };
                    } else if (type == "facestruc") {
                        json = json11::Json::object { { s, get_value<facestruc>(s) } };
                    } else if (type == "geoidpos") {
                        json = json11::Json::object { { s, get_value<Convert::geoidpos>(s) } };
                    } else if (type == "group") {
                        json = json11::Json::object { { s, get_value<Cosmos::wavefront::group>(s) } };
                    } else if (type == "gvector") {
                        json = json11::Json::object { { s, get_value<gvector>(s) } };
                    } else if (type == "line") {
                        json = json11::Json::object { { s, get_value<Cosmos::wavefront::line>(s) } };
                    } else if (type == "jsonhandle") {
                        json = json11::Json::object { { s, get_value<jsonhandle>(s) } };
                    } else if (type == "jsonnode") {
                        json = json11::Json::object { { s, get_value<jsonnode>(s) } };
                    } else if (type == "locstruc") {
                        json = json11::Json::object { { s, get_value<Convert::locstruc>(s) } };
                    } else if (type == "material") {
                        json = json11::Json::object { { s, get_value<Cosmos::wavefront::material>(s) } };
                    } else if (type == "NetworkType") {
                        //	json = json11::Json::object { { s, get_value<NetworkType>(s) } };
                    } else if (type == "nodestruc") {
                        json = json11::Json::object { { s, get_value<nodestruc>(s) } };
                    } else if (type == "piecestruc") {
                        json = json11::Json::object { { s, get_value<piecestruc>(s) } };
                    } else if (type == "physicsstruc") {
                        json = json11::Json::object { { s, get_value<physicsstruc>(s) } };
                    } else if (type == "point") {
                        json = json11::Json::object { { s, get_value<Cosmos::wavefront::point>(s) } };
                    } else if (type == "portstruc") {
                        json = json11::Json::object { { s, get_value<portstruc>(s) } };
                    } else if (type == "PORT_TYPE") {
                        json = json11::Json::object { { s, get_value<PORT_TYPE>(s) } };
                    } else if (type == "posstruc") {
                        json = json11::Json::object { { s, get_value<Convert::posstruc>(s) } };
                    } else if (type == "qatt") {
                        json = json11::Json::object { { s, get_value<Convert::qatt>(s) } };
                    } else if (type == "quaternion") {
                        json = json11::Json::object { { s, get_value<quaternion>(s) } };
                    } else if (type == "rmatrix") {
                        json = json11::Json::object { { s, get_value<rmatrix>(s) } };
                    } else if (type == "rvector") {
                        json = json11::Json::object { { s, get_value<rvector>(s) } };
                    } else if (type == "rvector[]") {
                        //	json = json11::Json::object { { s, get_value<rvector[]>(s) } };
                    } else if (type == "socket_channel") {
                        //	json = json11::Json::object { { s, get_value<socket_channel>(s) } };
                    } else if (type == "socket_channel[]") {
                        //	json = json11::Json::object { { s, get_value<socket_channel[]>(s) } };
                    } else if (type == "spherpos") {
                        json = json11::Json::object { { s, get_value<Convert::spherpos>(s) } };
                    } else if (type == "svector") {
                        json = json11::Json::object { { s, get_value<svector>(s) } };
                    } else if (type == "targetstruc") {
                        json = json11::Json::object { { s, get_value<targetstruc>(s) } };
                    } else if (type == "tlestruc") {
                        json = json11::Json::object { { s, get_value<Convert::tlestruc>(s) } };
                    } else if (type == "trianglestruc") {
                        json = json11::Json::object { { s, get_value<trianglestruc>(s) } };
                    } else if (type == "unitstruc") {
                        json = json11::Json::object { { s, get_value<unitstruc>(s) } };
                    } else if (type == "userstruc") {
                        json = json11::Json::object { { s, get_value<userstruc>(s) } };
                    } else if (type == "Vector") {
                        json = json11::Json::object { { s, get_value<Vector>(s) } };
                    } else if (type == "vertexstruc") {
                        json = json11::Json::object { { s, get_value<vertexstruc>(s) } };
                    } else if (type == "wavefront") {
                        json = json11::Json::object { { s, get_value<wavefront>(s) } };
                    } else if (type == "sim_param") {
                        json = json11::Json::object { { s, get_value<sim_param>(s) } };
                    } else if (type == "sim_state") {
                        json = json11::Json::object { { s, get_value<sim_state>(s) } };

                        // vector of primitives
                    } else if (type == "vector<uint32_t>") {
                        json = json11::Json::object { { s, get_value<vector<int> >(s) } };
                    } else if (type == "vector<vector<uint32_t>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<int> > >(s) } };
                    } else if (type == "vector<int32_t>") {
                        json = json11::Json::object { { s, get_value<vector<int32_t> >(s) } };
                    } else if (type == "vector<vector<int32_t>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<int32_t> > >(s) } };
                    } else if (type == "vector<uint16_t>") {
                        json = json11::Json::object { { s, get_value<vector<uint16_t> >(s) } };
                    } else if (type == "vector<vector<uint16_t>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<uint16_t> > >(s) } };
                    } else if (type == "vector<int16_t>") {
                        json = json11::Json::object { { s, get_value<vector<int16_t> >(s) } };
                    } else if (type == "vector<vector<int16_t>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<int16_t> > >(s) } };
                    } else if (type == "vector<uint8_t>") {
                        json = json11::Json::object { { s, get_value<vector<uint8_t> >(s) } };
                    } else if (type == "vector<vector<uint8_t>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<uint8_t> > >(s) } };
                    } else if (type == "vector<int8_t>") {
                        json = json11::Json::object { { s, get_value<vector<int8_t> >(s) } };
                    } else if (type == "vector<vector<int8_t>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<int8_t> > >(s) } };
                    } else if (type == "vector<int>") {
                        json = json11::Json::object { { s, get_value<vector<int> >(s) } };
                    } else if (type == "vector<vector<int>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<int> > >(s) } };
                    } else if (type == "vector<size_t>") {
                        json = json11::Json::object { { s, get_value<vector<int> >(s) } };
                    } else if (type == "vector<vector<size_t>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<int> > >(s) } };
                    } else if (type == "vector<bool>") {
                        json = json11::Json::object { { s, get_value<vector<bool> >(s) } };
                    } else if (type == "vector<vector<bool>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<bool> > >(s) } };
                    } else if (type == "vector<string>") {
                        json = json11::Json::object { { s, get_value<vector<string> >(s) } };
                    } else if (type == "vector<vector<string>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<string> > >(s) } };
                    } else if (type == "vector<float>") {
                        json = json11::Json::object { { s, get_value<vector<float> >(s) } };
                    } else if (type == "vector<vector<float>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<float> > >(s) } };
                    } else if (type == "vector<double>") {
                        json = json11::Json::object { { s, get_value<vector<double> >(s) } };
                    } else if (type == "vector<vector<double>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<double> > >(s) } };

                        // vector of user-defined types
                    } else if (type == "vector<agent_request_entry>") {
                        //	json = json11::Json::object { { s, get_value<vector<agent_request_entry> >(s) } };
                    } else if (type == "vector<agentstruc>") {
                        json = json11::Json::object { { s, get_value<vector<agentstruc> >(s) } };
                    } else if (type == "vector<devicestruc>") {
                        json = json11::Json::object { { s, get_value<vector<devicestruc> >(s) } };
                    } else if (type == "vector<devicestruc*>") {
                        json = json11::Json::object { { s, get_ivalue<vector<devicestruc> > (s) } };
                    } else if (type == "vector<equationstruc>") {
                        json = json11::Json::object { { s, get_value<vector<equationstruc> >(s) } };
                    } else if (type == "vector<sim_param>") {
                        json = json11::Json::object { { s, get_value<vector<sim_param> >(s) } };
                    } else if (type == "vector<sim_state>") {
                        json = json11::Json::object { { s, get_value<vector<sim_state> >(s) } };
                    } else if (type == "vector<eventstruc>") {
                        json = json11::Json::object { { s, get_value<vector<eventstruc> >(s) } };
                    } else if (type == "vector<face>") {
                        json = json11::Json::object { { s, get_value<vector<Cosmos::wavefront::face> >(s) } };
                    } else if (type == "vector<facestruc>") {
                        json = json11::Json::object { { s, get_value<vector<facestruc> >(s) } };
                    } else if (type == "vector<group>") {
                        json = json11::Json::object { { s, get_value<vector<Cosmos::wavefront::group> >(s) } };
                    } else if (type == "vector<line>") {
                        json = json11::Json::object { { s, get_value<vector<Cosmos::wavefront::line> >(s) } };
                    } else if (type == "vector<material>") {
                        json = json11::Json::object { { s, get_value<vector<Cosmos::wavefront::material> >(s) } };
                    } else if (type == "vector<piecestruc>") {
                        json = json11::Json::object { { s, get_value<vector<piecestruc> >(s) } };
                    } else if (type == "vector<point>") {
                        json = json11::Json::object { { s, get_value<vector<Cosmos::wavefront::point> >(s) } };
                    } else if (type == "vector<portstruc>") {
                        json = json11::Json::object { { s, get_value<vector<portstruc> >(s) } };
                    } else if (type == "vector<targetstruc>") {
                        json = json11::Json::object { { s, get_value<vector<targetstruc> >(s) } };
                    } else if (type == "vector<Convert::tlestruc>") {
                        json = json11::Json::object { { s, get_value<vector<Convert::tlestruc> >(s) } };
                    } else if (type == "vector<trianglestruc>") {
                        json = json11::Json::object { { s, get_value<vector<trianglestruc> >(s) } };
                    } else if (type == "vector<unitstruc>") {
                        json = json11::Json::object { { s, get_value<vector<unitstruc> >(s) } };
                    } else if (type == "vector<userstruc>") {
                        json = json11::Json::object { { s, get_value<vector<userstruc> >(s) } };
                    } else if (type == "vector<Vector>") {
                        json = json11::Json::object { { s, get_value<vector<Vector> >(s) } };
                    } else if (type == "vector<vector<unitstruc>>") {
                        json = json11::Json::object { { s, get_value<vector<vector<unitstruc> > >(s) } };
                    } else if (type == "vector<vertexstruc>") {
                        json = json11::Json::object { { s, get_value<vector<vertexstruc> >(s) } };
                    } else if (type == "vector<sim_param>") {
                        json = json11::Json::object { { s, get_value<vector<sim_param> >(s) } };
                    } else if (type == "vector<sim_state>") {
                        json = json11::Json::object { { s, get_value<vector<sim_state> >(s) } };
                    }
                    return json.dump();
                } else {
                    return "";
                }
            }

            /// Replace every occurance of a substring within a string with another subtring.
            /** Search through a string for every instance of a specified substring, then replace. For internal use.
				@param	str		string to search through
				@param	from	substring to replace
				@param	to		substring to be replaced with
				@return	n/a
			*/
            void replace(string& str, const string& from, const string& to) {
                if(from.empty()) return;
                size_t start_pos = 0;
                while((start_pos = str.find(from, start_pos)) != string::npos) {
                    str.replace(start_pos, from.length(), to);
                    start_pos += to.length();
                }
                return;
            }

            /// Pretty-fy text
            /** Takes an input JSON-formatted string and makes it more readable. For internal use.
				@param	js		JSON-formatted string
				@return	n/a
			*/
            string pretty_form(string& js)	{

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
                    for(size_t j = 0; j < indents[newlines[i]]; ++j)	indent_string += "  ";
                    js.insert(newlines[i]+1, indent_string);
                }
                return js;
            }

            /// Gets a human-readable JSON-formatted string of the data associated with the provided name in Namespace 2.0.
            /** Searches through Namespace 2.0 and gets the JSON-formatted string of the data pointed to by the pointer to the memory address associated with the provided name.
        @param	s	string representing name to search for
        @return	JSON-formatted string of data. Returns empty string if name is not found.
    */
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

            /// Gets a human-readable JSON-formatted string of the data associated with the provided name in Namespace 2.0. (Non-template version)
            /** Searches through Namespace 2.0 and gets the JSON-formatted string of the data pointed to by the pointer to the memory address associated with the provided name.
        @param	s	string representing name to search for
        @return	JSON-formatted string of data. Returns empty string if name is not found.
    */
            string get_json_pretty(const string& s)	{
                if(name_exists(s))	{
                    //json11::Json json = json11::Json::object { { s, this->get_value(s) } };
                    //string pretty = json.dump();
                    //pretty_form(pretty);
                    string pretty = get_json(s);
                    pretty_form(pretty);
                    return pretty;
                } else {
                    return "";
                }
            }


            /// Convert class contents to a JSON object
            /** Returns a json11 JSON object of the COSMOS Data Structure (::cosmosstruc)
        @return	A json11 JSON object containing every member variable within the COSMOS Data Structure (::cosmosstruc)
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "timestamp" , timestamp },
                    { "jmapped" , jmapped },
                    { "ujmapped" , ujmapped },
                    { "unit" , unit },
                    { "equation" , equation },
                    { "node" , node },
                    { "vertexs" , vertexs },
                    { "normals" , normals },
                    { "faces" , faces },
                    { "pieces" , pieces },
                    { "obj" , obj },
//                    { "device" , device },
                    { "devspec" , devspec },
                    { "port" , port },
                    { "agent" , agent0 },
                    { "event" , event },
                    { "target" , target },
                    { "user" , user },
                    { "tle" , tle },
                    { "sim_states" , sim_states },
                    { "vertex_cnt" , vertex_cnt },
                    { "normal_cnt" , normal_cnt },
                    { "face_cnt" , face_cnt },
                    { "piece_cnt" , piece_cnt },
                    { "device_cnt" , device_cnt },
                    { "port_cnt" , port_cnt },
//                    { "agent_cnt" , agent_cnt },
                    { "event_cnt" , event_cnt },
                    { "target_cnt" , target_cnt },
                    { "user_cnt" , user_cnt },
                    { "tle_cnt" , tle_cnt }
                };
            }

            /// Set class contents from JSON string
            /** Parses the provided JSON-formatted string and sets the class data. String should be formatted like the string returned from #to_json()
        @param	s	JSON-formatted string to set class contents to
        @return n/a
    */
            void from_json(const string& s) {
                string error;
                json11::Json p = json11::Json::parse(s,error);
                if(error.empty()) {
                    if (!p["timestamp"].is_null()) { timestamp = p["timestamp"].number_value(); }
                    if (!p["jmapped"].is_null()) { jmapped = p["jmapped"].number_value(); }
                    if (!p["ujmapped"].is_null()) { ujmapped = p["ujmapped"].number_value(); }

                    if(!p["vertex_cnt"].is_null())	{ vertex_cnt = p["vertex_cnt"].int_value(); }
                    if(!p["normal_cnt"].is_null())	{ normal_cnt = p["normal_cnt"].int_value(); }
                    if(!p["face_cnt"].is_null())	{ face_cnt = p["face_cnt"].int_value(); }
                    if(!p["piece_cnt"].is_null())	{ piece_cnt = p["piece_cnt"].int_value(); }
                    if(!p["device_cnt"].is_null())	{ device_cnt = p["device_cnt"].int_value(); }
                    if(!p["port_cnt"].is_null())	{ port_cnt = p["port_cnt"].int_value(); }
//                    if(!p["agent_cnt"].is_null())	{ agent_cnt = p["agent_cnt"].int_value(); }
                    if(!p["event_cnt"].is_null())	{ event_cnt = p["event_cnt"].int_value(); }
                    if(!p["target_cnt"].is_null())	{ target_cnt = p["target_cnt"].int_value(); }
                    if(!p["user_cnt"].is_null())	{ user_cnt = p["user_cnt"].int_value(); }
                    if(!p["tle_cnt"].is_null())	{ tle_cnt = p["tle_cnt"].int_value(); }


                    for (size_t i = 0; i < unit.size(); ++i) {
                        for (size_t j = 0; j < unit[i].size(); ++j) {
                            if (!p["unit"][i][j].is_null()) { unit[i][j].from_json(p["unit"][i][j].dump()); }
                        }
                    }
                    for (size_t i = 0; i < equation.size(); ++i) {
                        if (!p["equation"][i].is_null()) { equation[i].from_json(p["equation"][i].dump()); }
                    }
                    if (!p["node"].is_null()) { node.from_json(p["node"].dump()); }
                    for (size_t i = 0; i < vertexs.size(); ++i) {
                        if (!p["vertexs"][i].is_null()) { vertexs[i].from_json(p["vertexs"][i].dump()); }
                    }
                    for (size_t i = 0; i < normals.size(); ++i) {
                        if (!p["normals"][i].is_null()) { normals[i].from_json(p["normals"][i].dump()); }
                    }
                    for (size_t i = 0; i < faces.size(); ++i) {
                        if (!p["faces"][i].is_null()) { faces[i].from_json(p["faces"][i].dump()); }
                    }
                    if (!p["obj"].is_null()) { cosmosstruc::obj.from_json(p["obj"].dump()); }
                    for (size_t i = 0; i < device.size(); ++i) {
                        if (!p["device"][i].is_null()) { device[i]->from_json(p["device"][i].dump()); }
                    }
                    if (!p["devspec"].is_null()) { devspec.from_json(p["devspec"].dump()); }
                    for (size_t i = 0; i < port.size(); ++i) {
                        if (!p["port"][i].is_null()) { port[i].from_json(p["port"][i].dump()); }
                    }
//                    for (size_t i = 0; i < agent.size(); ++i) {
//                        if (!p["agent"][i].is_null()) { agent[i].from_json(p["agent"][i].dump()); }
//                    }
                    if (!p["agent0"][i].is_null()) { agent0.from_json(p["agent0"].dump()); }
                    for (size_t i = 0; i < event.size(); ++i) {
                        if (!p["event"][i].is_null()) { event[i].from_json(p["event"][i].dump()); }
                    }
                    for (size_t i = 0; i < target.size(); ++i) {
                        if (!p["target"][i].is_null()) { target[i].from_json(p["target"][i].dump()); }
                    }
                    for (size_t i = 0; i < user.size(); ++i) {
                        if (!p["user"][i].is_null()) { user[i].from_json(p["user"][i].dump()); }
                    }
                    for (size_t i = 0; i < tle.size(); ++i) {
                        if (!p["tle"][i].is_null()) { tle[i].from_json(p["tle"][i].dump()); }
                    }
                    for (size_t i = 0; i < sim_states.size(); ++i) {
                        if (!p["sim_states"][i].is_null()) { sim_states[i].from_json(p["sim_states"][i].dump()); }
                    }
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }

            /// Checks if operator is left-associative
            /**	For internal use.
        @param	a	char representing operator to check left-associativity for
        @return Returns true if operator is left-associative. Returns false if not.
    */
            bool left_ass(char a)	{
                if(a == '+')	return true;
                if(a == '-')	return true;
                if(a == '*')	return true;
                if(a == '/')	return true;
                if(a == '^')	return false;

                return false;
            }

            /// Checks if two operators have equal precedence.
            /**	For internal use.
        @param	a	char representing operator
        @param	b	char representing operator
        @return Returns true if operators have equal precedence. Returns false if not.
    */
            bool equal_ass(char a, char b)	{
                if(a == '+' && b == '+')	return true;
                if(a == '+' && b == '-')	return true;
                if(a == '+' && b == '*')	return false;
                if(a == '+' && b == '/')	return false;
                if(a == '+' && b == '^')	return false;

                if(a == '-' && b == '+')	return true;
                if(a == '-' && b == '-')	return true;
                if(a == '-' && b == '*')	return false;
                if(a == '-' && b == '/')	return false;
                if(a == '-' && b == '^')	return false;

                if(a == '*' && b == '+')	return false;
                if(a == '*' && b == '-')	return false;
                if(a == '*' && b == '*')	return true;
                if(a == '*' && b == '/')	return true;
                if(a == '*' && b == '^')	return false;

                if(a == '/' && b == '+')	return false;
                if(a == '/' && b == '-')	return false;
                if(a == '/' && b == '*')	return true;
                if(a == '/' && b == '/')	return true;
                if(a == '/' && b == '^')	return false;

                if(a == '^' && b == '+')	return false;
                if(a == '^' && b == '-')	return false;
                if(a == '^' && b == '*')	return false;
                if(a == '^' && b == '/')	return false;
                if(a == '^' && b == '^')	return true;

                return false;
            }

            /// Checks for higher operator precedence
            /**	For internal use.
        @param	a	char representing operator
        @param	b	char representing operator
        @return Returns true if operator `a` has higher precedence than operator `b`. Returns false if not.
    */
            bool higher(char a, char b)	{

                if(a == '+' && b == '+')	return false;
                if(a == '+' && b == '-')	return false;
                if(a == '+' && b == '*')	return false;
                if(a == '+' && b == '/')	return false;
                if(a == '+' && b == '^')	return false;

                if(a == '-' && b == '+')	return false;
                if(a == '-' && b == '-')	return false;
                if(a == '-' && b == '*')	return false;
                if(a == '-' && b == '/')	return false;
                if(a == '-' && b == '^')	return false;

                if(a == '*' && b == '+')	return true;
                if(a == '*' && b == '-')	return true;
                if(a == '*' && b == '*')	return false;
                if(a == '*' && b == '/')	return false;
                if(a == '*' && b == '^')	return false;

                if(a == '/' && b == '+')	return true;
                if(a == '/' && b == '-')	return true;
                if(a == '/' && b == '*')	return false;
                if(a == '/' && b == '/')	return false;
                if(a == '/' && b == '^')	return false;

                if(a == '^' && b == '+')	return true;
                if(a == '^' && b == '-')	return true;
                if(a == '^' && b == '*')	return true;
                if(a == '^' && b == '/')	return true;
                if(a == '^' && b == '^')	return false;

                return false;
            }

            /// Applies an arithmetic operation between two values
            /**	Two operands are popped from the `answer` stack, and one operator is popped from the `ops` stack. The result of the operation is pushed onto the `answer` stack. For internal use.
        @param	ops		stack of operators
        @param	answer	stack of operands
        @return Returns -1 if `answer` does not contain at least 2 operands. Returns 0 if operation was successfully applied.
    */
            int apply_op(stack<char>& ops, stack<double>& answer)	{
                if(answer.size()<2) return -1;
                double b = answer.top();
                answer.pop();
                double a = answer.top();
                answer.pop();
                switch(ops.top())	{
                case '+':	answer.push(a+b);
                    break;
                case '-':	answer.push(a-b);
                    break;
                case '*':	answer.push(a*b);
                    break;
                case '/':	answer.push(a/b);
                    break;
                case '^':	answer.push(pow(a,b));
                    break;
                }
                //cout<<"		calculating "<<a<<" "<<ops.top()<<" "<<b<<" = "<<answer.top()<<endl;
                ops.pop();
                return 0;
            }

            /// Evaluate the expression of a python?Matlab? equation
            /**	Parses a python?Matlab? formatted string and evaluates the expression
        @param	str	string representing a python?Matlab? formatted expression
        @return Returns a double type result of the expression. Returns *NaN* if an error is encountered.
    */
            double equationator(const string& str)	{
                string eq(str);

                // START EQUATION PRE-PROCESSING

                // check if empty
                if(eq.empty())	return nan("");

                // check if double quotes are balanced
                int q_count = 0;
                for(string::const_iterator it = eq.begin(); it != eq.end(); ++it) {
                    if(*it=='"')	q_count++;
                }
                if(q_count%2==1)	return nan("");

                // TODO: you should never have #( or )#...  implied multiplication, but make explicit already!

                // trim leading whitespace
                const auto notwhite = eq.find_first_not_of(" \n\r\t\f\v");
                eq = eq.substr(notwhite);

                // replace "cosmos_variable_names" with values
                vector<string> replacements;
                for(size_t i = 0; i < eq.size(); ++i)	{
                    //cout<<"char = <"<<eq[i]<<">"<<endl;
                    if(eq[i]=='"')	{
                        string name("");
                        //cout<<"found opening quote"<<endl;
                        while(eq[++i]!='"'&&i<eq.size())	{ name.push_back(eq[i]); }
                        //cout<<"name = "<<name<<endl;
                        replacements.push_back(name);
                    }
                }

                for(size_t i = 0; i < replacements.size(); ++i)	{
                    string replace_me = "\"" + replacements[i] + "\"";
                    string type = get_type(replacements[i]);
                    if(type=="double")	{
                        replace(eq, replace_me, std::to_string(get_value<double>(replacements[i])));
                    } else if(type=="float")	{
                        replace(eq, replace_me, std::to_string(get_value<float>(replacements[i])));
                    } else if(type=="size_t")	{
                        replace(eq, replace_me, std::to_string(get_value<size_t>(replacements[i])));
                    } else if(type=="int")	{
                        replace(eq, replace_me, std::to_string(get_value<int>(replacements[i])));
                    } else if(type=="uint16_t")	{
                        replace(eq, replace_me, std::to_string(get_value<uint16_t>(replacements[i])));
                    } else if(type=="int16_t")	{
                        replace(eq, replace_me, std::to_string(get_value<int16_t>(replacements[i])));
                    } else if(type=="uint32_t")	{
                        replace(eq, replace_me, std::to_string(get_value<uint32_t>(replacements[i])));
                    } else if(type=="int32_t")	{
                        replace(eq, replace_me, std::to_string(get_value<int32_t>(replacements[i])));
                    } else if(type=="uint8_t")	{
                        replace(eq, replace_me, std::to_string(get_value<uint8_t>(replacements[i])));
                    } else if(type=="int8_t")	{
                        replace(eq, replace_me, std::to_string(get_value<int8_t>(replacements[i])));
                    } else if(type=="bool")	{
                        replace(eq, replace_me, std::to_string(get_value<bool>(replacements[i])));
                    } else	{
                        cout<<"type <"<<type<<"> for <"<<replacements[i]<<"> not supported"<<endl;
                        return nan("");
                    }
                }

                // replace {}[] with ()
                replace(eq, "{", "(");
                replace(eq, "[", "(");
                replace(eq, "}", ")");
                replace(eq, "]", ")");

                // replace ÷,– with /,-
                replace(eq, "÷", "/");
                replace(eq, "–", "-");

                // check if parenthesis are balanced
                int p_count = 0;
                for(string::const_iterator it = eq.begin(); it != eq.end(); ++it) {
                    if(*it=='(')	p_count++;
                    if(*it==')')	p_count--;
                    if(p_count<0)	return nan("");
                }
                if(p_count!=0)	return nan("");


                // START EQUATION PROCESSING

                string			output;
                stack<double>	answer;
                stack<char>		ops;

                //int count = 0;
                for(string::const_iterator it = eq.begin(); it != eq.end(); ++it) {
                    // debug
                    // cout<<"char #"<<count++<<" = '"<<*it<<"' :\n\t<"<<output<<">"<<endl;
                    // cout<<"\t: operators = <";
                    // for(stack<char> op = ops; !op.empty(); op.pop())	{ cout<<op.top()<<" "; }
                    // cout<<">"<<endl;

                    // all letters should have been replaced by values
                    if(isalpha(*it))	return nan("");
                    // skip all whitespace
                    if(isspace(*it))	continue;
                    // if token is number
                    if(isdigit(*it)||*it=='.')	{
                        bool negative = false;
                        if(*(it-1)=='+'||*(it-1)=='-')	{
                            string::const_iterator iit = it-1;
                            if(iit==eq.begin())	{
                                if(*(it-1)=='-')	negative = true;
                                ops.pop();
                            } else {
                                while(iit--!=eq.begin())	{
                                    if(isspace(*iit))	continue;
                                    if(*iit=='('||*iit=='+'||*iit=='-'||*iit=='*'||*iit=='/'||*iit=='^')	{
                                        if(*(it-1)=='-')	negative = true;
                                        ops.pop();
                                        break;
                                    } else	{
                                        break;
                                    }
                                }
                                if(iit==eq.begin() && (*iit==' '||*iit=='\t'||*iit=='\n'))	{
                                    if(*(it-1)=='-')	negative = true;
                                    ops.pop();
                                }
                            }
                        }
                        vector<int> integer, fraction;
                        if(isdigit(*it))	{
                            integer.push_back(*it-'0');
                        } else {
                            integer.push_back(0); --it;
                        }
                        while(isdigit(*(it+1)))	{ integer.push_back(*(++it)-'0'); }
                        if(*(it+1)=='.')	{ ++it; while(isdigit(*(it+1)))	{ fraction.push_back(*(++it)-'0'); } }
                        double numnum = 0.;
                        for(size_t i = 0; i < integer.size(); ++i)	{ numnum += integer[i]*1.0 * pow(10, integer.size()-i-1); }
                        for(size_t i = 0; i < fraction.size(); ++i)	{ numnum += fraction[i]*1.0 * pow(10.0, -(i+1.0)); }
                        if(negative) numnum *= -1.;
                        stringstream ss;
                        ss<<setprecision(std::numeric_limits<double>::digits10)<<numnum;
                        output += ss.str() + " ";
                        answer.push(numnum);
                        continue;
                    }
                    // if token is operator
                    if(*it=='+'||*it=='-'||*it=='*'||*it=='/'||*it=='^')	{
                        if((*it=='+'||*it=='-')&&(isdigit(*(it+1))||*(it+1)=='.'))	{
                            if(it==eq.begin())	{ ops.push(*it); continue; }
                            string::const_iterator t = it-1;
                            while(t!=eq.begin() && (*t==' '||*t=='\n'||*t=='\t')) --t;
                            if(*t=='+'||*t=='-'||*t=='*'||*t=='/'||*t=='^')	{
                                // only gently pushed, will be popped when negative number found
                                ops.push(*it);
                                continue;
                            }
                        }
                        while(	!ops.empty() &&
                                ( higher(ops.top(), *it) || (equal_ass(ops.top(), *it) && left_ass(*it)) ) &&
                                ops.top()!='('
                                )	{ output += string(1,(*it)) + " "; if(apply_op(ops, answer)<0) return nan(""); }
                        ops.push(*it);
                    } else if(*it == '(')	{
                        ops.push(*it);
                    } else if(*it == ')')	{
                        while(ops.top()!='(')	{
                            output += string(1,(*it)) + " ";
                            if(apply_op(ops, answer)<0) return nan("");
                        }
                        if(ops.top()=='(')	{ if(ops.empty()) return nan(""); else ops.pop(); }
                    }
                }
                while(!ops.empty())	{ output += string(1,ops.top()) + " "; if(apply_op(ops, answer)<0) return nan(""); }
                return answer.top();
            }

            // other namespace member functions??
            // maybe set_json for use with namespace names (calls from_json...)
        }; // End cosmosstruc

        /// Class to represent cosmosstruc data using mysql database tables
        /**
            This class defines which sub-sections of the cosmosstruc go into which tables.
            A sub-section of the cosmosstruc is defined as a set of namespace names.

            Multiple sub-sections may be stored in the same table, provided they each have appropriate types (e.g. multiple batteries are stored in one battstruc table)
            The same sub-setion may be stored in multiple tables (if that should ever prove useful)

            A single mapping is provided by each instance of this class

        */
        class cosmos2table	{
        public:

            string			schema_name;
            string			table_name;
            vector<string>	column_names;
            vector<string>	namespace_names;
            //! Specify either "insert" or "replace"
            string          insert_type = "insert";

            cosmos2table(const string& s_name, const string& t_name, const vector<string>& c_names, const vector<string>& n_names) :
                schema_name(s_name),
                table_name(t_name),
                column_names(c_names),
                namespace_names(n_names)
            {};

            cosmos2table(const string& s_name, const string& t_name, const vector<string>& c_names, const vector<string>& n_names, const string& insert_type) :
                schema_name(s_name),
                table_name(t_name),
                column_names(c_names),
                namespace_names(n_names),
                insert_type(insert_type)
            {};

            //! Get formatted string value for namespace value
            //! \param C cosmosstruc to refer to
            //! \param name Namespace name
            //! \return Formatted string value if found, empty string on failure
            string insert_value(cosmosstruc& C, const string& name);

            //! Generate insert statement for all rows of this table
            //! \param C cosmosstruc to refer to
            //! \return Insert statement on success, empty string on failure
            string insert_statement(cosmosstruc& C);
        };

        /// Class to represent individual instances of mysql databases (i.e. schemas)
        /**
            Each database (i.e. schema) is a collection of table definitions.
            Each table supports a mapping of columns to cosmosstruc entries.
        */
        class DBSchema
        {
        public:
            //! Database name
            string					schema_name;
            //! Collection of tables
            vector<cosmos2table>	tables;
            //! The cosmosstruc this schema refers to
            cosmosstruc*            cinfo;

            DBSchema(const string& schema_name) : schema_name(schema_name) {}

            //! Inititializes default schema which covers the essential cosmosstruc structures
            //! \param db_cinfo Cosmosstruc to have this database refer to
            void init(cosmosstruc* db_cinfo);

            //! Find the index of the specified table name
            //! \param name Table name
            //! \return 0 on success, negative on error
            int32_t find_table(const string& name);

            //! Generate insert statement for the specified table
            //! \param name Table name
            //! \return String insert statement on success, empty string on failure
            string table_insert_statement(const string& name);

            //! Generate insert statements for the specified list of tables
            //! \param name Table names
            //! \return Vector of string insert statements on success, empty strings will not be added to return vector
            vector<string> multi_table_insert_statement(const vector<string>& names);

            //! Create create database statement for this database
            //! \return Create statement on success, no failure state
            string init_database();
        };
        //! @}
    } // End namespace Support
} // End namespace Cosmos

#endif
