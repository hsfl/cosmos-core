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
#include "support/convertlib.h"
#include "support/timelib.h"
#include "support/nrlmsise-00.h"
#include "support/datalib.h"


namespace Cosmos
{
    namespace Physics
    {
        //! Finite Triangle Element
        //! Holds minimum information necessary to use smallest possible triangular element
        //! of a larger piece.
        struct trianglestruc
        {
            //! center of mass
            Vector com;
            //! outward facing normal
            Vector normal;
            //! Area
            float area;
            //! Index to parent piece
            uint16_t pidx;
            uint16_t tidx[3];
            float heat;
            float temp;
            float irradiance;
            float cell_percentage;
            vector<vector<size_t>> triangleindex;
        };

        //! Propagator Simulation Structure
        /*! Holds parameters used specifically for the physical simulation of the
     * environment and hardware of a Node.
    */
        struct physicstruc
        {
            //! Time step in seconds
            double dt;
            //! Time step in Julian days
            double dtj;
            //! Simulated starting time in MJD
            double mjdbase;
            //! Acceleration factor for simulated time
            double mjdaccel;
            //! Offset factor for simulated time (simtime = mjdaccel * realtime + mjddiff)
            double mjddiff;
            //! Simulation mode as listed in \def defs_physics
            int32_t mode;
            Vector ftorque;
            Vector atorque;
            Vector rtorque;
            Vector gtorque;
            Vector htorque;
            Vector hmomentum;
            Vector ctorque;
            Vector fdrag;
            Vector adrag;
            Vector rdrag;
            Vector thrust;
            Vector moi = Vector(1.,1.,1.);
            Vector com;
            float heat = 300. * 900. * 1.;
            float hcap = 900.;
            float charge;
            float mass = 1.;
            float area = .001f;
            vector <Vector> vertices;
            vector <trianglestruc> faces;
        } ;

//        class GaussJacksonPropagator
//        {
//        public:
//            GaussJacksonPropagator(uint16_t order, double iutc, double dt);
//            void Init(locstruc iloc, physicstruc iphys);
//            void Init(double iutc, vector<tlestruc>lines, attstruc att, physicstruc phys);
//            void Init(double iutc, vector <cartpos> ipos, vector <qatt> iatt, physicstruc phys);
//            int32_t Propagate();
//            int32_t Converge();

//        private:
//            locstruc loc;
//            physicstruc phys;

//            //! Gauss Jackson Integration structure
//            /*! Holds the working variables for one step of an order N Gauss Jackson
//             * Integration. A complete integration will require an array of N+2.
//            */
//            struct gjstruc
//            {
//                vector <double> a;
//                vector <double> b;
//                rvector s;
//                rvector ss;
//                rvector sa;
//                rvector sb;
//                rvector tau;
//                locstruc loc;
//            };

//            vector< vector<int32_t> > binom;
//            vector<double> c;
//            vector<double> gam;
//            vector< vector<double> > beta;
//            vector<double> q;
//            vector<double> lam;
//            vector< vector<double> > alpha;
//            double dt;
//            double dtj;
//            double dtsq;
//            uint32_t order;
//            uint32_t order2;
//            vector<gjstruc> step;
//        };

        class Propagator
        {
        public:
            double dt;
            double dtj;
            locstruc *loc;
            physicstruc *phys;

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

            Propagator(locstruc *locp, physicstruc *physp, double idt) : loc{locp}, phys{physp}
            {
//                loc = locp;
//                phys = physp;
                dt = 86400.*((locp->utc + (idt / 86400.))-locp->utc);
                dtj = dt / 86400.;
//                type = itype;
            }

            int32_t Increment(double nextutc);

        private:
        };

        class InertialPositionPropagator : public Propagator
        {
        public:
            InertialPositionPropagator(locstruc *locp, physicstruc *physp, double idt)
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
            IterativePositionPropagator(locstruc *locp, physicstruc *physp, double idt)
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

            GaussJacksonPositionPropagator(locstruc *locp, physicstruc *physp, double idt, uint16_t iorder)
                : Propagator{ locp, physp, idt }, order { iorder }
            {
                type = PositionGaussJackson;
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
            InertialAttitudePropagator(locstruc *locp, physicstruc *physp, double idt)
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
            IterativeAttitudePropagator(locstruc *locp, physicstruc *physp, double idt)
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
            LVLHAttitudePropagator(locstruc *locp, physicstruc *physp, double idt)
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
            ThermalPropagator(locstruc *locp, physicstruc *physp, double idt, float itemperature)
                : Propagator{ locp, physp, idt }, temperature{itemperature}
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
            ElectricalPropagator(locstruc *locp, physicstruc *physp, double idt, float ibattery_charge)
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
            locstruc *loc;
            physicstruc *phys;
            double dt;
            double dtj;
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

        int32_t PosAccel(locstruc &loc, physicstruc &physics);
        int32_t AttAccel(locstruc &loc, physicstruc &physics);


    } //end of namespace Physics
} // end of namespace Cosmos

#endif // PHYSICSCLASS_H
