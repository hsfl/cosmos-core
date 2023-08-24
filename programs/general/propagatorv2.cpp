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
static Convert::locstruc initialloc;
static double initialsep = 0.;
static double deltat = .0655;
static double runcount = 1500;
static double currentutc;
static double simdt = 1.;
static double attdt = 1.;
static double minaccelratio = 10;
constexpr double d2s = 1./86400.;
constexpr double d2s2 = 1./(86400.*86400.);
static string targetfile = "targets.dat";

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
    int32_t iretn = 0;

    string estring;
    json11::Json jargs = json11::Json::parse(argv[1], estring);
    if (!jargs["altprint"].is_null()) altprint = jargs["altprint"].bool_value();
    if (!jargs["shapetype"].is_null()) shapetype = jargs["shapetype"].number_value();
    if (!jargs["shapeseparation"].is_null()) shapeseparation = jargs["shapeseparation"].number_value();
    if (!jargs["runcount"].is_null()) runcount = jargs["runcount"].number_value();
    if (!jargs["speed"].is_null()) speed = jargs["speed"].number_value();
    if (!jargs["maxaccel"].is_null()) maxaccel = jargs["maxaccel"].number_value();
    if (!jargs["initialutc"].is_null()) initialutc = jargs["initialutc"].number_value();
    initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
    if (!jargs["phys"].is_null())
    {
        json11::Json::object values = jargs["phys"].object_items();
        initiallat = RADOF(values["lat"].number_value());
        initiallon = RADOF(values["lon"].number_value());
        initialalt = values["alt"].number_value();
        initialangle = RADOF(values["angle"].number_value());
        initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
    }
    if (!jargs["eci"].is_null())
    {
        json11::Json::object values = jargs["eci"].object_items();
        initialloc.pos.eci.s.col[0] = (values["x"].number_value());
        initialloc.pos.eci.s.col[1] = (values["y"].number_value());
        initialloc.pos.eci.s.col[2] = (values["z"].number_value());
        initialloc.pos.eci.v.col[0] = (values["vx"].number_value());
        initialloc.pos.eci.v.col[1] = (values["vy"].number_value());
        initialloc.pos.eci.v.col[2] = (values["vz"].number_value());
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
    }
    if (!jargs["kep"].is_null())
    {
        json11::Json::object values = jargs["kep"].object_items();
        kepstruc kep;
        kep.utc = initialutc;
        kep.ea = values["ea"].number_value();
        kep.i = values["i"].number_value();
        kep.ap = values["ap"].number_value();
        kep.raan = values["raan"].number_value();
        kep.e = values["e"].number_value();
        kep.a = values["a"].number_value();
        kep2eci(kep, initialloc.pos.eci);
        initialloc.pos.eci.pass++;
        pos_eci(initialloc);
    }
    if (!jargs["initialsep"].is_null()) initialsep = jargs["initialsep"].number_value();
    if (!jargs["deltat"].is_null()) deltat = jargs["deltat"].number_value();
    if (!jargs["simdt"].is_null()) simdt = jargs["simdt"].number_value();
    if (!jargs["attdt"].is_null()) attdt = jargs["attdt"].number_value();
    if (!jargs["minaccelratio"].is_null()) minaccelratio = jargs["minaccelratio"].number_value();
    if (!jargs["targetfile"].is_null()) targetfile = jargs["targetfile"].string_value();

    // initialize simulation agent
    agent = new Agent("", "", "", 0.);
    agent->set_debug_level(0);

    // initialize simulator object
    sim = new Physics::Simulator();
    iretn = sim->GetError();
    if (iretn <0) {
        agent->debug_log.Printf("Error Creating Simulator: %s\n", cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    currentutc = initialutc;
    sim->Init(currentutc, simdt);

//    iretn = sim->AddNode("mother", Physics::Structure::U12, Physics::Propagator::PositionGaussJackson, Physics::Propagator::AttitudeInertial, Physics::Propagator::Thermal, Physics::Propagator::Electrical, initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
    initialloc.att.icrf.s = q_eye();
    iretn = sim->AddNode("mother", Physics::Structure::HEX65W80H, Physics::Propagator::PositionTle, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, Physics::Propagator::OrbitalEvent, initialloc.pos.eci, initialloc.att.icrf);
    sit = sim->GetNode("mother");
    FILE *fp = fopen(targetfile.c_str(), "r");
    if (fp == nullptr)
    {
        agent->debug_log.Printf("Bad Target File: %s\n", targetfile.c_str());
        exit(1);
    }
    string line;
    line.resize(200);
    while (fgets((char *)line.data(), 200, fp) != nullptr)
    {
        vector<string> args = string_split(line);
        if (args.size() == 4)
        {
            (*sit)->AddTarget(args[0], RADOF(stof(args[1])), RADOF(stod(args[2])), 0., stod(args[3]), NODE_TYPE_GROUNDSTATION);
        }
        else if (args.size() == 5)
        {
            (*sit)->AddTarget(args[0], RADOF(stof(args[1])), RADOF(stod(args[2])), RADOF(stof(args[3])), RADOF(stod(args[4])));
        }
    }
//    (*sit)->AddTarget("Punta_Arenas", RADOF(-53.1638), RADOF(-70.9171), 0.);
//    (*sit)->AddTarget("Awarua", RADOF(-46.4923), RADOF(168.2808), 0.);
//    (*sit)->AddTarget("Puertollano", RADOF(38.6884), RADOF(4.1079), 0.);
//    (*sit)->AddTarget("Svalbard", RADOF(77.8750), RADOF(20.9752), 0.);
//    (*sit)->AddTarget("CONUS", RADOF(48.8719), RADOF(-124.0436), RADOF(26.9051), RADOF(-66.3209));
//    (*sit)->AddTarget("Sahara", RADOF(32.4981), RADOF(-1.1719), RADOF(15.9423), RADOF(31.7363));
//    (*sit)->AddTarget("Greenland", RADOF(80.7336), RADOF(-54.8762), RADOF(73.8102), RADOF(-31.8558));

    string header;
    header += "Name\tSeconds\tUTC\t";

    header += "Alt\tEcc\tInc\tRAAN\tPeriod\tBeta\t";

    header += "pos_lon\tpos_lat\tpos_h\t";
    header += "sun_lat\tsun_lon\t";
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

    for (uint16_t id=0; id<(*sit)->currentinfo.target.size(); ++id)
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
    string path = data_name(mjd, "txt", "", "", "orbit");
    FILE *ofp = fopen(path.c_str(), "w");
    fprintf(ofp, "%s\n", header.c_str());


    // Targets
    header = "Seconds\t";
    header += "UTC\t";
    header += "Name\t";
    header += "Type\t";

    header += "Latitude\t";
    header += "Longitude\t";

    header += "Range\t";
    header += "Close\t";
    header += "AzTo\t";
    header += "ElTo\t";
    header += "AzFrom\t";
    header += "ElFrom\t";

    path = data_name(mjd, "txt", "", "", "event");
    FILE *efp = fopen(path.c_str(), "w");
    fprintf(efp, "%s\n", header.c_str());

    double elapsed = 0;
    double pcount = 0.;
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
        lastloc = (*sit)->currentinfo.node.loc;
        lasttargets = (*sit)->currentinfo.target;
        sim->Propagate();
        pcount += simdt;

        stloc.pos.eci.s = rv_sub((*sit)->currentinfo.target[0].loc.pos.eci.s, (*sit)->currentinfo.node.loc.pos.eci.s);

        Vector eci_z = Vector(stloc.pos.eci.s);

        // Desired Y is cross product of Desired Z and velocity vector
        Vector eci_y = eci_z.cross(Vector((*sit)->currentinfo.node.loc.pos.eci.v));

        tatt.utc = (*sit)->currentinfo.node.loc.pos.eci.utc + simdt;
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
        //                satt = (*sit)->currentinfo.node.loc.att.icrf;
        //                (*sit)->currentinfo.node.phys.ftorque = rv_smult(1., calc_control_torque_b(tatt, satt, (*sit)->currentinfo.node.phys.moi, 1.).to_rv());
        (*sit)->currentinfo.node.loc.att.icrf = tatt;

        string output;

        if (pcount > runcount / 100000.)
        {
            output += (*sit)->currentinfo.node.name + "\t";
            output += to_floatany(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc - initialutc)) + "\t";
            output += to_mjd((*sit)->currentinfo.node.loc.pos.eci.utc) + "\t";

            // Keplerian
            kepstruc kep;
            Convert::eci2kep((*sit)->currentinfo.node.loc.pos.eci, kep);
            output += to_floatany(kep.a-REARTHM) + "\t";
            output += to_floatany(kep.e) + "\t";
            output += to_floatany(kep.i) + "\t";

            output += to_floatany(kep.raan) + "\t";
            output += to_floatany(kep.period) + "\t";
            output += to_floatany(kep.beta) + "\t";

            // Geod

            output += to_floatany((*sit)->currentinfo.node.loc.pos.geod.s.lat) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.geod.s.lon) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.geod.s.h) + "\t";

//            cartpos geocg;
//            geoidpos geodg = (*sit)->currentinfo.node.loc.pos.geod;
//            geodg.s.h = 0.;
//            geod2geoc(geodg, geocg);
//            rvector topog;
//            geoc2topo(geodg.s, (*sit)->currentinfo.node.loc.pos.geoc.s, topog);
//            float azg;
//            float elg;
//            topo2azel(topog, azg, elg);
            output += to_floatany((*sit)->currentinfo.node.loc.pos.extra.sungeo.lat) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.extra.sungeo.lon) + "\t";

            // ECI

            output += to_floatany((*sit)->currentinfo.node.loc.pos.eci.s.col[0]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.eci.s.col[1]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.eci.s.col[2]) + "\t";

            output += to_floatany((*sit)->currentinfo.node.loc.pos.eci.v.col[0]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.eci.v.col[1]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.eci.v.col[2]) + "\t";

            output += to_floatany((*sit)->currentinfo.node.loc.pos.eci.a.col[0]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.eci.a.col[1]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.eci.a.col[2]) + "\t";

            // attitudes

            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.s.w) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.s.d.x) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.s.d.y) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.s.d.z) + "\t";

            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.v.col[0]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.v.col[1]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.v.col[2]) + "\t";

            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.a.col[0]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.a.col[1]) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.att.icrf.a.col[2]) + "\t";

            output += to_floatany((*sit)->currentinfo.node.phys.temp) + "\t";
            output += to_floatany((*sit)->currentinfo.node.phys.powgen) + "\t";

            for (targetstruc &target : (*sit)->currentinfo.target)
            {
                output += (target.name) + "\t";
                output += to_floatany(target.loc.pos.geod.s.lat) + "\t";
                output += to_floatany(target.loc.pos.geod.s.lon) + "\t";
                output += to_floatany(target.loc.pos.geod.s.h) + "\t";
                output += to_floatany(target.range) + "\t";
                output += to_floatany(target.close) + "\t";
                output += to_floatany(target.azto) + "\t";
                output += to_floatany(target.elto) + "\t";
            }

            fprintf(ofp, "%s\n", output.c_str());
            pcount = 0.;
        }

        // Check events
        // Eclipse
        if (lastloc.pos.sunradiance && !(*sit)->currentinfo.node.loc.pos.sunradiance)
        {
            output = to_floatany(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc - initialutc)) + "\t";
            output += to_mjd((*sit)->currentinfo.node.loc.pos.eci.utc) + "\t";

            output += "Sun\tECLIPSE_IN\t";

            output += to_floatany((*sit)->currentinfo.node.loc.pos.geod.s.lat) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.geod.s.lon) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.geod.s.h) + "\t";

            fprintf(efp, "%s\n", output.c_str());
        }

        if (!lastloc.pos.sunradiance && (*sit)->currentinfo.node.loc.pos.sunradiance)
        {
            output = to_floatany(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc - initialutc)) + "\t";
            output += to_mjd((*sit)->currentinfo.node.loc.pos.eci.utc) + "\t";

            output += "Sun\tECLIPSE_OUT\t";

            output += to_floatany((*sit)->currentinfo.node.loc.pos.geod.s.lat) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.geod.s.lon) + "\t";
            output += to_floatany((*sit)->currentinfo.node.loc.pos.geod.s.h) + "\t";

            fprintf(efp, "%s\n", output.c_str());
        }

        // Heading and Angular Velocity
        double cphi = cos((*sit)->currentinfo.node.loc.pos.geos.v.phi);
        double cphi2 = cphi * cphi;
        double phi2 = (*sit)->currentinfo.node.loc.pos.geos.v.phi * (*sit)->currentinfo.node.loc.pos.geos.v.phi;
        double lambda2 = (*sit)->currentinfo.node.loc.pos.geos.v.lambda * (*sit)->currentinfo.node.loc.pos.geos.v.lambda;
        double anglev;
        double heading;
        if (cphi2 != 0.)
        {
            anglev = sqrt(phi2 + lambda2 / cphi2);
            heading = atan2((*sit)->currentinfo.node.loc.pos.geos.v.phi, (*sit)->currentinfo.node.loc.pos.geos.v.lambda / cphi);
        }
        else
        {
            anglev = sqrt(phi2);
            heading = DPI;
        }

        for (uint16_t id=0; id<(*sit)->currentinfo.target.size(); ++id)
        {
            // Check ahead for target
            double cd = cos(heading - (*sit)->currentinfo.target[id].bearing);
            if (cd > .8)
            {
                double atime = (*sit)->currentinfo.target[id].distance / (anglev * cd);
                double latime = lasttargets[id].distance / (anglev * cd);
                if ((latime >= 600.8 && atime < 600.9) || (latime >= 300.8 && atime <= 300.9))
                {
                    output.clear();
                    output += to_floatany(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc - initialutc)) + "\t";
                    output += to_mjd((*sit)->currentinfo.node.loc.pos.eci.utc) + "\t";
                    output += (*sit)->currentinfo.target[id].name + "\t";

                    if ((*sit)->currentinfo.target[id].type == NODE_TYPE_GROUNDSTATION)
                    {
                        output += "GS_PREP_SECONDS-" + to_unsigned(atime, 3, true) + "\t";
                    }
                    else
                    {
                        output += "IMAGE_PREP_SECONDS-" + to_unsigned(atime, 3, true) + "\t";
                    }

                    output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lat) + "\t";
                    output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lon) + "\t";
                    output += to_floatany((*sit)->currentinfo.target[id].range) + "\t";
                    output += to_floatany((*sit)->currentinfo.target[id].close) + "\t";
                    output += to_floatany((*sit)->currentinfo.target[id].azto) + "\t";
                    output += to_floatany((*sit)->currentinfo.target[id].elto) + "\t";
                    output += to_floatany((*sit)->currentinfo.target[id].azfrom) + "\t";
                    output += to_floatany((*sit)->currentinfo.target[id].elfrom) + "\t";

                    fprintf(efp, "%s\n", output.c_str());
                }
            }

            //Check start of image
            if (lasttargets[id].min != 2. && (*sit)->currentinfo.target[id].min == 2.)
            {
                output.clear();
                output += to_floatany(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc - initialutc)) + "\t";
                output += to_mjd((*sit)->currentinfo.node.loc.pos.eci.utc) + "\t";
                output += (*sit)->currentinfo.target[id].name + "\t";

                output += "IMAGE_START\t";

                output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lat) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lon) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].range) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].close) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].azto) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].elto) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].azfrom) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].elfrom) + "\t";

                fprintf(efp, "%s\n", output.c_str());
            }

            //Check stop of image
            if (lasttargets[id].min == 2. && (*sit)->currentinfo.target[id].min != 2.)
            {
                output.clear();
                output += to_floatany(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc - initialutc)) + "\t";
                output += to_mjd((*sit)->currentinfo.node.loc.pos.eci.utc) + "\t";
                output += (*sit)->currentinfo.target[id].name + "\t";

                output += "IMAGE_STOP+" + to_unsigned(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc-lasttargets[id].utc), 3, true) + "\t";

                output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lat) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lon) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].range) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].close) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].azto) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].elto) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].azfrom) + "\t";
                output += to_floatany((*sit)->currentinfo.target[id].elfrom) + "\t";

                fprintf(efp, "%s\n", output.c_str());
            }

            // If Ground Station, Check current elevation
            if ((*sit)->currentinfo.target[id].type == NODE_TYPE_GROUNDSTATION)
            {
                for (float el = RADOF(0.); el < RADOF(90.); el += RADOF(15.))
                {
                    if ((*sit)->currentinfo.target[id].elto >= el &&  lasttargets[id].elto <= el)
                    {
                        output.clear();
                        output += to_floatany(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc - initialutc)) + "\t";
                        output += to_mjd((*sit)->currentinfo.node.loc.pos.eci.utc) + "\t";
                        output += (*sit)->currentinfo.target[id].name + "\t";

                        output += "GS_ELEVATION_" + to_unsigned(DEGOF(el), 2, true) + "\t";

                        output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lat) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lon) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].range) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].close) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].azto) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].elto) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].azfrom) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].elfrom) + "\t";

                        fprintf(efp, "%s\n", output.c_str());
                    }
                }

                // If Groundstation, check max elevation
                if ((*sit)->currentinfo.target[id].maxelto > 0. && (*sit)->currentinfo.target[id].maxelto > (*sit)->currentinfo.target[id].elto && lasttargets[id].maxelto == lasttargets[id].elto)
                {
                    output.clear();
                    output += to_floatany(86400.*(lasttargets[id].utc - initialutc)) + "\t";
                    output += to_mjd(lasttargets[id].utc) + "\t";
                    output += lasttargets[id].name + "\t";

                    output += "GS_MAX_" + to_unsigned(DEGOF(lasttargets[id].maxelto), 2, true) + "\t";

                    output += to_floatany(lasttargets[id].loc.pos.geod.s.lat) + "\t";
                    output += to_floatany(lasttargets[id].loc.pos.geod.s.lon) + "\t";
                    output += to_floatany(lasttargets[id].range) + "\t";
                    output += to_floatany(lasttargets[id].close) + "\t";
                    output += to_floatany(lasttargets[id].azto) + "\t";
                    output += to_floatany(lasttargets[id].elto) + "\t";
                    output += to_floatany(lasttargets[id].azfrom) + "\t";
                    output += to_floatany(lasttargets[id].elfrom) + "\t";

                    fprintf(efp, "%s\n", output.c_str());
                }
            }

            // Check image progress
            else if ((*sit)->currentinfo.target[id].min == 2.)
            {
                for (int32_t dtime=100; dtime<900; dtime+=100)
                {
                    if (int(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc-lasttargets[id].utc)) == dtime)
                    {
                        output.clear();
                        output += to_floatany(86400.*((*sit)->currentinfo.node.loc.pos.eci.utc - initialutc)) + "\t";
                        output += to_mjd((*sit)->currentinfo.node.loc.pos.eci.utc) + "\t";
                        output += (*sit)->currentinfo.target[id].name + "\t";

                        output += "IMAGE_ELAPSED_SECONDS+" + to_unsigned(dtime, 3, true) + "\t";

                        output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lat) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].loc.pos.geod.s.lon) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].range) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].close) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].azto) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].elto) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].azfrom) + "\t";
                        output += to_floatany((*sit)->currentinfo.target[id].elfrom) + "\t";

                        fprintf(efp, "%s\n", output.c_str());
                    }
                }
            }
        }

        ++elapsed;
    }

    fclose(ofp);
    fclose(efp);
    agent->shutdown();
}
