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

static Agent *agent;
static string nodename;
static string tracknames;
struct trackstruc
{
    targetstruc target;
    targetstruc aos;
    targetstruc tca;
    targetstruc los;
    vector<Convert::tlestruc> tles;
    string name;
    bool visible;
    bool peaked;
    float highest;
    double startutc;
    std::mutex *control_mutex;
    std::thread *control_thread;
    bool running;
};
static std::vector <trackstruc> track;

static float highest = -RADOF(90.);
static double utcstart;
static double period = 1.;
static double minelev = RADOF(15.);

void proptrack(size_t index, double utcnow);

int main(int argc, char *argv[])
{

    utcstart = currentmjd();
    switch (argc)
    {
    case 12:
        tracknames += argv[11];
    case 11:
        tracknames += argv[10];
    case 10:
        tracknames += argv[9];
    case 9:
        tracknames += argv[8];
    case 8:
        tracknames += argv[7];
    case 7:
        tracknames += argv[6];
    case 6:
        tracknames += argv[5];
    case 5:
        tracknames += argv[4];
    case 4:
        tracknames += argv[3];
    case 3:
        period = atof(argv[2]);
        if (period < 0.)
        {
            period = -period;
            utcstart -= period;
        }
    case 2:
        nodename = argv[1];
        if (nodename.find(":") != string::npos)
        {
            minelev = RADOF(stof(nodename.substr(nodename.find(":")+1)));
            nodename = nodename.substr(0, nodename.find(":"));
        }
        break;
    default:
        printf("Usage: get_contacts gsname[:minelev] [days [sat1 [sat2 [...]]]]\n");
        exit (1);
    }

    // Establish the command channel and heartbeat
    if (!(agent = new Agent(nodename)))
    {
        std::cout << ": agent_setup_client failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<std::endl;
        exit (AGENT_ERROR_JSON_CREATE);
    }

    // Load Nodes

    std::vector <string> nodes;
    int32_t iretn = 0;
    iretn = data_list_nodes(nodes);
    for (size_t i=0; i<nodes.size(); ++i)
    {
        if (tracknames.size() == 0 || tracknames.find(nodes[i], 0) != string::npos)
        {
            string path = data_base_path(nodes[i]) + "/node.ini";
            FILE *fp = fopen(path.c_str(), "r");
            if (fp != nullptr)
            {
                int32_t type;
                fscanf(fp, "{\"node_type\":%d", &type);
                fclose(fp);
                switch (type)
                {
                case NODE_TYPE_SATELLITE:
                    path = data_base_path(nodes[i]) + "/state.tle";
                    trackstruc ttrack;
                    ttrack.name = nodes[i];
                    iretn = load_lines(path, ttrack.tles);
                    if (iretn >= 0 && ttrack.tles.size())
                    {
                        iretn = lines2eci(utcstart, ttrack.tles, ttrack.target.loc.pos.eci);
                        if (iretn >= 0)
                        {
                            // Valid node. Initialize tracking and push it to list
                            ttrack.visible = false;
                            ttrack.peaked = false;
                            ttrack.target.type = type;
                            track.push_back(ttrack);
                        }
                    }
                }
            }
        }
    }

    double utc;
    for (utc=utcstart; utc<utcstart+period; utc+=1./86400)
    {
        for (size_t i=0; i<track.size(); ++i)
        {
            proptrack(i, utc);
        }
    }

}

void proptrack(size_t index, double utcnow)
{
    lines2eci(utcnow, track[index].tles, track[index].target.loc.pos.eci);
    track[index].target.loc.pos.eci.pass++;
    Convert::pos_eci(track[index].target.loc);
    update_target(agent->cinfo->node.loc, track[index].target);
    if (track[index].target.elfrom > highest)
    {
        highest = track[index].target.elfrom;
    }
    switch (static_cast<uint8_t>(track[index].visible))
    {
    case 0:
        if (track[index].target.elfrom > 0.)
        {
            track[index].aos = track[index].target;
            track[index].highest = 0.;
            track[index].startutc = utcnow;
            track[index].visible = true;
            track[index].peaked = false;
        }
        break;
    case 1:
        if (track[index].target.elfrom > track[index].highest)
        {
            track[index].highest = track[index].target.elfrom;
        }
        else if (!track[index].peaked)
        {
            track[index].peaked = true;
            track[index].tca = track[index].target;
        }
        if (track[index].target.elfrom < 0.)
        {
            track[index].los = track[index].target;
            track[index].visible = false;
            if (track[index].tca.elfrom >= minelev)
            {
                Convert::kepstruc kep;
                Convert::eci2kep(track[index].tca.loc.pos.eci, kep);
                printf("%s\t%f\t%f\t", track[index].name.c_str(), DEGOF(track[index].tca.loc.pos.earthsep), DEGOF(kep.beta));
                printf("AOS0:\t%s\t%13.5f\t[\t%6.1f\t%6.1f\t]\t", mjdToGregorian(track[index].aos.utc).c_str(), track[index].aos.utc, DEGOF(track[index].aos.azfrom), DEGOF(track[index].aos.elfrom));
                printf("TCA[ %3.0f ]:\t%s\t%13.5f\t[ %6.1f\t%6.1f ]\t", 86400.*(track[index].tca.utc-track[index].startutc), mjdToGregorian(track[index].tca.utc).c_str(), track[index].tca.utc, DEGOF(track[index].tca.azfrom), DEGOF(track[index].tca.elfrom));
                printf("LOS0[ %3.0f ]:\t%s\t%13.5f\t[ %6.1f\t%6.1f ]\n", 86400.*(utcnow-track[index].startutc), mjdToGregorian(track[index].los.utc).c_str(), track[index].los.utc, DEGOF(track[index].los.azfrom), DEGOF(track[index].los.elfrom));
                fflush(stdout);
            }
        }
        break;
    }
}
