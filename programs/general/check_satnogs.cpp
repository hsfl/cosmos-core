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
#include "support/jsonclass.h"
#include "support/datalib.h"
#include "support/timelib.h"
#include "support/convertlib.h"
#include "agent/agentclass.h"

int main(int argc, char *argv[])
{
    int32_t iretn = 0;
    vector<tlestruc> tles;

    iretn = load_lines(argv[1], tles);

    string input;
    double tovernal;

    if (argc == 3)
    {
        if (data_isfile(argv[2]))
        {
            FILE *fp = fopen(argv[2], "r");
            int ic;
            while ((ic=fgetc(fp)) != EOF)
            {
                input.push_back(ic);
            };

            Json jobject;
            jobject.extract_contents(input);

            locstruc currentloc;
            lines2eci(currentmjd(), tles, currentloc.pos.eci);
            currentloc.pos.eci.pass++;
            pos_eci(currentloc);
            kepstruc currentkep;
            eci2kep(currentloc.pos.eci, currentkep);
            tovernal = atan2(currentloc.pos.extra.sun2earth.s.col[1], currentloc.pos.extra.sun2earth.s.col[0]);

            for (const auto& member : jobject.ArrayContents)
            {
                double start = iso86012utc(member.object.at("start").svalue);
                locstruc startloc;
                lines2eci(start, tles, startloc.pos.eci);
                startloc.pos.eci.pass++;
                pos_eci(startloc);
                kepstruc startkep;
                eci2kep(startloc.pos.eci, startkep);
                tovernal = atan2(startloc.pos.extra.sun2earth.s.col[1], startloc.pos.extra.sun2earth.s.col[0]);
                double startphase = fixangle(startkep.ma + startkep.raan - tovernal);

                double end = iso86012utc(member.object.at("end").svalue);
                locstruc endloc;
                lines2eci(end, tles, endloc.pos.eci);
                endloc.pos.eci.pass++;
                pos_eci(endloc);
                kepstruc endkep;
                eci2kep(endloc.pos.eci, endkep);
                tovernal = atan2(endloc.pos.extra.sun2earth.s.col[1], endloc.pos.extra.sun2earth.s.col[0]);
                double endphase = fixangle(endkep.ma + endkep.raan - tovernal);
                //        double endphase = (endloc.pos.earthsep) / cos(endkep.beta);

                double dphase = (1./86400.) * (endphase - startphase) / (end - start);

                printf("%s\t", member.object.at("id").svalue.c_str());
                printf("%s\t", member.object.at("start").svalue.c_str());
                printf("%s\t", to_mjd(start).c_str());
                printf("%s\t", to_double(DEGOF(startloc.pos.earthsep)).c_str());
                printf("%s\t", to_double(DEGOF(startphase)).c_str());

                printf("%s\t", member.object.at("end").svalue.c_str());
                printf("%s\t", to_mjd(end).c_str());
                printf("%s\t", to_double(DEGOF(endloc.pos.earthsep)).c_str());
                printf("%s\t", to_double(DEGOF(endphase)).c_str());

                printf("%s\t", to_double(DEGOF(dphase), 5).c_str());

                if (member.object.at("status").svalue == "good")
                {
                    printf("1");
                }
                else {
                    printf("0");
                }

                printf("\n");
            }
        }
        else {
            float minelev = RADOF(15.);
            string nodename = argv[2];
            if (nodename.find(":") != string::npos)
            {
                minelev = RADOF(stof(nodename.substr(nodename.find(":")+1)));
                nodename = nodename.substr(0, nodename.find(":"));
            }
            Agent *agent = new Agent(nodename);
            if ((iretn = agent->wait()) >= 0)
            {
                double utcnow;
                targetstruc target;
                targetstruc aos;
                targetstruc tca;
                targetstruc los;
                double startutc;
                bool visible = false;
                bool peaked = false;
                float highest = -RADOF(90.);
                for (utcnow=59159.642; utcnow<currentmjd(); utcnow+=1./86400)
                {
                    lines2eci(utcnow, tles, target.loc.pos.eci);
                    target.loc.pos.eci.pass++;
                    pos_eci(target.loc);
                    update_target(agent->cinfo->node.loc, target);
                    switch (static_cast<uint8_t>(visible))
                    {
                    case 0:
                        if (target.elfrom > 0.f)
                        {
                            aos = target;
                            highest = 0.;
                            startutc = utcnow;
                            visible = true;
                            peaked = false;
                        }
                        break;
                    case 1:
                        if (target.elfrom > highest)
                        {
                            highest = target.elfrom;
                        }
                        else if (!peaked)
                        {
                            peaked = true;
                            tca = target;
                        }
                        if (target.elfrom < 0.f)
                        {
                            los = target;
                            visible = false;
                            if (tca.elfrom >= minelev)
                            {
                                kepstruc startkep;
                                eci2kep(aos.loc.pos.eci, startkep);
                                tovernal = atan2(aos.loc.pos.extra.sun2earth.s.col[1], aos.loc.pos.extra.sun2earth.s.col[0]);
                                double startphase = fixangle(startkep.ma + startkep.raan - tovernal);

                                kepstruc endkep;
                                eci2kep(los.loc.pos.eci, endkep);
                                tovernal = atan2(los.loc.pos.extra.sun2earth.s.col[1], los.loc.pos.extra.sun2earth.s.col[0]);
                                double endphase = fixangle(endkep.ma + endkep.raan - tovernal);

                                double dphase = (1./86400.) * (endphase - startphase) / (los.loc.pos.eci.utc - aos.loc.pos.eci.utc);

                                printf("00000001\t");
                                printf("%s\t", mjd2iso8601(aos.loc.pos.eci.utc).c_str());
                                printf("%s\t", to_mjd(aos.loc.pos.eci.utc).c_str());
                                printf("%s\t", to_double(DEGOF(aos.loc.pos.earthsep)).c_str());
                                printf("%s\t", to_double(DEGOF(startphase)).c_str());

                                printf("%s\t", mjd2iso8601(los.loc.pos.eci.utc).c_str());
                                printf("%s\t", to_mjd(los.loc.pos.eci.utc).c_str());
                                printf("%s\t", to_double(DEGOF(los.loc.pos.earthsep)).c_str());
                                printf("%s\t", to_double(DEGOF(endphase)).c_str());

                                printf("%s\t", to_double(DEGOF(dphase), 5).c_str());

                                printf("2");
                                printf("\n");

                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    else {
        vector<double> amateurs = {59174.41688, 59177.03267, 59184.09772, 59187.79667, 59188.54722, 59189.95972};
        for (double amateur : amateurs)
        {
            locstruc startloc;
            lines2eci(amateur-2./1440., tles, startloc.pos.eci);
            startloc.pos.eci.pass++;
            pos_eci(startloc);
            kepstruc startkep;
            eci2kep(startloc.pos.eci, startkep);
            tovernal = atan2(startloc.pos.extra.sun2earth.s.col[1], startloc.pos.extra.sun2earth.s.col[0]);
            double startphase = fixangle(startkep.ma + startkep.raan - tovernal);

            locstruc endloc;
            lines2eci(amateur+2./1440., tles, endloc.pos.eci);
            endloc.pos.eci.pass++;
            pos_eci(endloc);
            kepstruc endkep;
            eci2kep(endloc.pos.eci, endkep);
            tovernal = atan2(endloc.pos.extra.sun2earth.s.col[1], endloc.pos.extra.sun2earth.s.col[0]);
            double endphase = fixangle(endkep.ma + endkep.raan - tovernal);

            double dphase = (1./86400.) * (endphase - startphase) / (endloc.pos.eci.utc - startloc.pos.eci.utc);

            printf("00000000\t");
            printf("%s\t", mjd2iso8601(startloc.pos.eci.utc).c_str());
            printf("%s\t", to_mjd(startloc.pos.eci.utc).c_str());
            printf("%s\t", to_double(DEGOF(startloc.pos.earthsep)).c_str());
            printf("%s\t", to_double(DEGOF(startphase)).c_str());

            printf("%s\t", mjd2iso8601(endloc.pos.eci.utc).c_str());
            printf("%s\t", to_mjd(endloc.pos.eci.utc).c_str());
            printf("%s\t", to_double(DEGOF(endloc.pos.earthsep)).c_str());
            printf("%s\t", to_double(DEGOF(endphase)).c_str());

            printf("%s\t", to_double(DEGOF(dphase), 5).c_str());

            printf("1");
            printf("\n");
        }
    }
}
