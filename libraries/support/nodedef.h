#ifndef _NODEDEF_H
#define _NODEDEF_H 1

/*! \file nodedef.h
*	\brief Node definitions file
*/


#include "configCosmos.h"

#include "cosmos-defs.h"
#include "mathlib.h"
#include "convertdef.h"
//#include "physicsdef.h"
//#include "jsondef.h"

//! \ingroup nodelib
//! \defgroup nodelib_constants Node Library constants
//! @{

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

//! \ingroup nodelib
//! \defgroup nodelib_typedefs Node Library typedefs
//! @{

//! Part structure: physical information for each piece of Node
typedef struct
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
} piecestruc;

//! General Component structure
/*! These are the elements that are repeated in all devices. Each
device specific structure has these as its first elements, followed by
any specific elements. They are all then combined together in one grand
union as a ::devicestruc.
*/
typedef struct
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
} genstruc;

//! Payload (PLOAD) structure.
/*! You can define up to ::MAXPLOADKEYCNT keys by giving them unique
 * names. You can then set double precision values for these keys in
 * the dynamic structure.
 */
typedef struct
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
	//! Number of keys being used.
	uint16_t key_cnt;
	//! Name for each key.
	uint16_t keyidx[MAXPLOADKEYCNT];
	//! Rate at which the payload generates data.
	uint32_t bps;
	//! Connection information for device.
	uint16_t portidx;
	//! Value for each key.
	float keyval[MAXPLOADKEYCNT];
} ploadstruc;

//! Sun Sensor (SSEN) Sructure
typedef struct
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
	//! Sensor alignment quaternion.
	quaternion align;
	float qva;
	float qvb;
	float qvc;
	float qvd;
	float azimuth;
	float elevation;
} ssenstruc;

//! Inertial Measurement Unit (IMU) structure
typedef struct
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
	//! Connection information for device.
	uint16_t portidx;
} imustruc;

//! Reaction Wheel structure: z axis is aligned with axis of rotation.
typedef struct
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
	//! Connection information for device.
	uint16_t portidx;
	//! Current angular velocity
	float omg;
	//! Current angular acceleration
	float alp;
	//! Requested angular velocity
	float romg;
	//! Requested angular acceleration
	float ralp;
} rwstruc;

//! Magnetic Torque Rod structure: z axis is aligned with rod.
typedef struct
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
	//! Connection information for device.
	uint16_t portidx;
	//! Requested Magnetic Moment.
	float rmom;
	//! Actual Magnetic Moment.
	float mom;
} mtrstruc;

typedef struct
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
	uint16_t portidx;
	uint32_t uptime;
	float maxdisk;
	float maxmem;
	float maxload;
	float disk;
	float mem;
	float load;
    // >> number of reboots
    uint32_t boot_count;
} cpustruc;


typedef struct
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
    //! Device information time stamp, Universal Time Coordinate (UTC) in Modified Julian Date (MJD)
	double utc;
	//! Connection information for device.
	uint16_t portidx;

	rvector position;

    rvector velocity;

    rvector position_sd;

    rvector velocity_sd;

    //! Geographic position: longitude, latitude, altitude
    gvector geo;

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

} gpsstruc;

//! Antenna information
/*! This structure holds the information concerning antennas.
*/
typedef struct
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
	//! Alignment
	quaternion align;
	//! Minimum elevation
	float minelev;
} antstruc;

//! Receiver information
/*! This structure holds the information concerning receivers.
*/
typedef struct
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
	//! Connection information for device.
	uint16_t portidx;
	//! Frequency
	float freq;
} rxrstruc;

//! Transmitter information
/*! This structure holds the information concerning transmitters.
*/
typedef struct
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
	//! Connection information for device.
	uint16_t portidx;
	//! Frequency
	float freq;
} txrstruc;

//! Transceiver information
/*! This structure holds the information concerning transceivers.
*/
typedef struct
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
	//! Connection information for device.
	uint16_t portidx;
	//! Input Frequency
	float freqin;
	//! Output Frequency
	float freqout;
} tcvstruc;

//! PV String (STRG) structure.
/*! Efficiency is goven as effbase + effslope * Tkelvin.
*/
typedef struct
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
	//! Efficiency 0th order term
	float effbase;
	//! Efficiency 1st order term
	float effslope;
	//! Maximum power generation in Watts
	float maxpower;
	//! Current power being generated in Watts.
	float power;
} strgstruc;

//! Battery (BATT) structure.
typedef struct
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
	//! Capacity in amp hours
	float capacity;
	//! Charge conversion efficiency
	float efficiency;
	//! Charge in amp hours
	float charge;
} battstruc;

//! Heater Structure definition
/*! This structure holds the description of a heaters.
*/
typedef struct
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
	//! Temperature set point
	float setpoint;
} htrstruc;

typedef struct
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
	float max;
	float rat;
	float spd;
} motrstruc;

typedef struct
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
} tsenstruc;

//! Thruster (THST) dynamic structure
typedef struct
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
	//! Flow
	quaternion align;
	//! Specific Impulse in dynes per kg per second
	float flw;
	//! Rotation of thrust vector (+z) in to node frame.
	float isp;
} thststruc;

//! Propellant Tank (PROP) structure.
typedef struct
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
	//! Propellant capacity in kg
	float cap;
	//! Propellant level in kg
	float lev;
} propstruc;

//! Switch Structure definition
/*! This structure holds the description of a switches.
*/
typedef struct
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
} swchstruc;

//! Rotor Structure definition
/*! This structure holds the description of a rotors.
*/
typedef struct
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
	//! Angular position
	float angle;
} rotstruc;

//! Star Tracker (STT) Sructure
typedef struct
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
	//! alignment quaternion
	quaternion align;
	//! Connection information for device.
	uint16_t portidx;
	//! includes 0 and 1st order derivative
	quaternion att;
	rvector omega;
	rvector alpha;
	uint16_t retcode;
	uint32_t status;
} sttstruc;

//! Motion Capture Camera (MCC) Structure
typedef struct
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
	//! Sensor alignment quaternion.
	quaternion align;
	//! Connection information for device.
	uint16_t portidx;
	//! attitude
	quaternion q;
} mccstruc;

//! Torque Rod Control Unit
typedef struct
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
	//! Connection information for device.
	uint16_t portidx;
	//! Torque Rod count
	uint16_t mcnt;
	//! Torque Rod Component indices
	uint16_t mcidx[3];
} tcustruc;

typedef struct
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
	//! Total energy usage
	float energy;
	//! Watch Dog Timer (MJD)
	float wdt;
} busstruc;

typedef struct
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
	//! Current Pressure
	float press;
} psenstruc;

//! SUCHI Sructure
typedef struct
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
	//! alignment quaternion
	quaternion align;
	//! Connection information for device.
	uint16_t portidx;
	//! Internal pressure
	float press;
	//! Internal temperatures
	float temps[8];
} suchistruc;

typedef struct
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
	uint16_t portidx;
	uint16_t pwidth;
	uint16_t pheight;
	float width;
	float height;
	float flength;
} camstruc;

//! Node Structure
//! Structure for storing all information about a Node that never changes, or only
//! changes slowly. The information for initializing this should be in node.ini.
typedef struct
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
} nodestruc;

//! Device structure
/*! Complete details of each Device. It is a union of all the
possible device types, with a generic type for looking up basic
information.
*/
typedef struct
{
	union
	{
	genstruc gen;
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
	txrstruc txr;
	rxrstruc rxr;
	thststruc thst;
	tsenstruc tsen;
	};
} devicestruc;

//! Specific Device structure
/*! Counts and arrays of pointers to each type of device, ordered by type.
*/
typedef struct
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
	uint16_t txr_cnt;
	uint16_t thst_cnt;
	uint16_t tsen_cnt;
	vector<genstruc *>gen;
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
	vector<tcvstruc *>tcv;
	vector<txrstruc *>txr;
	vector<rxrstruc *>rxr;
	vector<thststruc *>thst;
	vector<tsenstruc *>tsen;
} devspecstruc;

//! @}

#endif
