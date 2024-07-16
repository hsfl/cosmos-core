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

/*! \file convertlib.cpp
    \brief Coordinate conversion library source file
*/

#include "support/convertlib.h"
#include "support/jsondef.h"
#include "support/stringlib.h"
#include "physics/physicsclass.h"

// Used to mark unused variables as known
#ifndef UNUSED_VARIABLE_LOCALDEF
#define UNUSED_VARIABLE_LOCALDEF(x) (void)(x)
#endif // UNUSED_VARIABLE_LOCALDEF

#include <iostream>

namespace Cosmos
{
namespace Convert
{

struct iersstruc
{
    uint32_t mjd;
    double pmx;
    double pmy;
    double dutc;
    uint32_t ls;
};

static uint16_t tlecount;
static vector<iersstruc> iers;
static uint32_t iersbase = 0;

//! \addtogroup convertlib_functions
//! @{

//! Initialize ::Cosmos::Convert::locstruc.
/*! Set entire structure to zero so that we can
 * properly propagate changes.
    \param loc Pointer to ::Cosmos::Convert::locstruc that contains positions.
    attitudes.
*/
int32_t loc_clear(locstruc *loc)
{
    int32_t iretn = 0;
    iretn = loc_clear(*loc);
    return iretn;
}

int32_t loc_clear(locstruc &loc)
{
    memset(static_cast<void *>(&loc), 0, sizeof(locstruc));
    return 0;
}

//! Initialize ::Cosmos::Convert::posstruc.
/*! Set entire structure to zero so that we can
 * properly propagate changes.
    \param loc Pointer to ::Cosmos::Convert::locstruc that contains positions.
    attitudes.
*/
int32_t pos_clear(locstruc *loc)
{
    int32_t iretn = 0;
    iretn = pos_clear(*loc);
    return iretn;
}

int32_t pos_clear(locstruc &loc)
{
    memset(static_cast<void *>(&loc.pos), 0, sizeof(posstruc));
    att_clear(loc.att);
    return 0;
}

//! Initialize ::Cosmos::Convert::attstruc.
/*! Set entire structure to zero so that we can
 * properly propagate changes.
    \param att Pointer to ::Cosmos::Convert::attstruc that contains attitudes.
*/
int32_t att_clear(attstruc &att)
{
    memset(static_cast<void *>(&att), 0, sizeof(attstruc));
    return 0;
}

//! Calculate Extra position information
/*! Calculate things like sun position and insolation, and elements that are used in
 * conversion, like libration and J2000 rotation matrices.
    \param loc ::Cosmos::Convert::locstruc with the current position and those to be updated.
*/
int32_t pos_extra(double utc, locstruc *loc)
{
    return pos_extra(utc, *loc);
}

int32_t pos_extra(double utc, locstruc &loc)
{
    // Check time
    if (!isfinite(utc) || utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    // These are all based on time, so they don't need to be repeated if time hasn't changed.
    if (loc.pos.extra.utc == utc)
    {
        return 0;
    }

    double tt = utc2tt(utc);
    if (tt <= 0.)
    {
        return static_cast<int32_t>(tt);
    }
    loc.pos.extra.tt = tt;

    loc.pos.extra.utc = utc;
    loc.pos.extra.tdb = utc2tdb(utc);
    loc.pos.extra.ut = utc2ut1(utc);

    gcrf2itrs(utc, &loc.pos.extra.j2t, &loc.pos.extra.j2e, &loc.pos.extra.dj2e, &loc.pos.extra.ddj2e);
    loc.pos.extra.t2j = rm_transpose(loc.pos.extra.j2t);
    loc.pos.extra.e2j = rm_transpose(loc.pos.extra.j2e);
    loc.pos.extra.de2j = rm_transpose(loc.pos.extra.dj2e);
    loc.pos.extra.dde2j = rm_transpose(loc.pos.extra.ddj2e);

    jpllib(utc, &loc.pos.extra.s2t, &loc.pos.extra.ds2t);
    loc.pos.extra.t2s = rm_transpose(loc.pos.extra.s2t);
    loc.pos.extra.dt2s = rm_transpose(loc.pos.extra.ds2t);

    loc.pos.extra.j2s = rm_mmult(loc.pos.extra.t2s, loc.pos.extra.j2t);
    loc.pos.extra.s2j = rm_transpose(loc.pos.extra.j2s);

    jplpos(JPL_SUN_BARY, JPL_EARTH, loc.pos.extra.tt, &loc.pos.extra.sun2earth);
    loc.pos.extra.sun2earth.utc = utc;
    locstruc tloc;
    tloc.utc = utc;
    tloc.pos.extra = loc.pos.extra;
    tloc.pos.eci = loc.pos.extra.sun2earth;
    tloc.pos.eci.s = rv_smult(-1., tloc.pos.eci.s);
    pos_eci2geoc(tloc);
    loc.pos.extra.sungeo = tloc.pos.geod.s;
    jplpos(JPL_SUN_BARY, JPL_MOON, loc.pos.extra.tt, &loc.pos.extra.sun2moon);
    loc.pos.extra.sun2moon.utc = utc;
    tloc.pos.eci.s = rv_sub(loc.pos.extra.sun2moon.s, loc.pos.extra.sun2earth.s);
    pos_eci2geoc(tloc);
    loc.pos.extra.moongeo = tloc.pos.geod.s;

//    pos_lvlh(utc, loc);
    return 0;
}

int32_t pos_lvlh(locstruc *loc)
{
    return pos_lvlh(*loc);
}

int32_t pos_lvlh(locstruc &loc)
{
    // Check time
//    if (!isfinite(utc) || utc == 0.)
//    {
//        return CONVERT_ERROR_UTC;
//    }

//    pos_extra(utc, loc);

    // LVLH related
    loc.pos.extra.p2l = {{0., 0., 0.},{0., 0., 0.},{0., 0., 0.}};
    loc.pos.extra.l2p = {{0., 0., 0.},{0., 0., 0.},{0., 0., 0.}};
    loc.pos.extra.dp2l = {{0., 0., 0.},{0., 0., 0.},{0., 0., 0.}};
    loc.pos.extra.dl2p = {{0., 0., 0.},{0., 0., 0.},{0., 0., 0.}};
    loc.pos.extra.ddp2l = {{0., 0., 0.},{0., 0., 0.},{0., 0., 0.}};
    loc.pos.extra.ddl2p = {{0., 0., 0.},{0., 0., 0.},{0., 0., 0.}};

    quaternion qe_z = {{0., 0., 0.}, 1.}, qe_y = {{0., 0., 0.}, 1.};
    loc.pos.extra.e2l = {{0., 0., 0.}, 1.};
    loc.pos.extra.l2e = {{0., 0., 0.}, 1.};
    rvector lvlh_z = {0., 0., 1.}, lvlh_y = {0., 1., 0.}, planec_z = {0., 0., 0.}, planec_y = {0., 0., 0.};

    cartpos *ppos;
    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        // Check time
//        if (!isfinite(loc.pos.eci.utc) || loc.pos.eci.utc == 0.)
//        {
//            return CONVERT_ERROR_UTC;
//        }

        ppos = &loc.pos.eci;
        break;
    case COSMOS_MOON:
        // Check time
//        if (!isfinite(loc.pos.sci.utc) || loc.pos.sci.utc == 0.)
//        {
//            return CONVERT_ERROR_UTC;
//        }

        ppos = &loc.pos.sci;
        break;
    }

    double s = length_rv(ppos->s);
    if (s < 1e6)
    {
        return CONVERT_ERROR_RADIUS;
    }
    loc.pos.extra.l2p.row[2] = -ppos->s / s;
    loc.pos.extra.dl2p.row[2] = -(ppos->v - dot_rv(-loc.pos.extra.l2p.row[2], ppos->v) * (-loc.pos.extra.l2p.row[2])) / s;
    loc.pos.extra.ddl2p.row[2] = -(ppos->a - dot_rv(-loc.pos.extra.l2p.row[2], ppos->v) * -loc.pos.extra.dl2p.row[2] - (dot_rv(-loc.pos.extra.l2p.row[2], ppos->a) + dot_rv(-loc.pos.extra.dl2p.row[2], ppos->v)) * -loc.pos.extra.l2p.row[2]) / s;

    rvector hbar = rv_cross(ppos->s, ppos->v);
    double h = length_rv(hbar);
    if (s < 1e6)
    {
        return CONVERT_ERROR_RADIUS;
    }
    rvector hdbar = rv_cross(ppos->s, ppos->a);
    rvector hddbar = rv_cross(ppos->s, ppos->j) + rv_cross(ppos->v, ppos->a);
    loc.pos.extra.l2p.row[1] = -hbar / h;
    loc.pos.extra.dl2p.row[2] = -(hdbar - dot_rv(-loc.pos.extra.l2p.row[1], hdbar) * (-loc.pos.extra.l2p.row[1])) / h;
    loc.pos.extra.ddl2p.row[2] = -(hddbar - dot_rv(-loc.pos.extra.l2p.row[1], hdbar) * -loc.pos.extra.dl2p.row[1] - (dot_rv(-loc.pos.extra.l2p.row[1], hddbar) + dot_rv(-loc.pos.extra.dl2p.row[1], hdbar)) * -loc.pos.extra.l2p.row[1]) / h;

    loc.pos.extra.l2p.row[0] = rv_cross(loc.pos.extra.l2p.row[1], loc.pos.extra.l2p.row[2]);
    loc.pos.extra.dl2p.row[0] = rv_cross(loc.pos.extra.l2p.row[1], loc.pos.extra.dl2p.row[2]) + rv_cross(loc.pos.extra.dl2p.row[1], loc.pos.extra.l2p.row[2]);
    loc.pos.extra.ddl2p.row[0] = rv_cross(loc.pos.extra.l2p.row[1], loc.pos.extra.ddl2p.row[2]) + 2. * rv_cross(loc.pos.extra.dl2p.row[1], loc.pos.extra.dl2p.row[2]) + rv_cross(loc.pos.extra.ddl2p.row[1], loc.pos.extra.l2p.row[2]);

    loc.pos.extra.p2l = rm_transpose(loc.pos.extra.l2p);
    loc.pos.extra.dp2l= rm_transpose(loc.pos.extra.dl2p);
    loc.pos.extra.ddp2l = rm_transpose(loc.pos.extra.ddl2p);

    // LVLH Z is opposite of direction to satellite
    planec_z = rv_smult(-1., ppos->s);
    normalize_rv(planec_z);

    // LVLH Y is Cross Product of LVLH Z and velocity vector
    planec_y = rv_cross(planec_z, ppos->v);
    normalize_rv(planec_y);

    // Determine intrinsic rotation of ITRF Z  into LVLH Z
    qe_z = q_conjugate(q_drotate_between_rv(planec_z, lvlh_z));

    // Use to intrinsically rotate ITRF Y into intermediate Y
    planec_y = irotate(qe_z, planec_y);

    // Determine intrinsic rotation of this intermediate Y into LVLH Y
    qe_y = q_conjugate(q_drotate_between_rv(planec_y, lvlh_y));

    // Combine to determine intrinsic rotation of ITRF into LVLH
    loc.pos.extra.e2l = q_fmult(qe_z, qe_y);
    normalize_q(&loc.pos.extra.e2l);
    loc.pos.extra.l2e = q_conjugate(loc.pos.extra.e2l);

    return 0;
}

//! Set Barycentric position
/*! Set the current time and position to whatever is in the Barycentric position of the
 * ::Cosmos::Convert::locstruc. Then propagate to all the other positions.
    \param loc ::Cosmos::Convert::locstruc with the current position and those to be updated.
*/
int32_t pos_icrf(locstruc *loc)
{
    return pos_icrf(*loc);
}

int32_t pos_icrf(locstruc &loc)
{
    int32_t iretn = 0;
    double distance, theta;
    rvector sat2body;

    // Synchronize time
    if (loc.pos.icrf.utc == 0. || !isfinite(loc.pos.icrf.utc))
    {
        return CONVERT_ERROR_UTC;
    }
    loc.utc = loc.pos.utc = loc.pos.icrf.utc;

    iretn = pos_extra(loc.pos.icrf.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Determine closest planetary body
//    loc.pos.extra.closest = COSMOS_EARTH;
//    if (length_rv(rv_sub(loc.pos.icrf.s, loc.pos.extra.sun2moon.s)) < length_rv(rv_sub(loc.pos.icrf.s, loc.pos.extra.sun2earth.s)))
//    {
//        loc.pos.extra.closest = COSMOS_MOON;
//    }

    // Set SUN specific stuff
    distance = length_rv(loc.pos.icrf.s);
    loc.pos.sunsize = static_cast<float>(RSUNM / distance);
    loc.pos.sunradiance = static_cast<float>(3.839e26 / (4. * DPI * distance * distance));

    // Check Earth:Sun separation
    sat2body = rv_sub(loc.pos.icrf.s, loc.pos.extra.sun2earth.s);
    loc.pos.earthsep = static_cast<float>(sep_rv(loc.pos.icrf.s, sat2body));
    loc.pos.earthsep -= static_cast<float>(asin(REARTHM / length_rv(sat2body)));
    if (loc.pos.earthsep < -loc.pos.sunsize)
        loc.pos.sunradiance = 0.;
    else if (loc.pos.earthsep <= loc.pos.sunsize)
    {
        theta = DPI * (loc.pos.sunsize + loc.pos.earthsep) / loc.pos.sunsize;
        loc.pos.sunradiance *= static_cast<float>((theta - sin(theta)) / D2PI);
    }

    // Set Moon specific stuff
    sat2body = rv_sub(loc.pos.icrf.s, loc.pos.extra.sun2moon.s);

    // Check Earth:Moon separation
    loc.pos.moonsep = static_cast<float>(sep_rv(loc.pos.icrf.s, sat2body));
    loc.pos.moonsep -= static_cast<float>(asin(RMOONM / length_rv(sat2body)));
    if (loc.pos.moonsep < -loc.pos.sunsize)
        loc.pos.sunradiance = 0.;
    else if (loc.pos.moonsep <= loc.pos.sunsize)
    {
        theta = DPI * (loc.pos.sunsize + loc.pos.moonsep) / loc.pos.sunsize;
        loc.pos.sunradiance *= static_cast<float>((theta - sin(theta)) / D2PI);
    }

    // Set related attitudes
    loc.att.icrf.pass = loc.pos.icrf.pass;
    loc.att.icrf.utc = loc.pos.icrf.utc;

    // Set adjoining positions
    if (loc.pos.icrf.pass > loc.pos.eci.pass)
    {
        pos_icrf2eci(loc);
        pos_eci(loc);
    }
    if (loc.pos.icrf.pass > loc.pos.sci.pass)
    {
        pos_icrf2sci(loc);
        pos_sci(loc);
    }
    return 0;
}

//! Set ECI position
/*! Set the current time and position to whatever is in the Earth Centered Inertial position of the
 * ::Cosmos::Convert::locstruc. Then propagate to all the other positions.
    \param loc ::Cosmos::Convert::locstruc with the current position and those to be updated.
*/
int32_t pos_eci(locstruc *loc)
{
    return pos_eci(*loc);
}

int32_t pos_eci(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.eci.utc || !isfinite(loc.pos.eci.utc))
    {
        return CONVERT_ERROR_UTC;
    }
    loc.utc = loc.pos.utc = loc.pos.eci.utc;

    iretn = pos_extra(loc.pos.eci.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Set closest
    double moongrav = MMOON / (length_rv(loc.pos.sci.s) * length_rv(loc.pos.sci.s));
    double earthgrav = MEARTH / (length_rv(loc.pos.eci.s) * length_rv(loc.pos.eci.s));
    loc.pos.extra.closest = COSMOS_EARTH;
    if (earthgrav < moongrav)
    {
        loc.pos.extra.closest = COSMOS_MOON;
    }
    pos_lvlh(loc);

    // Set adjoining positions
    if (loc.pos.eci.pass > loc.pos.icrf.pass)
    {
        pos_eci2icrf(loc);
        pos_icrf(loc);
    }
    if (loc.pos.eci.pass > loc.pos.geoc.pass)
    {
        pos_eci2geoc(loc);
        pos_geoc(loc);
    }

    // Set related attitude
    loc.att.icrf.pass = loc.pos.eci.pass;
    return 0;
}

//! Set SCI position
/*! Set the current time and position to whatever is in the Selene Centered Inertial position of the
 * ::Cosmos::Convert::locstruc. Then propagate to all the other positions.
    \param loc ::Cosmos::Convert::locstruc with the current position and those to be updated.
*/
int32_t pos_sci(locstruc *loc)
{
    return pos_sci(*loc);
}

int32_t pos_sci(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.sci.utc || !isfinite(loc.pos.sci.utc))
    {
        return CONVERT_ERROR_UTC;
    }
    loc.utc = loc.pos.utc = loc.pos.sci.utc;

    iretn = pos_extra(loc.pos.sci.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Set closest
    double moongrav = MMOON / (length_rv(loc.pos.sci.s) * length_rv(loc.pos.sci.s));
    double earthgrav = MEARTH / (length_rv(loc.pos.eci.s) * length_rv(loc.pos.eci.s));
    loc.pos.extra.closest = COSMOS_EARTH;
    if (earthgrav < moongrav)
    {
        loc.pos.extra.closest = COSMOS_MOON;
    }
    pos_lvlh(loc);

    // Set adjoining positions
    if (loc.pos.sci.pass > loc.pos.icrf.pass)
    {
        pos_sci2icrf(loc);
        pos_icrf(loc);
    }
    if (loc.pos.sci.pass > loc.pos.selc.pass)
    {
        pos_sci2selc(loc);
        pos_selc(loc);
    }

    // Set related attitude
    loc.att.icrf.pass = loc.pos.sci.pass;
    return 0;
}

//! Set Geocentric position
/*! Set the current time and position to whatever is in the Geocentric position of the
 * ::Cosmos::Convert::locstruc. Then propagate to all the other positions.
    \param loc ::Cosmos::Convert::locstruc with the current position and those to be updated.
*/
int32_t pos_geoc(locstruc *loc)
{
    return pos_geoc(*loc);
}

int32_t pos_geoc(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.geoc.utc || !isfinite(loc.pos.geoc.utc))
    {
        return CONVERT_ERROR_UTC;
    }
    loc.utc = loc.pos.utc = loc.pos.geoc.utc;

    iretn = pos_extra(loc.pos.geoc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Go to ECI if necessary
    if (loc.pos.geoc.pass > loc.pos.eci.pass)
    {
        pos_geoc2eci(loc);
        pos_eci(loc);
    }
    // Go to Geocentric Spherical if necessary
    if (loc.pos.geoc.pass > loc.pos.geos.pass)
    {
        pos_geoc2geos(loc);
        pos_geos(loc);
    }
    // Go to Geodetic if necessary
    if (loc.pos.geoc.pass > loc.pos.geod.pass)
    {
        pos_geoc2geod(loc);
        pos_geod(loc);
    }

    // Set related attitude
    loc.att.geoc.pass = loc.pos.geoc.pass;

    return 0;
}

//! Set Selenocentric position
/*! Set the current time and position to whatever is in the Selenocentric position of the
 * ::Cosmos::Convert::locstruc. Then propagate to all the other positions.
    \param loc ::Cosmos::Convert::locstruc with the current position and those to be updated.
*/
int32_t pos_selc(locstruc *loc)
{
    return pos_selc(*loc);
}

int32_t pos_selc(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.selc.utc || !isfinite(loc.pos.selc.utc))
    {
        return CONVERT_ERROR_UTC;
    }
    loc.utc = loc.pos.utc = loc.pos.selc.utc;

    iretn = pos_extra(loc.pos.selc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Go to SCI if necessary
    if (loc.pos.selc.pass > loc.pos.sci.pass)
    {
        pos_selc2sci(loc);
        pos_sci(loc);
    }
    // Go to Selenographic if necessary
    if (loc.pos.selc.pass > loc.pos.selg.pass)
    {
        pos_selc2selg(loc);
        pos_selg(loc);
    }

    // Set related attitude
    loc.att.selc.pass = loc.pos.selc.pass;

    return 0;
}

//! Set Selenographic position
/*! Set the current time and position to whatever is in the Selenographic position of the
 * ::Cosmos::Convert::locstruc. Then propagate to all the other positions.
    \param loc ::Cosmos::Convert::locstruc with the current position and those to be updated.
*/
int32_t pos_selg(locstruc *loc)
{
    return pos_selg(*loc);
}

int32_t pos_selg(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchroniz time
    if (0. == loc.pos.selg.utc || !isfinite(loc.pos.selg.utc))
    {
        return CONVERT_ERROR_UTC;
    }
    loc.utc = loc.pos.utc = loc.pos.selg.utc;

    iretn = pos_extra(loc.pos.selg.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    if (loc.pos.selg.pass > loc.pos.selc.pass)
    {
        pos_selg2selc(loc);
        pos_selc(loc);
    }

    return 0;
}

//! Set Geographic position
/*! Set the current time and position to whatever is in the Geographic position of the
 * ::Cosmos::Convert::locstruc. Then propagate to all the other positions.
    \param loc ::Cosmos::Convert::locstruc with the current position and those to be updated.
*/
int32_t pos_geos(locstruc *loc)
{
    return pos_geos(*loc);
}

int32_t pos_geos(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.geos.utc || !isfinite(loc.pos.geos.utc))
    {
        return CONVERT_ERROR_UTC;
    }
    loc.utc = loc.pos.utc = loc.pos.geos.utc;

    iretn = pos_extra(loc.pos.geos.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    if (loc.pos.geos.pass > loc.pos.geoc.pass)
    {
        pos_geos2geoc(loc);
        pos_geoc(loc);
    }

    return 0;
}

//! Set Geodetic position
/*! Set the current time and position to whatever is in the Geodetic position of the
 * ::Cosmos::Convert::locstruc. Then propagate to all the other positions.
    \param loc ::Cosmos::Convert::locstruc with the current position and those to be updated.
*/
int32_t pos_geod(locstruc *loc)
{
    return pos_geod(*loc);
}

int32_t pos_geod(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.geod.utc || !isfinite(loc.pos.geod.utc))
    {
        return CONVERT_ERROR_UTC;
    }
    loc.utc = loc.pos.utc = loc.pos.geod.utc;

    iretn = pos_extra(loc.pos.geod.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    if (loc.pos.geod.pass > loc.pos.geoc.pass)
    {
        pos_geod2geoc(loc);
        pos_geoc(loc);
    }
    // Determine magnetic field in Topocentric system
    geomag_front(loc.pos.geod.s, mjd2year(loc.utc), loc.pos.bearth);

    // Transform to ITRS
    loc.pos.bearth = irotate(q_change_around_z(-loc.pos.geod.s.lon), irotate(q_change_around_y(DPI2 + loc.pos.geod.s.lat), loc.pos.bearth));
    return 0;
}

//! Convert Barycentric to ECI
/*! Propagate the position found in the Barycentric slot of the supplied ::Cosmos::Convert::locstruc to
 * the Earth Centered Inertial slot, performing all relevant updates.
    \param loc Working ::Cosmos::Convert::locstruc
*/
int32_t pos_icrf2eci(locstruc *loc)
{
    return pos_icrf2eci(*loc);
}

int32_t pos_icrf2eci(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (loc.pos.icrf.utc == 0. || !isfinite(loc.pos.icrf.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.icrf.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.eci.utc = loc.pos.icrf.utc;

    // Update pass
    loc.pos.eci.pass = loc.pos.icrf.pass;

    // Heliocentric to Geocentric Ecliptic
    loc.pos.eci.s = loc.pos.eci.v = loc.pos.eci.a = loc.pos.eci.j = rv_zero();

    loc.pos.eci.s = rv_sub(loc.pos.icrf.s, loc.pos.extra.sun2earth.s);
    loc.pos.eci.v = rv_sub(loc.pos.icrf.v, loc.pos.extra.sun2earth.v);
    loc.pos.eci.a = rv_sub(loc.pos.icrf.a, loc.pos.extra.sun2earth.a);
    loc.pos.eci.j = rv_sub(loc.pos.icrf.j, loc.pos.extra.sun2earth.j);
    return 0;
}

//! Convert ECI to Barycentric
/*! Propagate the position found in the Earth Centered Inertial slot of the supplied ::Cosmos::Convert::locstruc to
 * the Barycentric slot, performing all relevant updates.
    \param loc Working ::Cosmos::Convert::locstruc
*/
int32_t pos_eci2icrf(locstruc *loc)
{
    return pos_eci2icrf(*loc);
}

int32_t pos_eci2icrf(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.eci.utc || !isfinite(loc.pos.eci.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.eci.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update pass
    loc.pos.icrf.pass = loc.pos.eci.pass;

    // Update time
    loc.pos.icrf.utc = loc.pos.eci.utc;

    // Geocentric Equatorial to Heliocentric
    loc.pos.icrf.s = rv_add(loc.pos.eci.s, loc.pos.extra.sun2earth.s);
    loc.pos.icrf.v = rv_add(loc.pos.eci.v, loc.pos.extra.sun2earth.v);
    loc.pos.icrf.a = rv_add(loc.pos.eci.a, loc.pos.extra.sun2earth.a);
    loc.pos.icrf.j = rv_add(loc.pos.eci.a, loc.pos.extra.sun2earth.j);
    return 0;
}

//! Convert Barycentric to SCI
/*! Propagate the position found in the Barycentric slot of the supplied ::Cosmos::Convert::locstruc to
 * the Selene Centered Inertial slot, performing all relevant updates.
    \param loc Working ::Cosmos::Convert::locstruc
*/
int32_t pos_icrf2sci(locstruc *loc)
{
    return pos_icrf2sci(*loc);
}

int32_t pos_icrf2sci(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (loc.pos.icrf.utc == 0. || !isfinite(loc.pos.icrf.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.icrf.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.sci.utc = loc.pos.icrf.utc;

    // Update pass
    loc.pos.sci.pass = loc.pos.icrf.pass;

    // Heliocentric to Geocentric Ecliptic
    loc.pos.sci.s = loc.pos.sci.v = loc.pos.sci.a = loc.pos.sci.j = rv_zero();

    loc.pos.sci.s = rv_sub(loc.pos.icrf.s, loc.pos.extra.sun2moon.s);
    loc.pos.sci.v = rv_sub(loc.pos.icrf.v, loc.pos.extra.sun2moon.v);
    loc.pos.sci.a = rv_sub(loc.pos.icrf.a, loc.pos.extra.sun2moon.a);
    loc.pos.sci.j = rv_sub(loc.pos.icrf.j, loc.pos.extra.sun2moon.j);

    return 0;
}

//! Convert SCI to Barycentric
/*! Propagate the position found in the Selene Centered Inertial slot of the supplied ::Cosmos::Convert::locstruc to
 * the Barycentric slot, performing all relevant updates.
    \param loc Working ::Cosmos::Convert::locstruc
*/
int32_t pos_sci2icrf(locstruc *loc)
{
    return pos_sci2icrf(*loc);
}

int32_t pos_sci2icrf(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.sci.utc || !isfinite(loc.pos.sci.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.sci.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.icrf.utc = loc.pos.sci.utc;

    // Update pass
    loc.pos.icrf.pass = loc.pos.sci.pass;

    // Geocentric Equatorial to Heliocentric
    loc.pos.icrf.s = rv_add(loc.pos.sci.s, loc.pos.extra.sun2moon.s);
    loc.pos.icrf.v = rv_add(loc.pos.sci.v, loc.pos.extra.sun2moon.v);
    loc.pos.icrf.a = rv_add(loc.pos.sci.a, loc.pos.extra.sun2moon.a);
    loc.pos.icrf.j = rv_add(loc.pos.sci.j, loc.pos.extra.sun2moon.j);

    return 0;
}

//! Convert ECI to GEOC
/*! Propagate the position found in the Earth Centered Inertial slot of the supplied ::Cosmos::Convert::locstruc to
 * the Geocentric slot, performing all relevant updates.
    \param loc Working ::Cosmos::Convert::locstruc
*/
int32_t pos_eci2geoc(locstruc *loc)
{
    return pos_eci2geoc(*loc);
}

int32_t pos_eci2geoc(locstruc &loc)
{
    int32_t iretn = 0;
    rvector v2;

    // Synchronize time
    if (0. == loc.pos.eci.utc || !isfinite(loc.pos.eci.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.eci.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.geoc.utc = loc.pos.eci.utc;

    // Update pass
    loc.pos.geoc.pass = loc.att.icrf.pass = loc.pos.eci.pass;

    // Apply transforms
    // St = S * e2j
    loc.pos.geoc.s = rv_mmult(loc.pos.extra.j2e, loc.pos.eci.s);

    // Vt = V * e2j + S * e2j'
    loc.pos.geoc.v = rv_mmult(loc.pos.extra.j2e, loc.pos.eci.v);
    v2 = rv_mmult(loc.pos.extra.dj2e, loc.pos.eci.s);
    loc.pos.geoc.v = rv_add(loc.pos.geoc.v, v2);

    // At = A * e2j + 2 * V * e2j' + S * e2j''
    loc.pos.geoc.a = rv_mmult(loc.pos.extra.j2e, loc.pos.eci.a);
    v2 = rv_smult(2., rv_mmult(loc.pos.extra.dj2e, loc.pos.eci.v));
    loc.pos.geoc.a = rv_add(loc.pos.geoc.a, v2);
    v2 = rv_mmult(loc.pos.extra.ddj2e, loc.pos.eci.s);
    loc.pos.geoc.a = rv_add(loc.pos.geoc.a, v2);

    // Jt = J * e2j + 6 * A * e2j' + 6 * V * e2j'' + S * e2j'''
    loc.pos.geoc.j = rv_mmult(loc.pos.extra.j2e, loc.pos.eci.j);
    v2 = rv_smult(6., rv_mmult(loc.pos.extra.dj2e, loc.pos.eci.a));
    loc.pos.geoc.j = rv_add(loc.pos.geoc.j, v2);
    v2 = rv_smult(6., rv_mmult(loc.pos.extra.ddj2e, loc.pos.eci.v));
    loc.pos.geoc.j = rv_add(loc.pos.geoc.j, v2);

    // Apply second order term due to first derivative of rotation matrix
    // Apply third order correction due to second derivative of rotation matrix

    // Convert GEOC Position to GEOD
    pos_geoc2geod(loc);

    // Convert GEOC Position to GEOS
    pos_geoc2geos(loc);

    // Convert ICRF attitude to ITRF
    iretn = att_icrf2geoc(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Convert ITRF Attitude to Topo
    iretn = att_planec2topo(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Convert ITRF Attitude to LVLH
    iretn = att_planec2lvlh(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

//! Convert GEOC to ECI
/*! Propagate the position found in the Geocentric slot of the supplied ::Cosmos::Convert::locstruc to
 * the Earth Centered Inertial slot, performing all relevant updates.
    \param loc Working ::Cosmos::Convert::locstruc
*/
int32_t pos_geoc2eci(locstruc *loc)
{
    return pos_geoc2eci(*loc);
}

int32_t pos_geoc2eci(locstruc &loc)
{
    int32_t iretn = 0;
    rvector ds;

    // Synchronize time
    if (0. == loc.pos.geoc.utc || !isfinite(loc.pos.geoc.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.geoc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.eci.utc = loc.pos.geoc.utc;

    // Update pass
    loc.pos.eci.pass = loc.att.geoc.pass = loc.pos.geoc.pass;

    // Apply transforms
    // St = S * e2j
    loc.pos.eci.s = rv_mmult(loc.pos.extra.e2j, loc.pos.geoc.s);

    // Vt = V * e2j + S * e2j'
    loc.pos.eci.v = rv_mmult(loc.pos.extra.e2j, loc.pos.geoc.v);
    ds = rv_mmult(loc.pos.extra.de2j, loc.pos.geoc.s);
    loc.pos.eci.v = rv_add(loc.pos.eci.v, ds);

    // At = A * e2j + 2 * V * e2j' + S * e2j''
    loc.pos.eci.a = rv_mmult(loc.pos.extra.e2j, loc.pos.geoc.a);
    ds = rv_smult(2., rv_mmult(loc.pos.extra.de2j, loc.pos.geoc.v));
    loc.pos.eci.a = rv_add(loc.pos.eci.a, ds);
    ds = rv_mmult(loc.pos.extra.dde2j, loc.pos.geoc.s);
    loc.pos.eci.a = rv_add(loc.pos.eci.a, ds);

    // Jt = J * e2j + 6 * A * e2j' + 6 * V * e2j'' + S * e2j'''
    loc.pos.eci.j = rv_mmult(loc.pos.extra.e2j, loc.pos.geoc.j);
    ds = rv_smult(6., rv_mmult(loc.pos.extra.de2j, loc.pos.geoc.a));
    loc.pos.eci.j = rv_add(loc.pos.eci.j, ds);
    ds = rv_smult(6., rv_mmult(loc.pos.extra.dde2j, loc.pos.geoc.v));
    loc.pos.eci.j = rv_add(loc.pos.eci.j, ds);

    // Convert ITRF Attitude to ICRF
    iretn = att_geoc2icrf(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Convert ITRF Attitude to LVLH
    iretn = att_planec2lvlh(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Convert ITRF Attitude to TOPO
    iretn = att_planec2topo(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

//! Convert GEOC to GEOS
/*! Convert a Geocentric ::Cosmos::Convert::cartpos to a Geographic ::Cosmos::Convert::spherpos.
    \param loc ::Cosmos::Convert::locstruc containing position.
*/
int32_t pos_geoc2geos(locstruc *loc)
{
    return pos_geoc2geos(*loc);
}

int32_t pos_geoc2geos(locstruc &loc)
{
    double xvx, yvy, r2, r, minir, minir2;
    double cp, cl, sl, sp;
    int32_t iretn = 0;

    // Synchronize time
    if (0. == loc.pos.geoc.utc || !isfinite(loc.pos.geoc.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.geoc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.geos.utc = loc.pos.geoc.utc;

    // Update pass
    loc.pos.geos.pass = loc.pos.geoc.pass;

    // Convert Geocentric Cartesian to Spherical
    minir2 = loc.pos.geoc.s.col[0] * loc.pos.geoc.s.col[0] + loc.pos.geoc.s.col[1] * loc.pos.geoc.s.col[1];
    minir = sqrt(minir2);
    r2 = minir2 + loc.pos.geoc.s.col[2] * loc.pos.geoc.s.col[2];
    loc.pos.geos.s.r = r = sqrt(r2);
    sp = loc.pos.geoc.s.col[2] / r;
    loc.pos.geos.s.phi = asin(sp);
    loc.pos.geos.s.lambda = atan2(loc.pos.geoc.s.col[1], loc.pos.geoc.s.col[0]);

    xvx = loc.pos.geoc.s.col[0] * loc.pos.geoc.v.col[0];
    yvy = loc.pos.geoc.s.col[1] * loc.pos.geoc.v.col[1];
    loc.pos.geos.v.r = (xvx + yvy + loc.pos.geoc.s.col[2] * loc.pos.geoc.v.col[2]) / r;
    loc.pos.geos.v.phi = (-(xvx + yvy) * loc.pos.geoc.s.col[2] + minir2 * loc.pos.geoc.v.col[2]) / (r2 * minir);
    // loc.pos.geos.v.lambda = -(loc.pos.geoc.s.col[0]*loc.pos.geoc.v.col[1]+loc.pos.geoc.s.col[1]*loc.pos.geoc.v.col[0])/minir2;

    cp = minir / r;
    cl = loc.pos.geoc.s.col[0] / minir;
    sl = loc.pos.geoc.s.col[1] / minir;
    if (fabs(loc.pos.geoc.s.col[1]) > fabs(loc.pos.geoc.s.col[0]))
        loc.pos.geos.v.lambda = (loc.pos.geoc.v.col[0] - cp * cl * loc.pos.geos.v.r + loc.pos.geoc.s.col[2] * cl * loc.pos.geos.v.phi) / (-loc.pos.geoc.s.col[1]);
    else
        loc.pos.geos.v.lambda = (loc.pos.geoc.v.col[1] - cp * sl * loc.pos.geos.v.r + loc.pos.geoc.s.col[2] * sl * loc.pos.geos.v.phi) / loc.pos.geoc.s.col[0];

    return 0;
}

//! Convert GEOS to GEOC
/*! Convert a Geographic ::Cosmos::Convert::spherpos to a Geocentric ::Cosmos::Convert::cartpos.
    \param loc ::Cosmos::Convert::locstruc containing position.
*/
int32_t pos_geos2geoc(locstruc *loc)
{
    return pos_geos2geoc(*loc);
}

int32_t pos_geos2geoc(locstruc &loc)
{
    double sp, cp, sl, cl, cpr;
    int32_t iretn = 0;

    // Synchronize time
    if (0. == loc.pos.geos.utc || !isfinite(loc.pos.geos.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.geos.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }
    // Update time
    loc.pos.geoc.utc = loc.pos.geos.utc;

    // Update pass
    loc.pos.geoc.pass = loc.pos.geos.pass;

    sp = sin(loc.pos.geos.s.phi);
    cp = cos(loc.pos.geos.s.phi);
    sl = sin(loc.pos.geos.s.lambda);
    cl = cos(loc.pos.geos.s.lambda);
    cpr = cp * loc.pos.geos.s.r;

    loc.pos.geoc.s = loc.pos.geoc.v = loc.pos.geoc.a = loc.pos.geoc.j = rv_zero();

    loc.pos.geoc.s.col[0] = cpr * cl;
    loc.pos.geoc.s.col[1] = cpr * sl;
    loc.pos.geoc.s.col[2] = loc.pos.geos.s.r * sp;

    // loc.pos.geoc.v.col[0] = loc.pos.geos.v.r * cp * cl - loc.pos.geos.v.phi * loc.pos.geos.s.r * sp * cl - loc.pos.geos.v.lambda * cpr * sl;
    // loc.pos.geoc.v.col[1] = loc.pos.geos.v.r * cp * sl - loc.pos.geos.v.phi * loc.pos.geos.s.r * sp * sl + loc.pos.geos.v.lambda * cpr * cl;
    loc.pos.geoc.v.col[0] = loc.pos.geos.v.r * cp * cl - loc.pos.geos.v.lambda * cpr * sl - loc.pos.geos.v.phi * loc.pos.geos.s.r * sp * cl;
    loc.pos.geoc.v.col[1] = loc.pos.geos.v.r * cp * sl + loc.pos.geos.v.lambda * cpr * cl - loc.pos.geos.v.phi * loc.pos.geos.s.r * sp * sl;
    loc.pos.geoc.v.col[2] = loc.pos.geos.v.r * sp + loc.pos.geos.v.phi * cpr;
    return 0;
}

//! Convert GEOC to GEOD
/*! Convert a Geocentric ::Cosmos::Convert::cartpos to a Geodetic ::Cosmos::Convert::geoidpos.
    \param geoc Source Geocentric position.
    \param geod Destination Geodetic position.
*/
int32_t geoc2geod(cartpos &geoc, geoidpos &geod)
{
    double e2;
    double st;
    double ct, cn, sn;
    double c, rp, a1, a2, a3, b1, b2, c1, c2, c3, rbc;
    double p, phi, h, nh, rn;

    // calculate geodetic longitude = atan2(py/px)
    geod.s.lon = atan2(geoc.s.col[1], geoc.s.col[0]);

    // Calculate effects of oblate spheroid
    // TODO: Explain math
    // e2 (square of first eccentricity) = 1 - (1 - f)^2
    e2 = (1. - FRATIO2);
    p = sqrt(geoc.s.col[0] * geoc.s.col[0] + geoc.s.col[1] * geoc.s.col[1]);
    nh = sqrt(p * p + geoc.s.col[2] * geoc.s.col[2]) - REARTHM;
    phi = atan2(geoc.s.col[2], p);
    do
    {
        h = nh;
        st = sin(phi);
        // rn = radius of curvature in the vertical prime
        rn = REARTHM / sqrt(1. - e2 * st * st);
        nh = p / cos(phi) - rn;
        phi = atan((geoc.s.col[2] / p) / (1. - e2 * rn / (rn + h)));
    } while (fabs(nh - h) > .01);

    geod.s.lat = phi;
    geod.s.h = h;

    // TODO: Explain math
    st = sin(geod.s.lat);
    ct = cos(geod.s.lat);
    sn = sin(geod.s.lon);
    cn = cos(geod.s.lon);

    c = 1. / sqrt(ct * ct + FRATIO2 * st * st);
    rp = geod.s.h + REARTHM * FRATIO2 * c * c * c;
    a1 = ct * cn;
    b1 = -geoc.s.col[1];
    c1 = -st * cn * rp;
    a2 = ct * sn;
    b2 = geoc.s.col[0];
    c2 = -st * sn * rp;
    a3 = st;
    c3 = ct * rp;
    rbc = (b1 * c2 - b2 * c1) / c3;
    geod.v.h = (b2 * geoc.v.col[0] - b1 * geoc.v.col[1] + rbc * geoc.v.col[2]) / (b2 * a1 - b1 * a2 + rbc * a3);
    geod.v.lat = (geoc.v.col[2] - a3 * geod.v.h) / c3;
    if (fabs(b1) > fabs(b2))
        geod.v.lon = (geoc.v.col[0] - a1 * geod.v.h - c1 * geod.v.lat) / b1;
    else
        geod.v.lon = (geoc.v.col[1] - a2 * geod.v.h - c2 * geod.v.lat) / b2;

    return 0;
}

//! Update locstruc GEOC to GEOD
/*! Convert a Geocentric ::Cosmos::Convert::cartpos to a Geodetic ::Cosmos::Convert::geoidpos in the provided locstruc.
    \param loc ::Cosmos::Convert::locstruc to be updated.
*/
int32_t pos_geoc2geod(locstruc *loc)
{
    return pos_geoc2geod(*loc);
}

int32_t pos_geoc2geod(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.geoc.utc || !isfinite(loc.pos.geoc.utc))
    {
        return 0;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.geoc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }
    // Update time
    loc.pos.geod.utc = loc.pos.geoc.utc;

    // Update pass
    loc.pos.geod.pass = loc.pos.geoc.pass;

    // Update geodetic position
    geoc2geod(loc.pos.geoc, loc.pos.geod);

    // Determine magnetic field in Topocentric system
    geomag_front(loc.pos.geod.s, mjd2year(loc.utc), loc.pos.bearth);

    // Transform to ITRS
    loc.pos.bearth = irotate(q_change_around_z(-loc.pos.geod.s.lon), irotate(q_change_around_y(DPI2 + loc.pos.geod.s.lat), loc.pos.bearth));

    return 0;
}

//! Convert GEOD to UTM
//! Convert a Geodetic ::Cosmos::Convert::geoidpos to a UTM value
//! \param geod Source Geodetic position.
//! \param utm Destination UTM position.
int32_t geod2utm(geoidpos &geod, Vector &utm)
{
    // Calculate meridional arc
    // Reference: https://fypandroid.wordpress.com/2011/09/03/converting-utm-to-latitude-and-longitude-or-vice-versa/
    constexpr double e = sqrt(1. - FRATIO2);
    //            constexpr double ep = (1. - FRATIO2) / FRATIO2;
    //            constexpr double e2 = e * e;
    //            constexpr double ep2 = ep * ep;
    //            constexpr double e4 = e2 * e2;
    //            constexpr double ep4 = ep2 * ep2;
    //            constexpr double e6 = e4 * e2;
    //            constexpr double e8 = e4 * e4;
    //            constexpr double e10 = e6 * e4;
    //            constexpr double c1 = 1 + (3./4.) * e2 + (45./64) * e4 + (175./256.) * e6 + (11025./16384.) * e8 + (43659./65536.) * e10;
    //            constexpr double c2 = (3./4.) * e2 + (15./16.) * e4 + (525./512.) * e6 + (2205./2048.) * e8 + (72765./65536.) * e10;
    //            constexpr double c3 = (15./64.) * e4 + (105./256.) * e6 + (2205./4096.) * e8 + (10395./16384.) * e10;
    //            constexpr double c4 = (35./512) * e6 + (315./2048.) * e8 + (31185./131072.) * e10;
    //            constexpr double c5 = (315./16384.) * e8 + (3465./65536.) * e10;
    //            constexpr double c6 = (693./131072.) * e10;

    //            double s = REARTHM * FRATIO2 * (c1 * geod.s.lat - c2 * sin(2. * geod.s.lat) / 2. + c3 * sin(4. * geod.s.lat) / 4. - c4 * sin(6. * geod.s.lat) / 6. + c5 * sin(8. * geod.s.lat) / 8. - c6 * sin(10. * geod.s.lat) / 10.);

    //            double k1 = .9996 * s;
    //            double slat = sin(geod.s.lat);
    //            double nu = REARTHM / sqrt(1. - e2 * slat * slat);
    //            double k2 = .9996 * nu * sin(2. * geod.s.lat) / 4.;
    //            double clat2 = clat * clat;
    //            double clat3 = clat * clat2;
    //            double clat4 = clat2 * clat2;
    //            double k3 = (5. - tlat2 + 9. * ep2 * clat2 + 4. * ep4 * clat4) * .9996 * slat * clat3 / 24.;
    //            double p = geod.s.lon;
    //            double p2 = p * p;
    //            double p3 = p2 * p;
    //            double p4 = p2 * p2;
    //            utm.x = k1 + p2 * (k2 + p2 * k3);
    //            double k4 = .9996 * nu * clat;
    //            double k5 = (1. - tlat2 + ep2 * clat2) * .9996 * nu * clat3 / 6.;
    //            utm.y = k4 * p + k5 * p3;

    constexpr double n = FLATTENING / (2 - FLATTENING);
    constexpr double n2 = n * n;
    constexpr double n3 = n2 * n;
    constexpr double n4 = n3 * n;
    constexpr double n5 = n4 * n;
    constexpr double n6 = n5 * n;
    constexpr double A = REARTHM * (1. + n2 / 4. + n4 / 64. + n6 / 256.);

    double a[6];
    a[0] = n / 2. - 2. * n2 / 3. + 5 * n3 / 16. + 41. * n4 / 180. - 127. * n5 / 288. + 7891. * n6 / 37800.;
    a[1] = 13. * n2 / 48. - 3. * n3 / 5. + 557. * n4 / 1440. + 281. * n5 / 630. - 1983433. * n6 / 1935360.;
    a[2] = 61. * n3 / 240. - 103 * n4 / 140. + 15061. * n5 / 26880. + 167603. * n6 / 181440.;
    a[3] = 49561. * n4 / 161280. - 179. * n5 / 168. + 6601661. * n6 / 7257600.;
    a[4] = 34729. * n5 / 80640. - 3418889. * n6 / 1995840.;
    a[5] = 212378941. * n6 / 319334400.;

    //            double b[6];
    //            b[0] = n / 2. - 2. * n2 / 3. + 37. * n3 / 96. - n4 / 360. - 81. * n5 / 512. + 96199. * n6 / 604800.;
    //            b[1] = n2 / 48. + n3 / 15. - 437. * n4 / 1440. + 46. * n5 / 105. - 1118711. * n6 / 3870720.;
    //            b[2] = 17. * n3 / 180. - 37. * n4 / 840. - 209 * n5 / 4480 + 5569 * n6 / 90720;
    //            b[3] = 4397 * n4 / 161280 - 11 * n5 / 504 - 830251 * n6 / 7257600;
    //            b[4] = 4583 * n5 / 161280 - 108847 * n6 / 3991680;
    //            b[5] = 20648693 * n6 / 638668800;

    double clat = cos(geod.s.lat);
    double tlat = tan(geod.s.lat);
    double tlat2 = tlat * tlat;
    double sigma = sinh(e * atanh(e * tlat / sqrt(1 + tlat2)));
    double tlatp = tlat * sqrt(1 + sigma * sigma) - sigma * sqrt(1 + tlat2);
    double zetap = atan(tlatp / clat);
    double slon = sin(geod.s.lon);
    double clon = cos(geod.s.lon);
    double clon2 = clon * clon;
    double etap = asinh(slon / sqrt(tlatp * tlatp + clon2));
    double zeta = zetap;
    double eta = etap;
    for (uint16_t i = 0; i < 6; ++i)
    {
        zeta += a[i] * sin(2 * (i + 1) * zetap) * cosh(2 * (i + 1) * etap);
        eta += a[i] * cos(2 * (i + 1) * zetap) * sinh(2 * (i + 1) * etap);
    }
    utm.x = .9996 * A * eta;
    utm.y = .9996 * A * zeta;

    return 0;
}

//! Convert GEOD to GEOC
/*! Convert a Geodetic ::Cosmos::Convert::geoidpos to a Geocentric ::Cosmos::Convert::cartpos.
    \param geod Source Geodetic position.
    \param geoc Destination Geocentric position.
*/
int32_t geod2geoc(geoidpos &geod, cartpos &geoc)
{
    double lst, dlst, r, c, s, c2, rp;
    double cn, ct, sn, st;

    // Determine effects of oblate spheroid
    ct = cos(geod.s.lat);
    st = sin(geod.s.lat);
    c = 1. / sqrt(ct * ct + FRATIO2 * st * st);
    c2 = c * c;
    s = FRATIO2 * c;
    r = (REARTHM * c + geod.s.h) * ct;

    geoc.s.col[2] = (REARTHM * s + geod.s.h) * st;

    lst = geod.s.lon;
    cn = cos(lst);
    sn = sin(lst);
    geoc.s.col[0] = r * cn;
    geoc.s.col[1] = r * sn;

    rp = geod.s.h + REARTHM * s * c2;
    geoc.v.col[2] = st * geod.v.h + rp * ct * geod.v.lat;

    dlst = geod.v.lon;
    geoc.v.col[0] = cn * ct * geod.v.h - geoc.s.col[1] * dlst - rp * cn * st * geod.v.lat;
    geoc.v.col[1] = sn * ct * geod.v.h + geoc.s.col[0] * dlst - rp * sn * st * geod.v.lat;

    return 0;
}

//! Update GEOD to GEOC in locstruc
/*! Update the Geodetic ::Cosmos::Convert::geoidpos to a Geocentric ::Cosmos::Convert::cartpos in the provided locstruc.
    \param loc ::Cosmos::Convert::locstruc to be updated.
*/
int32_t pos_geod2geoc(locstruc *loc)
{
    return pos_geod2geoc(*loc);
}

int32_t pos_geod2geoc(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.pos.geod.utc || !isfinite(loc.pos.geod.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.geod.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.geoc.utc = loc.pos.geod.utc;

    // Update pass
    loc.pos.geoc.pass = loc.pos.geod.pass;

    // Update the geocentric position
    geod2geoc(loc.pos.geod, loc.pos.geoc);
    return 0;
}

//! Convert SCI to SELC
/*! Propagate the position found in the Selene Centered Inertial slot of the supplied ::Cosmos::Convert::locstruc to
 * the Selenocentric slot, performing all relevant updates.
    \param loc Working ::Cosmos::Convert::locstruc
*/
int32_t pos_sci2selc(locstruc *loc)
{
    return pos_sci2selc(*loc);
}

int32_t pos_sci2selc(locstruc &loc)
{
    int32_t iretn = 0;
    rvector v2;
    rmatrix m1;

    // Synchronize time
    if (0. == loc.pos.sci.utc || !isfinite(loc.pos.sci.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.sci.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.selc.utc = loc.pos.sci.utc;

    // Update pass
    loc.pos.selc.pass = loc.att.icrf.pass = loc.pos.sci.pass;

    // Apply first order transform to all: J2000 to ITRS, then Earth to Moon
    loc.pos.selc.s = rv_mmult(loc.pos.extra.j2s, loc.pos.sci.s);
    loc.pos.selc.v = rv_mmult(loc.pos.extra.j2s, loc.pos.sci.v);
    loc.pos.selc.a = rv_mmult(loc.pos.extra.j2s, loc.pos.sci.a);
    loc.pos.selc.j = rv_mmult(loc.pos.extra.j2s, loc.pos.sci.j);

    // Apply second order term due to first derivative of rotation matrix
    m1 = rm_mmult(loc.pos.extra.dt2s, loc.pos.extra.j2t);
    v2 = rv_mmult(m1, loc.pos.sci.s);
    loc.pos.selc.v = rv_add(loc.pos.selc.v, v2);

    v2 = rv_smult(2., rv_mmult(m1, loc.pos.sci.v));
    loc.pos.selc.a = rv_add(loc.pos.selc.a, v2);

    v2 = rv_smult(2., rv_mmult(m1, loc.pos.sci.a));
    loc.pos.selc.j = rv_add(loc.pos.selc.j, v2);

    // Convert SELC Position to SELG
    pos_selc2selg(loc);

    // Convert ICRF Attitude to SELC
    iretn = att_icrf2selc(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Convert ITRF Attitude to Topo
    iretn = att_planec2topo(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Convert ITRF Attitude to LVLH
    iretn = att_planec2lvlh(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return iretn;
}

//! Convert SELC to SCI
/*! Propagate the position found in the Selenocentric slot of the supplied ::Cosmos::Convert::locstruc to
 * the Selene Centered Inertial slot, performing all relevant updates.
    \param loc Working ::Cosmos::Convert::locstruc
*/
int32_t pos_selc2sci(locstruc *loc)
{
    return pos_selc2sci(*loc);
}

int32_t pos_selc2sci(locstruc &loc)
{
    int32_t iretn = 0;
    rvector v2;
    rmatrix m1;

    // Synchroniz time
    if (0. == loc.pos.selc.utc || !isfinite(loc.pos.selc.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.selc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.sci.utc = loc.pos.selc.utc;

    // Update pass
    loc.pos.sci.pass = loc.att.selc.pass = loc.pos.selc.pass;

    // Apply first order transform to all
    loc.pos.sci.s = rv_mmult(loc.pos.extra.s2j, loc.pos.selc.s);
    loc.pos.sci.v = rv_mmult(loc.pos.extra.s2j, loc.pos.selc.v);
    loc.pos.sci.a = rv_mmult(loc.pos.extra.s2j, loc.pos.selc.a);
    loc.pos.sci.j = rv_mmult(loc.pos.extra.s2j, loc.pos.selc.j);

    // Apply second order correction due to first derivative of rotation matrix
    m1 = rm_mmult(loc.pos.extra.t2j, loc.pos.extra.ds2t);
    v2 = rv_mmult(m1, loc.pos.selc.s);
    loc.pos.sci.v = rv_add(loc.pos.selc.v, v2);

    m1 = rm_smult(2., m1);
    v2 = rv_mmult(m1, loc.pos.sci.v);
    loc.pos.sci.a = rv_add(loc.pos.selc.a, v2);

    // Convert SCI Attitude to ICRF
    iretn = att_selc2icrf(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

//! Convert SELC to SELG
/*! Propagate the position found in the Selenocentric slot of the supplied ::Cosmos::Convert::locstruc to
 * the Selenographic slot, performing all relevant updates.
    \param loc Working ::Cosmos::Convert::locstruc
*/
int32_t pos_selc2selg(locstruc *loc)
{
    return pos_selc2selg(*loc);
}

int32_t pos_selc2selg(locstruc &loc)
{
    int32_t iretn = 0;
    double xvx, yvy, r2, r, minir, minir2;

    // Synchroniz time
    if (0. == loc.pos.selc.utc || !isfinite(loc.pos.selc.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.selc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.selg.utc = loc.pos.selc.utc;

    // Update pass
    loc.pos.selg.pass = loc.pos.selc.pass;

    // Convert Geocentric Cartesian to Spherical
    minir2 = loc.pos.selc.s.col[0] * loc.pos.selc.s.col[0] + loc.pos.selc.s.col[1] * loc.pos.selc.s.col[1];
    minir = fixprecision(sqrt(minir2), .1);
    r2 = minir2 + loc.pos.selc.s.col[2] * loc.pos.selc.s.col[2];
    r = fixprecision(sqrt(r2), .1);
    loc.pos.selg.s.lat = asin(loc.pos.selc.s.col[2] / r);
    loc.pos.selg.s.lon = atan2(loc.pos.selc.s.col[1], loc.pos.selc.s.col[0]);
    loc.pos.selg.s.h = r - (RMOONM);

    xvx = loc.pos.selc.s.col[0] * loc.pos.selc.v.col[0];
    yvy = loc.pos.selc.s.col[1] * loc.pos.selc.v.col[1];
    loc.pos.selg.v.h = (xvx + yvy + loc.pos.selc.s.col[2] * loc.pos.selc.v.col[2]) / r;
    loc.pos.selg.v.lat = (-(xvx + yvy) * loc.pos.selc.s.col[2] + minir2 * loc.pos.selc.v.col[2]) / (r2 * minir);
    loc.pos.selg.v.lon = (loc.pos.selc.s.col[0] * loc.pos.selc.v.col[1] + loc.pos.selc.s.col[1] * loc.pos.selc.v.col[0]) / minir2;

    // Indicate we have set SELG position
    loc.pos.selg.s.lat = fixprecision(loc.pos.selg.s.lat, .1 / r);
    loc.pos.selg.s.lon = fixprecision(loc.pos.selg.s.lon, .1 / r);
    return 0;
}

int32_t pos_selg2selc(locstruc *loc)
{
    return pos_selg2selc(*loc);
}

int32_t pos_selg2selc(locstruc &loc)
{
    int32_t iretn = 0;
    double sp, cp, sl, cl, cpr, r;

    // Synchroniz time
    if (0. == loc.pos.selg.utc || !isfinite(loc.pos.selg.utc))
    {
        return CONVERT_ERROR_UTC;
    }

    // Update extra information
    iretn = pos_extra(loc.pos.selg.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Update time
    loc.pos.selc.utc = loc.pos.selg.utc;

    // Update pass
    loc.pos.selc.pass = loc.pos.selg.pass;

    r = loc.pos.selg.s.h + RMOONM;

    sp = sin(loc.pos.selg.s.lat);
    cp = cos(loc.pos.selg.s.lat);
    sl = sin(loc.pos.selg.s.lon);
    cl = cos(loc.pos.selg.s.lon);
    cpr = cp * r;

    loc.pos.selc.s = loc.pos.selc.v = loc.pos.selc.a = rv_zero();

    loc.pos.selc.s.col[0] = cpr * cl;
    loc.pos.selc.s.col[1] = cpr * sl;
    loc.pos.selc.s.col[2] = r * sp;

    loc.pos.selc.v.col[0] = loc.pos.selg.v.h * cp * cl - loc.pos.selg.v.lat * r * sp * cl - loc.pos.selg.v.lon * cpr * sl;
    loc.pos.selc.v.col[1] = loc.pos.selg.v.h * cp * sl - loc.pos.selg.v.lat * r * sp * sl + loc.pos.selg.v.lon * cpr * cl;
    loc.pos.selc.v.col[2] = loc.pos.selg.v.h * sp + loc.pos.selg.v.lat * cpr;

    return 0;
}

//! Calculate Extra attitude information
/*! Calculate things like conversion matrix for ICRF to Body and Body to
 * ICRF.
    \param loc ::Cosmos::Convert::locstruc with the current location and those to be updated.
*/
int32_t att_extra(locstruc *loc)
{
    att_extra(*loc);

    return 0;
}

int32_t att_extra(locstruc &loc)
{
    // Check time
    if (!isfinite(loc.att.icrf.utc) || loc.att.icrf.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    if (loc.att.extra.utc == loc.att.icrf.utc)
        return 0;

    loc.att.extra.b2j = rm_quaternion2dcm(loc.att.icrf.s);
    loc.att.extra.j2b = rm_transpose(loc.att.extra.b2j);
    loc.att.extra.utc = loc.att.icrf.utc;

    return 0;
}

int32_t att_icrf2geoc(locstruc *loc)
{
    return att_icrf2geoc(*loc);
}

int32_t att_icrf2geoc(locstruc &loc)
{
    int32_t iretn = 0;
    rvector alpha;
    double radius;

    // Check time
    if (!isfinite(loc.att.icrf.utc) || loc.att.icrf.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    // Update time
    loc.att.geoc.utc = loc.att.icrf.utc;

    // Update pass
    loc.att.geoc.pass = loc.att.icrf.pass;

    // Calculate rotation matrix to J2000
    iretn = pos_extra(loc.att.icrf.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Use to rotate ECI into ITRS
    loc.att.geoc.s = q_fmult(q_dcm2quaternion_rm(loc.pos.extra.j2e), loc.att.icrf.s);
    normalize_q(&loc.att.geoc.s);
    loc.att.geoc.v = rv_mmult(loc.pos.extra.j2e, loc.att.icrf.v);
    loc.att.geoc.a = rv_mmult(loc.pos.extra.j2e, loc.att.icrf.a);

    // Correct velocity for ECI angular velocity wrt ITRS, expressed in ITRS
    radius = length_rv(loc.pos.eci.s);

    alpha = rv_smult(1. / (radius * radius), rv_cross(rv_mmult(loc.pos.extra.j2e, loc.pos.eci.s), rv_mmult(loc.pos.extra.dj2e, loc.pos.eci.s)));
    loc.att.geoc.v = rv_add(loc.att.geoc.v, alpha);

    alpha = rv_smult(2. / (radius * radius), rv_cross(rv_mmult(loc.pos.extra.j2e, loc.pos.eci.s), rv_mmult(loc.pos.extra.dj2e, loc.pos.eci.v)));
    loc.att.geoc.a = rv_add(loc.att.geoc.a, alpha);

    // Convert ITRF attitude to Topocentric
    iretn = att_planec2topo(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Convert ITRF attitude to LVLH
    iretn = att_planec2lvlh(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

int32_t att_geoc2icrf(locstruc *loc)
{
    att_geoc2icrf(*loc);

    return 0;
}

int32_t att_geoc2icrf(locstruc &loc)
{
    //	rmatrix fpm = {{{{0.}}}}, dpm = {{{{0.}}}};
    rvector alpha;
    double radius;
    int32_t iretn = 0;

    // Check time
    if (!isfinite(loc.att.geoc.utc) || loc.att.geoc.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    // Propagate time
    loc.att.icrf.utc = loc.att.geoc.utc;

    // Update pass
    loc.att.icrf.pass = loc.att.geoc.pass;

    // Calculate rotation matrix to J2000
    iretn = pos_extra(loc.att.geoc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Perform first order rotation of ITRS frame into ECI frame
    loc.att.icrf.s = q_fmult(q_dcm2quaternion_rm(loc.pos.extra.e2j), loc.att.geoc.s);
    normalize_q(&loc.att.icrf.s);
    loc.att.icrf.v = rv_mmult(loc.pos.extra.e2j, loc.att.geoc.v);
    loc.att.icrf.a = rv_mmult(loc.pos.extra.e2j, loc.att.geoc.a);

    // Correct for ITRS angular velocity wrt ECI, expressed in ECI
    radius = length_rv(loc.pos.geoc.s);

    alpha = rv_smult(1. / (radius * radius), rv_cross(rv_mmult(loc.pos.extra.e2j, loc.pos.geoc.s), rv_mmult(loc.pos.extra.de2j, loc.pos.geoc.s)));
    loc.att.icrf.v = rv_add(loc.att.icrf.v, alpha);

    alpha = rv_smult(2. / (radius * radius), rv_cross(rv_mmult(loc.pos.extra.e2j, loc.pos.geoc.s), rv_mmult(loc.pos.extra.de2j, loc.pos.geoc.v)));
    loc.att.icrf.a = rv_add(loc.att.icrf.a, alpha);

    // Extra attitude information
    iretn = att_extra(loc);

    return iretn;
}

int32_t att_geoc(locstruc *loc)
{
    return att_geoc(*loc);
}

int32_t att_geoc(locstruc &loc)
{
    int32_t iretn = 0;
    // Synchronize time
    if (0. == loc.att.geoc.utc || !isfinite(loc.att.geoc.utc))
    {
        return CONVERT_ERROR_UTC;
    }
    loc.att.utc = loc.att.geoc.utc;

    if (loc.att.geoc.pass > loc.att.topo.pass)
    {
        iretn = att_planec2topo(loc);
        if (iretn < 0)
        {
            return iretn;
        }
        iretn = att_topo(loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    if (loc.att.geoc.pass > loc.att.lvlh.pass)
    {
        iretn = att_planec2lvlh(loc);
        if (iretn < 0)
        {
            return iretn;
        }
        iretn = att_lvlh(loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    if (loc.att.geoc.pass > loc.att.icrf.pass)
    {
        iretn = att_geoc2icrf(loc);
        if (iretn < 0)
        {
            return iretn;
        }
        iretn = att_icrf(loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }

    return 0;
}

int32_t att_icrf2selc(locstruc *loc)
{
    return att_icrf2selc(*loc);
}

int32_t att_icrf2selc(locstruc &loc)
{
    int32_t iretn = 0;
    rmatrix dpm;
    rvector alpha;
    double radius;

    // Check time
    if (!isfinite(loc.att.icrf.utc) || loc.att.icrf.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    // Propagate time
    loc.att.selc.utc = loc.att.icrf.utc;

    // Update pass
    loc.att.selc.pass = loc.att.icrf.pass;

    // Calculate rotation matrix to J2000
    iretn = pos_extra(loc.att.selc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Use to rotate ICRF into SELC
    loc.att.selc.s = q_fmult(q_dcm2quaternion_rm(loc.pos.extra.j2s), loc.att.icrf.s);
    normalize_q(&loc.att.selc.s);
    loc.att.selc.v = rv_mmult(loc.pos.extra.j2s, loc.att.icrf.v);
    loc.att.selc.a = rv_mmult(loc.pos.extra.j2s, loc.att.icrf.a);

    // Correct velocity for ECI angular velocity wrt ITRS, expressed in ITRS
    radius = length_rv(loc.pos.eci.s);

    dpm = rm_zero();

    alpha = rv_smult(1. / (radius * radius), rv_cross(rv_mmult(loc.pos.extra.j2s, loc.pos.eci.s), rv_mmult(dpm, loc.pos.eci.s)));
    loc.att.selc.v = rv_add(loc.att.selc.v, alpha);

    alpha = rv_smult(2. / (radius * radius), rv_cross(rv_mmult(loc.pos.extra.j2s, loc.pos.eci.s), rv_mmult(dpm, loc.pos.eci.v)));
    loc.att.selc.a = rv_add(loc.att.selc.a, alpha);

    // Synchronize LVLH
    iretn = att_planec2lvlh(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Synchronize Topo
    iretn = att_planec2topo(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

int32_t att_selc2icrf(locstruc *loc)
{
    att_selc2icrf(*loc);

    return 0;
}

int32_t att_selc2icrf(locstruc &loc)
{
    int32_t iretn = 0;
    //	rmatrix fpm = {{{{0.}}}};
    // Check time
    if (!isfinite(loc.att.selc.utc) || loc.att.selc.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    // Propagate time
    loc.att.icrf.utc = loc.att.selc.utc = loc.utc;

    // Update pass
    loc.att.icrf.pass = loc.att.selc.pass;

    // Calculate rotation matrix to J2000
    iretn = pos_extra(loc.att.selc.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    // Perform first order rotation of SELC frame into ICRF frame
    loc.att.icrf.s = q_fmult(q_dcm2quaternion_rm(loc.pos.extra.s2j), loc.att.selc.s);
    normalize_q(&loc.att.icrf.s);
    loc.att.icrf.v = rv_mmult(loc.pos.extra.s2j, loc.att.selc.v);
    loc.att.icrf.a = rv_mmult(loc.pos.extra.s2j, loc.att.selc.a);

    // Extra attitude information
    iretn = att_extra(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

int32_t att_selc(locstruc *loc)
{
    att_selc(*loc);

    return 0;
}

int32_t att_selc(locstruc &loc)
{
    int32_t iretn = 0;
    // Check time
    if (!isfinite(loc.att.selc.utc) || loc.att.selc.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    if (loc.att.selc.pass > loc.att.topo.pass)
    {
        iretn = att_planec2topo(loc);
        if (iretn < 0)
        {
            return iretn;
        }

        iretn = att_topo(loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    if (loc.att.selc.pass > loc.att.lvlh.pass)
    {
        iretn = att_planec2lvlh(loc);
        if (iretn < 0)
        {
            return iretn;
        }

        iretn = att_lvlh(loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    if (loc.att.selc.pass > loc.att.icrf.pass)
    {
        iretn = att_selc2icrf(loc);
        if (iretn < 0)
        {
            return iretn;
        }
        iretn = att_icrf(loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }

    return 0;
}

int32_t att_icrf2lvlh(locstruc *loc)
{
    att_icrf2lvlh(*loc);
    return 0;
}

int32_t att_icrf2lvlh(locstruc &loc)
{
    int32_t iretn = 0;
    // Check time
    if (!isfinite(loc.att.icrf.utc) || loc.att.icrf.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    iretn = att_icrf2geoc(loc);
    if (iretn < 0)
    {
        return iretn;
    }
    iretn = att_icrf2selc(loc);
    if (iretn < 0)
    {
        return iretn;
    }
    iretn = att_planec2lvlh(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

int32_t att_icrf(locstruc *loc)
{
    return att_icrf(*loc);
    return 0;
}

int32_t att_icrf(locstruc &loc)
{
    int32_t iretn = 0;
    // Check time
    if (!isfinite(loc.att.icrf.utc) || loc.att.icrf.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    if (loc.att.icrf.pass > loc.att.geoc.pass)
    {
        iretn = att_icrf2geoc(loc);
        if (iretn < 0)
        {
            return iretn;
        }

        iretn = att_geoc(loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }

    if (loc.att.icrf.pass > loc.att.selc.pass)
    {
        iretn = att_icrf2selc(loc);
        if (iretn < 0)
        {
            return iretn;
        }

        iretn = att_selc(loc);
        if (iretn < 0)
        {
            return iretn;
        }
    }
    return 0;
}

//! Convert ITRS attitude to LVLH attitude
/*! Calculate the rotation quaternion for taking a vector from LVLH to
 * Body given the similar quaternion for ITRS
    \param loc Location structure to propagate the changes in
*/
int32_t att_planec2lvlh(locstruc *loc)
{
    att_planec2lvlh(*loc);
    return 0;
}

int32_t att_planec2lvlh(locstruc &loc)
{
    //            quaternion qe_z = {{0., 0., 0.}, 1.}, qe_y = {{0., 0., 0.}, 1.};
    //            loc.pos.extra.e2l = {{0., 0., 0.}, 1.};
    //            loc.pos.extra.l2e = {{0., 0., 0.}, 1.};
    //            rvector lvlh_z = {0., 0., 1.}, lvlh_y = {0., 1., 0.}, geoc_z = {0., 0., 0.}, geoc_y = {0., 0., 0.}, alpha = {0., 0., 0.};
    qatt *patt;
    cartpos *ppos;
    double radius;

    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        // Check time
        if (!isfinite(loc.att.geoc.utc) || loc.att.geoc.utc == 0.)
        {
            return CONVERT_ERROR_UTC;
        }

        patt = &loc.att.geoc;
        ppos = &loc.pos.geoc;
        break;
    case COSMOS_MOON:
        // Check time
        if (!isfinite(loc.att.selc.utc) || loc.att.selc.utc == 0.)
        {
            return CONVERT_ERROR_UTC;
        }

        patt = &loc.att.selc;
        ppos = &loc.pos.selc;
        break;
    }

    radius = length_rv(ppos->s);

    // Update time
    loc.att.lvlh.utc = patt->utc;

    // Update pass
    loc.att.lvlh.pass = patt->pass;

//    pos_lvlh(patt->utc, loc);

    // Correct velocity for LVLH angular velocity wrt ITRS, expressed in ITRS
    rvector alpha = rv_smult(1. / (radius * radius), rv_cross(ppos->s, ppos->v));
    loc.att.lvlh.v = rv_sub(patt->v, alpha);

    // Transform ITRS into LVLH
    loc.att.lvlh.s = q_fmult(loc.pos.extra.l2e, patt->s);
    loc.att.lvlh.v = irotate(loc.pos.extra.e2l, loc.att.lvlh.v);
    loc.att.lvlh.a = irotate(loc.pos.extra.e2l, patt->a);

    return 0;
}

//! Convert LVLH attitude to ITRS attitude
/*! Calculate the rotation quaternion for taking a vector from ITRS to
 * Body given the similar quaternion for LVLH
    \param loc Location structure to propagate the changes in
*/
int32_t att_lvlh2planec(locstruc *loc)
{
    att_lvlh2planec(*loc);
    return 0;
}

int32_t att_lvlh2planec(locstruc &loc)
{
    //            quaternion qe_z = {{0., 0., 0.}, 1.}, qe_y = {{0., 0., 0.}, 1.}, fqe = {{0., 0., 0.}, 1.}, rqe = {{0., 0., 0.}, 1.};
    //            rvector lvlh_z = {0., 0., 1.}, lvlh_y = {0., 1., 0.}, geoc_z = {0., 0., 0.}, geoc_y = {0., 0., 0.}, alpha = {0., 0., 0.};
    qatt *patt;
    cartpos *ppos;
    double radius;
    int32_t iretn = 0;

    // Check time
    if (!isfinite(loc.att.lvlh.utc) || loc.att.lvlh.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        patt = &loc.att.geoc;
        ppos = &loc.pos.geoc;
        break;
    case COSMOS_MOON:
        patt = &loc.att.selc;
        ppos = &loc.pos.selc;
        break;
    }
    radius = length_rv(ppos->s);

    // Update time
    patt->utc = loc.att.lvlh.utc;

    // Update pass
    patt->pass = loc.att.lvlh.pass;

//    pos_lvlh(loc.att.lvlh.utc, loc);

    // Rotate LVLH frame into ITRS frame
    patt->s = q_fmult(loc.pos.extra.e2l, loc.att.lvlh.s);
    patt->v = irotate(loc.pos.extra.l2e, loc.att.lvlh.v);
    patt->a = irotate(loc.pos.extra.l2e, loc.att.lvlh.a);

    // Correct velocity for LVLH angular velocity wrt ITRS, expressed in ITRS
    rvector alpha = rv_smult(1. / (radius * radius), rv_cross(ppos->s, ppos->v));
    patt->v = rv_add(patt->v, alpha);

    // Synchronize Topo
    iretn = att_planec2topo(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

//! Convert LVLH attitude to ICRF attitude
/*! Calculate the rotation quaternion for taking a vector from ICRF to
 * Body given the similar quaternion for LVLH
    \param loc Location structure to propagate the changes in
*/
int32_t att_lvlh2icrf(locstruc *loc)
{
    return att_lvlh2icrf(*loc);
}

int32_t att_lvlh2icrf(locstruc &loc)
{
    int32_t iretn = 0;

    // Check time
    if (!isfinite(loc.att.lvlh.utc) || loc.att.lvlh.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    iretn = att_lvlh2planec(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    iretn = pos_extra(loc.att.lvlh.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        iretn = att_geoc2icrf(loc);
        if (iretn < 0)
        {
            return iretn;
        }

        break;
    case COSMOS_MOON:
        iretn = att_selc2icrf(loc);
        if (iretn < 0)
        {
            return iretn;
        }

        break;
    }
    return att_extra(loc);
}

int32_t att_lvlh(locstruc *loc)
{
    return att_lvlh(*loc);
}

int32_t att_lvlh(locstruc &loc)
{
    int32_t iretn = 0;

    // Check time
    if (!isfinite(loc.att.lvlh.utc) || loc.att.lvlh.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    iretn = pos_extra(loc.att.lvlh.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        if (loc.att.lvlh.pass > loc.att.geoc.pass)
        {
            iretn = att_lvlh2planec(loc);
            if (iretn < 0)
            {
                return iretn;
            }
            iretn = att_geoc(loc);
            if (iretn < 0)
            {
                return iretn;
            }
        }
        break;
    case COSMOS_MOON:
        if (loc.att.lvlh.pass > loc.att.selc.pass)
        {
            iretn = att_lvlh2planec(loc);
            if (iretn < 0)
            {
                return iretn;
            }

            iretn = att_selc(loc);
            if (iretn < 0)
            {
                return iretn;
            }
        }
        break;
    }
    return 0;
}

//! Planetocentric to Topo attitude
/*! Calculate the rotation quaternion for taking a vector between Topo
 * and the closest planetocentric system  and update whichever is older.
    \param loc Location structure to update
*/
int32_t att_planec2topo(locstruc *loc)
{
    att_planec2topo(*loc);
    return 0;
}

int32_t att_planec2topo(locstruc &loc)
{
    quaternion t2g, t2g_z, t2g_x, g2t;
    rvector geoc_x, topo_x;
    qatt *patt;
    cartpos *ppos;

    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        patt = &loc.att.geoc;
        ppos = &loc.pos.geoc;
        break;
    case COSMOS_MOON:
        patt = &loc.att.selc;
        ppos = &loc.pos.selc;
        break;
    }

    // Check time
    if (!isfinite(patt->utc) || patt->utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    // Update time
    loc.att.topo.utc = patt->utc;

    // Update pass
    loc.att.topo.pass = patt->pass;

    // Determine rotation of Topo unit Z  into ITRS Z
    t2g_z = q_conjugate(q_drotate_between_rv(rv_unitz(), ppos->s));

    // Use to rotate Topo unit X into intermediate Topo X
    topo_x = irotate(t2g_z, rv_unitx());

    // Define ITRS unit x as x=-Topo.y and y=Topo.x
    geoc_x.col[0] = -ppos->s.col[1];
    geoc_x.col[1] = ppos->s.col[0];

    // Determine rotation of intermediate Topo X into ITRS unit X
    t2g_x = q_conjugate(q_drotate_between_rv(topo_x, geoc_x));

    // Multiply to determine rotation of Topo frame into ITRS frame
    t2g = q_fmult(t2g_z, t2g_x);
    normalize_q(&t2g);
    g2t = q_conjugate(t2g);

    // Correct velocity for Topo angular velocity wrt ITRS, expressed in ITRS
    rvector alpha = rv_smult(-1. / (length_rv(ppos->s) * length_rv(ppos->s)), rv_cross(ppos->s, ppos->v));
    loc.att.topo.v = rv_add(patt->v, alpha);

    // Rotate ITRS frame into Topo frame
    loc.att.topo.s = q_fmult(g2t, patt->s);
    loc.att.topo.v = irotate(t2g, loc.att.topo.v);
    loc.att.topo.a = irotate(t2g, patt->a);

    return 0;
}

//! Topocentric to Planetocentric attitude
/*! Calculate the rotation quaternion for taking a vector between Topo
 * and the closest planetocentric system  and update whichever is older.
    \param loc Location structure to update
*/
int32_t att_topo2planec(locstruc *loc)
{
    att_topo2planec(*loc);
    return 0;
}

int32_t att_topo2planec(locstruc &loc)
{
    quaternion t2g, t2g_z, t2g_x, g2t;
    rvector geoc_x, topo_x;
    qatt *patt;
    cartpos *ppos;
    int32_t iretn = 0;

    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        patt = &loc.att.geoc;
        ppos = &loc.pos.geoc;
        break;
    case COSMOS_MOON:
        patt = &loc.att.selc;
        ppos = &loc.pos.selc;
        break;
    }

    // Update time
    patt->utc = loc.att.topo.utc;

    // Update pass
    ppos->pass = patt->pass = loc.att.topo.pass;

    // Determine rotation of Topo unit Z  into ITRS Z
    t2g_z = q_conjugate(q_drotate_between_rv(rv_unitz(), ppos->s));

    // Use to rotate Topo unit X into intermediate Topo X
    topo_x = irotate(t2g_z, rv_unitx());

    // Define ITRS unit x as x=-Topo.y and y=Topo.x
    geoc_x.col[0] = -ppos->s.col[1];
    geoc_x.col[1] = ppos->s.col[0];

    // Determine rotation of intermediate Topo X into ITRS unit X
    t2g_x = q_conjugate(q_drotate_between_rv(topo_x, geoc_x));

    // Multiply to determine rotation of Topo frame into ITRS frame
    t2g = q_fmult(t2g_z, t2g_x);
    normalize_q(&t2g);
    g2t = q_conjugate(t2g);

    // Rotate Topo frame into ITRS frame
    patt->s = q_fmult(loc.att.topo.s, t2g);
    patt->v = irotate(g2t, loc.att.topo.v);
    patt->a = irotate(g2t, loc.att.topo.a);

    // Correct velocity for Topo angular velocity wrt ITRS, expressed in ITRS
    rvector alpha = rv_smult(-1. / (length_rv(ppos->s) * length_rv(ppos->s)), rv_cross(ppos->s, ppos->v));

    // Correct for rotation of frame
    patt->v = rv_add(loc.att.topo.v, alpha);

    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        iretn = att_geoc2icrf(loc);
        if (iretn < 0)
        {
            return iretn;
        }

        break;
    case COSMOS_MOON:
        iretn = att_selc2icrf(loc);
        if (iretn < 0)
        {
            return iretn;
        }

        break;
    }
    iretn = att_planec2lvlh(loc);
    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

int32_t att_topo(locstruc *loc)
{
    return att_topo(*loc);
}

int32_t att_topo(locstruc &loc)
{
    int32_t iretn = 0;

    // Check time
    if (!isfinite(loc.att.topo.utc) || loc.att.topo.utc == 0.)
    {
        return CONVERT_ERROR_UTC;
    }

    iretn = pos_extra(loc.att.topo.utc, loc);
    if (iretn < 0)
    {
        return iretn;
    }

    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        if (loc.att.topo.pass > loc.att.geoc.pass)
        {
            att_topo2planec(loc);
            iretn = att_geoc(loc);
            if (iretn < 0)
            {
                return iretn;
            }
        }
        break;
    case COSMOS_MOON:
        if (loc.att.topo.pass > loc.att.selc.pass)
        {
            att_topo2planec(loc);
            iretn = att_selc(loc);
            if (iretn < 0)
            {
                return iretn;
            }
        }
        break;
    }
    return 0;
}

//! Synchronize all frames in location structure.
/*! Work through provided location structure, first identifying the
frame that is most up to date, then updating all other frames to
match.
    \param loc ::Cosmos::Convert::locstruc to be synchronized
*/
int32_t loc_update(locstruc *loc)
{
    loc_update(*loc);
    return 0;
}

int32_t loc_update(locstruc &loc)
{
    uint32_t ppass = 0, apass = 0;
    int32_t ptype = -1, atype = -1;
    int32_t iretn = 0;

    if (loc.att.icrf.pass > apass)
    {
        apass = loc.att.icrf.pass;
        atype = JSON_TYPE_QATT_ICRF;
    }
    if (loc.att.geoc.pass > apass)
    {
        apass = loc.att.geoc.pass;
        atype = JSON_TYPE_QATT_GEOC;
    }
    if (loc.att.selc.pass > apass)
    {
        apass = loc.att.selc.pass;
        atype = JSON_TYPE_QATT_SELC;
    }
    if (loc.att.lvlh.pass > apass)
    {
        apass = loc.att.lvlh.pass;
        atype = JSON_TYPE_QATT_LVLH;
    }
    if (loc.att.topo.pass > apass)
    {
        apass = loc.att.topo.pass;
        atype = JSON_TYPE_QATT_TOPO;
    }
    if (loc.pos.icrf.pass > ppass)
    {
        ppass = loc.pos.icrf.pass;
        ptype = JSON_TYPE_POS_ICRF;
    }
    if (loc.pos.eci.pass > ppass)
    {
        ppass = loc.pos.eci.pass;
        ptype = JSON_TYPE_POS_ECI;
    }
    if (loc.pos.sci.pass > ppass)
    {
        ppass = loc.pos.sci.pass;
        ptype = JSON_TYPE_POS_SCI;
    }
    if (loc.pos.geoc.pass > ppass)
    {
        ppass = loc.pos.geoc.pass;
        ptype = JSON_TYPE_POS_GEOC;
    }
    if (loc.pos.selc.pass > ppass)
    {
        ppass = loc.pos.selc.pass;
        ptype = JSON_TYPE_POS_SELC;
    }
    if (loc.pos.geod.pass > ppass)
    {
        ppass = loc.pos.geod.pass;
        ptype = JSON_TYPE_POS_GEOD;
    }
    if (loc.pos.geos.pass > ppass)
    {
        ppass = loc.pos.geos.pass;
        ptype = JSON_TYPE_POS_GEOS;
    }
    if (loc.pos.selg.pass > ppass)
    {
        ppass = loc.pos.selg.pass;
        ptype = JSON_TYPE_POS_SELG;
    }

    switch (ptype)
    {
    case JSON_TYPE_POS_ICRF:
        iretn = pos_icrf(loc);
        break;
    case JSON_TYPE_POS_ECI:
        iretn = pos_eci(loc);
        break;
    case JSON_TYPE_POS_SCI:
        iretn = pos_sci(loc);
        break;
    case JSON_TYPE_POS_GEOC:
        iretn = pos_geoc(loc);
        break;
    case JSON_TYPE_POS_SELC:
        iretn = pos_selc(loc);
        break;
    case JSON_TYPE_POS_GEOD:
        iretn = pos_geod(loc);
        break;
    case JSON_TYPE_POS_GEOS:
        iretn = pos_geos(loc);
        break;
    case JSON_TYPE_POS_SELG:
        iretn = pos_selg(loc);
        break;
    }
    if (iretn < 0)
    {
        return iretn;
    }

    switch (atype)
    {
    case JSON_TYPE_QATT_ICRF:
        iretn = att_icrf(loc);
        break;
    case JSON_TYPE_QATT_GEOC:
        iretn = att_geoc(loc);
        break;
    case JSON_TYPE_QATT_SELC:
        iretn = att_selc(loc);
        break;
    case JSON_TYPE_QATT_LVLH:
        iretn = att_lvlh(loc);
        break;
    case JSON_TYPE_QATT_TOPO:
        iretn = att_topo(loc);
        break;
    }

    if (iretn < 0)
    {
        return iretn;
    }

    return 0;
}

int32_t teme2true(double ep0, rmatrix *rm)
{
    // TEME to True of Date (Equation of Equinoxes)
    double eeq = utc2gast(ep0) - utc2gmst1982(ep0);
    *rm = rm_change_around_z(eeq);
    return 0;
}

int32_t true2teme(double ep0, rmatrix *rm)
{
    double eeq = utc2gast(ep0) - utc2gmst1982(ep0);
    *rm = rm_change_around_z(-eeq);
    return 0;
}

int32_t true2pef(double utc, rmatrix *rm)
{
    double gast = utc2gast(utc);
    *rm = rm_change_around_z(-gast);
    return 0;
}

int32_t pef2true(double utc, rmatrix *rm)
{
    double gast = utc2gast(utc);
    *rm = rm_change_around_z(gast);
    return 0;
}

int32_t pef2itrs(double utc, rmatrix *rm)
{
    double ttc = utc2jcentt(utc);
    cvector polm = polar_motion(utc);
    double pols = -47. * 4.848136811095359935899141e-12 * ttc;

    *rm = rm_mmult(rm_change_around_z(-pols), rm_mmult(rm_change_around_y(polm.x), rm_change_around_x(polm.y)));
    return 0;
}

int32_t itrs2pef(double utc, rmatrix *rm)
{
    static rmatrix orm;
    static double outc = 0.;

    if (utc == outc)
    {
        *rm = orm;
        return 0;
    }

    pef2itrs(utc, rm);
    *rm = rm_transpose(*rm);

    outc = utc;
    orm = *rm;
    return 0;
}

//! Rotate Mean of Epoch to True of Epoch
/*! Calculate the rotation matrix for converting coordinates from a system based on the
 * Mean orientation for that Epoch to one based on the True orientation. Adds effects of
 * Nutation.
    \param ep0 Epoch, in units of Modified Julian Day
    \param pm Rotation matrix
*/
int32_t mean2true(double ep0, rmatrix *pm)
{
    static rmatrix opm;
    static double oep0 = 0.;

    if (ep0 == oep0)
    {
        *pm = opm;
        return 0;
    }

    true2mean(ep0, pm);
    *pm = rm_transpose(*pm);

    oep0 = ep0;
    opm = *pm;
    return 0;
}

//! Rotate True of Epoch to Mean of Epoch
/*! Calculate the rotation matrix for converting coordinates from a system based on the
 * True orientation for that Epoch to one based on the Mean orientation. Removes effects
 * of Nutation.
    \param ep0 Epoch, in units of Modified Julian Day
    \param pm Rotation matrix
*/
int32_t true2mean(double ep0, rmatrix *pm)
{
    static rmatrix opm;
    static double oep0 = 0.;
    //	double nuts[4], jt;
    double eps;
    double cdp, sdp, ce, se, cde, sde;

    if (ep0 == oep0)
    {
        *pm = opm;
        return 0;
    }

    rvector nuts = utc2nuts(ep0);
    eps = utc2epsilon(ep0);
    //	jplnut(utc2tt(ep0),nuts);
    //	jt = (ep0-51544.5)/36525.;
    //	eps = DAS2R*(84381.448+jt*(-46.84024+jt*(-.00059+jt*.001813)));

    se = sin(eps);
    sdp = sin(nuts.col[0]);
    sde = sin(-eps + nuts.col[1]);
    ce = cos(eps);
    cdp = cos(nuts.col[0]);
    cde = cos(-eps + nuts.col[1]);

    pm->row[0].col[0] = cdp;
    pm->row[0].col[1] = sdp * ce;
    pm->row[0].col[2] = sdp * se;

    pm->row[1].col[0] = -sdp * cde;
    pm->row[1].col[1] = cde * cdp * ce - se * sde;
    pm->row[1].col[2] = cde * cdp * se + ce * sde;

    pm->row[2].col[0] = sdp * sde;
    pm->row[2].col[1] = -sde * cdp * ce - se * cde;
    pm->row[2].col[2] = -sde * cdp * se + cde * ce;

    oep0 = ep0;
    opm = *pm;
    return 0;
}

//! Rotate Mean of Epoch to J2000
/*! Calculate the rotation matrix for converting coordinates from a system based on the
 * Mean of the indicated Epoch, to one based on J2000. Reflects the effects of
 * precession.
    \param ep0 Epoch, in units of Modified Julian Day
    \param pm Rotation matrix
*/
int32_t mean2j2000(double ep0, rmatrix *pm)
{
    //	double t0, t, tas2r, w, zeta, z, theta;
    //	double ca, sa, cb, sb, cg, sg;
    static rmatrix opm;
    static double oep0 = 0.;

    if (ep0 == oep0)
    {
        *pm = opm;
        return 0;
    }

    //	double ttc = utc2jcentt(ep0);

    /* Euler angles */
    // Capitaine, et. al, A&A, 412, 567-586 (2003)
    // Expressions for IAU 2000 precession quantities
    // Equation 40
    //	double zeta = (2.650545 + ttc*(2306.083227 + ttc*(0.2988499 + ttc*(0.01801828 + ttc*(-0.000005971 + ttc*(0.0000003173))))))*DAS2R;
    //	double z = (-2.650545 + ttc*(2306.077181 + ttc*(1.0927348 + ttc*(0.01826837 + ttc*(-0.000028596 + ttc*(0.0000002904))))))*DAS2R;
    //	double theta = ttc*(2004.191903 + ttc*(-0.4294934 + ttc*(-0.04182264 + ttc*(-0.000007089 + ttc*(-0.0000001274)))))*DAS2R;
    double zeta = utc2zeta(ep0);
    double theta = utc2theta(ep0);
    double z = utc2z(ep0);

    //	*pm = rm_mmult(rm_change_around_z(zeta),rm_mmult(rm_change_around_y(-theta),rm_change_around_z(z)));
    *pm = rm_mmult(rm_change_around_z(-z), rm_mmult(rm_change_around_y(theta), rm_change_around_z(-zeta)));

    //	zeta = ttc*(2306.2181 + ttc*(0.30188 + ttc*(0.017998)))*DAS2R;
    //	z = zeta + ttc*ttc*(0.79280 + ttc*(0.000205))*DAS2R;
    //	theta = ttc*(2004.3109 + ttc*(-0.42665 + ttc*(-0.041833)))*DAS2R;

    //	ca = cos(zeta);
    //	sa = -sin(zeta);
    //	cb = cos(theta);
    //	sb = sin(theta);
    //	cg = cos(z);
    //	sg = -sin(z);

    //	pm->row[0].col[0] = ca*cb*cg - sa*sg;
    //	pm->row[0].col[1] = cg*sa*cb +ca*sg;
    //	pm->row[0].col[2] = -sb*cg;
    //	pm->row[1].col[0] = -ca*cb*sg - sa*cg;
    //	pm->row[1].col[1] = -sa*cb*sg + ca*cg;
    //	pm->row[1].col[2] = sg*sb;
    //	pm->row[2].col[0] = ca*sb;
    //	pm->row[2].col[1] = sa*sb;
    //	pm->row[2].col[2] = cb;
    opm = *pm;
    return 0;
}

//! ITRS to J2000 rotation matrix
/*! Rotation matrix for transformation from an ITRS coordinate system of date to the
 * J2000 system. Includes all effects of Precession, Nutation, Sidereal Time and Polar Motion.
    \param utc Epoch to change from, UTC in MJD
    \param rnp Pointer to ITRF rotation matrix.
    \param rm pointer to rotation matrix
    \param drm pointer to rotation matrix derivative
    \param ddrm pointer to rotation matrix 2nd derivative
*/
int32_t itrs2gcrf(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm)
{
    static rmatrix orm, odrm, oddrm, ornp;
    static double outc = 0.;

    if (utc == outc)
    {
        *rnp = ornp;
        *rm = orm;
        *drm = odrm;
        *ddrm = oddrm;
    }
    else
    {
        gcrf2itrs(utc, rnp, rm, drm, ddrm);
        ornp = *rnp = rm_transpose(*rnp);
        orm = *rm = rm_transpose(*rm);
        odrm = *drm = rm_transpose(*drm);
        oddrm = *drm = rm_transpose(*ddrm);
        outc = utc;
    }
    return 0;
}

//! J2000 to ITRS rotation matrix
/*! Rotation matrix for transformation from a J2000 coordinate system to the
 * International Terrestrial Reference System of date. Includes all effects of Precession,
 * Nutation, Sidereal Time and Polar Motion.
    \param utc Epoch to change to, UTC in MJD
    \param rnp Pointer to ITRF rotation matrix.
    \param rm pointer to rotation matrix
    \param drm pointer to rotation matrix derivative
    \param ddrm pointer to rotation matrix second derivative
*/
int32_t gcrf2itrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm)
{
    double ut1;
    rmatrix nrm[3], ndrm, nddrm;
    rmatrix pm, nm, sm, pw;
    static rmatrix bm = {{1., -0.000273e-8, 9.740996e-8}, {0.000273e-8, 1., 1.324146e-8}, {-9.740996e-8, -1.324146e-8, 1.}};
    static rmatrix orm, odrm, oddrm, ornp;
    static double outc = 0.;
    static double realsec = 0.;
    int i;

    if (!isfinite(utc))
    {
        return 0;
    }

    if (utc == outc)
    {
        *rm = orm;
        *drm = odrm;
        *ddrm = oddrm;
        *rnp = ornp;
        return 0;
    }

    // Set size of actual second if first time in
    if (realsec == 0.)
    {
        ut1 = utc + 1. / 86400.;
        realsec = ut1 - utc;
    }

    // Do it 3 times to get rate change
    outc = utc;
    utc -= realsec;
    // Calculate bias offset for GCRF to J2000
    gcrf2j2000(&bm);
    for (i = 0; i < 3; i++)
    {
        // Calculate Precession Matrix (pm)
        //		ttc = utc2jcentt(utc);

        /* Euler angles */
        //		zeta = (2.650545 + ttc*(2306.083227 + ttc*(0.2988499 + ttc*(0.01801828 + ttc*(-0.000005971)))))*DAS2R;
        //		z = (-2.650545 + ttc*(2306.077181 + ttc*(1.0927348 + ttc*(0.01826837 + ttc*(-0.000028596)))))*DAS2R;
        //		theta = ttc*(2004.191903 + ttc*(-0.4294934 + ttc*(-0.04182264 + ttc*(-0.000007089 + ttc*(-0.0000001274)))))*DAS2R;
        //		pm = rm_mmult(rm_change_around_z(zeta),rm_mmult(rm_change_around_y(-theta),rm_change_around_z(z)));

        j20002mean(utc, &pm);

        // Calculate Nutation Matrix (nm)
        //		nuts = utc2nuts(utc);
        //		dpsi = nuts.col[0];
        //		deps = nuts.col[1];
        //		eps = utc2epsilon(utc);

        //		se = sin(eps);
        //		sdp = sin(dpsi);
        //		sde = sin(-eps+deps);
        //		ce = cos(eps);
        //		cdp = cos(dpsi);
        //		cde = cos(-eps+deps);

        //		nm = rm_zero();
        //		nm.row[0].col[0] = cdp;
        //		nm.row[1].col[0] = sdp*ce;
        //		nm.row[2].col[0] = sdp*se;

        //		nm.row[0].col[1] = -sdp*cde;
        //		nm.row[1].col[1] = cde*cdp*ce-se*sde;
        //		nm.row[2].col[1] = cde*cdp*se+ce*sde;

        //		nm.row[0].col[2] = sdp*sde;
        //		nm.row[1].col[2] = -sde*cdp*ce-se*cde;
        //		nm.row[2].col[2] = -sde*cdp*se+cde*ce;
        mean2true(utc, &nm);

        // Calculate Earth Rotation (Sidereal Time) Matrix (sm)
        //		gast = utc2gast(utc);
        //		sm = rm_change_around_z(-gast);
        true2pef(utc, &sm);

        //		dsm = rm_zero();
        //		dsm.row[1].col[0] = cos(gast);
        //		dsm.row[0].col[1] = -dsm.row[1].col[0];
        //		dsm.row[0].col[0] = dsm.row[1].col[1] = -sin(gast);
        //		dsm = rm_smult(-.000072921158553,dsm);

        // Calculate Polar Motion (Wander) Matrix (pw)
        //		polm = polar_motion(utc);
        //		pols = -47. * 4.848136811095359935899141e-12 * ttc;

        //		pw = rm_mmult(rm_change_around_z(-pols),rm_mmult(rm_change_around_y(polm.x),rm_change_around_x(polm.y)));
        pef2itrs(utc, &pw);

        // Start with ICRS to J2000 Matrix (bm)
        // Final Matrix = pw * sm * nm * pm * bm
        nrm[i] = rm_mmult(nm, rm_mmult(pm, bm));
        if (i == 1)
            *rnp = nrm[i];

        nrm[i] = rm_mmult(sm, nrm[i]);
        nrm[i] = rm_mmult(pw, nrm[i]);
        utc += realsec;
    }

    nddrm = rm_sub(rm_sub(nrm[2], nrm[1]), rm_sub(nrm[1], nrm[0]));
    ndrm = rm_smult(.5, rm_sub(nrm[2], nrm[0]));
    orm = *rm = (rm_quaternion2dcm(q_dcm2quaternion_rm(nrm[1])));
    odrm = *drm = ndrm;
    oddrm = *ddrm = nddrm;
    ornp = *rnp;
    return 0;
}

/*! Rotation matrix for transformation from a J2000 coordinate system to one based on
 * the Mean of the Epoch.
    \param ep1 Epoch to change to, UTC in MJD
    \param pm pointer to rotation matrix
*/
int32_t j20002mean(double ep1, rmatrix *pm)
{
    double t, tas2r, w, zeta, z, theta;
    double ca, sa, cb, sb, cg, sg;
    static rmatrix opm;
    static double oep1 = 0.;

    if (ep1 == oep1)
    {
        *pm = opm;
        return 0;
    }

    /* Interval over which precession required (JC) */
    t = (ep1 - 51544.5) / 36525.;

    /* Euler angles */
    tas2r = t * DAS2R;
    w = 2306.2181;
    zeta = (w + ((0.30188) + 0.017998 * t) * t) * tas2r;
    z = (w + ((1.09468) + 0.018203 * t) * t) * tas2r;
    theta = ((2004.3109) + ((-0.42665) - 0.041833 * t) * t) * tas2r;

    ca = cos(zeta);
    sa = -sin(zeta);
    cb = cos(theta);
    sb = sin(theta);
    cg = cos(z);
    sg = -sin(z);

    pm->row[0].col[0] = ca * cb * cg - sa * sg;
    pm->row[0].col[1] = cg * sa * cb + ca * sg;
    pm->row[0].col[2] = -sb * cg;
    pm->row[1].col[0] = -ca * cb * sg - sa * cg;
    pm->row[1].col[1] = -sa * cb * sg + ca * cg;
    pm->row[1].col[2] = sg * sb;
    pm->row[2].col[0] = ca * sb;
    pm->row[2].col[1] = sa * sb;
    pm->row[2].col[2] = cb;
    opm = *pm;
    oep1 = ep1;
    return 0;
}

int32_t gcrf2j2000(rmatrix *rm)
{
    // Vallado, Seago, Seidelmann: Implementation Issues Surrounding the New IAU Reference System for Astrodynamics
    static rmatrix bm = {{0.99999999999999, -0.0000000707827974, 0.0000000805621715}, {0.0000000707827948, 0.9999999999999969, 0.0000000330604145}, {-0.0000000805621738, -0.0000000330604088, 0.9999999999999962}};
    *rm = bm;
    return 0;
}

int32_t j20002gcrf(rmatrix *rm)
{
    static rmatrix bm = {{0.99999999999999, -0.0000000707827974, 0.0000000805621715}, {0.0000000707827948, 0.9999999999999969, 0.0000000330604145}, {-0.0000000805621738, -0.0000000330604088, 0.9999999999999962}};
    *rm = rm_transpose(bm);
    return 0;
}

int32_t mean2mean(double ep0, double ep1, rmatrix *pm)
{
    double t0, t, tas2r, w, zeta, z, theta;
    double ca, sa, cb, sb, cg, sg;

    /* Interval between basic epoch J2000.0 and beginning epoch (JC) */
    // t0 = ( ep0 - 2000.0 ) / 100.0;
    t0 = (ep0 - 51544.5) / 36525.;

    /* Interval over which precession required (JC) */
    // t =  ( ep1 - ep0 ) / 100.0;
    t = (ep1 - ep0) / 36525.;

    /* Euler angles */
    tas2r = t * DAS2R;
    w = 2306.2181 + ((1.39656 - (0.000139 * t0)) * t0);
    zeta = (w + ((0.30188 - 0.000344 * t0) + 0.017998 * t) * t) * tas2r;
    z = (w + ((1.09468 + 0.000066 * t0) + 0.018203 * t) * t) * tas2r;
    theta = ((2004.3109 + (-0.85330 - 0.000217 * t0) * t0) + ((-0.42665 - 0.000217 * t0) - 0.041833 * t) * t) * tas2r;

    ca = cos(zeta);
    sa = -sin(zeta);
    cb = cos(theta);
    sb = sin(theta);
    cg = cos(z);
    sg = -sin(z);

    pm->row[0].col[0] = ca * cb * cg - sa * sg;
    pm->row[0].col[1] = cg * sa * cb + ca * sg;
    pm->row[0].col[2] = -sb * cg;
    pm->row[1].col[0] = -ca * cb * sg - sa * cg;
    pm->row[1].col[1] = -sa * cb * sg + ca * cg;
    pm->row[1].col[2] = sg * sb;
    pm->row[2].col[0] = ca * sb;
    pm->row[2].col[1] = sa * sb;
    pm->row[2].col[2] = cb;

    return 0;
}

int32_t cart2peri(cartpos cart, Quaternion &qperi)
{
    Vector S = Vector(cart.s);
    Vector V = Vector(cart.v);
    Vector H = S.cross(V);
    //            Vector nbar = Vector(0, 0, 1).cross(H);
    double c1 = V.norm2() - GM / S.norm();
    double rdotv = S.dot(V);
    Vector ebar = (c1 * S - rdotv * V) / GM;
    qperi = irotate_for(Vector(1., 0., 0.), Vector(0., 0., 1.), ebar, H);

    return 0;
}

int32_t peri2cart(cartpos cart, Quaternion &qcart)
{
    int32_t iretn = cart2peri(cart, qcart);
    if (iretn < 0)
    {
        return iretn;
    }
    qcart = qcart.conjugate();
    return 0;
}

/**
         * @brief Converts RIC coordinates to ECI
         *
         * Requires origin's position and velocity to be set.
         * Returns RIC-offsetted position and velocity in result.
         *
         * @param orig Origin of the RIC frame in ECI
         * @param ric Offset from origin of RIC frame, [0,1,2] = [R,I,C]
         * @param result Converted ECI coordinates
         * @return int32_t 0 on success, negative on error
         */
int32_t ric2eci(cartpos orig, rvector ric, cartpos& result)
{
    return ric2eci(orig, Vector(ric), result);
}

/**
         * @brief Converts RIC coordinates to ECI
         *
         * Requires origin's position and velocity to be set.
         * Returns RIC-offsetted position and velocity in result.
         *
         * @param orig Origin of the RIC frame in ECI
         * @param ric Offset from origin of RIC frame, [x, y, z] = [R,I,C]
         * @param result Converted ECI coordinates
         * @return int32_t 0 on success, negative on error
         */
int32_t ric2eci(cartpos orig, Vector ric, cartpos& result)
{
    // 1 set the radius
    double rad = length_rv(orig.s);

    // 2 set the unit normal (to the position / velocity plane)
    rvector u_n_orig = rv_cross(orig.s, orig.v);
    normalize_rv(u_n_orig);

    // 3 correct the unit velocity and then velocity vector to be the one for a completely circular orbit
    rvector u_v_orig = rv_cross(u_n_orig, orig.s);
    normalize_rv(u_v_orig);
    // expand to actual circular velocity
    double v_mag = sqrt(GM / length_rv(orig.s));
    // orig.v = u_v_orig * v_mag;

    // 4 Find rotations axises for I and C rotations
    rvector I_rotation_axis = u_n_orig;
    rvector C_rotation_axis = u_v_orig * -1.0;

    // first rotate by I about the normal
    // position
    rvector pos = rv_rotate(orig.s, I_rotation_axis, ric.y/rad);
    // velocity
    rvector vel = rv_rotate(orig.v, I_rotation_axis, ric.y/rad);

    // second rotate by C about the velocity (actually the rotated velocity!)
    // rotate the velocity axis
    C_rotation_axis = rv_rotate(C_rotation_axis, I_rotation_axis, ric.y/rad);

    // position
    pos = rv_rotate(pos, C_rotation_axis, ric.z/rad);
    // velocity
    vel = rv_rotate(vel, C_rotation_axis, ric.z/rad);

    // expand to new radius
    pos = pos * ((ric.x+rad)/rad);

    // correct to new velocity of new circular radius
    vel = vel * (sqrt(GM / length_rv(pos)) / v_mag);

    result.s = pos;
    result.v = vel;

    return 0;
}

/**
         * @brief Converts Origin coordinates to RIC offset coordinates
         *
         * Requires loc.pos to be fully set.
         * Leaves RIC offset position, velocity and acceleration in loc.
         *
         * @param ric ::cartpos RIC offsets to apply
         * @param loc ::locstruc containing Origin to convert
         * @return int32_t 0 on success, negative on error
         */
int32_t pos_ric2eci(cartpos ric, locstruc& loc)
{
    //            qatt *patt;
    cartpos *ppos;
    switch (loc.pos.extra.closest)
    {
    case COSMOS_EARTH:
    default:
        // Check time
        if (!isfinite(loc.att.geoc.utc) || loc.att.geoc.utc == 0.)
        {
            return CONVERT_ERROR_UTC;
        }

        //                patt = &loc.att.geoc;
        ppos = &loc.pos.geoc;
        break;
    case COSMOS_MOON:
        // Check time
        if (!isfinite(loc.att.selc.utc) || loc.att.selc.utc == 0.)
        {
            return CONVERT_ERROR_UTC;
        }

        //                patt = &loc.att.selc;
        ppos = &loc.pos.selc;
        break;
    }

    // 1 set the radius
    double rad = length_rv(ppos->s);

    // 2 set the unit normal (to the position / velocity plane)
    rvector u_n_orig = rv_cross(ppos->s, ppos->v);
    normalize_rv(u_n_orig);

    // 3 correct the unit velocity and then velocity vector to be the one for a completely circular orbit
    rvector u_v_orig = rv_cross(u_n_orig, ppos->s);
    normalize_rv(u_v_orig);

    // expand to actual circular velocity
    double v_mag = sqrt(GM / length_rv(ppos->s));
    // ppos->v = u_v_orig * v_mag;

    // 4 Find rotations axises for I and C rotations
    rvector I_rotation_axis = u_n_orig;
    rvector C_rotation_axis = -u_v_orig;

    // first rotate by I about the normal
    ppos->s = rv_rotate(ppos->s, I_rotation_axis, ric.s.col[1]/rad);
    ppos->v = rv_rotate(ppos->v, I_rotation_axis, ric.s.col[1]/rad);
    ppos->a = rv_rotate(ppos->a, I_rotation_axis, ric.s.col[1]/rad);

    // second rotate by C about the velocity (actually the rotated velocity!)
    // rotate the velocity axis
    C_rotation_axis = rv_rotate(C_rotation_axis, I_rotation_axis, ric.s.col[1]/rad);

    ppos->s = rv_rotate(ppos->s, C_rotation_axis, ric.s.col[2]/rad);
    ppos->v = rv_rotate(ppos->v, C_rotation_axis, ric.s.col[2]/rad);
    ppos->a = rv_rotate(ppos->a, C_rotation_axis, ric.s.col[2]/rad);

    // expand to new radius
    ppos->s *= ((ric.s.col[0]+rad)/rad);

    // correct to new velocity of new circular radius
    ppos->v *= (sqrt(GM / length_rv(ppos->s)) / v_mag);


    return 0;
}

/**
         * @brief Converts Origin coordinates to LVLH offset coordinates
         *
         * Convert position as if it was Origin (zero LVLH offsets) into Offset (LVLH offsets applied).
         * Leaves LVLH offset position, velocity and acceleration in loc.pos.lvlh.
         *
         * @param loc ::locstruc containing Origin to convert to Offset
         * @param lvlh ::cartpos LVLH offsets to apply
         * @return int32_t 0 on success, negative on error
         */
int32_t pos_origin2lvlh(locstruc *loc, cartpos lvlh)
{
    return pos_origin2lvlh(*loc, lvlh);
}

int32_t pos_origin2lvlh(locstruc& loc, cartpos lvlh)
{
    rvector lvlh_x;
    rvector lvlh_y;
    rvector lvlh_z;
    locstruc tloc1 = loc;
//    rvector h = rv_cross(loc.pos.eci.s, loc.pos.eci.v);

    cartpos origin = tloc1.pos.eci;
    // 1 Get lvlh basis vectors
    lvlh_z = -rv_normal(origin.s);
    lvlh_y = rv_normal(rv_cross(lvlh_z, origin.v));
    lvlh_x = rv_normal(rv_cross(lvlh_y, lvlh_z));

    // 2 Convert LVLH offsets into ECI
    cartpos eci_offset;
    eci_offset.s.col[0] = lvlh_x.col[0]*lvlh.s.col[0] + lvlh_y.col[0]*lvlh.s.col[1] + lvlh_z.col[0]*lvlh.s.col[2];
    eci_offset.s.col[1] = lvlh_x.col[1]*lvlh.s.col[0] + lvlh_y.col[1]*lvlh.s.col[1] + lvlh_z.col[1]*lvlh.s.col[2];
    eci_offset.s.col[2] = lvlh_x.col[2]*lvlh.s.col[0] + lvlh_y.col[2]*lvlh.s.col[1] + lvlh_z.col[2]*lvlh.s.col[2];
    eci_offset.v.col[0] = lvlh_x.col[0]*lvlh.v.col[0] + lvlh_y.col[0]*lvlh.v.col[1] + lvlh_z.col[0]*lvlh.v.col[2];
    eci_offset.v.col[1] = lvlh_x.col[1]*lvlh.v.col[0] + lvlh_y.col[1]*lvlh.v.col[1] + lvlh_z.col[1]*lvlh.v.col[2];
    eci_offset.v.col[2] = lvlh_x.col[2]*lvlh.v.col[0] + lvlh_y.col[2]*lvlh.v.col[1] + lvlh_z.col[2]*lvlh.v.col[2];
    eci_offset.a.col[0] = lvlh_x.col[0]*lvlh.a.col[0] + lvlh_y.col[0]*lvlh.a.col[1] + lvlh_z.col[0]*lvlh.a.col[2];
    eci_offset.a.col[1] = lvlh_x.col[1]*lvlh.a.col[0] + lvlh_y.col[1]*lvlh.a.col[1] + lvlh_z.col[1]*lvlh.a.col[2];
    eci_offset.a.col[2] = lvlh_x.col[2]*lvlh.a.col[0] + lvlh_y.col[2]*lvlh.a.col[1] + lvlh_z.col[2]*lvlh.a.col[2];

    tloc1.pos.eci.s = origin.s + eci_offset.s;

    // This is the equation to find the velocity of a point using observations
    // from a translating and rotating reference frame B
    // A_v_P = B_v_P/Q + A_v_Q
    //       + (A_w_B x r_P/Q)    = Tangential velocity
    // where:
    // Reference frame A: The inertial reference frame
    // Reference frame B: The rotating and translating reference frame
    // Point P: The point that moves with respect to point Q
    // Point Q: The origin of reference frame B
    // A_v_P: The velocity of point P in reference frame A. The unknown quantity to solve for.
    // B_v_P/Q: The velocity of point P in reference frame B as observed from point Q.
    // A_v_Q: The velocity of point Q in reference frame A.
    // A_w_B: The angular velocity of reference frame B
    // r_P/Q: The distance between points P and Q (in reference frame A)

    // B_v_P/Q = eci_offset.v
    // A_v_Q = origin.v
    // A_w_B = angular velocity = (s x v) / ||s||^2
    rvector angular_velocity = rv_smult(1./pow(length_rv(origin.s),2),rv_cross(origin.s,origin.v));
    // r_P/Q = eci_offset.s

    // A_w_B x r_P/Q
    rvector w_x_r = rv_cross(angular_velocity, eci_offset.s);

    // Compute B_v_P/Q + A_v_Q + (A_w_B x r_P/Q)
    tloc1.pos.eci.v = eci_offset.v + origin.v + w_x_r;

    // This is the equation to find the acceleration of a point using observations
    // from a rotating and translating reference frame B
    // A_a_P = A_a_Q + B_a_P/Q
    //       + A_alpha_B x r_P/Q        = Euler acceleration
    //       + 2 * A_w_B x B_v_P/Q      = Coriolis acceleration
    //       + A_w_B x (A_w_B x r_P/Q)  = Centripetal acceleration
    // where the new terms are:
    // A_a_P: The acceleration of point P in reference frame A. The unknown quantity to solve for.
    // A_a_Q: The acceleration of point Q in reference frame A.
    // B_a_P/Q: The acceleration of point P in reference frame B as observed from point Q.
    // A_alpha_B = The angular acceleration of reference frame B as observed by reference frame A.

    // A_a_Q = origin.a
    // B_a_P/Q = eci_offset.a
    // A_alpha_B = angular acceleration = (s x a) / ||s||^2
    rvector angular_acceleration = rv_smult(1./pow(length_rv(origin.s),2),rv_cross(origin.s,origin.a));

    // Compute
    // A_a_P = A_a_Q + B_a_P/Q
    tloc1.pos.eci.a = origin.a + eci_offset.a
                    //     + A_alpha_B x r_P/Q        = Euler acceleration
                    + rv_cross(angular_acceleration, eci_offset.s)
                    //     + 2 * A_w_B x B_v_P/Q      = Coriolis acceleration
                    + 2 * rv_cross(angular_velocity, eci_offset.v)
                    //     + A_w_B x (A_w_B x r_P/Q)  = Centripetal acceleration
                    + rv_cross(angular_velocity, w_x_r);

    tloc1.pos.eci.pass = std::max(tloc1.pos.icrf.pass, tloc1.pos.geoc.pass) + 1;
    tloc1.pos.eci.utc = origin.utc;
    ++tloc1.pos.eci.pass;
    pos_eci(tloc1);
//    printf("Scott:\n");
//    printf("x %.1f y %.1f z %.1f mag %.1f\n", tloc1.pos.eci.s.col[0], tloc1.pos.eci.s.col[1], tloc1.pos.eci.s.col[2], length_rv(tloc1.pos.eci.s));
//    printf("vx %.2f vy %.2f vz %.2f mag %.2f\n", tloc1.pos.eci.v.col[0], tloc1.pos.eci.v.col[1], tloc1.pos.eci.v.col[2], length_rv(tloc1.pos.eci.v));
//    printf("ax %.3f ay %.3f az %.3f mag %.3f\n", tloc1.pos.eci.a.col[0], tloc1.pos.eci.a.col[1], tloc1.pos.eci.a.col[2], length_rv(tloc1.pos.eci.a));

    locstruc tloc = loc;
    eci_offset.s = rv_mmult(tloc.pos.extra.p2l, lvlh.s);
    tloc.pos.eci.s += eci_offset.s;
    tloc.pos.eci.v += rv_mmult(tloc.pos.extra.dp2l, lvlh.v);
    tloc.pos.eci.a += rv_mmult(tloc.pos.extra.ddp2l, lvlh.a);
    tloc.pos.eci.pass++;
    pos_eci(tloc);
//    printf("Eric:\n");
//    printf("x %.1f y %.1f z %.1f mag %.1f\n", tloc.pos.eci.s.col[0], tloc.pos.eci.s.col[1], tloc.pos.eci.s.col[2], length_rv(tloc.pos.eci.s));
//    printf("vx %.2f vy %.2f vz %.2f mag %.2f\n", tloc.pos.eci.v.col[0], tloc.pos.eci.v.col[1], tloc.pos.eci.v.col[2], length_rv(tloc.pos.eci.v));
//    printf("ax %.3f ay %.3f az %.3f mag %.3f\n", tloc.pos.eci.a.col[0], tloc.pos.eci.a.col[1], tloc.pos.eci.a.col[2], length_rv(tloc.pos.eci.a));
//    fflush(stdout);

    loc = tloc1;    
    loc.pos.lvlh = lvlh;
    loc.pos.lvlh.utc = loc.pos.eci.utc;
    return 0;

    lvlh_x = drotate(loc.pos.extra.e2l, rvector(1., 0., 0.));
    lvlh_y = drotate(loc.pos.extra.e2l, rvector(0., 1., 0.));

    loc.pos.geoc.v += drotate(loc.pos.extra.e2l, lvlh.v);
    loc.pos.geoc.a += drotate(loc.pos.extra.e2l, lvlh.a);
    loc.pos.geoc.j += drotate(loc.pos.extra.e2l, lvlh.j);

    double r = length_rv(loc.pos.geoc.s);

    // Rotate around LVLH y axis by -dx/r
    loc.pos.geoc.s = rv_rotate(loc.pos.geoc.s, lvlh_y, -lvlh.s.col[0] / r);

    // Rotate forwards around LVLH x axis by dy/r
    loc.pos.geoc.s = rv_rotate(loc.pos.geoc.s, lvlh_x, lvlh.s.col[1] / r);

    // Scale the whole thing by dz/z
    loc.pos.geoc.s *= ((r - lvlh.s.col[2]) / r);

    // Set LVLH
    loc.pos.lvlh = lvlh;
    loc.pos.lvlh.utc = loc.pos.geoc.utc;

    // Set geoc
    ++loc.pos.geoc.pass;
    pos_geoc(loc);

    return 0;
}

/**
         * @brief Converts LVLH offset coordinates to base coordinates
         *
         * Requires loc.pos to be fully set with current LVLH offset position.
         *
         * @param loc ::locstruc containing Offset to convert to Origin
         * @param lvlh ::cartpos LVLH offsets to apply
         * @return int32_t 0 on success, negative on error
         */
int32_t pos_lvlh2origin(locstruc* loc)
{
    return pos_lvlh2origin(*loc);
}

int32_t pos_lvlh2origin(locstruc& loc)
{
    // Save for later
    //    cartpos geoc = loc.pos.geoc;

    rvector lvlh_x;
    rvector lvlh_y;
    rvector lvlh_z;

    double r = length_rv(loc.pos.geoc.s) +  loc.pos.lvlh.s.col[2];

    lvlh_x = drotate(loc.pos.extra.e2l, rvector(1., 0., 0.));
    lvlh_y = drotate(loc.pos.extra.e2l, rvector(0., 1., 0.));
    lvlh_z = drotate(loc.pos.extra.e2l, rvector(0., 0., 1.));

    // TODO: should this be ECI, not GEOC?
    loc.pos.geoc.v -= drotate(loc.pos.extra.e2l, loc.pos.lvlh.v);
    loc.pos.geoc.a -= drotate(loc.pos.extra.e2l, loc.pos.lvlh.a);
    loc.pos.geoc.j -= drotate(loc.pos.extra.e2l, loc.pos.lvlh.j);

    // Rotate around LVLH y axis by dx/r
    loc.pos.geoc.s = rv_rotate(loc.pos.geoc.s, lvlh_y, loc.pos.lvlh.s.col[0] / r);

    // Rotate backwards around LVLH x axis by -dy/r
    loc.pos.geoc.s = rv_rotate(loc.pos.geoc.s, lvlh_x, -loc.pos.lvlh.s.col[1] / r);

    // Scale by dz/z
    loc.pos.geoc.s *= (r / (r - loc.pos.lvlh.s.col[2]));

    ++loc.pos.geoc.pass;
    pos_geoc(loc);

    loc.pos.lvlh = cartpos();

    return 0;
}

/**
         * @brief Converts Origin coordinates to RIC offset coordinates
         *
         * Convert position as if it was Origin (zero RIC offsets) into Offset (RIC offsets applied).
         * Leaves RIC offset position, velocity and acceleration in loc.pos.lvlh.
         *
         * @param loc ::locstruc containing Origin to convert to Offset
         * @param lvlh ::cartpos RIC offsets to apply
         * @return int32_t 0 on success, negative on error
         */
int32_t ric2lvlh(cartpos ric, cartpos& lvlh)
{
    lvlh.s.col[0] = ric.s.col[1];
    lvlh.s.col[1] = -ric.s.col[2];
    lvlh.s.col[2] = -ric.s.col[0];

    lvlh.v.col[0] = ric.v.col[1];
    lvlh.v.col[1] = -ric.v.col[2];
    lvlh.v.col[2] = -ric.v.col[0];

    lvlh.a.col[0] = ric.a.col[1];
    lvlh.a.col[1] = -ric.a.col[2];
    lvlh.a.col[2] = -ric.a.col[0];

    lvlh.j.col[0] = ric.j.col[1];
    lvlh.j.col[1] = -ric.j.col[2];
    lvlh.j.col[2] = -ric.j.col[0];

    return 0;
}

/**
         * @brief Converts RIC offset coordinates to LVLH coordinates
         *
         * Requires loc.pos to be fully set with current RIC offset position.
         *
         * @param loc ::locstruc containing Offset to convert to Origin
         * @param ric ::cartpos RIC offsets to apply
         * @return int32_t 0 on success, negative on error
         */
int32_t lvlh2ric(cartpos lvlh, cartpos& ric)
{
    ric.s.col[0] = -lvlh.s.col[2];
    ric.s.col[1] = lvlh.s.col[0];
    ric.s.col[2] = -lvlh.s.col[1];

    ric.v.col[0] = -lvlh.v.col[2];
    ric.v.col[1] = lvlh.v.col[0];
    ric.v.col[2] = -lvlh.v.col[1];

    ric.a.col[0] = -lvlh.a.col[2];
    ric.a.col[1] = lvlh.a.col[0];
    ric.a.col[2] = -lvlh.a.col[1];

    ric.j.col[0] = -lvlh.j.col[2];
    ric.j.col[1] = lvlh.j.col[0];
    ric.j.col[2] = -lvlh.j.col[1];

    return 0;
}

int32_t kep2eci(kepstruc &kep, cartpos &eci)
{
    rvector qpos, qvel;
    double s1, s2, s3, c1, c2, c3;
    double xx, xy, xz, yx, yy, yz, zx, zy, zz;
    double sea, cea, sqe;

    sea = sin(kep.ea);
    cea = cos(kep.ea);
    sqe = sqrt(1. - kep.e * kep.e);

    qpos.col[0] = kep.a * (cea - kep.e);
    qpos.col[1] = kep.a * sqe * sea;
    qpos.col[2] = 0.;

    // find mean motion and period
    kep.mm = sqrt(GM / pow(kep.a, 3.));
    kep.period = 2. * DPI / kep.mm;
    qvel.col[0] = -kep.mm * kep.a * sea / (1. - kep.e * cea);
    qvel.col[1] = kep.mm * kep.a * sqe * cea / (1. - kep.e * cea);
    qvel.col[2] = 0.;

    s1 = sin(kep.raan);
    c1 = cos(kep.raan);
    s2 = sin(kep.i);
    c2 = cos(kep.i);
    s3 = sin(kep.ap);
    c3 = cos(kep.ap);

    xx = c1 * c3 - s1 * c2 * s3;
    xy = -c1 * s3 - s1 * c2 * c3;
    xz = s1 * s2;

    yx = s1 * c3 + c1 * c2 * s3;
    yy = -s1 * s3 + c1 * c2 * c3;
    yz = -c1 * s2;

    zx = s2 * s3;
    zy = s2 * c3;
    zz = c2;

    eci.s = eci.v = eci.a = eci.j = rv_zero();

    eci.s.col[0] = qpos.col[0] * xx + qpos.col[1] * xy + qpos.col[2] * xz;
    eci.s.col[1] = qpos.col[0] * yx + qpos.col[1] * yy + qpos.col[2] * yz;
    eci.s.col[2] = qpos.col[0] * zx + qpos.col[1] * zy + qpos.col[2] * zz;

    eci.v.col[0] = qvel.col[0] * xx + qvel.col[1] * xy + qvel.col[2] * xz;
    eci.v.col[1] = qvel.col[0] * yx + qvel.col[1] * yy + qvel.col[2] * yz;
    eci.v.col[2] = qvel.col[0] * zx + qvel.col[1] * zy + qvel.col[2] * zz;

    eci.utc = kep.utc;

    return 0;
}

int32_t eci2kep(cartpos &eci, kepstruc &kep)
{
    double rmag, rmag2=D_SMALL, rmag3=D_SMALL, drmag=0.;
    double vmag, vmag2=D_SMALL, vmag3=D_SMALL, vmag4=D_SMALL, vmag5=D_SMALL, vmag6=D_SMALL, dvmag=0.;
    double magn, sme, rdotv, temp;
    double c1, hk, magh;
    rvector nbar, ebar, rsun, hsat;
    cartpos earthpos;

    kep.utc = eci.utc;
    kep.ddmm = 0.;

    rmag = length_rv(eci.s);
    if (rmag < D_SMALL)
    {
        rmag = D_SMALL;
    }
    else
    {
        rmag2= rmag * rmag;
        rmag3 = rmag * rmag2;
        drmag = (eci.s.col[0] * eci.v.col[0] + eci.s.col[1] * eci.v.col[1] + eci.s.col[2] * eci.v.col[2]) / rmag;
    }
    vmag = length_rv(eci.v);
    if (vmag < D_SMALL)
    {
        vmag = D_SMALL;
    }
    else
    {
        vmag2 = vmag * vmag;
        vmag3 = vmag * vmag2;
        vmag4 = vmag * vmag3;
        vmag5 = vmag * vmag4;
        vmag6 = vmag * vmag5;
        dvmag = (eci.v.col[0] * eci.a.col[0] + eci.v.col[1] * eci.a.col[1] + eci.v.col[2] * eci.a.col[2]) / vmag;
    }
    kep.h = rv_cross(eci.s, eci.v);
    if (rmag == D_SMALL && vmag == D_SMALL)
        kep.fa = acos(1. / D_SMALL);
    else
        kep.fa = acos(length_rv(kep.h) / (rmag * vmag));
    kep.eta = vmag2 / 2. - GM / rmag;
    magh = length_rv(kep.h);
    jplpos(JPL_EARTH, JPL_SUN_BARY, utc2tt(eci.utc), &earthpos);
    earthpos.utc = eci.utc;
    rsun = earthpos.s;
    normalize_rv(rsun);
    hsat = kep.h;
    normalize_rv(hsat);
    kep.beta = asin(dot_rv(rsun, hsat));

    if (magh > O_SMALL)
    {
        // ------------  find a e and semi-latus rectum   ----------
        nbar.col[0] = -kep.h.col[1];
        nbar.col[1] = kep.h.col[0];
        nbar.col[2] = 0.0;
        magn = length_rv(nbar);
        c1 = vmag2 - GM / rmag;
        rdotv = dot_rv(eci.s, eci.v);
        ebar.col[0] = (c1 * eci.s.col[0] - rdotv * eci.v.col[0]) / GM;
        ebar.col[1] = (c1 * eci.s.col[1] - rdotv * eci.v.col[1]) / GM;
        ebar.col[2] = (c1 * eci.s.col[2] - rdotv * eci.v.col[2]) / GM;
        kep.e = length_rv(ebar);

        sme = (vmag2 * 0.5) - (GM / rmag);
        if (fabs(sme) > O_SMALL)
        {
            kep.a = -GM / (2. * sme);
            kep.mm = sqrt(GM / pow(kep.a, 3.));
            kep.dmm = (-6. * vmag6 / GM2 + 6. * vmag4 / (rmag * GM) - 24. * vmag / rmag2) * dvmag;
            kep.dmm += (-6. * vmag4 / (rmag2 * GM) + 24. * vmag2 / rmag3 - 24. * GM / rmag3) * drmag;
            kep.dmm /= 2. * kep.mm;
        }
        else
        {
            kep.a = O_INFINITE;
            kep.mm = O_SMALL;
            kep.dmm = D_SMALL;
        }
        kep.period = 2. * DPI / kep.mm;

        // find mean motion and period

        // -----------------  find inclination   -------------------
        hk = kep.h.col[2] / magh;
        kep.i = acos(hk);

        // ----------  find longitude of ascending node ------------
        if (magn > O_SMALL)
        {
            temp = nbar.col[0] / magn;
            if (fabs(temp) > 1.)
                temp = temp < 1. ? -1. : (temp > 1. ? 1. : 0.);
            kep.raan = acos(temp);
            if (nbar.col[1] < 0.)
                kep.raan = D2PI - kep.raan;
        }
        else
            kep.raan = O_UNDEFINED;

        // Find eccentric and mean anomaly
        kep.ea = atan2(rdotv / sqrt(kep.a * GM), 1. - rmag / kep.a);
        kep.ma = kep.ea - kep.e * sin(kep.ea);

        // Find argument of latitude
        kep.alat = atan2(eci.s.col[2], (eci.s.col[1] * kep.h.col[0] - eci.s.col[0] * kep.h.col[1]) / magh);

        // Find true anomaly
        kep.ta = atan2(sqrt(1 - kep.e * kep.e) * sin(kep.ea), cos(kep.ea) - kep.e);

        // Find argument of perigee
        kep.ap = kep.alat - kep.ta;
    }
    else
    {
        kep.a = kep.e = kep.i = kep.raan = O_UNDEFINED;
        kep.ap = kep.alat = kep.ma = kep.ta = kep.ea = kep.mm = kep.dmm = kep.ddmm = O_UNDEFINED;
    }

    return 0;
}

//! Geocentric to Topocentric
/*! Calculate the Topocentric position of a Target with respect to a Source.
         * \param source Geodetic location of Source.
         * \param targetgeoc Geocentric location of Target.
         * \param topo Resulting Topocentric position.
         */
int32_t geoc2topo(gvector source, rvector targetgeoc, rvector &topo)
{
    rmatrix g2t;
    double clat, clon, slat, slon;
    double lst, r, c, s;
    double cs, ct, ss, st;
    rvector sourcegeoc;

    clon = cos(source.lon);
    slon = sin(source.lon);
    clat = cos(source.lat);
    slat = sin(source.lat);

    g2t.row[0].col[0] = -slon;
    g2t.row[0].col[1] = clon;
    g2t.row[0].col[2] = 0.;
    g2t.row[1].col[0] = -slat * clon;
    g2t.row[1].col[1] = -slat * slon;
    g2t.row[1].col[2] = clat;
    g2t.row[2].col[0] = clat * clon;
    g2t.row[2].col[1] = clat * slon;
    g2t.row[2].col[2] = slat;

    ct = cos(source.lat);
    st = sin(source.lat);
    c = 1. / sqrt(ct * ct + FRATIO2 * st * st);
    s = FRATIO2 * c;
    r = (REARTHM * c + source.h) * ct;
    sourcegeoc.col[2] = (REARTHM * s + source.h) * st;

    lst = source.lon;
    cs = cos(lst);
    ss = sin(lst);
    sourcegeoc.col[0] = r * cs;
    sourcegeoc.col[1] = r * ss;

    topo = rv_mmult(g2t, rv_sub(targetgeoc, sourcegeoc));

    return 0;
}

//! Body Centric to Topocentric
/*! Calculate the Topocentric position of a Target with respect to a Source.
         * \param source Body centered location of Source.
         * \param target Body centered location of Target.
         * \param topo Resulting Topocentric position.
         */
int32_t body2topo(Vector source, Vector target, Vector &topo)
{
    Matrix b2t;
    double clat, clon, slat, slon;

    double llon = sqrt(source[0] * source[0] + source[1] * source[1]);
    if (llon > 0.)
    {
        clon = source[0] / llon;
        slon = source[1] / llon;
    }
    else
    {
        clon = 1.;
        slon = 0.;
    }

    double llat = sqrt(llon * llon + source[2] * source[2]);
    if (llat > 0.)
    {
        clat = llon / llat;
        slat = source[2] / llat;
    }
    else
    {
        clat = 1.;
        slat = 0.;
    }

    b2t[0][0] = -slon;
    b2t[0][1] = clon;
    b2t[0][2] = 0.;
    b2t[1][0] = -slat * clon;
    b2t[1][1] = -slat * slon;
    b2t[1][2] = clat;
    b2t[2][0] = clat * clon;
    b2t[2][1] = clat * slon;
    b2t[2][2] = slat;

    topo = b2t * (target - source);

    return 0;
}

//! Topocentric to Azimuth and Eleveation
//! Calculate the Azimuth and Elevation of a position based on its Topocentric position
//! \param tpos Topocentric position
//! \param az Calculated azimuth in radians
//! \param el Calculated elevation in radians
int32_t topo2azel(rvector tpos, float &az, float &el)
{
    az = static_cast<float>(atan2(tpos.col[0], tpos.col[1]));
    el = static_cast<float>(atan2(tpos.col[2], sqrt(tpos.col[0] * tpos.col[0] + tpos.col[1] * tpos.col[1])));
    return 0;
}

//! Topocentric to Azimuth and Eleveation
//! Calculate the Azimuth and Elevation of a position based on its Topocentric position
//! \param tpos Topocentric position
//! \param az Calculated azimuth in radians
//! \param el Calculated elevation in radians
int32_t topo2azel(Vector tpos, float &az, float &el)
{
    az = static_cast<float>(atan2(tpos[0], tpos[1]));
    el = static_cast<float>(atan2(tpos[2], sqrt(tpos[0] * tpos[0] + tpos[1] * tpos[1])));
    return 0;
}

//! Geodetic to Separation
//! Calculate the separation distance between two geodetic positions.
//! \param src Starting position
//! \param dst Target position
//! \param sep Reference to distance between the two
int32_t geod2sep(gvector src, gvector dst, double &sep)
{
    //            sep = acos( sin(src.lat) * sin(dst.lat) + cos(src.lat) * cos(dst.lat) * cos(dst.lat - src.lat) ) * REARTHM;
    sep = geod2sep(src, dst);
    return 0;
}

double geod2sep(gvector src, gvector dst)
{
    double dphi = dst.lat - src.lat;
    double dlambda = dst.lon - src.lon;
    double a = sin(dphi / 2.) * sin(dphi / 2.) + cos(src.lat) * cos(dst.lat) * sin(dlambda / 2.) * sin(dlambda / 2.);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return REARTHM * c;
}

//! Return position from TLE set
/*! Find the TLE closest to, but not exceeding the provided utc, then
 * return the position in ECI coordinates.
    \param utc Coordinated Universal Time in Modified Julian Days.
    \param lines Vector of TLE's.
    \param eci Pointer to ::Cosmos::Convert::cartpos in ECI frame.
    */
int lines2eci(double utc, vector<tlestruc> lines, cartpos &eci)
{
    uint16_t lindex = 0;
    int32_t iretn = 0;

    if (utc >= lines[lindex].utc)
    {
        for (uint16_t i = lindex; i < lines.size(); i++)
        {
            if (utc >= lines[i].utc)
            {
                lindex = i;
            }
            else
            {
                break;
            }
        }
    }
    else
        return (TLE_ERROR_OUTOFRANGE);

    iretn = tle2eci(utc, lines[lindex], eci);
    return iretn;
}

/**
         * SGP4 propagator algoritm
         * @param utc Specified time as Modified Julian Date
         * @param tle Two Line Element structure, given as pointer to a ::Cosmos::Convert::tlestruc
         * @param pos_teme result from SGP4 algorithm is a cartesian state given in TEME frame, as pointer to a ::Cosmos::Convert::cartpos
         */
int sgp4(double utc, tlestruc tle, cartpos &pos_teme)
{
    //	rmatrix pm = {{{{0.}}}};
    //	static int lsnumber=-99;
    static double c1 = 0.;
    static double cosio = 0., x3thm1 = 0., xnodp = 0., aodp = 0., isimp = 0., eta = 0., sinio = 0., x1mth2 = 0., c4 = 0., c5 = 0.;
    static double xmdot = 0., omgdot = 0., xnodot = 0., omgcof = 0., xmcof = 0., xnodcf = 0., t2cof = 0., xlcof = 0., aycof = 0.;
    int i;
    double temp, temp1, temp2, temp3, temp4, temp5, temp6;
    double tempa, tempe, templ;
    // Mean motion per minute
    double mmm;
    double ao, a1, c2, c3, coef, coef1, theta4, c1sq;
    double theta2, betao2, betao, delo, del1, s4, qoms24, x1m5th, xhdot1;
    double perige, eosq, pinvsq, tsi, etasq, eeta, psisq, g, xmdf;
    double tsince, omgadf, alpha, xnoddf, xmp, tsq, xnode, delomg, delm;
    double tcube, tfour, a, e, xl, beta, axn, xn, xll, ayn, capu, aynl;
    double xlt, sinepw, cosepw, epw, ecose, esine, pl, r, elsq;
    double rdot, rfdot, cosu, sinu, u, sin2u, cos2u, uk, rk, ux, uy, uz;
    double vx, vy, vz, xinck, rdotk, rfdotk, sinuk, cosuk, sinik, cosik, xnodek;
    double xmx, xmy, sinnok, cosnok;
    //	locstruc loc;
    static double lutc = 0.;
    static uint16_t lsnumber = 0;

    static double delmo, sinmo, x7thm1, d2, d3, d4, t3cof, t4cof, t5cof, betal;

    if (tle.utc != lutc || tle.snumber != lsnumber)
    {
        if (tle.e < .00000000001)
        {
            tle.e = .00000000001;
        }
        // RECOVER ORIGINAL MEAN MOTION ( xnodp ) AND SEMIMAJOR AXIS (aodp)
        // FROM INPUT ELEMENTS
        mmm = tle.mm * 60.;
        a1 = pow((SGP4_XKE / mmm), SGP4_TOTHRD);
        cosio = cos(tle.i);
        theta2 = cosio * cosio;
        x3thm1 = 3. * theta2 - 1.;
        eosq = tle.e * tle.e;
        betao2 = 1. - eosq;
        betao = sqrt(betao2);
        del1 = 1.5 * SGP4_CK2 * x3thm1 / (a1 * a1 * betao * betao2);
        ao = a1 * (1. - del1 * (.5 * SGP4_TOTHRD + del1 * (1. + 134. / 81. * del1)));
        delo = 1.5 * SGP4_CK2 * x3thm1 / (ao * ao * betao * betao2);
        xnodp = mmm / (1. + delo);
        aodp = ao / (1. - delo);
        // INITIALIZATION
        // FOR PERIGEE LESS THAN 220 KILOMETERS, THE isimp FLAG IS SET AND
        // THE EQUATIONS ARE TRUNCATED TO LINEAR VARIATION IN sqrt A AND
        // QUADRATIC VARIATION IN MEAN ANOMALY. ALSO, THE c3 TERM, THE
        // DELTA alpha TERM, AND THE DELTA M TERM ARE DROPPED.
        isimp = 0;
        if ((aodp * (1. - tle.e) / SGP4_AE) < (220. / SGP4_XKMPER + SGP4_AE))
            isimp = 1;
        // FOR PERIGEE BELOW 156 KM, THE VALUES OF
        // S AND SGP4_QOMS2T ARE ALTERED
        s4 = SGP4_S;
        qoms24 = SGP4_QOMS2T;
        perige = (aodp * (1. - tle.e) - SGP4_AE) * SGP4_XKMPER;
        if (perige < 156.)
        {
            s4 = perige - 78.;
            if (perige <= 98.)
            {
                s4 = 20.;
                qoms24 = pow(((120. - s4) * SGP4_AE / SGP4_XKMPER), 4.);
                s4 = s4 / SGP4_XKMPER + SGP4_AE;
            }
        }
        pinvsq = 1. / (aodp * aodp * betao2 * betao2);
        tsi = 1. / (aodp - s4);
        eta = aodp * tle.e * tsi;
        etasq = eta * eta;
        eeta = tle.e * eta;
        psisq = fabs(1. - etasq);
        coef = qoms24 * pow(tsi, 4.);
        coef1 = coef / pow(psisq, 3.5);
        c2 = coef1 * xnodp * (aodp * (1. + 1.5 * etasq + eeta * (4. + etasq)) + .75 * SGP4_CK2 * tsi / psisq * x3thm1 * (8. + 3. * etasq * (8. + etasq)));
        c1 = tle.bstar * c2;
        sinio = sin(tle.i);
        g = -SGP4_XJ3 / SGP4_CK2 * pow(SGP4_AE, 3.);
        c3 = coef * tsi * g * xnodp * SGP4_AE * sinio / tle.e;
        x1mth2 = 1. - theta2;
        c4 = 2. * xnodp * coef1 * aodp * betao2 * (eta * (2. + .5 * etasq) + tle.e * (.5 + 2. * etasq) - 2. * SGP4_CK2 * tsi / (aodp * psisq) * (-3. * x3thm1 * (1. - 2. * eeta + etasq * (1.5 - .5 * eeta)) + .75 * x1mth2 * (2. * etasq - eeta * (1. + etasq)) * cos(2. * tle.ap)));
        c5 = 2. * coef1 * aodp * betao2 * (1. + 2.75 * (etasq + eeta) + eeta * etasq);
        theta4 = theta2 * theta2;
        temp1 = 3. * SGP4_CK2 * pinvsq * xnodp;
        temp2 = temp1 * SGP4_CK2 * pinvsq;
        temp3 = 1.25 * SGP4_CK4 * pinvsq * pinvsq * xnodp;
        xmdot = xnodp + .5 * temp1 * betao * x3thm1 + .0625 * temp2 * betao * (13. - 78. * theta2 + 137. * theta4);
        x1m5th = 1. - 5. * theta2;
        omgdot = -.5 * temp1 * x1m5th + .0625 * temp2 * (7. - 114. * theta2 + 395. * theta4) + temp3 * (3. - 36. * theta2 + 49. * theta4);
        xhdot1 = -temp1 * cosio;
        xnodot = xhdot1 + (.5 * temp2 * (4. - 19. * theta2) + 2. * temp3 * (3. - 7. * theta2)) * cosio;
        omgcof = tle.bstar * c3 * cos(tle.ap);
        xmcof = -SGP4_TOTHRD * coef * tle.bstar * SGP4_AE / eeta;
        xnodcf = 3.5 * betao2 * xhdot1 * c1;
        t2cof = 1.5 * c1;
        xlcof = .125 * g * sinio * (3. + 5. * cosio) / (1. + cosio);
        aycof = .25 * g * sinio;
        delmo = pow((1. + eta * cos(tle.ma)), 3.);
        sinmo = sin(tle.ma);
        x7thm1 = 7. * theta2 - 1.;
        if (isimp != 1)
        {
            c1sq = c1 * c1;
            d2 = 4. * aodp * tsi * c1sq;
            temp = d2 * tsi * c1 / 3.;
            d3 = (17. * aodp + s4) * temp;
            d4 = .5 * temp * aodp * tsi * (221. * aodp + 31. * s4) * c1;
            t3cof = d2 + 2. * c1sq;
            t4cof = .25 * (3. * d3 + c1 * (12. * d2 + 10. * c1sq));
            t5cof = .2 * (3. * d4 + 12. * c1 * d3 + 6. * d2 * d2 + 15. * c1sq * (2. * d2 + c1sq));
        }
        lsnumber = tle.snumber;
        lutc = tle.utc;
    }

    // UPDATE FOR SECULAR GRAVITY AND ATMOSPHERIC DRAG
    tsince = (utc - tle.utc) * SGP4_XMNPDA;
    xmdf = tle.ma + xmdot * tsince;
    omgadf = tle.ap + omgdot * tsince;
    xnoddf = tle.raan + xnodot * tsince;
    alpha = omgadf;
    xmp = xmdf;
    tsq = tsince * tsince;
    xnode = xnoddf + xnodcf * tsq;
    tempa = 1. - c1 * tsince;
    tempe = tle.bstar * c4 * tsince;
    templ = t2cof * tsq;
    if (isimp != 1)
    {
        delomg = omgcof * tsince;
        delm = xmcof * (pow((1. + eta * cos(xmdf)), 3.) - delmo);
        temp = delomg + delm;
        xmp = xmdf + temp;
        alpha = omgadf - temp;
        tcube = tsq * tsince;
        tfour = tsince * tcube;
        tempa = tempa - d2 * tsq - d3 * tcube - d4 * tfour;
        tempe = tempe + tle.bstar * c5 * (sin(xmp) - sinmo);
        templ = templ + t3cof * tcube + tfour * (t4cof + tsince * t5cof);
    }
    a = aodp * tempa * tempa;
    e = tle.e - tempe;
    xl = xmp + alpha + xnode + xnodp * templ;
    beta = sqrt(1. - e * e);
    xn = SGP4_XKE / pow(a, 1.5);
    // LONG PERIOD PERIODICS
    axn = e * cos(alpha);
    temp = 1. / (a * beta * beta);
    xll = temp * xlcof * axn;
    aynl = temp * aycof;
    xlt = xl + xll;
    ayn = e * sin(alpha) + aynl;
    // SOLVE KEplERS EQUATION;
    capu = ranrm(xlt - xnode);
    temp2 = capu;
    for (i = 1; i <= 10; i++)
    {
        sinepw = sin(temp2);
        cosepw = cos(temp2);
        temp3 = axn * sinepw;
        temp4 = ayn * cosepw;
        temp5 = axn * cosepw;
        temp6 = ayn * sinepw;
        epw = (capu - temp4 + temp3 - temp2) / (1. - temp5 - temp6) + temp2;
        if (fabs(epw - temp2) <= SGP4_E6A)
            break;
        temp2 = epw;
    }
    // SHORT PERIOD PRELIMINARY QUANTITIES;
    ecose = temp5 + temp6;
    esine = temp3 - temp4;
    elsq = axn * axn + ayn * ayn;
    temp = 1. - elsq;
    pl = a * temp;
    r = a * (1. - ecose);
    temp1 = 1. / r;
    rdot = SGP4_XKE * sqrt(a) * esine * temp1;
    rfdot = SGP4_XKE * sqrt(pl) * temp1;
    temp2 = a * temp1;
    betal = sqrt(temp);
    temp3 = 1. / (1. + betal);
    cosu = temp2 * (cosepw - axn + ayn * esine * temp3);
    sinu = temp2 * (sinepw - ayn - axn * esine * temp3);
    u = actan(sinu, cosu);
    sin2u = 2. * sinu * cosu;
    cos2u = 2. * cosu * cosu - 1.;
    temp = 1. / pl;
    temp1 = SGP4_CK2 * temp;
    temp2 = temp1 * temp;
    // UPDATE FOR SHORT PERIODICS;
    rk = r * (1. - 1.5 * temp2 * betal * x3thm1) + .5 * temp1 * x1mth2 * cos2u;
    uk = u - .25 * temp2 * x7thm1 * sin2u;
    xnodek = xnode + 1.5 * temp2 * cosio * sin2u;
    xinck = tle.i + 1.5 * temp2 * cosio * sinio * cos2u;
    rdotk = rdot - xn * temp1 * x1mth2 * sin2u;
    rfdotk = rfdot + xn * temp1 * (x1mth2 * cos2u + 1.5 * x3thm1);
    // ORIENTATION VECTORS;
    sinuk = sin(uk);
    cosuk = cos(uk);
    sinik = sin(xinck);
    cosik = cos(xinck);
    sinnok = sin(xnodek);
    cosnok = cos(xnodek);
    xmx = -sinnok * cosik;
    xmy = cosnok * cosik;
    ux = xmx * sinuk + cosnok * cosuk;
    uy = xmy * sinuk + sinnok * cosuk;
    uz = sinik * sinuk;
    vx = xmx * cosuk - cosnok * sinuk;
    vy = xmy * cosuk - sinnok * sinuk;
    vz = sinik * cosuk;
    // POSITION AND VELOCITY in TEME
    pos_teme.s = pos_teme.v = pos_teme.a = pos_teme.j = rv_zero();

    pos_teme.s.col[0] = 1000. * SGP4_XKMPER * rk * ux;
    pos_teme.s.col[1] = 1000. * SGP4_XKMPER * rk * uy;
    pos_teme.s.col[2] = 1000. * SGP4_XKMPER * rk * uz;
    pos_teme.v.col[0] = 1000. * SGP4_XKMPER * (rdotk * ux + rfdotk * vx) / 60.;
    pos_teme.v.col[1] = 1000. * SGP4_XKMPER * (rdotk * uy + rfdotk * vy) / 60.;
    pos_teme.v.col[2] = 1000. * SGP4_XKMPER * (rdotk * uz + rfdotk * vz) / 60.;

    return 0;
}

double atan3(double sa, double cb)
{
    // sa = sine of angle, cb = cos of angle
    double y = 0.;
    double epsilon = 0.0000000001;
    if (fabs(sa) < epsilon)
    {
        y = cb >= 0 ? DPI : 0.;
        //        y = (1 - sign(cb)) * DPI2;
    }
    else
    {
        //        c = (2 - sign(sa)) * DPI2;
        if (fabs(cb) < epsilon)
        {
            y = cb;
        }
        else
        {
            y = (sa <= 0 ? D3PI2 : DPI2) + (sa < 0 ? -1. : 1.) * (cb < 0 ? -1. : 1.) * (fabs(atan(sa / cb)) - DPI2);
        }
    }
    return y;
}

int32_t eci2tle(cartpos eci, tlestruc &tle)
{
    // ICRF to Mean of Data (undo Precession)
    //            rmatrix bm;
    //            gcrf2j2000(&bm);
    //            eci.s = rv_mmult(bm,eci.s);
    //            eci.v = rv_mmult(bm,eci.v);

    //            rmatrix pm;
    //            j20002mean(utc,&pm);
    //            eci.s = rv_mmult(pm,eci.s);
    //            eci.v = rv_mmult(pm,eci.v);

    //            // Mean of Date to True of Date (undo Nutation)
    //            rmatrix nm;
    //            mean2true(utc,&nm);
    //            eci.s = rv_mmult(nm,eci.s);
    //            eci.v = rv_mmult(nm,eci.v);

    //            // True of Date to Uniform of Date (undo Equation of Equinoxes)
    //            rmatrix sm;
    //            true2teme(utc, &sm);
    //            eci.s = rv_mmult(sm,eci.s);
    //            eci.v = rv_mmult(sm,eci.v);

    //            function [e, mm, ma, i, ap, raan] = eci2tle(reci, veci)
    //            % convert osculating position and velocity vectors
    //            % to components of two line element set (TLE)
    //            % input
    //            %  reci = eci position vector (kilometers)
    //            %  veci = eci velocity vector (kiometers/second)
    //            % output
    //            %  e     = orbital eccentricity (non-dimensional)
    //            %  mm    = mean motion (orbits per day)
    //            %  ma    = mean anomaly (radians)
    //            %  i  = orbital inclination (radians)
    //            %  ap = argument of perigee (radians)
    //            %  raan = right ascension of ascending node (radians)
    //            % reference: Scott Campbell's Satellite Orbit Determination
    //            %            web site www.satelliteorbitdetermination.com
    //            % Orbital Mechanics with MATLAB
    //            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    tle.utc = eci.utc;
    double pi2 = D2PI;
    double xke = 0.0743669161331734132;
    double xj3 = -2.53881e-6;
    double req = 6378135.;
    double ck2 = 5.413079e-4;
    double a3ovk2 = -xj3 / ck2;
    //            % convert position vector to earth radii
    //            % and velocity vector to earth radii per minute
    //            for i = 1:1:3
    Vector rr2 = Vector(eci.s) / req;
    Vector vv2 = 60.0 * Vector(eci.v) / req;
    Vector vk = vv2 / xke;
    //            for (uint16_t i=0; i<3; ++i)
    //            {
    //                rr2[i] = reci[i] / req;
    //                vv2[i] = 60.0 * veci[i] / req;
    //                vk[i] = vv2[i] / xke;
    //            }

    //            end
    Vector hv = rr2.cross(vk);
    double pl = hv.dot(hv);
    Vector vz = Vector(0., 0., 1.);
    Vector vxh = vz.cross(hv);
    if (vxh[0] == 0.0 && vxh[1] == 0.0)
    {
        vxh[0] = 1.0;
    }

    //            end
    Vector vxhhat = vxh / vxh.norm();
    double rk = rr2.norm();
    double rdotv = rr2.dot(vv2);
    double rdotk = rdotv / rk;
    double hmag = hv.norm();
    double rfdotk = hmag * xke / rk;
    double vwrk = rr2.dot(vxhhat);
    double temp = vwrk / rk;
    double uk = acos(temp);
    if (rr2[1] < 0.0)
    {
        uk = pi2 - uk;
    }
    //            end
    vz = vk.cross(hv);
    Vector vy = -rr2 / rk;
    //            for i = 1:1:3
    //            for (uint16_t i=0; i<3; ++i)
    //            {
    //                vy[i] = -1.0 * rr2[i] / rk;
    //            }

    //            end
    //            for i = 1:1:3
    //            for (uint16_t i=0; i<3; ++i)
    //            {
    //                vec[i] = vz[i] + vy[i];
    //            }
    Vector vec = vz + vy;

    //            end
    double ek = vec.norm();
    if (ek >= 1.0)
    {
        return -1;
    }

    //            end
    double xnodek = atan3(vxhhat[1], vxhhat[0]);
    temp = sqrt(hv[0] * hv[0] + hv[1] * hv[1]);
    double xinck = atan2(temp, hv[2]);
    vwrk = vec.dot(vxhhat);
    temp = vwrk / ek;
    double wk = acos(temp);
    if (vec[2] < 0.0)
    {
        wk = fmod(pi2 - wk, pi2);
    }
    double aodp = pl / (1.0 - ek * ek);
    double xn = xke * pow(aodp, (-1.5));
    //            % in the first loop the osculating elements rk, uk, xnodek, xinck, rdotk,
    //            % and rfdotk are used as anchors to find the corresponding final sgp4
    //            % mean elements r, u, raan, i, rdot, and rfdot. several other final
    //            % mean values based on these are also found: betal, cosio, sinio, theta2,
    //            % cos2u, sin2u, x3thm1, x7thm1, x1mth2. in addition, the osculating values
    //            % initially held by aodp, pl, and xn are replaced by intermediate
    //            % (not osculating and not mean) values used by sgp4. the loop converges
    //            % on the value of pl in about four iterations.
    //            % seed value for first loop
    tle.i = xinck;
    double u = uk;
    double r;
    double rdot;
    double sinio;
    double cosio;
    double x3thm1;
    for (uint16_t i = 0; i < 20; ++i)
    {
        double a2 = pl;
        double betal = sqrt(pl / aodp);
        double temp1 = ck2 / pl;
        double temp2 = temp1 / pl;
        cosio = cos(tle.i);
        sinio = sin(tle.i);
        double sin2u = sin(2.0 * u);
        double cos2u = cos(2.0 * u);
        double theta2 = cosio * cosio;
        x3thm1 = 3.0 * theta2 - 1.0;
        double x1mth2 = 1.0 - theta2;
        double x7thm1 = 7.0 * theta2 - 1.0;
        r = (rk - 0.5 * temp1 * x1mth2 * cos2u) / (1.0 - 1.5 * temp2 * betal * x3thm1);
        u = uk + 0.25 * temp2 * x7thm1 * sin2u;
        tle.raan = xnodek - 1.5 * temp2 * cosio * sin2u;
        tle.i = xinck - 1.5 * temp2 * cosio * sinio * cos2u;
        rdot = rdotk + xn * temp1 * x1mth2 * sin2u;
        double rfdot = rfdotk - xn * temp1 * (x1mth2 * cos2u + 1.5 * x3thm1);
        temp = r * rfdot / xke;
        pl = temp * temp;
        temp = 2.0 / r - (rdot * rdot + rfdot * rfdot) / (xke * xke);
        aodp = 1.0 / temp;
        xn = xke * pow(aodp, (-1.5));
        if (fabs(a2 - pl) < 1.0e-17)
        {
            break;
        }
    }
    //            % the next values are calculated from constants and a combination of mean
    //            % and intermediate quantities from the first loop. these values all remain
    //            % fixed and are used in the second loop.
    //            % preliminary values for the second loop
    double ecose = 1.0 - r / aodp;
    double esine = r * rdot / (xke * sqrt(aodp));
    double elsq = 1.0 - pl / aodp;
    double xlcof = 0.125 * a3ovk2 * sinio * (3.0 + 5.0 * cosio) / (1.0 + cosio);
    double aycof = 0.25 * a3ovk2 * sinio;
    double temp1 = esine / (1.0 + sqrt(1.0 - elsq));
    double cosu = cos(u);
    double sinu = sin(u);
    //            % the second loop normally converges in about six iterations to the final
    //            % mean value for the eccentricity, e. the mean perigee, ap, is also
    //            % determined. cosepw and sinepw are found to twelve decimal places and
    //            % are used to calculate an intermediate value for the eccentric anomaly,
    //            % temp2. temp2 is then used in kepler's equation to find an intermediate
    //            % value for the true longitude, capu.
    //            % seed values for loop
    tle.e = sqrt(elsq);
    tle.ap = wk;
    double axn = tle.e * cos(tle.ap);
    double sinepw;
    double cosepw;
    double beta;
    for (uint16_t i = 0; i < 20; ++i)
    {
        double a2 = tle.e;
        beta = 1.0 - tle.e * tle.e;
        temp = 1.0 / (aodp * beta);
        double aynl = temp * aycof;
        double ayn = tle.e * sin(tle.ap) + aynl;
        cosepw = r * cosu / aodp + axn - ayn * temp1;
        sinepw = r * sinu / aodp + ayn + axn * temp1;
        axn = cosepw * ecose + sinepw * esine;
        ayn = sinepw * ecose - cosepw * esine;
        tle.ap = fmod(atan2(ayn - aynl, axn), pi2);

        if (tle.e > 0.5)
        {
            tle.e = 0.9 * tle.e + 0.1 * (axn / cos(tle.ap));
        }
        else
        {
            tle.e = axn / cos(tle.ap);
        }

        if (tle.e > 0.999)
        {
            tle.e = 0.999;
        }

        if (fabs(a2 - tle.e) < 1.0e-17)
        {
            break;
        }
    }
    double temp2 = atan2(sinepw, cosepw);
    double capu = temp2 - esine;
    double xll = temp * xlcof * axn;
    //            % xll adjusts the intermediate true longitude
    //                    % capu, to the mean true longitude, xl
    double xl = capu - xll;
    tle.ma = fmod(xl - tle.ap, pi2);
    //            % the third loop usually converges after three iterations to the
    //                    % mean semi-major axis, a1, which is then used to find the mean motion, mm
    double a0 = aodp;
    double a1 = a0;
    double beta2 = sqrt(beta);
    temp = 1.5 * ck2 * x3thm1 / (beta * beta2);
    for (uint16_t i = 0; i < 20; ++i)
    {
        double a2 = a1;
        double d0 = temp / (a0 * a0);
        a0 = aodp * (1.0 - d0);
        double d1 = temp / (a1 * a1);
        a1 = a0 / (1.0 - d1 / 3.0 - d1 * d1 - 134.0 * d1 * d1 * d1 / 81.0);
        if (fabs(a2 - a1) < 1.0e-16)
        {
            break;
        }
    }
    tle.mm = xke * pow(a1, -1.5) / 60.;

    return 0;
}

//! TLE from ECI
/*! Convert an ECI state vector into an SGP4 TLE
         * \param utc UTC time of ECI State Vector and TLE
         * \param eci State Vector to convert, stored as ::Cosmos::Convert::cartpos
         * \param tle Two Line Element, stored as ::Cosmos::Convert::tlestruc
         */
int32_t eci2tle2(cartpos eci, tlestruc &tle)
{
    // ICRF to Mean of Data (undo Precession)
    rmatrix bm;
    gcrf2j2000(&bm);
    eci.s = rv_mmult(bm,eci.s);
    eci.v = rv_mmult(bm,eci.v);

    rmatrix pm;
    j20002mean(eci.utc,&pm);
    eci.s = rv_mmult(pm,eci.s);
    eci.v = rv_mmult(pm,eci.v);

    // Mean of Date to True of Date (undo Nutation)
    rmatrix nm;
    mean2true(eci.utc,&nm);
    eci.s = rv_mmult(nm,eci.s);
    eci.v = rv_mmult(nm,eci.v);

    // True of Date to Uniform of Date (undo Equation of Equinoxes)
    rmatrix sm;
    true2teme(eci.utc, &sm);
    eci.s = rv_mmult(sm,eci.s);
    eci.v = rv_mmult(sm,eci.v);

    // Convert to Keplerian Elements
    kepstruc kep;
    eci2kep(eci, kep);

    // Store in relevant parts of TLE
    tle.bstar = .0001;
    tle.orbit = 0;
    tle.ap = kep.ap;
    tle.e = kep.e;
    tle.i = kep.i;
    tle.ma = kep.ma;
    tle.mm = kep.mm;
    tle.dmm = kep.dmm * D2PI / (86400. * 86400.);
    tle.raan = kep.raan;
    tle.utc = eci.utc;
//    eci2tle(eci, tle);
//    tle.bstar = .0001;
    cartpos ecinew;
//    tle2eci(eci.utc, tle, ecinew);
    tlestruc tlenew;
    tlenew = tle;
    double errornew;
    double error;

    double scale = .01;
//    tle2eci(eci.utc, tle, ecinew);
//    error = length_rv(ecinew.s-eci.s);
    bool improved = false;
    size_t count = 0;
    do
    {
        tle2eci(eci.utc, tlenew, ecinew);
        error = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
        improved = false;

        tlenew.bstar = (1.+scale) * tle.bstar;
        tle2eci(eci.utc, tlenew, ecinew);
        errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
        if (error - errornew > scale)
        {
            improved = true;
            error = errornew;
            tlenew.bstar = (1.-scale) * tle.bstar;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                error = errornew;
                tle.bstar = tlenew.bstar;
            }
            else
            {
                tlenew.bstar = (1.+scale) * tle.bstar;
                tle.bstar = tlenew.bstar;
            }
        }
        else
        {
            tlenew.bstar = (1.-scale) * tle.bstar;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                improved = true;
                error = errornew;
                tle.bstar = tlenew.bstar;
            }
            else
            {
                tlenew.bstar = tle.bstar;
            }
        }

        tlenew.ap = (1.+scale) * tle.ap;
        tle2eci(eci.utc, tlenew, ecinew);
        errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
        if (error - errornew > scale)
        {
            improved = true;
            error = errornew;
            tlenew.ap = (1.-scale) * tle.ap;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                error = errornew;
                tle.ap = tlenew.ap;
            }
            else
            {
                tlenew.ap = (1.+scale) * tle.ap;
                tle.ap = tlenew.ap;
            }
        }
        else
        {
            tlenew.ap = (1.-scale) * tle.ap;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                improved = true;
                error = errornew;
                tle.ap = tlenew.ap;
            }
            else
            {
                tlenew.ap = tle.ap;
            }
        }

       tlenew.e = (1.+scale) * tle.e;
        tle2eci(eci.utc, tlenew, ecinew);
        errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
        if (error - errornew > scale)
        {
            improved = true;
            error = errornew;
            tlenew.e = (1.-scale) * tle.e;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                error = errornew;
                tle.e = tlenew.e;
            }
            else
            {
                tlenew.e = (1.+scale) * tle.e;
                tle.e = tlenew.e;
            }
        }
        else
        {
            tlenew.e = (1.-scale) * tle.e;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                improved = true;
                error = errornew;
                tle.e = tlenew.e;
            }
            else
            {
                tlenew.e = tle.e;
            }
        }

        tlenew.i = (1.+scale) * tle.i;
        tle2eci(eci.utc, tlenew, ecinew);
        errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
        if (error - errornew > scale)
        {
            improved = true;
            error = errornew;
            tlenew.i = (1.-scale) * tle.i;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                error = errornew;
                tle.i = tlenew.i;
            }
            else
            {
                tlenew.i = (1.+scale) * tle.i;
                tle.i = tlenew.i;
            }
        }
        else
        {
            tlenew.i = (1.-scale) * tle.i;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                improved = true;
                error = errornew;
                tle.i = tlenew.i;
            }
            else
            {
                tlenew.i = tle.i;
            }
        }

        tlenew.mm = (1.+scale) * tle.mm;
        tle2eci(eci.utc, tlenew, ecinew);
        errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
        if (error - errornew > scale)
        {
            improved = true;
            error = errornew;
            tlenew.mm = (1.-scale) * tle.mm;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                error = errornew;
                tle.mm = tlenew.mm;
            }
            else
            {
                tlenew.mm = (1.+scale) * tle.mm;
                tle.mm = tlenew.mm;
            }
        }
        else
        {
            tlenew.mm = (1.-scale) * tle.mm;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                improved = true;
                error = errornew;
                tle.mm = tlenew.mm;
            }
            else
            {
                tlenew.mm = tle.mm;
            }
        }

        tlenew.ma = (1.+scale) * tle.ma;
        tle2eci(eci.utc, tlenew, ecinew);
        errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
        if (error - errornew > scale)
        {
            improved = true;
            error = errornew;
            tlenew.ma = (1.-scale) * tle.ma;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                error = errornew;
                tle.ma = tlenew.ma;
            }
            else
            {
                tlenew.ma = (1.+scale) * tle.ma;
                tle.ma = tlenew.ma;
            }
        }
        else
        {
            tlenew.ma = (1.-scale) * tle.ma;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                improved = true;
                error = errornew;
                tle.ma = tlenew.ma;
            }
            else
            {
                tlenew.ma = tle.ma;
            }
        }

        tlenew.raan = (1.+scale) * tle.raan;
        tle2eci(eci.utc, tlenew, ecinew);
        errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
        if (error - errornew > scale)
        {
            improved = true;
            error = errornew;
            tlenew.raan = (1.-scale) * tle.raan;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                error = errornew;
                tle.raan = tlenew.raan;
            }
            else
            {
                tlenew.raan = (1.+scale) * tle.raan;
                tle.raan = tlenew.raan;
            }
        }
        else
        {
            tlenew.raan = (1.-scale) * tle.raan;
            tle2eci(eci.utc, tlenew, ecinew);
            errornew = length_rv(ecinew.s-eci.s) / length_rv(eci.s) + length_rv(ecinew.v-eci.v) / length_rv(eci.v);
            if (error - errornew > scale)
            {
                improved = true;
                error = errornew;
                tle.raan = tlenew.raan;
            }
            else
            {
                tlenew.raan = tle.raan;
            }
        }
        ++count;

        if (!improved)
        {
            scale /= 10.;
            count = 0;
        }
    } while (scale > .000001);

    return 0;
}

/**
         * Convert a Two Line Element into a location at the specified time.
         * @param utc Specified time as Modified Julian Date
         * @param tle Two Line Element, given as pointer to a ::Cosmos::Convert::tlestruc
         * @param eci Converted location, given as pointer to a ::Cosmos::Convert::cartpos
         */
int tle2eci(double utc, tlestruc tle, cartpos &eci)
{

    // call sgp4, eci is passed by reference
    sgp4(utc, tle, eci);

    // eci = *teme;

    // Uniform of Date to True of Date (Equation of Equinoxes)
    rmatrix sm;
    teme2true(utc, &sm);
    eci.s = rv_mmult(sm, eci.s);
    eci.v = rv_mmult(sm, eci.v);

    // True of Date to Mean of Date (Nutation)
    rmatrix nm;
    true2mean(utc, &nm);
    eci.s = rv_mmult(nm, eci.s);
    eci.v = rv_mmult(nm, eci.v);

    // Mean of Date to ICRF (precession)
    rmatrix pm;
    mean2j2000(utc, &pm);
    eci.s = rv_mmult(pm, eci.s);
    eci.v = rv_mmult(pm, eci.v);

    rmatrix bm;
    j20002gcrf(&bm);
    eci.s = rv_mmult(bm, eci.s);
    eci.v = rv_mmult(bm, eci.v);

    eci.utc = utc;

    return 0;
}

//! Get TLE from array of TLE's
/*! Return the indexed entry from an array of ::Cosmos::Convert::tlestruc. If the
 * index is larger than the size of the array, an empty TLE with time
 * set to zero is returned.
    \param index Index into the array.
    \param lines Array of TLE's.
    \return Indexed TLE.
    */
tlestruc get_line(uint16_t index, vector<tlestruc> lines)
{
    tlestruc ttle;

    if (lines.size() <= 0 || index >= lines.size())
    {
        ttle.utc = 0.;
        return (ttle);
    }
    else
    {
        return (lines[index]);
    }
}

//! Load TLE from file. TODO!!! create new class for dealing with TLEs
/*!
         * Load Two Line Element file into TLE structure
         * \param fname Name of file containing elements
         * \param tle structure to contain TLE elements
         * \return 0 if parsing was sucessfull, otherwise a negative error.
         */
int32_t loadTLE(char *fname, tlestruc &tle)
{
    return load_tle(fname, tle);
}

int32_t load_tle(string fname, tlestruc &tle)
{
    return load_tle((char *)fname.c_str(), tle);
}

int32_t load_tle(char *fname, tlestruc &tle)
{
    FILE *fdes;
    uint16_t year;
    double jday;
    int32_t bdragm, bdrage, ecc;
    char ibuf[81], tlename[81];
    int i;

    if ((fdes = fopen(fname, "r")) == nullptr)
        return (-1);

    tlecount = 0;

    // Name Line
    char *ichar = fgets(tlename, 80, fdes);
    if (ichar == nullptr || feof(fdes))
        return (-1);

    for (i = strlen(tlename) - 1; i > 0; i--)
    {
        if (tlename[i] != ' ' && tlename[i] != '\r' && tlename[i] != '\n')
            break;
    }
    tlename[i + 1] = 0;

    while (!feof(fdes))
    {
        tle.name = tlename;

        // Line 1
        if (fgets(ibuf, 80, fdes) == nullptr)
            break;
        sscanf(&ibuf[2], "%5u", &tle.snumber);
        tle.id = string(ibuf).substr(9, 9);
        sscanf(&ibuf[18], "%2hu", &year);
        if (year < 57)
            year += 2000;
        else
            year += 1900;
        sscanf(&ibuf[20], "%12lf", &jday);
        tle.utc = cal2mjd((int)year, 1, 0.);
        tle.utc += jday;
        sscanf(&ibuf[33], "%lf", &tle.dmm);
        tle.dmm *= D2PI / (86400. * 86400.);
        tle.ddmm = 0.;
        if (strlen(ibuf) > 50)
        {
            sscanf(&ibuf[53], "%6d%2d", &bdragm, &bdrage);
            tle.bstar = pow(10., bdrage) * bdragm / 1.e5;
        }
        else
            tle.bstar = 0.;

        // Line 2
        char *ichar = fgets(ibuf, 80, fdes);
        if (ichar != NULL)
        {
            ibuf[68] = 0;
            sscanf(&ibuf[8], "%8lf %8lf %7d %8lf %8lf %11lf%5u", &tle.i, &tle.raan, &ecc, &tle.ap, &tle.ma, &tle.mm, &tle.orbit);
            tle.i = RADOF(tle.i);
            tle.raan = RADOF(tle.raan);
            tle.ap = RADOF(tle.ap);
            tle.ma = RADOF(tle.ma);
            tle.mm *= D2PI / 86400.;
            tle.e = ecc / 1.e7;
        }
    }
    fclose(fdes);
    return 0;
}

//! Load TLE from file. TODO!!! Rename Function to loadTle and create new class for dealing with TLEs
/*!
         * Load Two Line Element file into array of TLE's
         * \param fname Name of file containing elements
         * \param lines Array of ::Cosmos::Convert::tlestruc structures to contain elements
         * \return A 32 bit signed integer indicating number of elements, otherwise a negative error.
         */
int32_t load_lines(string fname, vector<tlestruc> &lines)
{
    FILE *fdes;
    uint16_t year;
    double jday;
    int32_t bdragm, bdrage, ecc;
    char ibuf[81], tlename[81];
    int i;
    tlestruc tle;

    if ((fdes = fopen(fname.c_str(), "r")) == nullptr)
        return (-1);

    tlecount = 0;

    // Name Line
    char *ichar = fgets(tlename, 80, fdes);
    if (ichar == nullptr || feof(fdes))
        return (-1);

    for (i = strlen(tlename) - 1; i > 0; i--)
    {
        if (tlename[i] != ' ' && tlename[i] != '\r' && tlename[i] != '\n')
            break;
    }
    tlename[i + 1] = 0;

    while (!feof(fdes))
    {
        tle.name = tlename;

        // Line 1
        if (fgets(ibuf, 80, fdes) == nullptr)
            break;
        if (strlen(ibuf) < 68)
        {
            break;
        }
        uint16_t cs = 0;
        for (uint16_t i = 0; i < 68; ++i)
        {
            if (ibuf[i] >= '0' && ibuf[i] <= '9')
            {
                cs = (cs + (ibuf[i] - '0'));
            }
            else if (ibuf[i] == '-')
            {
                cs = (cs + 1);
            }
        }
        cs = cs % 10;
        sscanf(&ibuf[2], "%5u", &tle.snumber);
        tle.id = string(ibuf).substr(9, 9);
        sscanf(&ibuf[18], "%2hu", &year);
        if (year < 57)
            year += 2000;
        else
            year += 1900;
        sscanf(&ibuf[20], "%12lf", &jday);
        tle.utc = cal2mjd((int)year, 1, 0.);
        tle.utc += jday;
        sscanf(&ibuf[33], "%lf", &tle.dmm);
        tle.dmm *= D2PI / (86400. * 86400.);
        tle.ddmm = 0.;
        if (strlen(ibuf) > 50)
        {
            sscanf(&ibuf[53], "%6d%2d", &bdragm, &bdrage);
            tle.bstar = pow(10., bdrage) * bdragm / 1.e5;
        }
        else
            tle.bstar = 0.;

        // Line 2
        char *ichar = fgets(ibuf, 80, fdes);
        if (ichar != NULL)
        {
            cs = 0;
            for (uint16_t i = 0; i < 68; ++i)
            {
                if (ichar[i] >= '0' && ichar[i] <= '9')
                {
                    cs = (cs + (ichar[i] - '0'));
                }
                else if (ichar[i] == '-')
                {
                    cs = (cs + 1);
                }
            }
            cs = cs % 10;
            ibuf[68] = 0;
            sscanf(&ibuf[8], "%8lf %8lf %7d %8lf %8lf %11lf%5u", &tle.i, &tle.raan, &ecc, &tle.ap, &tle.ma, &tle.mm, &tle.orbit);
            tle.i = RADOF(tle.i);
            tle.raan = RADOF(tle.raan);
            tle.ap = RADOF(tle.ap);
            tle.ma = RADOF(tle.ma);
            tle.mm *= D2PI / 86400.;
            tle.e = ecc / 1.e7;
            lines.push_back(tle);
        }
    }
    fclose(fdes);
    return (lines.size());
}

/*! Load Two Line Element file for multiple satellites into array of TLE's
         * \param fname Name of file containing elements
         * \param lines Array of ::Cosmos::Convert::tlestruc structures to contain elements
         * \return A 32 bit signed integer indicating number of elements, otherwise a negative error.
         */
int32_t load_lines_multi(string fname, vector<tlestruc> &lines)
{
    FILE *fdes;
    uint16_t year;
    double jday;
    int32_t bdragm, bdrage, ecc;
    char ibuf[81], tlename[81];
    int i;
    tlestruc tle;

    if ((fdes = fopen(fname.c_str(), "r")) == nullptr)
        return (-1);

    tlecount = 0;

    while (!feof(fdes))
    {
        // Name Line
        char *ichar = fgets(tlename, 80, fdes);
        if (ichar == nullptr || feof(fdes))
            break;

        for (i = strlen(tlename) - 1; i > 0; i--)
        {
            if (tlename[i] != ' ' && tlename[i] != '\r' && tlename[i] != '\n')
                break;
        }
        tlename[i + 1] = 0;

        tle.name = tlename;

        // Line 1
        if (fgets(ibuf, 80, fdes) == nullptr)
            break;
        sscanf(&ibuf[2], "%5u", &tle.snumber);
        tle.id = string(ibuf).substr(9, 9);
        sscanf(&ibuf[18], "%2hu", &year);
        if (year < 57)
            year += 2000;
        else
            year += 1900;
        sscanf(&ibuf[20], "%12lf", &jday);
        tle.utc = cal2mjd((int)year, 1, 0.);
        tle.utc += jday;
        sscanf(&ibuf[33], "%lf", &tle.dmm);
        tle.dmm *= D2PI / (86400. * 86400.);
        tle.ddmm = 0.;
        if (strlen(ibuf) > 50)
        {
            sscanf(&ibuf[53], "%6d%2d", &bdragm, &bdrage);
            tle.bstar = pow(10., bdrage) * bdragm / 1.e5;
        }
        else
            tle.bstar = 0.;

        // Line 2
        ichar = fgets(ibuf, 80, fdes);
        if (ichar != NULL)
        {
            ibuf[68] = 0;
            sscanf(&ibuf[8], "%8lf %8lf %7d %8lf %8lf %11lf%5u", &tle.i, &tle.raan, &ecc, &tle.ap, &tle.ma, &tle.mm, &tle.orbit);
            tle.i = RADOF(tle.i);
            tle.raan = RADOF(tle.raan);
            tle.ap = RADOF(tle.ap);
            tle.ma = RADOF(tle.ma);
            tle.mm *= D2PI / 86400.;
            tle.e = ecc / 1.e7;
            lines.push_back(tle);
        }
    }
    fclose(fdes);
    return (lines.size());
}

//! Load STK elements
/*! Load a table of locations calculated in STK. Format is expected to be J2000; position,
 * velocity and acceleration; in X, Y, and Z; all in meters.
    \param filename Name of file containing positions.
    \param stkdata ::stkstruc holding satellite position.
    \return The number of entries in the table, otherwise a negative error.
*/
int32_t load_stk(string filename, stkstruc &stkdata)
{
    FILE *fdes;
    int32_t maxcount;
    int32_t iretn = 0;
    cposstruc *tpos;
    char ibuf[250];

    if ((fdes = fopen(filename.c_str(), "r")) == nullptr)
        return (STK_ERROR_NOTFOUND);

    maxcount = 1000;
    stkdata.pos = (cposstruc *)calloc(maxcount, sizeof(cposstruc));
    stkdata.count = 0;
    while (!feof(fdes))
    {
        char *ichar = fgets(ibuf, 250, fdes);
        if (ichar == nullptr || feof(fdes))
            break;
        if ((iretn = sscanf(ibuf, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &stkdata.pos[stkdata.count].pos.utc, &stkdata.pos[stkdata.count].pos.s.col[0], &stkdata.pos[stkdata.count].pos.s.col[1], &stkdata.pos[stkdata.count].pos.s.col[2], &stkdata.pos[stkdata.count].pos.v.col[0], &stkdata.pos[stkdata.count].pos.v.col[1], &stkdata.pos[stkdata.count].pos.v.col[2], &stkdata.pos[stkdata.count].pos.a.col[0], &stkdata.pos[stkdata.count].pos.a.col[1], &stkdata.pos[stkdata.count].pos.a.col[2])) == 10)
        {
            stkdata.pos[stkdata.count].utc = stkdata.pos[stkdata.count].pos.utc;
            stkdata.count++;
            if (!(stkdata.count % 1000))
            {
                maxcount += 1000;
                tpos = stkdata.pos;
                stkdata.pos = (cposstruc *)calloc(maxcount, sizeof(cposstruc));
                memcpy(stkdata.pos, tpos, (maxcount - 1000) * sizeof(cposstruc));
                free(tpos);
            }
        }
        else
            iretn = 0;
    }
    fclose(fdes);
    if (stkdata.count)
    {
        stkdata.dt = ((stkdata.pos[9].utc - stkdata.pos[0].utc)) / 9.;
    }
    return (stkdata.count);
}

//! ECI from STK data
/*! Return an interpolated cartpos from time and STK data
    \param utc UTC in Modified Julian Days
    \param stk Structure containing array of STK positions
    \param eci Structure to return position in
    \return 0 if successful, otherwise negative error
*/
int stk2eci(double utc, stkstruc &stk, cartpos &eci)
{
    int32_t index, i, j;
    double findex;
    uvector t{}, p{}, su{}, vu{}, au{};
    rmatrix s, v, a;

    findex = ((utc - stk.pos[0].utc) / stk.dt) + .5;
    if (findex < 1)
    {
        findex = 1.;
    }
    if (findex >= stk.count)
    {
        findex = stk.count - 1;
    }

    findex = (int)findex + ((utc - stk.pos[(int)findex].utc) / stk.dt) + .5;

    index = (int)findex - 1;
    if (index < 0)
        return (STK_ERROR_LOWINDEX);

    if (index > (int32_t)stk.count - 3)
        return (STK_ERROR_HIGHINDEX);

    for (i = 0; i < 3; i++)
    {
        t.a4[i] = utc - stk.pos[index + i].utc;
        for (j = 0; j < 3; j++)
        {
            s.row[j].col[i] = stk.pos[index + i].pos.s.col[j];
            v.row[j].col[i] = stk.pos[index + i].pos.v.col[j];
            a.row[j].col[i] = stk.pos[index + i].pos.a.col[j];
        }
    }

    eci.utc = utc;
    eci.s = eci.v = eci.a = eci.j = rv_zero();

    for (j = 0; j < 3; j++)
    {
        su.r = s.row[j];
        p = rv_fitpoly(t, su, 2);
        //	eci.s.col[j] = p.a4[0] + utc * (p.a4[1] + utc * p.a4[2]);
        eci.s.col[j] = p.a4[0];
        vu.r = v.row[j];
        p = rv_fitpoly(t, vu, 2);
        //	eci.v.col[j] = p.a4[0] + utc * (p.a4[1] + utc * p.a4[2]);
        eci.v.col[j] = p.a4[0];
        au.r = a.row[j];
        p = rv_fitpoly(t, au, 2);
        //	eci.a.col[j] = p.a4[0] + utc * (p.a4[1] + utc * p.a4[2]);
        eci.a.col[j] = p.a4[0];
    }

    return 0;
}

::std::ostream &operator<<(::std::ostream &out, const cartpos &a)
{
    out << a.utc << "\t" << a.s << "\t" << a.v << "\t" << a.a << "\t" << a.pass;
    return out;
}

::std::istream &operator>>(::std::istream &in, cartpos &a)
{
    in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const cposstruc &a)
{
    out << a.utc << "\t" << a.pos;
    return out;
}

::std::istream &operator>>(::std::istream &in, cposstruc &a)
{
    in >> a.utc >> a.pos;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const geoidpos &a)
{
    out << a.utc << "\t" << a.s << "\t" << a.v << "\t" << a.a << "\t" << a.pass;
    return out;
}

::std::istream &operator>>(::std::istream &in, geoidpos &a)
{
    in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const spherpos &a)
{
    out << a.utc << "\t" << a.s << "\t" << a.v << "\t" << a.a << "\t" << a.pass;
    return out;
}

::std::istream &operator>>(::std::istream &in, spherpos &a)
{
    in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const aattstruc &a)
{
    out << a.utc << "\t" << a.s << "\t" << a.v << "\t" << a.a;
    return out;
}

::std::istream &operator>>(::std::istream &in, aattstruc &a)
{
    in >> a.utc >> a.s >> a.v >> a.a;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const quatatt &a)
{
    out << a.utc << "\t" << a.s << "\t" << a.v << "\t" << a.a;
    return out;
}

::std::istream &operator>>(::std::istream &in, quatatt &a)
{
    in >> a.utc >> a.s >> a.v >> a.a;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const dcmatt &a)
{
    out << a.utc << "\t" << a.s << "\t" << a.v << "\t" << a.a;
    return out;
}

::std::istream &operator>>(::std::istream &in, dcmatt &a)
{
    in >> a.utc >> a.s >> a.v >> a.a;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const qatt &a)
{
    out << a.utc << "\t" << a.s << "\t" << a.v << "\t" << a.a << "\t" << a.pass;
    return out;
}

::std::istream &operator>>(::std::istream &in, qatt &a)
{
    in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const kepstruc &a)
{
    out << a.utc << "\t"
        << a.orbit << "\t"
        << a.period << "\t"
        << a.a << "\t"
        << a.e << "\t"
        << a.h << "\t"
        << a.beta << "\t"
        << a.eta << "\t"
        << a.i << "\t"
        << a.raan << "\t"
        << a.ap << "\t"
        << a.alat << "\t"
        << a.ma << "\t"
        << a.ta << "\t"
        << a.ea << "\t"
        << a.mm << "\t"
        << a.dmm << "\t"
        << a.fa;
    return out;
}

::std::istream &operator>>(::std::istream &in, kepstruc &a)
{
    in >> a.utc >> a.orbit >> a.period >> a.a >> a.e >> a.h >> a.beta >> a.eta >> a.i >> a.raan >> a.ap >> a.alat >> a.ma >> a.ta >> a.ea >> a.mm >> a.dmm >> a.fa;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const bodypos &a)
{
    out << a.sepangle << "\t" << a.size << "\t" << a.radiance;
    return out;
}

::std::istream &operator<<(::std::istream &in, bodypos &a)
{
    in >> a.sepangle >> a.size >> a.radiance;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const extrapos &a)
{
    out << a.utc << "\t"
        << a.tt << "\t"
        << a.ut << "\t"
        << a.tdb << "\t"
        << a.j2e << "\t"
        << a.dj2e << "\t"
        << a.ddj2e << "\t"
        << a.e2j << "\t"
        << a.de2j << "\t"
        << a.dde2j << "\t"
        << a.j2t << "\t"
        << a.j2s << "\t"
        << a.t2j << "\t"
        << a.s2j << "\t"
        << a.s2t << "\t"
        << a.ds2t << "\t"
        << a.t2s << "\t"
        << a.dt2s << "\t"
        << a.l2e << "\t"
        << a.e2l << "\t"
        << a.sun2earth << "\t"
        << a.sun2moon << "\t"
        << a.closest;
    return out;
}

::std::istream &operator>>(::std::istream &in, extrapos &a)
{
    in >> a.utc >> a.tt >> a.ut >> a.tdb >> a.j2e >> a.dj2e >> a.ddj2e >> a.e2j >> a.de2j >> a.dde2j >> a.j2t >> a.j2s >> a.t2j >> a.s2j >> a.s2t >> a.ds2t >> a.t2s >> a.dt2s >> a.e2l >> a.l2e >> a.sun2earth >> a.sun2moon >> a.closest;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const extraatt &a)
{
    out << a.utc << "\t" << a.j2b << "\t" << a.b2j;
    return out;
}

::std::istream &operator>>(::std::istream &in, extraatt &a)
{
    in >> a.utc >> a.j2b >> a.b2j;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const posstruc &a)
{
    out << a.utc << "\t"
        << a.icrf << "\t"
        << a.eci << "\t"
        << a.sci << "\t"
        << a.geoc << "\t"
        << a.selc << "\t"
        << a.geod << "\t"
        << a.selg << "\t"
        << a.geos << "\t"
        << a.extra << "\t"
        << a.earthsep << "\t"
        << a.moonsep << "\t"
        << a.sunsize << "\t"
        << a.sunradiance;
    return out;
}

::std::istream &operator>>(::std::istream &in, posstruc &a)
{
    in >> a.utc >> a.icrf >> a.eci >> a.sci >> a.geoc >> a.selc >> a.geod >> a.selg >> a.geos >> a.extra >> a.earthsep >> a.moonsep >> a.sunsize >> a.sunradiance;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const attstruc &a)
{
    out << a.utc << "\t"
        << a.topo << "\t"
        << a.lvlh << "\t"
        << a.geoc << "\t"
        << a.selc << "\t"
        << a.icrf << "\t"
        << a.extra;
    return out;
}

::std::istream &operator>>(::std::istream &in, attstruc &a)
{
    in >> a.utc >> a.topo >> a.lvlh >> a.geoc >> a.selc >> a.icrf >> a.extra;
    return in;
}

::std::ostream &operator<<(::std::ostream &out, const locstruc &a)
{
    out << a.utc << "\t"
        << a.pos << "\t"
        << a.att;
    return out;
}

::std::istream &operator>>(::std::istream &in, locstruc &a)
{
    in >> a.utc >> a.pos >> a.att;
    return in;
}

//! @}

int32_t tle2sgp4(tlestruc tle, sgp4struc &sgp4)
{
    sgp4.i = DEGOF(tle.i);
    sgp4.ap = DEGOF(tle.ap);
    sgp4.bstar = tle.bstar;
    sgp4.e = tle.e;
    sgp4.ma = DEGOF(tle.ma);
    sgp4.mm = tle.mm * 86400. / D2PI;
    calstruc cal = mjd2cal(tle.utc);
    sgp4.ep = (cal.year - 2000.) * 1000. + cal.doy + cal.hour / 24. + cal.minute / 1440. + (cal.second + cal.nsecond / 1e9) / 86400.;
    sgp4.raan = DEGOF(tle.raan);
    return 0;
}

int32_t sgp42tle(sgp4struc sgp4, tlestruc &tle)
{
    tle.i = RADOF(sgp4.i);
    tle.ap = RADOF(sgp4.ap);
    tle.bstar = sgp4.bstar;
    tle.e = sgp4.e;
    tle.ma = RADOF(sgp4.ma);
    tle.mm = sgp4.mm * D2PI / 86400.;
    tle.raan = RADOF(sgp4.raan);
    int year = sgp4.ep / 1000;
    double jday = sgp4.ep - (year * 1000);
    if (year < 57)
        year += 2000;
    else
        year += 1900;
    tle.utc = cal2mjd((int)year, 1, 0.);
    tle.utc += jday;

    return 0;
}

// https://space.stackexchange.com/questions/5358/what-does-check-sum-tle-mean
int tle_checksum(const char *line)
{
    const int TLE_LINE_LENGTH = 68; // Ignore current checksum.
    int checksum = 0;

    for (int i = 0; i < TLE_LINE_LENGTH; i++)
    {
        if (line[i] == '-')
        {
            checksum++;
        }
        else if (isdigit(line[i]))
        {
            checksum += line[i] - '0';
        } // Ignore whitespace.
    }

    return checksum % 10;
}

string eci2tlestring(cartpos eci, tlestruc &reftle)
{
    eci2tle2(eci, reftle);
    return tle2tlestring(reftle);
}

string tle2tlestring(tlestruc reftle)
{
//    string line_1(69, ' ');
//    string line_2(69, ' ');
    string line_1;
    string line_2;

    // Ignore the name line. Populate our epoch field.
    line_1 = "1 ";
    line_1 += to_unsigned(reftle.snumber, 5, true) + "U ";
    line_1 += reftle.id + string(9-reftle.id.size(), ' ');
    line_1 += to_unsigned(mjd2year(reftle.utc)-2000, 2, true);
    line_1 += to_fixed(mjd2doy(reftle.utc), 12, 8, true) + " ";
    line_1 += to_floating(reftle.dmm*86400.*86400./D2PI, 8) + " ";
    line_1 += " 00000-0 ";
    int16_t bdrage = log10(reftle.bstar) + 5;
    line_1 += to_unsigned(bdrage, 6, true);
    int16_t bdragm = reftle.bstar / pow(10., bdrage-5);
    line_1 += to_unsigned(bdragm, 2, true) + " 0    0";
    line_1 += to_unsigned(tle_checksum(line_1.data()), 1);

    line_2 = "2 ";
    line_2 += to_unsigned(reftle.snumber, 5, true) + " ";
    line_2 += to_fixed(DEGOF(reftle.i), 8, 4) + " ";
    line_2 += to_fixed(DEGOF(reftle.raan), 8, 4) + " ";
    line_2 += to_unsigned(1.e7*reftle.e, 7) + " ";
    line_2 += to_fixed(DEGOF(reftle.ap), 8, 4) + " ";
    line_2 += to_fixed((DEGOF(reftle.ma) < 0) ? 360 + DEGOF(reftle.ma) : DEGOF(reftle.ma), 8, 4) + " ";
    line_2 += to_fixed(reftle.mm*86400./D2PI, 11, 8);
    line_2 += "00000";
    line_2 += to_unsigned(tle_checksum(line_1.data()), 1);

    string response = reftle.name + "\n";
    response += line_1 + "\n";
    response += line_2 + "\n";
    return response;
}

//! Nutation values
/*! Calculate the nutation values from the JPL Ephemeris for the provided UTC date.
* Values are in order: Psi, Epsilon, dPsi, dEpsilon. Units are radians and
* radians/second.
\param mjd UTC in Modified Julian Day.
\return Nutation values in an ::rvector.
*/
rvector utc2nuts(double mjd)
{
    static double lmjd = 0.;
    static uvector lcalc = {{{0., 0., 0.}, 0.}};

    if (mjd != lmjd)
    {
        double tt = utc2tt(mjd);
        if (tt > 0.)
        {
            jplnut(tt, (double *)&lcalc.a4);
            lmjd = mjd;
        }
    }
    return (lcalc.r);
}

//! Nutation Delta Psi value.
/*! Calculate the Delta Psi value (nutation in longitude), for use in the Nutation
matrix, for the provided UTC date.
\param mjd UTC in Modified Julian Day.
\return Delta Psi in radians.
*/
double utc2dpsi(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;
    rvector nuts;

    if (mjd != lmjd)
    {
        nuts = utc2nuts(mjd);
        lcalc = nuts.col[0];
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation Delta Epsilon value.
/*! Calculate the Delta Psi value (nutation in obliquity), for use in the Nutation
matrix, for the provided UTC date.
\param mjd UTC in Modified Julian Day.
\return Delta Psi in radians.longitudilon
*/
double utc2depsilon(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;
    rvector nuts;

    if (mjd != lmjd)
    {
        nuts = utc2nuts(mjd);
        lcalc = nuts.col[1];
        lmjd = mjd;
    }
    return (lcalc);
}

//! UTC to GAST
/*! Convert current UTC to Greenwhich Apparent Sidereal Time. Accounts for nutations.
\param mjd UTC as Modified Julian Day
\return GAST as Modified Julian Day
*/
double utc2gast(double mjd)
{
    static double lmjd = 0.;
    static double lgast = 0.;
    double omega, F, D;

    if (mjd != lmjd)
    {
        omega = utc2omega(mjd);
        F = utc2F(mjd);
        D = utc2D(mjd);
        lgast = utc2gmst1982(mjd) + utc2dpsi(mjd) * cos(utc2epsilon(mjd));
        lgast += DAS2R * .00264096 * sin(omega);
        lgast += DAS2R * .00006352 * sin(2. * omega);
        lgast += DAS2R * .00001175 * sin(2. * F - 2. * D + 3. * omega);
        lgast += DAS2R * .00001121 * sin(2. * F - 2. * D + omega);
        lgast = ranrm(lgast);
        lmjd = mjd;
    }
    return (lgast);
}

//! Calculate DUT1
/*! Calculate DUT1 = UT1 - UTC, based on lookup in IERS Bulletin A.
\param mjd UTC in Modified Julian Day.
\return DUT1 in Modified Julian Day, otherwise 0.
*/
double utc2dut1(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;
    double frac;
    //	uint32_t mjdi;
    uint32_t iersidx;

    if (mjd != lmjd)
    {
        if (load_iers() && iers.size() > 2)
        {
            if ((uint32_t)mjd >= iersbase)
            {
                if ((iersidx = (uint32_t)mjd - iersbase) > iers.size())
                {
                    iersidx = iers.size() - 2;
                }
            }
            else
            {
                iersidx = 0;
            }
            //			mjdi = (uint32_t)mjd;
            frac = mjd - (uint32_t)mjd;
            lcalc = ((frac * iers[1 + iersidx].dutc + (1. - frac) * iers[iersidx].dutc) / 86400.);
            lmjd = mjd;
        }
        else
            lcalc = 0.;
    }
    return (lcalc);
}

//! Convert UTC to UT1
/*! Convert Coordinated Universal Time to Universal Time by correcting for the offset
* between them at the given time. Table of DUT1 is first initialized from disk if it
* hasn't yet been.
\param mjd UTC in Modified Julian Day.
\return UTC1 in Modified Julian Day, otherwise 0.
*/
double utc2ut1(double mjd)
{
    static double lmjd = 0.;
    static double lut = 0.;

    if (mjd != lmjd)
    {
        if (load_iers())
        {
            lut = mjd + utc2dut1(mjd);
            lmjd = mjd;
        }
        else
            lut = 0.;
    }
    return (lut);
}

//! Convert TT to UTC.
/*! Convert Terrestrial Dynamical Time to Coordinated Universal Time by correcting for
* the appropriate number of Leap Seconds. Leap Second table is first initialized
* from disk if it hasn't yet been.
\param mjd TT in Modified Julian Day.
\return UTC in Modified Julian Day, otherwise 0.
*/
double tt2utc(double mjd)
{
    uint32_t iersidx;
    int32_t iretn = 0;

    if ((iretn = load_iers()) && iers.size() > 1)
    {
        if ((uint32_t)mjd >= iersbase)
        {
            if ((iersidx = (uint32_t)mjd - iersbase) > iers.size())
            {
                iersidx = iers.size() - 1;
            }
        }
        else
        {
            iersidx = 0;
            return ((double)iretn);
        }
        return (mjd - (32.184 + iers[iersidx].ls) / 86400.);
    }
    else
        return (0.);
}

//! Convert UTC to TT.
/*! Convert Coordinated Universal Time to Terrestrial Dynamical Time by correcting for
* the appropriate number of Leap Seconds. Leap Second table is first initialized from
* disk if it hasn't yet been.
\param mjd UTC in Modified Julian Day.
\return TT in Modified Julian Day, otherwise negative error
*/
double utc2tt(double mjd)
{
    static double lmjd = 0.;
    static double ltt = 0.;
    uint32_t iersidx = 0;
    int32_t iretn = 0;

    if (mjd != lmjd)
    {
        if ((iretn = load_iers()) && iers.size() > 1)
        {
            if ((uint32_t)mjd >= iersbase)
            {
                if ((iersidx = (uint32_t)mjd - iersbase) > iers.size())
                {
                    iersidx = iers.size() - 1;
                }
            }
            else
            {
                iersidx = 0;
            }
            ltt = (mjd + (32.184 + iers[iersidx].ls) / 86400.);
            lmjd = mjd;
            return (ltt);
        }
        else
        {
            return ((double)iretn);
        }
    }
    return (ltt);
}

//! Load IERS Polar Motion, UT1-UTC, Leap Seconds.
/*! Allocate and load array for storing data from IERS file. Each
* record includes the MJD, the Polar Motion for X and Y in radians,
* UT1-UTC in seconds of time, and the number of Leap Seconds since
* the creation of TAI in 1958. The following are then defined:
* - TT = TAI +32.184
* - TAI = UTC + Leap_Seconds
* - UT1 = UTC + (UT1-UTC)
\return Number of records.
*/
int32_t load_iers()
{
    FILE *fdes;
    iersstruc tiers;

    if (iers.size() == 0)
    {
        string fname;
        int32_t iretn = get_cosmosresources(fname);
        if (iretn < 0)
        {
            return iretn;
        }
        fname += "/general/iers_pm_dut_ls.txt";
        if ((fdes = fopen(fname.c_str(), "r")) != NULL)
        {
            char data[100];
            while (fgets(data, 100, fdes))
            {
                sscanf(data, "%u %lg %lg %lg %u", &tiers.mjd, &tiers.pmx, &tiers.pmy, &tiers.dutc, &tiers.ls);
                iers.push_back(tiers);
            }
            fclose(fdes);
        }
        if (iers.size())
            iersbase = iers[0].mjd;
    }
    return (iers.size());
}

//! Leap Seconds
/*! Returns number of leap seconds for provided Modified Julian Day.
\param mjd Provided time.
\return Number of leap seconds, or 0.
*/
int32_t leap_seconds(double mjd)
{
    uint32_t iersidx;

    if (load_iers() && iers.size() > 1)
    {
        if ((uint32_t)mjd >= iersbase)
        {
            if ((iersidx = (uint32_t)mjd - iersbase) > iers.size())
            {
                iersidx = iers.size() - 1;
            }
        }
        else
        {
            iersidx = 0;
        }
        return (iers[iersidx].ls);
    }
    else
        return 0;
}

//! Polar motion
/*! Returns polar motion in radians for provided Modified Julian Day.
\param mjd Provided time.
\return Polar motion in ::rvector.
*/
cvector polar_motion(double mjd)
{
    cvector pm;
    double frac;
    //	uint32_t mjdi;
    uint32_t iersidx;

    pm = cv_zero();
    if (load_iers() && iers.size() > 2)
    {
        if ((uint32_t)mjd >= iersbase)
        {
            if ((iersidx = (uint32_t)mjd - iersbase) > iers.size())
            {
                iersidx = iers.size() - 2;
            }
        }
        else
        {
            iersidx = 0;
        }
        //		mjdi = (uint32_t)mjd;
        frac = mjd - (uint32_t)mjd;
        pm = cv_zero();
        pm.x = frac * iers[1 + iersidx].pmx + (1. - frac) * iers[iersidx].pmx;
        pm.y = frac * iers[1 + iersidx].pmy + (1. - frac) * iers[iersidx].pmy;
    }

    return (pm);
}

//! Earth Rotation Angle
/*! Calculate the Earth Rotation Angle for a given Earth Rotation Time based on the
* provided UTC.
\param mjd Coordinated Universal Time as Modified Julian Day.
\return Earth Rotation Angle, theta, in radians.
*/
double utc2era(double mjd)
{
    static double lmjd = 0.;
    static double ltheta = 0.;
    double ut1;

    if (mjd != lmjd)
    {
        ut1 = utc2ut1(mjd);
        ltheta = D2PI * (.779057273264 + 1.00273781191135448 * (ut1 - 51544.5));
        //        ltheta = ranrm(ltheta);
    }

    return (ltheta);
}

double utc2gmst2000(double utc)
{
    static double lutc = 0.;
    static double lgmst = 0.;
    double tt;

    if (utc > 0.)
    {
        utc = currentmjd();
    }
    if (utc != lutc)
    {
        //		ut1 = utc2ut1(utc);
        tt = utc2jcentt(utc);
        lgmst = 24110.54841 + 8640184.812866 * utc2jcenut1(utc) + tt * tt * (0.093104 + tt * (-0.0000062));
        lgmst = ranrm(lgmst);
    }

    return lgmst;
}

//! UTC (Modified Julian Day) to GMST
/*! Convert current UT to Greenwhich Mean Sidereal Time
\param mjd UT as Modified Julian Day
\return GMST as radians
*/
double utc2gmst1982(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = utc2era(mjd) + DS2R * (.014506 + jcen * (4612.156534 + jcen * (1.3915817 + jcen * (-.00000044 + jcen * (-.000029956 + jcen * (-.0000000368)))))) / 15.;
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }

    return (lcalc);
}

//! UT1 Julian Century
/*! Caculate the number of centuries since J2000, UT1, for the provided date.
\param mjd Date in Modified Julian Day.
\return Julian century in decimal form.
*/
double utc2jcenut1(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;

    if (mjd != lmjd)
    {
        lcalc = (utc2ut1(mjd) - 51544.5) / 36525.;
        lmjd = mjd;
    }
    return (lcalc);
}
//! TT Julian Century
/*! Caculate the number of centuries since J2000, Terrestrial Time, for the provided date.
        \param mjd Date in Modified Julian Day.
        \return Julian century in decimal form, otherwise negative error.
        */
double utc2jcentt(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;

    if (mjd != lmjd)
    {
        double tt = utc2tt(mjd);
        if (tt <= 0.)
        {
            lcalc = tt;
        }
        else
        {
            lcalc = (tt - 51544.5) / 36525.;
            lmjd = mjd;
        }
    }
    return (lcalc);
}

//! Nutation Epsilon value.
/*! Calculate the Epsilon value (obliquity of the ecliptic), for use in the Nutation
        matrix, for the provided UTC date.
        \param mjd UTC in Modified Julian Day.
        \return Epsilon in radians.
        */
double utc2epsilon(double mjd)
{
    // Vallado, et al, AAS-06_134, eq. 17
    static double lmjd = 0.;
    static double lcalc = 0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R * (84381.406 + jcen * (-46.836769 + jcen * (-.0001831 + jcen * (0.0020034 + jcen * (-0.000000576 + jcen * (-0.0000000434))))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation L value.
/*! Calculate the L value,  for use in the Nutation matrix, for the provided UTC date.
        \param mjd UTC in Modified Julian Day.
        \return L in radians.
        */
double utc2L(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R * (485868.249036 + jcen * (1717915923.2178 + jcen * (31.8792 + jcen * (.051635 + jcen * (-.0002447)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation L prime value.
/*! Calculate the L prime value,  for use in the Nutation matrix, for the provided UTC date.
        \param mjd UTC in Modified Julian Day.
        \return L prime in radians.
        */
double utc2Lp(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R * (1287104.79305 + jcen * (129596581.0481 + jcen * (-.5532 + jcen * (.000136 + jcen * (-.00001149)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation F value.
/*! Calculate the F value,  for use in the Nutation matrix, for the provided UTC date.
        \param mjd UTC in Modified Julian Day.
        \return F in radians.
        */
double utc2F(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R * (335779.526232 + jcen * (1739527262.8478 + jcen * (-12.7512 + jcen * (-.001037 + jcen * (.00000417)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation D value.
/*! Calculate the D value,  for use in the Nutation matrix, for the provided UTC date.
        \param mjd UTC in Modified Julian Day.
        \return D in radians.
        */
double utc2D(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R * (1072260.70369 + jcen * (1602961601.209 + jcen * (-6.3706 + jcen * (.006593 + jcen * (-.00003169)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Nutation omega value.
/*! Calculate the omega value,  for use in the Nutation matrix, for the provided UTC date.
        \param mjd UTC in Modified Julian Day.
        \return Omega in radians.
        */
double utc2omega(double mjd)
{
    static double lmjd = 0.;
    static double lcalc = 0.;
    double jcen;

    if (mjd != lmjd)
    {
        jcen = utc2jcentt(mjd);
        lcalc = DAS2R * (450160.398036 + jcen * (-6962890.5431 + jcen * (7.4722 + jcen * (.007702 + jcen * (-.00005939)))));
        lcalc = ranrm(lcalc);
        lmjd = mjd;
    }
    return (lcalc);
}

//! Precession zeta value
/*! Calculate angle zeta used in the calculation of Precession, re.
         *  Capitaine, et. al, A&A, 412, 567-586 (2003)
         * Expressions for IAU 2000 precession quantities
         * Equation 40
         * \param utc Epoch in Modified Julian Day.
         * \return Zeta in radians
         */
double utc2zeta(double utc)
{
    double ttc = utc2jcentt(utc);
    //	double zeta = (2.650545 + ttc*(2306.083227 + ttc*(0.2988499 + ttc*(0.01801828 + ttc*(-0.000005971 + ttc*(0.0000003173))))))*DAS2R;
    // Vallado, eqn. 3-88
    double zeta = (ttc * (2306.2181 + ttc * (0.30188 + ttc * (0.017998)))) * DAS2R;
    return zeta;
}

//! Precession z value
/*! Calculate angle z used in the calculation of Precession, re.
         *  Capitaine, et. al, A&A, 412, 567-586 (2003)
         * Expressions for IAU 2000 precession quantities
         * Equation 40
         * \param utc Epoch in Modified Julian Day.
         * \return Zeta in radians
         */
double utc2z(double utc)
{
    double ttc = utc2jcentt(utc);
    //	double z = (-2.650545 + ttc*(2306.077181 + ttc*(1.0927348 + ttc*(0.01826837 + ttc*(-0.000028596 + ttc*(0.0000002904))))))*DAS2R;
    // Vallado, eqn. 3-88
    double z = (ttc * (2306.2181 + ttc * (1.09468 + ttc * (0.018203)))) * DAS2R;
    return z;
}

//! Precession theta value
/*! Calculate angle theta used in the calculation of Precession, re.
         *  Capitaine, et. al, A&A, 412, 567-586 (2003)
         * Expressions for IAU 2000 precession quantities
         * Equation 40
         * \param utc Epoch in Modified Julian Day.
         * \return Zeta in radians
         */
double utc2theta(double utc)
{
    double ttc = utc2jcentt(utc);
    //	double theta = ttc*(2004.191903 + ttc*(-0.4294934 + ttc*(-0.04182264 + ttc*(-0.000007089 + ttc*(-0.0000001274)))))*DAS2R;
    // Vallado, eqn. 3-88
    double theta = ttc * (2004.3109 + ttc * (-0.42665 + ttc * (-0.041833))) * DAS2R;
    return theta;
}

//! Convert UTC to TDB.
/*! Convert Coordinated Universal Time to Barycentric Dynamical Time by correcting for
        * the mean variations as a function of Julian days since 4713 BC Jan 1.5.
        \param mjd UTC in Modified Julian Day.
        \return TDB in Modified Julian Day, otherwise 0.
        */
double utc2tdb(double mjd)
{
    static double lmjd = 0.;
    static double ltdb = 0.;
    double tt, g;

    if (mjd != lmjd)
    {
        tt = utc2tt(mjd);
        if (tt > 0.)
        {
            g = 6.2400756746 + .0172019703436 * (mjd - 51544.5);
            ltdb = (tt + (.001658 * sin(g) + .000014 * sin(2 * g)) / 86400.);
            lmjd = mjd;
        }
    }
    return (ltdb);
}

//! Convert UTC to GPS
/*! Convert Coordinated Universal Time to GPS Time, correcting for the appropriate
         * number of leap seconds. Leap Second table is first initialized from
         * disk if it hasn't yet been.
         * \param utc UTC expressed in Modified Julian Days
         * \return GPS Time expressed in Modified Julian Days, otherwise negative error
         */
double utc2gps(double utc)
{
    double gps;

    if ((gps = utc2tt(utc)) <= 0.)
    {
        return (gps);
    }

    gps -= 51.184 / 86400.;

    return (gps);
}

//! Convert GPS to UTC
/*! Convert GPS Time to Coordinated Universal Time, correcting for the appropriate
         * number of leap seconds. Leap Second table is first initialized from
         * disk if it hasn't yet been.
         * \param gps GPS Time expressed in Modified Julian Days
         * \return UTC expressed in Modified Julian Days, otherwise 0.
         */
double gps2utc(double gps)
{
    double utc;

    gps += 51.184 / 86400.;

    if ((utc = tt2utc(gps)) <= 0.)
    {
        return (utc);
    }

    return (utc);
}

}
}
