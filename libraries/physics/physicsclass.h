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
#include "support/convertdef.h"


namespace Cosmos
{
    namespace Physics
    {
        class Position
        {
        public:
            Position();

//            struct statestruc
//            {
//                double utc;
//                cartpos icrf;
//                cartpos eci;
//                cartpos sci;
//                cartpos geoc;
//                cartpos selc;
//                geoidpos geod;
//                geoidpos selg;
//                spherpos geos;
//                extrapos extra;
//                //! Separation between sun/satellite and sun/limbofearth vectors in radians
//                float earthsep;
//                //! Separation between sun/satellite and sun/limbofmoon vectors in radians
//                float moonsep;
//                //! Radius of sun in radians
//                float sunsize;
//                //! Watts per square meter per steradian
//                float sunradiance;
//                //! Earth magnetic vector in ITRS for this time and location.
//                Vector bearth;
//            };

        private:
            posstruc state;
        };

        class Attitude
        {
        public:
            Attitude();

//            struct statestruc
//            {
//                double utc;
//                qatt topo;
//                qatt lvlh;
//                qatt geoc;
//                qatt selc;
//                qatt icrf;
//                extraatt extra;
//            };

        private:
            attstruc state;
        };

        class Thermal
        {
        public:
            Thermal();
        };

        class Electrical
        {
        public:
            Electrical();
        };

        class General
        {
        public:
            General();

        private:
            struct generalstruc
            {
            };
        };

        class Summary
        {
        public:
            Summary();

        private:
            Position position;
            Attitude attitude;
            Thermal thermal;
            Electrical electrical;
            General general;
        };
    } //end of namespace Physics
} // end of namespace Cosmos

#endif // PHYSICSCLASS_H
