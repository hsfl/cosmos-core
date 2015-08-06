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

#include "math/types.h"
#include "math/constants.h"
#include "math/mathlib.h"

cvector rotate_q(quaternion q,cvector v);
//cvector transform_q(quaternion q,cvector v); // doesn't exist in .cpp?
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
quaternion q_dcm2quaternion_cm(cmatrix dcm);
quaternion q_dcm2quaternion_rm(rmatrix m);
quaternion q_axis2quaternion_cv(cvector v);
quaternion q_axis2quaternion_rv(rvector v);
quaternion q_change_between_cv(cvector from, cvector to);
quaternion q_change_between_rv(rvector from, rvector to);
quaternion q_change_around_cv(cvector around, double angle);
quaternion q_change_around_rv(rvector around, double angle);
quaternion q_change_around_x(double angle);
quaternion q_change_around_y(double angle);
quaternion q_change_around_z(double angle);
quaternion q_rotate_around(int axis, double angle);
quaternion q_transform_for(rvector sourcea, rvector sourceb, rvector targeta, rvector targetb);
quaternion q_eye();
quaternion q_evaluate_poly(double x, std::vector< std::vector<double> > parms);
quaternion q_evaluate_poly_slope(double x, std::vector< std::vector<double> > parms);
double length_q(quaternion q);
double q_norm(quaternion q);
void qrotate(double ipos[3], double rpos[3], double angle, double *opos);
avector a_quaternion2euler(quaternion q);

#endif
