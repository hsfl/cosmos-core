#ifndef _ORBITDEF_H
#define _ORBITDEF_H

/*! \file physicsdef.h
* \brief Orbit library support definitions
*/

#include "configCosmos.h"

#include "convertdef.h"

#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <errno.h>

//! \ingroup physicslib
//! \defgroup physicslib_constants Physics constants
//! @{

#define SUBSYS_ADCS 0
#define SUBSYS_OBCS 1
#define SUBSYS_TCS 2
#define SUBSYS_PLOAD 3
#define SUBSYS_TELECOMM 4
#define SUBSYS_EPS 5

#define FLAG_ADRAG 1
#define FLAG_SDRAG 2
#define FLAG_ATORQUE 4
#define FLAG_STORQUE 8
#define FLAG_GTORQUE 16

#define MAXGJORDER 15

#define GRAVITY_PGM2000A 1
#define GRAVITY_EGM2008 2
#define GRAVITY_PGM2000A_NORM 3
#define GRAVITY_EGM2008_NORM 4

//! @}

//! \ingroup physicslib
//! \defgroup physicslib_typedefs Physics typedefs
//! @{

//! Gauss Jackson Integration structure
/*! Holds the working variables for one step of an order N Gauss Jackson
 * Integration. A complete integration will require an array of N+2.
*/
typedef struct
{
	locstruc sloc;
	double a[MAXGJORDER+1];
	double b[MAXGJORDER+1];
	rvector s;
	rvector ss;
	rvector sa;
	rvector sb;
	//	rvector q;
	rvector tau;
} gjstruc;

//! Physics Simulation Structure
/*! Holds parameters used specifically for the physical simulation of the
 * environment and hardware of a Node.
*/
typedef struct
{
	//! Time step in seconds
	double dt;
	//! Time step in Julian days
	double dtj;
	//! Simulated starting time in MJD
	double mjdbase;
	//! Acceleration factor for simulated time
	double mjdaccel;
	//! Offset factor for simulated time (simtime = mjdaccel * realtime + mjddiff)
	double mjddiff;
	//! Simulation mode as listed in \def defs_physics
	int32_t mode;
	float heat;
	rvector ftorque;
	rvector atorque;
	rvector rtorque;
	rvector gtorque;
	rvector htorque;
	rvector hmomentum;
	rvector ctorque;
	rvector adrag;
	rvector rdrag;
	rvector thrust;
	rvector moi;
	rvector com;
	float hcap;
	float mass;
	float area;
} physicsstruc;
//! @}

#endif // ORBITDEF_H
