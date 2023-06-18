/* 
 * Run this program from the ~/cosmos/realms/propagate folder.
 * The initialutc 60107.01 should set the initial position of the satellites over Hawaii.
 * If running on a local install, also add the argpair "cosmos_web_addr":"127.0.0.1"
 * agent_propagator '{"initialutc":60107.01}'
 * agent_propagator '{"initialutc":60107.01, "cosmos_web_addr":"127.0.0.1"}'
 */

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

//vector<cosmosstruc> sats;
vector<Physics::Simulator::StateList::iterator> sits;


int net_port_in = 10080;
socket_channel net_channel_in;
string net_address_out;
int net_port_out = 10081;
socket_bus net_channel_out;

// For cosmos web
socket_channel cosmos_web_telegraf_channel, cosmos_web_api_channel;
const string TELEGRAF_ADDR = "cosmos_telegraf";
string cosmos_web_addr = "";
const int TELEGRAF_PORT = 10096;
const int API_PORT = 10097;
void add_sim_devices();
void reset_db();
int32_t send_telem_to_cosmos_web(cosmosstruc* cinfo);

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

    // Open sockets for sending data to cosmos web
    // By default, attempts to send to the hostname "cosmos_telegraf",
    // which is available in the COSMOS Docker application. If running
    // on the host (i.e., not in a docker container), specify the
    // cosmos_web_addr variable manually via the terminal args,
    // such as {"cosmos_web_addr": "127.0.0.1"}
    if (cosmos_web_addr.empty()) {
        string response;
        iretn = hostnameToIP(TELEGRAF_ADDR, cosmos_web_addr, response);
        if (iretn < 0)
        {
            cout << "Encountered error in hostnameToIP: " << response << endl;
            exit(0);
        }
    }
    iretn = socket_open(&cosmos_web_telegraf_channel, NetworkType::UDP, cosmos_web_addr.c_str(), TELEGRAF_PORT, SOCKET_TALK, SOCKET_BLOCKING, 2000000 );
    if ((iretn) < 0)
    {
        cout << "Failed to open socket cosmos_web_telegraf_channel: " << cosmos_error_string(iretn) << endl;
        exit(0);
    }
    iretn = socket_open(&cosmos_web_api_channel, NetworkType::UDP, cosmos_web_addr.c_str(), API_PORT, SOCKET_TALK, SOCKET_BLOCKING, 2000000);
    if ((iretn) < 0)
    {
        cout << "Failed to open socket cosmos_web_api_channel: " << cosmos_error_string(iretn) << endl;
        exit(0);
    }

    add_sim_devices();
    // Reset simulation db
    reset_db();

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

            send_telem_to_cosmos_web(&sit->second->currentinfo);
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
    if (!jargs["cosmos_web_addr"].is_null())
    {
        ++argcount;
        cosmos_web_addr = jargs["cosmos_web_addr"].string_value();
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
    if (!sits.size())
    {
        nodename = "mother";
        initialutc = initialloc.utc;
    }
    else
    {
        nodename = "child_" + to_unsigned(sits.size(), 2, true);
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

/**
 * @brief Sends a single node's telems to cosmos web
 * 
 * @param cinfo cosmosstruc of some node to store telems for
 * @return int32_t 0 on success, negative on failure
 */
int32_t send_telem_to_cosmos_web(cosmosstruc* cinfo)
{
    // locstruc
    json11::Json jobj = json11::Json::object({
        {"node_name", cinfo->node.name },
        {"node_loc", json11::Json::object({
            {"pos", json11::Json::object({
                {"eci", json11::Json::object({
                    { "utc", cinfo->node.loc.pos.eci.utc },
                    { "s", cinfo->node.loc.pos.eci.s },
                    { "v", cinfo->node.loc.pos.eci.v }
                })}
            })},
            {"att", json11::Json::object({
                {"icrf", json11::Json::object({
                    { "utc", cinfo->node.loc.att.icrf.utc },
                    { "s", cinfo->node.loc.att.icrf.s },
                    { "v", cinfo->node.loc.att.icrf.v }
                })}
            })}
        })},
    });
    int32_t iretn = socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    if (iretn < 0) { return iretn; }

    // Devices are not auto-populated, so just use some RANDOM VALUES
    // battstruc
    jobj = json11::Json::object({
        {"node_name", cinfo->node.name },
        {"devspec", json11::Json::object({
            {"batt", [cinfo]() -> vector<json11::Json>
                {
                    vector<json11::Json> ret;
                    for (auto batt : cinfo->devspec.batt) {
                        batt.amp = (rand()/(double)RAND_MAX) * (1.-0.5) +0.5;
                        batt.volt = (rand()/(double)RAND_MAX) * (8.-5.) +5.;
                        batt.power = batt.amp * batt.volt;
                        ret.push_back({
                            json11::Json::object({
                                {"didx", batt.didx},
                                {"utc", sim->currentutc},
                                {"amp", batt.amp},
                                {"volt", batt.volt},
                                {"power", batt.power},
                                {"temp", (rand()/(double)RAND_MAX) * (400.-375.) +375.},
                                {"percentage", (rand()/(double)RAND_MAX) * (0.65-0.35) +0.35},
                            })
                        });
                    }
                    return ret;
                }()
            }
        })}
    });
    iretn = socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    if (iretn < 0) { return iretn; }

    // bcregstruc
    jobj = json11::Json::object({
        {"node_name", cinfo->node.name },
        {"devspec", json11::Json::object({
            {"bcreg", [cinfo]() -> vector<json11::Json>
                {
                    vector<json11::Json> ret;
                    for (auto bcr : cinfo->devspec.bcreg) {
                        bcr.amp = (rand()/(double)RAND_MAX) * (1.-0.5) +0.5;
                        bcr.volt = (rand()/(double)RAND_MAX) * (8.-5.) +5.;
                        bcr.power = bcr.amp * bcr.volt;
                        ret.push_back({
                            json11::Json::object({
                                {"didx", bcr.didx},
                                {"utc", sim->currentutc},
                                {"amp", bcr.amp},
                                {"volt", bcr.volt},
                                {"power", bcr.power},
                                {"temp", (rand()/(double)RAND_MAX) * (400.-375.) +375.},
                                {"mpptin_amp", bcr.amp * 0.9},
                                {"mpptin_volt", bcr.volt * 0.9},
                                {"mpptout_amp", bcr.amp * 0.5},
                                {"mpptout_volt", bcr.volt * 0.5},
                            })
                        });
                    }
                    return ret;
                }()
            }
        })}
    });
    iretn = socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    if (iretn < 0) { return iretn; }

    // cpustruc
    jobj = json11::Json::object({
        {"node_name", cinfo->node.name },
        {"devspec", json11::Json::object({
            {"cpu", [cinfo]() -> vector<json11::Json>
                {
                    vector<json11::Json> ret;
                    for (auto cpu : cinfo->devspec.cpu) {
                        ret.push_back({
                            json11::Json::object({
                                {"didx", cpu.didx},
                                {"utc", sim->currentutc},
                                {"temp", (rand()/(double)RAND_MAX) * (400.-375.) +375.},
                                {"uptime", sim->currentutc - 59945.0}, // 01/01/2023
                                {"gib", (rand()/(double)RAND_MAX) * (4.-2.) +2.},
                                {"load", (rand()/(double)RAND_MAX) * (0.95-0.5) +0.5},
                                {"boot_count", 2},
                                {"storage", (rand()/(double)RAND_MAX) * (25.-9.) +9.},
                            })
                        });
                    }
                    return ret;
                }()
            }
        })}
    });
    iretn = socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    if (iretn < 0) { return iretn; }

    // tsenstruc
    jobj = json11::Json::object({
        {"node_name", cinfo->node.name },
        {"devspec", json11::Json::object({
            {"tsen", [cinfo]() -> vector<json11::Json>
                {
                    vector<json11::Json> ret;
                    for (auto tsen : cinfo->devspec.tsen) {
                        ret.push_back({
                            json11::Json::object({
                                {"didx", tsen.didx},
                                {"utc", sim->currentutc},
                                {"temp", (rand()/(double)RAND_MAX) * (400.-375.) +375.},
                            })
                        });
                    }
                    return ret;
                }()
            }
        })}
    });
    iretn = socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    if (iretn < 0) { return iretn; }

    return 0;
}

/**
 * @brief Add devices to simulate to the nodes
 */
void add_sim_devices()
{
    for (auto s : sits)
    {
        // Battery
        for (size_t i=0; i < 2; ++i) {
            json_createpiece(&s->second->currentinfo, "Battery"+std::to_string(i), DeviceType::BATT);
        }
        // BC Regulators (solar panels)
        json_createpiece(&s->second->currentinfo, "Left", DeviceType::BCREG);
        json_createpiece(&s->second->currentinfo, "Right", DeviceType::BCREG);
        // CPU
        json_createpiece(&s->second->currentinfo, "iOBC", DeviceType::CPU);
        json_createpiece(&s->second->currentinfo, "iX5-100", DeviceType::CPU);
        // Thermal Sensors
        json_createpiece(&s->second->currentinfo, "Camera", DeviceType::TSEN);
        json_createpiece(&s->second->currentinfo, "Heat sink", DeviceType::TSEN);
        json_createpiece(&s->second->currentinfo, "CPU", DeviceType::TSEN);

        // Fix pointers
        json_updatecosmosstruc(&s->second->currentinfo);
        // Update all physical quantities
        node_calc(&s->second->currentinfo);
    }
}

/**
 * @brief Reset the simulation database
 */
void reset_db()
{
    socket_sendto(cosmos_web_api_channel, "{\"swchstruc\": true, \"battstruc\": true, \"bcregstruc\": true, \"cpustruc\": true, \"device\": true, \"device_type\": true, \"locstruc\": true, \"magstruc\": true, \"node\": true, \"tsenstruc\": true, \"rwstruc\": true, \"mtrstruc\": true, \"attstruc_icrf\": true, \"cosmos_event\": true, \"event_type\": true, \"gyrostruc\": true, \"locstruc_eci\": true }");
    // Iterate over sats
    for (size_t id=0; id<sits.size(); ++id)
    {
        // Repopulate node table
        json11::Json jobj = json11::Json::object({
            {"node", json11::Json::object({
                { "node_id", static_cast<uint16_t>(id) },
                { "node_name", sits[id]->second->currentinfo.node.name },
                { "node_type", NODE_TYPE_SATELLITE },
                { "agent_name", sits[id]->second->currentinfo.agent0.name },
                { "utc", sim->currentutc },
                { "utcstart", currentutc }
            })}
        });
        socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
        // Add device info
        jobj = json11::Json::object({
            {"node_name", sits[id]->second->currentinfo.node.name },
            {"device", [id]() -> vector<json11::Json>
                {
                    vector<json11::Json> ret;
                    for (auto device : sits[id]->second->currentinfo.device) {
                        ret.push_back({
                            json11::Json::object({
                                { "type", device->type },
                                { "cidx", device->cidx },
                                { "didx", device->didx },
                                { "name", device->name }
                            })
                        });
                    }
                    return ret;
                }()
            }
        });
        cout << jobj.dump() << endl;
        socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    }
    cout << "Resetting db..." << endl;
    secondsleep(1.);
}
