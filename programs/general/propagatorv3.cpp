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
int32_t request_dump_node(string &request, string &response, Agent *agent);
Physics::Simulator *sim;
Agent *agent;
//uint16_t thrustctl = 0;
double simdt = 1.;
uint16_t realtime=1;
uint16_t summarize=0;
uint16_t settle=0;
uint16_t printevent=0;
uint16_t postevent=0;
double initialutc = 60107.01;
double endutc = 0.;
double deltautc = 0.;
string formation = "string";
double spacing = 1000.;
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

// For cosmos web
socket_channel cosmos_web_telegraf_channel, cosmos_web_api_channel;
const string TELEGRAF_ADDR = "";
string cosmos_web_addr = "";
const int TELEGRAF_PORT = 10096;
const int API_PORT = 10097;
bool sockets_initialized = false;
int32_t open_cosmos_web_sockets(string cosmos_web_addr);
void reset_db(Physics::Simulator *sim);
int32_t send_telem_to_cosmos_web(cosmosstruc* cinfo);

int main(int argc, char *argv[])
{
    int32_t iretn;
    double elapsed = 0;

    if (argc > 1)
    {
        parse_control(argv[1]);
    }

    agent = new Agent(realmname, "", "propagate", 0., AGENTMAXBUFFER, true);
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
    agent->add_request("dump_node", request_dump_node, "nodename", "Dump description for Node nodename");

    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn <0) {
        agent->debug_log.Printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }
    sim->Init(simdt, realmname);
    sim->ParseOrbitFile();
    sim->ParseSatFile(satfile);
    if (satfile.find("/") != string::npos)
    {
        satfile = satfile.substr(satfile.find_last_of('/'));
    }
    lvlhoffset.resize(sim->cnodes.size());
    sim->ParseTargetFile();
    if (realtime)
    {
        agent->cinfo->agent0.aprd = simdt;
        agent->start_active_loop();
    }

    // Open socket for returning sim->cnodes[i] information to simulator
    iretn = socket_open(data_channel_out, CLIENT_PORT_OUT, 2000000);

    // Cosmos web output initializations
    open_cosmos_web_sockets(cosmos_web_addr);
    // Reset simulation db
    reset_db(sim);

    vector<Vector> sumdeltav;
    sumdeltav.resize(sim->cnodes.size());
    vector<cartpos> delta;
    delta.resize(sim->cnodes.size());
    vector<bool> settled;
    settled.resize(sim->cnodes.size());
    for (uint16_t i=0; i<sim->cnodes.size(); ++i)
    {
        delta[i].s.col[0] = 1.;
        settled[i] = false;
    }
    vector<vector<Physics::coverage>> summaries;
    summaries.resize(sim->cnodes.size() + 1);
    for (uint16_t i=0; i<sim->cnodes.size()+1; ++i)
    {
        summaries[i].resize(sim->targets.size() + 1);
    }
    sim->Formation(formation, spacing);
    sim->Thrust();
    sim->Target();
    sim->Metric();
    while (agent->running() && elapsed < runcount)
    {
        if (settle)
        {
            bool finished = true;
            for (uint16_t i=0; i<sim->cnodes.size(); ++i)
            {
                if (!settled[i])
                {
                    finished = false;
                    break;
                }
            }
            if (finished)
            {
                break;
            }
        }
        sim->Formation(formation, spacing);
        for (uint16_t i=0; i<sim->cnodes.size(); ++i)
        {
            if (settle && settled[i])
            {
                continue;
            }
            sumdeltav[i].x = sumdeltav[i].x + sim->dt * fabs(sim->cnodes[i]->currentinfo.node.phys.fpush.x) / sim->cnodes[i]->currentinfo.node.phys.mass;
            sumdeltav[i].y = sumdeltav[i].y + sim->dt * fabs(sim->cnodes[i]->currentinfo.node.phys.fpush.y) / sim->cnodes[i]->currentinfo.node.phys.mass;
            sumdeltav[i].z = sumdeltav[i].z + sim->dt * fabs(sim->cnodes[i]->currentinfo.node.phys.fpush.z) / sim->cnodes[i]->currentinfo.node.phys.mass;
            delta[i].s = sim->cnodes[i]->currentinfo.node.loc.pos.eci.s - sim->cnodes[i]->currentinfo.node.loc_req.pos.eci.s;
            delta[i].v = sim->cnodes[i]->currentinfo.node.loc.pos.eci.v - sim->cnodes[i]->currentinfo.node.loc_req.pos.eci.v;
            delta[i].a = sim->cnodes[i]->currentinfo.node.loc.pos.eci.a - sim->cnodes[i]->currentinfo.node.loc_req.pos.eci.a;
            if (settle && !settled[i] && length_rv(delta[i].s) < 5. && length_rv(delta[i].v) < .5)
            {
                settled[i] = true;
                string output = "type: settled";
                output += to_label("\tnode", sim->cnodes[i]->currentinfo.node.name);
                output += to_label("\tdt", to_floating(86400.*(sim->cnodes[i]->currentinfo.node.utc-sim->initialutc), 1));
                output += to_label("\tutc", to_mjd(sim->cnodes[i]->currentinfo.node.utc));
                output += to_label("\tlvlhx", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.lvlh.s.col[0], 1));
                output += to_label("\tlvlhy", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.lvlh.s.col[1], 1));
                output += to_label("\tlvlhz", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.lvlh.s.col[2], 1));
                output += to_label("\tlvlhvx", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.lvlh.v.col[0], 2));
                output += to_label("\tlvlhvy", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.lvlh.v.col[1], 2));
                output += to_label("\tlvlhvz", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.lvlh.v.col[2], 2));
                output += to_label("\tlvlhax", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.lvlh.a.col[0], 3));
                output += to_label("\tlvlhay", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.lvlh.a.col[1], 3));
                output += to_label("\tlvlhaz", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.lvlh.a.col[2], 3));
                output += to_label("\tpowerin", to_floating(sim->cnodes[i]->currentinfo.node.phys.powgen, 2));
                output += to_label("\tpowerout", to_floating(sim->cnodes[i]->currentinfo.node.phys.powuse, 2));
                sim->cnodes[i]->currentinfo.devspec.cpu[0].load = static_cast <float>(deviceCpu.getLoad());
                sim->cnodes[i]->currentinfo.devspec.cpu[0].gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/1073741824.);
                sim->cnodes[i]->currentinfo.devspec.cpu[0].maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/1073741824.);
                sim->cnodes[i]->currentinfo.devspec.cpu[0].maxload = deviceCpu.getCpuCount();
                output += to_label("\tload", to_floating(sim->cnodes[i]->currentinfo.devspec.cpu[0].load / sim->cnodes[i]->currentinfo.devspec.cpu[0].maxload, 3));
                output += to_label("\tmemory", to_floating(sim->cnodes[i]->currentinfo.devspec.cpu[0].gib / sim->cnodes[i]->currentinfo.devspec.cpu[0].maxgib, 3));
                output += to_label("\tstorage", to_floating(sim->cnodes[i]->currentinfo.devspec.cpu[0].storage, 4));
                output += to_label("\tdx", to_floating(delta[i].s.col[0], 1));
                output += to_label("\tdy", to_floating(delta[i].s.col[1], 1));
                output += to_label("\tdz", to_floating(delta[i].s.col[2], 1));
                output += to_label("\tdd", to_floating(length_rv(delta[i].s), 2));
                output += to_label("\tdvx", to_floating(delta[i].v.col[0], 2));
                output += to_label("\tdvy", to_floating(delta[i].v.col[1], 2));
                output += to_label("\tdvz", to_floating(delta[i].v.col[2], 2));
                output += to_label("\tddv", to_floating(length_rv(delta[i].v), 3));
                output += to_label("\tdeltavx", to_floating(sumdeltav[i].x, 3));
                output += to_label("\tdeltavy", to_floating(sumdeltav[i].y, 3));
                output += to_label("\tdeltavz", to_floating(sumdeltav[i].z, 3));
                output += to_label("\tdeltav", to_floating(sumdeltav[i].norm(), 3));
                printf("%s\n", output.c_str());
            }
            if (sim->cnodes[i]->currentinfo.event.size())
            {
                for (eventstruc event : sim->cnodes[i]->currentinfo.event)
                {
                    if (printevent)
                    {
                        string output = "type: event";
                        output += to_label("\tnode", sim->cnodes[i]->currentinfo.node.name);
                        output += to_label("\tutc", to_mjd(sim->cnodes[i]->currentinfo.node.utc));
                        output += to_label("\tname", event.name);
                        output += to_label("\ttype", to_unsigned(static_cast<int>(event.type)));
                        output += to_label("\tflag", to_unsigned(static_cast<int>(event.flag)));
                        output += to_label("\tel", to_floating(event.el, 4));
                        output += to_label("\taz", to_floating(event.az, 4));
                        output += to_label("\tdtime", to_floating(86400.*event.dtime, 1));
                        output += to_label("\tlon", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.geod.s.lon, 5));
                        output += to_label("\tlat", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.geod.s.lat, 5));
                        output += to_label("\talt", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.geod.s.h, 1));
                        output += to_label("\tpowerin", to_floating(sim->cnodes[i]->currentinfo.node.phys.powgen, 2));
                        output += to_label("\tland", (event.flag & EVENT_FLAG_LAND) != 0);
                        output += to_label("\tumbra", (event.flag & EVENT_FLAG_UMBRA) != 0);
                        output += to_label("\tgs", (event.flag & EVENT_FLAG_GS) != 0);
                        printf("%s\n", output.c_str());
                    }
                    if (postevent)
                    {
                        json11::Json jobj = json11::Json::object({
                            {"mtype", "event"},
                            {"node_name", sim->cnodes[i]->currentinfo.node.name},
                            {"event_utc", event.utc},
                            {"event_name", event.name},
                            {"event_type", static_cast<int>(event.type)},
                            {"event_flag", static_cast<int>(event.flag)},
                            {"event_el", event.el},
                            {"event_az", event.az},
                            {"geodpos", sim->cnodes[i]->currentinfo.node.loc.pos.geod.s}
                        });
                        string output = jobj.dump();
                        iretn = socket_post(data_channel_out, output.c_str());

                        // Post SOH
                        string jstring;
                        agent->post(Agent::AgentMessage::EVENT, jobj.dump());
                    }
                }
            }
            if (printevent)
            {
                string output = "type: node";
                output += to_label("\tnode", sim->cnodes[i]->currentinfo.node.name);
                output += to_label("\tdt", to_floating(86400.*(sim->cnodes[i]->currentinfo.node.utc-sim->initialutc), 1));
                output += to_label("\tutc", to_mjd(sim->cnodes[i]->currentinfo.node.utc));
                output += to_label("\tecix", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.eci.s.col[0], 1));
                output += to_label("\teciy", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.eci.s.col[1], 1));
                output += to_label("\teciz", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.eci.s.col[2], 1));
                output += to_label("\tecivx", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.eci.v.col[0], 2));
                output += to_label("\tecivy", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.eci.v.col[1], 2));
                output += to_label("\tecivz", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.eci.v.col[2], 2));
                output += to_label("\teciax", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.eci.a.col[0], 3));
                output += to_label("\teciay", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.eci.a.col[1], 3));
                output += to_label("\teciaz", to_floating(sim->cnodes[i]->currentinfo.node.loc.pos.eci.a.col[2], 3));
                output += to_label("\tpowerin", to_floating(sim->cnodes[i]->currentinfo.node.phys.powgen, 2));
                output += to_label("\tpowerout", to_floating(sim->cnodes[i]->currentinfo.node.phys.powuse, 2));
                sim->cnodes[i]->currentinfo.devspec.cpu[0].load = static_cast <float>(deviceCpu.getLoad());
                sim->cnodes[i]->currentinfo.devspec.cpu[0].gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/1073741824.);
                sim->cnodes[i]->currentinfo.devspec.cpu[0].maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/1073741824.);
                sim->cnodes[i]->currentinfo.devspec.cpu[0].maxload = deviceCpu.getCpuCount();
                output += to_label("\tload", to_floating(sim->cnodes[i]->currentinfo.devspec.cpu[0].load / sim->cnodes[i]->currentinfo.devspec.cpu[0].maxload, 3));
                output += to_label("\tmemory", to_floating(sim->cnodes[i]->currentinfo.devspec.cpu[0].gib / sim->cnodes[i]->currentinfo.devspec.cpu[0].maxgib, 3));
                output += to_label("\tstorage", to_floating(sim->cnodes[i]->currentinfo.devspec.cpu[0].storage, 4));
                output += to_label("\tdx", to_floating(delta[i].s.col[0], 1));
                output += to_label("\tdy", to_floating(delta[i].s.col[1], 1));
                output += to_label("\tdz", to_floating(delta[i].s.col[2], 1));
                output += to_label("\tdd", to_floating(length_rv(delta[i].s), 2));
                output += to_label("\tdvx", to_floating(delta[i].v.col[0], 2));
                output += to_label("\tdvy", to_floating(delta[i].v.col[1], 2));
                output += to_label("\tdvz", to_floating(delta[i].v.col[2], 2));
                output += to_label("\tddv", to_floating(length_rv(delta[i].v), 3));
                output += to_label("\tdeltavx", to_floating(sumdeltav[i].x, 3));
                output += to_label("\tdeltavy", to_floating(sumdeltav[i].y, 3));
                output += to_label("\tdeltavz", to_floating(sumdeltav[i].z, 3));
                output += to_label("\tdeltav", to_floating(sumdeltav[i].norm(), 3));
                printf("%s\n", output.c_str());
            }
            if (postevent)
            {
                sim->cnodes[i]->currentinfo.devspec.cpu[0].load = static_cast <float>(deviceCpu.getLoad());
                sim->cnodes[i]->currentinfo.devspec.cpu[0].gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/1073741824.);
                sim->cnodes[i]->currentinfo.devspec.cpu[0].maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/1073741824.);
                sim->cnodes[i]->currentinfo.devspec.cpu[0].maxload = deviceCpu.getCpuCount();
                json11::Json jobj = json11::Json::object({
                    {"type", "node"},
                    {"node", sim->cnodes[i]->currentinfo.node.name},
                    {"ecipos", sim->cnodes[i]->currentinfo.node.loc.pos.eci},
                    {"alphatt", sim->cnodes[i]->currentinfo.node.loc.att.icrf},
                    {"powerin", sim->cnodes[i]->currentinfo.node.phys.powgen},
                    {"powerout", sim->cnodes[i]->currentinfo.node.phys.powuse},
                    {"load", sim->cnodes[i]->currentinfo.devspec.cpu[0].load / sim->cnodes[i]->currentinfo.devspec.cpu[0].maxload},
                    {"memory", sim->cnodes[i]->currentinfo.devspec.cpu[0].gib / sim->cnodes[i]->currentinfo.devspec.cpu[0].maxgib}
                });
                string output = jobj.dump();
                iretn = socket_post(data_channel_out, output.c_str());
                send_telem_to_cosmos_web(&sim->cnodes[i]->currentinfo);

                // Post SOH
                string jstring;
                agent->post(Agent::AgentMessage::SOH, json_of_list(jstring, sim->cnodes[i]->sohstring, &sim->cnodes[i]->currentinfo));
            }
        }
        //        if (printevent)
        //        {
        //            printf("\n");
        //            fflush(stdout);
        //        }
        sim->Propagate();
        sim->Thrust();
        sim->Target();
        sim->Metric();
        if (summarize)
        {
            for (uint16_t i=1; i<sim->cnodes.size(); ++i)
            {
                for (uint16_t j=0; j<sim->targets.size(); ++j)
                {
                    if (sim->cnodes[i]->currentinfo.target[j].cover[0].area != 0.)
                    {
                        ++summaries[i][j].count;
                        summaries[i][j].area += sim->cnodes[i]->currentinfo.target[j].cover[0].area;
                        summaries[i][j].percent += sim->cnodes[i]->currentinfo.target[j].cover[0].percent;
                        summaries[i][j].resolution += sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
                        summaries[i][j].resstd += sim->cnodes[i]->currentinfo.target[j].cover[0].resolution * sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
                        summaries[i][j].azimuth += sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
                        summaries[i][j].azstd += sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth * sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
                        summaries[i][j].elevation += sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;
                        summaries[i][j].elstd += sim->cnodes[i]->currentinfo.target[j].cover[0].elevation * sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;

//                        if (summaries[i][j].resmin == 0. || sim->cnodes[i]->currentinfo.target[j].cover[0].resolution < summaries[i][j].resmin)
//                        {
//                            summaries[i][j].resmin = sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
//                        }
//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].resolution > summaries[i][j].resmax)
//                        {
//                            summaries[i][j].resmax = sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
//                        }

//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth < summaries[i][j].azmin)
//                        {
//                            summaries[i][j].azmin = sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
//                        }
//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth > summaries[i][j].azmax)
//                        {
//                            summaries[i][j].azmax = sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
//                        }

//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].elevation < summaries[i][j].elmin)
//                        {
//                            summaries[i][j].elmin = sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;
//                        }
//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].elevation > summaries[i][j].elmax)
//                        {
//                            summaries[i][j].elmax = sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;
//                        }

                        ++summaries[i][sim->targets.size()].count;
                        summaries[i][sim->targets.size()].area += sim->cnodes[i]->currentinfo.target[j].cover[0].area;
                        summaries[i][sim->targets.size()].percent += sim->cnodes[i]->currentinfo.target[j].cover[0].percent;
                        summaries[i][sim->targets.size()].resolution += sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
                        summaries[i][sim->targets.size()].azimuth += sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
                        summaries[i][sim->targets.size()].elevation += sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;
                        summaries[i][sim->targets.size()].resstd += sim->cnodes[i]->currentinfo.target[j].cover[0].resolution * sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
                        summaries[i][sim->targets.size()].azstd += sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth * sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
                        summaries[i][sim->targets.size()].elstd += sim->cnodes[i]->currentinfo.target[j].cover[0].elevation * sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;

//                        if (summaries[i][sim->targets.size()].resmin == 0. || sim->cnodes[i]->currentinfo.target[j].cover[0].resolution < summaries[i][sim->targets.size()].resmin)
//                        {
//                            summaries[i][sim->targets.size()].resmin = sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
//                        }
//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].resolution > summaries[i][sim->targets.size()].resmax)
//                        {
//                            summaries[i][sim->targets.size()].resmax = sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
//                        }

//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth < summaries[i][sim->targets.size()].azmin)
//                        {
//                            summaries[i][sim->targets.size()].azmin = sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
//                        }
//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth > summaries[i][sim->targets.size()].azmax)
//                        {
//                            summaries[i][sim->targets.size()].azmax = sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
//                        }

//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].elevation < summaries[i][sim->targets.size()].elmin)
//                        {
//                            summaries[i][sim->targets.size()].elmin = sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;
//                        }
//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].elevation > summaries[i][sim->targets.size()].elmax)
//                        {
//                            summaries[i][sim->targets.size()].elmax = sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;
//                        }

                        ++summaries[sim->cnodes.size()][sim->targets.size()].count;
                        summaries[sim->cnodes.size()][sim->targets.size()].area += sim->cnodes[i]->currentinfo.target[j].cover[0].area;
                        summaries[sim->cnodes.size()][sim->targets.size()].percent += sim->cnodes[i]->currentinfo.target[j].cover[0].percent;
                        summaries[sim->cnodes.size()][sim->targets.size()].resolution += sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
                        summaries[sim->cnodes.size()][sim->targets.size()].azimuth += sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
                        summaries[sim->cnodes.size()][sim->targets.size()].elevation += sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;
                        summaries[sim->cnodes.size()][sim->targets.size()].resstd += sim->cnodes[i]->currentinfo.target[j].cover[0].resolution * sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
                        summaries[sim->cnodes.size()][sim->targets.size()].azstd += sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth * sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
                        summaries[sim->cnodes.size()][sim->targets.size()].elstd += sim->cnodes[i]->currentinfo.target[j].cover[0].elevation * sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;

//                        if (summaries[sim->cnodes.size()][sim->targets.size()].resmin == 0. || sim->cnodes[i]->currentinfo.target[j].cover[0].resolution < summaries[sim->cnodes.size()][sim->targets.size()].resmin)
//                        {
//                            summaries[sim->cnodes.size()][sim->targets.size()].resmin = sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
//                        }
//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].resolution > summaries[sim->cnodes.size()][sim->targets.size()].resmax)
//                        {
//                            summaries[sim->cnodes.size()][sim->targets.size()].resmax = sim->cnodes[i]->currentinfo.target[j].cover[0].resolution;
//                        }

//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth < summaries[sim->cnodes.size()][sim->targets.size()].azmin)
//                        {
//                            summaries[sim->cnodes.size()][sim->targets.size()].azmin = sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
//                        }
//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth > summaries[sim->cnodes.size()][sim->targets.size()].azmax)
//                        {
//                            summaries[sim->cnodes.size()][sim->targets.size()].azmax = sim->cnodes[i]->currentinfo.target[j].cover[0].azimuth;
//                        }

//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].elevation < summaries[sim->cnodes.size()][sim->targets.size()].elmin)
//                        {
//                            summaries[sim->cnodes.size()][sim->targets.size()].elmin = sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;
//                        }
//                        if (sim->cnodes[i]->currentinfo.target[j].cover[0].elevation > summaries[sim->cnodes.size()][sim->targets.size()].elmax)
//                        {
//                            summaries[sim->cnodes.size()][sim->targets.size()].elmax = sim->cnodes[i]->currentinfo.target[j].cover[0].elevation;
//                        }
                    }
                }
            }
        }
        if (realtime)
        {
            agent->finish_active_loop();
            elapsed += simdt;
        }
        else
        {
            ++elapsed;
        }
    }
    if (summarize)
    {
        // Heading
        printf("file\ttype\tnode");
        for (uint16_t j=0; j<sim->targets.size(); ++j)
        {
            printf("\tT%02u", j);
        }
        printf("\n");

        // Count
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            printf("%s\tcount", satfile.c_str());
            printf("\t%s", sim->cnodes[i]->currentinfo.node.name.c_str());
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                printf("\t%u", summaries[i][j].count);
            }
            printf("\n");
        }

        // Area
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            printf("%s\tarea", satfile.c_str());
            printf("\t%s", sim->cnodes[i]->currentinfo.node.name.c_str());
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count > 1)
                {
                    summaries[i][j].area /= summaries[i][j].count;
                }
                printf("\t%0.0f", summaries[i][j].area);
            }
            printf("\n");
        }

        // Percent
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            printf("%s\tpercent", satfile.c_str());
            printf("\t%s", sim->cnodes[i]->currentinfo.node.name.c_str());
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                summaries[i][j].percent *= 100.;
                if (summaries[i][j].count > 1)
                {
                    summaries[i][j].percent /= summaries[i][j].count;
                }
                printf("\t%0.2f", summaries[i][j].percent);
            }
            printf("\n");
        }

        // Mean Resolution
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            printf("%s\tresmean", satfile.c_str());
            printf("\t%s", sim->cnodes[i]->currentinfo.node.name.c_str());
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count > 1)
                {
                    summaries[i][j].resstd = sqrt((summaries[i][j].resstd - summaries[i][j].resolution * summaries[i][j].resolution / summaries[i][j].count) / (summaries[i][j].count - 1));
                    summaries[i][j].resolution /= summaries[i][j].count;
                }
                printf("\t%0.2f", summaries[i][j].resolution);
            }
            printf("\n");
        }

        // StDev Resolution
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            printf("%s\tresstd", satfile.c_str());
            printf("\t%s", sim->cnodes[i]->currentinfo.node.name.c_str());
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                printf("\t%0.2f", summaries[i][j].resstd);
            }
            printf("\n");
        }

        // Mean Azimuth
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            printf("%s\tazmean", satfile.c_str());
            printf("\t%s", sim->cnodes[i]->currentinfo.node.name.c_str());
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count > 1)
                {
                    summaries[i][j].azstd = sqrt((summaries[i][j].azstd - summaries[i][j].azimuth * summaries[i][j].azimuth / summaries[i][j].count) / (summaries[i][j].count - 1));
                    summaries[i][j].azimuth /= summaries[i][j].count;
                }
                printf("\t%0.2f", summaries[i][j].azimuth);
            }
            printf("\n");
        }

        // StDev Azimuth
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            printf("%s\tazstd", satfile.c_str());
            printf("\t%s", sim->cnodes[i]->currentinfo.node.name.c_str());
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                printf("\t%0.2f", summaries[i][j].azstd);
            }
            printf("\n");
        }

        // Mean Elevation
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            printf("%s\telmean", satfile.c_str());
            printf("\t%s", sim->cnodes[i]->currentinfo.node.name.c_str());
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count > 1)
                {
                    summaries[i][j].elstd = sqrt((summaries[i][j].elstd - summaries[i][j].elevation * summaries[i][j].elevation / summaries[i][j].count) / (summaries[i][j].count - 1));
                    summaries[i][j].elevation /= summaries[i][j].count;
                }
                printf("\t%0.2f", summaries[i][j].elevation);
            }
            printf("\n");
        }

        // StDev Elevation
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            printf("%s\telstd", satfile.c_str());
            printf("\t%s", sim->cnodes[i]->currentinfo.node.name.c_str());
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                printf("\t%0.2f", summaries[i][j].elstd);
            }
            printf("\n");
        }
        printf("\n");

        // Heading
        printf("file\ttype");
        for (uint16_t j=0; j<11; ++j)
        {
            printf("\tBin%02u", j);
        }
        printf("\n");

        vector<double> histbin(11, 0.);
        vector<uint32_t> histcnt(11, 0);
        double binmax;

        // Count
        binmax = 0.;
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count > binmax)
                {
                    binmax = summaries[i][j].count;
                }
            }
        }

        binmax /= 9.5;
        histcnt[0] = 0;
        for (uint16_t i=0; i<10; ++i)
        {
            histbin[i+1] = binmax * ( 0.5 + i);
            histcnt[i+1] = 0;
        }
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count)
                {
                    uint16_t bin = summaries[i][j].count / binmax;
                    ++histcnt[bin];
                }
                else
                {
                ++histcnt[0];
                }
            }
        }
        printf("%s\tcount_bin", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%0.2f", histbin[i]);
        }
        printf("\n");
        printf("%s\tcount_value", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%u", histcnt[i]);
        }
        printf("\n");

        // Area
        binmax = 0.;
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].area > binmax)
                {
                    binmax = summaries[i][j].area;
                }
            }
        }

        binmax /= 9.5;
        histcnt[0] = 0;
        for (uint16_t i=0; i<10; ++i)
        {
            histbin[i+1] = binmax * ( 0.5 + i);
            histcnt[i+1] = 0;
        }
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count)
                {
                    uint16_t bin = summaries[i][j].area / binmax;
                    ++histcnt[bin];
                }
                else
                {
                    ++histcnt[0];
                }
            }
        }
        printf("%s\tarea_bin", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%0.2f", histbin[i]);
        }
        printf("\n");
        printf("%s\tarea_value", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%u", histcnt[i]);
        }
        printf("\n");

        // Percent
        binmax = 0.;
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].percent > binmax)
                {
                    binmax = summaries[i][j].percent;
                }
            }
        }

        binmax /= 9.5;
        histcnt[0] = 0;
        for (uint16_t i=0; i<10; ++i)
        {
            histbin[i+1] = binmax * ( 0.5 + i);
            histcnt[i+1] = 0;
        }
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count)
                {
                    uint16_t bin = summaries[i][j].percent / binmax;
                    ++histcnt[bin];
                }
                else
                {
                    ++histcnt[0];
                }
            }
        }
        printf("%s\tpercent_bin", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%0.2f", histbin[i]);
        }
        printf("\n");
        printf("%s\tpercent_value", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%u", histcnt[i]);
        }
        printf("\n");

        // Mean Resolution
        binmax = 0.;
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].resolution > binmax)
                {
                    binmax = summaries[i][j].resolution;
                }
            }
        }

        binmax /= 9.5;
        histcnt[0] = 0;
        for (uint16_t i=0; i<10; ++i)
        {
            histbin[i+1] = binmax * ( 0.5 + i);
            histcnt[i+1] = 0;
        }
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count)
                {
                    uint16_t bin = summaries[i][j].resolution / binmax;
                    ++histcnt[bin];
                }
                else
                {
                    ++histcnt[0];
                }
            }
        }
        printf("%s\tresolution_bin", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%0.2f", histbin[i]);
        }
        printf("\n");
        printf("%s\tresolution_value", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%u", histcnt[i]);
        }
        printf("\n");

        // Stdev Resolution
        binmax = 0.;
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].resstd > binmax)
                {
                    binmax = summaries[i][j].resstd;
                }
            }
        }

        binmax /= 9.5;
        histcnt[0] = 0;
        for (uint16_t i=0; i<10; ++i)
        {
            histbin[i+1] = binmax * ( 0.5 + i);
            histcnt[i+1] = 0;
        }
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count)
                {
                    uint16_t bin = summaries[i][j].resstd / binmax;
                    ++histcnt[bin];
                }
                else
                {
                    ++histcnt[0];
                }
            }
        }
        printf("%s\tresstd_bin", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%0.2f", histbin[i]);
        }
        printf("\n");
        printf("%s\tresstd_value", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%u", histcnt[i]);
        }
        printf("\n");

        // Mean Azimuth
        binmax = 0.;
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].azimuth > binmax)
                {
                    binmax = summaries[i][j].azimuth;
                }
            }
        }

        binmax /= 9.5;
        histcnt[0] = 0;
        for (uint16_t i=0; i<10; ++i)
        {
            histbin[i+1] = binmax * ( 0.5 + i);
            histcnt[i+1] = 0;
        }
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count)
                {
                    uint16_t bin = summaries[i][j].azimuth / binmax;
                    ++histcnt[bin];
                }
                else
                {
                    ++histcnt[0];
                }
            }
        }
        printf("%s\tazimuth_bin", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%0.2f", histbin[i]);
        }
        printf("\n");
        printf("%s\tazimuth_value", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%u", histcnt[i]);
        }
        printf("\n");

        // Stdev Azimuth
        binmax = 0.;
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].azstd > binmax)
                {
                    binmax = summaries[i][j].azstd;
                }
            }
        }

        binmax /= 9.5;
        histcnt[0] = 0;
        for (uint16_t i=0; i<10; ++i)
        {
            histbin[i+1] = binmax * ( 0.5 + i);
            histcnt[i+1] = 0;
        }
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count)
                {
                    uint16_t bin = summaries[i][j].azstd / binmax;
                    ++histcnt[bin];
                }
                else
                {
                    ++histcnt[0];
                }
            }
        }
        printf("%s\tazstd_bin", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%0.2f", histbin[i]);
        }
        printf("\n");
        printf("%s\tazstd_value", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%u", histcnt[i]);
        }
        printf("\n");

        // Mean Elevation
        binmax = 0.;
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].elevation > binmax)
                {
                    binmax = summaries[i][j].elevation;
                }
            }
        }

        binmax /= 9.5;
        histcnt[0] = 0;
        for (uint16_t i=0; i<10; ++i)
        {
            histbin[i+1] = binmax * ( 0.5 + i);
            histcnt[i+1] = 0;
        }
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count)
                {
                    uint16_t bin = summaries[i][j].elevation / binmax;
                    ++histcnt[bin];
                }
                else
                {
                    ++histcnt[0];
                }
            }
        }
        printf("%s\televation_bin", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%0.2f", histbin[i]);
        }
        printf("\n");
        printf("%s\televation_value", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%u", histcnt[i]);
        }
        printf("\n");

        // Stdev Elevation
        binmax = 0.;
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].elstd > binmax)
                {
                    binmax = summaries[i][j].elstd;
                }
            }
        }

        binmax /= 9.5;
        histcnt[0] = 0;
        for (uint16_t i=0; i<10; ++i)
        {
            histbin[i+1] = binmax * ( 0.5 + i);
            histcnt[i+1] = 0;
        }
        for (uint16_t i=1; i<sim->cnodes.size(); ++i)
        {
            for (uint16_t j=0; j<sim->targets.size(); ++j)
            {
                if (summaries[i][j].count)
                {
                    uint16_t bin = summaries[i][j].elstd / binmax;
                    ++histcnt[bin];
                }
                else
                {
                    ++histcnt[0];
                }
            }
        }
        printf("%s\telstd_bin", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%0.2f", histbin[i]);
        }
        printf("\n");
        printf("%s\telstd_value", satfile.c_str());
        for (uint16_t i=0; i<11; ++i)
        {
            printf("\t%u", histcnt[i]);
        }
        printf("\n");

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
    if (!jargs["summarize"].is_null())
    {
        ++argcount;
        summarize = jargs["summarize"].number_value();
    }
    if (!jargs["settle"].is_null())
    {
        ++argcount;
        settle = jargs["settle"].number_value();
    }
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
    if (!jargs["cosmos_web_addr"].is_null())
    {
        ++argcount;
        cosmos_web_addr = jargs["cosmos_web_addr"].string_value();
    }
    if (!jargs["formation"].is_null())
    {
        ++argcount;
        formation = jargs["formation"].string_value();
    }
    if (!jargs["spacing"].is_null())
    {
        ++argcount;
        spacing = jargs["spacing"].number_value();
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

int32_t request_dump_node(string &request, string &response, Agent *agent)
{
    response.clear();
    vector<string> args = string_split(request);
    if (args.size() > 1)
    {
        Physics::Simulator::StateList::iterator sit = sim->GetNode(args[1]);
        if (sit != sim->cnodes.end())
        {
            json_dump_node(&(*sit)->currentinfo);
            response = args[1];
        }
    }
    return response.length();
}

int32_t open_cosmos_web_sockets(string cosmos_web_addr)
{
    // No telems are to be emitted if no address was specified
    if (cosmos_web_addr.empty())
    {
        return COSMOS_GENERAL_ERROR_NOTSTARTED;
    }
    string ipv4_address = cosmos_web_addr;
    // If no . or : are found, then it may be a hostname instead, in which case convert to an ipv4 address
    if (cosmos_web_addr.find(".") == std::string::npos && cosmos_web_addr.find(":") == std::string::npos)
    {
        string response;
        int32_t iretn = hostnameToIP(cosmos_web_addr, ipv4_address, response);
        if (iretn < 0)
        {
            cout << "Encountered error in hostnameToIP: " << response << endl;
            exit(0);
        }
    }

    int32_t iretn = socket_open(&cosmos_web_telegraf_channel, NetworkType::UDP, ipv4_address.c_str(), TELEGRAF_PORT, SOCKET_TALK, SOCKET_BLOCKING, 2000000 );
    if ((iretn) < 0)
    {
        cout << "Failed to open socket cosmos_web_telegraf_channel: " << cosmos_error_string(iretn) << endl;
        exit(0);
    }
    iretn = socket_open(&cosmos_web_api_channel, NetworkType::UDP, ipv4_address.c_str(), API_PORT, SOCKET_TALK, SOCKET_BLOCKING, 2000000);
    if ((iretn) < 0)
    {
        cout << "Failed to open socket cosmos_web_api_channel: " << cosmos_error_string(iretn) << endl;
        exit(0);
    }
    sockets_initialized = true;
    return iretn;
}

/**
 * @brief Sends a single node's telems to cosmos web
 * 
 * @param cinfo cosmosstruc of some node to store telems for
 * @return int32_t 0 on success, negative on failure
 */
int32_t send_telem_to_cosmos_web(cosmosstruc* cinfo)
{
    if (!sockets_initialized)
    {
        return COSMOS_GENERAL_ERROR_NOTSTARTED;
    }
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

    return 0;
}

/**
 * @brief Reset the simulation database
 */
void reset_db(Physics::Simulator* sim)
{
    if (!sockets_initialized)
    {
        return;
    }
    socket_sendto(cosmos_web_api_channel, "{\"swchstruc\": true, \"battstruc\": true, \"bcregstruc\": true, \"cpustruc\": true, \"device\": true, \"device_type\": true, \"locstruc\": true, \"magstruc\": true, \"node\": true, \"tsenstruc\": true, \"rwstruc\": true, \"mtrstruc\": true, \"attstruc_icrf\": true, \"cosmos_event\": true, \"event_type\": true, \"gyrostruc\": true, \"locstruc_eci\": true, \"target\": true, \"cosmos_event\": true }");
    // Iterate over sats
    uint16_t node_id = 1;
    for (auto sit = sim->cnodes.begin(); sit != sim->cnodes.end(); sit++)
    {
        // Compute node id manually until it's stored somewhere in cosmosstruc
        uint16_t id = node_id;
        (*sit)->currentinfo.node.name == "mother" ? id = 0 : ++node_id;

        // Repopulate node table
        json11::Json jobj = json11::Json::object({
            {"node", json11::Json::object({
                         { "node_id", id },
                         { "node_name", (*sit)->currentinfo.node.name },
                         { "node_type", (*sit)->currentinfo.node.type },
                         { "agent_name", (*sit)->currentinfo.agent0.name },
                         { "utc", sim->currentutc },
                         { "utcstart", sim->initialutc }
                     })}
        });
        socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    }
    auto mother = sim->GetNode("mother");
    if (mother != sim->cnodes.end())
    {
        // Groundstations & targets
        json11::Json jobj = json11::Json::object({
            {"target", [mother]() -> vector<json11::Json>
                {
                    vector<json11::Json> ret;
                    uint16_t target_id = 0;
                    for (auto target : (*mother)->currentinfo.target) {
                        ret.push_back({
                            json11::Json::object({
                                { "id"  , target_id++ },
                                { "name", target.name },
                                { "type", target.type },
                                { "lat" , target.loc.pos.geod.s.lat },
                                { "lon" , target.loc.pos.geod.s.lon },
                                { "h" , target.loc.pos.geod.s.h },
                                { "area" , target.area }
                            })
                        });
                    }
                    return ret;
                }()
            }
        });
        socket_sendto(cosmos_web_telegraf_channel, jobj.dump());
    }
    // A bit silly, but reset requires some wait time at the moment 
    // cout << "Resetting db..." << endl;
    secondsleep(1.);
}
