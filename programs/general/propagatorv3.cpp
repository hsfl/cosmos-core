#include "support/configCosmos.h"
#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"

int32_t parse_control(string args);
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
    while (agent->running() && elapsed < runcount)
    {
        sim->Propagate();
        for (auto &state : sim->cnodes)
        {
            if (state->currentinfo.event.size())
            {
                for (eventstruc event : state->currentinfo.event)
                {
//                    string jstringa;
//                    json_out_commandevent(jstringa, event);
//                    string jstringb;
//                    json_out_geoidpos(jstringb, state->currentinfo.node.loc.pos.geod);
//                    string jstringc;
//                    json_join(jstringa, jstringb, jstringc);
//                    printf("%s\n", jstringc.c_str());

                    json11::Json jobj = json11::Json::object({
                                          {"event_utc", event.utc},
                                          {"event_name", event.name},
                                          {"event_type", event.type},
                                          {"event_flag", event.flag},
                        {"geodpos", state->currentinfo.node.loc.pos.geod.s}
                    });
                    printf("%s\n", jobj.dump().c_str());
                }
            }
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

