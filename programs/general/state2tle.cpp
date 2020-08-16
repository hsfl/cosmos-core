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
        std::cout << "Usage: state2tle sx sy sz vx vy vz utc reference-tle-file" << std::endl;
        exit(1);
    }

    // Load our TLE.
    std::ifstream tle_file(argv[8]);
    std::stringstream ref_tle;
    ref_tle << tle_file.rdbuf();

    cartpos eci_pos;
    eci_pos.s.col[0] = std::stod(argv[1]);
    eci_pos.s.col[1] = std::stod(argv[2]);
    eci_pos.s.col[2] = std::stod(argv[3]);
    eci_pos.v.col[0] = std::stod(argv[4]);
    eci_pos.v.col[1] = std::stod(argv[5]);
    eci_pos.v.col[2] = std::stod(argv[6]);
    eci_pos.utc = std::stod(argv[7]);;

    string generated_tle;
    eci2tlestring(eci_pos, generated_tle, ref_tle.str());
    std::cout << generated_tle
              << std::endl;
}
