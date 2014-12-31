/*! \file convertlib.h
	\brief convertlib include file
	A library providing functions for conversion between the various
	position and attitude coordinate systems used in orbital mechanics.
*/

//! \ingroup support
//! \defgroup convertlib Coordinate conversion library
//! Coordinate conversion support library.
//!
//! COSMOS supports a wide variety of coordinate systems for both position and attitude. This
//! library provides both definitions of structures for representing these various systems, as
//! well as functions for conversion between them. A number of constants are also defined that
//! relate to the various physical parameters involved in these conversions.
//!
//! COSMOS approaches the process of defining position and attitude by defining something called
//! a Location. A fully defined Location, stored in a ::locstruc, represents the complete state
//! vector for an object, including time, represented in all possible coordinate systems. This
//! Location is first broken down in to a ::posstruc and ::attstruc, representing Position and
//! Attitude. These are subsequently broken down in to the various possible representations in
//! the various systems supported.
//!
//! Position is actually a triple of the 0th, 1st and 2nd derivative of position, measured
//! in either meters, or meters and radians, with respect
//! to a standardly defined coordinate system. The systems currently supported are:
//! - Barycentric: Cartesian, ICRF centered on Solar System barycenter
//! - Earth Centered Inertial: Cartesian, ICRF centered on instantaneous Earth position
//! - Selene Centered Inertial: Cartesian, ICRF centered on instantaneous Moon position
//! - Geocentric: Cartesian, ITRF centered on instantaneous Earth position
//! - Selenocentric: Cartesian, lunar pole and equator of epoch and the IAU node of epoch
//! centered on instantaneous Moon position
//! - Geodetic: Oblate Spherical, ITRF centered on instantaneous Earth position
//! - Selenographic: Spherical, lunar pole and equator of epoch and the IAU node of epoch
//! - Geospherical: Spherical, ITRF centered on instantaneous Earth position
//!
//! The ::posstruc also contains some extra derivative information such as:
//! - angular separation between Earth and Sun
//! - angular separation between Moon and Sun
//! - angular size of Sun
//! - Solar radiance
//! - Sun to Earth vector
//! - Sun to Moon vector
//!
//! Attitude is also a triple of the 0th, 1st and 2nd derivatives. The 1st and 2nd derivatives,
//! or Omega and Alpha, are represented as vectors in the relevant coordinate system. The 0th
//! derivative is represented as the quaternion that will transform a vector in the relevant
//! coordinate system into a vector in the body frame. The systems currently supported are:
//! - Topocentric: Geocentric based, East North Up
//! - Local Vertical Local Horizontal: Geocentric based, +z = -position, +y = +z cross velocity, +x = +y cross +z
//! - Geocentric: Geocentric based
//! - Selenocentric:: Selenocentric based
//! - ICRF: ICRF based
//!
//! Additionally, support is provided for Two Line Element and table based coordinates.

#ifndef _CONVERTLIB_H
#define _CONVERTLIB_H 1

#include "configCosmos.h"

#include "datalib.h"
#include "mathlib.h"
#include "jpleph.h"
#include "timelib.h"
#include "memlib.h"
#include "convertdef.h"
//#include "demlib.h"
#include "geomag.h"

#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <cstring>

//! \ingroup convertlib
//! \defgroup convertlib_functions Coordinate conversion functions
//! @{

void loc_clear(locstruc *loc);
void jplnut(double mjd, double nuts[]);
void jplpos(long from, long to, double mjd, cartpos *pos);
void jpllib(double utc,rmatrix *rm, rmatrix *drm);
//void jpllib(double utc, aattstruc *pos);
void geoc2geod(cartpos *geoc, geoidpos *geod);
void geos2geoc(spherpos *geos, cartpos *geoc);
void geod2geoc(geoidpos *geod, cartpos *geoc);
void geoc2geos(cartpos *geoc, spherpos *geos);
void selg2selc(geoidpos *selg, cartpos *selc);
void pos_extra(locstruc *loc);
void pos_clear(locstruc *loc);
void pos_baryc(locstruc *loc);
void pos_eci(locstruc *loc);
void pos_sci(locstruc *loc);
void pos_geoc(locstruc *loc);
void pos_geos(locstruc *loc);
void pos_geod(locstruc *loc);
void pos_selc(locstruc *loc);
void pos_selg(locstruc *loc);
void pos_baryc2eci(locstruc *loc);
void pos_eci2baryc(locstruc *loc);
void pos_baryc2sci(locstruc *loc);
void pos_sci2baryc(locstruc *loc);
void pos_eci2geoc(locstruc *loc);
void pos_eci2selc(locstruc *loc);
void pos_geoc2eci(locstruc *loc);
void pos_geoc2geod(locstruc *loc);
void pos_geod2geoc(locstruc *loc);
void pos_geoc2geos(locstruc *loc);
void pos_geos2geoc(locstruc *loc);
void pos_eci2sci(locstruc *loc);
void pos_sci2eci(locstruc *loc);
void pos_sci2selc(locstruc *loc);
void pos_selc2selg(locstruc *loc);
void pos_selc2sci(locstruc *loc);
void pos_selg2selc(locstruc *loc);
void pos_selc2eci(locstruc *loc);
void eci2kep(cartpos *eci,kepstruc *kep);
void kep2eci(kepstruc *kep,cartpos *eci);
double rearth(double lat);
double mjd2year(double mjd);
void att_extra(locstruc *loc);
void att_clear(locstruc *loc);
void att_icrf(locstruc *loc);
void att_lvlh(locstruc *loc);
void att_geoc(locstruc *loc);
void att_selc(locstruc *loc);
void att_topo(locstruc *loc);
void att_planec2topo(locstruc *loc);
void att_topo2planec(locstruc *loc);
void att_icrf2geoc(locstruc *loc);
void att_icrf2lvlh(locstruc *loc);
void att_icrf2selc(locstruc *loc);
void att_geoc2icrf(locstruc *loc);
void att_planec2lvlh(locstruc *loc);
void att_lvlh2planec(locstruc *loc);
void att_lvlh2icrf(locstruc *loc);
void att_selc2icrf(locstruc *loc);
void loc_update(locstruc *loc);
double mjd2gmst(double mjd);
void euler2dcm( char *order, double phi, double theta, double psi, double rmat[3][3] );
void icrs2itrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
void itrs2icrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
void true2mean(double ep1, rmatrix *pm);
void mean2true(double ep0, rmatrix *pm);
void icrs2mean(double ep1, rmatrix *pm);
void mean2icrs(double ep0, rmatrix *pm);
void mean2mean(double ep0, double ep1, rmatrix *pm);
void geoc2topo(gvector gs, rvector geoc, rvector *topo);
void topo2azel(rvector tpos, float *az, float *el);
int lines2eci(double mjd, vector<tlestruc> tle, cartpos *eci);
int line2eci(double mjd, tlestruc tle, cartpos *eci);
tlestruc get_line(uint16_t index, vector<tlestruc> tle);
int32_t load_lines(char *fname, vector<tlestruc>& tle);
int32_t load_stk(char *filename, stkstruc *stkdata);
int stk2eci(double utc, stkstruc *stk, cartpos *eci);

//! @}

#endif
