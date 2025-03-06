#ifndef CONTROLLIB_H
#define CONTROLLIB_H

/*! \file controllib.h
    \brief controllib include file
    A library of functions to be used in support of Attitude control.
*/

//! \ingroup support
//! \defgroup controllib Attitude Control Library
//! \brief Attitude Control support library.
//! Various functions for the calculation of both torques and hardware settings necessary to
//! maintain a given attitude.

#include "support/configCosmos.h"
#include "physics/physicslib.h"
#include "support/datalib.h"
#include "math/mathlib.h"
#include "support/convertdef.h"

//! \ingroup controllib
//! \defgroup controllib_functions Attitude Control Library functions
//! @{

rvector calc_control_torque(double lag, Convert::qatt tatt, Convert::qatt catt, rvector moi);

//! @}

#endif // CONTROLLIB_H
