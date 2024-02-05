#ifndef SIMULATORCLASS_H
#define SIMULATORCLASS_H

#include "support/configCosmos.h"
#include "support/jsondef.h"
#include "support/convertlib.h"
#include "physics/physicsclass.h"

#define SIMULATOR_PORT_OUT 10030
#define CLIENT_PORT_OUT 10031

namespace Cosmos
{
    namespace Physics
    {
        class Simulator
        {
        public:

            // constructor
            Simulator(string inodename="") : nodename{inodename} {
                cnodes.clear();
                server = nodename.empty() ? true : false ;
                error = 0;
            }

            enum State {
                Halted =0,
                Paused,
                Running
                };

            typedef vector<Physics::State*> StateList;

            int32_t Init(double iutc, double idt=1.);
            int32_t Connect();
            StateList::iterator AddNode(string nodename, uint8_t propagation_priority);
            int32_t AddNode(string nodename, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::cartpos eci, Convert::qatt icrf=Convert::qatt(), uint8_t propagation_priority=0);
            int32_t AddNode(string nodename, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::cartpos origineci, Convert::cartpos lvlh, Convert::qatt icrf=Convert::qatt(), uint8_t propagation_priority=0);
            int32_t AddNode(string nodename, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::tlestruc tle, Convert::qatt icrf=Convert::qatt(), uint8_t propagation_priority=0);
            int32_t AddNode(string nodename, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, double utc, double lat, double lon, double altitude, double angle, double timeshift=0.);
            int32_t AddDetector(camstruc& det);
            int32_t AddDetector(string name, float fov, float ifov, float specmin, float specmax);
            int32_t AddTarget(targetstruc& targ);
            int32_t AddTarget(string name, locstruc loc, NODE_TYPE type=NODE_TYPE_GROUNDSTATION, gvector size={0.,0.,0.});
            int32_t AddTarget(string name, locstruc loc, NODE_TYPE type=NODE_TYPE_GROUNDSTATION, double area=0.);
            int32_t AddTarget(string name, double lat, double lon, double alt, NODE_TYPE type=NODE_TYPE_GROUNDSTATION);
            int32_t AddTarget(string name, double lat, double lon, double area, double alt, NODE_TYPE type=NODE_TYPE_GROUNDSTATION);
            int32_t AddTarget(string name, double ullat, double ullon, double lrlat, double lrlon, double alt, NODE_TYPE type=NODE_TYPE_SQUARE);
            int32_t GetError();
            int32_t Propagate(double nextutc=0.);
            //! Ends the simulation and runs any code that the propagators need to run at the end
            int32_t End();
            int32_t Reset();
            int32_t Run();
            int32_t Pause();
            State GetState();
            int32_t GetNode(string name, Physics::State *&node);
            StateList::iterator GetNode(string name);
            StateList::iterator GetEnd();
            StateList GetNodes();
            int32_t UpdatePush(string name, Vector fpush);
            int32_t UpdateThrust(string name, Vector thrust);
            int32_t UpdateTorque(string name, Vector torque);
            double initialutc = 0.;
            double currentutc = 0.;
            StateList cnodes;
            map<string, camstruc> detectors;
            map<string, targetstruc> targets;

        private:
            bool server;
            int32_t error;
            double dt;
            double dtj;

            State RunState = State::Halted;
            //    socket_channel data_channel;

            //    thread listener;
            //    void listen_loop();

            //    thread talker;
            //    void talk_loop();
            //    socket_channel data_channel;

            string nodename;
        };
    }
}

#endif // SIMULATORCLASS_H
