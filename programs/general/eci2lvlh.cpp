// For use in describing an orbit at an exact moment. In doing so, we can ignore...
// - mean motion derivative,
// - mean motion second derivative
// - drag (b^star)

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "physics/physicslib.h"
#include "agent/agentclass.h"
#include <fstream>

// We pass a TLE file to populate the fields that we cannot derive ourselves.
// If used for a short period (10 minutes), these shouldn't matter.
int main(int argc, char *argv[])
{
    if (argc != 8) {
        std::cout << "Usage: state2tle utc sx sy sz vx vy vz" << std::endl;
        exit(1);
    }

    Convert::locstruc loc;
    loc.pos.eci.utc = std::stod(argv[1]);
    loc.pos.eci.s.col[0] = std::stod(argv[2]);
    loc.pos.eci.s.col[1] = std::stod(argv[3]);
    loc.pos.eci.s.col[2] = std::stod(argv[4]);
    loc.pos.eci.v.col[0] = std::stod(argv[5]);
    loc.pos.eci.v.col[1] = std::stod(argv[6]);
    loc.pos.eci.v.col[2] = std::stod(argv[7]);
    loc.pos.eci.pass++;
    Convert::pos_eci(loc);

    Convert::tlestruc tle;
    Convert::eci2tle2(loc.pos.eci, tle);

    Convert::sgp4struc sgp;
    Convert::tle2sgp4(tle, sgp);

    string output;
    output = to_label("MJD", loc.pos.lvlh.utc, 0, true);
    printf("%s\n", output.c_str());

    output = "LVLH -";
    output += " " + to_label("X", loc.pos.lvlh.s.col[0], 8);
    output += " " + to_label("Y", loc.pos.lvlh.s.col[1], 8);
    output += " " + to_label("Z", loc.pos.lvlh.s.col[2], 8);
    output += " " + to_label("VX", loc.pos.lvlh.v.col[0], 8);
    output += " " + to_label("VY", loc.pos.lvlh.v.col[1], 8);
    output += " " + to_label("VZ", loc.pos.lvlh.v.col[2], 8);
    printf("%s\n", output.c_str());

    output = "TLE -";
    output += " " + to_label("I", tle.i, 8);
    output += " " + to_label("ECC", tle.e, 8);
    output += " " + to_label("RAAN", tle.raan, 8);
    output += " " + to_label("AP", tle.raan, 8);
    output += " " + to_label("BSTAR", tle.bstar, 8);
    output += " " + to_label("MM", tle.mm, 8);
    output += " " + to_label("DMM", tle.dmm, 8);
    output += " " + to_label("MA", tle.ma, 8);
    output += " " + to_label("EPOCH", tle.utc, 8);
    printf("%s\n", output.c_str());

    output = "SGP4 -";
    output += " " + to_label("I", sgp.i, 8);
    output += " " + to_label("ECC", sgp.e, 8);
    output += " " + to_label("RAAN", sgp.raan, 8);
    output += " " + to_label("AP", sgp.raan, 8);
    output += " " + to_label("BSTAR", sgp.bstar, 8);
    output += " " + to_label("MM", sgp.mm, 8);
    output += " " + to_label("MA", sgp.ma, 8);
    output += " " + to_label("EPOCH", sgp.ep, 8);
    printf("%s\n", output.c_str());
}
