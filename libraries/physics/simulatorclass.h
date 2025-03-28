#ifndef SIMULATORCLASS_H
#define SIMULATORCLASS_H

#include "support/configCosmos.h"
#include "support/jsondef.h"
#include "support/convertlib.h"
#include "physics/physicsclass.h"
#include "physics/controllib.h"

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

            int32_t Init(double idt=1., string realm="propagate", double iutc=0.);
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
            int32_t ParseOrbitFile(string filename="");
            int32_t ParseOrbitString(string args);
            int32_t ParseSatFile(string filename="");
            int32_t ParseSatString(string args);
            int32_t ParseTargetFile(string filename="");
            int32_t ParseTargetString(string line);
            int32_t ParseTargetJson(json11::Json jargs);
            int32_t GetError();
            int32_t Propagate(double nextutc=0.);
            int32_t Propagate(vector<vector<cosmosstruc> > &results, uint32_t runcount);
            int32_t Target(map<uint32_t, vector<qatt> > &pschedule);
            int32_t Target();
            int32_t Target(vector<vector<cosmosstruc> > &results);
            int32_t Metric();
            int32_t Metric(vector<vector<cosmosstruc> > &results);
            int32_t Thrust();
            int32_t Formation(string type, double spacing);
            int32_t Update();
            //! Ends the simulation and runs any code that the propagators need to run at the end
            int32_t End();
            int32_t Reset();
            int32_t Run();
            int32_t Pause();
            State GetState();
            int32_t GetNode(string name, Physics::State *&node);
            StateList::iterator GetNode(string name);
            StateList::const_iterator GetNode(string name) const;
            StateList::const_iterator GetEnd() const;
            StateList GetNodes();
            int32_t NudgeNode(string name, cartpos pos=cartpos(), qatt att=qatt());
            int32_t UpdatePush(string name, Vector fpush);
            int32_t UpdateThrust(string name, Vector thrust);
            int32_t UpdateTorque(string name, Vector torque);
            double initialutc = 0.;
            double endutc = 0.;
            double currentutc = 0.;
            double offsetutc = 0.;
            locstruc initialloc;
            StateList cnodes;
            map<string, camstruc> detectors;
            map<string, targetstruc> targets;
            string realmname="propagate";
            double dt;
            double dtj;
            bool fastcalc = true;

        private:
            bool server;
            int32_t error;

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
