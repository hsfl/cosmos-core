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

/* PROGRAM MAGPOINT (GEOMAG DRIVER) */
/************************************************************************
	 Contact Information

	 Software and Model Support
		National Geophysical Data Center
		NOAA EGC/2
		325 Broadway
		Boulder, CO 80303 USA
		Attn: Susan McLean or Stefan Maus
		Phone:  (303) 497-6478 or -6522
		Email:  Susan.McLean@noaa.gov or Stefan.Maus@noaa.gov
		Web: http://www.ngdc.noaa.gov/seg/WMM/

	 Sponsoring Government Agency
	   National Geospatial-Intelligence Agency
		   PRG / CSAT, M.S. L-41
		   3838 Vogel Road
		   Arnold, MO 63010
		   Attn: Craig Rollins
		   Phone:  (314) 263-4186
		   Email:  Craig.M.Rollins@Nga.Mil

	  Original Program By:
		Dr. John Quinn
		FLEET PRODUCTS DIVISION, CODE N342
		NAVAL OCEANOGRAPHIC OFFICE (NAVOCEANO)
		STENNIS SPACE CENTER (SSC), MS 39522-5001

		3/25/05 Stefan Maus corrected 2 bugs:
		 - use %c instead of %s for character read
		 - help text: positive inclination is downward

	10 / 2008 Modified from interactive prompting of
	- input variables to a c routine with ultimate
	- goal of calling from a matlab routine
*/

#include "support/geomag.h"
#include "support/datalib.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

/* The following include file must define a function 'std::isnan' */
/* This function, which returns '1' if the number is NaN and 0*/
/* otherwise, could be hand-written if not available. */
/* Comment out one of the two following lines, as applicable */
#include <cmath>               /* for gcc */
//#include <mathimf.h>            /* for Intel icc */

#define NaN log(-1.0)

int32_t E0000(int IENTRY, int *maxdeg, float alt,float rlat,float rlon, float time, float *dec, float *dip, float *ti, float *gv);
int32_t geomag(int *maxdeg);
int32_t geomg1(float alt, float rlat, float rlon, float time, float *dec, float *dip, float *ti, float *gv, float *bx, float *by, float *bz);
char geomag_introduction(float epochlowlim);

static int initialized = 0;
static char fname[100];


//! Main function to compute the magnetic field from the
//! World Magnetic Model
/*! Input: Position, time | output: Mag Field
        \param pos geodetic position (lon, lat, alt) in (rad, rad, meters)
        \param time in decimal year, ex. use mjd2year(currentmjd())
        \param comp are the magnetic field x,y,z components in Topocentric System
*/
int32_t geomag_front(gvector pos, double time, rvector *comp)
{
	static int maxdeg, itime;
	static float alt,  dec, dip, ti, gv, bx, by, bz;

	// this reads the coefficients, specify path if neccessary
    itime = 5 * (int)(time/5.);
	string rname;
	int32_t iretn = get_cosmosresources(rname);
	if (iretn < 0)
	{
		return iretn;
	}
	sprintf(fname, "%s/general/wmm_%04d.cof", rname.c_str(), itime);

	/* INITIALIZE GEOMAG ROUTINE */


	maxdeg = 12;
	//	warn_H = 0;
	//	warn_H_val = 99999.0;
	//	warn_H_strong = 0;
	//	warn_H_strong_val = 99999.0;
	//	warn_P = 0;

	if (!initialized)
	{
		iretn = geomag(&maxdeg);
		if (iretn < 0)
		{
			return iretn;
		}
		initialized = 1;
	}


	// convert alt to km
	alt = (float)(pos.h/1000.);
    // !!! add comment/description to each filed
    iretn = geomg1(alt, // altitude in km
                   (float)pos.lat, // lattitude in rad?
                   (float)pos.lon, // longitude in rad?
                   (float)time, // time in decimal year
                   &dec,
                   &dip,
                   &ti,
                   &gv,
                   &bx,
                   &by,
                   &bz);
	if (iretn < 0)
	{
		return iretn;
	}
	//      time = time1 + 1.0;


	/*COMPUTE X, Y, Z, AND H COMPONENTS OF THE MAGNETIC FIELD*/
	//	ti *= 1e-9;
	//	comp->col[0] = ti*(cos(dec)*cos(dip));
	//	comp->col[1] = ti*(cos(dip)*sin(dec));
	//	comp->col[2] = ti*(sin(dip));
	comp->col[0] = bx*1e-9;
	comp->col[1] = by*1e-9;
	comp->col[2] = bz*1e-9;

	/*
	if (length_rv(lcomp) != 0. && lcomp.col[0] != comp->col[0] && lcomp.col[1] != comp->col[1] &&lcomp.col[2] != comp->col[2])
	{
		dcomp = rv_smult(1./(time-ltime),rv_sub(*comp,lcomp));
		lperiod = .001*length_rv(*comp)/length_rv(dcomp);
	}
	ltime = time;
	lcomp = *comp;
	*/
	return 0;
}
/*************************************************************************/

int32_t E0000(int IENTRY, int *maxdeg, float alt, float rlat, float rlon, float time, float *dec, float *dip, float *ti, float *gv, float *bx, float *by, float *bz)
{
	register int n,m,D3,D4;
	static int maxord,i,icomp,j,D1,D2,in,im;
	static float c[13][13],cd[13][13],tc[13][13],dp[13][13],snorm[169],
			sp[13],cp[13],fn[13],fm[13],pp[13],k[13][13],dtr,a,b,re,
			a2,b2,c2,a4,b4,c4,epoch=0.,gnm,hnm,dgnm,dhnm,flnmj,otime,oalt,
			olat,olon,dt,glon,glat,srlon,srlat,crlon,crlat,srlat2,
			crlat2,q,q1,q2,ct,st,r2,r,d,ca,sa,aor,ar,br,bt,bp,bpp,
			par,temp1,temp2,parp,bh;
	static char model[20], c_str[81], c_new[5];
	static float *p = snorm;
	//char answer;

	FILE *wmmdat;
	char* ichar;

	switch(IENTRY){case 0: goto GEOMAG; case 1: goto GEOMG1;}

GEOMAG:
	if ((wmmdat=fopen(fname,"r")) == NULL)
	{
		return GEOMAG_ERROR_NOTFOUND;
	}


	/* INITIALIZE CONSTANTS */
	maxord = *maxdeg;
	sp[0] = 0.0;
	cp[0] = *p = pp[0] = 1.0;
	dp[0][0] = 0.0;
	a = (float)6378.137;
	b = (float)6356.7523142;
	re = (float)6371.2;
	a2 = a*a;
	b2 = b*b;
	c2 = a2-b2;
	a4 = a2*a2;
	b4 = b2*b2;
	c4 = a4 - b4;

	/* READ WORLD MAGNETIC MODEL SPHERICAL HARMONIC COEFFICIENTS */
	c[0][0] = 0.0;
	cd[0][0] = 0.0;

	ichar = fgets(c_str, 80, wmmdat);
	if (ichar != NULL)
	{
		sscanf(c_str,"%f%s",&epoch,model);
	}
S3:
	ichar = fgets(c_str, 80, wmmdat);
	/* CHECK FOR LAST LINE IN FILE */
	for (i=0; i<4 && (c_str[i] != '\0'); i++)
	{
		c_new[i] = c_str[i];
		c_new[i+1] = '\0';
	}
	icomp = strcmp("9999", c_new);
	if (icomp == 0) goto S4;
	/* END OF FILE NOT ENCOUNTERED, GET VALUES */
	sscanf(c_str,"%d%d%f%f%f%f",&in,&im,&gnm,&hnm,&dgnm,&dhnm);
	if (im <= in)
	{
		c[im][in] = gnm;
		cd[im][in] = dgnm;
		if (im != 0)
		{
			c[in][im-1] = hnm;
			cd[in][im-1] = dhnm;
		}
	}
	goto S3;

	/* CONVERT SCHMIDT NORMALIZED GAUSS COEFFICIENTS TO UNNORMALIZED */
S4:
	*snorm = 1.0;
	for (n=1; n<=maxord; n++)
	{
		*(snorm+n) = *(snorm+n-1)*(float)(2*n-1)/(float)n;
		j = 2;
		for (m=0,D1=1,D2=(n-m+D1)/D1; D2>0; D2--,m+=D1)
		{
			k[m][n] = (float)(((n-1)*(n-1))-(m*m))/(float)((2*n-1)*(2*n-3));
			if (m > 0)
			{
				flnmj = (float)((n-m+1)*j)/(float)(n+m);
				*(snorm+n+m*13) = *(snorm+n+(m-1)*13)*sqrt(flnmj);
				j = 1;
				c[n][m-1] = *(snorm+n+m*13)*c[n][m-1];
				cd[n][m-1] = *(snorm+n+m*13)*cd[n][m-1];
			}
			c[m][n] = *(snorm+n+m*13)*c[m][n];
			cd[m][n] = *(snorm+n+m*13)*cd[m][n];
		}
		fn[n] = (float)(n+1);
		fm[n] = (float)n;
	}
	k[1][1] = 0.0;

	otime = oalt = olat = olon = -1000.0;
	fclose(wmmdat);
	return 0;

	/*************************************************************************/

GEOMG1:
	dt = time - epoch;
	if (otime < 0.0 && (dt < 0.0 || dt > 15.0))
	{
		/*
		printf("\n\n WARNING - TIME EXTENDS BEYOND MODEL 5-YEAR LIFE SPAN");
		printf("\n CONTACT NGDC FOR PRODUCT UPDATES:");
		printf("\n         National Geophysical Data Center");
		printf("\n         NOAA EGC/2");
		printf("\n         325 Broadway");
		printf("\n         Boulder, CO 80303 USA");
		printf("\n         Attn: Susan McLean or Stefan Maus");
		printf("\n         Phone:  (303) 497-6478 or -6522");
		printf("\n         Email:  Susan.McLean@noaa.gov");
		printf("\n         or");
		printf("\n         Stefan.Maus@noaa.gov");
		printf("\n         Web: http://www.ngdc.noaa.gov/seg/WMM/");
		printf("\n\n EPOCH  = %.3f",epoch);
		printf("\n TIME   = %.3f",time);
		*/
		*bx = *by =*bz = *dec = *dip = *ti = *gv = 0.;
		return GEOMAG_ERROR_OUTOFRANGE;
		/*
		printf("\n Do you wish to continue? (y or n) ");
		scanf("%c%*[^\n]",&answer);
		getchar();
		if ((answer == 'n') || (answer == 'N'))
		{
			printf("\n Do you wish to enter more point data? (y or n) ");
			scanf("%c%*[^\n]",&answer);
			getchar();
			if ((answer == 'y')||(answer == 'Y')) goto GEOMG1;
			else exit (0);
		}
		*/
	}

	dtr = (float)(DPI/180.0);
	glon = rlon/dtr;
	glat = rlat/dtr;
	srlon = sin(rlon);
	srlat = sin(rlat);
	crlon = cos(rlon);
	crlat = cos(rlat);
	srlat2 = srlat*srlat;
	crlat2 = crlat*crlat;
	sp[1] = srlon;
	cp[1] = crlon;

	/* CONVERT FROM GEODETIC COORDS. TO SPHERICAL COORDS. */
	if (alt != oalt || glat != olat)
	{
		q = sqrt(a2-c2*srlat2);
		q1 = alt*q;
		q2 = ((q1+a2)/(q1+b2))*((q1+a2)/(q1+b2));
		ct = srlat/sqrt(q2*crlat2+srlat2);
		st = (float)sqrt(1.0-(ct*ct));
		r2 = (float)((alt*alt)+2.0*q1+(a4-c4*srlat2)/(q*q));
		r = sqrt(r2);
		d = sqrt(a2*crlat2+b2*srlat2);
		ca = (alt+d)/r;
		sa = c2*crlat*srlat/(r*d);
	}
	if (glon != olon)
	{
		for (m=2; m<=maxord; m++)
		{
			sp[m] = sp[1]*cp[m-1]+cp[1]*sp[m-1];
			cp[m] = cp[1]*cp[m-1]-sp[1]*sp[m-1];
		}
	}
	aor = re/r;
	ar = aor*aor;
	br = bt = bp = bpp = 0.0;
	for (n=1; n<=maxord; n++)
	{
		ar = ar*aor;
		for (m=0,D3=1,D4=(n+m+D3)/D3; D4>0; D4--,m+=D3)
		{
			/*
   COMPUTE UNNORMALIZED ASSOCIATED LEGENDRE POLYNOMIALS
   AND DERIVATIVES VIA RECURSION RELATIONS
*/
			if (alt != oalt || glat != olat)
			{
				if (n == m)
				{
					*(p+n+m*13) = st**(p+n-1+(m-1)*13);
					dp[m][n] = st*dp[m-1][n-1]+ct**(p+n-1+(m-1)*13);
					goto S50;
				}
				if (n == 1 && m == 0)
				{
					*(p+n+m*13) = ct**(p+n-1+m*13);
					dp[m][n] = ct*dp[m][n-1]-st**(p+n-1+m*13);
					goto S50;
				}
				if (n > 1 && n != m)
				{
					if (m > n-2) *(p+n-2+m*13) = 0.0;
					if (m > n-2) dp[m][n-2] = 0.0;
					*(p+n+m*13) = ct**(p+n-1+m*13)-k[m][n]**(p+n-2+m*13);
					dp[m][n] = ct*dp[m][n-1] - st**(p+n-1+m*13)-k[m][n]*dp[m][n-2];
				}
			}
S50:
			/*
	TIME ADJUST THE GAUSS COEFFICIENTS
*/
			if (time != otime)
			{
				tc[m][n] = c[m][n]+dt*cd[m][n];
				if (m != 0) tc[n][m-1] = c[n][m-1]+dt*cd[n][m-1];
			}
			/*
	ACCUMULATE TERMS OF THE SPHERICAL HARMONIC EXPANSIONS
*/
			par = ar**(p+n+m*13);
			if (m == 0)
			{
				temp1 = tc[m][n]*cp[m];
				temp2 = tc[m][n]*sp[m];
			}
			else
			{
				temp1 = tc[m][n]*cp[m]+tc[n][m-1]*sp[m];
				temp2 = tc[m][n]*sp[m]-tc[n][m-1]*cp[m];
			}
			bt = bt-ar*temp1*dp[m][n];
			bp += (fm[m]*temp2*par);
			br += (fn[n]*temp1*par);
			/*
	SPECIAL CASE:  NORTH/SOUTH GEOGRAPHIC POLES
*/
			if (st == 0.0 && m == 1)
			{
				if (n == 1) pp[n] = pp[n-1];
				else pp[n] = ct*pp[n-1]-k[m][n]*pp[n-2];
				parp = ar*pp[n];
				bpp += (fm[m]*temp2*parp);
			}
		}
	}
	if (st == 0.0) bp = bpp;
	else bp /= st;
	/*
	ROTATE MAGNETIC VECTOR COMPONENTS FROM SPHERICAL TO
	GEODETIC COORDINATES
*/
	*bx = -bt*ca-br*sa;
	*by = bp;
	*bz = bt*sa-br*ca;
	// EJP: 1.) switch from LH to RH coordinates, 2.) reverse direction of field to represent actual direction
	/*
	*bx = bt*ca+br*sa;
	*by = -bp;
	*bz = bt*sa-br*ca;
  */
	/*
	COMPUTE DECLINATION (DEC), INCLINATION (DIP) AND
	TOTAL INTENSITY (TI)
*/
	bh = sqrt((*bx**bx)+(*by**by));
	*ti = sqrt((bh*bh)+(*bz**bz));
	*dec = atan2(*by,*bx);
	*dip = atan2(*bz,bh);
	/*
	COMPUTE MAGNETIC GRID VARIATION IF THE CURRENT
	GEODETIC POSITION IS IN THE ARCTIC OR ANTARCTIC
	(I.E. GLAT > +55 DEGREES OR GLAT < -55 DEGREES)

	OTHERWISE, SET MAGNETIC GRID VARIATION TO -999.0
*/
	*gv = -999.0;
	if (fabs(glat) >= 55.)
	{
		if (glat > 0.0 && glon >= 0.0) *gv = (*dec-rlon)/dtr;
		if (glat > 0.0 && glon < 0.0) *gv = (*dec+fabs(rlon))/dtr;
		if (glat < 0.0 && glon >= 0.0) *gv = (*dec+rlon)/dtr;
		if (glat < 0.0 && glon < 0.0) *gv = (*dec-fabs(rlon))/dtr;
		if (*gv > +180.0) *gv -= 360.0;
		if (*gv < -180.0) *gv += 360.0;
	}
	otime = time;
	oalt = alt;
	olat = glat;
	olon = glon;
	return 0;
}

/*************************************************************************/

int32_t geomag(int *maxdeg)
{
	return E0000(0,maxdeg,0.0,0.0,0.0,0.0,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
}

/*************************************************************************/

int32_t geomg1(float alt, float rlat, float rlon, float time, float *dec, float *dip, float *ti, float *gv, float *bx, float *by, float *bz)
{
	return E0000(1,NULL,alt,rlat,rlon,time,dec,dip,ti,gv,bx,by,bz);
}

/*************************************************************************/

char geomag_introduction(float epochlowlim)
{
	char help;
	static char ans;
	int iretn;

	printf("\n\n Welcome to the World Magnetic Model (WMM) %4.0f C-Program\n\n", epochlowlim);
	printf("            --- Version 2.0, September 2005 ---\n\n");
	printf("\n This program estimates the strength and direction of ");
	printf("\n Earth's main magnetic field for a given point/area.");
	printf("\n Enter h for help and contact information or c to continue.");
	printf ("\n >");
	iretn = scanf("%c%*[^\n]",&help);
	if (!iretn)
	{
		ans = 'y';
		return ans;
	}
	getchar();

	if ((help == 'h') || (help == 'H'))
	{
		printf("\n Help information ");

		printf("\n The World Magnetic Model (WMM) for %7.2f", epochlowlim);
		printf("\n is a model of Earth's main magnetic field.  The WMM");
		printf("\n is recomputed every five (5) years, in years divisible by ");
		printf("\n five (i.e. 2005, 2010).  See the contact information below");
		printf("\n to obtain more information on the WMM and associated software.");
		printf("\n ");
		printf("\n Input required is the location in geodetic latitude and");
		printf("\n longitude (positive for northern latitudes and eastern ");
		printf("\n longitudes), geodetic altitude in meters, and the date of ");
		printf("\n interest in years.");

		printf("\n\n\n The program computes the estimated magnetic Declination");
		printf("\n (D) which is sometimes called MAGVAR, Inclination (I), Total");
		printf("\n Intensity (F or TI), Horizontal Intensity (H or HI), Vertical");
		printf("\n Intensity (Z), and Grid Variation (GV). Declination and Grid");
		printf("\n Variation are measured in units of degrees and are considered");
		printf("\n positive when east or north.  Inclination is measured in units");
		printf("\n of degrees and is considered positive when pointing down (into");
		printf("\n the Earth).  The WMM is reference to the WGS-84 ellipsoid and");
		printf("\n is valid for 5 years after the base epoch.");

		printf("\n\n\n It is very important to note that a  degree and  order 12 model,");
		printf("\n such as WMM, describes only the long  wavelength spatial magnetic ");
		printf("\n fluctuations due to  Earth's core.  Not included in the WMM series");
		printf("\n models are intermediate and short wavelength spatial fluctuations ");
		printf("\n that originate in Earth's mantle and crust. Consequently, isolated");
		printf("\n angular errors at various  positions on the surface (primarily over");
		printf("\n land, incontinental margins and  over oceanic seamounts, ridges and");
		printf("\n trenches) of several degrees may be expected.  Also not included in");
		printf("\n the model are temporal fluctuations of magnetospheric and ionospheric");
		printf("\n origin. On the days during and immediately following magnetic storms,");
		printf("\n temporal fluctuations can cause substantial deviations of the geomagnetic");
		printf("\n field  from model  values.  If the required  declination accuracy  is");
		printf("\n more stringent than the WMM  series of models provide, the user is");
		printf("\n advised to request special (regional or local) surveys be performed");
		printf("\n and models prepared. Please make requests of this nature to the");
		printf("\n National Geospatial-Intelligence Agency (NGA) at the address below.");

		printf("\n\n\n Contact Information");

		printf("\n  Software and Model Support");
		printf("\n	National Geophysical Data Center");
		printf("\n	NOAA EGC/2");
		printf("\n	325 Broadway");
		printf("\n	Boulder, CO 80303 USA");
		printf("\n	Attn: Susan McLean or Stefan Maus");
		printf("\n	Phone:  (303) 497-6478 or -6522");
		printf("\n	Email:  Susan.McLean@noaa.gov or Stefan.Maus@noaa.gov ");

		printf("\n\n\n Continue with program? (y or n) ");
		iretn = scanf("%c%*[^\n]", &ans);
		if (!iretn)
		{
			ans = 'y';
			return ans;
		}
		getchar();
	}
	else
	{
		ans = 'y';
	}

	return(ans);
}
