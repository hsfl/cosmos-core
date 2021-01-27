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

#include "support/configCosmos.h"
#include "physics/physicsclass.h"
#include "physics/physicslib.h"

int main(int argc, char *argv[])
{
    double initialutc;
    double dp = 60.;
    double endp = 86400.;
    int32_t iretn = 0;
    gj_handle gjh;
//    physicsstruc gphys;
//    locstruc gloc;
//    int32_t iretn;
    vector<tlestruc>lines;

    Physics::State *state;
//    Physics::GaussJacksonPositionPropagator *posprop;
//    Physics::LVLHAttitudePropagator *attprop;
//    Physics::ThermalPropagator *thermprop;
//    Physics::ElectricalPropagator *elecprop;
    locstruc loc;
    physicsstruc phys;

    state = new Physics::State();
//    posprop = new Physics::GaussJacksonPositionPropagator(&loc, &phys, 1., 6);
//    attprop = new Physics::LVLHAttitudePropagator(&loc, &phys, 1.);
//    thermprop = new Physics::ThermalPropagator(&loc, &phys, 1., 300.);
//    elecprop = new Physics::ElectricalPropagator(&loc, &phys, 1., 40.);

    loc_clear(loc);
    loc.utc = currentmjd();
    switch (argc)
    {
    case 5:
        endp = atof(argv[4]);
    case 4:
        dp = atof(argv[3]);
    case 3:
        loc.utc = atof(argv[2]);
    case 2:
        {
            string fname = argv[1];
            if (fname.find(".tle") != string::npos)
            {
                iretn = load_lines(fname, lines);
                state->Init(Physics::Structure::Type::U6, Physics::Propagator::Type::PositionGaussJackson, Physics::Propagator::Type::AttitudeLVLH, Physics::Propagator::Type::Thermal, Physics::Propagator::Type::Electrical, &loc, &phys, dp/10., lines);
            }
        }
        break;
    case 8:
        loc.utc = atof(argv[1]);
        loc.pos.eci.s.col[0] = atof(argv[2]);
        loc.pos.eci.s.col[1] = atof(argv[3]);
        loc.pos.eci.s.col[2] = atof(argv[4]);
        loc.pos.eci.v.col[0] = atof(argv[5]);
        loc.pos.eci.v.col[1] = atof(argv[6]);
        loc.pos.eci.v.col[2] = atof(argv[7]);
        loc.pos.eci.utc = loc.utc;
        ++loc.pos.eci.pass;
        state->Init(Physics::Structure::Type::U6, Physics::Propagator::Type::PositionGaussJackson, Physics::Propagator::Type::AttitudeLVLH, Physics::Propagator::Type::Thermal, Physics::Propagator::Type::Electrical, &loc, &phys, dp/10., lines);
    }

//    locstruc cloc = loc;
    initialutc = loc.utc;
    for (double second=0.; second<endp; ++second)
    {
        state->Increment(initialutc + second / 86400.);
//        posprop->Propagate();
//        attprop->Propagate();
//        thermprop->Propagate();
//        elecprop->Propagate();
        if (second == dp*static_cast<int32_t>(second/(dp)))
        {
            printf("%s %10f %10f %10f %7f %7f %7f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n"
                   , mjd2iso8601(loc.utc).c_str(),DEGOF(loc.pos.geod.s.lat), DEGOF(loc.pos.geod.s.lon)
                   , loc.pos.geod.s.h, DEGOF(loc.pos.geod.v.lat), DEGOF(loc.pos.geod.v.lon), loc.pos.geod.v.h
                   , loc.pos.eci.s.col[0], loc.pos.eci.s.col[1], loc.pos.eci.s.col[2]
                    , loc.pos.eci.v.col[0], loc.pos.eci.v.col[1], loc.pos.eci.v.col[2]
                    , loc.pos.eci.a.col[0], loc.pos.eci.a.col[1], loc.pos.eci.a.col[2]
                    , phys.temp, phys.radiation);
            fflush(stdout);
        }
    }
	return iretn;
}
