#include "convertlib.h"

int main(int argc, char *argv[])
{
cartpos epos[6], mpos[6];
double mjd;
int count;
rvector pos, mediff;

get_resdir();
count = 0;
for (mjd=58135.4; mjd<58135.5; mjd+=1./86400.)
{
if (count == 0)
	{
	jplpos(JPL_SUN_BARY,JPL_EARTH,utc2tt(mjd),&epos[0]);
	jplpos(JPL_SUN_BARY,JPL_MOON,utc2tt(mjd),&mpos[0]);
	mediff = rv_sub(mpos[0].s,epos[0].s);
	}
else
	{
	jplpos(JPL_SUN_BARY,JPL_EARTH,utc2tt(mjd),&epos[1]);
	jplpos(JPL_SUN_BARY,JPL_MOON,utc2tt(mjd),&mpos[1]);
	pos = rv_sub(rv_sub(mpos[1].s,epos[1].s),mediff);
	printf("%.15g %.15g %.15g %.15g\n",mjd,pos.col[0],pos.col[1],pos.col[2]);
	}
count++;
}
/*
jplpos(JPL_SUN_BARY,JPL_VENUS,JD2MJD(2451545.5),&pos,"/home/cosmos/data");
printf("%f %f %f %f %f %f %f\n",2451545.5,pos.s.x/1000.,pos.s.y/1000.,pos.s.z/1000.,pos.v.x/1000.,pos.v.y/1000.,pos.v.z/1000.);
jplpos(JPL_SUN_BARY,JPL_MOON,utc2tt(JD2MJD(2455620.5)),&pos,"/home/cosmos/data");
printf("%f %f %f %f %f %f %f\n",2455620.5,pos.s.x/1000.,pos.s.y/1000.,pos.s.z/1000.,pos.v.x/1000.,pos.v.y/1000.,pos.v.z/1000.);
jplpos(JPL_SUN_BARY,JPL_MOON,utc2tt(JD2MJD(2455621.5)),&pos,"/home/cosmos/data");
printf("%f %f %f %f %f %f %f\n",2455621.5,pos.s.x/1000.,pos.s.y/1000.,pos.s.z/1000.,pos.v.x/1000.,pos.v.y/1000.,pos.v.z/1000.);
jplpos(JPL_SUN_BARY,JPL_MOON,utc2tt(JD2MJD(2455622.5)),&pos,"/home/cosmos/data");
printf("%f %f %f %f %f %f %f\n",2455622.5,pos.s.x/1000.,pos.s.y/1000.,pos.s.z/1000.,pos.v.x/1000.,pos.v.y/1000.,pos.v.z/1000.);
jplpos(JPL_SUN_BARY,JPL_MOON,utc2tt(JD2MJD(2455623.5)),&pos,"/home/cosmos/data");
printf("%f %f %f %f %f %f %f\n",2455623.5,pos.s.x/1000.,pos.s.y/1000.,pos.s.z/1000.,pos.v.x/1000.,pos.v.y/1000.,pos.v.z/1000.);
jplpos(JPL_SUN_BARY,JPL_MOON,utc2tt(JD2MJD(2455624.5)),&pos,"/home/cosmos/data");
printf("%f %f %f %f %f %f %f\n",2455624.5,pos.s.x/1000.,pos.s.y/1000.,pos.s.z/1000.,pos.v.x/1000.,pos.v.y/1000.,pos.v.z/1000.);
jplpos(JPL_SUN_BARY,JPL_MOON,utc2tt(JD2MJD(2455625.5)),&pos,"/home/cosmos/data");
printf("%f %f %f %f %f %f %f\n",2455625.5,pos.s.x/1000.,pos.s.y/1000.,pos.s.z/1000.,pos.v.x/1000.,pos.v.y/1000.,pos.v.z/1000.);
*/

}
