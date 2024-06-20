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
    float minelev = RADOF(5.);
};
static std::vector <trackstruc> tracks;
trackstruc ttrack;

//static float highest = -RADOF(90.);
static double utcstart = 60418.48472;
static double missionstart = 60418.48472;
static double period = 1.;
string mode = "table";
bool calheader = true;

void proptrack(size_t index, double utcnow);

int main(int argc, char *argv[])
{
    int32_t iretn;
    string path;
    if (utcstart < currentmjd())
    {
        utcstart = currentmjd();
    }
    if (string(argv[argc-1]) == "cal")
    {
        mode = "cal";
        --argc;
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
        printf("Usage: get_ground_contacts days [satname [ground1:minelev [ground2:minelev [...]]]]\n");
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

    if (mode != "cal")
    {
        printf("MET\tMJD\tStation\t");
        printf("AOS UTC Date\tAOS UTC Time\tAOS HST Date\tAOS HST Time\tAOS Az\tAOS El\tAOS DT0\tAOS DT1\t");
        printf("TSTART UTC Date\tTSTART UTC Time\tTSTART HST Date\tTSTART HST Time\tTSTART Az\tTSTART El\tTSTART DT0\tTSTART DT1\t");
        printf("TMAX UTC Date\tTMAX UTC Time\tTMAX HST Date\tTMAX HST Time\tTMAX Az\tTMAX El\tTMAX DT0\tTMAX DT1\t");
        printf("TEND UTC Date\tTEND UTC Time\tTEND HST Date\tTEND HST Time\tTEND Az\tTEND El\tTEND DT0\tTEND DT1\t");
        printf("LOS UTC Date\tLOS UTC Time\tLOS HST Date\tLOS HST Time\tLOS Az\tLOS El\tLOS DT0\tLOS DT1\n");
    }
    double utc;
    for (utc=utcstart; utc<utcstart+period; utc+=.5/86400)
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
            tracks[index].aos1 = tracks[index].target;
            tracks[index].tca = tracks[index].target;
            tracks[index].los1 = tracks[index].target;
            tracks[index].los0 = tracks[index].target;
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
                if (mode == "cal")
                {
                    calstruc calaosa = mjd2cal(tracks[index].aos0.utc-10./24.);
                    calstruc calaosb = mjd2cal(tracks[index].aos1.utc+1./1440.-10./24.);
                    calstruc caltcaa = mjd2cal(tracks[index].aos1.utc-10./24.);
                    calstruc caltcab = mjd2cal(tracks[index].los1.utc+1./1440.-10./24.);
                    calstruc callosa = mjd2cal(tracks[index].los1.utc-10./24.);
                    calstruc callosb = mjd2cal(tracks[index].los0.utc+1./1440.-10./24.);
                    if (calheader)
                    {
                        printf("Subject,Start Date,Start Time,End Date,End Time,All Day Event,Description,Location\n");
                        calheader = false;
                    }
                    printf("%s %02u,", tracks[index].target.name.c_str(), uint16_t(DEGOF(tracks[index].tca.elto)+.1));
                    printf("%02d/%02d/%04d,%02d:%02d,", calaosa.month, calaosa.dom, calaosa.year, calaosa.hour, calaosa.minute);
                    printf("%02d/%02d/%04d,%02d:%02d,", callosb.month, callosb.dom, callosb.year, callosb.hour, callosb.minute);
                    printf("FALSE,TMAX %.0f degrees,%s\n", DEGOF(tracks[index].minelev), tracks[index].target.name.c_str());
                }
                else
                {
                    calstruc calutc;
                    calstruc calhst;
                    printf("%.5f\t%.5f\t%s\t", tracks[index].aos0.utc - missionstart, tracks[index].aos0.utc, tracks[index].target.name.c_str());

                    calutc = mjd2cal(tracks[index].aos0.utc);
                    calhst = mjd2cal(tracks[index].aos0.utc-10./24.);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calutc.year, calutc.month, calutc.dom, calutc.hour, calutc.minute, calutc.second);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calhst.year, calhst.month, calhst.dom, calhst.hour, calhst.minute, calhst.second);
                    printf("%.1f\t%.1f\t%.0f\t%.0f\t", DEGOF(tracks[index].aos0.azto), DEGOF(tracks[index].aos0.elto), 86400.*(tracks[index].aos0.utc-tracks[index].aos0.utc), 86400.*(tracks[index].aos0.utc-tracks[index].aos1.utc));

                    calutc = mjd2cal(tracks[index].aos1.utc);
                    calhst = mjd2cal(tracks[index].aos1.utc-10./24.);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calutc.year, calutc.month, calutc.dom, calutc.hour, calutc.minute, calutc.second);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calhst.year, calhst.month, calhst.dom, calhst.hour, calhst.minute, calhst.second);
                    printf("%.1f\t%.1f\t%.0f\t%.0f\t", DEGOF(tracks[index].aos1.azto), DEGOF(tracks[index].aos1.elto), 86400.*(tracks[index].aos1.utc-tracks[index].aos0.utc), 86400.*(tracks[index].aos1.utc-tracks[index].aos1.utc));

                    calutc = mjd2cal(tracks[index].tca.utc);
                    calhst = mjd2cal(tracks[index].tca.utc-10./24.);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calutc.year, calutc.month, calutc.dom, calutc.hour, calutc.minute, calutc.second);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calhst.year, calhst.month, calhst.dom, calhst.hour, calhst.minute, calhst.second);
                    printf("%.1f\t%.1f\t%.0f\t%.0f\t", DEGOF(tracks[index].tca.azto), DEGOF(tracks[index].tca.elto), 86400.*(tracks[index].tca.utc-tracks[index].aos0.utc), 86400.*(tracks[index].tca.utc-tracks[index].aos1.utc));

                    calutc = mjd2cal(tracks[index].los1.utc);
                    calhst = mjd2cal(tracks[index].los1.utc-10./24.);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calutc.year, calutc.month, calutc.dom, calutc.hour, calutc.minute, calutc.second);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calhst.year, calhst.month, calhst.dom, calhst.hour, calhst.minute, calhst.second);
                    printf("%.1f\t%.1f\t%.0f\t%.0f\t", DEGOF(tracks[index].los1.azto), DEGOF(tracks[index].los1.elto), 86400.*(tracks[index].los1.utc-tracks[index].aos0.utc), 86400.*(tracks[index].los1.utc-tracks[index].aos1.utc));

                    calutc = mjd2cal(tracks[index].los0.utc);
                    calhst = mjd2cal(tracks[index].los0.utc-10./24.);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calutc.year, calutc.month, calutc.dom, calutc.hour, calutc.minute, calutc.second);
                    printf("%04d/%02d/%02d\t%02d:%02d:%02d\t", calhst.year, calhst.month, calhst.dom, calhst.hour, calhst.minute, calhst.second);
                    printf("%.1f\t%.1f\t%.0f\t%.0f\n", DEGOF(tracks[index].los0.azto), DEGOF(tracks[index].los0.elto), 86400.*(tracks[index].los0.utc-tracks[index].aos0.utc), 86400.*(tracks[index].los0.utc-tracks[index].aos1.utc));
                }
                fflush(stdout);
            }
        }
        break;
    }
}
