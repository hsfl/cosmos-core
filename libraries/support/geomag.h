#ifndef GEOMAG_H
#define GEOMAG_H

/*! \file geomag.h
	\brief geomag include file
	A library of routines to calculate the magnetic field of the Earth.
*/

//! \ingroup support
//!	\defgroup geomag Earth geomagnetic field calculation
//! Front end for calculating the World Magnetic Model provided by the National
//! Geophysical Data Center

#include "configCosmos.h"

#include "mathlib.h"

//! \ingroup geomag
//! \defgroup geomag_functions World Magnetic Model support library function declarations
//! @{

int32_t geomag_front(gvector pos, double year, rvector *comp);

//! @}

#endif
