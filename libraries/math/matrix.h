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

// dependencies: vector
#ifndef _MATH_MATRIX_H
#define _MATH_MATRIX_H

#include "math/vector.h"

//! 3x3 element generic matrix
/*! 3 rvector elements representing 3 rows of a matrix
*/
struct rmatrix
{
        rvector row[3];
} ;

std::ostream& operator << (std::ostream& out, const rmatrix& a);
std::ostream& operator << (std::ostream& out, const std::vector<rmatrix>& a);
std::istream& operator >> (std::istream& out, rmatrix& a);



//! 3x3 element cartesian matrix
/*! 3 ::cvector elements representing 3 rows of a matrix in a right
 * handed cartesian system */
struct cmatrix
{
        //! Row 1
        cvector r1;
        //! Row 2
        cvector r2;
        //! Row 3
        cvector r3;
} ;

std::ostream& operator << (std::ostream& out, const cmatrix& a);
std::istream& operator >> (std::istream& in, cmatrix& a);


//! n element row matrix
struct matrix1d
{
        //! Elements
        double vector[4];
        //! Number of elements
        uint16_t cols;
} ;

//! nxm element 2D matrix
struct matrix2d
{
        //! Number of rows
        uint16_t rows;
        //! Number of elements
        uint16_t cols;
        //! Elements
        double array[4][4];
} ;


rvector rv_mmult(rmatrix a, rvector b);
rvector rv_diag(rmatrix a);
rvector rv_unskew(rmatrix matrix);
cvector cv_mmult(cmatrix a, cvector b);
cvector cv_diag(cmatrix a);

rmatrix rm_transpose(rmatrix a);
rmatrix rm_square(rmatrix a);
rmatrix rm_sqrt(rmatrix a);
rmatrix rm_eye();
rmatrix rm_zero();
rmatrix rm_mmult(rmatrix a, rmatrix b);
rmatrix rm_mult(rmatrix a, rmatrix b);
rmatrix rm_smult(double a, rmatrix b);
rmatrix rm_add(rmatrix a, rmatrix b);
rmatrix rm_sub(rmatrix a, rmatrix b);
rmatrix rm_change_between_rv(rvector from, rvector to);
rmatrix rm_change_around_x(double angle);
rmatrix rm_change_around_y(double angle);
rmatrix rm_change_around_z(double angle);
rmatrix rm_change_around(int axis, double angle);
rmatrix rm_diag(rvector a);
rmatrix rm_from_cm(cmatrix matrix);
rmatrix rm_from_rv(rvector vector,int direction);
rmatrix rm_skew(rvector row1);
rmatrix rm_inverse(rmatrix m);
rmatrix rm_from_m2(matrix2d m);

double norm_rm(rmatrix a);
double norm_rm2(rmatrix a);

double trace_rm(rmatrix a);
double trace_rm2(rmatrix a);
double determinant_rm(rmatrix a);


double norm_cm(cmatrix a);
double trace_cm(cmatrix a);

cmatrix cm_transpose(cmatrix a);
cmatrix cm_square(cmatrix a);
cmatrix cm_sqrt(cmatrix a);
cmatrix cm_eye();
cmatrix cm_zero();
cmatrix cm_mmult(cmatrix a, cmatrix b);
cmatrix cm_mult(cmatrix a, cmatrix b);
cmatrix cm_smult(double a, cmatrix b);
cmatrix cm_add(cmatrix a, cmatrix b);
cmatrix cm_sub(cmatrix a, cmatrix b);
cmatrix cm_change_between_cv(cvector from, cvector to);
cmatrix cm_change_around_x(double angle);
cmatrix cm_change_around_y(double angle);
cmatrix cm_change_around_z(double angle);
cmatrix cm_change_around(int axis, double angle);
cmatrix cm_from_rm(rmatrix matrix);
cmatrix cm_diag(cvector a);

matrix2d m2_inverse(matrix2d m);
double m2_determinant(matrix2d m);
matrix2d m2_zero(uint16_t rows, uint16_t cols);
matrix2d m2_eye(uint16_t rows);
matrix2d m2_cross(matrix2d matrix1, matrix2d matrix2);
matrix2d m2_smult(double number, matrix2d matrix);
matrix2d m2_sub(matrix2d matrix1, matrix2d matrix2);
matrix2d m2_add(matrix2d matrix1, matrix2d matrix2);
matrix2d m2_transpose(matrix2d matrix);
matrix1d m2_unskew(matrix2d matrix);
double m2_trace(matrix2d matrix);
matrix2d m2_mmult(matrix2d matrix1, matrix2d matrix2);
matrix1d m2_eig2x2(matrix2d matrix);
double m2_snorm2x2(matrix2d matrix);
matrix2d cv_to_m2(cvector vector,int direction);
matrix1d cv_to_m1(cvector vector);
matrix2d cm3x3_to_m2(cmatrix matrix);
matrix2d m1_to_m2(matrix1d vector,int direction);
matrix2d m2_from_rm(rmatrix matrix);

matrix1d m1_zero(uint16_t cols);
matrix1d m1_smult(double number, matrix1d row);
matrix1d m1_mmult(matrix2d matrix, matrix1d row);
matrix1d m1_sub(matrix1d row1, matrix1d row2);
matrix1d m1_add(matrix1d row1, matrix1d row2);
matrix1d m1_cross(matrix1d row1, matrix1d row2);
matrix2d m2_skew(matrix1d row1);
matrix2d m2_diag(matrix1d row1);
double m1_norm(matrix1d matrix);
double m1_dot(matrix1d a, matrix1d b);

#endif
