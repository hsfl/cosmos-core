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

void jplnut(double mjd, double nuts[]);
void jplpos(long from, long to, double mjd, cartpos *pos);
void jpllib(double utc,rmatrix *rm, rmatrix *drm);

//! @}
