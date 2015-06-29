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

#include "physicslib.h"
#include "timelib.h"
#include "datalib.h"

#define MAXDEGREE 360
#define ASTEP 1

static double ftl[2*MAXDEGREE+1];
static int cmodel = -1;
static double coef[MAXDEGREE+1][MAXDEGREE+1][2];
static double spmm[MAXDEGREE+1];
static double lastx = 10.;
static uint16_t lastm = 65535;
static double initialutc;
static char orbitfile[50] = {""};
static stkstruc stkhandle;

static locstruc sloc[MAXGJORDER+2];

//! Data structures for spherical harmonic expansion
/*! Coefficients for real and imaginary components of expansion. Of order and rank MAXDEGREE
*/
static double vc[MAXDEGREE+1][MAXDEGREE+1], wc[MAXDEGREE+1][MAXDEGREE+1];

//! Spherical harmonic  gravitational vector
/*!
* Calculates a spherical harmonic expansion of the chosen model of indicated order and
* degree for the requested position.
* The result is returned as a geocentric vector calculated at the epoch.
	\param pos a ::posstruc providing the position at the epoch
	\param model Model to use for coefficients
	\param degree Order and degree to calculate
	\return A ::rvector pointing toward the earth
	\see pgm2000a_coef.txt
*/
rvector gravity_accel(posstruc pos,int model,uint32_t degree)
{
	register uint32_t il, im;
//	double dlat, dlon;
	double tmult;
	//double ratio, rratio, xratio, yratio, zratio, vc[MAXDEGREE+1][MAXDEGREE+1], wc[MAXDEGREE+1][MAXDEGREE+1];
	double ratio, rratio, xratio, yratio, zratio;
	rvector accel;
	double fr;
	//double dc[5][4], ds[5][4];
	//static double co[5][4][2] = {{{-9999.}}};

	// Zero out vc and wc
	memset(vc,0,sizeof(vc));
	memset(wc,0,sizeof(wc));

	// Load Params
	gravity_params(model);

	/*
if (co[0][0][0] == -9999.)
	{
	for (il=0; il<5; il++)
		{
		for (im=0; im<4; im++)
			{
			co[il][im][0] = coef[il][im][0];
			co[il][im][1] = coef[il][im][1];
			}
		}
	}

SolidTide(pos,dc,ds);
for (il=0; il<5; il++)
	{
	for (im=0; im<4; im++)
		{
		coef[il][im][0] = co[il][im][0] + dc[il][im];
		coef[il][im][1] = co[il][im][1] + ds[il][im];
		}
	}
*/

	// Calculate cartesian Legendre terms
	vc[0][0] = REARTHM/pos.geos.s.r;
	wc[0][0] = 0.;
	ratio = vc[0][0] / pos.geos.s.r;
	rratio = REARTHM * ratio;
	xratio = pos.geoc.s.col[0] * ratio;
	yratio = pos.geoc.s.col[1] * ratio;
	zratio = pos.geoc.s.col[2] * ratio;
	vc[1][0] = zratio * vc[0][0];
	wc[1][0] = 0.;
	for (il=2; il<=degree+1; il++)
	{
		vc[il][0] = (2*il-1)*zratio * vc[il-1][0] / il - (il-1) * rratio * vc[il-2][0] / il;
		wc[il][0] = 0.;
	}
	for (im=1; im<=degree+1; im++)
	{
		vc[im][im] = (2*im-1) * (xratio * vc[im-1][im-1] - yratio * wc[im-1][im-1]);
		wc[im][im] = (2*im-1) * (xratio * wc[im-1][im-1] + yratio * vc[im-1][im-1]);
		if (im <= degree)
		{
			vc[im+1][im] = (2*im+1) * zratio * vc[im][im];
			wc[im+1][im] = (2*im+1) * zratio * wc[im][im];
		}
		for (il=im+2; il<=degree+1; il++)
		{
			vc[il][im] = (2*il-1) * zratio * vc[il-1][im] / (il-im) - (il+im-1) * rratio * vc[il-2][im] / (il-im);
			wc[il][im] = (2*il-1) * zratio * wc[il-1][im] / (il-im) - (il+im-1) * rratio * wc[il-2][im] / (il-im);
		}
	}

//	dr = dlon = dlat = 0.;

	accel = rv_zero();
	for (im=0; im<=degree; im++)
	{
		for (il=im; il<=degree; il++)
		{
			if (im == 0)
			{
				accel.col[0] -= coef[il][0][0] * vc[il+1][1];
				accel.col[1] -= coef[il][0][0] * wc[il+1][1];
				accel.col[2] -= (il+1) * (coef[il][0][0] * vc[il+1][0]);
			}
			else
			{
				fr = ftl[il-im+2] / ftl[il-im];
				accel.col[0] -= .5 * (coef[il][im][0] * vc[il+1][im+1] + coef[il][im][1] * wc[il+1][im+1] - fr * (coef[il][im][0] * vc[il+1][im-1] + coef[il][im][1] * wc[il+1][im-1]));
				accel.col[1] -= .5 * (coef[il][im][0] * wc[il+1][im+1] - coef[il][im][1] * vc[il+1][im+1] + fr * (coef[il][im][0] * wc[il+1][im-1] - coef[il][im][1] * vc[il+1][im-1]));
				accel.col[2] -= (il-im+1) * (coef[il][im][0] * vc[il+1][im] + coef[il][im][1] * wc[il+1][im]);
			}
		}
	}
	tmult = GM / (REARTHM*REARTHM);
	accel.col[0] *= tmult;
	accel.col[2] *= tmult;
	accel.col[1] *= tmult;

	return (accel);
}

//void SolidTide(posstruc pos, double dc[5][4], double ds[5][4])
//{
//	/* Love numbers for anelastic Earth model*/
//	const double
//	k20 = 0.30190,
//	kr21 = 0.29830, /* k21 & k22 complex numbers */
//	ki21 = -0.00144,
//	kr22 = 0.30102,
//	ki22 = -0.00130,
//	k30 = 0.093,
//	k31 = 0.093,
//	k32 = 0.093,
//	k33 = 0.094,
//	kp20 = -0.00089,
//	kp21 = -0.00080,
//	kp22 = -0.00057;

//	/* geopotential tide constants */
//	const double
//	f20 = sqrt(5.0),	/* normalization factors for potential coefficients */
//	f21 = sqrt(5.0/3.0),
//	f22 = sqrt(5.0/12.0),
//	//	f30 = sqrt(7.0),
//	//	f31 = sqrt(7.0/6.0),
//	//	f32 = sqrt(7.0/60.0),
//	//	f33 = sqrt(7.0/360.0),
//	//	f40 = sqrt(9.0),
//	//	f41 = sqrt(9.0/10.0),
//	//	f42 = sqrt(1.0/20.0),
//	f20s = 5.0,	/* squared */
//	f21s = 5.0/3.0,
//	f22s = 5.0/12.0,
//	f30s = 7.0,
//	f31s = 7.0/6.0,
//	f32s = 7.0/60.0,
//	f33s = 7.0/360.0,
//	f40s = 9.0,
//	f41s = 9.0/10.0,
//	f42s = 1.0/20.0;

//	cvector Moon, Sun;
//	uvector utmp;
//	int m, n;
//	double rM, phiM, lambdaM, rS, phiS, lambdaS,
//	PM[5][5], PS[5][5],
//	sinphiM, cosphiM, sinphiS, cosphiS,
//	sinlambdaM, coslambdaM, sinlambdaS, coslambdaS,
//	sin2lambdaM, cos2lambdaM, sin2lambdaS, cos2lambdaS,
//	sin3lambdaM, cos3lambdaM, sin3lambdaS, cos3lambdaS,
//	//		fM, fS, C[5][4], S[5][4],
//	fM, fS,
//	g, l, lp, F, D, omega,
//	s, h, p, Np, ps, tau,
//	arg, sine, cosine, f, dC, dS;

//	/* angular coordinates of Moon & Sun */
//	utmp.r = rv_sub(pos.extra.sun2moon.s,pos.extra.sun2earth.s);
//	Moon = utmp.c;
//	utmp.r = rv_smult(-1.,pos.extra.sun2earth.s);
//	Sun = utmp.c;
//	rM = sqrt(Moon.x*Moon.x + Moon.y*Moon.y + Moon.z*Moon.z);
//	phiM = asin(Moon.z/rM);
//	lambdaM = asin(Moon.y/(rM*cos(phiM)));
//	rS = sqrt(Sun.x*Sun.x + Sun.y*Sun.y + Sun.z*Sun.z);
//	phiS = asin(Sun.z/rS);
//	lambdaS = asin(Sun.y/(rS*cos(phiS)));
//	/* compute unnormalized associated Legendre functions */
//	sinphiM = sin(phiM);
//	cosphiM = cos(phiM);
//	PM[0][0] = 1;
//    for (m = 0; m < 3; m++)
//    {
//		for (n = m + 1; n < 4; n++)
//			PM[n][m] = ((2*n - 1)*sinphiM*PM[n-1][m] - (n + m - 1)*PM[n-2][m])/(n - m);
//		PM[m+1][m+1] = (2*m + 1)*cosphiM*PM[m][m];
//	}
//	sinphiS = sin(phiS);
//	cosphiS = cos(phiS);
//	PS[0][0] = 1;
//    for (m = 0; m < 3; m++)
//    {
//		for (n = m + 1; n < 4; n++)
//			PS[n][m] = ((2*n - 1)*sinphiS*PS[n-1][m]-(n + m - 1)*PS[n-2][m])/(n - m);
//		PS[m+1][m+1] = (2*m + 1)*cosphiS*PS[m][m];
//	}

//	/* compute corrections C & S to unnormalized Stokes coefficients */
//	/* corrections for frequency independent Love numbers */
//	sinlambdaM = sin(lambdaM);
//	coslambdaM = cos(lambdaM);
//	sinlambdaS = sin(lambdaS);
//	coslambdaS = cos(lambdaS);
//	sin2lambdaM = sin(2*lambdaM);
//	cos2lambdaM = cos(2*lambdaM);
//	sin2lambdaS = sin(2*lambdaS);
//	cos2lambdaS = cos(2*lambdaS);
//	sin3lambdaM = sin(3*lambdaM);
//	cos3lambdaM = cos(3*lambdaM);
//	sin3lambdaS = sin(3*lambdaS);
//	cos3lambdaS = cos(3*lambdaS);
//	fM = (GMOON/GM)*pow(REARTHKM/rM,3);
//	fS = (GSUN/GM)*pow(REARTHKM/rS,3);
//	dc[2][0] = f20s*k20*(fM*PM[2][0] + fS*PS[2][0])/5;
//	dc[2][1] = f21s*((kr21*coslambdaM + ki21*sinlambdaM)*fM*PM[2][1]
//	+ (kr21*coslambdaS + ki21*sinlambdaS)*fS*PS[2][1])/5;
//	ds[2][1] = f21s*((kr21*sinlambdaM - ki21*coslambdaM)*fM*PM[2][1]
//	+ (kr21*sinlambdaS - ki21*coslambdaS)*fS*PS[2][1])/5;
//	dc[2][2] = f22s*((kr22*cos2lambdaM + ki22*sin2lambdaM)*fM*PM[2][2]
//	+ (kr22*cos2lambdaS + ki22*sin2lambdaS)*fS*PS[2][2])/5;
//	ds[2][2] = f22s*((kr22*sin2lambdaM - ki22*cos2lambdaM)*fM*PM[2][2]
//	+ (kr22*sin2lambdaS - ki22*cos2lambdaS)*fS*PS[2][2])/5;
//	fM = fM*(REARTHKM/rM);
//	fS = fS*(REARTHKM/rS);
//	dc[3][0] = f30s*k30*(fM*PM[3][0] + fS*PS[3][0])/7;
//	dc[3][1] = f31s*k31*(fM*PM[3][1]*coslambdaM + fS*PS[3][1]*coslambdaS)/7;
//	ds[3][1] = f31s*k31*(fM*PM[3][1]*sinlambdaM + fS*PS[3][1]*sinlambdaS)/7;
//	dc[3][2] = f32s*k32*(fM*PM[3][2]*cos2lambdaM + fS*PS[3][2]*cos2lambdaS)/7;
//	ds[3][2] = f32s*k32*(fM*PM[3][2]*sin2lambdaM + fS*PS[3][2]*sin2lambdaS)/7;
//	dc[3][3] = f33s*k33*(fM*PM[3][3]*cos3lambdaM + fS*PS[3][3]*cos3lambdaS)/7;
//	ds[3][3] = f33s*k33*(fM*PM[3][3]*sin3lambdaM + fS*PS[3][3]*sin3lambdaS)/7;
//	fM = fM/(REARTHKM/rM);
//	fS = fS/(REARTHKM/rS);
//	dc[4][0] = f40s*kp20*(fM*PM[2][0] + fS*PS[2][0])/5;
//	dc[4][1] = f41s*kp21*(fM*PM[2][1]*coslambdaM + fS*PS[2][1]*coslambdaS)/5;
//	ds[4][1] = f41s*kp21*(fM*PM[2][1]*sinlambdaM + fS*PS[2][1]*sinlambdaS)/5;
//	dc[4][2] = f42s*kp22*(fM*PM[2][2]*cos2lambdaM + fS*PS[2][2]*cos2lambdaS)/5;
//	ds[4][2] = f42s*kp22*(fM*PM[2][2]*sin2lambdaM + fS*PS[2][2]*sin2lambdaS)/5;

//	/* compute corrections due to deviations of Love numbers from nominal values */
//	//	Delaunay(&g, &l, &lp, &F, &D, &omega);
//	l = utc2L(pos.utc);
//	lp = utc2Lp(pos.utc);
//	F = utc2F(pos.utc);
//	D = utc2D(pos.utc);
//	g = utc2gast(pos.utc);
//	omega = utc2omega(pos.utc);
//	/* Doodson variables */
//	s = F + omega;
//	h = s - D;
//	p = s - l;
//	Np = - omega;
//	ps = s - D - lp;
//	tau = g - s;
//	/* series for normalized C[2][0] */
//	arg = Np;						/* Doodson number 55,565 */
//	dC = 16.6*cos(arg) - 6.7*sin(arg);
//	arg = 2*Np;
//	dC += -0.1*cos(arg) + 0.1*sin(arg);
//	arg = h - ps;
//	dC += -1.2*cos(arg) + 0.8*sin(arg);
//	arg = 2*h;						/* Ssa tide */
//	dC += -5.5*cos(arg) + 4.3*sin(arg);
//	arg = 2*h + Np;
//	dC += +0.1*cos(arg) - 0.1*sin(arg);
//	arg = 3*h - ps;
//	dC += -0.3*cos(arg) + 0.2*sin(arg);
//	arg = s - 2*h + p;
//	dC += -0.3*cos(arg) + 0.7*sin(arg);
//	arg = s - p - Np;
//	dC += +0.1*cos(arg) - 0.2*sin(arg);
//	arg = s - p;						/* Mm tide */
//	dC += -1.2*cos(arg) + 3.7*sin(arg);
//	arg = s - p + Np;
//	dC += +0.1*cos(arg) - 0.2*sin(arg);
//	arg = s + p;
//	dC += +0.1*cos(arg) - 0.2*sin(arg);
//	arg = 2*s - 2*h;
//	dC += +0.6*sin(arg);
//	arg = 2*s - 2*p;
//	dC += +0.3*sin(arg);
//	arg = 2*s;						/* Mf tide */
//	dC += +0.6*cos(arg) + 6.3*sin(arg);
//	arg = 2*s + Np;
//	dC += +0.2*cos(arg) + 2.6*sin(arg);
//	arg = 2*s + 2*Np;
//	dC += +0.2*sin(arg);
//	arg = 3*s - 2*h + p;
//	dC += +0.1*cos(arg) + 0.2*sin(arg);
//	arg = 3*s - p;
//	dC += +0.4*cos(arg) + 1.1*sin(arg);
//	arg = 3*s - p + Np;
//	dC += +0.2*cos(arg) + 0.5*sin(arg);
//	arg = 4*s - 2*h;
//	dC += +0.1*cos(arg) + 0.2*sin(arg);
//	arg = 4*s - 2*p;
//	dC += +0.1*cos(arg) + 0.1*sin(arg);
//	/* unnormalize */
//	f = f20*1.0e-12;
//	dc[2][0] += f*dC;

//	/* series for normalized dc[2][1] & ds[2][1] */
//	arg = tau - 3*s + 2*p;
//	dC = -0.1*sin(arg);
//	dS = -0.1*cos(arg);
//	arg = tau - 3*s + 2*h;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	arg = tau - 2*s + p - Np;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	arg = tau - 2*s + p;				/* beware complex amplitude */
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -0.7*sine + 0.1*cosine;
//	dS += -0.7*cosine - 0.1*sine;
//	arg = tau - 2*s + 2*h - p;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	arg = tau - s - Np;
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -1.3*sine + 0.1*cosine;
//	dS += -1.3*cosine - 0.1*sine;
//	arg = tau - s; 					/* O1 tide */
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -6.8*sine + 0.6*cosine;
//	dS += -6.8*cosine - 0.6*sine;
//	arg = tau - s + 2*h;
//	dC += +0.1*sin(arg);
//	dS += +0.1*cos(arg);
//	arg = tau - 2*h + p;
//	dC += +0.1*sin(arg);
//	dS += +0.1*cos(arg);
//	arg = tau - p - Np;
//	dC += +0.1*sin(arg);
//	dS += +0.1*cos(arg);
//	arg = tau - p;
//	dC += +0.4*sin(arg);
//	dS += +0.4*cos(arg);
//	arg = tau + p;
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += +1.3*sine - 0.1*cosine;
//	dS += +1.3*cosine + 0.1*sine;
//	arg = tau + p + Np;
//	dC += +0.3*sin(arg);
//	dS += +0.3*cos(arg);
//	arg = tau + 2*h - p;
//	dC += +0.3*sin(arg);
//	dS += +0.3*cos(arg);
//	arg = tau + 2*h - p + Np;
//	dC += +0.1*sin(arg);
//	dS += +0.1*cos(arg);
//	arg = tau + s - 3*h + ps;
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -1.9*sine + 0.1*cosine;
//	dS += -1.9*cosine - 0.1*sine;
//	arg = tau + s - 2*h - Np;
//	dC += +0.5*sin(arg);
//	dS += +0.5*cos(arg);
//	arg = tau + s - 2*h;				/* P1 tide */
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -43.4*sine + 2.9*cosine;
//	dS += -43.4*cosine - 2.9*sine;
//	arg = tau + s - h - ps;
//	dC += +0.6*sin(arg);
//	dS += +0.6*cos(arg);
//	arg = tau + s - h + ps;
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += +1.6*sine - 0.1*cosine;
//	dS += +1.6*cosine + 0.1*sine;
//	arg = tau + s - 2*p - Np;
//	dC += +0.1*sin(arg);
//	dS += +0.1*cos(arg);
//	arg = tau + s - 2*Np;
//	dC += +0.1*sin(arg);
//	dS += +0.1*cos(arg);
//	arg = tau + s - Np;					/* Doodson number 165,545 */
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -8.8*sine + 0.5*cosine;
//	dS += -8.8*cosine - 0.5*sine;
//	arg = tau + s;						/* K1 tide */
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += +470.9*sine - 30.2*cosine;
//	dS += +470.9*cosine + 30.2*sine;
//	arg = tau + s + Np;					/* Doodson number 165,565 */
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += +68.1*sine - 4.6*cosine;
//	dS += +68.1*cosine + 4.6*sine;
//	arg = tau + s + 2*Np;
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -1.6*sine + 0.1*cosine;
//	dS += -1.6*cosine - 0.1*sine;
//	arg = tau + s + h - p;
//	dC += 0.1*sin(arg);
//	dS += 0.1*cos(arg);
//	arg = tau + s + h - Np - ps;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	arg = tau + s + h - ps;				/* psi1 tide */
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -20.6*sine - 0.3*cosine;
//	dS += -20.6*cosine + 0.3*sine;
//	arg = tau + s + h + ps;
//	dC += +0.3*sin(arg);
//	dS += +0.3*cos(arg);
//	arg = tau + s + h + Np - ps;
//	dC += -0.3*sin(arg);
//	dS += -0.3*cos(arg);
//	arg = tau + s + 2*h - 2*p;
//	dC += -0.2*sin(arg);
//	dS += -0.2*cos(arg);
//	arg = tau + s + 2*h - 2*p + Np;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	arg = tau + s + 2*h;
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -5.0*sine + 0.3*cosine;
//	dS += -5.0*cosine - 0.3*sine;
//	arg = tau + s + 2*h + Np;
//	dC += +0.2*sin(arg);
//	dS += +0.2*cos(arg);
//	arg = tau + s + 3*h - ps;
//	dC += -0.2*sin(arg);
//	dS += -0.2*cos(arg);
//	arg = tau + 2*s - 2*h + p;
//	dC += -0.5*sin(arg);
//	dS += -0.5*cos(arg);
//	arg = tau + 2*s - 2*h + p + Np;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	arg = tau + 2*s - p - Np;
//	dC += +0.1*sin(arg);
//	dS += +0.1*cos(arg);
//	arg = tau + 2*s - p;
//	sine = sin(arg);
//	cosine = cos(arg);
//	dC += -2.1*sine + 0.1*cosine;
//	dS += -2.1*cosine - 0.1*sine;
//	arg = tau + 2*s - p + Np;
//	dC += -0.4*sin(arg);
//	dS += -0.4*cos(arg);
//	arg = tau + 3*s - 2*h;
//	dC += -0.2*sin(arg);
//	dS += -0.2*cos(arg);
//	arg = tau + 3*s - 2*p;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	arg = tau + 3*s;
//	dC += -0.6*sin(arg);
//	dS += -0.6*cos(arg);
//	arg = tau + 3*s + Np;
//	dC += -0.4*sin(arg);
//	dS += -0.4*cos(arg);
//	arg = tau + 3*s + 2*Np;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	arg = tau + 4*s - p;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	arg = tau + 4*s - p + Np;
//	dC += -0.1*sin(arg);
//	dS += -0.1*cos(arg);
//	/* unnormalize */
//	f = f21*1.0e-12;
//	dc[2][1] += f*dC;
//	ds[2][1] += f*dS;

//	/* series for normalized C[2][2] */
//	arg = 2*tau - s + p;
//	dC = -0.3*cos(arg);
//	dS = +0.3*sin(arg);
//	arg = 2*tau;
//	dC += -1.2*cos(arg);
//	dS += +1.2*sin(arg);
//	/* unnormalize */
//	f = f22*1.0e-12;
//	dc[2][2] += f*dC;
//	ds[2][2] += f*dS;
//	/* correction due to solid Earth pole tide */
//	/*
//	f = f21;
//	dc[2][1] -= 1.333e-9*f*(m1-0.0115*m2);
//	ds[2][1] -= 1.333e-9*f*(m2-0.0115*m1);
//	*/
//}

rvector gravity_vector(svector pos,int model,uint32_t degree)
{
	double gp, g0, gm;
	locstruc tloc;
	rvector gvec;

	// Calculate potential and geocentric equivalent of position
	g0 = gravity_potential(pos.lambda,pos.phi,pos.r,model,degree);
	tloc.pos.geos.s = pos;
	pos_geos2geoc(&tloc);

	gvec = rv_zero();

	// Difference in X direction
	tloc.pos.geoc.s.col[0] -= .5;
	pos_geoc2geos(&tloc);
	gm = gravity_potential(tloc.pos.geos.s.lambda,tloc.pos.geos.s.phi,tloc.pos.geos.s.r,model,degree);
	tloc.pos.geoc.s.col[0] += 1.;
	pos_geoc2geos(&tloc);
	gp = gravity_potential(tloc.pos.geos.s.lambda,tloc.pos.geos.s.phi,tloc.pos.geos.s.r,model,degree);
	gvec.col[0] = (gp-gm);
	tloc.pos.geoc.s.col[0] -= .5;

	// Difference in Y direction
	tloc.pos.geoc.s.col[1] -= .5;
	pos_geoc2geos(&tloc);
	gm = gravity_potential(tloc.pos.geos.s.lambda,tloc.pos.geos.s.phi,tloc.pos.geos.s.r,model,degree);
	tloc.pos.geoc.s.col[1] += 1.;
	pos_geoc2geos(&tloc);
	gp = gravity_potential(tloc.pos.geos.s.lambda,tloc.pos.geos.s.phi,tloc.pos.geos.s.r,model,degree);
	gvec.col[1] = (gp-gm);
	tloc.pos.geoc.s.col[1] -= .5;

	// Difference in Z direction
	tloc.pos.geoc.s.col[2] -= .5;
	pos_geoc2geos(&tloc);
	gm = gravity_potential(tloc.pos.geos.s.lambda,tloc.pos.geos.s.phi,tloc.pos.geos.s.r,model,degree);
	tloc.pos.geoc.s.col[2] += 1.;
	pos_geoc2geos(&tloc);
	gp = gravity_potential(tloc.pos.geos.s.lambda,tloc.pos.geos.s.phi,tloc.pos.geos.s.r,model,degree);
	gvec.col[2] = (gp-gm);
	tloc.pos.geoc.s.col[2] -= .5;

	gvec = rv_normal(gvec);
	gvec = rv_smult(g0,gvec);

	return (gvec);
}

double gravity_potential(double lambda, double phi, double r,int model,uint32_t degree)
{
	register uint32_t il, im;
	double slat;
	double v, v1, v2, vm, plg, ilon;
	double clon[MAXDEGREE+1], slon[MAXDEGREE+1];

	// Load Params
	gravity_params(model);

	if (lambda < 0.)
		lambda += D2PI;

	v = v1 = v2 = 0.;
	slat = sin(phi);
	ilon = 0.;
	for (im=0; im<=degree; im++)
	{
		clon[im] = cos(ilon);
		slon[im] = sin(ilon);
		ilon += lambda;
		if (ilon >= D2PI)
			ilon -= D2PI;
	}
	for (il=2; il<=degree; il++)
	{
		vm = 0.;
		for (im=0; im<=il; im++)
		{
			plg = nplgndr(il,im,slat);
			vm += (coef[il][im][0]*clon[im] + coef[il][im][1]*slon[im]) * plg;
		}
		v += (il+1) * vm * pow((REARTHM/(r)),(double)il);
	}
	v += 1;
	v *= -GM / (r*r);

	return (v);
}

rvector gravity_accel2(posstruc pos,int model,uint32_t degree)
{
	register uint32_t il, im;
	double slat;
	double v, v1, v2, vm, plg, ilon;
	double clon[MAXDEGREE+1], slon[MAXDEGREE+1];
	double radius, colat, elon;
	rvector accel1;
	locstruc sloc;

	// Load Params
	gravity_params(model);

	radius = pos.geos.s.r;
	colat = pos.geod.s.lat;
	colat = pos.geos.s.phi;
	elon = pos.geod.s.lon;
	elon = pos.geos.s.lambda;
	if (elon < 0.)
		elon += D2PI;

	v = v1 = v2 = 0.;
	slat = sin(colat);
	ilon = 0.;
	for (im=0; im<=degree; im++)
	{
		clon[im] = cos(ilon);
		slon[im] = sin(ilon);
		ilon += elon;
		if (ilon >= D2PI)
			ilon -= D2PI;
	}
	for (il=2; il<=degree; il++)
	{
		vm = 0.;
		for (im=0; im<=il; im++)
		{
			plg = nplgndr(il,im,slat);
			vm += (coef[il][im][0]*clon[im] + coef[il][im][1]*slon[im]) * plg;
		}
		v += (il+1) * vm * pow((REARTHM/(radius)),(double)il);
	}
	v += 1;
	v *= -GM / (radius*radius);

	sloc.pos = pos;
	sloc.pos.geod.s.lat += .4448*(sloc.pos.geod.s.lat - sloc.pos.geos.s.phi);
	pos_geod2geoc(&sloc);
	accel1 = sloc.pos.geoc.s;
	normalize_rv(&accel1);
	accel1 = rv_smult(v,accel1);
	return (accel1);
}

int32_t gravity_params(int model)
{
	int32_t iretn;
	register uint32_t il, im;
	double norm;
	uint32_t dil, dim;
	double dummy1, dummy2;

	// Calculate factorial
	if (ftl[0] == 0.)
	{
		ftl[0] = 1.;
		for (il=1; il<2*MAXDEGREE+1; il++)
		{
			ftl[il] = il * ftl[il-1];
		}
	}

	// Load Coefficients
	if (cmodel != model)
	{
		coef[0][0][0] = 1.;
		coef[0][0][1] = 0.;
		coef[1][0][0] = coef[1][0][1] = 0.;
		coef[1][1][0] = coef[1][1][1] = 0.;
		string fname;
		FILE *fi;
		switch (model)
		{
		case GRAVITY_EGM2008:
		case GRAVITY_EGM2008_NORM:
			iretn = get_cosmosresources(fname);
			if (iretn < 0)
			{
				return iretn;
			}
			fname += "/general/egm2008_coef.txt";
			fi = fopen(fname.c_str(),"r");
			for (il=2; il<101; il++)
			{
				for (im=0; im<= il; im++)
				{
					iretn = fscanf(fi,"%u %u %lf %lf\n",&dil,&dim,&coef[il][im][0],&coef[il][im][1]);
					if (iretn && model == GRAVITY_EGM2008_NORM)
					{
						norm = sqrt(ftl[il+im]/((2-(im==0?1:0))*(2*il+1)*ftl[il-im]));
						coef[il][im][0] /= norm;
						coef[il][im][1] /= norm;
					}
				}
			}
			fclose(fi);
			cmodel = model;
			break;
		case GRAVITY_PGM2000A:
		case GRAVITY_PGM2000A_NORM:
		default:
			iretn = get_cosmosresources(fname);
			if (iretn < 0)
			{
				return iretn;
			}
			fname += "/general/pgm2000a_coef.txt";
			fi = fopen(fname.c_str(),"r");
			for (il=2; il<361; il++)
			{
				for (im=0; im<= il; im++)
				{
					iretn = fscanf(fi,"%u %u %lf %lf %lf %lf\n",&dil,&dim,&coef[il][im][0],&coef[il][im][1],&dummy1,&dummy2);
					if (iretn && model == GRAVITY_PGM2000A_NORM)
					{
						norm = sqrt(ftl[il+im]/((2-(il==im?1:0))*(2*il+1)*ftl[il-im]));
						coef[il][im][0] /= norm;
						coef[il][im][1] /= norm;
					}
				}
			}
			fclose(fi);
			cmodel = model;
			break;
		}
	}
	return 0;
}

double nplgndr(uint32_t l, uint32_t m, double x)
{
	double fact,pll,pmm,pmmp1,omx2, oldfact;
	register uint16_t i, ll, mm;

	pll = 0.;
	if (lastm == 65535 || m > lastm || x != lastx)
	{
		lastx = x;
		lastm = m;
		mm = m;
		//	for (mm=0; mm<=MAXDEGREE; mm++)
		//		{
		pmm=1.0;
		if (mm > 0)
		{
			omx2=((1.0-x)*(1.0+x));
			fact=1.0;
			for (i=1;i<=mm;i++)
			{
				pmm *= fact*omx2/(fact+1.);
				fact += 2.0;
			}
		}
		pmm = sqrt((2.*m+1.)*pmm);
		if (mm%2 == 1)
			pmm = - pmm;
		spmm[mm] = pmm;
		//		}
	}

	pmm = spmm[m];
	if (l == m)
		return pmm;
	else {
		pmmp1=x*sqrt(2.*m+3.)*pmm;
		if (l == (m+1))
			return pmmp1;
		else {
			oldfact = sqrt(2.*m+3.);
            for (ll=m+2;ll<=l;ll++)
            {
				fact = sqrt((4.*ll*ll-1.)/(ll*ll-m*m));
				pll=(x*pmmp1-pmm/oldfact)*fact;
				oldfact = fact;
				pmm=pmmp1;
				pmmp1=pll;
			}
			return pll;
		}
	}
}

//! Update Ground Station data
/*!
* Calculates aziumth and elevation for each gound station in the list of ground stations
* for a satellite at the indicated position.
	\param satellite pointer to a ::locstruc specifying satellite position
	\param groundstation pointer to a ::locstruc specifying groundstation to be targeted
	\return ::svector containing azimuth in lambda, elevation in phi
	and slant range in r.
	\see geoc2topo
	\see topo2azel
*/
svector groundstation(locstruc *satellite,locstruc *groundstation)
{
	rvector topo;
	svector azel = {0.,0.,0.};
	float lambda, phi;

	pos_icrf2eci(satellite);
	pos_eci2geoc(satellite);
	geoc2topo(groundstation->pos.geod.s,satellite->pos.geoc.s,&topo);
	topo2azel(topo,&lambda,&phi);
	azel.lambda = lambda;
	azel.phi = phi;
	azel.r = length_rv(topo);
	return (azel);
}

//! Initialize Hardware
/*! Set up the hardware simulation by initializing the various
 * components to their base values.
	\param stat Structure holding static specs on satellite
	\param dyn Structure holding dynamic specs on satellite
	\param loc Structure specifying location
*/
void hardware_init_eci(devspecstruc &devspec, locstruc &loc)
{
	int i;

	// Initialize power
//	cdata.node.battlev = cdata.node.battcap;
	for (i=0; i<devspec.bus_cnt; i++)
	{
		devspec.bus[i]->gen.amp = 0.;
		devspec.bus[i]->gen.flag |= DEVICE_FLAG_ON;
	}

	//! Initialize temperature sensors
	for (i=0; i<devspec.tsen_cnt; i++)
	{
		devspec.tsen[i]->gen.temp = 300.;
	}

	//! Reaction Wheels
	for (i=0; i<devspec.rw_cnt; i++)
	{
		devspec.rw[i]->gen.utc = loc.utc;
		devspec.rw[i]->omg = devspec.rw[i]->alp = 0.;
		devspec.rw[i]->romg = devspec.rw[i]->ralp = 0.;
	}

	//! Magnetic Torque Rods
	if (devspec.tcu_cnt)
	{
		devspec.tcu[0]->gen.utc = loc.utc;
		for (i=0; i<devspec.mtr_cnt; i++)
		{
			devspec.mtr[i]->gen.utc = loc.utc;
			devspec.mtr[i]->mom = 0.;
			devspec.mtr[i]->rmom = 0.;
		}
	}

	// Inertial Measurement Units
	for (i=0; i<devspec.imu_cnt; i++)
	{
		initialize_imu(i, devspec, loc);
		devspec.imu[i]->gen.utc = loc.utc;
	}

	//! Star Trackers
	for (i=0; i<devspec.stt_cnt; i++)
	{
		devspec.stt[i]->att = q_mult(devspec.stt[i]->align,q_conjugate(loc.att.icrf.s));
		devspec.stt[i]->omega = transform_q(q_conjugate(devspec.stt[i]->align),loc.att.icrf.v);
		devspec.stt[i]->gen.utc = loc.utc;
	}
}

//! Simulate Hardware data
/*! Simulate the behavior of all the hardware in the indicated satellite, at the
 * indicated location.
	\param stat Structure holding static specs on satellite
	\param dyn Structure holding dynamic specs on satellite
	\param loc Structure specifying location
*/
void simulate_hardware(cosmosstruc &cdata, locstruc &loc)
{
	double speed, density, sattemp;
	rvector vbody, vplanet;
	double adrag, edot, sdot, vdot, ddrag;
	double energy, watts, efficiency, energyd, dcharge, dheat;
	double sdheat;
	rvector geov, vec;
	rvector unitv, units, unite, dtorque, da;
	uint16_t i, j, index;
	rmatrix mom;
//	rmatrix tskew;
	rvector mag_moment;
	rvector mtorque, wtorque, wmomentum;
	rvector bearth;
	double tcexp, dalp, domg, dmom;
	quaternion tq;


	// Atmospheric and solar drag
	unitv = loc.pos.geoc.v;
	normalize_rv(&unitv);
	unitv = transform_q((loc.att.geoc.s),unitv);

	units = rv_smult(-1.,loc.pos.icrf.s);
	normalize_rv(&units);
	units = transform_q((loc.att.icrf.s),units);

	unite = rv_smult(-1.,loc.pos.eci.s);
	normalize_rv(&unite);
	unite = transform_q((loc.att.icrf.s),unite);

	geov = loc.pos.geoc.v;
	speed = geov.col[0]*geov.col[0]+geov.col[1]*geov.col[1]+geov.col[2]*geov.col[2];
	if (loc.pos.geod.s.h < 10000. || std::isnan(loc.pos.geod.s.h))
		density = 1.225;
	else
		density = 1000. * msis00_density(loc.pos,150.,150.,3.);
	//	density = 0.;
	adrag = density * 1.1 * speed;

	cdata.node.powgen = 0.;

	cdata.physics.atorque = cdata.physics.rtorque = rv_zero();
	cdata.physics.adrag = cdata.physics.rdrag = rv_zero();
	sdheat = 0;
	sattemp = cdata.physics.heat / (cdata.physics.mass * cdata.physics.hcap);
	for (i=0; i<cdata.piece.size(); i++)
	{
		cdata.piece[i].heat = cdata.piece[i].mass * cdata.piece[i].temp * cdata.piece[i].hcap;
		energyd = cdata.physics.dt * SIGMA * pow(cdata.piece[i].temp,4);
		dheat = (cdata.piece[i].temp-sattemp)/100000.*cdata.piece[i].heat;
		cdata.piece[i].heat -= dheat;
		sdheat += dheat;
		dheat = cdata.piece[i].emi*cdata.piece[i].area * energyd;
		cdata.piece[i].heat -= dheat;
		sdheat += dheat;

		if (cdata.piece[i].type == PIECE_TYPE_EXTERNAL_PANEL)
		{
			cdata.piece[i].heat -= dheat;
			vdot = dot_rv(unitv,cdata.piece[i].normal);
			if (vdot > 0)
			{
				ddrag = adrag*vdot/cdata.physics.mass;
				dtorque = rv_smult(ddrag,cdata.piece[i].twist);
				cdata.physics.atorque = rv_add(cdata.physics.atorque,dtorque);
				da = rv_smult(ddrag,cdata.piece[i].shove);
				cdata.physics.adrag = rv_add(cdata.physics.adrag,da);
			}

			sdot = dot_rv(units,cdata.piece[i].normal);
			if (loc.pos.sunradiance && sdot > 0)
			{
				ddrag = loc.pos.sunradiance * sdot / (3e8*cdata.physics.mass);
				dtorque = rv_smult(ddrag,cdata.piece[i].twist);
				cdata.physics.rtorque = rv_add(cdata.physics.rtorque,dtorque);
				da = rv_smult(ddrag,cdata.piece[i].shove);
				cdata.physics.rdrag = rv_add(cdata.physics.rdrag,da);

				cdata.piece[i].insol = loc.pos.sunradiance * sdot/length_rv(cdata.piece[i].normal);
				energyd =  cdata.piece[i].insol * cdata.physics.dt;
				cdata.piece[i].heat += cdata.piece[i].area * cdata.piece[i].abs * energyd;
				if (cdata.piece[i].cidx<DEVICE_TYPE_NONE && cdata.device[cdata.piece[i].cidx].all.gen.type == DEVICE_TYPE_STRG)
				{
					j = cdata.device[cdata.piece[i].cidx].all.gen.didx;
					if (cdata.devspec.strg[j]->effbase > 0.)
					{
						efficiency = cdata.devspec.strg[j]->effbase + cdata.devspec.strg[j]->effslope * cdata.piece[i].temp;
						cdata.devspec.strg[j]->power = cdata.piece[i].area*efficiency*cdata.piece[i].insol;
						cdata.device[cdata.devspec.strg[j]->gen.cidx].all.gen.volt = 35.;
						cdata.device[cdata.devspec.strg[j]->gen.cidx].all.gen.amp = -cdata.devspec.strg[j]->power / cdata.device[cdata.devspec.strg[j]->gen.cidx].all.gen.volt;
						cdata.node.powgen += .4 * cdata.devspec.strg[j]->power;
						cdata.piece[i].heat += (cdata.piece[i].abs * cdata.piece[i].area * cdata.piece[i].insol - cdata.devspec.strg[j]->power) * cdata.physics.dt;
						cdata.piece[i].heat -= cdata.piece[i].emi * cdata.piece[i].area * energyd;
					}
				}
			}
			else
			{
				for (j=0; j<cdata.devspec.strg_cnt; j++)
				{
					if (cdata.device[cdata.devspec.strg[j]->gen.cidx].all.gen.pidx == i)
					{
						cdata.devspec.strg[j]->power = 0.;
						cdata.device[cdata.devspec.strg[j]->gen.cidx].all.gen.volt = 0.;
						cdata.device[cdata.devspec.strg[j]->gen.cidx].all.gen.amp = 0.;
						cdata.piece[i].heat -= cdata.piece[i].emi * cdata.piece[i].area * energyd;
					}
				}
			}
			edot = acos(dot_rv(unite,cdata.piece[i].normal)/length_rv(cdata.piece[i].normal)) - RADOF(5.);
			if (edot < 0.)
				edot = 1.;
			else
				edot = cos(edot);
			if (edot > 0)
			{
				energyd = edot * cdata.physics.dt * SIGMA * pow(290.,4);
				cdata.piece[i].heat += cdata.piece[i].abs*cdata.piece[i].area * energyd;
				for (j=0; j<cdata.devspec.strg_cnt; j++)
				{
					if (cdata.device[cdata.devspec.strg[j]->gen.cidx].all.gen.pidx == i)
					{
						energy += cdata.piece[i].abs * cdata.piece[i].area * energyd;
					}
				}
			}
		}
	}

	// Simulate devices

	cdata.physics.ctorque = rv_zero();
	cdata.physics.hmomentum = rv_zero();
	cdata.physics.ctorque = cdata.physics.ftorque;

	// Start with Reaction Wheel Torque

	for (i=0; i<cdata.devspec.rw_cnt; i++)
	{
		//! Calculate change in Angular Momentum and therefore Acceleration

		// Moments of Reaction Wheel in Device frame
		mom = rm_diag(cdata.devspec.rw[i]->mom);

		// Torque and Momentum from reaction wheel

		// Calculate Momentum in Body frame and transform to ICRF
		//tskew = rm_skew(loc.att.icrf.v);

		wmomentum = rv_mmult(mom,rv_smult(cdata.devspec.rw[i]->omg,rv_unitz()));
		wmomentum = transform_q(cdata.devspec.rw[i]->align,wmomentum);
		cdata.physics.hmomentum = rv_add(cdata.physics.hmomentum,wmomentum);

		// Calculate Torque in Body frame and transform to ICRF
		wtorque = rv_mmult(mom,rv_smult(cdata.devspec.rw[i]->alp,rv_unitz()));
		wtorque = transform_q(cdata.devspec.rw[i]->align,wtorque);
		cdata.physics.ctorque = rv_sub(cdata.physics.ctorque,wtorque);

		// Term for exponential change
		tcexp = exp(-cdata.physics.dt/cdata.devspec.rw[i]->tc);

		// Keep alpha within allowed limits
		if (cdata.devspec.rw[i]->ralp > cdata.devspec.rw[i]->mxalp)
			cdata.devspec.rw[i]->ralp = cdata.devspec.rw[i]->mxalp;
		if (cdata.devspec.rw[i]->ralp < -cdata.devspec.rw[i]->mxalp)
			cdata.devspec.rw[i]->ralp = -cdata.devspec.rw[i]->mxalp;

		// Set alpha to zero if accelerating past limit
		if (cdata.devspec.rw[i]->omg < -cdata.devspec.rw[i]->mxomg)
		{
			if (cdata.devspec.rw[i]->ralp < 0.)
				cdata.devspec.rw[i]->ralp = 0.;
		}
		if (cdata.devspec.rw[i]->omg > cdata.devspec.rw[i]->mxomg)
		{
			if (cdata.devspec.rw[i]->ralp > 0.)
				cdata.devspec.rw[i]->ralp = 0.;
		}

		// Calculate change in acceleration effects
		if (cdata.devspec.rw[i]->ralp != cdata.devspec.rw[i]->alp)
		{
			dalp = tcexp * (cdata.devspec.rw[i]->alp - cdata.devspec.rw[i]->ralp);
			cdata.devspec.rw[i]->alp = cdata.devspec.rw[i]->ralp + dalp;
			domg = -cdata.devspec.rw[i]->tc * dalp;
		}
		else
			domg = 0.;

		//! Accelerate Reaction Wheel and calculate Component currents.
		cdata.devspec.rw[i]->omg += cdata.physics.dt * cdata.devspec.rw[i]->alp + domg;
		cdata.device[cdata.devspec.rw[i]->gen.cidx].all.gen.amp = .054 * fabs(cdata.devspec.rw[i]->omg)/400. + .093 * fabs(cdata.devspec.rw[i]->alp) / 30.;
		cdata.devspec.rw[i]->gen.utc = loc.utc;
	}

	// Determine magtorquer moments in body frame
	// Magtorquer Torque is Cross Product of B field and Moments
	mtorque = rv_zero();
	mag_moment = rv_zero();
	for (i=0; i<cdata.devspec.mtr_cnt; i++)
	{
		// Magnetic Moments in Body frame
		mag_moment = rv_add(mag_moment,transform_q(cdata.devspec.mtr[i]->align,rv_smult(cdata.devspec.mtr[i]->mom,rv_unitz())));

		tcexp = exp(-cdata.physics.dt/cdata.devspec.mtr[i]->tc);

		// Keep field within allowed limits
		if (cdata.devspec.mtr[i]->rmom > cdata.devspec.mtr[i]->mxmom)
			cdata.devspec.mtr[i]->rmom = cdata.devspec.mtr[i]->mxmom;
		if (cdata.devspec.mtr[i]->rmom < -cdata.devspec.mtr[i]->mxmom)
			cdata.devspec.mtr[i]->rmom = -cdata.devspec.mtr[i]->mxmom;

		if (cdata.devspec.mtr[i]->rmom != cdata.devspec.mtr[i]->mom)
		{
			dmom = tcexp * (cdata.devspec.mtr[i]->mom - cdata.devspec.mtr[i]->rmom);
			cdata.devspec.mtr[i]->mom = cdata.devspec.mtr[i]->rmom + dmom;
		}

		//! Component currents for each MTR
		if (cdata.devspec.mtr[i]->rmom < 0.)
		{
			cdata.device[cdata.devspec.mtr[i]->gen.cidx].all.gen.amp = cdata.devspec.mtr[i]->mom * (3.6519e-3 - cdata.devspec.mtr[i]->mom * 8.6439e-5);
		}
		else
		{
			cdata.device[cdata.devspec.mtr[i]->gen.cidx].all.gen.amp = cdata.devspec.mtr[i]->mom * (3.6519e-3 + cdata.devspec.mtr[i]->mom * 8.6439e-5);
		}
		//	cdata.devspec.mtr[i]->mom = cdata.device[cdata.devspec.mtr[i]->gen.cidx].all.gen.amp*(229.43-cdata.device[cdata.devspec.mtr[i]->gen.cidx].all.gen.amp*382.65);

		cdata.devspec.mtr[i]->gen.utc = loc.utc;
	}

	// Get magnetic field in body frame
	bearth = transform_q(loc.att.geoc.s,loc.bearth);

	mtorque = rv_cross(mag_moment,bearth);
//	mtorque = transform_q(q_conjugate(loc.att.icrf.s),mtorque);
	cdata.physics.ctorque = rv_add(cdata.physics.ctorque,mtorque);

	// Star Trackers
	for (i=0; i<cdata.devspec.stt_cnt; i++)
	{
		cdata.devspec.stt[i]->gen.utc = loc.utc;
		tq = q_mult(q_conjugate(cdata.devspec.stt[i]->align),loc.att.icrf.s);
		cdata.devspec.stt[i]->att = tq;
		cdata.devspec.stt[i]->omega = transform_q(tq,loc.att.icrf.v);
		cdata.devspec.stt[i]->gen.utc = loc.utc;
	}

	// Inertial Measurement Units
	for (i=0; i<cdata.devspec.imu_cnt; i++)
	{
		// convert attitude from ICRF to sensor body frame
		// att.s = 0th order derivative (quaternion)
		// embbed into a function
		// 'updates' cdata.devspec.imu[i] with noise modified data
		simulate_imu(i, cdata, loc);
		cdata.devspec.imu[i]->gen.utc = loc.utc;
	}

	for (i=0; i<cdata.devspec.gps_cnt; i++)
	{
		cdata.devspec.gps[i]->gen.utc = loc.utc;
		cdata.devspec.gps[i]->geocs = loc.pos.geoc.s;
		cdata.devspec.gps[i]->dgeocs = rv_one(5., 5., 5.);
		cdata.devspec.gps[i]->geocv = loc.pos.geoc.v;
		cdata.devspec.gps[i]->dgeocv = rv_one();
	}
	
	for (i=0; i<cdata.devspec.ssen_cnt; i++)
	{
		// Get sun vector in body frame
		vec = transform_q(loc.att.icrf.s,rv_smult(-1.,loc.pos.icrf.s));
		// Rotate into sun sensor frame
		vec = transform_q(q_conjugate(cdata.devspec.ssen[i]->align),vec);
		// Convert this to az and el
		topo2azel(vec,&cdata.devspec.ssen[i]->azimuth,&cdata.devspec.ssen[i]->elevation);
		if (cdata.devspec.ssen[i]->azimuth < 0.)
			cdata.devspec.ssen[i]->azimuth += D2PI;
		cdata.devspec.ssen[i]->qva = cdata.devspec.ssen[i]->qvb = cdata.devspec.ssen[i]->qvc = cdata.devspec.ssen[i]->qvd = 0.;
		if (cdata.devspec.ssen[i]->elevation > RADOF(70.))
		{
			cdata.devspec.ssen[i]->qva = cdata.devspec.ssen[i]->qvb = cdata.devspec.ssen[i]->qvc = cdata.devspec.ssen[i]->qvd = sin(cdata.devspec.ssen[i]->elevation);
		}
		else
		{
			if (cdata.devspec.ssen[i]->elevation > RADOF(30.))
			{
				switch ((int)(cdata.devspec.ssen[i]->azimuth/(DPI/4.)+.5))
				{
				case 0:
					cdata.devspec.ssen[i]->qva = sin(cdata.devspec.ssen[i]->elevation);
					break;
				case 1:
					cdata.devspec.ssen[i]->qva = sin(cdata.devspec.ssen[i]->elevation);
					cdata.devspec.ssen[i]->qvb = sin(cdata.devspec.ssen[i]->elevation);
					break;
				case 2:
					cdata.devspec.ssen[i]->qvb = sin(cdata.devspec.ssen[i]->elevation);
					break;
				case 3:
					cdata.devspec.ssen[i]->qvb = sin(cdata.devspec.ssen[i]->elevation);
					cdata.devspec.ssen[i]->qvc = sin(cdata.devspec.ssen[i]->elevation);
					break;
				case 4:
					cdata.devspec.ssen[i]->qvc = sin(cdata.devspec.ssen[i]->elevation);
					break;
				case 5:
					cdata.devspec.ssen[i]->qvc = sin(cdata.devspec.ssen[i]->elevation);
					cdata.devspec.ssen[i]->qvd = sin(cdata.devspec.ssen[i]->elevation);
					break;
				case 6:
					cdata.devspec.ssen[i]->qvd = sin(cdata.devspec.ssen[i]->elevation);
					break;
				case 7:
					cdata.devspec.ssen[i]->qvd = sin(cdata.devspec.ssen[i]->elevation);
					cdata.devspec.ssen[i]->qva = sin(cdata.devspec.ssen[i]->elevation);
					break;
				}
			}
		}
		cdata.devspec.ssen[i]->gen.utc = loc.utc;
	}

	// Simulate thsters
	for (i=0; i<cdata.devspec.thst_cnt; i++)
	{
		cdata.devspec.thst[i]->flw = (length_rv(cdata.physics.thrust) / cdata.devspec.thst_cnt) / cdata.devspec.thst[i]->isp;
		if (cdata.devspec.thst[i]->flw < .002)
			cdata.devspec.thst[i]->flw = 0.;
		cdata.devspec.prop[0]->lev -= cdata.physics.dt * cdata.devspec.thst[i]->flw;
		cdata.devspec.thst[i]->gen.utc = loc.utc;
	}

	// Simulate drive motors
	vbody = rv_zero();
	for (i=0; i<cdata.devspec.motr_cnt; i++)
	{
		cdata.device[cdata.devspec.motr[i]->gen.cidx].all.gen.amp = cdata.devspec.motr[i]->spd;
		vbody = rv_add(vbody,rv_smult(cdata.devspec.motr[i]->spd*cdata.devspec.motr[i]->rat,rv_unitx()));
		if (i == cdata.devspec.motr_cnt-1)
		{
			switch (loc.pos.extra.closest)
			{
			case COSMOS_EARTH:
				vplanet = transform_q(q_conjugate(loc.att.geoc.s),vbody);
				loc.pos.geoc.s = rv_add(loc.pos.geoc.s,vplanet);
				break;
			case COSMOS_MOON:
				vplanet = transform_q(q_conjugate(loc.att.selc.s),vbody);
				loc.pos.selc.s = rv_add(loc.pos.selc.s,vplanet);
				break;
			}
		}
		cdata.devspec.motr[i]->gen.utc = loc.utc;
	}

	// Power details
	cdata.node.powuse = 0.;
	for (i=0; i<cdata.devspec.bus_cnt; i++)
	{
		cdata.devspec.bus[i]->gen.amp = 0.;
	}

	for (i=0; i<cdata.node.device_cnt; i++)
	{
		index = cdata.device[i].all.gen.bidx;
		if (index >= cdata.devspec.bus_cnt)
			index = 0;
		if (cdata.devspec.bus_cnt && cdata.device[i].all.gen.flag&DEVICE_FLAG_ON && cdata.devspec.bus[index]->gen.flag&DEVICE_FLAG_ON)
		{
			cdata.devspec.bus[index]->gen.amp += cdata.device[i].all.gen.amp;
			if (cdata.device[i].all.gen.volt > cdata.devspec.bus[index]->gen.volt)
				cdata.devspec.bus[index]->gen.volt = cdata.device[i].all.gen.volt;
			watts = cdata.device[i].all.gen.amp * cdata.device[i].all.gen.volt;
			if (watts <= 0.)
				continue;
			if (cdata.device[i].all.gen.pidx < cdata.node.piece_cnt)
			{
				cdata.piece[cdata.device[i].all.gen.pidx].heat += .8 * watts * cdata.physics.dt;
			}
			cdata.node.powuse += watts;
		}
	}

	// Heat details
	cdata.physics.heat = 0.;
	for (i=0; i<cdata.piece.size(); i++)
	{
		cdata.piece[i].heat += sdheat*cdata.piece[i].mass/cdata.physics.mass;
		//	cdata.piece[i].heat += sdheat/cdata.piece.size();
		cdata.physics.heat += cdata.piece[i].heat;
		cdata.piece[i].temp = cdata.piece[i].heat / (cdata.piece[i].mass * cdata.piece[i].hcap);
	}

	for (i=0; i<cdata.devspec.tsen_cnt; i++)
	{
		cdata.devspec.tsen[i]->gen.temp = cdata.piece[cdata.device[cdata.devspec.tsen[i]->gen.cidx].all.gen.pidx].temp;
		cdata.devspec.tsen[i]->gen.utc = loc.utc;
	}

	dcharge = (cdata.physics.dt/3600.) * ((cdata.node.powgen-cdata.node.powuse)) / cdata.devspec.batt_cnt;
	for (i=0; i<cdata.devspec.batt_cnt; i++)
	{
		cdata.devspec.batt[i]->charge += dcharge;
		cdata.node.battlev += dcharge;
		if (cdata.devspec.batt[i]->charge > cdata.devspec.batt[i]->capacity)
			cdata.devspec.batt[i]->charge = cdata.devspec.batt[i]->capacity;
		if (cdata.devspec.batt[i]->charge < 0.)
			cdata.devspec.batt[i]->charge = 0.;
		cdata.devspec.batt[i]->gen.utc = loc.utc;
	}

	if (cdata.node.powgen > cdata.node.powuse)
		cdata.node.charging = 1;
	else
		cdata.node.charging = 0;

	if (cdata.node.battlev < 0.)
		cdata.node.battlev = 0.;

	if (cdata.node.battlev >= cdata.node.battcap)
	{
		cdata.node.charging = 0;
		cdata.node.battlev = cdata.node.battcap;
	}

}

//! Initialize IMU
/*! Set the indicated IMU to a base attitude and position. Use the
 * logic that the current attitude and position are equivalent to the
 * origin of the position frame and the identity attitude. Set
 * all velocities and accelerations to zero.
	\param stat Structure holding static specs on satellite
	\param dyn Structure holding dynamic specs on satellite
	\param loc Structure specifying location
	\param index Index of desired IMU.
*/
void initialize_imu(uint16_t index, devspecstruc &devspec, locstruc &)
{
	if (index >= devspec.imu_cnt)
		return;

}

//! Simulate IMU
/*! Turn the current attitude into likely values for the indicated
 * IMU. Inject any likely noise due to the nature of the IMU.
	\param stat Structure holding static specs on satellite
	\param dyn Structure holding dynamic specs on satellite
	\param loc Structure specifying location
	\param index Index of desired IMU.
*/
void simulate_imu(int index, cosmosstruc &cdata, locstruc &loc)
{
	quaternion toimu;

	if (index >= cdata.devspec.imu_cnt)
		return;

	toimu = q_mult(q_conjugate(cdata.devspec.imu[index]->align),loc.att.icrf.s);
	//! Set time of reading
	cdata.devspec.imu[index]->gen.utc = loc.utc;

	//! Set raw values for accelerometer and gyros
	cdata.devspec.imu[index]->accel = transform_q(toimu,loc.pos.icrf.a);
	cdata.devspec.imu[index]->omega = transform_q(toimu,loc.att.icrf.v);

	//! Set magnetic field in IMU frame
	cdata.devspec.imu[index]->mag = transform_q(q_conjugate(cdata.devspec.imu[index]->align),transform_q(loc.att.geoc.s,loc.bearth));
}

//! Attitude acceleration
/*! Calculate the torque forces on the specified satellite at the specified location/
	\param sat Structure specifying satellite.
	\param loc Structure specifying location.
*/
void att_accel(physicsstruc &physics, locstruc &loc)
{
	rvector ue, ta, tv;
	rvector ttorque;
	rmatrix mom;

	att_extra(&loc);

	ttorque = physics.ctorque;

	// Now calculate Gravity Gradient Torque
	// Unit vector towards earth, rotated into body frame
	ue = transform_q((loc.att.icrf.s),rv_smult(-1.,loc.pos.eci.s));
	normalize_rv(&ue);

	physics.gtorque = rv_smult((3.*GM/pow(loc.pos.geos.s.r,3.)),rv_cross(ue,rv_mult(physics.moi,ue)));

	ttorque = rv_add(ttorque,physics.gtorque);

	// Atmospheric and solar torque
//	ttorque = rv_add(ttorque,physics.atorque);
//	ttorque = rv_add(ttorque,physics.rtorque);

	// Torque from rotational effects

	// Moment of Inertia in Body frame
	mom = rm_diag(physics.moi);
	// Attitude rate in Body frame
	tv = transform_q(loc.att.icrf.s,loc.att.icrf.v);

	// Torque from cross product of angular velocity and angular momentum
	physics.htorque = rv_smult(-1., rv_cross(tv,rv_add(rv_mmult(mom,tv),physics.hmomentum)));
	ttorque = rv_add(ttorque,physics.htorque);

	// Convert torque into accelerations, doing math in Body frame

	// I x alpha = tau, so alpha = I inverse x tau
	ta = rv_mmult(rm_inverse(mom),ttorque);

	// Convert body frame acceleration back to other frames.
	loc.att.icrf.a = transform_q(q_conjugate(loc.att.icrf.s),ta);
	loc.att.topo.a = transform_q(q_conjugate(loc.att.topo.s),ta);
	loc.att.lvlh.a = transform_q(q_conjugate(loc.att.lvlh.s),ta);
	loc.att.geoc.a = transform_q(q_conjugate(loc.att.geoc.s),ta);
	loc.att.selc.a = transform_q(q_conjugate(loc.att.selc.s),ta);
}

//! Position acceleration
/*! Calculate the linear forces on the specified sattelite at the specified location/
	\param sat Structure specifying satellite.
	\param loc Structure specifying location.
*/
void pos_accel(physicsstruc &physics, locstruc &loc)
{
	double radius;
	rvector ctpos, da, tda;
	cartpos bodypos;

	radius = length_rv(loc.pos.eci.s);

	loc.pos.eci.a = rv_zero();

	// Earth gravity
	// Calculate Geocentric acceleration vector

	if (radius > REARTHM)
	{
		// Start with gravity vector in ITRS

		da = gravity_accel(loc.pos,GRAVITY_EGM2008_NORM,12);

		// Correct for earth rotation, polar motion, precession, nutation

		da = rv_mmult(loc.pos.extra.e2j,da);
	}
	else
	{
		// Simple 2 body
		da = rv_smult(-GM/(radius*radius*radius),loc.pos.eci.s);
	}
	loc.pos.eci.a = rv_add(loc.pos.eci.a,da);

	// Sun gravity
	// Calculate Satellite to Sun vector
	ctpos = rv_sub(rv_smult(-1.,loc.pos.extra.sun2earth.s),loc.pos.eci.s);
	radius = length_rv(ctpos);
	da = rv_smult(GSUN/(radius*radius*radius),ctpos);
	loc.pos.eci.a = rv_add(loc.pos.eci.a,da);

	// Adjust for acceleration of frame
	radius = length_rv(loc.pos.extra.sun2earth.s);
	da = rv_smult(GSUN/(radius*radius*radius),loc.pos.extra.sun2earth.s);
	tda = da;
	loc.pos.eci.a = rv_add(loc.pos.eci.a,da);

	// Moon gravity
	// Calculate Satellite to Moon vector
	bodypos.s = rv_sub(loc.pos.extra.sun2earth.s,loc.pos.extra.sun2moon.s);
	ctpos = rv_sub(bodypos.s,loc.pos.eci.s);
	radius = length_rv(ctpos);
	da = rv_smult(GMOON/(radius*radius*radius),ctpos);
	loc.pos.eci.a = rv_add(loc.pos.eci.a,da);

	// Adjust for acceleration of frame due to moon
	radius = length_rv(bodypos.s);
	da = rv_smult(GMOON/(radius*radius*radius),bodypos.s);
	tda = rv_sub(tda,da);
	loc.pos.eci.a = rv_sub(loc.pos.eci.a,da);

	/*
// Jupiter gravity
// Calculate Satellite to Jupiter vector
jplpos(JPL_EARTH,JPL_JUPITER,loc.pos.extra.tt,(cartpos *)&bodypos);
ctpos = rv_sub(bodypos.s,loc.pos.eci.s);
radius = length_rv(ctpos);

// Calculate acceleration
da = rv_smult(GJUPITER/(radius*radius*radius),ctpos);
//loc.pos.eci.a = rv_add(loc.pos.eci.a,da);
*/

	// Atmospheric drag
	if (length_rv(physics.adrag))
	{
		da = transform_q(q_conjugate(loc.att.icrf.s),physics.adrag);
		loc.pos.eci.a = rv_add(loc.pos.eci.a,da);
	}
	// Solar drag
	if (length_rv(physics.rdrag))
	{
		da = transform_q(q_conjugate(loc.att.icrf.s),physics.rdrag);
		loc.pos.eci.a = rv_add(loc.pos.eci.a,da);
	}
	// Fictitious drag
	if (length_rv(physics.fdrag))
	{
		da = transform_q(q_conjugate(loc.att.icrf.s),physics.fdrag);
		loc.pos.eci.a = rv_add(loc.pos.eci.a,da);
	}

	loc.pos.eci.pass++;
	pos_eci(&loc);
	if (std::isnan(loc.pos.eci.a.col[0]))
		loc.pos.eci.a.col[0] = 0.;
}

//! Calculate atmospheric density
/*! Calculate atmospheric density at indicated Latitute/Longitude/Altitude using the
 * NRLMSISE-00 atmospheric model.
	\param pos Structure indicating position
	\param f107avg Average 10.7 cm solar flux
	\param f107 Current 10.7 cm solar flux
	\param magidx Ap daily geomagnetic index
	\return Density in kg/m3
*/
double msis00_density(posstruc pos,float f107avg,float f107,float magidx)
{
	struct nrlmsise_output output;
	struct nrlmsise_input input;
	struct nrlmsise_flags flags = {
		{0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.},
		{0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.}};
	int year, month;
	double day, doy;
	static double lastmjd = 0.;
	static double lastdensity = 0.;
	static double lastperiod = 0.;

	if (lastperiod != 0.)
	{
		if (fabs(lastperiod) > (pos.extra.utc-lastmjd))
		{
			return (lastdensity*(1.+(.001*(pos.extra.utc-lastmjd)/lastperiod)));
		}
	}

	mjd2ymd(pos.extra.utc,year,month,day,doy);
	input.doy = (int32_t)doy;
	input.g_lat = pos.geod.s.lat*180./DPI;
	input.g_long = pos.geod.s.lon*180./DPI;
	input.alt = pos.geod.s.h / 1000.;
	input.sec = (doy - input.doy)*86400.;;
	input.lst = input.sec / 3600. + input.g_long / 15.;
	input.f107A = f107avg;
	input.f107 = f107;
	input.ap = magidx;
	gtd7d(&input,&flags,&output);

	if (lastdensity != 0. && lastdensity != output.d[5])
		lastperiod = (pos.extra.utc-lastmjd)*.001*output.d[5]/(output.d[5]-lastdensity);
	lastmjd = pos.extra.utc;
	lastdensity = output.d[5];
	return((double)output.d[5]);
}

void orbit_init_tle(int32_t mode,double dt,double utc,cosmosstruc &cdata)
{

	uint16_t i;

	// Munge time step to fit local granularity
	dt = 86400.*((utc + dt/86400.)-utc);

	cdata.physics.dt = dt;
	cdata.physics.dtj = cdata.physics.dt/86400.;
	cdata.physics.mode = mode;

	locstruc loc;
	pos_clear(loc);

	lines2eci(utc,cdata.tle,&loc.pos.eci);
	loc.pos.eci.pass++;
	pos_eci(&loc);

	// Initial attitude
	cdata.physics.ftorque = rv_zero();
	switch (cdata.physics.mode)
	{
	//case 0:
	//	loc.att.icrf.utc = loc.utc;
	//	loc.att.icrf.s = q_eye();
	//	loc.att.icrf.v = loc.att.icrf.a = rv_zero();
	//	att_icrf(&loc);
	//	break;
	case 1:
		loc.att.lvlh.utc = loc.utc;
		loc.att.lvlh.s = q_eye();
		loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&loc);
		break;
	case 2:
		loc.att.lvlh.utc = loc.utc;
		loc.att.lvlh.s = q_change_around_y(-DPI2);
		loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&loc);
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		loc.att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,loc.pos.icrf.s));
		//	loc.att.icrf.s = rm_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,loc.pos.icrf.s));
		loc.att.icrf.v = rv_zero();
		att_icrf2lvlh(&loc);
		break;
	}


	pos_accel(cdata.physics, loc);
    // Initialize hardware,
    // ?? is this only for simulation?
	hardware_init_eci(cdata.devspec,loc);

    // ?? check
	att_accel(cdata.physics, loc);
//	groundstations(cdata,&loc);

	sloc[0] = loc;

	// Position at t0-dt
	for (i=1; i<4; i++)
	{
		sloc[i] = sloc[i-1];
		sloc[i].utc -= dt / 86400.;
		lines2eci(sloc[i].utc,cdata.tle,&sloc[i].pos.eci);
		sloc[i].pos.eci.pass++;
		pos_eci(&sloc[i]);

		sloc[i].att.lvlh = sloc[i-1].att.lvlh;
		att_lvlh2icrf(&sloc[i]);

		// Initialize hardware
		pos_accel(cdata.physics, sloc[i]);
		simulate_hardware(cdata, sloc[i]);
		att_accel(cdata.physics, sloc[i]);
	}
	cdata.physics.mjdbase = loc.utc;
}

void orbit_init_eci(int32_t mode,double dt,double utc,cartpos ipos, cosmosstruc &cdata)
{
	kepstruc kep;
	double dea;
	uint16_t i;

	// Munge time step to fit local granularity
	dt = 86400.*((utc + dt/86400.)-utc);

	cdata.physics.dt = dt;
	cdata.physics.dtj = cdata.physics.dt/86400.;
	cdata.physics.mode = mode;

	locstruc loc;
	pos_clear(loc);

	loc.pos.eci = ipos;
	loc.utc = loc.att.icrf.utc = utc;
	loc.pos.eci.pass++;
	pos_eci(&loc);
	eci2kep(&loc.pos.eci,&kep);

	// Initial attitude
	cdata.physics.ftorque = rv_zero();
	switch (cdata.physics.mode)
	{
	//case 0:
	//	loc.att.icrf.utc = loc.utc;
	//	loc.att.icrf.s = q_eye();
	//	loc.att.icrf.v = loc.att.icrf.a = rv_zero();
	//	att_icrf(&loc);
	//	break;
	case 1:
		loc.att.lvlh.utc = loc.utc;
		loc.att.lvlh.s = q_eye();
		loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&loc);
		break;
	case 2:
		loc.att.lvlh.utc = loc.utc;
		loc.att.lvlh.s = q_change_around_y(-DPI2);
		loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&loc);
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		loc.att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,loc.pos.icrf.s));
		//	loc.att.icrf.s = rm_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,loc.pos.icrf.s));
		loc.att.icrf.v = rv_zero();
		att_icrf2lvlh(&loc);
		break;
	}


//	groundstations(cdata,&loc);
	pos_accel(cdata.physics, loc);
	// Initialize hardware
	hardware_init_eci(cdata.devspec, loc);
	att_accel(cdata.physics, loc);

	sloc[0] = loc;

	// Position at t0-dt
	for (i=1; i<4; i++)
	{
		sloc[i] = sloc[i-1];
		sloc[i].utc -= dt / 86400.;
		kep.ma -= dt * kep.mm;

		uint16_t count = 0;
		do
		{
			dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
			kep.ea -= dea;
		} while (++count < 100 && fabs(dea) > .000001);
		kep2eci(&kep,&sloc[i].pos.eci);
		sloc[i].pos.eci.pass++;
		pos_eci(&sloc[i]);

		sloc[i].att.lvlh = sloc[i-1].att.lvlh;
		att_lvlh2icrf(&sloc[i]);


		pos_accel(cdata.physics, sloc[i]);
		simulate_hardware(cdata, sloc[i]);
		att_accel(cdata.physics, sloc[i]);
	}
	cdata.physics.mjdbase = loc.utc;
}

void orbit_init_shape(int32_t mode,double dt,double utc,double altitude,double angle,double hour,cosmosstruc &cdata)
{
	double lon;

	lon = 2. * DPI * (fabs(hour)/24. - (utc - (int)utc));

	// Munge time step to fit local granularity
	dt = 86400.*((utc + dt/86400.)-utc);

	cdata.physics.dt = dt;
	cdata.physics.dtj = cdata.physics.dt/86400.;
	cdata.physics.mode = mode;
	initialutc = utc;

	locstruc loc;
	pos_clear(loc);

	// Initial position
	sloc[0] = sloc[1] = sloc[2] = loc;
	sloc[0].pos.geod.utc = sloc[0].att.geoc.utc = utc;
	sloc[0].pos.geod.s.h = altitude;
	sloc[0].pos.geos.s.r = rearth(0.) + altitude;
	sloc[0].pos.geod.s.lat = 0.;
	sloc[0].pos.geod.s.lon = lon;
	sloc[0].pos.geod.v.lat =  sin(angle) * sqrt(GM/sloc[0].pos.geos.s.r) / sloc[0].pos.geos.s.r;
	sloc[0].pos.geod.v.h = 0.;
	sloc[0].pos.geod.v.lon = cos(angle) * sqrt(GM/sloc[0].pos.geos.s.r) / sloc[0].pos.geos.s.r;
	if (hour < 0)
	{
		sloc[0].pos.geod.v.lat = -sloc[0].pos.geod.v.lat;
		sloc[0].pos.geod.v.lon = -sloc[0].pos.geod.v.lon;
	}
	sloc[0].pos.geod.v.lon -= DPI / 43200.;
	sloc[0].pos.geod.pass++;
	pos_geod(&sloc[0]);

	// Initial attitude
	cdata.physics.ftorque = rv_zero();
	switch (cdata.physics.mode)
	{
	case 0:
		sloc[0].att.icrf.utc =sloc[0].utc;
		sloc[0].att.icrf.s = q_eye();
		sloc[0].att.icrf.v = loc.att.icrf.a = rv_zero();
		att_icrf2lvlh(&sloc[0]);
		break;
	case 1:
		sloc[0].att.lvlh.utc = sloc[0].utc;
		sloc[0].att.lvlh.s = q_eye();
		sloc[0].att.lvlh.v = rv_zero();
		att_lvlh2icrf(&sloc[0]);
		break;
	case 2:
		sloc[0].att.lvlh.utc =sloc[0].utc;
		sloc[0].att.lvlh.s = q_change_around_y(-DPI2);
		sloc[0].att.lvlh.v = rv_zero();
		att_lvlh2icrf(&sloc[0]);
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		sloc[0].att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,sloc[0].pos.icrf.s));
		//	sloc[0].att.icrf.s = rm_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,loc.pos.icrf.s));
		sloc[0].att.icrf.v = rv_zero();
		att_icrf2lvlh(&sloc[0]);
		break;
	}

//	groundstations(cdata,&sloc[0]);
	pos_accel(cdata.physics, sloc[0]);
	// Initialize hardware
	hardware_init_eci(cdata.devspec, sloc[0]);
	att_accel(cdata.physics, sloc[0]);

	loc = sloc[0];

	// Position at t0-dt
	sloc[1].pos.geod.utc =  sloc[1].att.geoc.utc = sloc[0].pos.geod.utc - dt/86400.;
	sloc[1].pos.geod.s.lat = sloc[0].pos.geod.s.lat - dt * sloc[0].pos.geod.v.lat;
	sloc[1].pos.geod.v.lat = sloc[0].pos.geod.v.lat;
	sloc[1].pos.geod.s.lon = sloc[0].pos.geod.s.lon - dt * sloc[0].pos.geod.v.lon;
	sloc[1].pos.geod.v.lon = (sloc[0].pos.geod.v.lon + DPI / 43200.) * cos(sloc[0].pos.geod.s.lat) / cos(sloc[1].pos.geod.s.lat) - DPI / 43200.;
	sloc[1].pos.geod.s.h = sloc[0].pos.geos.s.r - rearth(sloc[1].pos.geod.s.lat);
	sloc[1].pos.geod.v.h = (sloc[0].pos.geod.s.h - sloc[1].pos.geod.s.h) / dt;
	pos_geod(&sloc[1]);

	// Attitude at t0-dt
	sloc[1].att.lvlh = sloc[0].att.lvlh;
	att_lvlh2icrf(&sloc[1]);

	cdata.node.battlev = cdata.node.battcap;

	pos_accel(cdata.physics, sloc[1]);
	simulate_hardware(cdata, sloc[1]);
	att_accel(cdata.physics, sloc[1]);

	// Position at t0-2*dt
	sloc[2].pos.geod.utc = sloc[2].att.geoc.utc = sloc[1].utc - dt/86400.;
	sloc[2].pos.geod.s.lat = sloc[1].pos.geod.s.lat - dt * sloc[1].pos.geod.v.lat;
	sloc[2].pos.geod.v.lat = sloc[1].pos.geod.v.lat;
	sloc[2].pos.geod.s.lon = sloc[1].pos.geod.s.lon - dt * sloc[1].pos.geod.v.lon;
	sloc[2].pos.geod.v.lon = (sloc[1].pos.geod.v.lon + DPI / 43200.) * cos(sloc[1].pos.geod.s.lat) / cos(sloc[2].pos.geod.s.lat) - DPI / 43200.;
	sloc[2].pos.geod.s.h = sloc[1].pos.geos.s.r - rearth(sloc[2].pos.geod.s.lat);
	sloc[2].pos.geod.v.h = (sloc[1].pos.geod.s.h - sloc[2].pos.geod.s.h) / dt;
	pos_geod(&sloc[2]);

	// Attitude at t0-2*dt
	sloc[2].att.lvlh = sloc[0].att.lvlh;
	att_lvlh2icrf(&sloc[2]);

	cdata.node.battlev = cdata.node.battcap;

	pos_accel(cdata.physics, sloc[2]);
	simulate_hardware(cdata, sloc[2]);
	att_accel(cdata.physics, sloc[2]);

	// Position at t0-3*dt
	sloc[3].pos.geod.utc = sloc[3].att.geoc.utc = sloc[2].utc - dt/86400.;
	sloc[3].pos.geod.s.lat = sloc[2].pos.geod.s.lat - dt * sloc[2].pos.geod.v.lat;
	sloc[3].pos.geod.v.lat = sloc[2].pos.geod.v.lat;
	sloc[3].pos.geod.s.lon = sloc[2].pos.geod.s.lon - dt * sloc[2].pos.geod.v.lon;
	sloc[3].pos.geod.v.lon = (sloc[2].pos.geod.v.lon + DPI / 43200.) * cos(sloc[2].pos.geod.s.lat) / cos(sloc[3].pos.geod.s.lat) - DPI / 43200.;
	sloc[3].pos.geod.s.h = sloc[2].pos.geos.s.r - rearth(sloc[3].pos.geod.s.lat);
	sloc[3].pos.geod.v.h = (sloc[2].pos.geod.s.h - sloc[3].pos.geod.s.h) / dt;
	pos_geod(&sloc[3]);

	// Attitude at t0-3*dt
	sloc[3].att.lvlh = sloc[0].att.lvlh;
	att_lvlh2icrf(&sloc[3]);

	cdata.node.battlev = cdata.node.battcap;

	pos_accel(cdata.physics, sloc[3]);
	simulate_hardware(cdata, sloc[3]);
	att_accel(cdata.physics, sloc[3]);
	cdata.physics.mjdbase = loc.utc;
}

void propagate(cosmosstruc &cdata, double utc)
{
	locstruc lnew, lnewp;
	rvector ds, unitp, unitx;
	quaternion dq;
	double angle;
	double lutc;
	uint32_t chunks, i, j;

	if (sloc[0].pos.geod.s.h < 100.)
	{
		return;
	}

	chunks = (uint32_t)(.5 + 86400.*(utc-sloc[0].utc)/cdata.physics.dt);
	if (chunks > 100000)
	{
		chunks = 100000;
	}
	lutc = sloc[0].pos.geoc.utc;
	for (i=0; i<chunks; ++i)
	{
		if (sloc[0].pos.geod.s.h < 100.)
		{
			break;
		}

		lnew = lnewp = sloc[0];

		lutc += (double)(cdata.physics.dt)/86400.;

		lnewp.utc = lnewp.att.icrf.utc = lutc;

		lnewp.pos.eci.s.col[0] += cdata.physics.dt * (lnewp.pos.eci.v.col[0] + cdata.physics.dt * (323.*sloc[0].pos.eci.a.col[0] - 264.*sloc[1].pos.eci.a.col[0] + 159.*sloc[2].pos.eci.a.col[0] - 38.*sloc[3].pos.eci.a.col[0]) / 360.);
		lnewp.pos.eci.s.col[1] += cdata.physics.dt * (lnewp.pos.eci.v.col[1] + cdata.physics.dt * (323.*sloc[0].pos.eci.a.col[1] - 264.*sloc[1].pos.eci.a.col[1] + 159.*sloc[2].pos.eci.a.col[1] - 38.*sloc[3].pos.eci.a.col[1]) / 360.);
		lnewp.pos.eci.s.col[2] += cdata.physics.dt * (lnewp.pos.eci.v.col[2] + cdata.physics.dt * (323.*sloc[0].pos.eci.a.col[2] - 264.*sloc[1].pos.eci.a.col[2] + 159.*sloc[2].pos.eci.a.col[2] - 38.*sloc[3].pos.eci.a.col[2]) / 360.);
		lnewp.pos.eci.v.col[0] += cdata.physics.dt * (55.*sloc[0].pos.eci.a.col[0] - 59.*sloc[1].pos.eci.a.col[0] + 37.*sloc[2].pos.eci.a.col[0] - 9.*sloc[3].pos.eci.a.col[0]) / 24.;
		lnewp.pos.eci.v.col[1] += cdata.physics.dt * (55.*sloc[0].pos.eci.a.col[1] - 59.*sloc[1].pos.eci.a.col[1] + 37.*sloc[2].pos.eci.a.col[1] - 9.*sloc[3].pos.eci.a.col[1]) / 24.;
		lnewp.pos.eci.v.col[2] += cdata.physics.dt * (55.*sloc[0].pos.eci.a.col[2] - 59.*sloc[1].pos.eci.a.col[2] + 37.*sloc[2].pos.eci.a.col[2] - 9.*sloc[3].pos.eci.a.col[2]) / 24.;
		lnewp.pos.eci.pass++;
		pos_eci(&lnewp);
		pos_accel(cdata.physics, lnewp);

		lnew.utc = lnew.att.icrf.utc = lutc;
		lnew.pos.eci.s.col[0] += cdata.physics.dt * (lnew.pos.eci.v.col[0] + cdata.physics.dt * (38.*lnewp.pos.eci.a.col[0] + 171.*sloc[0].pos.eci.a.col[0] - 36.*sloc[1].pos.eci.a.col[0] + 7.*sloc[2].pos.eci.a.col[0]) / 360.);
		lnew.pos.eci.s.col[1] += cdata.physics.dt * (lnew.pos.eci.v.col[1] + cdata.physics.dt * (38.*lnewp.pos.eci.a.col[1] + 171.*sloc[0].pos.eci.a.col[1] - 36.*sloc[1].pos.eci.a.col[1] + 7.*sloc[2].pos.eci.a.col[1]) / 360.);
		lnew.pos.eci.s.col[2] += cdata.physics.dt * (lnew.pos.eci.v.col[2] + cdata.physics.dt * (38.*lnewp.pos.eci.a.col[2] + 171.*sloc[0].pos.eci.a.col[2] - 36.*sloc[1].pos.eci.a.col[2] + 7.*sloc[2].pos.eci.a.col[2]) / 360.);
		lnew.pos.eci.v.col[0] += cdata.physics.dt * (9.*lnewp.pos.eci.a.col[0] + 19.*sloc[0].pos.eci.a.col[0] - 5.*sloc[1].pos.eci.a.col[0] + sloc[2].pos.eci.a.col[0]) / 24.;
		lnew.pos.eci.v.col[1] += cdata.physics.dt * (9.*lnewp.pos.eci.a.col[1] + 19.*sloc[0].pos.eci.a.col[1] - 5.*sloc[1].pos.eci.a.col[1] + sloc[2].pos.eci.a.col[1]) / 24.;
		lnew.pos.eci.v.col[2] += cdata.physics.dt * (9.*lnewp.pos.eci.a.col[2] + 19.*sloc[0].pos.eci.a.col[2] - 5.*sloc[1].pos.eci.a.col[2] + sloc[2].pos.eci.a.col[2]) / 24.;
		lnew.pos.eci.pass++;
		pos_eci(&lnew);
		pos_accel(cdata.physics, lnew);


		switch (cdata.physics.mode)
		{
		case 0:
			lnew.att.icrf.utc = lnew.utc;
			ds = rv_zero();
			ds.col[0] = cdata.physics.dt * (lnewp.att.icrf.v.col[0] + cdata.physics.dt * (323.*sloc[0].att.icrf.a.col[0] - 264.*sloc[1].att.icrf.a.col[0] + 159.*sloc[2].att.icrf.a.col[0] - 38.*sloc[3].att.icrf.a.col[0]) / 360.);
			ds.col[1] = cdata.physics.dt * (lnewp.att.icrf.v.col[1] + cdata.physics.dt * (323.*sloc[0].att.icrf.a.col[1] - 264.*sloc[1].att.icrf.a.col[1] + 159.*sloc[2].att.icrf.a.col[1] - 38.*sloc[3].att.icrf.a.col[1]) / 360.);
			ds.col[2] = cdata.physics.dt * (lnewp.att.icrf.v.col[2] + cdata.physics.dt * (323.*sloc[0].att.icrf.a.col[2] - 264.*sloc[1].att.icrf.a.col[2] + 159.*sloc[2].att.icrf.a.col[2] - 38.*sloc[3].att.icrf.a.col[2]) / 360.);
			dq = q_axis2quaternion_rv(rv_smult(.1,ds));
			for (j=0; j<10; ++j)
			{
				lnewp.att.icrf.s = q_mult(dq,lnewp.att.icrf.s);
			}
			lnewp.att.icrf.v.col[0] += cdata.physics.dt * (55.*sloc[0].att.icrf.a.col[0] - 59.*sloc[1].att.icrf.a.col[0] + 37.*sloc[2].att.icrf.a.col[0] - 9.*sloc[3].att.icrf.a.col[0]) / 24.;
			lnewp.att.icrf.v.col[1] += cdata.physics.dt * (55.*sloc[0].att.icrf.a.col[1] - 59.*sloc[1].att.icrf.a.col[1] + 37.*sloc[2].att.icrf.a.col[1] - 9.*sloc[3].att.icrf.a.col[1]) / 24.;
			lnewp.att.icrf.v.col[2] += cdata.physics.dt * (55.*sloc[0].att.icrf.a.col[2] - 59.*sloc[1].att.icrf.a.col[2] + 37.*sloc[2].att.icrf.a.col[2] - 9.*sloc[3].att.icrf.a.col[2]) / 24.;
			att_icrf2lvlh(&lnew);
			att_accel(cdata.physics, lnewp);

			ds = rv_zero();
			ds.col[0] = cdata.physics.dt * (lnew.att.icrf.v.col[0] + cdata.physics.dt * (38.*lnewp.att.icrf.a.col[0] + 171.*sloc[0].att.icrf.a.col[0] - 36.*sloc[1].att.icrf.a.col[0] + 7.*sloc[2].att.icrf.a.col[0]) / 3600.);
			ds.col[1] = cdata.physics.dt * (lnew.att.icrf.v.col[1] + cdata.physics.dt * (38.*lnewp.att.icrf.a.col[1] + 171.*sloc[0].att.icrf.a.col[1] - 36.*sloc[1].att.icrf.a.col[1] + 7.*sloc[2].att.icrf.a.col[1]) / 3600.);
			ds.col[2] = cdata.physics.dt * (lnew.att.icrf.v.col[2] + cdata.physics.dt * (38.*lnewp.att.icrf.a.col[2] + 171.*sloc[0].att.icrf.a.col[2] - 36.*sloc[1].att.icrf.a.col[2] + 7.*sloc[2].att.icrf.a.col[2]) / 3600.);
			dq = q_axis2quaternion_rv(rv_smult(.1,ds));
			for (j=0; j<10; ++j)
			{
				lnew.att.icrf.s = q_mult(dq,lnew.att.icrf.s);
			}
			lnew.att.icrf.v.col[0] += cdata.physics.dt * (9.*lnewp.att.icrf.a.col[0] + 19.*sloc[0].att.icrf.a.col[0] - 5.*sloc[1].att.icrf.a.col[0] + sloc[2].att.icrf.a.col[0]) / 24.;
			lnew.att.icrf.v.col[1] += cdata.physics.dt * (9.*lnewp.att.icrf.a.col[1] + 19.*sloc[0].att.icrf.a.col[1] - 5.*sloc[1].att.icrf.a.col[1] + sloc[2].att.icrf.a.col[1]) / 24.;
			lnew.att.icrf.v.col[2] += cdata.physics.dt * (9.*lnewp.att.icrf.a.col[2] + 19.*sloc[0].att.icrf.a.col[2] - 5.*sloc[1].att.icrf.a.col[2] + sloc[2].att.icrf.a.col[2]) / 24.;
			att_icrf2lvlh(&lnew);
			break;
		case 1:
			lnew.att.lvlh.utc = lnew.utc;
			lnew.att.lvlh.s = q_eye();
			lnew.att.lvlh.v = rv_zero();
			att_lvlh2icrf(&lnew);
			break;
		case 2:
			lnew.att.lvlh.utc = lnew.utc;
			lnew.att.lvlh.s = q_change_around_y(-DPI2);
			lnew.att.lvlh.v = rv_zero();
			att_lvlh2icrf(&lnew);
			break;
		case 3:
			lnew.att.icrf.utc = lnew.utc;
			lnew.att.icrf.s = q_change_between_rv(rv_unitz(),lnew.pos.eci.a);
			lnew.att.icrf.v = rv_zero();
			att_icrf2lvlh(&lnew);
			break;
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
			lnew.att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,lnew.pos.icrf.s));
			//		lnew.att.icrf.s = rm_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,lnew.pos.icrf.s));
			lnew.att.icrf.v = rv_zero();
			att_icrf2lvlh(&lnew);
			break;
		case 12:
			angle = (1440.*(lnew.utc - initialutc) - (int)(1440.*(lnew.utc - initialutc))) * 2.*DPI;
			unitx = rv_zero();
			unitx.col[0] = 1.;
			unitp = rv_zero();
			unitp.col[0] = cos(angle);
			unitp.col[1] = sin(angle);
			lnew.att.lvlh.s = q_change_between_rv(unitp,unitx);
			lnew.att.lvlh.v = rv_zero();
			lnew.att.lvlh.v.col[2] = 2.*DPI/1440.;
			att_lvlh2icrf(&lnew);
			break;
		}

//		groundstations(cdata,&lnew);
		cdata.node.battlev += (cdata.physics.dt/3600.) * (cdata.node.powgen-cdata.node.powuse);

		if (cdata.node.powgen > cdata.node.powuse)
			cdata.node.charging = 1;
		else
			cdata.node.charging = 0;

		if (cdata.node.battlev < 0.)
			cdata.node.battlev = 0.;

		if (cdata.node.battlev > cdata.node.battcap)
		{
			cdata.node.charging = 0;
			cdata.node.battlev = cdata.node.battcap;
		}
		// Simulate hardware values
		simulate_hardware(cdata, lnew);
		att_accel(cdata.physics, lnew);


		sloc[3] = sloc[2];
		sloc[2] = sloc[1];
		sloc[1] = sloc[0];
		sloc[0] = lnew;

	}

	cdata.node.loc = sloc[0];
}

//! Prepare for Gauss-Jackson integration
/*!
* Initializes Gauss-Jackson integration parameters for indicated
* order (must be even). Binomial coefficients are initialized first time through.
	\param order The order at which the integration will be performed
	\param dt Step size in seconds.
*/
void gauss_jackson_setup(gj_handle &gjh, uint32_t order, double utc, double &dt)
{
	uint32_t i, n, j,m,k;
	double test;

	gjh.step.resize(order+2);
	gjh.binom.resize(order+2);
	gjh.beta.resize(order+2);
	gjh.alpha.resize(order+2);
	for (i=0; i<order+2; ++i)
	{
		gjh.binom[i].resize(order+2);
		gjh.beta[i].resize(order+1);
		gjh.alpha[i].resize(order+1);
	}
	gjh.c.resize(order+3);
	gjh.gam.resize(order+2);
	gjh.q.resize(order+3);
	gjh.lam.resize(order+3);
	gjh.order = 0;

	// Munge time step to fit local granularity
	test = utc + dt/86400.;
	dt = 86400.*(test-utc);

	gjh.order = order;
	gjh.order2 = gjh.order/2;
	gjh.order = gjh.order2 * 2;
	gjh.dt = dt;
	gjh.dtsq = gjh.dt * gjh.dt;

	for (m=0; m<gjh.order+2; m++)
	{
		for (i=0; i<gjh.order+2; i++)
		{
			if (m > i)
				gjh.binom[m][i] = 0;
			else
			{
				if (m == i)
					gjh.binom[m][i] = 1;
				else
				{
					if (m == 0)
						gjh.binom[m][i] = 1;
					else
					{
						gjh.binom[m][i] = gjh.binom[m-1][i-1] + gjh.binom[m][i-1];
					}
				}
			}
		}
	}

	gjh.c[0] = 1.;
	for (n=1; n<gjh.order+3; n++)
	{
		gjh.c[n] = 0.;
		for (i=0; i<=n-1; i++)
		{
			gjh.c[n] -= gjh.c[i] / (n+1-i);
		}
	}

	gjh.gam[0] = gjh.c[0];
	for (i=1; i<gjh.order+2; i++)
	{
		gjh.gam[i] = gjh.gam[i-1] + gjh.c[i];
	}

	for (i=0; i<gjh.order+1; i++)
	{
		gjh.beta[gjh.order+1][i] = gjh.gam[i+1];
		gjh.beta[gjh.order][i] = gjh.c[i+1];
		for (j=gjh.order-1; j<gjh.order; --j)
		{
			if (!i)
				gjh.beta[j][i] = gjh.beta[j+1][i];
			else
				gjh.beta[j][i] = gjh.beta[j+1][i] - gjh.beta[j+1][i-1];
		}
	}

	gjh.q[0] = 1.;
	for (i=1; i<gjh.order+3; i++)
	{
		gjh.q[i] = 0.;
		for (k=0; k<=i; k++)
		{
			gjh.q[i] += gjh.c[k]*gjh.c[i-k];
		}
	}

	gjh.lam[0] = gjh.q[0];
	for (i=1; i<gjh.order+3; i++)
	{
		gjh.lam[i] = gjh.lam[i-1] + gjh.q[i];
	}

	for (i=0; i<gjh.order+1; i++)
	{
		gjh.alpha[gjh.order+1][i] = gjh.lam[i+2];
		gjh.alpha[gjh.order][i] = gjh.q[i+2];
		for (j=gjh.order-1; j<gjh.order; --j)
		{
			if (!i)
				gjh.alpha[j][i] = gjh.alpha[j+1][i];
			else
				gjh.alpha[j][i] = gjh.alpha[j+1][i] - gjh.alpha[j+1][i-1];
		}
	}

	for (j=0; j<gjh.order+2; j++)
	{
		for (m=0; m<gjh.order+1; m++)
		{
			gjh.step[j].a[gjh.order-m] = gjh.step[j].b[gjh.order-m] = 0.;
			for (i=m; i<=gjh.order; i++)
			{
				gjh.step[j].a[gjh.order-m] += gjh.alpha[j][i] * gjh.binom[m][i];
				gjh.step[j].b[gjh.order-m] += gjh.beta[j][i] * gjh.binom[m][i];
			}
			gjh.step[j].a[gjh.order-m] *= pow(-1.,m);
			gjh.step[j].b[gjh.order-m] *= pow(-1.,m);
			if (gjh.order-m == j)
				gjh.step[j].b[gjh.order-m] += .5;
		}
	}
}

//! Initialize Gauss-Jackson orbit using Two Line Elements
/*! Initializes Gauss-Jackson structures using starting time and position from a Two
 * Line Element set.
	\param order the order at which the integration will be performed (must be even)
	\param dt Step size in seconds
	\param utc Initial step time as UTC in Modified Julian Days
	\param sat Structure specifying satellite info
*/

void gauss_jackson_init_tle(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double utc, cosmosstruc &cdata, locstruc &loc)
{
	uint32_t i;

	cdata.physics.dt = dt;
	cdata.physics.mode = mode;
	gauss_jackson_setup(gjh, order, utc, cdata.physics.dt);
	cdata.physics.dtj = cdata.physics.dt/86400.;

	utc -= (order/2.)*dt/86400.;
	pos_clear(loc);
	lines2eci(utc,cdata.tle,&loc.pos.eci);
	loc.pos.eci.pass++;
	pos_eci(&loc);

	// Initial attitude
	cdata.physics.ftorque = rv_zero();
	switch (cdata.physics.mode)
	{
	//case 0:
	//	loc.att.icrf.utc = loc.utc;
	//	loc.att.icrf.s = q_eye();
	loc.att.icrf.a = rv_zero();
	//	att_icrf(&loc);
	//	break;
	case 1:
		loc.att.lvlh.utc = loc.utc;
		loc.att.lvlh.s = q_eye();
		loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&loc);
		break;
	case 2:
		loc.att.lvlh.utc = loc.utc;
		loc.att.lvlh.s = q_change_around_y(-DPI2);
		loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&loc);
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		loc.att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,loc.pos.icrf.s));
		loc.att.icrf.v = rv_zero();
		att_icrf2lvlh(&loc);
		break;
	}


	pos_accel(cdata.physics, loc);
	// Initialize hardware
	hardware_init_eci(cdata.devspec, loc);
	att_accel(cdata.physics, loc);
//	groundstations(cdata,&loc);

	gjh.step[gjh.order2].sloc = loc;

	// Position at t0-dt
	for (i=gjh.order2-1; i<gjh.order2; --i)
	{
		gjh.step[i].sloc = gjh.step[i+1].sloc;
		gjh.step[i].sloc.utc -= dt / 86400.;
		lines2eci(gjh.step[i].sloc.utc,cdata.tle,&gjh.step[i].sloc.pos.eci);
		gjh.step[i].sloc.pos.eci.pass++;
		pos_eci(&gjh.step[i].sloc);

		gjh.step[i].sloc.att.lvlh = gjh.step[i+1].sloc.att.lvlh;
		att_lvlh2icrf(&gjh.step[i].sloc);

		pos_accel(cdata.physics, gjh.step[i].sloc);
		// Initialize hardware
		hardware_init_eci(cdata.devspec,gjh.step[i].sloc);
		att_accel(cdata.physics, gjh.step[i].sloc);
	}

	for (i=gjh.order2+1; i<=gjh.order; i++)
	{
		gjh.step[i].sloc = gjh.step[i-1].sloc;
		gjh.step[i].sloc.utc += dt / 86400.;
		lines2eci(gjh.step[i].sloc.utc,cdata.tle,&gjh.step[i].sloc.pos.eci);
		gjh.step[i].sloc.pos.eci.pass++;
		pos_eci(&gjh.step[i].sloc);

		gjh.step[i].sloc.att.lvlh = gjh.step[i-1].sloc.att.lvlh;
		att_lvlh2icrf(&gjh.step[i].sloc);

		pos_accel(cdata.physics, gjh.step[i].sloc);
		// Initialize hardware
		hardware_init_eci(cdata.devspec,gjh.step[i].sloc);
		att_accel(cdata.physics, gjh.step[i].sloc);
	}

	loc = gauss_jackson_converge_orbit(gjh, cdata);
	gauss_jackson_converge_hardware(gjh, cdata);

	cdata.physics.mjdbase = loc.utc;
}

//! Initialize Gauss-Jackson orbit using ECI state vector
/*! Initializes Gauss-Jackson structures using supplied initial state vector.
	\param order the order at which the integration will be performed (must be even)
	\param dt Step size in seconds
	\param utc Initial step time as UTC in Modified Julian Days
	\param ipos Initial ECI Position
	\param iatt Initial ICRF Attitude
	\param sat Structure specifying satellite
*/
void gauss_jackson_init_eci(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double utc, cartpos ipos, qatt iatt, cosmosstruc &cdata)
{
	kepstruc kep;
	double dea;
	uint32_t i;
	quaternion q1;

	// dt is modified during setup
	gauss_jackson_setup(gjh, order, utc, dt);
	cdata.physics.dt = dt;
	cdata.physics.dtj = cdata.physics.dt/86400.;
	cdata.physics.mode = mode;

	pos_clear(cdata.node.loc);
	gjh.step[gjh.order+1].sloc = cdata.node.loc;
	ipos.pass = iatt.pass = 0;
	cdata.node.loc.pos.eci = ipos;
	cdata.node.loc.pos.eci.pass++;
	cdata.node.loc.utc = cdata.node.loc.pos.eci.utc= utc;
	pos_eci(&cdata.node.loc);

	// Initial attitude
	switch (cdata.physics.mode)
	{
	case 0:
		// Pure propagation
		cdata.node.loc.att.icrf = iatt;
		cdata.node.loc.att.icrf.pass++;
		att_icrf(&cdata.node.loc);
		break;
	case 1:
		// Force LVLH
		cdata.node.loc.att.lvlh.utc = cdata.node.loc.utc;
		cdata.node.loc.att.lvlh.s = q_eye();
		cdata.node.loc.att.lvlh.v = rv_zero();
		cdata.node.loc.att.lvlh.pass++;
		att_lvlh(&cdata.node.loc);
		break;
	case 2:
		// Force 90 degrees off LVLH
		cdata.node.loc.att.lvlh.utc = cdata.node.loc.utc;
		cdata.node.loc.att.lvlh.s = q_change_around_y(-DPI2);
		cdata.node.loc.att.lvlh.v = rv_zero();
		pos_eci2geoc(&cdata.node.loc);
		att_lvlh2icrf(&cdata.node.loc);
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		cdata.node.loc.att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,cdata.node.loc.pos.icrf.s));
		cdata.node.loc.att.icrf.v = rv_zero();
		pos_eci2geoc(&cdata.node.loc);
		att_icrf2lvlh(&cdata.node.loc);
		break;
	}


	pos_accel(cdata.physics, cdata.node.loc);

	att_accel(cdata.physics, cdata.node.loc);

	gjh.step[gjh.order2].sloc = cdata.node.loc;

	// Position at t0-dt
	eci2kep(&cdata.node.loc.pos.eci,&kep);
	for (i=gjh.order2-1; i<gjh.order2; --i)
	{
		gjh.step[i].sloc = gjh.step[i+1].sloc;
		gjh.step[i].sloc.utc -= dt / 86400.;
		kep.utc = gjh.step[i].sloc.att.icrf.utc = gjh.step[i].sloc.utc;
		kep.ma -= dt * kep.mm;

		uint16_t count = 0;
		do
		{
			dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
			kep.ea -= dea;
		} while (++count < 100 && fabs(dea) > .000001);
		kep2eci(&kep,&gjh.step[i].sloc.pos.eci);
		gjh.step[i].sloc.pos.eci.pass++;

		q1 = q_axis2quaternion_rv(rv_smult(-dt,gjh.step[i].sloc.att.icrf.v));
		gjh.step[i].sloc.att.icrf.s = q_mult(q1,gjh.step[i].sloc.att.icrf.s);
		q_normalize(&gjh.step[i].sloc.att.icrf.s);
		// Calculate new v from da
		gjh.step[i].sloc.att.icrf.v = rv_add(gjh.step[i].sloc.att.icrf.v,rv_smult(-dt,gjh.step[i].sloc.att.icrf.a));
//		gjh.step[i].sloc.att.icrf.utc -= dt/86400.;
//		att_icrf2lvlh(&gjh.step[i].sloc);
		pos_eci(&gjh.step[i].sloc);

		pos_accel(cdata.physics, gjh.step[i].sloc);
		// Initialize hardware
		hardware_init_eci(cdata.devspec,gjh.step[i].sloc);
		att_accel(cdata.physics, gjh.step[i].sloc);
	}

	eci2kep(&cdata.node.loc.pos.eci,&kep);
	for (i=gjh.order2+1; i<=gjh.order; i++)
	{
		gjh.step[i].sloc = gjh.step[i-1].sloc;
		gjh.step[i].sloc.utc += dt / 86400.;
		kep.utc = gjh.step[i].sloc.att.icrf.utc = gjh.step[i].sloc.utc;
		kep.ma += dt * kep.mm;

		uint16_t count = 0;
		do
		{
			dea = (kep.ea - kep.e * sin(kep.ea) - kep.ma) / (1. - kep.e * cos(kep.ea));
			kep.ea -= dea;
		} while (++count < 100 && fabs(dea) > .000001);
		kep2eci(&kep,&gjh.step[i].sloc.pos.eci);
		gjh.step[i].sloc.pos.eci.pass++;

		q1 = q_axis2quaternion_rv(rv_smult(dt,gjh.step[i].sloc.att.icrf.v));
		gjh.step[i].sloc.att.icrf.s = q_mult(q1,gjh.step[i].sloc.att.icrf.s);
		q_normalize(&gjh.step[i].sloc.att.icrf.s);
		// Calculate new v from da
		gjh.step[i].sloc.att.icrf.v = rv_add(gjh.step[i].sloc.att.icrf.v,rv_smult(dt,gjh.step[i].sloc.att.icrf.a));
//		gjh.step[i].sloc.att.icrf.utc += dt/86400.;
//		att_icrf2lvlh(&gjh.step[i].sloc);
		pos_eci(&gjh.step[i].sloc);

		pos_accel(cdata.physics, gjh.step[i].sloc);
		// Initialize hardware
		hardware_init_eci(cdata.devspec, gjh.step[i].sloc);
		att_accel(cdata.physics, gjh.step[i].sloc);
	}
	cdata.node.loc = gauss_jackson_converge_orbit(gjh, cdata);
	gauss_jackson_converge_hardware(gjh, cdata);
	cdata.physics.mjdbase = cdata.node.loc.utc;
}

void gauss_jackson_init_stk(gj_handle &gjh, uint32_t order,int32_t mode,double dt,double utc,stkstruc *stk, cosmosstruc &cdata, locstruc &loc)
{
	uint32_t i;

	cdata.physics.dt = dt;
	cdata.physics.mode = mode;
	gauss_jackson_setup(gjh, order, utc, cdata.physics.dt);
	cdata.physics.dtj = cdata.physics.dt/86400.;

	pos_clear(loc);
	gjh.step[gjh.order+1].sloc = loc;
	stk2eci(utc,stk,&loc.pos.eci);
	loc.att.icrf.utc = utc;
	loc.pos.eci.pass++;
	pos_eci(&loc);

	// Initial attitude
	cdata.physics.ftorque = rv_zero();
	switch (cdata.physics.mode)
	{
	//case 0:
	//	loc.att.icrf.utc = loc.utc;
	//	loc.att.icrf.s = q_eye();
	//	loc.att.icrf.v = loc.att.icrf.a = rv_zero();
	//	att_icrf(&loc);
	//	break;
	case 1:
		loc.att.lvlh.utc = loc.utc;
		loc.att.lvlh.s = q_eye();
		loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&loc);
		break;
	case 2:
		loc.att.lvlh.utc = loc.utc;
		loc.att.lvlh.s = q_change_around_y(-DPI2);
		loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&loc);
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		loc.att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,loc.pos.icrf.s));
		loc.att.icrf.v = rv_zero();
		att_icrf2lvlh(&loc);
		break;
	}


	// Initialize hardware
	hardware_init_eci(cdata.devspec, loc);
	att_accel(cdata.physics, loc);
//	groundstations(cdata,&loc);

	gjh.step[gjh.order2].sloc = loc;

	// Position at t0-dt
	for (i=gjh.order2-1; i<gjh.order2; --i)
	{
		gjh.step[i].sloc = gjh.step[i+1].sloc;
		gjh.step[i].sloc.utc -= dt / 86400.;
		gjh.step[i].sloc.att.icrf.utc = gjh.step[i].sloc.utc;
		stk2eci(gjh.step[i].sloc.utc,stk,&gjh.step[i].sloc.pos.eci);
		gjh.step[i].sloc.pos.eci.pass++;
		pos_eci(&gjh.step[i].sloc);

		gjh.step[i].sloc.att.lvlh = gjh.step[i+1].sloc.att.lvlh;
		att_lvlh2icrf(&gjh.step[i].sloc);

		// Initialize hardware
		hardware_init_eci(cdata.devspec,gjh.step[i].sloc);
		att_accel(cdata.physics, gjh.step[i].sloc);
	}

	for (i=gjh.order2+1; i<=gjh.order; i++)
	{
		gjh.step[i].sloc = gjh.step[i-1].sloc;
		gjh.step[i].sloc.utc += dt / 86400.;
		stk2eci(gjh.step[i].sloc.utc,stk,&gjh.step[i].sloc.pos.eci);
		gjh.step[i].sloc.pos.eci.pass++;
		pos_eci(&gjh.step[i].sloc);

		gjh.step[i].sloc.att.lvlh = gjh.step[i-1].sloc.att.lvlh;
		gjh.step[i].sloc.att.lvlh.utc = gjh.step[i].sloc.utc;
		att_lvlh2icrf(&gjh.step[i].sloc);

		// Initialize hardware
		hardware_init_eci(cdata.devspec,gjh.step[i].sloc);
		att_accel(cdata.physics, gjh.step[i].sloc);
	}

	loc = gauss_jackson_converge_orbit(gjh, cdata);
	gauss_jackson_converge_hardware(gjh, cdata);
	cdata.physics.mjdbase = loc.utc;
}

void gauss_jackson_init(gj_handle &gjh, uint32_t order, int32_t mode, double dt, double utc, double altitude, double angle, double hour, cosmosstruc &cdata, locstruc &loc)
{
	double lon;
	kepstruc kep;

	lon = D2PI * (fabs(hour)/24. - (utc - (int)utc));
	if (lon < -DPI)
		lon += D2PI;
	if (lon > DPI)
		lon -= D2PI;

	cdata.physics.dt = dt;
	cdata.physics.mode = mode;
	gauss_jackson_setup(gjh, order, utc, cdata.physics.dt);
	cdata.physics.dtj = cdata.physics.dt/86400.;
	initialutc = utc;

	// Initial position
	pos_clear(loc);
	kep.utc = utc;
	kep.a = REARTHM + altitude;
	kep.i = angle;
	kep.e = 0.;
	kep.ea = 0.;
	kep.ap = 0.;
	kep.raan = lon;
	kep2eci(&kep, &loc.pos.geoc);
	++loc.pos.geoc.pass;
	pos_geoc(&loc);

	loc.att.lvlh.s = q_eye();
	loc.att.lvlh.v = rv_zero();
	loc.att.lvlh.a = rv_zero();
	++loc.att.lvlh.pass;
	att_lvlh(&loc);

	gauss_jackson_init_eci(gjh, order, mode, cdata.physics.dt, utc, loc.pos.eci, loc.att.icrf, cdata);
	cdata.physics.mjdbase = loc.utc;
}

locstruc gauss_jackson_converge_orbit(gj_handle &gjh, cosmosstruc &cdata)
{
	uint32_t c_cnt, cflag=0, k, n, i;
	rvector oldsa;

	c_cnt = 0;
	do
	{
		gjh.step[gjh.order2].s.col[0] = gjh.step[gjh.order2].sloc.pos.eci.v.col[0]/gjh.dt;
		gjh.step[gjh.order2].s.col[1] = gjh.step[gjh.order2].sloc.pos.eci.v.col[1]/gjh.dt;
		gjh.step[gjh.order2].s.col[2] = gjh.step[gjh.order2].sloc.pos.eci.v.col[2]/gjh.dt;
		for (k=0; k<=gjh.order; k++)
		{
			gjh.step[gjh.order2].s.col[0] -= gjh.step[gjh.order2].b[k] * gjh.step[k].sloc.pos.eci.a.col[0];
			gjh.step[gjh.order2].s.col[1] -= gjh.step[gjh.order2].b[k] * gjh.step[k].sloc.pos.eci.a.col[1];
			gjh.step[gjh.order2].s.col[2] -= gjh.step[gjh.order2].b[k] * gjh.step[k].sloc.pos.eci.a.col[2];
		}
		for (n=1; n<=gjh.order2; n++)
		{
			gjh.step[gjh.order2+n].s.col[0] = gjh.step[gjh.order2+n-1].s.col[0] + (gjh.step[gjh.order2+n].sloc.pos.eci.a.col[0]+gjh.step[gjh.order2+n-1].sloc.pos.eci.a.col[0])/2;
			gjh.step[gjh.order2+n].s.col[1] = gjh.step[gjh.order2+n-1].s.col[1] + (gjh.step[gjh.order2+n].sloc.pos.eci.a.col[1]+gjh.step[gjh.order2+n-1].sloc.pos.eci.a.col[1])/2;
			gjh.step[gjh.order2+n].s.col[2] = gjh.step[gjh.order2+n-1].s.col[2] + (gjh.step[gjh.order2+n].sloc.pos.eci.a.col[2]+gjh.step[gjh.order2+n-1].sloc.pos.eci.a.col[2])/2;
			gjh.step[gjh.order2-n].s.col[0] = gjh.step[gjh.order2-n+1].s.col[0] - (gjh.step[gjh.order2-n].sloc.pos.eci.a.col[0]+gjh.step[gjh.order2-n+1].sloc.pos.eci.a.col[0])/2;
			gjh.step[gjh.order2-n].s.col[1] = gjh.step[gjh.order2-n+1].s.col[1] - (gjh.step[gjh.order2-n].sloc.pos.eci.a.col[1]+gjh.step[gjh.order2-n+1].sloc.pos.eci.a.col[1])/2;
			gjh.step[gjh.order2-n].s.col[2] = gjh.step[gjh.order2-n+1].s.col[2] - (gjh.step[gjh.order2-n].sloc.pos.eci.a.col[2]+gjh.step[gjh.order2-n+1].sloc.pos.eci.a.col[2])/2;
		}
		gjh.step[gjh.order2].ss.col[0] = gjh.step[gjh.order2].sloc.pos.eci.s.col[0]/gjh.dtsq;
		gjh.step[gjh.order2].ss.col[1] = gjh.step[gjh.order2].sloc.pos.eci.s.col[1]/gjh.dtsq;
		gjh.step[gjh.order2].ss.col[2] = gjh.step[gjh.order2].sloc.pos.eci.s.col[2]/gjh.dtsq;
		for (k=0; k<=gjh.order; k++)
		{
			gjh.step[gjh.order2].ss.col[0] -= gjh.step[gjh.order2].a[k] * gjh.step[k].sloc.pos.eci.a.col[0];
			gjh.step[gjh.order2].ss.col[1] -= gjh.step[gjh.order2].a[k] * gjh.step[k].sloc.pos.eci.a.col[1];
			gjh.step[gjh.order2].ss.col[2] -= gjh.step[gjh.order2].a[k] * gjh.step[k].sloc.pos.eci.a.col[2];
		}
		for (n=1; n<=gjh.order2; n++)
		{
			gjh.step[gjh.order2+n].ss.col[0] = gjh.step[gjh.order2+n-1].ss.col[0] + gjh.step[gjh.order2+n-1].s.col[0] + (gjh.step[gjh.order2+n-1].sloc.pos.eci.a.col[0])/2;
			gjh.step[gjh.order2+n].ss.col[1] = gjh.step[gjh.order2+n-1].ss.col[1] + gjh.step[gjh.order2+n-1].s.col[1] + (gjh.step[gjh.order2+n-1].sloc.pos.eci.a.col[1])/2;
			gjh.step[gjh.order2+n].ss.col[2] = gjh.step[gjh.order2+n-1].ss.col[2] + gjh.step[gjh.order2+n-1].s.col[2] + (gjh.step[gjh.order2+n-1].sloc.pos.eci.a.col[2])/2;
			gjh.step[gjh.order2-n].ss.col[0] = gjh.step[gjh.order2-n+1].ss.col[0] - gjh.step[gjh.order2-n+1].s.col[0] + (gjh.step[gjh.order2-n+1].sloc.pos.eci.a.col[0])/2;
			gjh.step[gjh.order2-n].ss.col[1] = gjh.step[gjh.order2-n+1].ss.col[1] - gjh.step[gjh.order2-n+1].s.col[1] + (gjh.step[gjh.order2-n+1].sloc.pos.eci.a.col[1])/2;
			gjh.step[gjh.order2-n].ss.col[2] = gjh.step[gjh.order2-n+1].ss.col[2] - gjh.step[gjh.order2-n+1].s.col[2] + (gjh.step[gjh.order2-n+1].sloc.pos.eci.a.col[2])/2;
		}

		for (n=0; n<=gjh.order; n++)
		{
			if (n == gjh.order2)
				continue;
			gjh.step[n].sb = gjh.step[n].sa = rv_zero();
			for (k=0; k<=gjh.order; k++)
			{
				gjh.step[n].sb.col[0] += gjh.step[n].b[k] * gjh.step[k].sloc.pos.eci.a.col[0];
				gjh.step[n].sa.col[0] += gjh.step[n].a[k] * gjh.step[k].sloc.pos.eci.a.col[0];
				gjh.step[n].sb.col[1] += gjh.step[n].b[k] * gjh.step[k].sloc.pos.eci.a.col[1];
				gjh.step[n].sa.col[1] += gjh.step[n].a[k] * gjh.step[k].sloc.pos.eci.a.col[1];
				gjh.step[n].sb.col[2] += gjh.step[n].b[k] * gjh.step[k].sloc.pos.eci.a.col[2];
				gjh.step[n].sa.col[2] += gjh.step[n].a[k] * gjh.step[k].sloc.pos.eci.a.col[2];
			}
		}

		for (n=1; n<=gjh.order2; n++)
		{
			for (i=-1; i<2; i+=2)
			{
				cflag = 0;

				// Save current acceleration for comparison with next iteration
				oldsa.col[0] = gjh.step[gjh.order2+i*n].sloc.pos.eci.a.col[0];
				oldsa.col[1] = gjh.step[gjh.order2+i*n].sloc.pos.eci.a.col[1];
				oldsa.col[2] = gjh.step[gjh.order2+i*n].sloc.pos.eci.a.col[2];

				// Calculate new probable position and velocity
				gjh.step[gjh.order2+i*n].sloc.pos.eci.v.col[0] = gjh.dt * (gjh.step[gjh.order2+i*n].s.col[0] + gjh.step[gjh.order2+i*n].sb.col[0]);
				gjh.step[gjh.order2+i*n].sloc.pos.eci.v.col[1] = gjh.dt * (gjh.step[gjh.order2+i*n].s.col[1] + gjh.step[gjh.order2+i*n].sb.col[1]);
				gjh.step[gjh.order2+i*n].sloc.pos.eci.v.col[2] = gjh.dt * (gjh.step[gjh.order2+i*n].s.col[2] + gjh.step[gjh.order2+i*n].sb.col[2]);
				gjh.step[gjh.order2+i*n].sloc.pos.eci.s.col[0] = gjh.dtsq * (gjh.step[gjh.order2+i*n].ss.col[0] + gjh.step[gjh.order2+i*n].sa.col[0]);
				gjh.step[gjh.order2+i*n].sloc.pos.eci.s.col[1] = gjh.dtsq * (gjh.step[gjh.order2+i*n].ss.col[1] + gjh.step[gjh.order2+i*n].sa.col[1]);
				gjh.step[gjh.order2+i*n].sloc.pos.eci.s.col[2] = gjh.dtsq * (gjh.step[gjh.order2+i*n].ss.col[2] + gjh.step[gjh.order2+i*n].sa.col[2]);

				// Perform conversions between different systems
				gjh.step[gjh.order2+i*n].sloc.pos.eci.pass++;
				pos_eci(&gjh.step[gjh.order2+i*n].sloc);
				att_icrf2lvlh(&gjh.step[gjh.order2+i*n].sloc);
				//		eci2earth(&gjh.step[gjh.order2+i*n].sloc.pos,&gjh.step[gjh.order2+i*n].sloc.att);

				// Calculate acceleration at new position
				pos_accel(cdata.physics, gjh.step[gjh.order2+i*n].sloc);
//				hardware_init_eci(cdata.devspec,gjh.step[gjh.order2+i*n].sloc);

				// Compare acceleration at new position to previous iteration
				if (fabs(oldsa.col[0]-gjh.step[gjh.order2+i*n].sloc.pos.eci.a.col[0])>1e-14 || fabs(oldsa.col[1]-gjh.step[gjh.order2+i*n].sloc.pos.eci.a.col[1])>1e-14 || fabs(oldsa.col[2]-gjh.step[gjh.order2+i*n].sloc.pos.eci.a.col[2])>1e-14)
					cflag = 1;
			}
		}
		c_cnt++;
	} while (c_cnt<10 && cflag);

	return gjh.step[gjh.order].sloc;
}

void gauss_jackson_converge_hardware(gj_handle &gjh, cosmosstruc &cdata)
{
	for (uint16_t i=0; i<=gjh.order; ++i)
	{
		simulate_hardware(cdata, gjh.step[i].sloc);
		att_accel(cdata.physics, gjh.step[i].sloc);
	}
}

void gauss_jackson_propagate(gj_handle &gjh, cosmosstruc &cdata, double tomjd)
{
	uint32_t i,chunks , astep;
	uint32_t j, k;
//	static quaternion qs[2]={{{0.,0.,0.},0.}};
//	static rvector alpha[2] = {{{0.,0.,0.}}};
//	static rvector omega[2] = {{{0.,0.,0.}}};
	quaternion q1, dsq, q2;
	dem_pixel val;
	rvector normal, unitv, unitx, unitp, unitp1, unitp2;
	static rvector lunitp1 = {{.1,.1,0.}};
	double angle;
	uvector utemp;
	double dtsave;
	rmatrix tskew;
	uvector tvector1;
	matrix2d tmatrix2;
	rvector tvector;

	// Don't bother if too low
	if (gjh.step[gjh.order].sloc.pos.geod.s.h < 100.)
	{
		return;
	}
//	if (qs[0].w == 0.)
//	{
//		qs[0] = qs[1] = loc.att.icrf.s;
//		alpha[0] = alpha[1] = loc.att.icrf.a;
//		omega[0] = omega[1] = loc.att.icrf.v;
//	}
	// Return immediately if we are trying to propagate earlier but dt is positive or vice versa
	if ((tomjd < gjh.step[gjh.order].sloc.utc && cdata.physics.dt > 0.) || (tomjd > gjh.step[gjh.order].sloc.utc && cdata.physics.dt < 0.))
	{
		return;
	}
	chunks = (uint32_t)(.5 + 86400.*(tomjd - gjh.step[gjh.order].sloc.utc)/cdata.physics.dt);
	if (chunks > 100000)
	{
		chunks = 100000;
	}
	for (i=0; i<chunks; i++)
	{
		if (gjh.step[gjh.order].sloc.pos.geod.s.h < 100.)
		{
			break;
		}

		 gjh.step[gjh.order+1].sloc.pos.eci.utc = gjh.step[gjh.order+1].sloc.utc = gjh.step[gjh.order].sloc.utc + (cdata.physics.dt)/86400.;

		// Calculate S(order/2+1)
		gjh.step[gjh.order+1].ss.col[0] = gjh.step[gjh.order].ss.col[0] + gjh.step[gjh.order].s.col[0] + gjh.step[gjh.order].sloc.pos.eci.a.col[0]/2.;
		gjh.step[gjh.order+1].ss.col[1] = gjh.step[gjh.order].ss.col[1] + gjh.step[gjh.order].s.col[1] + gjh.step[gjh.order].sloc.pos.eci.a.col[1]/2.;
		gjh.step[gjh.order+1].ss.col[2] = gjh.step[gjh.order].ss.col[2] + gjh.step[gjh.order].s.col[2] + gjh.step[gjh.order].sloc.pos.eci.a.col[2]/2.;

		// Calculate Sum(order/2+1) for a and b
		gjh.step[gjh.order+1].sb = gjh.step[gjh.order+1].sa = rv_zero();
		for (k=0; k<=gjh.order; k++)
		{
			gjh.step[gjh.order+1].sb.col[0] += gjh.step[gjh.order+1].b[k] * gjh.step[k].sloc.pos.eci.a.col[0];
			gjh.step[gjh.order+1].sa.col[0] += gjh.step[gjh.order+1].a[k] * gjh.step[k].sloc.pos.eci.a.col[0];
			gjh.step[gjh.order+1].sb.col[1] += gjh.step[gjh.order+1].b[k] * gjh.step[k].sloc.pos.eci.a.col[1];
			gjh.step[gjh.order+1].sa.col[1] += gjh.step[gjh.order+1].a[k] * gjh.step[k].sloc.pos.eci.a.col[1];
			gjh.step[gjh.order+1].sb.col[2] += gjh.step[gjh.order+1].b[k] * gjh.step[k].sloc.pos.eci.a.col[2];
			gjh.step[gjh.order+1].sa.col[2] += gjh.step[gjh.order+1].a[k] * gjh.step[k].sloc.pos.eci.a.col[2];
		}

		// Calculate pos.v(order/2+1)
		gjh.step[gjh.order+1].sloc.pos.eci.v.col[0] = gjh.dt * (gjh.step[gjh.order].s.col[0] + gjh.step[gjh.order].sloc.pos.eci.a.col[0]/2. + gjh.step[gjh.order+1].sb.col[0]);
		gjh.step[gjh.order+1].sloc.pos.eci.v.col[1] = gjh.dt * (gjh.step[gjh.order].s.col[1] + gjh.step[gjh.order].sloc.pos.eci.a.col[1]/2. + gjh.step[gjh.order+1].sb.col[1]);
		gjh.step[gjh.order+1].sloc.pos.eci.v.col[2] = gjh.dt * (gjh.step[gjh.order].s.col[2] + gjh.step[gjh.order].sloc.pos.eci.a.col[2]/2. + gjh.step[gjh.order+1].sb.col[2]);

		// Calculate pos.s(order/2+1)
		gjh.step[gjh.order+1].sloc.pos.eci.s.col[0] = gjh.dtsq * (gjh.step[gjh.order+1].ss.col[0] + gjh.step[gjh.order+1].sa.col[0]);
		gjh.step[gjh.order+1].sloc.pos.eci.s.col[1] = gjh.dtsq * (gjh.step[gjh.order+1].ss.col[1] + gjh.step[gjh.order+1].sa.col[1]);
		gjh.step[gjh.order+1].sloc.pos.eci.s.col[2] = gjh.dtsq * (gjh.step[gjh.order+1].ss.col[2] + gjh.step[gjh.order+1].sa.col[2]);
		gjh.step[gjh.order+1].sloc.pos.eci.pass++;
		pos_eci(&gjh.step[gjh.order+1].sloc);

		// Calculate att.s(order/2+1) + hardware
		gjh.step[gjh.order+1].sloc.att.icrf = gjh.step[gjh.order].sloc.att.icrf;
		gjh.step[gjh.order+1].sloc.att.icrf.utc = gjh.step[gjh.order].sloc.utc;
			switch (cdata.physics.mode)
			{
			case 0:
				// Calculate att.v(order/2+1)
				astep = 1 + (length_rv(gjh.step[gjh.order+1].sloc.att.icrf.v) * cdata.physics.dt) / .01;
				if (astep > 1000)
				{
					astep = 1000;
				}
				dtsave = cdata.physics.dt;
				cdata.physics.dt /= astep;
				gjh.step[gjh.order+1].sloc.utc = gjh.step[gjh.order].sloc.utc;
				simulate_hardware(cdata, gjh.step[gjh.order+1].sloc);
				att_accel(cdata.physics, gjh.step[gjh.order+1].sloc);
				for (k=0; k<astep; k++)
				{
					tvector = transform_q(gjh.step[gjh.order+1].sloc.att.icrf.s,rv_smult(cdata.physics.dt,gjh.step[gjh.order+1].sloc.att.icrf.v));
					tskew = rm_skew(tvector);
					tmatrix2.rows = tmatrix2.cols = 4;
					for (int l=0; l<3; ++l)
					{
					tmatrix2.array[3][l] = -tvector.col[l];
					tmatrix2.array[l][3] = tvector.col[l];
						for (int m=0; m<3; m++)
						{
							tmatrix2.array[l][m] = -tskew.row[l].col[m];
						}
					}
					tmatrix2.array[3][3] = 0.;
					tvector1.m1.cols = 4;
					tvector1.q = gjh.step[gjh.order+1].sloc.att.icrf.s;
					tvector1.m1 = m1_smult(.5,m1_mmult(tmatrix2,tvector1.m1));
					gjh.step[gjh.order+1].sloc.att.icrf.s = q_add(gjh.step[gjh.order+1].sloc.att.icrf.s,tvector1.q);

//					q1 = q_axis2quaternion_rv(rv_smult(cdata.physics.dt,gjh.step[gjh.order+1].sloc.att.icrf.v));
//					gjh.step[gjh.order+1].sloc.att.icrf.s = q_mult(q1,gjh.step[gjh.order+1].sloc.att.icrf.s);
					q_normalize(&gjh.step[gjh.order+1].sloc.att.icrf.s);

					// Calculate new v from da
					gjh.step[gjh.order+1].sloc.att.icrf.v = rv_add(gjh.step[gjh.order+1].sloc.att.icrf.v,rv_smult(cdata.physics.dt,gjh.step[gjh.order+1].sloc.att.icrf.a));
					gjh.step[gjh.order+1].sloc.utc += (cdata.physics.dt)/86400.;
					++gjh.step[gjh.order+1].sloc.att.icrf.pass;
					att_icrf(&gjh.step[gjh.order+1].sloc);
				}
					cdata.physics.dt = dtsave;
				break;
			case 1:
				// Force LVLH
				gjh.step[gjh.order+1].sloc.att.lvlh.utc = gjh.step[gjh.order+1].sloc.utc;
				gjh.step[gjh.order+1].sloc.att.lvlh.s = q_eye();
				gjh.step[gjh.order+1].sloc.att.lvlh.v = rv_zero();
				att_lvlh2icrf(&gjh.step[gjh.order+1].sloc);
				break;
			case 2:
				// Force surface normal (rover)
				gjh.step[gjh.order+1].sloc.att.topo.v = gjh.step[gjh.order+1].sloc.att.topo.a = rv_zero();
				switch (gjh.step[gjh.order+1].sloc.pos.extra.closest)
				{
				case COSMOS_EARTH:
				default:
					val = map_dem_pixel(COSMOS_EARTH,gjh.step[gjh.order+1].sloc.pos.geod.s.lon,gjh.step[gjh.order+1].sloc.pos.geod.s.lat,1./REARTHM);
					for (j=0; j<3; j++)
					{
						normal.col[j] = val.nmap[j];
					}
					unitv = rv_zero();
					unitv.col[0] = gjh.step[gjh.order+1].sloc.pos.geod.v.lon / cos(gjh.step[gjh.order+1].sloc.pos.geod.s.lat);
					unitv.col[1] = gjh.step[gjh.order+1].sloc.pos.geod.v.lat;
					break;
				case COSMOS_MOON:
					val = map_dem_pixel(COSMOS_MOON,gjh.step[gjh.order+1].sloc.pos.selg.s.lon,gjh.step[gjh.order+1].sloc.pos.selg.s.lat,1./RMOONM);
					for (j=0; j<3; j++)
					{
						normal.col[j] = -val.nmap[j];
					}
					unitv = rv_zero();
					unitv.col[0] = gjh.step[gjh.order+1].sloc.pos.selg.v.lon / cos(gjh.step[gjh.order+1].sloc.pos.selg.s.lat);
					unitv.col[1] = gjh.step[gjh.order+1].sloc.pos.selg.v.lat;
					break;
				}
				q1 = q_change_between_rv(rv_unitz(),normal);
				unitx = rv_cross(normal,rv_unity());
				unitx = transform_q(q1,unitx);
				q2 = q_change_between_rv(unitx,unitv);
				//		gjh.step[gjh.order+1].sloc.att.topo.s = q_mult(q2,q1);
				gjh.step[gjh.order+1].sloc.att.topo.s = q1;
				gjh.step[gjh.order+1].sloc.att.topo.utc = gjh.step[gjh.order+1].sloc.pos.utc+1.e8;
				gjh.step[gjh.order+1].sloc.att.topo.pass++;
				att_topo(&gjh.step[gjh.order+1].sloc);
				break;
			case 3:
				gjh.step[gjh.order+1].sloc.att.icrf.utc = gjh.step[gjh.order+1].sloc.utc;
				q1 = gjh.step[gjh.order+1].sloc.att.icrf.s;
				gjh.step[gjh.order+1].sloc.att.icrf.s = q_change_between_rv(cdata.physics.thrust,rv_unitz());
				dsq = q_sub(gjh.step[gjh.order+1].sloc.att.icrf.s,q1);
				angle = 2. * atan(length_q(dsq)/2.);
				q2 = q_smult(1./cos(angle),gjh.step[gjh.order+1].sloc.att.icrf.s);
				dsq = q_sub(q2,q1);
				utemp.q = q_smult(2.,q_mult(q_conjugate(q1),dsq));
				gjh.step[gjh.order+1].sloc.att.icrf.v = utemp.r;
				att_icrf2lvlh(&gjh.step[gjh.order+1].sloc);
				break;
			case 4:
				gjh.step[gjh.order+1].sloc.att.selc.utc = gjh.step[gjh.order+1].sloc.utc;
				unitp1.col[0] += .1*(gjh.step[gjh.order+1].sloc.pos.selg.v.lon-unitp1.col[0]);
				unitp1.col[1] += .1*(gjh.step[gjh.order+1].sloc.pos.selg.v.lat-unitp1.col[1]);
				unitp1.col[2] =  0.;
				if (length_rv(unitp1) < 1e-9)
					unitp1 = lunitp1;
				else
					lunitp1 = unitp1;
				q1 = q_change_between_rv(rv_unitx(),unitp1);
				val = map_dem_pixel(COSMOS_MOON,gjh.step[gjh.order+1].sloc.pos.selg.s.lon,gjh.step[gjh.order+1].sloc.pos.selg.s.lat,.0003);
				unitp2.col[0] += .1*(val.nmap[0]-unitp2.col[0]);
				unitp2.col[1] += .1*(val.nmap[1]-unitp2.col[1]);
				unitp2.col[2] += .1*(val.nmap[2]-unitp2.col[2]);
				q2 = q_change_between_rv(transform_q(q1,rv_unitz()),unitp2);
				gjh.step[gjh.order+1].sloc.att.selc.s = q_conjugate(q_mult(q2,q1));
				gjh.step[gjh.order+1].sloc.att.selc.v = rv_zero();
				att_selc2icrf(&gjh.step[gjh.order+1].sloc);
				break;
			case 5:
				gjh.step[gjh.order+1].sloc.att.geoc.utc = gjh.step[gjh.order+1].sloc.utc;
				angle = 2.*acos(gjh.step[gjh.order+1].sloc.att.geoc.s.w);
				gjh.step[gjh.order+1].sloc.att.geoc.s = q_change_around_z(angle+.2*D2PI*cdata.physics.dt);
				gjh.step[gjh.order+1].sloc.att.geoc.v = rv_smult(.2*D2PI,rv_unitz());
				att_geoc2icrf(&gjh.step[gjh.order+1].sloc);
				att_planec2lvlh(&gjh.step[gjh.order+1].sloc);
				break;
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				gjh.step[gjh.order+1].sloc.att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,gjh.step[gjh.order+1].sloc.pos.icrf.s));
				gjh.step[gjh.order+1].sloc.att.icrf.v = rv_zero();
				att_icrf2lvlh(&gjh.step[gjh.order+1].sloc);
				break;
			case 12:
				angle = (1440.*(gjh.step[gjh.order+1].sloc.utc - initialutc) - (int)(1440.*(gjh.step[gjh.order+1].sloc.utc - initialutc))) * 2.*DPI;
				unitx = unitp =  rv_zero();
				unitx.col[0] = 1.;
				unitp.col[0] = cos(angle);
				unitp.col[1] = sin(angle);
				gjh.step[gjh.order+1].sloc.att.lvlh.s = q_change_between_rv(unitp,unitx);
				gjh.step[gjh.order+1].sloc.att.lvlh.v = rv_zero();
				gjh.step[gjh.order+1].sloc.att.lvlh.v.col[2] = 2.*DPI/1440.;
				att_lvlh2icrf(&gjh.step[gjh.order+1].sloc);
				break;
			}
		simulate_hardware(cdata, gjh.step[gjh.order+1].sloc);
		att_accel(cdata.physics, gjh.step[gjh.order+1].sloc);
		// Perform positional and attitude accelerations at new position
		pos_accel(cdata.physics, gjh.step[gjh.order+1].sloc);

		// Calculate s(order/2+1)
		gjh.step[gjh.order+1].s.col[0] = gjh.step[gjh.order].s.col[0] + (gjh.step[gjh.order].sloc.pos.eci.a.col[0]+gjh.step[gjh.order+1].sloc.pos.eci.a.col[0])/2.;
		gjh.step[gjh.order+1].s.col[1] = gjh.step[gjh.order].s.col[1] + (gjh.step[gjh.order].sloc.pos.eci.a.col[1]+gjh.step[gjh.order+1].sloc.pos.eci.a.col[1])/2.;
		gjh.step[gjh.order+1].s.col[2] = gjh.step[gjh.order].s.col[2] + (gjh.step[gjh.order].sloc.pos.eci.a.col[2]+gjh.step[gjh.order+1].sloc.pos.eci.a.col[2])/2.;

		// Shift everything over 1
		for (j=0; j<=gjh.order; j++)
			gjh.step[j] = gjh.step[j+1];

	}

	cdata.node.loc = gjh.step[gjh.order].sloc;
}

//! Initialize orbit from orbital data
/*! Initializes satellite structure using orbital data
	\param order the order at which the integration will be performed (must be even)
	\param dt Step size in seconds
	\param utc Initial step time as UTC in Modified Julian Days. If set to 0., first time in the orbital data
	will be used.
	\param orbitfile Name of the file containing orbital data. Two Line Element set if first letter is 't',
	STK data if first letter is 's'.
	\param sat Structure specifying satellite info
	\return Returns 0 if succsessful, otherwise negative error.
*/

int orbit_init(int32_t mode, double dt, double utc, char *ofile, cosmosstruc &cdata)
{
	int32_t iretn;
	tlestruc tline;

	// Munge time step to fit local granularity
	dt = 86400.*((utc + dt/86400.)-utc);

	cdata.physics.dt = dt;
	cdata.physics.dtj = cdata.physics.dt/86400.;
	cdata.physics.mode = mode;

	pos_clear(cdata.node.loc);

	switch (ofile[0])
	{
	case 's':
		if ((iretn=load_stk(ofile,&stkhandle)) < 2)
			return (iretn);
		if (utc == 0.)
		{
			utc = stkhandle.pos[1].utc;
		}
		if ((iretn=stk2eci(utc,&stkhandle,&cdata.node.loc.pos.eci)) < 0)
			return (iretn);
		break;
	case 't':
		if ((iretn=load_lines(ofile, cdata.tle)) < 0)
			return (iretn);
		if (utc == 0.)
		{
			tline = get_line(0, cdata.tle);
			utc = tline.utc;
		}
		if ((iretn=lines2eci(utc,cdata.tle,&cdata.node.loc.pos.eci)) < 0)
			return (iretn);
		break;
	default:
		return (ORBIT_ERROR_NOTSUPPORTED);
		break;
	}

	strcpy(orbitfile,ofile);
	cdata.node.loc.pos.eci.pass++;
	pos_eci(&cdata.node.loc);

	// Initial attitude
	cdata.physics.ftorque = rv_zero();
	switch (cdata.physics.mode)
	{
	case 0:
		cdata.node.loc.att.icrf.utc = cdata.node.loc.utc;
		//	cdata.node.loc.att.icrf.s = q_eye();
		//	cdata.node.loc.att.icrf.v = rv_smult(1.,rv_unitz());
		att_icrf2lvlh(&cdata.node.loc);
		break;
	case 1:
		cdata.node.loc.att.lvlh.utc = cdata.node.loc.utc;
		cdata.node.loc.att.lvlh.s = q_eye();
		cdata.node.loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&cdata.node.loc);
		break;
	case 2:
		cdata.node.loc.att.topo.utc = cdata.node.loc.utc;
		cdata.node.loc.att.topo.s = q_eye();
		cdata.node.loc.att.topo.v = rv_zero();
		cdata.node.loc.att.topo.pass++;
		att_topo(&cdata.node.loc);
		break;
	case 3:
		cdata.node.loc.att.lvlh.utc = cdata.node.loc.utc;
		cdata.node.loc.att.lvlh.s = q_eye();
		cdata.node.loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&cdata.node.loc);
		break;
	case 4:
		break;
	case 5:
		cdata.node.loc.att.geoc.utc = cdata.node.loc.utc;
		cdata.node.loc.att.geoc.s = q_eye();
		cdata.node.loc.att.geoc.v = rv_smult(.2*D2PI,rv_unitz());
		att_geoc2icrf(&cdata.node.loc);
		att_planec2lvlh(&cdata.node.loc);
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		cdata.node.loc.att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,cdata.node.loc.pos.icrf.s));
		cdata.node.loc.att.icrf.v = rv_zero();
		att_icrf2lvlh(&cdata.node.loc);
		break;
	}


	// Initialize hardware
	hardware_init_eci(cdata.devspec,cdata.node.loc);
	att_accel(cdata.physics, cdata.node.loc);
//	groundstations(cdata,&cdata.node.loc);

	cdata.physics.mjdbase = cdata.node.loc.utc;
	return 0;
}

int orbit_propagate(cosmosstruc &cdata, double utc)
{
	cartpos npos;
	int32_t chunks, i, iretn;
	double nutc;

	chunks = (uint32_t)(.5 + 86400.*(utc-cdata.node.loc.utc)/cdata.physics.dt);
	if (chunks > 100000)
	{
		chunks = 100000;
	}
	nutc = cdata.node.loc.utc;
	for (i=0; i<chunks; i++)
	{
		nutc += cdata.physics.dt/86400.;
		switch (orbitfile[0])
		{
		case 's':
			if ((iretn=stk2eci(nutc,&stkhandle,&npos)) < 0)
				return (iretn);
			break;
		case 't':
			if ((iretn=lines2eci(nutc,cdata.tle,&npos)) < 0)
				return (iretn);
			break;
		default:
			return (ORBIT_ERROR_NOTSUPPORTED);
			break;
		}
		update_eci(cdata,nutc,npos);

	}

	return 0;
}

int update_eci(cosmosstruc &cdata, double utc, cartpos pos)
{
	quaternion dsq, q1, q2;
	uvector utemp;
	static rvector unitp1, unitp2, lunitp1;
	rvector unitp, unitx, normal, unitv;
	locstruc tloc;
	dem_pixel val;
	double angle;
	int j, k;

	cdata.node.loc.utc = utc;
	cdata.node.loc.pos.eci = pos;
	cdata.node.loc.pos.eci.pass++;
	pos_eci(&cdata.node.loc);
	if (cdata.physics.mode == PHYSICS_MODE_SURFACE)
	{
		switch (cdata.node.loc.pos.extra.closest)
		{
		case COSMOS_EARTH:
		default:
			cdata.node.loc.pos.geod.s.h = 0.5;
			cdata.node.loc.pos.geod.utc += 1e-8;
			pos_geod(&cdata.node.loc);
			break;
		case COSMOS_MOON:
			cdata.node.loc.pos.selg.s.h = 2.5;
			cdata.node.loc.pos.selg.utc += 1e-8;
			pos_selg(&cdata.node.loc);
			break;
		}
	}
	tloc = cdata.node.loc;
	pos_accel(cdata.physics, tloc);

	// Calculate probable thrust
	if (cdata.devspec.thst_cnt)
	{
		cdata.physics.thrust = rv_sub(cdata.node.loc.pos.eci.a,tloc.pos.eci.a);
		if (length_rv(cdata.physics.thrust) < 5.)
		{
			cdata.physics.thrust = rv_zero();
		}
		else
		{
			cdata.physics.thrust = rv_smult(cdata.physics.mass,cdata.physics.thrust);
		}
	}

	// Calculate probable motor motion
	if (cdata.devspec.motr_cnt)
	{

		for (j=0; j<cdata.devspec.motr_cnt; j++)
		{
			switch (cdata.node.loc.pos.extra.closest)
			{
			case COSMOS_EARTH:
			default:
				cdata.devspec.motr[j]->spd = length_rv(cdata.node.loc.pos.geoc.v)/cdata.devspec.motr[j]->rat;
				break;
			case COSMOS_MOON:
				cdata.devspec.motr[j]->spd = length_rv(cdata.node.loc.pos.selc.v)/cdata.devspec.motr[j]->rat;
				break;
			}
		}
	}

	switch (cdata.physics.mode)
	{
	case 0:
		cdata.node.loc.att.icrf.utc = cdata.node.loc.utc;
		for (k=0; k<10; k++)
		{
			q1 = q_axis2quaternion_rv(rv_smult(cdata.physics.dt/10.,cdata.node.loc.att.icrf.v));
			cdata.node.loc.att.icrf.s = q_mult(q1,cdata.node.loc.att.icrf.s);
			q_normalize(&cdata.node.loc.att.icrf.s);
			cdata.node.loc.att.icrf.v = rv_add(cdata.node.loc.att.icrf.v,rv_smult(cdata.physics.dt/10.,cdata.node.loc.att.icrf.a));
		}
		att_icrf2lvlh(&cdata.node.loc);
		break;
	case 1:
		cdata.node.loc.att.lvlh.utc = cdata.node.loc.utc;
		cdata.node.loc.att.lvlh.s = q_eye();
		cdata.node.loc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&cdata.node.loc);
		break;
	case 2:
		cdata.node.loc.att.topo.v = cdata.node.loc.att.topo.a = rv_zero();
		switch (cdata.node.loc.pos.extra.closest)
		{
		case COSMOS_EARTH:
		default:
			val = map_dem_pixel(COSMOS_EARTH,cdata.node.loc.pos.geod.s.lon,cdata.node.loc.pos.geod.s.lat,1./REARTHM);
			for (j=0; j<3; j++)
			{
				normal.col[j] = val.nmap[j];
			}
			unitv = rv_zero();
			unitv.col[0] = cdata.node.loc.pos.geod.v.lon / cos(cdata.node.loc.pos.geod.s.lat);
			unitv.col[1] = cdata.node.loc.pos.geod.v.lat;
			break;
		case COSMOS_MOON:
			val = map_dem_pixel(COSMOS_MOON,cdata.node.loc.pos.selg.s.lon,cdata.node.loc.pos.selg.s.lat,1./RMOONM);
			for (j=0; j<3; j++)
			{
				normal.col[j] = -val.nmap[j];
			}
			unitv = rv_zero();
			unitv.col[0] = cdata.node.loc.pos.selg.v.lon / cos(cdata.node.loc.pos.selg.s.lat);
			unitv.col[1] = cdata.node.loc.pos.selg.v.lat;
			break;
		}
		q1 = q_change_between_rv(rv_unitz(),normal);
		unitx = rv_cross(normal,rv_unity());
		unitx = transform_q(q1,unitx);
		q2 = q_change_between_rv(unitx,unitv);
		//		cdata.node.loc.att.topo.s = q_mult(q2,q1);
		cdata.node.loc.att.topo.s = q1;
		cdata.node.loc.att.topo.utc = cdata.node.loc.pos.utc+1.e8;
		cdata.node.loc.att.topo.pass++;
		att_topo(&cdata.node.loc);
		break;
	case 3:
		cdata.node.loc.att.icrf.utc = cdata.node.loc.utc;
		q1 = cdata.node.loc.att.icrf.s;
		cdata.node.loc.att.icrf.s = q_change_between_rv(cdata.physics.thrust,rv_unitz());
		dsq = q_sub(cdata.node.loc.att.icrf.s,q1);
		angle = 2. * atan(length_q(dsq)/2.);
		q2 = q_smult(1./cos(angle),cdata.node.loc.att.icrf.s);
		dsq = q_sub(q2,q1);
		utemp.q = q_smult(2.,q_mult(q_conjugate(q1),dsq));
		cdata.node.loc.att.icrf.v = utemp.r;
		att_icrf2lvlh(&cdata.node.loc);
		break;
	case 4:
		cdata.node.loc.att.selc.utc = cdata.node.loc.utc;
		unitp1.col[0] += .1*(cdata.node.loc.pos.selg.v.lon-unitp1.col[0]);
		unitp1.col[1] += .1*(cdata.node.loc.pos.selg.v.lat-unitp1.col[1]);
		unitp1.col[2] =  0.;
		if (length_rv(unitp1) < 1e-9)
			unitp1 = lunitp1;
		else
			lunitp1 = unitp1;
		q1 = q_change_between_rv(rv_unitx(),unitp1);
		val = map_dem_pixel(COSMOS_MOON,cdata.node.loc.pos.selg.s.lon,cdata.node.loc.pos.selg.s.lat,.0003);
		unitp2.col[0] += .1*(val.nmap[0]-unitp2.col[0]);
		unitp2.col[1] += .1*(val.nmap[1]-unitp2.col[1]);
		unitp2.col[2] += .1*(val.nmap[2]-unitp2.col[2]);
		q2 = q_change_between_rv(transform_q(q1,rv_unitz()),unitp2);
		cdata.node.loc.att.selc.s = q_conjugate(q_mult(q2,q1));
		cdata.node.loc.att.selc.v = rv_zero();
		att_selc2icrf(&cdata.node.loc);
		break;
	case 5:
		cdata.node.loc.att.geoc.utc = cdata.node.loc.utc;
		angle = 2.*acos(cdata.node.loc.att.geoc.s.w);
		cdata.node.loc.att.geoc.s = q_change_around_z(angle+.2*D2PI*cdata.physics.dt);
		cdata.node.loc.att.geoc.v = rv_smult(.2*D2PI,rv_unitz());
		att_geoc2icrf(&cdata.node.loc);
		att_planec2lvlh(&cdata.node.loc);
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		cdata.node.loc.att.icrf.s = q_change_between_rv(cdata.piece[cdata.physics.mode-2].normal,rv_smult(-1.,cdata.node.loc.pos.icrf.s));
		cdata.node.loc.att.icrf.v = rv_zero();
		att_icrf2lvlh(&cdata.node.loc);
		break;
	case 12:
		angle = (1440.*(cdata.node.loc.utc - initialutc) - (int)(1440.*(cdata.node.loc.utc - initialutc))) * 2.*DPI;
		unitx = unitp =  rv_zero();
		unitx.col[0] = 1.;
		unitp.col[0] = cos(angle);
		unitp.col[1] = sin(angle);
		cdata.node.loc.att.lvlh.s = q_change_between_rv(unitp,unitx);
		cdata.node.loc.att.lvlh.v = rv_zero();
		cdata.node.loc.att.lvlh.v.col[2] = 2.*DPI/1440.;
		att_lvlh2icrf(&cdata.node.loc);
		break;
	}

	// Perform positional and attitude accelerations at new position
	simulate_hardware(cdata, cdata.node.loc);
	att_accel(cdata.physics, cdata.node.loc);

	// Simulate at new position
//	groundstations(cdata,&cdata.node.loc);
	return 0;
}
