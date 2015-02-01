#ifndef EPHEMLIB_H
#define EPHEMLIB_H

#endif // EPHEMLIB_H


/*! \file ephemlib.h
        \brief ephemlib include file
        A library providing functions for conversion between the various
        position and attitude coordinate systems used in orbital mechanics.
*/

#include "mathlib.h"
#include "convertdef.h"

//! \ingroup support
//! \defgroup ephemlib Ephemeris library
//! Ephemeris support library.

//! \ingroup ephemlib
//! \defgroup ephemlib_functions Ephemeris functions
//! @{

int32_t jplnut(double mjd, double nuts[]);
int32_t jplpos(long from, long to, double mjd, cartpos *pos);
int32_t jpllib(double utc,rmatrix *rm, rmatrix *drm);
int32_t jplopen();

//! @}
