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

#include "rotation.h"

//! \addtogroup mathlib_functions
//! @{

//! Quaternion to Direction Cosine Matrix
//! This function expects a quaternion that represents the coordinate frame transformation not the rotation.
//! If the quaternion represents the rotation from the inertial reference frame
//! into the frame of the sensor/body then this DCM will represent
//! the rotation from the sensor body frame (B) to the inertial frame (I)
//! TODO: later this should be changed to be more consistent
/*! Convert supplied quaternion to an equivalent direction cosine matrix
        \param q quaternion
        \return direction cosine matrix
*/

// TODO: create a matrix type (apart from cmatrix and rmatrix)
// TODO: rename function simply to quaternion2dcm or quaternionToRotationMatrix
// TODO: check consistency with ST ICD (this DCM is transpose)
// TODO: use kuipers notation, see pg 126
// Ref: Star Tracker ICD sec 9: http://www.sinclairinterplanetary.com/startrackers/ICD%201.23.docx
cmatrix cm_quaternion2dcm(quaternion q)
{
    cmatrix m;
    double yy, xx, zz, xy, xz, xw, yz, yw, zw;

    normalize_q(&q);

    xx = 2. * q.d.x;
    xy = xx * q.d.y;
    xz = xx * q.d.z;
    xw = xx * q.w;
    xx *= q.d.x;

    yy = 2. * q.d.y;
    yz = yy * q.d.z;
    yw = yy * q.w;
    yy *= q.d.y;

    zz = 2. * q.d.z;
    zw = zz * q.w;
    zz *= q.d.z;

    // first row
    m.r1.x = 1. - yy - zz; // 1 - 2(qy^2 + qz^2) == 1 - 2(q2^ + q3^2)
    m.r1.y = xy - zw;      // 2*(qx*qy - qw*qz)
    m.r1.z = xz + yw;      // 2*(qx*qz + qw*qy)

    // second row
    m.r2.x = xy + zw;      // 2*(qx*qy + qw*qz)   == 2(q1q2 + q0q3)
    m.r2.y = 1. - xx - zz; // 1 - 2*(qz^2 + qx^2) == 1 - 2(q3^2 + q1^2)
    m.r2.z = yz - xw;      // 2*(qy*qz - qw*qx)   == 2(q2q3 - q0q1)

    // third row
    m.r3.x = xz - yw;      // 2(qx*qz - qw*qy)    == 2(q1q3 - q0q2)
    m.r3.y = yz + xw;      // 2(qy*qz + qw*qx)    == 2(q2q3 + q0q1)
    m.r3.z = 1. - xx - yy; // 1 - 2(qx^2 + qy^2)  == 1 - 2(q1^2 + q2^2)

    return (m);
}

//! Direction Cosine Matrix to Quaternion
/*! Convert the given DCM to an equivalent quaternion
    \param dcm direction cosine matrix
    \return q quaternion
*/

// Ref:
// - Kuipers pg. 169
// - https://en.wikipedia.org/wiki/Rotation_formalisms_in_three_dimensions
quaternion q_dcm2quaternion_cm(cmatrix dcm)
{
    quaternion q;
    double t, tr;

    // removed trace_cm(dcm) to clear the dependencies from mathlib
    // computing the trace in place
    tr = dcm.r1.x + dcm.r2.y + dcm.r3.z; //trace_cm(dcm)

    // TODO: explain the different cases
    if (tr > 0.)
    {
        // TODO: simplify
        // q.w = 0.5 * sqrt(1 + tr)
        t = .5 / sqrt(1.+tr);
        q.w = .25 / t;

        // TODO: check because Kuipers does the inverse here
        q.d.x = t*(dcm.r3.y - dcm.r2.z);
        q.d.y = t*(dcm.r1.z - dcm.r3.x);
        q.d.z = t*(dcm.r2.x - dcm.r1.y);
    }
    else
    {
        if (dcm.r1.x > dcm.r2.y && dcm.r1.x > dcm.r3.z)
        {
            t = 2. * sqrt(1. + dcm.r1.x - dcm.r2.y - dcm.r3.z);
            q.w = (dcm.r3.y - dcm.r2.z) / t;
            q.d.x = .25 * t;
            q.d.y = (dcm.r1.y + dcm.r2.x) / t;
            q.d.z = (dcm.r1.z + dcm.r3.x) / t;
        }
        else
        {
            if (dcm.r2.x > dcm.r3.z)
            {
                t = 2. * sqrt(1. + dcm.r2.y - dcm.r1.x - dcm.r3.z);
                q.w = (dcm.r1.z - dcm.r3.x) / t;
                q.d.x = (dcm.r1.y + dcm.r2.x) / t;
                q.d.y = .25 * t;
                q.d.z = (dcm.r2.z + dcm.r3.y) / t;
            }
            else
            {
                t = 2. * sqrt(1. + dcm.r3.z - dcm.r1.x - dcm.r2.y);
                q.w = (dcm.r2.x - dcm.r1.y) / t;
                q.d.x = (dcm.r1.z + dcm.r3.x) / t;
                q.d.y = (dcm.r2.z + dcm.r3.y) / t;
                q.d.z = .25 * t;
            }
        }
    }

    normalize_q(&q);
    return(q);
}

//! Create rotation matrix from 2 row vectors
/*! Generate the direction cosine matrix that represents a rotation from one row order vector
 * to a second row order vector.
        \param from initial row order vector
        \param to final row order vector
        \return direction cosine matrix, in row matrix form, that can be used to rotate points
*/
rmatrix rm_change_between_rv(rvector from, rvector to)
{
    rmatrix m = {{{{0.}}}};

    m = rm_quaternion2dcm(q_change_between_rv(from,to));

    return (m);
}

//! Row matrix DCM to Quaternion
/*! Convert Direction Cosine Matrix in row matrix form to a Quaternion.
        \param m Direction Cosine Matrix in rmatrix form
        \return Quaternion representing DCM.
*/
quaternion q_dcm2quaternion_rm(rmatrix m)
{
    quaternion q={{0.,0.,0.},0.};
    double t, tr;

    if ((tr=trace_rm(m)) > 0.)
    {
        t = .5 / sqrt(1.+tr);
        q.w = .25 / t;
        q.d.x = t*(m.row[2].col[1] - m.row[1].col[2]);
        q.d.y = t*(m.row[0].col[2] - m.row[2].col[0]);
        q.d.z = t*(m.row[1].col[0] - m.row[0].col[1]);
    }
    else
    {
        if (m.row[0].col[0] > m.row[1].col[1] && m.row[0].col[0] > m.row[2].col[2])
        {
            t = 2. * sqrt(1. + m.row[0].col[0] - m.row[1].col[1] - m.row[2].col[2]);
            q.w = (m.row[2].col[1] - m.row[1].col[2]) / t;
            q.d.x = .25 * t;
            q.d.y = (m.row[0].col[1] + m.row[1].col[0]) / t;
            q.d.z = (m.row[0].col[2] + m.row[2].col[0]) / t;
        }
        else
        {
            if (m.row[1].col[0] > m.row[2].col[2])
            {
                t = 2. * sqrt(1. + m.row[1].col[1] - m.row[0].col[0] - m.row[2].col[2]);
                q.w = (m.row[0].col[2] - m.row[2].col[0]) / t;
                q.d.x = (m.row[0].col[1] + m.row[1].col[0]) / t;
                q.d.y = .25 * t;
                q.d.z = (m.row[1].col[2] + m.row[2].col[1]) / t;
            }
            else
            {
                t = 2. * sqrt(1. + m.row[2].col[2] - m.row[0].col[0] - m.row[1].col[1]);
                q.w = (m.row[1].col[0] - m.row[0].col[1]) / t;
                q.d.x = (m.row[0].col[2] + m.row[2].col[0]) / t;
                q.d.y = (m.row[1].col[2] + m.row[2].col[1]) / t;
                q.d.z = .25 * t;
            }
        }
    }

    normalize_q(&q);
    return(q);
}



// this is same function as dot_cv from mathlib.h
// TODO: when mathlib has been breaken appart then use the math module
// that computes th dotProduct
double DCM::dotProduct(cvector a, cvector b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

// this is same function as cm_transpose from mathlib.h
// TODO: when mathlib has been broken appart then use the math module
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

cmatrix DCM::base2_from_base1(basisOrthonormal base2, basisOrthonormal base1){

    // compute dcm matrix (A) to represent vector in base 2 coordinates

    // References
    // - Quaternion and Rotation Sequences, Kuipers, pg 161 eq 7.8 (I think these
    //   formulas are wrong in the book! they are inversed, must check)
    // - http://people.ae.illinois.edu/tbretl/ae403/handouts/06-dcm.pdf (this
    //   reference seems to be sound)
    // - http://www.starlino.com/dcm_tutorial.html (eq 1.4)

    // example: vector_body_coordinates = dcm_base2_from_base1 * vector_inertial_coodinates

    // Notes:
    // - matrix A represents a frame rotation that relates the initial reference
    //   frame {X,Y,Z} to a rotated frame {x,y,z}: x = AX. Example, appliying
    //   the operation AX - where X is for instance a vector in the inertial
    //   frame - results in the coordinates of that vector in the new frame {x,y,z}
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

    // normalize the basis just in case

    base1.normalize();
    base2.normalize();


    return  {
        { dotProduct(base1.i, base2.i) , dotProduct(base1.j, base2.i), dotProduct(base1.k, base2.i) },
        { dotProduct(base1.i, base2.j) , dotProduct(base1.j, base2.j), dotProduct(base1.k, base2.j) },
        { dotProduct(base1.i, base2.k) , dotProduct(base1.j, base2.k), dotProduct(base1.k, base2.k) }
    };
}

// TODO: rename base to basis
// ref: https://en.wikipedia.org/wiki/Basis_(linear_algebra)
cmatrix DCM::base1_from_base2(basisOrthonormal base1, basisOrthonormal base2){
    // compute dcm matrix (A) to represent vector given in base 2
    // in base 1 coordinates

    // example:
    // vector_inertial_coordinates = dcm_base1_from_base2 * vector_body_coodinates

    // TODOs
    // - add validation step to verify if the bases are orthogonal

    // just compute the transpose
    return transposeMatrix(base2_from_base1(base2,base1));
}


void basisOrthonormal::normalize()
{
    this->i.normalize();
    this->j.normalize();
    this->k.normalize();
}

//! @}



