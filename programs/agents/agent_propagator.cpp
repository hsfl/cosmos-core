#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"
#include "support/datalib.h"
#include "support/beacon.h"
#include "support/packetcomm.h"
#include "support/packethandler.h"

using namespace Convert;

Vector calc_control_torque_b(Convert::qatt tatt, Convert::qatt catt, Vector moi, double portion);

static Physics::Simulator::StateList::iterator sit;
static Physics::Simulator *sim;
static Agent *agent;
string agentname = "propagate";
static double speed=1.;
static double maxaccel = .1;
static double minaccel;
double offset = 0.;
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
string targetfile = "targets.dat";
string satfile = "sats.dat";

//// 0 : string of pearls
//// 1 : surrounding diamond
//// 2 : reverse string of pearls
//// 3 : high/low attraction experiment
//static uint8_t shapetype = 0;
//static double shapeseparation = 500.;
static LsFit tattfit;
static LsFit omegafit;
//static bool altprint = false;
int32_t parse_jargs(string args);
int32_t parse_sat(string args);

vector<cosmosstruc> sats;

int main(int argc, char *argv[])
{
    int32_t iretn = 0;
    FILE *fp;

    // initialize simulation agent
    if (argc > 1)
    {
        parse_jargs(argv[1]);
    }

    agent = new Agent("", agentname, 0.);
    agent->set_debug_level(0);

    // initialize simulator object
    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn <0) {
        agent->debug_log.Printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }
    currentutc = initialutc;
    sim->Init(currentutc, simdt);

    // Load in satellites
    fp = fopen(satfile.c_str(), "r");
    char buf[150];
    while (fgets(buf, 149, fp) != nullptr)
    {
        parse_sat(buf);
    }

    initialloc.att.icrf.s = q_eye();
    iretn = sim->AddNode("mother", Physics::Structure::HEX65W80H, Physics::Propagator::PositionTle, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, initialloc.pos.eci, initialloc.att.icrf);
    sit = sim->GetNode("mother");

    // Load in targets
    fp = fopen(targetfile.c_str(), "r");
    if (fp == nullptr)
    {
        agent->debug_log.Printf("Bad Target File: %s\n", targetfile.c_str());
        agent->shutdown();
        exit(1);
    }
    string line;
    line.resize(200);
    while (fgets((char *)line.data(), 200, fp) != nullptr)
    {
        vector<string> args = string_split(line);
        if (args.size() == 4)
        {
            sit->second->AddTarget(args[0], RADOF(stof(args[1])), RADOF(stod(args[2])), 0., stod(args[3]), NODE_TYPE_GROUNDSTATION);
        }
        else if (args.size() == 5)
        {
            sit->second->AddTarget(args[0], RADOF(stof(args[1])), RADOF(stod(args[2])), RADOF(stof(args[3])), RADOF(stod(args[4])));
        }
    }
    double mjd = currentmjd();

    PacketComm packet;
    double elapsed = 0;
    double pcount = 0.;
    minaccel = maxaccel / minaccelratio;
    locstruc lastloc;
    vector<targetstruc> lasttargets;
    ElapsedTime ret;
    offset = initialutc - currentmjd();
    while (agent->running())
    {
//        iretn = PacketHandler::CreateBeacon(packet, static_cast<uint8_t>(Beacon::TypeId::NodeLocBeacon), agent);
//        iretn = PacketHandler::CreateBeacon(packet, static_cast<uint8_t>(Beacon::TypeId::NodePhysBeacon), agent);
//        iretn = PacketHandler::CreateBeacon(packet, static_cast<uint8_t>(Beacon::TypeId::NodeTargetBeacon), agent);

        // Attitude adjustment
        // Desired attitude comes from aligning satellite Z with desired Z and satellite Y with desired Y
        qatt tatt;
        qatt satt;
        locstruc stloc;

        // update states information for all nodes
        lastloc = sit->second->currentinfo.node.loc;
        lasttargets = sit->second->targets;
        sim->Propagate();
        pcount += simdt;

        stloc.pos.eci.s = rv_sub(sit->second->targets[0].loc.pos.eci.s, sit->second->currentinfo.node.loc.pos.eci.s);

        Vector eci_z = Vector(stloc.pos.eci.s);

        // Desired Y is cross product of Desired Z and velocity vector
        Vector eci_y = eci_z.cross(Vector(sit->second->currentinfo.node.loc.pos.eci.v));

        tatt.utc = currentmjd() - offset;
        tatt.s = irotate_for(eci_y, eci_z, unityV(), unitzV()).to_q();
        rvector test;
        test  = rv_smult(eci_y.norm(), drotate(tatt.s, rv_unity()));
        test  = rv_smult(eci_z.norm(), drotate(tatt.s, rv_unitz()));
        tattfit.update(tatt.utc, tatt.s);
        Quaternion dqs = Quaternion(q_smult(d2s, tattfit.slopequaternion(tatt.utc)));
        Quaternion omega = 2. * dqs * Quaternion(tatt.s).conjugate();
        tatt.v.col[0] = omega.x;
        tatt.v.col[1] = omega.y;
        tatt.v.col[2] = omega.z;
        omegafit.update(tatt.utc, tatt.v);
        tatt.a = rv_smult(d2s, omegafit.slopervector(tatt.utc));

        // Determine full rotation of Desired into Satellite
        //                satt = sit->second->currentinfo.node.loc.att.icrf;
        //                sit->second->currentinfo.node.phys.ftorque = rv_smult(1., calc_control_torque_b(tatt, satt, sit->second->currentinfo.node.phys.moi, 1.).to_rv());
        sit->second->currentinfo.node.loc.att.icrf = tatt;


        // Heading and Angular Velocity
        double cphi = cos(sit->second->currentinfo.node.loc.pos.geos.v.phi);
        double cphi2 = cphi * cphi;
        double phi2 = sit->second->currentinfo.node.loc.pos.geos.v.phi * sit->second->currentinfo.node.loc.pos.geos.v.phi;
        double lambda2 = sit->second->currentinfo.node.loc.pos.geos.v.lambda * sit->second->currentinfo.node.loc.pos.geos.v.lambda;
        double anglev;
        double heading;
        if (cphi2 != 0.)
        {
            anglev = sqrt(phi2 + lambda2 / cphi2);
            heading = atan2(sit->second->currentinfo.node.loc.pos.geos.v.phi, sit->second->currentinfo.node.loc.pos.geos.v.lambda / cphi);
        }
        else
        {
            anglev = sqrt(phi2);
            heading = DPI;
        }

        ++elapsed;
        secondsleep(simdt - ret.lap());
        ret.reset();
    }

    agent->shutdown();
}

int32_t parse_jargs(string args)
{
    uint16_t argcount = 0;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
//    if (!jargs["altprint"].is_null())
//    {
//        ++argcount;
//        altprint = jargs["altprint"].bool_value();
//    }
//    if (!jargs["shapetype"].is_null())
//    {
//        ++argcount;
//        shapetype = jargs["shapetype"].number_value();
//    }
//    if (!jargs["shapeseparation"].is_null())
//    {
//        ++argcount;
//        shapeseparation = jargs["shapeseparation"].number_value();
//    }
    if (!jargs["runcount"].is_null())
    {
        ++argcount;
        runcount = jargs["runcount"].number_value();
    }
    if (!jargs["speed"].is_null())
    {
        ++argcount;
        speed = jargs["speed"].number_value();
    }
    if (!jargs["maxaccel"].is_null())
    {
        ++argcount;
        maxaccel = jargs["maxaccel"].number_value();
    }
    if (!jargs["initialutc"].is_null())
    {
        ++argcount;
        initialutc = jargs["initialutc"].number_value();
    }
    if (!jargs["initialsep"].is_null())
    {
        ++argcount;
        initialsep = jargs["initialsep"].number_value();
    }
    if (!jargs["deltat"].is_null())
    {
        ++argcount;
        deltat = jargs["deltat"].number_value();
    }
    if (!jargs["simdt"].is_null())
    {
        ++argcount;
        simdt = jargs["simdt"].number_value();
    }
    if (!jargs["attdt"].is_null())
    {
        ++argcount;
        attdt = jargs["attdt"].number_value();
    }
    if (!jargs["minaccelratio"].is_null())
    {
        ++argcount;
        minaccelratio = jargs["minaccelratio"].number_value();
    }
    if (!jargs["targetfile"].is_null())
    {
        ++argcount;
        targetfile = jargs["targetfile"].string_value();
    }
    if (!jargs["agentname"].is_null())
    {
        ++argcount;
        agentname = jargs["agentname"].string_value();
    }

    return argcount;
}

int32_t parse_sat(string args)
{
    uint16_t argcount = 0;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
    cosmosstruc cinfo;
    initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
    if (!jargs["phys"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["phys"].object_items();
        initiallat = RADOF(values["lat"].number_value());
        initiallon = RADOF(values["lon"].number_value());
        initialalt = values["alt"].number_value();
        initialangle = RADOF(values["angle"].number_value());
        initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
    }
    if (!jargs["eci"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["eci"].object_items();
        initialloc.pos.eci.s.col[0] = (values["x"].number_value());
        initialloc.pos.eci.s.col[1] = (values["y"].number_value());
        initialloc.pos.eci.s.col[2] = (values["z"].number_value());
        initialloc.pos.eci.v.col[0] = (values["vx"].number_value());
        initialloc.pos.eci.v.col[1] = (values["vy"].number_value());
        initialloc.pos.eci.v.col[2] = (values["vz"].number_value());
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
    }
    if (!jargs["kep"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["kep"].object_items();
        kepstruc kep;
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
    }

}
