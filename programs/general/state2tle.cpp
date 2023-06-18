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
    if (argc != 9) {
        std::cout << "Usage: state2tle sx sy sz vx vy vz utc" << std::endl;
        exit(1);
    }

    Convert::cartpos eci_pos;
    Convert::kepstruc kep_pos;
    // Determine Cartesian versus Keplerian
    Vector test(std::stod(argv[1]), std::stod(argv[2]), std::stod(argv[3]));
    if (test.norm() > REARTHM)
    {
    eci_pos.s.col[0] = std::stod(argv[1]);
    eci_pos.s.col[1] = std::stod(argv[2]);
    eci_pos.s.col[2] = std::stod(argv[3]);
    eci_pos.v.col[0] = std::stod(argv[4]);
    eci_pos.v.col[1] = std::stod(argv[5]);
    eci_pos.v.col[2] = std::stod(argv[6]);
    eci_pos.utc = std::stod(argv[7]);
    Convert::eci2kep(eci_pos, kep_pos);
    }
    else
    {
        kep_pos.ea = std::stod(argv[1]);
        kep_pos.i = std::stod(argv[2]);
        kep_pos.ap = std::stod(argv[3]);
        kep_pos.raan = std::stod(argv[4]);
        kep_pos.e = std::stod(argv[5]);
        kep_pos.a = std::stod(argv[6]);
        kep_pos.utc = std::stod(argv[7]);
        Convert::eci2kep(eci_pos, kep_pos);
    }

    string output;
    output = to_label("MJD", eci_pos.utc, 0, true);
    printf("%s\n", output.c_str());

    output = "ECI -";
    output += " " + to_label("X", eci_pos.s.col[0], 8);
    output += " " + to_label("Y", eci_pos.s.col[1], 8);
    output += " " + to_label("Z", eci_pos.s.col[2], 8);
    output += " " + to_label("VX", eci_pos.v.col[0], 8);
    output += " " + to_label("VY", eci_pos.v.col[1], 8);
    output += " " + to_label("VZ", eci_pos.v.col[2], 8);
    printf("%s\n", output.c_str());

    output = "Keplerian -";
    output += " " + to_label("EA", kep_pos.ea, 8);
    output += " " + to_label("I", kep_pos.i, 8);
    output += " " + to_label("AP", kep_pos.ap, 8);
    output += " " + to_label("RAAN", kep_pos.raan, 8);
    output += " " + to_label("E", kep_pos.e, 8);
    output += " " + to_label("A", kep_pos.a, 8);
    printf("%s\n", output.c_str());

    string generated_tle;
    std::cout << generated_tle
              << std::endl;
}
