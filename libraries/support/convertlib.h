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
#include "support/timelib.h"
#include "support/geomag.h"
#include "support/ephemlib.h"
namespace Cosmos {
    namespace Convert {


        //! \ingroup convertlib
        //! \defgroup convertlib_functions Coordinate conversion functions
        //! @{

        // TODO: implement using pos_geoc2geod
        int32_t loc_clear(locstruc *loc);
        int32_t pos_extra(double utc, locstruc *loc);
        int32_t pos_lvlh(locstruc *loc);
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
        int32_t pos_geoc2geod(locstruc *loc);
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
        int32_t pos_lvlh2origin(locstruc *loc);
        int32_t pos_origin2lvlh(locstruc *loc);
        int32_t pos_geoc2lvlh(locstruc *geoc, locstruc *base);
        int32_t pos_lvlh2geoc(locstruc *base, locstruc *geoc);

        int32_t loc_clear(locstruc &loc);
        int32_t pos_extra(double utc, locstruc &loc);
        int32_t pos_lvlh(locstruc &loc);
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
        int32_t pos_geoc2geod(locstruc &loc);
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
        int32_t pos_lvlh2origin(locstruc &loc);
        int32_t pos_origin2lvlh(locstruc &loc);
        int32_t pos_geoc2lvlh(locstruc &geoc, locstruc &base);
        int32_t pos_lvlh2geoc(locstruc &base, locstruc &geoc);

        int32_t eci2kep(cartpos &eci, kepstruc &kep);
        int32_t kep2eci(kepstruc &kep,cartpos &eci);
        int32_t cart2peri(cartpos cart, Quaternion &qperi);
        int32_t peri2cart(cartpos cart, Quaternion &qcart);
        int32_t ric2eci(cartpos orig, rvector ric, cartpos& result);
        int32_t ric2eci(cartpos orig, Vector ric, cartpos& result);
        int32_t ric2lvlh(double radius, cartpos ric, cartpos& lvlh);
        int32_t lvlh2ric(double radius, cartpos lvlh, cartpos& ric);
        cartpos eci2lvlh(cartpos origin, cartpos point);

        /**
         * @brief This function finds the interceptor's hill (relative) interceptor vectors
         * given the ECI target and ECI interceptor vectors.
         * Routine not dependent on km or m for distance unit, but needs to be consistent!
         * author        : sal alfano         719-573-2600   13 aug 2010
         * author        : SG                 July 2024 (conversion to C++)
         * 
         * @param tgteci Target's position/velocity in ECI
         * @param inteci Interceptor's position/velocity in ECI
         * @return Interceptor's position/velocity in Hill-frame (relative) vectors
         *      Output is the following axis definitions, which is the COSMOS LVLH frame:
         *      0: Normal to orbital plane in direction of velocity vector
         *      1: Radial vector
         *      2: Cross-product
         */
        Convert::cartpos eci2hill(const Convert::cartpos& tgteci, const Convert::cartpos& inteci);

        /**
         * @brief This function finds the interceptor's ECI pos/vel/acc vectors
         * given the ECI target and hill (relative) interceptor vectors.
         * Routine not dependent on km or m for distance unit, but needs to be consistent!
         * author        : sal alfano         719-573-2600   13 aug 2010
         * author        : SG                 July 2024 (conversion to C++, added acceleration)
         * 
         * @param tgteci Target's position/velocity/acceleration in ECI
         * @param inthill Interceptor's position/velocity/acceleration in Hill-frame (relative) vectors.
         *      Input expects the following axis definitions, which is the COSMOS LVLH frame:
         *      0: Normal to orbital plane in direction of velocity vector
         *      1: Radial vector
         *      2: Cross-product
         * @param relative_accel If true, the acceleration returned is relative to the target (i.e., when desiring external forces only)
         * @return Interceptor's position/velocity/acceleration in ECI
         */
        Convert::cartpos hill2eci (const Convert::cartpos& tgteci, const Convert::cartpos& inthill, bool relative_accel=false);


        locstruc shape2eci(double utc, double altitude, double angle, double timeshift);
        locstruc shape2eci(double utc, double latitude, double longitude, double altitude, double angle, double timeshift);

        int32_t att_extra(locstruc *loc);
        int32_t att_icrf(locstruc *loc);
        int32_t att_lvlh(locstruc *loc);
        int32_t att_geoc(locstruc *loc);
        int32_t att_selc(locstruc *loc);
        int32_t att_topo(locstruc *loc);
        int32_t att_planec2topo(locstruc *loc);
        int32_t att_topo2planec(locstruc *loc);
        int32_t att_icrf2geoc(locstruc *loc);
        int32_t att_icrf2lvlh(locstruc *loc);
        int32_t att_icrf2selc(locstruc *loc);
        int32_t att_geoc2icrf(locstruc *loc);
        int32_t att_planec2lvlh(locstruc *loc);
        int32_t att_lvlh2planec(locstruc *loc);
        int32_t att_lvlh2icrf(locstruc *loc);
        int32_t att_selc2icrf(locstruc *loc);
        int32_t loc_update(locstruc *loc);

        int32_t att_extra(locstruc &loc);
        int32_t att_clear(attstruc &att);
        int32_t att_icrf(locstruc &loc);
        int32_t att_lvlh(locstruc &loc);
        int32_t att_geoc(locstruc &loc);
        int32_t att_selc(locstruc &loc);
        int32_t att_topo(locstruc &loc);
        int32_t att_planec2topo(locstruc &loc);
        int32_t att_topo2planec(locstruc &loc);
        int32_t att_icrf2geoc(locstruc &loc);
        int32_t att_icrf2geoc(locstruc &loc);
        int32_t att_icrf2lvlh(locstruc &loc);
        int32_t att_icrf2selc(locstruc &loc);
        int32_t att_geoc2icrf(locstruc &loc);
        int32_t att_planec2lvlh(locstruc &loc);
        int32_t att_lvlh2planec(locstruc &loc);
        int32_t att_lvlh2icrf(locstruc &loc);
        int32_t att_selc2icrf(locstruc &loc);
        int32_t loc_update(locstruc &loc);

        double mjd2gmst(double mjd);
        int32_t geod2utm(geoidpos &geod, Vector &utm);
        int32_t geoc2geod(Convert::cartpos &geoc, geoidpos &geod);
        int32_t geos2geoc(Convert::spherpos *geos, cartpos *geoc);
        int32_t geod2geoc(geoidpos &geod, cartpos &geoc);
        int32_t geoc2geos(Convert::cartpos *geoc, spherpos *geos);
        int32_t selg2selc(geoidpos *selg, cartpos *selc);
        int32_t gcrf2itrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
        int32_t itrs2gcrf(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm);
        int32_t true2pef(double utc, rmatrix *rm);
        int32_t pef2true(double utc, rmatrix *rm);
        int32_t pef2itrs(double utc, rmatrix *rm);
        int32_t itrs2pef(double utc, rmatrix *rm);
        int32_t true2mean(double ep1, rmatrix *pm);
        int32_t mean2true(double ep0, rmatrix *pm);
        int32_t j20002mean(double ep1, rmatrix *pm);
        int32_t mean2j2000(double ep0, rmatrix *pm);
        int32_t gcrf2j2000(rmatrix *rm);
        int32_t j20002gcrf(rmatrix *rm);
        int32_t teme2true(double ep0, rmatrix *rm);
        int32_t true2teme(double ep0, rmatrix *rm);
        int32_t mean2mean(double ep0, double ep1, rmatrix *pm);
        int32_t geoc2topo(gvector gs, rvector geoc, rvector &topo);
        int32_t body2topo(Vector com, Vector body, Vector &topo);
        int32_t topo2azel(rvector tpos, float &az, float &el);
        int32_t topo2azel(Vector tpos, float &az, float &el);
        int32_t sat2geoc(rvector sat, locstruc &loc, rvector &pos);
        int32_t geod2sep(gvector src, gvector dst, double &sep);
        double geod2sep(gvector src, gvector dst);
        int lines2eci(double mjd, vector<tlestruc> tle, cartpos &eci);
        int tle2eci(double mjd, tlestruc tle, cartpos &eci);
        double atan3(double sa, double cb);
        int32_t eci2tle(cartpos eci, tlestruc &tle);
        int32_t eci2tle2(cartpos eci, tlestruc &tle);
//        int32_t rv2tle(double utc, cartpos eci, tlestruc &tle);
        int sgp4(double utc, tlestruc tle, cartpos &pos_teme);
        tlestruc get_line(uint16_t index, vector<tlestruc> tle);
        int32_t load_lines(string fname, vector<tlestruc>& tle);
        int32_t load_lines_multi(string fname, vector<tlestruc>& tle);
        int32_t loadTLE(char *fname, tlestruc &tle);
        int32_t load_tle(char *fname, tlestruc &tle);
        int32_t load_tle(string fname, tlestruc &tle);
        int32_t load_stk(string filename, stkstruc &stkdata);
        int stk2eci(double utc, stkstruc &stk, cartpos &eci);
        int32_t tle2sgp4(tlestruc tle, sgp4struc &sgp4);
        int32_t sgp42tle(sgp4struc sgp4, tlestruc &tle);
        int tle_checksum(const char *line);
        string eci2tlestring(Convert::cartpos eci, tlestruc &reftle);
        string tle2tlestring(tlestruc tles);
        rvector utc2nuts(double mjd);
        double utc2depsilon(double mjd);
        double utc2dpsi(double mjd);
        double utc2gast(double mjd);
        double utc2ut1(double mjd);
        double utc2dut1(double mjd);
        int32_t load_iers();
        cvector polar_motion(double mjd);
        int32_t leap_seconds(double mjd);
        double utc2era(double mjd);
        double utc2gmst1982(double mjd);
        double utc2gmst2000(double mjd=0.);
        double utc2jcenut1(double mjd);
        double utc2tt(double mjd);
        double utc2jcentt(double mjd);
        double utc2epsilon(double mjd);
        double utc2L(double mjd);
        double utc2Lp(double mjd);
        double utc2F(double mjd);
        double utc2D(double mjd);
        double utc2omega(double mjd);
        double utc2zeta(double mjd);
        double utc2z(double mjd);
        double utc2gps(double utc);
        double utc2tdb(double mjd);
        double utc2theta(double mjd);
        double  tt2utc(double mjd);
        double  gps2utc(double gps);
        double Rearth(double lat);

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
        //! @}
	json11::Json make_swarm_information_object(const string& name, const string& desc, const vector<string>& node_names);
	json11::Json make_sensor_information_object(const string& sensor_name, const double& fov, const double& ifov);
	json11::Json make_constraints_information_object(const double& max_slew_rate = 0.0, const double& max_thrust_total = 0.0, const double& max_thrust_impulse = 0.0);
	json11::Json make_tle_information_object(const string& tle_file = "/home/user/cosmos/source/core/data/demo/tle.dat");
	json11::Json make_node_information_object(json11::Json swarm_info_obj);
    } // end Convert namespace
} // end Cosmos namespace
#endif
