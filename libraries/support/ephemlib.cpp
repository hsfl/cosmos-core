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

/*! \file ephemlib.cpp
		\brief Ephemeris library source file
*/

#include "ephemlib.h"
#include "jpleph.h"
#include "datalib.h"

#include <cmath>

static void *jplephem = NULL;

//! \addtogroup ephemlib_functions
//! @{

//! Librations from JPL Ephemeris
/*! Position and Velocity values for Lunar Libration from the JPL Ephemeris
		\param utc Modified julian day of position
		\param rm Pointer to the rotation matrix that represents Libration motion from
		Selenocentric (Principal Axis) to ICRF.
		\param drm Pointer to the rotation matrix that represents derivative Libration motion
*/
int32_t jpllib(double utc,rmatrix *rm, rmatrix *drm)
{
	double pvec[6];
	int32_t iretn;

	iretn = jplopen();
	if (iretn < 0)
	{
		return iretn;
	}

	iretn = jpl_pleph(jplephem,utc + 2400000.5,15,0,pvec,1);
	if (iretn < 0)
	{
		return iretn;
	}

	*rm = rm_mmult(rm_change_around_z(pvec[0]),rm_mmult(rm_change_around_x(pvec[1]),rm_change_around_z(pvec[2])));
	*drm = rm_mmult(rm_change_around_z(pvec[0]+pvec[3]/86400.),rm_mmult(rm_change_around_x(pvec[1]+pvec[4]/86400.),rm_change_around_z(pvec[2]+pvec[5]/86400.)));
	*drm = rm_sub(*drm,*rm);

	return 0;
}

//! Nutations from JPL Ephemeris
/*! Longitude and obliquity values and rates.
		\param utc Modified julian day of nutations
		\param nuts Storage for nutations: psi, epsilon, dpsi, depsilon
*/
int32_t jplnut(double utc, double nuts[])
{
	double pvec[6];

	if (!std::isfinite(utc))
	{
		return JPLEPHEM_ERROR_OUTOFRANGE;
	}

	int32_t iretn = jplopen();
	if (iretn < 0)
	{
		return iretn;
	}

	iretn = jpl_pleph(jplephem,utc + 2400000.5,(int)JPL_NUTATIONS,0,pvec,1);
	if (iretn < 0)
	{
		return iretn;
	}

	nuts[0] = pvec[0];
	nuts[1] = pvec[1];
	nuts[2] = pvec[2] / 86400.;
	nuts[3] = pvec[3] / 86400.;

	return 0;
}

//! Position from JPL Ephemeris
/*! Position, velocity and acceleration vectors from one solar system object to another in J2000  coordinates.
		\param from Starting object for vectors.
		\param to Ending object for vectors.
		\param pos Storage from returned vectors
		\param utc Modified julian day of position
*/
int32_t jplpos(long from, long to, double utc, cartpos *pos)
{
	static double pvec[3][6];

	pos->s = pos->v = pos->a = rv_zero();

	int32_t iretn = jplopen();
	if (iretn < 0)
	{
		return iretn;
	}

	iretn = jpl_pleph(jplephem,utc + 2400000.5 - .05/86400.,(int)to,(int)from,pvec[0],1);
	if (iretn < 0)
	{
		return iretn;
	}

	iretn = jpl_pleph(jplephem,utc + 2400000.5,(int)to,(int)from,pvec[1],1);
	if (iretn < 0)
	{
		return iretn;
	}

	iretn = jpl_pleph(jplephem,utc + 2400000.5 + .05/86400.,(int)to,(int)from,pvec[2],1);
	if (iretn < 0)
	{
		return iretn;
	}

	pos->s.col[0] = pvec[1][0] * 1000.;
	pos->s.col[1] = pvec[1][1] * 1000.;
	pos->s.col[2] = pvec[1][2] * 1000.;
	pos->v.col[0] = pvec[1][3] * 1000.;
	pos->v.col[1] = pvec[1][4] * 1000.;
	pos->v.col[2] = pvec[1][5] * 1000.;
	pos->a.col[0] = (pvec[2][3] - pvec[0][3]) * 10000.;
	pos->a.col[1] = (pvec[2][4] - pvec[0][4]) * 10000.;
	pos->a.col[2] = (pvec[2][5] - pvec[0][5]) * 10000.;
	pos->utc = utc;

	return 0;
}

int32_t jplopen()
{
	if (jplephem == NULL)
	{
		std::string fname;
		int32_t iretn = get_cosmosresources(fname);
		if (iretn < 0)
		{
			return iretn;
		}
		fname +=  "/general/lnx1900.405";
		jplephem = jpl_init_ephemeris(fname.c_str(),NULL,NULL);
		if (jplephem == nullptr)
		{
			return -errno;
		}
	}
	return 0;
}

//! @}
