#include "support/configCosmos.h"
#include "support/convertlib.h"
#include "support/jsonlib.h"
#include "physics/physicslib.h"

int main(int argc, char *argv[])
{
    Convert::locstruc loc;
    cosmosstruc* cinfo = json_init();

    Physics::orbit_init_shape(0, 1., currentmjd(), 400000, RADOF(56.), 0., cinfo);
    Convert::cartpos geoc = cinfo->node.loc.pos.geoc;

    //Convert::qatt lvlhq = cinfo->node.loc.att.lvlh;
    Convert::cartpos lvlh;

    lvlh.s.col[0] = 0.;
    lvlh.s.col[1] = 0.;
    lvlh.s.col[2] = 0.;
    if (argc > 1)
    {
        lvlh.s.col[0] = atof(argv[1]);
    }
    if (argc > 2)
    {
        lvlh.s.col[1] = atof(argv[2]);
    }
    if (argc > 3)
    {
        lvlh.s.col[2] = atof(argv[3]);
    }

    Convert::pos_origin2lvlh(cinfo->node.loc, lvlh);
    Convert::pos_lvlh2origin(cinfo->node.loc);

    printf("%f %f %f\n", cinfo->node.loc.pos.lvlh.s.col[0], cinfo->node.loc.pos.lvlh.s.col[1], cinfo->node.loc.pos.lvlh.s.col[2]);
    printf("%f %f %f\n", cinfo->node.loc.pos.geoc.s.col[0]-geoc.s.col[0], cinfo->node.loc.pos.geoc.s.col[1]-geoc.s.col[1], cinfo->node.loc.pos.geoc.s.col[2]-geoc.s.col[2]);
}
