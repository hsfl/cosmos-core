#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"
#include "support/datalib.h"

using namespace Convert;

Vector calc_control_torque_b(Convert::qatt tatt, Convert::qatt catt, Vector moi, double portion);

static Physics::Simulator::StateList::iterator sit;
static Physics::Simulator *sim;
static Agent *agent;
static double speed=1.;
static double maxaccel = .1;
static double minaccel;
static double initialutc = 59270.949409722227;
static double initiallat = RADOF(21.3069);
static double initiallon = RADOF(-157.8583);
static double initialalt = 400000.;
static double initialangle = RADOF(54.);
static Convert::locstruc initialloc;
static double initialsep = 0.;
static double deltat = .0655;
static double runcount = 1500;
static double currentutc;
static double simdt = 1.;
static double attdt = 1.;
static double minaccelratio = 10;
constexpr double d2s = 1./86400.;
constexpr double d2s2 = 1./(86400.*86400.);
static string targetfile = "targets.dat";

// 0 : string of pearls
// 1 : surrounding diamond
// 2 : reverse string of pearls
// 3 : high/low attraction experiment
static uint8_t shapetype = 0;
static double shapeseparation = 500.;
static LsFit tattfit;
static LsFit omegafit;
static bool altprint = false;

int main(int argc, char *argv[])
{
    int32_t iretn = 0;

    if (argc != 5)
    {
        printf("Usage: targetsim targetfile inclination altitude separation\n");
        exit(1);
    }

    string targetfile = argv[1];
    FILE *fp = fopen(targetfile.c_str(), "r");
    if (fp == nullptr)
    {
        agent->debug_error.Printf("Bad Target File: %s\n", targetfile.c_str());
        exit(1);
    }

    struct targetentry
    {
        string name;
        double clat;
        double clon;
        double alt;
        double size;
    };

    string line;
    line.resize(200);
//    double avglat = 0.;
//    double avglon = 0.;
//    double avgalt = 0.;
    double minlat = 999.;
    double minlon = 999.;
    double minalt = 9999999.;
    double maxlat = -999.;
    double maxlon = -999.;
    double maxalt = 0.;
    vector<targetentry> targets;
    while (fgets((char *)line.data(), 200, fp) != nullptr)
    {
        targetentry target;
        vector<string> args = string_split(line);

        if (args.size() == 5)
        {
            target.name = args[0];
            target.clat = RADOF(stod(args[1]));
//            avglat += target.clat;
            if (target.clat < minlat)
            {
                minlat = target.clat;
            }
            if (target.clat > maxlat)
            {
                maxlat = target.clat;
            }
            target.clon = RADOF(stod(args[2]));
//            avglon += target.clon;
            if (target.clon < minlon)
            {
                minlon = target.clon;
            }
            if (target.clon > maxlon)
            {
                maxlon = target.clon;
            }
            target.alt = stod(args[3]);
//            avgalt += target.alt;
            if (target.alt < minalt)
            {
                minalt = target.alt;
            }
            if (target.alt > maxalt)
            {
                maxalt = target.alt;
            }
            target.size = stod(args[4]);
            targets.push_back(target);
        }
    }
//    avglat /= targets.size();
//    avglon /=  targets.size();
//    avgalt /= targets.size();
    double sizelat = (maxlat - minlat) / 2.;
    double sizelon = (maxlon - minlon) / 2.;

    initialangle = RADOF(atof(argv[2]));
    initialalt = atof(argv[3]);
    initialsep = atof(argv[4]);
    initiallat = minlat - sin(initialangle) * (sizelat + RADOF(10.));
    if (cos(initialangle) > 0)
    {
        initiallon = minlon - cos(initialangle) * (sizelon + RADOF(10.) / cos(initialloc.pos.geod.s.lat));
    }
    else
    {
        initiallon = maxlon - cos(initialangle) * (sizelon + RADOF(10.) / cos(initialloc.pos.geod.s.lat));
    }
    initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
    if (fabs(initialloc.pos.geod.v.lat) > fabs(initialloc.pos.geod.v.lon))
    {
        runcount = (maxlat - initiallat) / initialloc.pos.geod.v.lat;
    }
    else
    {
        runcount = (maxlon - initiallon) / initialloc.pos.geod.v.lon;
    }

    // initialize simulation agent
    agent = new Agent("", "", 0.);
    agent->set_debug_level(0);

    // initialize simulator object
    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn <0) {
        agent->debug_error.Printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    currentutc = initialutc;
    sim->Init(currentutc, simdt);

//    initialloc.att.icrf.s = q_eye();
    vector<tlestruc> tles;
    tles.resize(1);
    eci2tle(initialutc, initialloc.pos.eci, tles[0]);
    iretn = sim->AddNode("mother", Physics::Structure::HEX65W80H, Physics::Propagator::PositionTle, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, tles);
    sit = sim->GetNode("mother");
    for (targetentry target : targets)
    {
        sit->second->AddTarget(target.name, target.clat, target.clon, target.size, target.alt, NODE_TYPE_CIRCLE);
    }

    double elapsed = 0;
    double pcount = 0.;
    minaccel = maxaccel / minaccelratio;
    locstruc lastloc;
    vector<targetstruc> lasttargets;
    for (uint32_t i=0; i<runcount; ++i)
    {
        // update states information for all nodes
        lastloc = sit->second->currentinfo.node.loc;
        lasttargets = sit->second->targets;
        sim->Propagate();
        pcount += simdt;

        for (uint16_t id=0; id<sit->second->targets.size(); ++id)
        {
            // Check ahead for target
//            double cd = cos(heading - sit->second->targets[id].bearing);
        }

        ++elapsed;
    }

    agent->shutdown();
}
