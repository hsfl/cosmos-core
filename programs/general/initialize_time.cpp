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
    Agent *agent = new Agent();


    FILE *fp = fopen(("/cosmos/nodes/" + agent->nodeName + "/last_date").c_str(), "r");
    if (fp != nullptr)
    {
        calstruc date;
        int32_t offset = 0;
        fscanf(fp, "%02d%02d%02d%02d%04d%*c%02d\n", &date.month, &date.dom, &date.hour, &date.minute, &date.year, &date.second);
        fclose(fp);
        fp = fopen(("/cosmos/nodes/" + agent->nodeName + "/last_offset").c_str(), "r");
        if (fp != nullptr)
        {
            fscanf(fp, "%d", &offset);
        }
        date.second += offset;
        double delta = cal2mjd(date) -  currentmjd();
        if (delta > 3.5e-4)
        {
            delta = set_local_clock(cal2mjd(date));
            printf("Initialized time from file: Delta %f Offset %d\n", delta, offset);
        }
    }
    else {
        double rmjd;
        double epsilon;
        double offset;
        int32_t iretn = agent->get_agent_time(rmjd, epsilon, offset, "ntp");
        if (iretn >= 0)
        {
            if (offset > 3.5e-4)
            {
                double delta = set_local_clock(currentmjd() + offset);
                printf("Initialized time from server: Delta %f Offset %f\n", delta, offset*86400.);
            }
        }
        else {
            printf("Failed to find time server\n");
        }
    }

}
