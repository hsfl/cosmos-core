#include "physics/simulatorclass.h"
#include "support/jsonclass.h"
#include "support/datalib.h"
#include "support/packetcomm.h"

using namespace Convert;

/*
 * For use with Cosmos Web's orbital display panel.
 * This will be run by the orbital backend Grafana plugin and return CZML
 * formatted orbital data.
 */

const int precision = 8;

// Number of minutes for half and orbit
const int half_orbit_t = 50;

// Network socket stuff
const int SOCKET_IN_PORT = 10090;
const char OUT_ADDRESS[] = "grafana";
static socket_channel data_channel;

// Self-contained propagator unit
struct prop_unit
{
    Physics::Simulator sim;
    vector<Convert::locstruc> initiallocs;
    vector<string> nodes;
    map<string, string> czmls;
    double startutc = 0;
    double endutc;
    // Simulation time step
    double simdt = 60.;
    // Number of times to increment simulation state (i.e., total simulated time = simdt*runcount)
    double runcount = half_orbit_t;
};

// Utility functions
int32_t to_czml(string& output, string arg);
int32_t czml_head(prop_unit& prop, string& output);
int32_t czml_body(prop_unit& prop);
int32_t czml_foot(prop_unit& prop, string& output);

int main(int argc, char *argv[])
{
    int32_t iretn;
    PacketComm packet;
    // open up a socket for getting data to/from grafana backend
    iretn = socket_open(
                &data_channel,
                NetworkType::UDP,
                "",
                SOCKET_IN_PORT,
                SOCKET_LISTEN,
                SOCKET_BLOCKING,
                2000000
                );
    if ((iretn) < 0)
    {
        printf("Failed to open socket for listening: %s\n", cosmos_error_string(iretn));
        exit (iretn);
    }

    while(true)
    {
        // Wait for incoming packets
        iretn = socket_recvfrom(data_channel, packet.packetized, 10000);
        if (iretn <= 0) {
            continue;
        }
        string response;
        string arg(packet.packetized.begin(), packet.packetized.end());

        // Requesting propagator
        // if (arg == "[1]")
        // {
            iretn = to_czml(response, arg);
        // }

        iretn = socket_sendto(data_channel, response);
    }

    return 0;
}

////////////////////////////////////////////
// UTILITY FUNCTIONS
////////////////////////////////////////////
// output is the czml-formatted response string
// arg is the json string arguments for the initial propagator settings
int32_t to_czml(string& output, string arg)
{
    int32_t iretn;
    
    // initialize simulator object
    prop_unit prop;
    iretn = prop.sim.GetError();
    if (iretn < 0) {
        printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        output = cosmos_error_string(iretn).c_str();
        return iretn;
    }

    output.clear();

    double now = currentmjd();
    prop.endutc = now + half_orbit_t/1440.;

    string estring;
    json11::Json jargs = json11::Json::parse(arg, estring);
    // Argument format is:
    // [{node_name, utc, px, py, pz, vx, vy, vz}, ...]
    if (!jargs.is_array())
    {
        output = "Argument format error";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    // Each array element is a node in the simulator
    for (auto& el : jargs.array_items())
    {
        if (el["Node_name"].is_null()   // Name of the node
        || el["Utc"].is_null()  // Timestamp of this position/velocity
        || el["Px"].is_null()   // ECI frame position
        || el["Py"].is_null()
        || el["Pz"].is_null()
        || el["Vx"].is_null()   // ECI frame velocity
        || el["Vy"].is_null()
        || el["Vz"].is_null())
        {
            cout << "Argument format error" << endl;
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        // Since propagating to startutc can take a long time, specify arbitrary
        // time range limit, say, at most a week old data. (Which still can take a few seconds)
        if (now - el["Utc"].number_value() > 7.)
        {
            // Time range error
            cout << "Error in node <" << el["Node_name"].string_value() << ">, must be at most a week old" << endl;
            return 0;
        }
        if (el["Utc"].number_value() > now)
        {
            // Time range error
            cout << "Error in node <" << el["Node_name"].string_value() << ">, initialutc error, is in the future?" << endl;
            return 0;
        }
        Convert::locstruc initialloc;
        initialloc.pos.eci.utc = el["Utc"].number_value();
        initialloc.pos.eci.s.col[0] = el["Px"].number_value();
        initialloc.pos.eci.s.col[1] = el["Py"].number_value();
        initialloc.pos.eci.s.col[2] = el["Pz"].number_value();
        initialloc.pos.eci.v.col[0] = el["Vx"].number_value();
        initialloc.pos.eci.v.col[1] = el["Vy"].number_value();
        initialloc.pos.eci.v.col[2] = el["Vz"].number_value();
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
        prop.initiallocs.push_back(initialloc);
        prop.nodes.push_back(el["Node_name"].string_value());
        prop.startutc = std::max(prop.startutc, el["Utc"].number_value());
    }
    // Optional argument for simdt
    if (!jargs["Simdt"].is_null()) prop.simdt = jargs["Simdt"].number_value();
    
    // The goal is to predict a full orbit's worth of data centered at current time
    // (i.e., -45min to +45min of current time)
    prop.startutc = std::max(prop.startutc, now-half_orbit_t/1440.);
    prop.runcount = (now-prop.startutc)*1440. + half_orbit_t;

    prop.sim.Init(prop.startutc, prop.simdt);

    // Add all nodes
    // Note, adding node automatically advances it to startutc
    for (size_t i = 0; i < prop.nodes.size(); ++i)
    {
        iretn = prop.sim.AddNode(prop.nodes[i], Physics::Structure::HEX65W80H, Physics::Propagator::PositionGaussJackson, Physics::Propagator::AttitudeLVLH, Physics::Propagator::None, Physics::Propagator::None, prop.initiallocs[i]);
        if (iretn < 0)
        {
            printf("Error adding node %s: %s\n", prop.nodes[i].c_str(), cosmos_error_string(iretn).c_str());
            output = cosmos_error_string(iretn).c_str();
            return iretn;
        }
    }
    // Add initial header
    czml_head(prop, output);

    double elapsed = 0;
    while (elapsed < prop.runcount)
    {
        // Step forward in simulation
        prop.sim.Propagate();

        // Construct body of czml
        czml_body(prop);

        ++elapsed;
    }

    // Add footer
    czml_foot(prop, output);

    // cout << output << endl;

    return 0;
}

// Header of czmls
// Pass this same output arg into czml_foot at end of program
int32_t czml_head(prop_unit& prop, string& output)
{
    output.clear();
    output +=
    "["
        "{"
            "\"id\": \"document\","
            "\"name\": \"Cesium Orbit Display for Cosmos Web\","
            "\"version\": \"1.0\""
        "},\n";
    for (auto sit = prop.sim.cnodes.begin(); sit != prop.sim.cnodes.end(); ++sit)
    {
        string& czml_pos = prop.czmls[sit->first];
        //double utc = sit->second->currentinfo.node.loc.pos.eci.utc;
        string epoch = utc2iso8601(prop.startutc);
        string interval = epoch + "/" + utc2iso8601(prop.endutc);
        czml_pos +=
            "{"
                "\"id\": \"" + sit->first + "\","
                "\"availability\": \"" + interval + "\","
                "\"position\": {"
                    "\"epoch\": \"" + epoch + "\","
                    "\"referenceFrame\": \"INERTIAL\","
                    "\"cartesian\": [";
        string& czml_att = prop.czmls[sit->first + "att"];
        czml_att +=
                "\"orientation\": {"
                    "\"epoch\": \"" + epoch + "\","
                    "\"interpolationAlgorithm\": \"LINEAR\","
                    "\"interpolationDegree\": 1,"
                    "\"unitQuaternion\": [";
    }

    return 0;
}

// Create a CZML-format JSON string out of provided values
// To be consumed by Cosmos Web's Cesium panel
int32_t czml_body(prop_unit& prop)
{
    for (auto sit = prop.sim.cnodes.begin(); sit != prop.sim.cnodes.end(); ++sit)
    {
        double utc;
        double px, py, pz;
        double qw, qx, qy, qz;
        utc = sit->second->currentinfo.node.loc.pos.eci.utc;
        px = sit->second->currentinfo.node.loc.pos.eci.s.col[0];
        py = sit->second->currentinfo.node.loc.pos.eci.s.col[1];
        pz = sit->second->currentinfo.node.loc.pos.eci.s.col[2];
        qx = sit->second->currentinfo.node.loc.att.geoc.s.d.x;
        qy = sit->second->currentinfo.node.loc.att.geoc.s.d.y;
        qz = sit->second->currentinfo.node.loc.att.geoc.s.d.z;
        qw = sit->second->currentinfo.node.loc.att.geoc.s.w;

        // Get appropriate czml string of node_name
        string& czml = prop.czmls[sit->first];
        // Time offset from specified epoch (in seconds)
        czml += "\n";
        czml += to_floatany(86400.*(utc-prop.startutc), precision) + ",";

        // ECI
        czml += to_floatany(px, precision) + ",";
        czml += to_floatany(py, precision) + ",";
        czml += to_floatany(pz, precision) + ",";

        string& czml_att = prop.czmls[sit->first + "att"];
        czml_att += "\n";
        czml_att += to_floatany(86400.*(utc-prop.startutc), precision) + ",";

        // attitudes
        czml_att += to_floatany(qx, precision) + ",";
        czml_att += to_floatany(qy, precision) + ",";
        czml_att += to_floatany(qz, precision) + ",";
        czml_att += to_floatany(qw, precision) + ",";
    }

    return 0;
}

// Complete the czml formatting
int32_t czml_foot(prop_unit& prop, string& output)
{
    for (auto sit = prop.sim.cnodes.begin(); sit != prop.sim.cnodes.end(); ++sit)
    {
        string& czml = prop.czmls[sit->first];
        // Remove trailing comma
        if (!czml.empty())
        {
            czml.pop_back();
        }
        // Complete cartesian property, add some other properties
        czml +=     "]"    
                "},"
                "\"model\": {"
                    "\"gltf\":\"./public/plugins/testorg-testplugin/img/HyTI.glb\","
                    "\"scale\":4.0,"
                    "\"minimumPixelSize\": 50"
                "},"
                "\"path\": {"
                    "\"material\": {"
                        "\"polylineOutline\": {"
                            "\"color\": {"
                                "\"rgba\": [255, 0, 255, 255]"
                            "}"
                        "}"
                    "},"
                    "\"width\": 5,"
                    "\"leadTime\": 5400,"
                    "\"trailTime\": 5400,"
                    "\"resolution\": 1"
                "},"
            ;
        
        output += czml;

        string& czml_att = prop.czmls[sit->first + "att"];
        // Remove trailing comma
        if (!czml_att.empty())
        {
            czml_att.pop_back();
        }
        // Complete orientation property
        czml_att += "]"    
                "}"
            "},";
        output += czml_att;
    }
    // Remove trailing comma
    if (!output.empty())
    {
        output.pop_back();
    }
    // Close array
    output += "]";

    return 0;
}