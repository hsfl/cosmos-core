#include "physics/simulatorclass.h"
#include "support/jsonclass.h"
#include "support/datalib.h"

using namespace Convert;

/*
 * For use with Cosmos Web's orbital display panel.
 * This will be run by the orbital backend Grafana plugin and return CZML
 * formatted orbital data.
 */

static Physics::Simulator *sim;
static double startutc = 0;
static vector<Convert::locstruc> initiallocs;
static vector<string> nodes;
// Simulation time step
static double simdt = 60.;
// Number of times to increment simulation state (i.e., total simulated time = simdt*runcount)
static double runcount = 45.;

string to_czml(const Cosmos::Physics::Simulator::StateList::iterator sit);

int main(int argc, char *argv[])
{
    int32_t iretn;

    double now = currentmjd();

    string estring;
    json11::Json jargs = json11::Json::parse(argv[1], estring);
    // Argument format is:
    // [{node_name, utc, px, py, pz, vx, vy, vz}, ...]
    if (!jargs.is_array())
    {
        cout << "Argument format error" << endl;
        exit(COSMOS_GENERAL_ERROR_ARGS);
    }
    int nnn = 0;
    for (auto& el : jargs.array_items())
    {
        if (el["node_name"].is_null()
        || el["utc"].is_null()
        || el["px"].is_null()
        || el["py"].is_null()
        || el["pz"].is_null()
        || el["vx"].is_null()
        || el["vy"].is_null()
        || el["vz"].is_null())
        {
            cout << "Argument format error" << endl;
            Convert::locstruc initialloc = Physics::shape2eci(currentmjd(), RADOF(21.3069), RADOF(-157.8583), 400000., RADOF(54.), 0.);
            initiallocs.push_back(initialloc);
            nodes.push_back("node" + std::to_string(nnn++));
            startutc = currentmjd();
            continue;
            exit(COSMOS_GENERAL_ERROR_ARGS);
        }
        // Since propagating to startutc can take a long time, specify arbitrary
        // time range limit, say, at most a week old data. (Which still can take a few seconds)
        if (now - el["utc"].number_value() > 7)
        {
            // Time range error
            cout << "Error in node <" << el["node_name"].string_value() << ">, must be at most a week old" << endl;
            return 0;
        }
        if (el["utc"].number_value() > now)
        {
            // Time range error
            cout << "Error in node <" << el["node_name"].string_value() << ">, initialutc error, is in the future?" << endl;
            return 0;
        }
        Convert::locstruc initialloc;
        initialloc.pos.eci.s.col[0] = el["utc"].number_value();
        initialloc.pos.eci.s.col[1] = el["py"].number_value();
        initialloc.pos.eci.s.col[2] = el["pz"].number_value();
        initialloc.pos.eci.v.col[0] = el["vx"].number_value();
        initialloc.pos.eci.v.col[1] = el["vy"].number_value();
        initialloc.pos.eci.v.col[2] = el["vz"].number_value();
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
        initiallocs.push_back(initialloc);
        nodes.push_back(el["node_name"].string_value());
        startutc = std::max(startutc, el["utc"].number_value());
    }

    if (!jargs["simdt"].is_null()) simdt = jargs["simdt"].number_value();

    // initialize simulator object
    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn < 0) {
        printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    
    // The goal is to predict a full orbit's worth of data centered at current time
    // (i.e., -45min to +45min of current time)
    startutc = std::max(startutc, now-45./1440.);
    runcount = (now-startutc)*1440. + 45.;
    sim->Init(startutc, simdt);

    // Add all nodes
    // Note, adding node automatically advances it to startutc
    for (size_t i = 0; i < nodes.size(); ++i)
    {
        iretn = sim->AddNode(nodes[i], Physics::Structure::HEX65W80H, Physics::Propagator::PositionGaussJackson, Physics::Propagator::AttitudeLVLH, Physics::Propagator::None, Physics::Propagator::None, initiallocs[i]);
        if (iretn < 0)
        {
            printf("Error adding node %s: %s\n", nodes[i].c_str(), cosmos_error_string(iretn).c_str());
            exit(iretn);
        }
    }

    string header;
    header += "Name\tUTC\t";

    header += "pos_x\tpos_y\tpos_z\t";

    header += "att_w\tatt_x\tatt_y\tatt_z\t";

    double mjd = currentmjd();
    string path = data_name("", mjd, "txt", "orbit");
    FILE *ofp = fopen(path.c_str(), "w");
    fprintf(ofp, "%s\n", header.c_str());

    path = data_name("", mjd, "txt", "event");
    FILE *efp = fopen(path.c_str(), "w");
    fprintf(efp, "%s\n", header.c_str());

    double elapsed = 0;
    while (elapsed < runcount)
    {
        // Step forward in simulation
        sim->Propagate();

        // Populate result
        string output;

        for (auto sit = sim->cnodes.begin(); sit != sim->cnodes.end(); ++sit)
        {
            output += to_czml(sit);

            output += "\n";
        }
        fprintf(ofp, "%s\n", output.c_str());

        ++elapsed;
    }

    fclose(ofp);
    fclose(efp);
}

// Create a CZML-format JSON string out of provided values
// To be consumed by Cosmos Web's Cesium panel
string to_czml(const Cosmos::Physics::Simulator::StateList::iterator sit)
{
    string czml;
    double utc;
    double px, py, pz;
    double qw, qx, qy, qz;
    utc = sit->second->currentinfo.node.loc.pos.eci.utc;
    px = sit->second->currentinfo.node.loc.pos.eci.s.col[0];
    py = sit->second->currentinfo.node.loc.pos.eci.s.col[1];
    pz = sit->second->currentinfo.node.loc.pos.eci.s.col[2];
    qw = sit->second->currentinfo.node.loc.att.icrf.s.w;
    qx = sit->second->currentinfo.node.loc.att.icrf.s.d.x;
    qy = sit->second->currentinfo.node.loc.att.icrf.s.d.y;
    qz = sit->second->currentinfo.node.loc.att.icrf.s.d.z;

    czml += sit->second->currentinfo.node.name + "\t";
    czml += to_mjd(utc) + "\t";

    // ECI
    czml += to_floatany(px) + "\t";
    czml += to_floatany(py) + "\t";
    czml += to_floatany(pz) + "\t";

    // attitudes
    czml += to_floatany(qw) + "\t";
    czml += to_floatany(qx) + "\t";
    czml += to_floatany(qy) + "\t";
    czml += to_floatany(qz) + "\t";

    return czml;
}
