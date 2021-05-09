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
                U12XY
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

            int32_t Setup(Type type);
            int32_t add_u(double x=1, double y=1, double z=1, ExternalPanelType type=NoPanel);
            int32_t add_cuboid(string name, Vector size, double depth, Quaternion orientation, Vector offset);
            int32_t add_face(string name, Vector point0, Vector point1, Vector point2, Vector point3, double depth, uint8_t external=1, float pcell=.85, Quaternion orientation=Math::Quaternions::eye(), Vector offset=Vector());
            int32_t add_face(string name, Vector size, Quaternion orientation, Vector offset);
            int32_t add_triangle(Vector pointa, Vector pointb, Vector pointc, double depth, bool external=true, float pcell=.85);
            int32_t add_vertex(Vector point);

        private:
            Type type;

        };

        class Propagator
        {
        public:
            Propagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt) : currentloc{newloc}, currentphys{newphys}
            {
                dt = 86400.*((currentloc->utc + (idt / 86400.))-currentloc->utc);
                dtj = dt / 86400.;
                initialloc = *currentloc;
                currentutc = currentloc->utc;
                initialphys = *currentphys;
            }

            double dt;
            double dtj;
            double currentutc=0.;
            Convert::locstruc initialloc;
            Convert::locstruc *currentloc;
            physicsstruc initialphys;
            physicsstruc *currentphys;

            enum Type
                {
                None = 0,
                PositionInertial = 10,
                PositionIterative = 11,
                PositionGaussJackson = 12,
                PositionGeo = 13,
                AttitudeInertial = 20,
                AttitudeIterative = 21,
                AttitudeLVLH = 22,
                AttitudeGeo = 23,
                Thermal = 30,
                Electrical = 40
                };
            Type type;

            int32_t Increment(double nextutc=0.);

        private:
        };

        class InertialPositionPropagator : public Propagator
        {
        public:
            InertialPositionPropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt)
                : Propagator{ newloc, newphys, idt }
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
            GeoPositionPropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt)
                : Propagator{ newloc, newphys, idt }
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
            IterativePositionPropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt)
                : Propagator{ newloc, newphys, idt }
            {
                type = PositionIterative;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class GaussJacksonPositionPropagator : public Propagator
        {
        public:
            uint16_t order;

            GaussJacksonPositionPropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt, uint16_t iorder)
                : Propagator{ newloc, newphys, idt }, order { iorder }
            {
                type = PositionGaussJackson;
                Setup(iorder);
            }
            int32_t Setup(uint16_t order);
            int32_t Init(vector<Convert::tlestruc> lines);
            int32_t Init(vector<Convert::locstruc> locs);
            int32_t Init();
            int32_t Converge();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:

            struct gjstruc
            {
                vector <double> a;
                vector <double> b;
                rvector s;
                rvector ss;
                rvector sa;
                rvector sb;
                rvector tau;
                Convert::locstruc loc;
            };

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

        class InertialAttitudePropagator : public Propagator
        {
        public:
            InertialAttitudePropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt)
                : Propagator{ newloc, newphys, idt }
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
            GeoAttitudePropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt)
                : Propagator{ newloc, newphys, idt }
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
            IterativeAttitudePropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt)
                : Propagator{ newloc, newphys, idt }
            {
                type = AttitudeIterative;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class LVLHAttitudePropagator : public Propagator
        {
        public:
            LVLHAttitudePropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt)
                : Propagator{ newloc, newphys, idt }
            {
                type = AttitudeLVLH;
            }

            int32_t Init();
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);

        private:
        };

        class ThermalPropagator : public Propagator
        {
        public:
            ThermalPropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt, float itemperature)
                : Propagator{ newloc, newphys, idt }, temperature{itemperature}
            {
                type = AttitudeLVLH;
                currentphys->temp = temperature;
                currentphys->heat = currentphys->temp * (currentphys->mass * currentphys->hcap);
                currentphys->radiation = SIGMA * pow(temperature, 4.);
                for (trianglestruc& triangle : currentphys->triangles)
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
            ElectricalPropagator(Convert::locstruc *newloc, physicsstruc *newphys, double idt, float ibattery_charge)
                : Propagator{ newloc, newphys, idt }, battery_charge{ibattery_charge}
            {
                type = AttitudeLVLH;
            }

            int32_t Init(float bp=.5);
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(float bp=0.);

            float battery_charge;
        };

        class State
        {
        public:
            State()
            {
                json_init(&currentinfo);
                currentinfo.node.utc = 0.;
            }

//            string nodename;
            cosmosstruc currentinfo;
//            double currentutc=0.;
            Convert::locstruc initialloc;
//            Convert::locstruc currentloc;
            physicsstruc initialphys;
//            physicsstruc currentphys;
            double dt;
            double dtj;
            Propagator::Type ptype;
            InertialPositionPropagator *inposition;
            GeoPositionPropagator *geoposition;
            IterativePositionPropagator *itposition;
            GaussJacksonPositionPropagator *gjposition;

            Propagator::Type atype;
            InertialAttitudePropagator *inattitude;
            IterativeAttitudePropagator *itattitude;
            LVLHAttitudePropagator *lvattitude;
            GeoAttitudePropagator *geoattitude;

            Propagator::Type ttype;
            ThermalPropagator *thermal;

            Propagator::Type etype;
            ElectricalPropagator *electrical;
            vector<Convert::tlestruc> tle;

            Structure::Type stype;
            Structure *structure;

//            int32_t Init(Propagator *posprop, Propagator *attprop, Propagator *thermprop, Propagator *elecprop);
            int32_t Init(string name, double idt, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, vector<Convert::tlestruc> lines, double utc);
            int32_t Init(string name, double idt, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::posstruc pos);
            int32_t Init(string name, double idt, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, Convert::locstruc loc);
            int32_t Init(string name, double idt, Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype);
            int32_t Propagate(double nextutc=0.);
            int32_t Reset(double nextutc=0.);
        };


        double Msis00Density(Convert::posstruc pos,float f107avg,float f107,float magidx);
        Vector GravityAccel(Convert::posstruc pos, uint16_t model, uint32_t degree);
        int32_t GravityParams(int16_t model);
        double Nplgndr(uint32_t l, uint32_t m, double x);

        int32_t PosAccel(Convert::locstruc &loc, physicsstruc &physics);
        int32_t PosAccel(Convert::locstruc* loc, physicsstruc* physics);
        int32_t AttAccel(Convert::locstruc &loc, physicsstruc &physics);
        int32_t AttAccel(Convert::locstruc* loc, physicsstruc* physics);
        int32_t PhysSetup(physicsstruc *phys);
        int32_t PhysCalc(Convert::locstruc* loc, physicsstruc *phys);

        Convert::locstruc shape2eci(double utc, double altitude, double angle, double timeshift);
        Convert::locstruc shape2eci(double utc, double latitude, double longitude, double altitude, double angle, double timeshift);


    } //end of namespace Physics
} // end of namespace Cosmos

#endif // PHYSICSCLASS_H
