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

//   TODO:
// * create a new class just for quaternion operations, and other logical groups
//   this will make it much easier to use the separate libraries

/*! \file mathlib.cpp
	\brief mathlib source file
*/
//#include "mathlib.h"

#include "directionCosineMatrix.h"


// this is same function as dot_cv from mathlib.h
// TODO: when mathlib has been breaken appart then use the math module
// that computes th dotProduct
double DCM::dotProduct(cvector a, cvector b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

// this is same function as cm_transpose from mathlib.h
// TODO: when mathlib has been breaken appart then use the math module
// that computes the transpose
cmatrix DCM::transposeMatrix(cmatrix a)
{
    cmatrix b;

    b.r1.x = a.r1.x;
    b.r1.y = a.r2.x;
    b.r1.z = a.r3.x;

    b.r2.x = a.r1.y;
    b.r2.y = a.r2.y;
    b.r2.z = a.r3.y;

    b.r3.x = a.r1.z;
    b.r3.y = a.r2.z;
    b.r3.z = a.r3.z;

    return (b);
}

cmatrix DCM::base2_from_base1(basisOrthonormal base2,basisOrthonormal base1){

// compute dcm matrix (A) to represent vector in base 2 coordinates

// References
// - Quaternion and Rotation Sequences, Kuipers, pg 160 (I think these
//   formulas are wrong in the book! they are inversed, must check)
// - http://people.ae.illinois.edu/tbretl/ae403/handouts/06-dcm.pdf (this
//   reference seems to be sound)
// - http://www.starlino.com/dcm_tutorial.html (eq 1.4)

// example: vector_body_coordinates = dcm_base2_from_base1 * vector_inertial_coodinates

// Notes:
// - matrix A represents a frame rotation that relates the initial reference
//   frame {X,Y,Z} to a rotated frame {x,y,z}: x = AX. Example, appliying
//   the operation AX - where X is for instance a vector in the inertial
//   frame - resutls in the coordinates of that vector in the new frame {x,y,z}
//   in general the vector x is simply the vector X expressed in a new frame coordinates

// Example to run:
// define a base1 (inertial)
// base1.i = [1,0,0];
// base1.j = [0,1,0];
// base1.k = [0,0,1];
// define a frame2 rotated 90 deg aroud z axis (of the inertial)
// base2.i = [0,1,0];
// base2.j = [-1,0,0];
// base2.k = [0,0,1];
// compute the dcm
// dcm_base2_from_base1(base2,base1);
// check if it's right, base1.i vector shoud be now [0,-1,0]
// dcm_base2_from_base1(base2,base1)*base1.i'

// TODOs
// - add validation step to verify if the bases are orthogonal

    return  {
        { dotProduct(base1.i, base2.i) , dotProduct(base1.j, base2.i), dotProduct(base1.k, base2.i) },
        { dotProduct(base1.i, base2.j) , dotProduct(base1.j, base2.j), dotProduct(base1.k, base2.j) },
        { dotProduct(base1.i, base2.k) , dotProduct(base1.j, base2.k), dotProduct(base1.k, base2.k) }
    };
}

cmatrix DCM::base1_from_base2(basisOrthonormal base1, basisOrthonormal base2){
    // compute dcm matrix (A) to represent vector given in base 2
    // in base 1 coordinates

    // References
    // - Quaternion and Rotation Sequences, Kuipers, pg 160
    // - http://people.ae.illinois.edu/tbretl/ae403/handouts/06-dcm.pdf
    // - http://www.starlino.com/dcm_tutorial.html (eq 1.4)

    // example:
    // vector_inertial_coordinates = dcm_base1_from_base2 * vector_body_coodinates

    // TODOs
    // - add validation step to verify if the bases are orthogonal

    // just compute the transpose
    return transposeMatrix(base2_from_base1(base2,base1));
}


//! @}
