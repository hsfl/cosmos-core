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

#include "mathlib.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
int i, j;
uvector uvec[4];
rvector rvec1, rvec2, rvec3;
rmatrix rm1, rm2, rmx, rmy, rmz, rm3;
quaternion q1, q2;
avector a1;

a1.h = RADOF(90.);;
a1.e = RADOF(0.);
a1.b = RADOF(90.);
q1 = q_euler2quaternion(a1);
q2.d.x = q2.d.y = q2.d.z = sin(RADOF(60.))/sqrt(3.);
q2.w = cos(RADOF(60.));
printf("quaternion [%11f %11f %11f %11f] [%11f %11f %11f %11f]\n",q1.w,q1.d.x,q1.d.y,q1.d.z,q2.w,q2.d.x,q2.d.y,q2.d.z);
//rvec1 = rv_rotate_q(rv_unitx(),q1);
//rvec2 = rv_rotate_q(rv_unity(),q1);
//rvec3 = rv_rotate_q(rv_unitz(),q1);
printf("[%11f %11f %11f] ",rvec1.col[0],rvec1.col[1],rvec1.col[2]);
printf("[%11f %11f %11f] ",rvec2.col[0],rvec2.col[1],rvec2.col[2]);
printf("[%11f %11f %11f]\n",rvec3.col[0],rvec3.col[1],rvec3.col[2]);
printf("\n");
rm1 = rm_quaternion2dcm(q1);
printf("dcm [ ");
for (j=0; j<3; j++)
	{
	if (j)
		printf(";");
	printf("%11f %11f %11f",rm1.row[j].col[0],rm1.row[j].col[1],rm1.row[j].col[2]);
	}
printf("]\n");
rvec1 = rv_mmult(rm1,rv_unitx());
rvec2 = rv_mmult(rm1,rv_unity());
rvec3 = rv_mmult(rm1,rv_unitz());
printf("[%11f %11f %11f] ",rvec1.col[0],rvec1.col[1],rvec1.col[2]);
printf("[%11f %11f %11f] ",rvec2.col[0],rvec2.col[1],rvec2.col[2]);
printf("[%11f %11f %11f]\n",rvec3.col[0],rvec3.col[1],rvec3.col[2]);
printf("\n");

rmx = rm_roty(a1.b);
rmy = rm_rotx(a1.e);
rmz = rm_rotz(a1.h);
printf("dcmx [ ");
for (j=0; j<3; j++)
	{
	if (j)
		printf(";");
	printf("%11f %11f %11f",rmx.row[j].col[0],rmx.row[j].col[1],rmx.row[j].col[2]);
	}
printf("]\n");
printf("dcmy [ ");
for (j=0; j<3; j++)
	{
	if (j)
		printf(";");
	printf("%11f %11f %11f",rmy.row[j].col[0],rmy.row[j].col[1],rmy.row[j].col[2]);
	}
printf("]\n");
printf("dcmz [ ");
for (j=0; j<3; j++)
	{
	if (j)
		printf(";");
	printf("%11f %11f %11f",rmz.row[j].col[0],rmz.row[j].col[1],rmz.row[j].col[2]);
	}
printf("]\n");
rm1 = rm_eye();
rm1 = rm_mmult(rmz,rm1);
rm1 = rm_mmult(rmy,rm1);
rm1 = rm_mmult(rmx,rm1);
printf("dcm [ ");
for (j=0; j<3; j++)
	{
	if (j)
		printf(";");
	printf("%11f %11f %11f",rm1.row[j].col[0],rm1.row[j].col[1],rm1.row[j].col[2]);
	}
printf("]\n");
printf("\n");

rvec1 = rv_mmult(rm1,rv_unitx());
printf("[%11f %11f %11f]\n",rvec1.col[0],rvec1.col[1],rvec1.col[2]);

q1 = q_dcm2quaternion_rm(rm1);
a1 = a_quaternion2euler(q1);
printf("[%11f %11f %11f]\n",DEGOF(a1.h),DEGOF(a1.e),DEGOF(a1.b));

exit(1);
// Create 4 vectors at random
for (i=0; i<4; i++)
	{
	uvec[i].r = rv_zero();
	for (j=0; j<3; j++)
		uvec[i].r.col[j] = rand()-RAND_MAX/2.;
	normalize_rv(&uvec[i].r);
	}

// For each pair, calculate and output the cross product and the
// rotation matrix for one to the other.
rm2 = rm_eye();
for (i=0; i<4; i++)
	{
	printf("vector [%11f %11f %11f] to [%11f %11f %11f]\n",uvec[i].c.x,uvec[i].c.y,uvec[i].c.z,uvec[(i+1)%4].c.x,uvec[(i+1)%4].c.y,uvec[(i+1)%4].c.z);
//	rvec1 = rv_cross(uvec[i].r,uvec[(i+1)%4].r);
//	printf("[%11f %11f %11f]\n",rvec1.col[0],rvec1.col[1],rvec1.col[2]);
	rm1 = rm_rtdcm(uvec[i].r,uvec[(i+1)%4].r);
	q1 = q_dcm2quaternion_rm(rm1);
	a1 = a_quaternion2euler(q1);
	printf("euler [%11f %11f %11f]\n",a1.h,a1.b,a1.e);
	printf("quaternion [%11f %11f %11f %11f]\n",q1.d.x,q1.d.y,q1.d.z,q1.w);
	printf("dcm [ ");
	for (j=0; j<3; j++)
		{
		if (j)
			printf(";");
		printf("%11f %11f %11f",rm1.row[j].col[0],rm1.row[j].col[1],rm1.row[j].col[2]);
		}
	printf("]\n");
	rm2 = rm_mmult(rm1,rm2);
	printf("total dcm [ ");
	for (j=0; j<3; j++)
		{
		if (j)
			printf(";");
		printf("%11f %11f %11f",rm2.row[j].col[0],rm2.row[j].col[1],rm2.row[j].col[2]);
		}
	printf("]\n");
	rvec1 = rv_mmult(rm1,uvec[i].r);
	rvec2 = rv_mmult(rm2,uvec[0].r);
	printf("rotated [%11f %11f %11f]   [%11f %11f %11f]",rvec1.col[0],rvec1.col[1],rvec1.col[2],rvec2.col[0],rvec2.col[1],rvec2.col[2]);
	printf("\n\n");
	}
}
