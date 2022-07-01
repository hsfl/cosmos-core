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
 * 
 * example usage:
 * ./propagator_web_json '{"start":59270,"runcount":5,"simdt":60,"telem":"ecipos,eciatt","nodes":[{"name":"node0","frame":"phys", "lat":0.371876,"lon":-2.755147,"alt":400000,"angle":0.942478}]}'
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
// Telem keys
const string JMJD = "mjd";          // MJD timestamp
const string JECIPOS = "ecipos";    // ECI Positions
const string JECIVEL = "ecivel";    // ECI Velocities
const string JECIACC = "eciacc";    // ECI Accelerations
const string JAX = "ax";
const string JAY = "ay";
const string JAZ = "az";
const string JECIATT = "eciatt";    // ECI Attitude
const string JATTX = "qx";
const string JATTY = "qy";
const string JATTZ = "qz";
const string JATTW = "qw";


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
int32_t prop_parse_nodes(prop_unit& prop, const json11::Json& nodes, string& response);
int32_t create_sim_snapshot(const prop_unit& prop, json11::Json::array& output);
int32_t validate_json_args(const json11::Json& jargs, string& response);
int32_t validate_json_node(const json11::Json& jargs, string& response);

int main(int argc, char *argv[])
{
    int32_t iretn;
    // Pass this to functions to return as JSON string or as an error message
    string response;

    if (argc != 2)
    {
        cerr << "Incorrect number of arguments";
        return -1;
    }
    string args = string(argv[argc-1]);

    // initialize simulator object
    prop_unit prop;
    iretn = init_propagator(prop, args, response);
    if (iretn < 0)
    {
        cerr << response;
        return iretn;
    }

    iretn = run_propagator(prop, response);
    if (iretn < 0)
    {
        cerr << response;
        return iretn;
    }

    // Propagator run was a success
    cout << response;

    return 0;
}

////////////////////////////////////////////
// PROPAGATOR FUNCTIONS
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

    // Determine initial node conditions
    iretn = prop_parse_nodes(prop, jargs[JNODES], response);
    if (iretn < 0) {
        return iretn;
    }

    // Initialize the simulator
    prop.sim.Init(prop.startutc, prop.simdt);

    // Add all nodes
    // Note, adding node automatically advances it to startutc
    for (size_t i = 0; i < prop.nodes.size(); ++i)
    {
        iretn = prop.sim.AddNode(prop.nodes[i], Physics::Structure::HEX65W80H, Physics::Propagator::PositionGaussJackson, Physics::Propagator::AttitudeLVLH, Physics::Propagator::None, Physics::Propagator::None, prop.initiallocs[i].pos.eci, prop.initiallocs[i].att.icrf);
        if (iretn < 0)
        {
            response = "Error adding node " + prop.nodes[i] + ": " + cosmos_error_string(iretn);
            return iretn;
        }
    }

    // Clear all unused vectors
    prop.initiallocs.clear();
    prop.nodes.clear();

    return 0;
}

/**
 * @brief Run the propagator
 * 
 * The propagator object is initialized with the initial conditions specified in 
 * the JSON string \a args.
 * 
 * @param prop prop_unit object to run simulation for
 * @param response JSON-encoded string data for one iteration of the propagator
 * @return int32_t 0 on success, negative on error
 */
int32_t run_propagator(prop_unit& prop, string& response)
{
    int32_t iretn;
    response.clear();
    // Array of all telem snapshots
    json11::Json::array output;

    // Push initial state to output
    iretn = create_sim_snapshot(prop, output);
    if (iretn < 0)
    {
        return iretn;
    }

    double elapsed = 0;
    while (elapsed < prop.runcount)
    {
        // Pre propagation step
        // ...

        // Step forward in simulation
        prop.sim.Propagate();
        iretn = prop.sim.GetError();
        if (iretn < 0) {
            response = "Error in Propagate(): " + cosmos_error_string(iretn);
            return iretn;
        }

        // Post-propagation step
        // ...
        // Save results of new timestep to output
        iretn = create_sim_snapshot(prop, output);
        if (iretn < 0)
        {
            return iretn;
        }

        ++elapsed;
    }

    // Convert final JSON array to string and return
    json11::Json final_output = json11::Json { output };
    response = final_output.dump();

    return 0;
}

////////////////////////////////////////////
// VALIDATION FUNCTIONS
////////////////////////////////////////////

/**
 * @brief Validate args provided to propagator
 * 
 * Checks that all required fields are provided and are of the correct type.
 * 
 * @param jargs JSON-parsed object of JSON-string provided through command line.
 * @param response Error message, if any.
 * @return int32_t 0 on success, negative on error.
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

/**
 * @brief Validate initial node parameters
 * 
 * Checks that all required fields for initializing the node are provided and are of the correct type.
 * 
 * @param node JSON-parsed object in the array of nodes, from the nodes field of the JSON args.
 * @param response Error messages, if any.
 * @return int32_t 0 on success, negative on error.
 */
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
        if (!node[JPX].is_number()
         || !node[JPY].is_number()
         || !node[JPZ].is_number()
         || !node[JVX].is_number()
         || !node[JVY].is_number()
         || !node[JVZ].is_number())
        {
            response = "Argument format error, all required elements for " + JECI + " must be numbers.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }
    else if (frame == JPHYS)
    {
        if (node[JLAT].is_null()
         || node[JLON].is_null()
         || node[JALT].is_null()
         || node[JANGLE].is_null())
        {
            response = "Argument format error, all required elements for " + JPHYS + " must be provided.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        if (!node[JLAT].is_number()
         || !node[JLON].is_number()
         || !node[JALT].is_number()
         || !node[JANGLE].is_number())
        {
            response = "Argument format error, all required elements for " + JPHYS + " must be numbers.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }
    else if (frame == JKEP)
    {
        if (node[JEA].is_null()
         || node[JINC].is_null()
         || node[JAP].is_null()
         || node[JRAAN].is_null()
         || node[JECC].is_null()
         || node[JSMA].is_null())
        {
            response = "Argument format error, all required elements for " + JKEP + " must be provided.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        if (!node[JEA].is_number()
         || !node[JINC].is_number()
         || !node[JAP].is_number()
         || !node[JRAAN].is_number()
         || !node[JECC].is_number()
         || !node[JSMA].is_number())
        {
            response = "Argument format error, all required elements for " + JKEP + " must be numbers.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }
    else
    {
        response = "Argument format error, " + JFRAME + " must be one of: " + JECI + ", " + JPHYS + ", or " + JKEP + ".";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    if (!node[JUTC].is_null() && !node[JUTC].is_number())
    {
        response = "Argument format error, " + JUTC + " must be a number.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }

    return 0;
}

////////////////////////////////////////////
// OTHER FUNCTIONS
////////////////////////////////////////////

/**
 * @brief Parse initial node conditions from JSON
 * 
 * @param prop Propagator object
 * @param nodes JSON-parsed array of nodes with initial conditions.
 * @param response Error messages, if any.
 * @return int32_t 0 on success, negative on error.
 */
int32_t prop_parse_nodes(prop_unit& prop, const json11::Json& nodes, string& response)
{
    int32_t iretn;
    response.clear();
    
    // Each array element is a node in the simulator
    for (const auto& node : nodes.array_items())
    {
        // Check that all required fields are provided and are of the correct type
        iretn = validate_json_node(node, response);
        if (iretn < 0)
        {
            return iretn;
        }
        double nodeutc = prop.startutc;
        if (!node[JUTC].is_null())
        {
            nodeutc = node[JUTC].number_value();
        }

        Convert::locstruc initialloc;
        string frame = node[JFRAME].string_value();
        if (frame == JECI)
        {
            initialloc.pos.eci.utc = nodeutc;
            initialloc.pos.eci.s.col[0] = node[JPX].number_value();
            initialloc.pos.eci.s.col[1] = node[JPY].number_value();
            initialloc.pos.eci.s.col[2] = node[JPZ].number_value();
            initialloc.pos.eci.v.col[0] = node[JVX].number_value();
            initialloc.pos.eci.v.col[1] = node[JVY].number_value();
            initialloc.pos.eci.v.col[2] = node[JVZ].number_value();
            initialloc.pos.eci.pass++;
            iretn = pos_eci(initialloc);
            if (iretn < 0)
            {
                response = "Error in pos_eci() for node " + node[JNAME].string_value() + ", " + cosmos_error_string(iretn);
                return iretn;
            }
        }
        else if (frame == JPHYS)
        {
            initialloc = Physics::shape2eci(
                nodeutc,
                node[JLAT].number_value(),
                node[JLON].number_value(),
                node[JALT].number_value(),
                node[JANGLE].number_value(),
                0.
            );
        }
        else if (frame == JKEP)
        {
            kepstruc kep;
            kep.utc  = nodeutc;
            kep.ea   = node[JEA].number_value();
            kep.i    = node[JINC].number_value();
            kep.ap   = node[JAP].number_value();
            kep.raan = node[JRAAN].number_value();
            kep.e    = node[JECC].number_value();
            kep.a    = node[JSMA].number_value();
            iretn = kep2eci(kep, initialloc.pos.eci);
            if (iretn < 0)
            {
                response = "Error in kep2eci() for node " + node[JNAME].string_value() + ", " + cosmos_error_string(iretn);
                return iretn;
            }
            initialloc.pos.eci.pass++;
            iretn = pos_eci(initialloc);
            if (iretn < 0)
            {
                response = "Error in pos_eci() for node " + node[JNAME].string_value() + ", " + cosmos_error_string(iretn);
                return iretn;
            }
        }

        prop.initiallocs.push_back(initialloc);
        prop.nodes.push_back(node[JNAME].string_value());
        prop.startutc = std::max(prop.startutc, nodeutc); // TODO: if initial backpropagating is possible, we don't need this line
    }

    return 0;
}

/**
 * @brief Append a node telem JSON object containing desired telem fields to output
 * 
 * @param prop prop_unit object with current simulator state
 * @param output Array to push nodes state to
 * @return int32_t 
 */
int32_t create_sim_snapshot(const prop_unit& prop, json11::Json::array& output)
{
    for (auto sit = prop.sim.cnodes.begin(); sit != prop.sim.cnodes.end(); ++sit)
    {
        json11::Json::object node_telem;
        node_telem[JNAME] = sit->first;
        node_telem[JUTC] = sit->second->currentinfo.node.loc.pos.eci.utc;
        // Iterate over user's desired telems, adding it to the output
        for (auto t : prop.telem)
        {
            if (t == JECIPOS) {
                node_telem[JPX] = sit->second->currentinfo.node.loc.pos.eci.s.col[0];
                node_telem[JPY] = sit->second->currentinfo.node.loc.pos.eci.s.col[1];
                node_telem[JPZ] = sit->second->currentinfo.node.loc.pos.eci.s.col[2];
            } else if (t == JECIVEL) {
                node_telem[JVX] = sit->second->currentinfo.node.loc.pos.eci.v.col[0];
                node_telem[JVY] = sit->second->currentinfo.node.loc.pos.eci.v.col[1];
                node_telem[JVZ] = sit->second->currentinfo.node.loc.pos.eci.v.col[2];
            } else if (t == JECIACC) {
                node_telem[JAX] = sit->second->currentinfo.node.loc.pos.eci.a.col[0];
                node_telem[JAY] = sit->second->currentinfo.node.loc.pos.eci.a.col[1];
                node_telem[JAZ] = sit->second->currentinfo.node.loc.pos.eci.a.col[2];
            } else if (t == JECIATT) {
                node_telem[JATTX] = sit->second->currentinfo.node.loc.att.geoc.s.d.x;
                node_telem[JATTY] = sit->second->currentinfo.node.loc.att.geoc.s.d.y;
                node_telem[JATTZ] = sit->second->currentinfo.node.loc.att.geoc.s.d.z;
                node_telem[JATTW] = sit->second->currentinfo.node.loc.att.geoc.s.w;
            }
        } // end telem for-loop
        output.push_back(node_telem);
    } // end sim node for-loop

    return 0;
}
