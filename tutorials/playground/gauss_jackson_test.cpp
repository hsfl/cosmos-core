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

#include <stdlib.h>
#include <stdio.h>
#include "mathlib.h"

double myv0(double vi);
double myv1(double vi);
double myv2(double vi);
void mycalc(double vi, double *vd0, double *vd2, int32_t axes);

#define ORDER 8
#define AXES 1

static double *p = NULL;

int main(int argc, char *argv[])
{
int i;
gj_kernel *kern;
gj_instance *gji;
double vd0[AXES], vd1[AXES], vd2[AXES];


kern = gauss_jackson_kernel(ORDER,1.);
gji = gauss_jackson_instance(kern,AXES,mycalc);

for (i=-ORDER/2; i<=ORDER/2; i++)
	{
	vd0[0] = myv0(i*1.);
	vd1[0] = myv1(i*1.);
	vd2[0] = myv2(i*1.);
	gauss_jackson_setstep(gji,i*1.,vd0,vd1,vd2,i+ORDER/2);
	}

gauss_jackson_preset(gji);

gauss_jackson_extrapolate(gji,10.);
vd0[0] = myv0(10.);
vd1[0] = myv1(10.);
vd2[0] = myv2(10.);
}

double myv0(double vi)
{
int i;
double vd0;

if (p == NULL)
	{
	p = (double *)zalloc(ORDER+1,sizeof(double));
	for (i=0; i<ORDER+1; i++)
		p[i] = drand48();
	}
p[0] = p[1] = 0.;
p[2] = 1.;
p[3] = p[4] = p[5] = p[6] = p[7] = p[8] = 0.;
vd0 = 0.;
for (i=0; i<ORDER+1; i++)
	{
	vd0 += p[i]*pow(vi,i);
	}
return (vd0);
}

double myv1(double vi)
{
int i;
double vd1;

if (p == NULL)
	{
	p = (double *)zalloc(ORDER+1,sizeof(double));
	for (i=0; i<ORDER+1; i++)
		p[i] = drand48();
	}
vd1 = 0.;
for (i=0; i<ORDER; i++)
	{
	vd1 += (i+1)*p[i+1]*pow(vi,i);
	}
return (vd1);
}

double myv2(double vi)
{
int i;
double vd2;

if (p == NULL)
	{
	p = (double *)zalloc(ORDER+1,sizeof(double));
	for (i=0; i<ORDER+1; i++)
		p[i] = drand48();
	}

vd2 = 0.;
for (i=0; i<ORDER-1; i++)
	{
	vd2 += (i+1)*(i+2)*p[i+2]*pow(vi,i);
	}
return (vd2);
}

void mycalc(double vi, double *vd0, double *vd2, int32_t axes)
{
vd2[0] = myv2(vi);
}
