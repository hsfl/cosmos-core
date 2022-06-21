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
#include "physics/simulatorclass.h"

int main(int argc, char *argv[])
{
    double initialutc = 0.;
    double currentutc;
    double finalutc;
    double dp = 10.;
    double endp = 86400.;
    int32_t iretn = 0;
    Physics::gj_handle gjh;
    vector<Convert::tlestruc>lines;

    Physics::Simulator *sim;
    Physics::Simulator::StateList::iterator sit;
    Convert::locstruc loc;
    physicsstruc phys;

    sim = new Physics::Simulator();

    Convert::loc_clear(loc);
    loc.att.lvlh.s = q_eye();
    loc.att.lvlh.v = rv_zero();
    loc.att.lvlh.a = rv_zero();
    loc.att.lvlh.pass++;

    switch (argc)
    {
    case 5:
        endp = atof(argv[4]);
    case 4:
        dp = atof(argv[3]);
    case 3:
        initialutc = atof(argv[2]);
    case 2:
        {
            string fname = argv[1];
            if (fname.find(".tle") != string::npos)
            {
                iretn = load_lines(fname, lines);
                if (initialutc == 0.)
                {
                    initialutc = lines[0].utc;
                }
                sim->Init(initialutc, dp/10.);
                sim->AddNode("test", Physics::Structure::Type::U6, Physics::Propagator::Type::PositionGaussJackson, Physics::Propagator::Type::AttitudeLVLH, Physics::Propagator::Type::Thermal, Physics::Propagator::Type::Electrical, lines);
            }
        }
        break;
    case 8:
        initialutc = atof(argv[1]);
        loc.pos.eci.s.col[0] = atof(argv[2]);
        loc.pos.eci.s.col[1] = atof(argv[3]);
        loc.pos.eci.s.col[2] = atof(argv[4]);
        loc.pos.eci.v.col[0] = atof(argv[5]);
        loc.pos.eci.v.col[1] = atof(argv[6]);
        loc.pos.eci.v.col[2] = atof(argv[7]);
        loc.pos.eci.utc = initialutc;
        ++loc.pos.eci.pass;
        sim->Init(initialutc, dp/10.);
        sim->AddNode("test", Physics::Structure::Type::U6, Physics::Propagator::Type::PositionGaussJackson, Physics::Propagator::Type::AttitudeLVLH, Physics::Propagator::Type::Thermal, Physics::Propagator::Type::Electrical, loc.pos.eci);
    }

    sit = sim->GetNode("test");
//    iretn = sim->Reset();
    currentutc = initialutc;
    finalutc = initialutc + endp / 86400.;
    while  (sit->second->currentinfo.node.loc.pos.eci.utc < finalutc)
    {
        printf("%s %10f %10f %10f %7f %7f %7f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n"
               , mjd2iso8601(sit->second->currentinfo.node.loc.pos.eci.utc).c_str(),DEGOF(sit->second->currentinfo.node.loc.pos.geod.s.lat), DEGOF(sit->second->currentinfo.node.loc.pos.geod.s.lon)
               , sit->second->currentinfo.node.loc.pos.geod.s.h, DEGOF(sit->second->currentinfo.node.loc.pos.geod.v.lat), DEGOF(sit->second->currentinfo.node.loc.pos.geod.v.lon), sit->second->currentinfo.node.loc.pos.geod.v.h
               , sit->second->currentinfo.node.loc.pos.eci.s.col[0], sit->second->currentinfo.node.loc.pos.eci.s.col[1], sit->second->currentinfo.node.loc.pos.eci.s.col[2]
                , sit->second->currentinfo.node.loc.pos.eci.v.col[0], sit->second->currentinfo.node.loc.pos.eci.v.col[1], sit->second->currentinfo.node.loc.pos.eci.v.col[2]
                , sit->second->currentinfo.node.loc.pos.eci.a.col[0], sit->second->currentinfo.node.loc.pos.eci.a.col[1], sit->second->currentinfo.node.loc.pos.eci.a.col[2]
                , phys.temp, phys.radiation);
        fflush(stdout);
        currentutc += dp / 86400.;
        sim->Propagate(currentutc);
    }
	return iretn;
}
