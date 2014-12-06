#include "mathlib.h"
#include "convertlib.h"

int main(int argc, char *argv[])
{
rvector r_a, r_ad;
rvector r_b, r_bd;
rvector omega_a, omega_b, domega;
rmatrix s0_a2b, s1_a2b, s0_b2a, s1_b2a;
locstruc loc;
int i;
double rl, rl2;

//set_resdir("/home/cosmos/data");
loc.pos.eci.s = loc.pos.eci.v = loc.pos.eci.a = rv_zero();
//loc.pos.eci.utc = 55898.085729;
/*
loc.pos.eci.utc = 55593.41666667;
loc.pos.eci.s.col[0] = -1.35425064646195e+006;
loc.pos.eci.s.col[1] = 6.79448949176058e+006;
loc.pos.eci.s.col[2] = 1.53887448487181e+002;
loc.pos.eci.v.col[0] = 9.83318338395099e+002;
loc.pos.eci.v.col[1] = 1.95820817667284e+002;
loc.pos.eci.v.col[2] = 7.51853092957268e+003;
*/
loc.pos.eci.utc = 55593.417361114;
loc.pos.eci.s.col[0] = -1.29236902697246e+006;
loc.pos.eci.s.col[1] = 6.79155588127674e+006;
loc.pos.eci.s.col[2] = 4.50939536126163e+005;
loc.pos.eci.v.col[0] = 1.07865806254567e+003;
loc.pos.eci.v.col[1] = -2.93571016435623e+002;
loc.pos.eci.v.col[2] = 7.50223303990417e+003;
loc.pos.eci.a.col[0] = 1.551240;
loc.pos.eci.a.col[1] = -8.151717;
loc.pos.eci.a.col[2] = -0.542812;

loc.att.icrf.s = q_eye();
loc.att.icrf.v = loc.att.icrf.a = rv_zero();
pos_eci2geoc(&loc);
att_icrf2lvlh(&loc);

pos_geoc2eci(&loc);

/*
loc.pos.geoc.s.col[0] = -6.92811300474276e+006;
loc.pos.geoc.s.col[1] = -1.81834522971509e+004;
loc.pos.geoc.s.col[2] = -1.35880301329570e+003;
loc.pos.geoc.v.col[0] = -5.41067032674325e+000;
loc.pos.geoc.v.col[1] = 1.49960794154777e+003;
loc.pos.geoc.v.col[2] = 7.51962270141293e+003;
*/
loc.pos.geoc.s.col[0] = -6.91314723731658e+006;
loc.pos.geoc.s.col[1] = 7.17245429728969e+004;
loc.pos.geoc.s.col[2] = 4.49495629279657e+005;
loc.pos.geoc.v.col[0] = 5.04082852450504e+002;
loc.pos.geoc.v.col[1] = 1.49550752144134e+003;
loc.pos.geoc.v.col[2] = 7.50343144307068e+003;
pos_geoc2eci(&loc);

r_a = omega_a = rv_one();
r_a = rv_smult(5e6,r_a);
rl = length_rv(r_a);
rl2 = 1./(rl*rl);

for (i=0; i<100000; i++)
	{
r_b = rv_mmult(s0_a2b,r_a);
r_bd = rv_mmult(s1_a2b,r_a);
omega_b = rv_mmult(s0_a2b,omega_a);
domega = rv_smult(rl2,rv_cross(r_b,r_bd));
omega_b = rv_add(omega_b,domega);
//omega_a = rv_sub(omega_b,domega);
//omega_a = rv_mmult(s0_b2a,omega_a);

r_a = rv_mmult(s0_b2a,r_b);
r_ad = rv_mmult(s1_b2a,r_b);
omega_a = rv_mmult(s0_b2a,omega_b);
domega = rv_smult(rl2,rv_cross(r_a,r_ad));
omega_a = rv_add(omega_a,domega);
printf("%15g %15g %15g %15g %15g %15g %15g %15g \r",(5e6-r_a.col[0])/5e6,(5e6-r_a.col[1])/5e6,(5e6-r_a.col[2])/5e6,length_rv(r_a)/rl,1.-omega_a.col[0],1.-omega_a.col[1],1.-omega_a.col[2],length_rv(omega_a)/sqrt(3.));
	}
printf("\n");
}
