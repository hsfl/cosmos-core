/*! \file ephemlib.c
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
void jpllib(double utc,rmatrix *rm, rmatrix *drm)
{
		double pvec[6];

		if (jplephem == NULL)
		{
				string fname = get_resdir() +"/general/lnx1900.405";
				jplephem = jpl_init_ephemeris(fname.c_str(),NULL,NULL);
		}

		jpl_pleph(jplephem,utc + 2400000.5,15,0,pvec,1);
		*rm = rm_mmult(rm_change_around_z(pvec[0]),rm_mmult(rm_change_around_x(pvec[1]),rm_change_around_z(pvec[2])));
		*drm = rm_mmult(rm_change_around_z(pvec[0]+pvec[3]/86400.),rm_mmult(rm_change_around_x(pvec[1]+pvec[4]/86400.),rm_change_around_z(pvec[2]+pvec[5]/86400.)));
		*drm = rm_sub(*drm,*rm);
}

//! Nutations from JPL Ephemeris
/*! Longitude and obliquity values and rates.
		\param utc Modified julian day of nutations
		\param nuts Storage for nutations: psi, epsilon, dpsi, depsilon
*/
void jplnut(double utc, double nuts[])
{
		double pvec[6];

		if (!isfinite(utc))
		{
				return;
		}

		if (jplephem == NULL)
		{
				string fname = get_resdir() + "/general/lnx1900.405";
				jplephem = jpl_init_ephemeris(fname.c_str(),NULL,NULL);
				if (jplephem == NULL)
				{
						return;
				}
		}

		jpl_pleph(jplephem,utc + 2400000.5,(int)JPL_NUTATIONS,0,pvec,1);
		nuts[0] = pvec[0];
		nuts[1] = pvec[1];
		nuts[2] = pvec[2] / 86400.;
		nuts[3] = pvec[3] / 86400.;

}

//! Position from JPL Ephemeris
/*! Position, velocity and acceleration vectors from one solar system object to another in J2000  coordinates.
		\param from Starting object for vectors.
		\param to Ending object for vectors.
		\param pos Storage from returned vectors
		\param utc Modified julian day of position
*/
void jplpos(long from, long to, double utc, cartpos *pos)
{
		static double pvec[3][6];

		pos->s = pos->v = pos->a = rv_zero();

		if (jplephem == NULL)
		{
				string fname = get_resdir() + "/general/lnx1900.405";
				jplephem = jpl_init_ephemeris(fname.c_str(),NULL,NULL);
		}

		jpl_pleph(jplephem,utc + 2400000.5 - .05/86400.,(int)to,(int)from,pvec[0],1);
		jpl_pleph(jplephem,utc + 2400000.5,(int)to,(int)from,pvec[1],1);
		jpl_pleph(jplephem,utc + 2400000.5 + .05/86400.,(int)to,(int)from,pvec[2],1);
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
}



//! @}
