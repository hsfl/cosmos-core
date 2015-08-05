#ifndef _MATH_TYPES_H
#define _MATH_TYPES_H

#include <cmath>
#include <iostream>
#include <vector>

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

//! 3 element cartesian vector
/*! 3 double precision numbers representing a vector in a right handed
 * cartesian space */
struct cvector
{
	//! X value
	double x;
	//! Y value
	double y;
	//! Z value
	double z;
} ;

std::ostream& operator << (std::ostream& out, const cvector& a);
std::istream& operator >> (std::istream& in, cvector& a);

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

//! Scalar value type Union
/*! A union of double, float, int32, int16, unit32, uint16 that allows
 * manipulating all.
*/
typedef union
{
	double d;
	float f;
	int32_t i32;
	int16_t i16;
	uint32_t u32;
	uint16_t u16;
} utype;

//! Quaternion/Rvector Union
/*! A union of a ::cvector, ::rvector, ::matrix1d, and a ::quaternion that allows manipulating all.
*/

typedef union
{
	quaternion q;
	qcomplex qc;
	qfirst qf;
	qlast ql;
	rvector r;
	cvector c;
	svector s;
	gvector g;
	matrix1d m1;
	avector a;
	double a4[4];
} uvector;

//! Orthonormal basis
/*! Used to crease frame basis such as inertial, body and sensor frames
 * The DCM class uses this struct as default
*/
struct basisOrthonormal{
    cvector i; // = {1,0,0}
    cvector j; // = {0,1,0}
    cvector k; // = {0,0,1};
};


#endif
