#include "support/configCosmos.h"
#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"

int32_t parse_control(string args);
int32_t request_get_sat_json(string &request, string &response, Agent *agent);
int32_t request_get_node_json(string &request, string &response, Agent *agent);
int32_t request_get_pieces_json(string &request, string &response, Agent *agent);
int32_t request_get_devspec_json(string &request, string &response, Agent *agent);
int32_t request_get_devgen_json(string &request, string &response, Agent *agent);
int32_t request_get_location(string &request, string &response, Agent *agent);
Physics::Simulator *sim;
Agent *agent;
double simdt = 1.;
double speed=1.;
double initialutc = 60107.01;
double endutc = 0.;
double deltautc = 0.;
string realmname = "propagate";
string orbitfile = "orbit.dat";
string satfile = "sats.dat";
string targetfile = "targets.dat";
string tlefile = "tle.dat";
double runcount = 1500;

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
    agent->add_request("get_location", request_get_location, "nodename", "Get JSON of position and attitude for Node nodename");

    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn <0) {
        agent->debug_log.Printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }
    sim->Init(initialutc, simdt, realmname);
    sim->ParseOrbitFile();
    sim->ParseSatFile();
    sim->ParseTargetFile();
    if (speed == 1.)
    {
        agent->cinfo->agent0.aprd = simdt;
        agent->start_active_loop();
    }
    while (agent->running() && elapsed < runcount)
    {
        sim->Propagate();
        for (auto &state : sim->cnodes)
        {
            if (speed > 1.0 && state->currentinfo.event.size())
            {
                for (eventstruc event : state->currentinfo.event)
                {
                    json11::Json jobj = json11::Json::object({
                                          {"event_utc", event.utc},
                                          {"event_name", event.name},
                                          {"event_type", event.type},
                                          {"event_flag", event.flag},
                                          {"event_el", event.el},
                                          {"event_az", event.az},
                                          {"geodpos", state->currentinfo.node.loc.pos.geod.s}
                    });
                    printf("%s\n", jobj.dump().c_str());
                }
            }
        }
        if (speed == 1.)
        {
            agent->finish_active_loop();
        }
    }


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

int32_t request_get_location(string &request, string &response, Agent *agent)
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
                {"offset", (*sit)->currentinfo.node.utcoffset},
                {"pos", (*sit)->currentinfo.node.loc.pos.eci},
                {"att", (*sit)->currentinfo.node.loc.att.icrf}
            });
            response = jobj.dump();
        }
    }
    return response.length();
}
