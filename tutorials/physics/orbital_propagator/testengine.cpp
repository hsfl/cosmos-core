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

/*! \file testengine.cpp
*	\brief Command line simulator
*/
#include "physics/physicslib.h"
#include "math/mathlib.h"
#include "support/jsonlib.h"
//#include "support/controllib.h"
#include "support/ephemlib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//#define MMCORRECT -2.766301289e-10
#define MMCORRECT 0.0

cosmosstruc *cosmos_data;
stkstruc stk;

int main(int argc, char *argv[])
{
	int iretn;
	kepstruc kep;
	cartpos ipos;
	locstruc tloc, iloc;
	double localtime, lastlat;
	double dt, dp, maxr, minr, mjdnow, mjdbase, mjdlast, tp, cp;
	double mjdnext;
	double radius, angvel;
	double deltas, deltav;
	double rwomg,ralp, mtrx, mtry, mtrz;
	uint32_t order = 8;
	rvector torque, mtorque, rtorque1, rtorque2, bearth, mmoment, ftorque;
	int i, mode;
	char stkname[50], tstring[100];
	const char *tpointer;

	i = sizeof(cosmosstruc);

	if (argc != 6)
	{
		printf("Usage: testengine node mode deltat deltap totalp\n");
		exit (1);
	}

	// seconds
	dt = atof(argv[3]);

	// printing
	dp = atof(argv[4]);

	// total time
	tp = atof(argv[5]);

	node_init(argv[1],cosmos_data);

	// mode is just for attitude
	// mode = 0, free propagation for attitude
	// mode = 1, force LVLH
	mode = atol(argv[2]);

	cosmos_data->sdata.node.loc.att.icrf.v = rv_smult(1.,rv_unitz());
	cosmos_data->sdata.node.loc.att.icrf.s = q_eye();
	cosmos_data->sdata.node.loc.att.icrf.a = rv_zero();

	// STK: CDR
	// STK: a(6578km),e(0deg),i(45deg),raan(0deg),ea(0deg),mm()
	strcpy(stkname,"stk_cdr_j2000.txt");
	if (load_stk(stkname, stk)<2)
		exit(1);

	// STK
	mjdbase = mjdnow = stk.pos[1].utc;
	if ((iretn=stk2eci(mjdnow, stk, ipos)) < 0)
	{
		printf("Error: stk2eci()\n");
		exit(iretn);
	}
	radius = length_rv(ipos.s);
	angvel = sqrt(GM/(radius*radius*radius));

	// JERS
	/*
mjdbase = mjdnow = 49809.15208333333;
ipos.s.col[0] = 4762230.08;
ipos.s.col[1] = -165142.097;
ipos.s.col[2] = 5052273.733;
ipos.v.col[0] = 5356.758;
ipos.v.col[1] = -1669.669;
ipos.v.col[2] = -5090.863;
*/

	//STK Me
	/*
mjdbase = mjdnow = 55593.416666669997;
ipos.s.col[0] = -1354250.647;
ipos.s.col[1] = 6794489.492;
ipos.s.col[2] = 153.839;
ipos.v.col[0] = 983.319318;
ipos.v.col[1] = 195.821067;
ipos.v.col[2] = 7518.530795;
*/

	// STK (Miguel)
	/*
mjdbase = mjdnow = 55927.79166667;
ipos.s.col[0] = 3905510.;
ipos.s.col[1] = 5722420.;
ipos.s.col[2] = -1.03369e-10;
ipos.v.col[0] = 828.335;
ipos.v.col[1] = -565.334;
ipos.v.col[2] = 7518.5;
*/

	// Midstar
	/*
load_lines("midstar.tle",&tle);
mjdbase = mjdnow = tle.mjd;
ipos.s.col[0] = 327679.002;
ipos.s.col[1] = 6866364.173;
ipos.s.col[2] = -528.566;
ipos.v.col[0] = -5277.99;
ipos.v.col[1] = 248.735;
ipos.v.col[2] = 5488.55;
gauss_jackson_init_tle(order,dt,tle.mjd,tle,cosmos_data);
line2eci(cosmos_data->sdata.node.mjd,&tle,&ipos);
printf("%.15g\t%f\t%f\t%f\t",tle.mjd,ipos.s.col[0],ipos.s.col[1],ipos.s.col[2]);
printf("%f\t%f\t%f\n",ipos.v.col[0],ipos.v.col[1],ipos.v.col[2]);
*/


	iloc.pos.eci = ipos;
	pos_eci(&iloc);
	//iloc.att.lvlh.s = q_roty(DPI2);
	//iloc.att.lvlh.v = rv_smult(D2PI,rv_unitx());
	iloc.att.lvlh.s = q_eye();
	iloc.att.lvlh.v = rv_zero();
	iloc.att.lvlh.a = rv_zero();
	att_lvlh2planec(&iloc);
	pos_clear(iloc);
	iloc.pos.geoc.utc = mjdnow;
	pos_geoc(&iloc);

	// propagator
	gj_handle gjh;
	gauss_jackson_init_eci(gjh,
	order,
	mode,
	dt, //s
	mjdnow, // modified julian date
	ipos, // initial ECI position
	iloc.att.icrf, // initial ICRF attitude
	cosmos_data->sdata.physics,
	cosmos_data->sdata.node.loc);

	mjdnow = cosmos_data->sdata.node.loc.utc;

	eci2kep(cosmos_data->sdata.node.loc.pos.eci, kep);
	if (!dp)
	{
		dp = dt;
		tp = kep.period;
	}

	maxr = 0.;
	minr = 10. * cosmos_data->sdata.node.loc.pos.geos.s.r;
	lastlat = cosmos_data->sdata.node.loc.pos.geod.s.lat;
	mjdbase = mjdnow;
	mjdlast = mjdnow + tp/86400.;
	cp = 0;
	cosmos_data->sdata.devspec.rw[0]->omg = rwomg = 0;
	cosmos_data->sdata.physics.moi.col[0] = 2.3;
	cosmos_data->sdata.physics.moi.col[1] = 2.6;
	cosmos_data->sdata.physics.moi.col[2] = 2.7;
	tpointer = tstring;
	strcpy(tstring,"(\"info_powuse\">10.)");
	//strcpy(tstring,"(1+1)");
	radius = json_equation(tpointer,cosmos_data->meta, cosmos_data->pdata);
	do
	{

		jplpos(JPL_EARTH,JPL_MOON,utc2tt(cosmos_data->sdata.node.utc),&ipos);
		localtime = atan2(cosmos_data->sdata.node.loc.pos.icrf.s.col[1], cosmos_data->sdata.node.loc.pos.icrf.s.col[0]);
		if (localtime < 0.)
			localtime += D2PI;
		localtime = kep.raan + DPI - localtime;
		if (localtime < 0.)
			localtime += D2PI;

		mjdnow += dt/86400.;
		cp += dt;

		gauss_jackson_propagate(gjh, cosmos_data->pdata.physics, cosmos_data->pdata.node.loc, mjdnow);

		tloc = cosmos_data->sdata.node.loc;
		tloc.att.lvlh.s = q_eye();
		tloc.att.lvlh.v = rv_zero();
		att_lvlh2icrf(&tloc);
		att_icrf2lvlh(&tloc);
		att_lvlh2icrf(&tloc);

//		torque = calc_control_torque(dt,tloc,cosmos_data);
//		calc_hardware_torque(cosmos_data->sdata.node.loc,torque,&rtorque1,&rtorque2,&mtorque,&ralp,&mtrx,&mtry,&mtrz,cosmos_data);
		ftorque = rv_add(rtorque1,rv_add(rtorque2,mtorque));
		ftorque = rotate_q(q_conjugate(cosmos_data->sdata.node.loc.att.icrf.s),ftorque);

		rwomg += ralp * dt;
		//	cosmos_data->dnode[0].rw[0].omg += cosmos_data->dnode[0].rw[0].ralp * dt;
		//	cosmos_data->dnode[0].rw[0].ralp = length_rv(rtorque2) / cosmos_data->sdata.node.rw[0].mom.col[2];

		bearth =  rotate_q(cosmos_data->sdata.node.loc.att.icrf.s,cosmos_data->sdata.node.loc.bearth);
		mmoment = rv_smult(length_rv(mtorque)/length_rv(bearth),rv_normal(rv_cross(bearth,mtorque)));
		//	cosmos_data->dnode[0].mtr[0].rfld = mmoment.col[0];
		//	cosmos_data->dnode[0].mtr[1].rfld = mmoment.col[1];
		//	cosmos_data->dnode[0].mtr[2].rfld = mmoment.col[2];

		rtorque1 = rotate_q(q_conjugate(cosmos_data->sdata.node.loc.att.icrf.s),rtorque1);
		rtorque2 = rotate_q(q_conjugate(cosmos_data->sdata.node.loc.att.icrf.s),rtorque2);
		mtorque = rotate_q(q_conjugate(cosmos_data->sdata.node.loc.att.icrf.s),mtorque);

		//	cosmos_data->pdata.physics.ftorque = rv_zero();
		cosmos_data->pdata.physics.ftorque = torque;
		//	cosmos_data->pdata.physics.ftorque = ftorque;

		if (cp >= dp)
		{
			cp = 0;
			deltas = length_rv(rv_quaternion2axis(q_mult(tloc.att.icrf.s,q_conjugate(cosmos_data->sdata.node.loc.att.icrf.s))));
			if (deltas > DPI)
				deltas = length_rv(rv_quaternion2axis(q_smult(-1.,q_mult(tloc.att.icrf.s,q_conjugate(cosmos_data->sdata.node.loc.att.icrf.s)))));
			deltav = length_rv(rv_sub(tloc.att.icrf.v,cosmos_data->sdata.node.loc.att.icrf.v));
			printf("%.15g\t%12.6f\t",cosmos_data->sdata.node.utc,1440.*(cosmos_data->sdata.node.utc-mjdbase));
			printf("%.10g\t%.10g\t%.5g\t",DEGOF(cosmos_data->sdata.node.loc.pos.geod.s.lat),DEGOF(cosmos_data->sdata.node.loc.pos.geod.s.lon),cosmos_data->sdata.node.loc.pos.geod.s.h/1000.);
			printf("%.10g\t%.10g\t%.10g\t%.10g\t",cosmos_data->sdata.node.loc.att.lvlh.s.d.x,cosmos_data->sdata.node.loc.att.lvlh.s.d.y,cosmos_data->sdata.node.loc.att.lvlh.s.d.z,cosmos_data->sdata.node.loc.att.lvlh.s.w);
			printf("%.10g\t%.10g\t%.10g\t",cosmos_data->sdata.node.loc.att.lvlh.v.col[0],cosmos_data->sdata.node.loc.att.lvlh.v.col[1],cosmos_data->sdata.node.loc.att.lvlh.v.col[2]);
			printf("%.10g\t%.10g\t%.10g\t",cosmos_data->sdata.node.loc.att.lvlh.a.col[0],cosmos_data->sdata.node.loc.att.lvlh.a.col[1],cosmos_data->sdata.node.loc.att.lvlh.a.col[2]);
			printf("%.10g\t%.10g\t%.10g\t",torque.col[0],torque.col[1],torque.col[2]);
			printf("%.10g\t%.10g\t%.10g\t",cosmos_data->pdata.physics.ftorque.col[0],cosmos_data->pdata.physics.ftorque.col[1],cosmos_data->pdata.physics.ftorque.col[2]);
			printf("%.10g\t%.10g\t%.10g\t",mtorque.col[0],mtorque.col[1],mtorque.col[2]);
			printf("%.10g\t%.10g\t",rwomg,ralp);
			printf("%.10g\t%.10g\t%.10g\t",mtrx,mtry,mtrz);
			printf("%.10g\t%.10g\t",deltas,deltav);
			printf("\n");
			fflush(stdout);
		}
		mjdnext = currentmjd(cosmos_data->sdata.node.utcoffset);
		if (cosmos_data->sdata.node.loc.pos.geos.s.r < minr)
			minr = cosmos_data->sdata.node.loc.pos.geos.s.r;
		if (cosmos_data->sdata.node.loc.pos.geos.s.r > maxr)
			maxr= cosmos_data->sdata.node.loc.pos.geos.s.r;
	} while (mjdnow < mjdlast && cosmos_data->sdata.node.loc.pos.geod.s.h > 8600.);
	printf("\n");
}
