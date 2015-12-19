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


//! \addtogroup mathlib_functions
//! @{

namespace Cosmos {
    namespace Math {

        Quaternion::Quaternion()
        {
            // by default create the identity quaternion
            x = 0;
            y = 0;
            z = 0;
            w = 1;
        }

        Quaternion::Quaternion(double qx, double qy, double qz, double qw)
        {
            x = qx;
            y = qy;
            z = qz;
            w = qw;
        }

        // TODO: change to Vector
        cvector Quaternion::vector()
        {
            cvector o; // output

            o.x = x;
            o.y = y;
            o.z = z;

            return o;
        }


        // same as q_conjugate(quaternion q)
        Quaternion Quaternion::conjugate()
        {
            Quaternion o; // output

            o.w = w;
            o.x = -x;
            o.y = -y;
            o.z = -z;

            return o;
        }

        // same as q_smult(double a, quaternion b)
        Quaternion Quaternion::multiplyScalar(double a)
        {
            Quaternion c;

            c.w = a * w;
            c.x = a * x;
            c.y = a * y;
            c.z = a * z;

            return c;
        }


        // function to easily convert from the Quaternion to quaternion
        // hopefully this function will be removed when the quaternion type
        // has been fully converted to Quaternion
        quaternion Quaternion::Quaternion2quaternion(Quaternion Q)
        {
            quaternion q;

            q.d.x = Q.x;
            q.d.y = Q.y;
            q.d.z = Q.z;
            q.w = Q.w;

            return q;
        }

        // function to easily convert from the quaternion type to Quaternion
        // hopefully this function will be removed when the quaternion type
        // has been fully converted to Quaternion
        Quaternion Quaternion::quaternion2Quaternion(quaternion q)
        {
            Quaternion Q;

            Q.x = q.d.x;
            Q.y = q.d.y;
            Q.z = q.d.z;
            Q.w = q.w;

            return Q;
        }

        // adition operator for quaternion class
        Quaternion Quaternion::operator+(const Quaternion& q)
        {
            Quaternion q1, q2, q3;

            q1 = *this;
            q2 = q;

            //    q3->x = q1.x + q2.x;
            //    q3->y = q1.y + q2.y;
            //    q3->z = q1.z + q2.z;
            //    q3->w = q1.w + q2.w;

            q3.x = q1.x + q2.x;
            q3.y = q1.y + q2.y;
            q3.z = q1.z + q2.z;
            q3.w = q1.w + q2.w;

            return q3;
        }

        // substraction operator for quaternion class
        Quaternion Quaternion::operator-(const Quaternion& q)
        {
            Quaternion q1,q2,q3;

            q1 = *this;
            q2 = q;

            q3.x = q1.x - q2.x;
            q3.y = q1.y - q2.y;
            q3.z = q1.z - q2.z;
            q3.w = q1.w - q2.w;

            return q3;
        }

        // product operator for quaternion class
        Quaternion Quaternion::operator*(const Quaternion& q)
        {
            Quaternion q1,q2,q3;

            q1 = *this;
            q2 = q;

            q3.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
            q3.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
            q3.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
            q3.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

            return q3;
        }


        std::ostream& operator<<(std::ostream& os, const Quaternion& q)
        {
            //out << "[(";
            os << q.x << ",";
            os << q.y << ",";
            os << q.z << ", ";
            os << q.w;
            //out << "]"; //<< std::endl;
            return os;
        }


        // copy *this quaternion into a new Quaternion object
        // basically this collapses (x,y,z,w) into a Quaternion object
        Quaternion Quaternion::getQuaternion()
        {
            Quaternion o;
            o.x = x;
            o.y = y;
            o.z = z;
            o.w = w;

            return o;
        }

        // calculate the angular rate given the current quaternion and it's derivative
        cvector Quaternion::omegaFromDerivative(Quaternion dq)
        {
            cvector o; // output
            Quaternion q = getQuaternion();
            o = (dq * q.conjugate()).multiplyScalar(2.).vector();

            return o;
        }

        //! compute the quaternion that represents the rotation from vector a to vector b
        //! Ref: - http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
        void Quaternion::fromTwoVectors(Vector a, Vector b)
        {
            // normalize the vectors in place
            a.normalize();
            b.normalize();

            Vector w = a.cross(b);
            Quaternion q = Quaternion(w.x, w.y, w.z, 1.f + a.dot(b));

            q.normalize();

            *this = q;
        }

        //! Normalizes the quaternion in place (*this)
        void Quaternion::normalize()
        {
            double norm;

            this->w = round(this->w/D_SMALL)*D_SMALL;
            this->x = round(this->x/D_SMALL)*D_SMALL;
            this->y = round(this->y/D_SMALL)*D_SMALL;
            this->z = round(this->z/D_SMALL)*D_SMALL;

            norm = this->norm();

            if (fabs(norm - (double)0.) > D_SMALL && fabs(norm - (double)1.) > D_SMALL)
            {
                this->w /= norm;
                this->x /= norm;
                this->y /= norm;
                this->z /= norm;
            }
        }

        // ref: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
        Vector Quaternion::toEuler(){

            Vector euler;

            this->normalize();

            double sqx = this->x * this->x;
            double sqy = this->y * this->y;
            double sqz = this->z * this->z;
            double phi   = atan2( 2.*(this->w*this->x + this->y*this->z), 1. - 2.*(sqx + sqy));
            double theta = asin(  2.*(this->x*this->y - this->z*this->x) );
            double psi   = atan2( 2.*(this->w*this->z + this->x*this->y), 1. - 2.*(sqy + sqz));

            euler.x = phi;
            euler.y = theta;
            euler.z = psi;

            return (euler);
        }



        //! return the norm of the quaternion's coefficients
        double Quaternion::norm()
        {
            return ( sqrt(this->w*this->w + this->x*this->x + this->y*this->y + this->z*this->z) ) ;
        }

    } // end namespace Math
} // end namespace Cosmos


// ----------------------------------------------

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

//! Square root of the elements of a quaternion
/*! Take the square root of each element of one quaternion.
 * \param q1 quaternion
 * \return Quaternion result
 */
quaternion q_sqrt(quaternion q1)
{
    quaternion o;

    o.d.x = sqrt(q1.d.x);
    o.d.y = sqrt(q1.d.y);
    o.d.z = sqrt(q1.d.z);
    o.w = sqrt(q1.w);

    return (o);
}

//! rvector quaternion multiply
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

//! Quaternion multiplication. The result of multiplying two quaternions is the
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
// TODO: check the redundancy with q_normalize
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
        v = {0.,0.,0.};

    return (v);
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

    // normalize vector
    // before it was normalize_cv(around);

    double mag = around.x*around.x
                 + around.y*around.y
                 + around.z*around.z;

    if (fabs(mag - (double)0.) > D_SMALL && fabs(mag - (double)1.) > D_SMALL)
    {
        mag = sqrt(mag);
        around.x /= mag;
        around.y /= mag;
        around.z /= mag;
    }

    rq.d.x = around.x * sa;
    rq.d.y = around.y * sa;
    rq.d.z = around.z * sa;
    rq.w = cos(angle);
    q_normalize(&rq);
    return (rq);
}


//! Rotation quaternion for X axis
/*! Create the ::quaternion that represents a rotation of the given angle around the X axis.
        \param angle Angle of rotation in radians
        \return Resulting ::quaternion
*/
// TODO: change function name to q_rotate_object_around_z vs q_rotate_coord_around_z
quaternion q_change_around_x(double angle)
{
    quaternion a = {{1.,0.,0.},0.};

    // removed cv_mult to clean the dependency from mathlib
    // previously was: a.d = cv_smult(sin(angle/2.),a.d);

    // new
    double sa = sin(angle/2.);

    a.d.x = sa * a.d.x;
    a.d.y = sa * a.d.y;
    a.d.z = sa * a.d.z;

    a.w = cos(angle/2.);

    return (a);
}

//! Rotation quaternion for Y axis
/*! Create the ::quaternion that represents a rotation of the given angle around the Y axis.
        \param angle Angle of rotation in radians
        \return Resulting ::quaternion
*/
// TODO: change function name to q_rotate_object_around_z vs q_rotate_coord_around_z
quaternion q_change_around_y(double angle)
{
    quaternion a = {{0.,1.,0.},0.};

    // removed cv_mult to clean the dependency from mathlib
    // previously was: a.d = cv_smult(sin(angle/2.),a.d);

    // new
    double sa = sin(angle/2.);

    a.d.x = sa * a.d.x;
    a.d.y = sa * a.d.y;
    a.d.z = sa * a.d.z;

    a.w = cos(angle/2.);

    return (a);
}


//! Rotation quaternion for Z axis
/*! Create the ::quaternion that represents a rotation of the given angle around the Z axis.
        \param angle Angle of rotation in radians
        \return Resulting ::quaternion
*/
// TODO: change function name to q_rotate_object_around_z vs q_rotate_coord_around_z
quaternion q_change_around_z(double angle)
{
    quaternion a = {{0.,0.,1.},0.};

    // removed cv_mult to clean the dependency from mathlib
    // previously was: a.d = cv_smult(sin(angle/2.),a.d);

    // new
    double sa = sin(angle/2.);

    a.d.x = sa * a.d.x;
    a.d.y = sa * a.d.y;
    a.d.z = sa * a.d.z;
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


//! @}

// default operator for printing the quaternion values
std::ostream& operator << (std::ostream& out, const quaternion& a)
{
    //out << std::fixed;
    //out << std::setprecision(5);
    //out<< "[("
    //out << std::setw(6);
    out << a.d.x << ",";
    //out << std::setw(6);
    out << a.d.y << ",";
    //out << std::setw(6);
    out << a.d.z << ",,";
    //out << std::setw(6)
    out << a.w;
    //   << "]";

    // remove formating for floatfield (not set)
    // std::cout.unsetf ( std::ios::floatfield );

    return out;
}

std::istream& operator >> (std::istream& in, quaternion& a)
{
    char junk;
    in >> junk >> a.d.x >> junk >> a.d.y >> junk >> a.d.z >> junk >> a.w >> junk;
    return in;
}

std::ostream& operator << (std::ostream& out, const qcomplex& a)
{
    out<< "[" << a.i << "," << a.j << "," << a.k << "," << a.r << "]";
    return out;
}

std::istream& operator >> (std::istream& in, qcomplex& a)
{
    char junk;
    in >> junk >> a.i >> junk >> a.j >> junk >> a.k >> junk >> a.r >> junk;
    return in;
}

std::ostream& operator << (std::ostream& out, const qlast& a)
{
    out << "[" << a.q1 << "," << a.q2 << "," << a.q3 << "," << a.q4 << "]";
    return out;
}

std::istream& operator >> (std::istream& in, qlast& a)
{
    char junk;
    in >> junk >> a.q1 >> junk >> a.q2 >> junk >> a.q3 >> junk >> a.q4 >> junk;
    return in;
}

std::ostream& operator << (std::ostream& out, const qfirst& a)
{
    out << "[" << a.q0 << "," << a.q1 << "," << a.q2 << "," << a.q3 << "]";
    return out;
}

std::istream& operator >> (std::istream& in, qfirst& a)
{
    char junk;
    in >> junk >> a.q0 >> junk >> a.q1 >> junk >> a.q2 >> junk >> a.q3 >> junk;
    return in;
}
