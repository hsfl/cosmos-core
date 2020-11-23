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
static std::string nodename;
static std::string tracknames;
struct trackstruc
{
    targetstruc target;
    targetstruc aos;
    targetstruc tca;
    targetstruc los;
    physicsstruc physics;
    gj_handle gjh;
    std::string name;
    bool visible;
    bool peaked;
    float highest;
    double startutc;
    std::mutex *control_mutex;
    thread *control_thread;
    bool running;
};
static std::vector <trackstruc> track;

static float highest = -RADOF(90.);
static double utcnow;
static double minelev = RADOF(15.);

void propinit(size_t index, double dt);
void proptrack(size_t index, double utcnow);
void propthread(size_t index);

int main(int argc, char *argv[])
{
    double period = 1.;

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

    std::vector <std::string> nodes;
    int32_t iretn;
    iretn = data_list_nodes(nodes);
    for (size_t i=0; i<nodes.size(); ++i)
    {
        if (tracknames.size() == 0 || tracknames.find(nodes[i], 0) != std::string::npos)
        {
            std::string path = data_base_path(nodes[i]) + "/node.ini";
            FILE *fp = fopen(path.c_str(), "r");
            if (fp != nullptr)
            {
                int32_t type;
                fscanf(fp, "{\"node_type\":%d", &type);
                fclose(fp);
                switch (type)
                {
                case NODE_TYPE_SATELLITE:
                    trackstruc ttrack;
                    ttrack.name = nodes[i];
                    cosmosstruc *cinfo = json_init();
                    iretn = json_setup_node(ttrack.name, cinfo);
                    if (iretn == 0)
                    {
                        if (iretn == 0 && (currentmjd()-cinfo->node.loc.pos.eci.utc) < 10.)
                        {
                            // Valid node. Initialize tracking and push it to list
                            ttrack.visible = false;
                            ttrack.peaked = false;
                            ttrack.target.type = cinfo->node.type;
                            ttrack.target.loc = cinfo->node.loc;
                            ttrack.target.utc = cinfo->node.loc.utc;
                            ttrack.physics = cinfo->node.phys;
                            track.push_back(ttrack);
                            json_destroy(cinfo);
                        }
                    }
                }
            }
        }
    }

    for (size_t i=0; i<track.size(); ++i)
    {
        propinit(i, 10.);
        propinit(i, 1.);
    }

    double utc;
    for (utc=currentmjd(); utc<currentmjd()+period; utc+=1./86400)
    {
        for (size_t i=0; i<track.size(); ++i)
        {
            proptrack(i, utc);
        }
    }

}

void propinit(size_t index, double dt)
{
    printf("Propagating Node %s forward %f seconds\n", track[index].name.c_str(), 86400.*(currentmjd()-track[index].target.loc.pos.eci.utc));
    track[index].physics.mass = 1.;
    track[index].physics.area = .01f;
    gauss_jackson_init_eci(track[index].gjh, 12, 0, dt, track[index].target.loc.pos.eci.utc, track[index].target.loc.pos.eci, track[index].target.loc.att.icrf, track[index].physics, track[index].target.loc);
    gauss_jackson_propagate(track[index].gjh, track[index].physics, track[index].target.loc, currentmjd());
    track[index].running = true;
    track[index].tca.elfrom = 0.;
    track[index].visible = false;
    track[index].target.utc = track[index].target.loc.utc;
}

void proptrack(size_t index, double utcnow)
{

    gauss_jackson_propagate(track[index].gjh, track[index].physics, track[index].target.loc, utcnow);
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
                printf("%s %13.5f [%6.1f %6.1f] AOS0: %.0f m/s %s\n", mjdToGregorian(track[index].aos.utc).c_str(), track[index].aos.utc, DEGOF(track[index].aos.azfrom), DEGOF(track[index].aos.elfrom), track[index].aos.close, track[index].name.c_str());
                printf("%s %13.5f [%6.1f %6.1f] TCA: %4.0f sec %s\n", mjdToGregorian(track[index].tca.utc).c_str(), track[index].tca.utc, DEGOF(track[index].tca.azfrom), DEGOF(track[index].tca.elfrom), 86400.*(utcnow-track[index].startutc), track[index].name.c_str());
                printf("%s %13.5f [%6.1f %6.1f] LOS0: %4.0f sec %.0f m/s %s\n", mjdToGregorian(track[index].los.utc).c_str(), track[index].los.utc, DEGOF(track[index].los.azfrom), DEGOF(track[index].los.elfrom), 86400.*(utcnow-track[index].startutc), track[index].los.close, track[index].name.c_str());
                fflush(stdout);
            }
        }
        break;
    }
}

void propthread(size_t index)
{
    printf("Propagating Node %s forward %f seconds\n", track[index].name.c_str(), 86400.*(currentmjd()-track[index].target.loc.pos.eci.utc));
    track[index].physics.mass = 1.;
    track[index].physics.area = .01;
    gauss_jackson_init_eci(track[index].gjh, 12, 0, 1., track[index].target.loc.pos.eci.utc, track[index].target.loc.pos.eci, track[index].target.loc.att.icrf, track[index].physics, track[index].target.loc);
    gauss_jackson_propagate(track[index].gjh, track[index].physics, track[index].target.loc, currentmjd());
    track[index].running = true;

    while (track[index].running)
    {
        track[index].control_mutex->lock();
        gauss_jackson_propagate(track[index].gjh, track[index].physics, track[index].target.loc, utcnow);
        update_target(agent->cinfo->node.loc, track[index].target);
        if (track[index].target.elfrom > highest)
        {
            highest = track[index].target.elfrom;
        }
        switch ((uint8_t)track[index].visible)
        {
        case 0:
            if (track[index].target.elfrom > 0.)
            {
                track[index].highest = 0.;
                track[index].startutc = utcnow;
                track[index].visible = true;
                track[index].peaked = false;
                printf("%s %13.5f %6.1f         AOS0: %s\n", mjdToGregorian(utcnow).c_str(), utcnow, DEGOF(track[index].target.azfrom), track[index].name.c_str());
                fflush(stdout);
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
                printf("%s %13.5f %6.1f             MAX: %4.0f sec %5.1f deg %s\n", mjdToGregorian(utcnow).c_str(), utcnow, DEGOF(track[index].target.azfrom), 86400.*(utcnow-track[index].startutc), DEGOF(track[index].target.elfrom), track[index].name.c_str());
            }
            if (track[index].target.elfrom < 0.)
            {
                track[index].visible = false;
                printf("%s %13.5f %6.1f     LOS0: %4.0f sec %s\n", mjdToGregorian(utcnow).c_str(), utcnow, DEGOF(track[index].target.azfrom), 86400.*(utcnow-track[index].startutc), track[index].name.c_str());
                fflush(stdout);
            }
            break;
        }
        track[index].control_mutex->unlock();
        COSMOS_USLEEP(1);
    }
}
