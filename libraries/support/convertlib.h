/*! \file convertlib.h
	\brief convertlib include file
	A library providing functions for conversion between the various
	position and attitude coordinate systems used in orbital mechanics.
*/

//! \ingroup support
//! \defgroup convertlib Coordinate conversion library
//! Coordinate conversion.
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
//! - Barycentric: Cartesian, ICRF centered on Solar System Barycenter
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

#include "support/configCosmos.h"

#include "math/mathlib.h"
using namespace Cosmos::Math::Matrices;
using namespace Cosmos::Math::Vectors;
using namespace Cosmos::Math::Quaternions;
#include "support/convertdef.h"

//#include <fcntl.h>
//#include <cmath>
//#include <ctime>
//#include <cerrno>
//#include <cstring>

//! \ingroup convertlib
//! \defgroup convertlib_functions Coordinate conversion functions
//! @{

void loc_clear(locstruc *loc);
void loc_clear(locstruc &loc);
// TODO: implement using pos_geoc2geod
void geoc2geod(cartpos &geoc, geoidpos &geod);
void geos2geoc(spherpos *geos, cartpos *geoc);
void geod2geoc(geoidpos &geod, cartpos &geoc);
void geoc2geos(cartpos *geoc, spherpos *geos);
void selg2selc(geoidpos *selg, cartpos *selc);
int32_t pos_extra(locstruc *loc);
int32_t pos_icrf(locstruc *loc);
int32_t pos_eci(locstruc *loc);
int32_t pos_sci(locstruc *loc);
int32_t pos_geoc(locstruc *loc);
int32_t pos_geos(locstruc *loc);
int32_t pos_geod(locstruc *loc);
int32_t pos_selc(locstruc *loc);
int32_t pos_selg(locstruc *loc);
int32_t pos_icrf2eci(locstruc *loc);
int32_t pos_eci2icrf(locstruc *loc);
int32_t pos_icrf2sci(locstruc *loc);
int32_t pos_sci2icrf(locstruc *loc);
int32_t pos_eci2geoc(locstruc *loc);
int32_t pos_eci2selc(locstruc *loc);
int32_t pos_geoc2eci(locstruc *loc);
void pos_geoc2geod(locstruc *loc);
int32_t pos_geod2geoc(locstruc *loc);
int32_t pos_geoc2geos(locstruc *loc);
int32_t pos_geos2geoc(locstruc *loc);
int32_t pos_eci2sci(locstruc *loc);
int32_t pos_sci2eci(locstruc *loc);
int32_t pos_sci2selc(locstruc *loc);
int32_t pos_selc2selg(locstruc *loc);
int32_t pos_selc2sci(locstruc *loc);
int32_t pos_selg2selc(locstruc *loc);
int32_t pos_selc2eci(locstruc *loc);

int32_t pos_extra(locstruc &loc);
int32_t pos_clear(locstruc &loc);
int32_t pos_icrf(locstruc &loc);
int32_t pos_eci(locstruc &loc);
int32_t pos_sci(locstruc &loc);
int32_t pos_geoc(locstruc &loc);
int32_t pos_geos(locstruc &loc);
int32_t pos_geod(locstruc &loc);
int32_t pos_selc(locstruc &loc);
int32_t pos_selg(locstruc &loc);
int32_t pos_icrf2eci(locstruc &loc);
int32_t pos_eci2icrf(locstruc &loc);
int32_t pos_icrf2sci(locstruc &loc);
int32_t pos_sci2icrf(locstruc &loc);
int32_t pos_eci2geoc(locstruc &loc);
int32_t pos_eci2selc(locstruc &loc);
int32_t pos_geoc2eci(locstruc &loc);
void pos_geoc2geod(locstruc &loc);
int32_t pos_geod2geoc(locstruc &loc);
int32_t pos_geoc2geos(locstruc &loc);
int32_t pos_geos2geoc(locstruc &loc);
int32_t pos_eci2sci(locstruc &loc);
int32_t pos_sci2eci(locstruc &loc);
int32_t pos_sci2selc(locstruc &loc);
int32_t pos_selc2selg(locstruc &loc);
int32_t pos_selc2sci(locstruc &loc);
int32_t pos_selg2selc(locstruc &loc);
int32_t pos_selc2eci(locstruc &loc);

void eci2kep(cartpos &eci, kepstruc &kep);
void kep2eci(kepstruc &kep,cartpos &eci);
double rearth(double lat);
double mjd2year(double mjd);

void att_extra(locstruc *loc);
void att_icrf(locstruc *loc);
int32_t att_lvlh(locstruc *loc);
void att_geoc(locstruc *loc);
void att_selc(locstruc *loc);
int32_t att_topo(locstruc *loc);
void att_planec2topo(locstruc *loc);
void att_topo2planec(locstruc *loc);
int32_t att_icrf2geoc(locstruc *loc);
void att_icrf2lvlh(locstruc *loc);
int32_t att_icrf2selc(locstruc *loc);
void att_geoc2icrf(locstruc *loc);
void att_planec2lvlh(locstruc *loc);
void att_lvlh2planec(locstruc *loc);
int32_t att_lvlh2icrf(locstruc *loc);
void att_selc2icrf(locstruc *loc);
void loc_update(locstruc *loc);

void att_extra(locstruc &loc);
void att_clear(attstruc &att);
void att_icrf(locstruc &loc);
int32_t att_lvlh(locstruc &loc);
void att_geoc(locstruc &loc);
void att_selc(locstruc &loc);
int32_t att_topo(locstruc &loc);
void att_planec2topo(locstruc &loc);
void att_topo2planec(locstruc &loc);
int32_t att_icrf2geoc(locstruc &loc);
int32_t att_icrf2geoc(locstruc &loc);
void att_icrf2lvlh(locstruc &loc);
int32_t att_icrf2selc(locstruc &loc);
void att_geoc2icrf(locstruc &loc);
void att_planec2lvlh(locstruc &loc);
void att_lvlh2planec(locstruc &loc);
int32_t att_lvlh2icrf(locstruc &loc);
void att_selc2icrf(locstruc &loc);
void loc_update(locstruc &loc);

double mjd2gmst(double mjd);
void gcrf2itrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
void itrs2gcrf(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
void true2pef(double utc, rmatrix *rm);
void pef2true(double utc, rmatrix *rm);
void pef2itrs(double utc, rmatrix *rm);
void itrs2pef(double utc, rmatrix *rm);
void true2mean(double ep1, rmatrix *pm);
void mean2true(double ep0, rmatrix *pm);
void j20002mean(double ep1, rmatrix *pm);
void mean2j2000(double ep0, rmatrix *pm);
void gcrf2j2000(rmatrix *rm);
void j20002gcrf(rmatrix *rm);
void teme2true(double ep0, rmatrix *rm);
void true2teme(double ep0, rmatrix *rm);
void mean2mean(double ep0, double ep1, rmatrix *pm);
void geoc2topo(gvector gs, rvector geoc, rvector &topo);
void body2topo(Vector com, Vector body, Vector &topo);
void topo2azel(rvector tpos, float &az, float &el);
void topo2azel(Vector tpos, float &az, float &el);
int lines2eci(double mjd, vector<tlestruc> tle, cartpos &eci);
int tle2eci(double mjd, tlestruc tle, cartpos &eci);
int32_t eci2tle(double utc, cartpos eci, tlestruc &tle);
int sgp4(double utc, tlestruc tle, cartpos &pos_teme);
tlestruc get_line(uint16_t index, vector<tlestruc> tle);
int32_t load_lines(string fname, vector<tlestruc>& tle);
int32_t load_lines_multi(string fname, vector<tlestruc>& tle);
int32_t loadTLE(char *fname, tlestruc &tle);
int32_t load_stk(string filename, stkstruc &stkdata);
int stk2eci(double utc, stkstruc &stk, cartpos &eci);
void tle2sgp4(tlestruc tle, sgp4struc &sgp4);
void sgp42tle(sgp4struc sgp4, tlestruc &tle);
int tle_checksum(char *line);
void eci2tlestring(cartpos eci, string &tle, std::string ref_tle, double bstar=0);

namespace Cosmos {
    namespace Coords {

        class Position {

        public:

            explicit Position(Vector s0=Vector(), Vector v0=Vector(), Vector a0=Vector())
            {
                s = s0;
                v = v0;
                a = a0;
            }

            // 0th derivative
            Vector s;
            // 1st derivative
            Vector v;
            // 2nd derivative
            Vector a;
        };
    }
}

//! @}

#endif
