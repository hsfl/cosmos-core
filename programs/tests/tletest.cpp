#include "support/configCosmos.h"
#include "support/convertlib.h"


int main(int argc, char *argv[])
{
    vector<Convert::tlestruc>lines;
    Convert::locstruc loc;

    loc_clear(loc);
    string fname = argv[1];
    if (fname.find(".tle") != string::npos)
    {
        load_lines(fname, lines);
        Convert::lines2eci(lines[0].utc, lines, loc.pos.eci);
    }

    Convert::tle2eci(lines[0].utc, lines[0], loc.pos.eci);

    Convert::tlestruc tle2;
    Convert::locstruc loc2;
    Convert::kepstruc kep;
    Convert::eci2kep(loc.pos.eci, kep);
    Convert::kep2eci(kep, loc2.pos.eci);


//    Convert::eci2tle(loc.pos.eci.utc, loc.pos.eci, tle2);
//    Convert::tle2eci(tle2.utc, tle2, loc2.pos.eci);
//    tle2.bstar = 300.*lines[0].bstar;
//    for (double dt=0.; dt<1.; dt+=.01)
//    {
//        lines2eci(lines[0].utc+dt, lines, loc.pos.eci);
//        tle2eci(tle2.utc+dt, tle2, loc2.pos.eci);

//        printf("tle1/2: %f %f %f %f %f %f %f %f\n", dt, loc.pos.eci.s.col[0], loc2.pos.eci.s.col[0], loc.pos.eci.s.col[1], loc2.pos.eci.s.col[1], loc.pos.eci.s.col[2], loc2.pos.eci.s.col[2], length_rv(rv_sub(loc2.pos.eci.s,loc.pos.eci.s)));
//    }
//    printf("\n");

    Convert::tlestruc tle3;
    Convert::tle2eci(lines[0].utc, lines[0], loc.pos.eci);
    Convert::rv2tle(loc.pos.eci.utc, loc.pos.eci, tle3);
    Convert::tle2eci(tle3.utc, tle3, loc2.pos.eci);
//    tle3.bstar = 4.7*lines[0].bstar;
    for (double dt=0.; dt<1.; dt+=.1)
    {
        lines2eci(lines[0].utc+dt, lines, loc.pos.eci);
        Convert::tle2eci(tle3.utc+dt, tle3, loc2.pos.eci);

        printf("tle1/3: %f %f %f %f %f %f %f %f\n", dt, loc.pos.eci.s.col[0], loc2.pos.eci.s.col[0], loc.pos.eci.s.col[1], loc2.pos.eci.s.col[1], loc.pos.eci.s.col[2], loc2.pos.eci.s.col[2], length_rv(rv_sub(loc2.pos.eci.s,loc.pos.eci.s)));
    }
}
