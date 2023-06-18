#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"
#include "support/datalib.h"
#include "support/beacon.h"
#include "support/packetcomm.h"
#include "support/packethandler.h"

using namespace Convert;

Vector calc_control_torque_b(Convert::qatt tatt, Convert::qatt catt, Vector moi, double portion);

Physics::Simulator::StateList::iterator sit;
Physics::Simulator *sim;
Agent *agent;
string realmname = "propagate";
double initialutc = 0.;
double speed=1.;
double maxaccel = .1;
double minaccel;
double offset = 0.;
double deltat = .0655;
double runcount = 1500;
double currentutc;
double simdt = 1.;
double attdt = 1.;
double minaccelratio = 10;
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
LsFit tattfit;
LsFit omegafit;
//static bool altprint = false;
int32_t parse_control(string args);
int32_t parse_sat(string args);

vector<cosmosstruc> sats;
vector<Physics::Simulator::StateList::iterator> sits;


int net_port_in = 10080;
socket_channel net_channel_in;
string net_address_out;
int net_port_out = 10081;
socket_bus net_channel_out;

int main(int argc, char *argv[])
{
    int32_t iretn = 0;
    FILE *fp;

    // initialize simulation agent
    if (argc > 1)
    {
        parse_control(argv[1]);
    }

    agent = new Agent(realmname, "master", "propagate", 0.);
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


    // Load in targets
    fp = fopen(targetfile.c_str(), "r");
    if (fp != nullptr)
    {
        string line;
        line.resize(200);
        while (fgets((char *)line.data(), 200, fp) != nullptr)
        {
            vector<string> args = string_split(line, " \t\n");
            for (Physics::Simulator::StateList::iterator sit : sits)
            {
                if (args.size() == 4)
                {
                    sit->second->AddTarget(args[0], RADOF(stof(args[1])), RADOF(stod(args[2])), 0., stod(args[3]), NODE_TYPE_GROUNDSTATION);
                }
                else if (args.size() == 5)
                {
                    sit->second->AddTarget(args[0], RADOF(stof(args[1])), RADOF(stod(args[2])), RADOF(stof(args[3])), RADOF(stod(args[4])));
                }
            }
        }
    }

    // Open socket for sending data to and from agent_propagator
    iretn = socket_open(&net_channel_in, NetworkType::UDP, "", net_port_in, SOCKET_LISTEN, SOCKET_NONBLOCKING);
//    iretn = socket_open(net_channel_out, net_port_out);
    iretn = socket_publish(net_channel_out, net_port_out);

    PacketComm packet;
    double elapsed = 0;
    double pcount = 0.;
    minaccel = maxaccel / minaccelratio;
    offset = initialutc - currentmjd();
    while (agent->running())
    {
        ElapsedTime ret;
        for (Physics::Simulator::StateList::iterator sit : sits)
        {
            string output;
            output += sit->second->currentinfo.get_json("node.utc");
            output += sit->second->currentinfo.get_json("node.name");
            output += sit->second->currentinfo.get_json("node.loc.pos.eci.s");
            output += sit->second->currentinfo.get_json("node.loc.pos.eci.v");
//            iretn = socket_sendto(net_channel_out, output);
            iretn = socket_post(net_channel_out, output);
        }
//        iretn = PacketHandler::CreateBeacon(packet, static_cast<uint8_t>(Beacon::TypeId::NodeLocBeacon), agent);
//        iretn = socket_sendto(net_channel_out, packet.wrapped);
//        iretn = PacketHandler::CreateBeacon(packet, static_cast<uint8_t>(Beacon::TypeId::NodePhysBeacon), agent);
//        iretn = socket_sendto(net_channel_out, packet.wrapped);
//        iretn = PacketHandler::CreateBeacon(packet, static_cast<uint8_t>(Beacon::TypeId::NodeTargetBeacon), agent);
//        iretn = socket_sendto(net_channel_out, packet.wrapped);

        // Attitude adjustment
        // Desired attitude comes from aligning satellite Z with desired Z and satellite Y with desired Y
        qatt tatt;
        qatt satt;
        locstruc stloc;

        // update states information for all nodes
        sim->Propagate();
        pcount += simdt;

        ++elapsed;
        secondsleep(simdt/speed - ret.lap());
    }

    agent->shutdown();
}

int32_t parse_control(string args)
{
    uint16_t argcount = 0;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
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
    if (!jargs["initialutc"].is_null())
    {
        ++argcount;
        initialutc = jargs["initialutc"].number_value();
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
    if (!jargs["realmname"].is_null())
    {
        ++argcount;
        realmname = jargs["realmname"].string_value();
    }

    return argcount;
}

int32_t parse_sat(string args)
{
    int32_t iretn;
    string nodename;
    double initiallat = RADOF(21.3069);
    double initiallon = RADOF(-157.8583);
    double initialalt = 400000.;
    double initialangle = RADOF(54.);
    Convert::locstruc initialloc;
    uint16_t argcount = 0;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
//    initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
    if (!jargs["maxaccel"].is_null())
    {
        ++argcount;
        maxaccel = jargs["maxaccel"].number_value();
    }
    if (!jargs["lvlh"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["lvlh"].object_items();
        Physics::Simulator::StateList::iterator sit = sim->GetNode("mother");
        initialloc = sit->second->currentinfo.node.loc;
        cartpos dpos;
        dpos.s.col[0] = values["x"].number_value();
        dpos.s.col[1] = values["y"].number_value();
        dpos.s.col[2] = values["z"].number_value();
        dpos.s = irotate(initialloc.pos.extra.l2g, dpos.s);
        dpos.s = rv_mmult(initialloc.pos.extra.e2j, dpos.s);
        initialloc.pos.eci.s = initialloc.pos.eci.s + dpos.s;
        dpos.v.col[0] = values["vx"].number_value();
        dpos.v.col[1] = values["vy"].number_value();
        dpos.v.col[2] = values["vz"].number_value();
        dpos.v = irotate(initialloc.pos.extra.l2g, dpos.v);
        dpos.v = rv_mmult(initialloc.pos.extra.e2j, dpos.v);
        initialloc.pos.eci.v = initialloc.pos.eci.v + dpos.v;
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
    }
    if (!jargs["phys"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["phys"].object_items();
        initiallat = RADOF(values["lat"].number_value());
        initiallon = RADOF(values["lon"].number_value());
        initialalt = values["alt"].number_value();
        initialangle = RADOF(values["angle"].number_value());
        if (initialutc == 0.)
        {
            initialutc = currentmjd();
        }
        initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
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
    }
    if (!jargs["tle"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["tle"].object_items();
        vector<Convert::tlestruc>lines;
        string fname = values["filename"].string_value();
        load_lines(fname, lines);
        if (initialutc == 0.)
        {
            initialutc = lines[0].utc;
        }
        lines2eci(initialutc, lines, initialloc.pos.eci);
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
    }
    initialloc.att.icrf.s = q_eye();
    if (!sats.size())
    {
        nodename = "mother";
        initialutc = initialloc.utc;
    }
    else
    {
        nodename = "child_" + to_unsigned(sats.size(), 2);
    }
    iretn = sim->AddNode(nodename, Physics::Structure::HEX65W80H, Physics::Propagator::PositionTle, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, initialloc.pos.eci, initialloc.att.icrf);
    sits.push_back(sim->GetNode(nodename));
    return iretn;
}

int32_t parse_target(string args)
{
//    int32_t iretn;
    string name;
    NODE_TYPE type = NODE_TYPE_TARGET;
    double clat = RADOF(21.3069);
    double clon = RADOF(-157.8583);
    double dlat = 0.;
    double dlon = 0.;
    double alt = 0.;
    uint16_t argcount = 0;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
    if (!jargs["name"].is_null())
    {
        ++argcount;
        name = jargs["name"].string_value();
    }
    if (!jargs["type"].is_null())
    {
        ++argcount;
        type = static_cast<NODE_TYPE>(jargs["type"].number_value());
    }
    if (!jargs["alt"].is_null())
    {
        ++argcount;
        alt = jargs["alt"].number_value();
    }
    if (!jargs["clat"].is_null())
    {
        ++argcount;
        clat = jargs["clat"].number_value();
    }
    if (!jargs["clon"].is_null())
    {
        ++argcount;
        clon = jargs["clon"].number_value();
    }
    if (!jargs["dlat"].is_null())
    {
        ++argcount;
        dlat = jargs["dlat"].number_value();
    }
    if (!jargs["dlon"].is_null())
    {
        ++argcount;
        dlon = jargs["dlon"].number_value();
    }
    for (Physics::Simulator::StateList::iterator sit : sits)
    {
        sit->second->AddTarget(name, clat+dlat, clon-dlon, clat-dlat, clon+dlon, alt, type);
    }
    return sits.size();
}
