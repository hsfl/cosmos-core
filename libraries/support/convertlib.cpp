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

/*! \file convertlib.c
	\brief Coordinate conversion library source file
*/

#include "convertlib.h"
#include "timelib.h"
#include "geomag.h"
#include "jsondef.h"
#include "ephemlib.h"

#include <iostream>

uint16_t tlecount;

//! \addtogroup convertlib_functions
//! @{

//! Initialize ::locstruc.
/*! Set entire structure to zero so that we can
 * properly propagate changes.
	\param loc Pointer to ::locstruc that contains positions.
	attitudes.
*/
void loc_clear(locstruc *loc)
{
	memset((void*) loc, 0, sizeof(locstruc));
}

//! Initialize ::posstruc.
/*! Set entire structure to zero so that we can
 * properly propagate changes.
	\param loc Pointer to ::locstruc that contains positions.
	attitudes.
*/
void pos_clear(locstruc &loc)
{
	memset((void*) &loc.pos, 0, sizeof(posstruc));
	att_clear(loc.att);
}

//! Initialize ::attstruc.
/*! Set entire structure to zero so that we can
 * properly propagate changes.
	\param loc Pointer to ::locstruc that contains attitudes.
*/
void att_clear(attstruc &att)
{
	memset((void*) &att, 0, sizeof(attstruc));
}

//! Calculate Extra position information
/*! Calculate things like sun position and insolation, and elements that are used in
 * conversion, like libration and J2000 rotation matrices.
	\param pos ::posstruc with the current position and those to be updated.
	\param utc Universal time of extra information
*/
void pos_extra(locstruc *loc)
{
    // avoid expensive calculations
    // ?? not sure why the second condition is here
	if (!isfinite(loc->utc) || loc->pos.extra.utc == loc->utc)
		return;

	loc->pos.extra.utc = loc->utc;
	loc->pos.extra.tt = utc2tt(loc->utc);
	loc->pos.extra.tdb = utc2tdb(loc->utc);
	loc->pos.extra.ut = utc2ut1(loc->utc);

	icrs2itrs(loc->utc,&loc->pos.extra.j2t,&loc->pos.extra.j2e,&loc->pos.extra.dj2e,&loc->pos.extra.ddj2e);
	loc->pos.extra.t2j = rm_transpose(loc->pos.extra.j2t);
	loc->pos.extra.e2j = rm_transpose(loc->pos.extra.j2e);
	loc->pos.extra.de2j = rm_transpose(loc->pos.extra.dj2e);
	loc->pos.extra.dde2j = rm_transpose(loc->pos.extra.ddj2e);

	jpllib(loc->utc,&loc->pos.extra.s2t,&loc->pos.extra.ds2t);
	loc->pos.extra.t2s = rm_transpose(loc->pos.extra.s2t);
	loc->pos.extra.dt2s = rm_transpose(loc->pos.extra.ds2t);

	loc->pos.extra.j2s = rm_mmult(loc->pos.extra.t2s,loc->pos.extra.j2t);
	loc->pos.extra.s2j = rm_transpose(loc->pos.extra.j2s);


	jplpos(JPL_SUN_BARY,JPL_EARTH,loc->pos.extra.tt,&loc->pos.extra.sun2earth);
	jplpos(JPL_SUN_BARY,JPL_MOON,loc->pos.extra.tt,&loc->pos.extra.sun2moon);
}

//! Set Barycentric position
/*! Set the current time and position to whatever is in the Barycentric position of the
 * ::posstruc. Then propagate to all the other positions.
	\param pos ::posstruc with the current position and those to be updated.
*/
void pos_baryc(locstruc *loc)
{
	double distance, theta;
	rvector sat2body;

	// Synchronize time
	if (loc->pos.baryc.utc == 0.)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.baryc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.baryc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.baryc.utc;
	}
	
	pos_extra(loc);

	// Determine closest planetary body
	loc->pos.extra.closest = COSMOS_EARTH;
	if (length_rv(rv_sub(loc->pos.baryc.s,loc->pos.extra.sun2moon.s)) < length_rv(rv_sub(loc->pos.baryc.s,loc->pos.extra.sun2earth.s)))
		loc->pos.extra.closest = COSMOS_MOON;

	// Set SUN specific stuff
	distance = length_rv(loc->pos.baryc.s);
	loc->pos.sunsize = (float)(RSUNM/distance);
	loc->pos.sunradiance = (float)(3.839e26/(4.*DPI*distance*distance));

	// Check Earth:Sun separation
	sat2body = rv_sub(loc->pos.baryc.s,loc->pos.extra.sun2earth.s);
	loc->pos.earthsep = (float)(sep_rv(loc->pos.baryc.s,sat2body));
	loc->pos.earthsep -= (float)(asin(REARTHM/length_rv(sat2body)));
	if (loc->pos.earthsep < -loc->pos.sunsize)
		loc->pos.sunradiance = 0.;
	else
		if (loc->pos.earthsep <= loc->pos.sunsize)
		{
			theta = DPI*(loc->pos.sunsize+loc->pos.earthsep)/loc->pos.sunsize;
			loc->pos.sunradiance *= (float)((theta - sin(theta))/D2PI);
		}

	// Set Moon specific stuff
	sat2body = rv_sub(loc->pos.baryc.s,loc->pos.extra.sun2moon.s);

	// Check Earth:Moon separation
	loc->pos.moonsep = (float)(sep_rv(loc->pos.baryc.s,sat2body));
	loc->pos.moonsep -= (float)(asin(RMOONM/length_rv(sat2body)));
	if (loc->pos.moonsep < -loc->pos.sunsize)
		loc->pos.sunradiance = 0.;
	else
		if (loc->pos.moonsep <= loc->pos.sunsize)
		{
			theta = DPI*(loc->pos.sunsize+loc->pos.moonsep)/loc->pos.sunsize;
			loc->pos.sunradiance *= (float)((theta - sin(theta))/D2PI);
		}

	// Set related attitudes
	loc->att.icrf.pass = loc->pos.baryc.pass;
	loc->att.icrf.utc = loc->pos.baryc.utc;

	// Set adjoining positions
	if (loc->pos.baryc.pass > loc->pos.eci.pass)
	{
		pos_baryc2eci(loc);
		pos_eci(loc);
	}
	if (loc->pos.baryc.pass > loc->pos.sci.pass)
	{
		pos_baryc2sci(loc);
		pos_sci(loc);
	}

}

//! Set ECI position
/*! Set the current time and position to whatever is in the Earth Centered Inertial position of the
 * ::posstruc. Then propagate to all the other positions.
	\param pos ::posstruc with the current position and those to be updated.
*/
void pos_eci(locstruc *loc)
{
	// Synchronize time
	if (0. == loc->pos.eci.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.eci.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.eci.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.eci.utc;
	}
	
	pos_extra(loc);

	// Set adjoining positions
	if (loc->pos.eci.pass > loc->pos.baryc.pass)
	{
		pos_eci2baryc(loc);
		pos_baryc(loc);
	}
	if (loc->pos.eci.pass > loc->pos.geoc.pass)
	{
		pos_eci2geoc(loc);
		pos_geoc(loc);
	}

	// Set related attitude
	loc->att.icrf.pass = loc->pos.eci.pass;

}

//! Set SCI position
/*! Set the current time and position to whatever is in the Selene Centered Inertial position of the
 * ::posstruc. Then propagate to all the other positions.
	\param pos ::posstruc with the current position and those to be updated.
*/
void pos_sci(locstruc *loc)
{
	// Synchronize time
	if (0. == loc->pos.sci.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.sci.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.sci.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.sci.utc;
	}

	pos_extra(loc);

	// Set adjoining positions
	if (loc->pos.sci.pass > loc->pos.baryc.pass)
	{
		pos_sci2baryc(loc);
		pos_baryc(loc);
	}
	if (loc->pos.sci.pass > loc->pos.selc.pass)
	{
		pos_sci2selc(loc);
		pos_selc(loc);
	}

	// Set related attitude
	loc->att.icrf.pass = loc->pos.sci.pass;

}

//! Set Geocentric position
/*! Set the current time and position to whatever is in the Geocentric position of the
 * ::posstruc. Then propagate to all the other positions.
	\param pos ::posstruc with the current position and those to be updated.
*/
void pos_geoc(locstruc *loc)
{
	// Synchronize time
	if (0. == loc->pos.geoc.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.geoc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.geoc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.geoc.utc;
	}
	
	pos_extra(loc);

	// Go to ECI if necessary
	if (loc->pos.geoc.pass > loc->pos.eci.pass)
	{
		pos_geoc2eci(loc);
		pos_eci(loc);
	}
	// Go to Geocentric Spherical if necessary
	if (loc->pos.geoc.pass > loc->pos.geos.pass)
	{
		pos_geoc2geos(loc);
		pos_geos(loc);
	}
	// Go to Geodetic if necessary
	if (loc->pos.geoc.pass > loc->pos.geod.pass)
	{
		pos_geoc2geod(loc);
		pos_geod(loc);
	}

	// Set related attitude
	loc->att.geoc.pass = loc->pos.geoc.pass;

}

//! Set Selenocentric position
/*! Set the current time and position to whatever is in the Selenocentric position of the
 * ::posstruc. Then propagate to all the other positions.
	\param pos ::posstruc with the current position and those to be updated.
*/
void pos_selc(locstruc *loc)
{
	// Synchronize time
	if (0. == loc->pos.selc.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.selc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.selc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.selc.utc;
	}

	pos_extra(loc);

	// Go to SCI if necessary
	if (loc->pos.selc.pass > loc->pos.sci.pass)
	{
		pos_selc2sci(loc);
		pos_sci(loc);
	}
	// Go to Selenographic if necessary
	if (loc->pos.selc.pass > loc->pos.selg.pass)
	{
		pos_selc2selg(loc);
		pos_selg(loc);
	}

	// Set related attitude
	loc->att.selc.pass = loc->pos.selc.pass;

}

//! Set Selenographic position
/*! Set the current time and position to whatever is in the Selenographic position of the
 * ::posstruc. Then propagate to all the other positions.
	\param pos ::posstruc with the current position and those to be updated.
*/
void pos_selg(locstruc *loc)
{
	// Synchroniz time
	if (0. == loc->pos.selg.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.selg.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.selg.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.selg.utc;
	}

	pos_extra(loc);

	if (loc->pos.selg.pass > loc->pos.selc.pass)
	{
		pos_selg2selc(loc);
		pos_selc(loc);
	}

}

//! Set Geographic position
/*! Set the current time and position to whatever is in the Geographic position of the
 * ::posstruc. Then propagate to all the other positions.
	\param pos ::posstruc with the current position and those to be updated.
*/
void pos_geos(locstruc *loc)
{
	// Synchronize time
	if (0. == loc->pos.geos.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.geos.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.geos.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.geos.utc;
	}

	pos_extra(loc);

	if (loc->pos.geos.pass > loc->pos.geoc.pass)
	{
		pos_geos2geoc(loc);
		pos_geoc(loc);
	}

}

//! Set Geodetic position
/*! Set the current time and position to whatever is in the Geodetic position of the
 * ::posstruc. Then propagate to all the other positions.
	\param pos ::posstruc with the current position and those to be updated.
*/
void pos_geod(locstruc *loc)
{
	// Synchroniz time
	if (0. == loc->pos.geod.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.geod.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.geod.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.geod.utc;
	}

	pos_extra(loc);

	if (loc->pos.geod.pass > loc->pos.geoc.pass)
	{
		pos_geod2geoc(loc);
		pos_geoc(loc);
	}
	// Determine magnetic field in Topocentric system
	geomag_front(loc->pos.geod.s,mjd2year(loc->utc),&loc->bearth);

	// Transform to ITRS
	loc->bearth = transform_q(q_change_around_z(-loc->pos.geod.s.lon),transform_q(q_change_around_y(DPI2+loc->pos.geod.s.lat),loc->bearth));
	//	loc->bearth = transform_q(q_change_around_z(loc->pos.geod.s.lon-DPI),transform_q(q_change_around_y(DPI2-loc->pos.geod.s.lat),loc->bearth));
}

//! Convert Barycentric to ECI
/*! Propogate the position found in the Barycentric slot of the supplied ::posstruc to
 * the Earth Centered Inertial slot, performing all relevant updates.
	\param pos Working ::posstruc
*/
void pos_baryc2eci(locstruc *loc)
{
	// Synchronize time
	if (loc->pos.baryc.utc == 0.)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.baryc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.baryc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.baryc.utc;
	}
	
	// Update extra information
	pos_extra(loc);

	// Update time
	loc->pos.eci.utc = loc->utc;

	// Update pass
	loc->pos.eci.pass = loc->pos.baryc.pass;

	// Heliocentric to Geocentric Ecliptic
	loc->pos.eci.s = loc->pos.eci.v = loc->pos.eci.a = rv_zero();

	loc->pos.eci.s = rv_sub(loc->pos.baryc.s,loc->pos.extra.sun2earth.s);
	loc->pos.eci.v = rv_sub(loc->pos.baryc.v,loc->pos.extra.sun2earth.v);
	loc->pos.eci.a = rv_sub(loc->pos.baryc.a,loc->pos.extra.sun2earth.a);
}

//! Convert ECI to Barycentric
/*! Propogate the position found in the Earth Centered Inertial slot of the supplied ::posstruc to
 * the Barycentric slot, performing all relevant updates.
	\param pos Working ::posstruc
*/
void pos_eci2baryc(locstruc *loc)
{
	// Synchronize time
	if (0. == loc->pos.eci.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.eci.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.eci.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.eci.utc;
	}
	
	// Update extra information
	pos_extra(loc);

	// Update pass
	loc->pos.baryc.pass = loc->pos.eci.pass;

	// Update time
	loc->pos.baryc.utc = loc->utc;

	// Geocentric Equatorial to Heliocentric
	loc->pos.baryc.s = rv_add(loc->pos.eci.s,loc->pos.extra.sun2earth.s);
	loc->pos.baryc.v = rv_add(loc->pos.eci.v,loc->pos.extra.sun2earth.v);
	loc->pos.baryc.a = rv_add(loc->pos.eci.a,loc->pos.extra.sun2earth.a);
}

//! Convert Barycentric to SCI
/*! Propogate the position found in the Barycentric slot of the supplied ::posstruc to
 * the Selene Centered Inertial slot, performing all relevant updates.
	\param pos Working ::posstruc
*/
void pos_baryc2sci(locstruc *loc)
{
	// Synchronize time
	if (loc->pos.baryc.utc == 0.)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.baryc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.baryc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.baryc.utc;
	}

	// Update extra information
	pos_extra(loc);

	// Update time
	loc->pos.sci.utc = loc->utc;

	// Update pass
	loc->pos.sci.pass = loc->pos.baryc.pass;

	// Heliocentric to Geocentric Ecliptic
	loc->pos.sci.s = loc->pos.sci.v = loc->pos.sci.a = rv_zero();

	loc->pos.sci.s = rv_sub(loc->pos.baryc.s,loc->pos.extra.sun2moon.s);
	loc->pos.sci.v = rv_sub(loc->pos.baryc.v,loc->pos.extra.sun2moon.v);
	loc->pos.sci.a = rv_sub(loc->pos.baryc.a,loc->pos.extra.sun2moon.a);

}

//! Convert SCI to Barycentric
/*! Propogate the position found in the Selene Centered Inertial slot of the supplied ::posstruc to
 * the Barycentric slot, performing all relevant updates.
	\param pos Working ::posstruc
*/
void pos_sci2baryc(locstruc *loc)
{
	// Synchronize time
	if (0. == loc->pos.sci.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.sci.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.sci.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.sci.utc;
	}

	// Update extra information
	pos_extra(loc);

	// Update time
	loc->pos.baryc.utc = loc->utc;

	// Update pass
	loc->pos.baryc.pass = loc->pos.sci.pass;

	// Geocentric Equatorial to Heliocentric
	loc->pos.baryc.s = rv_add(loc->pos.sci.s,loc->pos.extra.sun2moon.s);
	loc->pos.baryc.v = rv_add(loc->pos.sci.v,loc->pos.extra.sun2moon.v);
	loc->pos.baryc.a = rv_add(loc->pos.sci.a,loc->pos.extra.sun2moon.a);

}

//! Convert ECI to GEOC
/*! Propogate the position found in the Earth Centered Inertial slot of the supplied ::posstruc to
 * the Geocentric slot, performing all relevant updates.
	\param pos Working ::posstruc
*/
void pos_eci2geoc(locstruc *loc)
{
	rvector v2 = {{0.}};

	// Synchronize time
	if (0. == loc->pos.eci.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.eci.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.eci.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.eci.utc;
	}
	
	// Update extra information
	pos_extra(loc);

	// Update time
	loc->pos.geoc.utc = loc->utc;

	// Update pass
	loc->pos.geoc.pass = loc->att.icrf.pass = loc->pos.eci.pass;

	// Apply first order transform to all
	loc->pos.geoc.s = rv_mmult(loc->pos.extra.j2e,loc->pos.eci.s);
	loc->pos.geoc.v = rv_mmult(loc->pos.extra.j2e,loc->pos.eci.v);
	loc->pos.geoc.a = rv_mmult(loc->pos.extra.j2e,loc->pos.eci.a);

	// Apply second order term due to first derivative of rotation matrix
	v2 = rv_mmult(loc->pos.extra.dj2e,loc->pos.eci.s);
	loc->pos.geoc.v = rv_add(loc->pos.geoc.v,v2);
	v2 = rv_smult(2.,rv_mmult(loc->pos.extra.dj2e,loc->pos.eci.v));
	loc->pos.geoc.a = rv_add(loc->pos.geoc.a,v2);
	// Apply third order correction due to second derivative of rotation matrix
	v2 = rv_mmult(loc->pos.extra.ddj2e,loc->pos.eci.s);
	loc->pos.geoc.a = rv_add(loc->pos.geoc.a,v2);

	// Convert GEOC Position to GEOD
	pos_geoc2geod(loc);

	// Convert GEOC Position to GEOS
	pos_geoc2geos(loc);

	// Convert ICRF attitude to ITRF
	att_icrf2geoc(loc);

	// Convert ITRF Attitude to Topo
	att_planec2topo(loc);

	// Convert ITRF Attitude to LVLH
	att_planec2lvlh(loc);
}

//! Convert GEOC to ECI
/*! Propogate the position found in the Geocentric slot of the supplied ::posstruc to
 * the Earth Centered Inertial slot, performing all relevant updates.
	\param pos Working ::posstruc
*/
void pos_geoc2eci(locstruc *loc)
{
	rvector ds;

	// Synchronize time
	if (0. == loc->pos.geoc.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.geoc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.geoc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.geoc.utc;
	}
	
	// Update extra information
	pos_extra(loc);

	// Update time
	loc->pos.eci.utc = loc->utc;

	// Update pass
	loc->pos.eci.pass = loc->att.geoc.pass = loc->pos.geoc.pass;

	// Apply first order transform to all
	loc->pos.eci.s = rv_mmult(loc->pos.extra.e2j,loc->pos.geoc.s);
	loc->pos.eci.v = rv_mmult(loc->pos.extra.e2j,loc->pos.geoc.v);
	loc->pos.eci.a = rv_mmult(loc->pos.extra.e2j,loc->pos.geoc.a);

	// Apply second order correction due to first derivative of rotation matrix
	ds = rv_mmult(loc->pos.extra.de2j,loc->pos.geoc.s);
	loc->pos.eci.v = rv_add(loc->pos.eci.v,ds);
	ds = rv_smult(2.,rv_mmult(loc->pos.extra.de2j,loc->pos.geoc.v));
	loc->pos.eci.a = rv_add(loc->pos.eci.a,ds);
	// Apply third order correction due to second derivative of rotation matrix
	ds = rv_mmult(loc->pos.extra.dde2j,loc->pos.geoc.s);
	loc->pos.eci.a = rv_add(loc->pos.eci.a,ds);

	// Convert ITRF Attitude to ICRF
	att_geoc2icrf(loc);

	// Convert ITRF Attitude to LVLH
	att_planec2lvlh(loc);

	// Convert ITRF Attitude to TOPO
	att_planec2topo(loc);
}

//! Convert GEOC to GEOS
/*! Convert a Geocentric ::cartpos to a Geographic ::spherpos.
	\param geoc Source Geocentric position.
	\param geos Destination Geographic position.
*/
void pos_geoc2geos(locstruc *loc)
{
	double xvx, yvy, r2, r, minir, minir2;
	double cp, cl, sl, sp;

	// Synchronize time
	if (0. == loc->pos.geoc.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.geoc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.geoc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.geoc.utc;
	}
	
	// Update time
	loc->pos.geos.utc = loc->utc;

	// Update pass
	loc->pos.geos.pass = loc->pos.geoc.pass;

	// Convert Geocentric Cartesian to Spherical
	minir2 = loc->pos.geoc.s.col[0]*loc->pos.geoc.s.col[0]+loc->pos.geoc.s.col[1]*loc->pos.geoc.s.col[1];
	minir = sqrt(minir2);
	r2 = minir2+loc->pos.geoc.s.col[2]*loc->pos.geoc.s.col[2];
	loc->pos.geos.s.r = r = sqrt(r2);
	sp = loc->pos.geoc.s.col[2]/r;
	loc->pos.geos.s.phi = asin(sp);
	loc->pos.geos.s.lambda = atan2(loc->pos.geoc.s.col[1],loc->pos.geoc.s.col[0]);

	xvx = loc->pos.geoc.s.col[0]*loc->pos.geoc.v.col[0];
	yvy = loc->pos.geoc.s.col[1]*loc->pos.geoc.v.col[1];
	loc->pos.geos.v.r = (xvx+yvy+loc->pos.geoc.s.col[2]*loc->pos.geoc.v.col[2])/r;
	loc->pos.geos.v.phi = (-(xvx+yvy) * loc->pos.geoc.s.col[2] + minir2*loc->pos.geoc.v.col[2])/(r2*minir);
	//loc->pos.geos.v.lambda = -(loc->pos.geoc.s.col[0]*loc->pos.geoc.v.col[1]+loc->pos.geoc.s.col[1]*loc->pos.geoc.v.col[0])/minir2;

	cp = minir / r;
	cl = loc->pos.geoc.s.col[0] / minir;
	sl = loc->pos.geoc.s.col[1] / minir;
	if (fabs(loc->pos.geoc.s.col[1]) > fabs(loc->pos.geoc.s.col[0]))
		loc->pos.geos.v.lambda = (loc->pos.geoc.v.col[0] - cp * cl * loc->pos.geos.v.r + loc->pos.geoc.s.col[2] * cl * loc->pos.geos.v.phi) / (-loc->pos.geoc.s.col[1]);
	else
		loc->pos.geos.v.lambda = (loc->pos.geoc.v.col[1] - cp * sl * loc->pos.geos.v.r + loc->pos.geoc.s.col[2] * sl * loc->pos.geos.v.phi) / loc->pos.geoc.s.col[0];

}

//! Convert GEOS to GEOC
/*! Convert a Geographic ::spherpos to a Geocentric ::cartpos.
	\param geos Source Geographic position.
	\param geoc Destination Geocentric position.
*/
void pos_geos2geoc(locstruc *loc)
{
	double sp, cp, sl, cl, cpr;

	// Synchronize time
	if (0. == loc->pos.geos.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.geos.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.geoc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.geos.utc;
	}

	// Update time
	loc->pos.geoc.utc = loc->utc;

	// Update pass
	loc->pos.geoc.pass = loc->pos.geos.pass;

	sp = sin(loc->pos.geos.s.phi);
	cp = cos(loc->pos.geos.s.phi);
	sl = sin(loc->pos.geos.s.lambda);
	cl = cos(loc->pos.geos.s.lambda);
	cpr = cp * loc->pos.geos.s.r;

	loc->pos.geoc.s = loc->pos.geoc.v = loc->pos.geoc.a = rv_zero();

	loc->pos.geoc.s.col[0] = cpr * cl;
	loc->pos.geoc.s.col[1] = cpr * sl;
	loc->pos.geoc.s.col[2] = loc->pos.geos.s.r * sp;

	//loc->pos.geoc.v.col[0] = loc->pos.geos.v.r * cp * cl - loc->pos.geos.v.phi * loc->pos.geos.s.r * sp * cl - loc->pos.geos.v.lambda * cpr * sl;
	//loc->pos.geoc.v.col[1] = loc->pos.geos.v.r * cp * sl - loc->pos.geos.v.phi * loc->pos.geos.s.r * sp * sl + loc->pos.geos.v.lambda * cpr * cl;
	loc->pos.geoc.v.col[0] = loc->pos.geos.v.r * cp * cl - loc->pos.geos.v.lambda * cpr * sl - loc->pos.geos.v.phi * loc->pos.geos.s.r * sp * cl;
	loc->pos.geoc.v.col[1] = loc->pos.geos.v.r * cp * sl + loc->pos.geos.v.lambda * cpr * cl - loc->pos.geos.v.phi * loc->pos.geos.s.r * sp * sl;
	loc->pos.geoc.v.col[2] = loc->pos.geos.v.r * sp + loc->pos.geos.v.phi * cpr;
}

//! Convert GEOC to GEOD
/*! Convert a Geocentric ::cartpos to a Geodetic ::geoidpos.
	\param geoc Source Geocentric position.
	\param geod Destination Geodetic position.
*/
void pos_geoc2geod(locstruc *loc)
{
	double e2;
	double st;
	double ct, cn, sn;
	double c, rp, a1, a2, a3, b1, b2, c1, c2, c3, rbc;
	double p, phi, h, nh, rn;

	// Synchronize time
	if (0. == loc->pos.geoc.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.geoc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.geoc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.geoc.utc;
	}
	
	// Update time
	loc->pos.geod.utc = loc->utc;

	// Update pass
	loc->pos.geod.pass = loc->pos.geoc.pass;

	loc->pos.geod.s.lon = atan2(loc->pos.geoc.s.col[1],loc->pos.geoc.s.col[0]);

	// Calculate effects of oblate spheroid
	e2 = (1. - FRATIO2);
	p = sqrt(loc->pos.geoc.s.col[0]*loc->pos.geoc.s.col[0] + loc->pos.geoc.s.col[1]*loc->pos.geoc.s.col[1]);
	nh = sqrt(p*p+loc->pos.geoc.s.col[2]*loc->pos.geoc.s.col[2]) - REARTHM;
	phi = atan2(loc->pos.geoc.s.col[2],p);
	do
	{
		h = nh;
		st = sin(phi);
		rn = REARTHM / sqrt(1.-e2*st*st);
		nh = p/cos(phi) - rn;
		phi = atan((loc->pos.geoc.s.col[2]/p)/(1.-e2*rn/(rn+h)));
	} while (fabs(nh-h) > .01);

	loc->pos.geod.s.lat = phi;
	loc->pos.geod.s.h = h;

	st = sin(loc->pos.geod.s.lat);
	ct = cos(loc->pos.geod.s.lat);
	sn = sin(loc->pos.geod.s.lon);
	cn = cos(loc->pos.geod.s.lon);

	c = 1./sqrt(ct * ct + FRATIO2 * st *st);
	rp = loc->pos.geod.s.h + REARTHM * FRATIO2 * c * c * c;
	a1 = ct * cn;
	b1 = -loc->pos.geoc.s.col[1];
	c1 = -st * cn * rp;
	a2 = ct * sn;
	b2 = loc->pos.geoc.s.col[0];
	c2 = -st * sn * rp;
	a3 = st;
	c3 = ct * rp;
	rbc = (b1*c2 - b2*c1)/c3;
	loc->pos.geod.v.h = (b2 * loc->pos.geoc.v.col[0] - b1 * loc->pos.geoc.v.col[1]
						+ rbc * loc->pos.geoc.v.col[2])/(b2 * a1 - b1 * a2 + rbc*a3);
	loc->pos.geod.v.lat = (loc->pos.geoc.v.col[2] - a3 * loc->pos.geod.v.h) / c3;
	if (fabs(b1) > fabs(b2))
		loc->pos.geod.v.lon = (loc->pos.geoc.v.col[0] - a1 * loc->pos.geod.v.h - c1 * loc->pos.geod.v.lat) / b1;
	else
		loc->pos.geod.v.lon = (loc->pos.geoc.v.col[1] - a2 * loc->pos.geod.v.h - c2 * loc->pos.geod.v.lat) / b2;

	// Determine magnetic field in Topocentric system
	geomag_front(loc->pos.geod.s,mjd2year(loc->utc),&loc->bearth);

	// Transform to ITRS
	//	loc->bearth = transform_q(q_change_around_z(loc->pos.geod.s.lon-DPI),transform_q(q_change_around_y(DPI2-loc->pos.geod.s.lat),loc->bearth));
	loc->bearth = transform_q(q_change_around_z(-loc->pos.geod.s.lon),transform_q(q_change_around_y(DPI2+loc->pos.geod.s.lat),loc->bearth));

}

//! Convert GEOD to GEOC
/*! Convert a Geodetic ::geoidpos to a Geocentric ::cartpos.
	\param geod Source Geodetic position.
	\param geoc Destination Geocentric position.
*/
void pos_geod2geoc(locstruc *loc)
{
	double lst, dlst, r, c, s, c2, rp;
	double cn, ct, sn, st;

	// Synchroniz time
	if (0. == loc->pos.geod.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.geod.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.geod.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.geod.utc;
	}

	// Update time
	loc->pos.geoc.utc = loc->pos.geod.utc = loc->pos.utc = loc->utc;

	// Update pass
	loc->pos.geoc.pass = loc->pos.geod.pass;

	// Determine effects of oblate spheroid
	ct = cos(loc->pos.geod.s.lat);
	st = sin(loc->pos.geod.s.lat);
	c = 1./sqrt(ct * ct + FRATIO2 * st * st);
	c2 = c * c;
	s = FRATIO2 * c;
	r = (REARTHM * c + loc->pos.geod.s.h) * ct;

	loc->pos.geoc.s.col[2] = (REARTHM * s + loc->pos.geod.s.h) * st;

	lst = loc->pos.geod.s.lon;
	cn = cos(lst);
	sn = sin(lst);
	loc->pos.geoc.s.col[0] = r * cn;
	loc->pos.geoc.s.col[1] = r * sn;

	rp = loc->pos.geod.s.h + REARTHM * s * c2;
	loc->pos.geoc.v.col[2] = st * loc->pos.geod.v.h + rp * ct * loc->pos.geod.v.lat;

	dlst = loc->pos.geod.v.lon;
	loc->pos.geoc.v.col[0] = cn * ct * loc->pos.geod.v.h - loc->pos.geoc.s.col[1] * dlst - rp * cn * st * loc->pos.geod.v.lat;
	loc->pos.geoc.v.col[1] = sn * ct * loc->pos.geod.v.h + loc->pos.geoc.s.col[0] * dlst - rp * sn * st * loc->pos.geod.v.lat;
}

//! Convert SCI to SELC
/*! Propogate the position found in the Selene Centered Inertial slot of the supplied ::posstruc to
 * the Selenocentric slot, performing all relevant updates.
	\param pos Working ::posstruc
*/
void pos_sci2selc(locstruc *loc)
{
	rvector v2 = {{0.}};
	rmatrix m1;

	// Synchronize time
	if (0. == loc->pos.sci.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.sci.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.sci.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.sci.utc;
	}

	// Update extra information
	pos_extra(loc);

	// Update time
	loc->pos.selc.utc = loc->utc;

	// Update pass
	loc->pos.selc.pass = loc->att.icrf.pass = loc->pos.sci.pass;

	// Apply first order transform to all: J2000 to ITRS, then Earth to Moon
	loc->pos.selc.s = rv_mmult(loc->pos.extra.j2s,loc->pos.sci.s);
	loc->pos.selc.v = rv_mmult(loc->pos.extra.j2s,loc->pos.sci.v);
	loc->pos.selc.a = rv_mmult(loc->pos.extra.j2s,loc->pos.sci.a);

	// Apply second order term due to first derivative of rotation matrix
	m1 = rm_mmult(loc->pos.extra.dt2s,loc->pos.extra.j2t);
	v2 = rv_mmult(m1,loc->pos.sci.s);
	loc->pos.selc.v = rv_add(loc->pos.selc.v,v2);

	v2 = rv_smult(2.,rv_mmult(m1,loc->pos.sci.v));
	loc->pos.selc.a = rv_add(loc->pos.selc.a,v2);

	// Convert SELC Position to SELG
	pos_selc2selg(loc);

	// Convert ICRF Attitude to SELC
	att_icrf2selc(loc);

	// Convert ITRF Attitude to Topo
	att_planec2topo(loc);

	// Convert ITRF Attitude to LVLH
	att_planec2lvlh(loc);
}

//! Convert SELC to SCI
/*! Propogate the position found in the Selenocentric slot of the supplied ::posstruc to
 * the Selene Centered Inertial slot, performing all relevant updates.
	\param pos Working ::posstruc
*/
void pos_selc2sci(locstruc *loc)
{
	rvector v2;
	rmatrix m1;

	// Synchroniz time
	if (0. == loc->pos.selc.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.selc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.selc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.selc.utc;
	}

	// Update extra information
	pos_extra(loc);

	// Update time
	loc->pos.sci.utc = loc->utc;

	// Update pass
	loc->pos.sci.pass = loc->att.selc.pass = loc->pos.selc.pass;

	// Apply first order transform to all
	loc->pos.sci.s = rv_mmult(loc->pos.extra.s2j,loc->pos.selc.s);
	loc->pos.sci.v = rv_mmult(loc->pos.extra.s2j,loc->pos.selc.v);
	loc->pos.sci.a = rv_mmult(loc->pos.extra.s2j,loc->pos.selc.a);

	// Apply second order correction due to first derivative of rotation matrix
	m1 = rm_mmult(loc->pos.extra.t2j,loc->pos.extra.ds2t);
	v2 = rv_mmult(m1,loc->pos.selc.s);
	loc->pos.sci.v = rv_add(loc->pos.selc.v,v2);

	m1 = rm_smult(2.,m1);
	v2 = rv_mmult(m1,loc->pos.sci.v);
	loc->pos.sci.a = rv_add(loc->pos.selc.a,v2);

	// Convert SCI Attitude to ICRF
	att_selc2icrf(loc);
}

//! Convert SELC to SELG
/*! Propogate the position found in the Selenocentric slot of the supplied ::posstruc to
 * the Selenographic slot, performing all relevant updates.
	\param pos Working ::posstruc
*/
void pos_selc2selg(locstruc *loc)
{
	double xvx, yvy, r2, r, minir, minir2;

	// Synchroniz time
	if (0. == loc->pos.selc.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.selc.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.selc.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.selc.utc;
	}

	// Update extra information
	pos_extra(loc);

	// Update time
	loc->pos.selg.utc = loc->utc;

	// Update pass
	loc->pos.selg.pass = loc->pos.selc.pass;

	// Convert Geocentric Cartesian to Spherical
	minir2 = loc->pos.selc.s.col[0]*loc->pos.selc.s.col[0]+loc->pos.selc.s.col[1]*loc->pos.selc.s.col[1];
	minir = fixprecision(sqrt(minir2),.1);
	r2 = minir2+loc->pos.selc.s.col[2]*loc->pos.selc.s.col[2];
	r = fixprecision(sqrt(r2),.1);
	loc->pos.selg.s.lat = asin(loc->pos.selc.s.col[2]/r);
	loc->pos.selg.s.lon = atan2(loc->pos.selc.s.col[1],loc->pos.selc.s.col[0]);
	loc->pos.selg.s.h = r - (RMOONM);

	xvx = loc->pos.selc.s.col[0]*loc->pos.selc.v.col[0];
	yvy = loc->pos.selc.s.col[1]*loc->pos.selc.v.col[1];
	loc->pos.selg.v.h = (xvx+yvy+loc->pos.selc.s.col[2]*loc->pos.selc.v.col[2])/r;
	loc->pos.selg.v.lat = (-(xvx+yvy) * loc->pos.selc.s.col[2] + minir2*loc->pos.selc.v.col[2])/(r2*minir);
	loc->pos.selg.v.lon = (loc->pos.selc.s.col[0]*loc->pos.selc.v.col[1]+loc->pos.selc.s.col[1]*loc->pos.selc.v.col[0])/minir2;

	// Indicate we have set SELG position
	loc->pos.selg.s.lat = fixprecision(loc->pos.selg.s.lat,.1/r);
	loc->pos.selg.s.lon = fixprecision(loc->pos.selg.s.lon,.1/r);
}

void pos_selg2selc(locstruc *loc)
{
	double sp, cp, sl, cl, cpr, r;

	// Synchroniz time
	if (0. == loc->pos.selg.utc)
	{
		if (!isfinite(loc->utc))
		{
			return;
		}
		loc->pos.selg.utc = loc->pos.utc = loc->utc;
	}
	else
	{
		if (!isfinite(loc->pos.selg.utc))
		{
			return;
		}
		loc->utc = loc->pos.utc = loc->pos.selg.utc;
	}

	// Update extra information
	pos_extra(loc);

	// Update time
	loc->pos.selc.utc = loc->utc;

	// Update pass
	loc->pos.selc.pass = loc->pos.selg.pass;

	r = loc->pos.selg.s.h + RMOONM;

	sp = sin(loc->pos.selg.s.lat);
	cp = cos(loc->pos.selg.s.lat);
	sl = sin(loc->pos.selg.s.lon);
	cl = cos(loc->pos.selg.s.lon);
	cpr = cp * r;

	loc->pos.selc.s = loc->pos.selc.v = loc->pos.selc.a = rv_zero();

	loc->pos.selc.s.col[0] = cpr * cl;
	loc->pos.selc.s.col[1] = cpr * sl;
	loc->pos.selc.s.col[2] = r * sp;

	loc->pos.selc.v.col[0] = loc->pos.selg.v.h * cp * cl - loc->pos.selg.v.lat * r * sp * cl - loc->pos.selg.v.lon * cpr * sl;
	loc->pos.selc.v.col[1] = loc->pos.selg.v.h * cp * sl - loc->pos.selg.v.lat * r * sp * sl + loc->pos.selg.v.lon * cpr * cl;
	loc->pos.selc.v.col[2] = loc->pos.selg.v.h * sp + loc->pos.selg.v.lat * cpr;

}

double rearth(double lat)
{
	double st,ct;
	double c;

	st = sin(lat);
	ct = cos(lat);
	c = sqrt(((FRATIO2 * FRATIO2 * st * st) + (ct * ct))/((ct * ct) + (FRATIO2 * st * st)));
	return (REARTHM * c);
}

//! Calculate Extra attitude information
/*! Calculate things like conversion matrix for ICRF to Body and Body to
 * ICRF.
	\param loc ::locstruc with the current location and those to be updated.
*/
void att_extra(locstruc *loc)
{
	if (loc->att.extra.utc == loc->att.icrf.utc)
		return;

	loc->att.extra.b2j = rm_quaternion2dcm(loc->att.icrf.s);
	loc->att.extra.j2b = rm_transpose(loc->att.extra.b2j);
	loc->att.extra.utc = loc->att.icrf.utc;
}

void att_icrf2geoc(locstruc *loc)
{
	//	rmatrix fpm = {{{{0.}}}}, dpm = {{{{0.}}}};
	rvector alpha;
	double radius;

	// Update extra
	pos_extra(loc);
	att_extra(loc);

	// Update time
	loc->att.geoc.utc = loc->att.icrf.utc = loc->utc;

	// Update pass
	loc->att.geoc.pass = loc->att.icrf.pass;

	// Use to rotate ECI into ITRS
	loc->att.geoc.s = q_mult(q_dcm2quaternion_rm(loc->pos.extra.j2e),loc->att.icrf.s);
	q_normalize(&loc->att.geoc.s);
	loc->att.geoc.v = rv_mmult(loc->pos.extra.j2e,loc->att.icrf.v);
	loc->att.geoc.a = rv_mmult(loc->pos.extra.j2e,loc->att.icrf.a);

	// Correct velocity for ECI angular velocity wrt ITRS, expressed in ITRS
	radius = length_rv(loc->pos.eci.s);

	alpha = rv_smult(1./(radius*radius),rv_cross(rv_mmult(loc->pos.extra.j2e,loc->pos.eci.s),rv_mmult(loc->pos.extra.dj2e,loc->pos.eci.s)));
	loc->att.geoc.v = rv_add(loc->att.geoc.v,alpha);

	alpha = rv_smult(2./(radius*radius),rv_cross(rv_mmult(loc->pos.extra.j2e,loc->pos.eci.s),rv_mmult(loc->pos.extra.dj2e,loc->pos.eci.v)));
	loc->att.geoc.a = rv_add(loc->att.geoc.a,alpha);

	// Convert ITRF attitude to Topocentric
	att_planec2topo(loc);

	// Convert ITRF attitude to LVLH
	att_planec2lvlh(loc);
}

void att_geoc2icrf(locstruc *loc)
{
	//	rmatrix fpm = {{{{0.}}}}, dpm = {{{{0.}}}};
	rvector alpha;
	double radius;

	// Propagate time
	loc->att.icrf.utc = loc->att.geoc.utc = loc->utc;

	// Update pass
	loc->att.icrf.pass = loc->att.geoc.pass;

	// Update extra
	att_extra(loc);

	// Perform first order rotation of ITRS frame into ECI frame
	loc->att.icrf.s = q_mult(q_dcm2quaternion_rm(loc->pos.extra.e2j),loc->att.geoc.s);
	q_normalize(&loc->att.icrf.s);
	loc->att.icrf.v = rv_mmult(loc->pos.extra.e2j,loc->att.geoc.v);
	loc->att.icrf.a = rv_mmult(loc->pos.extra.e2j,loc->att.geoc.a);

	// Correct for ITRS angular velocity wrt ECI, expressed in ECI
	radius = length_rv(loc->pos.geoc.s);

	alpha = rv_smult(1./(radius*radius),rv_cross(rv_mmult(loc->pos.extra.e2j,loc->pos.geoc.s),rv_mmult(loc->pos.extra.de2j,loc->pos.geoc.s)));
	loc->att.icrf.v = rv_add(loc->att.icrf.v,alpha);

	alpha = rv_smult(2./(radius*radius),rv_cross(rv_mmult(loc->pos.extra.e2j,loc->pos.geoc.s),rv_mmult(loc->pos.extra.de2j,loc->pos.geoc.v)));
	loc->att.icrf.a = rv_add(loc->att.icrf.a,alpha);

	// Extra attitude information
	att_extra(loc);
}

void att_geoc(locstruc *loc)
{
	if (loc->att.geoc.pass > loc->att.topo.pass)
	{
		att_planec2topo(loc);
		att_topo(loc);
	}
	if (loc->att.geoc.pass > loc->att.lvlh.pass)
	{
		att_planec2lvlh(loc);
		att_lvlh(loc);
	}
	if (loc->att.geoc.pass > loc->att.icrf.pass)
	{
		att_geoc2icrf(loc);
		att_icrf(loc);
	}
}

void att_icrf2selc(locstruc *loc)
{
	rmatrix dpm = {{{{0.}}}};
	rvector alpha;
	double radius;

	// Propagate time
	loc->att.icrf.utc = loc->att.selc.utc = loc->utc;

	// Update pass
	loc->att.selc.pass = loc->att.icrf.pass;

	att_extra(loc);
	pos_extra(loc);

	// Use to rotate ICRF into SELC
	loc->att.selc.s = q_mult(q_dcm2quaternion_rm(loc->pos.extra.j2s),loc->att.icrf.s);
	q_normalize(&loc->att.selc.s);
	loc->att.selc.v = rv_mmult(loc->pos.extra.j2s,loc->att.icrf.v);
	loc->att.selc.a = rv_mmult(loc->pos.extra.j2s,loc->att.icrf.a);

	// Correct velocity for ECI angular velocity wrt ITRS, expressed in ITRS
	radius = length_rv(loc->pos.eci.s);

	dpm = rm_zero();

	alpha = rv_smult(1./(radius*radius),rv_cross(rv_mmult(loc->pos.extra.j2s,loc->pos.eci.s),rv_mmult(dpm,loc->pos.eci.s)));
	loc->att.selc.v = rv_add(loc->att.selc.v,alpha);

	alpha = rv_smult(2./(radius*radius),rv_cross(rv_mmult(loc->pos.extra.j2s,loc->pos.eci.s),rv_mmult(dpm,loc->pos.eci.v)));
	loc->att.selc.a = rv_add(loc->att.selc.a,alpha);

	// Synchronize LVLH
	att_planec2lvlh(loc);

	// Synchronize Topo
	att_planec2topo(loc);
}

void att_selc2icrf(locstruc *loc)
{
	//	rmatrix fpm = {{{{0.}}}};

	// Propagate time
	loc->att.icrf.utc = loc->att.selc.utc = loc->utc;

	// Update pass
	loc->att.icrf.pass = loc->att.selc.pass;

	att_extra(loc);

	// Calculate rotation matrix to J2000
	//	fpm = loc->pos.extra.s2j;

	// Perform first order rotation of SELC frame into ICRF frame
	loc->att.icrf.s = q_mult(q_dcm2quaternion_rm(loc->pos.extra.s2j),loc->att.selc.s);
	q_normalize(&loc->att.icrf.s);
	loc->att.icrf.v = rv_mmult(loc->pos.extra.s2j,loc->att.selc.v);
	loc->att.icrf.a = rv_mmult(loc->pos.extra.s2j,loc->att.selc.a);

	// Extra attitude information
	att_extra(loc);
}

void att_selc(locstruc *loc)
{
	if (loc->att.selc.pass > loc->att.topo.pass)
	{
		att_planec2topo(loc);
		att_topo(loc);
	}
	if (loc->att.selc.pass > loc->att.lvlh.pass)
	{
		att_planec2lvlh(loc);
		att_lvlh(loc);
	}
	if (loc->att.selc.pass > loc->att.icrf.pass)
	{
		att_selc2icrf(loc);
		att_icrf(loc);
	}
}

void att_icrf2lvlh(locstruc *loc)
{
	att_icrf2geoc(loc);
	att_icrf2selc(loc);
	att_planec2lvlh(loc);
}

void att_icrf(locstruc *loc)
{
	if (loc->att.icrf.pass > loc->att.geoc.pass)
	{
		att_icrf2geoc(loc);
		att_geoc(loc);
	}
	
	if (loc->att.icrf.pass > loc->att.selc.pass)
	{
		att_icrf2selc(loc);
		att_selc(loc);
	}
}

//! Convert ITRS attitude to LVLH attitude
/*! Calculate the rotation quaternion for taking a vector from LVLH to
 * Body given the similar quaternion for ITRS
	\param loc Location structure to propagate the changes in
*/
void att_planec2lvlh(locstruc *loc)
{
	quaternion qe_z = {{0.,0.,0.},1.}, qe_y = {{0.,0.,0.},1.}, fqe = {{0.,0.,0.},1.}, rqe = {{0.,0.,0.},1.};
	rvector lvlh_z = {{0.,0.,1.}}, lvlh_y = {{0.,1.,0.}}, geoc_z = {{0.}}, geoc_y = {{0.}}, alpha = {{0.}};
	qatt *patt;
	cartpos *ppos;
	double radius;

	switch (loc->pos.extra.closest)
	{
	case COSMOS_EARTH:
	default:
		patt = &loc->att.geoc;
		ppos = &loc->pos.geoc;
		break;
	case COSMOS_MOON:
		patt = &loc->att.selc;
		ppos = &loc->pos.selc;
		break;
	}

	radius = length_rv(ppos->s);

	// Update time
	loc->att.lvlh.utc = patt->utc = loc->utc;

	// Update pass
	loc->att.lvlh.pass = patt->pass;

	att_extra(loc);

	// LVLH Z is opposite of direction to satellite
	geoc_z = rv_smult(-1.,ppos->s);
	normalize_rv(&geoc_z);

	// LVLH Y is Cross Product of LVLH Z and velocity vector
	geoc_y = rv_cross(geoc_z,ppos->v);
	normalize_rv(&geoc_y);

	// Determine rotation of ITRF Z  into LVLH Z
	qe_z = q_conjugate(q_change_between_rv(geoc_z,lvlh_z));

	// Use to transform ITRF Y into intermediate Y
	//	geoc_y = rotate_q(qe_z,geoc_y);
	geoc_y = transform_q(qe_z,geoc_y);

	// Determine transformation of this intermediate Y into LVLH Y
	qe_y = q_conjugate(q_change_between_rv(geoc_y,lvlh_y));

	// Combine to determine transformation of ITRF into LVLH
	fqe = q_mult(qe_z,qe_y);
	q_normalize(&fqe);
	rqe = q_conjugate(fqe);

	// Correct velocity for LVLH angular velocity wrt ITRS, expressed in ITRS
	alpha = rv_smult(1./(radius*radius),rv_cross(ppos->s,ppos->v));
	loc->att.lvlh.v = rv_sub(patt->v,alpha);

	// Transform ITRS into LVLH
	//	loc->att.lvlh.s = q_mult(patt->s,fqe);
	//	loc->att.lvlh.v = transform_q(fqe,loc->att.lvlh.v);
	//	loc->att.lvlh.a = transform_q(fqe,patt->a);
	loc->att.lvlh.s = q_mult(rqe,patt->s);
	loc->att.lvlh.v = transform_q(fqe,loc->att.lvlh.v);
	loc->att.lvlh.a = transform_q(fqe,patt->a);

}

//! Convert LVLH attitude to ITRS attitude
/*! Calculate the rotation quaternion for taking a vector from ITRS to
 * Body given the similar quaternion for LVLH
	\param loc Location structure to propagate the changes in
*/
void att_lvlh2planec(locstruc *loc)
{
	quaternion qe_z = {{0.,0.,0.},1.}, qe_y = {{0.,0.,0.},1.}, fqe = {{0.,0.,0.},1.}, rqe = {{0.,0.,0.},1.};
	rvector lvlh_z = {{0.,0.,1.}}, lvlh_y = {{0.,1.,0.}}, geoc_z = {{0.}}, geoc_y = {{0.}}, alpha = {{0.}};
	qatt *patt;
	cartpos *ppos;
	double radius;

	switch (loc->pos.extra.closest)
	{
	case COSMOS_EARTH:
	default:
		patt = &loc->att.geoc;
		ppos = &loc->pos.geoc;
		break;
	case COSMOS_MOON:
		patt = &loc->att.selc;
		ppos = &loc->pos.selc;
		break;
	}
	radius = length_rv(ppos->s);


	// Update time
	loc->att.lvlh.utc = patt->utc = loc->utc;

	// Update pass
	ppos->pass = patt->pass = loc->att.lvlh.pass;

	att_extra(loc);

	// LVLH Z is opposite of earth to satellite vector
	geoc_z = rv_smult(-1.,ppos->s);
	normalize_rv(&geoc_z);

	// LVLH Y is Cross Product of LVLH Z and velocity vector
	geoc_y = rv_cross(geoc_z,ppos->v);
	normalize_rv(&geoc_y);

	// Determine rotation of ITRF Z  into LVLH Z
	qe_z = q_conjugate(q_change_between_rv(geoc_z,lvlh_z));
	geoc_z = transform_q(qe_z,geoc_z);

	// Use to rotate LVLH Y into intermediate LVLH Y
	//	geoc_y = rotate_q(qe_z,geoc_y);
	geoc_y = transform_q(qe_z,geoc_y);

	// Determine rotation of this LVLH Y into ITRF Y
	qe_y = q_conjugate(q_change_between_rv(geoc_y,lvlh_y));

	// Multiply to determine transformation of ITRF frame into LVLH frame
	fqe = q_mult(qe_z,qe_y);
	q_normalize(&fqe);
	rqe = q_conjugate(fqe);

	// LVLH Z is opposite of earth to satellite vector
	geoc_z = rv_smult(-1.,ppos->s);
	normalize_rv(&geoc_z);

	// LVLH Y is Cross Product of LVLH Z and velocity vector
	geoc_y = rv_cross(geoc_z,ppos->v);
	normalize_rv(&geoc_y);

	// Rotate LVLH frame into ITRS frame
	//	patt->s = q_mult(rqe,loc->att.lvlh.s);
	//	patt->v = transform_q(fqe,loc->att.lvlh.v);
	//	patt->a = transform_q(fqe,loc->att.lvlh.a);
	patt->s = q_mult(fqe,loc->att.lvlh.s);
	patt->v = transform_q(rqe,loc->att.lvlh.v);
	patt->a = transform_q(rqe,loc->att.lvlh.a);

	// Correct velocity for LVLH angular velocity wrt ITRS, expressed in ITRS
	alpha = rv_smult(1./(radius*radius),rv_cross(ppos->s,ppos->v));
	patt->v = rv_add(patt->v,alpha);

	// Synchronize Topo
	att_planec2topo(loc);
}

//! Convert LVLH attitude to ICRF attitude
/*! Calculate the rotation quaternion for taking a vector from ICRF to
 * Body given the similar quaternion for LVLH
	\param loc Location structure to propagate the changes in
*/
void att_lvlh2icrf(locstruc *loc)
{
	pos_extra(loc);
	att_extra(loc);

	att_lvlh2planec(loc);
	switch (loc->pos.extra.closest)
	{
	case COSMOS_EARTH:
	default:
		att_geoc2icrf(loc);
		break;
	case COSMOS_MOON:
		att_selc2icrf(loc);
		break;
	}
}

void att_lvlh(locstruc *loc)
{
	pos_extra(loc);
	att_extra(loc);

	switch (loc->pos.extra.closest)
	{
	case COSMOS_EARTH:
	default:
		if (loc->att.lvlh.pass > loc->att.geoc.pass)
		{
			att_lvlh2planec(loc);
			att_geoc(loc);
		}
		break;
	case COSMOS_MOON:
		if (loc->att.lvlh.pass > loc->att.selc.pass)
		{
			att_lvlh2planec(loc);
			att_selc(loc);
		}
		break;
	}
}

//! Planetocentric to Topo attitude
/*! Calculate the rotation quaternion for taking a vector between Topo
 * and the closest planetocentric system  and update whichever is older.
	\param loc Location structure to update
*/
void att_planec2topo(locstruc *loc)
{
	quaternion t2g, t2g_z, t2g_x, g2t;
	rvector geoc_x={{0.}}, topo_x={{0.}}, alpha;
	qatt *patt;
	cartpos *ppos;

	switch (loc->pos.extra.closest)
	{
	case COSMOS_EARTH:
	default:
		patt = &loc->att.geoc;
		ppos = &loc->pos.geoc;
		break;
	case COSMOS_MOON:
		patt = &loc->att.selc;
		ppos = &loc->pos.selc;
		break;
	}

	// Update time
	loc->att.topo.utc = patt->utc = loc->utc;

	// Update pass
	loc->att.topo.pass = patt->pass;

	att_extra(loc);

	// Determine rotation of Topo unit Z  into ITRS Z
	t2g_z = q_conjugate(q_change_between_rv(rv_unitz(),ppos->s));

	// Use to rotate Topo unit X into intermediate Topo X
	topo_x = transform_q(t2g_z,rv_unitx());

	// Define ITRS unit x as x=-Topo.y and y=Topo.x
	geoc_x.col[0] = -ppos->s.col[1];
	geoc_x.col[1] = ppos->s.col[0];

	// Determine rotation of intermediate Topo X into ITRS unit X
	t2g_x = q_conjugate(q_change_between_rv(topo_x,geoc_x));

	// Multiply to determine rotation of Topo frame into ITRS frame
	t2g = q_mult(t2g_z,t2g_x);
	q_normalize(&t2g);
	g2t = q_conjugate(t2g);

	// Correct velocity for Topo angular velocity wrt ITRS, expressed in ITRS
	alpha = rv_smult(-1./(length_rv(ppos->s)*length_rv(ppos->s)),rv_cross(ppos->s,ppos->v));
	loc->att.topo.v = rv_add(patt->v, alpha);

	// Rotate ITRS frame into Topo frame
	loc->att.topo.s = q_mult(g2t, patt->s);
	loc->att.topo.v = transform_q(t2g,loc->att.topo.v);
	loc->att.topo.a = transform_q(t2g,patt->a);

}

//! Topocentric to Planetocentric attitude
/*! Calculate the rotation quaternion for taking a vector between Topo
 * and the closest planetocentric system  and update whichever is older.
	\param loc Location structure to update
*/
void att_topo2planec(locstruc *loc)
{
	quaternion t2g, t2g_z, t2g_x, g2t;
	rvector geoc_x={{0.}}, topo_x={{0.}}, alpha;
	qatt *patt;
	cartpos *ppos;

	switch (loc->pos.extra.closest)
	{
	case COSMOS_EARTH:
	default:
		patt = &loc->att.geoc;
		ppos = &loc->pos.geoc;
		break;
	case COSMOS_MOON:
		patt = &loc->att.selc;
		ppos = &loc->pos.selc;
		break;
	}

	// Update time
	patt->utc = loc->att.topo.utc = loc->utc;

	// Update pass
	ppos->pass = patt->pass = loc->att.topo.pass;

	att_extra(loc);

	// Determine rotation of Topo unit Z  into ITRS Z
	t2g_z = q_conjugate(q_change_between_rv(rv_unitz(),ppos->s));

	// Use to rotate Topo unit X into intermediate Topo X
	topo_x = transform_q(t2g_z,rv_unitx());

	// Define ITRS unit x as x=-Topo.y and y=Topo.x
	geoc_x.col[0] = -ppos->s.col[1];
	geoc_x.col[1] = ppos->s.col[0];

	// Determine rotation of intermediate Topo X into ITRS unit X
	t2g_x = q_conjugate(q_change_between_rv(topo_x,geoc_x));

	// Multiply to determine rotation of Topo frame into ITRS frame
	t2g = q_mult(t2g_z,t2g_x);
	q_normalize(&t2g);
	g2t = q_conjugate(t2g);

	// Rotate Topo frame into ITRS frame
	patt->s = q_mult(loc->att.topo.s,t2g);
	patt->v = transform_q(g2t,loc->att.topo.v);
	patt->a = transform_q(g2t,loc->att.topo.a);

	// Correct velocity for Topo angular velocity wrt ITRS, expressed in ITRS
	alpha = rv_smult(-1./(length_rv(ppos->s)*length_rv(ppos->s)),rv_cross(ppos->s,ppos->v));

	// Correct for rotation of frame
	patt->v = rv_add(loc->att.topo.v,alpha);

	switch (loc->pos.extra.closest)
	{
	case COSMOS_EARTH:
	default:
		att_geoc2icrf(loc);
		break;
	case COSMOS_MOON:
		att_selc2icrf(loc);
		break;
	}
	att_planec2lvlh(loc);
}

void att_topo(locstruc *loc)
{
	pos_extra(loc);
	att_extra(loc);

	switch (loc->pos.extra.closest)
	{
	case COSMOS_EARTH:
	default:
		if (loc->att.topo.pass > loc->att.geoc.pass)
		{
			att_topo2planec(loc);
			att_geoc(loc);
		}
		break;
	case COSMOS_MOON:
		if (loc->att.topo.pass > loc->att.selc.pass)
		{
			att_topo2planec(loc);
			att_selc(loc);
		}
		break;
	}
}

//! Synchronize all frames in location structure.
/*! Work through provided location structure, first identifying the
frame that is most up to date, then updating all other frames to
match.
	\param loc ::locstruc to be synchronized
*/
void loc_update(locstruc *loc)
{
	uint32_t ppass=0, apass = 0;
	int32_t ptype=-1, atype = -1;

	if (loc->att.icrf.pass > apass)
	{
		apass = loc->att.icrf.pass;
		atype = JSON_TYPE_QATT_ICRF;
	}
	if (loc->att.geoc.pass > apass)
	{
		apass = loc->att.geoc.pass;
		atype = JSON_TYPE_QATT_GEOC;
	}
	if (loc->att.selc.pass > apass)
	{
		apass = loc->att.selc.pass;
		atype = JSON_TYPE_QATT_SELC;
	}
	if (loc->att.lvlh.pass > apass)
	{
		apass = loc->att.lvlh.pass;
		atype = JSON_TYPE_QATT_LVLH;
	}
	if (loc->att.topo.pass > apass)
	{
		apass = loc->att.topo.pass;
		atype = JSON_TYPE_QATT_TOPO;
	}
	if (loc->pos.baryc.pass > ppass)
	{
		ppass = loc->pos.baryc.pass;
		ptype = JSON_TYPE_POS_BARYC;
	}
	if (loc->pos.eci.pass > ppass)
	{
		ppass = loc->pos.eci.pass;
		ptype = JSON_TYPE_POS_ECI;
	}
	if (loc->pos.sci.pass > ppass)
	{
		ppass = loc->pos.sci.pass;
		ptype = JSON_TYPE_POS_SCI;
	}
	if (loc->pos.geoc.pass > ppass)
	{
		ppass = loc->pos.geoc.pass;
		ptype = JSON_TYPE_POS_GEOC;
	}
	if (loc->pos.selc.pass > ppass)
	{
		ppass = loc->pos.selc.pass;
		ptype = JSON_TYPE_POS_SELC;
	}
	if (loc->pos.geod.pass > ppass)
	{
		ppass = loc->pos.geod.pass;
		ptype = JSON_TYPE_POS_GEOD;
	}
	if (loc->pos.geos.pass > ppass)
	{
		ppass = loc->pos.geos.pass;
		ptype = JSON_TYPE_POS_GEOS;
	}
	if (loc->pos.selg.pass > ppass)
	{
		ppass = loc->pos.selg.pass;
		ptype = JSON_TYPE_POS_SELG;
	}

	switch (ptype)
	{
	case JSON_TYPE_POS_BARYC:
		pos_baryc(loc);
		break;
	case JSON_TYPE_POS_ECI:
		pos_eci(loc);
		break;
	case JSON_TYPE_POS_SCI:
		pos_sci(loc);
		break;
	case JSON_TYPE_POS_GEOC:
		pos_geoc(loc);
		break;
	case JSON_TYPE_POS_SELC:
		pos_selc(loc);
		break;
	case JSON_TYPE_POS_GEOD:
		pos_geod(loc);
		break;
	case JSON_TYPE_POS_GEOS:
		pos_geos(loc);
		break;
	case JSON_TYPE_POS_SELG:
		pos_selg(loc);
		break;
	}

	switch (atype)
	{
	case JSON_TYPE_QATT_ICRF:
		att_icrf(loc);
		break;
	case JSON_TYPE_QATT_GEOC:
		att_geoc(loc);
		break;
	case JSON_TYPE_QATT_SELC:
		att_selc(loc);
		break;
	case JSON_TYPE_QATT_LVLH:
		att_lvlh(loc);
		break;
	case JSON_TYPE_QATT_TOPO:
		att_topo(loc);
		break;
	}

}

//! Rotate Mean of Epoch to True of Epoch
/*! Calculate the rotation matrix for converting coordinates from a system based on the
 * Mean orientation for that Epoch to one based on the True orientation. Adds effects of
 * Nutation.
	\param ep0 Epoch, in units of Modified Julian Day
	\param pm Rotation matrix
*/
void mean2true(double ep0, rmatrix *pm)
{
	static rmatrix opm;
	static double oep0 = 0.;

	if (ep0 == oep0)
	{
		*pm = opm;
		return;
	}

	true2mean(ep0, pm);
	*pm = rm_transpose(*pm);

	oep0 = ep0;
	opm = *pm;
}

//! Rotate True of Epoch to Mean of Epoch
/*! Calculate the rotation matrix for converting coordinates from a system based on the
 * True orientation for that Epoch to one based on the Mean orientation. Removes effects
 * of Nutation.
	\param ep0 Epoch, in units of Modified Julian Day
	\param pm Rotation matrix
*/
void true2mean(double ep0, rmatrix *pm)
{
	static rmatrix opm;
	static double oep0 = 0.;
	double nuts[4], jt;
	double eps;
	double cdp, sdp, ce, se, cde, sde;

	if (ep0 == oep0)
	{
		*pm = opm;
		return;
	}

	jplnut(utc2tt(ep0),nuts);
	jt = (ep0-51544.5)/36525.;
	eps = DAS2R*(84381.448+jt*(-46.84024+jt*(-.00059+jt*.001813)));

	cdp = cos(nuts[0]);
	sdp = sin(nuts[0]);
	ce = cos(eps);
	se = sin(eps);
	cde = cos(eps+nuts[1]);
	sde = sin(eps+nuts[1]);

	pm->row[0].col[0] = cdp;
	pm->row[0].col[1] = -ce*sdp;
	pm->row[0].col[2] = -se*sdp;
	pm->row[1].col[0] = cde*sdp;
	pm->row[1].col[1] = ce*cde*cdp + se*sde;
	pm->row[1].col[2] = se*cde*cdp - ce*sde;
	pm->row[2].col[0] = sde*sdp;
	pm->row[2].col[1] = ce*sde*sdp - se*sde;
	pm->row[2].col[2] = se*sde*cdp + ce*cde;
	oep0 = ep0;
	opm = *pm;
}

//! Rotate Mean of Epoch to J2000
/*! Calculate the rotation matrix for converting coordinates from a system based on the
 * Mean of the indicated Epoch, to one based on J2000. Reflects the effects of
 * precession.
	\param ep0 Epoch, in units of Modified Julian Day
	\param pm Rotation matrix
*/
void mean2icrs(double ep0, rmatrix *pm)
{
	double t0, t, tas2r, w, zeta, z, theta;
	double ca, sa, cb, sb, cg, sg;
	static rmatrix opm;
	static double oep0 = 0.;

	if (ep0 == oep0)
	{
		*pm = opm;
		return;
	}

	/* Interval between basic epoch J2000.0 and beginning epoch (JC) */
	//t0 = ( ep0 - 2000.0 ) / 100.0;
	t0 = (ep0 - 51544.5) / 36525.;

	/* Interval over which precession required (JC) */
	t =  -t0;

	/* Euler angles */
	tas2r = t * DAS2R;
	w = 2306.2181 + ( ( 1.39656 - ( 0.000139 * t0 ) ) * t0 );
	zeta = (w + ( ( 0.30188 - 0.000344 * t0 ) + 0.017998 * t ) * t ) * tas2r;
	z = (w + ( ( 1.09468 + 0.000066 * t0 ) + 0.018203 * t ) * t ) * tas2r;
	theta = ((2004.3109 + (-0.85330 - 0.000217 * t0) * t0)+ ((-0.42665 - 0.000217 * t0) - 0.041833 * t) * t) * tas2r;

	ca = cos(zeta);
	sa = -sin(zeta);
	cb = cos(theta);
	sb = sin(theta);
	cg = cos(z);
	sg = -sin(z);

	pm->row[0].col[0] = ca*cb*cg - sa*sg;
	pm->row[0].col[1] = cg*sa*cb +ca*sg;
	pm->row[0].col[2] = -sb*cg;
	pm->row[1].col[0] = -ca*cb*sg - sa*cg;
	pm->row[1].col[1] = -sa*cb*sg + ca*cg;
	pm->row[1].col[2] = sg*sb;
	pm->row[2].col[0] = ca*sb;
	pm->row[2].col[1] = sa*sb;
	pm->row[2].col[2] = cb;
	opm = *pm;
}

//! ITRS to J2000 rotation matrix
/*! Rotation matrix for transformation from an ITRS coordinate system of date to the
 * J2000 system. Includes all effects of Precession, Nutation, Sidereal Time and Polar Motion.
	\param utc Epoch to change from, UTC in MJD
	\param rm pointer to rotation matrix
	\param drm pointer to rotation matrix derivative
	\param ddrm pointer to rotation matrix 2nd derivative
*/
void itrs2icrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm)
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
		icrs2itrs(utc,rnp,rm,drm,ddrm);
		ornp = *rnp = rm_transpose(*rnp);
		orm = *rm = rm_transpose(*rm);
		odrm = *drm = rm_transpose(*drm);
		oddrm = *drm = rm_transpose(*ddrm);
		outc = utc;
	}
}

//! J2000 to ITRS rotation matrix
/*! Rotation matrix for transformation from a J2000 coordinate system to the
 * International Terrestrial Reference System of date. Includes all effects of Precession,
 * Nutation, Sidereal Time and Polar Motion.
	\param utc Epoch to change to, UTC in MJD
	\param rm pointer to rotation matrix
	\param drm pointer to rotation matrix derivative
*/
void icrs2itrs(double utc, rmatrix *rnp, rmatrix *rm, rmatrix *drm, rmatrix *ddrm)
{
	double gast, ut1, ttc;
	rvector nuts;
	double eps, pols, dpsi, deps;
	double zeta, z, theta;
	double s1, s2, s3, c1, c2, c3;
	rmatrix nrm[3], ndrm, nddrm;
	rmatrix pm, nm, sm, pw = {{{{0.}}}}, dsm = {{{{0.}}}};
	//static rmatrix bm = {{{{1.,-14.6*DAS2R,16.617*DAS2R,0.}},{{14.6*DAS2R,1.,6.8192*DAS2R}},{{-16.617*DAS2R,-6.8192*DAS2R,1.}},{{0.}}}};
	static rmatrix bm = {{{{9.99999999999994E-01,-7.07836896097156E-08,8.05621397761319E-08}},{{7.07836869463768E-08,9.99999999999997E-01,3.30594373543214E-08}},{{-8.05621421162006E-08,-3.30594316921839E-08,9.99999999999996E-01}}}};
	cvector polm= {0.,0.,0.};
	static rmatrix orm, odrm, oddrm, ornp;
	static double outc = 0.;
	static double realsec = 0.;
	int i;

	if (!isfinite(utc))
	{
		return;
	}

	if (utc == outc)
	{
		*rm = orm;
		*drm = odrm;
		*ddrm = oddrm;
		*rnp = ornp;
		return;
	}

	// Set size of actual second if first time in
	if (realsec == 0.)
	{
		ut1 = utc + 1./86400.;
		realsec = ut1 - utc;
	}

	// Do it 3 times to get rate change
	outc = utc;
	utc -= realsec;
	for (i=0; i<3; i++)
	{
		/* Interval over which precession required (JC) */
		ut1 = utc2ut1(utc);
		//		tt = utc2tt(utc);
		ttc = utc2jcen(utc);

		/* Euler angles */
		zeta = (2.650545 + ttc*(2306.083227 + ttc*(0.2988499 + ttc*(0.01801828 + ttc*(-0.000005971)))))*DAS2R;
		z = (-2.650545 + ttc*(2306.077181 + ttc*(1.0927348 + ttc*(0.01826837 + ttc*(-0.000028596)))))*DAS2R;
		theta = ttc*(2004.191903 + ttc*(-0.4294934 + ttc*(-0.04182264 + ttc*(-0.000007089 + ttc*(-0.0000001274)))))*DAS2R;

		pm = rm_mmult(rm_change_around_z(zeta),rm_mmult(rm_change_around_y(-theta),rm_change_around_z(z)));

		nuts = utc2nuts(utc);
		dpsi = -nuts.col[0];
		deps = -nuts.col[1];
		eps = utc2epsilon(utc);

		s1 = sin(eps);
		s2 = sin(-dpsi);
		s3 = sin(-eps-deps);
		c1 = cos(eps);
		c2 = cos(-dpsi);
		c3 = cos(-eps-deps);

		//nm = rm_mmult(rm_change_around_x(-eps),rm_mmult(rm_change_around_z(dpsi),rm_change_around_x(eps+deps)));
		nm = rm_zero();
		nm.row[0].col[0] = c2;
		nm.row[1].col[0] = s2*c1;
		nm.row[2].col[0] = s2*s1;

		nm.row[0].col[1] = -s2*c3;
		nm.row[1].col[1] = c3*c2*c1-s1*s3;
		nm.row[2].col[1] = c3*c2*s1+c1*s3;;

		nm.row[0].col[2] = s2*s3;
		nm.row[1].col[2] = -s3*c2*c1-s1*c3;
		nm.row[2].col[2] = -s3*c2*s1+c3*c1;

		gast = utc2gast(utc);
		sm = rm_change_around_z(-gast);

		dsm = rm_zero();
		dsm.row[1].col[0] = cos(gast);
		dsm.row[0].col[1] = -dsm.row[1].col[0];
		dsm.row[0].col[0] = dsm.row[1].col[1] = -sin(gast);
		dsm = rm_smult(-.000072921158553,dsm);

		polm = polar_motion(utc);
		pols = -47. * 4.848136811095359935899141e-12 * ttc;

		pw = rm_mmult(rm_change_around_z(-pols),rm_mmult(rm_change_around_y(polm.x),rm_change_around_x(polm.y)));

		nrm[i] = rm_mmult(nm,rm_mmult(pm,bm));
		if (i==1)
			*rnp = nrm[i];

		nrm[i] = rm_mmult(sm,nrm[i]);
		nrm[i] = rm_mmult(pw,nrm[i]);
		utc += realsec;
	}

	nddrm = rm_sub(rm_sub(nrm[2],nrm[1]),rm_sub(nrm[1],nrm[0]));
	ndrm = rm_smult(.5,rm_sub(nrm[2],nrm[0]));
	orm = *rm = (rm_quaternion2dcm(q_dcm2quaternion_rm(nrm[1])));
	odrm = *drm = ndrm;
	oddrm = *ddrm = nddrm;
	ornp = *rnp;
}

/*! Rotation matrix for transformation from a J2000 coordinate system to one based on
 * the Mean of the Epoch.
	\param ep1 Epoch to change to, UTC in MJD
	\param pm pointer to rotation matrix
*/
void icrs2mean(double ep1, rmatrix *pm)
{
	double t, tas2r, w, zeta, z, theta;
	double ca, sa, cb, sb, cg, sg;
	static rmatrix opm;
	static double oep1 = 0.;

	if (ep1 == oep1)
	{
		*pm = opm;
		return;
	}

	/* Interval over which precession required (JC) */
	t = (ep1 - 51544.5) / 36525.;

	/* Euler angles */
	tas2r = t * DAS2R;
	w = 2306.2181;
	zeta = (w + ( ( 0.30188) + 0.017998 * t ) * t ) * tas2r;
	z = (w + ( ( 1.09468) + 0.018203 * t ) * t ) * tas2r;
	theta = ((2004.3109)+ ((-0.42665) - 0.041833 * t) * t) * tas2r;

	ca = cos(zeta);
	sa = -sin(zeta);
	cb = cos(theta);
	sb = sin(theta);
	cg = cos(z);
	sg = -sin(z);

	pm->row[0].col[0] = ca*cb*cg - sa*sg;
	pm->row[0].col[1] = cg*sa*cb +ca*sg;
	pm->row[0].col[2] = -sb*cg;
	pm->row[1].col[0] = -ca*cb*sg - sa*cg;
	pm->row[1].col[1] = -sa*cb*sg + ca*cg;
	pm->row[1].col[2] = sg*sb;
	pm->row[2].col[0] = ca*sb;
	pm->row[2].col[1] = sa*sb;
	pm->row[2].col[2] = cb;
	opm = *pm;
	oep1 = ep1;
}

void mean2mean(double ep0, double ep1, rmatrix *pm)
{
	double t0, t, tas2r, w, zeta, z, theta;
	double ca, sa, cb, sb, cg, sg;

	/* Interval between basic epoch J2000.0 and beginning epoch (JC) */
	//t0 = ( ep0 - 2000.0 ) / 100.0;
	t0 = (ep0 - 51544.5) / 36525.;

	/* Interval over which precession required (JC) */
	//t =  ( ep1 - ep0 ) / 100.0;
	t = (ep1 - ep0) / 36525.;

	/* Euler angles */
	tas2r = t * DAS2R;
	w = 2306.2181 + ( ( 1.39656 - ( 0.000139 * t0 ) ) * t0 );
	zeta = (w + ( ( 0.30188 - 0.000344 * t0 ) + 0.017998 * t ) * t ) * tas2r;
	z = (w + ( ( 1.09468 + 0.000066 * t0 ) + 0.018203 * t ) * t ) * tas2r;
	theta = ((2004.3109 + (-0.85330 - 0.000217 * t0) * t0)+ ((-0.42665 - 0.000217 * t0) - 0.041833 * t) * t) * tas2r;

	ca = cos(zeta);
	sa = -sin(zeta);
	cb = cos(theta);
	sb = sin(theta);
	cg = cos(z);
	sg = -sin(z);

	pm->row[0].col[0] = ca*cb*cg - sa*sg;
	pm->row[0].col[1] = cg*sa*cb +ca*sg;
	pm->row[0].col[2] = -sb*cg;
	pm->row[1].col[0] = -ca*cb*sg - sa*cg;
	pm->row[1].col[1] = -sa*cb*sg + ca*cg;
	pm->row[1].col[2] = sg*sb;
	pm->row[2].col[0] = ca*sb;
	pm->row[2].col[1] = sa*sb;
	pm->row[2].col[2] = cb;

}

void kep2eci(kepstruc *kep, cartpos *eci)
{
	rvector qpos = {{0.}}, qvel = {{0.}};
	double s1, s2, s3, c1, c2, c3;
	double xx, xy, xz, yx, yy, yz, zx, zy, zz;
	double sea, cea, sqe;

	sea = sin(kep->ea);
	cea = cos(kep->ea);
	sqe = sqrt(1. - kep->e * kep->e);

	qpos.col[0] = kep->a * (cea - kep->e);
	qpos.col[1] = kep->a * sqe * sea;
	qpos.col[2] = 0.;

	kep->mm = sqrt(GM/pow(kep->a,3.));
	qvel.col[0] = -kep->mm * kep->a * sea / (1. - kep->e*cea);
	qvel.col[1] = kep->mm * kep->a * sqe * cea / (1. - kep->e*cea);
	qvel.col[2] = 0.;

	s1 = sin(kep->raan);
	c1 = cos(kep->raan);
	s2 = sin(kep->i);
	c2 = cos(kep->i);
	s3 = sin(kep->ap);
	c3 = cos(kep->ap);

	xx = c1*c3 - s1*c2*s3;
	xy = -c1*s3 - s1*c2*c3;
	xz = s1*s2;

	yx = s1*c3 + c1*c2*s3;
	yy = -s1*s3 + c1*c2*c3;
	yz = -c1*s2;

	zx = s2*s3;
	zy = s2*c3;
	zz = c2;

	eci->s = eci->v = eci->a = rv_zero();

	eci->s.col[0] = qpos.col[0] * xx + qpos.col[1] * xy + qpos.col[2] * xz;
	eci->s.col[1] = qpos.col[0] * yx + qpos.col[1] * yy + qpos.col[2] * yz;
	eci->s.col[2] = qpos.col[0] * zx + qpos.col[1] * zy + qpos.col[2] * zz;

	eci->v.col[0] = qvel.col[0] * xx + qvel.col[1] * xy + qvel.col[2] * xz;
	eci->v.col[1] = qvel.col[0] * yx + qvel.col[1] * yy + qvel.col[2] * yz;
	eci->v.col[2] = qvel.col[0] * zx + qvel.col[1] * zy + qvel.col[2] * zz;

	eci->utc = kep->utc;
}

void eci2kep(cartpos *eci,kepstruc *kep)
{
	double magr, magv, magn, sme, rdotv, temp;
	double c1, hk, magh;
	rvector nbar = {{0.}}, ebar = {{0.}}, rsun = {{0.}}, hsat = {{0.}};
	cartpos earthpos;

	kep->utc = eci->utc;

	magr = length_rv(eci->s);
	if (magr < D_SMALL)
		magr = D_SMALL;
	magv = length_rv(eci->v);
	if (magv < D_SMALL)
		magv = D_SMALL;
	kep->h = rv_cross(eci->s,eci->v);
	if (magr == D_SMALL && magv == D_SMALL)
		kep->fa = acos(1./D_SMALL);
	else
		kep->fa = acos(length_rv(kep->h)/(magr*magv));
	kep->eta = magv*magv/2. - GM/magr;
	magh = length_rv(kep->h);
	jplpos(JPL_EARTH,JPL_SUN_BARY,utc2tt(eci->utc),&earthpos);
	rsun = earthpos.s;
	normalize_rv(&rsun);
	hsat = kep->h;
	normalize_rv(&hsat);
	kep->beta = asin(dot_rv(rsun,hsat));

	if (magh > O_SMALL)
	{
		// ------------  find a e and semi-latus rectum   ----------
		nbar.col[0] = -kep->h.col[1];
		nbar.col[1] = kep->h.col[0];
		nbar.col[2] = 0.0;
		magn = length_rv(nbar);
		c1 = magv * magv - GM / magr;
		rdotv = dot_rv(eci->s,eci->v);
		ebar.col[0] = (c1 * eci->s.col[0] - rdotv * eci->v.col[0]) / GM;
		ebar.col[1] = (c1 * eci->s.col[1] - rdotv * eci->v.col[1]) / GM;
		ebar.col[2] = (c1 * eci->s.col[2] - rdotv * eci->v.col[2]) / GM;
		kep->e = length_rv(ebar);

		sme = (magv * magv * 0.5) - (GM / magr);
		if (fabs(sme) > O_SMALL)
			kep->a = -GM / (2. * sme);
		else
			kep->a = O_INFINITE;
		//	p = magh * magh / GM;

		// find mean motion and period
		kep->mm = sqrt(GM/pow(kep->a,3.));
		kep->period = 2. * DPI / kep->mm;

		// -----------------  find inclination   -------------------
		hk = kep->h.col[2] / magh;
		kep->i = acos(hk);

		// ----------  find longitude of ascending node ------------
		if (magn > O_SMALL)
		{
			temp = nbar.col[0] / magn;
			if (fabs(temp) > 1.)
				temp = temp<1.?-1.:(temp>1.?1.:0.);
			kep->raan = acos(temp);
			if (nbar.col[1] < 0.)
				kep->raan = D2PI - kep->raan;
		}
		else
			kep->raan = O_UNDEFINED;

		// Find eccentric and mean anomaly
		kep->ea = atan2(rdotv/sqrt(kep->a*GM),1.-magr/kep->a);
		kep->ma = kep->ea - kep->e*sin(kep->ea);

		// Find argument of latitude
		kep->alat = atan2(eci->s.col[2],(eci->s.col[1]*kep->h.col[0]-eci->s.col[0]*kep->h.col[1])/magh);

		// Find true anomaly
		kep->ta = atan2(sqrt(1-kep->e*kep->e)*sin(kep->ea),cos(kep->ea)-kep->e);

		// Find argument of perigee
		kep->ap = kep->alat - kep->ta;
	}
	else
	{
		kep->a = kep->e = kep->i = kep->raan = O_UNDEFINED;
		kep->ap = kep->alat = kep->ma = kep->ta = kep->ea = kep->mm = O_UNDEFINED;
	}
}

//! Geocentric to Topocentric
/*! Calculate the Topocentric position of a Target with respect to a Source.
 * \param source Geodetic location of Source.
 * \param targetgeoc Geocentric location of Target.
 * \param topo Resulting Topocentric position.
 */
void geoc2topo(gvector source, rvector targetgeoc, rvector *topo)
{
	rmatrix g2t = {{{{0.}}}};
	double clat, clon, slat, slon;
	double lst, r, c, s;
	double cs, ct, ss, st;
	rvector sourcegeoc = {{0.}};

	clon = cos(source.lon);
	slon = sin(source.lon);
	clat = cos(source.lat);
	slat = sin(source.lat);

	g2t.row[0].col[0] = -slon;
	g2t.row[0].col[1] = clon;
	g2t.row[0].col[2] = 0.;
	g2t.row[1].col[0] = -slat*clon;
	g2t.row[1].col[1] = -slat*slon;
	g2t.row[1].col[2] = clat;
	g2t.row[2].col[0] = clat*clon;
	g2t.row[2].col[1] = clat*slon;
	g2t.row[2].col[2] = slat;

	ct = cos(source.lat);
	st = sin(source.lat);
	c = 1./sqrt(ct * ct + FRATIO2 * st * st);
	s = FRATIO2 * c;
	r = (REARTHM * c + source.h) * ct;
	sourcegeoc.col[2] = (REARTHM * s + source.h) * st;

	lst = source.lon;
	cs = cos(lst);
	ss = sin(lst);
	sourcegeoc.col[0] = r * cs;
	sourcegeoc.col[1] = r * ss;

	*topo = rv_mmult(g2t,rv_sub(targetgeoc,sourcegeoc));
}

// ??
void topo2azel(rvector tpos, float *az, float *el)
{
	*az = (float)(atan2(tpos.col[0],tpos.col[1]));
	*el = (float)(atan2(tpos.col[2],sqrt(tpos.col[0]*tpos.col[0]+tpos.col[1]*tpos.col[1])));
}

//! Return position from TLE set
/*! Find the TLE closest to, but not exceeding the provided utc, then
 * return the position in ECI coordinates.
	\param utc Coordinated Universal Time in Modified Julian Days.
	\param lines Vector of TLE's.
	\param eci Pointer to ::cartpos in ECI frame.
	*/
int lines2eci(double utc, vector<tlestruc>lines, cartpos *eci)
{
	static uint16_t lindex=0;
	int32_t iretn;

	if (utc >= lines[lindex].utc)
	{
		for (uint16_t i=lindex; i<lines.size(); i++)
		{
			if (utc >= lines[i].utc)
			{
				lindex = i;
				break;
			}
		}
	}
	else
		return (TLE_ERROR_OUTOFRANGE);

	iretn = tle2eci(utc, lines[lindex], eci);
	return (iretn);
}

/**
* Convert a Two Line Element into a location at the specified time.
* @param utc Specified time as Modified Julian Date
* @param line Two Line Element, given as pointer to a ::tlestruc
* @param eci Converted location, given as pointer to a ::cartpos
*/
int tle2eci(double utc, tlestruc tle, cartpos *eci)
{
	//	rmatrix pm = {{{{0.}}}};
	//	static int lsnumber=-99;
	static double c1=0.;
	static double cosio=0. ,x3thm1=0. , xnodp=0. ,aodp=0.,isimp=0.,eta=0.,sinio=0. ,ximth2=0.,c4=0.,c5=0.;
	static double xmdot=0.,omgdot=0., xnodot=0.,omgcof=0.,xmcof=0., xnodcf=0.,t2cof=0.,xlcof=0.,aycof=0.;
	int i;
	double temp, temp1, temp2, temp3, temp4, temp5, temp6;
	double tempa, tempe, templ;
	double ao, a1, c2, c3, coef, coef1, theta4, c1sq;
	double theta2, betao2, betao, delo, del1, s4, qoms24, x1m5th, xhdot1;
	double perige, eosq, pinvsq, tsi, etasq, eeta, psisq, g, xmdf;
	double tsince, omgadf, alpha, xnoddf, xmp, tsq, xnode, delomg, delm;
	double tcube, tfour, a, e, xl, beta, axn, xn, xll, ayn, capu, aynl;
	double xlt, sinepw, cosepw, epw, ecose, esine,  pl, r, elsq;
	double rdot, rfdot, cosu, sinu, u, sin2u, cos2u, uk, rk, ux, uy, uz;
	double vx, vy, vz, xinck, rdotk, rfdotk, sinuk, cosuk, sinik, cosik, xnodek;
	double xmx, xmy, sinnok, cosnok;
	//	locstruc loc;
	static double lutc=0.;
	static uint16_t lsnumber=0;

	static double delmo,sinmo,x7thm1,d2,d3,d4,t3cof,t4cof,t5cof, betal;

	if (tle.utc != lutc || tle.snumber != lsnumber)
	{
		// RECOVER ORIGINAL MEAN MOTION ( xnodp ) AND SEMIMAJOR AXIS (aodp)
		// FROM INPUT ELEMENTS
		a1=pow((SGP4_XKE/ tle.mm ),SGP4_TOTHRD);
		cosio = cos(tle.i);
		theta2=cosio * cosio;
		x3thm1 =3.*theta2-1.;
		eosq = tle.e * tle.e;
		betao2=1.- eosq;
		betao=sqrt(betao2);
		del1=1.5*SGP4_CK2*x3thm1 /(a1*a1*betao*betao2);
		ao=a1*(1.-del1*(.5*SGP4_TOTHRD+del1*(1.+134./81.*del1)));
		delo=1.5*SGP4_CK2*x3thm1 /(ao*ao*betao*betao2);
		xnodp = tle.mm /(1.+delo);
		aodp=ao/(1.-delo);
		// INITIALIZATION
		// FOR PERIGEE LESS THAN 220 KILOMETERS, THE isimp FLAG IS SET AND
		// THE EQUATIONS ARE TRUNCATED TO LINEAR VARIATION IN sqrt A AND
		// QUADRATIC VARIATION IN MEAN ANOMALY. ALSO, THE c3 TERM, THE
		// DELTA alpha TERM, AND THE DELTA M TERM ARE DROPPED.
		isimp=0;
		if((aodp*(1.- tle.e)/SGP4_AE) < (220./SGP4_XKMPER+SGP4_AE))
			isimp=1;
		// FOR PERIGEE BELOW 156 KM, THE VALUES OF
		// S AND SGP4_QOMS2T ARE ALTERED
		s4=SGP4_S;
		qoms24=SGP4_QOMS2T;
		perige=(aodp*(1.- tle.e )-SGP4_AE)*SGP4_XKMPER;
		if(perige < 156.)
		{
			s4=perige-78.;
			if(perige <= 98.)
			{
				s4=20.;
				qoms24=pow(((120.-s4)*SGP4_AE/SGP4_XKMPER),4.);
				s4=s4/SGP4_XKMPER+SGP4_AE;
			}
		}
		pinvsq = 1./(aodp*aodp*betao2*betao2);
		tsi =1./(aodp-s4);
		eta=aodp* tle.e * tsi;
		etasq=eta*eta;
		eeta= tle.e *eta;
		psisq=fabs(1.-etasq);
		coef=qoms24*pow(tsi,4.);
		coef1=coef/pow(psisq,3.5);
		c2=coef1* xnodp *(aodp*(1.+1.5*etasq+eeta*(4.+etasq))+.75* SGP4_CK2*tsi/psisq*x3thm1 *(8.+3.*etasq*(8.+etasq)));
		c1 = tle.bstar *c2;
		sinio =sin( tle.i );
		g =-SGP4_XJ3/SGP4_CK2*pow(SGP4_AE,3.);
		c3 =coef*tsi*g* xnodp *SGP4_AE*sinio / tle.e;
		ximth2 =1.-theta2;
		c4 =2.* xnodp *coef1*aodp*betao2*(eta* (2.+.5*etasq)+ tle.e *(.5+2.*etasq)-2.*SGP4_CK2*tsi/ (aodp*psisq)*(-3.*x3thm1 *(1.-2.*eeta+etasq* (1.5-.5*eeta))+.75*ximth2*(2.*etasq-eeta* (1.+etasq))*cos(2.* tle.ap )));
		c5 =2.*coef1*aodp*betao2*(1.+2.75*(etasq+eeta)+eeta*etasq);
		theta4 =theta2*theta2;
		temp1 =3.*SGP4_CK2*pinvsq* xnodp;
		temp2 = temp1*SGP4_CK2*pinvsq;
		temp3 =1.25*SGP4_CK4*pinvsq*pinvsq* xnodp;
		xmdot = xnodp +.5* temp1*betao*x3thm1 +.0625* temp2*betao* (13.-78.*theta2+137.*theta4);
		x1m5th =1.-5.*theta2;
		omgdot =-.5* temp1*x1m5th+.0625* temp2*(7.-114.*theta2+ 395.*theta4)+ temp3*(3.-36.*theta2+49.*theta4);
		xhdot1 =- temp1*cosio;
		xnodot =xhdot1+(.5* temp2*(4.-19.*theta2)+2.* temp3*(3.- 7.*theta2))*cosio;
		omgcof = tle.bstar *c3*cos( tle.ap );
		xmcof =-SGP4_TOTHRD*coef* tle.bstar *SGP4_AE/eeta;
		xnodcf =3.5*betao2*xhdot1*c1;
		t2cof =1.5*c1;
		xlcof =.125*g*sinio *(3.+5.*cosio )/(1.+cosio );
		aycof =.25*g*sinio;
		delmo =pow((1.+eta*cos( tle.ma )),3.);
		sinmo =sin( tle.ma );
		x7thm1 =7.*theta2-1.;
		if(isimp != 1)
		{
			c1sq=c1*c1;
			d2=4.*aodp*tsi*c1sq;
			temp =d2*tsi*c1/3.;
			d3=(17.*aodp+s4)* temp;
			d4=.5* temp *aodp*tsi*(221.*aodp+31.*s4)*c1;
			t3cof=d2+2.*c1sq;
			t4cof=.25*(3.*d3+c1*(12.*d2+10.*c1sq));
			t5cof=.2*(3.*d4+12.*c1*d3+6.*d2*d2+15.*c1sq*( 2.*d2+c1sq));
		}
		lsnumber = tle.snumber;
		lutc = tle.utc;
	}

	// UPDATE FOR SECULAR GRAVITY AND ATMOSPHERIC DRAG
	tsince = (utc - tle.utc) * 1440.;
	xmdf = tle.ma +xmdot*tsince;
	omgadf= tle.ap +omgdot*tsince;
	xnoddf= tle.raan + xnodot*tsince;
	alpha=omgadf;
	xmp = xmdf;
	tsq=tsince*tsince;
	xnode= xnoddf+ xnodcf*tsq;
	tempa=1.-c1*tsince;
	tempe= tle.bstar *c4*tsince;
	templ=t2cof*tsq;
	if(isimp != 1)
	{
		delomg=omgcof*tsince;
		delm=xmcof*(pow((1.+eta*cos( xmdf )),3.)-delmo);
		temp =delomg+delm;
		xmp = xmdf + temp;
		alpha=omgadf- temp;
		tcube=tsq*tsince;
		tfour=tsince*tcube;
		tempa = tempa-d2*tsq-d3*tcube-d4*tfour;
		tempe = tempe+ tle.bstar *c5*(sin( xmp )-sinmo);
		templ = templ+t3cof*tcube+ tfour*(t4cof+tsince*t5cof);
	}
	a =aodp* tempa * tempa;
	e = tle.e - tempe;
	xl= xmp +alpha+ xnode+ xnodp * templ;
	beta=sqrt(1.-e*e);
	xn=SGP4_XKE/pow(a,1.5);
	// LONG PERIOD PERIODICS
	axn=e*cos(alpha);
	temp =1./(a*beta*beta);
	xll= temp *xlcof*axn;
	aynl= temp *aycof;
	xlt=xl+xll;
	ayn=e*sin(alpha)+aynl;
	// SOLVE KEplERS EQUATION;
	capu=ranrm(xlt- xnode);
	temp2=capu;
	for (i=1; i<=10; i++)
	{
		sinepw=sin( temp2);
		cosepw=cos( temp2);
		temp3=axn*sinepw;
		temp4=ayn*cosepw;
		temp5=axn*cosepw;
		temp6=ayn*sinepw;
		epw=(capu- temp4+ temp3- temp2)/(1.- temp5- temp6)+ temp2;
		if(fabs(epw- temp2) <= SGP4_E6A)
			break;
		temp2=epw;
	}
	// SHORT PERIOD PRELIMINARY QUANTITIES;
	ecose= temp5+ temp6;
	esine= temp3- temp4;
	elsq=axn*axn+ayn*ayn;
	temp =1.-elsq;
	pl=a* temp;
	r=a*(1.-ecose);
	temp1=1./r;
	rdot=SGP4_XKE*sqrt(a)*esine* temp1;
	rfdot=SGP4_XKE*sqrt(pl)* temp1;
	temp2=a* temp1;
	betal=sqrt( temp );
	temp3=1./(1.+betal);
	cosu = temp2*(cosepw-axn+ayn*esine* temp3);
	sinu= temp2*(sinepw-ayn-axn*esine* temp3);
	u=actan(sinu, cosu );
	sin2u=2.*sinu* cosu;
	cos2u =2.* cosu * cosu -1.;
	temp =1./pl;
	temp1=SGP4_CK2* temp;
	temp2= temp1* temp;
	// UPDATE FOR SHORT PERIODICS;
	rk =r*(1.-1.5* temp2*betal*x3thm1 )+.5* temp1*ximth2* cos2u;
	uk=u-.25* temp2*x7thm1*sin2u;
	xnodek= xnode+1.5* temp2*cosio *sin2u;
	xinck= tle.i +1.5* temp2*cosio *sinio * cos2u;
	rdotk=rdot-xn* temp1*ximth2*sin2u;
	rfdotk=rfdot+xn* temp1*(ximth2* cos2u +1.5*x3thm1 );
	// ORIENTATION VECTORS;
	sinuk =sin(uk);
	cosuk=cos(uk);
	sinik =sin(xinck);
	cosik =cos(xinck);
	sinnok=sin( xnodek);
	cosnok=cos( xnodek);
	xmx=-sinnok* cosik;
	xmy=cosnok* cosik;
	ux=xmx* sinuk +cosnok* cosuk;
	uy=xmy* sinuk +sinnok* cosuk;
	uz= sinik * sinuk;
	vx=xmx* cosuk-cosnok* sinuk;
	vy=xmy* cosuk-sinnok* sinuk;
	vz= sinik * cosuk;
	// POSITION AND VELOCITY;
	eci->s = eci->v = eci->a = rv_zero();

	eci->s.col[0] = REARTHM * rk *ux;
	eci->s.col[1] = REARTHM * rk *uy;
	eci->s.col[2] = REARTHM * rk *uz;
	eci->v.col[0] =REARTHM * (rdotk*ux+rfdotk*vx) / 60.;
	eci->v.col[1] =REARTHM * (rdotk*uy+rfdotk*vy) / 60.;
	eci->v.col[2] =REARTHM * (rdotk*uz+rfdotk*vz) / 60.;

	/*
	mean2icrs(utc,&pm);
	eci->s = rv_mmult(pm,eci->s);
	eci->v = rv_mmult(pm,eci->v);
	*/
	eci->utc = utc;

	/*
	loc.utc = loc.pos.utc = loc.pos.geoc.utc = utc;
	loc.pos.geoc.s = loc.pos.geoc.v = loc.pos.geoc.a = rv_zero();

	loc.pos.geoc.s.col[0] = REARTHM * rk *ux;
	loc.pos.geoc.s.col[1] = REARTHM * rk *uy;
	loc.pos.geoc.s.col[2] = REARTHM * rk *uz;
	loc.pos.geoc.v.col[0] =REARTHM * (rdotk*ux+rfdotk*vx) / 60.;
	loc.pos.geoc.v.col[1] =REARTHM * (rdotk*uy+rfdotk*vy) / 60.;
	loc.pos.geoc.v.col[2] =REARTHM * (rdotk*uz+rfdotk*vz) / 60.;

	pos_geoc2eci(&loc);
	*eci = loc.pos.eci;
	*/

	return 0;
}

//! Get TLE from array of TLE's
/*! Return the indexed entry from an array of ::tlestruc. If the
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

//! Load TLE from file.
/*!
* Load Two Line Element file into array of TLE's
* \param fname Name of file containing elements
* \param lines Array of ::tlestruc structures to contain elements
* \return A ::int32_t indicating number of elements, otherwise a negative error.
*/
int32_t load_lines(char *fname, vector<tlestruc>& lines)
{
	FILE *fdes;
	uint16_t year;
	double jday;
	int32_t bdragm, bdrage, ecc;
	char ibuf[81], tlename[81];
	int i;
	tlestruc tle;

	if ((fdes=fopen(fname,"r")) == NULL)
		return (-1);

	tlecount = 0;

	// Name Line
	char* ichar = fgets(tlename,80,fdes);
	if (ichar == NULL || feof(fdes))
		return (-1);

	for (i=strlen(tlename)-1; i>0; i--)
	{
		if (tlename[i]!=' ' && tlename[i]!='\r' && tlename[i]!='\n')
			break;
	}
	tlename[i+1] = 0;

	while (!feof(fdes))
	{
		strcpy(tle.name,tlename);

		// Line 1
		if (fgets(ibuf,80,fdes) == NULL)
			break;
		sscanf(&ibuf[2],"%5hu",&tle.snumber);
		sscanf(&ibuf[9],"%6s",tle.id);
		sscanf(&ibuf[18],"%2hu",&year);
		if (year < 57)
			year += 2000;
		else
			year += 1900;
		sscanf(&ibuf[20],"%12lf",&jday);
		tle.utc = cal2mjd((int)year,1,0.);
		tle.utc += jday;
		if (strlen(ibuf) > 50)
		{
			sscanf(&ibuf[53],"%6d%2d",&bdragm,&bdrage);
			tle.bstar = pow(10.,bdrage)*bdragm/1.e5;
		}
		else
			tle.bstar = 0.;

		// Line 2
		char* ichar = fgets(ibuf,80,fdes);
		if (ichar != NULL)
		{
			ibuf[68] = 0;
			sscanf(&ibuf[8],"%8lf %8lf %7d %8lf %8lf %11lf%5u",&tle.i,&tle.raan,&ecc,&tle.ap,&tle.ma,&tle.mm,&tle.orbit);
			tle.i = RADOF(tle.i);
			tle.raan = RADOF(tle.raan);
			tle.ap = RADOF(tle.ap);
			tle.ma = RADOF(tle.ma);
			tle.mm *= D2PI/1440.;
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
	\return The number of entries in the table, otherwise a negative error.
*/
int32_t load_stk(char *filename, stkstruc *stkdata)
{
	FILE *fdes;
	int32_t maxcount;
	int32_t iretn;
	cposstruc *tpos;
	char ibuf[250];

	if ((fdes=fopen(filename,"r")) == NULL)
		return (STK_ERROR_NOTFOUND);

	maxcount = 1000;
	stkdata->pos = (cposstruc *)calloc(maxcount,sizeof(cposstruc));
	stkdata->count = 0;
	while (!feof(fdes))
	{
		char* ichar = fgets(ibuf,250,fdes);
		if (ichar == NULL || feof(fdes))
			break;
		if ((iretn=sscanf(ibuf,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",&stkdata->pos[stkdata->count].pos.utc,&stkdata->pos[stkdata->count].pos.s.col[0],&stkdata->pos[stkdata->count].pos.s.col[1],&stkdata->pos[stkdata->count].pos.s.col[2],&stkdata->pos[stkdata->count].pos.v.col[0],&stkdata->pos[stkdata->count].pos.v.col[1],&stkdata->pos[stkdata->count].pos.v.col[2],&stkdata->pos[stkdata->count].pos.a.col[0],&stkdata->pos[stkdata->count].pos.a.col[1],&stkdata->pos[stkdata->count].pos.a.col[2])) == 10)
		{
			stkdata->pos[stkdata->count].utc = stkdata->pos[stkdata->count].pos.utc;
			stkdata->count++;
			if (!(stkdata->count%1000))
			{
				maxcount += 1000;
				tpos = stkdata->pos;
				stkdata->pos = (cposstruc *)calloc(maxcount,sizeof(cposstruc));
				memcpy(stkdata->pos,tpos,(maxcount-1000)*sizeof(cposstruc));
				free(tpos);
			}
		}
		else
			iretn = 0;
	}
	fclose(fdes);
	if (stkdata->count)
		stkdata->dt = ((stkdata->pos[9].utc - stkdata->pos[0].utc))/9.;
	return (stkdata->count);
}

/**
* Convert a set of STK data into a location at the specified time.
* @param utc Specified time as Modified Julian Date
* @param line Two Line Element, given as pointer to a ::tlestruc
* @param eci Converted locition, given as pointer to a ::cartpos
*/
//! ECI from STK data
/*! Return an interpolated cartpos from time and STK data
	\param utc UTC in Modified Julian Days
	\param stk Structure containing array of STK positions
	\param eci Structure to return position in
	\return 0 if successful, otherwise negative error
*/
int stk2eci(double utc, stkstruc *stk, cartpos *eci)
{
	int32_t index, i, j;
	double findex;
	uvector t, p, su, vu, au;
	rmatrix s, v, a;

	findex = ((utc-stk->pos[0].utc)/stk->dt)+.5;
	if (findex < 1)
	{
		findex = 1.;
	}
	if (findex >= stk->count)
	{
		findex = stk->count - 1;
	}

	findex = (int)findex + ((utc-stk->pos[(int)findex].utc)/stk->dt) + .5;

	index = (int)findex-1;
	if (index < 0)
		return (STK_ERROR_LOWINDEX);

	if (index > stk->count-3)
		return (STK_ERROR_HIGHINDEX);


	for (i=0; i<3; i++)
	{
		t.a4[i] = utc-stk->pos[index+i].utc;
		for (j=0; j<3; j++)
		{
			s.row[j].col[i] = stk->pos[index+i].pos.s.col[j];
			v.row[j].col[i] = stk->pos[index+i].pos.v.col[j];
			a.row[j].col[i] = stk->pos[index+i].pos.a.col[j];
		}
	}

	eci->utc = utc;
	eci->s = eci->v = eci->a = rv_zero();

	for (j=0; j<3; j++)
	{
		su.r = s.row[j];
		p = rv_fitpoly(t,su,2);
		//	eci->s.col[j] = p.a4[0] + utc * (p.a4[1] + utc * p.a4[2]);
		eci->s.col[j] = p.a4[0];
		vu.r = v.row[j];
		p = rv_fitpoly(t,vu,2);
		//	eci->v.col[j] = p.a4[0] + utc * (p.a4[1] + utc * p.a4[2]);
		eci->v.col[j] = p.a4[0];
		au.r = a.row[j];
		p = rv_fitpoly(t,au,2);
		//	eci->a.col[j] = p.a4[0] + utc * (p.a4[1] + utc * p.a4[2]);
		eci->a.col[j] = p.a4[0];
	}

	return 0;
}

std::ostream& operator << (std::ostream& out, const cartpos& a)
{
	out << a.utc << "\t" << a.s << "\t" << a.v << "\t" << a.a << "\t" << a.pass;
	return out;
}

std::istream& operator >> (std::istream& in, cartpos& a)
{
	in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
	return in;
}

std::ostream& operator << (std::ostream& out, const cposstruc& a)
{
	out << a.utc << "\t" << a.pos;
	return out;
}

std::istream& operator >> (std::istream& in, cposstruc& a)
{
	in >> a.utc >> a.pos;
	return in;
}

std::ostream& operator << (std::ostream& out, const geoidpos& a)
{
	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a<<"\t"<<a.pass;
	return out;
}

std::istream& operator >> (std::istream& in, geoidpos& a)
{
	in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
	return in;
}

std::ostream& operator << (std::ostream& out, const spherpos& a)
{
	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a<<"\t"<<a.pass;
	return out;
}

std::istream& operator >> (std::istream& in, spherpos& a)
{
	in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
	return in;
}

std::ostream& operator << (std::ostream& out, const aattstruc& a)
{
	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a;
	return out;
}

std::istream& operator >> (std::istream& in, aattstruc& a)
{
	in >> a.utc >> a.s >> a.v >> a.a;
	return in;
}

std::ostream& operator << (std::ostream& out, const quatatt& a)
{
	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a;
	return out;
}

std::istream& operator >> (std::istream& in, quatatt& a)
{
	in >> a.utc >> a.s >> a.v >> a.a;
	return in;
}

std::ostream& operator << (std::ostream& out, const dcmatt& a)
{
	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a;
	return out;
}

std::istream& operator >> (std::istream& in, dcmatt& a)
{
	in >> a.utc >> a.s >> a.v >> a.a;
	return in;
}

std::ostream& operator << (std::ostream& out, const qatt& a)
{
	out<<a.utc<<"\t"<<a.s<<"\t"<<a.v<<"\t"<<a.a<<"\t"<<a.pass;
	return out;
}

std::istream& operator >> (std::istream& in, qatt& a)
{
	in >> a.utc >> a.s >> a.v >> a.a >> a.pass;
	return in;
}

std::ostream& operator << (std::ostream& out, const kepstruc& a)
{
	out <<a.utc<<"\t"
		<<a.orbit<<"\t"
		<<a.period<<"\t"
		<<a.a<<"\t"
		<<a.e<<"\t"
		<<a.h<<"\t"
		<<a.beta<<"\t"
		<<a.eta<<"\t"
		<<a.i<<"\t"
		<<a.raan<<"\t"
		<<a.ap<<"\t"
		<<a.alat<<"\t"
		<<a.ma<<"\t"
		<<a.ta<<"\t"
		<<a.ea<<"\t"
		<<a.mm<<"\t"
		<<a.fa;
	return out;
}

std::istream& operator >> (std::istream& in, kepstruc& a)
{
	in  >>a.utc
		>>a.orbit
		>>a.period
		>>a.a
		>>a.e
		>>a.h
		>>a.beta
		>>a.eta
		>>a.i
		>>a.raan
		>>a.ap
		>>a.alat
		>>a.ma
		>>a.ta
		>>a.ea
		>>a.mm
		>>a.fa;
	return in;
}

std::ostream& operator << (std::ostream& out, const bodypos& a)
{
	out << a.sepangle << "\t" << a.size << "\t" << a.radiance;
	return out;
}

std::istream& operator << (std::istream& in, bodypos& a)
{
	in >> a.sepangle >> a.size >> a.radiance;
	return in;
}

std::ostream& operator << (std::ostream& out, const extrapos& a)
{
	out <<a.utc<<"\t"
		<<a.tt<<"\t"
		<<a.ut<<"\t"
		<<a.tdb<<"\t"
		<<a.j2e<<"\t"
		<<a.dj2e<<"\t"
		<<a.ddj2e<<"\t"
		<<a.e2j<<"\t"
		<<a.de2j<<"\t"
		<<a.dde2j<<"\t"
		<<a.j2t<<"\t"
		<<a.j2s<<"\t"
		<<a.t2j<<"\t"
		<<a.s2j<<"\t"
		<<a.s2t<<"\t"
		<<a.ds2t<<"\t"
		<<a.t2s<<"\t"
		<<a.dt2s<<"\t"
		<<a.sun2earth<<"\t"
		<<a.sun2moon<<"\t"
		<<a.closest;
	return out;
}

std::istream& operator >> (std::istream& in, extrapos& a)
{
	in >>a.utc
		>>a.tt
		>>a.ut
		>>a.tdb
		>>a.j2e
		>>a.dj2e
		>>a.ddj2e
		>>a.e2j
		>>a.de2j
		>>a.dde2j
		>>a.j2t
		>>a.j2s
		>>a.t2j
		>>a.s2j
		>>a.s2t
		>>a.ds2t
		>>a.t2s
		>>a.dt2s
		>>a.sun2earth
		>>a.sun2moon
		>>a.closest;
	return in;
}

std::ostream& operator << (std::ostream& out, const extraatt& a)
{
	out << a.utc << "\t" << a.j2b << "\t" << a.b2j;
	return out;
}

std::istream& operator >> (std::istream& in, extraatt& a)
{
	in >> a.utc >> a.j2b >> a.b2j;
	return in;
}

std::ostream& operator << (std::ostream& out, const posstruc& a)
{
	out << a.utc << "\t"
		<< a.baryc << "\t"
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

std::istream& operator >> (std::istream& in, posstruc& a)
{
	in  >> a.utc
		>> a.baryc
		>> a.eci
		>> a.sci
		>> a.geoc
		>> a.selc
		>> a.geod
		>> a.selg
		>> a.geos
		>> a.extra
		>> a.earthsep
		>> a.moonsep
		>> a.sunsize
		>> a.sunradiance;
	return in;
}

std::ostream& operator << (std::ostream& out, const attstruc& a)
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

std::istream& operator >> (std::istream& in, attstruc& a)
{
	in  >> a.utc
		>> a.topo
		>> a.lvlh
		>> a.geoc
		>> a.selc
		>> a.icrf
		>> a.extra;
	return in;
}

std::ostream& operator << (std::ostream& out, const locstruc& a)
{
	out << a.utc << "\t"
		<< a.pos << "\t"
		<< a.att << "\t"
		<< a.bearth;
	return out;
}

std::istream& operator >> (std::istream& in, locstruc& a)
{
	in  >> a.utc
		>> a.pos
		>> a.att
		>> a.bearth;
	return in;
}

//! @}
