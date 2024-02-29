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

#include "support/configCosmos.h"

#include "math/mathlib.h"

namespace Cosmos {
    namespace Convert {

        //! \ingroup convertlib
        //! \defgroup convertlib_constants Coordinate conversion constants
        //! @{

        // TODO: move these constants to physics/constants.h
        //! Speed of light
#define CLIGHT 299792458.
        //! SI Stefan-Boltzmann constant
#define SIGMA 5.67e-8
#define PLANCK1U 3.741771852e8
#define PLANCK2U 1.438776877e4
        //! SI Gravitational constant
        //#define GC 6.672e-11
#define SOL (86400.*2.99792458e8)
        //! SI Radius of Sun
#define RSUNM static_cast<double>(6.955e8)
        //! SI Radius of Moon
#define RMOONM static_cast<double>(1738000.)
        //! SI Astronomical Unit
#define AU static_cast<double>(1.49597870691e11)
        //! SI Mass of Sun
#define MSUN 1.9891e30
        //! SI Mass of Earth
#define MEARTH 5.9742e24
        //! SI Mass of Moon
#define MMOON 7.3477e22
        //! SI Mass of Jupiter
#define MJUPITER 1.8966e27
        //! SI Mass * Gravitational Constant for Earth
        //#define GM static_cast<double>((3.98600441789e14))
#define GM static_cast<double>((3.986004415e14))
#define GM2 (GM*GM)
#define GM3 (GM*GM2)
        //! SI Mass * Gravitational Constant for Moon
#define GMOON  static_cast<double>((4.9028029535968e+12))
        //! SI Mass * Gravitational Constant for Sun
#define GSUN  static_cast<double>((1.327122e20))
        //! SI Mass * Gravitational Constant for Jupiter
#define GJUPITER static_cast<double>((1.266865349218008e17))
        //! Latitude of HIGP, in radians
#define LAT_HIGP RADOF(21.297440)
        //! Longitude of HIGP, in radians
#define LON_HIGP RADOF(157.81630)
        //! Altitude of HIGP, in meters
#define ALT_HIGP 33.
        //! Geocentric location of Honolulu
#define GEOC_HONOLULU {-5588871.,-2278876.,2353191.}

        // TODO: create SGP4 module
        // SGP4 constants
#define SGP4_CK2 5.413080E-4
#define SGP4_CK4 0.62098875E-6
#define SGP4_E6A 1E-6
#define SGP4_QOMS2T 1.88027916E-9
#define SGP4_S 1.01222928
#define SGP4_TOTHRD 2./3.
#define SGP4_XJ3 -0.253881E-5
#define SGP4_XKE 0.743669161E-1
#define SGP4_XKMPER REARTHKM
        //#define SGP4_XKMPER 6378.135
#define SGP4_XMNPDA 1440.0
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
        struct cartpos
        {
            //! UTC of Position
            double utc = 0.;
            //! Location
            rvector s = {0., 0., 0.};
            //! Velocity
            rvector v = {0., 0., 0.};
            //! Acceleration
            rvector a = {0., 0., 0.};
            //! Jerk
            rvector j = {0., 0., 0.};
            // TODO: get rid of this kind of stuff?
            //! pass indicator: allows synchronization with other attitude and position values.
            uint32_t pass = 0;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc" , utc },
                    { "s", s },
                    { "v", v },
                    { "a", a },
                    { "j", j },
                    { "pass", static_cast<int>(pass) }
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
                    if(!parsed["utc"].is_null())	utc = parsed["utc"].number_value();
                    if(!parsed["s"].is_null())		s.from_json(parsed["s"].dump());
                    if(!parsed["v"].is_null())		v.from_json(parsed["v"].dump());
                    if(!parsed["a"].is_null())		a.from_json(parsed["a"].dump());
                    if(!parsed["j"].is_null())		j.from_json(parsed["j"].dump());
                    if(!parsed["pass"].is_null())	pass = parsed["pass"].int_value();
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const cartpos& a);
        ::std::istream& operator >> (::std::istream& in, cartpos& a);

        //! Cartesian position with time
        /*! Cartesian position structure with an additional time element.
        \see ::cartpos
*/
        struct cposstruc
        {
            //! UTC as Modified Julian Day
            double utc = 0.;
            //! Cartesian structure with all elements of position
            cartpos pos;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc", utc },
                    { "pos", pos }
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
                    if(!parsed["utc"].is_null())    utc = parsed["utc"].number_value();
                    if(!parsed["pos"].is_null())    pos.from_json(parsed["pos"].dump());
                } else {
                    cerr << "ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const cposstruc& a);
        ::std::istream& operator >> (::std::istream& in, cposstruc& a);

        //! Geodetic position structure
        /*! Structure containing 3 geodetic vectors representing position, velocity and acceleration.
        \see ::gvector
*/
        class geoidpos
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(utc) + 3 * sizeof(s) + sizeof(pass);
                return total;
            }

            double utc = 0.;
            //! Position vector
            gvector s;
            //! Velocity vector
            gvector v;
            //! Acceleration vector
            gvector a;
            //! pass indicator: allows synchronization with other attitude and position values.
            uint32_t pass = 0;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc" , utc },
                    { "s", s },
                    { "v", v },
                    { "a", a },
                    { "pass", static_cast<int>(pass) }
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
                    if(!parsed["utc"].is_null())	utc = parsed["utc"].number_value();
                    if(!parsed["s"].is_null())		s.from_json(parsed["s"].dump());
                    if(!parsed["v"].is_null())		v.from_json(parsed["v"].dump());
                    if(!parsed["a"].is_null())		a.from_json(parsed["a"].dump());
                    if(!parsed["pass"].is_null())	pass = parsed["pass"].int_value();
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const geoidpos& a);
        ::std::istream& operator >> (::std::istream& in, geoidpos& a);

        //! Spherical position structure
        /*! Structure containing 3 spherical vectors representing position, velocity and acceleration.
        \see ::svector
*/
        class spherpos
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(utc) + 3 * sizeof(s) + sizeof(pass);
                return total;
            }

            double utc = 0.;
            //! Position vector
            svector s;
            //! Velocity vector
            svector v;
            //! Acceleration vector
            svector a;
            //! pass indicator: allows synchronization with other attitude and position values.
            uint32_t pass = 0;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc" , utc },
                    { "s", s },
                    { "v", v },
                    { "a", a },
                    { "pass", static_cast<int>(pass) }
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
                    if(!parsed["utc"].is_null())	utc = parsed["utc"].number_value();
                    if(!parsed["s"].is_null())		s.from_json(parsed["s"].dump());
                    if(!parsed["v"].is_null())		v.from_json(parsed["v"].dump());
                    if(!parsed["a"].is_null())		a.from_json(parsed["a"].dump());
                    if(!parsed["pass"].is_null())	pass = parsed["pass"].int_value();
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const spherpos& a);
        ::std::istream& operator >> (::std::istream& in, spherpos& a);

        struct aattstruc
        {
            double utc = 0.;
            avector s;
            avector v;
            avector a;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc" , utc },
                    { "s" , s },
                    { "v" , v },
                    { "a" , a }
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
                    if(!parsed["utc"].is_null())	utc = parsed["utc"].number_value();
                    if(!parsed["s"].is_null())		s.from_json(parsed["s"].dump());
                    if(!parsed["v"].is_null())		v.from_json(parsed["v"].dump());
                    if(!parsed["a"].is_null())		a.from_json(parsed["a"].dump());
                } else  {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const aattstruc& a);
        ::std::istream& operator >> (::std::istream& in, aattstruc& a);

        // TODO: add struct description
        struct quatatt
        {
            double utc;
            quaternion s;
            rvector v;
            rvector a;
        };

        ::std::ostream& operator << (::std::ostream& out, const quatatt& a);
        ::std::istream& operator >> (::std::istream& in, quatatt& a);

        //! Direction Cosine Matrix Attitude
        /*! Full attitude structure (0th, 1st and 2nd derivative), using a Direction Cosine Matrix
* to present the 0th derivative.
*/
        struct dcmatt
        {
            double utc = 0.;
            //! 0th derivative
            rmatrix s;
            //! 1st derivative
            rvector v;
            //! 2nd derivative
            rvector a;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc" , utc },
                    //{ "s", s },
                    { "v", v },
                    { "a", a }
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
                    if(!parsed["utc"].is_null())	utc = parsed["utc"].number_value();
                    //if(!parsed["s"].is_null())		s.from_json(parsed["s"].dump());
                    if(!parsed["v"].is_null())		v.from_json(parsed["v"].dump());
                    if(!parsed["a"].is_null())		a.from_json(parsed["a"].dump());
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const dcmatt& a);
        ::std::istream& operator >> (::std::istream& in, dcmatt& a);

        //! Quaternion Attitude
        /*! Full attitude structure (0th, 1st and 2nd derivative), using a Quaternion
* to present the 0th derivative.
*
*/
        class qatt
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(utc) + sizeof(pass) + sizeof(s) + 2 * sizeof(v);
                return total;
            }

            double utc = 0.;
            //! 0th derivative: Quaternion
            quaternion s = {{0., 0., 0.}, 0.}; //q
            //! 1st derivative: Omega - angular velocity
            rvector v = {0., 0., 0.};  //dq
            //! 2nd derivative: Alpha - acceleration
            rvector a = {0., 0., 0.};  //ddq
            //! pass indicator: allows synchronization with other attitude and position values.
            uint32_t pass = 0;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc" , utc },
                    { "s" , s },
                    { "v" , v },
                    { "a" , a },
                    { "pass" , static_cast<int>(pass) }
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
                    if(!parsed["utc"].is_null())    utc = parsed["utc"].number_value();
                    if(!parsed["s"].is_null())    s.from_json(parsed["s"].dump());
                    if(!parsed["v"].is_null())    v.from_json(parsed["v"].dump());
                    if(!parsed["a"].is_null())    a.from_json(parsed["a"].dump());
                    if(!parsed["pass"].is_null())    pass = parsed["pass"].int_value();
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const qatt& a);
        ::std::istream& operator >> (::std::istream& in, qatt& a);

        //! Classical elements structure
        /*! Position structure representing the full state vector as classical Keplerian elements.
*/
        struct kepstruc
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
            double dmm;
            double ddmm;
            double fa;
        };

        ::std::ostream& operator << (::std::ostream& out, const kepstruc& a);
        ::std::istream& operator >> (::std::istream& in, kepstruc& a);

        // TODO: add struct description
        struct bodypos
        {
            double sepangle;
            double size;
            double radiance;
        };

        ::std::ostream& operator << (::std::ostream& out, const bodypos& a);
        ::std::istream& operator << (::std::istream& in, bodypos& a);

        //! Additional parameters relating to position that need only be calculated once.
        struct extrapos
        {
            //! Coordinated Universal Time
            double utc = 0.;
            //! Terrestrial Time
            double tt = 0.;
            //! UT0
            double ut = 0.;
            //! Dynamical Barycentric Time
            double tdb = 0.;
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
            //! Transform between LVLH and GEOC
            quaternion l2e;
            quaternion e2l;
            //! Transform between LVLH and ECI
            rmatrix l2p;
            rmatrix p2l;
            rmatrix dl2p;
            rmatrix dp2l;
            rmatrix ddl2p;
            rmatrix ddp2l;
            //! Misc
            cartpos sun2earth;
            gvector sungeo;
            cartpos sun2moon;
            gvector moongeo;
            uint16_t closest = 0;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc"   , utc },
                    { "tt"    , tt },
                    { "ut"    , ut },
                    { "tdb"   , tdb },

                    { "j2e"   , j2e },
                    { "dj2e"  , dj2e },
                    { "ddj2e" , ddj2e },

                    { "e2j"   , e2j },
                    { "de2j"  , de2j },
                    { "dde2j" , dde2j },

                    { "j2t"   , j2t },
                    { "j2s"   , j2s },
                    { "t2j"   , t2j },
                    { "s2j"   , s2j },
                    { "s2t"   , s2t },
                    { "ds2t"  , ds2t },
                    { "t2s"   , t2s },
                    { "dt2s"  , dt2s },

                    { "l2e" , l2e },
                    { "e2l" , e2l },

                    { "sun2earth" , sun2earth },
                    { "sun2moon"  , sun2moon },
                    { "closest"   , closest }
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
                    if(!p["utc"].is_null())     utc = p["utc"].number_value();
                    if(!p["tt"].is_null())      tt = p["tt"].number_value();
                    if(!p["ut"].is_null())      ut = p["ut"].number_value();
                    if(!p["tdb"].is_null())     tdb = p["tdb"].number_value();

                    if(!p["j2e"].is_null())     j2e.from_json(p["j2e"].dump());
                    if(!p["dj2e"].is_null())    dj2e.from_json(p["dj2e"].dump());
                    if(!p["ddj2e"].is_null())   ddj2e.from_json(p["ddj2e"].dump());

                    if(!p["e2j"].is_null())     e2j.from_json(p["e2j"].dump());
                    if(!p["de2j"].is_null())    de2j.from_json(p["de2j"].dump());
                    if(!p["dde2j"].is_null())   dde2j.from_json(p["dde2j"].dump());

                    if(!p["j2t"].is_null())     j2t.from_json(p["j2t"].dump());
                    if(!p["j2s"].is_null())     j2s.from_json(p["j2s"].dump());
                    if(!p["t2j"].is_null())     t2j.from_json(p["t2j"].dump());
                    if(!p["s2j"].is_null())     s2j.from_json(p["s2j"].dump());
                    if(!p["s2t"].is_null())     s2t.from_json(p["s2t"].dump());
                    if(!p["ds2t"].is_null())    ds2t.from_json(p["ds2t"].dump());
                    if(!p["t2s"].is_null())     t2s.from_json(p["t2s"].dump());
                    if(!p["dt2s"].is_null())    dt2s.from_json(p["dt2s"].dump());

                    if(!p["l2e"].is_null())     l2e.from_json(p["l2e"].dump());
                    if(!p["e2l"].is_null())     e2l.from_json(p["e2l"].dump());

                    if(!p["sun2earth"].is_null())   sun2earth.from_json(p["sun2earth"].dump());
                    if(!p["sun2moon"].is_null())    sun2moon.from_json(p["sun2moon"].dump());
                    if(!p["closest"].is_null()) closest = p["closest"].int_value();
                } else {
                    cerr<<"ERROR: <"<<error<<">"<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const extrapos& a);
        ::std::istream& operator >> (::std::istream& in, extrapos& a);

        //! In units **for** the SGP4 propagator (not NORAD TLE itself).
        struct tlestruc
        {
            double utc = 0.;
            // JIMNOTE: remove magic number
//            string name = string(25, ' ');
            string name = " ";
            uint16_t snumber = 0;
            string id = " ";
            //! Drag (1/Earth radii)
            double bstar = 0.;
            //! Inclination (radians)
            double i = 0.;
            //! Right ascension of ascending node (radians)
            double raan = 0.;
            //! Eccentricity (unitless)
            double e = 0.;
            //! Argument of perigee (radians)
            double ap = 0.;
            //! Mean anomaly (radians)
            double ma = 0.;
            //! Mean motion (radians / minute)
            double mm = 0.;
            //! Mean motion first derivative (radians / minute)
            double dmm = 0.;
            //! Mean motion second derivative (radians / minute)
            double ddmm = 0.;
            uint32_t orbit = 0;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc" , utc },
                    { "name" , name },
                    { "snumber" , snumber },
                    { "id" , id },
                    { "bstar" , bstar },
                    { "i" , i },
                    { "raan" , raan },
                    { "e" , e },
                    { "ap" , ap },
                    { "ma" , ma },
                    { "mm" , mm },
                    { "dmm" , dmm },
                    { "ddmm" , ddmm },
                    { "orbit" , static_cast<int>(orbit) }
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
                    if(!parsed["utc"].is_null())    utc =  parsed["utc"].number_value();
                    if(!parsed["name"].is_null())    name = parsed["name"].string_value();
                    if(!parsed["snumber"].is_null())    snumber =  parsed["snumber"].int_value();
                    if(!parsed["id"].is_null())    id = parsed["id"].string_value();
                    if(!parsed["bstar"].is_null())    bstar =  parsed["bstar"].number_value();
                    if(!parsed["i"].is_null())    i =  parsed["i"].number_value();
                    if(!parsed["raan"].is_null())    raan =  parsed["raan"].number_value();
                    if(!parsed["e"].is_null())    e =  parsed["e"].number_value();
                    if(!parsed["ap"].is_null())    ap =  parsed["ap"].number_value();
                    if(!parsed["ma"].is_null())    ma =  parsed["ma"].number_value();
                    if(!parsed["mm"].is_null())    mm =  parsed["mm"].number_value();
                    if(!parsed["dmm"].is_null())    dmm =  parsed["dmm"].number_value();
                    if(!parsed["ddmm"].is_null())    ddmm =  parsed["ddmm"].number_value();
                    if(!parsed["orbit"].is_null())    orbit =  parsed["orbit"].int_value();
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const tlestruc& a);
        ::std::istream& operator >> (::std::istream& in, tlestruc& a);

        //! Additional parameters relating to position that need only be calculated once.
        class extraatt
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(utc) + 2 * sizeof(j2b);
                return total;
            }

            //! Coordinated Universal Time
            double utc = 0.;
            //! Transform from ICRF to Body frame
            rmatrix j2b;
            //! Transform from Body frame to ICRF
            rmatrix b2j;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc" , utc },
                    { "j2b" , j2b },
                    { "b2j" , b2j }
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
                    if(!parsed["utc"].is_null())     utc =  parsed["utc"].number_value();
                    if(!parsed["j2b"].is_null())    j2b.from_json(parsed["j2b"].dump());
                    if(!parsed["b2j"].is_null())    b2j.from_json(parsed["b2j"].dump());
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const extraatt& a);
        ::std::istream& operator >> (::std::istream& in, extraatt& a);

        struct posstruc
        {
            double utc = 0.;
            cartpos icrf;
            cartpos eci;
            cartpos sci;
            cartpos geoc;
            cartpos selc;
            cartpos lvlh;
            geoidpos geod;
            geoidpos selg;
            spherpos geos;
            extrapos extra;
            //! Separation between sun/satellite and sun/limbofearth vectors in radians
            float earthsep = 0.f;
            //! Separation between sun/satellite and sun/limbofmoon vectors in radians
            float moonsep = 0.f;
            //! Radius of sun in radians
            float sunsize = 0.f;
            //! Watts per square meter per steradian
            float sunradiance = 0.f;
            //! Earth magnetic vector in ITRS for this time and location.
            rvector bearth;
            //! Decimal Orbit number
            double orbit = 0.;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc"  , utc },
                    { "icrf" , icrf },
                    { "eci"  , eci },
                    { "sci"  , sci },
                    { "geoc" , geoc },
                    { "selc" , selc },
                    { "lvlh" , lvlh },
                    { "geod" , geod },
                    { "selg" , selg },
                    { "geos" , geos },
                    { "extra", extra },

                    { "earthsep" , earthsep },
                    { "moonsep"  , moonsep },
                    { "sunsize"  , sunsize },
                    { "sunradiance" , sunradiance },
                    { "bearth"   , bearth },
                    { "orbit"    , orbit }
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
                    if(!parsed["utc"].is_null())    utc = parsed["utc"].number_value();
                    if(!parsed["icrf"].is_null())   icrf.from_json(parsed["icrf"].dump());
                    if(!parsed["eci"].is_null())    eci.from_json(parsed["eci"].dump());
                    if(!parsed["sci"].is_null())    sci.from_json(parsed["sci"].dump());
                    if(!parsed["geoc"].is_null())   geoc.from_json(parsed["geoc"].dump());
                    if(!parsed["selc"].is_null())   selc.from_json(parsed["selc"].dump());
                    if(!parsed["lvlh"].is_null())   lvlh.from_json(parsed["lvlh"].dump());
                    if(!parsed["geod"].is_null())   geod.from_json(parsed["geod"].dump());
                    if(!parsed["selg"].is_null())   selg.from_json(parsed["selg"].dump());
                    if(!parsed["geos"].is_null())   geos.from_json(parsed["geos"].dump());
                    if(!parsed["extra"].is_null())  extra.from_json(parsed["extra"].dump());

                    if(!parsed["earthsep"].is_null())    earthsep = parsed["earthsep"].number_value();
                    if(!parsed["moonsep"].is_null())     moonsep = parsed["moonsep"].number_value();
                    if(!parsed["sunsize"].is_null())     sunsize = parsed["sunsize"].number_value();
                    if(!parsed["sunradiance"].is_null()) sunradiance = parsed["sunradiance"].number_value();

                    if(!parsed["bearth"].is_null())   bearth.from_json(parsed["bearth"].dump());
                    if(!parsed["orbit"].is_null())    orbit = parsed["orbit"].number_value();

                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const posstruc& a);
        ::std::istream& operator >> (::std::istream& in, posstruc& a);

        // TODO: add struct description
        class attstruc
        {
        public:
            size_t memoryusage()
            {
                size_t total = sizeof(utc);
                total += topo.memoryusage();
                total += lvlh.memoryusage();
                total += geoc.memoryusage();
                total += selc.memoryusage();
                total += icrf.memoryusage();
                total += extra.memoryusage();
                return total;
            }

            double utc = 0.;
            qatt topo;
            qatt lvlh;
            qatt geoc;
            qatt selc;
            qatt icrf;
            extraatt extra;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc"  , utc },
                    { "topo" , topo },
                    { "lvlh" , lvlh },
                    { "geoc" , geoc },
                    { "selc" , selc },
                    { "icrf" , icrf },
                    { "extra" , extra }
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
                    if(!parsed["utc"].is_null())     utc =  parsed["utc"].number_value();
                    if(!parsed["topo"].is_null())    topo.from_json(parsed["utc"].dump());
                    if(!parsed["lvlh"].is_null())    lvlh.from_json(parsed["lvlh"].dump());
                    if(!parsed["geoc"].is_null())    geoc.from_json(parsed["geoc"].dump());
                    if(!parsed["selc"].is_null())    selc.from_json(parsed["selc"].dump());
                    if(!parsed["icrf"].is_null())    icrf.from_json(parsed["icrf"].dump());
                    if(!parsed["extra"].is_null())   extra.from_json(parsed["extra"].dump());
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const attstruc& a);
        ::std::istream& operator >> (::std::istream& in, attstruc& a);

        // TODO: add struct description
        struct locstruc
        {
            //! Master time for location, in Modified Julian Day
            double utc = 0.;
            tlestruc tle;
            //! Cosmos::Support::posstruc for this time.
            posstruc pos;
            //! Cosmos::Support::attstruc for this time.
            attstruc att;

            /// Convert class contents to JSON object
            /** Returns a json11 JSON object of the class
        @return	A json11 JSON object containing every member variable within the class
    */
            json11::Json to_json() const {
                return json11::Json::object {
                    { "utc" , utc },
                    { "tle" , tle },
                    { "pos" , pos },
                    { "att" , att }
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
                    if(!parsed["utc"].is_null())    utc =  parsed["utc"].number_value();
                    if(!parsed["tle"].is_null())   tle.from_json(parsed["tle"].dump());
                    if(!parsed["pos"].is_null())    pos.from_json(parsed["pos"].dump());
                    if(!parsed["att"].is_null())    att.from_json(parsed["att"].dump());
                } else {
                    cerr<<"ERROR = "<<error<<endl;
                }
                return;
            }
        };

        ::std::ostream& operator << (::std::ostream& out, const locstruc& a);
        ::std::istream& operator >> (::std::istream& in, locstruc& a);

        //! STK positions structure
        /*! Structure for holding an array of position structures generated by STK.
        \see ::cposstruc
*/
        struct stkstruc
        {
            //! Number of positions
            size_t count;
            //! Time step in Modified Julian Days
            double dt;
            //! Array of positions
            cposstruc *pos;
        };

        //! @}

        // Name clashes with literature... these are published units (in TLE file), not for use in the SGP4 propogator itself.
        struct sgp4struc
        {
            // Inclination (deg)
            double i;
            // Eccentricity
            double e;
            // Right-ascension of the Ascending Node (deg)
            double raan;
            // Argument of Perigee (deg)
            double ap;
            //B-Star drag term
            double bstar;
            // Mean Motion (rev/day)
            double mm;
            // Mean Anomaly (deg)
            double ma;
            // Epoch (year.day)
            double ep = 0.;
        };
    }
}

#endif
