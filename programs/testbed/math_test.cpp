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

#include "math/mathlib.h"
#include <cstdio>
#include <cmath>

int main(int argc, char *argv[])
{
	/*Do Not Alter rvec 1-4 or cvec 1-4 or qrot 1-5(tests depend on these exact values)*/
	rvector rvec1 = {{3.,3.,0.}};
	rvector rvec2 = {{0.,5.,5.}};
	rvector rvec3 = {{7.,0.,7.}};
	rvector rvec4 = {{-1.,-1.,-1.}};
	rvector rvec5, rvec6;
	cvector cvec1, cvec2, cvec3, cvec4, cvec5, cvec6;
	cvec1.x = 3.; cvec1.y = 3.; cvec1.z = 0.;
	cvec2.x = 0.; cvec2.y = 5.; cvec2.z = 5.;
	cvec3.x = 7.; cvec3.y = 0.; cvec3.z = 7.;
	cvec4.x = -1.; cvec4.y = -1.; cvec4.z = -1.;
	quaternion qrot1, qrot2, qrot3, qrot4, qrot5;
	qrot1.w = cos(RADOF(22.5)); qrot1.d.x = qrot1.d.y = sqrt(1./3.)*sin(RADOF(22.5)); qrot1.d.z = -sqrt(1./3.)*sin(RADOF(22.5)); //45 around vector(1, 1, -1)
	qrot2.w = sqrt(.5); qrot2.d.x = sqrt(.5); qrot2.d.y = 0.; qrot2.d.z = 0.; //90 around X (a positive rotation around a positive axis, viewed from the origin, is clockwise)
	qrot3.w = sqrt(.5); qrot3.d.x = 0.; qrot3.d.y = sqrt(.5); qrot3.d.z = 0.; //90 around Y
	qrot4.w = sqrt(.5); qrot4.d.x = 0.; qrot4.d.y = 0.; qrot4.d.z = sqrt(.5); //90 around Z
	qrot5.w = sqrt(.5); qrot5.d.x = 0.; qrot5.d.y = -sqrt(.5); qrot5.d.z = 0.; //-90 around Y

	avector avec1, avec2;
	uvector uvec1, uvec2, uvec3, uvec4, uvec5;
	cmatrix cmat1, cmat2, cmat3;
	rmatrix rmat1, rmat2, rmat3;
	double angle1;
	double error, terror = 0.;

	matrix2d dmat1, dmat2, dmat3, dmat4, dmat5;
	matrix1d dvec1;
	double val1, val2, val3, val4;

	printf("Basic Functionality tests: mathlib\n");
	printf("\n");

	uvec1.r = rvec1;
	uvec2.r = rvec2;
	uvec3.r = rvec3;
	uvec4.r = rvec4;

	error = sep_cv(uvec1.c,uvec2.c)+sep_cv(uvec1.c,uvec3.c)+sep_cv(uvec1.c,uvec4.c) +sep_cv(uvec2.c,uvec1.c)+sep_cv(uvec2.c,uvec3.c)+sep_cv(uvec2.c,uvec4.c) +sep_cv(uvec3.c,uvec1.c)+sep_cv(uvec3.c,uvec2.c)+sep_cv(uvec3.c,uvec4.c) +sep_cv(uvec4.c,uvec1.c)+sep_cv(uvec4.c,uvec2.c)+sep_cv(uvec4.c,uvec3.c) -2.*DPI-6.*(DPI-asin(sqrt(1./3.)));
	terror += fabs(error);

	printf("Function: sep_cv()\n");
	printf("%11.5g %11.5g %11.5g\n",sep_cv(uvec1.c,uvec2.c),sep_cv(uvec1.c,uvec3.c),sep_cv(uvec1.c,uvec4.c));
	printf("%11.5g %11.5g %11.5g\n",sep_cv(uvec2.c,uvec1.c),sep_cv(uvec2.c,uvec3.c),sep_cv(uvec2.c,uvec4.c));
	printf("%11.5g %11.5g %11.5g\n",sep_cv(uvec3.c,uvec1.c),sep_cv(uvec3.c,uvec2.c),sep_cv(uvec3.c,uvec4.c));
	printf("%11.5g %11.5g %11.5g\n",sep_cv(uvec4.c,uvec1.c),sep_cv(uvec4.c,uvec2.c),sep_cv(uvec4.c,uvec3.c));

	printf("sep_cv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	error = sep_rv(uvec1.r,uvec2.r)+sep_rv(uvec1.r,uvec3.r)+sep_rv(uvec1.r,uvec4.r) +sep_rv(uvec2.r,uvec1.r)+sep_rv(uvec2.r,uvec3.r)+sep_rv(uvec2.r,uvec4.r) +sep_rv(uvec3.r,uvec1.r)+sep_rv(uvec3.r,uvec2.r)+sep_rv(uvec3.r,uvec4.r) +sep_rv(uvec4.r,uvec1.r)+sep_rv(uvec4.r,uvec2.r)+sep_rv(uvec4.r,uvec3.r)-2.*DPI-6.*(DPI-asin(sqrt(1./3.)));
	terror += fabs(error);

	printf("Function: sep_rv()\n");
	printf("%11.5g %11.5g %11.5g\n",sep_rv(uvec1.r,uvec2.r),sep_rv(uvec1.r,uvec3.r),sep_rv(uvec1.r,uvec4.r));
	printf("%11.5g %11.5g %11.5g\n",sep_rv(uvec2.r,uvec1.r),sep_rv(uvec2.r,uvec3.r),sep_rv(uvec2.r,uvec4.r));
	printf("%11.5g %11.5g %11.5g\n",sep_rv(uvec3.r,uvec1.r),sep_rv(uvec3.r,uvec2.r),sep_rv(uvec3.r,uvec4.r));
	printf("%11.5g %11.5g %11.5g\n",sep_rv(uvec4.r,uvec1.r),sep_rv(uvec4.r,uvec2.r),sep_rv(uvec4.r,uvec3.r));

	printf("sep_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: dot_cv() dot_rv() m1_dot()\n");
	uvec1.m1.cols = uvec2.m1.cols = uvec3.m1.cols = uvec4.m1.cols = 3;
	printf("dot_cv() tests:                       dot_rv() tests:                       m1_dot() tests:\n");
	printf("(%11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g)\n",dot_cv(uvec1.c,uvec2.c),dot_cv(uvec1.c,uvec3.c),dot_cv(uvec1.c,uvec4.c),dot_rv(uvec1.r,uvec2.r),dot_rv(uvec1.r,uvec3.r),dot_rv(uvec1.r,uvec4.r),m1_dot(uvec1.m1,uvec2.m1),m1_dot(uvec1.m1,uvec3.m1),m1_dot(uvec1.m1,uvec4.m1));
	printf("(%11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g)\n",dot_cv(uvec2.c,uvec1.c),dot_cv(uvec2.c,uvec3.c),dot_cv(uvec2.c,uvec4.c),dot_rv(uvec2.r,uvec1.r),dot_rv(uvec2.r,uvec3.r),dot_rv(uvec2.r,uvec4.r),m1_dot(uvec2.m1,uvec1.m1),m1_dot(uvec2.m1,uvec3.m1),m1_dot(uvec2.m1,uvec4.m1));
	printf("(%11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g)\n",dot_cv(uvec3.c,uvec1.c),dot_cv(uvec3.c,uvec2.c),dot_cv(uvec3.c,uvec4.c),dot_rv(uvec3.r,uvec1.r),dot_rv(uvec3.r,uvec2.r),dot_rv(uvec3.r,uvec4.r),m1_dot(uvec3.m1,uvec1.m1),m1_dot(uvec3.m1,uvec2.m1),m1_dot(uvec3.m1,uvec4.m1));
	printf("(%11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g)\n",dot_cv(uvec4.c,uvec1.c),dot_cv(uvec4.c,uvec2.c),dot_cv(uvec4.c,uvec3.c),dot_rv(uvec4.r,uvec1.r),dot_rv(uvec4.r,uvec2.r),dot_rv(uvec4.r,uvec3.r),m1_dot(uvec4.m1,uvec1.m1),m1_dot(uvec4.m1,uvec2.m1),m1_dot(uvec4.m1,uvec3.m1));
	error = fabs(dot_cv(uvec1.c,uvec2.c)-15.)+fabs(dot_cv(uvec1.c,uvec3.c)-21.)+fabs(dot_cv(uvec1.c,uvec4.c)+6.)
			+fabs(dot_cv(uvec2.c,uvec1.c)-15.)+fabs(dot_cv(uvec2.c,uvec3.c)-35.)+fabs(dot_cv(uvec2.c,uvec4.c)+10.)
			+fabs(dot_cv(uvec3.c,uvec1.c)-21.)+fabs(dot_cv(uvec3.c,uvec2.c)-35.)+fabs(dot_cv(uvec3.c,uvec4.c)+14.)
			+fabs(dot_cv(uvec4.c,uvec1.c)+6.)+fabs(dot_cv(uvec4.c,uvec2.c)+10.)+fabs(dot_cv(uvec4.c,uvec3.c)+14.);
	terror += fabs(error);
	printf("dot_cv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dot_rv(uvec1.r,uvec2.r)-15.)+fabs(dot_rv(uvec1.r,uvec3.r)-21.)+fabs(dot_rv(uvec1.r,uvec4.r)+6.)
			+fabs(dot_rv(uvec2.r,uvec1.r)-15.)+fabs(dot_rv(uvec2.r,uvec3.r)-35.)+fabs(dot_rv(uvec2.r,uvec4.r)+10.)
			+fabs(dot_rv(uvec3.r,uvec1.r)-21.)+fabs(dot_rv(uvec3.r,uvec2.r)-35.)+fabs(dot_rv(uvec3.r,uvec4.r)+14.)
			+fabs(dot_rv(uvec4.r,uvec1.r)+6.)+fabs(dot_rv(uvec4.r,uvec2.r)+10.)+fabs(dot_rv(uvec4.r,uvec3.r)+14.);
	terror += fabs(error);
	printf("dot_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(m1_dot(uvec1.m1,uvec2.m1)-15.)+fabs(m1_dot(uvec1.m1,uvec3.m1)-21.)+fabs(m1_dot(uvec1.m1,uvec4.m1)+6.)
			+fabs(m1_dot(uvec2.m1,uvec1.m1)-15.)+fabs(m1_dot(uvec2.m1,uvec3.m1)-35.)+fabs(m1_dot(uvec2.m1,uvec4.m1)+10.)
			+fabs(m1_dot(uvec3.m1,uvec1.m1)-21.)+fabs(m1_dot(uvec3.m1,uvec2.m1)-35.)+fabs(m1_dot(uvec3.m1,uvec4.m1)+14.)
			+fabs(m1_dot(uvec4.m1,uvec1.m1)+6.)+fabs(m1_dot(uvec4.m1,uvec2.m1)+10.)+fabs(m1_dot(uvec4.m1,uvec3.m1)+14.);
	terror += fabs(error);
	printf("m1_dot() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: length_cv() length_rv()\n");
	printf("cvect:1           2           3           4       rvect:1           2           3           4\n");
	printf("(%11.5g %11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g %11.5g)\n",length_cv(uvec1.c),length_cv(uvec2.c),length_cv(uvec3.c),length_cv(uvec4.c),length_rv(uvec1.r),length_rv(uvec2.r),length_rv(uvec3.r),length_rv(uvec4.r));
	error = length_cv(uvec1.c)+length_cv(uvec2.c)+length_cv(uvec3.c)+length_cv(uvec4.c) -sqrt(3.)-sqrt(2.)*(3.+5.+7.);
	terror += fabs(error);
	printf("length_cv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = length_rv(uvec1.r)+length_rv(uvec2.r)+length_rv(uvec3.r)+length_rv(uvec4.r) -sqrt(3.)-sqrt(2.)*(3.+5.+7.);
	terror += fabs(error);
	printf("length_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: norm_cv() norm_rv()\n");
	printf("cvector: %11.5g %11.5g %11.5g %11.5g\n",norm_cv(uvec1.c),norm_cv(uvec2.c),norm_cv(uvec3.c),norm_cv(uvec4.c));
	printf("rvector: %11.5g %11.5g %11.5g %11.5g\n",norm_rv(uvec1.r),norm_rv(uvec2.r),norm_rv(uvec3.r),norm_rv(uvec4.r));
	error = fabs(norm_cv(uvec1.c)-3.)+fabs(norm_cv(uvec2.c)-5.)+fabs(norm_cv(uvec3.c)-7.)+fabs(norm_cv(uvec4.c)-1.);
	terror += fabs(error);
	printf("norm_cv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(norm_rv(uvec1.r)-3.)+fabs(norm_rv(uvec2.r)-5.)+fabs(norm_rv(uvec3.r)-7.)+fabs(norm_rv(uvec4.r)-1.);
	terror += fabs(error);
	printf("norm_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: sum_cv() sum_rv()\n");
	printf("cvector: %11.5g %11.5g %11.5g %11.5g\n",sum_cv(uvec1.c),sum_cv(uvec2.c),sum_cv(uvec3.c),sum_cv(uvec4.c));
	printf("rvector: %11.5g %11.5g %11.5g %11.5g\n",sum_rv(uvec1.r),sum_rv(uvec2.r),sum_rv(uvec3.r),sum_rv(uvec4.r));
	error = fabs(sum_cv(uvec1.c)-6.)+fabs(sum_cv(uvec2.c)-10.)+fabs(sum_cv(uvec3.c)-14.)+fabs(sum_cv(uvec4.c)+3.);
	terror += fabs(error);
	printf("sum_cv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(sum_rv(uvec1.r)-6.)+fabs(sum_rv(uvec2.r)-10.)+fabs(sum_rv(uvec3.r)-14.)+fabs(sum_rv(uvec4.r)+3.);
	terror += fabs(error);
	printf("sum_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cv_zero() rv_zero() m1_zero()\n");
	uvec1.c = cv_zero();
	printf("cvector: (%11.5g %11.5g %11.5g) ",uvec1.c.x,uvec1.c.y,uvec1.c.z);
	uvec1.r = rv_zero();
	printf("rvector: (%11.5g %11.5g %11.5g) ",uvec1.r.col[0],uvec1.r.col[1],uvec1.r.col[2]);
	uvec1.m1.cols = 3;
	uvec1.m1 = m1_zero(3);
	printf("matrix1d: (%11.5g %11.5g %11.5g)\n",uvec1.m1.vector[0],uvec1.m1.vector[1],uvec1.m1.vector[2]);
	uvec1.c = cv_zero();
	error = fabs(uvec1.c.x)+fabs(uvec1.c.y)+fabs(uvec1.c.z);
	terror += fabs(error);
	printf("cv_zero() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(uvec1.r.col[0])+fabs(uvec1.r.col[1])+fabs(uvec1.r.col[2]);
	terror += fabs(error);
	printf("rv_zero() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	uvec1.m1 = m1_zero(3);
	error = fabs(uvec1.m1.vector[0])+fabs(uvec1.m1.vector[1])+fabs(uvec1.m1.vector[2]);
	terror += fabs(error);
	printf("m1_zero() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cm_zero() rm_zero() m2_zero()\n");
	dmat1.rows = dmat1.cols = 3;
	dmat1 = m2_zero(3,3);
	rmat1 = rm_zero();
	cmat1 = cm_zero();
	printf("cmatrix: [%3.0f %3.0f %3.0f] rmatrix: [%3.0f %3.0f %3.0f] matrix2d: [%3.0f %3.0f %3.0f]\n",cmat1.r1.x,cmat1.r1.y,cmat1.r1.z,rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2],dmat1.array[0][0],dmat1.array[0][1],dmat1.array[0][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]\n",cmat1.r2.x,cmat1.r2.y,cmat1.r2.z,rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2],dmat1.array[1][0],dmat1.array[1][1],dmat1.array[1][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]\n",cmat1.r3.x,cmat1.r3.y,cmat1.r3.z,rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2],dmat1.array[2][0],dmat1.array[2][1],dmat1.array[2][2]);
	error = fabs(cmat1.r1.x)+fabs(cmat1.r1.y)+fabs(cmat1.r1.z)+fabs(cmat1.r2.x)+fabs(cmat1.r2.y)+fabs(cmat1.r2.z)+fabs(cmat1.r3.x)+fabs(cmat1.r3.y)+fabs(cmat1.r3.z);
	terror += fabs(error);
	printf("cm_zero() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat1.row[0].col[0])+fabs(rmat1.row[0].col[1])+fabs(rmat1.row[0].col[2])+fabs(rmat1.row[1].col[0])+fabs(rmat1.row[1].col[1])+fabs(rmat1.row[1].col[2])+fabs(rmat1.row[2].col[0])+fabs(rmat1.row[2].col[1])+fabs(rmat1.row[2].col[2]);
	terror += fabs(error);
	printf("rm_zero() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dmat1.array[0][0])+fabs(dmat1.array[0][1])+fabs(dmat1.array[0][2])+fabs(dmat1.array[1][0])+fabs(dmat1.array[1][1])+fabs(dmat1.array[1][2])+fabs(dmat1.array[2][0])+fabs(dmat1.array[2][1])+fabs(dmat1.array[2][2]);
	terror += fabs(error);
	printf("m2_zero() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cv_unitx() cv_unity() cv_unitz()\n");
	uvec1.c = cv_unitx();
	printf("x: (%11.5g %11.5g %11.5g)\n",uvec1.c.x,uvec1.c.y,uvec1.c.z);
	uvec2.c = cv_unity();
	printf("y: (%11.5g %11.5g %11.5g)\n",uvec2.c.x,uvec2.c.y,uvec2.c.z);
	uvec3.c = cv_unitz();
	printf("z: (%11.5g %11.5g %11.5g)\n",uvec3.c.x,uvec3.c.y,uvec3.c.z);
	error = uvec1.c.x*(uvec1.c.x+uvec1.c.y+uvec1.c.z)-1.;
	terror += fabs(error);
	printf("cv_unitx() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = uvec2.c.y*(uvec2.c.x+uvec2.c.y+uvec2.c.z)-1.;
	terror += fabs(error);
	printf("cv_unity() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = uvec3.c.z*(uvec3.c.x+uvec3.c.y+uvec3.c.z)-1.;
	terror += fabs(error);
	printf("cv_unitz() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: rv_unitx() rv_unity() rv_unitz()\n");
	uvec1.r = rv_unitx();
	printf("x: (%11.5g %11.5g %11.5g)\n",uvec1.r.col[0],uvec1.r.col[1],uvec1.r.col[2]);
	uvec2.r = rv_unity();
	printf("y: (%11.5g %11.5g %11.5g)\n",uvec2.r.col[0],uvec2.r.col[1],uvec2.r.col[2]);
	uvec3.r = rv_unitz();
	printf("z: (%11.5g %11.5g %11.5g)\n",uvec3.r.col[0],uvec3.r.col[1],uvec3.r.col[2]);
	error = uvec1.r.col[0]*(uvec1.r.col[0]+uvec1.r.col[1]+uvec1.r.col[2])-1.;
	terror += fabs(error);
	printf("rv_unitx() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = uvec2.r.col[1]*(uvec2.r.col[0]+uvec2.r.col[1]+uvec2.r.col[2])-1.;
	terror += fabs(error);
	printf("rv_unity() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = uvec3.r.col[2]*(uvec3.r.col[0]+uvec3.r.col[1]+uvec3.r.col[2])-1.;
	terror += fabs(error);
	printf("v_unitz() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cv_one() rv_one()\n");
	uvec1.c = cv_one();
	printf("cvector: %11.5g %11.5g %11.5g ",uvec1.c.x,uvec1.c.y,uvec1.c.z);
	uvec1.r = rv_one();
	printf("rvector: %11.5g %11.5g %11.5g\n",uvec1.r.col[0],uvec1.r.col[1],uvec1.r.col[2]);
	uvec1.c = cv_one();
	error = fabs(uvec1.c.x-1.)+fabs(uvec1.c.y-1.)+fabs(uvec1.c.z-1.);
	terror += fabs(error);
	printf("cv_one() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	uvec1.r = rv_one();
	error = fabs(uvec1.r.col[0]-1.)+fabs(uvec1.r.col[1]-1.)+fabs(uvec1.r.col[2]-1.);
	terror += fabs(error);
	printf("rv_one() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cv_smult() rv_smult() m1_smult()\n");
	uvec1.c = cv_one();
	uvec1.c = cv_smult(10.,uvec1.c);
	printf("%11.5g %11.5g %11.5g\n",uvec1.c.x,uvec1.c.y,uvec1.c.z);
	error = fabs(uvec1.c.x-10.)+fabs(uvec1.c.y-10.)+fabs(uvec1.c.z-10.);
	terror += fabs(error);
	printf("cv_smult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	uvec1.r = rv_one();
	uvec1.r = rv_smult(10.,uvec1.r);
	printf("%11.5g %11.5g %11.5g\n",uvec1.r.col[0],uvec1.r.col[1],uvec1.r.col[2]);
	error = fabs(uvec1.r.col[0]-10.)+fabs(uvec1.r.col[1]-10.)+fabs(uvec1.r.col[2]-10.);
	terror += fabs(error);
	printf("rv_smult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	uvec1.m1.cols = 3;
	uvec1.m1.vector[0] = uvec1.m1.vector[1] = uvec1.m1.vector[2] = 1.;
	uvec1.m1 = m1_smult(10.,uvec1.m1);
	printf("%11.5g %11.5g %11.5g\n",uvec1.m1.vector[0],uvec1.m1.vector[1],uvec1.m1.vector[2]);
	error = fabs(uvec1.m1.vector[0]-10.)+fabs(uvec1.m1.vector[1]-10.)+fabs(uvec1.m1.vector[2]-10.);
	terror += fabs(error);
	printf("m1_smult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cv_normal() rv_normal()\n");
	uvec1.c = cv_one();
	uvec1.c = cv_smult(10.,uvec1.c);
	uvec2.c = cv_normal(uvec1.c);
	printf("%11.5g %11.5g %11.5g\n",uvec2.c.x,uvec2.c.y,uvec2.c.z);
	error = uvec2.c.x+uvec2.c.y+uvec2.c.z-sqrt(3.);
	terror += fabs(error);
	printf("cv_normal() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	uvec1.r = rv_one();
	uvec1.r = rv_smult(10.,uvec1.r);
	uvec2.r = rv_normal(uvec1.r);
	printf("%11.5g %11.5g %11.5g\n",uvec2.r.col[0],uvec2.r.col[1],uvec2.r.col[2]);
	error = uvec2.r.col[0]+uvec2.r.col[1]+uvec2.r.col[2]-sqrt(3.);
	terror += fabs(error);
	printf("rv_normal() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cv_sadd() rv_sadd()\n");
	uvec1.c.x = 1.; uvec1.c.y = 2.; uvec1.c.z = -3.;
	uvec2.c = cv_sadd(2.,uvec1.c);
	uvec3.r = rv_sadd(2.,uvec1.r);
	printf("cvector: %11.5g %11.5g %11.5g rvector: %11.5g %11.5g %11.5g\n",uvec2.c.x,uvec2.c.y,uvec2.c.z,uvec3.r.col[0],uvec3.r.col[1],uvec3.r.col[2]);
	error = fabs(uvec2.c.x-3.)+fabs(uvec2.c.y-4.)+fabs(uvec2.c.z+1.);
	terror += fabs(error);
	printf("cv_sadd() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(uvec3.r.col[0]-3.)+fabs(uvec3.r.col[1]-4.)+fabs(uvec3.r.col[2]+1.);
	terror += fabs(error);
	printf("rv_sadd() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cv_add() rv_add() m1_add()\n");
	uvec1.c.x = 10.; uvec1.c.y = -12.; uvec1.c.z = 15.;
	uvec2.c.x = 15.; uvec2.c.y = 10.; uvec2.c.z = -12.;
	uvec3.c = cv_add(uvec1.c,uvec2.c);
	uvec4.r = rv_add(uvec1.r,uvec2.r);
	uvec1.m1 = m1_add(uvec1.m1,uvec2.m1);
	printf("cvector: %11.5g %11.5g %11.5g rvector: %11.5g %11.5g %11.5g matrix1d: %11.5g %11.5g %11.5g\n",uvec3.c.x,uvec3.c.y,uvec3.c.z,uvec4.r.col[0],uvec4.r.col[1],uvec4.r.col[2],uvec1.m1.vector[0],uvec1.m1.vector[1],uvec1.m1.vector[2]);
	error = fabs(uvec3.c.x-25.)+fabs(uvec3.c.y+2.)+fabs(uvec3.c.z-3.);
	terror += fabs(error);
	printf("cv_add() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(uvec4.r.col[0]-25.)+fabs(uvec4.r.col[1]+2.)+fabs(uvec4.r.col[2]-3.);
	terror += fabs(error);
	printf("rv_add() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(uvec1.m1.vector[0]-25.)+fabs(uvec1.m1.vector[1]+2.)+fabs(uvec1.m1.vector[2]-3.);
	terror += fabs(error);
	printf("m1_add() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: cv_sub()\n");
	uvec1.c.x = 10.; uvec1.c.y = -12.; uvec1.c.z = 15.;
	uvec2.c.x = 15.; uvec2.c.y = 10.; uvec2.c.z = -12.;
	uvec3.c = cv_sub(uvec1.c,uvec2.c);
	printf("%11.5g %11.5g %11.5g\n",uvec3.c.x,uvec3.c.y,uvec3.c.z);
	error = fabs(uvec3.c.x+5.)+fabs(uvec3.c.y+22.)+fabs(uvec3.c.z-27.);
	terror += fabs(error);
	printf("cv_sub() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: rv_sub()\n");
	uvec1.r.col[0] = 10.; uvec1.r.col[1] = -12.; uvec1.r.col[2] = 15.;
	uvec2.r.col[0] = 15.; uvec2.r.col[1] = 10.; uvec2.r.col[2] = -12.;
	uvec3.r = rv_sub(uvec1.r,uvec2.r);
	printf("%11.5g %11.5g %11.5g\n",uvec3.r.col[0],uvec3.r.col[1],uvec3.r.col[2]);
	error = fabs(uvec3.r.col[0]+5.)+fabs(uvec3.r.col[1]+22.)+fabs(uvec3.r.col[2]-27.);
	terror += fabs(error);
	printf("rv_sub() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: m1_sub()\n");
	uvec1.m1.vector[0] = 10.; uvec1.m1.vector[1] = -12.; uvec1.m1.vector[2] = 15.;
	uvec2.m1.vector[0] = 15.; uvec2.m1.vector[1] = 10.; uvec2.m1.vector[2] = -12.;
	uvec3.m1 = m1_sub(uvec1.m1,uvec2.m1);
	printf("%11.5g %11.5g %11.5g\n",uvec3.m1.vector[0],uvec3.m1.vector[1],uvec3.m1.vector[2]);
	error = fabs(uvec3.m1.vector[0]+5.)+fabs(uvec3.m1.vector[1]+22.)+fabs(uvec3.m1.vector[2]-27.);
	terror += fabs(error);
	printf("m1_sub() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: cv_mult()\n");
	uvec1.c = cv_one();
	uvec1.c = cv_sadd(1.,uvec1.c);
	uvec2.c = cv_one();
	uvec2.c = cv_sadd(2.,uvec2.c);
	uvec3.c = cv_mult(uvec1.c,uvec2.c);
	printf("%11.5g %11.5g %11.5g\n",uvec3.c.x,uvec3.c.y,uvec3.c.z);
	error = fabs(uvec3.c.x-6.)+fabs(uvec3.c.y-6.)+fabs(uvec3.c.z-6.);
	terror += fabs(error);
	printf("cv_mult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: rv_mult()\n");
	uvec1.r = rv_one();
	uvec1.r = rv_sadd(1.,uvec1.r);
	uvec2.r = rv_one();
	uvec2.r = rv_sadd(2.,uvec2.r);
	uvec3.r = rv_mult(uvec1.r,uvec2.r);
	printf("%11.5g %11.5g %11.5g\n",uvec3.r.col[0],uvec3.r.col[1],uvec3.r.col[2]);
	error = fabs(uvec3.r.col[0]-6.)+fabs(uvec3.r.col[1]-6.)+fabs(uvec3.r.col[2]-6.);
	terror += fabs(error);
	printf("rv_mult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: cv_div()\n");
	uvec1.c = cv_one();
	uvec1.c = cv_sadd(1.,uvec1.c);
	uvec2.c = cv_one();
	uvec2.c = cv_sadd(2.,uvec2.c);
	uvec3.c = cv_div(uvec1.c,uvec2.c);
	printf("%11.5g %11.5g %11.5g\n",uvec3.c.x,uvec3.c.y,uvec3.c.z);
	error = uvec3.c.x+uvec3.c.y+uvec3.c.z-2.;
	terror += fabs(error);
	printf("cv_div() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: rv_div()\n");
	uvec1.r = rv_one();
	uvec1.r = rv_sadd(1.,uvec1.r);
	uvec2.r = rv_one();
	uvec2.r = rv_sadd(2.,uvec2.r);
	uvec3.r = rv_div(uvec1.r,uvec2.r);
	printf("%11.5g %11.5g %11.5g\n",uvec3.r.col[0],uvec3.r.col[1],uvec3.r.col[2]);
	error = uvec3.r.col[0]+uvec3.r.col[1]+uvec3.r.col[2]-2.;
	terror += fabs(error);
	printf("rv_div() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: cv_sqrt()\n");
	uvec1.c = cv_smult(4.,cv_one());
	uvec1.c = cv_sqrt(uvec1.c);
	printf("%11.5g %11.5g %11.5g\n",uvec1.c.x,uvec1.c.y,uvec1.c.z);
	error = fabs(uvec1.c.x-2.)+fabs(uvec1.c.y-2.)+fabs(uvec1.c.z-2.);
	terror += fabs(error);
	printf("cv_sqrt() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: rv_sqrt()\n");
	uvec1.r = rv_smult(4.,rv_one());
	uvec1.r = rv_sqrt(uvec1.r);
	printf("%11.5g %11.5g %11.5g\n",uvec1.r.col[0],uvec1.r.col[1],uvec1.r.col[2]);
	error = fabs(uvec1.r.col[0]-2.)+fabs(uvec1.r.col[1]-2.)+fabs(uvec1.r.col[2]-2.);
	terror += fabs(error);
	printf("rv_sqrt() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: cv_cross()\n");
	uvec1.c = cv_cross(cvec1,cvec4);
	uvec2.c = cv_cross(cvec2,cvec4);
	uvec3.c = cv_cross(cvec3,cvec4);
	printf("Tests: 1x4:%11.5g %11.5g %11.5g\n",uvec1.c.x,uvec1.c.y,uvec1.c.z);
	printf("       2x4:%11.5g %11.5g %11.5g\n",uvec2.c.x,uvec2.c.y,uvec2.c.z);
	printf("       3x4:%11.5g %11.5g %11.5g\n",uvec3.c.x,uvec3.c.y,uvec3.c.z);
	error = uvec1.c.x+uvec1.c.y+uvec1.c.z+uvec2.c.x+uvec2.c.y+uvec2.c.z+uvec3.c.x+uvec3.c.y+uvec3.c.z;
	terror += fabs(error);
	printf("cv_cross() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: rv_cross()\n");
	uvec1.r = rv_cross(rvec1,rvec4);
	uvec2.r = rv_cross(rvec2,rvec4);
	uvec3.r = rv_cross(rvec3,rvec4);
	printf("Tests: 1x4:%11.5g %11.5g %11.5g\n",uvec1.r.col[0],uvec1.r.col[1],uvec1.r.col[2]);
	printf("       2x4:%11.5g %11.5g %11.5g\n",uvec2.r.col[0],uvec2.r.col[1],uvec2.r.col[2]);
	printf("       3x4:%11.5g %11.5g %11.5g\n",uvec3.r.col[0],uvec3.r.col[1],uvec3.r.col[2]);
	error = uvec1.r.col[0]+uvec1.r.col[1]+uvec1.r.col[2]+uvec2.r.col[0]+uvec2.r.col[1]+uvec2.r.col[2]+uvec3.r.col[0]+uvec3.r.col[1]+uvec3.r.col[2];
	terror += fabs(error);
	printf("rv_cross() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: m1_cross()\n");
	uvec1.r = rvec1;
	uvec2.r = rvec2;
	uvec3.r = rvec3;
	uvec4.r = rvec4;
	uvec1.m1 = m1_cross(uvec1.m1,uvec4.m1);
	uvec2.m1 = m1_cross(uvec2.m1,uvec4.m1);
	uvec3.m1 = m1_cross(uvec3.m1,uvec4.m1);
	printf("Tests: 1x4:%11.5g %11.5g %11.5g\n",uvec1.m1.vector[0],uvec1.m1.vector[1],uvec1.m1.vector[2]);
	printf("       2x4:%11.5g %11.5g %11.5g\n",uvec2.m1.vector[0],uvec2.m1.vector[1],uvec2.m1.vector[2]);
	printf("       3x4:%11.5g %11.5g %11.5g\n",uvec3.m1.vector[0],uvec3.m1.vector[1],uvec3.m1.vector[2]);
	error = uvec1.m1.vector[0]+uvec1.m1.vector[1]+uvec1.m1.vector[2]+uvec2.m1.vector[0]+uvec2.m1.vector[1]+uvec2.m1.vector[2]+uvec3.m1.vector[0]+uvec3.m1.vector[1]+uvec3.m1.vector[2];
	terror += fabs(error);
	printf("m1_cross() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: cv_diag()\n");
	uvec1.r = rvec1;
	uvec2.r = rvec2;
	uvec3.r = rvec3;
	uvec4.r = rvec4;
	cmat1.r1 = uvec1.c;
	cmat1.r2 = uvec2.c;
	cmat1.r3 = uvec3.c;
	uvec1.c = cv_diag(cmat1);
	printf("%11.5g %11.5g %11.5g\n",uvec1.c.x,uvec1.c.y,uvec1.c.z);
	error = fabs(uvec1.c.x-3.)+fabs(uvec1.c.y-5.)+fabs(uvec1.c.z-7.);
	terror += fabs(error);
	printf("cv_diag() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: rv_diag()\n");
	uvec1.r = rvec1;
	uvec2.r = rvec2;
	uvec3.r = rvec3;
	uvec4.r = rvec4;
	rmat1.row[0] = uvec1.r;
	rmat1.row[1] = uvec2.r;
	rmat1.row[2] = uvec3.r;
	uvec1.r = rv_diag(rmat1);
	printf("%11.5g %11.5g %11.5g\n",uvec1.r.col[0],uvec1.r.col[1],uvec1.r.col[2]);
	error = fabs(uvec1.r.col[0]-3.)+fabs(uvec1.r.col[1]-5.)+fabs(uvec1.r.col[2]-7.);
	terror += fabs(error);
	printf("rv_diag() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: actan()\n");
	error = actan(-1.,1.)-(D2PI-(DPI2/2.));
	terror += fabs(error);
	printf("actan() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: trace_cm() trace_rm() m2_trace()\n");
	rmat1.row[0] = rvec1;
	rmat1.row[1] = rvec2;
	rmat1.row[2] = rvec3;
	cmat1.r1 = cvec1;
	cmat1.r2 = cvec2;
	cmat1.r3 = cvec3;
	dmat1.rows = dmat1.cols = 3;
	dmat1.array[0][0] = cvec1.x; dmat1.array[0][1] = cvec1.y; dmat1.array[0][2] = cvec1.z;
	dmat1.array[1][0] = cvec2.x; dmat1.array[1][1] = cvec2.y; dmat1.array[1][2] = cvec2.z;
	dmat1.array[2][0] = cvec3.x; dmat1.array[2][1] = cvec3.y; dmat1.array[2][2] = cvec3.z;
	error = trace_cm(cmat1)-15.;
	terror += fabs(error);
	printf("trace_cm() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = trace_rm(rmat1)-15.;
	terror += fabs(error);
	printf("trace_rm() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = m2_trace(dmat1)-15.;
	terror += fabs(error);
	printf("m2_trace() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cm_eye() rm_eye() m2_eye()\n");
	printf("\n");
	dmat1.rows = dmat1.cols = 3;
	dmat1 = m2_eye(3);
	rmat1 = rm_eye();
	cmat1 = cm_eye();
	printf("cmatrix: [%3.0f %3.0f %3.0f] rmatrix: [%3.0f %3.0f %3.0f] matrix2d: [%3.0f %3.0f %3.0f]\n",cmat1.r1.x,cmat1.r1.y,cmat1.r1.z,rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2],dmat1.array[0][0],dmat1.array[0][1],dmat1.array[0][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]\n",cmat1.r2.x,cmat1.r2.y,cmat1.r2.z,rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2],dmat1.array[1][0],dmat1.array[1][1],dmat1.array[1][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]\n",cmat1.r3.x,cmat1.r3.y,cmat1.r3.z,rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2],dmat1.array[2][0],dmat1.array[2][1],dmat1.array[2][2]);
	error = fabs(cmat1.r1.x-1.)+fabs(cmat1.r1.y)+fabs(cmat1.r1.z)+fabs(cmat1.r2.x)+fabs(cmat1.r2.y-1.)+fabs(cmat1.r2.z)+fabs(cmat1.r3.x)+fabs(cmat1.r3.y)+fabs(cmat1.r3.z-1.);
	terror += fabs(error);
	printf("cm_eye() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat1.row[0].col[0]-1.)+fabs(rmat1.row[0].col[1])+fabs(rmat1.row[0].col[2])+fabs(rmat1.row[1].col[0])+fabs(rmat1.row[1].col[1]-1.)+fabs(rmat1.row[1].col[2])+fabs(rmat1.row[2].col[0])+fabs(rmat1.row[2].col[1])+fabs(rmat1.row[2].col[2]-1.);
	terror += fabs(error);
	printf("rm_eye() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dmat1.array[0][0]-1.)+fabs(dmat1.array[0][1])+fabs(dmat1.array[0][2])+fabs(dmat1.array[1][0])+fabs(dmat1.array[1][1]-1.)+fabs(dmat1.array[1][2])+fabs(dmat1.array[2][0])+fabs(dmat1.array[2][1])+fabs(dmat1.array[2][2]-1.);
	terror += fabs(error);
	printf("m2_eye() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cv_to_m1() cv_to_m2() m1_to_m2() rm_from_rv() cm_from_rm() rm_from_cm() m2_from_rm() rm_from_m2() cm3x3_to_m2()\n");
	printf("\n");
	dmat4.cols = 3;
	dmat4.rows = 1;
	printf("Correct:          %11.5g %11.5g %11.5g\n",cvec1.x,cvec1.y,cvec1.z);
	printf("\n");
	uvec1.m1 = cv_to_m1(cvec1);
	printf("cv_to_m1():       %11.5g %11.5g %11.5g\n",uvec1.m1.vector[0],uvec1.m1.vector[1],uvec1.m1.vector[2]);
	error = fabs(uvec1.m1.vector[0]-cvec1.x)+fabs(uvec1.m1.vector[1]-cvec1.y)+fabs(uvec1.m1.vector[2]-cvec1.z);
	terror += fabs(error);
	printf("cv_to_m1() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");
	dmat4 = cv_to_m2(cvec1, 1);
	printf("cv_to_m2():   col:%11.5g %11.5g %11.5g\n",dmat4.array[0][0],dmat4.array[1][0],dmat4.array[2][0]);
	error = (fabs(dmat4.array[0][0]-cvec1.x)+fabs(dmat4.array[1][0]-cvec1.y)+fabs(dmat4.array[2][0]-cvec1.z))/2.; //first half of the error
	dmat4.cols = 1;
	dmat4.rows = 3;
	dmat4 = cv_to_m2(cvec1, 0);
	printf("              row:%11.5g %11.5g %11.5g\n",dmat4.array[0][0],dmat4.array[0][1],dmat4.array[0][2]);
	error += (fabs(dmat4.array[0][0]-cvec1.x)+fabs(dmat4.array[0][1]-cvec1.y)+fabs(dmat4.array[0][2]-cvec1.z))/2.; //second half of the error
	terror += fabs(error);
	printf("cv_to_m2() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");
	uvec1.c = cvec1;
	dmat4.cols = 3;
	dmat4.rows = 1;
	dmat4 = m1_to_m2(uvec1.m1, 1);
	printf("m1_to_m2():   col:%11.5g %11.5g %11.5g\n",dmat4.array[0][0],dmat4.array[1][0],dmat4.array[2][0]);
	error = (fabs(dmat4.array[0][0]-uvec1.m1.vector[0])+fabs(dmat4.array[1][0]-uvec1.m1.vector[1])+fabs(dmat4.array[2][0]-uvec1.m1.vector[2]))/2.; //first half of the error
	dmat4.cols = 1;
	dmat4.rows = 3;
	dmat4 = m1_to_m2(uvec1.m1, 0);
	printf("              row:%11.5g %11.5g %11.5g\n",dmat4.array[0][0],dmat4.array[0][1],dmat4.array[0][2]);
	error += (fabs(dmat4.array[0][0]-uvec1.m1.vector[0])+fabs(dmat4.array[0][1]-uvec1.m1.vector[1])+fabs(dmat4.array[0][2]-uvec1.m1.vector[2]))/2.; //second half of the error
	terror += fabs(error);
	printf("m1_to_m2() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");
	uvec1.c = cvec1;
	rmat1 = rm_from_rv(uvec1.r, 1);
	printf("rm_from_rv(): col:%11.5g %11.5g %11.5g\n",rmat1.row[0].col[0],rmat1.row[1].col[0],rmat1.row[2].col[0]);
	error = (fabs(rmat1.row[0].col[0]-uvec1.r.col[0])+fabs(rmat1.row[1].col[0]-uvec1.r.col[1])+fabs(rmat1.row[2].col[0]-uvec1.r.col[2]))/2.; //first half of the error
	rmat1 = rm_from_rv(uvec1.r, 0);
	printf("              row:%11.5g %11.5g %11.5g\n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2]);
	error += (fabs(rmat1.row[0].col[0]-uvec1.r.col[0])+fabs(rmat1.row[0].col[1]-uvec1.r.col[1])+fabs(rmat1.row[0].col[2]-uvec1.r.col[2]))/2.; //second half of the error
	terror += fabs(error);
	printf("rm_from_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");
	rmat1.row[0] = rvec1;
	rmat1.row[1] = rvec2;
	rmat1.row[2] = rvec3;
	printf("Correct:      [%3.0f %3.0f %3.0f]\n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2]);
	printf("              [%3.0f %3.0f %3.0f]\n",rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2]);
	printf("              [%3.0f %3.0f %3.0f]\n",rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2]);
	printf("\n");
	cmat1 = cm_from_rm(rmat1);
	printf("cm_from_rm(): [%3.0f %3.0f %3.0f]\n",cmat1.r1.x,cmat1.r1.y,cmat1.r1.z);
	printf("              [%3.0f %3.0f %3.0f]\n",cmat1.r2.x,cmat1.r2.y,cmat1.r2.z);
	printf("              [%3.0f %3.0f %3.0f]\n",cmat1.r3.x,cmat1.r3.y,cmat1.r3.z);
	error = fabs(cmat1.r1.x-rmat1.row[0].col[0])+fabs(cmat1.r1.y-rmat1.row[0].col[1])+fabs(cmat1.r1.z-rmat1.row[0].col[2])+fabs(cmat1.r2.x-rmat1.row[1].col[0])+fabs(cmat1.r2.y-rmat1.row[1].col[1])+fabs(cmat1.r2.z-rmat1.row[1].col[2])+fabs(cmat1.r3.x-rmat1.row[2].col[0])+fabs(cmat1.r3.y-rmat1.row[2].col[1])+fabs(cmat1.r3.z-rmat1.row[2].col[2]);
	terror += fabs(error);
	printf("cm_from_rm() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");
	cmat1.r1 = cvec1;
	cmat1.r2 = cvec2;
	cmat1.r3 = cvec3;
	rmat1 = rm_from_cm(cmat1);
	printf("rm_from_cm(): [%3.0f %3.0f %3.0f]\n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2]);
	printf("              [%3.0f %3.0f %3.0f]\n",rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2]);
	printf("              [%3.0f %3.0f %3.0f]\n",rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2]);
	error = fabs(cmat1.r1.x-rmat1.row[0].col[0])+fabs(cmat1.r1.y-rmat1.row[0].col[1])+fabs(cmat1.r1.z-rmat1.row[0].col[2])+fabs(cmat1.r2.x-rmat1.row[1].col[0])+fabs(cmat1.r2.y-rmat1.row[1].col[1])+fabs(cmat1.r2.z-rmat1.row[1].col[2])+fabs(cmat1.r3.x-rmat1.row[2].col[0])+fabs(cmat1.r3.y-rmat1.row[2].col[1])+fabs(cmat1.r3.z-rmat1.row[2].col[2]);
	terror += fabs(error);
	printf("rm_from_cm() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");
	rmat1.row[0] = rvec1;
	rmat1.row[1] = rvec2;
	rmat1.row[2] = rvec3;
	dmat4.cols = dmat4.rows = 3;
	dmat4 = m2_from_rm(rmat1);
	printf("m2_from_rm(): [%3.0f %3.0f %3.0f]\n",dmat4.array[0][0],dmat4.array[0][1],dmat4.array[0][2]);
	printf("              [%3.0f %3.0f %3.0f]\n",dmat4.array[1][0],dmat4.array[1][1],dmat4.array[1][2]);
	printf("              [%3.0f %3.0f %3.0f]\n",dmat4.array[2][0],dmat4.array[2][1],dmat4.array[2][2]);
	error = fabs(dmat4.array[0][0]-rmat1.row[0].col[0])+fabs(dmat4.array[0][1]-rmat1.row[0].col[1])+fabs(dmat4.array[0][2]-rmat1.row[0].col[2])+fabs(dmat4.array[1][0]-rmat1.row[1].col[0])+fabs(dmat4.array[1][1]-rmat1.row[1].col[1])+fabs(dmat4.array[1][2]-rmat1.row[1].col[2])+fabs(dmat4.array[2][0]-rmat1.row[2].col[0])+fabs(dmat4.array[2][1]-rmat1.row[2].col[1])+fabs(dmat4.array[2][2]-rmat1.row[2].col[2]);
	terror += fabs(error);
	printf("m2_from_rm() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");
	dmat4.array[0][0] = rmat1.row[0].col[0]; dmat4.array[0][1] = rmat1.row[0].col[1]; dmat4.array[0][2] = rmat1.row[0].col[2];
	dmat4.array[1][0] = rmat1.row[1].col[0]; dmat4.array[1][1] = rmat1.row[1].col[1]; dmat4.array[1][2] = rmat1.row[1].col[2];
	dmat4.array[2][0] = rmat1.row[2].col[0]; dmat4.array[2][1] = rmat1.row[2].col[1]; dmat4.array[2][2] = rmat1.row[2].col[2];
	rmat1 = rm_from_m2(dmat4);
	printf("rm_from_m2(): [%3.0f %3.0f %3.0f]\n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2]);
	printf("              [%3.0f %3.0f %3.0f]\n",rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2]);
	printf("              [%3.0f %3.0f %3.0f]\n",rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2]);
	error = fabs(dmat4.array[0][0]-rmat1.row[0].col[0])+fabs(dmat4.array[0][1]-rmat1.row[0].col[1])+fabs(dmat4.array[0][2]-rmat1.row[0].col[2])+fabs(dmat4.array[1][0]-rmat1.row[1].col[0])+fabs(dmat4.array[1][1]-rmat1.row[1].col[1])+fabs(dmat4.array[1][2]-rmat1.row[1].col[2])+fabs(dmat4.array[2][0]-rmat1.row[2].col[0])+fabs(dmat4.array[2][1]-rmat1.row[2].col[1])+fabs(dmat4.array[2][2]-rmat1.row[2].col[2]);
	terror += fabs(error);
	printf("rm_from_m2() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");
	dmat4 = cm3x3_to_m2(cmat1);
	printf("cm3x3_to_m2():[%3.0f %3.0f %3.0f]\n",dmat4.array[0][0],dmat4.array[0][1],dmat4.array[0][2]);
	printf("              [%3.0f %3.0f %3.0f]\n",dmat4.array[1][0],dmat4.array[1][1],dmat4.array[1][2]);
	printf("              [%3.0f %3.0f %3.0f]\n",dmat4.array[2][0],dmat4.array[2][1],dmat4.array[2][2]);
	error = fabs(dmat4.array[0][0]-cmat1.r1.x)+fabs(dmat4.array[0][1]-cmat1.r1.y)+fabs(dmat4.array[0][2]-cmat1.r1.z)+fabs(dmat4.array[1][0]-cmat1.r2.x)+fabs(dmat4.array[1][1]-cmat1.r2.y)+fabs(dmat4.array[1][2]-cmat1.r2.z)+fabs(dmat4.array[2][0]-cmat1.r3.x)+fabs(dmat4.array[2][1]-cmat1.r3.y)+fabs(dmat4.array[2][2]-cmat1.r3.z);
	terror += fabs(error);
	printf("cm3x3_to_m2() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("\nMatrix Math:\n");
	uvec1.c.x = 10.;
	uvec1.c.y = 11.;
	uvec1.c.z = 12.;

	cmat2.r1.x = 10.;
	cmat2.r1.y = 20.;
	cmat2.r1.z = 30.;
	cmat2.r2 = cv_sadd(1.,cmat2.r1);
	cmat2.r3 = cv_sadd(1.,cmat2.r2);

	cmat1.r1.x = 1.;
	cmat1.r1.y = 2.;
	cmat1.r1.z = 3.;
	cmat1.r2 = cv_sadd(3.,cmat1.r1);
	cmat1.r3 = cv_sadd(3.,cmat1.r2);

	rmat1.row[0].col[0] = cmat1.r1.x;
	rmat1.row[0].col[1] = cmat1.r1.y;
	rmat1.row[0].col[2] = cmat1.r1.z;
	rmat1.row[1].col[0] = cmat1.r2.x;
	rmat1.row[1].col[1] = cmat1.r2.y;
	rmat1.row[1].col[2] = cmat1.r2.z;
	rmat1.row[2].col[0] = cmat1.r3.x;
	rmat1.row[2].col[1] = cmat1.r3.y;
	rmat1.row[2].col[2] = cmat1.r3.z;

	rmat2.row[0].col[0] = cmat2.r1.x;
	rmat2.row[0].col[1] = cmat2.r1.y;
	rmat2.row[0].col[2] = cmat2.r1.z;
	rmat2.row[1].col[0] = cmat2.r2.x;
	rmat2.row[1].col[1] = cmat2.r2.y;
	rmat2.row[1].col[2] = cmat2.r2.z;
	rmat2.row[2].col[0] = cmat2.r3.x;
	rmat2.row[2].col[1] = cmat2.r3.y;
	rmat2.row[2].col[2] = cmat2.r3.z;

	dmat4.cols = 1;
	dmat4.rows = 3;
	dmat4.array[0][0] = uvec1.c.x;
	dmat4.array[1][0] = uvec1.c.y;
	dmat4.array[2][0] = uvec1.c.z;

	dmat1.cols = dmat1.rows = 3;
	dmat1.array[0][0] = cmat1.r1.x;
	dmat1.array[0][1] = cmat1.r1.y;
	dmat1.array[0][2] = cmat1.r1.z;
	dmat1.array[1][0] = cmat1.r2.x;
	dmat1.array[1][1] = cmat1.r2.y;
	dmat1.array[1][2] = cmat1.r2.z;
	dmat1.array[2][0] = cmat1.r3.x;
	dmat1.array[2][1] = cmat1.r3.y;
	dmat1.array[2][2] = cmat1.r3.z;

	dmat2.cols = dmat2.rows = 3;
	dmat2.array[0][0] = cmat2.r1.x;
	dmat2.array[0][1] = cmat2.r1.y;
	dmat2.array[0][2] = cmat2.r1.z;
	dmat2.array[1][0] = cmat2.r2.x;
	dmat2.array[1][1] = cmat2.r2.y;
	dmat2.array[1][2] = cmat2.r2.z;
	dmat2.array[2][0] = cmat2.r3.x;
	dmat2.array[2][1] = cmat2.r3.y;
	dmat2.array[2][2] = cmat2.r3.z;

	dmat5 = m2_mmult(dmat1,dmat4);

	printf("Functions: cv_mmult() rv_mmult() m1_mmult()\n");
	printf("\n");
	printf("[%3.0f %3.0f %3.0f]     [%3.0f]\n",cmat1.r1.x,cmat1.r1.y,cmat1.r1.z,uvec1.c.x);
	printf("[%3.0f %3.0f %3.0f]  X  [%3.0f]\n",cmat1.r2.x,cmat1.r2.y,cmat1.r2.z,uvec1.c.y);
	printf("[%3.0f %3.0f %3.0f]     [%3.0f]\n",cmat1.r3.x,cmat1.r3.y,cmat1.r3.z,uvec1.c.z);
	uvec2.c = cv_mmult(cmat1,uvec1.c);
	uvec3.r = rv_mmult(rmat1,uvec1.r);
	uvec4.m1.cols = uvec1.m1.cols = 3;
	uvec4.m1 = m1_mmult(dmat1,uvec1.m1);
	printf("\ncvector: [%3.0f %3.0f %3.0f] rvector: [%3.0f %3.0f %3.0f]  matrix1d: [%3.0f %3.0f %3.0f]  correct: [ 68 167 266]\n",uvec2.c.x,uvec2.c.y,uvec2.c.z,uvec3.r.col[0],uvec3.r.col[1],uvec3.r.col[2],uvec4.m1.vector[0],uvec4.m1.vector[1],uvec4.m1.vector[2]);
	error = fabs(68.-uvec2.c.x)+fabs(167.-uvec2.c.y)+fabs(266.-uvec2.c.z);
	terror += fabs(error);
	printf("cv_mmult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(68.-uvec3.r.col[0])+fabs(167.-uvec3.r.col[1])+fabs(266.-uvec3.r.col[2]);
	terror += fabs(error);
	printf("rv_mmult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(68.-uvec4.m1.vector[0])+fabs(167.-uvec4.m1.vector[1])+fabs(266.-uvec4.m1.vector[2]);
	terror += fabs(error);
	printf("m1_mmult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cm_mmult() rm_mmult() m2_mmult()\n");
	printf("\n");
	printf("[%3.0f %3.0f %3.0f]     [%3.0f %3.0f %3.0f]\n",cmat1.r1.x,cmat1.r1.y,cmat1.r1.z,cmat2.r1.x,cmat2.r1.y,cmat2.r1.z);
	printf("[%3.0f %3.0f %3.0f]  X  [%3.0f %3.0f %3.0f]\n",cmat1.r2.x,cmat1.r2.y,cmat1.r2.z,cmat2.r2.x,cmat2.r2.y,cmat2.r2.z);
	printf("[%3.0f %3.0f %3.0f]     [%3.0f %3.0f %3.0f]\n",cmat1.r3.x,cmat1.r3.y,cmat1.r3.z,cmat2.r3.x,cmat2.r3.y,cmat2.r3.z);
	cmat3 = cm_mmult(cmat1,cmat2);
	rmat3 = rm_mmult(rmat1, rmat2);
	dmat3 = m2_mmult(dmat1,dmat2);
	printf("\ncmatrix: [%3.0f %3.0f %3.0f]   rmatrix: [%3.0f %3.0f %3.0f] matrix2d: [%3.0f %3.0f %3.0f]  correct: [ 68 128 188]\n",cmat3.r1.x,cmat3.r1.y,cmat3.r1.z,rmat3.row[0].col[0],rmat3.row[0].col[1],rmat3.row[0].col[2],dmat3.array[0][0],dmat3.array[0][1],dmat3.array[0][2]);
	printf("         [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]           [167 317 467]\n",cmat3.r2.x,cmat3.r2.y,cmat3.r2.z,rmat3.row[1].col[0],rmat3.row[1].col[1],rmat3.row[1].col[2],dmat3.array[1][0],dmat3.array[1][1],dmat3.array[1][2]);
	printf("         [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]           [266 506 746]\n",cmat3.r3.x,cmat3.r3.y,cmat3.r3.z,rmat3.row[2].col[0],rmat3.row[2].col[1],rmat3.row[2].col[2],dmat3.array[2][0],dmat3.array[2][1],dmat3.array[2][2]);
	error = fabs(cmat3.r1.x-68.)+fabs(cmat3.r1.y-128.)+fabs(cmat3.r1.z-188.)+fabs(cmat3.r2.x-167.)+fabs(cmat3.r2.y-317.)+fabs(cmat3.r2.z-467.)+fabs(cmat3.r3.x-266.)+fabs(cmat3.r3.y-506.)+fabs(cmat3.r3.z-746.);
	terror += fabs(error);
	printf("cm_mmult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat3.row[0].col[0]-68.)+fabs(rmat3.row[0].col[1]-128.)+fabs(rmat3.row[0].col[2]-188.)+fabs(rmat3.row[1].col[0]-167.)+fabs(rmat3.row[1].col[1]-317.)+fabs(rmat3.row[1].col[2]-467.)+fabs(rmat3.row[2].col[0]-266.)+fabs(rmat3.row[2].col[1]-506.)+fabs(rmat3.row[2].col[2]-746.);
	terror += fabs(error);
	printf("rm_mmult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dmat3.array[0][0]-68.)+fabs(dmat3.array[0][1]-128.)+fabs(dmat3.array[0][2]-188.)+fabs(dmat3.array[1][0]-167.)+fabs(dmat3.array[1][1]-317.)+fabs(dmat3.array[1][2]-467.)+fabs(dmat3.array[2][0]-266.)+fabs(dmat3.array[2][1]-506.)+fabs(dmat3.array[2][2]-746.);
	terror += fabs(error);
	printf("m2_mmult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cm_mult() rm_mult()\n");
	printf("\n");
	printf("[%3.0f %3.0f %3.0f]     [%3.0f %3.0f %3.0f]\n",cmat1.r1.x,cmat1.r1.y,cmat1.r1.z,cmat2.r1.x,cmat2.r1.y,cmat2.r1.z);
	printf("[%3.0f %3.0f %3.0f]  X  [%3.0f %3.0f %3.0f]\n",cmat1.r2.x,cmat1.r2.y,cmat1.r2.z,cmat2.r2.x,cmat2.r2.y,cmat2.r2.z);
	printf("[%3.0f %3.0f %3.0f]     [%3.0f %3.0f %3.0f]\n",cmat1.r3.x,cmat1.r3.y,cmat1.r3.z,cmat2.r3.x,cmat2.r3.y,cmat2.r3.z);
	rmat3 = rm_mult(rmat1, rmat2);
	cmat3 = cm_mult(cmat1, cmat2);
	printf("\ncmatrix: [%3.0f %3.0f %3.0f]   rmatrix: [%3.0f %3.0f %3.0f]  correct: [ 10  40  90]\n",cmat3.r1.x,cmat3.r1.y,cmat3.r1.z,rmat3.row[0].col[0],rmat3.row[0].col[1],rmat3.row[0].col[2]);
	printf("         [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]           [ 44 105 186]\n",cmat3.r2.x,cmat3.r2.y,cmat3.r2.z,rmat3.row[1].col[0],rmat3.row[1].col[1],rmat3.row[1].col[2]);
	printf("         [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]           [ 84 176 288]\n",cmat3.r3.x,cmat3.r3.y,cmat3.r3.z,rmat3.row[2].col[0],rmat3.row[2].col[1],rmat3.row[2].col[2]);
	error = fabs(cmat3.r1.x-10.)+fabs(cmat3.r1.y-40.)+fabs(cmat3.r1.z-90.)+fabs(cmat3.r2.x-44.)+fabs(cmat3.r2.y-105.)+fabs(cmat3.r2.z-186.)+fabs(cmat3.r3.x-84.)+fabs(cmat3.r3.y-176.)+fabs(cmat3.r3.z-288.);
	terror += fabs(error);
	printf("cm_mult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat3.row[0].col[0]-10.)+fabs(rmat3.row[0].col[1]-40.)+fabs(rmat3.row[0].col[2]-90.)+fabs(rmat3.row[1].col[0]-44.)+fabs(rmat3.row[1].col[1]-105.)+fabs(rmat3.row[1].col[2]-186.)+fabs(rmat3.row[2].col[0]-84.)+fabs(rmat3.row[2].col[1]-176.)+fabs(rmat3.row[2].col[2]-288.);
	terror += fabs(error);
	printf("rm_mult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cm_smult() rm_smult() m2_smult()\n");
	printf("\n");
	rmat1.row[0] = rv_one();
	rmat1.row[1] = rv_one();
	rmat1.row[2] = rv_one();
	cmat1.r1 = cv_one();
	cmat1.r2 = cv_one();
	cmat1.r3 = cv_one();
	dmat3.array[0][0] = 1.;
	dmat3.array[0][1] = 1.;
	dmat3.array[0][2] = 1.;
	dmat3.array[1][0] = 1.;
	dmat3.array[1][1] = 1.;
	dmat3.array[1][2] = 1.;
	dmat3.array[2][0] = 1.;
	dmat3.array[2][1] = 1.;
	dmat3.array[2][2] = 1.;
	printf("      [%3.0f %3.0f %3.0f]\n",cmat1.r1.x,cmat1.r1.y,cmat1.r1.z);
	printf("5.0 X [%3.0f %3.0f %3.0f]\n",cmat1.r2.x,cmat1.r2.y,cmat1.r2.z);
	printf("      [%3.0f %3.0f %3.0f]\n",cmat1.r3.x,cmat1.r3.y,cmat1.r3.z);
	cmat1 = cm_smult(5., cmat1);
	rmat1 = rm_smult(5., rmat1);
	dmat3 = m2_smult(5., dmat3);
	printf("\ncmatrix: [%3.0f %3.0f %3.0f] rmatirx: [%3.0f %3.0f %3.0f]  matrix2d: [%3.0f %3.0f %3.0f] correct: [  5   5   5]\n",cmat1.r1.x,cmat1.r1.y,cmat1.r1.z,rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2],dmat3.array[0][0],dmat3.array[0][1],dmat3.array[0][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]          [  5   5   5]\n",cmat1.r2.x,cmat1.r2.y,cmat1.r2.z,rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2],dmat3.array[1][0],dmat3.array[1][1],dmat3.array[1][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]          [  5   5   5]\n",cmat1.r3.x,cmat1.r3.y,cmat1.r3.z,rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2],dmat3.array[2][0],dmat3.array[2][1],dmat3.array[2][2]);
	error = fabs(cmat1.r1.x-5.)+fabs(cmat1.r1.y-5.)+fabs(cmat1.r1.z-5.)+fabs(cmat1.r2.x-5.)+fabs(cmat1.r2.y-5.)+fabs(cmat1.r2.z-5.)+fabs(cmat1.r3.x-5.)+fabs(cmat1.r3.y-5.)+fabs(cmat1.r3.z-5.);
	terror += fabs(error);
	printf("cm_smult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat1.row[0].col[0]-5.)+fabs(rmat1.row[0].col[1]-5.)+fabs(rmat1.row[0].col[2]-5.)+fabs(rmat1.row[1].col[0]-5.)+fabs(rmat1.row[1].col[1]-5.)+fabs(rmat1.row[1].col[2]-5.)+fabs(rmat1.row[2].col[0]-5.)+fabs(rmat1.row[2].col[1]-5.)+fabs(rmat1.row[2].col[2]-5.);
	terror += fabs(error);
	printf("rm_smult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dmat3.array[0][0]-5.)+fabs(dmat3.array[0][1]-5.)+fabs(dmat3.array[0][2]-5.)+fabs(dmat3.array[1][0]-5.)+fabs(dmat3.array[1][1]-5.)+fabs(dmat3.array[1][2]-5.)+fabs(dmat3.array[2][0]-5.)+fabs(dmat3.array[2][1]-5.)+fabs(dmat3.array[2][2]-5.);
	terror += fabs(error);
	printf("m2_smult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cm_add() rm_add() m2_add()\n");
	printf("\n");
	rmat1.row[0] = rvec1;
	rmat1.row[1] = rvec2;
	rmat1.row[2] = rvec3;
	rmat2.row[0] = rvec2;
	rmat2.row[1] = rvec3;
	rmat2.row[2] = rvec4;
	cmat1.r1 = cvec1;
	cmat1.r2 = cvec2;
	cmat1.r3 = cvec3;
	cmat2.r1 = cvec2;
	cmat2.r2 = cvec3;
	cmat2.r3 = cvec4;
	dmat1.array[0][0] = cvec1.x; dmat1.array[0][1] = cvec1.y; dmat1.array[0][2] = cvec1.z;
	dmat1.array[1][0] = cvec2.x; dmat1.array[1][1] = cvec2.y; dmat1.array[1][2] = cvec2.z;
	dmat1.array[2][0] = cvec3.x; dmat1.array[2][1] = cvec3.y; dmat1.array[2][2] = cvec3.z;
	dmat2.array[0][0] = cvec2.x; dmat2.array[0][1] = cvec2.y; dmat2.array[0][2] = cvec2.z;
	dmat2.array[1][0] = cvec3.x; dmat2.array[1][1] = cvec3.y; dmat2.array[1][2] = cvec3.z;
	dmat2.array[2][0] = cvec4.x; dmat2.array[2][1] = cvec4.y; dmat2.array[2][2] = cvec4.z;
	dmat3 = m2_add(dmat1, dmat2);
	rmat3 = rm_add(rmat1, rmat2);
	cmat3 = cm_add(cmat1, cmat2);
	printf("[%3.0f %3.0f %3.0f]   [%3.0f %3.0f %3.0f]\n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2],rmat2.row[0].col[0],rmat2.row[0].col[1],rmat2.row[0].col[2]);
	printf("[%3.0f %3.0f %3.0f] + [%3.0f %3.0f %3.0f]\n",rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2],rmat2.row[1].col[0],rmat2.row[1].col[1],rmat2.row[1].col[2]);
	printf("[%3.0f %3.0f %3.0f]   [%3.0f %3.0f %3.0f]\n",rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2],rmat2.row[2].col[0],rmat2.row[2].col[1],rmat2.row[2].col[2]);
	printf("\ncmatrix: [%3.0f %3.0f %3.0f] rmatirx: [%3.0f %3.0f %3.0f]  matrix2d: [%3.0f %3.0f %3.0f] correct: [  3   8   5]\n",cmat3.r1.x,cmat3.r1.y,cmat3.r1.z,rmat3.row[0].col[0],rmat3.row[0].col[1],rmat3.row[0].col[2],dmat3.array[0][0],dmat3.array[0][1],dmat3.array[0][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]          [  7   5  12]\n",cmat3.r2.x,cmat3.r2.y,cmat3.r2.z,rmat3.row[1].col[0],rmat3.row[1].col[1],rmat3.row[1].col[2],dmat3.array[1][0],dmat3.array[1][1],dmat3.array[1][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]          [  6  -1   6]\n",cmat3.r3.x,cmat3.r3.y,cmat3.r3.z,rmat3.row[2].col[0],rmat3.row[2].col[1],rmat3.row[2].col[2],dmat3.array[2][0],dmat3.array[2][1],dmat3.array[2][2]);
	error = fabs(cmat3.r1.x-3.)+fabs(cmat3.r1.y-8.)+fabs(cmat3.r1.z-5.)+fabs(cmat3.r2.x-7.)+fabs(cmat3.r2.y-5.)+fabs(cmat3.r2.z-12.)+fabs(cmat3.r3.x-6.)+fabs(cmat3.r3.y+1.)+fabs(cmat3.r3.z-6.);
	terror += fabs(error);
	printf("cm_add() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat3.row[0].col[0]-3.)+fabs(rmat3.row[0].col[1]-8.)+fabs(rmat3.row[0].col[2]-5.)+fabs(rmat3.row[1].col[0]-7.)+fabs(rmat3.row[1].col[1]-5.)+fabs(rmat3.row[1].col[2]-12.)+fabs(rmat3.row[2].col[0]-6.)+fabs(rmat3.row[2].col[1]+1.)+fabs(rmat3.row[2].col[2]-6.);
	terror += fabs(error);
	printf("rm_add() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dmat3.array[0][0]-3.)+fabs(dmat3.array[0][1]-8.)+fabs(dmat3.array[0][2]-5.)+fabs(dmat3.array[1][0]-7.)+fabs(dmat3.array[1][1]-5.)+fabs(dmat3.array[1][2]-12.)+fabs(dmat3.array[2][0]-6.)+fabs(dmat3.array[2][1]+1.)+fabs(dmat3.array[2][2]-6.);
	terror += fabs(error);
	printf("m2_add() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cm_sub() rm_sub() m2_sub()\n");
	printf("\n");
	dmat3 = m2_sub(dmat1, dmat2);
	rmat3 = rm_sub(rmat1, rmat2);
	cmat3 = cm_sub(cmat1, cmat2);
	printf("[%3.0f %3.0f %3.0f]   [%3.0f %3.0f %3.0f]\n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2],rmat2.row[0].col[0],rmat2.row[0].col[1],rmat2.row[0].col[2]);
	printf("[%3.0f %3.0f %3.0f] - [%3.0f %3.0f %3.0f]\n",rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2],rmat2.row[1].col[0],rmat2.row[1].col[1],rmat2.row[1].col[2]);
	printf("[%3.0f %3.0f %3.0f]   [%3.0f %3.0f %3.0f]\n",rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2],rmat2.row[2].col[0],rmat2.row[2].col[1],rmat2.row[2].col[2]);
	printf("\ncmatrix: [%3.0f %3.0f %3.0f] rmatirx: [%3.0f %3.0f %3.0f]  matrix2d: [%3.0f %3.0f %3.0f] correct: [  3  -2  -5]\n",cmat3.r1.x,cmat3.r1.y,cmat3.r1.z,rmat3.row[0].col[0],rmat3.row[0].col[1],rmat3.row[0].col[2],dmat3.array[0][0],dmat3.array[0][1],dmat3.array[0][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]          [ -7   5  -2]\n",cmat3.r2.x,cmat3.r2.y,cmat3.r2.z,rmat3.row[1].col[0],rmat3.row[1].col[1],rmat3.row[1].col[2],dmat3.array[1][0],dmat3.array[1][1],dmat3.array[1][2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]          [  8   1   8]\n",cmat3.r3.x,cmat3.r3.y,cmat3.r3.z,rmat3.row[2].col[0],rmat3.row[2].col[1],rmat3.row[2].col[2],dmat3.array[2][0],dmat3.array[2][1],dmat3.array[2][2]);
	error = fabs(cmat3.r1.x-3.)+fabs(cmat3.r1.y+2.)+fabs(cmat3.r1.z+5.)+fabs(cmat3.r2.x+7.)+fabs(cmat3.r2.y-5.)+fabs(cmat3.r2.z+2.)+fabs(cmat3.r3.x-8.)+fabs(cmat3.r3.y-1.)+fabs(cmat3.r3.z-8.);
	terror += fabs(error);
	printf("cm_sub() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat3.row[0].col[0]-3.)+fabs(rmat3.row[0].col[1]+2.)+fabs(rmat3.row[0].col[2]+5.)+fabs(rmat3.row[1].col[0]+7.)+fabs(rmat3.row[1].col[1]-5.)+fabs(rmat3.row[1].col[2]+2.)+fabs(rmat3.row[2].col[0]-8.)+fabs(rmat3.row[2].col[1]-1.)+fabs(rmat3.row[2].col[2]-8.);
	terror += fabs(error);
	printf("rm_sub() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dmat3.array[0][0]-3.)+fabs(dmat3.array[0][1]+2.)+fabs(dmat3.array[0][2]+5.)+fabs(dmat3.array[1][0]+7.)+fabs(dmat3.array[1][1]-5.)+fabs(dmat3.array[1][2]+2.)+fabs(dmat3.array[2][0]-8.)+fabs(dmat3.array[2][1]-1.)+fabs(dmat3.array[2][2]-8.);
	terror += fabs(error);
	printf("m2_sub() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: q_normalize()\n");
	uvec1.c = cv_smult(1.5, qrot1.d); //making uvec1 and un-normalized version of qrot1.
	uvec1.q.w = 1.5*qrot1.w;
	q_normalize(&uvec1.q);
	printf("q_normalize: (%11.10g %11.10g %11.10g %11.10g) correct: (%11.10g %11.10g %11.10g %11.10g)\n",uvec1.q.w,uvec1.q.d.x,uvec1.q.d.y,uvec1.q.d.z,qrot1.w,qrot1.d.x,qrot1.d.y,qrot1.d.z);
	error = (fabs(uvec1.q.w)+fabs(uvec1.q.d.x)+fabs(uvec1.q.d.y)+fabs(uvec1.q.d.z))-(fabs(qrot1.w)+fabs(qrot1.d.x)+fabs(qrot1.d.y)+fabs(qrot1.d.z));
	terror += fabs(error);
	printf("q_normalize() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: q_mult()\n");
	uvec1.q.w = 1.; uvec1.q.d.x = uvec1.q.d.y = uvec1.q.d.z = 0.;
	uvec1.q = q_mult(uvec1.q, qrot2); //90 X :
	uvec1.q = q_mult(uvec1.q, qrot3); //90 Y :
	uvec1.q = q_mult(uvec1.q, qrot4); //90 Z :
	uvec1.q = q_mult(uvec1.q, qrot3); //90 Y : = 180 Z from start
	printf("q_mult: (%11.10g %11.10g %11.10g %11.10g) correct: (          0          0          0          1)\n",uvec1.q.w,uvec1.q.d.x,uvec1.q.d.y,uvec1.q.d.z);
	error = fabs(uvec1.q.w)+fabs(uvec1.q.d.x)+fabs(uvec1.q.d.y)+fabs(uvec1.q.d.z-1.);
	terror += fabs(error);
	printf("q_mult() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: q_conjugate()\n");
	uvec1.q = q_conjugate(qrot1);
	printf("q_conjugate: (%11.10g %11.10g %11.10g %11.10g) correct (%11.10g %11.10g %11.10g %11.10g)\n",uvec1.q.w,uvec1.q.d.x,uvec1.q.d.y,uvec1.q.d.z,qrot1.w,-qrot1.d.x,-qrot1.d.y,-qrot1.d.z);
	error = fabs(uvec1.q.w-qrot1.w)+fabs(uvec1.q.d.x+qrot1.d.x)+fabs(uvec1.q.d.y+qrot1.d.y)+fabs(uvec1.q.d.z+qrot1.d.z);
	terror += error;
	printf("q_conjugate() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: cv_quaternion2axis() rv_quaternion2axis() q_axis2quaternion_cv() q_axis2quaternion_rv()\n");
	uvec1.r.col[0] = uvec1.r.col[1] = sqrt(1./3.)*RADOF(45.); uvec1.r.col[2] = -sqrt(1./3.)*RADOF(45.);
	printf("\tQuaternion to vector:\n");
	uvec2.r = rv_quaternion2axis(qrot1);
	uvec3.c = cv_quaternion2axis(qrot1);
	printf("q to rvector: (%11.5g %11.5g %11.5g) q to cvector: (%11.5g %11.5g %11.5g)\n",uvec2.r.col[0],uvec2.r.col[1],uvec2.r.col[2],uvec3.c.x,uvec3.c.y,uvec3.c.z);
	printf("correct: (%11.5g %11.5g %11.5g)\n",uvec1.c.x,uvec1.c.y,uvec1.c.z);
	error = (fabs(uvec2.r.col[0])+fabs(uvec2.r.col[1])+fabs(uvec2.r.col[2]))-(fabs(uvec1.r.col[0])+fabs(uvec1.r.col[1])+fabs(uvec1.r.col[2]));
	terror += fabs(error);
	printf("rv_quaternion2axis() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = (fabs(uvec3.c.x)+fabs(uvec3.c.y)+fabs(uvec3.c.z))-(fabs(uvec1.r.col[0])+fabs(uvec1.r.col[1])+fabs(uvec1.r.col[2]));
	terror += fabs(error);
	printf("cv_quaternion2axis() Error: %11.5g\tTerror: %11.5g\n",error,terror);

	printf("\tVector to quaternion:\n");
	uvec2.q = q_axis2quaternion_rv(uvec1.r);
	uvec3.q = q_axis2quaternion_cv(uvec1.c);
	printf("rvector to q: (%11.5g %11.5g %11.5g %11.5g) cvector to q: (%11.5g %11.5g %11.5g %11.5g)\n",uvec2.q.w,uvec2.q.d.x,uvec2.q.d.y,uvec2.q.d.z,uvec3.q.w,uvec3.q.d.x,uvec3.q.d.y,uvec3.q.d.z);
	printf("correct: (%11.5g %11.5g %11.5g %11.5g)\n",qrot1.w,qrot1.d.x,qrot1.d.y,qrot1.d.z);
	error = (fabs(uvec2.q.w)+fabs(uvec2.q.d.x)+fabs(uvec2.q.d.y)+fabs(uvec2.q.d.z))-(fabs(qrot1.w)+fabs(qrot1.d.x)+fabs(qrot1.d.y)+fabs(qrot1.d.z));
	terror += fabs(error);
	printf("q_axis2quaternion_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = (fabs(uvec3.q.w)+fabs(uvec3.q.d.x)+fabs(uvec3.q.d.y)+fabs(uvec3.q.d.z))-(fabs(qrot1.w)+fabs(qrot1.d.x)+fabs(qrot1.d.y)+fabs(qrot1.d.z));
	terror += fabs(error);
	printf("q_axis2quaternion_cv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: rotate_q() rotate_q()\n");
	uvec1.r = rotate_q(q_change_around_y(DPI2),rv_unitz());
	error = fabs(uvec1.r.col[0]-1.)+fabs(uvec1.r.col[1])+fabs(uvec1.r.col[2]);
	terror += fabs(error);
	uvec1.r = rotate_q(q_change_around_z(.1),uvec1.r);
	error = fabs(uvec1.r.col[0]-cos(.1))+fabs(uvec1.r.col[1]-sin(.1))+fabs(uvec1.r.col[2]);
	terror += fabs(error);
	uvec1.c = rotate_q(qrot2,cvec1); //+90X
	uvec1.c = rotate_q(qrot4,uvec1.c); //+90Z
	uvec1.c = rotate_q(qrot5,uvec1.c); //-90Y
	uvec2.r = rotate_q(qrot2,rvec1); //+90X
	uvec2.r = rotate_q(qrot4,uvec2.r); //+90Z
	uvec2.r = rotate_q(qrot5,uvec2.r); //-90Y, equivilant (for these vectors) of 180Y
	printf("cvector: (%11.5g %11.5g %11.5g) rvector: (%11.5g %11.5g %11.5g) correct: (%11.5g %11.5g %11.5g)\n",uvec1.c.x,uvec1.c.y,uvec1.c.z,uvec2.r.col[0],uvec2.r.col[1],uvec2.r.col[2],-cvec1.x,cvec1.y,cvec1.z);
	error = fabs(uvec1.c.x+cvec1.x)+fabs(uvec1.c.y-cvec1.y)+fabs(uvec1.c.z-cvec1.z);
	terror += fabs(error);
	printf("rotate_q() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(uvec2.r.col[0]+rvec1.col[0])+fabs(uvec2.r.col[1]-rvec1.col[1])+fabs(uvec2.r.col[2]-rvec1.col[2]);
	terror += fabs(error);
	printf("rotate_q() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	uvec1.c = rotate_q(qrot2,cvec1); //+90X
	uvec1.c = rotate_q(qrot4,uvec1.c); //+90Z
	uvec1.c = rotate_q(qrot5,uvec1.c); //-90Y
	uvec2.r = rotate_q(qrot2,rvec1); //+90X
	uvec2.r = rotate_q(qrot4,uvec2.r); //+90Z
	uvec2.r = rotate_q(qrot5,uvec2.r); //-90Y, equivilant (for these vectors) of 180Y
	printf("cvector: (%11.5g %11.5g %11.5g) rvector: (%11.5g %11.5g %11.5g) correct: (%11.5g %11.5g %11.5g)\n",uvec1.c.x,uvec1.c.y,uvec1.c.z,uvec2.r.col[0],uvec2.r.col[1],uvec2.r.col[2],-cvec1.x,cvec1.y,cvec1.z);


	printf("Functions: q_change_between_cv() q_change_between_rv()\n");
	uvec1.r = rv_zero(); //Learned this lesson the hard way... (last 3 elements of rvectors must be zeroed or they will mess up rv_length(), which will mess up normalize_rv() which will mess up q_change_between_rv())
	uvec1.c.x = uvec1.c.y = 1.; uvec1.c.z = -1.; //An axis of (1, 1, -1) (sound familiar?)
	uvec2.q = q_change_around_cv(uvec1.c,RADOF(45.)); //A rotation of +45
	uvec3.q = q_change_around_rv(uvec1.r,RADOF(45.));
	printf("cvector to q: (%11.5g %11.5g %11.5g %11.5g) rvector to q: (%11.5g %11.5g %11.5g %11.5g) correct: (%11.5g %11.5g %11.5g %11.5g)\n",uvec2.q.w,uvec2.q.d.x,uvec2.q.d.y,uvec2.q.d.z,uvec3.q.w,uvec3.q.d.x,uvec3.q.d.y,uvec3.q.d.z,qrot1.w,qrot1.d.x,qrot1.d.y,qrot1.d.z);
	error = fabs(uvec2.q.w-qrot1.w)+fabs(uvec2.q.d.x-qrot1.d.x)+fabs(uvec2.q.d.y-qrot1.d.y)+fabs(uvec2.q.d.z-qrot1.d.z);
	terror += error;
	printf("q_change_between_cv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(uvec3.q.w-qrot1.w)+fabs(uvec3.q.d.x-qrot1.d.x)+fabs(uvec3.q.d.y-qrot1.d.y)+fabs(uvec3.q.d.z-qrot1.d.z);
	terror += error;
	printf("q_change_between_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: q_change_between_cv() q_change_between_rv()\n");
	uvec1.r = rv_zero(); uvec1.r.col[0] = 1.; //Vector (1, 0, 0)
	uvec2.r = rv_zero(); uvec2.c.y = uvec2.c.z = 1.; //Vector (0, 1, 1)
	uvec5.r = rv_cross(uvec1.r, uvec2.r);
	normalize_cv(&uvec5.c);
	uvec5.q.w = cos(RADOF(45.)); uvec5.q.d = cv_smult(sin(RADOF(45.)),uvec5.q.d); //The quaternion we expect to get from the functions.
	uvec3.q = q_change_between_cv(uvec1.c, uvec2.c);
	uvec4.q = q_change_between_rv(uvec1.r, uvec2.r);
	printf("from cvector: (%11.5g %11.5g %11.5g %11.5g) from rvector: (%11.5g %11.5g %11.5g %11.5g) correct: (%11.5g %11.5g %11.5g %11.5g)\n",uvec3.q.w,uvec3.q.d.x,uvec3.q.d.y,uvec3.q.d.z,uvec4.q.w,uvec4.q.d.x,uvec4.q.d.y,uvec4.q.d.z,uvec5.q.w,uvec5.q.d.x,uvec5.q.d.y,uvec5.q.d.z);
	error = fabs(uvec3.q.w-uvec5.q.w)+fabs(uvec3.q.d.x-uvec5.q.d.x)+fabs(uvec3.q.d.y-uvec5.q.d.y)+fabs(uvec3.q.d.z-uvec5.q.d.z);
	terror += error;
	printf("q_change_between_cv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(uvec4.q.w-uvec5.q.w)+fabs(uvec4.q.d.x-uvec5.q.d.x)+fabs(uvec4.q.d.y-uvec5.q.d.y)+fabs(uvec4.q.d.z-uvec5.q.d.z);
	terror += error;
	printf("q_change_between_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: q_euler2quaternion() a_quaternion2euler()\n");
	//Angle vector:
	avec1.h = RADOF(30.); avec1.e = RADOF(60.); avec1.b = RADOF(90.); //30Z*60Y*90X
	//Corresponding quaternion:
	uvec2.q.w = cos(RADOF(15.)); uvec2.q.d.x = uvec2.q.d.y = 0.; uvec2.q.d.z = sin(RADOF(15.)); //30Z
	uvec3.q.w = cos(RADOF(30.)); uvec3.q.d.x = uvec3.q.d.z = 0.; uvec3.q.d.y = sin(RADOF(30.)); //60Y
	uvec2.q = q_mult(uvec2.q, uvec3.q); //30Z*60Y
	uvec2.q = q_mult(uvec2.q, qrot2); //...      *90X

	uvec1.q = q_euler2quaternion(avec1);
	printf("from avector: (%11.5g %11.5g %11.5g %11.5g) correct: (%11.5g %11.5g %11.5g %11.5g)\n",uvec1.q.w,uvec1.q.d.x,uvec1.q.d.y,uvec1.q.d.z,uvec2.q.w,uvec2.q.d.x,uvec2.q.d.y,uvec2.q.d.z);
	error = fabs(uvec1.q.w-uvec2.q.w)+fabs(uvec1.q.d.x-uvec2.q.d.x)+fabs(uvec1.q.d.y-uvec2.q.d.y)+fabs(uvec1.q.d.z-uvec2.q.d.z);
	terror += error;
	printf("q_euler2quaternion() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	avec2 = a_quaternion2euler(uvec2.q);
	printf("from quaternion: (%11.5g %11.5g %11.5g) correct: (%11.5g %11.5g %11.5g)\n",avec2.h,avec2.e,avec2.b,avec1.h,avec1.e,avec1.b);
	error = fabs(avec2.h-avec1.h)+fabs(avec2.e-avec1.e)+fabs(avec2.b-avec1.b);
	terror += error;
	printf("a_quaternion2euler() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: q_dcm2quaternion_cm() q_dcm2quaternion_rm() cm_quaternion2dcm() rm_quaternion2dcm()\n");
	//rotation cmatrix(same rotation as qrot1):
	cmat1.r1.x = cmat1.r2.y = cmat1.r3.z = (1.+sqrt(2.))/3.;
	cmat1.r1.y = (sqrt(2.)+sqrt(3.)-1.)/(3.*sqrt(2.));
	cmat1.r2.z = cmat1.r3.x = -((sqrt(2.)+sqrt(3.)-1.)/(3.*sqrt(2.)));
	cmat1.r2.x = (sqrt(2.)-sqrt(3.)-1.)/(3.*sqrt(2.));
	cmat1.r1.z = cmat1.r3.y = -((sqrt(2.)-sqrt(3.)-1.)/(3.*sqrt(2.)));
	//rotation rmatrix(from cmatrix):
	rmat1.row[0].col[0] = rmat1.row[1].col[1] = rmat1.row[2].col[2] = cmat1.r3.z;
	rmat1.row[0].col[1] = cmat1.r1.y;
	rmat1.row[1].col[2] = rmat1.row[2].col[0] =  cmat1.r3.x;
	rmat1.row[1].col[0] = cmat1.r2.x;
	rmat1.row[0].col[2] = rmat1.row[2].col[1] = cmat1.r3.y;
	printf("\nfrom cm:                                          from rm:                                          correct:\n");
	uvec1.q = q_dcm2quaternion_cm(cmat1);
	uvec2.q = q_dcm2quaternion_rm(rmat1);
	printf("(%11.5g %11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g %11.5g) (%11.5g %11.5g %11.5g %11.5g)\n",uvec1.q.w,uvec1.q.d.x,uvec1.q.d.y,uvec1.q.d.z,uvec2.q.w,uvec2.q.d.x,uvec2.q.d.y,uvec2.q.d.z,qrot1.w,qrot1.d.x,qrot1.d.y,qrot1.d.z);
	error = fabs(uvec1.q.w-qrot1.w)+fabs(uvec1.q.d.x-qrot1.d.x)+fabs(uvec1.q.d.y-qrot1.d.y)+fabs(uvec1.q.d.z-qrot1.d.z);
	terror += fabs(error);
	printf("q_dcm2quaternion_cm Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(uvec2.q.w-qrot1.w)+fabs(uvec2.q.d.x-qrot1.d.x)+fabs(uvec2.q.d.y-qrot1.d.y)+fabs(uvec2.q.d.z-qrot1.d.z);
	terror += fabs(error);
	printf("q_dcm2quaternion_rm Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\ncmatrix:                     rmatrix:                     correct:\n");
	cmat2 = cm_quaternion2dcm(qrot1);
	rmat2 = rm_quaternion2dcm(qrot1);
	printf("[%8.5g %8.5g %8.5g] [%8.5g %8.5g %8.5g] [%8.5g %8.5g %8.5g]\n",cmat2.r1.x,cmat2.r1.y,cmat2.r1.z,rmat2.row[0].col[0],rmat2.row[0].col[1],rmat2.row[0].col[2],cmat1.r1.x,cmat1.r1.y,cmat1.r1.z);
	printf("[%8.5g %8.5g %8.5g] [%8.5g %8.5g %8.5g] [%8.5g %8.5g %8.5g]\n",cmat2.r2.x,cmat2.r2.y,cmat2.r2.z,rmat2.row[1].col[0],rmat2.row[1].col[1],rmat2.row[1].col[2],cmat1.r2.x,cmat1.r2.y,cmat1.r2.z);
	printf("[%8.5g %8.5g %8.5g] [%8.5g %8.5g %8.5g] [%8.5g %8.5g %8.5g]\n",cmat2.r3.x,cmat2.r3.y,cmat2.r3.z,rmat2.row[2].col[0],rmat2.row[2].col[1],rmat2.row[2].col[2],cmat1.r3.x,cmat1.r3.y,cmat1.r3.z);
	error = fabs(cmat2.r1.x-cmat1.r1.x)+fabs(cmat2.r1.y-cmat1.r1.y)+fabs(cmat2.r1.z-cmat1.r1.z)+fabs(cmat2.r2.x-cmat1.r2.x)+fabs(cmat2.r2.y-cmat1.r2.y)+fabs(cmat2.r2.z-cmat1.r2.z)+fabs(cmat2.r3.x-cmat1.r3.x)+fabs(cmat2.r3.y-cmat1.r3.y)+fabs(cmat2.r3.z-cmat1.r3.z);
	terror += fabs(error);
	printf("cm_quaternion2dcm Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat2.row[0].col[0]-cmat1.r1.x)+fabs(rmat2.row[0].col[1]-cmat1.r1.y)+fabs(rmat2.row[0].col[2]-cmat1.r1.z)+fabs(rmat2.row[1].col[0]-cmat1.r2.x)+fabs(rmat2.row[1].col[1]-cmat1.r2.y)+fabs(rmat2.row[1].col[2]-cmat1.r2.z)+fabs(rmat2.row[2].col[0]-cmat1.r3.x)+fabs(rmat2.row[2].col[1]-cmat1.r3.y)+fabs(rmat2.row[2].col[2]-cmat1.r3.z);
	terror += fabs(error);
	printf("rm_quaternion2dcm Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: rm_transpose() m2_transpose() cm_transpose()\n");
	printf("\n");
	rmat2.row[0] = rvec1; //3.,3.,0.,0.,0.,0. 	> 3.,0.,-1,0.,0.,0.
	rmat2.row[1] = rvec2; //0.,5.,5.,0.,0.,0. 	> 3.,5.,-1,0.,0.,0.
	rmat2.row[2] = rvec4; //-1.,-1.,-1.,0.,0.,0.	> 0.,5.,-1,0.,0.,0.
	rmat3 = rm_transpose(rmat2);
	dmat1.array[0][0] = 3.;
	dmat1.array[0][1] = 3.;
	dmat1.array[0][2] = 0.;
	dmat1.array[1][0] = 0.;
	dmat1.array[1][1] = 5.;
	dmat1.array[1][2] = 5.;
	dmat1.array[2][0] = -1.;
	dmat1.array[2][1] = -1.;
	dmat1.array[2][2] = -1.;
	dmat2 = m2_transpose(dmat1);
	cmat1.r1 = cvec1;
	cmat1.r2 = cvec2;
	cmat1.r3 = cvec4;
	cmat2 = cm_transpose(cmat1);
	printf("rmatrix: [%3.0f %3.0f %3.0f] matrix2d: [%3.0f %3.0f %3.0f] cmatrix: [%3.0f %3.0f %3.0f]\n",rmat2.row[0].col[0],rmat2.row[0].col[1],rmat2.row[0].col[2], dmat1.array[0][0],dmat1.array[0][1],dmat1.array[0][2], cmat1.r1.x,cmat1.r1.y,cmat1.r1.z);
	printf("	 [%3.0f %3.0f %3.0f] 	         [%3.0f %3.0f %3.0f]	        [%3.0f %3.0f %3.0f]\n",rmat2.row[1].col[0],rmat2.row[1].col[1],rmat2.row[1].col[2], dmat1.array[1][0],dmat1.array[1][1],dmat1.array[1][2], cmat1.r2.x,cmat1.r2.y,cmat1.r2.z);
	printf("	 [%3.0f %3.0f %3.0f] 	         [%3.0f %3.0f %3.0f]	        [%3.0f %3.0f %3.0f]\n",rmat2.row[2].col[0],rmat2.row[2].col[1],rmat2.row[2].col[2], dmat1.array[2][0],dmat1.array[2][1],dmat1.array[2][2], cmat1.r3.x,cmat1.r3.y,cmat1.r3.z);
	printf("\n");
	printf("Transposed:\n");
	printf("rmatrix: [%3.0f %3.0f %3.0f] matrix2d: [%3.0f %3.0f %3.0f] cmatrix: [%3.0f %3.0f %3.0f] Actual: [  3   0  -1]\n",rmat3.row[0].col[0],rmat3.row[0].col[1],rmat3.row[0].col[2], dmat2.array[0][0],dmat2.array[0][1],dmat2.array[0][2], cmat2.r1.x,cmat2.r1.y,cmat2.r1.z);
	printf("	 [%3.0f %3.0f %3.0f] 	         [%3.0f %3.0f %3.0f]	        [%3.0f %3.0f %3.0f]	      [  3   5  -1]\n",rmat3.row[1].col[0],rmat3.row[1].col[1],rmat3.row[1].col[2], dmat2.array[1][0],dmat2.array[1][1],dmat2.array[1][2], cmat2.r2.x,cmat2.r2.y,cmat2.r2.z);
	printf("	 [%3.0f %3.0f %3.0f] 	         [%3.0f %3.0f %3.0f]	        [%3.0f %3.0f %3.0f]	      [  0   5  -1]\n",rmat3.row[2].col[0],rmat3.row[2].col[1],rmat3.row[2].col[2], dmat2.array[2][0],dmat2.array[2][1],dmat2.array[2][2], cmat2.r3.x,cmat2.r3.y,cmat2.r3.z);
	error = fabs(rmat3.row[0].col[0]-3.) + fabs(rmat3.row[0].col[1]) + fabs(rmat3.row[0].col[2]+1.) + fabs(rmat3.row[1].col[0]-3.) + fabs(rmat3.row[1].col[1]-5.) + fabs(rmat3.row[1].col[2]+1.)+fabs(rmat3.row[2].col[0]) + fabs(rmat3.row[2].col[1]-5.) + fabs(rmat3.row[2].col[2]+1.);
	terror += fabs(error);
	printf("rm_transpose() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dmat2.array[0][0]-3.) + fabs(dmat2.array[0][1]) + fabs(dmat2.array[0][2]+1.) + fabs(dmat2.array[1][0]-3.) + fabs(dmat2.array[1][1]-5.) + fabs(dmat2.array[1][2]+1.) + fabs(dmat2.array[2][0]) + fabs(dmat2.array[2][1]-5.) + fabs(dmat2.array[2][2]+1.);
	terror += fabs(error);
	printf("m2_transpose() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(cmat2.r1.x-3.) + fabs(cmat2.r1.y) + fabs(cmat2.r1.z+1.) + fabs(cmat2.r2.x-3.) + fabs(cmat2.r2.y-5.) + fabs(cmat2.r2.z+1.) + fabs(cmat2.r3.x) + fabs(cmat2.r3.y-5.) + fabs(cmat2.r3.z+1.);
	terror += fabs(error);
	printf("cm_transpose() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");


	printf("Functions: determinant_rm() m2_determinant()\n");
	printf("\n");
	dmat1.array[0][0] = 3.;
	dmat1.array[0][1] = 3.;
	dmat1.array[0][2] = 0.;
	dmat1.array[1][0] = 0.;
	dmat1.array[1][1] = 5.;
	dmat1.array[1][2] = 5.;
	dmat1.array[2][0] = -1.;
	dmat1.array[2][1] = -1.;
	dmat1.array[2][2] = -1.;
	double dmatdet = m2_determinant(dmat1);
	rmat2.row[0] = rvec1; //3.,3.,0.,0.,0.,0.
	rmat2.row[1] = rvec2; //0.,5.,5.,0.,0.,0.
	rmat2.row[2] = rvec4; //-1.,-1.,-1.,0.,0.,0.
	double rmdet  = determinant_rm(rmat2);
	printf("rmatrix: [%3.0f %3.0f %3.0f] matrix2d: [%3.0f %3.0f %3.0f]\n",rmat2.row[0].col[0],rmat2.row[0].col[1],rmat2.row[0].col[2], dmat1.array[0][0],dmat1.array[0][1],dmat1.array[0][2]);
	printf("         [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]\n",rmat2.row[1].col[0],rmat2.row[1].col[1],rmat2.row[1].col[2], dmat1.array[1][0],dmat1.array[1][1],dmat1.array[1][2]);
	printf("         [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]\n",rmat2.row[2].col[0],rmat2.row[2].col[1],rmat2.row[2].col[2], dmat1.array[2][0],dmat1.array[2][1],dmat1.array[2][2]);
	printf("Determinants - rmatrix: %3.0f	matrix2d: %3.0f  Actual: -15\n", rmdet, dmatdet);
	error = fabs(-15. - rmdet);
	terror += fabs(error);
	printf("determinant_rm() Error: %11.5g\tTerror: %11.5g\n", error, terror);
	error = fabs(-15. - dmatdet);
	terror += fabs(error);
	printf("m2_determinant() Error: %11.5g\tTerror: %11.5g\n", error, terror);
	printf("\n");


	printf("Functions: rm_inverse() m2_inverse()\n");
	printf("\n");
	//Inverse[{{1, 1, 2}, {-1, 1, 1}, {-2, 3, 3}}] -> {{0, -3, 1}, {-1, -7, 3}, {1, 5, -2}}
	rmat2.row[0].col[0] = 1.;
	rmat2.row[0].col[1] = 1.;
	rmat2.row[0].col[2] = 2.;
	rmat2.row[1].col[0] = -1.;
	rmat2.row[1].col[1] = 1.;
	rmat2.row[1].col[2] = 1.;
	rmat2.row[2].col[0] = -2.;
	rmat2.row[2].col[1] = 3.;
	rmat2.row[2].col[2] = 3.;
	dmat1.array[0][0] = 1.;
	dmat1.array[0][1] = 1.;
	dmat1.array[0][2] = 2.;
	dmat1.array[1][0] = -1.;
	dmat1.array[1][1] = 1.;
	dmat1.array[1][2] = 1.;
	dmat1.array[2][0] = -2.;
	dmat1.array[2][1] = 3.;
	dmat1.array[2][2] = 3.;
	printf("Initial: [  1   1   2]\n");
	printf("         [ -1   1   1]\n");
	printf("         [ -2   3   3]\n");
	printf("\n");
	dmat2 = m2_inverse(dmat1);
	rmat3 = rm_inverse(rmat2);
	printf("Inverse:\n");
	printf("rmatrix: [%3.0f %3.0f %3.0f] matrix2d: [%3.0f %3.0f %3.0f] Actual: [  0  -3   1]\n",rmat3.row[0].col[0],rmat3.row[0].col[1],rmat3.row[0].col[2], dmat2.array[0][0],dmat2.array[0][1],dmat2.array[0][2]);
	printf("         [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f] 	       [ -1  -7   3]\n",rmat3.row[1].col[0],rmat3.row[1].col[1],rmat3.row[1].col[2], dmat2.array[1][0],dmat2.array[1][1],dmat2.array[1][2]);
	printf("         [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f] 	       [  1   5  -2]\n",rmat3.row[2].col[0],rmat3.row[2].col[1],rmat3.row[2].col[2], dmat2.array[2][0],dmat2.array[2][1],dmat2.array[2][2]);
	error = fabs(rmat3.row[0].col[0]) + fabs(rmat3.row[0].col[1]+3.) + fabs(rmat3.row[0].col[2]-1.) + fabs(rmat3.row[1].col[0]+1.) + fabs(rmat3.row[1].col[1]+7.) + fabs(rmat3.row[1].col[2]-3.)+fabs(rmat3.row[2].col[0]-1.) + fabs(rmat3.row[2].col[1]-5.) + fabs(rmat3.row[2].col[2]+2.);
	terror += fabs(error);
	printf("rm_inverse() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dmat2.array[0][0]) + fabs(dmat2.array[0][1]+3.) + fabs(dmat2.array[0][2]-1.) + fabs(dmat2.array[1][0]+1.) + fabs(dmat2.array[1][1]+7.) + fabs(dmat2.array[1][2]-3.) + fabs(dmat2.array[2][0]-1.) + fabs(dmat2.array[2][1]-5.) + fabs(dmat2.array[2][2]+2.);
	terror += fabs(error);
	printf("m2_inverse() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: rm_diag() m2_diag() cm_diag()\n");
	printf("\n");
	printf("Diagonal: {2, -2,  2}\n");
	rvec5.col[0] = 2.; rvec5.col[1] = -2.; rvec5.col[2] = 2.;
	rmat3 = rm_diag(rvec5);
	dvec1.vector[0] = 2.; dvec1.vector[1] = -2.; dvec1.vector[2] = 2.; dvec1.vector[3] = 0.;
	dvec1.cols = 3;
	dmat2 = m2_diag(dvec1);
	cvec5.x = 2.; cvec5.y = -2.; cvec5.z = 2.;
	cmat2 = cm_diag(cvec5);
	printf("rmatrix: [%3.0f %3.0f %3.0f] matrix2d: [%3.0f %3.0f %3.0f] cmatrix: [%3.0f %3.0f %3.0f] Actual: [  2   0   0]\n",rmat3.row[0].col[0],rmat3.row[0].col[1],rmat3.row[0].col[2], dmat2.array[0][0],dmat2.array[0][1],dmat2.array[0][2], cmat2.r1.x,cmat2.r1.y,cmat2.r1.z);
	printf("	 [%3.0f %3.0f %3.0f] 	         [%3.0f %3.0f %3.0f]	        [%3.0f %3.0f %3.0f]	      [  0  -2   0]\n",rmat3.row[1].col[0],rmat3.row[1].col[1],rmat3.row[1].col[2], dmat2.array[1][0],dmat2.array[1][1],dmat2.array[1][2], cmat2.r2.x,cmat2.r2.y,cmat2.r2.z);
	printf("	 [%3.0f %3.0f %3.0f] 	         [%3.0f %3.0f %3.0f]	        [%3.0f %3.0f %3.0f]	      [  0   0   2]\n",rmat3.row[2].col[0],rmat3.row[2].col[1],rmat3.row[2].col[2], dmat2.array[2][0],dmat2.array[2][1],dmat2.array[2][2], cmat2.r3.x,cmat2.r3.y,cmat2.r3.z);
	error = fabs(rmat3.row[0].col[0]-2.) + fabs(rmat3.row[0].col[1]) + fabs(rmat3.row[0].col[2]) + fabs(rmat3.row[1].col[0]) + fabs(rmat3.row[1].col[1]+2.) + fabs(rmat3.row[1].col[2])+fabs(rmat3.row[2].col[0]) + fabs(rmat3.row[2].col[1]) + fabs(rmat3.row[2].col[2]-2.);
	terror += fabs(error);
	printf("rm_diag() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dmat2.array[0][0]-2.) + fabs(dmat2.array[0][1]) + fabs(dmat2.array[0][2]) + fabs(dmat2.array[1][0]) + fabs(dmat2.array[1][1]+2.) + fabs(dmat2.array[1][2]) + fabs(dmat2.array[2][0]) + fabs(dmat2.array[2][1]) + fabs(dmat2.array[2][2]-2.);
	terror += fabs(error);
	printf("m2_diag() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(cmat2.r1.x-2.) + fabs(cmat2.r1.y) + fabs(cmat2.r1.z) + fabs(cmat2.r2.x) + fabs(cmat2.r2.y+2.) + fabs(cmat2.r2.z) + fabs(cmat2.r3.x) + fabs(cmat2.r3.y) + fabs(cmat2.r3.z-2.);
	terror += fabs(error);
	printf("cm_diag() Error: %11.5g\tTerror: %11.5g\n",error,terror);

	printf("\n");
	printf("Functions: m2_skew() rm_skew() m2_unskew() rv_unskew()\n");
	printf("\n");
	/*Vector: { 1  -3   4 }*/
	dvec1.cols = 3;
	dvec1.vector[0] = 1.; dvec1.vector[1] = -3.; dvec1.vector[2] = 4.; dvec1.vector[3] = 0.;
	rvec5.col[0] = 1.;
	rvec5.col[1] = -3.;
	rvec5.col[2] = 4;
	dmat1 = m2_skew(dvec1);
	rmat2 = rm_skew(rvec5);
	printf("m2skew: [%3.0f %3.0f %3.0f] rmskew: [%3.0f %3.0f %3.0f] correct: [  0   -4   -3]\n",dmat1.array[0][0],dmat1.array[0][1],dmat1.array[0][2],rmat2.row[0].col[0],rmat2.row[0].col[1],rmat2.row[0].col[2]);
	printf("        [%3.0f %3.0f %3.0f]         [%3.0f %3.0f %3.0f]          [  4    0   -1]\n",dmat1.array[1][0],dmat1.array[1][1],dmat1.array[1][2],rmat2.row[1].col[0],rmat2.row[1].col[1],rmat2.row[1].col[2]);
	printf("        [%3.0f %3.0f %3.0f]         [%3.0f %3.0f %3.0f]          [  3    1    0]\n",dmat1.array[2][0],dmat1.array[2][1],dmat1.array[2][2],rmat2.row[2].col[0],rmat2.row[2].col[1],rmat2.row[2].col[2]);
	dvec1 = m2_unskew(dmat1);
	rvec5 = rv_unskew(rmat2);
	printf("m2unskewed: {%3.0f %3.0f %3.0f} rvunskewed: {%3.0f %3.0f %3.0f} Actual: { 1  -3   4 }\n", dvec1.vector[0], dvec1.vector[1], dvec1.vector[2], rvec5.col[0], rvec5.col[1], rvec5.col[2]);
	error = fabs(dmat1.array[0][0]) + fabs(dmat1.array[0][1]+4.) + fabs(dmat1.array[0][2]+3) + fabs(dmat1.array[1][0]-4) + fabs(dmat1.array[1][1]) + fabs(dmat1.array[1][2]+1) + fabs(dmat1.array[2][0]-3) + fabs(dmat1.array[2][1]-1) + fabs(dmat1.array[2][2]);
	terror += fabs(error);
	printf("m2_skew() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat2.row[0].col[0]) + fabs(rmat2.row[0].col[1] + 4) + fabs(rmat2.row[0].col[2]+ 3) + fabs(rmat2.row[1].col[0] - 4) + fabs(rmat2.row[1].col[1]) + fabs(rmat2.row[1].col[2] + 1) + fabs(rmat2.row[2].col[0] - 3) + fabs(rmat2.row[2].col[1] - 1) + fabs(rmat2.row[2].col[2]);
	terror += fabs(error);
	printf("rm_skew() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(dvec1.vector[0] - 1) + fabs(dvec1.vector[1]+3) + fabs(dvec1.vector[2]-4);
	terror += fabs(error);
	printf("m2_unskew() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rvec5.col[0] -1) + fabs(rvec5.col[1]+3) + fabs(rvec5.col[2]-4);
	terror += fabs(error);
	printf("rv_unskew() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: rm_rot() (x, y, z)\n");
	printf("\n");
	printf("Angle: 1.5 rad\n");
	angle1  = 1.5;
	val1 = cos(angle1); val2 = -val1; val3 = sin(angle1); val4 = -val3;
	rmat1 = rm_change_around_x(angle1);
	rmat2 = rm_change_around_y(angle1);
	rmat3 = rm_change_around_z(angle1);
	printf("X calc.: [%3.0f %3.0f %3.0f] Y calc.: [%3.0f %3.0f %3.0f] Z calc.: [%3.0f %3.0f %3.0f]\n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2],rmat2.row[0].col[0],rmat2.row[0].col[1],rmat2.row[0].col[2],rmat3.row[0].col[0],rmat3.row[0].col[1],rmat3.row[0].col[2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]\n",rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2],rmat2.row[1].col[0],rmat2.row[1].col[1],rmat2.row[1].col[2],rmat3.row[1].col[0],rmat3.row[1].col[1],rmat3.row[1].col[2]);
	printf("         [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]          [%3.0f %3.0f %3.0f]\n",rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2],rmat2.row[2].col[0],rmat2.row[2].col[1],rmat2.row[2].col[2],rmat3.row[2].col[0],rmat3.row[2].col[1],rmat3.row[2].col[2]);
	printf("X correct: [%3.0f %3.0f %3.0f] Y correct: [%3.0f %3.0f %3.0f] Z correct: [%3.0f %3.0f %3.0f]\n",1.,0.,0.,val1,0.,val3,val1,val4,0.);
	printf("           [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]\n",0.,val1,val4,0.,1.,0.,val3,val1,0.);
	printf("           [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]            [%3.0f %3.0f %3.0f]\n",0.,val3,val1,val4,0.,val1,0.,0.,1.);
	error = fabs(rmat1.row[0].col[0] - 1.) + fabs(rmat1.row[0].col[1]) + fabs(rmat1.row[0].col[2]) + fabs(rmat1.row[1].col[0]) + fabs(rmat1.row[1].col[1] - val1) + fabs(rmat1.row[1].col[2]- val4) + fabs(rmat1.row[2].col[0]) + fabs(rmat1.row[2].col[1]-val3) +
			fabs(rmat1.row[2].col[2]-val1);
	terror += error;
	printf("rm_change_around_x() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat2.row[0].col[0]-val1) + fabs(rmat2.row[0].col[1]) + fabs(rmat2.row[0].col[2]-val3) + fabs(rmat2.row[1].col[0]) + fabs(rmat2.row[1].col[1] - 1.) + fabs(rmat2.row[1].col[2]) + fabs(rmat2.row[2].col[0] - val4) + fabs(rmat2.row[2].col[1]) +
			fabs(rmat2.row[2].col[2]-val1);
	terror += error;
	printf("rm_change_around_y() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(rmat3.row[0].col[0] - val1) + fabs(rmat3.row[0].col[1] - val4) + fabs(rmat3.row[0].col[2]) + fabs(rmat3.row[1].col[0] - val3) + fabs(rmat3.row[1].col[1] - val1) + fabs(rmat3.row[1].col[2]) + fabs(rmat3.row[2].col[0]) + fabs(rmat3.row[2].col[1])
			+ fabs(rmat3.row[2].col[2] - 1.);
	terror += error;
	printf("rm_change_around_z() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: rm_change_between_rv() cm_change_between_rv\n");
	printf("Vector 1: {0, 0, 1} -> Vector 2: {1, 0, 0}\n");
	rvec5.col[0] = 0.; rvec5.col[1] = 0.; rvec5.col[2] = 1.;
	rvec6.col[0] = 1.; rvec6.col[1] = 0.; rvec6.col[2] = 0.;
	cvec5.x = 0.; cvec5.y = 0.; cvec5.z = 1.;
	cvec6.x = 1.; cvec6.y = 0.; cvec6.z = 0.;
	cmat1 = cm_change_between_cv(cvec5, cvec6);
	rmat1 = rm_change_between_rv(rvec5, rvec6);
	rmat2.row[0].col[0] = 0.; rmat2.row[0].col[1] = 0.; rmat2.row[0].col[2] = 1.;
	rmat2.row[1].col[0] = 0.; rmat2.row[1].col[1] = 1.; rmat2.row[1].col[2] = 0.;
	rmat2.row[2].col[0] = -1.; rmat2.row[2].col[1] = 0.; rmat2.row[2].col[2] = 0.;
	printf("rmDCM: [%+4.2f %+4.2f %+4.2f] cmDCM: [%+4.2f %+4.2f %+4.2f] Actual: [ 0  0  1]\n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2],cmat1.r1.x, cmat1.r1.y, cmat1.r1.z);
	printf("       [%+4.2f %+4.2f %+4.2f]        [%+4.2f %+4.2f %+4.2f]         [ 0  1  0]\n",rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2],cmat1.r2.x, cmat1.r2.y, cmat1.r2.z);
	printf("       [%+4.2f %+4.2f %+4.2f]        [%+4.2f %+4.2f %+4.2f]         [-1  0  0]\n",rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2],cmat1.r3.x, cmat1.r3.y, cmat1.r3.z);
	error= fabs(rmat1.row[0].col[0]) + fabs(rmat1.row[0].col[1]) + fabs(rmat1.row[0].col[2] - 1.) + fabs(rmat1.row[1].col[0]) + fabs(rmat1.row[1].col[1] -1.) + fabs(rmat1.row[1].col[2]) + fabs(rmat1.row[2].col[0] + 1.) + fabs(rmat1.row[2].col[1]) + fabs(rmat1.row[2].col[2]);
	terror+= error;
	printf("rm_change_between_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(cmat1.r1.x) + fabs(cmat1.r1.y) + fabs(cmat1.r1.z-1) + fabs(cmat1.r2.x) + fabs(cmat1.r2.y-1) + fabs(cmat1.r2.z) + fabs(cmat1.r3.x + 1) + fabs(cmat1.r3.y) + fabs(cmat1.r3.z);
	terror += error;
	printf("cm_change_between_rv() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: m1_norm()\n");
	printf("Vector: {1, 2, 3}\n");
	dvec1.vector[0] = 1; dvec1.vector[1] = 2; dvec1.vector[2] = 3;
	val1 = m1_norm(dvec1);
	val2 = sqrt(pow(dvec1.vector[0], 2) + pow(dvec1.vector[1], 2) + pow(dvec1.vector[2], 2));
	printf("Calc: %4.2f Actual: %4.2f\n", val1, val2);
	error = fabs(val1 - val2);
	terror += error;
	printf("m1_norm() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: m2_eig2x2()\n");
	dmat1.array[0][0] = 1.;
	dmat1.array[0][1] = 3.;
	dmat1.array[0][2] = 0.;
	dmat1.array[1][0] = 4.;
	dmat1.array[1][1] = 2.;
	dmat1.array[1][2] = 0.;
	dmat1.array[2][0] = 0.;
	dmat1.array[2][1] = 0.;
	dmat1.array[2][2] = 0.;
	dmat1.cols = 2;
	dmat1.rows = 2;
	dvec1 = m2_eig2x2(dmat1);
	printf("Calc. vals: {%4.2f %4.2f} Actual: {5 -2}\n",dvec1.vector[0], dvec1.vector[1]);
	error = fabs(dvec1.vector[0] - 5) + fabs(dvec1.vector[1] + 2);
	terror += error;
	printf("m2_eig2x2() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: m2_snorm2x2()\n");
	dmat1.array[0][0] = 1.;
	dmat1.array[0][1] = 2.;
	dmat1.array[0][2] = 0.;
	dmat1.array[1][0] = 1.;
	dmat1.array[1][1] = 2.;
	dmat1.array[1][2] = 0.;
	dmat1.array[2][0] = 0.;
	dmat1.array[2][1] = 0.;
	dmat1.array[2][2] = 0.;
	dmat1.cols = 2;
	dmat1.rows = 2;
	val1 = m2_snorm2x2(dmat1);
	val2 = sqrt(10);
	printf("Matrix: [1  2]  Spectral norm: %4.2f\n", val1);
	printf("        [1  2]  Actual: %4.2f\n", val2);
	error = fabs(val1 - val2);
	terror += error;
	printf("m2_snorm2x2() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: rm_square() cm_square()\n");
	rmat2.row[0] = rvec1; //3, 3, 0
	rmat2.row[1] = rvec2; //0, 5, 5
	rmat2.row[2] = rvec4; //-1, -1, -1
	cmat1.r1 = cvec1;
	cmat1.r2 = cvec2;
	cmat1.r3 = cvec4;
	rmat1 = rm_square(rmat2);
	cmat2 = cm_square(cmat1);
	printf("rmsquare: [%3.0f %3.0f %3.0f] cmsquare: [%3.0f %3.0f %3.0f] Actual: [ 9    24   15]\n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2],cmat2.r1.x, cmat2.r1.y, cmat2.r1.z);
	printf("          [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]         [ -5   20   20]\n",rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2],cmat2.r2.x, cmat2.r2.y, cmat2.r2.z);
	printf("          [%3.0f %3.0f %3.0f]           [%3.0f %3.0f %3.0f]         [ -2   -7   -4]\n",rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2],cmat2.r3.x, cmat2.r3.y, cmat2.r3.z);
	error= fabs(rmat1.row[0].col[0] - 9) + fabs(rmat1.row[0].col[1] - 24) + fabs(rmat1.row[0].col[2] - 15) + fabs(rmat1.row[1].col[0] + 5) + fabs(rmat1.row[1].col[1] - 20.) + fabs(rmat1.row[1].col[2] - 20.) + fabs(rmat1.row[2].col[0] + 2.) + fabs(rmat1.row[2].col[1] + 7.) + fabs(rmat1.row[2].col[2] + 4.);
	terror += error;
	printf("rm_square() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(cmat2.r1.x - 9) + fabs(cmat2.r1.y - 24) + fabs(cmat2.r1.z - 15) + fabs(cmat2.r2.x + 5) + fabs(cmat2.r2.y - 20) + fabs(cmat2.r2.z - 20) + fabs(cmat2.r3.x + 2) + fabs(cmat2.r3.y + 7) + fabs(cmat2.r3.z + 4);
	terror += error;
	printf("cm_square() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Functions: norm_rm() norm_cm()\n");
	rmat1.row[0] = rvec1; //3, 3, 0
	rmat1.row[1] = rvec2; //0, 5, 5
	rmat1.row[2] = rvec4; //-1, -1, -1
	cmat2.r1 = cvec1;
	cmat2.r2 = cvec2;
	cmat2.r3 = cvec4;
	val1 = norm_rm(rmat1);
	val2 = norm_cm(cmat2);
	printf("rm: [%3.0f %3.0f %3.0f] cm: [%3.0f %3.0f %3.0f] \n",rmat1.row[0].col[0],rmat1.row[0].col[1],rmat1.row[0].col[2],cmat2.r1.x, cmat2.r1.y, cmat2.r1.z);
	printf("    [%3.0f %3.0f %3.0f]     [%3.0f %3.0f %3.0f] \n",rmat1.row[1].col[0],rmat1.row[1].col[1],rmat1.row[1].col[2],cmat2.r2.x, cmat2.r2.y, cmat2.r2.z);
	printf("    [%3.0f %3.0f %3.0f]     [%3.0f %3.0f %3.0f] \n",rmat1.row[2].col[0],rmat1.row[2].col[1],rmat1.row[2].col[2],cmat2.r3.x, cmat2.r3.y, cmat2.r3.z);
	printf("rmnorm: %4.2f cmnorm: %4.2f\n", val1, val2);
	error = fabs(val1 - 5);
	terror += error;
	printf("norm_rm() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	error = fabs(val2 - 5);
	terror += error;
	printf("norm_cm() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: rv_fitpoly()\n");
	//The X coordinates:
	uvec1.r.col[0] = -4.0; uvec1.r.col[1] = -2.1; uvec1.r.col[2] = 1.3;
	//The actual coefficients:
	uvec3.r.col[0] = -4.5; uvec3.r.col[1] = 12.0; uvec3.r.col[2] = -5.3;
	//Test:
	uvec2.r = rv_sadd(uvec3.r.col[0], rv_zero());
	printf("First order:\n");
	uvec2.r = rv_add(uvec2.r, rv_smult(uvec3.r.col[1], uvec1.r));
	uvec4 = rv_fitpoly(uvec1, uvec2, 1);
	printf("\tfunction: %8.5gX + %8.5g\n",uvec4.r.col[1],uvec4.r.col[0]);
	printf("\tcorrect:  %8.5gX + %8.5g\n",uvec3.r.col[1],uvec3.r.col[0]);
	error = fabs(uvec3.r.col[0]-uvec4.r.col[0])+fabs(uvec3.r.col[1]-uvec4.r.col[1])+fabs(uvec4.r.col[2]);
	printf("Second order:\n");
	uvec2.r = rv_add(uvec2.r, rv_smult(uvec3.r.col[2], rv_mult(uvec1.r, uvec1.r)));
	uvec4 = rv_fitpoly(uvec1, uvec2, 2);
	printf("\tfunction: %8.5gX^2 + %8.5gX + %8.5g\n",uvec4.r.col[2],uvec4.r.col[1],uvec4.r.col[0]);
	printf("\tcorrect:  %8.5gX^2 + %8.5gX + %8.5g\n",uvec3.r.col[2],uvec3.r.col[1],uvec3.r.col[0]);
	error += fabs(uvec3.r.col[0]-uvec4.r.col[0])+fabs(uvec3.r.col[1]-uvec4.r.col[1])+fabs(uvec4.r.col[2]-uvec3.r.col[2]);
	/*
printf("Third order:\n");
uvec2.r = rv_add(uvec2.r, rv_smult(uvec3.r.col[3], rv_mult(rv_mult(uvec1.r, uvec1.r), uvec1.r)));
uvec4 = rv_fitpoly(uvec1, uvec2, 3);
printf("\tfunction: %8.5gX^3 + %8.5gX^2 + %8.5gX + %8.5g\n",uvec4.r.col[3],uvec4.r.col[2],uvec4.r.col[1],uvec4.r.col[0]);
printf("\tcorrect:  %8.5gX^3 + %8.5gX^2 + %8.5gX + %8.5g\n",uvec3.r.col[3],uvec3.r.col[2],uvec3.r.col[1],uvec3.r.col[0]);
error += fabs(uvec3.r.col[0]-uvec4.r.col[0])+fabs(uvec3.r.col[1]-uvec4.r.col[1])+fabs(uvec4.r.col[2]-uvec3.r.col[2])+fabs(uvec4.r.col[3]-uvec3.r.col[3]);
terror += error;
printf("rv_fitpoly() Error: %11.5g\tTerror: %11.5g\n",error,terror);
printf("\n");
*/

	printf("\n");
	printf("Function: polyfit()\n");
	//The X coordinates:
	vector<double> x(5,0.);
	x[0] = 0.0005787042027805;
	x[1] = 0.00059027828683611;
	x[2] = 0.00060185237089172;
	x[3] = 0.00061342645494733;
	x[4] = 0.00062500053900294;
	//The actual coefficients:
	vector<double> a(5,0.);
	a[0] = -0.00047752;
	a[1] = 93731000.;
	a[2] = 5770600000.;
	a[3] = -140230000000.;
	a[4] = -4191900000000.;

	a.resize(5);
	// The Y values
	vector<double> y;
	vector<double> newy;
	y.resize(a.size());
	newy.resize(a.size());
	x.resize(a.size());

	for (uint32_t i=0; i<a.size(); i++)
	{
		y[i] = a[0];
		double tx = x[i];
		for (uint32_t j=1; j<a.size(); ++j)
		{
			y[i] += a[j] * tx;
			tx *= x[i];
		}
	}

	vector<double> aback;
	aback.resize(a.size());
	aback = polyfit(x,y);

	for (uint32_t i=0; i<a.size(); i++)
	{
		newy[i] = aback[0];
		double tx = x[i];
		for (uint32_t j=1; j<a.size(); ++j)
		{
			newy[i] += aback[j] * tx;
			tx *= x[i];
		}
	}

	error = 0.;
	for (uint32_t i=0; i<a.size(); i++)
	{
		if (a[i] != 0.) error += fabs((y[i]-newy[i])/y[i]);
	}
	terror += error;
	printf("polyfit() Error: %11.5g\tTerror: %11.5g\n",error,terror);
	printf("\n");

	printf("Function: local_byte_order()\n");
	union {
		int x;
		char c[sizeof(int)];
	} check;
	check.x = 1;
	if (check.c[0] == 1 && local_byte_order() == ORDER_LITTLEENDIAN)
		printf("Little endian order\n");
	else if(check.c[0] == 0 && local_byte_order() == ORDER_BIGENDIAN)
		printf("Big endian order\n");
	else
		printf("Error!\n");
	printf("\n");
}
