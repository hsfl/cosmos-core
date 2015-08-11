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

// TODO: convert to class
#include "math/quaternion.h"


double q_norm(quaternion q)
{
    return length_q(q);
}

void q_normalize(quaternion *q)
{
    double mag;

    q->w = round(q->w/D_SMALL)*D_SMALL;
    q->d.x = round(q->d.x/D_SMALL)*D_SMALL;
    q->d.y = round(q->d.y/D_SMALL)*D_SMALL;
    q->d.z = round(q->d.z/D_SMALL)*D_SMALL;

    mag = q->w * q->w + q->d.x * q->d.x + q->d.y * q->d.y + q->d.z * q->d.z;

    if (fabs(mag - (double)0.) > D_SMALL && fabs(mag - (double)1.) > D_SMALL)
    {
        mag = sqrt(mag);
        q->w /= mag;
        q->d.x /= mag;
        q->d.y /= mag;
        q->d.z /= mag;
    }
}

//! Zero quaternion
/*! Creates a quaternion filled with zeros.
        \return a ::quaternion of zeros
*/
quaternion q_zero()
{
    quaternion q={{0.,0.,0.},0.};

    return (q);
}

quaternion q_conjugate(quaternion q)
{
    quaternion o;

    o.w = q.w;
    o.d.x = -q.d.x;
    o.d.y = -q.d.y;
    o.d.z = -q.d.z;

    return (o);
}

//! Multiply the elements of 2 quaternions
/*! Multiply each element of one quaternion by the same element of the other.
 * \param q1 First quaternion
 * \param q2 second quaternion
 * \return Quaternion result
 */
quaternion q_times(quaternion q1, quaternion q2)
{
    quaternion o;

    o.d.x = q1.d.x * q2.d.x;
    o.d.y = q1.d.y * q2.d.y;
    o.d.z = q1.d.z * q2.d.z;
    o.w = q1.w * q2.w;

    return (o);
}

//! rvector quaternion muliply
/*! Treat an ::rvector as a ::quaternion with scalar set to zero, then
 * quaternion multiply the rvector by the quaternion.
 * \param r1 ::rvector as first quaternion.
 * \param q2 second quaternion
 * \return Quaternion result
 */
quaternion q_mult(rvector r1, quaternion q2)
{
    quaternion o;

    o.d.x = r1.col[0] * q2.w + r1.col[1] * q2.d.z - r1.col[2] * q2.d.y;
    o.d.y = r1.col[1] * q2.w + r1.col[2] * q2.d.x - r1.col[0] * q2.d.z;
    o.d.z = r1.col[2] * q2.w + r1.col[0] * q2.d.y - r1.col[1] * q2.d.x;
    o.w = - r1.col[0] * q2.d.x - r1.col[1] * q2.d.y - r1.col[2] * q2.d.z;

    return (o);
}

//! Quaternion multiplication. The result of moltuplying two quaternions is the
//! composition of two rotations (not commutative).
/*! Quaternion multiply two quaternions.
 * \param q1 First quaternion
 * \param q2 Second quaternion
 * \return Quaternion result.
 */
quaternion q_mult(quaternion q1, quaternion q2)
{
    quaternion o;

    o.d.x = q1.w * q2.d.x + q1.d.x * q2.w + q1.d.y * q2.d.z - q1.d.z * q2.d.y;
    o.d.y = q1.w * q2.d.y + q1.d.y * q2.w + q1.d.z * q2.d.x - q1.d.x * q2.d.z;
    o.d.z = q1.w * q2.d.z + q1.d.z * q2.w + q1.d.x * q2.d.y - q1.d.y * q2.d.x;
    o.w = q1.w * q2.w - q1.d.x * q2.d.x - q1.d.y * q2.d.y - q1.d.z * q2.d.z;

    return (o);
}

//! Multiply quaternion by scalar
/*! Multiply a 3 element quaternion by a double precision scalar.
        \param b quaternion to be tranformed, in ::quaternion form
        \param a double precision scalar to multiply by
        \return the transformed quaternion, in ::quaternion form
*/
quaternion q_smult(double a, quaternion b)
{
    quaternion c;

    c.w = a * b.w;
    c.d.x = a * b.d.x;
    c.d.y = a * b.d.y;
    c.d.z = a * b.d.z;
    return (c);
}

//! Add two quaternions
/*! Add two quaternions in ::quaternion form, returning a ::quaternion.
        \param a first quaternion to be added, in ::quaternion form
        \param b second quaternion to be added, in ::quaternion form
        \result the transformed quaternion, in ::quaternion form
*/
quaternion q_add(quaternion a, quaternion b)
{
    quaternion c;

    c.w = a.w + b.w;
    c.d.x = a.d.x + b.d.x;
    c.d.y = a.d.y + b.d.y;
    c.d.z = a.d.z + b.d.z;
    return (c);
}

//! Subtract two quaternions
/*! Subtract two quaternions in ::quaternion form, returning a
 * ::quaternion.
        \param a quaternion to be subtracted from, in ::quaternion form
        \param b quaternion to be subtracted, in ::quaternion form
        \result the transformed quaternion, in ::quaternion form
*/
quaternion q_sub(quaternion a, quaternion b)
{
    quaternion c;

    c.w = a.w - b.w;
    c.d.x = a.d.x - b.d.x;
    c.d.y = a.d.y - b.d.y;
    c.d.z = a.d.z - b.d.z;
    return (c);
}

//! Rotate a row vector using a quaternion
/*! Rotate a row vector within one coordinate system using the
 * provided left quaternion.
        \param v row vector to be rotated
        \param q quaternion representing the rotation
        \return rotated row vector in the same system
*/
rvector rotate_q(quaternion q, rvector v)
{
    uvector t = {{{0.,0.,0.},0.}};

    //	t.q.d = ((uvector *)&v)->c;
    //	t.q.w = 0.0;

    //	t.q = q_mult(q,q_mult(t.q,q_conjugate(q)));
    t.q = q_mult(q,q_mult(v,q_conjugate(q)));

    return (t.r);
}

//! Transform a row vector using a quaternion
/*! Transform a row vector from one coordinate system to another using the
 * provided left quaternion.
        \param v row vector to be rotated
        \param q quaternion representing the transformation
        \return row vector in the transformed system
*/
rvector transform_q(quaternion q, rvector v)
{
    uvector t = {{{0.,0.,0.},0.}};

    //	t.q.d = ((uvector *)&v)->c;
    //	t.q.w = 0.0;

    //	t.q = q_mult(q_conjugate(q),q_mult(t.q,q));
    t.q = q_mult(q_conjugate(q),q_mult(v,q));

    return (t.r);
}

//! Rotate a cartesiian vector using a quaternion
/*! Rotate a cartesian vector from one coordinate system to another using the
 * provided quaternion.
        \param v cartesian vector to be rotated
        \param q quaternion representing the rotation
        \return cartesian vector in the rotated system
*/
cvector rotate_q(quaternion q, cvector v)
{
    uvector qt;
    quaternion qc;

    qt.c = v;
    qt.q.w = 0.0;

    qc = q_conjugate(q);
    qt.q = q_mult(qt.q,qc);
    qt.q = q_mult(q,qt.q);

    return (qt.c);
}

//! Create rotation matrix from 2 vectors
/*! Generate the direction cosine matrix that represents a rotation from one cartesian vector
 * to a second cartesian vector.
        \param from initial cartesian vector
        \param to final cartesian vector
        \return direction cosine matrix that can be used to rotate points
*/
cmatrix cm_change_between_cv(cvector from, cvector to)
{
    //cmatrix m;
    //m = cm_quaternion2dcm(q_change_between_cv(from,to));
    //return (m);

    return cm_quaternion2dcm(q_change_between_cv(from,to));
}

//! Create rotation quaternion from 2 vectors
/*! Generate the quaternion that represents a rotation of from one cartesian vector
 * to a second cartesian vector.
        \param from initial cartesian vector
        \param to final cartesian vector
        \return quaternion that can be used to rotate points
*/
quaternion q_change_between_cv(cvector from, cvector to)
{
    uvector rq;
    cvector vec1, vec2;

    normalize_cv(&from);
    normalize_cv(&to);

    if (length_cv(cv_add(from,to)) < 1e-14)
    {
        vec1.x = rand();
        vec1.y = rand();
        vec1.z = rand();
        normalize_cv(&vec1);
        vec2 = cv_cross(vec1,to);
        normalize_cv(&vec2);
        if (length_cv(vec2)<D_SMALL)
        {
            vec1.x = rand();
            vec1.y = rand();
            vec1.z = rand();
            normalize_cv(&vec1);
            vec2 = cv_cross(vec1,to);
            normalize_cv(&vec2);
        }
        rq.c = vec2;
        rq.q.w = 0.;
    }
    else
    {
        rq.c = cv_cross(from,to);
        rq.q.w = 1. + dot_cv(from,to);
    }

    q_normalize(&rq.q);
    return (rq.q);
}

//! Create rotation quaternion from axis and angle
/*! Generate the quaternion that represents a rotation of the specified angle
 * around the specified axis.
        \param around cartesian vector around which the rotation will occur
        \param angle amount of rotation in radians
        \return quaternion that can be used to rotate points
*/
quaternion q_change_around_cv(cvector around, double angle)
{
    double sa;
    quaternion rq;

    angle /= 2.;
    sa = sin(angle);
    normalize_cv(&around);

    rq.d.x = around.x * sa;
    rq.d.y = around.y * sa;
    rq.d.z = around.z * sa;
    rq.w = cos(angle);
    q_normalize(&rq);
    return (rq);
}

// TODO: explain
void qrotate(double ipos[3], double rpos[3], double angle, double *opos)
{
    double q1, q2, q3, q4, length, sa;
    double q11, q12, q13, q14, q22, q23, q24, q33, q34;

    length = sqrt(rpos[0]*rpos[0]+rpos[1]*rpos[1]+rpos[2]*rpos[2]);
    if (length>0.)
    {
        rpos[0] /= length;
        rpos[1] /= length;
        rpos[2] /= length;
    }

    sa = sin(angle/2.);
    q1 = rpos[0] * sa;
    q2 = rpos[1] * sa;
    q3 = rpos[2] * sa;
    q4 = cos(angle/2.);
    q11 = q1 * q1;
    q12 = q1 * q2;
    q13 = q1 * q3;
    q14 = q1 * q4;
    q22 = q2 * q2;
    q23 = q2 * q3;
    q24 = q2 * q4;
    q33 = q3 * q3;
    q34 = q3 * q4;

    opos[0] = ipos[0] *(1.-2.*(q22+q33)) + ipos[1] * 2.*(q12-q34) + ipos[2] * 2.*(q13+q24);
    opos[1] = ipos[0]*2.*(q12+q34)+ipos[1]*(1.-2.*(q11+q33))+ipos[2]*2.*(q23-q14);
    opos[2] = ipos[0]*2.*(q13-q24)+ipos[1]*2.*(q14+q23)+ipos[2]*(1.-2.*(q11+q22));

}

quaternion q_euler2quaternion(avector rpw)
{
    quaternion q;
    double sr, sp, sy, cr, cp, cy;

    sr = sin(rpw.b/2.);
    sp = sin(rpw.e/2.);
    sy = sin(rpw.h/2.);
    cr = cos(rpw.b/2.);
    cp = cos(rpw.e/2.);
    cy = cos(rpw.h/2.);

    q.d.x = sr * cp * cy - cr * sp * sy;
    q.d.y = cr * sp * cy + sr * cp * sy;
    q.d.z = cr * cp * sy - sr * sp * cy;
    q.w = (cr * cp * cy + sr * sp * sy);

    q_normalize(&q);

    return (q);
}

// TODO: validade and add reference
avector a_quaternion2euler(quaternion q)
{
    avector rpw;

    q_normalize(&q);
    /*
        rpw.b = atan2(q.d.y*q.d.z+q.w*q.d.x,q.w*q.w+q.d.z*q.d.z-.5);
        rpw.e = asin(-2.*(q.d.x*q.d.z-q.w*q.d.y));
        rpw.h = atan2(q.d.x*q.d.y+q.w*q.d.z,q.w*q.w+q.d.x*q.d.x-.5);
        */
    //	double sqw = q.w * q.w;
    double sqx = q.d.x * q.d.x;
    double sqy = q.d.y * q.d.y;
    double sqz = q.d.z * q.d.z;
    rpw.b = atan2((q.d.z*q.d.y + q.d.x*q.w), .5 - (sqx + sqy));
    rpw.e = asin(-2.*(q.d.x*q.d.z - q.d.y*q.w));
    rpw.h = atan2((q.d.x*q.d.y + q.d.z*q.w), .5 - (sqy + sqz));

    return (rpw);
}

cvector cv_quaternion2axis(quaternion q)
{
    cvector v;
    double ca, sa;

    ca = 2.*acos(q.w);
    if (ca > 0.)
    {
        sa = sin(ca/2.);
        v.x = ca*(q.d.x/sa);
        v.y = ca*(q.d.y/sa);
        v.z = ca*(q.d.z/sa);
    }
    else
        v = cv_zero();

    return (v);
}

//! Quaternion to Direction Cosine Matrix
/*! Convert supplied quaternion to an equivalent direction cosine matrix
        \param q quaternion
        \return direction cosine matrix
*/

cmatrix cm_quaternion2dcm(quaternion q)
{
    cmatrix m;
    double yy, xx, zz, xy, xz, xw, yz, yw, zw;

    q_normalize(&q);

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

    m.r1.x = 1. - yy - zz;
    m.r1.y = xy - zw;
    m.r1.z = xz + yw;
    m.r2.x = xy + zw;
    m.r2.y = 1. - xx - zz;
    m.r2.z = yz - xw;
    m.r3.x = xz - yw;
    m.r3.y = yz + xw;
    m.r3.z = 1. - xx - yy;

    return (m);
}

//! Direction Cosine Matrix to Quaternion
/*! Convert the given DCM to an equivalent quaternion
    \param dcm direction cosine matrix
    \return q quaternion
*/
quaternion q_dcm2quaternion_cm(cmatrix dcm)
{
    quaternion q;
    double t, tr;

    if ((tr=trace_cm(dcm)) > 0.)
    {
        t = .5 / sqrt(1.+tr);
        q.w = .25 / t;
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

    q_normalize(&q);
    return(q);
}

// TODO: explain
quaternion q_axis2quaternion_cv(cvector v)
{
    double length, s2;
    quaternion q;

    length = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
    s2 = sin(length/2.)/length;
    if (length)
    {
        q.d.x = s2*v.x;
        q.d.y = s2*v.y;
        q.d.z = s2*v.z;
    }
    else
        q.d.x = q.d.y = q.d.z = 0.;
    q.w =cos(length/2);

    q_normalize(&q);
    return (q);
}

//! Rotation quaternion for X axis
/*! Create the ::quaternion that represents a rotation of the given angle around the X axis.
        \param angle Angle of rotation in radians
        \return Resulting ::quaternion
*/
quaternion q_change_around_x(double angle)
{
    quaternion a = {{1.,0.,0.},0.};

    a.d = cv_smult(sin(angle/2.),a.d);
    a.w = cos(angle/2.);

    return (a);
}

//! Rotation quaternion for Y axis
/*! Create the ::quaternion that represents a rotation of the given angle around the Y axis.
        \param angle Angle of rotation in radians
        \return Resulting ::quaternion
*/
quaternion q_change_around_y(double angle)
{
    quaternion a = {{0.,1.,0.},0.};

    a.d = cv_smult(sin(angle/2.),a.d);
    a.w = cos(angle/2.);

    return (a);
}

//! Rotation quaternion for Z axis
/*! Create the ::quaternion that represents a rotation of the given angle around the Z axis.
        \param angle Angle of rotation in radians
        \return Resulting ::quaternion
*/
quaternion q_change_around_z(double angle)
{
    quaternion a = {{0.,0.,1.},0.};

    a.d = cv_smult(sin(angle/2.),a.d);
    a.w = cos(angle/2.);

    return (a);
}

//! Rotation quaternion for indicated axis
/*! Create the ::quaternion that represents a rotation of the given angle around the indicated axis.
        \param axis Axis of rotation: 1=X, 2=Y, 3=Z
        \param angle Angle of rotation in radians
        \return Resulting ::quaternion
*/
quaternion q_change_around(int axis,double angle)
{
    quaternion a = {{1.,0.,0.},0.};

    switch (axis)
    {
    case 1:
        q_change_around_x(angle);
        break;
    case 2:
        q_change_around_y(angle);
        break;
    case 3:
        q_change_around_z(angle);
        break;
    }

    return (a);
}

//! Identity quaternion
/*! Returns a quaternion that will cause no rotation when multiplied by a vector.
        \return Identity quaternion
*/
quaternion q_eye()
{
    quaternion q = {{0.,0.,0.},1.};

    return (q);
}

quaternion q_identity()
{
    return q_eye();
}

//! Length of quaternion
/*! Calculate the length of a ::quaternion by summing the squares of
 * its elements.
        \param q Quaternion to find the length of.
        \return Length of quaternion.
*/
double length_q(quaternion q)
{
    double length;

    length = q.w * q.w + q.d.x * q.d.x + q.d.y * q.d.y + q.d.z * q.d.z;
    length = sqrt(length);

    if (length < D_SMALL)
        return (0.);
    else
        return (length);
}
