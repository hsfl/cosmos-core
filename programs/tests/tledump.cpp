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

    for (double dt=0.; dt<(D2PI/lines[0].mm)/1400; dt+=10./86400)
    {
        Convert::lines2eci(lines[0].utc+dt, lines, loc.pos.eci);
        ++loc.pos.eci.pass;
        Convert::pos_eci(loc);

        printf("MJD:\t%f\t%f\tECI:\t%f\t%f\t%f\tGEOD:\t%f\t%f\t%f\n"
               , dt, lines[0].utc+dt
               , loc.pos.eci.s.col[0], loc.pos.eci.s.col[1], loc.pos.eci.s.col[2]
                , DEGOF(loc.pos.geod.s.lat), DEGOF(loc.pos.geod.s.lon), (loc.pos.geod.s.h)
                );
    }
}
