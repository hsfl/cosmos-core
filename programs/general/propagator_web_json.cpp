#include "physics/simulatorclass.h"
#include "support/cosmos-errclass.h"
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
 * ./propagator_web_json '{"start":59270,"runcount":5,"simdt":60,"telem":["poseci"],"nodes":[{"name":"node0","phys":{"lat":0.371876,"lon":-2.755147,"alt":400000,"angle":0.942478}}]}'
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
const string JECI = "eci";
const string JKEP = "kep";
const string JPHYS = "phys";
const string JLVLH= "lvlh";
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
const string JSHAPE = "shape";
const string JPUSH = "push";
// Telem keys
const string JMJD = "mjd";          // MJD timestamp
const string JPOSECI = "poseci";    // ECI Positions
const string JVELECI = "veleci";    // ECI Velocities
const string JACCECI = "acceci";    // ECI Accelerations
const string JPOSKEP = "poskep";    // KEP Positions
const string JPOSPHYS = "posphys";  // PHYS Positions
const string JAX = "ax";
const string JAY = "ay";
const string JAZ = "az";
const string JATTECI = "atteci";    // ECI Attitude
const string JATTX = "qx";
const string JATTY = "qy";
const string JATTZ = "qz";
const string JATTW = "qw";
const string JATTLVLH = "attlvlh";  // LVLH Attitude

// Special keys in args, will be treated a bit specially
const string JSENDTODB = "db";

// Struct of all propagator types to use in simulation
// Specify these with json args
struct prop_types
{
    Physics::Structure::Type structure = Physics::Structure::NoType;
    Physics::Propagator::Type position_prop = Physics::Propagator::None;
    Physics::Propagator::Type attitude_prop = Physics::Propagator::None;
    Physics::Propagator::Type thermal_prop = Physics::Propagator::None;
    Physics::Propagator::Type electrical_prop = Physics::Propagator::None;
};


struct prop_node
{
    string name;
    prop_types pt;
    Convert::locstruc initialloc;
    vector<double> push;
    string shape;
};

// Self-contained propagator unit
struct prop_unit
{
    Physics::Simulator sim;
    vector<prop_node> nodes;
    double startutc = 0;
    double endutc;
    // Simulation time step
    double simdt = 60.;
    // Number of times to increment simulation state (i.e., total simulated time = simdt*runcount)
    double runcount = 1;
    // List of telem to return
    vector<string> telem;
    // To send to database or not (through telegraf)
    bool to_db = false;
};

int32_t run_propagator(prop_unit& prop, string& response);
int32_t init_propagator(prop_unit& prop, const string& args, string& response);
int32_t prop_parse_nodes(prop_unit& prop, const json11::Json& nodes, string& response);
int32_t create_sim_snapshot(const prop_unit& prop, json11::Json::array& output);
int32_t validate_json_args(const json11::Json& jargs, string& response);
int32_t validate_json_node(const json11::Json& jargs, string& response);
int32_t sendToTelegraf(string& response);
int32_t hostnameToIP(const string hostname, string& ipaddr, string& response);

int main(int argc, char *argv[])
{
    int32_t iretn = 0;
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
    // Send to database
    if (prop.to_db)
    {
        iretn = sendToTelegraf(response);
        if (iretn < 0)
        {
            cerr << response;
            return iretn;
        }
        cout << "Propagator ran successfully and was sent to Telegraf";
    }
    else {
        cout << response;
    }

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
    for (auto& item : jargs[JTELEM].array_items()) {
        if (item.string_value().empty()) {
            continue;
        }
        prop.telem.push_back(item.string_value());
    }
    if (!jargs[JEND].is_null()) {
        prop.endutc = jargs[JEND].number_value();
        prop.runcount = (prop.endutc - prop.startutc)/(prop.simdt/86400.);
    } else {
        prop.runcount = jargs[JRUNCOUNT].number_value();
        prop.endutc = prop.startutc + (prop.simdt/86400. * prop.runcount);
    }
    if (!jargs[JSENDTODB].is_null()) {
        prop.to_db = jargs[JSENDTODB].bool_value();
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
    for (auto& node : prop.nodes)
    {
        node.pt.position_prop = Physics::Propagator::PositionGaussJackson;
        node.pt.attitude_prop = Physics::Propagator::AttitudeLVLH;
        iretn = prop.sim.AddNode(node.name, node.pt.structure, node.pt.position_prop, node.pt.attitude_prop, node.pt.thermal_prop, node.pt.electrical_prop, node.initialloc.pos.eci, node.initialloc.att.icrf);
        if (iretn < 0)
        {
            response = "Error adding node " + node.name + ": " + cosmos_error_string(iretn);
            return iretn;
        }

        // Apply push
        auto sit = prop.sim.cnodes.find(node.name);
        if (sit == prop.sim.cnodes.end())
        {
            response = "Error at cnodes.find(" + node.name + "): " + cosmos_error_string(iretn);
            return COSMOS_GENERAL_ERROR_OUTOFRANGE;
        }
        sit->second->currentinfo.node.phys.fpush.x = sit->second->currentinfo.node.phys.mass * node.push[0];
        sit->second->currentinfo.node.phys.fpush.y = sit->second->currentinfo.node.phys.mass * node.push[1];
        sit->second->currentinfo.node.phys.fpush.z = sit->second->currentinfo.node.phys.mass * node.push[2];
    }

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
    int32_t iretn = 0;
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
     || jargs[JTELEM].is_null()    // Array of string values specifies which telems to return
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
    if (!jargs[JTELEM].is_array()) {
        response = "Argument format error, " + JTELEM + " must be an array of strings.";
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
        if (jargs[JSTART].number_value() >= jargs[JEND].number_value()) {
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
    if (node[JNAME].is_null())   // Name of the node
    {
        response = "Argument format error, " + JNAME + " must be provided.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    if (!node[JNAME].is_string()) {
        response = "Argument format error, " + JNAME + " name must be a string.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    vector<string> frames;
    if (!node[JECI].is_null()) {
        frames.push_back(JECI);
    }
    if (!node[JPHYS].is_null()) {
        frames.push_back(JPHYS);
    }
    if (!node[JKEP].is_null()) {
        frames.push_back(JKEP);
    }
    if (frames.size() < 1) {
        response = "Argument format error, frame was not provided.";
        return COSMOS_GENERAL_ERROR_ARGS;
    } else if (frames.size() > 1) {
        response = "Argument format error, more than one frame detected: ";
        for (string frame : frames) {
            response += frame + ", ";
        }
        response.erase(response.size()-2);
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    if (!node[frames.front()].is_object()) {
        response = "Argument format error, frame must be a dict.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    if (frames.front() == JECI)
    {
        if (node[frames.front()][JPX].is_null()
         || node[frames.front()][JPY].is_null()
         || node[frames.front()][JPZ].is_null()
         || node[frames.front()][JVX].is_null()
         || node[frames.front()][JVY].is_null()
         || node[frames.front()][JVZ].is_null())
        {
            response = "Argument format error, not all keys for " + JECI + " were provided.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        if (!node[frames.front()][JPX].is_number()
         || !node[frames.front()][JPY].is_number()
         || !node[frames.front()][JPZ].is_number()
         || !node[frames.front()][JVX].is_number()
         || !node[frames.front()][JVY].is_number()
         || !node[frames.front()][JVZ].is_number())
        {
            response = "Argument format error, all keys for " + JECI + " must be numbers.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }
    else if (frames.front() == JPHYS)
    {
        if (node[frames.front()][JLAT].is_null()
         || node[frames.front()][JLON].is_null()
         || node[frames.front()][JALT].is_null()
         || node[frames.front()][JANGLE].is_null())
        {
            response = "Argument format error, not all keys for " + JPHYS + " were provided.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        if (!node[frames.front()][JLAT].is_number()
         || !node[frames.front()][JLON].is_number()
         || !node[frames.front()][JALT].is_number()
         || !node[frames.front()][JANGLE].is_number())
        {
            response = "Argument format error, all keys for " + JPHYS + " must be numbers.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }
    else if (frames.front() == JKEP)
    {
        if (node[frames.front()][JEA].is_null()
         || node[frames.front()][JINC].is_null()
         || node[frames.front()][JAP].is_null()
         || node[frames.front()][JRAAN].is_null()
         || node[frames.front()][JECC].is_null()
         || node[frames.front()][JSMA].is_null())
        {
            response = "Argument format error, not all keys for " + JKEP + " were provided.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        if (!node[frames.front()][JEA].is_number()
         || !node[frames.front()][JINC].is_number()
         || !node[frames.front()][JAP].is_number()
         || !node[frames.front()][JRAAN].is_number()
         || !node[frames.front()][JECC].is_number()
         || !node[frames.front()][JSMA].is_number())
        {
            response = "Argument format error, all keys for " + JKEP + " must be numbers.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }
    else
    {
        response = "Strange error at frames.front()";
        return COSMOS_GENERAL_ERROR_ARGS;
    }

    if (!node[JUTC].is_null() && !node[JUTC].is_number())
    {
        response = "Argument format error, " + JUTC + " must be a number.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }

    if (!node[JSHAPE].is_null() && !node[JSHAPE].is_string())
    {
        response = "Argument format error, " + JSHAPE + " must be a string.";
        return COSMOS_GENERAL_ERROR_ARGS;
    }

    if (!node[JPUSH].is_null())
    {
        if (!node[JPUSH].is_object())
        {
            response = "Argument format error, " + JPUSH + " must be a dict.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        if (node[JPUSH][JAX].is_null()
         || node[JPUSH][JAY].is_null()
         || node[JPUSH][JAZ].is_null())
        {
            response = "Argument format error, not all keys for " + JPUSH + " were provided.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        if (!node[JPUSH][JAX].is_number()
         || !node[JPUSH][JAY].is_number()
         || !node[JPUSH][JAZ].is_number())
        {
            response = "Argument format error, keys for " + JPUSH + " must be numbers.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
    }

    return 0;
}

////////////////////////////////////////////
// OTHER FUNCTIONS
////////////////////////////////////////////

/**
 * @brief Parse initial node conditions from JSON
 * 
 * Call validate_json_node() first.
 * 
 * @param prop Propagator object
 * @param nodes JSON-parsed array of nodes with initial conditions.
 * @param response Error messages, if any.
 * @return int32_t 0 on success, negative on error.
 */
int32_t prop_parse_nodes(prop_unit& prop, const json11::Json& nodes, string& response)
{
    int32_t iretn = 0;
    response.clear();
    
    // Each array element is a node in the simulator
    for (const auto& node : nodes.array_items())
    {
        prop_node new_node;
        new_node.name = node[JNAME].string_value();

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
        if (!node[JECI].is_null())
        {
            initialloc.pos.eci.utc = nodeutc;
            initialloc.pos.eci.s.col[0] = node[JECI][JPX].number_value();
            initialloc.pos.eci.s.col[1] = node[JECI][JPY].number_value();
            initialloc.pos.eci.s.col[2] = node[JECI][JPZ].number_value();
            initialloc.pos.eci.v.col[0] = node[JECI][JVX].number_value();
            initialloc.pos.eci.v.col[1] = node[JECI][JVY].number_value();
            initialloc.pos.eci.v.col[2] = node[JECI][JVZ].number_value();
            initialloc.pos.eci.pass++;
            iretn = pos_eci(initialloc);
            if (iretn < 0)
            {
                response = "Error in pos_eci() for node " + node[JNAME].string_value() + ", " + cosmos_error_string(iretn);
                return iretn;
            }
        }
        else if (!node[JPHYS].is_null())
        {
            initialloc = Physics::shape2eci(
                nodeutc,
                node[JPHYS][JLAT].number_value(),
                node[JPHYS][JLON].number_value(),
                node[JPHYS][JALT].number_value(),
                node[JPHYS][JANGLE].number_value(),
                0.
            );
        }
        else if (!node[JKEP].is_null())
        {
            kepstruc kep;
            kep.utc  = nodeutc;
            kep.ea   = node[JKEP][JEA].number_value();
            kep.i    = node[JKEP][JINC].number_value();
            kep.ap   = node[JKEP][JAP].number_value();
            kep.raan = node[JKEP][JRAAN].number_value();
            kep.e    = node[JKEP][JECC].number_value();
            kep.a    = node[JKEP][JSMA].number_value();
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
        else {
            response = "Argument format error, frame is not provided.";
            return COSMOS_GENERAL_ERROR_ARGS;
        }
        
        new_node.initialloc = initialloc;

        new_node.push.push_back(node[JPUSH][JAX].number_value());
        new_node.push.push_back(node[JPUSH][JAY].number_value());
        new_node.push.push_back(node[JPUSH][JAZ].number_value());

        // TODO: add more rigorous constraints to shape
        if (!node[JSHAPE].is_null()) {
            if (node[JSHAPE].string_value() == "U1") {
                new_node.pt.structure = Physics::Structure::U1;
            } else if (node[JSHAPE].string_value() == "U2") {
                new_node.pt.structure = Physics::Structure::U2;
            } else if (node[JSHAPE].string_value() == "U3") {
                new_node.pt.structure = Physics::Structure::U3;
            } else if (node[JSHAPE].string_value() == "U6") {
                new_node.pt.structure = Physics::Structure::U6;
            } else if (node[JSHAPE].string_value() == "U12") {
                new_node.pt.structure = Physics::Structure::U12;
            } else {
                response = "Argument format error, " + JSHAPE + " " + node[JSHAPE].string_value() + " is not supported.";
                return COSMOS_GENERAL_ERROR_ARGS;
            }
        } else {
            new_node.pt.structure = Physics::Structure::U3;
        }
        

        prop.nodes.push_back(new_node);
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
        for (string t : prop.telem)
        {
            if (t == JPOSECI) {
                json11::Json::object eci_telem = node_telem[JECI].object_items();
                eci_telem[JPX] = sit->second->currentinfo.node.loc.pos.eci.s.col[0];
                eci_telem[JPY] = sit->second->currentinfo.node.loc.pos.eci.s.col[1];
                eci_telem[JPZ] = sit->second->currentinfo.node.loc.pos.eci.s.col[2];
                node_telem[JECI] = eci_telem;
            } else if (t == JVELECI) {
                json11::Json::object eci_telem(node_telem[JECI].object_items());
                eci_telem[JVX] = sit->second->currentinfo.node.loc.pos.eci.v.col[0];
                eci_telem[JVY] = sit->second->currentinfo.node.loc.pos.eci.v.col[1];
                eci_telem[JVZ] = sit->second->currentinfo.node.loc.pos.eci.v.col[2];
                node_telem[JECI] = eci_telem;
            } else if (t == JACCECI) {
                json11::Json::object eci_telem = node_telem[JECI].object_items();
                eci_telem[JAX] = sit->second->currentinfo.node.loc.pos.eci.a.col[0];
                eci_telem[JAY] = sit->second->currentinfo.node.loc.pos.eci.a.col[1];
                eci_telem[JAZ] = sit->second->currentinfo.node.loc.pos.eci.a.col[2];
                node_telem[JECI] = eci_telem;
            } else if (t == JATTECI) {
                json11::Json::object eci_telem = node_telem[JECI].object_items();
                // TODO: double check that att.geoc is correct for eci attitude
                eci_telem[JATTX] = sit->second->currentinfo.node.loc.att.geoc.s.d.x;
                eci_telem[JATTY] = sit->second->currentinfo.node.loc.att.geoc.s.d.y;
                eci_telem[JATTZ] = sit->second->currentinfo.node.loc.att.geoc.s.d.z;
                eci_telem[JATTW] = sit->second->currentinfo.node.loc.att.geoc.s.w;
                node_telem[JECI] = eci_telem;
            } else if (t == JATTLVLH) {
                json11::Json::object lvlh_telem = node_telem[JLVLH].object_items();
                lvlh_telem[JATTX] = sit->second->currentinfo.node.loc.att.lvlh.s.d.x;
                lvlh_telem[JATTY] = sit->second->currentinfo.node.loc.att.lvlh.s.d.y;
                lvlh_telem[JATTZ] = sit->second->currentinfo.node.loc.att.lvlh.s.d.z;
                lvlh_telem[JATTW] = sit->second->currentinfo.node.loc.att.lvlh.s.w;
                node_telem[JLVLH] = lvlh_telem;
            } else if (t == JPOSPHYS) {
                json11::Json::object phys_telem = node_telem[JPHYS].object_items();
                phys_telem[JLAT] = sit->second->currentinfo.node.loc.pos.geod.s.lat;
                phys_telem[JLON] = sit->second->currentinfo.node.loc.pos.geod.s.lon;
                phys_telem[JALT] = sit->second->currentinfo.node.loc.pos.geod.s.h;
                node_telem[JPHYS] = phys_telem;
            } else if (t == JPOSKEP) {
                json11::Json::object kep_telem = node_telem[JKEP].object_items();
                Convert::kepstruc kep;
                Convert::eci2kep(sit->second->currentinfo.node.loc.pos.eci, kep);
                kep_telem[JEA] = kep.ea;
                kep_telem[JINC] = kep.i;
                kep_telem[JAP] = kep.ap;
                kep_telem[JRAAN] = kep.raan;
                kep_telem[JECC] = kep.e;
                kep_telem[JSMA] = kep.a;
                node_telem[JKEP] = kep_telem;
            } else {
                continue;
            }
        } // end telem for-loop
        output.push_back(node_telem);
    } // end sim node for-loop

    return 0;
}

/**
 * @brief Converts a hostname to an ip address
 * 
 * Used here to convert docker-compose services' network aliases into usable ip addresses
 * 
 * @param hostname Host name to convert.
 * @param ipaddr Successful conversion stored here.
 * @param response Error messages, if any.
 * @return 0 on success, negative on error
 */
int32_t hostnameToIP(const string hostname, string& ipaddr, string& response)
{
    int32_t iretn = 0;
    addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // Docker binds to both ipv4 and ipv6
    hints.ai_socktype = 0;
    hints.ai_protocol = 0;
    addrinfo* addrs;
    iretn = getaddrinfo(hostname.c_str(), NULL, &hints, &addrs);
    if (iretn != 0)
    {
        response = "Failed to resolve hostname " + hostname + ": " + std::to_string(iretn) + " " + gai_strerror(iretn);
        return iretn;
    }
    char addr_string[100];
    // Only checking first in addrinfo list, they should all be similar
    switch (addrs->ai_family)
    {
    case AF_INET:
        inet_ntop(addrs->ai_family, &((struct sockaddr_in const *)addrs->ai_addr)->sin_addr, addr_string, 100);
        break;
    case AF_INET6:
        inet_ntop(addrs->ai_family, &((struct sockaddr_in6 const *)addrs->ai_addr)->sin6_addr, addr_string, 100);
        break;
    default:
        response = "addrs->ai_family was an unexpected value " + std::to_string(addrs->ai_family);
        return GENERAL_ERROR_ARGS;
        break;
    }
    ipaddr = addr_string;
    freeaddrinfo(addrs);
    return 0;
}

int32_t sendToTelegraf(string& response)
{
    int32_t iretn = 0;
    const string TELEGRAF_ADDR = "cosmos_telegraf";
    const int TELEGRAF_PORT = 10097;
    socket_channel cosmos_web_telegraf_channel;
    string addr_string;
    iretn = hostnameToIP(TELEGRAF_ADDR, addr_string, response);
    if (iretn < 0)
    {
        return iretn;
    }

    iretn = socket_open(
                &cosmos_web_telegraf_channel,
                NetworkType::UDP,
                addr_string.c_str(),
                TELEGRAF_PORT,
                SOCKET_TALK,
                SOCKET_BLOCKING,
                2000000
                );
    if ((iretn) < 0)
    {
        response = "Failed to open socket cosmos_web_telegraf_channel: " + cosmos_error_string(iretn);
        return iretn;
    }

    socket_sendto(cosmos_web_telegraf_channel, response);

    return 0;
}
