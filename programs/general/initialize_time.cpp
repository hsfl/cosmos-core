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
#include "agent/agentclass.h"
#include "support/timelib.h"

int main(int argc, char *argv[])
{
    int32_t iretn = 0;
    Agent *agent = new Agent();


    double mjdserver = 0.;
    double cmjd = currentmjd();
    double rmjd;
    double epsilon;
    double offset;
    if (argc == 2)
    {
        iretn = agent->get_agent_time(rmjd, epsilon, offset, argv[1]);
    }
    else
    {
        iretn = agent->get_agent_time(rmjd, epsilon, offset, "ntp");
    }
    if (iretn >= 0)
    {
        mjdserver = currentmjd() + offset;
    }

    char hostname[60];
    gethostname(hostname, sizeof (hostname));
    string node_name = hostname;

    double mjdfile=0.;
    // File
    FILE *fp = fopen((get_cosmosnodes() + node_name + "/last_date").c_str(), "r");
    if (fp != nullptr)
    {
        calstruc date;
        iretn = fscanf(fp, "%02d%02d%02d%02d%04d%*c%02d\n", &date.month, &date.dom, &date.hour, &date.minute, &date.year, &date.second);
        fclose(fp);
        mjdfile = cal2mjd(date);
    }

    if (mjdserver > mjdfile)
    {
        double delta = set_local_clock(mjdserver);
        printf("Initialized time from Server: Delta %f %s\n", delta, utc2iso8601(mjdserver).c_str());
    }
    else if (mjdfile > 0.)
    {
        double delta = set_local_clock(mjdfile);
        printf("Initialized time from File: Delta %f %s\n", delta, utc2iso8601(mjdfile).c_str());
    }
    else
    {
        printf("Unable to find time source\n");
    }
}
