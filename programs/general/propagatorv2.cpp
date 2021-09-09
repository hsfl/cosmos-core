#include "physics/simulatorclass.h"
#include "agent/agentclass.h"
#include "support/jsonclass.h"
#include "support/datalib.h"

using namespace Convert;

Vector calc_control_torque_b(Convert::qatt tatt, Convert::qatt catt, Vector moi, double portion);

static Physics::Simulator::StateList::iterator sit;
static Physics::Simulator *sim;
static Agent *agent;
static double speed=1.;
static double maxaccel = .1;
static double minaccel;
static double initialutc = 59270.949409722227;
static double initiallat = RADOF(21.3069);
static double initiallon = RADOF(-157.8583);
static double initialalt = 400000.;
static double initialangle = RADOF(54.);
static double initialsep = 0.;
static double deltat = .0655;
static double runcount = 1500;
static double currentutc;
static double simdt = 1.;
static double attdt = 1.;
static double minaccelratio = 10;
constexpr double d2s = 1./86400.;
constexpr double d2s2 = 1./(86400.*86400.);

// 0 : string of pearls
// 1 : surrounding diamond
// 2 : reverse string of pearls
// 3 : high/low attraction experiment
static uint8_t shapetype = 0;
static double shapeseparation = 500.;
static LsFit tattfit;
static LsFit omegafit;
static bool altprint = false;

int main(int argc, char *argv[])
{
    int32_t iretn;

    string estring;
    json11::Json jargs = json11::Json::parse(argv[1], estring);
    if (!jargs["altprint"].is_null()) altprint = jargs["altprint"].bool_value();
    if (!jargs["shapetype"].is_null()) shapetype = jargs["shapetype"].number_value();
    if (!jargs["shapeseparation"].is_null()) shapeseparation = jargs["shapeseparation"].number_value();
    if (!jargs["runcount"].is_null()) runcount = jargs["runcount"].number_value();
    if (!jargs["speed"].is_null()) speed = jargs["speed"].number_value();
    if (!jargs["maxaccel"].is_null()) maxaccel = jargs["maxaccel"].number_value();
    if (!jargs["initialutc"].is_null()) initialutc = jargs["initialutc"].number_value();
    if (!jargs["initiallat"].is_null()) initiallat = RADOF(jargs["initiallat"].number_value());
    if (!jargs["initiallon"].is_null()) initiallon = RADOF(jargs["initiallon"].number_value());
    if (!jargs["initialalt"].is_null()) initialalt = jargs["initialalt"].number_value();
    if (!jargs["initialangle"].is_null()) initialangle = RADOF(jargs["initialangle"].number_value());
    if (!jargs["initialsep"].is_null()) initialsep = jargs["initialsep"].number_value();
    if (!jargs["deltat"].is_null()) deltat = jargs["deltat"].number_value();
    if (!jargs["simdt"].is_null()) simdt = jargs["simdt"].number_value();
    if (!jargs["attdt"].is_null()) attdt = jargs["attdt"].number_value();
    if (!jargs["minaccelratio"].is_null()) minaccelratio = jargs["minaccelratio"].number_value();

    // initialize simulation agent
    agent = new Agent("", "", 0.);
    agent->set_debug_level(0);

    // initialize simulator object
    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn <0) {
        agent->debug_error.Printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    currentutc = initialutc;
    sim->Init(currentutc, simdt);

    iretn = sim->AddNode("mother", Physics::Structure::U12, Physics::Propagator::PositionGaussJackson, Physics::Propagator::AttitudeInertial, Physics::Propagator::Thermal, Physics::Propagator::Electrical, initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
    sit = sim->GetNode("mother");
    sit->second->AddTarget("Punta Arenas", RADOF(-53.1638), RADOF(-70.9171), 0.);
    sit->second->AddTarget("Awarua", RADOF(-46.4923), RADOF(168.2808), 0.);
    sit->second->AddTarget("Puertollano", RADOF(38.6884), RADOF(4.1079), 0.);
    sit->second->AddTarget("Svalbard", RADOF(77.8750), RADOF(20.9752), 0.);
    sit->second->AddTarget("CONUS", RADOF(48.8719), RADOF(-124.0436), RADOF(26.9051), RADOF(-66.3209));
    sit->second->AddTarget("Sahara", RADOF(32.4981), RADOF(-1.1719), RADOF(15.9423), RADOF(31.7363));
    sit->second->AddTarget("Greenland", RADOF(80.7336), RADOF(-54.8762), RADOF(73.8102), RADOF(-31.8558));

    string header;
    header += "Name\tSeconds\tUTC\t";

    header += "pos_lon\tpos_lat\tpos_h\t";
    header += "pos_x\tpos_y\tpos_z\t";
    header += "vel_x\tvel_y\tvel_z\t";
    header += "acc_x\tacc_y\tacc_z\t";

    header += "att_w\tatt_x\tatt_y\tatt_z\t";
    header += "omega_x\tomega_y\tomega_z\t";
    header += "alpha_x\talpha_y\talpha_z\t";

//    header += "thrust_x\tthrust_y\tthrust_z\t";
//    header += "impulse_x\timpulse_y\timpulse_z\t";
//    header += "torque_x\ttorque_y\ttorque_z\t";

    header += "temperature\tpowgen\t";

    for (uint16_t id=0; id<sit->second->targets.size(); ++id)
    {
        header += "target_name_" + to_unsigned(id, 0) + "\t";

        header += "target_lat_" + to_unsigned(id, 0) + "\t";
        header += "target_lon_" + to_unsigned(id, 0) + "\t";
        header += "target_h_" + to_unsigned(id, 0) + "\t";

        header += "target_range_" + to_unsigned(id, 0) + "\t";
        header += "target_close_" + to_unsigned(id, 0) + "\t";
        header += "target_azto_" + to_unsigned(id, 0) + "\t";
        header += "target_elto_" + to_unsigned(id, 0) + "\t";

    }

    double mjd = currentmjd();
    string path = data_name("", mjd, "orbit", "txt");
    FILE *ofp = fopen(path.c_str(), "w");
    path = data_name("", mjd, "event", "txt");
    FILE *efp = fopen(path.c_str(), "w");

    fprintf(ofp, "%s\n", header.c_str());

    double elapsed = 0;
    Vector impulse;
    minaccel = maxaccel / minaccelratio;
    locstruc lastloc;
    vector<targetstruc> lasttargets;
    while (agent->running() && elapsed < runcount)
    {
        // Attitude adjustment
        // Desired attitude comes from aligning satellite Z with desired Z and satellite Y with desired Y
        qatt tatt;
        qatt satt;
        locstruc stloc;

        // update states information for all nodes
        lastloc = sit->second->currentinfo.node.loc;
        lasttargets = sit->second->targets;
        sim->Propagate();

        stloc.pos.eci.s = rv_sub(sit->second->targets[0].loc.pos.eci.s, sit->second->currentinfo.node.loc.pos.eci.s);

        Vector eci_z = Vector(stloc.pos.eci.s);

        // Desired Y is cross product of Desired Z and velocity vector
        Vector eci_y = eci_z.cross(Vector(sit->second->currentinfo.node.loc.pos.eci.v));

        tatt.utc = sit->second->currentinfo.node.loc.pos.eci.utc + simdt;
        tatt.s = irotate_for(eci_y, eci_z, unityV(), unitzV()).to_q();
        rvector test;
        test  = rv_smult(eci_y.norm(), drotate(tatt.s, rv_unity()));
        test  = rv_smult(eci_z.norm(), drotate(tatt.s, rv_unitz()));
        tattfit.update(tatt.utc, tatt.s);
        Quaternion dqs = Quaternion(q_smult(d2s, tattfit.slopequaternion(tatt.utc)));
        Quaternion omega = 2. * dqs * Quaternion(tatt.s).conjugate();
        tatt.v.col[0] = omega.x;
        tatt.v.col[1] = omega.y;
        tatt.v.col[2] = omega.z;
        omegafit.update(tatt.utc, tatt.v);
        tatt.a = rv_smult(d2s, omegafit.slopervector(tatt.utc));

        // Determine full rotation of Desired into Satellite
        //                satt = sit->second->currentinfo.node.loc.att.icrf;
        //                sit->second->currentinfo.node.phys.ftorque = rv_smult(1., calc_control_torque_b(tatt, satt, sit->second->currentinfo.node.phys.moi, 1.).to_rv());
        sit->second->currentinfo.node.loc.att.icrf = tatt;

        string output;

        output += to_floatany(86400.*(sit->second->currentinfo.node.loc.pos.eci.utc - initialutc));
        output += to_mjd(sit->second->currentinfo.node.loc.pos.eci.utc) + "\t";

        output += sit->second->currentinfo.node.name + "\t";

        output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lat) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lon) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.h) + "\t";

        output += to_floatany(sit->second->currentinfo.node.loc.pos.eci.s.col[0]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.pos.eci.s.col[1]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.pos.eci.s.col[2]) + "\t";

        output += to_floatany(sit->second->currentinfo.node.loc.pos.eci.v.col[0]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.pos.eci.v.col[1]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.pos.eci.v.col[2]) + "\t";

        output += to_floatany(sit->second->currentinfo.node.loc.pos.eci.a.col[0]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.pos.eci.a.col[1]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.pos.eci.a.col[2]) + "\t";

        // attitudes

        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.s.w) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.s.d.x) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.s.d.y) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.s.d.z) + "\t";

        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.v.col[0]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.v.col[1]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.v.col[2]) + "\t";

        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.a.col[0]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.a.col[1]) + "\t";
        output += to_floatany(sit->second->currentinfo.node.loc.att.icrf.a.col[2]) + "\t";

        output += to_floatany(sit->second->currentinfo.node.phys.temp) + "\t";
        output += to_floatany(sit->second->currentinfo.node.phys.powgen) + "\t";

        for (uint16_t id=0; id<sit->second->targets.size(); ++id)
        {
            output += (sit->second->targets[id].name) + "\t";
            output += to_floatany(sit->second->targets[id].loc.pos.geod.s.lat) + "\t";
            output += to_floatany(sit->second->targets[id].loc.pos.geod.s.lon) + "\t";
            output += to_floatany(sit->second->targets[id].loc.pos.geod.s.h) + "\t";
            output += to_floatany(sit->second->targets[id].range) + "\t";
            output += to_floatany(sit->second->targets[id].close) + "\t";
            output += to_floatany(sit->second->targets[id].azto) + "\t";
            output += to_floatany(sit->second->targets[id].elto) + "\t";
        }

        fprintf(ofp, "%s\n", output.c_str());

        // Check events
        // Eclipse
        if (lastloc.pos.sunradiance && !sit->second->currentinfo.node.loc.pos.sunradiance)
        {
            output = to_floatany(86400.*(sit->second->currentinfo.node.loc.pos.eci.utc - initialutc));
            output += to_mjd(sit->second->currentinfo.node.loc.pos.eci.utc) + "\t";

            output += "ECLIPSEA\t";

            output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lat) + "\t";
            output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lon) + "\t";
            output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.h) + "\t";

            fprintf(efp, "%s\n", output.c_str());
        }

        if (!lastloc.pos.sunradiance && sit->second->currentinfo.node.loc.pos.sunradiance)
        {
            output = to_floatany(86400.*(sit->second->currentinfo.node.loc.pos.eci.utc - initialutc));
            output += to_mjd(sit->second->currentinfo.node.loc.pos.eci.utc) + "\t";

            output += "ECLIPSEL\t";

            output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lat) + "\t";
            output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lon) + "\t";
            output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.h) + "\t";

            fprintf(efp, "%s\n", output.c_str());
        }

        // Targets
        for (uint16_t id=0; id<sit->second->targets.size(); ++id)
        {
            if (sit->second->targets[id].close <= 0 && lasttargets[id].close > 0)
            {
                output = to_floatany(86400.*(sit->second->currentinfo.node.loc.pos.eci.utc - initialutc));
                output += to_mjd(sit->second->currentinfo.node.loc.pos.eci.utc) + "\t";

                output += sit->second->targets[id].name + "\t";
                if (sit->second->targets[id].min < 0.)
                {
                    output += "TARGETAPPROACHLOW_" + to_unsigned(id, 0) + "\t";
                }
                else if (sit->second->targets[id].min > 0.)
                {
                    output += "TARGETAPPROACHHIGH_" + to_unsigned(id, 0) + "\t";
                }
                else
                {
                    output += "TARGETMID_" + to_unsigned(id, 0) + "\t";
                }

                output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lat) + "\t";
                output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lon) + "\t";
                output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.h) + "\t";

                fprintf(efp, "%s\n", output.c_str());
            }

            if (sit->second->targets[id].close >= 0 && lasttargets[id].close < 0)
            {
                output = to_floatany(86400.*(sit->second->currentinfo.node.loc.pos.eci.utc - initialutc));
                output += to_mjd(sit->second->currentinfo.node.loc.pos.eci.utc) + "\t";

                output += sit->second->targets[id].name + "\t";
                if (sit->second->targets[id].min < 0.)
                {
                    output += "TARGETLEAVELOW_" + to_unsigned(id, 0) + "\t";
                }
                else if (sit->second->targets[id].min > 0.)
                {
                    output += "TARGETLEAVEHIGH_" + to_unsigned(id, 0) + "\t";
                }
                else
                {
                    output += "TARGETMID_" + to_unsigned(id, 0) + "\t";
                }

                output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lat) + "\t";
                output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.lon) + "\t";
                output += to_floatany(sit->second->currentinfo.node.loc.pos.geod.s.h) + "\t";

                fprintf(efp, "%s\n", output.c_str());
            }
        }

        ++elapsed;
    }

    fclose(ofp);
    fclose(efp);
    agent->shutdown();
}
