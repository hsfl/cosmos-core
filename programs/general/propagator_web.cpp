#include "physics/simulatorclass.h"
#include "support/jsonclass.h"
#include "support/datalib.h"
#include "support/packetcomm.h"
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

// Number of minutes for half and orbit
const int half_orbit_t = 50;

// Network socket stuff
const int PROPAGATOR_WEB_PORT = 10092;
// const char OUT_ADDRESS[] = "grafana";
static socket_channel data_channel;

// Cosmos Web Telegraf port
const int TELEGRAF_SIMDATA_PORT = 10097;
// Where telegraf is listening for simdata
const char TELEGRAF_SIMDATA_ADDR[] = "172.19.0.5"; // is this the static?
static socket_channel cosmos_web_telegraf_channel;

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
    double runcount = half_orbit_t*2;
};

// Czml functions
int32_t run_propagator(string& output, string arg);
int32_t czml_head(prop_unit& prop, string& output);
int32_t czml_body(prop_unit& prop);
int32_t czml_foot(prop_unit& prop, string& output);

// Other functions
int32_t calculate_distances(prop_unit& prop);
double degreesToRadians(double degrees);
double integral(double(*f)(double x), double a, double b, int n);
double IntBck(double x);
double vlc_snr(double distance);
int32_t create_simdata(prop_unit& prop);

int main(int argc, char *argv[])
{
    int32_t iretn;
    PacketComm packet;
    // open up a socket for getting data to/from grafana backend
    iretn = socket_open(
                &data_channel,
                NetworkType::UDP,
                "",
                PROPAGATOR_WEB_PORT,
                SOCKET_LISTEN,
                SOCKET_BLOCKING,
                2000000
                );
    if ((iretn) < 0)
    {
        printf("Failed to open socket for listening: %s\n", cosmos_error_string(iretn));
        exit (iretn);
    }
    // open up a socket for getting beacon data to Telegraf
    iretn = socket_open(
                &cosmos_web_telegraf_channel,
                NetworkType::UDP,
                TELEGRAF_SIMDATA_ADDR,
                TELEGRAF_SIMDATA_PORT,
                SOCKET_TALK,
                SOCKET_BLOCKING,
                2000000
                );
    if ((iretn) < 0)
    {
        printf("Failed to open socket cosmos_web_telegraf_channel: %s\n", cosmos_error_string(iretn));
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
            iretn = run_propagator(response, arg);
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
int32_t run_propagator(string& output, string arg)
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

    string estring;
    json11::Json jargs = json11::Json::parse(arg, estring);
    // Argument format is:
    // [{node_name, utc, px, py, pz, vx, vy, vz}, ...]
    if (!jargs.is_array())
    {
        output = "Argument format error, must be array";
        return COSMOS_GENERAL_ERROR_ARGS;
    }
    // Each array element is a node in the simulator
    for (auto& el : jargs.array_items())
    {
        if (el["node_name"].is_null()   // Name of the node
        || el["utc"].is_null()  // Timestamp of this position/velocity
        || el["px"].is_null()   // ECI frame position
        || el["py"].is_null()
        || el["pz"].is_null()
        || el["vx"].is_null()   // ECI frame velocity
        || el["vy"].is_null()
        || el["vz"].is_null()
        || el["startUtc"].is_null())
        {
            cout << "Argument format error, required keys not all detected" << endl;
            return COSMOS_GENERAL_ERROR_ARGS;
        }
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
        if (!el["runCount"].is_null())
        {
            prop.runcount = el["runCount"].int_value();
        }
        // Optional argument for simdt
        if (!jargs["simdt"].is_null())
        {
            prop.simdt = jargs["simdt"].number_value();
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
        }
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
            output = cosmos_error_string(iretn).c_str();
            return iretn;
        }
    }
    // Add initial header
    czml_head(prop, output);

    double elapsed = 0;
    while (elapsed < prop.runcount)
    {
        // Construct body of czml
        czml_body(prop);

        // Calculate other stuff
        create_simdata(prop);

        // Step forward in simulation
        prop.sim.Propagate();

        ++elapsed;
    }

    // Add footer
    czml_foot(prop, output);

    // cout << output << endl;
    cout << "Query successful" << endl;
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
            "\"name\": \"CosmosOrbitDisplay\","
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
        std::stringstream ss, ss_att;
        ss << "\n";
        ss << std::setprecision(precision) << std::fixed;
        ss << (86400.*(utc-prop.startutc)) << ",";

        // ECI
        ss << px << ",";
        ss << py << ",";
        ss << pz << ",";

        czml += ss.str();

        string& czml_att = prop.czmls[sit->first + "att"];
        ss_att << std::setprecision(precision) << std::fixed;
        ss_att << "\n";
        ss_att << (86400.*(utc-prop.startutc)) << ",";

        // attitudes
        ss_att << qx << ",";
        ss_att << qy << ",";
        ss_att << qz << ",";
        ss_att << qw << ",";
        czml_att += ss_att.str();
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
                    "\"gltf\":\"./public/plugins/hsfl-orbit-display/img/HyTI.glb\","
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
                    "\"leadTime\": 6000,"
                    "\"trailTime\": 6000,"
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

// Calculate inter-nodal distances and other data, and send to telegraf
int32_t create_simdata(prop_unit& prop)
{
    const string node_1 = prop.nodes[0];
    double utc;
    utc = prop.sim.cnodes[node_1]->currentinfo.node.loc.pos.eci.utc;
    // node 1 pos
    double px1, py1, pz1;
    px1 = prop.sim.cnodes[node_1]->currentinfo.node.loc.pos.eci.s.col[0];
    py1 = prop.sim.cnodes[node_1]->currentinfo.node.loc.pos.eci.s.col[1];
    pz1 = prop.sim.cnodes[node_1]->currentinfo.node.loc.pos.eci.s.col[2];

    // node 1 attitude
    double qw1, qx1, qy1, qz1;
    qx1 = prop.sim.cnodes[node_1]->currentinfo.node.loc.att.geoc.s.d.x;
    qy1 = prop.sim.cnodes[node_1]->currentinfo.node.loc.att.geoc.s.d.y;
    qz1 = prop.sim.cnodes[node_1]->currentinfo.node.loc.att.geoc.s.d.z;
    qw1 = prop.sim.cnodes[node_1]->currentinfo.node.loc.att.geoc.s.w;


    // Hold json of sim data to send to telegraf
    json11::Json::object simdata = json11::Json::object {
        { "node_name", node_1 },
        { "utc", utc },
        { "node.loc.pos.eci.s.col[0]", px1 },
        { "node.loc.pos.eci.s.col[1]", py1 },
        { "node.loc.pos.eci.s.col[2]", pz1 },
    };
    for (string node_2 : prop.nodes)
    {
        // Don't calculate distance to itself
        if (node_1 == node_2)
        {
            continue;
        }
        
        // node 2 pos
        double px2, py2, pz2;
        px2 = prop.sim.cnodes[node_2]->currentinfo.node.loc.pos.eci.s.col[0];
        py2 = prop.sim.cnodes[node_2]->currentinfo.node.loc.pos.eci.s.col[1];
        pz2 = prop.sim.cnodes[node_2]->currentinfo.node.loc.pos.eci.s.col[2];

        // d = sqrt( dx^2 + dy^2 + dz^2 )
        double d;
        double dx, dy, dz;
        dx = px2 - px1;
        dy = py2 - py1;
        dz = pz2 - pz1;
        d = sqrt(dx*dx + dy*dy + dz*dz);
        
        // vlc model using calculated distance as input
        double snr = vlc_snr(d);

        // Attitude difference
        // todo
        double angle = 0;

        // VLC link budget
        double vlc_link_budget = vlc_snr(d);



        // Add additional json keys
        simdata["distanceto_" + node_2] = d;
        simdata["snr_" + node_2] = vlc_link_budget;
        

        // Worry about what to do with this later
        // cout << std::fixed;
        // cout << std::setprecision(8) << utc << " ";
        // cout << std::setprecision(precision) << node_1 << ":" << node_2 << " d:" << d << " snr:" << snr << endl;
    }
    json11::Json resp = simdata;
    // cout << resp.dump() << endl;
    int32_t iretn = socket_sendto(cosmos_web_telegraf_channel, resp.dump());

    return 0;
}

// -------------------------------------
// code for new vlc system
double degreesToRadians(double degrees)
{
    return degrees * M_PI / 180;
}

// compute integral for function pointer
double integral(double(*f)(double x), double a, double b, int n) {
    double step = (b - a) / n;  // width of each small rectangle
    double area = 0.0;  // signed area
    for (int i = 0; i < n; i ++) {
        area += f(a + (i + 0.5) * step) * step; // sum up each small rectangle
    }
    return area;
}

double IntBck(double x){
    return pow(x,3)/(exp(x)-1);
}

double vlc_snr(double distance) {
    
    // from Table IV
    //Semi-angle at Half Power, Φ1/2
    double semi_angle_at_half_power = 30; // deg
    double phi_ = degreesToRadians(semi_angle_at_half_power); // convert to radians

    //LED Peak Wavelength, λpeak
    double led_peak_wavelength = 656.2808; //nm
    led_peak_wavelength = led_peak_wavelength*1e-9; // convert to meters

    // Concentrator FoV Semi-angle, ψc
    double concentrator_fov_semi_angle = 35; //deg
    double psi_c = degreesToRadians(concentrator_fov_semi_angle); // convert to radians

    // Filter Transmission Coefficient (or gain), To or Ts
    double Ts = 1;

    // Irradiance Angle, ψ
    double irradiance_angle = 15; //deg //TODO: not being used
    // Detector Responsivity, gamma
    double detector_responsivity = 0.51;
    //double gamma = detector_responsivity;
    // Refractive Index of Lens, n
    double n = 1.5;

    //Radius of Concentrator, R
    double R = 2; //cm
    // Detector Active Area, Apd
    double Apd = 7.84; //cm^2
    Apd = Apd*1e-4; // convert to m^2
    // Desired Electrical Bandwidth, B
    double B = 0.5; //MHz
    B = B*1e6; // convert to Hz
    // Optical Filter Bandwidth, ∆λ
    double d_lambda = 0.4020; //nm
    d_lambda = 0.4020*1e-9; // convert to meters
    // Optical Filter Lower Limit, lambda_1
    double lambda_1 = 656.0798; //nm
    lambda_1 = lambda_1*1e-9; // convert to meters
    // Optical Filter Upper Limit, lambda_2
    double lambda_2 = 656.4818; //nm
    lambda_2 = lambda_2*1e-9; // convert to meters
    // Open Loop Voltage Gain,
    double G = 10;
    // FET Transconductance, gm
    double gm = 30; // ms
    gm = 30*1e-3; // convert to sec
    // FET Channel Noise Factor, gamma
    double gamma_large = 0.82; // or 1.5
    // Capacitance of Photodetector, etta
    double etta = 38; //pF /cm^2
    etta = etta*1e-8; // convert to Farad / m^2
    // Noise Bandwidth Factor for White Noise, I2
    double I2 = 0.562;
    //Noise Bandwidth Factor for f^2 noise, I3
    double I3 = 0.0868;
    // Boltzmann Constant, k
    double k = 1.380649e-23; //J/K
    // Absolute Temperature, TA
    double Ta = 300; //K

    // c is the speed of light,
    double c = 299792458.0; // m/sec
    // hp is Planck's constant
    double hp = 6.62607015e-34;
    // T is average temperature of the Sun's surface.
    double Tsun = 5778; // K


    // -------------- distance from cosmos simulation
    //double distance = 10.79707118;
    // -------------- time from cosmos simulation


    // The angle of incidence with respect to the receiver axis is ψ,
    // test for now with
    double psi = 0;

    // transmitted optical power
    double Pt = 5.6; //W

    // Incidence Angle, φ / transmitter perpendicular axis
    double incidence_angle = 0; //deg, was 30
    double phi = degreesToRadians(incidence_angle); // convert to radians

    double g_psi = 0;
    // concentrator gain, g(ψ)
    if (0 <= degreesToRadians(psi) and  degreesToRadians(psi) <= psi_c) {
        g_psi = (pow(n,2))/(pow(sin(psi_c),2));
    } else {
        g_psi = 0;
    }


    
    // Signal Model

    // received optical power, Pr
    // the channel DC gain, H0
    // lambertian emission, m
    double m = -log(2.)/log(cos(phi_));

    double Hl1 = (Apd*(m + 1))/(2.*M_PI*pow(distance,2));
    double Hl2 = (cos(psi))*(pow(cos(phi),m));


    //
    double H0;
   
    if (0 <= degreesToRadians(psi) and degreesToRadians(psi) <= psi_c) {
        H0 = g_psi*Hl1*Hl2*Ts;         // Channel  Gain;
        //std::cout << "test" << std::endl;
    } else {
        H0 = 0;
    }
    // std::cout << H0 << std::endl;

    double Pr = H0 * Pt;
    double S = detector_responsivity * Pr;
    S = S*S;



    // q is the electronic charge,
    double q = 1.60217662e-19; //coulombs
    double Rse = 150e9;                   // Distance between Sun and Earth
    double SAsun = 4*M_PI*pow(Rse,2);  // Surface area of Sun
    double day = 15;                     // Day of year

    double xmin = (hp * c)/(lambda_1*k*Tsun);
    double xmax = (hp * c)/(lambda_2*k*Tsun);

    // computation of the integral (eq 9)
    //IntBck = lambda y: np.power(y,3)/(np.exp(y)-1)     # Computing background noise produced by Sun
    double Bck_NI = integral(IntBck,xmin,xmax,10);

    double PwrArea = ((2*M_PI*pow(k,4)*pow(Tsun,4)*Bck_NI))/(pow(hp,3)*pow(c,2));
    double So = PwrArea/SAsun;

    double ESD = 1.00011 + 0.034211*cos(B) + 0.00128*sin(B) + 0.000719*cos(2*B) + 0.000077*sin(2*B);
    double Sod = So*ESD;
    double E_det = Sod;

    // background noise power detected by the optical receiver
    double Pbg = E_det*Ts*Apd*pow(n,2);

    //shot noise
    double sigma_s_2 = 2*q*detector_responsivity*(Pr + I2*Pbg)*B; // TODO: confirm 0.5 factor, fix Pbg

    double SoN = S/sigma_s_2; // Recieved power over shot noise
    double Snr = 10 * log10(SoN);

    // std::cout << Snr << std::endl;

    return Snr;
}
