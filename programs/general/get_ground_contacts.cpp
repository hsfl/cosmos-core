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
Convert::locstruc loc;
struct trackstruc
{
    targetstruc target;
    targetstruc aos0;
    targetstruc aos1;
    targetstruc tca;
    targetstruc los1;
    targetstruc los0;
    vector<Convert::tlestruc> tles;
    string name;
    bool visible0 = false;
    bool visible1 = false;
    bool peaked = false;
    float highest = -RADOF(90.);
    float closest = 2. * REARTHM;
    double startutc;
    std::mutex *control_mutex;
    std::thread *control_thread;
    bool running;
    float minelev = RADOF(15.);
};
static std::vector <trackstruc> tracks;
trackstruc ttrack;

//static float highest = -RADOF(90.);
static double utcstart = 60418.48472;
static double missionstart = 60418.48472;
static double period = 1.;

void proptrack(size_t index, double utcnow);

int main(int argc, char *argv[])
{
    int32_t iretn;
    string path;
    if (utcstart < currentmjd())
    {
        utcstart = currentmjd();
    }
    switch (argc)
    {
    case 12:
        ttrack.target.name = (argv[11]);
        if (ttrack.target.name.find(":") != string::npos)
        {
            ttrack.minelev = RADOF(stof(ttrack.target.name.substr(ttrack.target.name.find(":")+1)));
            ttrack.target.name = ttrack.target.name.substr(0, ttrack.target.name.find(":"));
        }
        ttrack.name = ttrack.target.name;
        ttrack.target.type = NODE_TYPE_GROUNDSTATION;
        tracks.push_back(ttrack);
    case 11:
        ttrack.target.name = (argv[10]);
        if (ttrack.target.name.find(":") != string::npos)
        {
            ttrack.minelev = RADOF(stof(ttrack.target.name.substr(ttrack.target.name.find(":")+1)));
            ttrack.target.name = ttrack.target.name.substr(0, ttrack.target.name.find(":"));
        }
        ttrack.name = ttrack.target.name;
        ttrack.target.type = NODE_TYPE_GROUNDSTATION;
        tracks.push_back(ttrack);
    case 10:
        ttrack.target.name = (argv[9]);
        if (ttrack.target.name.find(":") != string::npos)
        {
            ttrack.minelev = RADOF(stof(ttrack.target.name.substr(ttrack.target.name.find(":")+1)));
            ttrack.target.name = ttrack.target.name.substr(0, ttrack.target.name.find(":"));
        }
        ttrack.name = ttrack.target.name;
        ttrack.target.type = NODE_TYPE_GROUNDSTATION;
        tracks.push_back(ttrack);
    case 9:
        ttrack.target.name = (argv[8]);
        if (ttrack.target.name.find(":") != string::npos)
        {
            ttrack.minelev = RADOF(stof(ttrack.target.name.substr(ttrack.target.name.find(":")+1)));
            ttrack.target.name = ttrack.target.name.substr(0, ttrack.target.name.find(":"));
        }
        ttrack.name = ttrack.target.name;
        ttrack.target.type = NODE_TYPE_GROUNDSTATION;
        tracks.push_back(ttrack);
    case 8:
        ttrack.target.name = (argv[7]);
        if (ttrack.target.name.find(":") != string::npos)
        {
            ttrack.minelev = RADOF(stof(ttrack.target.name.substr(ttrack.target.name.find(":")+1)));
            ttrack.target.name = ttrack.target.name.substr(0, ttrack.target.name.find(":"));
        }
        ttrack.name = ttrack.target.name;
        ttrack.target.type = NODE_TYPE_GROUNDSTATION;
        tracks.push_back(ttrack);
    case 7:
        ttrack.target.name = (argv[6]);
        if (ttrack.target.name.find(":") != string::npos)
        {
            ttrack.minelev = RADOF(stof(ttrack.target.name.substr(ttrack.target.name.find(":")+1)));
            ttrack.target.name = ttrack.target.name.substr(0, ttrack.target.name.find(":"));
        }
        ttrack.name = ttrack.target.name;
        ttrack.target.type = NODE_TYPE_GROUNDSTATION;
        tracks.push_back(ttrack);
    case 6:
        ttrack.target.name = (argv[5]);
        if (ttrack.target.name.find(":") != string::npos)
        {
            ttrack.minelev = RADOF(stof(ttrack.target.name.substr(ttrack.target.name.find(":")+1)));
            ttrack.target.name = ttrack.target.name.substr(0, ttrack.target.name.find(":"));
        }
        ttrack.name = ttrack.target.name;
        ttrack.target.type = NODE_TYPE_GROUNDSTATION;
        tracks.push_back(ttrack);
    case 5:
        ttrack.target.name = (argv[4]);
        if (ttrack.target.name.find(":") != string::npos)
        {
            ttrack.minelev = RADOF(stof(ttrack.target.name.substr(ttrack.target.name.find(":")+1)));
            ttrack.target.name = ttrack.target.name.substr(0, ttrack.target.name.find(":"));
        }
        ttrack.name = ttrack.target.name;
        ttrack.target.type = NODE_TYPE_GROUNDSTATION;
        tracks.push_back(ttrack);
    case 4:
        ttrack.target.name = (argv[3]);
        if (ttrack.target.name.find(":") != string::npos)
        {
            ttrack.minelev = RADOF(stof(ttrack.target.name.substr(ttrack.target.name.find(":")+1)));
            ttrack.target.name = ttrack.target.name.substr(0, ttrack.target.name.find(":"));
        }
        ttrack.name = ttrack.target.name;
        ttrack.target.type = NODE_TYPE_GROUNDSTATION;
        tracks.push_back(ttrack);
    case 3:
        nodename = argv[2];
        path = data_base_path(nodename) + "/state.tle";
        iretn = Convert::load_tle(path, loc.tle);
    case 2:
        period = atof(argv[1]);
        if (period < 0.)
        {
            period = -period;
            utcstart -= period;
        }
        break;
    default:
        printf("Usage: get_contacts days [satname [ground1:minelev [ground2:minelev [...]]]]\n");
        exit (1);
    }

    // Establish the command channel and heartbeat
    if (!(agent = new Agent("", nodename)))
    {
        std::cout << ": agent_setup_client failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<std::endl;
        exit (AGENT_ERROR_JSON_CREATE);
    }

    // Load Nodes

    for (trackstruc &track : tracks)
    {
        string path = data_base_path(track.target.name) + "/state.ini";
        jsonnode json;
        struct stat fstat;
        FILE* fdes;
        if ((iretn=stat(path.c_str(), &fstat)) == 0 && (fdes=fopen(path.c_str(),"r")) != NULL)
        {
            char* ibuf = (char *)calloc(1,fstat.st_size+1);
            size_t nbytes = fread(ibuf, 1, fstat.st_size, fdes);
            if (nbytes)
            {
                json_parse(ibuf, agent->cinfo);
            }
            free(ibuf);
        }
        track.target.cloc = agent->cinfo->node.loc;
    }

    double utc;
    for (utc=utcstart; utc<utcstart+period; utc+=1./86400)
    {
        Convert::tle2eci(utc, loc.tle, loc.pos.eci);
        loc.pos.eci.pass++;
        Convert::pos_eci(loc);
        for (size_t i=0; i<tracks.size(); ++i)
        {
            proptrack(i, utc);
        }
    }

}

void proptrack(size_t index, double utcnow)
{
    update_target(loc, tracks[index].target);
    if (tracks[index].target.range < tracks[index].closest)
    {
        tracks[index].closest = tracks[index].target.range;
    }
    switch (static_cast<uint8_t>(tracks[index].visible0))
    {
    case 0:
        if (tracks[index].target.elto > 0.)
        {
            tracks[index].aos0 = tracks[index].target;
            tracks[index].visible0 = true;

            tracks[index].highest = 0.;
            tracks[index].startutc = utcnow;
            tracks[index].peaked = false;
        }
        break;
    case 1:
        switch (static_cast<uint8_t>(tracks[index].visible1))
        {
        case 0:
            if (tracks[index].target.elto > tracks[index].minelev)
            {
                tracks[index].aos1 = tracks[index].target;
                tracks[index].visible1 = true;
            }
            break;
        case 1:
            if (tracks[index].target.elto > tracks[index].highest)
            {
                tracks[index].highest = tracks[index].target.elto;
            }
            else if (!tracks[index].peaked)
            {
                tracks[index].peaked = true;
                tracks[index].tca = tracks[index].target;
            }
            if (tracks[index].target.elto < tracks[index].minelev)
            {
                tracks[index].los1 = tracks[index].target;
                tracks[index].visible1 = false;
            }
            break;
        }
        if (tracks[index].target.elto < 0.)
        {
            tracks[index].los0 = tracks[index].target;
            tracks[index].visible0 = false;
            if (tracks[index].tca.elto >= tracks[index].minelev)
            {
                Convert::kepstruc kep;
                Convert::eci2kep(tracks[index].tca.loc.pos.eci, kep);
                //                printf("%s\t%f\t%f\t", tracks[index].target.name.c_str(), DEGOF(tracks[index].tca.loc.pos.earthsep), DEGOF(kep.beta));
                printf("%s\t%s\t%.5f\t%13.5f\t%s\tAOS0\t%6.1f\t%6.1f\t%5.1f\t%5.1f\n", mjd2iso8601(tracks[index].aos0.utc).c_str(), mjd2iso8601(tracks[index].aos0.utc-10./24.).c_str(), tracks[index].aos0.utc - missionstart, tracks[index].aos0.utc, tracks[index].target.name.c_str(), DEGOF(tracks[index].aos0.azfrom), DEGOF(tracks[index].aos0.elto), 86400.*(tracks[index].aos0.utc-tracks[index].aos0.utc), 86400.*(tracks[index].aos0.utc-tracks[index].aos1.utc));
                printf("%s\t%s\t%.5f\t%13.5f\t%s\tAOS1\t%6.1f\t%6.1f\t%5.1f\t%5.1f\n", mjd2iso8601(tracks[index].aos1.utc).c_str(), mjd2iso8601(tracks[index].aos1.utc-10./24.).c_str(), tracks[index].aos1.utc - missionstart, tracks[index].aos1.utc, tracks[index].target.name.c_str(), DEGOF(tracks[index].aos1.azfrom), DEGOF(tracks[index].aos1.elto), 86400.*(tracks[index].aos1.utc-tracks[index].aos0.utc), 86400.*(tracks[index].aos1.utc-tracks[index].aos1.utc));
                printf("%s\t%s\t%.5f\t%13.5f\t%s\tTCA\t%6.1f\t%6.1f\t%5.1f\t%5.1f\n", mjd2iso8601(tracks[index].tca.utc).c_str(), mjd2iso8601(tracks[index].tca.utc-10./24.).c_str(), tracks[index].tca.utc - missionstart, tracks[index].tca.utc, tracks[index].target.name.c_str(), DEGOF(tracks[index].tca.azfrom), DEGOF(tracks[index].tca.elto), 86400.*(tracks[index].tca.utc-tracks[index].aos0.utc), 86400.*(tracks[index].tca.utc-tracks[index].aos1.utc));
                printf("%s\t%s\t%.5f\t%13.5f\t%s\tLOS1\t%6.1f\t%6.1f\t%5.1f\t%5.1f\n", mjd2iso8601(tracks[index].los1.utc).c_str(), mjd2iso8601(tracks[index].los1.utc-10./24.).c_str(), tracks[index].los1.utc - missionstart, tracks[index].los1.utc, tracks[index].target.name.c_str(), DEGOF(tracks[index].los1.azfrom), DEGOF(tracks[index].los1.elto), 86400.*(tracks[index].los1.utc-tracks[index].aos0.utc), 86400.*(tracks[index].los1.utc-tracks[index].aos1.utc));
                printf("%s\t%s\t%.5f\t%13.5f\t%s\tLOS0\t%6.1f\t%6.1f\t%5.1f\t%5.1f\n", mjd2iso8601(tracks[index].los0.utc).c_str(), mjd2iso8601(tracks[index].los0.utc-10./24.).c_str(), tracks[index].los0.utc - missionstart, tracks[index].los0.utc, tracks[index].target.name.c_str(), DEGOF(tracks[index].los0.azfrom), DEGOF(tracks[index].los0.elto), 86400.*(tracks[index].los0.utc-tracks[index].aos0.utc), 86400.*(tracks[index].los0.utc-tracks[index].aos1.utc));
                fflush(stdout);
            }
        }
        break;
    }
}
