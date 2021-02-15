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
#include "support/jsondef.h"
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
            physicsstruc *newphys;

            enum Type
                {
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
                None = 0,
                X,
                Y,
                XY
                };

            Structure(physicsstruc *physp) : newphys{physp}
            {

            }

            int32_t Setup(Type type);
            int32_t add_u(double x, double y, double z, ExternalPanelType type);
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
            double dt;
            double dtj;
            locstruc oldloc;
            locstruc *newloc;
            physicsstruc oldphys;
            physicsstruc *newphys;

            enum Type
                {
                None = 0,
                PositionInertial = 10,
                PositionIterative = 11,
                PositionGaussJackson = 12,
                AttitudeInertial = 20,
                AttitudeIterative = 21,
                AttitudeLVLH = 22,
                Thermal = 30,
                Electrical = 40
                };
            Type type;

            Propagator(locstruc *locp, physicsstruc *physp, double idt) : newloc{locp}, newphys{physp}
            {
                dt = 86400.*((locp->utc + (idt / 86400.))-locp->utc);
                dtj = dt / 86400.;
            }

            int32_t Increment(double nextutc);

        private:
        };

        class InertialPositionPropagator : public Propagator
        {
        public:
            InertialPositionPropagator(locstruc *locp, physicsstruc *physp, double idt)
                : Propagator{ locp, physp, idt }
            {
                type = PositionInertial;
            }

            int32_t Init();
            int32_t Propagate();

        private:
        };

        class IterativePositionPropagator : public Propagator
        {
        public:
            IterativePositionPropagator(locstruc *locp, physicsstruc *physp, double idt)
                : Propagator{ locp, physp, idt }
            {
                type = PositionIterative;
            }

            int32_t Init();
            int32_t Propagate();

        private:
        };

        class GaussJacksonPositionPropagator : public Propagator
        {
        public:
            uint16_t order;

            GaussJacksonPositionPropagator(locstruc *locp, physicsstruc *physp, double idt, uint16_t iorder)
                : Propagator{ locp, physp, idt }, order { iorder }
            {
                type = PositionGaussJackson;
                Setup(iorder);
            }
            int32_t Setup(uint16_t order);
            int32_t Init(vector<tlestruc> lines);
            int32_t Init(vector<locstruc> locs);
            int32_t Init();
            int32_t Converge();
            int32_t Propagate();

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
                locstruc loc;
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
            InertialAttitudePropagator(locstruc *locp, physicsstruc *physp, double idt)
                : Propagator{ locp, physp, idt }
            {
                type = AttitudeInertial;
            }

            int32_t Init();
            int32_t Propagate();

        private:
        };

        class IterativeAttitudePropagator : public Propagator
        {
        public:
            IterativeAttitudePropagator(locstruc *locp, physicsstruc *physp, double idt)
                : Propagator{ locp, physp, idt }
            {
                type = AttitudeIterative;
            }

            int32_t Init();
            int32_t Propagate();

        private:
        };

        class LVLHAttitudePropagator : public Propagator
        {
        public:
            LVLHAttitudePropagator(locstruc *locp, physicsstruc *physp, double idt)
                : Propagator{ locp, physp, idt }
            {
                type = AttitudeLVLH;
            }

            int32_t Init();
            int32_t Propagate();

        private:
        };

        class ThermalPropagator : public Propagator
        {
        public:
            ThermalPropagator(locstruc *locp, physicsstruc *physp, double idt, float itemperature)
                : Propagator{ locp, physp, idt }, temperature{itemperature}
            {
                type = AttitudeLVLH;
                newphys->temp = temperature;
                newphys->heat = newphys->temp * (newphys->mass * newphys->hcap);
                newphys->radiation = SIGMA * pow(temperature, 4.);
                for (trianglestruc& triangle : newphys->triangles)
                {
                    triangle.temp = temperature;
                    triangle.heat = triangle.temp * (triangle.mass * triangle.hcap);
                }
            }
            int32_t Init(float temp);
            int32_t Propagate();

            float temperature;
        };

        class ElectricalPropagator : public Propagator
        {
        public:
            ElectricalPropagator(locstruc *locp, physicsstruc *physp, double idt, float ibattery_charge)
                : Propagator{ locp, physp, idt }, battery_charge{ibattery_charge}
            {
                type = AttitudeLVLH;
            }

            int32_t Init(float bp);
            int32_t Propagate();

            float battery_charge;
        };

        class State
        {
        public:
            locstruc oldloc;
            locstruc *newloc;
            physicsstruc oldphys;
            physicsstruc *newphys;
            double dt;
            double dtj;
            Propagator::Type ptype;
            InertialPositionPropagator *inposition;
            IterativePositionPropagator *itposition;
            GaussJacksonPositionPropagator *gjposition;

            Propagator::Type atype;
            InertialAttitudePropagator *inattitude;
            IterativeAttitudePropagator *itattitude;
            LVLHAttitudePropagator *lvattitude;

            Propagator::Type ttype;
            ThermalPropagator *thermal;

            Propagator::Type etype;
            ElectricalPropagator *electrical;
            vector<tlestruc> tle;

            Structure::Type stype;
            Structure *structure;

            State();
            int32_t Init(Propagator *posprop, Propagator *attprop, Propagator *thermprop, Propagator *elecprop);
            int32_t Init(Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, locstruc *loc, physicsstruc *phys, double idt, vector<tlestruc> lines);
            int32_t Init(Structure::Type stype, Propagator::Type ptype, Propagator::Type atype, Propagator::Type ttype, Propagator::Type etype, locstruc *loc, physicsstruc *phys, double idt);
            int32_t Increment(double nextutc=0.);
        };


        double Msis00Density(posstruc pos,float f107avg,float f107,float magidx);
        Vector GravityAccel(posstruc pos, uint16_t model, uint32_t degree);
        int32_t GravityParams(int16_t model);
        double Nplgndr(uint32_t l, uint32_t m, double x);

        int32_t PosAccel(locstruc* loc, physicsstruc* physics);
        int32_t AttAccel(locstruc* loc, physicsstruc* physics);
        int32_t PhysSetup(physicsstruc *phys);
        int32_t PhysCalc(locstruc* loc, physicsstruc *phys);

        locstruc shape2eci(double utc, double altitude, double angle, double hour);


    } //end of namespace Physics
} // end of namespace Cosmos

#endif // PHYSICSCLASS_H
