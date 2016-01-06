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
// TODO: consider using Eigen
struct rvector
{
    double col[3];
};

std::ostream& operator << (std::ostream& out, const rvector& a);
std::ostream& operator << (std::ostream& out, const std::vector<rvector>& a);
std::istream& operator >> (std::istream& out, rvector& a);
rvector operator * (rvector v, double scalar); // multiply vector by scalar operator
rvector operator / (rvector v, double scalar); // multiply vector by scalar operator

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

    void normalize();
    double norm();
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

// Row Vector operations
rvector rv_zero();
rvector rv_shortest(rvector v);
rvector rv_shortest2(rvector v);
rvector rv_unitx();
rvector rv_unitx(double scale);
rvector rv_unity();
rvector rv_unity(double scale);
rvector rv_unitz();
rvector rv_unitz(double scale);
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
rvector rv_evaluate_poly(double x, std::vector< std::vector<double> > parms);
rvector rv_evaluate_poly_slope(double x, std::vector< std::vector<double> > parms);
rvector rv_evaluate_poly_accel(double x, std::vector< std::vector<double> > parms);
rvector rv_evaluate_poly_jerk(double x, std::vector< std::vector<double> > parms);
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


namespace Cosmos {
    namespace Math {
        //! Vector Class
        /*! eventually this is where all row vector stuff would come?
 * do we really need to differentiate between cvector and rvector in the future?
 * maybe we can have vector type = {'c', 'r'}
*/
        class Vector{

        public:

            // default constructor
            Vector();
            Vector(double x, double y, double z);

            // TODO: check if we can iterated the vector
            double at(int i);

            //! X value
            double x;
            //! Y value
            double y;
            //! Z value
            double z;

            // convert from cartesian vector to row vector
            rvector from_cv(cvector v);
            Vector cross(Vector b);
            double dot(Vector b);
            void normalize();
            double norm();

            Vector operator * (double scale); // multiply vector by scalar operator

        };

        std::ostream& operator << (std::ostream& out, const Vector& v);
        Vector operator * (double scale, Vector v);


    } // end namespace Math
} // end namespace COSMOS

#endif // _MATH_VECTOR_H
