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

#include "convertlib.h"
#include "jsonlib.h"
#include "physicslib.h"
#include "timelib.h"

cosmosstruc *cdata;
gj_handle gjh;

int main(int argc, char *argv[])
{
	locstruc loc;
	kepstruc kep;
	double imjd;

	cdata = json_create();
	json_setup_node(NULL,cdata);

	cdata[0].physics.moi.col[0] = 3.;
	cdata[0].physics.moi.col[1] = 3.;
	cdata[0].physics.moi.col[2] = 3.;

	pos_clear(loc);

	kep.ea = 0.;
	kep.e = 0.;
	kep.a = 1000.* AU;
	kep.mm = sqrt(GM / pow(kep.a,3.));
	kep.i = 0.;
	kep.raan = 0.;
	kep.ap = 0.;
	kep2eci(&kep, &loc.pos.eci);

	imjd = loc.utc = currentmjd(0.);
	loc.pos.eci.utc = loc.utc;
//	loc.pos.eci.s = rv_smult((1000*AU),rv_unitz());
//	loc.pos.eci.v = rv_smult(sqrt(GSUN/(1000*AU)),rv_unitx());
	++loc.pos.eci.pass;
	loc.att.icrf.utc = loc.utc;
//	loc.att.icrf.v = rv_smult(-.017453293,rv_unitz());
	loc.att.icrf.v = rv_smult(-.17453293,rv_unitz());
	loc.att.icrf.s = q_eye();
	loc.att.icrf.a = rv_zero();
	pos_eci(&loc);

    hardware_init_eci(cdata[0].devspec, loc);
    gauss_jackson_init_eci(gjh, 6, 0, .1, loc.utc, loc.pos.eci, loc.att.icrf, cdata->physics, cdata->node.loc);

	for (uint16_t i=0; i<=1000; ++i)
	{
		printf("%f\t%f\t%f\t%f\t%f\n",86400.*(cdata[0].node.loc.utc-imjd),cdata[0].node.loc.att.icrf.v.col[0],cdata[0].node.loc.att.icrf.v.col[1],cdata[0].node.loc.att.icrf.v.col[2],length_rv(cdata[0].node.loc.att.icrf.v));
//		cdata[0].physics.ftorque = transform_q(cdata[0].node.loc.att.icrf.s,rv_smult(-5.2359406e-4,rv_unity()));
		cdata[0].physics.ftorque = transform_q(cdata[0].node.loc.att.icrf.s,rv_smult(-5.2359406e-3,rv_unity()));
        gauss_jackson_propagate(gjh, cdata->physics, cdata->node.loc, cdata[0].node.loc.utc+10./86400.);
        simulate_hardware(*cdata, cdata->node.loc);
    }
}
