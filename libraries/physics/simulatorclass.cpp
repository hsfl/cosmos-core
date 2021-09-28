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

        int32_t Simulator::AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype)
        {
            Physics::State* newstate = new Physics::State;
            cnodes.insert(pair<string, Physics::State*>(nodename, newstate));
            return cnodes.count(nodename);
        }

        int32_t Simulator::AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, vector<Convert::tlestruc> tles)
        {
            if (AddNode(nodename, stype, ptype, atype, ttype, etype))
            {
                error = cnodes[nodename]->Init(nodename, dt, stype, ptype, atype, ttype, etype, tles, currentutc);
                if (error < 0)
                {
                    return error;
                }
            }
            error = 0;
            return cnodes.count(nodename);
        }

        int32_t Simulator::AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::posstruc pos)
        {
            if (AddNode(nodename, stype, ptype, atype, ttype, etype))
            {
                error = cnodes[nodename]->Init(nodename, dt, stype, ptype, atype, ttype, etype, pos);
                if (error < 0)
                {
                    return error;
                }
                error = cnodes[nodename]->Propagate(currentutc);
                if (error < 0)
                {
                    return error;
                }
            }
            return cnodes.count(nodename);
        }

        int32_t Simulator::AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::locstruc loc)
        {
            if (AddNode(nodename, stype, ptype, atype, ttype, etype))
            {
                error = cnodes[nodename]->Init(nodename, dt, stype, ptype, atype, ttype, etype, loc);
                if (error < 0)
                {
                    return error;
                }
                error = cnodes[nodename]->Propagate(currentutc);
                if (error < 0)
                {
                    return error;
                }
            }
            return cnodes.count(nodename);
        }

        int32_t Simulator::AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, double utc, double latitude, double longitude, double altitude, double angle, double timeshift)
        {
//            Physics::State* newstate = new Physics::State;
//            auto testit = cnodes.insert(pair<string, Physics::State*>(nodename, newstate));
//            if (testit.second)
            if (AddNode(nodename, stype, ptype, atype, ttype, etype))
            {
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
                error = cnodes[nodename]->Init(nodename, dt, stype, ptype, atype, ttype, etype, loc);
                if (error < 0)
                {
                    return error;
                }
                error = cnodes[nodename]->Propagate(currentutc);
                if (error < 0)
                {
                    return error;
                }
            }
            return cnodes.count(nodename);
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
                state.second->Reset(currentutc);
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
                iretn = state.second->Propagate(currentutc);
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
            StateList::iterator nodeit;
            if ((nodeit=cnodes.find(name)) != cnodes.end()) {
                node = nodeit->second;
                return 1;
            } else {
                return 0;
            }
        }

        Simulator::StateList::iterator Simulator::GetNode(string name)
        {
            return cnodes.find(name);
        }

        Simulator::StateList::iterator Simulator::GetEnd()
        {
            return cnodes.end();
        }

        int32_t Simulator::UpdatePush(string name, Vector fpush)
        {
            StateList::iterator nodeit;
            if ((nodeit=cnodes.find(name)) != cnodes.end())
            {
                nodeit->second->currentinfo.node.phys.fpush = fpush;
                return 1;
            } else {
                return 0;
            }
        }

        int32_t Simulator::UpdateThrust(string name, Vector thrust)
        {
            StateList::iterator nodeit;
            if ((nodeit=cnodes.find(name)) != cnodes.end())
            {
                nodeit->second->currentinfo.node.phys.thrust = thrust;
                return 1;
            } else {
                return 0;
            }
        }

        int32_t Simulator::UpdateTorque(string name, Vector torque)
        {
            StateList::iterator nodeit;
            if ((nodeit=cnodes.find(name)) != cnodes.end())
            {
                nodeit->second->currentinfo.node.phys.ftorque = torque;
                return 1;
            } else {
                return 0;
            }
        }

        Simulator::StateList Simulator::GetNodes() {
            return cnodes;
        }

    }
}
