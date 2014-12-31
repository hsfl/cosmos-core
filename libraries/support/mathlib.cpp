// >> TBDs
// * Group all the quaternion operations in one class, and other logical groups
//   this will make it much easier to use matlib

/*! \file mathlib.cpp
	\brief mathlib source file
*/
#include "mathlib.h"
#include <stdio.h>

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

//! Multiply row order matrix by row order vector
/*! Multiply 3x3 row order matrix by 3 element row order vector (treated as a column order vector).
	\param m matrix to multiply by, in ::rmatrix form
	\param v vector to be tranformed, in ::rvector form
	\return multiplied vector, in ::rvector format
*/
rvector rv_mmult(rmatrix m, rvector v)
{
	rvector o = {{0.}};

	o.col[0] = m.row[0].col[0]*v.col[0] + m.row[0].col[1]*v.col[1] + m.row[0].col[2]*v.col[2];
	o.col[1] = m.row[1].col[0]*v.col[0] + m.row[1].col[1]*v.col[1] + m.row[1].col[2]*v.col[2];
	o.col[2] = m.row[2].col[0]*v.col[0] + m.row[2].col[1]*v.col[1] + m.row[2].col[2]*v.col[2];

	return (o);
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

//! Create rotation quaternion from 2 row vectors
/*! Generate the quaternion that represents a rotation from one row order vector
 * to a second row order vector.
	\param from initial row order vector
	\param to final row order vector
	\return quaternion that can be used to rotate points
*/
quaternion q_change_between_rv(rvector from, rvector to)
{
	uvector rq = {{{0.,0.,0.},0.}};
	rvector vec1 = {{0.}}, vec2 = {{0.}};

	normalize_rv(&from);
	normalize_rv(&to);

	if (length_rv(rv_add(from,to)) < 1e-14)
	{
		vec1.col[0] = rand();
		vec1.col[1] = rand();
		vec1.col[2] = rand();
		normalize_rv(&vec1);
		vec2 = rv_cross(vec1,to);
		normalize_rv(&vec2);
		if (length_rv(vec2)<D_SMALL)
		{
			vec1.col[0] = rand();
			vec1.col[1] = rand();
			vec1.col[2] = rand();
			normalize_rv(&vec1);
			vec2 = rv_cross(vec1,to);
			normalize_rv(&vec2);
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
quaternion q_change_around_rv(rvector around, double angle)
{
	double sa;
	uvector rq;

	angle /= 2.;
	sa = sin(angle);
	normalize_rv(&around);

	rq.r = rv_smult(sa,around);
	rq.q.w = cos(angle);
	q_normalize(&rq.q);
	return (rq.q);
}

//! Create quaternion from two orthogonal vectors
/*! Using two vectors, represented in both the original and target frames,
 * calculate the quaternion that will transform any vector from the original
 * to the target frame.
 * \param sourcea First vector in source frame
 * \param sourceb Second vector in source frame
 * \param targeta First vector in target frame
 * \param targetb Second vector in target frame
 * \return Quaternion to use with ::transform_q to transform from source to target.
 */
quaternion q_transform_for(rvector sourcea, rvector sourceb, rvector targeta, rvector targetb)
{
	quaternion qe_a;
	quaternion qe_b;
	quaternion fqe;

	normalize_rv(&sourcea);
	normalize_rv(&sourceb);

	// Determine rotation of source A into target A
	qe_a = q_conjugate(q_change_between_rv(sourcea,targeta));

	// Use to transform source B into intermediate B
	sourceb = transform_q(qe_a,sourceb);

	// Determine transformation of this intermediate B into target B
	qe_b = q_conjugate(q_change_between_rv(sourceb,targetb));

	// Combine to determine complete transformation of source into target
	fqe = q_mult(qe_a,qe_b);
	q_normalize(&fqe);

	return fqe;
}

//! Quaternion to row vector axis and angle
/*! Convert quaternion to a directional vector in row order form the length of which
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
/*! Convert supplied quaternion to an equivalent direction cosine matrix in row matrix form.
	\param q quaternion
	\return direction cosine matrix in row matrix form
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

	q_normalize(&q);
	return(q);
}

//! Row vector axis and angle to Quaternion.
/*! Convert axis and angle orientation represented as row vector to a Quaternion.
	\param v Row vector axis and angle.
	\return Orientation Quaternion.
*/
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

double distance_rv_1(rvector p0, rvector p1, rvector p2)
{
	rvector p21, p01, p02;
	double d, l;

	p21 = rv_sub(p2,p1);
	p01 = rv_sub(p0,p1);
	p02 = rv_sub(p0,p2);

	if ((l=length_rv(p21)) == 0.)
		return (0.);

	printf("cross product = %g\n", length_rv(rv_cross(p01,p21)));
	printf("P21 length = %g\n", l);

	d = length_rv(rv_cross(p01,p21))/l;

	return(d);

}

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


rmatrix rm_diag(rvector a)
{
	rmatrix b = {{{{0.}}}};;

	b.row[0].col[0] = a.col[0];
	b.row[1].col[1] = a.col[1];
	b.row[2].col[2] = a.col[2];

	b.row[0].col[1] = b.row[0].col[2] = b.row[1].col[0] = b.row[1].col[2] = b.row[2].col[0] = b.row[2].col[1] = 0.;

	return (b);
}

//! Identity rmatrix
/*! ::rmatrix with diagonal elements set to one and all others set to zero
	\return Identity matrix
*/
rmatrix rm_eye()
{
	rmatrix mat = {{{{1.,0.,0.}}, {{0.,1.,0.}}, {{0.,0.,1.}} }};

	return (mat);
}

//! Zero filled rmatrix
/*! ::rmatrix with all elements set to zero
	\return Zero filled matrix
*/
rmatrix rm_zero()
{
	rmatrix mat = {{{{0.,0.,0.}},{{0.,0.,0.}},{{0.,0.,0.}}}};
	return (mat);
}

double norm_rm(rmatrix mat)
{
	double norm;

	norm = norm_rv(mat.row[0]);
	norm = fmax(norm,norm_rv(mat.row[1]));
	norm = fmax(norm,norm_rv(mat.row[2]));

	return (norm);
}

double norm_rm2(rmatrix mat)
{
	return fmax(
				norm_rv2(mat.row[0]),
			fmax(
				norm_rv2(mat.row[1]),
			norm_rv2(mat.row[2])
			)
			);
}

double trace_rm(rmatrix mat)
{
	double trace;
	trace = mat.row[0].col[0] + mat.row[1].col[1] + mat.row[2].col[2];
	return (trace);
}

double trace_rm2(rmatrix mat)
{
	return mat.row[0].col[0] + mat.row[1].col[1] + mat.row[2].col[2];
}

rmatrix rm_transpose(rmatrix a)
{
	rmatrix b = {{{{0.}}}};;

	b.row[0].col[0] = a.row[0].col[0];
	b.row[0].col[1] = a.row[1].col[0];
	b.row[0].col[2] = a.row[2].col[0];

	b.row[1].col[0] = a.row[0].col[1];
	b.row[1].col[1] = a.row[1].col[1];
	b.row[1].col[2] = a.row[2].col[1];

	b.row[2].col[0] = a.row[0].col[2];
	b.row[2].col[1] = a.row[1].col[2];
	b.row[2].col[2] = a.row[2].col[2];

	return (b);
}

//! Matrix diagonal
/*! ::rvector representing the diagonal of a ::rmatrix
	\param a :rmatrix to get diagonal from.
	\return Diagonal
*/
rvector rv_diag(rmatrix a)
{
	rvector b = {{0.}};

	b.col[0] = a.row[0].col[0];
	b.col[1] = a.row[1].col[1];
	b.col[2] = a.row[2].col[2];

	return (b);
}

//! Matrix Product
/*! Multiply two row order matrices together.
	\param a first row order matrix
	\param b second row order matrix
	\return product row order matrix
*/
rmatrix rm_mmult(rmatrix a, rmatrix b)
{
	rmatrix mat = {{{{0.}}}};;

	mat.row[0].col[0]  = a.row[0].col[0]*b.row[0].col[0] + a.row[0].col[1]*b.row[1].col[0] + a.row[0].col[2]*b.row[2].col[0];
	mat.row[0].col[1]  = a.row[0].col[0]*b.row[0].col[1] + a.row[0].col[1]*b.row[1].col[1] + a.row[0].col[2]*b.row[2].col[1];
	mat.row[0].col[2]  = a.row[0].col[0]*b.row[0].col[2] + a.row[0].col[1]*b.row[1].col[2] + a.row[0].col[2]*b.row[2].col[2];

	mat.row[1].col[0]  = a.row[1].col[0]*b.row[0].col[0] + a.row[1].col[1]*b.row[1].col[0] + a.row[1].col[2]*b.row[2].col[0];
	mat.row[1].col[1]  = a.row[1].col[0]*b.row[0].col[1] + a.row[1].col[1]*b.row[1].col[1] + a.row[1].col[2]*b.row[2].col[1];
	mat.row[1].col[2]  = a.row[1].col[0]*b.row[0].col[2] + a.row[1].col[1]*b.row[1].col[2] + a.row[1].col[2]*b.row[2].col[2];

	mat.row[2].col[0]  = a.row[2].col[0]*b.row[0].col[0] + a.row[2].col[1]*b.row[1].col[0] + a.row[2].col[2]*b.row[2].col[0];
	mat.row[2].col[1]  = a.row[2].col[0]*b.row[0].col[1] + a.row[2].col[1]*b.row[1].col[1] + a.row[2].col[2]*b.row[2].col[1];
	mat.row[2].col[2]  = a.row[2].col[0]*b.row[0].col[2] + a.row[2].col[1]*b.row[1].col[2] + a.row[2].col[2]*b.row[2].col[2];

	return (mat);
}

rmatrix rm_mult(rmatrix a, rmatrix b)
{
	rmatrix mat = {{{{0.}}}};
	rvector *va, *vb;

	va = &a.row[0];
	vb = &b.row[0];
	mat.row[0] = rv_mult(*va,*vb);
	va = &a.row[1];
	vb = &b.row[1];
	mat.row[1] = rv_mult(*va,*vb);
	va = &a.row[2];
	vb = &b.row[2];
	mat.row[2] = rv_mult(*va,*vb);

	return (mat);
}

rmatrix rm_smult(double a, rmatrix b)
{
	rmatrix mat = {{{{0.}}}};
	rvector *vb;

	vb = &b.row[0];
	mat.row[0] = rv_smult(a,*vb);
	vb = &b.row[1];
	mat.row[1] = rv_smult(a,*vb);
	vb = &b.row[2];
	mat.row[2] = rv_smult(a,*vb);

	return (mat);
}

rmatrix rm_add(rmatrix a, rmatrix b)
{
	rmatrix mat = {{{{0.}}}};
	rvector *va, *vb;

	va = &a.row[0];
	vb = &b.row[0];
	mat.row[0] = rv_add(*va,*vb);
	va = &a.row[1];
	vb = &b.row[1];
	mat.row[1] = rv_add(*va,*vb);
	va = &a.row[2];
	vb = &b.row[2];
	mat.row[2] = rv_add(*va,*vb);

	return (mat);
}

rmatrix rm_sub(rmatrix a, rmatrix b)
{
	rmatrix mat = {{{{0.}}}};
	rvector *va, *vb;

	va = &a.row[0];
	vb = &b.row[0];
	mat.row[0] = rv_sub(*va,*vb);
	va = &a.row[1];
	vb = &b.row[1];
	mat.row[1] = rv_sub(*va,*vb);
	va = &a.row[2];
	vb = &b.row[2];
	mat.row[2] = rv_sub(*va,*vb);

	return (mat);
}

//! Square rmatrix
/*! Square a rmatrix matrix by matrix multiplying it by itself.
	\param a matrix to be squared
	\return squared matrix
*/
rmatrix rm_square(rmatrix a)
{
	rmatrix b = {{{{0.}}}};
	rvector r1, r2, r3, c1, c2, c3;

	r1 = a.row[0];
	r2 = a.row[1];
	r3 = a.row[2];

	c1.col[0] = a.row[0].col[0];
	c1.col[1] = a.row[1].col[0];
	c1.col[2] = a.row[2].col[0];

	c2.col[0] = a.row[0].col[1];
	c2.col[1] = a.row[1].col[1];
	c2.col[2] = a.row[2].col[1];

	c3.col[0] = a.row[0].col[2];
	c3.col[1] = a.row[1].col[2];
	c3.col[2] = a.row[2].col[2];

	b.row[0].col[0] = sum_rv(rv_mult(r1,c1));
	b.row[0].col[1] = sum_rv(rv_mult(r1,c2));
	b.row[0].col[2] = sum_rv(rv_mult(r1,c3));

	b.row[1].col[0] = sum_rv(rv_mult(r2,c1));
	b.row[1].col[1] = sum_rv(rv_mult(r2,c2));
	b.row[1].col[2] = sum_rv(rv_mult(r2,c3));

	b.row[2].col[0] = sum_rv(rv_mult(r3,c1));
	b.row[2].col[1] = sum_rv(rv_mult(r3,c2));
	b.row[2].col[2] = sum_rv(rv_mult(r3,c3));

	return (b);
}

//! Rotation matrix for X axis
/*! Create the DCM that represents a rotation of the given angle around the X axis.
	\param angle Angle of rotation in radians
	\return Resulting DCM
*/
rmatrix rm_change_around_x(double angle)
{
	return rm_change_around(1,angle);
}

//! Rotation matrix for Y axis
/*! Create the DCM that represents a rotation of the given angle around the Y axis.
	\param angle Angle of rotation in radians
	\return Resulting DCM
*/
rmatrix rm_change_around_y(double angle)
{
	//rmatrix a = {{{{0.}}}};
	//a = rm_change_around(2, angle);
	//return (a);
	
	return rm_change_around(2,angle);
}

//! Rotation matrix for Z axis
/*! Create the DCM that represents a rotation of the given angle around the Z axis.
	\param angle Angle of rotation in radians
	\return Resulting DCM
*/
rmatrix rm_change_around_z(double angle)
{
	//rmatrix a = {{{{0.}}}};
	//a = rm_change_around(3, angle);
	//return (a);

	return rm_change_around(3,angle);
}

//! Rotation matrix for indicated axis
/*! Create the DCM that represents a rotation of the given angle around the indicated axis.
	\param axis Axis of rotation: 1=X, 2=Y, 3=Z
	\param angle Angle of rotation in radians
	\return Resulting DCM
*/
rmatrix rm_change_around(int axis,double angle)
{
	rmatrix a = {{{{1.,0.,0.}},{{0.,1.,0.}},{{0.,0.,1.}}}};

	switch (axis)
	{
	case 1:
		a.row[1].col[1] = a.row[2].col[2] = cos(angle);
		a.row[2].col[1] = sin(angle);
		a.row[1].col[2] = -(a.row[2].col[1]);
		break;
	case 2:
		a.row[0].col[0] = a.row[2].col[2] = cos(angle);
		a.row[0].col[2] = sin(angle);
		a.row[2].col[0] = -(a.row[0].col[2]);
		break;
	case 3:
		a.row[0].col[0] = a.row[1].col[1] = cos(angle);
		a.row[1].col[0] = sin(angle);
		a.row[0].col[1] = -(a.row[1].col[0]);
		break;
	}

	return (a);
}

//! rmatrix from rmatrix
/*! Converts 3x3 matrix in row order form to cartesian form.
	\param matrix Row major matrix to convert
	\return Converted matrix in row order form
*/
cmatrix cm_from_rm(rmatrix matrix)
{
	cmatrix cm;

	cm.r1.x = matrix.row[0].col[0];
	cm.r1.y = matrix.row[0].col[1];
	cm.r1.z = matrix.row[0].col[2];
	cm.r2.x = matrix.row[1].col[0];
	cm.r2.y = matrix.row[1].col[1];
	cm.r2.z = matrix.row[1].col[2];
	cm.r3.x = matrix.row[2].col[0];
	cm.r3.y = matrix.row[2].col[1];
	cm.r3.z = matrix.row[2].col[2];

	return (cm);
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
*/ // >> normal is a term used for perpendicular, change to cv_normalize
cvector cv_normal(cvector v)
{
	normalize_cv(&v);
	return (v);
}

//! Normalize cartesian vector in place.
/*! Normalizes requested cartesian vector.
	\param v a pointer to the ::cvector to be normalized
*/
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

//! Multiply cartesian vector by cartesian matrix
/*! Multiply 3 element cartesian vector by 3x3 cartesian matrix
	\param v vector to be tranformed, in ::cvector form
	\param m matrix to multiply by, in ::cmatrix form
	\return multiplied vector, in ::cvector format
*/
cvector cv_mmult(cmatrix m, cvector v)
{
	cvector o;

	o.x = m.r1.x*v.x + m.r1.y*v.y + m.r1.z*v.z;
	o.y = m.r2.x*v.x + m.r2.y*v.y + m.r2.z*v.z;
	o.z = m.r3.x*v.x + m.r3.y*v.y + m.r3.z*v.z;

	return (o);
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

//! quaternion multiply
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

quaternion q_dcm2quaternion_cm(cmatrix m)
{
	quaternion q;
	double t, tr;

	if ((tr=trace_cm(m)) > 0.)
	{
		t = .5 / sqrt(1.+tr);
		q.w = .25 / t;
		q.d.x = t*(m.r3.y - m.r2.z);
		q.d.y = t*(m.r1.z - m.r3.x);
		q.d.z = t*(m.r2.x - m.r1.y);
	}
	else
	{
		if (m.r1.x > m.r2.y && m.r1.x > m.r3.z)
		{
			t = 2. * sqrt(1. + m.r1.x - m.r2.y - m.r3.z);
			q.w = (m.r3.y - m.r2.z) / t;
			q.d.x = .25 * t;
			q.d.y = (m.r1.y + m.r2.x) / t;
			q.d.z = (m.r1.z + m.r3.x) / t;
		}
		else
		{
			if (m.r2.x > m.r3.z)
			{
				t = 2. * sqrt(1. + m.r2.y - m.r1.x - m.r3.z);
				q.w = (m.r1.z - m.r3.x) / t;
				q.d.x = (m.r1.y + m.r2.x) / t;
				q.d.y = .25 * t;
				q.d.z = (m.r2.z + m.r3.y) / t;
			}
			else
			{
				t = 2. * sqrt(1. + m.r3.z - m.r1.x - m.r2.y);
				q.w = (m.r2.x - m.r1.y) / t;
				q.d.x = (m.r1.z + m.r3.x) / t;
				q.d.y = (m.r2.z + m.r3.y) / t;
				q.d.z = .25 * t;
			}
		}
	}

	q_normalize(&q);
	return(q);
}

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

double length_cv(cvector v)
{
	double length;

	length = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
	if (length < D_SMALL)
		return (0.);
	else
		return (length);
}
// >> what norm is this?
double norm_cv(cvector vec)
{
	double norm;

	norm = fmax(fabs(vec.x),fmax(fabs(vec.y),fabs(vec.z)));

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

cmatrix cm_diag(cvector a)
{
	cmatrix b;

	b.r1.x = a.x;
	b.r2.y = a.y;
	b.r3.z = a.z;

	b.r1.y = b.r1.z = b.r2.x = b.r2.z = b.r3.x = b.r3.y = 0.;

	return (b);
}

cmatrix cm_eye()
{
	cmatrix mat = {{1.,0.,0.},{0.,1.,0.},{0.,0.,1.}};

	return (mat);
}

cmatrix cm_zero()
{
	cmatrix mat = {{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}};
	return (mat);
}

double norm_cm(cmatrix mat)
{
	double norm;

	norm = norm_cv(mat.r1);
	norm = fmax(norm,norm_cv(mat.r2));
	norm = fmax(norm,norm_cv(mat.r3));

	return (norm);
}

double trace_cm(cmatrix mat)
{
	double trace;

	trace = mat.r1.x + mat.r2.y + mat.r3.z;

	return (trace);
}

cmatrix cm_transpose(cmatrix a)
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

cvector cv_diag(cmatrix a)
{
	cvector b;

	b.x = a.r1.x;
	b.y = a.r2.y;
	b.z = a.r3.z;

	return (b);
}

//! Matrix Product
/*! Multiply two cartesian matrices together.
	\param a first cartesian matrix
	\param b second cartesian matrix
	\return product cartesian matrix
*/
cmatrix cm_mmult(cmatrix a, cmatrix b)
{
	cmatrix mat;

	mat.r1.x  = a.r1.x*b.r1.x + a.r1.y*b.r2.x + a.r1.z*b.r3.x;
	mat.r1.y  = a.r1.x*b.r1.y + a.r1.y*b.r2.y + a.r1.z*b.r3.y;
	mat.r1.z  = a.r1.x*b.r1.z + a.r1.y*b.r2.z + a.r1.z*b.r3.z;

	mat.r2.x  = a.r2.x*b.r1.x + a.r2.y*b.r2.x + a.r2.z*b.r3.x;
	mat.r2.y  = a.r2.x*b.r1.y + a.r2.y*b.r2.y + a.r2.z*b.r3.y;
	mat.r2.z  = a.r2.x*b.r1.z + a.r2.y*b.r2.z + a.r2.z*b.r3.z;

	mat.r3.x  = a.r3.x*b.r1.x + a.r3.y*b.r2.x + a.r3.z*b.r3.x;
	mat.r3.y  = a.r3.x*b.r1.y + a.r3.y*b.r2.y + a.r3.z*b.r3.y;
	mat.r3.z  = a.r3.x*b.r1.z + a.r3.y*b.r2.z + a.r3.z*b.r3.z;

	return (mat);
}

cmatrix cm_mult(cmatrix a, cmatrix b)
{
	cmatrix mat;
	cvector *va, *vb;

	va = &a.r1;
	vb = &b.r1;
	mat.r1 = cv_mult(*va,*vb);
	va = &a.r2;
	vb = &b.r2;
	mat.r2 = cv_mult(*va,*vb);
	va = &a.r3;
	vb = &b.r3;
	mat.r3 = cv_mult(*va,*vb);

	return (mat);
}

cmatrix cm_smult(double a, cmatrix b)
{
	cmatrix mat;
	cvector *vb;

	vb = &b.r1;
	mat.r1 = cv_smult(a,*vb);
	vb = &b.r2;
	mat.r2 = cv_smult(a,*vb);
	vb = &b.r3;
	mat.r3 = cv_smult(a,*vb);

	return (mat);
}

cmatrix cm_add(cmatrix a, cmatrix b)
{
	cmatrix mat;
	cvector *va, *vb;

	va = &a.r1;
	vb = &b.r1;
	mat.r1 = cv_add(*va,*vb);
	va = &a.r2;
	vb = &b.r2;
	mat.r2 = cv_add(*va,*vb);
	va = &a.r3;
	vb = &b.r3;
	mat.r3 = cv_add(*va,*vb);

	return (mat);
}

cmatrix cm_sub(cmatrix a, cmatrix b)
{
	cmatrix mat;
	cvector *va, *vb;

	va = &a.r1;
	vb = &b.r1;
	mat.r1 = cv_sub(*va,*vb);
	va = &a.r2;
	vb = &b.r2;
	mat.r2 = cv_sub(*va,*vb);
	va = &a.r3;
	vb = &b.r3;
	mat.r3 = cv_sub(*va,*vb);

	return (mat);
}

//! Square cmatrix
/*! Square a cmatrix matrix by matrix multiplying it by itself.
	\param a matrix to be squared
	\return squared matrix
*/
cmatrix cm_square(cmatrix a)
{
	cmatrix b;
	cvector r1, r2, r3, c1, c2, c3;

	r1 = a.r1;
	r2 = a.r2;
	r3 = a.r3;

	c1.x = a.r1.x;
	c1.y = a.r2.x;
	c1.z = a.r3.x;

	c2.x = a.r1.y;
	c2.y = a.r2.y;
	c2.z = a.r3.y;

	c3.x = a.r1.z;
	c3.y = a.r2.z;
	c3.z = a.r3.z;

	b.r1.x = sum_cv(cv_mult(r1,c1));
	b.r1.y = sum_cv(cv_mult(r1,c2));
	b.r1.z = sum_cv(cv_mult(r1,c3));

	b.r2.x = sum_cv(cv_mult(r2,c1));
	b.r2.y = sum_cv(cv_mult(r2,c2));
	b.r2.z = sum_cv(cv_mult(r2,c3));

	b.r3.x = sum_cv(cv_mult(r3,c1));
	b.r3.y = sum_cv(cv_mult(r3,c2));
	b.r3.z = sum_cv(cv_mult(r3,c3));

	return (b);
}

//! Rotation matrix for X axis
/*! Create the DCM that represents a rotation of the given angle around the X axis.
	\param angle Angle of rotation in radians
	\return Resulting DCM
*/
cmatrix cm_change_around_x(double angle)
{
	cmatrix a;

	a = cm_change_around(1, angle);

	return (a);
}

//! Rotation matrix for Y axis
/*! Create the DCM that represents a rotation of the given angle around the Y axis.
	\param angle Angle of rotation in radians
	\return Resulting DCM
*/
cmatrix cm_change_around_y(double angle)
{
	cmatrix a;

	a = cm_change_around(2, angle);

	return (a);
}

//! Rotation matrix for Z axis
/*! Create the DCM that represents a rotation of the given angle around the Z axis.
	\param angle Angle of rotation in radians
	\return Resulting DCM
*/
cmatrix cm_change_around_z(double angle)
{
	cmatrix a;

	a = cm_change_around(3, angle);

	return (a);
}

//! Rotation matrix for indicated axis
/*! Create the DCM that represents a rotation of the given angle around the indicated axis.
	\param axis Axis of rotation: 1=X, 2=Y, 3=Z
	\param angle Angle of rotation in radians
	\return Resulting DCM
*/
cmatrix cm_change_around(int axis,double angle)
{
	cmatrix a = {{1.,0.,0.},{0.,1.,0.},{0.,0.,1.}};

	switch (axis)
	{
	case 1:
		a.r2.y = a.r3.z = cos(angle);
		a.r3.y = sin(angle);
		a.r2.z = -(a.r3.y);
		break;
	case 2:
		a.r1.x = a.r3.z = cos(angle);
		a.r1.z = sin(angle);
		a.r3.x = -(a.r1.z);
		break;
	case 3:
		a.r1.x = a.r2.y = cos(angle);
		a.r2.x = sin(angle);
		a.r1.y = -(a.r2.x);
		break;
	}

	return (a);
}

//! rmatrix from cmatrix
/*! Converts 3x3 matrix in cartesian form to row major form.
	\param matrix Cartesian matrix to convert
	\return Converted matrix in row major form
*/
rmatrix rm_from_cm(cmatrix matrix)
{
	rmatrix rm = {{{{0.}}}};

	rm.row[0].col[0] = matrix.r1.x;
	rm.row[0].col[1] = matrix.r1.y;
	rm.row[0].col[2] = matrix.r1.z;
	rm.row[1].col[0] = matrix.r2.x;
	rm.row[1].col[1] = matrix.r2.y;
	rm.row[1].col[2] = matrix.r2.z;
	rm.row[2].col[0] = matrix.r3.x;
	rm.row[2].col[1] = matrix.r3.y;
	rm.row[2].col[2] = matrix.r3.z;

	return (rm);
}

//! Row vector to row order matrix
/*! Convert a row vector to a row ordder ::rmatrix
	\param vector Row vector to be converted
	\param direction Alignment, column order if 1 (DIRECTION_COLUMN), otherwise row
	order.
	\return Single row or column ::rmatrix
*/
rmatrix rm_from_rv(rvector vector,int direction)
{
	rmatrix answer = {{{{0.}}}};;
	uint16_t i;

	answer = rm_zero();
	switch (direction)
	{
	case DIRECTION_COLUMN:
		for (i=0; i<3; i++)
		{
			answer.row[i].col[0] = vector.col[i];
		}
		break;
		break;
	default:
		answer.row[0] = vector;
		break;
	}

	return (answer);
}

//! Create skew symmetric ::rmatrix from ::rvector
/*! Generate a new 3x3 skew symmetric row matrix from the provided 3 element
 * vector.
	\param row row vector to be skewed
	\return skew symmetric row matrix
*/
rmatrix rm_skew(rvector row)
{
	rmatrix answer = {{{{0.}}}};;

	answer = rm_zero();

	answer.row[0].col[1] = -row.col[2];
	answer.row[0].col[2] = row.col[1];

	answer.row[1].col[0] = row.col[2];
	answer.row[1].col[2] = -row.col[0];

	answer.row[2].col[0] = -row.col[1];
	answer.row[2].col[1] = row.col[0];

	return (answer);
}

//! Unskew 3x3 row matrix
/*! Create the 3 element ::rvector correponding to a 3x3 skew symmetric ::rmatrix
	\param matrix 3x3 row skew symmetric matrix
	\return ::rvector representing unskewed elements
*/
rvector rv_unskew(rmatrix matrix)
{
	rvector answer={{0.,0.,0.}};

	answer.col[0] = -matrix.row[1].col[2];
	answer.col[1] = matrix.row[0].col[2];
	answer.col[2] = -matrix.row[0].col[1];

	return (answer);
}

//! Inverse of rmatrix
/*! Inverse of 3x3 ::rmatrix using algorithm at
	http://mathworld.wolfram.com/MatrixInverse.html
	\param m ::rmatrix to take inverse of
	\return Inverse matrix, or
*/
rmatrix rm_inverse(rmatrix m)
{
	rmatrix wm = {{{{0.}}}};;

	wm.row[0].col[0] = m.row[1].col[1]*m.row[2].col[2] - m.row[1].col[2]*m.row[2].col[1];
	wm.row[0].col[1] = m.row[0].col[2]*m.row[2].col[1] - m.row[0].col[1]*m.row[2].col[2];
	wm.row[0].col[2] = m.row[0].col[1]*m.row[1].col[2] - m.row[0].col[2]*m.row[1].col[1];

	wm.row[1].col[0] = m.row[1].col[2]*m.row[2].col[0] - m.row[1].col[0]*m.row[2].col[2];
	wm.row[1].col[1] = m.row[0].col[0]*m.row[2].col[2] - m.row[0].col[2]*m.row[2].col[0];
	wm.row[1].col[2] = m.row[0].col[2]*m.row[1].col[0] - m.row[0].col[0]*m.row[1].col[2];

	wm.row[2].col[0] = m.row[1].col[0]*m.row[2].col[1] - m.row[1].col[1]*m.row[2].col[0];
	wm.row[2].col[1] = m.row[0].col[1]*m.row[2].col[0] - m.row[0].col[0]*m.row[2].col[1];
	wm.row[2].col[2] = m.row[0].col[0]*m.row[1].col[1] - m.row[0].col[1]*m.row[1].col[0];

	return( rm_smult((1/determinant_rm(m)), wm) );
}

//! rmatrix from matrix2d
/*! Converts 3x3 matrix in matrix2d form to row major form.
	\param matrix matrix2d matrix to convert
	\return Converted matrix in row major form
*/
rmatrix rm_from_m2(matrix2d matrix)
{
	rmatrix rm = {{{{0.}}}};;
	int i, j;

	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
		{
			rm.row[i].col[j] = matrix.array[i][j];
		}
	}

	return (rm);
}

//!  Determinant of row column matrix
/*! Return the determinant for a 3x3 ::rmatrix
	\param m ;;rmatrix to calculate detrminant of
	\return determinant, otherwise NaN
*/
double determinant_rm(rmatrix m)
{
	double result;

	result = m.row[0].col[0] * (m.row[1].col[1] * m.row[2].col[2] - m.row[2].col[1] * m.row[1].col[2]);
	result -= m.row[1].col[0] * (m.row[0].col[1] * m.row[2].col[2] - m.row[0].col[2] * m.row[2].col[1]);
	result += m.row[2].col[0] * (m.row[0].col[1] * m.row[1].col[2] - m.row[0].col[2] * m.row[1].col[1]);
	return (result);

}

//! Fill 1D matrix with zeros
/*! Fill the provided vector with zeros, and set its size to the provided number of
 * columns
	\param cols Number of columns
	\return Zero filled vector
*/
matrix1d m1_zero(uint16_t cols)
{
	matrix1d answer={{0.,0.,0.,0.},0};
	uint16_t i;

	answer.cols = cols;
	for (i=0; i<cols; i++)
	{
		answer.vector[i] = 0.;
	}
	return (answer);
}

//! Multiply 1D matrix by a scalar
/*! Multiply each element of indicated 1D matrix by a scalar and
 * return as a new 1D matrix.
	\param number scalar to multiply by
	\param row 1D matrix to be multiplied
	\return pointer to the ::matrix1d result, otherwise NULL
*/
matrix1d m1_smult(double number, matrix1d row)
{
	matrix1d answer={{0.,0.,0.,0.},0};
	uint16_t i;

	answer.cols = row.cols;
	for (i=0; i<row.cols; i++)
	{
		answer.vector[i] = row.vector[i] * number;
	}

	return (answer);
}

//! Add one 1D matrix to another
/*! Add each element in the second matrix to each element in
 * the first matrix, returning the answer as a new matrix.
	\param row1 matrix to be added to
	\param row2 matrix to add
	\return sum, as newly created 1D matrix
*/
matrix1d m1_add(matrix1d row1, matrix1d row2)
{
	matrix1d answer={{0.,0.,0.,0.},0};
	uint16_t i;

	answer.cols = row1.cols;
	for (i=0; i<row1.cols; i++)
	{
		answer.vector[i] = row1.vector[i] + row2.vector[i];
	}

	return (answer);
}

//! Subtract one 1D matrix from another
/*! Subtract each element in the second matrix from each element in
 * the first matrix, returning the answer as a new matrix.
	\param row1 matrix to be subtracted from
	\param row2 matrix to subtract
	\return difference, as newly created 1D matrix
*/
matrix1d m1_sub(matrix1d row1, matrix1d row2)
{
	matrix1d answer={{0.,0.,0.,0.},0};
	uint16_t i;

	answer.cols = row1.cols;
	for (i=0; i<row1.cols; i++)
	{
		answer.vector[i] = row1.vector[i] - row2.vector[i];
	}

	return (answer);
}

//! Multiply matrix1d by matrix2d
/*! Perform a matrix multiplication of a matrix1d vector as though it were a column matrix.
	\param Matrix Matrix to multiply by
	\param Vector Vector to multiply
	\return Vector result
*/
matrix1d m1_mmult(matrix2d Matrix,matrix1d Vector)
{
	matrix1d result={{0.,0.,0.,0.},0};
	int i, j;

	if (Matrix.cols == Vector.cols)
	{
		result.cols = Vector.cols;
		for (i=0; i<result.cols; i++)
		{
			result.vector[i] = 0.;
			for (j=0; j<result.cols; j++)
			{
				result.vector[i] += Vector.vector[j]*Matrix.array[i][j];
			}
		}
	}

	return(result);
}

//! matrix1d cross product
/*! Computes the vector cross product (A x B) of two 1d vectors.
	\param Vector1 vector A
	\param Vector2 vector B
\return Vector cross product as 1d vector.*/
matrix1d m1_cross(matrix1d Vector1, matrix1d Vector2)
{
	matrix1d result;

	result.cols = Vector1.cols;
	result.vector[0] = Vector1.vector[1]*Vector2.vector[2] - Vector1.vector[2]*Vector2.vector[1];
	result.vector[1] = -(Vector1.vector[0]*Vector2.vector[2] - Vector1.vector[2]*Vector2.vector[0]);
	result.vector[2] = Vector1.vector[0]*Vector2.vector[1] - Vector1.vector[1]*Vector2.vector[0];

	return(result);
}

double m1_dot(matrix1d a, matrix1d b)
{
	double d = 0.;
	uint16_t i;

	for(i=0; i<a.cols; i++)
	{
		d += a.vector[i]*b.vector[i];
	}
	return (d);
}

//! Create skew symmetric matrix2d from matrix1d
/*! Generate a new 3x3 skew symmetric 2D matrix from the provided 3 element
 * vector.
	\param row row vector to be skewed
	\return skew symmetric 2D matrix
*/
matrix2d m2_skew(matrix1d row)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

	if (row.cols == 3)
	{
		answer.rows = answer.cols = 3;
		answer.array[0][0] = answer.array[1][1] = answer.array[2][2] = 0.;

		answer.array[0][1] = -row.vector[2];
		answer.array[0][2] = row.vector[1];

		answer.array[1][0] = row.vector[2];
		answer.array[1][2] = -row.vector[0];

		answer.array[2][0] = -row.vector[1];
		answer.array[2][1] = row.vector[0];
	}

	return (answer);
}

//! Create diagonal matrix2d from matrix1d
/*! Generate a new nxn diagonal 2D matrix from the provided n element
 * 1D matrix.
	\param row 1D matrix to be skewed
	\return result as new 2D matrix, otherwise NULL
*/
matrix2d m2_diag(matrix1d row)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

	if (row.cols == 3)
	{
		answer.cols = answer.rows = 3;
		answer.array[0][0] = row.vector[0];
		answer.array[1][1] = row.vector[1];
		answer.array[2][2] = row.vector[2];
	}

	return (answer);
}

//! Inverse of matrix2d
/*! Inverse of 2x2 or 3x3 Matrix2D using algorithm at
	http://mathworld.wolfram.com/MatrixInverse.html
	\param m Matrix2d to take inverse of
	\return Inverse matrix, or
*/
matrix2d m2_inverse(matrix2d m)
{
	matrix2d wm;

	wm.rows = m.rows;
	wm.cols = m.cols;

	if (m.rows == 2)
	{
		wm.array[0][0] = m.array[1][1];
		wm.array[1][1] = m.array[0][0];
		wm.array[1][0] = -m.array[1][0];
		wm.array[0][1] = -m.array[0][1];
	}
	else
	{
		wm.array[0][0] = m.array[1][1]*m.array[2][2] - m.array[1][2]*m.array[2][1];
		wm.array[0][1] = m.array[0][2]*m.array[2][1] - m.array[0][1]*m.array[2][2];
		wm.array[0][2] = m.array[0][1]*m.array[1][2] - m.array[0][2]*m.array[1][1];

		wm.array[1][0] = m.array[1][2]*m.array[2][0] - m.array[1][0]*m.array[2][2];
		wm.array[1][1] = m.array[0][0]*m.array[2][2] - m.array[0][2]*m.array[2][0];
		wm.array[1][2] = m.array[0][2]*m.array[1][0] - m.array[0][0]*m.array[1][2];

		wm.array[2][0] = m.array[1][0]*m.array[2][1] - m.array[1][1]*m.array[2][0];
		wm.array[2][1] = m.array[0][1]*m.array[2][0] - m.array[0][0]*m.array[2][1];
		wm.array[2][2] = m.array[0][0]*m.array[1][1] - m.array[0][1]*m.array[1][0];
	}

	return( m2_smult((1/m2_determinant(m)), wm) );
}

//!  Determinant of a 2D matrix
/*! Return the determinant for a 2x2 or 3x3 2D Matrix
	\param m Square 2D matrix to calculate detrminant of
	\return determinant, otherwise NaN
*/
double m2_determinant(matrix2d m)
{
	double result;

	if (m.rows == 2 && m.cols == 2)
	{
		result = m.array[0][0]*m.array[1][1] - m.array[0][1]*m.array[1][0];
		return (result);
	}

	if (m.rows == 3 && m.cols == 3)
	{
		result = m.array[0][0] * (m.array[1][1] * m.array[2][2] - m.array[1][2] * m.array[2][1]);
		result += m.array[0][1] * (m.array[1][2] * m.array[2][0] - m.array[1][0] * m.array[2][2]);
		result += m.array[0][2] * (m.array[1][0] * m.array[2][1] - m.array[1][1] * m.array[2][0]);
		return (result);
	}

	return (NAN);
}

//! Compute the Euclidean norm of a 1D matrix
/*! Calculate and return the Euclidean norm of the provided 1D matrix.
	\param row 1D matrix to be normed
	\return the calculated norm
*/
double m1_norm(matrix1d row)
{
	double norm=0.;

	for (int i=0; i<row.cols; i++)
	{
		norm += row.vector[i] * row.vector[i];
	}

	return (sqrt(norm));
}

//! Create 2D zero matrix
/*! Create a matrix of the requested sized, set to zero.
	\param rows Number of rows in the matrix
	\param cols Number of columns in the matrix
	\return New 2D matrix
*/
matrix2d m2_zero(uint16_t rows, uint16_t cols)
{
	int i, j;
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

	if (rows <= 3 && cols <=3)
	{
		answer.rows = rows;
		answer.cols = cols;
		for (i=0; i<rows; i++)
		{
			for (j=0; j<cols; j++)
			{
				answer.array[i][j] = 0.;
			}
		}
	}

	return answer;
}

//! Create 2D identity matrix
/*! Create a square identity matrix of the requested size and return
 * as a new matrix.
	\param rows the size, in rows and columns, of the new matrix
	\return the new identity matrix, as a 2D matrix
*/

matrix2d m2_eye(uint16_t rows)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

	if (rows <= 3)
	{
		answer = m2_zero(rows,rows);
		answer.array[0][0] = answer.array[1][1] = answer.array[2][2] = 1.;
	}

	return (answer);
}

//! Multiply 2D matrix by a scalar
/*! Multiply each element of indicated 2D matrix by a scalar and
 * return as a new 2D matrix.
	\param number scalar to multiply by
	\param matrix 2D matrix to be multiplied
	\return 2D matrix result
*/
matrix2d m2_smult(double number, matrix2d matrix)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
	uint16_t i, j;

	answer = matrix;
	for (i=0; i<matrix.rows; i++)
	{
		for (j=0; j<matrix.cols; j++)
		{
			answer.array[i][j] *= number;
		}
	}
	return (answer);
}

//! Add one matrix2d to another
/*! Add each element in the second matrix to each element in
 * the first matrix, returning the answer as a new matrix.
	\param matrix1 matrix to be added from
	\param matrix2 matrix to add
	\return sum, as newly created 2D matrix
*/
matrix2d m2_add(matrix2d matrix1, matrix2d matrix2)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
	uint16_t i, j;

	if (matrix1.rows == matrix2.rows && matrix1.cols == matrix2.cols)
	{
		answer.rows = matrix1.rows;
		answer.cols = matrix1.cols;
		for (i=0; i<matrix1.rows; i++)
		{
			for (j=0; j<matrix1.cols; j++)
			{
				answer.array[i][j] = matrix1.array[i][j] + matrix2.array[i][j];
			}
		}
	}

	return (answer);
}

//! Subtract one matrix2d from another
/*! Subtract each element in the second matrix from each element in
 * the first matrix, returning the answer as a new matrix.
	\param matrix1 matrix to be subtracted from
	\param matrix2 matrix to subtract
	\return difference, as newly created 2D matrix
*/
matrix2d m2_sub(matrix2d matrix1, matrix2d matrix2)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
	uint16_t i, j;

	if (matrix1.rows == matrix2.rows && matrix1.cols == matrix2.cols)
	{
		answer.rows = matrix1.rows;
		answer.cols = matrix1.cols;
		for (i=0; i<matrix1.rows; i++)
		{
			for (j=0; j<matrix1.cols; j++)
			{
				answer.array[i][j] = matrix1.array[i][j] - matrix2.array[i][j];
			}
		}
	}

	return (answer);
}

//! Add one matrix2d to another
/*! Add each element in the first matrix to each element in
 * the second matrix, returning the answer as a new matrix.
		\param matrix1 matrix to be added to
		\param matrix2 matrix to add
		\return sum, as newly created 2D matrix
*/

//! Return transpose of a 2D matrix
/*! Generate a new ::matrix2d that is is the transpose of the provided ::matrix2d
	\param matrix ::matrix2d to be transposed
	\return Transpose, as new ::matrix2d
*/
matrix2d m2_transpose(matrix2d matrix)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
	uint16_t i, j;

	if (matrix.rows <=3 && matrix.cols <= 3)
	{
		answer.rows = matrix.cols;
		answer.cols = matrix.rows;
		for (i=0; i<matrix.rows; i++)
		{
			for (j=0; j<matrix.cols; j++)
			{
				answer.array[j][i] = matrix.array[i][j];
			}
		}
	}

	return (answer);
}

//! Unskew 3x3 2D matrix
/*! Create the 3 element 1D matrix correponding to a 3x3 skew symmetric matrix
	\param matrix 3x3 2D skew symmetric matrix
	\return ::matrix1d representing unskewed elements
*/
matrix1d m2_unskew(matrix2d matrix)
{
	matrix1d answer={{0.,0.,0.,0.},0};

	if (matrix.rows == 3 && matrix.cols == 3)
	{
		answer.cols = 3;
		answer.vector[0] = -matrix.array[1][2];
		answer.vector[1] = matrix.array[0][2];
		answer.vector[2] = -matrix.array[0][1];
	}

	return (answer);
}

//! Calculate the trace of a 2D matrix
/*! Add the diagonal elements of an nxn 2D matrix.
	\param matrix nxn matrix to take the elements from
	\return Sum of diagonals, otherwise 0.0
*/
double m2_trace(matrix2d matrix)
{
	double answer=0.;
	uint16_t i;

	for (i=0; i<matrix.rows; i++)
	{
		answer += matrix.array[i][i];
	}

	return (answer);
}

//! Matrix product
/*! Calculate the matrix product of two 2D matrices and return a new 2D matrix
	\param matrix1 mxn matrix
	\param matrix2 nxm matrix
	\return new matrix, of dimension rows x cols, otherwise NULL
*/
matrix2d m2_mmult(matrix2d matrix1, matrix2d matrix2)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
	uint16_t i, j, k;

	if (matrix1.cols == matrix2.rows && matrix1.rows <= 3 && matrix1.cols <=3 && matrix2.rows <= 3 && matrix2.cols <=3)
	{
		answer.rows = matrix1.rows;
		answer.cols = matrix2.cols;
		for (i=0; i<matrix1.rows; i++)
		{
			for (j=0; j<matrix2.cols; j++)
			{
				answer.array[i][j] = 0.;
				for (k=0; k<matrix1.cols; k++)
				{
					answer.array[i][j] += matrix1.array[i][k] * matrix2.array[k][j];
				}
			}
		}
	}

	return (answer);
}

//! rmatrix from rmatrix
/*! Converts 3x3 matrix in row order form to cartesian form.
	\param matrix Row major matrix to convert
	\return Converted matrix in row order form
*/
matrix2d m2_from_rm(rmatrix matrix)
{
	matrix2d m2;
	int i, j;

	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
		{
			m2.array[i][j] = matrix.row[i].col[j];
		}
	}

	return (m2);
}

// Cmatrix3x3 to matrix2d
/*! Convert a 3x3 Cartesian matrix to a matrix2d.
	\param matrix cmatrix to convert
	\return matrix2d
*/
matrix2d cm3x3_to_m2(cmatrix matrix)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

	answer.rows = 3;
	answer.cols = 3;
	answer.array[0][0] = matrix.r1.x;
	answer.array[0][1] = matrix.r1.y;
	answer.array[0][2] = matrix.r1.z;
	answer.array[1][0] = matrix.r2.x;
	answer.array[1][1] = matrix.r2.y;
	answer.array[1][2] = matrix.r2.z;
	answer.array[2][0] = matrix.r3.x;
	answer.array[2][1] = matrix.r3.y;
	answer.array[2][2] = matrix.r3.z;

	return (answer);
}

// Cvector to matrix1d
/*! Convert a Cartesian vector to a matrix1d.
	\param vector cvector to convert
	\return Row vector
*/
matrix1d cv_to_m1(cvector vector)
{
	matrix1d answer={{0.,0.,0.,0.},0};

	answer.cols = 3;
	answer.vector[0] = vector.x;
	answer.vector[1] = vector.y;
	answer.vector[2] = vector.z;

	return (answer);
}

// Cvector to matrix2d
/*! Convert a Cartesian vector to either a row or column matrix2d.
	\param vector cvector to convert
	\param direction column if 1 (DIRECTION_COLUMN), otherwise row
	\return Row or column matrix
*/
matrix2d cv_to_m2(cvector vector,int direction)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};

	if (direction == DIRECTION_COLUMN)
	{
		answer.rows = 3;
		answer.cols = 1;
		answer.array[0][0] = vector.x;
		answer.array[1][0] = vector.y;
		answer.array[2][0] = vector.z;
	}
	else
	{
		answer.rows = 1;
		answer.cols = 3;
		answer.array[0][0] = vector.x;
		answer.array[0][1] = vector.y;
		answer.array[0][2] = vector.z;
	}
	return (answer);
}

//! Matrix1d to matrix2d
/*! Convert a matrix1d vector to a row element matrix2d
	\param vector Matrix1d to be converted
	\param direction Alignment, column order if 1 (DIRECTION_COLUMN), otherwise row
	order.
	\return Single row or column matrix2d
*/
matrix2d m1_to_m2(matrix1d vector,int direction)
{
	matrix2d answer={0,0,{{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}}};
	uint16_t i;

	switch (direction)
	{
	case DIRECTION_COLUMN:
		answer.cols = 1;
		answer.rows = vector.cols;
		for (i=0; i<vector.cols; i++)
		{
			answer.array[i][0] = vector.vector[i];
		}
		break;
		break;
	default:
		answer.rows = 1;
		answer.cols = vector.cols;
		for (i=0; i<vector.cols; i++)
		{
			answer.array[0][i] = vector.vector[i];
		}
		break;
	}

	return (answer);
}

//! Eigen values of a 2x2 square matrix
/*! Calculate the 2 element 1D matrix that is the set of eigenvalues of a 2x2 2D matrix
	\param matrix 2x2 2D matrix
	\return 2 element 1D matrix of eigenvalues, otherwise NULL
*/
matrix1d m2_eig2x2(matrix2d matrix)
{
	matrix1d answer={{0.,0.,0.,0.},0};
	double trm, detm;
	double trm2, detm4;

	if (matrix.cols == 2 && matrix.rows == 2)
	{
		trm = matrix.array[0][0] + matrix.array[1][1];
		detm = matrix.array[0][0]*matrix.array[1][1] - matrix.array[0][1]*matrix.array[1][0];

		if ((trm2=trm*trm) < (detm4=4*detm))
			return (answer);

		answer.cols = 2;
		answer.vector[0] = (trm + sqrt(trm2-detm4))/2.;
		answer.vector[1] = (trm - sqrt(trm2-detm4))/2.;
	}

	return (answer);
}

//! Spectral norm of a 2x2 matrix
/*! Calculate the spectral norm of the provided 2x2 matrix.
	\param matrix 2x2 matrix
	\return spectral norm, otherwise NaN
*/
double m2_snorm2x2(matrix2d matrix)
{
	double answer=NAN;
	matrix1d re;

	if (matrix.cols == 2 && matrix.rows == 2)
	{
		re = m2_eig2x2(m2_mmult(m2_transpose(matrix),matrix));

		if (re.cols == 0)
			return (NAN);

		if (re.vector[0] > re.vector[1])
			answer = sqrt(re.vector[0]);
		else
			answer = sqrt(re.vector[1]);
	}

	return (answer);
}

//! Determine local byte order
/*! Investigate a locally stored number to determine the byte order of the local machine.
	\return ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
*/

uint8_t local_byte_order()
{
	uint16_t test = 1;
	uint8_t *check;

	check = (uint8_t *)&test;

	if (check[0] == 0)
		return (ORDER_BIGENDIAN);
	else
		return (ORDER_LITTLEENDIAN);
}

//! Memory to 16 bit unsigned integer
/*! Return the 16 bit unsigned integer equivalent of a location in memory, corrected for the local byte order.
	\param pointer location in memory to be cast
	\param order byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
	\return 16 bit unsigned integer
*/

uint16_t uint16from(uint8_t *pointer, uint8_t order)
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
	\param order byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
	\return 16 bit signed integer
*/

int16_t int16from(uint8_t *pointer, uint8_t order)
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
	\param order byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
	\return 32 bit unsigned integer
*/

uint32_t uint32from(uint8_t *pointer, uint8_t order)
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
	\param order byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
	\return 32 bit signed integer
*/

int32_t int32from(uint8_t *pointer, uint8_t order)
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
	\param order byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
	\return 32 bit float
*/

float floatfrom(uint8_t *pointer, uint8_t order)
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
	\param order byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
	\return 64 bit float
*/

double doublefrom(uint8_t *pointer, uint8_t order)
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
	\param order desired byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
*/

void uint32to(uint32_t value, uint8_t *pointer, uint8_t order)
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
	\param order desired byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
*/

void int32to(int32_t value, uint8_t *pointer, uint8_t order)
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
	\param order desired byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
*/

void uint16to(uint16_t value, uint8_t *pointer, uint8_t order)
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
	\param order desired byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
*/

void int16to(int16_t value, uint8_t *pointer, uint8_t order)
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
	\param order desired byte order of the data in memory. ORDER_BIGENDIAN or ORDER_LITTLEENDIAN
*/

void floatto(float value, uint8_t *pointer, uint8_t order)
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

//! Initialize estimator
/*! Setup the provided ::estimatorhandle so that it can be fed values
 * and provide estimates.
	\param estimate Pointer to an ::estimatorhandle.
	\param degree The degree of the polynomial fit for the estimator.
	\param size The number of estimates to be averaged for the total estimate.
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
	\param x Vector of N vectors of N dependent variables.
	\param y vector of N independent variables.
	\return Vector of parameters
*/
void multisolve(vector< vector<double> > x, vector<double> y, vector<double>& a)
{
	uint32_t order;
	vector< vector<double> > dx(y.size()-1, vector<double>(y.size()-1));
	vector<double> dy(y.size()-1);
	//	vector<double> a(y.size());
	vector<double> da(y.size()-1);

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
		/*
		if (order == 1)
		{
			da[0] = dy[0] / dx[0][0];
		}
		else
		{
		*/
		multisolve(dx, dy, da);
		//		}

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

	//	return (a);

}

//! Evaluate polynomial
/*! Return the result of the given Nth order polynomial, evaluated at the given location.
 * \param x Independent variable where polynomial should be evaluated.
 * \param parms Vector of parameters for Nth order polynomial to be evaluated.
 * \return Value of the variable, evaluated at the location of the independent variable.
 */
double evaluate_poly(double x, vector<double> parms)
{
	double result;

	result = parms[parms.size()-1];
	for (uint16_t i=parms.size()-2; i<parms.size(); --i)
	{
		result *= x;
		result += parms[i];
	}

	return result;
}

double evaluate_poly_slope(double x, vector<double> parms)
{
	double result;

	result = parms[parms.size()-1] * (parms.size()-1);
	for (uint16_t i=parms.size()-2; i>0; --i)
	{
		result *= x;
		result += i * parms[i];
	}

	return result;
}

rvector rv_evaluate_poly(double x, vector< vector<double> > parms)
{
	uvector result;

	for (uint16_t ic=0; ic<parms.size(); ++ic)
	{
		result.a4[ic] = parms[ic][parms[ic].size()-1];
		for (uint16_t i=parms[ic].size()-2; i<parms[ic].size(); --i)
		{
			result.a4[ic] *= x;
			result.a4[ic] += parms[ic][i];
		}
	}

	return result.r;
}

rvector rv_evaluate_poly_slope(double x, vector< vector<double> > parms)
{
	uvector result;

	for (uint16_t ic=0; ic<parms.size(); ++ic)
	{
		result.a4[ic] = parms[ic][parms[ic].size()-1] * (parms[ic].size()-1);
		for (uint16_t i=parms[ic].size()-2; i>0; --i)
		{
			result.a4[ic] *= x;
			result.a4[ic] += i * parms[ic][i];
		}
	}

	return result.r;
}

quaternion q_evaluate_poly(double x, vector< vector<double> > parms)
{
	uvector result;

	for (uint16_t ic=0; ic<parms.size(); ++ic)
	{
		result.a4[ic] = parms[ic][parms[ic].size()-1];
		for (uint16_t i=parms[ic].size()-2; i<parms[ic].size(); --i)
		{
			result.a4[ic] *= x;
			result.a4[ic] += parms[ic][i];
		}
	}

	return result.q;
}

quaternion q_evaluate_poly_slope(double x, vector< vector<double> > parms)
{
	uvector result;

	for (uint16_t ic=0; ic<parms.size(); ++ic)
	{
		result.a4[ic] = parms[ic][parms[ic].size()-1] * (parms[ic].size()-1);
		for (uint16_t i=parms[ic].size()-2; i>0; --i)
		{
			result.a4[ic] *= x;
			result.a4[ic] += i * parms[ic][i];
		}
	}

	return result.q;
}

//! Perform general order polynomial fit.
/*! Fit a polynomial of type: y(x) = An*x^n + An-1*x^(n-1) + ... + * A2*x^2 + A1*x + A0
 * where n is the order of the polynomial. The input data is n
 * points in x and n matching points in y. The fit is achieved
 * using a Vandermonde interpolating polynomial.
	\param x Vector of x values
	\param y Vector of y values
	\param n Order of polynomial
	\return Vector of polynomial coefficients
*/
vector<double> polyfit(vector<double> &x, vector<double> &y)
{
	uint32_t order;
	vector< vector<double> > dx;
	vector<double> dy;
	vector<double> da;
	vector<double> a;

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
		return (NULL);
	}

	for (i=0; i<order+2; i++)
	{
		if ((gjk->binom[i] = (int32_t *)calloc(order+2,sizeof(int32_t))) == NULL)
		{
			gauss_jackson_dekernel(gjk);
			return (NULL);
		}
	}

	if ((gjk->alpha = (double **)calloc(order+2,sizeof(double *))) == NULL)
	{
		gauss_jackson_dekernel(gjk);
		return (NULL);
	}

	for (i=0; i<order+2; i++)
	{
		if ((gjk->alpha[i] = (double *)calloc(order+1,sizeof(double))) == NULL)
		{
			gauss_jackson_dekernel(gjk);
			return (NULL);
		}
	}

	if ((gjk->beta = (double **)calloc(order+2,sizeof(double *))) == NULL)
	{
		gauss_jackson_dekernel(gjk);
		return (NULL);
	}

	for (i=0; i<order+2; i++)
	{
		if ((gjk->beta[i] = (double *)calloc(order+1,sizeof(double))) == NULL)
		{
			gauss_jackson_dekernel(gjk);
			return (NULL);
		}
	}

	if ((gjk->c = (double *)calloc(order+3,sizeof(double *))) == NULL)
	{
		gauss_jackson_dekernel(gjk);
		return (NULL);
	}

	if ((gjk->gam = (double *)calloc(order+2,sizeof(double *))) == NULL)
	{
		gauss_jackson_dekernel(gjk);
		return (NULL);
	}

	if ((gjk->q = (double *)calloc(order+3,sizeof(double *))) == NULL)
	{
		gauss_jackson_dekernel(gjk);
		return (NULL);
	}

	if ((gjk->lam = (double *)calloc(order+3,sizeof(double *))) == NULL)
	{
		gauss_jackson_dekernel(gjk);
		return (NULL);
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
		return (NULL);

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
				return (NULL);
			}
			else
			{
				free(gji->vi);
				free(gji);
				return (NULL);
			}
		}
		else
		{
			free(gji);
			return (NULL);
		}
	}
	else
		return (NULL);
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
			return (NULL);
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
	\param gji Pointer to Gauss-Jackson Instance
	\param vi Pointer to Independent variable
	\param vd0 Pointer to array of Dependent variable for all axes at given step
	\param vd1 Pointer to array of First derivative of Dependent variable for all axes at given
	step
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
	\param vd1 Pointer to array of First derivative of Dependent variable for all axes at given
	step
	\param vd2 Pointer to array of Second derivative of Dependent variable for all axes at given
	step
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

double actan(double y,double x)
{
	double actan;

	actan = atan2(y,x);
	if (actan < 0. )
		actan += D2PI;
	return (actan);
}

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

std::ostream& operator << (std::ostream& out, const rvector& a)
{
	out.precision(15);
	out<<"["<<a.col[0]<<","<<a.col[1]<<","<<a.col[2]<<"]";
	return out;
}

std::ostream& operator << (std::ostream& out, const vector<rvector>& a)
{
	for(vector<rvector>::const_iterator I = a.begin(); I != a.end(); ++I)
		out<<*I<<"\t";
	return out;
} 

std::istream& operator >> (std::istream& in, rvector& a)
{
	char junk;
	in >> junk >> a.col[0] >> junk >> a.col[1] >> junk >> a.col[2] >> junk;
	return in;
}

std::ostream& operator << (std::ostream& out, const rmatrix& a)
{
	out.precision(15);
	out << "["  << a.row[0].col[0] << "," << a.row[0].col[1] << "," << a.row[0].col[2] << ";"
																					   << a.row[1].col[0] << "," << a.row[1].col[1] << "," << a.row[1].col[2] << ";"
																																							  << a.row[2].col[0] << "," << a.row[2].col[1] << "," << a.row[2].col[2] << "]";
	return out;
}

std::ostream& operator << (std::ostream& out, const vector<rmatrix>& a)
{
	for(vector<rmatrix>::const_iterator I = a.begin(); I != a.end(); ++I)
		out<<*I<<"\t";
	return out;
} 

std::istream& operator >> (std::istream& in, rmatrix& a)
{
	char junk;
	in >> junk >> a.row[0].col[0] >> junk >> a.row[0].col[1] >> junk >> a.row[0].col[2] >> junk
																						>> a.row[1].col[0] >> junk >> a.row[1].col[1] >> junk >> a.row[1].col[2] >> junk
																																								 >> a.row[2].col[0] >> junk >> a.row[2].col[1] >> junk >> a.row[2].col[2] >> junk;
	return in;
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

std::ostream& operator << (std::ostream& out, const cmatrix& a)
{
	out << "["  << a.r1.x << "," << a.r1.y << "," << a.r1.z << ";"
		<< a.r2.x << "," << a.r2.y << "," << a.r2.z << ";"
		<< a.r3.x << "," << a.r3.y << "," << a.r3.z << "]";
	return out;
}

std::istream& operator >> (std::istream& in, cmatrix& a)
{
	char junk;
	in >> junk >> a.r1.x >> junk >> a.r1.y >> junk >> a.r1.z >> junk
	   >> a.r2.x >> junk >> a.r2.y >> junk >> a.r2.z >> junk
	   >> a.r3.x >> junk >> a.r3.y >> junk >> a.r3.z >> junk;
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

std::ostream& operator << (std::ostream& out, const quaternion& a)
{
	out<< "[" << a.d.x << "," << a.d.y << "," << a.d.z << "," << a.w << "]";
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

lsfit::lsfit()
	: element_cnt(10), order(2), resolution(0.)
{
//	lsfit(10, 2, 0.);
	//	printf("lsfit()\n");
}

lsfit::lsfit(uint16_t cnt)
	: element_cnt(cnt), order(2), resolution(0.)
{
//	lsfit(cnt, 2, 0.);
	//	printf("lsfit(%u)\n",cnt);
}

lsfit::lsfit(uint16_t cnt, uint16_t ord)
	: element_cnt(cnt), order(ord), resolution(0.)
{
//	lsfit(cnt, ord, 0.);
	//	printf("lsfit(%u %u)\n",cnt,ord);
}

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

void lsfit::update(double x, double y)
{
	fitelement cfit;

	// Independent variable
	cfit.x = x;

	// Dependent variable for quaternion
	cfit.y.a4[0] = y;

	update(cfit, 1);
}

void lsfit::update(double x, rvector y)
{
	fitelement cfit;

	// Independent variable
	cfit.x = x;

	// Dependent variable for quaternion
	cfit.y.r = y;

	update(cfit, 3);
}

void lsfit::update(double x, quaternion y)
{
	fitelement cfit;

	// Independent variable
	cfit.x = x;

	// Dependent variable for quaternion
	cfit.y.q = y;

	update(cfit, 4);
	if (parms.size() && (isnan(parms[0][0]) || isnan(parms[0][1] || isnan(parms[0][2]))))
	{
		for (uint16_t i=0; i<var.size(); ++i)
		{
			printf("%.15g [%g %g %g %g]\n", var[i].x, var[i].y.q.w, var[i].y.q.d.x, var[i].y.q.d.y, var[i].y.q.d.z);
		}
		printf("\n");
	}
}

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

void lsfit::fit()
{
	// Minimize independent variable by zero offsetting
	basex = var[0].x;

	// For each independent, calculate sums of powers
	vector<double> sumx(2*order+1);
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
		vector<double> sumxy(order+1);
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

		vector< vector<double> > xs(order+1, vector<double>(order+1));
		vector<double> ys(order+1);
		vector<double> tx(order+1);
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


cmatrix DCM::base2_from_base1(basisOrthonormal base2,basisOrthonormal base1){

// compute dcm matrix (A) to represent vector in base 2 coordinates

// References
// - Quaternion and Rotation Sequences, Kuipers, pg 160 (I think these
//   formulas are wrong in the book! they are inversed, must check)
// - http://people.ae.illinois.edu/tbretl/ae403/handouts/06-dcm.pdf (this
//   reference seems to be sound)
// - http://www.starlino.com/dcm_tutorial.html (eq 1.4)

// example: vector_body_coordinates = dcm_base2_from_base1 * vector_inertial_coodinates

// Notes:
// - matrix A represents a frame rotation that relates the initial reference
//   frame {X,Y,Z} to a rotated frame {x,y,z}: x = AX. Example, appliying
//   the operation AX - where X is for instance a vector in the inertial
//   frame - resutls in the coordinates of that vector in the new frame {x,y,z}
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

    return  {
        { dot_cv(base1.i, base2.i) , dot_cv(base1.j, base2.i), dot_cv(base1.k, base2.i) },
        { dot_cv(base1.i, base2.j) , dot_cv(base1.j, base2.j), dot_cv(base1.k, base2.j) },
        { dot_cv(base1.i, base2.k) , dot_cv(base1.j, base2.k), dot_cv(base1.k, base2.k) }
    };
}

cmatrix DCM::base1_from_base2(basisOrthonormal base1, basisOrthonormal base2){
    // compute dcm matrix (A) to represent vector given in base 2
    // in base 1 coordinates

    // References
    // - Quaternion and Rotation Sequences, Kuipers, pg 160
    // - http://people.ae.illinois.edu/tbretl/ae403/handouts/06-dcm.pdf
    // - http://www.starlino.com/dcm_tutorial.html (eq 1.4)

    // example:
    // vector_inertial_coordinates = dcm_base1_from_base2 * vector_body_coodinates

    // TODOs
    // - add validation step to verify if the bases are orthogonal

    // just compute the transpose
    return cm_transpose(base2_from_base1(base2,base1));
}


// convert from cartesian vector to row vector
rvector RowVector::from_cv(cvector v){
    rvector rv;
    rv.col[0] = v.x;
    rv.col[1] = v.y;
    rv.col[2] = v.z;
    return rv;
}



//! @}
