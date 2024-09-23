#include "support/configCosmos.h"
#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"
#include "device/cpu/devicecpu.h"
#include "device/disk/devicedisk.h"


int32_t parse_control(string args);
int32_t request_get_sat_json(string &request, string &response, Agent *agent);
int32_t request_get_node_json(string &request, string &response, Agent *agent);
int32_t request_get_pieces_json(string &request, string &response, Agent *agent);
int32_t request_get_devspec_json(string &request, string &response, Agent *agent);
int32_t request_get_devgen_json(string &request, string &response, Agent *agent);
int32_t request_get_location_node(string &request, string &response, Agent *agent);
int32_t request_get_offsetutc(string &request, string &response, Agent *agent);
int32_t request_set_thrust(string &request, string &response, Agent *agent);
int32_t request_set_torque(string &request, string &response, Agent *agent);
Physics::Simulator *sim;
Agent *agent;
//uint16_t thrustctl = 0;
double simdt = 1.;
uint16_t realtime=1;
uint16_t printevent=0;
uint16_t postevent=0;
double initialutc = 60107.01;
double endutc = 0.;
double deltautc = 0.;
string realmname = "propagate";
string orbitfile = "orbit.dat";
string satfile = "sats.dat";
string targetfile = "targets.dat";
string tlefile = "tle.dat";
double runcount = 1500;
vector <cartpos> lvlhoffset;
socket_bus data_channel_out;
DeviceCpu deviceCpu;
DeviceDisk deviceDisk;

int main(int argc, char *argv[])
{
    int32_t iretn;
    double elapsed = 0;

    if (argc > 1)
    {
        parse_control(argv[1]);
    }

    agent = new Agent(realmname, "", "propagate", 0.);
    agent->set_debug_level(0);
    agent->add_request("get_sat_json", request_get_sat_json, "nodename", "Get JSON description of satellite for Node nodename");
    agent->add_request("get_node_json", request_get_node_json, "nodename", "Get JSON description of node for Node nodename");
    agent->add_request("get_pieces_json", request_get_pieces_json, "nodename", "Get JSON description of pieces for Node nodename");
    agent->add_request("get_devgen_json", request_get_devgen_json, "nodename", "Get JSON description of general devices for Node nodename");
    agent->add_request("get_devspec_json", request_get_devspec_json, "nodename", "Get JSON description of specific for Node nodename");
    agent->add_request("get_location_node", request_get_location_node, "nodename", "Get JSON of position and attitude for Node nodename");
    agent->add_request("get_offsetutc", request_get_offsetutc, "", "Get simulator offset of UTC in julian days");
    agent->add_request("set_thrust", request_set_thrust, "nodename {thrust}", "Set JSON Vector of thrust for Node nodename");
    agent->add_request("set_torque", request_set_torque, "nodename {torque}", "Set JSON Vector of torque for Node nodename");

    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn <0) {
        agent->debug_log.Printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }
    sim->Init(simdt, realmname);
    sim->ParseOrbitFile();
    sim->ParseSatFile();
    lvlhoffset.resize(sim->cnodes.size());
    sim->ParseTargetFile();
    if (realtime)
    {
        agent->cinfo->agent0.aprd = simdt;
        agent->start_active_loop();
    }

    // Open socket for returning state information to simulator
    iretn = socket_open(data_channel_out, CLIENT_PORT_OUT, 2000000);

    uint16_t tcount = 0;
    while (agent->running() && elapsed < runcount)
    {
        // Calculate thrust
//        if (tcount++ > 5)
//        {
//            for (uint16_t i=1; i<sim->cnodes.size(); ++i)
//            {
//                locstruc goal;
//                goal.pos.eci = sim->cnodes[0]->currentinfo.node.loc.pos.eci;
//                goal.pos.eci.pass++;
//                pos_eci(goal);
//                goal.pos.lvlh.s = rvector(0., 0., 0.);
//                pos_origin2lvlh(goal);
//                sim->UpdatePush(sim->cnodes[i]->currentinfo.node.name, Physics::ControlThrust(sim->cnodes[i]->currentinfo.node.loc.pos.eci, goal.pos.eci, sim->cnodes[i]->currentinfo.mass, sim->cnodes[i]->currentinfo.devspec.thst[0].maxthrust/sim->cnodes[i]->currentinfo.mass, simdt));
//            }
//        }
//        rvector ds = sim->cnodes[1]->currentinfo.node.loc.pos.eci.s - sim->cnodes[0]->currentinfo.node.loc.pos.eci.s;
//        rvector dv = sim->cnodes[1]->currentinfo.node.loc.pos.eci.v - sim->cnodes[0]->currentinfo.node.loc.pos.eci.v;
//        rvector da = sim->cnodes[1]->currentinfo.node.loc.pos.eci.a - sim->cnodes[0]->currentinfo.node.loc.pos.eci.a;
//        Vector fp = (1./sim->cnodes[1]->currentinfo.node.phys.mass) * sim->cnodes[1]->currentinfo.node.phys.fpush;
//        printf("ds: %f %f %f dv: %f %f %f da: % %f %f fpush: %f %f %f\n", ds.col[0], ds.col[1], ds.col[2], dv.col[0], dv.col[1], dv.col[2], da.col[0], da.col[1], da.col[2], fp.x, fp.y, fp.z);
//        rvector ms = sim->cnodes[0]->currentinfo.node.loc.pos.eci.s;
//        rvector mv = sim->cnodes[0]->currentinfo.node.loc.pos.eci.v;
//        rvector ma = sim->cnodes[0]->currentinfo.node.loc.pos.eci.a;
//        rvector cs = sim->cnodes[1]->currentinfo.node.loc.pos.eci.s;
//        rvector cv = sim->cnodes[1]->currentinfo.node.loc.pos.eci.v;
//        rvector ca = sim->cnodes[1]->currentinfo.node.loc.pos.eci.a;
//        printf("%u\t", tcount++);
//        printf("ms:\t%f\t%f\t%f\t", ms.col[0], ms.col[1], ms.col[2]);
//        printf("mv:\t%f\t%f\t%f\t", mv.col[0], mv.col[1], mv.col[2]);
//        printf("ma:\t%f\t%f\t%f\t", ma.col[0], ma.col[1], ma.col[2]);
//        printf("cs:\t%f\t%f\t%f\t", cs.col[0], cs.col[1], cs.col[2]);
//        printf("cv:\t%f\t%f\t%f\t", cv.col[0], cv.col[1], cv.col[2]);
//        printf("ca:\t%f\t%f\t%f\t", ca.col[0], ca.col[1], ca.col[2]);
//        printf("fp:\t%f\t%f\t%f\n", fp.x, fp.y, fp.z);
//        fflush(stdout);
        sim->Propagate();
        for (auto &state : sim->cnodes)
        {
            if (state->currentinfo.event.size())
            {
                for (eventstruc event : state->currentinfo.event)
                {
                    if (printevent)
                    {
                        string output = "type: event";
                        output += to_label(" node", state->currentinfo.node.name);
                        output += to_label(" utc", to_mjd(state->currentinfo.node.utc));
                        output += to_label(" name", event.name);
                        output += to_label(" type", to_unsigned(static_cast<int>(event.type)));
                        output += to_label(" flag", to_unsigned(static_cast<int>(event.flag)));
                        output += to_label(" el", to_floating(event.el, 4));
                        output += to_label(" az", to_floating(event.az, 4));
                        output += to_label(" lon", to_floating(state->currentinfo.node.loc.pos.geod.s.lon, 5));
                        output += to_label(" lat", to_floating(state->currentinfo.node.loc.pos.geod.s.lat, 5));
                        output += to_label(" alt", to_floating(state->currentinfo.node.loc.pos.geod.s.h, 1));
                        printf("%s\n", output.c_str());
                    }
                    if (postevent)
                    {
                        json11::Json jobj = json11::Json::object({
                            {"mtype", "event"},
                            {"node", state->currentinfo.node.name},
                            {"utc", event.utc},
                            {"name", event.name},
                            {"type", static_cast<int>(event.type)},
                            {"flag", static_cast<int>(event.flag)},
                            {"el", event.el},
                            {"az", event.az},
                            {"geodpos", state->currentinfo.node.loc.pos.geod.s}
                        });
                        string output = jobj.dump();
                        iretn = socket_post(data_channel_out, output.c_str());
                    }
                }
            }
            if (printevent)
            {
                string output = "type: node";
                output += to_label(" node", state->currentinfo.node.name);
                output += to_label(" utc", to_mjd(state->currentinfo.node.utc));
                output += to_label(" ecix", to_floating(state->currentinfo.node.loc.pos.eci.s.col[0], 1));
                output += to_label(" eciy", to_floating(state->currentinfo.node.loc.pos.eci.s.col[1], 1));
                output += to_label(" eciz", to_floating(state->currentinfo.node.loc.pos.eci.s.col[2], 1));
                output += to_label(" ecivx", to_floating(state->currentinfo.node.loc.pos.eci.v.col[0], 2));
                output += to_label(" ecivy", to_floating(state->currentinfo.node.loc.pos.eci.v.col[1], 2));
                output += to_label(" ecivz", to_floating(state->currentinfo.node.loc.pos.eci.v.col[2], 2));
                output += to_label(" eciax", to_floating(state->currentinfo.node.loc.pos.eci.a.col[0], 3));
                output += to_label(" eciay", to_floating(state->currentinfo.node.loc.pos.eci.a.col[1], 3));
                output += to_label(" eciaz", to_floating(state->currentinfo.node.loc.pos.eci.a.col[2], 3));
                output += to_label(" thetax", to_floating(state->currentinfo.node.loc.att.icrf.s.d.x, 4));
                output += to_label(" thetay", to_floating(state->currentinfo.node.loc.att.icrf.s.d.y, 4));
                output += to_label(" thetaz", to_floating(state->currentinfo.node.loc.att.icrf.s.d.z, 4));
                output += to_label(" thetaw", to_floating(state->currentinfo.node.loc.att.icrf.s.w, 4));
                output += to_label(" omegax", to_floating(state->currentinfo.node.loc.att.icrf.v.col[0], 1));
                output += to_label(" omegay", to_floating(state->currentinfo.node.loc.att.icrf.v.col[1], 1));
                output += to_label(" omegaz", to_floating(state->currentinfo.node.loc.att.icrf.v.col[2], 1));
                output += to_label(" alphax", to_floating(state->currentinfo.node.loc.att.icrf.a.col[0], 1));
                output += to_label(" alphay", to_floating(state->currentinfo.node.loc.att.icrf.a.col[1], 1));
                output += to_label(" alphaz", to_floating(state->currentinfo.node.loc.att.icrf.a.col[2], 1));
                output += to_label(" powerin", to_floating(state->currentinfo.node.phys.powgen, 2));
                output += to_label(" powerout", to_floating(state->currentinfo.node.phys.powuse, 2));
                state->currentinfo.devspec.cpu[0].load = static_cast <float>(deviceCpu.getLoad());
                state->currentinfo.devspec.cpu[0].gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/1073741824.);
                state->currentinfo.devspec.cpu[0].maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/1073741824.);
                state->currentinfo.devspec.cpu[0].maxload = deviceCpu.getCpuCount();
                output += to_label(" load", to_floating(state->currentinfo.devspec.cpu[0].load / state->currentinfo.devspec.cpu[0].maxload, 3));
                output += to_label(" memory", to_floating(state->currentinfo.devspec.cpu[0].gib / state->currentinfo.devspec.cpu[0].maxgib, 3));
                output += to_label(" storage", to_floating(state->currentinfo.devspec.cpu[0].storage, 4));
                rvector ds = state->currentinfo.node.loc.pos.eci.s - sim->cnodes[0]->currentinfo.node.loc.pos.eci.s;
                rvector dv = state->currentinfo.node.loc.pos.eci.v - sim->cnodes[0]->currentinfo.node.loc.pos.eci.v;
                output += to_label(" deltax", to_floating(ds.col[0], 2));
                output += to_label(" deltay", to_floating(ds.col[1], 2));
                output += to_label(" deltaz", to_floating(ds.col[2], 2));
                output += to_label(" deltavx", to_floating(dv.col[0], 3));
                output += to_label(" deltavy", to_floating(dv.col[1], 3));
                output += to_label(" deltavz", to_floating(dv.col[2], 3));
                printf("%s\n", output.c_str());
            }
            if (postevent)
            {
                state->currentinfo.devspec.cpu[0].load = static_cast <float>(deviceCpu.getLoad());
                state->currentinfo.devspec.cpu[0].gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/1073741824.);
                state->currentinfo.devspec.cpu[0].maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/1073741824.);
                state->currentinfo.devspec.cpu[0].maxload = deviceCpu.getCpuCount();
                json11::Json jobj = json11::Json::object({
                    {"type", "node"},
                    {"node", state->currentinfo.node.name},
                    {"ecipos", state->currentinfo.node.loc.pos.eci},
                    {"alphatt", state->currentinfo.node.loc.att.icrf},
                    {"powerin", state->currentinfo.node.phys.powgen},
                    {"powerout", state->currentinfo.node.phys.powuse},
                    {"load", state->currentinfo.devspec.cpu[0].load / state->currentinfo.devspec.cpu[0].maxload},
                    {"memory", state->currentinfo.devspec.cpu[0].gib / state->currentinfo.devspec.cpu[0].maxgib}
                });
                string output = jobj.dump();
                iretn = socket_post(data_channel_out, output.c_str());
            }
        }
        if (realtime)
        {
            agent->finish_active_loop();
            //            if (++tcount > 10)
            //            {
            //                printf("%8.3f %8.3f %8.3f %8.3f %8.3f %8.3f | %8.1f %8.1f %8.1f %8.1f %8.1f %8.1f | %8.1f %8.1f %8.1f %8.2f %8.2f %8.2f | %8.4f %8.2f %8.1f\n",
            //                       sim->cnodes[0]->currentinfo.node.loc.pos.eci.a.col[0],
            //                       sim->cnodes[0]->currentinfo.node.loc.pos.eci.a.col[1],
            //                       sim->cnodes[0]->currentinfo.node.loc.pos.eci.a.col[2],
            //                       sim->cnodes[1]->currentinfo.node.loc.pos.eci.a.col[0],
            //                       sim->cnodes[1]->currentinfo.node.loc.pos.eci.a.col[1],
            //                       sim->cnodes[1]->currentinfo.node.loc.pos.eci.a.col[2],
            //                       sim->cnodes[0]->currentinfo.node.loc.pos.eci.v.col[0],
            //                       sim->cnodes[0]->currentinfo.node.loc.pos.eci.v.col[1],
            //                       sim->cnodes[0]->currentinfo.node.loc.pos.eci.v.col[2],
            //                       sim->cnodes[1]->currentinfo.node.loc.pos.eci.v.col[0],
            //                       sim->cnodes[1]->currentinfo.node.loc.pos.eci.v.col[1],
            //                       sim->cnodes[1]->currentinfo.node.loc.pos.eci.v.col[2],
            //                       sim->cnodes[0]->currentinfo.node.loc.pos.eci.s.col[0],
            //                       sim->cnodes[0]->currentinfo.node.loc.pos.eci.s.col[1],
            //                       sim->cnodes[0]->currentinfo.node.loc.pos.eci.s.col[2],
            //                       sim->cnodes[1]->currentinfo.node.loc.pos.eci.s.col[0],
            //                       sim->cnodes[1]->currentinfo.node.loc.pos.eci.s.col[1],
            //                       sim->cnodes[1]->currentinfo.node.loc.pos.eci.s.col[2],
            //                       length_rv(sim->cnodes[1]->currentinfo.node.loc.pos.eci.a-sim->cnodes[0]->currentinfo.node.loc.pos.eci.a),
            //                       length_rv(sim->cnodes[1]->currentinfo.node.loc.pos.eci.v-sim->cnodes[0]->currentinfo.node.loc.pos.eci.v),
            //                       length_rv(sim->cnodes[1]->currentinfo.node.loc.pos.eci.s-sim->cnodes[0]->currentinfo.node.loc.pos.eci.s)
            //                       );
            //                fflush(stdout);
            //                tcount = 0;
            //            }
        }
        else
        {
            ++elapsed;
        }
    }
}

int32_t parse_control(string args)
{
    uint16_t argcount = 0;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
    //    if (!jargs["thrustctl"].is_null())
    //    {
    //        ++argcount;
    //        thrustctl = jargs["thrustctl"].number_value();
    //    }
    if (!jargs["printevent"].is_null())
    {
        ++argcount;
        printevent = jargs["printevent"].number_value();
    }
    if (!jargs["postevent"].is_null())
    {
        ++argcount;
        postevent = jargs["postevent"].number_value();
    }
    if (!jargs["runcount"].is_null())
    {
        ++argcount;
        runcount = jargs["runcount"].number_value();
    }
    if (!jargs["realtime"].is_null())
    {
        ++argcount;
        realtime = jargs["realtime"].number_value();
    }
    if (!jargs["initialutc"].is_null())
    {
        ++argcount;
        initialutc = jargs["initialutc"].number_value();
    }
    else
    {
        initialutc = currentmjd();
    }
    if (!jargs["endutc"].is_null())
    {
        ++argcount;
        endutc = jargs["endutc"].number_value();
    }
    if (!jargs["deltautc"].is_null())
    {
        ++argcount;
        deltautc = jargs["deltautc"].number_value();
    }
    if (!jargs["simdt"].is_null())
    {
        ++argcount;
        simdt = jargs["simdt"].number_value();
    }
    //    if (!jargs["minaccelratio"].is_null())
    //    {
    //        ++argcount;
    //        minaccelratio = jargs["minaccelratio"].number_value();
    //    }
    if (!jargs["satfile"].is_null())
    {
        ++argcount;
        satfile = jargs["satfile"].string_value();
    }
    if (!jargs["orbitfile"].is_null())
    {
        ++argcount;
        orbitfile = jargs["orbitfile"].string_value();
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

int32_t request_get_sat_json(string &request, string &response, Agent *agent)
{
    vector<string> args = string_split(request);
    response.clear();

    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            string temp;
            response = json_pieces(temp, &(*sit)->currentinfo);
            json_join(response, json_devices_general(temp, &(*sit)->currentinfo));
            json_join(response, json_devices_specific(temp, &(*sit)->currentinfo));
        }
    }
    return response.length();
}

int32_t request_get_node_json(string &request, string &response, Agent *agent)
{
    vector<string> args = string_split(request);
    response.clear();

    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            json_node(response, &(*sit)->currentinfo);
        }
    }
    return response.length();
}

int32_t request_get_pieces_json(string &request, string &response, Agent *agent)
{
    vector<string> args = string_split(request);
    response.clear();

    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            json_pieces(response, &(*sit)->currentinfo);
        }
    }
    return response.length();
}

int32_t request_get_devgen_json(string &request, string &response, Agent *agent)
{
    vector<string> args = string_split(request);
    response.clear();

    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            json_devices_general(response, &(*sit)->currentinfo);
        }
    }
    return response.length();
}

int32_t request_get_devspec_json(string &request, string &response, Agent *agent)
{
    vector<string> args = string_split(request);
    response.clear();

    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            json_devices_specific(response, &(*sit)->currentinfo);
        }
    }
    return response.length();
}

int32_t request_get_location_node(string &request, string &response, Agent *agent)
{
    vector<string> args = string_split(request);
    response.clear();

    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            json11::Json jobj = json11::Json::object({
                {"node", (*sit)->currentinfo.node.name},
                {"utcoffset", (*sit)->currentinfo.node.utcoffset},
                {"pos", (*sit)->currentinfo.node.loc.pos.eci},
                {"att", (*sit)->currentinfo.node.loc.att.icrf}
            });
            response = jobj.dump();
        }
    }
    return response.length();
}

int32_t request_get_offsetutc(string &request, string &response, Agent *agent)
{
    response = to_floating(sim->offsetutc, 8);
    return response.length();
}

int32_t request_set_thrust(string &request, string &response, Agent *agent)
{
    vector<string> args = string_split(request);
    response.clear();

    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            string estring;
            string jstring = request.substr(request.find("{"));
            json11::Json jargs = json11::Json::parse(jstring, estring);
            if (estring.empty())
            {
                (*sit)->currentinfo.node.phys.fpush.from_json(jstring);
                response = (*sit)->currentinfo.node.phys.fpush.to_json().dump();
            }
        }
    }
    return response.length();
}

int32_t request_set_torque(string &request, string &response, Agent *agent)
{
    vector<string> args = string_split(request);
    response.clear();

    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            string estring;
            json11::Json jargs = json11::Json::parse(args[2], estring);
            if (estring.empty())
            {
                (*sit)->currentinfo.node.phys.ftorque.from_json(args[2]);
                response = (*sit)->currentinfo.node.phys.ftorque.to_json().dump();
            }
        }
    }
    return response.length();
}
