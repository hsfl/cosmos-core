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

/*! \file
    \brief Antenna Tracking Program for Satellite Ground Station

    This program controls the ground station antenna (set elevatiom, azimuth, etc.).
    It allows tracking from a TLE, a trajectory file, or an object.
*/

/*! \ingroup programs
 \defgroup track_antenna Track Antenna
 This agent is listed in agent_antenna.cpp

 @code
 Usage: track_antenna antenna mode {file.tle, {date file.tra}, {date x y z vx vy vz}, {az el} [offset_az offset_el]
    Modes:
        tle - file.tle contains Two Line Elements
        tra -
            date is date from which relative times are offset
            file.tra contains columns of relative seconds, lat degrees, lon degrees
        prop - Propogate from date, x, y, z, vx, vy, vz
        abs - Set the antenna azimuth and elevation correction in degrees.
    Optional offsets will be added to az and el correct pointing of dish
 @endcode

*/



#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include <sys/stat.h>
#include <fcntl.h>
//#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
//#include <mqueue.h>
#include <semaphore.h>
//#include <sys/vfs.h>
#include "support/jsonlib.h"
#include "support/stringlib.h"
#include "support/timelib.h"
#include "device/general/gs232b_lib.h"
#include "device/general/prkx2su_lib.h"
#include "support/convertlib.h"
#include "support/elapsedtime.h"
#include "math/mathlib.h"
#include "math/vector.h"
#include "support/ephemlib.h"
#include "physics/physicslib.h"

int32_t connect_antenna();

static float gsmin = RADOF(10.);
//char tlename[20];
static string antbase = "";
static string nodename = "";
static string agentname;
static string antdevice;
static uint16_t devindex = -1;
static uint16_t antindex = -1;
static antstruc target;
static antstruc current;
static bool antconnected = false;
static bool antenabled = true;
static bool debug;

// Here are internally provided functions
int load_tle_info(char *file);

// Here are variables for internal use
static tlestruc tle;
static string tlename;
static Agent *agent;

struct azelstruc
{
    float az;
    float el;
};
static azelstruc antennaoffset = {0., 0.};

static LsFit trackaz(5, 2);
static LsFit trackel(5, 2);
static bool trackflag = false;
static string mode;
static double offset_az = 0.;
static double offset_el = 0.;
struct gentry
{
    double second;
    gvector geod;
    rvector geoc;
};
static vector <gentry> trajectory;
static string trajectoryname;
static double startdate = 0.;
struct trackstruc
{
    uint16_t type;
        targetstruc target;
        physicsstruc physics;
    string name;
        gj_handle gjh;
    vector <LsFit> position;
};
static trackstruc track;
static double timestep;

int main(int argc, char *argv[])
{
    int iretn;

    // Initialization stuff

    if (nodename.empty())
    {
        agent = new Agent("");
    }
    else
    {
        agent = new Agent(nodename);
    }

//    if ((iretn = agent->wait()) < 0)
//    {
//        fprintf(agent->get_debug_fd(), "%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
//        exit(iretn);
//    }
//    else
//    {
//        fprintf(agent->get_debug_fd(), "%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
//    }

    iretn = json_createpiece(agent->cinfo, antbase.c_str(), DeviceType::ANT);
    if (iretn < 0)
    {
        fprintf(agent->get_debug_fd(), "Failed to add %s ANT %s\n", antbase.c_str(), cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(iretn);
    }
    devindex = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
    antindex = agent->cinfo->device[devindex].ant.didx;
    agent->cinfo->device[devindex].ant.minelev = RADOF(10.);
    if (antbase == "sband")
    {
        agent->cinfo->device[devindex].ant.model = DEVICE_MODEL_PRKX2SU;
    }
    else if (antbase == "yagi")
    {
        agent->cinfo->device[devindex].ant.model = DEVICE_MODEL_GS232B;
    }
    else
    {
        agent->cinfo->device[devindex].ant.model = DEVICE_MODEL_LOOPBACK;
    }

    iretn = json_dump_node(agent->cinfo);
    if (iretn < 0)
    {
        fprintf(agent->get_debug_fd(), "Failed to save node %s\n", cosmos_error_string(iretn).c_str());
        exit(iretn);
    }

    antdevice = "/dev/tty_" + antbase;

    // Connect to antenna and set sensitivity;
    iretn = connect_antenna();
    switch (agent->cinfo->device[devindex].all.model)
    {
    case DEVICE_MODEL_GS232B:
        gs232b_set_sensitivity(RADOF(1.));
        break;
    case DEVICE_MODEL_PRKX2SU:
        prkx2su_set_sensitivity(RADOF(.5));
        break;
    }

    agent->cinfo->node.loc.pos.geod.s.lat = RADOF(21.969188888888888888888888888889);
    agent->cinfo->node.loc.pos.geod.s.lon = RADOF(-159.39775555555555555555555555556);
    agent->cinfo->node.loc.pos.geod.s.h = 348.;
    agent->cinfo->node.loc.pos.geod.v = gv_zero();
    agent->cinfo->node.loc.pos.geod.pass++;
    pos_geod(&agent->cinfo->node.loc);
    if (argc > 3)
    {
        antbase = argv[1];
        mode = argv[2];
        if (mode == "tra")
        {
            startdate = atof(argv[3]);
            trajectoryname = argv[4];
            if (!trajectoryname.empty() && data_isfile(trajectoryname))
            {
                FILE *fp = fopen(trajectoryname.c_str(), "r");
                if (fp != nullptr)
                {
                    while (!feof(fp))
                    {
                        gentry tentry;
                        iretn = fscanf(fp, "%lf %lf %lf %lf\n", &tentry.second, &tentry.geod.lat, &tentry.geod.lon, &tentry.geod.h);
                        if (iretn == 4)
                        {
                            tentry.geod.lat = RADOF(tentry.geod.lat);
                            tentry.geod.lon = RADOF(tentry.geod.lon);
                            trajectory.push_back(tentry);
                        }
                    }
                    fclose(fp);
                    if (trajectory.size() > 2)
                    {
                        track.type = 0;
                        track.name = trajectoryname;
                        track.target.type = NODE_TYPE_SATELLITE;
                        track.target.loc.att.icrf.s = q_eye();
                        track.target.loc.att.icrf.v = rv_zero();
                        track.target.loc.att.icrf.a = rv_zero();
                        track.physics.area = .01;
                        track.physics.mass = 1.;

                        LsFit tfit(3);
                        tfit.update(trajectory[0].second, trajectory[0].geod);
                        tfit.update(trajectory[1].second, trajectory[1].geod);
                        for (uint16_t i=0; i<(trajectory[0].second+trajectory[1].second)/2; ++i)
                        {
                            track.position.push_back(tfit);
                        }
                        for (uint16_t j=1; j<trajectory.size()-1; ++j)
                        {
                            tfit.initialize(3);
                            tfit.update(trajectory[j-1].second, trajectory[j-1].geod);
                            tfit.update(trajectory[j].second, trajectory[j].geod);
                            tfit.update(trajectory[j+1].second, trajectory[j+1].geod);
                            for (uint16_t i=static_cast<uint16_t>(.5+(trajectory[j-1].second+trajectory[j].second)/2); i<(trajectory[j].second+trajectory[j+1].second)/2; ++i)
                            {
                                track.position.push_back(tfit);
                            }
                        }
                        tfit.initialize(3);
                        tfit.update(trajectory[trajectory.size()-2].second, trajectory[trajectory.size()-2].geod);
                        tfit.update(trajectory[trajectory.size()-1].second, trajectory[trajectory.size()-1].geod);
                        for (uint16_t i=static_cast<uint16_t>(.5+(trajectory[trajectory.size()-2].second+trajectory[trajectory.size()-1].second)/2); i<trajectory[trajectory.size()-1].second; ++i)
                        {
                            track.position.push_back(tfit);
                        }

                        for (timestep=0.; timestep<=trajectory[trajectory.size()-1].second; timestep+=1.)
                        {
                            uint16_t timeidx = static_cast<uint16_t>(timestep);
                            gvector tpos = track.position[timeidx].evalgvector(timestep);
                            gvector tvel = track.position[timeidx].slopegvector(timestep);
                            printf("%f %f %f %f %f %f %f\n", timestep, DEGOF(tpos.lat), DEGOF(tpos.lon), tpos.h, DEGOF(tvel.lat), DEGOF(tvel.lon), tvel.h);
                        }
                    }
                }
            }
            if (argc == 5)
            {
                offset_az = atof(argv[argc-2]);
                offset_el = atof(argv[argc-1]);
            }
        }
        else if (mode == "tle")
        {
            tlename = argv[3];
            loadTLE(argv[3], tle);
            track.type = 0;
            track.name = tle.name;
            track.target.type = NODE_TYPE_SATELLITE;
            tle2eci(currentmjd()-10./86400., tle, track.target.loc.pos.eci);
            track.target.loc.att.icrf.s = q_eye();
            track.target.loc.att.icrf.v = rv_zero();
            track.target.loc.att.icrf.a = rv_zero();
            track.physics.area = .01;
            track.physics.mass = 1.;


            gauss_jackson_init_eci(track.gjh, 6, 0, 1., track.target.loc.pos.eci.utc, track.target.loc.pos.eci, track.target.loc.att.icrf, track.physics, track.target.loc);
            gauss_jackson_propagate(track.gjh, track.physics, track.target.loc, currentmjd());
        }
        else if (mode == "prop")
        {

        }
        else if (mode == "abs")
        {
            target.azim = RADOF(atof(argv[3]));
            target.elev = RADOF(atof(argv[4]));
        }
        else
        {
            if (argc == 5)
            {
                offset_az = atof(argv[argc-2]);
                offset_el = atof(argv[argc-1]);
            }
        }
    }
    else
    {
        printf("Usage: track_antenna antenna mode {file.tle, {date file.tra}, {date x y z vx vy vz}, {az el} [offset_az offset_el]\n");
        exit(1);
    }


    ElapsedTime et;

    // Start performing the body of the agent
    while(agent->running())
    {
        if (antconnected)
        {
            double ctime = currentmjd();
            // Find most recent position
            switch (agent->cinfo->device[devindex].all.model)
            {
            case DEVICE_MODEL_LOOPBACK:
                iretn = 0;
                break;
            case DEVICE_MODEL_GS232B:
                iretn = gs232b_get_az_el(current.azim, current.elev);
                break;
            case DEVICE_MODEL_PRKX2SU:
                iretn = prkx2su_get_az_el(current.azim, current.elev);
                break;
            }
            if (iretn < 0)
            {
                antconnected = false;
            }
            else
            {
                agent->cinfo->device[devindex].ant.azim = current.azim -  antennaoffset.az;
                agent->cinfo->device[devindex].ant.elev = current.elev -  antennaoffset.el;
                if (mode == "tra")
                {
                    if (ctime >= startdate)
                    {
                        timestep = 86400.*(ctime - startdate);
                        uint16_t timeidx = static_cast<uint16_t>(timestep);
                        track.target.loc.pos.geod.utc = ctime;
                        track.target.loc.pos.geod.s = track.position[timeidx].evalgvector(timestep);
                        track.target.loc.pos.geod.v = track.position[timeidx].slopegvector(timestep);
                        track.target.loc.pos.geod.pass++;
                        pos_geod(&track.target.loc);
                        update_target(agent->cinfo->node.loc, track.target);
                        target.azim = track.target.azfrom;
                        target.elev = track.target.elfrom;
                    }
                    else
                    {
                        target.azim = RADOF(90.);
                        target.elev = RADOF(45.);
                    }
                }
                else if (mode == "sun")
                {
                    jplpos(JPL_EARTH, JPL_SUN, ctime, &track.target.loc.pos.eci);
                    track.target.loc.pos.eci.pass++;
                    pos_eci(&track.target.loc);
                    update_target(agent->cinfo->node.loc, track.target);
                    target.azim = track.target.azfrom;
                    target.elev = track.target.elfrom;
                }
                else if (mode == "tle")
                {
                    gauss_jackson_propagate(track.gjh, track.physics, track.target.loc, ctime);
                    update_target(agent->cinfo->node.loc, track.target);
                }

                switch (agent->cinfo->device[devindex].all.model)
                {
                case DEVICE_MODEL_GS232B:
                    iretn = gs232b_goto(target.azim + antennaoffset.az, target.elev + antennaoffset.el);
                    break;
                case DEVICE_MODEL_PRKX2SU:
                    prkx2su_ramp(PRKX2SU_AXIS_AZ, 9);
                    prkx2su_ramp(PRKX2SU_AXIS_EL, 9);
                    prkx2su_minimum_speed(PRKX2SU_AXIS_AZ, 1);
                    prkx2su_minimum_speed(PRKX2SU_AXIS_EL, 1);
                    prkx2su_maximum_speed(PRKX2SU_AXIS_AZ, 9);
                    prkx2su_maximum_speed(PRKX2SU_AXIS_EL, 9);
                    iretn = prkx2su_goto(target.azim + antennaoffset.az, target.elev + antennaoffset.el);
                    break;
                }
                printf("%s %16.10f %.1f %.2f %.2f %.2f %.2f %.2f %.2f %.3f %.3f %.1f %.1f\n",
                       utc2iso8601(ctime).c_str(),
                       ctime,
                       timestep,
                       DEGOF(target.azim),
                       DEGOF(target.elev),
                       DEGOF(current.azim),
                       DEGOF(current.elev),
                       DEGOF(target.azim-current.azim),
                       DEGOF(target.elev-current.elev),
                       DEGOF(track.target.loc.pos.geod.s.lat),
                       DEGOF(track.target.loc.pos.geod.s.lon),
                       track.target.loc.pos.geod.s.h,
                       track.target.range);
                if (debug)
                {
                    printf("%f: goto %f %f [%d]\n", et.lap(), DEGOF(current.azim + antennaoffset.az), DEGOF(current.elev + antennaoffset.el), iretn);
                }
                if (iretn < 0)
                {
                    antconnected = false;
                }
            }
            COSMOS_SLEEP(.5);
        }
        else
        {
            if (debug)
            {
                printf("%f: Connect Antenna\n", et.lap());
            }
            connect_antenna();
            COSMOS_SLEEP(.1);
        }
    }

    agent->shutdown();
}

int32_t connect_antenna()
{
    int32_t iretn;
    antconnected = false;

    switch (agent->cinfo->device[devindex].all.model)
    {
    case DEVICE_MODEL_LOOPBACK:
        antconnected = true;
        break;
    case DEVICE_MODEL_GS232B:
        iretn = gs232b_connect(antdevice);

        // Initialize values if we are connected
        if (iretn == 0)
        {
            iretn = gs232b_get_az_el(agent->cinfo->device[devindex].ant.azim, agent->cinfo->device[devindex].ant.elev);
            if (iretn >= 0)
            {
                target.azim = agent->cinfo->device[devindex].ant.azim - antennaoffset.az;
                target.elev = agent->cinfo->device[devindex].ant.elev - antennaoffset.el;
                antconnected = true;
            }
        }
        break;
    case DEVICE_MODEL_PRKX2SU:
        iretn = prkx2su_connect(antdevice);

        // Initialize values if we are connected
        if (iretn == 0)
        {
            iretn = prkx2su_get_az_el(agent->cinfo->device[devindex].ant.azim, agent->cinfo->device[devindex].ant.elev);
            if (iretn >= 0)
            {
                target.azim = agent->cinfo->device[devindex].ant.azim - antennaoffset.az;
                target.elev = agent->cinfo->device[devindex].ant.elev - antennaoffset.el;
                antconnected = true;
            }
        }
        break;
    }

    return 0;

}
