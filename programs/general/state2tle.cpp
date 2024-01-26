// For use in describing an orbit at an exact moment. In doing so, we can ignore...
// - mean motion derivative,
// - mean motion second derivative
// - drag (b^star)

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "physics/physicslib.h"
#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include <fstream>

string nodename;

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "Usage: state2tle {json_string}" << std::endl;
        exit(1);
    }

    double initialutc;
    double initiallat = RADOF(21.3069);
    double initiallon = RADOF(-157.8583);
    double initialalt = 400000.;
    double initialangle = RADOF(54.);
    Convert::locstruc initialloc;
    string args(argv[1]);
    uint16_t argcount = 0;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
    if (!jargs["phys"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["phys"].object_items();
        initiallat = RADOF(values["lat"].number_value());
        initiallon = RADOF(values["lon"].number_value());
        initialalt = values["alt"].number_value();
        initialangle = RADOF(values["angle"].number_value());
            initialutc = currentmjd();
        initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
        pos_eci(initialloc);
        eci2tle2(initialloc.pos.eci, initialloc.tle);
//        tle2eci(initialutc, initialloc.tle, initialloc.pos.eci);
//        pos_eci(initialloc);
    }
    if (!jargs["eci"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["eci"].object_items();
        initialloc.pos.eci.utc = (values["utc"].number_value());
        initialloc.pos.eci.s.col[0] = (values["x"].number_value());
        initialloc.pos.eci.s.col[1] = (values["y"].number_value());
        initialloc.pos.eci.s.col[2] = (values["z"].number_value());
        initialloc.pos.eci.v.col[0] = (values["vx"].number_value());
        initialloc.pos.eci.v.col[1] = (values["vy"].number_value());
        initialloc.pos.eci.v.col[2] = (values["vz"].number_value());
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
        eci2tle2(initialloc.pos.eci, initialloc.tle);
//        type = Physics::Propagator::PositionTle;
    }
    if (!jargs["kep"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["kep"].object_items();
        kepstruc kep;
        if (initialutc == 0.)
        {
            initialutc = currentmjd();
        }
        kep.utc = initialutc;
        kep.ea = values["ea"].number_value();
        kep.i = values["i"].number_value();
        kep.ap = values["ap"].number_value();
        kep.raan = values["raan"].number_value();
        kep.e = values["e"].number_value();
        kep.a = values["a"].number_value();
        kep2eci(kep, initialloc.pos.eci);
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
        eci2tle2(initialloc.pos.eci, initialloc.tle);
//        type = Physics::Propagator::PositionTle;
    }
    if (!jargs["tle"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["tle"].object_items();
        vector<Convert::tlestruc>lines;
        string fname = get_nodedir(nodename, true) + "/" + values["filename"].string_value();
        load_lines(fname, lines);
        if (initialutc == 0.)
        {
            initialutc = lines[0].utc;
        }
        initialloc.tle = lines[0];
        tle2eci(initialutc, initialloc.tle, initialloc.pos.eci);
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
//        type = Physics::Propagator::PositionTle;
    }

    initialloc.tle.id = "00000A";
    string tlestring = tle2tlestring(initialloc.tle);
    printf("%s\n", tlestring.c_str());
}
