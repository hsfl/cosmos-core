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
#include "mathlib.h"
#include "jsonlib.h"
#include "timelib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

cosmosstruc *cdata;
stkstruc stk;

int main(int argc, char *argv[])
{
quaternion ql;
avector el;
kepstruc kep;
double localtime, lastlat;
double dt, dp, maxr, minr, mjdnow, mjdbase, mjdlast, tp, cp;


dt = atof(argv[2]);
dp = atof(argv[3]);
tp = atof(argv[4]);

cdata = json_create();
json_setup_node(argv[1],cdata);

cdata[0].physics.mode = atol(argv[1]);
//cdata[0].node.info.testflag = FLAG_ADRAG;
//cdata[0].node.info.testflag = FLAG_GTORQUE;

mjdbase = mjdnow = 55593.41666667;
orbit_init(0, dt, mjdbase, (char *)"stk_cdr_j2000.txt", *cdata);


//att_eci2lvlh(&cdata[0].node.loc.pos,&cdata[0].node.loc.att);
//att_lvlh2eci(&cdata[0].node.loc.pos,&cdata[0].node.loc.att);

cp = 0.;
maxr = 0.;
minr = 10. * cdata[0].node.loc.pos.geos.s.r;
lastlat = cdata[0].node.loc.pos.geod.s.lat;
mjdbase = mjdnow;
mjdlast = mjdnow + tp;
do
	{
		printf("%16.10f %12.6f %8.3f %8.3f ",cdata[0].node.loc.utc,1440.*(cdata[0].node.loc.utc-mjdbase),cdata[0].node.loc.pos.geod.s.lat*180./M_PI,cdata[0].node.loc.pos.geod.s.lon*180./M_PI);

//		printf("%f\t%f\t%f\t",cdata[0].node.loc.pos.geod.s.h,cdata[0].node.loc.pos.geod.s.lat*180./M_PI,cdata[0].node.loc.pos.geod.s.lon*180./M_PI);
//		printf("%f\t%f\t%f\t",cdata[0].node.loc.pos.geoc.s.col[0],cdata[0].node.loc.pos.geoc.s.col[1],cdata[0].node.loc.pos.geoc.s.col[2]);
//		printf("%f\t%f\t%f\t",cdata[0].node.loc.pos.geoc.v.col[0],cdata[0].node.loc.pos.geoc.v.col[1],cdata[0].node.loc.pos.geoc.v.col[2]);
//		printf("%f\t%f\t%f\t",cdata[0].node.loc.pos.geoc.a.col[0],cdata[0].node.loc.pos.geoc.a.col[1],cdata[0].node.loc.pos.geoc.a.col[2]);
//		printf("%f\t%f\t%f\t",cdata[0].node.loc.pos.eci.s.col[0],cdata[0].node.loc.pos.eci.s.col[1],cdata[0].node.loc.pos.eci.s.col[2]);
//		printf("%f\t%f\t%f\t",cdata[0].node.loc.pos.eci.v.col[0],cdata[0].node.loc.pos.eci.v.col[1],cdata[0].node.loc.pos.eci.v.col[2]);
//		printf("%f\t%f\t%f\t",cdata[0].node.loc.pos.eci.a.col[0],cdata[0].node.loc.pos.eci.a.col[1],cdata[0].node.loc.pos.eci.a.col[2]);
//		printf("%f\t%f\t%f\t",cdata[0].node.loc.att.eci.s.row[0].col[0],cdata[0].node.loc.att.eci.s.row[0].col[1],cdata[0].node.loc.att.eci.s.row[0].col[2]);
//		printf("%f\t%f\t%f\t",cdata[0].node.loc.att.eci.s.row[1].col[0],cdata[0].node.loc.att.eci.s.row[1].col[1],cdata[0].node.loc.att.eci.s.row[1].col[2]);
//		printf("%f\t%f\t%f\t",cdata[0].node.loc.att.eci.s.row[2].col[0],cdata[0].node.loc.att.eci.s.row[2].col[1],cdata[0].node.loc.att.eci.s.row[2].col[2]);
//		vec = rv_normal(rv_mmult((cdata[0].node.loc.att.eci.s),cdata[0].node.loc.pos.eci.s));
//		vec =
//		rv_normal(rv_mmult((cdata[0].node.loc.att.eci.s),rv_smult(-1.,cdata[0].node.loc.pos.baryc.s)));
//		vec = cdata[0].node.loc.att.eci.v;
//		printf("%f %f %f %f ",vec.col[0],vec.col[1],vec.col[2],length_rv(vec));
//		printf("%f %f %f %f ",DEGOF(vec.col[0]),DEGOF(vec.col[1]),DEGOF(vec.col[2]),DEGOF(length_rv(vec)));
//		vec = cdata[0].node.loc.att.body.a;
//		printf("%f %f %f %f ",vec.col[0],vec.col[1],vec.col[2],length_rv(vec));
//		printf("%f %f %f
//		",cdata[0].node.info.powuse,cdata[0].node.info.powgen,cdata[0].node.info.battlev);
//		printf("%f %f %f %f",cdata[0].devspec.rw[0]->omega,cdata[0].devspec.rw[0]->alpha,cdata[0].device[cdata[0].devspec.rw[0]->gen.cidx].gen.current,cdata[0].node.info.powuse);
	localtime = atan2(cdata[0].node.loc.pos.baryc.s.col[1],cdata[0].node.loc.pos.baryc.s.col[0]);
	if (localtime < 0.)
		localtime += D2PI;
	eci2kep(&cdata[0].node.loc.pos.eci,&kep);
	localtime = kep.raan + DPI - localtime;
	if (localtime < 0.)
		localtime += D2PI;
//		printf("%f\t%f\t%f\t",DEGOF(cdata[0].node.loc.pos.sunsepangle),24.*localtime/D2PI,24.*cdata[0].node.loc.pos.localtime/D2PI);

	if (dp)
		{
		mjdnow += dp/86400.;
		orbit_propagate(*cdata, mjdnow);
//		att_eci2lvlh(&cdata[0].node.loc.pos,&cdata[0].node.loc.att);
		ql = cdata[0].node.loc.att.lvlh.s;
//		printf("%f\t%f\t%f\t%f\t",ql.d.col[0],ql.d.col[1],ql.d.col[2],ql.w);
		el = a_quaternion2euler(ql);
//		printf("%f\t%f\t%f\t",DEGOF(el.h),DEGOF(el.e),DEGOF(el.b));
		cp += dp;
		}
	else
		{
		do
			{
			lastlat = cdata[0].node.loc.pos.geod.s.lat;
			mjdnow += dt/86400.;
			orbit_propagate(*cdata, mjdnow);
			cp += dt/86400.;
			}
			while (!(lastlat < 0. && cdata[0].node.loc.pos.geod.s.lat >= 0.));
		}

		printf("\n");
		fflush(stdout);
	currentmjd(cdata[0].node.utcoffset);
	if (cdata[0].node.loc.pos.geos.s.r < minr)
		minr = cdata[0].node.loc.pos.geos.s.r;
	if (cdata[0].node.loc.pos.geos.s.r > maxr)
		maxr= cdata[0].node.loc.pos.geos.s.r;
	} while (mjdnow < mjdlast && cdata[0].node.loc.pos.geod.s.h > 86000.);
}
