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

#include "matrix.h"

//! \addtogroup mathlib_functions
//! @{

//! Multiply ::rmatrix by ::rvector.
/*! Multiply 3x3 ::rmatrix by 3 element ::rvector (treated as a column order vector).
        \param m matrix to multiply by, in ::rmatrix form.
        \param v vector to be tranformed, in ::rvector form.
        \return multiplied vector, in ::rvector format.
*/
rvector rv_mmult(rmatrix m, rvector v)
{
    rvector o = {{0.}};

    o.col[0] = m.row[0].col[0]*v.col[0] + m.row[0].col[1]*v.col[1] + m.row[0].col[2]*v.col[2];
    o.col[1] = m.row[1].col[0]*v.col[0] + m.row[1].col[1]*v.col[1] + m.row[1].col[2]*v.col[2];
    o.col[2] = m.row[2].col[0]*v.col[0] + m.row[2].col[1]*v.col[1] + m.row[2].col[2]*v.col[2];

    return (o);
}

// multiply matrix by vector
rvector operator * (rmatrix m, rvector v)
{
    return rv_mmult(m, v);
}

//! Matrix diagonal
/*! ::rvector representing the diagonal of a ::rmatrix
        \param a :rmatrix to get diagonal from.
        \return Diagonal
*/
rvector rv_diag(rmatrix a)
{
    rvector b = {{0.}};

    b.col[0] = a.row[0].col[0];
    b.col[1] = a.row[1].col[1];
    b.col[2] = a.row[2].col[2];

    return (b);
}

//! Multiply cartesian vector by cartesian matrix
/*! Multiply 3 element cartesian vector by 3x3 cartesian matrix
        \param v vector to be tranformed, in ::cvector form
        \param m matrix to multiply by, in ::cmatrix form
        \return multiplied vector, in ::cvector format
*/
cvector cv_mmult(cmatrix m, cvector v)
{
    cvector o;

    o.x = m.r1.x*v.x + m.r1.y*v.y + m.r1.z*v.z;
    o.y = m.r2.x*v.x + m.r2.y*v.y + m.r2.z*v.z;
    o.z = m.r3.x*v.x + m.r3.y*v.y + m.r3.z*v.z;

    return (o);
}

/*!
 * \brief Diagonal ::rmatrix
 * Creates an ::rmatrix whose diagonal is filled with the supplied ::rvector.
 * \param a
 * \return New ::rmatrix.
 */
rmatrix rm_diag(rvector a)
{
    rmatrix b = {{{{0.}}}};;

    b.row[0].col[0] = a.col[0];
    b.row[1].col[1] = a.col[1];
    b.row[2].col[2] = a.col[2];

    b.row[0].col[1] = b.row[0].col[2] = b.row[1].col[0] = b.row[1].col[2] = b.row[2].col[0] = b.row[2].col[1] = 0.;

    return (b);
}

//! Identity ::rmatrix
/*! ::rmatrix with diagonal elements set to one and all others set to zero
        \return Identity matrix
*/
rmatrix rm_eye()
{
    rmatrix mat = {{{{1.,0.,0.}}, {{0.,1.,0.}}, {{0.,0.,1.}} }};

    return (mat);
}

//! Zero filled ::rmatrix
/*! ::rmatrix with all elements set to zero
        \return Zero filled matrix
*/
rmatrix rm_zero()
{
    rmatrix mat = {{{{0.,0.,0.}},{{0.,0.,0.}},{{0.,0.,0.}}}};
    return (mat);
}

/*!
 * \brief ::rmatrix norm.
 * Calculates the Norm of the supplied ::rmatrix
 * \param mat
 * \return Calculated norm.
 */
double norm_rm(rmatrix mat)
{
    return fmax(
                norm_rv(mat.row[0]),
            fmax(
                norm_rv(mat.row[1]),
            norm_rv(mat.row[2])
            )
            );
}

/*!
 * \brief ::rmatrix Trace
 * Calculates the trace of the supplied ::rmatrix.
 * \param mat
 * \return Calculated trace.
 */
double trace_rm(rmatrix mat)
{
    double trace;
    trace = mat.row[0].col[0] + mat.row[1].col[1] + mat.row[2].col[2];
    return (trace);
}

//double trace_rm2(rmatrix mat)
//{
//    return mat.row[0].col[0] + mat.row[1].col[1] + mat.row[2].col[2];
//}

/*!
 * \brief ::rmatrix Transpose.
 * Calculate the transpose of the supplied ::rmatrix.
 * \param a
 * \return Calculated transpose.
 */
rmatrix rm_transpose(rmatrix a)
{
    rmatrix b = {{{{0.}}}};;

    b.row[0].col[0] = a.row[0].col[0];
    b.row[0].col[1] = a.row[1].col[0];
    b.row[0].col[2] = a.row[2].col[0];

    b.row[1].col[0] = a.row[0].col[1];
    b.row[1].col[1] = a.row[1].col[1];
    b.row[1].col[2] = a.row[2].col[1];

    b.row[2].col[0] = a.row[0].col[2];
    b.row[2].col[1] = a.row[1].col[2];
    b.row[2].col[2] = a.row[2].col[2];

    return (b);
}


//! ::rmatrix Matrix Product
/*! Multiply two row order matrices together.
        \param a first ::rmatrix.
        \param b second ::rmatrix.
        \return product ::rmatrix.
*/
rmatrix rm_mmult(rmatrix a, rmatrix b)
{
    rmatrix mat = {{{{0.}}}};;

    mat.row[0].col[0]  = a.row[0].col[0]*b.row[0].col[0] + a.row[0].col[1]*b.row[1].col[0] + a.row[0].col[2]*b.row[2].col[0];
    mat.row[0].col[1]  = a.row[0].col[0]*b.row[0].col[1] + a.row[0].col[1]*b.row[1].col[1] + a.row[0].col[2]*b.row[2].col[1];
    mat.row[0].col[2]  = a.row[0].col[0]*b.row[0].col[2] + a.row[0].col[1]*b.row[1].col[2] + a.row[0].col[2]*b.row[2].col[2];

    mat.row[1].col[0]  = a.row[1].col[0]*b.row[0].col[0] + a.row[1].col[1]*b.row[1].col[0] + a.row[1].col[2]*b.row[2].col[0];
    mat.row[1].col[1]  = a.row[1].col[0]*b.row[0].col[1] + a.row[1].col[1]*b.row[1].col[1] + a.row[1].col[2]*b.row[2].col[1];
    mat.row[1].col[2]  = a.row[1].col[0]*b.row[0].col[2] + a.row[1].col[1]*b.row[1].col[2] + a.row[1].col[2]*b.row[2].col[2];

    mat.row[2].col[0]  = a.row[2].col[0]*b.row[0].col[0] + a.row[2].col[1]*b.row[1].col[0] + a.row[2].col[2]*b.row[2].col[0];
    mat.row[2].col[1]  = a.row[2].col[0]*b.row[0].col[1] + a.row[2].col[1]*b.row[1].col[1] + a.row[2].col[2]*b.row[2].col[1];
    mat.row[2].col[2]  = a.row[2].col[0]*b.row[0].col[2] + a.row[2].col[1]*b.row[1].col[2] + a.row[2].col[2]*b.row[2].col[2];

    return (mat);
}

/*!
 * \brief Element-wise ::rmatrix multiplication.
 * \param a first ::rmatrix.
 * \param b second ::rmatrix.
 * \return Element-wise product ::rmatrix.
 */
rmatrix rm_mult(rmatrix a, rmatrix b)
{
    rmatrix mat = {{{{0.}}}};
    rvector *va, *vb;

    va = &a.row[0];
    vb = &b.row[0];
    mat.row[0] = rv_mult(*va,*vb);
    va = &a.row[1];
    vb = &b.row[1];
    mat.row[1] = rv_mult(*va,*vb);
    va = &a.row[2];
    vb = &b.row[2];
    mat.row[2] = rv_mult(*va,*vb);

    return (mat);
}

/*!
 * \brief Scalar ::rmatrix multiplication.
 * \param a Scalar to multiply each element by.
 * \param b ::rmatrix.
 * \return Scalar product ::rmatrix.
 */
rmatrix rm_smult(double a, rmatrix b)
{
    rmatrix mat = {{{{0.}}}};
    rvector *vb;

    vb = &b.row[0];
    mat.row[0] = rv_smult(a,*vb);
    vb = &b.row[1];
    mat.row[1] = rv_smult(a,*vb);
    vb = &b.row[2];
    mat.row[2] = rv_smult(a,*vb);

    return (mat);
}

/*!
 * \brief ::rmatrix addition.
 * Sum of two ::rmatrix values.
 * \param a First term.
 * \param b Second term.
 * \return Matrix sum.
 */
rmatrix rm_add(rmatrix a, rmatrix b)
{
    rmatrix mat = {{{{0.}}}};
    rvector *va, *vb;

    va = &a.row[0];
    vb = &b.row[0];
    mat.row[0] = rv_add(*va,*vb);
    va = &a.row[1];
    vb = &b.row[1];
    mat.row[1] = rv_add(*va,*vb);
    va = &a.row[2];
    vb = &b.row[2];
    mat.row[2] = rv_add(*va,*vb);

    return (mat);
}

/*!
 * \brief ::rmatrix subtraction.
 * Subtract two ::rmatrix values.
 * \param a Minuend.
 * \param b Subtrahend.
 * \return Difference.
 */
rmatrix rm_sub(rmatrix a, rmatrix b)
{
    rmatrix mat = {{{{0.}}}};
    rvector *va, *vb;

    va = &a.row[0];
    vb = &b.row[0];
    mat.row[0] = rv_sub(*va,*vb);
    va = &a.row[1];
    vb = &b.row[1];
    mat.row[1] = rv_sub(*va,*vb);
    va = &a.row[2];
    vb = &b.row[2];
    mat.row[2] = rv_sub(*va,*vb);

    return (mat);
}

//! Square rmatrix
/*! Square a rmatrix matrix by matrix multiplying it by itself.
        \param a matrix to be squared
        \return squared matrix
*/
rmatrix rm_square(rmatrix a)
{
    rmatrix b = {{{{0.}}}};
    rvector r1, r2, r3, c1, c2, c3;

    r1 = a.row[0];
    r2 = a.row[1];
    r3 = a.row[2];

    c1.col[0] = a.row[0].col[0];
    c1.col[1] = a.row[1].col[0];
    c1.col[2] = a.row[2].col[0];

    c2.col[0] = a.row[0].col[1];
    c2.col[1] = a.row[1].col[1];
    c2.col[2] = a.row[2].col[1];

    c3.col[0] = a.row[0].col[2];
    c3.col[1] = a.row[1].col[2];
    c3.col[2] = a.row[2].col[2];

    b.row[0].col[0] = sum_rv(rv_mult(r1,c1));
    b.row[0].col[1] = sum_rv(rv_mult(r1,c2));
    b.row[0].col[2] = sum_rv(rv_mult(r1,c3));

    b.row[1].col[0] = sum_rv(rv_mult(r2,c1));
    b.row[1].col[1] = sum_rv(rv_mult(r2,c2));
    b.row[1].col[2] = sum_rv(rv_mult(r2,c3));

    b.row[2].col[0] = sum_rv(rv_mult(r3,c1));
    b.row[2].col[1] = sum_rv(rv_mult(r3,c2));
    b.row[2].col[2] = sum_rv(rv_mult(r3,c3));

    return (b);
}

//! Rotation matrix for X axis
/*! Create the DCM that represents a rotation of the given angle around the X axis.
        \param angle Angle of rotation in radians
        \return Resulting DCM
*/
rmatrix rm_change_around_x(double angle)
{
    return rm_change_around(1,angle);
}

//! Rotation matrix for Y axis
/*! Create the DCM that represents a rotation of the given angle around the Y axis.
        \param angle Angle of rotation in radians
        \return Resulting DCM
*/
rmatrix rm_change_around_y(double angle)
{
    //rmatrix a = {{{{0.}}}};
    //a = rm_change_around(2, angle);
    //return (a);

    return rm_change_around(2,angle);
}

//! Rotation matrix for Z axis
/*! Create the DCM that represents a rotation of the given angle around the Z axis.
        \param angle Angle of rotation in radians
        \return Resulting DCM
*/
rmatrix rm_change_around_z(double angle)
{
    //rmatrix a = {{{{0.}}}};
    //a = rm_change_around(3, angle);
    //return (a);

    return rm_change_around(3,angle);
}

//! Rotation matrix for indicated axis
/*! Create the DCM that represents a rotation of the given angle around the indicated axis.
        \param axis Axis of rotation: 1=X, 2=Y, 3=Z
        \param angle Angle of rotation in radians
        \return Resulting DCM
*/
rmatrix rm_change_around(int axis,double angle)
{
    rmatrix a = {{{{1.,0.,0.}},{{0.,1.,0.}},{{0.,0.,1.}}}};

    switch (axis)
    {
    case 1:
        a.row[1].col[1] = a.row[2].col[2] = cos(angle);
        a.row[2].col[1] = sin(angle);
        a.row[1].col[2] = -(a.row[2].col[1]);
        break;
    case 2:
        a.row[0].col[0] = a.row[2].col[2] = cos(angle);
        a.row[0].col[2] = sin(angle);
        a.row[2].col[0] = -(a.row[0].col[2]);
        break;
    case 3:
        a.row[0].col[0] = a.row[1].col[1] = cos(angle);
        a.row[1].col[0] = sin(angle);
        a.row[0].col[1] = -(a.row[1].col[0]);
        break;
    }

    return (a);
}

//! ::cmatrix from ::rmatrix
/*! Converts 3x3 matrix in ::rmatrix form to ::cmatrix form.
        \param matrix Row major matrix to convert
        \return Converted matrix in row order form
*/
cmatrix cm_from_rm(rmatrix matrix)
{
    cmatrix cm;

    cm.r1.x = matrix.row[0].col[0];
    cm.r1.y = matrix.row[0].col[1];
    cm.r1.z = matrix.row[0].col[2];
    cm.r2.x = matrix.row[1].col[0];
    cm.r2.y = matrix.row[1].col[1];
    cm.r2.z = matrix.row[1].col[2];
    cm.r3.x = matrix.row[2].col[0];
    cm.r3.y = matrix.row[2].col[1];
    cm.r3.z = matrix.row[2].col[2];

    return (cm);
}



/*!
 * \brief Diagonal ::cmatrix
 * Creates an ::cmatrix whose diagonal is filled with the supplied ::cvector.
 * \param a
 * \return New ::cmatrix.
 */
cmatrix cm_diag(cvector a)
{
    cmatrix b;

    b.r1.x = a.x;
    b.r2.y = a.y;
    b.r3.z = a.z;

    b.r1.y = b.r1.z = b.r2.x = b.r2.z = b.r3.x = b.r3.y = 0.;

    return (b);
}

//! Identity ::cmatrix
/*! ::cmatrix with diagonal elements set to one and all others set to zero
        \return Identity matrix
*/
cmatrix cm_eye()
{
    cmatrix mat = {{1.,0.,0.},{0.,1.,0.},{0.,0.,1.}};

    return (mat);
}

//! Zero filled ::cmatrix
/*! ::cmatrix with all elements set to zero
        \return Zero filled matrix
*/
cmatrix cm_zero()
{
    cmatrix mat = {{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}};
    return (mat);
}

/*!
 * \brief ::cmatrix norm.
 * Calculates the Norm of the supplied ::cmatrix
 * \param mat
 * \return Calculated norm.
 */
double norm_cm(cmatrix mat)
{
    double norm;

    norm = norm_cv(mat.r1);
    norm = fmax(norm,norm_cv(mat.r2));
    norm = fmax(norm,norm_cv(mat.r3));

    return (norm);
}

/*!
 * \brief ::cmatrix Trace
 * Calculates the trace of the supplied ::cmatrix.
 * \param mat
 * \return Calculated trace.
 */
double trace_cm(cmatrix mat)
{
    double trace;

    trace = mat.r1.x + mat.r2.y + mat.r3.z;

    return (trace);
}

/*!
 * \brief ::cmatrix Transpose.
 * Calculate the transpose of the supplied ::cmatrix.
 * \param a
 * \return Calculated transpose.
 */
cmatrix cm_transpose(cmatrix a)
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

//! Matrix diagonal
/*! ::cvector representing the diagonal of a ::cmatrix
        \param a :cmatrix to get diagonal from.
        \return Diagonal
*/
cvector cv_diag(cmatrix a)
{
    cvector b;

    b.x = a.r1.x;
    b.y = a.r2.y;
    b.z = a.r3.z;

    return (b);
}

//! ::cmatrix Matrix Product
/*! Multiply two cartesian matrices together.
        \param a first cartesian matrix
        \param b second cartesian matrix
        \return product cartesian matrix
*/
cmatrix cm_mmult(cmatrix a, cmatrix b)
{
    cmatrix mat;

    mat.r1.x  = a.r1.x*b.r1.x + a.r1.y*b.r2.x + a.r1.z*b.r3.x;
    mat.r1.y  = a.r1.x*b.r1.y + a.r1.y*b.r2.y + a.r1.z*b.r3.y;
    mat.r1.z  = a.r1.x*b.r1.z + a.r1.y*b.r2.z + a.r1.z*b.r3.z;

    mat.r2.x  = a.r2.x*b.r1.x + a.r2.y*b.r2.x + a.r2.z*b.r3.x;
    mat.r2.y  = a.r2.x*b.r1.y + a.r2.y*b.r2.y + a.r2.z*b.r3.y;
    mat.r2.z  = a.r2.x*b.r1.z + a.r2.y*b.r2.z + a.r2.z*b.r3.z;

    mat.r3.x  = a.r3.x*b.r1.x + a.r3.y*b.r2.x + a.r3.z*b.r3.x;
    mat.r3.y  = a.r3.x*b.r1.y + a.r3.y*b.r2.y + a.r3.z*b.r3.y;
    mat.r3.z  = a.r3.x*b.r1.z + a.r3.y*b.r2.z + a.r3.z*b.r3.z;

    return (mat);
}

/*!
 * \brief Element-wise ::cmatrix multiplication.
 * \param a first ::cmatrix.
 * \param b second ::cmatrix.
 * \return Element-wise product ::cmatrix.
 */
cmatrix cm_mult(cmatrix a, cmatrix b)
{
    cmatrix mat;
    cvector *va, *vb;

    va = &a.r1;
    vb = &b.r1;
    mat.r1 = cv_mult(*va,*vb);
    va = &a.r2;
    vb = &b.r2;
    mat.r2 = cv_mult(*va,*vb);
    va = &a.r3;
    vb = &b.r3;
    mat.r3 = cv_mult(*va,*vb);

    return (mat);
}

/*!
 * \brief Scalar ::cmatrix multiplication.
 * \param a Scalar to multiply each element by.
 * \param b ::cmatrix.
 * \return Scalar product ::cmatrix.
 */
cmatrix cm_smult(double a, cmatrix b)
{
    cmatrix mat;
    cvector *vb;

    vb = &b.r1;
    mat.r1 = cv_smult(a,*vb);
    vb = &b.r2;
    mat.r2 = cv_smult(a,*vb);
    vb = &b.r3;
    mat.r3 = cv_smult(a,*vb);

    return (mat);
}

/*!
 * \brief ::cmatrix addition.
 * Sum of two ::cmatrix values.
 * \param a First term.
 * \param b Second term.
 * \return Matrix sum.
 */
cmatrix cm_add(cmatrix a, cmatrix b)
{
    cmatrix mat;
    cvector *va, *vb;

    va = &a.r1;
    vb = &b.r1;
    mat.r1 = cv_add(*va,*vb);
    va = &a.r2;
    vb = &b.r2;
    mat.r2 = cv_add(*va,*vb);
    va = &a.r3;
    vb = &b.r3;
    mat.r3 = cv_add(*va,*vb);

    return (mat);
}

/*!
 * \brief ::cmatrix subtraction.
 * Subtract two ::cmatrix values.
 * \param a Minuend.
 * \param b Subtrahend.
 * \return Difference.
 */
cmatrix cm_sub(cmatrix a, cmatrix b)
{
    cmatrix mat;
    cvector *va, *vb;

    va = &a.r1;
    vb = &b.r1;
    mat.r1 = cv_sub(*va,*vb);
    va = &a.r2;
    vb = &b.r2;
    mat.r2 = cv_sub(*va,*vb);
    va = &a.r3;
    vb = &b.r3;
    mat.r3 = cv_sub(*va,*vb);

    return (mat);
}

//! Square ::cmatrix
/*! Square a ::cmatrix by matrix multiplying it by itself.
        \param a matrix to be squared
        \return squared matrix
*/
cmatrix cm_square(cmatrix a)
{
    cmatrix b;
    cvector r1, r2, r3, c1, c2, c3;

    r1 = a.r1;
    r2 = a.r2;
    r3 = a.r3;

    c1.x = a.r1.x;
    c1.y = a.r2.x;
    c1.z = a.r3.x;

    c2.x = a.r1.y;
    c2.y = a.r2.y;
    c2.z = a.r3.y;

    c3.x = a.r1.z;
    c3.y = a.r2.z;
    c3.z = a.r3.z;

    b.r1.x = sum_cv(cv_mult(r1,c1));
    b.r1.y = sum_cv(cv_mult(r1,c2));
    b.r1.z = sum_cv(cv_mult(r1,c3));

    b.r2.x = sum_cv(cv_mult(r2,c1));
    b.r2.y = sum_cv(cv_mult(r2,c2));
    b.r2.z = sum_cv(cv_mult(r2,c3));

    b.r3.x = sum_cv(cv_mult(r3,c1));
    b.r3.y = sum_cv(cv_mult(r3,c2));
    b.r3.z = sum_cv(cv_mult(r3,c3));

    return (b);
}

// TODO: move to rotation.cpp
//! Rotation matrix for X axis
/*! Create the DCM that represents a rotation of the given angle around the X axis.
        \param angle Angle of rotation in radians
        \return Resulting DCM
*/
cmatrix cm_change_around_x(double angle)
{
    cmatrix a;

    a = cm_change_around(1, angle);

    return (a);
}

// TODO: move to rotation.cpp
//! Rotation matrix for Y axis
/*! Create the DCM that represents a rotation of the given angle around the Y axis.
        \param angle Angle of rotation in radians
        \return Resulting DCM
*/
cmatrix cm_change_around_y(double angle)
{
    cmatrix a;

    a = cm_change_around(2, angle);

    return (a);
}

// TODO: move to rotation.cpp
//! Rotation matrix for Z axis
/*! Create the DCM that represents a rotation of the given angle around the Z axis.
        \param angle Angle of rotation in radians
        \return Resulting DCM
*/
cmatrix cm_change_around_z(double angle)
{
    cmatrix a;

    a = cm_change_around(3, angle);

    return (a);
}

// TODO: move to rotation.cpp
//! Rotation matrix for indicated axis
/*! Create the DCM that represents a rotation of the given angle around the indicated axis.
        \param axis Axis of rotation: 1=X, 2=Y, 3=Z
        \param angle Angle of rotation in radians
        \return Resulting DCM
*/
cmatrix cm_change_around(int axis,double angle)
{
    cmatrix a = {{1.,0.,0.},{0.,1.,0.},{0.,0.,1.}};

    switch (axis)
    {
    case 1:
        a.r2.y = a.r3.z = cos(angle);
        a.r3.y = sin(angle);
        a.r2.z = -(a.r3.y);
        break;
    case 2:
        a.r1.x = a.r3.z = cos(angle);
        a.r1.z = sin(angle);
        a.r3.x = -(a.r1.z);
        break;
    case 3:
        a.r1.x = a.r2.y = cos(angle);
        a.r2.x = sin(angle);
        a.r1.y = -(a.r2.x);
        break;
    }

    return (a);
}

//! ::rmatrix from ::cmatrix
/*! Converts 3x3 matrix in cartesian form to row major form.
        \param matrix Cartesian matrix to convert
        \return Converted matrix in row major form
*/
rmatrix rm_from_cm(cmatrix matrix)
{
    rmatrix rm = {{{{0.}}}};

    rm.row[0].col[0] = matrix.r1.x;
    rm.row[0].col[1] = matrix.r1.y;
    rm.row[0].col[2] = matrix.r1.z;
    rm.row[1].col[0] = matrix.r2.x;
    rm.row[1].col[1] = matrix.r2.y;
    rm.row[1].col[2] = matrix.r2.z;
    rm.row[2].col[0] = matrix.r3.x;
    rm.row[2].col[1] = matrix.r3.y;
    rm.row[2].col[2] = matrix.r3.z;

    return (rm);
}

//! ::rvector to ::rmatrix.
/*! Convert a row vector to a row ordder ::rmatrix
        \param vector Row vector to be converted
        \param direction Alignment, column order if 1 (DIRECTION_COLUMN), otherwise row
        order.
        \return Single row or column ::rmatrix
*/
rmatrix rm_from_rv(rvector vector,int direction)
{
    rmatrix answer = {{{{0.}}}};;
    uint16_t i;

    answer = rm_zero();
    switch (direction)
    {
    case DIRECTION_COLUMN:
        for (i=0; i<3; i++)
        {
            answer.row[i].col[0] = vector.col[i];
        }
        break;
        break;
    default:
        answer.row[0] = vector;
        break;
    }

    return (answer);
}

//! Create skew symmetric ::rmatrix from ::rvector
/*! Generate a new 3x3 skew symmetric row matrix from the provided 3 element
 * vector.
        \param row row vector to be skewed
        \return skew symmetric row matrix
*/
rmatrix rm_skew(rvector row)
{
    rmatrix answer = {{{{0.}}}};;

    answer = rm_zero();

    answer.row[0].col[1] = -row.col[2];
    answer.row[0].col[2] = row.col[1];

    answer.row[1].col[0] = row.col[2];
    answer.row[1].col[2] = -row.col[0];

    answer.row[2].col[0] = -row.col[1];
    answer.row[2].col[1] = row.col[0];

    return (answer);
}

//! Unskew 3x3 row matrix
/*! Create the 3 element ::rvector correponding to a 3x3 skew symmetric ::rmatrix
        \param matrix 3x3 row skew symmetric matrix
        \return ::rvector representing unskewed elements
*/
rvector rv_unskew(rmatrix matrix)
{
    rvector answer={{0.,0.,0.}};

    answer.col[0] = -matrix.row[1].col[2];
    answer.col[1] = matrix.row[0].col[2];
    answer.col[2] = -matrix.row[0].col[1];

    return (answer);
}

//! Inverse of rmatrix
/*! Inverse of 3x3 ::rmatrix using algorithm at
        http://mathworld.wolfram.com/MatrixInverse.html
        \param m ::rmatrix to take inverse of
        \return Inverse matrix, or
*/
rmatrix rm_inverse(rmatrix m)
{
    rmatrix wm = {{{{0.}}}};;

    wm.row[0].col[0] = m.row[1].col[1]*m.row[2].col[2] - m.row[1].col[2]*m.row[2].col[1];
    wm.row[0].col[1] = m.row[0].col[2]*m.row[2].col[1] - m.row[0].col[1]*m.row[2].col[2];
    wm.row[0].col[2] = m.row[0].col[1]*m.row[1].col[2] - m.row[0].col[2]*m.row[1].col[1];

    wm.row[1].col[0] = m.row[1].col[2]*m.row[2].col[0] - m.row[1].col[0]*m.row[2].col[2];
    wm.row[1].col[1] = m.row[0].col[0]*m.row[2].col[2] - m.row[0].col[2]*m.row[2].col[0];
    wm.row[1].col[2] = m.row[0].col[2]*m.row[1].col[0] - m.row[0].col[0]*m.row[1].col[2];

    wm.row[2].col[0] = m.row[1].col[0]*m.row[2].col[1] - m.row[1].col[1]*m.row[2].col[0];
    wm.row[2].col[1] = m.row[0].col[1]*m.row[2].col[0] - m.row[0].col[0]*m.row[2].col[1];
    wm.row[2].col[2] = m.row[0].col[0]*m.row[1].col[1] - m.row[0].col[1]*m.row[1].col[0];

    return( rm_smult((1/determinant_rm(m)), wm) );
}

//! rmatrix from matrix2d
/*! Converts 3x3 matrix in matrix2d form to row major form.
        \param matrix matrix2d matrix to convert
        \return Converted matrix in row major form
*/
rmatrix rm_from_m2(matrix2d matrix)
{
    rmatrix rm = {{{{0.}}}};;
    int i, j;

    for (i=0; i<3; i++)
    {
        for (j=0; j<3; j++)
        {
            rm.row[i].col[j] = matrix.array[i][j];
        }
    }

    return (rm);
}

//!  Determinant of row column matrix
/*! Return the determinant for a 3x3 ::rmatrix
        \param m ;;rmatrix to calculate detrminant of
        \return determinant, otherwise NaN
*/
double determinant_rm(rmatrix m)
{
    double result;

    result = m.row[0].col[0] * (m.row[1].col[1] * m.row[2].col[2] - m.row[2].col[1] * m.row[1].col[2]);
    result -= m.row[1].col[0] * (m.row[0].col[1] * m.row[2].col[2] - m.row[0].col[2] * m.row[2].col[1]);
    result += m.row[2].col[0] * (m.row[0].col[1] * m.row[1].col[2] - m.row[0].col[2] * m.row[1].col[1]);
    return (result);

}

//! Fill 1D matrix with zeros
/*! Fill the provided vector with zeros, and set its size to the provided number of
 * columns
        \param cols Number of columns
        \return Zero filled vector
*/
matrix1d m1_zero(uint16_t cols)
{
    matrix1d answer={{0.,0.,0.,0.},0};
    uint16_t i;

    answer.cols = cols;
    for (i=0; i<cols; i++)
    {
        answer.vector[i] = 0.;
    }
    return (answer);
}

//! Multiply 1D matrix by a scalar
/*! Multiply each element of indicated 1D matrix by a scalar and
 * return as a new 1D matrix.
        \param number scalar to multiply by
        \param row 1D matrix to be multiplied
        \return pointer to the ::matrix1d result, otherwise NULL
*/
matrix1d m1_smult(double number, matrix1d row)
{
    matrix1d answer={{0.,0.,0.,0.},0};
    uint16_t i;

    answer.cols = row.cols;
    for (i=0; i<row.cols; i++)
    {
        answer.vector[i] = row.vector[i] * number;
    }

    return (answer);
}

//! Add one 1D matrix to another
/*! Add each element in the second matrix to each element in
 * the first matrix, returning the answer as a new matrix.
        \param row1 matrix to be added to
        \param row2 matrix to add
        \return sum, as newly created 1D matrix
*/
matrix1d m1_add(matrix1d row1, matrix1d row2)
{
    matrix1d answer={{0.,0.,0.,0.},0};
    uint16_t i;

    answer.cols = row1.cols;
    for (i=0; i<row1.cols; i++)
    {
        answer.vector[i] = row1.vector[i] + row2.vector[i];
    }

    return (answer);
}

//! Subtract one 1D matrix from another
/*! Subtract each element in the second matrix from each element in
 * the first matrix, returning the answer as a new matrix.
        \param row1 matrix to be subtracted from
        \param row2 matrix to subtract
        \return difference, as newly created 1D matrix
*/
matrix1d m1_sub(matrix1d row1, matrix1d row2)
{
    matrix1d answer={{0.,0.,0.,0.},0};
    uint16_t i;

    answer.cols = row1.cols;
    for (i=0; i<row1.cols; i++)
    {
        answer.vector[i] = row1.vector[i] - row2.vector[i];
    }

    return (answer);
}

//! Multiply matrix1d by matrix2d
/*! Perform a matrix multiplication of a matrix1d vector as though it were a column matrix.
        \param Matrix Matrix to multiply by
        \param Vector Vector to multiply
        \return Vector result
*/
matrix1d m1_mmult(matrix2d Matrix,matrix1d Vector)
{
    matrix1d result={{0.,0.,0.,0.},0};
    int i, j;

    if (Matrix.cols == Vector.cols)
    {
        result.cols = Vector.cols;
        for (i=0; i<result.cols; i++)
        {
            result.vector[i] = 0.;
            for (j=0; j<result.cols; j++)
            {
                result.vector[i] += Vector.vector[j]*Matrix.array[i][j];
            }
        }
    }

    return(result);
}

//! matrix1d cross product
/*! Computes the vector cross product (A x B) of two 1d vectors.
        \param Vector1 vector A
        \param Vector2 vector B
\return Vector cross product as 1d vector.*/
matrix1d m1_cross(matrix1d Vector1, matrix1d Vector2)
{
    matrix1d result;

    result.cols = Vector1.cols;
    result.vector[0] = Vector1.vector[1]*Vector2.vector[2] - Vector1.vector[2]*Vector2.vector[1];
    result.vector[1] = -(Vector1.vector[0]*Vector2.vector[2] - Vector1.vector[2]*Vector2.vector[0]);
    result.vector[2] = Vector1.vector[0]*Vector2.vector[1] - Vector1.vector[1]*Vector2.vector[0];

    return(result);
}

//! matrix1d dot product
/*! Computes the vector dot product (A x B) of two 1d vectors.
        \param a vector A
        \param b vector B
\return Vector cross product as 1d vector.
*/
double m1_dot(matrix1d a, matrix1d b)
{
    double d = 0.;
    uint16_t i;

    for(i=0; i<a.cols; i++)
    {
        d += a.vector[i]*b.vector[i];
    }
    return (d);
}

//! Create skew symmetric matrix2d from matrix1d
/*! Generate a new 3x3 skew symmetric 2D matrix from the provided 3 element
 * vector.
        \param row row vector to be skewed
        \return skew symmetric 2D matrix
*/
matrix2d m2_skew(matrix1d row)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

    if (row.cols == 3)
    {
        answer.rows = answer.cols = 3;
        answer.array[0][0] = answer.array[1][1] = answer.array[2][2] = 0.;

        answer.array[0][1] = -row.vector[2];
        answer.array[0][2] = row.vector[1];

        answer.array[1][0] = row.vector[2];
        answer.array[1][2] = -row.vector[0];

        answer.array[2][0] = -row.vector[1];
        answer.array[2][1] = row.vector[0];
    }

    return (answer);
}

//! Create diagonal matrix2d from matrix1d
/*! Generate a new nxn diagonal 2D matrix from the provided n element
 * 1D matrix.
        \param row 1D matrix to be skewed
        \return result as new 2D matrix, otherwise NULL
*/
matrix2d m2_diag(matrix1d row)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

    if (row.cols == 3)
    {
        answer.cols = answer.rows = 3;
        answer.array[0][0] = row.vector[0];
        answer.array[1][1] = row.vector[1];
        answer.array[2][2] = row.vector[2];
    }

    return (answer);
}

//! Inverse of matrix2d
/*! Inverse of 2x2 or 3x3 Matrix2D using algorithm at
        http://mathworld.wolfram.com/MatrixInverse.html
        \param m Matrix2d to take inverse of
        \return Inverse matrix, or
*/
matrix2d m2_inverse(matrix2d m)
{
    matrix2d wm;

    wm.rows = m.rows;
    wm.cols = m.cols;

    if (m.rows == 2)
    {
        wm.array[0][0] = m.array[1][1];
        wm.array[1][1] = m.array[0][0];
        wm.array[1][0] = -m.array[1][0];
        wm.array[0][1] = -m.array[0][1];
    }
    else
    {
        wm.array[0][0] = m.array[1][1]*m.array[2][2] - m.array[1][2]*m.array[2][1];
        wm.array[0][1] = m.array[0][2]*m.array[2][1] - m.array[0][1]*m.array[2][2];
        wm.array[0][2] = m.array[0][1]*m.array[1][2] - m.array[0][2]*m.array[1][1];

        wm.array[1][0] = m.array[1][2]*m.array[2][0] - m.array[1][0]*m.array[2][2];
        wm.array[1][1] = m.array[0][0]*m.array[2][2] - m.array[0][2]*m.array[2][0];
        wm.array[1][2] = m.array[0][2]*m.array[1][0] - m.array[0][0]*m.array[1][2];

        wm.array[2][0] = m.array[1][0]*m.array[2][1] - m.array[1][1]*m.array[2][0];
        wm.array[2][1] = m.array[0][1]*m.array[2][0] - m.array[0][0]*m.array[2][1];
        wm.array[2][2] = m.array[0][0]*m.array[1][1] - m.array[0][1]*m.array[1][0];
    }

    return( m2_smult((1/m2_determinant(m)), wm) );
}

//!  Determinant of a 2D matrix
/*! Return the determinant for a 2x2 or 3x3 2D Matrix
        \param m Square 2D matrix to calculate detrminant of
        \return determinant, otherwise NaN
*/
double m2_determinant(matrix2d m)
{
    double result;

    if (m.rows == 2 && m.cols == 2)
    {
        result = m.array[0][0]*m.array[1][1] - m.array[0][1]*m.array[1][0];
        return (result);
    }

    if (m.rows == 3 && m.cols == 3)
    {
        result = m.array[0][0] * (m.array[1][1] * m.array[2][2] - m.array[1][2] * m.array[2][1]);
        result += m.array[0][1] * (m.array[1][2] * m.array[2][0] - m.array[1][0] * m.array[2][2]);
        result += m.array[0][2] * (m.array[1][0] * m.array[2][1] - m.array[1][1] * m.array[2][0]);
        return (result);
    }

    return (NAN);
}

//! Compute the Euclidean norm of a 1D matrix
/*! Calculate and return the Euclidean norm of the provided 1D matrix.
        \param row 1D matrix to be normed
        \return the calculated norm
*/
double m1_norm(matrix1d row)
{
    double norm=0.;

    for (int i=0; i<row.cols; i++)
    {
        norm += row.vector[i] * row.vector[i];
    }

    return (sqrt(norm));
}

//! Create 2D zero matrix
/*! Create a matrix of the requested sized, set to zero.
        \param rows Number of rows in the matrix
        \param cols Number of columns in the matrix
        \return New 2D matrix
*/
matrix2d m2_zero(uint16_t rows, uint16_t cols)
{
    int i, j;
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

    if (rows <= 3 && cols <=3)
    {
        answer.rows = rows;
        answer.cols = cols;
        for (i=0; i<rows; i++)
        {
            for (j=0; j<cols; j++)
            {
                answer.array[i][j] = 0.;
            }
        }
    }

    return answer;
}

//! Create 2D identity matrix
/*! Create a square identity matrix of the requested size and return
 * as a new matrix.
        \param rows the size, in rows and columns, of the new matrix
        \return the new identity matrix, as a 2D matrix
*/

matrix2d m2_eye(uint16_t rows)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

    if (rows <= 3)
    {
        answer = m2_zero(rows,rows);
        answer.array[0][0] = answer.array[1][1] = answer.array[2][2] = 1.;
    }

    return (answer);
}

//! Multiply 2D matrix by a scalar
/*! Multiply each element of indicated 2D matrix by a scalar and
 * return as a new 2D matrix.
        \param number scalar to multiply by
        \param matrix 2D matrix to be multiplied
        \return 2D matrix result
*/
matrix2d m2_smult(double number, matrix2d matrix)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
    uint16_t i, j;

    answer = matrix;
    for (i=0; i<matrix.rows; i++)
    {
        for (j=0; j<matrix.cols; j++)
        {
            answer.array[i][j] *= number;
        }
    }
    return (answer);
}

//! Add one matrix2d to another
/*! Add each element in the second matrix to each element in
 * the first matrix, returning the answer as a new matrix.
        \param matrix1 matrix to be added from
        \param matrix2 matrix to add
        \return sum, as newly created 2D matrix
*/
matrix2d m2_add(matrix2d matrix1, matrix2d matrix2)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
    uint16_t i, j;

    if (matrix1.rows == matrix2.rows && matrix1.cols == matrix2.cols)
    {
        answer.rows = matrix1.rows;
        answer.cols = matrix1.cols;
        for (i=0; i<matrix1.rows; i++)
        {
            for (j=0; j<matrix1.cols; j++)
            {
                answer.array[i][j] = matrix1.array[i][j] + matrix2.array[i][j];
            }
        }
    }

    return (answer);
}

//! Subtract one matrix2d from another
/*! Subtract each element in the second matrix from each element in
 * the first matrix, returning the answer as a new matrix.
        \param matrix1 matrix to be subtracted from
        \param matrix2 matrix to subtract
        \return difference, as newly created 2D matrix
*/
matrix2d m2_sub(matrix2d matrix1, matrix2d matrix2)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
    uint16_t i, j;

    if (matrix1.rows == matrix2.rows && matrix1.cols == matrix2.cols)
    {
        answer.rows = matrix1.rows;
        answer.cols = matrix1.cols;
        for (i=0; i<matrix1.rows; i++)
        {
            for (j=0; j<matrix1.cols; j++)
            {
                answer.array[i][j] = matrix1.array[i][j] - matrix2.array[i][j];
            }
        }
    }

    return (answer);
}

//! Return transpose of a 2D matrix
/*! Generate a new ::matrix2d that is is the transpose of the provided ::matrix2d
        \param matrix ::matrix2d to be transposed
        \return Transpose, as new ::matrix2d
*/
matrix2d m2_transpose(matrix2d matrix)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
    uint16_t i, j;

    if (matrix.rows <=3 && matrix.cols <= 3)
    {
        answer.rows = matrix.cols;
        answer.cols = matrix.rows;
        for (i=0; i<matrix.rows; i++)
        {
            for (j=0; j<matrix.cols; j++)
            {
                answer.array[j][i] = matrix.array[i][j];
            }
        }
    }

    return (answer);
}

//! Unskew 3x3 2D matrix
/*! Create the 3 element 1D matrix correponding to a 3x3 skew symmetric matrix
        \param matrix 3x3 2D skew symmetric matrix
        \return ::matrix1d representing unskewed elements
*/
matrix1d m2_unskew(matrix2d matrix)
{
    matrix1d answer={{0.,0.,0.,0.},0};

    if (matrix.rows == 3 && matrix.cols == 3)
    {
        answer.cols = 3;
        answer.vector[0] = -matrix.array[1][2];
        answer.vector[1] = matrix.array[0][2];
        answer.vector[2] = -matrix.array[0][1];
    }

    return (answer);
}

//! Calculate the trace of a 2D matrix
/*! Add the diagonal elements of an nxn 2D matrix.
        \param matrix nxn matrix to take the elements from
        \return Sum of diagonals, otherwise 0.0
*/
double m2_trace(matrix2d matrix)
{
    double answer=0.;
    uint16_t i;

    for (i=0; i<matrix.rows; i++)
    {
        answer += matrix.array[i][i];
    }

    return (answer);
}

//! Matrix product
/*! Calculate the matrix product of two 2D matrices and return a new 2D matrix
        \param matrix1 mxn matrix
        \param matrix2 nxm matrix
        \return new matrix, of dimension rows x cols, otherwise NULL
*/
matrix2d m2_mmult(matrix2d matrix1, matrix2d matrix2)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
    uint16_t i, j, k;

    if (matrix1.cols == matrix2.rows && matrix1.rows <= 3 && matrix1.cols <=3 && matrix2.rows <= 3 && matrix2.cols <=3)
    {
        answer.rows = matrix1.rows;
        answer.cols = matrix2.cols;
        for (i=0; i<matrix1.rows; i++)
        {
            for (j=0; j<matrix2.cols; j++)
            {
                answer.array[i][j] = 0.;
                for (k=0; k<matrix1.cols; k++)
                {
                    answer.array[i][j] += matrix1.array[i][k] * matrix2.array[k][j];
                }
            }
        }
    }

    return (answer);
}

//! rmatrix from rmatrix
/*! Converts 3x3 matrix in row order form to cartesian form.
        \param matrix Row major matrix to convert
        \return Converted matrix in row order form
*/
matrix2d m2_from_rm(rmatrix matrix)
{
    matrix2d m2;
    int i, j;

    for (i=0; i<3; i++)
    {
        for (j=0; j<3; j++)
        {
            m2.array[i][j] = matrix.row[i].col[j];
        }
    }

    return (m2);
}

// Cmatrix3x3 to matrix2d
/*! Convert a 3x3 Cartesian matrix to a matrix2d.
        \param matrix cmatrix to convert
        \return matrix2d
*/
matrix2d cm3x3_to_m2(cmatrix matrix)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

    answer.rows = 3;
    answer.cols = 3;
    answer.array[0][0] = matrix.r1.x;
    answer.array[0][1] = matrix.r1.y;
    answer.array[0][2] = matrix.r1.z;
    answer.array[1][0] = matrix.r2.x;
    answer.array[1][1] = matrix.r2.y;
    answer.array[1][2] = matrix.r2.z;
    answer.array[2][0] = matrix.r3.x;
    answer.array[2][1] = matrix.r3.y;
    answer.array[2][2] = matrix.r3.z;

    return (answer);
}

// Cvector to matrix1d
/*! Convert a Cartesian vector to a matrix1d.
        \param vector cvector to convert
        \return Row vector
*/
matrix1d cv_to_m1(cvector vector)
{
    matrix1d answer={{0.,0.,0.,0.},0};

    answer.cols = 3;
    answer.vector[0] = vector.x;
    answer.vector[1] = vector.y;
    answer.vector[2] = vector.z;

    return (answer);
}

// Cvector to matrix2d
/*! Convert a Cartesian vector to either a row or column matrix2d.
        \param vector cvector to convert
        \param direction column if 1 (DIRECTION_COLUMN), otherwise row
        \return Row or column matrix
*/
matrix2d cv_to_m2(cvector vector,int direction)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

    if (direction == DIRECTION_COLUMN)
    {
        answer.rows = 3;
        answer.cols = 1;
        answer.array[0][0] = vector.x;
        answer.array[1][0] = vector.y;
        answer.array[2][0] = vector.z;
    }
    else
    {
        answer.rows = 1;
        answer.cols = 3;
        answer.array[0][0] = vector.x;
        answer.array[0][1] = vector.y;
        answer.array[0][2] = vector.z;
    }
    return (answer);
}

//! Matrix1d to matrix2d
/*! Convert a matrix1d vector to a row element matrix2d
        \param vector Matrix1d to be converted
        \param direction Alignment, column order if 1 (DIRECTION_COLUMN), otherwise row
        order.
        \return Single row or column matrix2d
*/
matrix2d m1_to_m2(matrix1d vector,int direction)
{
    matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
    uint16_t i;

    switch (direction)
    {
    case DIRECTION_COLUMN:
        answer.cols = 1;
        answer.rows = vector.cols;
        for (i=0; i<vector.cols; i++)
        {
            answer.array[i][0] = vector.vector[i];
        }
        break;
        break;
    default:
        answer.rows = 1;
        answer.cols = vector.cols;
        for (i=0; i<vector.cols; i++)
        {
            answer.array[0][i] = vector.vector[i];
        }
        break;
    }

    return (answer);
}

//! Eigen values of a 2x2 square matrix
/*! Calculate the 2 element 1D matrix that is the set of eigenvalues of a 2x2 2D matrix
        \param matrix 2x2 2D matrix
        \return 2 element 1D matrix of eigenvalues, otherwise NULL
*/
matrix1d m2_eig2x2(matrix2d matrix)
{
    matrix1d answer={{0.,0.,0.,0.},0};
    double trm, detm;
    double trm2, detm4;

    if (matrix.cols == 2 && matrix.rows == 2)
    {
        trm = matrix.array[0][0] + matrix.array[1][1];
        detm = matrix.array[0][0]*matrix.array[1][1] - matrix.array[0][1]*matrix.array[1][0];

        if ((trm2=trm*trm) < (detm4=4*detm))
            return (answer);

        answer.cols = 2;
        answer.vector[0] = (trm + sqrt(trm2-detm4))/2.;
        answer.vector[1] = (trm - sqrt(trm2-detm4))/2.;
    }

    return (answer);
}

//! Spectral norm of a 2x2 matrix
/*! Calculate the spectral norm of the provided 2x2 matrix.
        \param matrix 2x2 matrix
        \return spectral norm, otherwise NaN
*/
double m2_snorm2x2(matrix2d matrix)
{
    double answer=NAN;
    matrix1d re;

    if (matrix.cols == 2 && matrix.rows == 2)
    {
        re = m2_eig2x2(m2_mmult(m2_transpose(matrix),matrix));

        if (re.cols == 0)
            return (NAN);

        if (re.vector[0] > re.vector[1])
            answer = sqrt(re.vector[0]);
        else
            answer = sqrt(re.vector[1]);
    }

    return (answer);
}


//! @}


std::ostream& operator << (std::ostream& out, const rmatrix& a)
{
    out.precision(15);
    out << "["  << a.row[0].col[0] << "," << a.row[0].col[1] << "," << a.row[0].col[2] << ";";
    out << a.row[1].col[0] << "," << a.row[1].col[1] << "," << a.row[1].col[2] << ";";
    out << a.row[2].col[0] << "," << a.row[2].col[1] << "," << a.row[2].col[2] << "]";
    return out;
}

std::ostream& operator << (std::ostream& out, const vector<rmatrix>& a)
{
    for(vector<rmatrix>::const_iterator I = a.begin(); I != a.end(); ++I)
        out<<*I<<"\t";
    return out;
}

std::istream& operator >> (std::istream& in, rmatrix& a)
{
    char junk;
    in >> junk >> a.row[0].col[0] >> junk >> a.row[0].col[1] >> junk >> a.row[0].col[2] >> junk
            >> a.row[1].col[0] >> junk >> a.row[1].col[1] >> junk >> a.row[1].col[2] >> junk
            >> a.row[2].col[0] >> junk >> a.row[2].col[1] >> junk >> a.row[2].col[2] >> junk;
    return in;
}



std::ostream& operator << (std::ostream& out, const cmatrix& a)
{
    out << "["  << a.r1.x << "," << a.r1.y << "," << a.r1.z << ";\n"
        << a.r2.x << "," << a.r2.y << "," << a.r2.z << ";\n"
        << a.r3.x << "," << a.r3.y << "," << a.r3.z << "]";
    return out;
}

std::istream& operator >> (std::istream& in, cmatrix& a)
{
    char junk;
    in >> junk >> a.r1.x >> junk >> a.r1.y >> junk >> a.r1.z >> junk
            >> a.r2.x >> junk >> a.r2.y >> junk >> a.r2.z >> junk
            >> a.r3.x >> junk >> a.r3.y >> junk >> a.r3.z >> junk;
    return in;
}

namespace Cosmos {
    namespace Math {
        namespace Matrices {

            //! Scalar product
            /*!
             * \brief Scalar ::Matrix multiplication.
             * \param a Scalar to multiply each element by.
             * \return Scalar product ::Matrix.
             */
//            Matrix Matrix::smult(const double scale) const
//            {
//                return *this * scale;
//            }

            //! Scalar product.
            /*! Calculate the scalar product with the provided scale.
         * \param scale Scale to multiply by.
         * \return This times b.
        */
            Matrix Matrix::operator * (const double scale) const
            {
                Matrix mo = *this;

                mo *= scale;

                return mo;
            }

            //! Compound Scalar product.
            /*! Calculate the scalar product with the provided scale inline.
         * \param scale Scale to multiply by.
         * \return Reference to this times scale.
        */
            Matrix &Matrix::operator *= (const double scale)
            {
                // If scale is basically 1., don't do anything
                if (fabs(scale - (double)1.) > D_SMALL)
                {
                    // If scale is basically 0., set to zero
                    if (fabs(scale - (double)0.) > D_SMALL)
                    {
                        // Otherwise, multiply
                        this->r0 *= scale;
                        this->r1 *= scale;
                        this->r2 *= scale;
                    }
                    else
                    {
                        this->r0 = Vector();
                        this->r1 = Vector();
                        this->r2 = Vector();
                    }
                }
                return *this;
            }

            //! Reverse scalar product.
            /*! Calculate the scalar product with the provided scale.
         * \param scale Scale to multiply by.
         * \return Scale times this.
        */
            Matrix operator * (const double scale, const Matrix &m)
            {
                return m * scale;
            }

            //! Vector product.
            /*! Calculate the vector product with the provided vector.
         * \param v Vector to multiply by.
         * \return This times v.
        */
            Vector Matrix::operator * (const Vector v) const
            {
                Vector o;

                o[0] =  (this->r0 * v).sum();
                o[1] =  (this->r1 * v).sum();
                o[2] =  (this->r2 * v).sum();

                return o;
            }

            //! Multiply ::Matrix by ::Vector.
            /*! Multiply 3x3 ::Matrix by 3 element ::Vector (treated as a column order vector).
                    \param m matrix to multiply by, in ::Matrix form.
                    \param v vector to be tranformed, in ::Vector form.
                    \return multiplied vector, in ::Vector format.
            */
//            Vector Matrix::mmult(const Vector v) const
//            {
//               return *this * v;
//            }

            //! Compound Matrix Product
            /*! Multiply two 3x3 matrices together inline.
                    \param a first 3x3 matrix
                    \param b second 3x3 matrix
                    \return reference to product 3x3 matrix
            */
            Matrix &Matrix::operator *= (const Matrix &m)
            {
                Matrix mat;

                mat.r0.x  = this->r0.x * m.r0.x + this->r0.y * m.r1.x + this->r0.z * m.r2.x;
                mat.r0.y  = this->r0.x * m.r0.y + this->r0.y * m.r1.y + this->r0.z * m.r2.y;
                mat.r0.z  = this->r0.x * m.r0.z + this->r0.y * m.r1.z + this->r0.z * m.r2.z;
                this->r0 = mat.r0;

                mat.r1.x  = this->r1.x * m.r0.x + this->r1.y * m.r1.x + this->r1.z * m.r2.x;
                mat.r1.y  = this->r1.x * m.r0.y + this->r1.y * m.r1.y + this->r1.z * m.r2.y;
                mat.r1.z  = this->r1.x * m.r0.z + this->r1.y * m.r1.z + this->r1.z * m.r2.z;
                this->r1 = mat.r1;

                mat.r2.x  = this->r2.x * m.r0.x + this->r2.y * m.r1.x + this->r2.z * m.r2.x;
                mat.r2.y  = this->r2.x * m.r0.y + this->r2.y * m.r1.y + this->r2.z * m.r2.y;
                mat.r2.z  = this->r2.x * m.r0.z + this->r2.y * m.r1.z + this->r2.z * m.r2.z;
                this->r2 = mat.r2;

                return *this;

            }

            //! Matrix Product
            //! Multiply two matrices together.
            //! \param m Matrix to multiply by
            //! \return Product
            Matrix Matrix::operator * (const Matrix &m) const
            {
                Matrix mo = *this;

                mo *= m;

                return mo;
            }

            //! Matrix Product
            //! Multiply two matrices together.
            //! \param b Matrix to multiply by
            //! \return Product
            Matrix Matrix::mmult(const Matrix &m) const
            {
                return *this * m;
            }

            /*!
             * \brief Element-wise ::Matrix multiplication.
             * \param a first ::Matrix.
             * \param b second ::Matrix.
             * \return Element-wise product ::Matrix.
             */
            Matrix Matrix::mult(const Matrix &m) const
            {
                Matrix mat;

                mat.r0 = this->r0 * m.r0;
                mat.r1 = this->r0 * m.r1;
                mat.r2 = this->r2 * m.r2;

                return (mat);
            }

            //! Compound Matrix Sum
            /*! Add two 3x3 matrices together inline.
                    \param a first 3x3 matrix
                    \param b second 3x3 matrix
                    \return reference to sum 3x3 matrix
            */
            Matrix &Matrix::operator += (const Matrix &m)
            {
                this->r0 += m.r0;
                this->r1 += m.r1;
                this->r2 += m.r2;

                return *this;

            }

            //! Matrix Sum
            //! Add two matrices together.
            //! \param m Matrix to add by
            //! \return Sum
            Matrix Matrix::operator + (const Matrix &m) const
            {
                Matrix mo = *this;

                mo += m;

                return mo;
            }

            //! Index.
            /*! Lookup ::Vector value by index.
         * \param index Indexed location to look up.
         * \return Value at indexed location.
        */
            Vector &Matrix::operator[] (const int &index)
            {
                switch (index)
                {
                case 0:
                default:
                    return r0;
                    break;
                case 1:
                    return r1;
                    break;
                case 2:
                    return r2;
                    break;
                }
            }

            //! Unskew 3x3 row matrix
            /*! Create the 3 element ::rvector correponding to a 3x3 skew symmetric ::rmatrix
                    \param matrix 3x3 row skew symmetric matrix
                    \return ::Vector representing unskewed elements
            */
            Vector Matrix::unskew()
            {
                Vector vec;

                vec[0] = -this->r1[2];
                vec[1] = this->r0[2];
                vec[2] = -this->r0[1];

                return vec;
            }

            //! Matrix diagonal
            /*! ::Vector representing the diagonal of a ::Matrix
                    \param a :Matrix to get diagonal from.
                    \return Diagonal
            */
            Vector Matrix::diag()
            {
                Vector vec;

                vec[0] = this->r0[0];
                vec[1] = this->r1[1];
                vec[2] = this->r2[2];

                return vec;
            }

            Matrix Matrix::transpose()
            {
                Matrix b;

                b[0][0] = (*this)[0][0];
                b[0][1] = (*this)[1][0];
                b[0][2] = (*this)[2][0];

                b[1][0] = (*this)[0][1];
                b[1][1] = (*this)[1][1];
                b[1][2] = (*this)[2][1];

                b[2][0] = (*this)[0][2];
                b[2][1] = (*this)[1][2];
                b[2][2] = (*this)[2][2];

                return b;
            }

            Matrix Matrix::square()
            {
                Matrix b;
                Vector c0 = this->c0();
                Vector c1 = this->c1();
                Vector c2 = this->c2();

                for (uint16_t i=0; i<3; ++i)
                {
                    b[i][0] = ((*this)[i] * c0).sum();
                    b[i][1] = ((*this)[i] * c1).sum();
                    b[i][2] = ((*this)[i] * c2).sum();
                }
                return b;
            }

           Matrix Matrix::diag(Vector v)
            {
                Matrix mat;

                mat[0][0] = v[0];
                mat[1][1] = v[1];
                mat[2][2] = v[2];

                return mat;
            }

           Matrix Matrix::skew(Vector v)
           {
                Matrix mat;

                mat[0][1] = -v[2];
                mat[0][2] = v[1];

                mat[1][0] = v[2];
                mat[1][2] = -v[0];

                mat[2][0] = -v[1];
                mat[2][1] = v[0];

                return mat;
           }

           Matrix Matrix::inverse()
           {
               Matrix mat;

               mat[0][0] = (*this)[1][1] * (*this)[2][2] - (*this)[1][2] * (*this)[2][1];
               mat[0][1] = (*this)[0][2] * (*this)[2][1] - (*this)[0][1] * (*this)[2][2];
               mat[0][2] = (*this)[0][1] * (*this)[1][2] - (*this)[0][2] * (*this)[1][1];

               mat[1][0] = (*this)[1][2] * (*this)[2][0] - (*this)[1][0] * (*this)[2][2];
               mat[1][1] = (*this)[0][0] * (*this)[2][2] - (*this)[0][2] * (*this)[2][0];
               mat[1][2] = (*this)[0][2] * (*this)[1][0] - (*this)[0][0] * (*this)[1][2];

               mat[2][0] = (*this)[1][0] * (*this)[2][1] - (*this)[1][1] * (*this)[2][0];
               mat[2][1] = (*this)[0][1] * (*this)[2][0] - (*this)[0][0] * (*this)[2][1];
               mat[2][2] = (*this)[0][0] * (*this)[1][1] - (*this)[0][1] * (*this)[1][0];

               mat *= 1. / this->determinant();

               return mat;
           }

           //!  Determinant of row column matrix
           /*! Return the determinant for a 3x3 ::rmatrix
                   \param m ;;rmatrix to calculate detrminant of
                   \return determinant, otherwise NaN
           */
           double Matrix::determinant()
           {
               double result;

               result = (*this)[0][0] * ((*this)[1][1] * (*this)[2][2] - (*this)[2][1] * (*this)[1][2]);
               result -= (*this)[1][0] * ((*this)[0][1] * (*this)[2][2] - (*this)[0][2] * (*this)[2][1]);
               result += (*this)[2][0] * ((*this)[0][1] * (*this)[1][2] - (*this)[0][2] * (*this)[1][1]);

               return (result);

           }

           //! Scaled Identity ::Matrix
           /*! ::Matrix with diagonal elements set to one and all others set to zero
                   \return Identity matrix
           */
           Matrix eye(double scale)
           {
                Matrix mat;
                mat[0][0] = scale;
                mat[1][1] = scale;
                mat[2][2] = scale;

                return mat;
           }

        }

    }
}
