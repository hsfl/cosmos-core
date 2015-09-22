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

#ifndef _MATH_ROTATE_H
#define _MATH_ROTATE_H

#include "math/vector.h"
#include "math/matrix.h"
#include "math/quaternion.h"

// these came from the quaternion lib because they depend on matrix types
cmatrix cm_quaternion2dcm(quaternion q);
quaternion q_dcm2quaternion_cm(cmatrix dcm);
quaternion q_dcm2quaternion_rm(rmatrix m);
// from matrix
rmatrix rm_quaternion2dcm(quaternion q);

//! Orthonormal basis
/*! Used to crease frame basis such as inertial, body and sensor frames
 * The DCM class uses this struct as default
*/
struct basisOrthonormal{
    cvector i; // = {1,0,0}
    cvector j; // = {0,1,0}
    cvector k; // = {0,0,1};
};

// TODO: rename to DirectionCosineMatrix
class DCM {
private:

public:
    cmatrix base2_from_base1(basisOrthonormal base2,basisOrthonormal base1);
    cmatrix base1_from_base2(basisOrthonormal base1, basisOrthonormal base2);

    double dotProduct(cvector a, cvector b);
    cmatrix transposeMatrix(cmatrix a);
};

#endif
