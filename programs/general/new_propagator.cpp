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
#include "support/ephemlib.h"

int main(int argc, char *argv[])
{
    float hcap = 900.;
    float  mass = 1.;
    float temp = 300.;
    float batt = 18000.;
    Vector moi(.0075, .036, .036);

    gj_handle gjh;
//    physicsstruc gphys;
//    locstruc gloc;
//    int32_t iretn;
    vector<tlestruc>lines;

//    Physics::State *state;
    Physics::GaussJacksonPositionPropagator *posprop;
    Physics::LVLHAttitudePropagator *attprop;
    Physics::ThermalPropagator *thermprop;
    Physics::ElectricalPropagator *elecprop;
    locstruc loc;
    physicsstruc phys;


    Physics::InitializePhysics(phys, mass, temp, batt, moi, hcap, 10.);
    Physics::InitializeStructure(phys, Physics::StructureType::Cube3a);
    posprop = new Physics::GaussJacksonPositionPropagator(&loc, &phys, 6);
    attprop = new Physics::LVLHAttitudePropagator(&loc, &phys);
    thermprop = new Physics::ThermalPropagator(&loc, &phys, 300.);
    elecprop = new Physics::ElectricalPropagator(&loc, &phys, 40.);

    targetstruc kccgs;
    kccgs.loc.pos.geod.s.lat = 0.383497357;
    kccgs.loc.pos.geod.s.lon = -2.782040341;
    kccgs.loc.pos.geod.s.h = 110.;
    kccgs.loc.pos.geod.v.lat = 0.;
    kccgs.loc.pos.geod.v.lon = 0.;
    kccgs.loc.pos.geod.v.h = 0.;
    ++kccgs.loc.pos.geod.pass;
    pos_geod(kccgs.loc);

    targetstruc mc3gs;
    mc3gs.loc.pos.geod.s.lat = 0.383497357;
    mc3gs.loc.pos.geod.s.lon = -2.782040341;
    mc3gs.loc.pos.geod.s.h = 110.;
    mc3gs.loc.pos.geod.v.lat = 0.;
    mc3gs.loc.pos.geod.v.lon = 0.;
    mc3gs.loc.pos.geod.v.h = 0.;
    ++mc3gs.loc.pos.geod.pass;
    pos_geod(mc3gs.loc);

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
//                iretn = load_lines(fname, lines);
                posprop->Init(lines);
            }
        }
        break;
    case 1:
        jplpos(JPL_EARTH, JPL_SUN, loc.utc, &loc.pos.eci);
        loc.pos.eci.pass++;
        posprop->Init();
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

    double cmjd = currentmjd();
    locstruc cloc = loc;
    for (uint16_t hour=0; hour<24; ++hour)
    {
        for (uint16_t minute=0; minute<60; ++minute)
        {
            double seconds = loc.utc-cmjd;
//            double distance = norm_rv(rv_sub(loc.pos.geoc.s, cloc.pos.geoc.s));
//            cartpos neweci;
//            lines2eci(loc.utc, lines, neweci);
//            double ecidistance = norm_rv(rv_sub(loc.pos.eci.s, neweci.s));

            rvector topo, dv, ds;

            geoc2topo(loc.pos.geod.s, kccgs.loc.pos.geoc.s, topo);
            topo2azel(topo, kccgs.azto, kccgs.elto);
            geoc2topo(kccgs.loc.pos.geod.s, loc.pos.geoc.s, topo);
            topo2azel(topo, kccgs.azfrom, kccgs.elfrom);
            // Calculate direct vector from source to target
            ds = rv_sub(kccgs.loc.pos.geoc.s, loc.pos.geoc.s);
            kccgs.range = length_rv(ds);
            // Calculate velocity of target WRT source
            dv = rv_sub(kccgs.loc.pos.geoc.v, loc.pos.geoc.v);
            // Closing speed is length of ds in 1 second minus length of ds now.
            kccgs.close = length_rv(rv_sub(ds,dv)) - length_rv(ds);

            geoc2topo(loc.pos.geod.s, mc3gs.loc.pos.geoc.s, topo);
            topo2azel(topo, mc3gs.azto, mc3gs.elto);
            geoc2topo(mc3gs.loc.pos.geod.s, loc.pos.geoc.s, topo);
            topo2azel(topo, mc3gs.azfrom, mc3gs.elfrom);
            // Calculate direct vector from source to target
            ds = rv_sub(mc3gs.loc.pos.geoc.s, loc.pos.geoc.s);
            mc3gs.range = length_rv(ds);
            // Calculate velocity of target WRT source
            dv = rv_sub(mc3gs.loc.pos.geoc.v, loc.pos.geoc.v);
            // Closing speed is length of ds in 1 second minus length of ds now.
            mc3gs.close = length_rv(rv_sub(ds,dv)) - length_rv(ds);

            printf("%s %.4f %.4f %.1f KCC: %.2f %.2f %.1f MC3: %.2f %.2f %.1f \n"
                   , mjd2iso8601(loc.utc).c_str(), DEGOF(loc.pos.geod.s.lat), DEGOF(loc.pos.geod.s.lon), loc.pos.geod.s.h
                   , DEGOF(kccgs.azfrom), DEGOF(kccgs.elfrom), kccgs.range
                   , DEGOF(mc3gs.azfrom), DEGOF(mc3gs.elfrom), mc3gs.range);
            fflush(stdout);
            cloc = loc;
            for (uint16_t i=0; i<static_cast<uint16_t>(60./phys.dt); ++i)
            {
                posprop->Propagate();
                attprop->Propagate();
                thermprop->Propagate();
                elecprop->Propagate();
                Physics::UpdatePhysics(phys, loc);
            }

        }
    }

}
