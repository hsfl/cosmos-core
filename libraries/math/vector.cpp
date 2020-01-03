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
#include "matrix.h"

//! \addtogroup mathlib_functions
//! @{

//! Angular separation between row vectors.
/*! Calculates the separation angle between two row order vectors, in radians.
        \param v1 the first vector, in ::rvector format
        \param v2 the second vector, in ::rvector format
        \return The separation angle in radians as a double precision
*/
double sep_rv(rvector v1, rvector v2)
{
    rvector dv = {{0.}};

    normalize_rv(v1);
    normalize_rv(v2);

    dv = rv_sub(v2,v1);
    double diff = length_rv(dv);

    double sepangle = 2. * atan2(diff/2.,sqrt(fmax(0.,1.-diff*diff/4.)));
    return (sepangle);
}

//double sep_rv2(rvector v1, rvector v2)
//{
//    return acos(dot_rv2(v1,v2)/(length_rv2(v1)*length_rv2(v2)));
//}

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
    normalize_rv(v);
    return (v);
}

//! Normalize row order vector in place.
/*! Normalizes requested row order vector.
        \param v a pointer to the ::rvector to be normalized
*/
void normalize_rv(rvector &v)
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
        \return the transformed vector, in ::rvector form
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
    normalize_cv(v);
    return (v);
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
void cvector::normalize(double scale)
{
    double weight = scale / norm();

    if (fabs(weight - (double)0.) > D_SMALL && fabs(weight - (double)1.) > D_SMALL)
    {
        x *= weight;
        y *= weight;
        z *= weight;
    }
}

//! Normalize cartesian vector
/*! Returns a normalized version of the requested cartesian vector.
        \param scale the weight to be applied after normalizing
        \return the normalized version of the vector as ::cvector
        */
cvector cvector::normalized(double scale)
{
    cvector newv = *this;
    newv.normalize(scale);
    return newv;
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

double cvector::norm2()
{
    double norm = (this->x*this->x + this->y*this->y + this->z*this->z);
    if (norm < D_SMALL)
        return (0.);
    else
        return (norm);
}

double cvector::norm()
{
    double norm = sqrt(this->norm2());
    if (norm < D_SMALL)
        return (0.);
    else
        return (norm);
}

double cvector::length()
{
    return (this->norm());
}

//! Index into cvector
double& cvector::operator[] (const int index)
{
    switch (index)
    {
    case 0:
    default:
        return x;
        break;
    case 1:
        return y;
        break;
    case 2:
        return z;
        break;
    }
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
    return (v1.col[0] == v2.col[0] && v1.col[1] == v2.col[1] && v1.col[2] == v2.col[2]);

}


// TODO: merge to norm_rv
//! Length of row vector
/*! Calculate the length of a vector in row vector format.
        \param v Vector to find the length of.
        \return Length of row vector.
*/
double length_rv(rvector v)
{
    double length;

    length = sqrt(v.col[0]*v.col[0]+v.col[1]*v.col[1]+v.col[2]*v.col[2]);

    if (length < D_SMALL)
        return (0.);
    else
        return (length);
}

//! Infinite norm of row vector
/*! Find the largest value in a row vector.
        \param vec Row vector to take infinte norm of
        \return Norm
*/
double norm_rv(rvector vec)
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
    return vec.col[0] + vec.col[1] + vec.col[2];
}

//! Row vector square root
/*! ::rvector whose elements are the square roots of the elements of a
 * ::rvector.
        \param vec ::rvector to take the square root of.
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


//! @}

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
    return rv_smult(scalar, v);
}

// multiply vector by scalar operator
rvector operator * (double scalar, rvector v)
{
    return rv_smult(scalar, v);
}

// multiply vector by vector operator
rvector operator * (rvector v1, rvector v2)
{
    return rv_mult(v1, v2);
}

// divide vector by scalar operator
rvector operator / (rvector v, double scalar)
{
    return rv_smult(1./scalar, v);
}

// compare vector to vector operator
bool operator == (rvector a, rvector b)
{
    if(a.col[0] == b.col[0] && a.col[1] == b.col[1] && a.col[2] == b.col[2])
    {
        return true;
    }
    else
    {
        return false;
    }
}

// compare vector to vector operator
bool operator != (rvector a, rvector b)
{
    if(a.col[0] != b.col[0] || a.col[1] != b.col[1] || a.col[2] != b.col[2])
    {
        return true;
    }
    else
    {
        return false;
    }
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


// ----------------------------------------------

double norm_q(quaternion q)
{
    return length_q(q);
}

void normalize_q(quaternion *q)
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

//! Angular separation between quaternions.
/*! Calculates the separation angle between two quaternions, in radians.
\param q1 the first quaternion
\param q2 the second quaternion
\return The separation angle in radians as a double precision
*/
double sep_q(quaternion q1, quaternion q2)
{

    normalize_q(&q1);
    normalize_q(&q2);

    double inner = inner_q(q1, q2);
    double sepangle = acos(2. * inner * inner - 1);
    return (sepangle);
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

//! Inner product of two quaternions
/*! Multiply each element of one quaternion by the same element of the other,
* and sum.
* \param q1 First quaternion
* \param q2 second quaternion
* \return Double result
*/
double inner_q(quaternion q1, quaternion q2)
{
    quaternion q = q_times(q1, q2);
    double result = q.d.x + q.d.y + q.d.z + q.w;

    return result;
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
quaternion q_fmult(rvector r1, quaternion q2)
{
    quaternion o;

    o.d.x = r1.col[0] * q2.w + r1.col[1] * q2.d.z - r1.col[2] * q2.d.y;
    o.d.y = r1.col[1] * q2.w + r1.col[2] * q2.d.x - r1.col[0] * q2.d.z;
    o.d.z = r1.col[2] * q2.w + r1.col[0] * q2.d.y - r1.col[1] * q2.d.x;
    o.w = - r1.col[0] * q2.d.x - r1.col[1] * q2.d.y - r1.col[2] * q2.d.z;

    return (o);
}

//! Quaternion multiplication. The result of multiplying two quaternions is the
//! composition of two rotations (not commutative). This function creates a result
//! that would behave like rotating first by q1, then by q2.
/*! Quaternion multiply two quaternions.
* \param q1 First quaternion
* \param q2 Second quaternion
* \return Quaternion result.
*/
quaternion q_fmult(quaternion q1, quaternion q2)
{
    quaternion o;

    o.d.x = q1.w * q2.d.x + q1.d.x * q2.w + q1.d.y * q2.d.z - q1.d.z * q2.d.y;
    o.d.y = q1.w * q2.d.y + q1.d.y * q2.w + q1.d.z * q2.d.x - q1.d.x * q2.d.z;
    o.d.z = q1.w * q2.d.z + q1.d.z * q2.w + q1.d.x * q2.d.y - q1.d.y * q2.d.x;
    o.w = q1.w * q2.w - q1.d.x * q2.d.x - q1.d.y * q2.d.y - q1.d.z * q2.d.z;

    return (o);
}

//! Quaternion multiplication. The result of multiplying two quaternions is the
//! composition of two rotations (not commutative). This function creates a result
//! that would behave like rotating first by q1, then by q2.
/*! Quaternion multiply two quaternions.
* \param q1 First quaternion
* \param q2 Second quaternion
* \return Quaternion result.
*/
quaternion q_mult(quaternion q1, quaternion q2)
{
    return q_fmult(q1, q2);
}

//! Quaternion multiplication. The result of multiplying two quaternions is the
//! composition of two rotations (not commutative). This function creates a result
//! that would behave like rotating first by q2, then by q1. It is the conjugate
//! of ::q_fmult().
/*! Quaternion multiply two quaternions.
* \param q1 First quaternion
* \param q2 Second quaternion
* \return Quaternion result.
*/
quaternion q_rmult(quaternion q1, quaternion q2)
{
    quaternion o;

    o.d.x = q1.w * q2.d.x + q1.d.x * q2.w + q1.d.y * q2.d.z - q1.d.z * q2.d.y;
    o.d.y = q1.w * q2.d.y + q1.d.y * q2.w + q1.d.z * q2.d.x - q1.d.x * q2.d.z;
    o.d.z = q1.w * q2.d.z + q1.d.z * q2.w + q1.d.x * q2.d.y - q1.d.y * q2.d.x;
    o.w = -q1.w * q2.w + q1.d.x * q2.d.x + q1.d.y * q2.d.y + q1.d.z * q2.d.z;

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

    normalize_q(&q);

    return (q);
}

// TODO: validade and add reference
avector a_quaternion2euler(quaternion q)
{
    avector rpw;

    normalize_q(&q);
    /*
rpw.b = atan2(q.d.y*q.d.z+q.w*q.d.x,q.w*q.w+q.d.z*q.d.z-.5);
rpw.e = asin(-2.*(q.d.x*q.d.z-q.w*q.d.y));
rpw.h = atan2(q.d.x*q.d.y+q.w*q.d.z,q.w*q.w+q.d.x*q.d.x-.5);
*/
    //	double sqw = q.w * q.w;
    double sqx = q.d.x * q.d.x;
    double sqy = q.d.y * q.d.y;
    double sqz = q.d.z * q.d.z;
    rpw.b = atan2(2.*(q.d.z*q.d.y + q.d.x*q.w), 1. - 2.*(sqx + sqy));
    rpw.e = asin(-2.*(q.d.x*q.d.z - q.d.y*q.w));
    rpw.h = atan2(2.*(q.d.x*q.d.y + q.d.z*q.w), 1. - 2.*(sqy + sqz));

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

    normalize_q(&q);
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
// TODO: check the redundancy with normalize_q
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
    normalize_q(&rq);
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
    out << a.d.z << ", ";
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



namespace Cosmos {
    namespace Math {
        namespace Vectors {

            //! Lookup value.
            /*! Return value of ::Vector at given index.
         * \param i Index.
         * \param y0 Default y value.
         * \param z0 Default z value.
         * \param w0 Default w value.
                \return Value at index, or NAN.
        */
            double Vector::at(int i)
            {
                switch (i) {
                case 0:
                    return x;
                    break;
                case 1:
                    return y;
                    break;
                case 2:
                    return z;
                    break;
                case 3:
                    return w;
                    break;
                default:
                    return ::nan("");
                    break;
                }
            }

            //! Convert from ::cvector.
            /*! Convert the provided ::cvector to ::Vector.
         * \param ::cvector to convert.
         * \return ::Vector representation.
        */
//            Vector Vector::from_cv(cvector cv)
//            {
//                Vector v;
//                v.x = cv.x;
//                v.y = cv.y;
//                v.z = cv.z;
//                v.w = 0.;
//                return v;
//            }

            //! Convert from ::rvector.
            /*! Convert the provided ::rvector to ::Vector.
         * \param ::rvector to convert.
         * \return ::Vector representation.
        */
//            Vector Vector::from_rv(rvector rv)
//            {
//                Vector v;
//                v.x = rv.col[0];
//                v.y = rv.col[1];
//                v.z = rv.col[2];
//                v.w = 0.;
//                return v;
//            }

            //! Convert to ::rvector
            /*! Convert the current ::Vector to ::rvector format.
         * \return ::rvector representation.
         */
            rvector Vector::to_rv()
            {
                rvector rv;
                rv.col[0] = x;
                rv.col[1] = y;
                rv.col[2] = z;

                return rv;
            }

            //! Convert to ::cvector
            /*! Convert the current ::Vector to ::cvector format.
         * \return ::cvector representation.
         */
            cvector Vector::to_cv()
            {
                cvector cv;
                cv.x = x;
                cv.y = y;
                cv.z = z;

                return cv;
            }

            //! Cross product.
            /*! Calculate the cross product with the provided vector.
         * \param ::Vector to cross.
         * \return This cross b.
        */
            Vector Vector::cross(Vector b)
            {
                Vector c;

                c.x = y*b.z - z*b.y;
                c.y = z*b.x - x*b.z;
                c.z = x*b.y - y*b.x;
                c.w = 0.;

                return c;
            }

            //! Dot product.
            /*! Calculate the dot product with the provided vector.
         * \param ::Vector to dot.
         * \return This dot b.
        */
            double Vector::dot(Vector b)
            {
                return (x*b.x + y*b.y + z*b.z);
            }

            //! Normalize.
            /*! Normalize the vector in place.
         * \return Reference to normalized ::Vector.
        */
            Vector &Vector::normalize(double size)
            {
                double tnorm = norm();

                // If norm is basically 1., don't do anything
                if (fabs(tnorm - (double)1.) > D_SMALL)
                {
                    // If norm is basically 0., flag an error
                    if (fabs(tnorm - (double)0.) > D_SMALL)
                    {
                        // Otherwise, normalize
                        *this = *this / tnorm;
                    }
                    else
                    {
                        this->x = NAN;
                        this->y = NAN;
                        this->z = NAN;
                        this->w = NAN;
                    }
                }
                *this *= size;
                return *this;
            }

            //! Norm.
            /*! Calculate the Norm.
         * \return Norm of ::Vector.
        */
            double Vector::norm()
            {
                double norm = sqrt(x*x + y*y + z*z + w*w);
                if (norm < D_SMALL)
                    return (0.);
                else
                    return (norm);
            }


            //! Scalar product.
            /*! Calculate the scalar product with the provided scale.
         * \param scale Scale to multiply by.
         * \return This times b.
        */
            Vector Vector::operator *(const double scale) const
            {
                Vector vo = *this;

                vo *= scale;

                return vo;
            }

            //! Compound Scalar product.
            /*! Calculate the scalar product with the provided scale inline.
         * \param scale Scale to multiply by.
         * \return Reference to this times scale.
        */
            Vector &Vector::operator *= (const double &scale)
            {
                // If scale is basically 1., don't do anything
                if (fabs(scale - (double)1.) > D_SMALL)
                {
                    // If scale is basically 0., set to zero
                    if (fabs(scale - (double)0.) > D_SMALL)
                    {
                        // Otherwise, multiply
                        this->x *= scale;
                        this->y *= scale;
                        this->z *= scale;
                        this->w *= scale;
                    }
                    else
                    {
                        this->x = 0.;
                        this->y = 0.;
                        this->z = 0.;
                        this->w = 0.;
                    }
                }
                return *this;
            }

            //! Reverse scalar product.
            /*! Calculate the scalar product with the provided scale.
         * \param scale Scale to multiply by.
         * \return Scale times this.
        */
            Vector operator * (const double scale, const Vector &v)
            {
                return v * scale;
            }

            //! Scalar division.
            /*! Calculate the scalar division with the provided scale.
         * \param scale Scale to divide by.
         * \return This divided by b.
        */
            Vector Vector::operator / (double scale) const
            {
                Vector vo = *this;

                vo /= scale;

                return vo;
            }

            //! Compound scalar division.
            /*! Calculate the scalar division with the provided scale inline.
         * \param scale Scale to divide by.
         * \return Reference to this divided by b.
        */
            Vector &Vector::operator /= (const double &scale)
            {
                // If scale is basically 1., don't do anything
                if (fabs(scale - (double)1.) > D_SMALL)
                {
                    // If scale is basically 0., flag an error
                    if (fabs(scale - (double)0.) > D_SMALL)
                    {
                        // Otherwise, divide
                        this->x /= scale;
                        this->y /= scale;
                        this->z /= scale;
                        this->w /= scale;
                    }
                    else
                    {
                        this->x = INFINITY;
                        this->y = INFINITY;
                        this->z = INFINITY;
                        this->w = INFINITY;
                    }
                }
                return *this;
            }

            //! Index.
            /*! Lookup ::Vector value by index.
         * \param index Indexed location to look up.
         * \return Value at indexed location.
        */
            double &Vector::operator[] (const int &index)
            {
                switch (index)
                {
                case 0:
                default:
                    return x;
                    break;
                case 1:
                    return y;
                    break;
                case 2:
                    return z;
                    break;
                case 3:
                    return w;
                    break;
                }
            }

            //! Multiply two ::Vector
            /*! Multiply two vectors in ::Vector form, returning a ::Vector.
                \param a first vector to be multiplied, in ::Vector form
                \param b second vector to be multiplied, in ::Vector form
                \result the transformed vector, in ::Vector form
        */
            Vector Vector::operator * (const Vector &b) const
            {
                Vector c;

                c.x = this->x * b.x;
                c.y = this->y * b.y;
                c.z = this->z * b.z;
                c.w = this->w * b.z;
                return (c);
            }

            //! Add two ::Vector
            /*! Add two vectors in ::Vector form, returning a ::Vector.
                \param a first vector to be added, in ::Vector form
                \param b second vector to be added, in ::Vector form
                \result the transformed vector, in ::Vector form
        */
            Vector Vector::operator + (const Vector &b) const
            {
                Vector c;

                c.x = this->x + b.x;
                c.y = this->y + b.y;
                c.z = this->z + b.z;
                c.w = this->w + b.w;
                return (c);
            }

            //! compound add two ::Vector
            /*! Add two vectors in ::Vector form, returning a ::Vector.
                \param a first vector to be added, in ::Vector form
                \param b second vector to be added, in ::Vector form
                \result the transformed vector, in ::Vector form
        */
            Vector& Vector::operator += (const Vector &b)
            {
                *this = *this + b;
                return *this;
            }

            //! Subtract two ::Vector
            /*! Subtract two vectors in ::Vector form, returning a ::Vector.
                \param b subtrahend vector, in ::Vector form
                \result the transformed vector, in ::Vector form
        */
            Vector Vector::operator - (const Vector &b) const
            {
                Vector c;

                c.x = this->x - b.x;
                c.y = this->y - b.y;
                c.z = this->z - b.z;
                c.w = this->w - b.w;
                return (c);
            }

            //! Negate ::Vector
            /*! Return a ::Vector with all elements negated.
                \result the transformed vector, in ::Vector form
        */
            Vector Vector::operator - () const
            {
                Vector c;

                c.x = -x;
                c.y = -y;
                c.z = -z;
                c.w = -w;
                return (c);
            }

            //! compound subtract two ::Vector
            /*! Subtract two vectors in ::Vector form, returning a ::Vector.
                \param a minuend vector, in ::Vector form
                \param b subtrahend vector, in ::Vector form
                \result the transformed vector, in ::Vector form
        */
            Vector &Vector::operator -= (const Vector &b)
            {
                *this = *this - b;
                return *this;
            }

            // compare vector to vector operator
            bool Vector::operator == (const Vector &b) const
            {
                if(x == b.x && y == b.y && z == b.z && w == b.w)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

            // compare vector to vector operator
            bool Vector::operator != (const Vector &b) const
            {
                if(x != b.x || y != b.y || z != b.z || w != b.w)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }

            std::ostream& operator << (std::ostream& out, const Vector& v)
            {
                out << v.x << ",";
                out << v.y << ",";
                out << v.z;

                return out;
            }

            //! Angular separation between two ::Vector.
            /*! Calculates the separation angle between ::Vector, in radians.
                \param v2 the second vector, in ::Vector format
                \return The separation angle in radians as a double precision
        */
            double Vector::separation(Vector v2)
            {
                Vector v1 = *this;
                Vector dv;

                v1.normalize();
                v2.normalize();

                dv = v2 - v1;
                double diff = dv.norm();

                double sepangle = 2. * atan2(diff/2.,sqrt(fmax(0.,1.-diff*diff/4.)));

                return sepangle;
            }

            //! Area subtended by two ::Vector.
            /*! Calculates the area of the triangle defined by two ::Vector.
                \param v2 the second vector, in ::Vector format.
                \return The area of the triangle.
        */
            double Vector::area(Vector v2)
            {
                double normv1 = this->norm();
                double normv2 = v2.norm();
                double normv3 = (v2 - *this).norm();

                // Calculate area using Heron's formula
                double s = (normv1 + normv2 + normv3) / 2.;
                double area = sqrt(s * (s - normv1) * (s - normv2) * (s - normv3));

                return area;
            }

            Vector Vector::vector()
            {
                Vector o; // output

                o = *this;
                o.w = 0.;

                return o;
            }

            Vector Vector::flattenx()
            {
                Vector v = *this;
                v.x = 0.;
                return v;
            }

            Vector Vector::flatteny()
            {
                Vector v = *this;
                v.y = 0.;
                return v;
            }

            Vector Vector::flattenz()
            {
                Vector v = *this;
                v.z = 0.;
                return v;
            }

            double Vector::sum()
            {
                double r = this->x + this->y + this->z +this->w;
                return r;
            }

            Vector eye(double scale)
            {
                Vector val = Vector(1., 1., 1.);
                val *= scale;
                return val;
            }

            Vector unitxV(double scale)
            {
                Vector val = Vector(1.);
                val *= scale;
                return val;
            }

            Vector unityV(double scale)
            {
                Vector val = Vector(0., 1.);
                val *= scale;
                return val;
            }

            Vector unitzV(double scale)
            {
                Vector val = Vector(0., 0., 1.);
                val *= scale;
                return val;
            }
        }

        namespace Quaternions {
            // same as q_conjugate(quaternion q)
            Quaternion Quaternion::conjugate() const
            {
                Quaternion o; // output

                o.w = w;
                o.x = -x;
                o.y = -y;
                o.z = -z;

                return o;
            }

            Vectors::Vector Quaternion::vector()
            {
                Vector o;
                o.x = x;
                o.y = y;
                o.z = z;

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

            //! Add two ::Quaternion
            /*! Add two quaternions in ::Quaternion form, returning a ::Quaternion.
                \param a first quaternion to be added, in ::Quaternion form
                \param b second quaternion to be added, in ::Quaternion form
                \result the transformed quaternion, in ::Quaternion form
        */
            Quaternion Quaternion::operator + (const Quaternion &b)
            {
                Quaternion c;

                c.x = this->x + b.x;
                c.y = this->y + b.y;
                c.z = this->z + b.z;
                c.w = this->w + b.w;
                return (c);
            }

            //! compound add two ::Quaternion
            /*! Add two quaternions in ::Quaternion form, returning a ::Quaternion.
                \param a first quaternion to be added, in ::Quaternion form
                \param b second quaternion to be added, in ::Quaternion form
                \result the transformed quaternion, in ::Quaternion form
        */
            Quaternion& Quaternion::operator += (const Quaternion &b)
            {
                *this = *this + b;
                return *this;
            }

            //         substraction operator for quaternion class
            Quaternion Quaternion::operator-(const Quaternion& q2) const
            {
                Quaternion q3;

                q3.x = this->x - q2.x;
                q3.y = this->y - q2.y;
                q3.z = this->z - q2.z;
                q3.w = this->w - q2.w;

                return q3;
            }

            //! compound subtract two ::Quaternion
            /*! Subtract two quaternions in ::Quaternion form, returning a ::Quaternion.
                \param a minuend quaternion, in ::Quaternion form
                \param b subtrahend quaternion, in ::Quaternion form
                \result the transformed quaternion, in ::Quaternion form
        */
            Quaternion &Quaternion::operator -= (const Quaternion &b)
            {
                *this = *this - b;
                return *this;
            }

            //! Negate ::Quaternion
            /*! Return a ::Quaternion with all elements negated.
                \result the transformed quaternion, in ::Quaternion form
        */
            Quaternion Quaternion::operator - () const
            {
                Quaternion c;

                c.x = -x;
                c.y = -y;
                c.z = -z;
                c.w = -w;
                return (c);
            }

            //! Scalar product.
            /*! Calculate the scalar product with the provided scale.
         * \param scale Scale to multiply by.
         * \return This times b.
        */
            Quaternion Quaternion::operator * (const double scale) const
            {
                Quaternion vo = *this;

                vo *= scale;

                return vo;
            }

            //! Compound Scalar product.
            /*! Calculate the scalar product with the provided scale inline.
         * \param scale Scale to multiply by.
         * \return Reference to this times scale.
        */
            Quaternion &Quaternion::operator *= (const double scale)
            {
                // If scale is basically 1., don't do anything
                if (fabs(scale - (double)1.) > D_SMALL)
                {
                    // If scale is basically 0., set to zero
                    if (fabs(scale - (double)0.) > D_SMALL)
                    {
                        // Otherwise, multiply
                        this->x *= scale;
                        this->y *= scale;
                        this->z *= scale;
                        this->w *= scale;
                    }
                    else
                    {
                        this->x = 0.;
                        this->y = 0.;
                        this->z = 0.;
                        this->w = 0.;
                    }
                }
                return *this;
            }

            // product operator for quaternion class
            Quaternion Quaternion::operator * (Quaternion &q2) const
            {
                Quaternion q1, q3;

                q1 = *this;

                q3.x = q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y;
                q3.y = q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z;
                q3.z = q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x;
                q3.w = q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z;

                return q3;
            }

            Quaternion operator * (const Vectors::Vector &v, Quaternion &q)
            {
                const Quaternion qv = Quaternion(v);
                return qv * q;
            }

            //! Scalar division.
            /*! Calculate the scalar division with the provided scale.
         * \param scale Scale to divide by.
         * \return This divided by b.
        */
//            Quaternion Quaternion::operator / (double scale)
//            {
//                Quaternion vo = *this;

//                vo /= scale;

//                return vo;
//            }

            //! Compound scalar division.
            /*! Calculate the scalar division with the provided scale inline.
         * \param scale Scale to divide by.
         * \return Reference to this divided by b.
        */
//            Quaternion &Quaternion::operator /= (const double &scale)
//            {
//                // If scale is basically 1., don't do anything
//                if (fabs(scale - (double)1.) > D_SMALL)
//                {
//                    // If scale is basically 0., flag an error
//                    if (fabs(scale - (double)0.) > D_SMALL)
//                    {
//                        // Otherwise, divide
//                        this->x /= scale;
//                        this->y /= scale;
//                        this->z /= scale;
//                        this->w = 0.;
//                    }
//                    else
//                    {
//                        this->x = INFINITY;
//                        this->y = INFINITY;
//                        this->z = INFINITY;
//                        this->w = 0.;
//                    }
//                }
//                return *this;
//            }


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
            Vectors::Vector Quaternion::omegaFromDerivative(Quaternion dq)
            {
                Vectors::Vector o; // output
                Quaternions::Quaternion q = this->conjugate();
                q = (dq * q);
                q = 2. * q;
                o = q.vector();


                return o;
            }

            //! Normalize.
            /*! Normalize the quaternion in place.
         * \return Reference to normalized ::Quaternion.
        */
//            Quaternion &Quaternion::normalize()
//            {
//                double tnorm = norm();

//                // If norm is basically 1., don't do anything
//                if (fabs(tnorm - (double)1.) > D_SMALL)
//                {
//                    // If norm is basically 0., flag an error
//                    if (fabs(tnorm - (double)0.) > D_SMALL)
//                    {
//                        // Otherwise, normalize
//                        *this = *this / tnorm;
//                    }
//                    else
//                    {
//                        this->x = INFINITY;
//                        this->y = INFINITY;
//                        this->z = INFINITY;
//                        this->w = INFINITY;
//                    }
//                }
//                return *this;
//            }

            // ref: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
            Vectors::Vector Quaternion::toEuler()
            {

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

            //! Reverse scalar product.
            /*! Calculate the scalar product with the provided scale.
         * \param scale Scale to multiply by.
         * \return Scale times this.
        */
            Quaternion operator * (double scale, Quaternion &q)
            {
                return q * scale;
            }

            //! Indirectly rotate a ::Vector using a ::Quaternion
            /*! Indirectly rotate a ::Vector from one coordinate system to another using the
         * provided ::Quaternion.
                \param v ::Vector to be rotated
                \return ::Vector in the intrinsically rotated system
        */
            Vectors::Vector Quaternion::irotate(const Vector &v)
            {
                Quaternion tq = v * (*this);
                const Quaternion qc = this->conjugate();
                Vector result = qc * tq;

                return result;
            }

            Vectors::Vector Quaternion::drotate(const Vector &v)
            {

                const Quaternion qc = this->conjugate();
                Quaternion tq (v * qc);
                Vector result =  (*this) * tq;

                return result;
            }

            //! compute the quaternion that represents the rotation from vector a to vector b
            //! Ref: - http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
            Quaternion drotate_between(Vectors::Vector a, Vectors::Vector b)
            {
                // normalize the vectors in place
                a.normalize();
                b.normalize();

                Vectors::Vector vec1;
                Vectors::Vector vec2;
                Quaternion rq;
                if ((a + b).norm() < 1e-14)
                {
                    vec1 = Vector(rand(), rand(), rand());
                    vec1.normalize();
                    vec2 = vec1.cross(b);
                    vec2.normalize();
                    if (vec2.norm() < D_SMALL)
                    {
                        vec1 = Vector(rand(), rand(), rand());
                        vec1.normalize();
                        vec2 = vec1.cross(b);
                        vec2.normalize();
                    }
                    rq = Quaternion(vec2);
                    rq.w = 0.;
                }
                else
                {
                    rq = Quaternion(a.cross(b));
                    rq.w = 1. + a.dot(b);
                }

                rq.normalize();

                return rq;
            }

            //! Create irotate quaternion from two orthogonal vectors
            /*! Using two vectors, represented in both the original and target frames,
         * calculate the quaternion that will irotate any vector from the original
         * to the target frame.
         * \param sourcea First vector in source frame
         * \param sourceb Second vector in source frame
         * \param targeta First vector in target frame
         * \param targetb Second vector in target frame
         * \return Quaternion to use with ::irotate to irotate from source to target.
         */
            Quaternion irotate_for(Vectors::Vector sourcea, Vectors::Vector sourceb, Vectors::Vector targeta, Vectors::Vector targetb)
            {
                Quaternion qe_a;
                Quaternion qe_b;
                Quaternion fqe;

                // Determine rotation of source A into target A
                qe_a = (drotate_between(sourcea, targeta)).conjugate();

                // Use to irotate source B into intermediate B
                sourceb = qe_a.irotate(sourceb);
                sourceb.normalize();
                targetb.normalize();
                if ((sourceb + targetb).norm() < 1e-14)
                {
                    // Antiparallel - rotate 180 degrees around vector A
                    qe_b.x = -targeta[0];
                    qe_b.y = -targeta[1];
                    qe_b.z = -targeta[2];
                    qe_b.w = 0;
                }
                else
                {
                    // Determine intrinsic rotation of this intermediate B into target B
                    qe_b = (drotate_between(sourceb, targetb)).conjugate();
                }

                // Combine to determine complete intrinsic rotation of source into target
                fqe = qe_a * qe_b;
                fqe.normalize();

                return fqe;
            }

            Quaternion eye(double scale)
            {
                Quaternion val = Quaternion(0., 0., 0., scale);
                return val;
            }

        } // end namespace Quaternions
    } // end namespace Math
} // end namespace Cosmos

