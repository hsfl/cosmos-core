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

#ifndef _MATH_VECTOR_H
#define _MATH_VECTOR_H

//#include "types.h"
#include "support/configCosmos.h"
#include "constants.h"

#include <iostream>
#include <iomanip>      // std::setw
#include <cmath>
#include <vector>

#ifdef _MSC_BUILD
#include <stdint.h> // for uint16_t et all.
#endif

//! 3 element generic row vector
/*! 3 double precision numbers representing a 3 element row major vector.
*/
// TODO: rename to row_vector
// TODO: create a generic type of vector like: vector3 that can be accessed wither by .x, .y, .z or with [0], [1], [2]
// TODO: consider using Eigen, no because Eigen is a heavily templated library and takes a long time to compile and has way more than needed
// it can be used for specific projects that require it
struct rvector
{
    double col[3];
};

std::ostream& operator << (std::ostream& out, const rvector& a);
std::ostream& operator << (std::ostream& out, const vector<rvector>& a);
std::istream& operator >> (std::istream& out, rvector& a);
rvector operator * (rvector v, double scalar); // multiply vector by vector operator
rvector operator * (rvector v1, rvector v2); // multiply vector by vector operator
rvector operator / (rvector v, double scalar); // divide vector by scalar operator
bool operator == (rvector a, rvector b); // Compares two vectors
bool operator != (rvector a, rvector b); // Compares two vectors

//! 3 element cartesian vector
/*! 3 double precision numbers representing a vector in a right handed
 * cartesian space */
class cvector
{

public:
    //! X value
    double x;
    //! Y value
    double y;
    //! Z value
    double z;

    void normalize(double scale=1.);
    double length();
    double norm();
    double norm2();
    cvector normalized(double scale=1.);
    double& operator[] (const int index);
} ;

//! 3 element spherical vector
/*! 3 double precision numbers representing a vector in a spherical
 * space. Lambda increases east. */
struct svector
{
    //! N/S in radians
    double phi;
    //! E/W in radians
    double lambda;
    //! Radius in meters
    double r;
} ;

std::ostream& operator << (std::ostream& out, const svector& a);
std::istream& operator >> (std::istream& out, svector& a);

//! 3 element geodetic vector
/*! 3 double precision numbers representing a vector in a WGS84
 * based geodetic space. Longitude increases east. */
// TODO: replace name gvector->geodetic
struct gvector
{
    //! Latitude in radians
    double lat;
    //! Longitude in radians
    double lon;
    //! Height in meters
    double h;
};

std::ostream& operator << (std::ostream& out, const gvector& a);
std::istream& operator >> (std::istream& out, gvector& a);

//! 3 element attitude vector.
/*! Uses Tait-Bryan representation in a  zyx, right handed order of
 * rotation */
// TODO: change name to something more explicit
struct avector
{
    //! Heading
    double h;
    //! Elevation
    double e;
    //! Bank
    double b;
} ;

std::ostream& operator << (std::ostream& out, const avector& a);
std::istream& operator >> (std::istream& out, avector& a);

std::ostream& operator << (std::ostream& out, const cvector& a);
std::istream& operator >> (std::istream& in, cvector& a);

// Geodetic Vector operations
gvector gv_zero();

// Row Vector operations
rvector rv_zero();
rvector rv_shortest(rvector v);
rvector rv_shortest2(rvector v);
rvector rv_unitx(double scale=1.);
rvector rv_unity(double scale=1.);
rvector rv_unitz(double scale=1.);
rvector rv_one();
rvector rv_one(double x, double y, double z);
rvector rv_smult(double a, rvector b);
rvector rv_normal(rvector v);
rvector rv_normalto(rvector p0, rvector p1, rvector p2);
rvector rv_sadd(double a, rvector b);
rvector rv_add(rvector a, rvector b);
rvector rv_sub(rvector a, rvector b);
rvector rv_mult(rvector a, rvector b);
rvector rv_div(rvector a, rvector b);
rvector rv_sqrt(rvector a);
rvector rv_cross(rvector a, rvector b);
rvector rv_convert(svector from);

double norm_rv(rvector a);

void normalize_rv(rvector &v);

double sep_rv(rvector v1, rvector v2);
//double sep_rv2(rvector v1, rvector v2);
double dot_rv(rvector a, rvector b);
double length_rv(rvector v);
bool equal_rv(rvector v1, rvector v2);
double sum_rv(rvector a);


svector s_convert(rvector from);


// Column Vector operations

cvector cv_zero();
cvector cv_unitx();
cvector cv_unity();
cvector cv_unitz();
cvector cv_one();
cvector cv_normal(cvector v);
cvector cv_cross(cvector a, cvector b);
cvector cv_sadd(double a, cvector b);
cvector cv_add(cvector a, cvector b);
cvector cv_sub(cvector a, cvector b);
cvector cv_mult(cvector a, cvector b);
cvector cv_div(cvector a, cvector b);
cvector cv_smult(double a, cvector b);
cvector cv_sqrt(cvector a);
void normalize_cv(cvector &v);

double sep_cv(cvector v1, cvector v2);
double dot_cv(cvector a, cvector b);
double length_cv(cvector v);
double norm_cv(cvector v);
double cv_norm(cvector v);
double sum_cv(cvector a);

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
void normalize_q(quaternion *q);
quaternion q_zero();
quaternion q_conjugate(quaternion q);
quaternion q_times(quaternion q1, quaternion q2);
quaternion q_sqrt(quaternion q1);
quaternion q_fmult(rvector r1, quaternion q2);
quaternion q_fmult(quaternion q1, quaternion q2);
quaternion q_mult(quaternion q1, quaternion q2);
quaternion q_rmult(quaternion q1, quaternion q2);
quaternion q_smult(double a, quaternion q);
quaternion q_add(quaternion q1, quaternion q2);
quaternion q_sub(quaternion q1, quaternion q2);
quaternion q_euler2quaternion(avector rpw);
quaternion q_axis2quaternion_cv(cvector v);
quaternion q_axis2quaternion_rv(rvector v);

quaternion q_drotate_between_rv(rvector from, rvector to);

quaternion q_change_around_cv(cvector around, double angle);
quaternion q_change_around_rv(rvector around, double angle);

quaternion q_change_around_x(double angle);
quaternion q_change_around_y(double angle);
quaternion q_change_around_z(double angle);

// TODO: check this function, does not seem to be implemented
//quaternion q_rotate_around(int axis, double angle);

quaternion q_irotate_for(rvector sourcea, rvector sourceb, rvector targeta, rvector targetb);
quaternion q_eye();
quaternion q_identity();
quaternion q_evaluate_poly(double x, vector< vector<double> > parms);
quaternion q_evaluate_poly_slope(double x, vector< vector<double> > parms);
quaternion q_evaluate_poly_accel(double x, vector< vector<double> > parms);
quaternion q_evaluate_poly_jerk(double x, vector< vector<double> > parms);

double length_q(quaternion q);
double norm_q(quaternion q);
double sep_q(quaternion q1, quaternion q2);
double inner_q(quaternion q1, quaternion q2);

void qrotate(double ipos[3], double rpos[3], double angle, double *opos);
avector a_quaternion2euler(quaternion q);

// TODO: bring these functions from mathlib
quaternion q_change_between_cv(cvector from, cvector to);
quaternion q_change_between_rv(rvector from, rvector to);
//cvector rotate_q(quaternion q, rvector v);
//cvector rotate_q(quaternion q, cvector v);
//cvector irotate(quaternion q,cvector v); // doesn't exist in .cpp?


namespace Cosmos {
    namespace Math {
        namespace Vectors {
        //! Vector Class
        /*! eventually this is where all row vector stuff would come?
 * do we really need to differentiate between cvector and rvector in the future?
 * maybe we can have vector type = {'c', 'r'}
*/
        class Vector
        {

        public:

            //! Constructor and initializer.
            //! Initialize ::Vector to zero.

            explicit Vector(double x0=0., double y0=0., double z0=0., double w0=0.)
            {
                x = x0;
                y = y0;
                z = z0;
                w = w0;
            }

            explicit Vector(rvector rv)
            {
                x = rv.col[0];
                y = rv.col[1];
                z = rv.col[2];
                w = 0.;
            }

            explicit Vector(quaternion q)
            {
                x = q.d.x;
                y = q.d.y;
                z = q.d.z;
                w = q.w;
            }

            explicit Vector(cvector cv)
            {
                x = cv.x;
                y = cv.y;
                z = cv.z;
                w = 0.;
            }



            //! X value
            double x;
            //! Y value
            double y;
            //! Z value
            double z;
            //! W value
            double w;

            // TODO: check if we can iterated the vector
            double at(int i);
            // convert from cartesian vector to row vector
//            Vector from_cv(cvector cv);
//            Vector from_rv(rvector rv);
//            Vector from_q(quaternion q);
            rvector to_rv();
            cvector to_cv();
            quaternion to_q();
            Vector cross(Vector b);
            double dot(Vector b);
            Vector &normalize(double size=1.);
            double norm();
            double separation(Vector b);
            double area(Vector b);
            Vector vector();
            Vector flattenx();
            Vector flatteny();
            Vector flattenz();
            double sum();

            Vector &operator = (const rvector rv)
            {
                *this = Vector(rv);
                return *this;
            }

            double &operator [] (const int &index);

            Vector operator *(const double scale) const; // multiply vector by scalar operator
            Vector &operator *=( const double &scale); // multiply vector by scalar operator in place
            Vector operator * (const Vector &v2) const; // multiply vector by vector operator

            Vector operator / (const double scale) const; // divide vector by scalar operator
            Vector &operator /= (const double &scale); // divide vector by scalar operator in place

            Vector operator - (const Vector &v2) const; // Subtract vector by vector operator
            Vector &operator -= (const Vector &v2); // subtract vector by vector operator in place
            Vector operator - () const; // Negate vector operator

            Vector operator + (const Vector &v2) const; // add vector by vector operator
            Vector &operator += (const Vector &v2); // add vector by vector operator in place

            bool operator == (const Vector &v2) const; // Compares two vectors
            bool operator != (const Vector &v2) const; // Compares two vectors
        };

        Vector operator * (const double scale, const Vector &v);
        std::ostream& operator << (std::ostream& out, const Vector& v);
        Vector eye(double scale=1.);
        Vector unitxV(double scale=1.);
        Vector unityV(double scale=1.);
        Vector unitzV(double scale=1.);
        }


        namespace Quaternions {
        class Quaternion : public Vectors::Vector
        {

        public:

            explicit Quaternion(double qx=0., double qy=0., double qz=0., double qw=0.) : Vector(qx, qy, qz, qw)
            {
                x = qx;
                y = qy;
                z = qz;
                w = qw;
            }

            explicit Quaternion(Vector v)
            {
                x = v.x;
                y = v.y;
                z = v.z;
                w = 0.;
            }

            explicit Quaternion(quaternion q)
            {
                x = q.d.x;
                y = q.d.y;
                z = q.d.z;
                w = q.w;
            }

            explicit Quaternion(rvector rv) : Vector(rv)
            {
                x = rv.col[0];
                y = rv.col[1];
                z = rv.col[2];
                w = 0.;
            }

            quaternion to_q();
            Quaternion getQuaternion();

            // temporary while the new Quaternion class is not finisheds
            Quaternion quaternion2Quaternion(quaternion q);
            quaternion Quaternion2quaternion(Quaternion Q);


            // operators
//            Quaternion &operator = (const Quaternion &q) { return *this; }
            Quaternion &operator = (const quaternion &q2);
            Quaternion operator * (const double scale) const; // multiply vector by scalar operator
            Quaternion &operator *= ( const double scale); // multiply vector by scalar operator in place
            Quaternion operator * (const Quaternion &q2) const;

//            Quaternion operator / (double scale); // multiply vector by scalar operator
//            Quaternion &operator /= (const double &scale); // multiply vector by scalar operator in place

            Quaternion operator - (const Quaternion &q2) const; // Subtract vector by vector operator
            Quaternion &operator -= (const Quaternion &q2); // subtract vector by vector operator in place
            Quaternion operator - () const; // Negate vector operator

            Quaternion operator + (const Quaternion &q2); // add vector by vector operator
            Quaternion &operator += (const Quaternion &q2); // add vector by vector operator in place
            //std::ostream& operator<<(std::ostream& os, const Quaternion& q);
            //friend std::ostream& operator << (std::ostream& os, const Quaternion& q);
            //std::istream& operator >> (std::istream& out, Quaternion& a);

            Quaternion multiplyScalar(double a);
            Quaternion conjugate() const;
            Vector vector();
            Vector omegaFromDerivative(Quaternion dq);
//            Quaternion &normalize();
//            double norm();
            Vector toEuler();
            Vector irotate(const Vector &v);
            Vector drotate(const Vector &v);
        };

        // declared outside class because it does not need to access members of the class Quaternion
        Quaternion operator * (double scale, const Quaternion &q);
        Quaternion operator * (const Vectors::Vector &v, const Quaternion &q);
        std::ostream& operator << (std::ostream& os, const Quaternion& q);
        Quaternion irotate_for(Vectors::Vector sourcea, Vectors::Vector sourceb, Vectors::Vector targeta, Vectors::Vector targetb);
        Quaternion drotate_between(Vectors::Vector a, Vectors::Vector b);
        Quaternion eye(double scale = 1.);

        }
    } // end namespace Math
} // end namespace COSMOS

#endif // _MATH_VECTOR_H
