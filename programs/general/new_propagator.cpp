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
    gj_handle gjh;
    physicsstruc gphys;
    locstruc gloc;
    int32_t iretn;
    vector<tlestruc>lines;

    Physics::State *state;
    Physics::GaussJacksonPositionPropagator *posprop;
    Physics::LVLHAttitudePropagator *attprop;
    Physics::ThermalPropagator *thermprop;
    Physics::ElectricalPropagator *elecprop;
    locstruc loc;
    Physics::physicstruc phys;

    posprop = new Physics::GaussJacksonPositionPropagator(&loc, &phys, 10., 6);
    posprop->Setup(6);
    attprop = new Physics::LVLHAttitudePropagator(&loc, &phys, 10.);
    thermprop = new Physics::ThermalPropagator(&loc, &phys, 10., 300.);
    elecprop = new Physics::ElectricalPropagator(&loc, &phys, 10., 40.);

    loc_clear(loc);
    loc.utc = currentmjd();
    switch (argc)
    {
    case 3:
        loc.utc = atof(argv[2]);
    case 2:
        {
            string fname = argv[1];
            if (fname.find(".tle") != string::npos)
            {
                iretn = load_lines(fname, lines);
                posprop->Init(lines);
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
        posprop->Init();
    }

    gauss_jackson_init_eci(gjh, 6, 1, 10., loc.utc, loc.pos.eci, loc.att.icrf, gphys, gloc);

    double cmjd = currentmjd();
    locstruc cloc = loc;
    for (uint16_t hour=0; hour<24; ++hour)
    {
        for (uint16_t minute=0; minute<60; ++minute)
        {
            double seconds = loc.utc-cmjd;
            double distance = norm_rv(rv_sub(loc.pos.geoc.s, cloc.pos.geoc.s));
            cartpos neweci;
            lines2eci(loc.utc, lines, neweci);
            double ecidistance = norm_rv(rv_sub(loc.pos.eci.s, neweci.s));
            printf("%s %10f %10f %10f %10f %7f %7f %7f\n", mjd2iso8601(loc.utc).c_str(), ecidistance, DEGOF(loc.pos.geod.s.lat), DEGOF(loc.pos.geod.s.lon), loc.pos.geod.s.h, DEGOF(loc.pos.geod.v.lat), DEGOF(loc.pos.geod.v.lon), loc.pos.geod.v.h);
            fflush(stdout);
            cloc = loc;
            for (uint16_t i=0; i<6; ++i)
            {
                posprop->Propagate();
                gauss_jackson_propagate(gjh, gphys, gloc, loc.utc);
                attprop->Propagate();
                thermprop->Propagate();
                elecprop->Propagate();
            }

        }
    }

}
