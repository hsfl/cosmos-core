#include "simulatorclass.h"
#include "support/jsonclass.h"
#include "support/stringlib.h"

namespace Cosmos
{
namespace Physics
{
int32_t Simulator::Init(double idt, string realm, double iutc)
{
    realmname = realm;
    initialutc = iutc;
    dt = idt;
    dtj = dt / 86400.;
//    if (initialutc > 3600.)
//    {
//        currentutc = initialutc;
//        offsetutc = initialutc - currentmjd();
//        dt = 86400.*((initialutc + (dt / 86400.))-initialutc);
//        dtj = dt / 86400.;
//    }

    RunState = State::Paused;
    if (server)
    {

    }
    error = 0;
    return error;
}

bool compareByPriority(const Physics::State* left, const Physics::State* right)
{
    return left->propagation_priority < right->propagation_priority;
}

//! @brief Add a detector to the list of available detectors in the Sim.
//! @param fov Full Field of View of the detector in radians.
//! @param ifov Instrument FOV (single pixel) in radians.
//! @param specmin Lower wavelength of spectral coverage, in meters.
//! @param specmax Upper wavelength of spectral coverage, in meters.
//! @return New number of detectors, or negative error.
int32_t Simulator::AddDetector(string name, float fov, float ifov, float specmin, float specmax)
{
    camstruc det;
    det.name = name;
    det.fov = fov;
    det.ifov = ifov;
    det.specmin = specmin;
    det.specmax = specmax;
    det.state = 0;
    return AddDetector(det);
}

int32_t Simulator::AddDetector(camstruc& det)
{
    if (detectors.find(det.name) == detectors.end())
    {
        detectors[det.name] = det;
    }
    return detectors.size();

}

//! @brief Add a target to the list of available targets in the Sim.
//! @param name Unique target name.
//! @param loc Location as ::Convert::locstruc.
//! @param type Node type.
//! @param size Size as ::gvector.
//! @return Current size of target list, or negative error.

int32_t Simulator::AddTarget(std::string name, locstruc loc, NODE_TYPE type, gvector size)
{
    targetstruc ttarget;
    ttarget.type = type;
    ttarget.name = name;
//    ttarget.cloc = loc;
    ttarget.area = 0.;
    ttarget.size = size;
    ttarget.loc = loc;

    return AddTarget(ttarget);
}

//! @brief Add a target to the list of available targets in the Sim.
//! @param name Unique target name.
//! @param loc Location as ::Convert::locstruc.
//! @param type Node type.
//! @param area Area of target, in square meters.
//! @return Current size of target list, or negative error.

int32_t Simulator::AddTarget(std::string name, locstruc loc, NODE_TYPE type, double area)
{
    targetstruc ttarget;
    ttarget.type = type;
    ttarget.name = name;
//    ttarget.cloc = loc;
    ttarget.size = gvector();
    ttarget.area  = area;
    ttarget.loc = loc;

    return AddTarget(ttarget);
}

//! @brief Add a target to the list of available targets in the Sim.
//! @param name Unique target name.
//! @param lat Central latitude of target, in radians.
//! @param lon Central longitude of target, in radians.
//! @param alt Central altitude of target, in meters.
//! @param type Node type.
//! @return Current size of target list, or negative error.

int32_t Simulator::AddTarget(string name, double lat, double lon, double alt, NODE_TYPE type)
{
    return AddTarget(name, lat, lon, DPI * 1e6, alt, type);
}

//! @brief Add a target to the list of available targets in the Sim.
//! @param name Unique target name.
//! @param lat Central latitude of target, in radians.
//! @param lon Central longitude of target, in radians.
//! @param alt Central altitude of target, in meters.
//! @param area Area of target, in square meters.
//! @param type Node type.
//! @return Current size of target list, or negative error.

int32_t Simulator::AddTarget(string name, double lat, double lon, double area, double alt, NODE_TYPE type)
{
    locstruc loc;
    loc.pos.geod.pass = 1;
    loc.pos.geod.utc = currentutc;
    loc.pos.geod.s.lat = lat;
    loc.pos.geod.s.lon = lon;
    loc.pos.geod.s.h = alt;
    loc.pos.geod.v = gv_zero();
    loc.pos.geod.a = gv_zero();
    loc.pos.geod.pass++;
    pos_geod(loc);
    return AddTarget(name, loc, type, area);
}

//! @brief Add a target to the list of available targets in the Sim.
//! @param name Unique target name.
//! @param ullat Upper left latitude of target area.
//! @param ullon Upper left longitude of target area.
//! @param lrlat Lower right latitude of target area.
//! @param lrlon Lower right longitude of target area.
//! @param type Node type.
//! @return Current size of target list, or negative error.

int32_t Simulator::AddTarget(string name, double ullat, double ullon, double lrlat, double lrlon, double alt, NODE_TYPE type)
{
    locstruc loc;
    loc.pos.geod.pass = 1;
    loc.pos.geod.utc = currentutc;
    loc.pos.geod.s.lat = (ullat + lrlat) / 2.;
    loc.pos.geod.s.lon = (ullon + lrlon) / 2.;
    loc.pos.geod.s.h = alt;
    loc.pos.geod.v = gv_zero();
    loc.pos.geod.a = gv_zero();
    loc.pos.geod.pass++;
    pos_geod(loc);
    double area = (ullat-lrlat) * (cos(lrlon) * lrlon - cos(ullon) * ullon) * REARTHM * REARTHM;
    return AddTarget(name, loc, type, area);
}

//! @brief Add a target to the list of available targets in the Sim.
//! @param targ Complete target as ::targetstruc.
//! @return Current size of target list, or negative error.

int32_t Simulator::AddTarget(targetstruc& targ)
{
    if (targets.find(targ.name) == targets.end())
    {
        targets[targ.name] = targ;
        for (auto &state : cnodes)
        {
            state->currentinfo.target.push_back(targ);
            ++state->currentinfo.target_cnt;
        }
    }
    return targets.size();
}

//! @brief Add orbit from line of JSON in a file.
//! @param filename Path to file containing satellite information.
//! @return Number of arguments, or negative error.

int32_t Simulator::ParseOrbitFile(string filename)
{
    int32_t iretn = 0;
    string  line;
    FILE *fp;
    if (filename.empty())
    {
        filename = get_realmdir(realmname, true) + "/" + "orbit.dat";
    }

    if ((fp = fopen(filename.c_str(), "r")) != nullptr)
    {
        line.resize(1010);
        while (fgets((char *)line.data(), 1000, fp) != nullptr)
        {
            iretn = ParseOrbitString(line);
            if (iretn < 0)
            {
                fclose(fp);
                return iretn;
            }
        }
        fclose(fp);
    }
    return iretn;
}

//! @brief Add orbit from line of JSON in a string.
//! @param args JSON line of orbit arguments.
//! @return Number of arguments, or negative error.

int32_t Simulator::ParseOrbitString(string args)
{
    double deltautc = 0.;
    uint16_t argcount = 0;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
    if (!jargs["model"].is_null())
    {
        ++argcount;
        if (jargs["model"].string_value() == "fast")
        {
            fastcalc = true;
        }
        else if (jargs["model"].string_value() == "slow")
        {
            fastcalc = false;
        }
    }
    if (!jargs["phys"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["phys"].object_items();
        if (values["utc"].number_value() != 0.)
        {
            initialutc += values["utc"].number_value();
        }
        else
        {
            initialutc += currentmjd();
        }
        currentutc = initialutc;
        offsetutc = initialutc - currentmjd();
        dt = 86400.*((initialutc + (dt / 86400.))-initialutc);
        dtj = dt / 86400.;
        if (deltautc != 0.)
        {
            endutc = initialutc + deltautc;
        }
        double initiallat = RADOF(21.3069);
        double initiallon = RADOF(-157.8583);
        double initialalt = 400000.;
        double initialangle = RADOF(54.);
        initiallat = RADOF(values["lat"].number_value());
        initiallon = RADOF(values["lon"].number_value());
        initialalt = values["alt"].number_value();
        initialangle = RADOF(values["angle"].number_value());
       initialloc = Physics::shape2eci(initialutc, initiallat, initiallon, initialalt, initialangle, 0.);
    }
    if (!jargs["eci"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["eci"].object_items();
        initialloc.pos.eci.utc = (values["utc"].number_value());
        initialloc.pos.eci.s.col[0] = (values["x"].number_value());
        initialloc.pos.eci.s.col[1] = (values["y"].number_value());
        initialloc.pos.eci.s.col[2] = (values["z"].number_value());
        initialloc.pos.eci.v.col[0] = (values["vx"].number_value());
        initialloc.pos.eci.v.col[1] = (values["vy"].number_value());
        initialloc.pos.eci.v.col[2] = (values["vz"].number_value());
        initialloc.pos.eci.pass++;
        initialutc += initialloc.pos.eci.utc;
        currentutc = initialutc;
        offsetutc = initialutc - currentmjd();
        dt = 86400.*((initialutc + (dt / 86400.))-initialutc);
        dtj = dt / 86400.;
        if (deltautc != 0.)
        {
            endutc = initialutc + deltautc;
        }
    }
    if (!jargs["kep"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["kep"].object_items();
        if (values["utc"].number_value() != 0.)
        {
            initialutc += values["utc"].number_value();
        }
        else
        {
            initialutc += currentmjd();
        }
        currentutc = initialutc;
        offsetutc = initialutc - currentmjd();
        dt = 86400.*((initialutc + (dt / 86400.))-initialutc);
        dtj = dt / 86400.;
        if (deltautc != 0.)
        {
            endutc = initialutc + deltautc;
        }
        kepstruc kep;
        initialutc += currentmjd();
        kep.utc = initialutc;
        kep.ea = values["ea"].number_value();
        kep.i = values["i"].number_value();
        kep.ap = values["ap"].number_value();
        kep.raan = values["raan"].number_value();
        kep.e = values["e"].number_value();
        kep.a = values["a"].number_value();
        kep2eci(kep, initialloc.pos.eci);
        initialloc.pos.eci.pass++;
    }
    if (!jargs["tle"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["tle"].object_items();
        vector<Convert::tlestruc>lines;
        string fname = get_realmdir(realmname, true) + "/" + values["filename"].string_value();
        load_lines(fname, lines);
        if (fabs(initialutc) <= 3600.)
        {
            initialutc +=lines[0].utc;
        }
        currentutc = initialutc;
        offsetutc = initialutc - currentmjd();
        dt = 86400.*((initialutc + (dt / 86400.))-initialutc);
        dtj = dt / 86400.;
        if (deltautc != 0.)
        {
            endutc = initialutc + deltautc;
        }
        initialloc.tle = lines[0];
        tle2eci(initialutc, initialloc.tle, initialloc.pos.eci);
        initialloc.pos.eci.pass++;
    }
    pos_eci(initialloc);
    if (initialloc.tle.utc == 0.)
    {
        eci2tle2(initialloc.pos.eci, initialloc.tle);
    }
    return argcount;
}

//! @brief Add satellites from lines of JSON in a file.
//! @param filename Path to file containing satellite information.
//! @return Number of satellites, or negative error.

int32_t Simulator::ParseSatFile(string filename)
{
    string  line;
    FILE *fp;
    int32_t iretn;

    if (filename.empty())
    {
        filename = get_realmdir(realmname, true) + "/" + "sats.dat";
    }

    if ((fp = fopen(filename.c_str(), "r")) != nullptr)
    {
        line.resize(1010);
        while (fgets((char *)line.data(), 1000, fp) != nullptr)
        {
            iretn = ParseSatString(line);
            if (iretn < 0)
            {
                fclose(fp);
                return iretn;
            }
        }
        fclose(fp);
    }
    return cnodes.size();
}

//! @brief Add single satellite from line of JSON in a string.
//! @param args JSON line of satellite arguments.
//! @return Number of arguments, or negative error.

int32_t Simulator::ParseSatString(string args)
{
    int32_t iretn;
    double maxthrust = 0.;
    double maxalpha = 0.;
    double maxomega = 0.;
    double maxmoment = 0.;
    uint16_t argcount = 0;
    string nodename;
    string type;
    uint16_t nodetype = NODE_TYPE_SATELLITE;
    vector<camstruc> dets;
    locstruc satloc;
    cartpos lvlh;
    string estring;
    json11::Json jargs = json11::Json::parse(args, estring);
    if (!jargs["type"].is_null())
    {
        ++argcount;
        type = jargs["type"].string_value();
    }
    if (!jargs["nodename"].is_null())
    {
        ++argcount;
        nodename = jargs["nodename"].string_value();
    }
    if (!jargs["nodetype"].is_null())
    {
        ++argcount;
        nodetype = jargs["nodetype"].long_value();
    }
    if (!jargs["detector"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["detector"].object_items();
        camstruc det;
        det.fov = values["fov"].number_value();
        det.ifov = values["ifov"].number_value();
        det.specmin = values["specmin"].number_value();
        det.specmax = values["specmax"].number_value();
        det.name = "det_" + to_unsigned(dets.size());
        dets.push_back(det);
        AddDetector(det);
    }
    if (!jargs["maxthrust"].is_null())
    {
        ++argcount;
        maxthrust = jargs["maxthrust"].number_value();
    }
    if (!jargs["maxalpha"].is_null())
    {
        ++argcount;
        maxalpha = jargs["maxalpha"].number_value();
    }
    if (!jargs["maxomega"].is_null())
    {
        ++argcount;
        maxomega = jargs["maxomega"].number_value();
    }
    if (!jargs["maxmoment"].is_null())
    {
        ++argcount;
        maxmoment = jargs["maxmoment"].number_value();
    }
    if (!jargs["lvlh"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["lvlh"].object_items();
        if (fastcalc)
        {
            satloc = cnodes[0]->currentinfo.node.loc;
        }
        else
        {
            satloc = initialloc;
        }
        satloc.pos.lvlh.s.col[0] = values["x"].number_value();
        satloc.pos.lvlh.s.col[1] = values["y"].number_value();
        satloc.pos.lvlh.s.col[2] = values["z"].number_value();
        satloc.pos.lvlh.v.col[0] = values["vx"].number_value();
        satloc.pos.lvlh.v.col[1] = values["vy"].number_value();
        satloc.pos.lvlh.v.col[2] = values["vz"].number_value();
        satloc.pos.lvlh.pass++;
        pos_origin2lvlh(satloc);
        eci2tle2(satloc.pos.eci, satloc.tle);
    }
    if (!jargs["ric"].is_null())
    {
        ++argcount;
        json11::Json::object values = jargs["ric"].object_items();
        cartpos ric;
        if (fastcalc)
        {
            satloc = cnodes[0]->currentinfo.node.loc;
        }
        else
        {
            satloc = initialloc;
        }
        satloc.pos.lvlh.s.col[0] = values["r"].number_value();
        satloc.pos.lvlh.s.col[1] = values["i"].number_value();
        satloc.pos.lvlh.s.col[2] = values["c"].number_value();
        satloc.pos.lvlh.v.col[0] = values["vr"].number_value();
        satloc.pos.lvlh.v.col[1] = values["vi"].number_value();
        satloc.pos.lvlh.v.col[2] = values["vc"].number_value();
        ric2lvlh(satloc.pos.lvlh, satloc.pos.lvlh);
        lvlh.pass++;
        pos_origin2lvlh(satloc);
        eci2tle2(satloc.pos.eci, satloc.tle);
    }
    if (!cnodes.size())
    {
        if (nodename.empty())
        {
            nodename = "mother";
        }
        if (type.empty())
        {
            type = "HEX65W80H";
        }
        if (fastcalc)
        {
            iretn = AddNode(nodename, type, Physics::Propagator::PositionTle, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, initialloc.tle, initialloc.att.icrf);
        }
        else
        {
            iretn = AddNode(nodename, type, Physics::Propagator::PositionGaussJackson, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, initialloc.pos.eci, initialloc.att.icrf);
        }
    }
    else
    {
        if (nodename.empty())
        {
            nodename = "child_" + to_unsigned(cnodes.size(), 2, true);
        }
        if (type.empty())
        {
            type = "U12";
        }
//        iretn = AddNode(nodename, type, Physics::Propagator::PositionLvlh, Physics::Propagator::AttitudeTarget, Physics::Propagator::Thermal, Physics::Propagator::Electrical, initialloc.pos.eci, satloc.pos.lvlh, initialloc.att.icrf);
        if (fastcalc)
        {
            iretn = AddNode(nodename, type, Physics::Propagator::PositionTle, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, satloc.tle, initialloc.att.icrf);
        }
        else
        {
            iretn = AddNode(nodename, type, Physics::Propagator::PositionGaussJackson, Physics::Propagator::AttitudeLVLH, Physics::Propagator::Thermal, Physics::Propagator::Electrical, satloc.pos.eci, initialloc.att.icrf);
        }
    }

    Physics::Simulator::StateList::iterator sit = GetNode(nodename);
    (*sit)->currentinfo.node.type = nodetype;

    // CPU
    iretn = json_createpiece(&(*sit)->currentinfo, "obc_cpu", DeviceType::CPU);

    // Disk
    iretn = json_createpiece(&(*sit)->currentinfo, "obc_disk", DeviceType::DISK);

    // Thruster
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_thrust", DeviceType::THST);
    (*sit)->currentinfo.devspec.thst[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].maxthrust = maxthrust;
    (*sit)->currentinfo.devspec.thst[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unitx(), rv_unitz(-1.), rv_unitx());

    // Reaction wheels
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_rw_x", DeviceType::RW);
    (*sit)->currentinfo.devspec.rw[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].mxalp = maxalpha;
    (*sit)->currentinfo.devspec.rw[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].mxomg = maxomega;
    (*sit)->currentinfo.devspec.rw[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitx(), rv_unity());
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_rw_y", DeviceType::RW);
    (*sit)->currentinfo.devspec.rw[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].mxalp = maxalpha;
    (*sit)->currentinfo.devspec.rw[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].mxomg = maxomega;
    (*sit)->currentinfo.devspec.rw[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unitx(), rv_unity(), rv_unitx());
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_rw_z", DeviceType::RW);
    (*sit)->currentinfo.devspec.rw[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].mxalp = maxalpha;
    (*sit)->currentinfo.devspec.rw[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].mxomg = maxomega;
    (*sit)->currentinfo.devspec.rw[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitz(), rv_unity());

    // Torque rods
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_mtr_x", DeviceType::MTR);
    (*sit)->currentinfo.devspec.mtr[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].mxmom = maxmoment;
    (*sit)->currentinfo.devspec.mtr[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitx(), rv_unity());
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_mtr_y", DeviceType::MTR);
    (*sit)->currentinfo.devspec.mtr[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].mxmom = maxmoment;
    (*sit)->currentinfo.devspec.mtr[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unitx(), rv_unity(), rv_unitx());
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_mtr_z", DeviceType::MTR);
    (*sit)->currentinfo.devspec.mtr[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].mxmom = maxmoment;
    (*sit)->currentinfo.devspec.mtr[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitz(), rv_unity());

    // GPS
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_gps", DeviceType::GPS);

    // Magnetometer
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_mag", DeviceType::MAG);
    (*sit)->currentinfo.devspec.mag[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitx(), rv_unity());

    // Gyros
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_gyro_x", DeviceType::GYRO);
    (*sit)->currentinfo.devspec.gyro[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitx(), rv_unity());
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_gyro_y", DeviceType::GYRO);
    (*sit)->currentinfo.devspec.gyro[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unitx(), rv_unity(), rv_unitx());
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_gyro_z", DeviceType::GYRO);
    (*sit)->currentinfo.devspec.gyro[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitz(), rv_unity());

    // Star trackers
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_stt_x+", DeviceType::STT);
    (*sit)->currentinfo.devspec.stt[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitx(), rv_unity());
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_stt_x-", DeviceType::STT);
    (*sit)->currentinfo.devspec.stt[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitx(-1.0), rv_unity());

    // Sun and Earth sensors
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_xyzsen_sun", DeviceType::XYZSEN);
    //        (*sit)->currentinfo.devspec.xyzsen[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitz(-1.0), rv_unity());
    iretn = json_createpiece(&(*sit)->currentinfo, "adcs_xyzsen_earth", DeviceType::XYZSEN);
    //        (*sit)->currentinfo.devspec.xyzsen[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].align = q_irotate_for(rv_unitz(), rv_unity(), rv_unitz(), rv_unity());


    // Detectors
    for (camstruc det : dets)
    {
        iretn = json_createpiece(&(*sit)->currentinfo, det.name, DeviceType::CAM);
        (*sit)->currentinfo.devspec.cam[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].volt = 5.;
        (*sit)->currentinfo.devspec.cam[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].amp = 20. / det.volt;
        (*sit)->currentinfo.devspec.cam[(*sit)->currentinfo.device[(*sit)->currentinfo.pieces[iretn].cidx]->didx].state = 0;
    }

//    json_dump_node(&(*sit)->currentinfo);
    json_updatecosmosstruc(&(*sit)->currentinfo);

    return argcount;
}

//! @brief Add targets from lines of JSON in a file.
//! @param filename Path to file containing target information.
//! @return Number of targets, or negative error.

int32_t Simulator::ParseTargetFile(string filename)
{
    string  line;
    FILE *fp;
    int32_t iretn;

    if (filename.empty())
    {
        filename = get_realmdir(realmname, true) + "/" + "targets.dat";
    }

    if ((fp = fopen(filename.c_str(), "r")) != nullptr)
    {
        line.resize(1010);
        while (fgets((char *)line.data(), 1000, fp) != nullptr)
        {
            iretn = ParseTargetString(line);
            if (iretn < 0)
            {
                fclose(fp);
                return iretn;
            }
        }
        fclose(fp);
    }
    return targets.size();
}

//! @brief Add single target from line of JSON in a string.
//! @param args JSON line of target arguments.
//! @return Number of arguments, or negative error.

/* given a JSON string of targets formatted this way:

{
"target_name_1":{"lat":19.398552,"lon":-155.284714,"h":190.1,"size":230},
"target_name_2":{"lat":19.400633,"lon":-155.271988,"h":213.2,"size":57},
"target_name_3":{"lat":19.417826,"lon":-155.073257,"h":-10.3,"size":176},
"target_name_4":{"lat":19.419832,"lon":-155.061142,"h":142.4,"size":438},
...

each target is loaded into cosmosstruc::target (via AddTarget(..))

notes:

longitude and latitude are in radians
height is in meters
missing fields receive a value of "" or 0

*/
int32_t Simulator::ParseTargetString(string line)
{
	// if valid JSON string, parse JSON string and populate cosmosstruc::target

	// else not valid JSON string, use legacy target format


    if (line[0] == '{')
    {
        targetstruc targ;
        string estring;
        json11::Json jargs = json11::Json::parse(line, estring);
        targ.type = 0;
        if (!jargs["type"].is_null())
        {
            targ.type = jargs["type"].number_value();
        }
        if (!jargs["name"].is_null())
        {
            targ.name = jargs["name"].string_value();
        }
        if (!jargs["latitude"].is_null())
        {
            targ.loc.pos.geod.s.lat = RADOF(jargs["latitude"].number_value());
        }
        if (!jargs["longitude"].is_null())
        {
            targ.loc.pos.geod.s.lon = RADOF(jargs["longitude"].number_value());
        }
        targ.loc.pos.geod.s.h = 0.;
        if (!jargs["altitude"].is_null())
        {
            targ.loc.pos.geod.s.h = jargs["altitude"].number_value();
        }
        targ.area = 1.;
        if (!jargs["area"].is_null())
        {
            targ.area = jargs["area"].number_value();
        }
        AddTarget(targ);
    }
    else
    {
        vector<string> args = string_split(line, " \t", true);
        if (args.size() == 4)
        {
            AddTarget(args[0], RADOF(stof(args[1])), RADOF(stod(args[2])), 0., stod(args[3]), NODE_TYPE_GROUNDSTATION);
        }
        else if (args.size() == 5)
        {
            AddTarget(args[0], RADOF(stof(args[1])), RADOF(stod(args[2])), RADOF(stof(args[3])), RADOF(stod(args[4])), NODE_TYPE_TARGET);
        }
    }
    return targets.size();
}

//! @brief Nudges Node in the desired direction
//!
//!
int32_t Simulator::NudgeNode(string nodename, cartpos pos, qatt att)
{
    auto node = GetNode(nodename);
    if (node == cnodes.end())
    {
        return 0;
    }

    Vector newpush;
    if (pos.utc != 0.)
    {
        switch((*node)->ptype)
        {
        case Propagator::Type::PositionIterative:
            (*node)->currentinfo.node.loc.pos.eci.s += pos.s;
            (*node)->currentinfo.node.loc.pos.eci.v += pos.v;
            (*node)->currentinfo.node.loc.pos.eci.a += pos.a;
            (*node)->currentinfo.node.loc.pos.eci.j += pos.j;
            (*node)->currentinfo.node.loc.pos.eci.pass++;
            pos_eci((*node)->currentinfo.node.loc);
            (*node)->currentinfo.node.phys.fpush = rv_zero();
            break;
        case Propagator::Type::PositionInertial:
            (*node)->currentinfo.node.loc.pos.icrf.s += pos.s;
            (*node)->currentinfo.node.loc.pos.icrf.v += pos.v;
            (*node)->currentinfo.node.loc.pos.icrf.a += pos.a;
            (*node)->currentinfo.node.loc.pos.icrf.j += pos.j;
            (*node)->currentinfo.node.loc.pos.icrf.pass++;
            pos_icrf((*node)->currentinfo.node.loc);
            (*node)->currentinfo.node.phys.fpush = rv_zero();
            break;
        case Propagator::Type::PositionGaussJackson:
        {
            double dt = 86400.* (pos.utc - (*node)->currentinfo.node.loc.pos.eci.utc);
            double dt2 = dt * dt;
            double dt3 = dt2 * dt;
            Vector s1 = Vector((*node)->currentinfo.node.loc.pos.eci.s);
            Vector v1 = Vector((*node)->currentinfo.node.loc.pos.eci.v);
            Vector a1 = Vector((*node)->currentinfo.node.loc.pos.eci.a);
            Vector j1 = Vector((*node)->currentinfo.node.loc.pos.eci.j);
            Vector s2 = s1 + v1 * dt + a1 * dt2 / 2. + j1 * dt3 / 6.;
            Vector v2 = v1 + a1 * dt + j1 * dt2 / 2.;
            Vector a2 = a1 + j1 * dt;
            Vector sp = Vector(pos.s);
            Vector vp = Vector(pos.v);
            Vector ap = Vector(pos.a);
            Vector ds1 = sp - s1;
            double deltas = ds1.norm();
            Vector ds2 = sp - s2;
            Vector as = 2. * ds2 / dt2;
            Vector dv1 = vp - v1;
            double deltav = dt * dv1.norm();
            Vector dv2 = vp - v2;
            Vector av = dv2 / dt;
            Vector da1 = ap - a1;
            double deltaa = dt2 * da1.norm();
            Vector da2 = ap - a2;
            Vector aa = da2;
            double delta = deltas + deltav + deltaa;
            Vector acc = (delta * aa + deltav * av + deltas * as) / delta;
            (*node)->currentinfo.node.phys.fpush = acc.to_rv();
        }
        break;
        case Propagator::Type::PositionGeo:
            (*node)->currentinfo.node.loc.pos.geod.s.lon += pos.s.col[0];
            (*node)->currentinfo.node.loc.pos.geod.s.lat += pos.s.col[1];
            (*node)->currentinfo.node.loc.pos.geod.s.h += pos.s.col[2];
            (*node)->currentinfo.node.loc.pos.geod.v.lon += pos.v.col[0];
            (*node)->currentinfo.node.loc.pos.geod.v.lat += pos.v.col[1];
            (*node)->currentinfo.node.loc.pos.geod.v.h += pos.v.col[2];
            (*node)->currentinfo.node.loc.pos.geod.a.lon += pos.a.col[0];
            (*node)->currentinfo.node.loc.pos.geod.a.lat += pos.a.col[1];
            (*node)->currentinfo.node.loc.pos.geod.a.h += pos.a.col[2];
            (*node)->currentinfo.node.loc.pos.geod.pass++;
            pos_geod((*node)->currentinfo.node.loc);
            (*node)->currentinfo.node.phys.fpush = rv_zero();
            break;
        case Propagator::Type::PositionTle:
            tle2eci((*node)->currentinfo.node.loc.pos.eci.utc, (*node)->currentinfo.node.loc.tle, (*node)->currentinfo.node.loc.pos.eci);
            (*node)->currentinfo.node.loc.pos.eci.s += pos.s;
            (*node)->currentinfo.node.loc.pos.eci.v += pos.v;
            (*node)->currentinfo.node.loc.pos.eci.a += pos.a;
            (*node)->currentinfo.node.loc.pos.eci.j += pos.j;
            (*node)->currentinfo.node.loc.pos.eci.pass++;
            pos_eci((*node)->currentinfo.node.loc);
            eci2tle2((*node)->currentinfo.node.loc.pos.eci, (*node)->currentinfo.node.loc.tle);
            (*node)->currentinfo.node.phys.fpush = rv_zero();
            break;
        case Propagator::Type::PositionLvlh:
        {
            pos_lvlh2origin((*node)->currentinfo.node.loc);
            (*node)->currentinfo.node.loc.pos.lvlh.s += pos.s;
            (*node)->currentinfo.node.loc.pos.lvlh.v += pos.v;
            (*node)->currentinfo.node.loc.pos.lvlh.a += pos.a;
            (*node)->currentinfo.node.loc.pos.lvlh.j += pos.j;
            (*node)->currentinfo.node.loc.pos.lvlh.pass++;
            pos_origin2lvlh((*node)->currentinfo.node.loc);
            double dt = 86400.* (pos.utc - (*node)->currentinfo.node.loc.pos.eci.utc);
            double dt2 = dt * dt;
            double dt3 = dt2 * dt;
            Vector s1 = Vector((*node)->currentinfo.node.loc.pos.eci.s);
            Vector v1 = Vector((*node)->currentinfo.node.loc.pos.eci.v);
            Vector a1 = Vector((*node)->currentinfo.node.loc.pos.eci.a);
            Vector j1 = Vector((*node)->currentinfo.node.loc.pos.eci.j);
            Vector s2 = s1 + v1 * dt + a1 * dt2 / 2. + j1 * dt3 / 6.;
            Vector v2 = v1 + a1 * dt + j1 * dt2 / 2.;
            Vector a2 = a1 + j1 * dt;
            Vector sp = Vector(pos.s);
            Vector vp = Vector(pos.v);
            Vector ap = Vector(pos.a);
            Vector ds1 = sp - s1;
            double deltas = ds1.norm();
            Vector ds2 = sp - s2;
            Vector as = 2. * ds2 / dt2;
            Vector dv1 = vp - v1;
            double deltav = dt * dv1.norm();
            Vector dv2 = vp - v2;
            Vector av = dv2 / dt;
            Vector da1 = ap - a1;
            double deltaa = dt2 * da1.norm();
            Vector da2 = ap - a2;
            Vector aa = da2;
            double delta = deltas + deltav + deltaa;
            Vector acc = (delta * aa + deltav * av + deltas * as) / delta;
            (*node)->currentinfo.node.phys.fpush = acc.to_rv();
        }
        break;
        default:
            break;
        }
    }
    if (att.utc != 0.)
    {

    }

    Vector newtorque;
    UpdateTorque(nodename, newtorque);
    return 1;
}

/**
         * @brief Addes a node to be propagated, in descending order of priority
         * 
         * @param nodename Name of node
         * @param propagation_priority Lower values are propagated before higher values
         * @return int32_t 
         */
Simulator::StateList::iterator Simulator::AddNode(string nodename, uint8_t propagation_priority)
{
    Physics::State* newstate = new Physics::State(nodename, propagation_priority);
    StateList::iterator it = std::upper_bound(cnodes.begin(), cnodes.end(), newstate, compareByPriority);
    return cnodes.insert(it, newstate);
}

int32_t Simulator::AddNode(string nodename, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::tlestruc tle, Convert::qatt icrf, uint8_t propagation_priority)
{
    auto it = AddNode(nodename, propagation_priority);
    error = (*it)->Init(nodename, dt, stype, ptype, atype, ttype, etype, tle, currentutc, icrf);
    if (error < 0)
    {
        return error;
    }
    return cnodes.size();
}

int32_t Simulator::AddNode(string nodename, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::cartpos eci, Convert::qatt icrf, uint8_t propagation_priority)
{
    auto it = AddNode(nodename, propagation_priority);
    error = (*it)->Init(nodename, dt, stype, ptype, atype, ttype, etype, eci, icrf);
    if (error < 0)
    {
        return error;
    }
    error = (*it)->Propagate(currentutc);
    if (error < 0)
    {
        return error;
    }
    return cnodes.size();
}

/**
         * @brief Add a node with an LVLH position propagator
         * 
         * @param nodename Name of node
         * @param stype Structural type
         * @param ptype Position propagator
         * @param atype Attitude propagator
         * @param ttype Thermal propagator
         * @param etype Electrical propagator
         * @param oeventtype Orbital Events propagator
         * @param lvlh LVLH coordinates
         * @param origineci The origin of the LVLH frame in geocentric frame
         * @param icrf Attitude of node
         * @return int32_t 0 on success, negative on error
         */
int32_t Simulator::AddNode(string nodename, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::cartpos origineci, Convert::cartpos lvlh, Convert::qatt icrf, uint8_t propagation_priority)
{
    auto it = AddNode(nodename, propagation_priority);
    error = (*it)->Init(nodename, dt, stype, ptype, atype, ttype, etype, origineci, lvlh, icrf);
    if (error < 0)
    {
        return error;
    }
    error = (*it)->Propagate(currentutc);
    if (error < 0)
    {
        return error;
    }
    return cnodes.size();
}

int32_t Simulator::AddNode(string nodename, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, double utc, double latitude, double longitude, double altitude, double angle, double timeshift)
{
    auto it = AddNode(nodename, 0);
    Convert::locstruc loc;
    if (ptype == Propagator::PositionGeo)
    {
        loc.pos.geod.s.lat = latitude;
        loc.pos.geod.s.lon = longitude;
        loc.pos.geod.s.h = altitude;
        loc.pos.geod.v = gv_zero();
        loc.pos.geod.a = gv_zero();
        loc.pos.geod.utc = utc;
        loc.pos.geod.pass++;
        Convert::pos_geod(loc);
        loc.att.geoc.s = q_eye();
        loc.att.geoc.v = rv_zero();
        loc.att.geoc.a = rv_zero();
        loc.att.geoc.utc = utc;
        loc.att.geoc.pass++;
        Convert::att_geoc(loc);
    }
    else
    {
        loc = Physics::shape2eci(utc, latitude, longitude, altitude, angle, timeshift);
        loc.att.lvlh.s = q_eye();
        loc.att.lvlh.v = rv_zero();
        loc.att.lvlh.a = rv_zero();
        loc.att.lvlh.utc = utc;
        loc.att.lvlh.pass++;
        Convert::att_lvlh(loc);
    }
    error = (*it)->Init(nodename, dt, stype, ptype, atype, ttype, etype, loc.pos.eci, loc.att.icrf);
    if (error < 0)
    {
        return error;
    }
    error = (*it)->Propagate(currentutc);
    if (error < 0)
    {
        return error;
    }
    return cnodes.size();
}

int32_t Simulator::GetError()
{
    return error;
}

int32_t Simulator::Reset()
{
    RunState = State::Paused;
    currentutc = initialutc;
    for (auto &state : cnodes)
    {
        state->Reset(currentutc);
    }
    return error;
}

int32_t Simulator::Propagate(double nextutc)
{
    int iretn = 0;
    if (nextutc == 0.)
    {
        currentutc += dtj;
    }
    else {
        currentutc = nextutc;
    }
    for (auto &state : cnodes)
    {
        switch(state->ptype)
        {
        case Physics::Propagator::Type::PositionInertial:
        case Physics::Propagator::Type::PositionIterative:
        case Physics::Propagator::Type::PositionGaussJackson:
        case Physics::Propagator::Type::PositionGeo:
        case Physics::Propagator::Type::PositionTle:
            iretn = state->Propagate(currentutc);
            break;
        case Physics::Propagator::Type::PositionLvlh:
            iretn = state->Propagate(cnodes[0]->currentinfo.node.loc);
            break;
        default:
            break;
        }
    }
    return iretn;
}

int32_t Simulator::End()
{
    int32_t iretn = 0;
    for (auto &state : cnodes)
    {
        iretn = state->End();
    }
    return iretn;
}

int32_t Simulator::Run()
{
    RunState = State::Running;
    return error;
}

int32_t Simulator::Pause()
{
    RunState = State::Paused;
    return error;
}

Simulator::State Simulator::GetState()
{
    return RunState;
}

int32_t Simulator::GetNode(string name, Physics::State* &node)
{
    auto it = std::find_if(cnodes.begin(), cnodes.end(), [name](const Physics::State* state) {
        return state->currentinfo.node.name == name;
    });
    if (it == cnodes.end())
    {
        return 0;
    }
    node = *it;
    return 1;
}

Simulator::StateList::iterator Simulator::GetNode(string name)
{
    auto node = std::find_if(cnodes.begin(), cnodes.end(), [name](const Physics::State* state) {
        return state->currentinfo.node.name == name;
    });
    return node;
}

Simulator::StateList::const_iterator Simulator::GetNode(string name) const
{
    auto node = std::find_if(cnodes.begin(), cnodes.end(), [name](const Physics::State* state) {
        return state->currentinfo.node.name == name;
    });
    return node;
}

Simulator::StateList::iterator Simulator::GetEnd()
{
    return cnodes.end();
}

int32_t Simulator::UpdatePush(string name, Vector fpush)
{
    auto node = GetNode(name);
    if (node == cnodes.end())
    {
        return 0;
    }
    (*node)->currentinfo.node.phys.fpush = fpush;
    return 1;
}

int32_t Simulator::UpdateThrust(string name, Vector thrust)
{
    auto node = GetNode(name);
    if (node == cnodes.end())
    {
        return 0;
    }
    (*node)->currentinfo.node.phys.thrust = thrust;
    return 1;
}

int32_t Simulator::UpdateTorque(string name, Vector torque)
{
    auto node = GetNode(name);
    if (node == cnodes.end())
    {
        return 0;
    }
    (*node)->currentinfo.node.phys.ftorque = torque;
    return 1;
}

Simulator::StateList Simulator::GetNodes() {
    return cnodes;
}

}
}
