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
#include "mathlib.h"

//! \addtogroup mathlib_functions
//! @{

//! Normal Distribution random number
/*! Random number generated using the Central Value Theorem to
    approximate a Gaussian distribution. Twelve random numbers between
    -1 and 1 are averaged to approximate the final random number. This
    will come from a distribution whose mean is 0 and variance is .5.
    The desired input parameters are then used to scale the output.
    \param mean Desired central value of the gaussian.
    \param stdev Desired Standard Deviation of the gaussian.
    \return Random number from desired distribution.
*/
double gaussian_random(double mean, double stdev)
{
    double trand;

    stdev *= stdev;

    trand = 0.;
    for (uint32_t i=0; i<12; ++i)
    {
        trand += rand();
    }

    trand = trand / (3. * RAND_MAX) - 2.;
    trand = trand * stdev + mean;

    return (trand);
}




//! Create rotation quaternion from 2 row vectors
/*! Generate the quaternion that represents a rotation from one row order vector
 * to a second row order vector.
    \param from initial row order vector
    \param to final row order vector
    \return quaternion that can be used to rotate points
*/
// TODO: move to quaternion.cpp
quaternion q_change_between_rv(rvector from, rvector to)
{
    uvector rq = {{{0.,0.,0.},0.}};
    rvector vec1 = {{0.}}, vec2 = {{0.}};

    normalize_rv(from);
    normalize_rv(to);

    if (length_rv(rv_add(from,to)) < 1e-14)
    {
        vec1.col[0] = rand();
        vec1.col[1] = rand();
        vec1.col[2] = rand();
        normalize_rv(vec1);
        vec2 = rv_cross(vec1,to);
        normalize_rv(vec2);
        if (length_rv(vec2)<D_SMALL)
        {
            vec1.col[0] = rand();
            vec1.col[1] = rand();
            vec1.col[2] = rand();
            normalize_rv(vec1);
            vec2 = rv_cross(vec1,to);
            normalize_rv(vec2);
        }
        rq.r = vec2;
        rq.q.w = 0.;
    }
    else
    {
        vec2 = rv_cross(from,to);
        rq.r = vec2;
        rq.q.w = 1. + dot_rv(from,to);
    }

    q_normalize(&rq.q);
    return (rq.q);
}

//! Create rotation quaternion from row vector axis and angle
/*! Generate the quaternion that represents a rotation of the specified angle
 * around the specified row vector axis.
    \param around row order vector around which the rotation will occur
    \param angle amount of rotation in radians
    \return quaternion that can be used to rotate points
*/
// TODO: move to quaternion.cpp
quaternion q_change_around_rv(rvector around, double angle)
{
    double sa;
    uvector rq;

    angle /= 2.;
    sa = sin(angle);
    normalize_rv(around);

    rq.r = rv_smult(sa,around);
    rq.q.w = cos(angle);
    q_normalize(&rq.q);
    return (rq.q);
}

//! Create transform quaternion from two orthogonal vectors
/*! Using two vectors, represented in both the original and target frames,
 * calculate the quaternion that will transform any vector from the original
 * to the target frame.
 * \param sourcea First vector in source frame
 * \param sourceb Second vector in source frame
 * \param targeta First vector in target frame
 * \param targetb Second vector in target frame
 * \return Quaternion to use with ::transform_q to transform from source to target.
 */
// TODO: move to quaternion.cpp
quaternion q_transform_for(rvector sourcea, rvector sourceb, rvector targeta, rvector targetb)
{
    quaternion qe_a;
    quaternion qe_b;
    quaternion fqe;

    // Determine rotation of source A into target A
    qe_a = q_conjugate(q_change_between_rv(sourcea,targeta));

    // Use to transform source B into intermediate B
    sourceb = transform_q(qe_a,sourceb);
    normalize_rv(sourceb);
    normalize_rv(targetb);
    if (length_rv(rv_add(sourceb,targetb)) < 1e-14)
    {
        // Antiparallel - rotate 180 degrees around vector A
        qe_b.d.x = -targeta.col[0];
        qe_b.d.y = -targeta.col[1];
        qe_b.d.z = -targeta.col[2];
        qe_b.w = 0;
    }
    else
    {
        // Determine transformation of this intermediate B into target B
        qe_b = q_conjugate(q_change_between_rv(sourceb,targetb));
    }

    // Combine to determine complete transformation of source into target
    fqe = q_mult(qe_a,qe_b);
    q_normalize(&fqe);

    return fqe;
}

//! Quaternion to row vector axis and angle
/*! Convert rotation quaternion to a directional vector in row order form the length of which
* represents the angle of rotation in radians.
    \param q Quaternion to be converted.
    \return Row vector representing the quaternion.
*/
rvector rv_quaternion2axis(quaternion q)
{
    double ca, sa;
    uvector rq = {{{0.,0.,0.},0.}};

    q_normalize(&q);
    ca = 2.*acos(q.w);
    if (ca > 0. && ca < D2PI)
    {
        sa = sin(ca/2.);
        rq.q = q;
        rq.r = rv_smult(ca/sa,rq.r);
    }
    else
        rq.r = rv_zero();

    return (rq.r);
}

//! Quaternion to row matrix Direction Cosine Matrix
/*! Convert rotation quaternion to an equivalent direction cosine matrix in row matrix form.
    \param q Quaternion representing rotation.
    \return direction Cosine rotation matrix in row matrix form
*/

rmatrix rm_quaternion2dcm(quaternion q)
{
    rmatrix m = {{{{0.}}}};;
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

    m.row[0].col[0] = 1. - yy - zz;
    m.row[0].col[1] = xy - zw;
    m.row[0].col[2] = xz + yw;
    m.row[1].col[0] = xy + zw;
    m.row[1].col[1] = 1. - xx - zz;
    m.row[1].col[2] = yz - xw;
    m.row[2].col[0] = xz - yw;
    m.row[2].col[1] = yz + xw;
    m.row[2].col[2] = 1. - xx - yy;

    return (m);
}



//! Row vector axis and angle to Quaternion.
/*! Convert axis and angle orientation represented as row vector to a rotation Quaternion.
    \param v Row vector axis and angle.
    \return Rotation Quaternion.
*/
// TODO: move to quaternion.cpp
quaternion q_axis2quaternion_rv(rvector v)
{
    double length, s2;
    quaternion q;

    length = sqrt(v.col[0]*v.col[0]+v.col[1]*v.col[1]+v.col[2]*v.col[2]);
    s2 = sin(length/2.)/length;
    if (length)
    {
        q.d.x = s2*v.col[0];
        q.d.y = s2*v.col[1];
        q.d.z = s2*v.col[2];
    }
    else
        q.d.x = q.d.y = q.d.z = 0.;
    q.w =cos(length/2);

    q_normalize(&q);
    return (q);
}



//! Distance from a line
/*! Calculates the distance of a third point from a line defined be
 * two points.
    \param p0 Point not on line.
    \param p1 First point defining line.
    \param p2 Second point defining line.
    \return Distance of Third point from line. Zero if any problems.
*/
double distance_rv(rvector p0, rvector p1, rvector p2)
{
    rvector p21, p01, p02;
    double d, l;

    p21 = rv_sub(p2,p1);
    p01 = rv_sub(p0,p1);
    p02 = rv_sub(p0,p2);

    if ((l=length_rv(p21)) == 0.)
        return (0.);

    d = length_rv(rv_cross(p01,p02))/l;

    return(d);
}

//double distance_rv_1(rvector p0, rvector p1, rvector p2)
//{
//	rvector p21, p01, p02;
//	double d, l;

//	p21 = rv_sub(p2,p1);
//	p01 = rv_sub(p0,p1);
//	p02 = rv_sub(p0,p2);

//	if ((l=length_rv(p21)) == 0.)
//		return (0.);

//	printf("cross product = %g\n", length_rv(rv_cross(p01,p21)));
//	printf("P21 length = %g\n", l);

//	d = length_rv(rv_cross(p01,p21))/l;

//	return(d);

//}

//! Area of a triangle
/*! Calculates the area of a triangle defined by three points.
    \param p0 First point defining triangle.
    \param p1 Second point defining triangle.
    \param p3 Third point defining triangle.
    \return Area of triangle. Zero if any problems.
*/
double area_rv(rvector p0, rvector p1, rvector p2)
{
    double base, altitude;

    base = length_rv(rv_sub(p1,p2));
    altitude = distance_rv(p0,p1,p2);

    return(.5*base*altitude);
}

//! Normal to a polygon
/*! Calculate the normal vector to a polygon that contains at least
 * three sequential vertices.
    \param p0 First point defining polygon.
    \param p1 Second point defining polygon.
    \param p3 Third point defining polygon.
    \return ::rvector representing normal in a normalized form.
*/
rvector rv_normalto(rvector p0, rvector p1, rvector p2)
{
    rvector normal, p10, p20;

    p10 = rv_sub(p1,p0);
    p20 = rv_sub(p2,p0);

    normal = rv_cross(p10,p20);

    return(rv_normal(normal));
}




//! Determine local byte order
/*! Investigate a locally stored number to determine the byte order of the local machine.
    \return Order as provided in ::BYTE_ORDER.
*/

//std::uint8_t local_byte_order()
ByteOrder local_byte_order()
{
    uint16_t test = 1;
    uint8_t *check;

    check = (uint8_t *)&test;

    if (check[0] == 0)
        return (ByteOrder::BIGENDIAN);
    else
        return (ByteOrder::LITTLEENDIAN);
}

//! Memory to 16 bit unsigned integer
/*! Return the 16 bit unsigned integer equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::BYTE_ORDER.
    \return 16 bit unsigned integer
*/

uint16_t uint16from(uint8_t *pointer, ByteOrder order)
{
    uint16_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (uint16_t *)rb;
    if (local_byte_order() == order)
    {
        memcpy((void *)rb,pointer,2);
    }
    else
    {
        rb[1] = pointer[0];
        rb[0] = pointer[1];
    }

    return (*result);
}

//! Memory to 16 bit signed integer
/*! Return the 16 bit signed integer equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::BYTE_ORDER.
    \return 16 bit signed integer
*/

int16_t int16from(uint8_t *pointer, ByteOrder order)
{
    int16_t *result;
    uint16_t rb;

    result = (int16_t *)&rb;
    rb = uint16from(pointer,order);

    return (*result);
}

//! Memory to 32 bit unsigned integer
/*! Return the 32 bit unsigned integer equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::BYTE_ORDER.
    \return 32 bit unsigned integer
*/

uint32_t uint32from(uint8_t *pointer, ByteOrder order)
{
    uint32_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (uint32_t *)rb;
    if (local_byte_order() == order)
    {
        memcpy((void *)rb,pointer,4);
    }
    else
    {
        rb[3] = pointer[0];
        rb[2] = pointer[1];
        rb[1] = pointer[2];
        rb[0] = pointer[3];
    }

    return (*result);
}

//! Memory to 32 bit signed integer
/*! Return the 32 bit signed integer equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::BYTE_ORDER.
    \return 32 bit signed integer
*/

int32_t int32from(uint8_t *pointer, ByteOrder order)
{
    int32_t *result;
    uint32_t rb;

    result = (int32_t *)&rb;
    rb = uint32from(pointer,order);

    return (*result);
}

//! Memory to 32 bit float
/*! Return the 32 bit float equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::BYTE_ORDER.
    \return 32 bit float
*/

float floatfrom(uint8_t *pointer, ByteOrder order)
{
    float result;
    uint8_t *rb;

    rb = (uint8_t *)&result;
    if (local_byte_order() == order)
    {
        memcpy((void *)rb,pointer,4);
    }
    else
    {
        rb[3] = pointer[0];
        rb[2] = pointer[1];
        rb[1] = pointer[2];
        rb[0] = pointer[3];
    }

    return (result);
}

//! Memory to 64 bit float
/*! Return the 64 bit float equivalent of a location in memory, corrected for the local byte order.
    \param pointer location in memory to be cast
    \param order byte order of the data in memory. Taken from ::BYTE_ORDER.
    \return 64 bit float
*/

double doublefrom(uint8_t *pointer, ByteOrder order)
{
    double result;
    uint8_t *rb;

    rb = (uint8_t *)&result;
    if (local_byte_order() == order)
    {
        memcpy((void *)rb,pointer,8);
    }
    else
    {
        rb[7] = pointer[0];
        rb[6] = pointer[1];
        rb[5] = pointer[2];
        rb[4] = pointer[3];
        rb[3] = pointer[4];
        rb[2] = pointer[5];
        rb[1] = pointer[6];
        rb[0] = pointer[7];
    }

    return (result);
}

//! 32 bit unsigned integer to memory
/*! Cast a 32 bit unsigned integer equivalent into a location in memory, corrected for the local byte order.
    \param value integer to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::BYTE_ORDER.
*/

void uint32to(uint32_t value, uint8_t *pointer, ByteOrder order)
{
    uint32_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (uint32_t *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,4);
    }
    else
    {
        pointer[0] = rb[3];
        pointer[1] = rb[2];
        pointer[2] = rb[1];
        pointer[3] = rb[0];
    }

}

//! 32 bit signed integer to memory
/*! Cast a 32 bit signed integer equivalent into a location in memory, corrected for the local byte order.
    \param value integer to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::BYTE_ORDER.
*/

void int32to(int32_t value, uint8_t *pointer, ByteOrder order)
{
    int32_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (int32_t *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,4);
    }
    else
    {
        pointer[0] = rb[3];
        pointer[1] = rb[2];
        pointer[2] = rb[1];
        pointer[3] = rb[0];
    }

}

//! 16 bit unsigned integer to memory
/*! Cast a 16 bit unsigned integer equivalent into a location in memory, corrected for the local byte order.
    \param value integer to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::BYTE_ORDER.
*/

void uint16to(uint16_t value, uint8_t *pointer, ByteOrder order)
{
    uint16_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (uint16_t *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,2);
    }
    else
    {
        pointer[0] = rb[1];
        pointer[1] = rb[0];
    }

}

//! 16 bit signed integer to memory
/*! Cast a 16 bit signed integer equivalent into a location in memory, corrected for the local byte order.
    \param value integer to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::BYTE_ORDER.
*/

void int16to(int16_t value, uint8_t *pointer, ByteOrder order)
{
    int16_t *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (int16_t *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,2);
    }
    else
    {
        pointer[0] = rb[1];
        pointer[1] = rb[0];
    }

}

//! 32 bit floating point to memory
/*! Cast a 32 bit floating point equivalent into a location in memory, corrected for the local byte order.
    \param value float to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::BYTE_ORDER.
*/

void floatto(float value, uint8_t *pointer, ByteOrder order)
{
    float *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (float *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,4);
    }
    else
    {
        pointer[0] = rb[3];
        pointer[1] = rb[2];
        pointer[2] = rb[1];
        pointer[3] = rb[0];
    }

}

//! 64 bit floating point to memory
/*! Cast a 64 bit floating point equivalent into a location in memory, corrected for the local byte order.
    \param value float to be cast
    \param pointer location in memory
    \param order desired byte order of the data in memory. Taken from ::BYTE_ORDER.
*/

void doubleto(double value, uint8_t *pointer, ByteOrder order)
{
    double *result;
    uint8_t *rb;
    double rd;

    rb = (uint8_t *)&rd;

    result = (double *)rb;
    *result = value;
    if (local_byte_order() == order)
    {
        memcpy(pointer,(void *)rb,8);
    }
    else
    {
        pointer[0] = rb[7];
        pointer[1] = rb[6];
        pointer[2] = rb[5];
        pointer[3] = rb[4];
        pointer[4] = rb[3];
        pointer[5] = rb[2];
        pointer[6] = rb[1];
        pointer[7] = rb[0];
    }
}

//! Initialize estimator
/*! Setup the provided ::estimatorhandle so that it can be fed values
 * and provide estimates.
    \param estimate Pointer to an ::estimatorhandle.
    \param size The number of estimates to be averaged for the total estimate.
    \param degree The degree of the polynomial fit for the estimator.
*/
void open_estimate(estimatorhandle *estimate, uint32_t size, uint32_t degree)
{
    if (degree > 4) degree = 4;
    estimate->degree = degree;
    if (size < degree) size = degree;
    estimate->size = size;
    estimate->r.resize(0);
}

//! Set estimator
/*! Add a pair of independent and dependent values to the supplied
 * ::estimatorhandle. If more than degree+1 values have been accumulated, a
 * fit will be done for the new value. If more than size values have
 * been added, the oldest will be dropped.
    \param estimate Pointer to an ::estimatorhandle.
    \param independent Independent value.
    \param dependent Dependent value.
    \return Number of stored values.
*/
int16_t set_estimate(estimatorhandle *estimate, double independent, double dependent)
{

    if (estimate->r.size() == estimate->size && estimate->r[estimate->size-1].x.size() == estimate->degree+1)
    {
        for (uint32_t i=0; i<estimate->size-1; ++i)
        {
            estimate->r[i] = estimate->r[i+1];
        }
        for (uint32_t i=0; i<estimate->degree; ++i)
        {
            estimate->r[estimate->size-1].x[i] = estimate->r[estimate->size-1].x[i+1];
            estimate->r[estimate->size-1].y[i] = estimate->r[estimate->size-1].y[i+1];
        }
        estimate->r[estimate->size-1].x[estimate->degree] = independent  - estimate->xbase;
        estimate->r[estimate->size-1].y[estimate->degree] = dependent  - estimate->ybase;
        estimate->r[estimate->size-1].a = polyfit(estimate->r[estimate->size-1].x,estimate->r[estimate->size-1].y);
    }
    else
    {
        if (estimate->r.size() > 0)
        {
            if (estimate->r.size() < estimate->size) estimate->r.resize(estimate->r.size()+1);
            for (uint32_t i=0; i<estimate->r.size(); ++i)
            {
                if (estimate->r[i].x.size() < estimate->degree+1)
                {
                    estimate->r[i].x.push_back(independent - estimate->xbase);
                    estimate->r[i].y.push_back(dependent - estimate->ybase);
                }
                if (estimate->r[i].x.size() == estimate->degree+1 && !estimate->r[i].a.size()) estimate->r[i].a = polyfit(estimate->r[i].x,estimate->r[i].y);
            }

        }
        else
        {
            estimate->xbase = independent;
            estimate->ybase = dependent;
            estimate->r.resize(1);
            estimate->r[0].x.push_back(independent - estimate->xbase);
            estimate->r[0].y.push_back(dependent - estimate->ybase);
        }
    }

    return (estimate->r.size()+estimate->r[estimate->r.size()-1].x.size());
}

//! Get estimate
/*! Return the best estimate for the supplied independent value.
 * Estimate will be returned as an ::estimatorstruc, containing the
 * value and likely error for the 0th, 1st and 2nd derivative of the
 * dependent value.
    \param estimate Pointer to an ::estimatorhandle.
    \param x Value of independent variable for estimate.
    \return ::estimatorstruc containing the estimate of the dependent
    value.
*/
estimatorstruc get_estimate(estimatorhandle *estimate, double x)
{
    estimatorstruc result;
    double tx;

    result.value[0] = result.value[1] = result.value[2] = 0.;
    result.error[0] = result.error[1] = result.error[2] = 0.;
    if (!estimate->r.size())
    {
        return (result);
    }

    x -= estimate->xbase;

    if (estimate->r.size() == estimate->size && estimate->r[estimate->size-1].a.size() == estimate->degree+1)
    {
        for (uint32_t i=0; i<estimate->size; i++)
        {
            tx = 1.;
            for (uint32_t j=0; j<estimate->degree+1; j++)
            {
                switch (j)
                {
                case 0:
                    estimate->r[i].value[0] = estimate->r[i].a[0];
                    if (estimate->degree > 0) estimate->r[i].value[1] = estimate->r[i].a[1];
                    if (estimate->degree > 1) estimate->r[i].value[2] = 2. * estimate->r[i].a[2];
                    break;
                default:
                    tx *= x;
                    estimate->r[i].value[0] += tx * estimate->r[i].a[j];
                    if (estimate->degree > j) estimate->r[i].value[1] += tx * (j+1) * estimate->r[i].a[j+1];
                    if (estimate->degree > j+1) estimate->r[i].value[2] += tx * (j+2) * (j+1) * estimate->r[i].a[j+2];
                    break;
                }
            }

            for (int j=0; j<3; ++j)
            {
                result.value[j] += estimate->r[i].value[j];
                result.error[j] += (estimate->r[i].value[j]*estimate->r[i].value[j]);
            }

        }

        for (int i=0; i<3; i++)
        {
            result.error[i] = result.error[i] - result.value[i]*result.value[i]/(estimate->size);
            if (result.error[i] < 0.) result.error[i] = 0.;
            if (estimate->size > 1)
            {
                result.error[i] /= (estimate->size - 1);
            }
            result.error[i] = sqrt(result.error[i]);
            result.value[i] /= (estimate->size);
        }

        result.value[0] += estimate->ybase;
    }
    else
    {
        for (uint32_t i=0; i<estimate->r[0].x.size(); ++i)
        {
            result.value[0] += estimate->r[0].y[i];
            result.error[0] += (estimate->r[0].y[i]*estimate->r[0].y[i]);
        }
        for (int i=0; i<3; ++i)
        {
            result.error[i] = result.error[i] - result.value[i]*result.value[i]/estimate->r[0].x.size();
            if (estimate->r[0].x.size() > 1)
            {
                result.error[i] /= estimate->r[0].x.size() - 1;
            }
            result.error[i] = sqrt(result.error[i]);
            result.value[i] /= estimate->r[0].x.size();
        }
    }

    return (result);
}

//! Perform N equation solution.
/*! Solve a system of N equations in N unknowns. The input data is N
vectors of N-1 points ( N independent and 1 dependent variable).
    \param x N vectors of N-1 dependent variables.
    \param y N independent variables.
    \param a N returned parameters
*/
void multisolve(std::vector< std::vector<double> > x, std::vector<double> y, std::vector<double>& a)
{
    uint32_t order;
    std::vector< std::vector<double> > dx(y.size()-1, std::vector<double>(y.size()-1));
    std::vector<double> dy(y.size()-1);
    std::vector<double> da(y.size()-1);

    order = y.size() - 1;

    // Order must be at least 1
    if (order)
    {
        for (uint32_t r=1; r<=order; ++r)
        {
            dy[r-1] = y[r] * x[0][0] - y[0] * x[r][0];
            for (uint32_t c=1; c<=order; ++c)
            {
                dx[r-1][c-1] = x[r][c] * x[0][0] - x[0][c] * x[r][0];
            }
        }
        multisolve(dx, dy, da);

        for (uint32_t i=0; i<order; ++i)
        {
            a[i+1] = da[i];
        }
    }

    // Make this final calculation with the largest dependent in the first position
    uint16_t bestindex = 0;
    double bestx = 0.;
    for (uint16_t i=0; i<y.size(); ++i)
    {
        if (fabs(x[i][0]) > bestx)
        {
            bestx = fabs(x[i][0]);
            bestindex = i;
        }
    }
    a[0] = y[bestindex];
    for (uint32_t i=1; i<=order; ++i)
    {
        a[0] -= a[i] * x[bestindex][i];
    }
    a[0] /= x[bestindex][0];

}

//! Evaluate polynomial
/*! Return the value of the given Nth order polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order polynomial to be evaluated.
 * \return Value of the variable, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
double evaluate_poly(double x, std::vector<double> parms)
{
    double result;

    if (parms.size() < 2)
    {
        return 0.;
    }

    result = parms[parms.size()-1];
    for (uint16_t i=parms.size()-2; i<parms.size(); --i)
    {
        result *= x;
        result += parms[i];
    }

    return result;
}

//! Evaluate polynomial slope.
/*! Return the value of the 1st derivative of the given Nth order polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order polynomial to be evaluated.
 * \return Value of the slope, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
double evaluate_poly_slope(double x, std::vector<double> parms)
{
    double result;

    if (parms.size() < 2)
    {
        return 0.;
    }

    result = parms[parms.size()-1] * (parms.size()-1);
    for (uint16_t i=parms.size()-2; i>0; --i)
    {
        result *= x;
        result += i * parms[i];
    }

    return result;
}

//! Evaluate polynomial acceleration.
/*! Return the value of the 2nd derivative of the given Nth order polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order polynomial to be evaluated.
 * \return Value of the acceleration, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
double evaluate_poly_accel(double x, std::vector<double> parms)
{
    double result;

    if (parms.size() < 3)
    {
        return 0.;
    }

    result = parms[parms.size()-1] * (parms.size()-1) * (parms.size()-2);
    for (uint16_t i=parms.size()-2; i>1; --i)
    {
        result *= x;
        result += i * (i-1) * parms[i];
    }

    return result;
}

//! Evaluate polynomial jerk.
/*! Return the value of the 3rd derivative of the given Nth order polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order polynomial to be evaluated.
 * \return Value of the jerk, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
double evaluate_poly_jerk(double x, std::vector<double> parms)
{
    double result;

    if (parms.size() < 4)
    {
        return 0.;
    }

    result = parms[parms.size()-1] * (parms.size()-1) * (parms.size()-2) * (parms.size()-3);
    for (uint16_t i=parms.size()-2; i>2; --i)
    {
        result *= x;
        result += i * (i-1) * (i-2) * parms[i];
    }

    return result;
}

//! Evaluate vector polynomial.
/*! Return the value of the given Nth order vector polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order vector polynomial to be evaluated.
 * \return Values, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
rvector rv_evaluate_poly(double x, std::vector< std::vector<double> > parms)
{
    uvector result;

    for (uint16_t ic=0; ic<parms.size(); ++ic)
    {
        if (parms[ic].size() < 2)
        {
            result.a4[ic] = 0.;
        }
        else
        {
            result.a4[ic] = parms[ic][parms[ic].size()-1];
            for (uint16_t i=parms[ic].size()-2; i<parms[ic].size(); --i)
            {
                result.a4[ic] *= x;
                result.a4[ic] += parms[ic][i];
            }
        }
    }

    return result.r;
}

//! Evaluate vector polynomial slope.
/*! Return the value of the 1st derivative of the given Nth order vector polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order vector polynomial to be evaluated.
 * \return Values of the slope, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
rvector rv_evaluate_poly_slope(double x, std::vector< std::vector<double> > parms)
{
    uvector result;

    for (uint16_t ic=0; ic<parms.size(); ++ic)
    {
        if (parms[ic].size() < 2)
        {
            result.a4[ic] = 0.;
        }
        else
        {
            result.a4[ic] = parms[ic][parms[ic].size()-1] * (parms[ic].size()-1);
            for (uint16_t i=parms[ic].size()-2; i>0; --i)
            {
                result.a4[ic] *= x;
                result.a4[ic] += i * parms[ic][i];
            }
        }
    }

    return result.r;
}

//! Evaluate vector polynomial acceleration.
/*! Return the value of the 2nd derivative of the given Nth order vector polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order vector polynomial to be evaluated.
 * \return Values of the acceleration, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
rvector rv_evaluate_poly_accel(double x, std::vector< std::vector<double> > parms)
{
    uvector result;

    for (uint16_t ic=0; ic<parms.size(); ++ic)
    {
        if (parms[ic].size() < 2)
        {
            result.a4[ic] = 0.;
        }
        else
        {
            result.a4[ic] = parms[ic][parms[ic].size()-1] * (parms[ic].size()-1) * (parms[ic].size()-2);
            for (uint16_t i=parms[ic].size()-2; i>1; --i)
            {
                result.a4[ic] *= x;
                result.a4[ic] += i * (i-1) * parms[ic][i];
            }
        }
    }

    return result.r;
}

//! Evaluate vector polynomial jerk.
/*! Return the value of the 3rd derivative of the given Nth order vector polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order vector polynomial to be evaluated.
 * \return Values of the jerk, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
rvector rv_evaluate_poly_jerk(double x, std::vector< std::vector<double> > parms)
{
    uvector result;

    for (uint16_t ic=0; ic<parms.size(); ++ic)
    {
        if (parms[ic].size() < 2)
        {
            result.a4[ic] = 0.;
        }
        else
        {
            result.a4[ic] = parms[ic][parms[ic].size()-1] * (parms[ic].size()-1) * (parms[ic].size()-2) * (parms[ic].size()-3);
            for (uint16_t i=parms[ic].size()-2; i>2; --i)
            {
                result.a4[ic] *= x;
                result.a4[ic] += i * (i-1) * (i-2) * parms[ic][i];
            }
        }
    }

    return result.r;
}

//! Evaluate quaternion polynomial.
/*! Return the value of the 1st derivative of the given Nth order quaternion polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order quaternion polynomial to be evaluated.
 * \return Values, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
quaternion q_evaluate_poly(double x, std::vector< std::vector<double> > parms)
{
    uvector result;

    for (uint16_t ic=0; ic<parms.size(); ++ic)
    {
        if (parms[ic].size() < 2)
        {
            result.a4[ic] = 0.;
        }
        else
        {
            result.a4[ic] = parms[ic][parms[ic].size()-1];
            for (uint16_t i=parms[ic].size()-2; i<parms[ic].size(); --i)
            {
                result.a4[ic] *= x;
                result.a4[ic] += parms[ic][i];
            }
        }
    }

    return result.q;
}

//! Evaluate quaternion polynomial slope.
/*! Return the value of the 1st derivative of the given Nth order quaternion polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order quaternion polynomial to be evaluated.
 * \return Values of the slope, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
quaternion q_evaluate_poly_slope(double x, std::vector< std::vector<double> > parms)
{
    uvector result;

    for (uint16_t ic=0; ic<parms.size(); ++ic)
    {
        if (parms[ic].size() < 2)
        {
            result.a4[ic] = 0.;
        }
        else
        {
            result.a4[ic] = parms[ic][parms[ic].size()-1] * (parms[ic].size()-1);
            for (uint16_t i=parms[ic].size()-2; i>0; --i)
            {
                result.a4[ic] *= x;
                result.a4[ic] += i * parms[ic][i];
            }
        }
    }

    return result.q;
}

//! Evaluate quaternion polynomial acceleration.
/*! Return the value of the 1st derivative of the given Nth order quaternion polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order quaternion polynomial to be evaluated.
 * \return Values of the acceleration, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
quaternion q_evaluate_poly_accel(double x, std::vector< std::vector<double> > parms)
{
    uvector result;

    for (uint16_t ic=0; ic<parms.size(); ++ic)
    {
        if (parms[ic].size() < 2)
        {
            result.a4[ic] = 0.;
        }
        else
        {
            result.a4[ic] = parms[ic][parms[ic].size()-1] * (parms[ic].size()-1) * (parms[ic].size()-2);
            for (uint16_t i=parms[ic].size()-2; i>1; --i)
            {
                result.a4[ic] *= x;
                result.a4[ic] += i * (i-1) * parms[ic][i];
            }
        }
    }

    return result.q;
}

//! Evaluate quaternion polynomial jerk.
/*! Return the value of the 1st derivative of the given Nth order quaternion polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order quaternion polynomial to be evaluated.
 * \return Values of the jerk, evaluated at the location of the independent variable.
 */
// TODO: move to lsfit.cpp
quaternion q_evaluate_poly_jerk(double x, std::vector< std::vector<double> > parms)
{
    uvector result;

    for (uint16_t ic=0; ic<parms.size(); ++ic)
    {
        if (parms[ic].size() < 2)
        {
            result.a4[ic] = 0.;
        }
        else
        {
            result.a4[ic] = parms[ic][parms[ic].size()-1] * (parms[ic].size()-1) * (parms[ic].size()-2) * (parms[ic].size()-3);
            for (uint16_t i=parms[ic].size()-2; i>2; --i)
            {
                result.a4[ic] *= x;
                result.a4[ic] += i * (i-1) * (i-2) * parms[ic][i];
            }
        }
    }

    return result.q;
}

//! Perform general order polynomial fit.
/*! Fit a polynomial of type: y(x) = An*x^n + An-1*x^(n-1) + ... + * A2*x^2 + A1*x + A0
 * where n-1 is the order of the polynomial. The input data is n
 * points in x and n matching points in y. The fit is achieved
 * using a Vandermonde interpolating polynomial.
    \param x Order+1 number of x values
    \param y Order+1 number of y values
    \return Order+1 number of polynomial coefficients
*/
// TODO: move to lsfit.cpp
std::vector<double> polyfit(std::vector<double> &x, std::vector<double> &y)
{
    uint32_t order;
    std::vector< std::vector<double> > dx;
    std::vector<double> dy;
    std::vector<double> da;
    std::vector<double> a;

    order = x.size() - 1;
    a.resize(x.size());

    switch (order)
    {
    case 1:
        a[1] = (y[1]-y[0])/(x[1]-x[0]);
        a[0] = y[1] - a[1]*x[1];
        break;
    case 2:
        a[2] = ((y[0]-y[1])/(x[1]-x[0])+(y[2]-y[1])/(x[2]-x[1]))/(x[2]-x[0]);
        a[1] = (y[0]-y[1])/(x[0]-x[1])-(a[2]*(x[1]+x[0]));
        a[0] = -((x[0]*(a[1]+(a[2]*x[0])))-y[0]);
        break;
    case 3:
        a[3] = (((x[1] - x[0])*(x[2] - x[0])*((y[3]*(x[2] - x[1]))
                + (y[2]*(x[1] - x[3])) + (y[1]*(x[3] - x[2])))) - ((((y[1]
                                                                     - y[0])*(x[1] - x[2])) + ((x[1] - x[0])*(y[2] -
                y[1])))*(x[1] - x[3])*(x[2] - x[3])))/((x[1] - x[0])*(x[2]
                - x[0])*((x[1]*(x[0] - x[3])*((x[1]*(x[2] - x[3])) +
                x[3]*x[3] - x[2]*x[2])) + (x[3]*x[2]*((x[0]*x[2]) + (x[3]*(x[3] - x[0] - x[2]))))));
        a[2] = ((y[1] - y[0])/(x[0] - x[1])+(y[2] - y[1])/(x[2] - x[1]))/(x[2] - x[0])-(a[3]*(x[1] + x[2] + x[0]));
        a[1] = (y[0] - y[1] + (a[2]*(x[1]*x[1] - x[0]*x[0])) + (a[3]*(pow(x[1],3) - pow(x[0],3))))/(x[0] - x[1]);
        a[0] = -((x[0]*(a[1] + (x[0]*(a[2] + (a[3]*x[0]))))) - y[0]);
        break;
    default:
        dx.resize(order+1);
        for (uint32_t r=0; r<=order; ++r)
        {
            dx[r].resize(order+1);
            for (uint32_t c=0; c<=order; ++c)
            {
                switch (c)
                {
                case 0:
                    dx[r][0] = 1.;
                    break;
                case 1:
                    dx[r][1] = x[r];
                    break;
                case 2:
                    dx[r][2] = x[r]*x[r];
                    break;
                default:
                    dx[r][c] = pow(x[r],c);
                    break;
                }
            }
        }
        multisolve(dx,y, a);
        break;
    }

    return (a);
}

//! Perform nth order polynomial fit.
/*! Using the n equations: y = A0 + A1*x + A2*x^2 + ... +An*x^n, solve for the
 * coefficients A0...An.
    \param x The n values of x
    \param y The n corresponding values of y
    \param order The order of the polynomial (< 5)
    \return The n resulting coefficients
*/
// TODO: move to lsfit.cpp
uvector rv_fitpoly(uvector x, uvector y, uint32_t order)
{
    uvector a = {{{0.,0.,0.},0.}};

    if (order < 5)
    {
        switch (order)
        {
        case 1:
            a.a4[1] = (y.a4[1]-y.a4[0])/(x.a4[1]-x.a4[0]);
            a.a4[0] = y.a4[1] - a.a4[1]*x.a4[1];
            break;
        case 2:
            a.a4[2] = ((y.a4[0]-y.a4[1])/(x.a4[1]-x.a4[0])+(y.a4[2]-y.a4[1])/(x.a4[2]-x.a4[1]))/(x.a4[2]-x.a4[0]);
            a.a4[1] = (y.a4[0]-y.a4[1])/(x.a4[0]-x.a4[1])-(a.a4[2]*(x.a4[1]+x.a4[0]));
            a.a4[0] = -((x.a4[0]*(a.a4[1]+(a.a4[2]*x.a4[0])))-y.a4[0]);
            break;
        case 3:
            a.a4[3] = (((x.a4[1] - x.a4[0])*(x.a4[2] - x.a4[0])*((y.a4[3]*(x.a4[2] - x.a4[1])) + (y.a4[2]*(x.a4[1] - x.a4[3])) + (y.a4[1]*(x.a4[3] - x.a4[2])))) - ((((y.a4[1] - y.a4[0])*(x.a4[1] - x.a4[2])) + ((x.a4[1] - x.a4[0])*(y.a4[2] - y.a4[1])))*(x.a4[1] - x.a4[3])*(x.a4[2] - x.a4[3])))/((x.a4[1] - x.a4[0])*(x.a4[2] - x.a4[0])*((x.a4[1]*(x.a4[0] - x.a4[3])*((x.a4[1]*(x.a4[2] - x.a4[3])) + pow(x.a4[3],2) - pow(x.a4[2],2))) + (x.a4[3]*x.a4[2]*((x.a4[0]*x.a4[2]) + (x.a4[3]*(x.a4[3] - x.a4[0] - x.a4[2]))))));
            a.a4[2] = ((y.a4[1] - y.a4[0])/(x.a4[0] - x.a4[1])+(y.a4[2] - y.a4[1])/(x.a4[2] - x.a4[1]))/(x.a4[2] - x.a4[0])-(a.a4[3]*(x.a4[1] + x.a4[2] + x.a4[0]));
            a.a4[1] = (y.a4[0] - y.a4[1] + (a.a4[2]*(pow(x.a4[1],2) - pow(x.a4[0],2))) + (a.a4[3]*(pow(x.a4[1],3) - pow(x.a4[0],3))))/(x.a4[0] - x.a4[1]);
            a.a4[0] = -((x.a4[0]*(a.a4[1] + (x.a4[0]*(a.a4[2] + (a.a4[3]*x.a4[0]))))) - y.a4[0]);
            break;
        }
    }
    return (a);
}

//! Create Gauss-Jackson Integration Kernel
/*! Allocate space for, and initialize all the parameters common to a Gauss-Jackson integration of
 * the requested order and step change in the independent variable.
    \param order Order at which integration will be performed.
    \param dvi Step size of independent variable.
    \return Pointer to a structure that can be reused for multiple integrations. NULL if error.
*/

gj_kernel *gauss_jackson_kernel(int32_t order, double dvi)
{
    int i, n, j,m,k;
    gj_kernel *gjk = NULL;

    if ((gjk = (gj_kernel *)calloc(1,sizeof(gj_kernel))) == NULL)
        return (gjk);

    gjk->dvi = dvi;
    gjk->order = order;
    gjk->dvi2 = dvi * dvi;
    gjk->horder = order/2;

    if ((gjk->binom = (int32_t **)calloc(order+2,sizeof(int32_t *))) == NULL)
    {
        gauss_jackson_dekernel(gjk);
        return nullptr;
    }

    for (i=0; i<order+2; i++)
    {
        if ((gjk->binom[i] = (int32_t *)calloc(order+2,sizeof(int32_t))) == NULL)
        {
            gauss_jackson_dekernel(gjk);
            return nullptr;
        }
    }

    if ((gjk->alpha = (double **)calloc(order+2,sizeof(double *))) == NULL)
    {
        gauss_jackson_dekernel(gjk);
        return nullptr;
    }

    for (i=0; i<order+2; i++)
    {
        if ((gjk->alpha[i] = (double *)calloc(order+1,sizeof(double))) == NULL)
        {
            gauss_jackson_dekernel(gjk);
            return nullptr;
        }
    }

    if ((gjk->beta = (double **)calloc(order+2,sizeof(double *))) == NULL)
    {
        gauss_jackson_dekernel(gjk);
        return nullptr;
    }

    for (i=0; i<order+2; i++)
    {
        if ((gjk->beta[i] = (double *)calloc(order+1,sizeof(double))) == NULL)
        {
            gauss_jackson_dekernel(gjk);
            return nullptr;
        }
    }

    if ((gjk->c = (double *)calloc(order+3,sizeof(double *))) == NULL)
    {
        gauss_jackson_dekernel(gjk);
        return nullptr;
    }

    if ((gjk->gam = (double *)calloc(order+2,sizeof(double *))) == NULL)
    {
        gauss_jackson_dekernel(gjk);
        return nullptr;
    }

    if ((gjk->q = (double *)calloc(order+3,sizeof(double *))) == NULL)
    {
        gauss_jackson_dekernel(gjk);
        return nullptr;
    }

    if ((gjk->lam = (double *)calloc(order+3,sizeof(double *))) == NULL)
    {
        gauss_jackson_dekernel(gjk);
        return nullptr;
    }

    for (m=0; m<order+2; m++)
    {
        for (i=0; i<order+2; i++)
        {
            if (m > i)
                gjk->binom[m][i] = 0;
            else
            {
                if (m == i)
                    gjk->binom[m][i] = 1;
                else
                {
                    if (m == 0)
                        gjk->binom[m][i] = 1;
                    else
                        gjk->binom[m][i] = gjk->binom[m-1][i-1] + gjk->binom[m][i-1];
                }
            }
        }
    }

    gjk->c[0] = 1.;
    for (n=1; n<order+3; n++)
    {
        gjk->c[n] = 0.;
        for (i=0; i<=n-1; i++)
        {
            gjk->c[n] -= gjk->c[i] / (n+1-i);
        }
    }

    gjk->gam[0] = gjk->c[0];
    for (i=1; i<gjk->order+2; i++)
    {
        gjk->gam[i] = gjk->gam[i-1] + gjk->c[i];
    }

    for (i=0; i<gjk->order+1; i++)
    {
        gjk->beta[gjk->order+1][i] = gjk->gam[i+1];
        gjk->beta[gjk->order][i] = gjk->c[i+1];
        for (j=gjk->order-1; j>=0; j--)
        {
            if (!i)
                gjk->beta[j][i] = gjk->beta[j+1][i];
            else
                gjk->beta[j][i] = gjk->beta[j+1][i] - gjk->beta[j+1][i-1];
        }
    }

    gjk->q[0] = 1.;
    for (i=1; i<gjk->order+3; i++)
    {
        gjk->q[i] = 0.;
        for (k=0; k<=i; k++)
        {
            gjk->q[i] += gjk->c[k]*gjk->c[i-k];
        }
    }

    gjk->lam[0] = gjk->q[0];
    for (i=1; i<gjk->order+3; i++)
    {
        gjk->lam[i] = gjk->lam[i-1] + gjk->q[i];
    }

    for (i=0; i<gjk->order+1; i++)
    {
        gjk->alpha[gjk->order+1][i] = gjk->lam[i+2];
        gjk->alpha[gjk->order][i] = gjk->q[i+2];
        for (j=gjk->order-1; j>=0; j--)
        {
            if (!i)
                gjk->alpha[j][i] = gjk->alpha[j+1][i];
            else
                gjk->alpha[j][i] = gjk->alpha[j+1][i] - gjk->alpha[j+1][i-1];
        }
    }

    return (gjk);
}

//! Free Gauss-Jackson Integration Kernel
/*! Free the space fully or partially allocated represented by the supplied kernel pointer.
    \param gjk Pointer to Gauss-Jackson Kernel
*/
void gauss_jackson_dekernel(gj_kernel *gjk)
{
    int i;

    if (gjk == NULL)
        return;

    if (gjk->binom != NULL)
    {
        for (i=0; i<gjk->order+2; i++)
        {
            if (gjk->binom[i] != NULL)
                free(gjk->binom[i]);
        }
        free(gjk->binom);
    }

    if (gjk->alpha != NULL)
    {
        for (i=0; i<gjk->order+2; i++)
        {
            if (gjk->alpha[i] != NULL)
                free(gjk->alpha[i]);
        }
        free(gjk->alpha);
    }

    if (gjk->beta != NULL)
    {
        for (i=0; i<gjk->order+2; i++)
        {
            if (gjk->beta[i] != NULL)
                free(gjk->beta[i]);
        }
        free(gjk->beta);
    }

    if (gjk->c != NULL)
        free(gjk->c);

    if (gjk->gam != NULL)
        free(gjk->gam);

    if (gjk->q != NULL)
        free(gjk->q);

    if (gjk->lam != NULL)
        free(gjk->lam);

    free(gjk);
}

//! Initialize an Instance of a Gauss-Jackson Integrator
/*! Set up the integration specific variables for an integration using the provided kernel. The
 * integration will be performed simultaneously for the indicated number of axes. The 2nd
 * derivative for a specific set of independent and dependent variables is calculated using the
 * provided function.
    \param kern Pointer to Gauss-Jackson Kernel previously initialized for order and step size.
    \param axes Number of axes to integrate simultaneously.
    \param calc_vd2 Function that will calculate 2nd derivative for all dependent variables at
    once.
    \return Pointer to a Gauss-Jackson Instance, initialized for use with this kernel. NULL if error.
*/
gj_instance *gauss_jackson_instance(gj_kernel *kern, int32_t axes, void (*calc_vd2)(double vi, double *vd0, double *vd2, int32_t axes))
{
    int i, m;
    gj_instance *gji;

    if (kern == NULL)
        return nullptr;

    if ((gji = (gj_instance *)calloc(1,sizeof(gj_instance))) != NULL)
    {
        gji->kern = kern;
        gji->axes = axes;
        gji->calc_vd2 = calc_vd2;
        if ((gji->vi = (double *)calloc(kern->order+2,sizeof(double))) != NULL)
        {
            if ((gji->steps = (gj_step **)calloc(axes,sizeof(gj_step *))) != NULL)
            {
                for (i=0; i<axes; i++)
                {
                    if ((gji->steps[i] = gauss_jackson_step(kern)) != NULL)
                    {
                        continue;
                    }
                    break;
                }
                if (i == axes)
                    return (gji);
                for (m=0; m<i; m++)
                {
                    gauss_jackson_destep(kern,gji->steps[m]);
                }
                free(gji->steps);
                free(gji->vi);
                free(gji);
                return nullptr;
            }
            else
            {
                free(gji->vi);
                free(gji);
                return nullptr;
            }
        }
        else
        {
            free(gji);
            return nullptr;
        }
    }
    else
        return nullptr;
}

//! Initialize a Step of a Gauss-Jackson integrator
/*! Allocate the space, and set up the steps for a single axis of a Gauss-Jackson integrator of specified
 * order.
    \param kern Kernel for this Gauss-Jackson integrator.
    \return Pointer to the Step.
*/
gj_step *gauss_jackson_step(gj_kernel *kern)
{
    int i,j, m;

    gj_step *step;

    if ((step = (gj_step *)calloc(kern->order+2,sizeof(gj_step))) != NULL)
    {
        for (j=0; j<kern->order+2; j++)
        {
            if ((step[j].a = (double *)calloc(kern->order+1,sizeof(double))) != NULL)
            {
                if ((step[j].b = (double *)calloc(kern->order+1,sizeof(double))) != NULL)
                    continue;
                free(step[j].a);
            }
            break;
        }
        if (j < kern->order+2)
        {
            for (i=0; i<j; i++)
            {
                free(step[i].a);
                free(step[i].b);
            }
            free(step);
            return nullptr;
        }
    }

    for (j=0; j<kern->order+2; j++)
    {
        for (m=0; m<kern->order+1; m++)
        {
            step[j].a[kern->order-m] = step[j].b[kern->order-m] = 0.;
            for (i=m; i<=kern->order; i++)
            {
                step[j].a[kern->order-m] += kern->alpha[j][i] * kern->binom[m][i];
                step[j].b[kern->order-m] += kern->beta[j][i] * kern->binom[m][i];
            }
            step[j].a[kern->order-m] *= pow(-1.,m);
            step[j].b[kern->order-m] *= pow(-1.,m);
            if (kern->order-m == j)
                step[j].b[kern->order-m] += .5;
        }
    }
    return (step);
}

//! Destroy a Step of a Gauss-Jackson integrator
/*! Deallocate the space a single axis of a Gauss-Jackson integrator of specified
 * order.
    \param kern Kernel for this Gauss-Jackson integrator.
    \param step Pointer to the step to be destroyed.
*/
void gauss_jackson_destep(gj_kernel *kern,gj_step *step)
{
    int j;

    if (step != NULL)
    {
        for (j=0; j<kern->order+2; j++)
        {
            if (step[j].a != NULL)
            {
                free(step[j].a);
            }
            if (step[j].b != NULL)
            {
                free(step[j].b);
            }
        }
        free(step);
    }
}

//! Set Independent and Dependent variables for Gauss-Jackson step
/*! Set the Independent and Dependent variables for a particular step of a particular Gauss-Jackson integration
 * Instance.
    \param gji Pointer to Gauss-Jackson Instance.
    \param vi Pointer to Independent variable.
    \param vd0 Pointer to array of Dependent variable for all axes at given step.
    \param vd1 Pointer to array of First derivative of Dependent variable for all axes at given step.
    \param vd2 Pointer to array of Second derivative of Dependent variable for all axes at given step.
    \param istep Integer number of step.
    \return 0, otherwise negative error.
*/
int gauss_jackson_setstep(gj_instance *gji, double vi, double *vd0, double *vd1, double *vd2, int32_t istep)
{
    int i;

    if (gji == NULL)
        return (MATH_ERROR_GJ_UNDEFINED);

    if (istep > gji->kern->order+2)
        return (MATH_ERROR_GJ_OUTOFRANGE);

    gji->vi[istep] = vi;
    for (i=0; i<gji->axes; i++)
    {
        gji->steps[i][istep].vd0 = vd0[i];
        gji->steps[i][istep].vd1 = vd1[i];
        gji->steps[i][istep].vd2 = vd2[i];
    }
    return 0;
}

//! Get Independent and Dependent variables for Gauss-Jackson step
/*! Get the Independent and Dependent variables for a particular step of a particular
 * Gauss-Jackson integration Instance.
    \param gji Pointer to Gauss-Jackson Instance
    \param vi Pointer to Independent variable
    \param vd0 Pointer to array of Dependent variable for all axes at given step
    \param vd1 Pointer to array of First derivative of Dependent variable for all axes at given step
    \param vd2 Pointer to array of Second derivative of Dependent variable for all axes at given step
    \return 0, otherwise negative error.
*/
int gauss_jackson_getstep(gj_instance *gji, double *vi, double *vd0, double *vd1, double *vd2, int32_t istep)
{
    int i;
    if (gji == NULL)
        return (MATH_ERROR_GJ_UNDEFINED);

    if (istep > gji->kern->order+2)
        return (MATH_ERROR_GJ_OUTOFRANGE);

    *vi = gji->vi[istep];
    for (i=0; i<gji->axes; i++)
    {
        vd0[i] = gji->steps[i][istep].vd0;
        vd1[i] = gji->steps[i][istep].vd1;
        vd2[i] = gji->steps[i][istep].vd2;
    }
    return 0;
}

//! Converge all axes of a Gauss-Jackson integrator prior to propagation
/*! Converge all axes of a Gauss-Jackson integrator prior to propagation.
    \param gji Pointer to a Gauss-Jackson Integration Instance that has valid values
*/
void gauss_jackson_preset(gj_instance *gji)
{
    int32_t ccount, cflag=1, k, n, i;
    int32_t gj_order, gj_2order;
    double gj_dvi, gj_dvi2;
    static double *oldvd2 = NULL;
    static double *newvd0 = NULL;
    static double *newvd2 = NULL;
    static int32_t axes = 0;

    if (newvd0 == NULL || axes < gji->axes)
    {
        if (newvd0)
        {
            free(newvd0);
            free(oldvd2);
            free(newvd2);
        }
        axes = gji->axes;
        oldvd2 = (double *)calloc(axes,sizeof(double));
        newvd0 = (double *)calloc(axes,sizeof(double));
        newvd2 = (double *)calloc(axes,sizeof(double));
    }

    gj_2order = gji->kern->horder;
    gj_order = gji->kern->order;
    gj_dvi = gji->kern->dvi;
    gj_dvi2 = gji->kern->dvi2;

    ccount = 0;
    do
    {
        for (i=0; i<axes; i++)
        {
            gji->steps[i][gj_2order].s = gji->steps[i][gj_2order].vd1/gj_dvi;
            for (k=0; k<=gj_order; k++)
                gji->steps[i][gj_2order].s -= gji->steps[i][gj_2order].b[k] * gji->steps[i][k].vd2;

            for (n=1; n<=gj_2order; n++)
            {
                gji->steps[i][gj_2order+n].s = gji->steps[i][gj_2order+n-1].s + (gji->steps[i][gj_2order+n].vd2+gji->steps[i][gj_2order+n-1].vd2)/2;
                gji->steps[i][gj_2order-n].s = gji->steps[i][gj_2order-n+1].s - (gji->steps[i][gj_2order-n].vd2+gji->steps[i][gj_2order-n+1].vd2)/2;
            }

            gji->steps[i][gj_2order].ss = gji->steps[i][gj_2order].vd0/gj_dvi2;

            for (k=0; k<=gj_order; k++)
                gji->steps[i][gj_2order].ss -= gji->steps[i][gj_2order].a[k] * gji->steps[i][k].vd2;

            for (n=1; n<=gj_2order; n++)
            {
                gji->steps[i][gj_2order+n].ss = gji->steps[i][gj_2order+n-1].ss + gji->steps[i][gj_2order+n-1].s + (gji->steps[i][gj_2order+n-1].vd2)/2;
                gji->steps[i][gj_2order-n].ss = gji->steps[i][gj_2order-n+1].ss - gji->steps[i][gj_2order-n+1].s + (gji->steps[i][gj_2order-n+1].vd2)/2;
            }

            for (n=0; n<=gj_order; n++)
            {
                if (n == gj_2order)
                    continue;
                gji->steps[i][n].sb = gji->steps[i][n].sa = 0.;
                for (k=0; k<=gj_order; k++)
                {
                    gji->steps[i][n].sb += gji->steps[i][n].b[k] * gji->steps[i][k].vd2;
                    gji->steps[i][n].sa += gji->steps[i][n].a[k] * gji->steps[i][k].vd2;
                }
            }
        }

        for (n=1; n<=gj_2order; n++)
        {
            for (i=-1; i<2; i+=2)
            {
                // Calculate new probable position and velocity
                for (k=0; k<axes; k++)
                {
                    gji->steps[k][gj_2order+i*n].vd1 = gj_dvi * (gji->steps[k][gj_2order+i*n].s + gji->steps[k][gj_2order+i*n].sb);
                    newvd0[k] = gji->steps[k][gj_2order+i*n].vd0 = gj_dvi2 * (gji->steps[k][gj_2order+i*n].ss + gji->steps[k][gj_2order+i*n].sa);
                    oldvd2[k] = gji->steps[k][gj_2order+i*n].vd2;
                }

                // Calculate new acceleration
                gji->calc_vd2(gji->vi[gj_2order+i*n],newvd0,newvd2,axes);

                // Compare acceleration at new position to previous iteration
                cflag = 0;

                for (k=0; k<axes; k++)
                {
                    gji->steps[k][gj_2order+i*n].vd2 = newvd2[k];
                    if (fabs(oldvd2[k]-newvd2[k])>1e-14)
                        cflag = 1;
                }

            }
        }
        ccount++;
    } while (ccount<10 && cflag);
}

//! Propagate Gauss-Jackson integration
/*! Propagate the Gauss-Jackson integration Instance for all axes. Will iterate a step at a time
 * until target value of indpendent variable has been reached.
    \param gji Gauss-Jackson instance
    \param target Target value of the independent variable
*/
void gauss_jackson_extrapolate(gj_instance *gji, double target)
{
    int32_t chunks, i, j, k;
    int32_t gj_order, gj_2order;
    double gj_dvi, gj_dvi2;
    static double *newvd0 = NULL;
    static double *newvd2 = NULL;
    static int32_t axes = 0;

    if (newvd0 == NULL || axes < gji->axes)
    {
        if (newvd0)
        {
            free(newvd0);
            free(newvd2);
        }
        axes = gji->axes;
        newvd0 = (double *)calloc(axes,sizeof(double));
        newvd2 = (double *)calloc(axes,sizeof(double));
    }


    gj_order = gji->kern->order;
    gj_2order = gj_order/2;
    gj_dvi = gji->kern->dvi;
    gj_dvi2 = gj_dvi * gj_dvi;

    chunks = (int32_t)(.5 + (target-gji->vi[gj_2order])/gji->kern->dvi);
    for (i=0; i<chunks; i++)
    {
        gji->vi[gj_order+1] = gji->vi[gj_order] + gji->kern->dvi;

        // For each axis
        for (j=0; j<axes; j++)
        {
            // Calculate S(order/2+1)
            gji->steps[j][gj_order+1].ss = gji->steps[j][gj_order].ss + gji->steps[j][gj_order].s + gji->steps[j][gj_order].vd2/2.;

            // Calculate Sum(order/2+1) for a and b
            gji->steps[j][gj_order+1].sb = gji->steps[j][gj_order+1].sa = 0.;
            for (k=0; k<=gj_order; k++)
            {
                gji->steps[j][gj_order+1].sb += gji->steps[j][gj_order+1].b[k] * gji->steps[j][k].vd2;
                gji->steps[j][gj_order+1].sa += gji->steps[j][gj_order+1].a[k] * gji->steps[j][k].vd2;
            }

            // Calculate pos.v(order/2+1)
            gji->steps[j][gj_order+1].vd1 = gj_dvi * (gji->steps[j][gj_order].s + gji->steps[j][gj_order].vd2/2. + gji->steps[j][gj_order+1].sb);

            // Calculate pos.s(order/2+1)
            newvd0[j] = gji->steps[j][gj_order+1].vd0 = gj_dvi2 * (gji->steps[j][gj_order+1].ss + gji->steps[j][gj_order+1].sa);
        }

        // Perform positional and attitude accelerations at new position
        gji->calc_vd2(gji->vi[gj_order+1],newvd0,newvd2,axes);

        for (j=0; j<axes; j++)
        {
            // Set new 2nd derivative
            gji->steps[j][gj_order+1].vd2 = newvd2[j];

            // Calculate s(order/2+1)
            gji->steps[j][gj_order+1].s = gji->steps[j][gj_order].s + (gji->steps[j][gj_order].vd2+gji->steps[j][gj_order+1].vd2)/2.;

            // Shift everything over 1
            for (k=0; k<=gj_order; k++)
                gji->steps[j][k] = gji->steps[j][k+1];
        }
        // Shift times over 1
        for (k=0; k<=gj_order; k++)
            gji->vi[k] = gji->vi[k+1];

    }
}

//! Limit angle to range 0-2PI
/*! Ensure that angle represents equivalent value within a range of 0 to 2*PI.
 * \param angle Input angle in radians.
 * \return Output angle adjusted to range 0 to 2*PI.
*/
double fixangle(double angle)
{
    double result;

    result = fmod(angle,D2PI);

    return (result >= 0.)?result:result+D2PI;
}

//! ArcTan, limited to range 0-2PI.
/*! Calculate Arc Tangent of y/x, ennsuring that angle represents equivalent value within a range of 0 to 2*PI.
 * \param y Numerator.
 * \param y Denominator.
 * \return Output angle adjusted to range 0 to 2*PI.
*/
double actan(double y,double x)
{
    double actan;

    actan = atan2(y,x);
    if (actan < 0. )
        actan += D2PI;
    return (actan);
}

//! Limit precision.
/*! Ensure that number represents equivalent value rounded to nearest precision.
 * The calculation performed is prec * round(number / prec).
 * \param number Original value.
 * \param prec Precision to round to.
 * \return Rounded value.
*/
double fixprecision(double number, double prec)
{
    return (prec*round(number/prec));
}

//! Calculate CRC-16-CCITT
/*! Calculate 16-bit CCITT CRC for the indicated buffer and number of bytes.
 * The initial shift register value is 0xffff, and the calculation
 * starts with the LSB, so the Polynomial is 0x8408.
    \param buf bytes to calculate on
    \param size number of bytes
    \return calculated CRC
*/

uint16_t calc_crc16ccitt(uint8_t *buf, int size)
{
    uint16_t crc = 0xffff;
    uint8_t ch;

    for (uint16_t i=0; i<size; i++)
    {
        ch = buf[i];
        for (uint16_t j=0; j<8; j++)
        {
            crc = (crc >> 1)^(((ch^crc)&0x01)?0x8408:0);
            ch >>= 1;
        }
    }
    return (crc);
}












// -------------------------------------------------
// TODO: redo the following quaternion related functions so that they can move
// to the quaternion library

//! Rotate a row vector using a quaternion
/*! Rotate a row vector within one coordinate system using the
 * provided left quaternion.
        \param q Quaternion representing the rotation.
        \param v Row vector to be rotated.
        \return Rotated row vector in the same system.
*/
rvector rotate_q(quaternion q, rvector v)
{
    // TODO: remove uvector, use quaternion
    uvector t = {{{0.,0.,0.},0.}};

    t.q = q_mult(q,q_mult(v,q_conjugate(q)));

    // TODO: how is this supposed to work?
    return (t.r);
}

//! Rotate a cartesian vector using a quaternion
/*! Rotate a cartesian vector from one coordinate system to another using the
 * provided quaternion.
        \param q quaternion representing the rotation
        \param v cartesian vector to be rotated
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


//! Transform a row vector using a quaternion
/*! Transform a row vector from one coordinate system to another using the
 * provided left quaternion.
        \param q quaternion representing the transformation
        \param v row vector to be rotated
        \return row vector in the transformed system
*/
rvector transform_q(quaternion q, rvector v)
{
    uvector t = {{{0.,0.,0.},0.}};

    t.q = q_mult(q_conjugate(q),q_mult(v,q));

    return (t.r);
}



//! Create rotation quaternion from 2 vectors
/*! Generate the quaternion that represents a rotation of from one cartesian vector
 * to a second cartesian vector.
        \param from initial cartesian vector
        \param to final cartesian vector
        \return quaternion that can be used to rotate points
*/
// TODO: move this function to quaternion library
// and remove dependency of uvector
quaternion q_change_between_cv(cvector from, cvector to)
{
    uvector rq;
    cvector vec1, vec2;

    normalize_cv(from);
    normalize_cv(to);

    if (length_cv(cv_add(from,to)) < 1e-14)
    {
        vec1.x = rand();
        vec1.y = rand();
        vec1.z = rand();
        normalize_cv(vec1);
        vec2 = cv_cross(vec1,to);
        normalize_cv(vec2);
        if (length_cv(vec2)<D_SMALL)
        {
            vec1.x = rand();
            vec1.y = rand();
            vec1.z = rand();
            normalize_cv(vec1);
            vec2 = cv_cross(vec1,to);
            normalize_cv(vec2);
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



//! Default least squares fit.
/*! Constructor for default 10 element, 2nd order least squares fit.
 * \return Least squares fit object.
*/
lsfit::lsfit()
    : element_cnt(10), order(2), resolution(0.)
{
}

//! Multi element least squares fit.
/*! Constructor for cnt element, 2nd order least squares fit.
 * \param cnt Number of elements to be fit.
 * \return Least squares fit object.
*/
lsfit::lsfit(uint16_t cnt)
    : element_cnt(cnt), order(2), resolution(0.)
{
}

//! Multi element, variable order least squares fit.
/*! Constructor for cnt element, ord order least squares fit.
 * \param cnt Number of elements to be fit.
 * \param ord Order of fit.
 * \return Least squares fit object.
*/
lsfit::lsfit(uint16_t cnt, uint16_t ord)
    : element_cnt(cnt), order(ord), resolution(0.)
{
}

//! Multi element, variable order, variable resolution least squares fit.
/*! Constructor for cnt element, ord order, res resolution least squares fit.
 * \param cnt Number of elements to be fit.
 * \param ord Order of fit.
 * \param res Resolution of fit.
 * \return Least squares fit object.
*/
lsfit::lsfit(uint16_t cnt, uint16_t ord, double res)
    : element_cnt(cnt), order(ord), resolution(res)
{
    if (ord)
    {
        order = ord;
    }
    else
    {
        order = 1;
    }
    if (cnt)
    {
        element_cnt = cnt;
    }
    else
    {
        element_cnt = order + 1;
    }
    resolution = res;
    var.resize(0);
    depth = 0;
    //	printf("lsfit(%u %u %f)\n",cnt,ord,res);
}

//! Initialize Least Squares Fit
/*! Perform setting of variables in ::lsfit so that it can be ready for use.
 * \param cnt Number of elements to be fit.
 * \param ord Order of fit.
 * \param res Resolution of fit.
*/
void lsfit::initialize(uint16_t cnt, uint16_t ord, double res)
{
    if (ord)
    {
        order = ord;
    }
    else
    {
        order = 1;
    }
    if (cnt)
    {
        element_cnt = cnt;
    }
    else
    {
        element_cnt = order + 1;
    }
    resolution = res;
    var.resize(0);
    depth = 0;
}

//! Update scalar Least Squares Fit
/*! Add independent and dependent value pair to existing ::lsfit, updating the fit. If the number
 * of elements in the fit has been reached, the oldest element is dropped before fitting.
 * \param x Dependent value.
 * \param y Independent value.
*/
void lsfit::update(double x, double y)
{
    fitelement cfit;

    // Independent variable
    cfit.x = x;

    // Dependent variable for quaternion
    cfit.y.a4[0] = y;

    update(cfit, 1);
}

//! Update ::rvector Least Squares Fit
/*! Add independent and dependent value pair to existing ::lsfit, updating the fit. If the number
 * of elements in the fit has been reached, the oldest element is dropped before fitting.
 * \param x Dependent value.
 * \param y Independent values.
*/
void lsfit::update(double x, rvector y)
{
    fitelement cfit;

    // Independent variable
    cfit.x = x;

    // Dependent variable for quaternion
    cfit.y.r = y;

    update(cfit, 3);
}

//! Update ::quaternion Least Squares Fit
/*! Add independent and dependent value pair to existing ::lsfit, updating the fit. If the number
 * of elements in the fit has been reached, the oldest element is dropped before fitting.
 * \param x Dependent value.
 * \param y Independent values.
*/
void lsfit::update(double x, quaternion y)
{
    fitelement cfit;

    // Independent variable
    cfit.x = x;

    // Dependent variable for quaternion
    cfit.y.q = y;

    update(cfit, 4);

    //    // TODO: check this problem!!!
    //    //if (parms.size() && (std::isnan(parms[0][0]) || std::isnan(parms[0][1] || std::isnan(parms[0][2]))))
    //    if (1)
    //	{
    //		for (uint16_t i=0; i<var.size(); ++i)
    //		{
    //            // TODO: why are we printing this?
    //			printf("%.15g [%g %g %g %g]\n", var[i].x, var[i].y.q.w, var[i].y.q.d.x, var[i].y.q.d.y, var[i].y.q.d.z);
    //		}
    //		printf("\n");
    //	}
}

//! Update generic Least Squares Fit
/*! Add ::fitelement containing independent and dependent value pair to existing ::lsfit, updating the fit. If the number
 * of elements in the fit has been reached, the oldest element is dropped before fitting.
 * \param x Dependent value.
 * \param y Independent values.
*/
void lsfit::update(fitelement cfit, uint16_t dep)
{
    if (var.size() && cfit.x == var[var.size()-1].x)
    {
        return;
    }

    depth = dep;

    // Sudden switch to mirror value will wreak havoc with fit of quaternion
    if (depth == 4 && var.size() > 0)
    {
        // If new value is closer to mirror of last value, then switch all previous values
        if (length_q(q_sub(cfit.y.q, var[var.size()-1].y.q)) > length_q(q_sub(q_smult(-1., cfit.y.q), var[var.size()-1].y.q)))
        {
            for (uint16_t i=0; i<var.size(); ++i)
            {
                var[i].y.q = q_smult(-1., var[i].y.q);
            }
        }
    }

    // Save to FIFO
    var.push_back(cfit);

    // Element_cnt element collected and we can start cycling FIFO
    if (var.size() > element_cnt)
    {
        var.pop_front();
    }

    // More than order elements collected and we can start fitting
    if (var.size() > order)
    {
        lsfit::fit();
    }
}

//! Calculate least squares fit
/*! Calculate least squares fit for each axis of exsiting ::lsfit. Parameters are updated
 * to reflect new fit.
*/
void lsfit::fit()
{
    // Minimize independent variable by zero offsetting
    basex = var[0].x;

    // For each independent, calculate sums of powers
    std::vector<double> sumx(2*order+1);
    sumx[0] = var.size();
    for (uint16_t i=0; i<sumx[0]; ++i)
    {
        double ix = 1.;
        double cx = var[i].x - basex;
        for (uint16_t j=0; j<2*order; ++j)
        {
            ix *= cx;
            sumx[j+1] += ix;
        }
    }
    meanx = sumx[1] / sumx[0];
    stdevx = sqrt(sumx[2] - sumx[1]*sumx[1]/sumx[0])/(sumx[0]-1);

    // Calculate sums of products of dependent and independent and do least squares fit
    parms.resize(depth);
    for (uint16_t i=0; i<depth; ++i)
    {
        std::vector<double> sumxy(order+1);
        stdevy.a4[i] = 0;
        for (uint16_t j=0; j<var.size(); ++j)
        {
            double ixy = var[j].y.a4[i];
            stdevy.a4[i] += ixy * ixy;
            double cx = var[j].x - basex;
            for (uint16_t k=0; k<order+1; ++k)
            {
                sumxy[k] += ixy;
                ixy *= cx;
            }
        }
        meany.a4[i] = sumxy[0] / var.size();
        stdevy.a4[i] = sqrt(stdevy.a4[i] - sumxy[0]*sumxy[0]/sumx[0])/(sumx[0]-1);

        std::vector< std::vector<double> > xs(order+1, std::vector<double>(order+1));
        std::vector<double> ys(order+1);
        std::vector<double> tx(order+1);
        for (uint16_t j=0; j<order+1; ++j)
        {
            for (uint16_t k=0; k<order+1; ++k)
            {
                tx[k] = sumx[k+j];
            }
            xs[j] = tx;
            ys[j] = sumxy[j];
        }
        parms[i].resize(ys.size());
        multisolve(xs, ys, parms[i]);
        //		printf("%u %u %f %f %f\n", i, parms[i].size(), parms[i][0], parms[i][1], parms[i][2]);
    }
}

//! Least squares last independent value.
/*! Return the value of the independent value added at the most recent ::lsfit::update.
 * \return Most recently updated independent value.
*/
double lsfit::lastx()
{
    if (var.size())
    {
        return var[var.size()-1].x;
    }
    else
    {
        return 0.;
    }
}

//! Least squares first independent value.
/*! Return the value of the independent value added at the least recent ::lsfit::update.
 * \return Least recently updated independent value.
*/
double lsfit::firstx()
{
    if (var.size())
    {
        return var[0].x;
    }
    else
    {
        return 0.;
    }
}

//! Least squares dependent scalar value.
/*! Return the value of the dependent scalar, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated scalar dependent value.
*/
double lsfit::eval(double x)
{
    if (var.size() > order)
    {
        return evaluate_poly(x - basex, parms[0]);
    }
    else
    {
        return 0.;
    }

}

//! Least squares dependent ::rvector value.
/*! Return the value of the dependent ::rvector, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated ::rvector dependent value.
*/
rvector lsfit::evalrvector(double x)
{
    if (var.size() > order)
    {
        return rv_evaluate_poly(x - basex, parms);
    }
    else
    {
        return rv_zero();
    }
}

//! Least squares dependent ::quaternion value.
/*! Return the value of the dependent ::quaternion, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated ::quaternion dependent value.
*/
quaternion lsfit::evalquaternion(double x)
{
    if (var.size() > order)
    {
        return q_evaluate_poly(x - basex, parms);
    }
    else
    {
        return q_zero();
    }
}

//! Least squares dependent scalar 1st derivative.
/*! Return the value of the dependent scalar 1st derivative, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated scalar dependent 1st derivative.
*/
double lsfit::slope(double x)
{
    if (var.size() > order)
    {
        return evaluate_poly_slope(x - basex, parms[0]);
    }
    else
    {
        return 0.;
    }
}

//! Least squares dependent ::rvector 1st derivative.
/*! Return the value of the dependent ::rvector 1st derivative, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated ::rvector dependent 1st derivative.
*/
rvector lsfit::slopervector(double x)
{
    if (var.size() > order)
    {
        return rv_evaluate_poly_slope(x - basex, parms);
    }
    else
    {
        return rv_zero();
    }
}

//! Least squares dependent ::quaternion 1st derivative.
/*! Return the value of the dependent ::quaternion 1st derivative, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated ::quaternion dependent 1st derivative.
*/
quaternion lsfit::slopequaternion(double x)
{
    if (var.size() > order)
    {
        return q_evaluate_poly_slope(x - basex, parms);
    }
    else
    {
        return q_zero();
    }
}

//! Least squares dependent scalar 2nd derivative.
/*! Return the value of the dependent scalar 2nd derivative, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated scalar dependent 2nd derivative.
*/
double lsfit::accel(double x)
{
    if (var.size() > order)
    {
        return evaluate_poly_accel(x - basex, parms[0]);
    }
    else
    {
        return 0.;
    }
}

//! Least squares dependent ::rvector 2nd derivative.
/*! Return the value of the dependent ::rvector 2nd derivative, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated ::rvector dependent 2nd derivative.
*/
rvector lsfit::accelrvector(double x)
{
    if (var.size() > order)
    {
        return rv_evaluate_poly_accel(x - basex, parms);
    }
    else
    {
        return rv_zero();
    }
}

//! Least squares dependent ::quaternion 2nd derivative.
/*! Return the value of the dependent ::quaternion 2nd derivative, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated ::quaternion dependent 2nd derivative.
*/
quaternion lsfit::accelquaternion(double x)
{
    if (var.size() > order)
    {
        return q_evaluate_poly_accel(x - basex, parms);
    }
    else
    {
        return q_zero();
    }
}

//! Least squares dependent scalar 3rd derivative.
/*! Return the value of the dependent scalar 3rd derivative, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated scalar dependent 3rd derivative.
*/
double lsfit::jerk(double x)
{
    if (var.size() > order)
    {
        return evaluate_poly_jerk(x - basex, parms[0]);
    }
    else
    {
        return 0.;
    }
}

//! Least squares dependent ::rvector 3rd derivative.
/*! Return the value of the dependent ::rvector 3rd derivative, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated ::rvector dependent 3rd derivative.
*/
rvector lsfit::jerkrvector(double x)
{
    if (var.size() > order)
    {
        return rv_evaluate_poly_jerk(x - basex, parms);
    }
    else
    {
        return rv_zero();
    }
}

//! Least squares dependent ::quaternion 3rd derivative.
/*! Return the value of the dependent ::quaternion 3rd derivative, calculated for the provided independent value, using the parameters from
 * the latest ::lsfit::update.
 * \param x Independent value.
 * \return Calculated ::quaternion dependent 3rd derivative.
*/
quaternion lsfit::jerkquaternion(double x)
{
    if (var.size() > order)
    {
        return q_evaluate_poly_jerk(x - basex, parms);
    }
    else
    {
        return q_zero();
    }
}

//! Least Squares parameters
/*! Return the values of the parameters for all axes of generated from the latest ::lsfit::update.
 * \return Parameters
*/
std::vector<std::vector<double> > lsfit::getparms()
{
    return parms;
}



//! @}
