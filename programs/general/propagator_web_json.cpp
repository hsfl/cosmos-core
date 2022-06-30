#include "physics/simulatorclass.h"
#include "support/datalib.h"
#include <iomanip>
#include <sstream>
#include <cmath>

using namespace Convert;

/*
 * For use with Cosmos Web's orbital display panel.
 * This will be run by the orbital backend Grafana plugin and return CZML
 * formatted orbital data.
 */

const int precision = 3;

// For JSON keys in args
const string JSTART = "start";
const string JSIMDT = "simdt";
const string JEND = "end";
const string JRUNCOUNT = "runcount";
const string JTELEM = "telem";
const string JNODES = "nodes";
// For JSON keys in node
const string JNAME = "name";
const string JFRAME = "frame";
const string JECI = "eci";
const string JKEP = "kep";
const string JPHYS = "phys";
const string JPX = "px";
const string JPY = "py";
const string JPZ = "pz";
const string JVX = "vx";
const string JVY = "vy";
const string JVZ = "vz";
const string JLAT = "lat";
const string JLON = "lon";
const string JALT = "alt";
const string JANGLE = "angle";
const string JEA = "ea";
const string JINC = "inc";
const string JAP = "ap";
const string JRAAN = "raan";
const string JECC = "ecc";
const string JSMA = "sma";
const string JUTC = "utc";

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
    double runcount = 1;
    // List of telem to return
    vector<string> telem;
};

int32_t run_propagator(prop_unit& prop, string& response);
int32_t init_propagator(prop_unit& prop, const string& args, string& response);
int32_t prop_add_nodes(prop_unit& prop, const json11::Json& nodes, string& response);
int32_t validate_json_args(const json11::Json& jargs, string& response);
int32_t validate_json_node(const json11::Json& jargs, string& response);

int main(int argc, char *argv[])
{
    int32_t iretn;
    // Pass to functions to return as JSON string or as an error message
    string response;

    if (argc != 2)
    {
        cerr << "Incorrect number of arguments" << endl;
        return -1;
    }
    string args = string(argv[argc-1]);

    // initialize simulator object
    prop_unit prop;
    iretn = init_propagator(prop, args, response);
    if (iretn < 0)
    {
        cerr << response << endl;
        return iretn;
    }

    while(true)
    {
        iretn = run_propagator(prop, response);
    }

    return 0;
}

////////////////////////////////////////////
// UTILITY FUNCTIONS
////////////////////////////////////////////

/**
 * @brief Initialize the propagator object
 * 
 * The propagator object is initialized with the initial conditions specified in 
 * the JSON string \a args.
 * 
 * @param prop prop_unit object to initialize
 * @param args JSON string from the command line argument
 * @param response The response string
 * @return int32_t 0 on success, negative on error
 */
int32_t init_propagator(prop_unit& prop, const string& args, string& response)
{
    response.clear();
    // See if simulator was properly created
    int32_t iretn = prop.sim.GetError();
    if (iretn < 0) {
        response = "Error Creating Simulator: " + cosmos_error_string(iretn);
        return iretn;
    }
    // Attempt JSON parsing of command line arg
    json11::Json jargs = json11::Json::parse(args, response);
    if (!response.empty()) {
        return COSMOS_JSON_ERROR_JSTRING;
    }

    // Check that all required keys were sent in args and that they were of the correct type
    iretn = validate_json_args(jargs, response);
    if (iretn < 0) {
        return iretn;
    }

    // Global propagator settings
    prop.startutc = jargs[JSTART].number_value();
    prop.simdt = jargs[JSIMDT].number_value();
    prop.telem = string_split(jargs[JTELEM].string_value());
    if (!jargs[JEND].is_null()) {
        prop.endutc = jargs[JEND].number_value();
        prop.runcount = (prop.endutc - prop.startutc)/(prop.simdt/86400.);
    } else {
        prop.runcount = jargs[JRUNCOUNT].number_value();
        prop.endutc = prop.startutc + (prop.simdt/86400. * prop.runcount);
    }

    // Add nodes to propagator
    iretn = prop_add_nodes(prop, jargs[JNODES], response);
    if (iretn < 0) {
        return iretn;
    }


    return 0;
}

/// Run one iteration of the propagator
/// \param output JSON-encoded string data for one iteration of the propagator
/// \param arg The json string arguments for the initial propagator settings
int32_t run_propagator(prop_unit& prop, string& response)
{
    //int32_t iretn;
    response.clear();


    double elapsed = 0;
    while (elapsed < prop.runcount)
    {
        // Step forward in simulation
        prop.sim.Propagate();

        ++elapsed;
    }

    return 0;
}

/**
 * @brief Validate args provided to propagator
 * 
 * @param prop 
 * @param jargs 
 * @param response 
 * @return int32_t 0 on success, negative on error
 */
int32_t validate_json_args(const json11::Json& jargs, string& response)
{
    response.clear();

    // All required fields must be provided
    if (jargs[JSTART].is_null()    // MJD start time of simulation
     || jargs[JSIMDT].is_null()    // MJD end time of simulation
     || jargs[JTELEM].is_null()    // Comma-separated list of telem tags to return
     || jargs[JNODES].is_null())   // Array of initial nodes
    {
        response = "Argument format error, all required fields must be provided.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }

    if (!jargs[JSTART].is_number()) {
        response = "Argument format error, " + JSTART + " must be a number.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    if (!jargs[JSIMDT].is_number()) {
        response = "Argument format error, " + JSIMDT + " must be a number.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    if (!jargs[JTELEM].is_string()) {
        response = "Argument format error, " + JTELEM + " must be a string.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    if (!jargs[JNODES].is_array()) {
        response = "Argument format error, " + JNODES + " must be an array.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }

    // Specify exactly one of end or runcount
    if (!jargs[JEND].is_null() && jargs[JRUNCOUNT].is_null()) {
        if (!jargs[JEND].is_number()) {
            response = "Argument format error, " + JEND + " must be a number.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        if (jargs[JSTART].number_value() <= jargs[JEND].number_value()) {
            response = "Argument error, " + JEND + " must be greater than " + JSTART + ".";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }
    else if (jargs[JEND].is_null() && !jargs[JRUNCOUNT].is_null()) {
        if (!jargs[JRUNCOUNT].is_number()) {
            response = "Argument format error, " + JRUNCOUNT + " must be a number.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    } else {
        response = "Argument format error, exactly one of either " + JEND + " or " + JRUNCOUNT + " must be specified.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }

    return 0;
}

int32_t validate_json_node(const json11::Json& node, string& response)
{
    if (node[JNAME].is_null()   // Name of the node
    || node[JFRAME].is_null())  // The frame of reference to be used to initialize node position and orbit
    {
        response = "Argument format error, all required node fields must be provided.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    if (!node[JNAME].is_string()) {
        response = "Argument format error, " + JNAME + " name must be a string.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    if (!node[JFRAME].is_string()) {
        response = "Argument format error, " + JFRAME + " must be a string.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    string frame = node[JFRAME].string_value();
    if (frame == JECI)
    {
        if (node[JPX].is_null()
         || node[JPY].is_null()
         || node[JPZ].is_null()
         || node[JVX].is_null()
         || node[JVY].is_null()
         || node[JVZ].is_null())
        {
            response = "Argument format error, all required elements for " + JECI + " must be provided.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }
    else if (frame == JPHYS)
    {

    }
    else if (frame == JKEP)
    {

    }
    else
    {
        response = "Argument format error, " + JFRAME + " must be one of: " + JECI + ", " + JPHYS + ", or " + JKEP + ".";
        return COSMOS_GENERAL_ERROR_ARGS;
    }

    return 0;
}

int32_t prop_add_nodes(prop_unit& prop, const json11::Json& nodes, string& response)
{
    int32_t iretn;
    response.clear();
    
    // Each array element is a node in the simulator
    for (const auto& node : nodes.array_items())
    {
        iretn = validate_json_node(node, response);
        if (iretn < 0)
        {
            return iretn;
        }

        /*
        // Grab all values from the query
        Convert::locstruc initialloc;
        initialloc.pos.eci.utc = el["utc"].number_value();
        initialloc.pos.eci.s.col[0] = el["px"].number_value();
        initialloc.pos.eci.s.col[1] = el["py"].number_value();
        initialloc.pos.eci.s.col[2] = el["pz"].number_value();
        initialloc.pos.eci.v.col[0] = el["vx"].number_value();
        initialloc.pos.eci.v.col[1] = el["vy"].number_value();
        initialloc.pos.eci.v.col[2] = el["vz"].number_value();
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
        prop.initiallocs.push_back(initialloc);
        prop.nodes.push_back(el["node_name"].string_value());
        prop.startutc = std::max(prop.startutc, el["startUtc"].number_value());
        if (!el[JRUNCOUNT].is_null())
        {
            prop.runcount = el[JRUNCOUNT].int_value();
        }
        // Optional argument for simdt
        if (!jargs[JSIMDT].is_null())
        {
            prop.simdt = jargs[JSIMDT].number_value();
        }

        // Since propagating to startutc can take a long time, specify arbitrary
        // time range limit, say, at most a week old data. (Which still can take a few seconds)
        if (prop.startutc - el["utc"].number_value() > 7.)
        {
            // Time range error
            cout << "Error in node <" << el["node_name"].string_value() << ">, must be at most a week old" << endl;
            return 0;
        }
        if (el["utc"].number_value() > prop.startutc)
        {
            // Time range error
            cout << "Error in node <" << el["node_name"].string_value() << ">, initialutc error, is in the future?" << endl;
            return 0;
        }*/
    }
    
    // The goal is to predict a full orbit's worth of data starting at current time
    // (i.e., +90min of current time)
    prop.endutc = prop.startutc + prop.runcount/60/24.;

    prop.sim.Init(prop.startutc, prop.simdt);

    // Add all nodes
    // Note, adding node automatically advances it to startutc
    for (size_t i = 0; i < prop.nodes.size(); ++i)
    {
        iretn = prop.sim.AddNode(prop.nodes[i], Physics::Structure::HEX65W80H, Physics::Propagator::PositionGaussJackson, Physics::Propagator::AttitudeLVLH, Physics::Propagator::None, Physics::Propagator::None, prop.initiallocs[i].pos.eci, prop.initiallocs[i].att.icrf);
        if (iretn < 0)
        {
            printf("Error adding node %s: %s\n", prop.nodes[i].c_str(), cosmos_error_string(iretn).c_str());
            response = cosmos_error_string(iretn).c_str();
            return iretn;
        }
    }
    return 0;
}
