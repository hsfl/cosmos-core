#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"
#include "support/datalib.h"

using namespace Convert;

Vector calc_control_torque_b(Convert::qatt tatt, Convert::qatt catt, Vector moi, double portion);

static Physics::Simulator::StateList::iterator sit;
static Physics::Simulator *sim;
static Agent *agent;
static double initialutc = 59270.949409722227;
static double initiallat = RADOF(21.3069);
static double initiallon = RADOF(-157.8583);
static double altitude = 400000.;
static double inclination = RADOF(54.);
static Convert::locstruc initialloc;
static double separation = 0.;
//static double deltat = .0655;
static double runcount = 1500;
static double currentutc;
static double simdt = 1.;
constexpr double d2s = 1./86400.;
constexpr double d2s2 = 1./(86400.*86400.);
static string targetfile = "targets.dat";
uint16_t pixels = 1024;
double footprint = 50000.;
double ifov = (footprint / pixels) / altitude; // radians
double resolution = 60.; // meters
double timespan = 100.;

// 0 : string of pearls
// 1 : surrounding diamond
// 2 : reverse string of pearls
// 3 : high/low attraction experiment
static LsFit tattfit;
static LsFit omegafit;

int main(int argc, char *argv[])
{
    int32_t iretn = 0;

    //    if (argc != 5)
    //    {
    //        printf("Usage: targetsim targetfile inclination altitude separation\n");
    //        exit(1);
    //    }

    string estring;
    json11::Json jargs = json11::Json::parse(argv[1], estring);
    if (!jargs["targetfile"].is_null()) targetfile = jargs["targetfile"].string_value();
    if (!jargs["runcount"].is_null()) runcount = jargs["runcount"].number_value();
    if (!jargs["inclination"].is_null()) inclination = jargs["inclination"].number_value();
    if (!jargs["altitude"].is_null()) altitude = jargs["altitude"].number_value();
    if (!jargs["separation"].is_null()) separation = jargs["separation"].number_value();
    if (!jargs["pixels"].is_null()) pixels = jargs["pixels"].number_value();
    if (!jargs["footprint"].is_null()) footprint = jargs["footprint"].number_value();
    if (!jargs["resolution"].is_null()) resolution = jargs["resolution"].number_value();
    if (!jargs["timespan"].is_null()) timespan = jargs["timespan"].number_value();

    ifov = (footprint / pixels) / altitude;

    FILE *fp = fopen(targetfile.c_str(), "r");
    if (fp == nullptr)
    {
        agent->debug_log.Printf("Bad Target File: %s\n", targetfile.c_str());
        exit(1);
    }

    struct targetentry
    {
        string name;
        double clat;
        double clon;
        double alt;
        double area;
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
            target.clon = RADOF(stod(args[1]));
            //            avglon += target.clon;
            if (target.clon < minlon)
            {
                minlon = target.clon;
            }
            if (target.clon > maxlon)
            {
                maxlon = target.clon;
            }
            target.clat = RADOF(stod(args[2]));
            //            avglat += target.clat;
            if (target.clat < minlat)
            {
                minlat = target.clat;
            }
            if (target.clat > maxlat)
            {
                maxlat = target.clat;
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
            target.area = stod(args[4]);
            target.area = DPI * (target.area/2.) * (target.area/2.) * 1e6;
            targets.push_back(target);
        }
    }
//    double sizelat = fabs(maxlat - minlat) / 2.;
//    double sizelon = fabs(maxlon - minlon) / 2.;
//    double avglat = (maxlat + minlat) / 2.;

//    if (inclination > 0)
//    {
//        initiallat = minlat - sin(inclination) * RADOF(2.) / cos(avglat);
//        if (cos(inclination) > 0)
//        {
//            initiallon = minlon - cos(inclination) * (RADOF(2.) / cos(initiallat));
//        }
//        else
//        {
//            initiallon = maxlon + cos(inclination) * (RADOF(2.) / cos(initiallat));
//        }
//    }
//    else
//    {
//        initiallat = maxlat + sin(inclination) * RADOF(2.) / cos(avglat);
//        if (cos(inclination) > 0)
//        {
//            initiallon = minlon - cos(inclination) * (RADOF(2.) / cos(initiallat));
//        }
//        else
//        {
//            initiallon = maxlon + cos(inclination) * (RADOF(2.) / cos(initiallat));
//        }
//    }
//    initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, altitude, inclination, 0.);
    initiallat = (maxlat + minlat) / 2.;
    initiallon = (maxlon + minlon) / 2.;
    initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, altitude, inclination, -timespan/2.);

    // Calculate run time
//    double range = 2. * sqrt(sizelat * sizelat + sizelon * sizelon);
//    double speed = sqrt(initialloc.pos.geod.v.lat * initialloc.pos.geod.v.lat + initialloc.pos.geod.v.lon * initialloc.pos.geod.v.lon);
    runcount = timespan;

    // initialize simulation agent
    agent = new Agent("", "", "", 0.);
    agent->set_debug_level(0);

    // initialize simulator object
    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn <0) {
        agent->debug_log.Printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    currentutc = initialutc;
    sim->Init(currentutc, simdt);

    //    initialloc.att.icrf.s = q_eye();
    tlestruc tle;
    eci2tle(initialloc.pos.eci, tle);
    iretn = sim->AddNode("mother", "HEX65W80H", Physics::Propagator::PositionTle, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, tle);
    sit = sim->GetNode("mother");
    for (targetentry target : targets)
    {
        sim->AddTarget(target.name, target.clat, target.clon, target.area, target.alt, NODE_TYPE_CIRCLE);
    }

    double elapsed = 0;
    double pcount = 0.;
    locstruc lastloc;
    vector<targetstruc> lasttargets;
    printf("Step\tMJD\tECIsx\tECIsy\tECIsz\tECIvx\tECIvy\tECIvz\tLon\tLat\tAlt");
    for (uint16_t id=0; id<(*sit)->currentinfo.target.size(); ++id)
    {
        printf("\tIndex%02u\tLon%02u\tLat%02u\tRange%02u\tEL%02u\tAz%02u\tNadirArea%02u\tPointArea%02u\tTargetArea%02u",id,id,id,id,id,id,id,id,id);
    }
    printf("\n");
    for (uint32_t i=0; i<runcount; ++i)
    {
        // update states information for all nodes
        lastloc = (*sit)->currentinfo.node.loc;
        lasttargets = (*sit)->currentinfo.target;
        sim->Propagate();
        printf("%u\t%f\t%.1f\t%.1f\t%.1f\t%.1f\t%.1f\t%.1f\t%.2f\t%.2f\t%.1f"
               , i
               , (*sit)->currentinfo.node.loc.pos.eci.utc
               , (*sit)->currentinfo.node.loc.pos.eci.s.col[0]
               , (*sit)->currentinfo.node.loc.pos.eci.s.col[1]
               , (*sit)->currentinfo.node.loc.pos.eci.s.col[2]
                , (*sit)->currentinfo.node.loc.pos.eci.v.col[0]
                , (*sit)->currentinfo.node.loc.pos.eci.v.col[1]
                , (*sit)->currentinfo.node.loc.pos.eci.v.col[2]
                , DEGOF((*sit)->currentinfo.node.loc.pos.geod.s.lon)
                , DEGOF((*sit)->currentinfo.node.loc.pos.geod.s.lat)
                , (*sit)->currentinfo.node.loc.pos.geod.s.h
               );
        pcount += simdt;

        double nadirres = (*sit)->currentinfo.node.loc.pos.geod.s.h * ifov;
        double nadirradius = nadirres * pixels / 2.;
        double nadirradius2 = nadirradius * nadirradius;
        double nadirarea = DPI * nadirradius2;

        // Perform metrics for each target
        for (uint16_t id=0; id<(*sit)->currentinfo.target.size(); ++id)
        {
            printf("\t%u\t%.2f\t%.2f\t%.1f\t%.1f\t%.0f"
                   , id
                   , DEGOF((*sit)->currentinfo.target[id].loc.pos.geod.s.lon)
                   , DEGOF((*sit)->currentinfo.target[id].loc.pos.geod.s.lat)
                   , (*sit)->currentinfo.target[id].range
                   , DEGOF((*sit)->currentinfo.target[id].elfrom)
                   , DEGOF((*sit)->currentinfo.target[id].azfrom)
                   );

            double eres = (*sit)->currentinfo.target[id].range * ifov;
            // double cameraradius;
            double cameraarea;
            double targetarea = (*sit)->currentinfo.target[id].area;
            double targetradius = sqrt(targetarea / DPI);
            double targetradius2 = targetradius * targetradius;
            double distance = (*sit)->currentinfo.target[id].range * cos((*sit)->currentinfo.target[id].elfrom);
            double distance2 = distance * distance;

            // Looking Nadir
            if (distance < nadirradius + targetradius)
            {
                if (distance > targetradius - nadirradius)
                {
                    double nadirdistance = (nadirradius2 - targetradius2 + distance2) / (2. * distance);
                    double nadirdistance2 = nadirdistance * nadirdistance;
                    double targetdistance = (targetradius2 - nadirradius2 + distance2) / (2. * distance);
                    double targetdistance2 = targetdistance * targetdistance;
                    cameraarea = nadirradius2 * acos(nadirdistance / nadirradius)
                                 - nadirdistance * sqrt(nadirradius2 - nadirdistance2)
                                 + targetradius2 * acos(targetdistance / targetradius)
                                 - targetdistance * sqrt(targetradius2 - targetdistance2);
                }
                else
                {
                    cameraarea = nadirarea;
                }
            }
            else
            {
                cameraarea = 0.;
            }
            printf("\t%.3f", cameraarea);

            // Elongated FOV
            if (eres <= resolution)
            {
                cameraarea = DPI * eres * (eres / -sin((*sit)->currentinfo.target[id].elfrom)) * (pixels / 2.) * (pixels / 2.);
                if (cameraarea > targetarea)
                {
                    cameraarea = targetarea;
                }
            }
            else
            {
                cameraarea = 0.;
            }
            printf("\t%.3f\t%.3f", cameraarea, targetarea);
        }
        printf("\n");
        fflush(stdout);

        ++elapsed;
    }

    agent->shutdown();
}
