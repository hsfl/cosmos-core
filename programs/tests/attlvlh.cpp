#include "support/configCosmos.h"
#include "support/convertlib.h"
#include "support/jsonlib.h"
#include "physics/physicslib.h"

int main(int argc, char *argv[])
{
    rvector svec;
    rvector vvec;
    Convert::locstruc loc;
    cosmosstruc* cinfo = json_init();
//    double utc = 60754.375;
    double utc = 60574.;

    // Set to vernal equinox 2025
    Physics::orbit_init_shape(0, 1., utc, 400000, RADOF(90.), 0., cinfo);
    // Convert::cartpos geoc = cinfo->node.loc.pos.geoc;

    cinfo->node.loc.att.lvlh.s = q_eye();
    cinfo->node.loc.att.lvlh.v = rv_zero();
    cinfo->node.loc.att.lvlh.a = rv_zero();
    cinfo->node.loc.att.lvlh.utc = utc;
    cinfo->node.loc.att.lvlh.pass++;
    att_lvlh(cinfo->node.loc);

    vvec = transform_q((cinfo->node.loc.att.geoc.s),rv_smult(10,rv_normal(cinfo->node.loc.pos.geoc.v)));
    svec = transform_q((cinfo->node.loc.att.geoc.s),rv_smult(10,rv_normal(cinfo->node.loc.pos.geoc.s)));
    vvec = transform_q((cinfo->node.loc.att.icrf.s),rv_smult(10,rv_normal(cinfo->node.loc.pos.eci.v)));
    svec = transform_q((cinfo->node.loc.att.icrf.s),rv_smult(10,rv_normal(cinfo->node.loc.pos.eci.s)));

    exit(0);
}
