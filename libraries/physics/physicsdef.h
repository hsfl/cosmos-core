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

#ifndef _ORBITDEF_H
#define _ORBITDEF_H

/*! \file physicsdef.h
* \brief Orbit library support definitions
*/

#include "support/configCosmos.h"

//#include "support/convertdef.h"

//#include <fcntl.h>
//#include <time.h>
//#include <errno.h>

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

//! Gauss-Jackson integration handle
/*! Holds all the parameters necessary to run a single instance of a GJ integration.
 */
typedef struct
{
    std::vector< std::vector<int32_t> > binom;
    std::vector<double> c;
    std::vector<double> gam;
    std::vector< std::vector<double> > beta;
    std::vector<double> q;
    std::vector<double> lam;
    std::vector< std::vector<double> > alpha;
	double dt;
	double dtsq;
	uint32_t order;
	uint32_t order2;
    std::vector<gjstruc> step;
} gj_handle;

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
	rvector fdrag;
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
