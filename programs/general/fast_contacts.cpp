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

#include "support/convertlib.h"
#include "support/jsonlib.h"
#include "agent/agentclass.h"
#include "physics/physicslib.h"
#include "math/mathlib.h"

Agent *agent;
double minimum_elevation = RADOF(10.);
string nodename;
string tlename;
vector <tlestruc> tlelist;
struct trackstruc
{
    tlestruc tle;
    locstruc loc;
    rvector topo;
    float elfrom;
    float azfrom;
    float elto;
    float azto;
    float highest;
    string name;
    bool visible;
    bool peaked;
    double startutc;
};
std::vector <trackstruc> track;

//mutex mut1;
//mutex mut2;
//mutex mut3;
//mutex mut4;

double utcnow;
double utcstart;
double utcend;
double mylat;
double mylon;

void propcalc(size_t index);

int main(int argc, char *argv[])
{
    int32_t iretn;
    double period = 1.;

    printf("Iniitial\n");
    fflush(stdout);
    switch (argc)
    {
    case 6:
        minimum_elevation = RADOF(atof(argv[5]));
    case 5:
        period = atof(argv[4]);
    case 4:
        mylon = atof(argv[3]);
        mylat = atof(argv[2]);
        tlename = argv[1];
        break;
    default:
        printf("Usage: fast_contacts tlename nodename [days]");
        exit (1);
        break;
    }

    // Establish the command channel and heartbeat
    printf("Starting agent\n");
    fflush(stdout);
    agent = new Agent();
//    if ((iretn = agent->wait()) < 0)
//    {
//        fprintf(agent->get_debug_fd(), "%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
//        exit(iretn);
//    }
//    else
//    {
//        fprintf(agent->get_debug_fd(), "%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
//    }


    agent->cinfo->node.loc.pos.geod.s.lat = RADOF(mylat);
    agent->cinfo->node.loc.pos.geod.s.lon = RADOF(mylon);
    agent->cinfo->node.loc.pos.geod.s.h = 348.;
    agent->cinfo->node.loc.pos.geod.v = gv_zero();
    agent->cinfo->node.loc.pos.geod.pass++;
    pos_geod(&agent->cinfo->node.loc);


    // Load Nodes

    printf("loading %s\n", tlename.c_str());
    fflush(stdout);
    load_lines_multi(tlename, tlelist);
    for (size_t i=0; i<tlelist.size(); ++i)
    {
        trackstruc ttrack;
        ttrack.name = tlelist[i].name;
        ttrack.visible = false;
        ttrack.peaked = false;
        ttrack.highest = 0.;
        //        ttrack.type = NODE_TYPE_SATELLITE;
        ttrack.tle = tlelist[i];
        pos_clear(ttrack.loc);
        track.push_back(ttrack);
    }

    utcstart = currentmjd();
    utcend = utcstart + period;
    for (utcnow=utcstart; utcnow<utcend; utcnow+=1./86400)
    {
//#pragma omp parallel for
        for (size_t i=0; i<track.size(); ++i)
        {
            propcalc(i);
        }
    }
}

void propcalc(size_t index)
{
    tle2eci(utcnow, track[index].tle, track[index].loc.pos.eci);
    track[index].loc.pos.eci.pass++;
    pos_eci(&track[index].loc);
    geoc2topo(agent->cinfo->node.loc.pos.geod.s, track[index].loc.pos.geoc.s, track[index].topo);
    topo2azel(track[index].topo, track[index].azfrom, track[index].elfrom);
    switch ((uint8_t)track[index].visible)
    {
    case 0:
        if (track[index].elfrom > minimum_elevation)
        {
            track[index].highest = track[index].elfrom;
            track[index].startutc = utcnow;
            track[index].visible = true;
            track[index].peaked = false;
            printf("%s %13.5f %s         AOS%02d: %4.0f sec %6.1f %5.1f deg\n", mjdToGregorian(utcnow).c_str(), utcnow, track[index].name.c_str(), (int)(DEGOF(minimum_elevation)), 86400.*(utcnow-track[index].startutc), DEGOF(track[index].azfrom), DEGOF(track[index].elfrom));
            fflush(stdout);
        }
        break;
    case 1:
        if (track[index].elfrom < minimum_elevation)
        {
            track[index].visible = false;
            printf("%s %13.5f %s     LOS%02d: %4.0f sec %6.1f %5.1f deg\n", mjdToGregorian(utcnow).c_str(), utcnow, track[index].name.c_str(), (int)(DEGOF(minimum_elevation)), 86400.*(utcnow-track[index].startutc), DEGOF(track[index].azfrom), DEGOF(track[index].elfrom));
            fflush(stdout);
        }
        else
        {
            if (track[index].elfrom > track[index].highest)
            {
                track[index].highest = track[index].elfrom;
            }
            else
            {
                if (!track[index].peaked)
                {
                    track[index].peaked = true;
                    printf("%s %13.5f %s             MAX: %4.0f sec %6.1f %5.1f deg\n", mjdToGregorian(utcnow).c_str(), utcnow, track[index].name.c_str(), 86400.*(utcnow-track[index].startutc), DEGOF(track[index].azfrom), DEGOF(track[index].elfrom));
                }
            }
        }
        break;
    }
}
