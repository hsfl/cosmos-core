#include "simulatorclass.h"
#include "support/jsonclass.h"

namespace Cosmos
{
namespace Physics
{
int32_t Simulator::Init(double iutc, double idt)
{
    currentutc = iutc;
    initialutc = iutc;
    dt = 86400.*((initialutc + (idt / 86400.))-initialutc);
    dtj = dt / 86400.;

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
    ttarget.cloc = loc;
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
    ttarget.cloc = loc;
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
            (*node)->currentinfo.node.loc.pos.lvlh.s += pos.s;
            (*node)->currentinfo.node.loc.pos.lvlh.v += pos.v;
            (*node)->currentinfo.node.loc.pos.lvlh.a += pos.a;
            (*node)->currentinfo.node.loc.pos.lvlh.j += pos.j;
            (*node)->currentinfo.node.loc.pos.lvlh.pass++;
            cartpos lvlh = (*node)->currentinfo.node.loc.pos.lvlh;
            pos_origin2lvlh((*node)->currentinfo.node.loc, lvlh);
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
