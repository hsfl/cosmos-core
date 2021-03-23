#include "support/configCosmos.h"
#include "support/convertlib.h"


int main(int argc, char *argv[])
{
    vector<tlestruc>lines;
    locstruc loc;
    int32_t iretn;

    loc_clear(loc);
    string fname = argv[1];
    if (fname.find(".tle") != string::npos)
    {
        iretn = load_lines(fname, lines);
        iretn = lines2eci(lines[0].utc, lines, loc.pos.eci);
    }

    tlestruc tle2;
    locstruc loc2;
    eci2tle(loc.pos.eci.utc, loc.pos.eci, tle2);
    for (double dt=0.; dt<1.; dt+=.1)
    {
        lines2eci(lines[0].utc+dt, lines, loc.pos.eci);
        tle2eci(tle2.utc+dt, tle2, loc2.pos.eci);

        printf("tle1/2: %f/%f %f/%f %f/%f %f\n", loc.pos.eci.s.col[0], loc2.pos.eci.s.col[0], loc.pos.eci.s.col[1], loc2.pos.eci.s.col[1], loc.pos.eci.s.col[2], loc2.pos.eci.s.col[2], norm_rv(rv_sub(loc2.pos.eci.s,loc.pos.eci.s)));
    }

}
