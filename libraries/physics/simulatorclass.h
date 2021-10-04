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
            }

            enum State {
                Halted =0,
                Paused,
                Running
                };

            typedef map<string, Physics::State*> StateList;

            int32_t Init(double iutc, double idt=1.);
            int32_t Connect();
            int32_t AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype);
            int32_t AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::posstruc pos);
            int32_t AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::locstruc loc);
            int32_t AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, vector<Convert::tlestruc> tles);
            int32_t AddNode(string nodename, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, double utc, double lat, double lon, double altitude, double angle, double timeshift=0.);
            int32_t GetError();
            int32_t Propagate(double nextutc=0.);
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
            double initialutc;
            double currentutc;
            StateList cnodes;

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
