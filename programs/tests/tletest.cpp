#include "support/configCosmos.h"
#include "support/convertlib.h"


int main(int argc, char *argv[])
{
    vector<Convert::tlestruc>lines;
    Convert::locstruc loc0;
    Convert::locstruc loc1;
    Convert::locstruc loc2;
    Convert::locstruc loc3;
    Convert::tlestruc tle1;
    Convert::tlestruc tle2;
    Convert::tlestruc tle3;

    string fname = argv[1];
    {
        load_lines(fname, lines);
    }

//    double mjd0 = currentmjd();
    double mjd0 = lines[0].utc;
//    double mjd1 = mjd0 + 1.;
    loc_clear(loc0);
    Convert::tle2eci(mjd0, lines[0], loc0.pos.eci);
    Convert::eci2tle2(loc0.pos.eci, tle1);
    ++loc0.pos.eci.pass;
    Convert::pos_eci(loc0);
    Vector utm;
    Convert::geod2utm(loc0.pos.geod, utm);

    loc_clear(loc0);
    Convert::tle2eci(mjd0, lines[0], loc0.pos.eci);
    Convert::eci2tle2(loc0.pos.eci, tle1);

    loc_clear(loc1);
    Convert::tle2eci(mjd0, tle1, loc1.pos.eci);

    Convert::eci2tle2(loc1.pos.eci, tle2);
    loc_clear(loc2);
    Convert::tle2eci(mjd0, tle2, loc2.pos.eci);

    Convert::eci2tle2(loc2.pos.eci, tle3);
    loc_clear(loc3);
    Convert::tle2eci(mjd0, tle3, loc3.pos.eci);

    Convert::kepstruc kep;
    Convert::eci2kep(loc1.pos.eci, kep);
    Convert::kep2eci(kep, loc2.pos.eci);


//    Convert::eci2tle2(loc1.pos.eci.utc, loc1.pos.eci, tle2);
//    Convert::tle2eci(tle2.utc, tle2, loc2.pos.eci);
//    tle2.bstar = 300.*lines[0].bstar;
//    for (double dt=0.; dt<1.; dt+=.01)
//    {
//        lines2eci(mjd0+dt, lines, loc1.pos.eci);
//        tle2eci(tle2.utc+dt, tle2, loc2.pos.eci);

//        printf("tle1/2: %f %f %f %f %f %f %f %f\n", dt, loc1.pos.eci.s.col[0], loc2.pos.eci.s.col[0], loc1.pos.eci.s.col[1], loc2.pos.eci.s.col[1], loc1.pos.eci.s.col[2], loc2.pos.eci.s.col[2], length_rv(rv_sub(loc2.pos.eci.s,loc1.pos.eci.s)));
//    }
//    printf("\n");

    Convert::tle2eci(mjd0, lines[0], loc1.pos.eci);
    Convert::eci2tle2(loc1.pos.eci, tle3);
    Convert::tle2eci(tle3.utc, tle3, loc2.pos.eci);
//    tle3.bstar = 4.7*lines[0].bstar;
    for (double dt=0.; dt<1.; dt+=.1)
    {
        lines2eci(mjd0+dt, lines, loc1.pos.eci);
        Convert::tle2eci(tle3.utc+dt, tle3, loc2.pos.eci);

        printf("tle1/3: %f %f %f %f %f %f %f %f\n", dt, loc1.pos.eci.s.col[0], loc2.pos.eci.s.col[0], loc1.pos.eci.s.col[1], loc2.pos.eci.s.col[1], loc1.pos.eci.s.col[2], loc2.pos.eci.s.col[2], length_rv(rv_sub(loc2.pos.eci.s,loc1.pos.eci.s)));
    }
}
