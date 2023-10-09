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

        int32_t Simulator::AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::tlestruc tle, Convert::qatt icrf, uint8_t propagation_priority)
        {
            auto it = AddNode(nodename, propagation_priority);
            error = (*it)->Init(nodename, dt, stype, ptype, atype, ttype, etype, tle, currentutc, icrf);
            if (error < 0)
            {
                return error;
            }
            return cnodes.size();
        }

        int32_t Simulator::AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::cartpos eci, Convert::qatt icrf, uint8_t propagation_priority)
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
         * @param originicrf The origin of the LVLH frame in ICRF
         * @param icrf Attitude of node
         * @return int32_t 0 on success, negative on error
         */
        int32_t Simulator::AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::cartpos lvlh, Convert::cartpos origineci, Convert::qatt icrf, uint8_t propagation_priority)
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

        int32_t Simulator::AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, double utc, double latitude, double longitude, double altitude, double angle, double timeshift)
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
                iretn = state->Propagate(currentutc);
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
