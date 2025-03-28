/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

#ifndef PHYSICSCLASS_H
#define PHYSICSCLASS_H

#include "support/configCosmos.h"
#include "support/jsonlib.h"
#include "support/convertlib.h"
#include "support/timelib.h"
#include "physics/nrlmsise-00.h"
#include "support/datalib.h"
#include "support/demlib.h"
using namespace Cosmos::Convert;


namespace Cosmos
{
    namespace Physics
    {
        class Structure
        {
        public:
            physicsstruc *currentphys;

            enum Type
                {
                NoType,
                U1,
                U1X,
                U1Y,
                U1XY,
                U1_5,
                U1_5X,
                U1_5Y,
                U1_5XY,
                U2,
                U2X,
                U2Y,
                U2XY,
                U3,
                U3X,
                U3Y,
                U3XY,
                U6,
                U6X,
                U6Y,
                U6XY,
                U12,
                U12X,
                U12Y,
                U12XY,
                HEX65W80H
                };

            enum ExternalPanelType
                {
                NoPanel = 0,
                X,
                Y,
                XY
                };

            Structure(physicsstruc *newphys) : currentphys{newphys}
            {

            }

            int32_t Setup(string stype);
            int32_t Setup(Type type);
            int32_t add_hex(double width=30, double height=30, ExternalPanelType type=NoPanel);
            int32_t add_oct(double width=30, double height=30, ExternalPanelType type=NoPanel);
            int32_t add_u(double x=1, double y=1, double z=1, ExternalPanelType type=NoPanel);
//            int32_t add_cuboid(string name, Vector dimensions, double depth, Quaternion orientation, Vector offset);
            int32_t add_panel(string name, Vector point0, Vector point1, Vector point2, Vector point3, double thickness=0.01, uint8_t external=1, float pcell=0.85, Quaternion orientation=Math::Quaternions::eye(), Vector offset=Vector(), float density=2700.);
            int32_t add_panel(string name, vector<Vector> points, double thickness=0.01, uint8_t external=1, float pcell=0.85, Quaternion orientation=Math::Quaternions::eye(), Vector offset=Vector(), float density=2700.);
            int32_t add_face(Vector point0, Vector point1, Vector point2, Vector point3, double thickness=0.01, uint8_t external=1, float pcell=0.85, Quaternion orientation=Math::Quaternions::eye(), Vector offset=Vector());
            int32_t add_face(vector<Vector> points, double thickness=0.01, uint8_t external=1, float pcell=0.85, Quaternion orientation=Math::Quaternions::eye(), Vector offset=Vector());
            int32_t add_face(Vector dimensions, uint8_t external=1, float pcell=0.85, Quaternion orientation=Math::Quaternions::eye(), Vector offset=Vector());
            int32_t add_triangle(Vector pointa, Vector pointb, Vector pointc, double depth, bool external=true, float pcell=.85);
            int32_t add_vertex(Vector point);

        private:
            Type type;

        };

        class Propagator
        {
        public:
            Propagator(cosmosstruc *newinfo, double idt) : currentinfo{newinfo}
            {
				// JIMNOTE:  dt = idt; ?
                dt = 86400.*((currentinfo->node.loc.utc + (idt / 86400.))-currentinfo->node.loc.utc);
                dtj = dt / 86400.;
                initialloc = currentinfo->node.loc;
                currentutc = currentinfo->node.loc.utc;
                initialphys = currentinfo->node.phys;
            }

            double dt;
            double dtj;
            double currentutc=0.;
            locstruc initialloc;
            physicsstruc initialphys;
            cosmosstruc *currentinfo;
            bool enable = true;

            enum Type
                {
                None = 0,
                PositionInertial = 10,
                PositionIterative = 11,
                PositionGaussJackson = 12,
                PositionGeo = 13,
                PositionTle = 14,
                PositionLvlh = 15,
                AttitudeInertial = 20,
                AttitudeIterative = 21,
                AttitudeLVLH = 22,
                AttitudeGeo = 23,
                AttitudeSolar = 24,
                AttitudeTarget = 25,
                AttitudeRequest = 26,
                Thermal = 30,
                Electrical = 40,
                OrbitalEvent = 50,
                Detector = 51,
                };
            Type type;

            int32_t Increment(double nextutc=0.);

        private:
        };

        class InertialPositionPropagator : public Propagator
        {
        public:
            InertialPositionPropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = PositionInertial;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class GeoPositionPropagator : public Propagator
        {
        public:
            GeoPositionPropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = PositionGeo;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class IterativePositionPropagator : public Propagator
        {
        public:
            IterativePositionPropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = PositionIterative;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class TlePositionPropagator : public Propagator
        {
        public:
            TlePositionPropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = PositionTle;
                Init();
            }

            int32_t Init(tlestruc tle);
            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class GaussJacksonPositionPropagator : public Propagator
        {
        public:
            uint16_t order;

            GaussJacksonPositionPropagator(cosmosstruc *newinfo, double idt, uint16_t iorder)
                : Propagator{ newinfo, idt }, order { iorder }
            {
                type = PositionGaussJackson;
                Setup();
            }
            int32_t Setup();
            int32_t Init(tlestruc tle);
            int32_t Init(vector<locstruc> locs);
            int32_t Init();
            int32_t Converge();
            int32_t Propagate(double nextutc=0., quaternion icrf={{0.,0.,0.},1.});
            int32_t Reset(double nextutc=0.);
            int32_t Update();

        private:

            struct gjstruc
            {
                rvector s;
                rvector ss;
                rvector sa;
                rvector sb;
                rvector tau;
                locstruc loc;
            };

            vector<vector <double>> a;
            vector<vector <double>> b;
            vector< vector<int32_t> > binom;
            vector<double> c;
            vector<double> gam;
            vector< vector<double> > beta;
            vector<double> q;
            vector<double> lam;
            vector< vector<double> > alpha;
            double dtsq;
            uint16_t order2;
            vector<gjstruc> step;
        };

        class LvlhPositionPropagator : public Propagator
        {
        public:
            LvlhPositionPropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = PositionLvlh;
            }

            int32_t Init();
            int32_t Init(cartpos lvlh);
            int32_t Propagate(locstruc &loc);
            int32_t Reset(locstruc& loc);

        private:
        };

        class InertialAttitudePropagator : public Propagator
        {
        public:
            InertialAttitudePropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = AttitudeInertial;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class GeoAttitudePropagator : public Propagator
        {
        public:
            GeoAttitudePropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = AttitudeGeo;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class IterativeAttitudePropagator : public Propagator
        {
        public:
            IterativeAttitudePropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = AttitudeIterative;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class LvlhAttitudePropagator : public Propagator
        {
        public:
            LvlhAttitudePropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = AttitudeLVLH;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class SolarAttitudePropagator : public Propagator
        {
        public:
            SolarAttitudePropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = AttitudeSolar;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
            Vector optimum{0.,0.,1.};
        };

        class TargetAttitudePropagator : public Propagator
        {
        public:
            TargetAttitudePropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = AttitudeTarget;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
            Vector optimum{0.,0.,1.};
        };

        class RequestAttitudePropagator : public Propagator
        {
        public:
            RequestAttitudePropagator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = AttitudeRequest;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
            Vector optimum{0.,0.,1.};
        };

        class ThermalPropagator : public Propagator
        {
        public:
            ThermalPropagator(cosmosstruc *newinfo, double idt, float itemperature)
                : Propagator{ newinfo, idt }, temperature{itemperature}
            {
                type = AttitudeLVLH;
                currentinfo->node.phys.temp = temperature;
                currentinfo->node.phys.heat = currentinfo->node.phys.temp * (currentinfo->node.phys.mass * currentinfo->node.phys.hcap);
                currentinfo->node.phys.radiation = SIGMA * pow(temperature, 4.);
                for (trianglestruc& triangle : currentinfo->node.phys.triangles)
                {
                    triangle.temp = temperature;
                    triangle.heat = triangle.temp * (triangle.mass * triangle.hcap);
                }
            }
            int32_t Init(float temp=300.);
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(float temp=0.);

            float temperature;
        };

        class ElectricalPropagator : public Propagator
        {
        public:
            ElectricalPropagator(cosmosstruc *newinfo, double idt, float ibattery_charge)
                : Propagator{ newinfo, idt }, battery_charge{ibattery_charge}
            {
                type = AttitudeLVLH;
            }

            int32_t Init(float bp=.5);
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(float bp=0.);

            float battery_charge;
        };

        // Generates orbital events and other events that arise naturally out of being in the space environment
        class OrbitalEventGenerator : public Propagator
        {
        public:
            OrbitalEventGenerator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {

            }
            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset();
            int32_t End();
        private:
            bool in_land = false;
            bool in_umbra = false;
            bool in_gs = false;
            bool in_targ = false;
            double time_start = 0.;
            double time_end = 0.;
            ElapsedTime time_alarm;
            //! Track when a land region starts
            double land_start = 0.;
            //! Track lat crossings
            int8_t lat_direction = 0;
            double alat_start = 0.;
            double dlat_start = 0.;
            double lastlat = 0.;
            double minlat_start = 0.;
            double maxlat = 0.;
            double minlat = 0.;
            //! Track when an umbra region starts
            double umbra_start = 0.;
//            //! Reference to target list in cosmosstruc
//            vector<targetstruc>& targets;
//            //! Reference to event list in cosmosstruc, orbital/physical events are appended to this as they occur
//            vector<eventstruc>& events;
            //! Each AoS tracks the time it started and the max value
            using aos_pair = std::pair<double, float>;
            //! Each target tracks 4 events: 0 deg AoS/LoS, 5 deg AoS/LoS, 10 deg AoS/LoS, max deg AoS/LoS
            using target_aos_set = std::array<aos_pair, 4>;
            //! Use these to access the 3 event types in aos_set
            const uint8_t DEG0 = 0;
            const uint8_t DEG5 = 1;
            const uint8_t DEG10 = 2;
            const uint8_t DEGMAX = 3;
            const uint32_t GS_EVENT_CODE[4] = {EVENT_TYPE_GS, EVENT_TYPE_GS5, EVENT_TYPE_GS10, EVENT_TYPE_GSMAX};
            const string GS_EVENT_STRING[4] = {"GS", "GS5", "GS10", "GSMAX"};
            //! Tracks Acquisition of Signal events for each groundstation
            map<string, target_aos_set> gs_AoS;
            //! Tracks Acquisition of Sight events for each target. Targets track only DEG0
            map<string, targetstruc> target_AoS;

            int32_t check_all_event(bool force_end);
            int32_t check_lat_event(bool force_end, float lat=0.0);
            //! Checks for Umbra enter/exit event
            //! \param final If true, forces end of event if active
            int32_t check_umbra_event(bool force_end);
            //! Iterates over targets, then calls gs or target function depending on the type
            int32_t check_target_events(bool force_end);
            int32_t check_land_event(bool force_end);
            int32_t check_time_event(bool force_end);
            //! Checks for groundstation AoS/LoS events
            //! \param gs Reference to groundstation to check
            //! \param final If true, forces end of event if active
            int32_t check_gs_aos_event(const targetstruc& gs, bool force_end);
            //! Checks for target AoS/LoS events
            //! \param target Reference to target to check
            //! \param final If true, forces end of event if active
            int32_t check_target_event(const targetstruc& target, bool force_end);
        };

        class MetricGenerator : public Propagator
        {
        public:
            MetricGenerator(cosmosstruc *newinfo, double idt)
                : Propagator{ newinfo, idt }
            {
                type = Detector;
            }

//            vector<targetstruc>& targets;
//            vector<camstruc>& detectors;
            vector<vector<Physics::coverage>> coverage;

            int32_t Init();
//            int32_t AddDetector(float fov, float ifov, float specmin, float specmax);
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

            struct detector
            {
                float fov;
                float ifov;
                float specmin;
                float specmax;
                float area;
                float resolution;
                double clat;
                double clon;
            };
        };

        class State
        {
        public:
            State(string node) : propagation_priority(0)
            {
                json_init(&currentinfo, node);
            }

            State(string node, uint8_t propagation_priority) : propagation_priority(propagation_priority)
            {
                json_init(&currentinfo, node);
            }

            //! States in a Simulation are propagated in ascending order, this preserves any
            //! dependencies that any States that must update after other have (e.g., mothership vs childsats)
            const uint8_t propagation_priority = 0;


            cosmosstruc currentinfo;
//            uint16_t targetidx = -1;

            string sohstring;

            locstruc initialloc;

            physicsstruc initialphys;

            double dt;
            double dtj;

//            vector<targetstruc> targets;

            Propagator::Type ptype;
            InertialPositionPropagator *inposition;
            GeoPositionPropagator *geoposition;
            IterativePositionPropagator *itposition;
            GaussJacksonPositionPropagator *gjposition;
            TlePositionPropagator *tleposition;
            LvlhPositionPropagator *lvlhposition;

            Propagator::Type atype;
            InertialAttitudePropagator *inattitude;
            IterativeAttitudePropagator *itattitude;
            LvlhAttitudePropagator *lvattitude;
            GeoAttitudePropagator *geoattitude;
            SolarAttitudePropagator *solarattitude;
            TargetAttitudePropagator *targetattitude;
            RequestAttitudePropagator *requestattitude;

            Propagator::Type ttype;
            ThermalPropagator *thermal;


            Propagator::Type etype;
            ElectricalPropagator *electrical;
//            tlestruc tle;

            OrbitalEventGenerator *orbitalevent;
            MetricGenerator *metric;

            string stype;
            Structure *structure;

            int32_t Init(string name, double idt, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype);
            int32_t Init(string name, double idt, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, tlestruc tle, double utc, qatt icrf=qatt());
            int32_t Init(string name, double idt, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, cartpos eci, qatt icrf=qatt());
            int32_t Init(string name, double idt, string stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, cartpos eci, cartpos lvlh, qatt icrf=qatt());
            int32_t Propagate(double nextutc=0.);
            int32_t Propagate(locstruc& nextloc);
            //! Propagates simulated physical state to the next timestep
            //! Runs any code that the propagators need to run at the end of a simulation run
            int32_t Update();
            int32_t End();
            int32_t Reset(double nextutc=0.);
        };


//        double Rearth(double lat);
        double Msis00Density(posstruc pos,float f107avg,float f107,float magidx);
        Vector GravityAccel(posstruc pos, uint16_t model, uint32_t degree);
        int32_t GravityParams(int16_t model);
        double Nplgndr(uint32_t l, uint32_t m, double x);

        int32_t PosAccel(locstruc &loc, physicsstruc &physics);
        int32_t PosAccel(locstruc* loc, physicsstruc* physics);
        int32_t AttAccel(locstruc &loc, physicsstruc &physics);
        int32_t AttAccel(locstruc* loc, physicsstruc* physics);
        int32_t PhysSetup(physicsstruc *phys);
        int32_t PhysCalc(locstruc* loc, physicsstruc *phys);



        locstruc shape2eci(double utc, double altitude, double angle, double timeshift);
        locstruc shape2eci(double utc, double latitude, double longitude, double altitude, double angle, double timeshift);
        int32_t load_loc(string fname, locstruc& loc);
        Vector ControlAlpha(qatt tatt, qatt catt, double seconds);
        Vector ControlTorque(qatt tatt, qatt catt, Vector moi, double seconds);
        Vector ControlAccel(cartpos cpos, cartpos tpos, double maxaccel, double seconds);
        Vector ControlThrust(cartpos cpos, cartpos tpos, double mass, double maxaccel, double seconds);


    } //end of namespace Physics
} // end of namespace Cosmos

#endif // PHYSICSCLASS_H
