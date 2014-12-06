#ifndef _MATHLIB_H
#define _MATHLIB_H 1

/*! \file mathlib.h
	\brief mathlib include file
*/

//! \ingroup support
//!  \defgroup mathlib Matrix and vector math library
//! COSMOS Math Support Library.
//!
//! This library of mathematical operations and the accompanying set of type
//! definitions provides support for all the mathematical computations
//! necessary within COSMOS. Special emphasis is placed on the calculation and
//! representation of Attitude, which requires vector and matrix operations,
//! and the attendant variable types representing it.
//!
//! The broad areas of support are:
//! - Angular conversion
//! - Byte order conversion
//! - Matrix and Vector storage and operations
//! - Coordinate system attitude conversion and rotation
//! - Fitting and Integration
//!
//! Angular Conversion
//!
//! Constants are defined to represent various multiples of Pi in double
//! precision. Constants are also defined to provide conversion factors for
//! conversion between Arc Seconds and Degrees. Finally, macros have been
//! defined to allow conversion both ways between Radians and Degrees.
//!
//! Byte Order Conversion
//!
//! This set of functions and constants allows the automatic sensing and
//! correction of various architecture byte orders. Constants are provided to
//! represent the 2 possible orders. The function ::local_byte_order() is
//! provided to determine the byte order locally. Finally various functions
//! are provided to swap floating point and interger variables between
//! local and other byte orders.
//!
//! Matrix and Vector Math
//!
//! Support for vectors and matrices up to rank and order 4 has been provided.
//! Various vector types have been defined to support various conventions.
//! Various matrix type are then built on top of these. Finally, a universal
//! vector type is defined to allow quick conversion between types. Matrices
//! are all considered to be Row Major, in that their data is stored
//! internally by row first, and their first index always indicates row.
//! Vectors are also considered to be rows, except that when they are
//! submitted for multiplication by a matrix, they are first considered to
//! have been transposed.
//!
//! Coordinate System Attitude Conversion and Rotation
//!
//! In support of rotations, types have been defined to support quaternions,
//! direction cosine matrices, euler angles, and simple direction and
//! magnitude. In the interest of conciseness, the following conventions have
//! been adopted:
//! - Quaternions represent Left Rotation.
//! - Euler angles represent a Right Handed, Intrinsic, ZYX, Tait-Bryan Rotation.
//! - Direction and Magnitude uses radians.
//!
//! Due to the multiplicity of quaternion representations, a number of naming
//! conventions are defined in parallel in COSMOS. These conventions are:
//! - Vector and Scalar: Vector first, expressed as x, y, z (default)
//! - Complex: Imaginary first, expressed as i, j, k
//! - Array with Scalar First: q1, q2, q3 and scalar q4
//! - Array with Scalar Last: scalar q0 and q1, q2, q3
//!
//! Universal Vector Type
//!
//! A union has been defined that contains all vector and quaternion types.
//! This allows for the quick equating of one to the other for easy switching
//! between conventions. As an example, one can define a Vector/Scalar
//! quaternion, and then use an equation that expects Scalar Last.

#include "configCosmos.h"

#include <math.h>
#include <stdlib.h>
#include <cstring>
#include "cosmos-errno.h"
#include "memlib.h"
#include <iostream>
#include <iomanip>

#include <vector>

using namespace std;

//! \ingroup mathlib
//! \defgroup mathlib_constants Math library constants
//! @{

//! PI
#define PI 3.1415926535897932384626433832795028841971693993751
//! Double precision PI
#define DPI 3.1415926535897932384626433832795028841971693993751
//! Double precision 2*PI
#define D2PI 6.2831853071795864769252867665590057683943387987502
//! Double precision PI/2
#define DPI2 1.5707963267948966192313216916397514420985846996876
//! Multiplicand for Seconds of Time to Radians
#define DS2R 7.2722052166430399038487115353692196393452995355905e-5
//! Multiplicand for Seconds of Arc to Radians
#define DAS2R 4.8481368110953599358991410235794797595635330237270e-6

//! Multiplicand for Degrees to Radians
#define DTOR            (DPI / (double)180.)
//! Multiplicand for Radians to Degrees
#define RTOD            ((double)180. / DPI)
//! Radians of a Degree value
#define RADOF(deg)  (double)(DTOR * (deg))
//! Radians of a Degree value
#define DEG2RAD(deg)  (double)(DTOR * (deg))
//! Degrees of a Radian value
#define DEGOF(rad)  (double)(RTOD * (rad))
//! Degrees of a Radian value
#define RAD2DEG(rad)  (double)(RTOD * (rad))

#define O_UNDEFINED 999999.1
#define O_INFINITE 10000000000000000000000000.9
#define O_SMALL 0.00000001
#define D_SMALL ((double)1e-76)
//! Big Endian byte order
#define ORDER_BIGENDIAN 0
//! PowerPC byte order
#define ORDER_PPC ORDER_BIGENDIAN
//! Little Endian byte order
#define ORDER_LITTLEENDIAN 1
//! Intel byte order
#define ORDER_INTEL ORDER_LITTLEENDIAN
//! Network byte order
#define ORDER_NETWORK ORDER_BIGENDIAN

#define DIRECTION_ROW 0
#define DIRECTION_COLUMN 1

#define ESTIMATOR_SIZE 5
//! @}

//#define true 1
//#define false 0


//! \ingroup mathlib
//! \defgroup mathlib_typedefs Math library typedefs
//! @{

//! 3 element generic row vector
/*! 3 double precision numbers representing a 3 element row major vector.
*/
typedef struct
{
	double col[3];
} rvector;

std::ostream& operator << (std::ostream& out, const rvector& a);
std::ostream& operator << (std::ostream& out, const vector<rvector>& a);
std::istream& operator >> (std::istream& out, rvector& a);


//! 3x3 element generic matrix
/*! 3 rvector elements representing 3 rows of a matrix
*/
typedef struct
{
	rvector row[3];
} rmatrix;

std::ostream& operator << (std::ostream& out, const rmatrix& a);
std::ostream& operator << (std::ostream& out, const vector<rmatrix>& a);
std::istream& operator >> (std::istream& out, rmatrix& a);

//! 3 element cartesian vector
/*! 3 double precision numbers representing a vector in a right handed
 * cartesian space */
typedef struct
{
	//! X value
	double x;
	//! Y value
	double y;
	//! Z value
	double z;
} cvector;

std::ostream& operator << (std::ostream& out, const cvector& a);
std::istream& operator >> (std::istream& in, cvector& a);

//! 3x3 element cartesian matrix
/*! 3 ::cvector elements representing 3 rows of a matrix in a right
 * handed cartesian system */
typedef struct
{
	//! Row 1
	cvector r1;
	//! Row 2
	cvector r2;
	//! Row 3
	cvector r3;
} cmatrix;

std::ostream& operator << (std::ostream& out, const cmatrix& a);
std::istream& operator >> (std::istream& in, cmatrix& a);

//! 3 element spherical vector
/*! 3 double precision numbers representing a vector in a spherical
 * space. Lambda increases east. */
typedef struct
{
	//! N/S in radians
	double phi;
	//! E/W in radians
	double lambda;
	//! Radius in meters
	double r;
} svector;

std::ostream& operator << (std::ostream& out, const svector& a);
std::istream& operator >> (std::istream& out, svector& a);

//! 3 element geodetic vector
/*! 3 double precision numbers representing a vector in a WGS84
 * based geodetic space. Longitude increases east. */
typedef struct
{
	//! Latitude in radians
	double lat;
	//! Longitude in radians
	double lon;
	//! Height in meters
	double h;
} gvector;

std::ostream& operator << (std::ostream& out, const gvector& a);
std::istream& operator >> (std::istream& out, gvector& a);

//! 3 element attitude vector.
/*! Uses Tait-Bryan representation in a  zyx, right handed order of
 * rotation */
typedef struct
{
	//! Heading
	double h;
	//! Elevation
	double e;
	//! Bank
	double b;
} avector;

std::ostream& operator << (std::ostream& out, const avector& a);
std::istream& operator >> (std::istream& out, avector& a);

//! Quaternion, scalar last, using x, y, z.
/*! Can be thought of as ::rvector with scalar last. One can be set equal to other.
 * First 3 elements are the scaled orientation axis. Fourth element is the scaled
 * amount of rotation. Can alternatively be thought of as a ::cvector,
 * followed by a scalar.
*/
typedef struct
{
	//! Orientation
	cvector d;
	//! Rotation
	double w;
} quaternion;

std::ostream& operator << (std::ostream& out, const quaternion& a);
std::istream& operator >> (std::istream& out, quaternion& a);

//! Quaternion, scalar last, using imaginary elements.
/*! Can be thought of as i, j, k elements, followed by scalar.
*/
typedef struct
{
	double i;
	double j;
	double k;
	double r;
} qcomplex;

std::ostream& operator << (std::ostream& out, const qcomplex& a);
std::istream& operator >> (std::istream& out, qcomplex& a);

//! Quaternion, scalar last, using vector elements.
/*! Can be thought of as vector elements, q1, q2, q3, followed by
 * scalar q4.
*/
typedef struct
{
	double q1;
	double q2;
	double q3;
	double q4;
} qlast;

std::ostream& operator << (std::ostream& out, const qlast& a);
std::istream& operator >> (std::istream& out, qlast& a);

//! Quaternion, scalar first using vector elements.
/*! Can be thought of as scalar element, q0, followed by vector
 * elements, q1, q2, q3.
*/
typedef struct
{
	double q1;
	double q2;
	double q3;
	double q0;
} qfirst;

std::ostream& operator << (std::ostream& out, const qfirst& a);
std::istream& operator >> (std::istream& out, qfirst& a);

//! n element row matrix
typedef struct
{
	//! Elements
	double vector[4];
	//! Number of elements
	uint16_t cols;
} matrix1d;

//! nxm element 2D matrix
typedef struct
{
	//! Number of rows
	uint16_t rows;
	//! Number of elements
	uint16_t cols;
	//! Elements
	double array[4][4];
} matrix2d;

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
    cvector i = {1,0,0};
    cvector j = {0,1,0};
    cvector k = {0,0,1};
};

//! Testing Row Vector Class
/*! eventually this is where all row vector stuff would come?
 * do we really need to differentiate between cvector and rvector in the future?
*/
class RowVector{

public:
    // convert from cartesian vector to row vector
    rvector from_cv(cvector v);
};

class DCM {
private:

public:
    cmatrix base2_from_base1(basisOrthonormal base2,basisOrthonormal base1);
    cmatrix base1_from_base2(basisOrthonormal base1, basisOrthonormal base2);

};

//! pxnxm element cube
//typedef double*** matrix3d;

//! Gauss-Jackson Integration Kernel
/*! Contains parameters that can be reused by any instance of a Gauss-Jackson integration of the given order and time step.
*/
typedef struct
{
	int32_t order;
	int32_t horder;
	double dvi;
	double dvi2;
	int32_t **binom;
	double **alpha;
	double **beta;
	double *c;
	double *gam;
	double *q;
	double *lam;
} gj_kernel;

//! Gauss-Jackson Integration Step
/*! Contains the variables specific to a single step of a particular integration of a given order
*/
typedef struct
{
	//! Dependent variable
	double vd0;
	//! Dependent variable 1st derivative
	double vd1;
	//! Dependent variable 2nd derivative
	double vd2;
	double *a;
	double *b;
	//! Current guess of dependent variable
	double s;
	double ss;
	double sa;
	double sb;
} gj_step;

//! Gauss-Jackson Integration Instance
/*! Contains the kernel and all steps necessary for a particular integration, order, time step.
*/
typedef struct
{
	//! Kernel Pointer
	gj_kernel *kern;
	//! Number of axes of integration
	int32_t axes;
	//! Pointer to array of steps; one for each axis, order+2 for the orders
	gj_step **steps;
	//! Pointer to array of independent variables; order+2
	double *vi;
	//! Pointer to a function that will calculate the 2nd derivative given a axes dependent and one independent variable.
	void (*calc_vd2)(double vi, double *vd0, double *vd2, int32_t axes);
} gj_instance;

//! Gauss-Jackson 3D Integration Instance
/*! Contains the kernel and all steps necessary for a particular integration, order, time step.
*/
typedef struct
{
	//! Kernel Pointer
	gj_kernel *kern;
	//! Pointer to array of x steps
	gj_step *stepx;
	//! Pointer to array of y steps
	gj_step *stepy;
	//! Pointer to array of z steps
	gj_step *stepz;
	//! Pointer to array of independent variables
	double *vi;
	//! Pointer to a function that will calculate the 2nd derivative given a dependent and independent variable.
	double (*calc_vd2)(double vi, double vdx0, double vdy0, double vdz0);
} gj_instance3d;

//! Estimator structure
/*! Contains an estimate returned by the estimator. This includes the
 * zeroth, first, and second derivatives of the dependent value, as
 * well as estimated errors for each.
*/
typedef struct
{
	double value[3];
	double error[3];
	vector<double> a;
	vector<double> x;
	vector<double> y;
} estimatorstruc;

//! Estimator handle
/*! Contains storage elements for a string of 2N dependent and
 * independent variables, plus polynomial coefficients for N
 * consecutive fits. This structure can then be used to either return
 * dependent values for an arbitrary independent value, or to update
 * the estimator with new pairs.
*/
typedef struct
{
	vector<estimatorstruc> r;
	int32_t index;
	uint32_t size;
	uint32_t degree;
	double xbase;
	double ybase;
} estimatorhandle;

//! @}

//! \ingroup mathlib
//! \defgroup mathlib_functions Math library functions
//! @{

double gaussian_random(double mean, double stdev);

double norm_rv(rvector a);
double norm_rv2(rvector a);

double norm_rm(rmatrix a);
double norm_rm2(rmatrix a);

double trace_rm(rmatrix a);
double trace_rm2(rmatrix a);
double determinant_rm(rmatrix a);

rmatrix rm_transpose(rmatrix a);
rmatrix rm_square(rmatrix a);
rmatrix rm_sqrt(rmatrix a);
rmatrix rm_eye();
rmatrix rm_zero();
rmatrix rm_mmult(rmatrix a, rmatrix b);
rmatrix rm_mult(rmatrix a, rmatrix b);
rmatrix rm_smult(double a, rmatrix b);
rmatrix rm_add(rmatrix a, rmatrix b);
rmatrix rm_sub(rmatrix a, rmatrix b);
rmatrix rm_change_between_rv(rvector from, rvector to);
rmatrix rm_change_around_x(double angle);
rmatrix rm_change_around_y(double angle);
rmatrix rm_change_around_z(double angle);
rmatrix rm_change_around(int axis, double angle);
rmatrix rm_diag(rvector a);
rmatrix rm_from_cm(cmatrix matrix);
rmatrix rm_quaternion2dcm(quaternion q);
rmatrix rm_from_rv(rvector vector,int direction);
rmatrix rm_skew(rvector row1);
rmatrix rm_inverse(rmatrix m);
rmatrix rm_from_m2(matrix2d m);

void normalize_rv(rvector *v);
void normalize_rv2(rvector &v);

double sep_rv(rvector v1, rvector v2);
double sep_rv2(rvector v1, rvector v2);
double dot_rv(rvector a, rvector b);
double dot_rv2(rvector a, rvector b);
double length_rv(rvector v);
double length_rv2(rvector v);
bool equal_rv(rvector v1, rvector v2);
bool equal_rv2(rvector v1, rvector v2);
double sum_rv(rvector a);
double sum_rv2(rvector a);
double distance_rv(rvector p0, rvector p1, rvector p2);
double distance_rv_1(rvector p0, rvector p1, rvector p2);
double area_rv(rvector p0, rvector p1, rvector p2);
double evaluate_poly(double x, rvector parms);
double evaluate_poly_slope(double x, rvector parms);

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
rvector rv_mmult(rmatrix a, rvector b);
rvector rv_diag(rmatrix a);
rvector rv_unskew(rmatrix matrix);
rvector rv_evaluate_poly(double x, vector< vector<double> > parms);
rvector rv_evaluate_poly_slope(double x, vector< vector<double> > parms);
rvector rv_convert(svector from);

rvector rotate_q(quaternion q,rvector v);
rvector transform_q(quaternion q,rvector v);
rvector rv_quaternion2axis(quaternion q);
uvector rv_fitpoly(uvector x, uvector y, uint32_t order);
vector<double> polyfit(vector<double> &x, vector<double> &y);
void multisolve(vector< vector<double> > x, vector<double> y, vector<double>& a);
void open_estimate(estimatorhandle *estimate, uint32_t size, uint32_t degree);
int16_t set_estimate(estimatorhandle *estimate, double independent, double dependent);
estimatorstruc get_estimate(estimatorhandle *estimate, double independent);

double norm_cm(cmatrix a);
double trace_cm(cmatrix a);

cmatrix cm_transpose(cmatrix a);
cmatrix cm_square(cmatrix a);
cmatrix cm_sqrt(cmatrix a);
cmatrix cm_eye();
cmatrix cm_zero();
cmatrix cm_mmult(cmatrix a, cmatrix b);
cmatrix cm_mult(cmatrix a, cmatrix b);
cmatrix cm_smult(double a, cmatrix b);
cmatrix cm_add(cmatrix a, cmatrix b);
cmatrix cm_sub(cmatrix a, cmatrix b);
cmatrix cm_change_between_cv(cvector from, cvector to);
cmatrix cm_change_around_x(double angle);
cmatrix cm_change_around_y(double angle);
cmatrix cm_change_around_z(double angle);
cmatrix cm_change_around(int axis, double angle);
cmatrix cm_from_rm(rmatrix matrix);
cmatrix cm_diag(cvector a);
cmatrix cm_quaternion2dcm(quaternion q);

void normalize_cv(cvector *v);

double sep_cv(cvector v1, cvector v2);
double dot_cv(cvector a, cvector b);
double length_cv(cvector v);
double norm_cv(cvector a);
double sum_cv(cvector a);

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
cvector cv_mmult(cmatrix a, cvector b);
cvector cv_sqrt(cvector a);
cvector cv_diag(cmatrix a);
cvector rotate_q(quaternion q,cvector v);
//cvector transform_q(quaternion q,cvector v); // doesn't exist in .cpp?
cvector cv_quaternion2axis(quaternion q);


void q_normalize(quaternion *q);

quaternion q_zero();
quaternion q_conjugate(quaternion q);
quaternion q_times(quaternion q1, quaternion q2);
quaternion q_mult(rvector r1, quaternion q2);
quaternion q_mult(quaternion q1, quaternion q2);
quaternion q_smult(double a, quaternion q);
quaternion q_add(quaternion q1, quaternion q2);
quaternion q_sub(quaternion q1, quaternion q2);
quaternion q_euler2quaternion(avector rpw);
quaternion q_dcm2quaternion_cm(cmatrix m);
quaternion q_dcm2quaternion_rm(rmatrix m);
quaternion q_axis2quaternion_cv(cvector v);
quaternion q_axis2quaternion_rv(rvector v);
quaternion q_change_between_cv(cvector from, cvector to);
quaternion q_change_between_rv(rvector from, rvector to);
quaternion q_change_around_cv(cvector around, double angle);
quaternion q_change_around_rv(rvector around, double angle);
quaternion q_change_around_x(double angle);
quaternion q_change_around_y(double angle);
quaternion q_change_around_z(double angle);
quaternion q_rotate_around(int axis, double angle);
quaternion q_transform_for(rvector sourcea, rvector sourceb, rvector targeta, rvector targetb);
quaternion q_eye();
quaternion q_evaluate_poly(double x, vector< vector<double> > parms);
quaternion q_evaluate_poly_slope(double x, vector< vector<double> > parms);
double length_q(quaternion q);

void qrotate(double ipos[3], double rpos[3], double angle, double *opos);

avector a_quaternion2euler(quaternion q);

svector s_convert(rvector from);

matrix2d m2_inverse(matrix2d m);
double m2_determinant(matrix2d m);
matrix2d m2_zero(uint16_t rows, uint16_t cols);
matrix2d m2_eye(uint16_t rows);
matrix2d m2_cross(matrix2d matrix1, matrix2d matrix2);
matrix2d m2_smult(double number, matrix2d matrix);
matrix2d m2_sub(matrix2d matrix1, matrix2d matrix2);
matrix2d m2_add(matrix2d matrix1, matrix2d matrix2);
matrix2d m2_transpose(matrix2d matrix);
matrix1d m2_unskew(matrix2d matrix);
double m2_trace(matrix2d matrix);
matrix2d m2_mmult(matrix2d matrix1, matrix2d matrix2);
matrix1d m2_eig2x2(matrix2d matrix);
double m2_snorm2x2(matrix2d matrix);
matrix2d cv_to_m2(cvector vector,int direction);
matrix1d cv_to_m1(cvector vector);
matrix2d cm3x3_to_m2(cmatrix matrix);
matrix2d m1_to_m2(matrix1d vector,int direction);
matrix2d m2_from_rm(rmatrix matrix);

matrix1d m1_zero(uint16_t cols);
matrix1d m1_smult(double number, matrix1d row);
matrix1d m1_mmult(matrix2d matrix, matrix1d row);
matrix1d m1_sub(matrix1d row1, matrix1d row2);
matrix1d m1_add(matrix1d row1, matrix1d row2);
matrix1d m1_cross(matrix1d row1, matrix1d row2);
matrix2d m2_skew(matrix1d row1);
matrix2d m2_diag(matrix1d row1);
double m1_norm(matrix1d matrix);
double m1_dot(matrix1d a, matrix1d b);

uint8_t local_byte_order();
uint16_t uint16from(uint8_t *pointer, uint8_t order);
int16_t int16from(uint8_t *pointer, uint8_t order);
uint32_t uint32from(uint8_t *pointer, uint8_t order);
int32_t int32from(uint8_t *pointer, uint8_t order);
float floatfrom(uint8_t *pointer, uint8_t order);
double doublefrom(uint8_t *pointer, uint8_t order);
void uint32to(uint32_t value, uint8_t *pointer, uint8_t order);
void int32to(int32_t value, uint8_t *pointer, uint8_t order);
void uint16to(uint16_t value, uint8_t *pointer, uint8_t order);
void int16to(int16_t value, uint8_t *pointer, uint8_t order);
void floatto(float value, uint8_t *pointer, uint8_t order);

// Gauss-Jackson routines
gj_kernel *gauss_jackson_kernel(int32_t order, double dvi);
void gauss_jackson_dekernel(gj_kernel *gjk);
gj_instance *gauss_jackson_instance(gj_kernel *gjk,int32_t axes,void (*calc_vd2)(double vi, double *vd0, double *vd2, int32_t axes));
gj_instance3d *gauss_jackson_instance3d(gj_kernel *gjk,double (*calc_vd2)(double vi, double vdx0, double vdy0, double vdz0));
int gauss_jackson_setstep(gj_instance *gji, double vi, double *vd0, double *vd1, double *vd2, int32_t istep);
int gauss_jackson_getstep(gj_instance *gji, double *vi, double *vd0, double *vd1, double *vd2, int32_t istep);
gj_step *gauss_jackson_step(gj_kernel *kern);
void gauss_jackson_destep(gj_kernel *kern, gj_step *step);
void gauss_jackson_preset(gj_instance *gji);
void gauss_jackson_extrapolate(gj_instance *gji, double target);

double actan(double y, double x);
double fixprecision(double number, double precision);
uint16_t calc_crc16ccitt(uint8_t *buf, int size);


class lsfit
{
private:
	//! Least Squares Fit Element
	/*! Contains the dependent (x) and independent (y) values for a single element of a ::fitstruc.
	 * The ::uvector allows both quaternions and rvector to be fit.
	 */
	typedef struct
	{
		// Independent variable
		double x;
		// Double, rvector or quaternion dependent variable
		uvector y;
	} fitelement;

	//! Least Squares Fit Structure
	/*! Contains the data for a second order least squares fit of N elements that are type
	 * ::rvector or ::quaternion.
	 */
	// Number of elements in fit
	uint16_t element_cnt;
	// Number of axes (double, rvector, quaternion)
	uint16_t depth;
	// Order of fit
	uint32_t order;
	// base level subtracted from independent variable before fitting
	double basex;
	// Of size ememnt_cnt
	deque<fitelement> var;

	vector< vector<double> > parms;

	void fit();

public:
	double meanx;
	uvector meany;
	double stdevx;
	uvector stdevy;
	// Minimum reasonable step in dependent variable
	double resolution;

	lsfit();
	lsfit(uint16_t element_cnt);
	lsfit(uint16_t element_cnt, uint16_t ord);
	lsfit(uint16_t element_cnt, uint16_t ord, double res);
	void update(double x, double y);
	void update(double x, rvector y);
	void update(double x, quaternion y);
	void update(fitelement cfit, uint16_t dep);
	double lastx();
	double eval(double x);
	rvector evalrvector(double x);
	quaternion evalquaternion(double x);
	double slope(double x);
	rvector slopervector(double x);
	quaternion slopequaternion(double x);
};

//! @}

#endif
