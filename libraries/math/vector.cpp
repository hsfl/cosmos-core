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

#include "vector.h"

//! \addtogroup mathlib_functions
//! @{

//! Angular separation between row vectors.
/*! Calculates the separation angle between two row order vectors, in radians.
        \param v1 the first vector, in ::rvector format
        \param v2 the first vector, in ::rvector format
        \return The separation angle in radians as a double precision
*/
double sep_rv(rvector v1, rvector v2)
{
    rvector dv = {{0.}};

    normalize_rv(&v1);
    normalize_rv(&v2);

    dv = rv_sub(v2,v1);
    double diff = length_rv(dv);

    double sepangle = 2. * atan2(diff/2.,sqrt(fmax(0.,1.-diff*diff/4.)));
    return (sepangle);
}

double sep_rv2(rvector v1, rvector v2)
{
    return acos(dot_rv2(v1,v2)/(length_rv2(v1)*length_rv2(v2)));
}

//! Convert ::rvector to ::svector
/*! Convert vector in cartesian coordinates to vector in spherical coordinates.
 * \param from Vector in cartesian coordinates to be converted.
 * \return Vector in spherical coordinates.
 */
svector s_convert(rvector from)
{
    svector result;

    double minir2 = from.col[0] * from.col[0] + from.col[1] * from.col[1];
    double r2 = minir2 + from.col[2] * from.col[2];
    result.r = sqrt(r2);

    double sp = from.col[2] / result.r;
    result.phi = asin(sp);
    result.lambda = atan2(from.col[1], from.col[0]);

    return result;
}

//! Convert ::svector to ::rvector
/*! Convert vector in spherical coordinates to vector in cartesian coordinates.
 * \param from Vector in spherical coordinates to be converted.
 * \return Vector in cartesian coordinates.
 */
rvector rv_convert(svector from)
{
    rvector result;

    double sp = sin(from.phi);
    double cp = cos(from.phi);
    double sl = sin(from.lambda);
    double cl = cos(from.lambda);
    double cpr = cp * from.r;

    result.col[0] = cpr * cl;
    result.col[1] = cpr * sl;
    result.col[2] = from.r * sp;

    return result;
}

//! Zero row order vector
/*! Creates a zero length row order vector.
        \return a ::rvector of zero length
*/
rvector rv_zero()
{
    rvector v={{0.,0.,0.}};

    return (v);
}

//! Unit x row vector
/*! Creates a row order vector with the X value set to one.
        \return the vector
*/
rvector rv_unitx()
{
    rvector v={{1.,0.,0.}};
    return (v);
}

//! Scaled x row vector
/*! Creates a row order vector with the X value set to scale.
        \return the vector
*/
rvector rv_unitx(double scale)
{
    rvector v={{1.,0.,0.}};
    v.col[0] *= scale;
    return (v);
}

//! Unit y row vector
/*! Creates a row order vector with the Y value set to one.
        \return the vector
*/
rvector rv_unity()
{
    rvector v={{0.,1.,0.}};
    return (v);
}

//! Scaled y row vector
/*! Creates a row order vector with the Y value set to scale.
        \return the vector
*/
rvector rv_unity(double scale)
{
    rvector v={{0.,1.,0.}};
    v.col[1] *= scale;
    return (v);
}

//! Unit z row vector
/*! Creates a row order vector with the Z value set to one.
        \return the vector
*/
rvector rv_unitz()
{
    rvector v={{0.,0.,1.}};
    return (v);
}

//! Scaled z row vector
/*! Creates a row order vector with the Z value set to scale.
        \return the vector
*/
rvector rv_unitz(double scale)
{
    rvector v={{0.,0.,1.}};
    v.col[2] *= scale;
    return (v);
}

//! Row vector of ones
/*! Creates a row order vector with all values set to one.
        \return a ::rvector with each dimension set to 1.
*/
rvector rv_one()
{
    rvector v={{1.,1.,1.}};

    return (v);
}

//! Row vector of values
/*! Creates a row order vector with values set to x, y, and z.
        \return a ::rvector with dimensions set to provided values.
*/
rvector rv_one(double x, double y, double z)
{
    rvector v={{1.,1.,1.}};
    v.col[0] *= x;
    v.col[1] *= y;
    v.col[2] *= z;

    return (v);
}

//! Shortest vector
/*! Creates a row order vector of unit length in the direction of the shortest element of the vector provided.
  \param v The ::rvector from which to draw the shortest element.
  \return The unit length ::rvector in the proper direction.
*/
rvector rv_shortest(rvector v)
{
    rvector a={{1.,0.,0.}};

    for (int i=1; i<3; i++)
    {
        if (fabs(v.col[i]) < fabs(v.col[i-1]))
        {
            a.col[i-1] = 0.;
            a.col[i] = 1.;
        }
    }
    return (a);
}

rvector rv_shortest2(rvector v)
{
    rvector rx = {{1,0,0}};
    rvector ry = {{0,1,0}};
    rvector rz = {{0,0,1}};

    if (v.col[0]<=v.col[1] && v.col[0]<=v.col[2])
        return (rx);
    else if (v.col[1]<=v.col[0] && v.col[1]<=v.col[2])
        return (ry);
    else
        return (rz);
}

//! Normalize row order vector
/*! Returns a normalized version of the requested row order vector.
        \param v the ::rvector to be normalized
        \return the normalized version of the vector as ::rvector
*/
// TODO: change name to normalize, normal means perpendicular
rvector rv_normal(rvector v)
{
    normalize_rv(&v);
    return (v);
}

//! Normalize row order vector in place.
/*! Normalizes requested row order vector.
        \param v a pointer to the ::rvector to be normalized
*/
void normalize_rv(rvector *v)
{
    double mag;

    mag = length_rv(*v);

    // if the current length is not zero (or already one)
    if (fabs(mag - (double)0.) > D_SMALL && fabs(mag - (double)1.) > D_SMALL)
    {
        v->col[0] /= mag;
        v->col[1] /= mag;
        v->col[2] /= mag;
    }
}

void normalize_rv2(rvector &v)
{
    double mag;

    mag = length_rv(v);

    // if the current length is not zero (or already one)
    if (fabs(mag - (double)0.) > D_SMALL && fabs(mag - (double)1.) > D_SMALL)
    {
        v.col[0] /= mag;
        v.col[1] /= mag;
        v.col[2] /= mag;
    }
}


//! basic function to compute the L2 norm of a 3d generic vector with separate entries
// TODO: create new class vector3
double normVector3(double x, double y, double z)
{
    return sqrt(x*x + y*y + z*z);
}

//! basic function to normalize any 3d vector with separate entries
// TODO: create new class vector3
void normalizeVector3(double &x, double &y, double &z)
{
    double norm = normVector3(x,y,z);

    // if the current norm is not zero (or already one)
    if (fabs(norm - (double)0.) > D_SMALL && fabs(norm - (double)1.) > D_SMALL)
    {
        x /= norm;
        y /= norm;
        z /= norm;
    }
}


//! Multiply row vector by scalar
/*! Multiply a row vector by a double precision scalar.
        \param b vector to be tranformed, in ::rvector form
        \param a double precision scalar to multiply by
        \return the transformed vector, in ::rvector form
*/
rvector rv_smult(double a, rvector b)
{
    rvector c = {{0.}};

    c.col[0] = a * b.col[0];
    c.col[1] = a * b.col[1];
    c.col[2] = a * b.col[2];
    return (c);
}

//! Add scalar to each element of vector.
/*! Add a double precision scalar to each element of a 3 element
 * vector.
        \param b vector to be tranformed, in ::rvector form
        \param a double precision scalar to add
        \return the transformed vector, in ::rvector form
*/
rvector rv_sadd(double a, rvector b)
{
    rvector c = {{0.}};

    c.col[0] = a + b.col[0];
    c.col[1] = a + b.col[1];
    c.col[2] = a + b.col[2];
    return (c);
}

//! Add two row vectors
/*! Add two vectors in ::rvector form, returning an ::rvector.
        \param a first vector to be added, in ::rvector form
        \param b second vector to be added, in ::rvector form
        \result the transformed vector, in ::rvector form
*/
rvector rv_add(rvector a, rvector b)
{
    rvector c = {{0.}};

    c.col[0] = a.col[0] + b.col[0];
    c.col[1] = a.col[1] + b.col[1];
    c.col[2] = a.col[2] + b.col[2];
    return (c);
}

//! Subtract two vectors
/*! Subtract two vectors in ::rvector form, returning a ::rvector.
        \param a vector to be subtracted from, in ::rvector form
        \param b vector to be subtracted, in ::rvector form
        \result the transformed vector, in ::rvector form
*/
rvector rv_sub(rvector a, rvector b)
{
    rvector c = {{0.}};

    c.col[0] = a.col[0] - b.col[0];
    c.col[1] = a.col[1] - b.col[1];
    c.col[2] = a.col[2] - b.col[2];
    return (c);
}

//! Divide two row vectors
/*! Divide one ::rvector by another, returning a ::rvector.
        \param a vector to be divided by, in ::rvector form
        \param b vector to divide by, in ::rvector form
        \result the transformed vector, in ::rvector form
*/
rvector rv_div(rvector a, rvector b)
{
    rvector c = {{0.}};

    c.col[0] = a.col[0] / b.col[0];
    c.col[1] = a.col[1] / b.col[1];
    c.col[2] = a.col[2] / b.col[2];
    return (c);
}

//! Multiply two row vectors
/*! Multiply two vectors in ::rvector form, returning a ::rvector.
        \param a first vector to be multiplied, in ::rvector form
        \param b second vector to be multiplied, in ::rvector form
        \result the transformed vector, in ::rvector form
*/
rvector rv_mult(rvector a, rvector b)
{
    rvector c = {{0.}};

    c.col[0] = a.col[0] * b.col[0];
    c.col[1] = a.col[1] * b.col[1];
    c.col[2] = a.col[2] * b.col[2];
    return (c);
}

//! Take cross product of two row vectors.
/*! Take the vector cross product of two 3 element vectors in ::rvector form, returning an ::rvector result.
        \param a First vector in product
        \param b Second vector in product, treated as column vector
        \return a cross b
*/
rvector rv_cross(rvector a, rvector b)
{
    rvector c = {{0.}};

    c.col[0] = a.col[1]*b.col[2] - a.col[2]*b.col[1];
    c.col[1] = a.col[2]*b.col[0] - a.col[0]*b.col[2];
    c.col[2] = a.col[0]*b.col[1] - a.col[1]*b.col[0];
    return (c);
}

//! Dot product of two row vectors.
/*! Take the vector dot product of two  vectors in ::rvector form.
        \param a First vector in product
        \param b Second vector in product, treated as column vector
        \return a dot b
*/
double dot_rv(rvector a, rvector b)
{
    double d;
    d = a.col[0]*b.col[0] + a.col[1]*b.col[1] + a.col[2]*b.col[2];
    return (d);
}
double dot_rv2(rvector a, rvector b)
{
    return a.col[0]*b.col[0] + a.col[1]*b.col[1] + a.col[2]*b.col[2];
}

//! Angular separation between vectors.
/*! Calculates the separation angle between two vectors, in radians.
        \param v1 the first vector, in ::cvector format
        \param v2 the first vector, in ::cvector format
        \return the separation angle in radians as a double precision
*/
double sep_cv(cvector v1, cvector v2)
{
    double length, sepangle;

    length = sqrt(v1.x*v1.x+v1.y*v1.y+v1.z*v1.z); /*!< Normalize first vector */
    v1.x /= length;
    v1.y /= length;
    v1.z /= length;
    length = sqrt(v2.x*v2.x+v2.y*v2.y+v2.z*v2.z);
    v2.x /= length;
    v2.y /= length;
    v2.z /= length;

    sepangle = ((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z))/4.;
    sepangle = 2. * atan2(sqrt(sepangle),sqrt(fmax(0.,1.-sepangle)));
    return (sepangle);
}

//! Zero cartesian vector
/*! Creates a zero length cartesian vector.
        \return a ::cvector of zero length
*/
cvector cv_zero()
{
    cvector v={0.,0.,0.};
    return (v);
}

//! Unit x vector
/*! Creates a cartesian vector with the X value set to one.
        \return the vector
*/
cvector cv_unitx()
{
    cvector v={1.,0.,0.};
    return (v);
}

//! Unit y vector
/*! Creates a cartesian vector with the Y value set to one.
        \return the vector
*/
cvector cv_unity()
{
    cvector v={0.,1.,0.};
    return (v);
}

//! Unit z vector
/*! Creates a cartesian vector with the Z value set to one.
        \return the vector
*/
cvector cv_unitz()
{
    cvector v={0.,0.,1.};
    return (v);
}

//! Vector of ones
/*! Creates a cartesian vector with all values set to one.
        \return a ::cvector of unit length, each dimension 1
*/
cvector cv_one()
{
    cvector v={1.,1.,1.};
    return (v);
}

//! Normalize cartesian vector
/*! Returns a normalized version of the requested cartesian vector.
        \param v the ::cvector to be normalized
        \return the normalized version of the vector as ::cvector
*/ // TODO: check! normal is a term used for perpendicular, change to cv_normalize
cvector cv_normal(cvector v)
{
    normalize_cv(&v);
    return (v);
}

//! Normalize cartesian vector in place, i.e. divides it by its own norm.
/*! Normalizes requested cartesian vector.
        \param v a pointer to the ::cvector to be normalized
*/
// TODO: replace normalize_cv2 to normalize_cv with &
void normalize_cv(cvector *v)
{
    double mag;

    mag = v->x*v->x + v->y*v->y + v->z*v->z;
    if (fabs(mag - (double)0.) > D_SMALL && fabs(mag - (double)1.) > D_SMALL)
    {
        mag = sqrt(mag);
        v->x /= mag;
        v->y /= mag;
        v->z /= mag;
    }
}

//! Normalize cartesian vector in place, i.e. divides it by its own norm.
/*! Normalizes requested cartesian vector.
        \param v a pointer to the ::cvector to be normalized
*/
// TODO: replace normalize_cv2 to normalize_cv with &
void normalize_cv(cvector &v)
{
    double mag;

    mag = v.x*v.x + v.y*v.y + v.z*v.z;
    if (fabs(mag - (double)0.) > D_SMALL && fabs(mag - (double)1.) > D_SMALL)
    {
        mag = sqrt(mag);
        v.x /= mag;
        v.y /= mag;
        v.z /= mag;
    }
}

//! Normalize cartesian vector in place, i.e. divides it by its own norm.
void cvector::normalize()
{
    double norm = this->norm();

    if (fabs(norm - (double)0.) > D_SMALL && fabs(norm - (double)1.) > D_SMALL)
    {
        this->x /= norm;
        this->y /= norm;
        this->z /= norm;
    }
}



//! Multiply vector by scalar
/*! Multiply a 3 element vector by a double precision scalar.
        \param b vector to be tranformed, in ::cvector form
        \param a double precision scalar to multiply by
        \return the transformed vector, in ::cvector form
*/
cvector cv_smult(double a, cvector b)
{
    cvector c;

    c.x = a * b.x;
    c.y = a * b.y;
    c.z = a * b.z;
    return (c);
}

//! Add scalar to each element of vector.
/*! Add a double precision scalar to each element of a 3 element
 * vector.
        \param b vector to be tranformed, in ::cvector form
        \param a double precision scalar to add
        \return the transformed vector, in ::cvector form
*/
cvector cv_sadd(double a, cvector b)
{
    cvector c;

    c.x = a + b.x;
    c.y = a + b.y;
    c.z = a + b.z;
    return (c);
}

//! Add two vectors
/*! Add two vectors in ::cvector form, returning a ::cvector.
        \param a first vector to be added, in ::cvector form
        \param b second vector to be added, in ::cvector form
        \result the transformed vector, in ::cvector form
*/
cvector cv_add(cvector a, cvector b)
{
    cvector c;

    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    return (c);
}

//! Subtract two vectors
/*! Subtract two vectors in ::cvector form, returning a ::cvector.
        \param a vector to be subtracted from, in ::cvector form
        \param b vector to be subtracted, in ::cvector form
        \result the transformed vector, in ::cvector form
*/
cvector cv_sub(cvector a, cvector b)
{
    cvector c;

    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    return (c);
}

//! Divide two vectors
/*! Divide one ::cvector by another, returning a ::cvector.
        \param a vector to be divided by, in ::cvector form
        \param b vector to divide by, in ::cvector form
        \result the transformed vector, in ::cvector form
*/
cvector cv_div(cvector a, cvector b)
{
    cvector c;

    c.x = a.x / b.x;
    c.y = a.y / b.y;
    c.z = a.z / b.z;
    return (c);
}

//! Multiply two vectors
/*! Multiply two vectors in ::cvector form, returning a ::cvector.
        \param a first vector to be multiplied, in ::cvector form
        \param b second vector to be multiplied, in ::cvector form
        \result the transformed vector, in ::cvector form
*/
cvector cv_mult(cvector a, cvector b)
{
    cvector c;

    c.x = a.x * b.x;
    c.y = a.y * b.y;
    c.z = a.z * b.z;
    return (c);
}

//! Take cross product of two vectors.
cvector cv_cross(cvector a, cvector b)
{
    cvector c;

    c.x = a.y*b.z - a.z*b.y;
    c.y = a.z*b.x - a.x*b.z;
    c.z = a.x*b.y - a.y*b.x;
    return (c);
}

double dot_cv(cvector a, cvector b)
{
    double d;

    d = a.x*b.x + a.y*b.y + a.z*b.z;
    return (d);
}


// TODO: delete and just keep cv_norm
// There are 4 options now
// - length_cv
// - cv_norm
// - norm_cv
// - cvector::norm()
double length_cv(cvector v)
{
    return cv_norm(v);
}


double cv_norm(cvector v)
{
    double length;

    length = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
    if (length < D_SMALL)
        return (0.);
    else
        return (length);
}

double cvector::norm()
{
    double norm = sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
    if (norm < D_SMALL)
        return (0.);
    else
        return (norm);
}


// TODO: what norm is this?
// TODO: replace by cv_norm?
double norm_cv(cvector v)
{
    double norm;

    norm = fmax(fabs(v.x),fmax(fabs(v.y),fabs(v.z)));

    return (norm);
}

double sum_cv(cvector vec)
{
    double sum;

    sum = vec.x + vec.y + vec.z;

    return (sum);
}

cvector cv_sqrt(cvector a)
{
    cvector s;

    s.x = sqrt(a.x);
    s.y = sqrt(a.y);
    s.z = sqrt(a.z);

    return (s);
}


//! Boolean equate of row vetor.
/*! Determine whether all the elements of an ::rvector are equal and return either true or false.
        \param v1 First ::rvector.
        \param v2 Second ::rvector.
        \return Boolean true or false.
*/
bool equal_rv(rvector v1, rvector v2)
{
    if (v1.col[0] == v2.col[0] && v1.col[1] == v2.col[1] && v1.col[2] == v2.col[2])
        return (true);
    else
        return (false);
}

bool equal_rv2(rvector v1, rvector v2)
{
    return (v1.col[0] == v2.col[0] && v1.col[1] == v2.col[1] && v1.col[2] == v2.col[2]);

}
//! Length of row vector
/*! Calculate the length of a vector in row vector format.
        \param v Vector to find the length of.
        \return Length of row vector.
*/
double length_rv(rvector v)
{
    double length;

    length = (v.col[0]*v.col[0]+v.col[1]*v.col[1]+v.col[2]*v.col[2]);
    length = sqrt(length);

    if (length < D_SMALL)
        return (0.);
    else
        return (length);
}

// TODO: merge to norm_rv
double length_rv2(rvector v)
{
    return sqrt(v.col[0]*v.col[0]+v.col[1]*v.col[1]+v.col[2]*v.col[2]);
}

//! Infinite norm of row vector
/*! Find the largest value in a row vector.
        \param vec Row vector to take infinte norm of
        \return Norm
*/
double norm_rv(rvector vec)
{
    double norm;
    norm = fmax(fabs(vec.col[0]),fmax(fabs(vec.col[1]),fabs(vec.col[2])));
    return (norm);
}

double norm_rv2(rvector vec)
{
    return fmax(fabs(vec.col[0]),fmax(fabs(vec.col[1]),fabs(vec.col[2])));
}

//! Sum elements of a row vector
/*! Add up the elements of a row vector and return the sum.
        \param vec Row vector to take sum of.
        \return Sum of elements
*/
double sum_rv(rvector vec)
{
    double sum;
    sum = vec.col[0] + vec.col[1] + vec.col[2];
    return (sum);
}

double sum_rv2(rvector vec)
{
    return vec.col[0] + vec.col[1] + vec.col[2];
}

//! Row vector square root
/*! ::rvector whose elements are the square roots of the elements of a
 * ::rvector.
        \param a ::rvector to take the square root of.
        \return ::rvector of resultant square roots.
*/
rvector rv_sqrt(rvector vec)
{
    rvector s = {{0.}};

    s.col[0] = sqrt(vec.col[0]);
    s.col[1] = sqrt(vec.col[1]);
    s.col[2] = sqrt(vec.col[2]);

    return (s);
}


std::ostream& operator << (std::ostream& out, const rvector& a)
{

    //    // fancy formating
    //    //out.precision(15);
    //    out << std::fixed;
    //    out << std::setprecision(6);
    //    out<< "["
    //       << std::setw(10) << a.col[0] << ","
    //       << std::setw(10) << a.col[1] << ","
    //       << std::setw(10) << a.col[2];
    //       << "]";

    //    // remove formating for floatfield (not set)
    //    std::cout.unsetf ( std::ios::floatfield );

    // simple formating
    out << a.col[0] << ","
                    << a.col[1] << ","
                    << a.col[2];

    return out;
}

std::ostream& operator << (std::ostream& out, const std::vector<rvector>& a)
{
    for(std::vector<rvector>::const_iterator I = a.begin(); I != a.end(); ++I)
        out<<*I<<"\t";
    return out;
}

std::istream& operator >> (std::istream& in, rvector& a)
{
    char junk;
    in >> junk >> a.col[0] >> junk >> a.col[1] >> junk >> a.col[2] >> junk;
    return in;
}

// multiply vector by scalar operator
rvector operator * (rvector v, double scalar)
{
    //rvector v = *this;
    v.col[0] = scalar * v.col[0];
    v.col[1] = scalar * v.col[1];
    v.col[2] = scalar * v.col[2];

    return v;
}

// divide vector by scalar operator
rvector operator / (rvector v, double scalar)
{
    v.col[0] = v.col[0] / scalar;
    v.col[1] = v.col[1] / scalar;
    v.col[2] = v.col[2] / scalar;

    return v;
}

std::ostream& operator << (std::ostream& out, const cvector& a)
{
    out << "[" << a.x << "," << a.y << "," << a.z << "]";
    return out;
}

std::istream& operator >> (std::istream& in, cvector& a)
{
    char junk;
    in >> junk >> a.x >> junk >> a.y >> junk >> a.z >> junk;
    return in;
}

std::ostream& operator << (std::ostream& out, const svector& a)
{
    out << "[" << a.phi << "," << a.lambda << "," << a.r << "]";
    return out;
}

std::istream& operator >> (std::istream& in, svector& a)
{
    char junk;
    in >> junk >> a.phi >> junk >> a.lambda >> junk >> a.r >> junk;
    return in;
}

std::ostream& operator << (std::ostream& out, const gvector& a)
{
    out<<"["<<a.lat<<","<<a.lon<<","<<a.h<<"]";
    return out;
}

std::istream& operator >> (std::istream& in, gvector& a)
{
    char junk;
    in >> junk >> a.lat >> junk >> a.lon >> junk >> a.h >> junk;
    return in;
}

std::ostream& operator << (std::ostream& out, const avector& a)
{
    out << "[" << a.h << "," << a.e << "," << a.b << "]";
    return out;
}

std::istream& operator >> (std::istream& in, avector& a)
{
    char junk;
    in >> junk >> a.h >> junk >> a.e >> junk >> a.b >> junk;
    return in;
}















namespace Cosmos {
namespace Math {


// TODO: the vector class is very similar to the cvector
// consider merging

// default constructor sets x,y,z members to 0
Vector::Vector()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

Vector::Vector(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

double Vector::at(int i)
{
    switch (i) {
    case 0:
        return this->x;
        break;
    case 1:
        return this->y;
        break;
    case 2:
        return this->z;
        break;
    default:
        return 0;
        break;
    }
}

// convert from column vector to row vector
rvector Vector::from_cv(cvector v){
    rvector rv;
    rv.col[0] = v.x;
    rv.col[1] = v.y;
    rv.col[2] = v.z;
    return rv;
}

//! Compute the cross product of two vectors.
//! where a = *this
Vector Vector::cross(Vector b)
{
    Vector c;

    c.x = this->y*b.z - this->z*b.y;
    c.y = this->z*b.x - this->x*b.z;
    c.z = this->x*b.y - this->y*b.x;

    return c;
}

double Vector::dot(Vector b)
{
    return (this->x*b.x + this->y*b.y + this->z*b.z);
}

//! normalize vector in place
void Vector::normalize()
{
    double norm = this->norm();

    if (fabs(norm - (double)0.) > D_SMALL && fabs(norm - (double)1.) > D_SMALL)
    {
        this->x /= norm;
        this->y /= norm;
        this->z /= norm;
    }

}

//! return the norm of the vector
double Vector::norm()
{
    double norm = sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
    if (norm < D_SMALL)
        return (0.);
    else
        return (norm);
}


// multiply vector with scalar operator
// result = Vector * scale
Vector Vector::operator * (double scale)
{
    Vector vo;

    vo.x = scale * this->x;
    vo.y = scale * this->y;
    vo.z = scale * this->z;

    return vo;
}

// multiply scalar with vector operator
// result = scale * Vector
// TODO: is is possible to add this as a member function of Vector?
Vector operator * (double scale, Vector v)
{
    return v*scale;
}


std::ostream& operator << (std::ostream& out, const Vector& v)
{
    out << v.x << ",";
    out << v.y << ",";
    out << v.z;

    return out;
}

} // end namespace Math
} // end namespace Cosmos


//! @}
