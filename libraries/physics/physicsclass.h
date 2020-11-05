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
#include "support/nrlmsise-00.h"
#include "support/datalib.h"


namespace Cosmos
{
    namespace Physics
    {
        class Propagator
        {
        public:
            locstruc *loc;
            physicsstruc *phys;

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

            Propagator(locstruc *locp, physicsstruc *physp) : loc{locp}, phys{physp}
            {
                phys->dt = 86400.*((locp->utc + (phys->dt / 86400.))-locp->utc);
                phys->dtj = phys->dt / 86400.;
                phys->utc = locp->utc;
            }

            int32_t Increment(double nextutc);

        private:
        };

        class InertialPositionPropagator : public Propagator
        {
        public:
            InertialPositionPropagator(locstruc *locp, physicsstruc *physp)
                : Propagator{ locp, physp }
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
            IterativePositionPropagator(locstruc *locp, physicsstruc *physp)
                : Propagator{ locp, physp }
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

            GaussJacksonPositionPropagator(locstruc *locp, physicsstruc *physp, uint16_t iorder)
                : Propagator{ locp, physp }, order { iorder }
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
            InertialAttitudePropagator(locstruc *locp, physicsstruc *physp)
                : Propagator{ locp, physp }
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
            IterativeAttitudePropagator(locstruc *locp, physicsstruc *physp)
                : Propagator{ locp, physp }
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
            LVLHAttitudePropagator(locstruc *locp, physicsstruc *physp)
                : Propagator{ locp, physp }
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
            ThermalPropagator(locstruc *locp, physicsstruc *physp, float itemperature)
                : Propagator{ locp, physp }, temperature{itemperature}
            {
                type = AttitudeLVLH;
            }

            int32_t Init(float temp);
            int32_t Propagate();

            float temperature;
        };

        class ElectricalPropagator : public Propagator
        {
        public:
            ElectricalPropagator(locstruc *locp, physicsstruc *physp, float ibattery_charge)
                : Propagator{ locp, physp }, battery_charge{ibattery_charge}
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
            locstruc *loc;
            physicsstruc *phys;
//            double dt;
//            double dtj;
            Propagator *position;
            Propagator *attitude;
            Propagator *thermal;
            Propagator *electrical;

            State();
            int32_t Init(Propagator *posprop, Propagator *attprop, Propagator *thermprop, Propagator *elecprop);
            int32_t Increment(double nextutc);
        };


        double Msis00Density(posstruc pos,float f107avg,float f107,float magidx);
        Vector GravityAccel(posstruc pos, uint16_t model, uint32_t degree);
        int32_t GravityParams(int16_t model);
        double Nplgndr(uint32_t l, uint32_t m, double x);

        int32_t PosAccel(locstruc &loc, physicsstruc &physics);
        int32_t AttAccel(locstruc &loc, physicsstruc &physics);

        enum StructureType
            {
            None = 0,
            Cube1a = 1,
            Cube3a = 2,
            Cube6a = 3,
            Cube6b = 4
            };

        int32_t InitializePhysics(physicsstruc &physics, float imass=1., float itemp=300., float ibatt=36000., Vector imoi=Vector(1.,1.,1.), float ihcap=900., double idt=1.);
        int32_t UpdatePhysics(physicsstruc &physics, locstruc &location);
        int32_t InitializeStructure(physicsstruc &physics, StructureType type, float abs=.65f, float emi=.9f);
        int32_t UpdateStructure(physicsstruc &physics);

    } //end of namespace Physics
} // end of namespace Cosmos

#endif // PHYSICSCLASS_H
