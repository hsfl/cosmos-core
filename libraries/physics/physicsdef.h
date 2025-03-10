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
#include "math/vector.h"
#include "support/convertdef.h"

using namespace Cosmos::Math::Vectors;
using namespace Cosmos::Math::Quaternions;

namespace Cosmos {
    namespace Physics {

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

        //! SI Radius of Earth
#define REARTHM static_cast<double>(6378137.)
        //! Radius of Earth in Km
#define REARTHKM static_cast<double>(6378.137)
        //! Flattening of Earth
#define FLATTENING (1./298.257223563)
#define FRATIO (1.-FLATTENING)
#define FRATIO2 (FRATIO*FRATIO)

        //! @}

        //! \ingroup physicslib
        //! \defgroup physicslib_typedefs Physics typedefs
        //! @{

        //! Gauss Jackson Integration structure
        /*! Holds the working variables for one step of an order N Gauss Jackson
 * Integration. A complete integration will require an array of N+2.
*/
        struct gjstruc
        {
            Convert::locstruc sloc;
            double a[MAXGJORDER+1];
            double b[MAXGJORDER+1];
            rvector s;
            rvector ss;
            rvector sa;
            rvector sb;
            //	rvector q;
            rvector tau;
        };

        //! Gauss-Jackson integration handle
        /*! Holds all the parameters necessary to run a single instance of a GJ integration.
 */
        struct gj_handle
        {
            vector< vector<int32_t> > binom;
            vector<double> c;
            vector<double> gam;
            vector< vector<double> > beta;
            vector<double> q;
            vector<double> lam;
            vector< vector<double> > alpha;
            double dt;
            double dtsq;
            uint32_t order;
            uint32_t order2;
            vector<gjstruc> step;
        };

        //! Sensor coverage values

        struct coverage
        {
            double percent = 0.;
            double area = 0.;
            double resolution = 0.;
            double resstd = 0.;
            double specmin;
            double specmax;
            double azimuth = 0.;
            double azstd = 0.;
            double elevation = 0.;
            double elstd = 0.;
            uint32_t count = 0;
        };

        //! Finite Triangle Element
        //! Holds minimum information necessary to use smallest possible triangular element
        //! of a larger piece.
        //struct trianglestruc
        //{
        //    //! center of mass
        //    Vector com;
        //    //! outward facing normal
        //    Vector normal;
        //    //! Area
        //    float area;
        //    //! Index to parent piece
        //    uint16_t pidx;
        //    uint16_t tidx[3];
        //    float heat;
        //    float temp;
        //    float irradiance;
        //    vector<vector<size_t>> triangleindex;
        //};

        //! Satellite structure as triangles
        //struct structurestruc
        //{
        //    vector <Vector> vertices;
        //    vector <trianglestruc> triangles;
        //};

        //! Physics Simulation Structure
        /*! Holds parameters used specifically for the physical simulation of the
 * environment and hardware of a Node.
*/
        //typedef struct
        //{
        //	//! Time step in seconds
        //	double dt;
        //	//! Time step in Julian days
        //	double dtj;
        //	//! Simulated starting time in MJD
        //	double mjdbase;
        //	//! Acceleration factor for simulated time
        //	double mjdaccel;
        //	//! Offset factor for simulated time (simtime = mjdaccel * realtime + mjddiff)
        //	double mjddiff;
        //	//! Simulation mode as listed in \def defs_physics
        //	int32_t mode;
        //    float heat = 300. * 900. * 1.;
        //    Vector ftorque;
        //    Vector atorque;
        //    Vector rtorque;
        //    Vector gtorque;
        //    Vector htorque;
        //    Vector hmomentum;
        //    Vector ctorque;
        //    Vector fdrag;
        //    Vector adrag;
        //    Vector rdrag;
        //    Vector thrust;
        //    Vector moi = Vector(1.,1.,1.);
        //    Vector com;
        //    float hcap = 900.;
        //    float mass = 1.;
        //    float area = .001f;
        //    vector <Vector> vertices;
        //    vector <trianglestruc> triangles;
        //} physicsstruc;
        //! @}
    }
}
#endif // ORBITDEF_H
