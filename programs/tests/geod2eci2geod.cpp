#include "support/configCosmos.h"
#include "support/convertlib.h"
#include "support/timelib.h"

using namespace Convert;

int main(int argc, char *argv[])
{
    locstruc iloc;
    locstruc cloc;

    iloc.pos.geod.utc = 59270.949421299999813;
    iloc.pos.geod.s.lat = 0.371876;
    iloc.pos.geod.s.lon = -2.755150;
    iloc.pos.geod.s.h = 0.;
    iloc.pos.geod.v = gv_zero();
    iloc.pos.geod.a = gv_zero();
    iloc.pos.geod.pass++;
    pos_geod(iloc);

    cloc = iloc;
    for (uint16_t id=0; id<10; id++)
    {
        cloc.pos.geod.pass++;
        pos_geod(cloc);
        printf("[%u] geod: %f %f %f eci %f %f %f\n", id, iloc.pos.geod.s.lat - cloc.pos.geod.s.lat, iloc.pos.geod.s.lon - cloc.pos.geod.s.lon, iloc.pos.geod.s.h - cloc.pos.geod.s.h, iloc.pos.eci.s.col[0] - cloc.pos.eci.s.col[0], iloc.pos.eci.s.col[1] - cloc.pos.eci.s.col[1], iloc.pos.eci.s.col[2] - cloc.pos.eci.s.col[2]);
        cloc.pos.eci.pass++;
        pos_eci(cloc);
    }
}
