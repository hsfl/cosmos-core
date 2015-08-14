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

// TODO: remove dependency of mathlib (quaternion.h must be a TIER 0 lib)
// it uses functions like normalize_cv, cv_add, etc.

// TODO: transform this lib into a class

#ifndef _MATH_QUATERNION_H
#define _MATH_QUATERNION_H

// TODO: remove dependency
#include "math/vector.h"
#include "math/constants.h"


//! Quaternion, scalar last, using x, y, z.
/*! Can be thought of as ::rvector with scalar last. One can be set equal to other.
 * First 3 elements are the scaled orientation axis. Fourth element is the scaled
 * amount of rotation. Can alternatively be thought of as a ::cvector,
 * followed by a scalar.
*/
// TODO: replace cvector with x,y,z
struct quaternion
{
    //! Orientation
    cvector d;
    //! Rotation
    double w;
} ;

std::ostream& operator << (std::ostream& out, const quaternion& a);
std::istream& operator >> (std::istream& out, quaternion& a);

//! Quaternion, scalar last, using imaginary elements.
/*! Can be thought of as i, j, k elements, followed by scalar.
*/
struct qcomplex
{
    double i;
    double j;
    double k;
    double r;
} ;

std::ostream& operator << (std::ostream& out, const qcomplex& a);
std::istream& operator >> (std::istream& out, qcomplex& a);

//! Quaternion, scalar last, using vector elements.
/*! Can be thought of as vector elements, q1, q2, q3, followed by
 * scalar q4.
*/
struct qlast
{
    double q1; // x
    double q2; // y
    double q3; // z
    double q4; // w
} ;

std::ostream& operator << (std::ostream& out, const qlast& a);
std::istream& operator >> (std::istream& out, qlast& a);

//! Quaternion, scalar first using vector elements.
/*! Can be thought of as scalar element, q0, followed by vector
 * elements, q1, q2, q3.
*/
struct qfirst
{
    double q0; // w
    double q1; // x
    double q2; // y
    double q3; // z
} ;

std::ostream& operator << (std::ostream& out, const qfirst& a);
std::istream& operator >> (std::istream& out, qfirst& a);



cvector cv_quaternion2axis(quaternion q);
void q_normalize(quaternion *q);
quaternion q_zero();
quaternion q_conjugate(quaternion q);
quaternion q_times(quaternion q1, quaternion q2);
quaternion q_mult(rvector r1, quaternion q2);
quaternion q_mult(quaternion q1, quaternion q2);
quaternion q_smult(double a, quaternion q);
quaternion q_add(quaternion q1, quaternion q2);
quaternion q_sub(quaternion q1, quaternion q2);
quaternion q_euler2quaternion(avector rpw);
quaternion q_axis2quaternion_cv(cvector v);
quaternion q_axis2quaternion_rv(rvector v);

quaternion q_change_between_rv(rvector from, rvector to);
quaternion q_change_around_cv(cvector around, double angle);
quaternion q_change_around_rv(rvector around, double angle);
quaternion q_change_around_x(double angle);
quaternion q_change_around_y(double angle);
quaternion q_change_around_z(double angle);
quaternion q_rotate_around(int axis, double angle);
quaternion q_transform_for(rvector sourcea, rvector sourceb, rvector targeta, rvector targetb);
quaternion q_eye();
quaternion q_identity();
quaternion q_evaluate_poly(double x, std::vector< std::vector<double> > parms);
quaternion q_evaluate_poly_slope(double x, std::vector< std::vector<double> > parms);
double length_q(quaternion q);
double q_norm(quaternion q);
void qrotate(double ipos[3], double rpos[3], double angle, double *opos);
avector a_quaternion2euler(quaternion q);

// TODO: bring these functions from mathlib
quaternion q_change_between_cv(cvector from, cvector to);
//cvector rotate_q(quaternion q, rvector v);
//cvector rotate_q(quaternion q, cvector v);
//cvector transform_q(quaternion q,cvector v); // doesn't exist in .cpp?

// TODO: implement new class
class Quaternion {

public:
    Quaternion();
    Quaternion(double qx, double qy, double qz, double qw);
    double x,y,z,w;
    Quaternion getQuaternion();

    Quaternion quaternion2Quaternion(quaternion q);
    quaternion Quaternion2quaternion(Quaternion Q);


    // operators
    Quaternion operator+(const Quaternion& );
    Quaternion operator-(const Quaternion& );
    Quaternion operator*(const Quaternion& );

    friend std::ostream& operator << (std::ostream& os, const Quaternion& q);
    //std::istream& operator >> (std::istream& out, Quaternion& a);

    Quaternion multiplyScalar(double a);
    Quaternion conjugate();
    cvector vector();
    cvector omegaFromDerivative(Quaternion dq);
};

#endif
