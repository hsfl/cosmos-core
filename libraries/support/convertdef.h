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

/*! \file convertlib.h
	\brief convertlib include file
	A library providing functions for conversion between the various
	position and attitude coordinate systems used in orbital mechanics.
	*/

#ifndef _CONVERTDEF_H
#define _CONVERTDEF_H 1

#include "configCosmos.h"

#include "mathlib.h"
//#include "jpleph.h"
//#include "timelib.h"

//#include <cmath>
//#include <ctime>
//#include <cerrno>
//#include <cstring>
//#include <iostream>
using namespace std;

//! \ingroup convertlib
//! \defgroup convertlib_constants Coordinate conversion constants
//! @{

//! SI Stefan-Boltzmann constant
#define SIGMA 5.67e-8
//! SI Gravitational constant
//#define GC 6.672e-11
#define SOL (86400.*2.99792458e8)
//! SI Radius of Sun
#define RSUNM ((double)6.955e8)
//! SI Radius of Moon
#define RMOONM ((double)1738000.)
//! SI Radius of Earth
#define REARTHM ((double)6378137.)
//! Radius of Earth in Km
#define REARTHKM ((double)6378.137)
//! Flattening of Earth
#define FLATTENING (1./298.257223563)
#define FRATIO (1.-FLATTENING)
#define FRATIO2 (FRATIO*FRATIO)
//! SI Astronomical Unit
#define AU ((double)1.49597870691e11)
//! SI Mass of Sun
#define MSUN 1.9891e30
//! SI Mass of Earth
#define MEARTH 5.9742e24
//! SI Mass of Moon
#define MMOON 7.3477e22
//! SI Mass of Jupiter
#define MJUPITER 1.8966e27
//! SI Mass * Gravitational Constant for Earth
//#define GM ((double)(3.98600441789e14))
#define GM ((double)(3.986004415e14))
//! SI Mass * Gravitational Constant for Moon
#define GMOON  ((double)(4.9028029535968e+12))
//! SI Mass * Gravitational Constant for Sun
#define GSUN  ((double)(1.327122e20))
//! SI Mass * Gravitational Constant for Jupiter
#define GJUPITER ((double)(1.266865349218008e17))
//! Latitude of HIGP, in radians
#define LAT_HIGP RADOF(21.297440)
//! Longitude of HIGP, in radians
#define LON_HIGP RADOF(157.81630)
//! Altitude of HIGP, in meters
#define ALT_HIGP 33.
//! Geocentric location of Honolulu
#define GEOC_HONOLULU {{-5588871.,-2278876.,2353191.}}

// SGP4 stuff
#define SGP4_CK2 5.413080E-4
#define SGP4_CK4 0.62098875E-6
#define SGP4_E6A 1E-6
#define SGP4_QOMS2T 1.88027916E-9
#define SGP4_S 1.01222928
#define SGP4_TOTHRD 2./3.
#define SGP4_XJ3 -0.253881E-5
#define SGP4_XKE 0.743669161E-1
#define SGP4_XKMPER 6378.135
#define SGP4_AE 1.

// JPL Planetary Ephemeris stuff
#define JPL_MERCURY 1
#define JPL_VENUS 2
#define JPL_EARTH 3
#define JPL_MARS 4
#define JPL_JUPITER 5
#define JPL_SATURN 6
#define JPL_URANUS 7
#define JPL_NEPTUNE 8
#define JPL_PLUTO 9
#define JPL_MOON 10
#define JPL_SUN 11
#define JPL_SUN_BARY 12
#define JPL_EARTH_BARY 13
#define JPL_NUTATIONS 14

// Two Line Element
#define MAXTLE 5000

//! @}

//! \ingroup demlib
//! \defgroup demlib_planets DEM Planetary bodies
//! @{

// COSMOS Planetary Bodies
#define COSMOS_MERCURY JPL_MERCURY
#define COSMOS_VENUS JPL_VENUS
#define COSMOS_EARTH JPL_EARTH
#define COSMOS_MARS JPL_MARS
#define COSMOS_JUPITER JPL_JUPITER
#define COSMOS_SATURN JPL_SATURN
#define COSMOS_URANUS JPL_URANUS
#define COSMOS_NEPTUNE JPL_NEPTUNE
#define COSMOS_PLUTO JPL_PLUTO
#define COSMOS_MOON JPL_MOON
#define COSMOS_SUN JPL_SUN

//! @}

//! \ingroup convertlib
//! \defgroup convertlib_typedefs Coordinate conversion typedefs
//! @{

//! Cartesian full position structure
/*!
* Cartesian position structure with three vectors specifying position,
* velocity and acceleration.
*/
typedef struct
{
	//! UTC of Position
	double utc;
	//! Location
	rvector s;
	//! Velocity
	rvector v;
	//! Acceleration
	rvector a;
	//! pass indicator: allows synchronization with other attitude and position values.
	uint32_t pass;
} cartpos;

std::ostream& operator << (std::ostream& out, const cartpos& a);
std::istream& operator >> (std::istream& in, cartpos& a);

//! Cartesian position with time
/*! Cartesian position structure with an additional time element.
	\see ::cartpos
*/
typedef struct
{
	//! UTC as Modified Julian Day
	double utc;
	//! Cartesian structure with all elements of position
	cartpos pos;
} cposstruc;

std::ostream& operator << (std::ostream& out, const cposstruc& a);
std::istream& operator >> (std::istream& in, cposstruc& a);

//! Geodetic position structure
/*! Structure containing 3 geodetic vectors representing position, velocity and acceleration.
	\see ::gvector
*/
typedef struct
{
	double utc;
	//! Position vector
	gvector s;
	//! Velocity vector
	gvector v;
	//! Acceleration vector
	gvector a;
	//! pass indicator: allows synchronization with other attitude and position values.
	uint32_t pass;
} geoidpos;

std::ostream& operator << (std::ostream& out, const geoidpos& a);
std::istream& operator >> (std::istream& in, geoidpos& a);

//! Spherical position structure
/*! Structure containing 3 spherical vectors representing position, velocity and acceleration.
	\see ::svector
*/
typedef struct
{
	double utc;
	//! Position vector
	svector s;
	//! Velocity vector
	svector v;
	//! Acceleration vector
	svector a;
	//! pass indicator: allows synchronization with other attitude and position values.
	uint32_t pass;
} spherpos;

std::ostream& operator << (std::ostream& out, const spherpos& a);
std::istream& operator >> (std::istream& in, spherpos& a);

typedef struct
{
	double utc;
	avector s;
	avector v;
	avector a;
} aattstruc;

std::ostream& operator << (std::ostream& out, const aattstruc& a);
std::istream& operator >> (std::istream& in, aattstruc& a);

typedef struct
{
	double utc;
	quaternion s;
	rvector v;
	rvector a;
} quatatt;

std::ostream& operator << (std::ostream& out, const quatatt& a);
std::istream& operator >> (std::istream& in, quatatt& a);

//! Direction Cosine Matrix Attitude
/*! Full attitude structure (0th, 1st and 2nd derivative), using a Direction Cosine Matrix
 * to present the 0th derivative.
*/
typedef struct
{
	double utc;
	//! 0th derivative
	rmatrix s;
	//! 1st derivative
	rvector v;
	//! 2nd derivative
	rvector a;
} dcmatt;

std::ostream& operator << (std::ostream& out, const dcmatt& a);
std::istream& operator >> (std::istream& in, dcmatt& a);

//! Quaternion Attitude
/*! Full attitude structure (0th, 1st and 2nd derivative), using a Quaternion
 * to present the 0th derivative.
 *
*/
typedef struct
{
	double utc;
	//! 0th derivative: Quaternion
	quaternion s; //q
	//! 1st derivative: Omega - angular velocity
	//!
	rvector v;  //dq
	//! 2nd derivative: Alpha - acceleration
	rvector a;  //ddq
	//! pass indicator: allows synchronization with other attitude and position values.
	uint32_t pass;
} qatt;

std::ostream& operator << (std::ostream& out, const qatt& a);
std::istream& operator >> (std::istream& in, qatt& a);

//! Classical elements structure
/*! Position structure representing the full state vector as classical Keplerian elements.
*/
typedef struct
{
	//! UTC time of state vector in MJD
	double utc;
	//! Orbit number
	uint32_t orbit;
	//! Orbital Period in seconds
	double period;
	//! Semi-Major Axis in meters
	double a;
	//! Eccentricity
	double e;
	//! Angular Momentum vector
	rvector h;
	//! Solar Beta Angle in radians
	double beta;
	double eta;
	//! Orbital Inclination in radians
	double i;
	//! Right Ascension of the Ascending Node in radians
	double raan;
	//! Argument of Perigee
	double ap;
	//! Argument of Latitude
	double alat;
	//! Mean Anomoly
	double ma;
	//! True Anomoly
	double ta;
	//! Eccentric Anomoly
	double ea;
	//! Mean Motion
	double mm;
	double fa;
} kepstruc;

std::ostream& operator << (std::ostream& out, const kepstruc& a);
std::istream& operator >> (std::istream& in, kepstruc& a);

typedef struct
{
	double sepangle;
	double size;
	double radiance;
} bodypos;

std::ostream& operator << (std::ostream& out, const bodypos& a);
std::istream& operator << (std::istream& in, bodypos& a);

//! Additional parameters relating to position that need only be calculated once.
typedef struct
{
	//! Coordinated Universal Time
	double utc;
	//! Terrestrial Time
	double tt;
	//! UT0
	double ut;
	//! Dynamical Barycentric Time
	double tdb;
	//! Transform from ICRF to Geocentric
	rmatrix j2e;
	rmatrix dj2e;
	rmatrix ddj2e;
	//! Transform from Geocentric to ICRF
	rmatrix e2j;
	rmatrix de2j;
	rmatrix dde2j;
	rmatrix j2t;
	rmatrix j2s;
	rmatrix t2j;
	rmatrix s2j;
	rmatrix s2t;
	rmatrix ds2t;
	rmatrix t2s;
	rmatrix dt2s;
	cartpos sun2earth;
	cartpos sun2moon;
	uint16_t closest;
} extrapos;

std::ostream& operator << (std::ostream& out, const extrapos& a);
std::istream& operator >> (std::istream& in, extrapos& a);

//! Additional parameters relating to position that need only be calculated once.
typedef struct
{
	//! Coordinated Universal Time
	double utc;
	//! Transform from ICRF to Body frame
	rmatrix j2b;
	//! Transform from Body frame to ICRF
	rmatrix b2j;
} extraatt;

std::ostream& operator << (std::ostream& out, const extraatt& a);
std::istream& operator >> (std::istream& in, extraatt& a);

typedef struct
{
	double utc;
	cartpos icrf;
	cartpos eci;
	cartpos sci;
	cartpos geoc;
	cartpos selc;
	geoidpos geod;
	geoidpos selg;
	spherpos geos;
	extrapos extra;
	float earthsep;
	float moonsep;
	float sunsize;
	float sunradiance;
} posstruc;

std::ostream& operator << (std::ostream& out, const posstruc& a);
std::istream& operator >> (std::istream& in, posstruc& a);

typedef struct
{
	double utc;
	qatt topo;
	qatt lvlh;
	qatt geoc;
	qatt selc;
	qatt icrf;
	extraatt extra;
} attstruc;

std::ostream& operator << (std::ostream& out, const attstruc& a);
std::istream& operator >> (std::istream& in, attstruc& a);

typedef struct
{
	//! Master time for location, in Modified Julian Day
	double utc;
	//! ::posstruc for this time.
	posstruc pos;
	//! ::attstruc for this time.
	attstruc att;
	//! Earth magnetic vector in ITRS for this time and location.
	rvector bearth;
	//! Decimal Orbit number
	double orbit;
} locstruc;

std::ostream& operator << (std::ostream& out, const locstruc& a);
std::istream& operator >> (std::istream& in, locstruc& a);

struct tlestruc
{
	double utc;
	char name[25];
	uint16_t snumber;
	char id[9];
	double bstar;
	double i;
	double raan;
	double e;
	double ap;
	double ma;
	double mm;
	uint32_t orbit;
};

//! STK positions structure
/*! Structure for holding an array of position structures generated by STK.
	\see ::cposstruc
*/
typedef struct
{
	//! Number of positions
	int32_t count;
	//! Time step in Modified Julian Days
	double dt;
	//! Array of positions
	cposstruc *pos;
} stkstruc;

//! @}

#endif
