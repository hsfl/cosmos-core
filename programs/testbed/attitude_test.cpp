#include "convertlib.h"
#include "jsonlib.h"
#include "jsonlib.h"
#include "physicslib.h"

cosmosstruc *cdata;

int main(int argc, char *argv[])
{
	locstruc loc;
	kepstruc kep;
	double imjd;

	cdata = json_create();
	node_init(NULL,cdata);

	cdata->physics.moi.col[0] = 3.;
	cdata->physics.moi.col[1] = 3.;
	cdata->physics.moi.col[2] = 3.;

	pos_clear(&loc);

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

	gauss_jackson_init_eci(6,0,.1,loc.utc,loc.pos.eci,loc.att.icrf,cdata);

	for (uint16_t i=0; i<=1000; ++i)
	{
		printf("%f\t%f\t%f\t%f\t%f\n",86400.*(cdata->node.loc.utc-imjd),cdata->node.loc.att.icrf.v.col[0],cdata->node.loc.att.icrf.v.col[1],cdata->node.loc.att.icrf.v.col[2],length_rv(cdata->node.loc.att.icrf.v));
//		cdata->physics.ftorque = transform_q(cdata->node.loc.att.icrf.s,rv_smult(-5.2359406e-4,rv_unity()));
		cdata->physics.ftorque = transform_q(cdata->node.loc.att.icrf.s,rv_smult(-5.2359406e-3,rv_unity()));
		gauss_jackson_propagate(cdata,cdata->node.loc.utc+10./86400.);
	}
}
