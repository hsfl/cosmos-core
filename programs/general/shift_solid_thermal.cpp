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

int main(int argc, char *argv[])
{
    string filename = argv[1];
    double deltat = atof(argv[2])/86400.;

    string input;

    FILE *fp = fopen(filename.c_str(), "r");
    int ic;
    int32_t id;
    int32_t im;
    int32_t iy;
    int32_t ihh;
    int32_t imm;
    int32_t iss;
    float x;
    float y;
    float z;
    float a;
    float b;
    float c;
    double initialutc = 0.;

    while ((ic=fscanf(fp, "%d/%d/%d %d:%d:%d %f %f %f %f %f %f\n", &im, &id, &iy, &ihh, &imm, &iss, &x, &y, &z, &a, &b, &c)) != EOF)
    {
        Convert::locstruc loc;
        loc.pos.eci.utc = cal2mjd(iy, im, id, ihh, imm, iss);
        if (initialutc == 0.)
        {
            initialutc = loc.pos.eci.utc;
        }
        loc.pos.eci.s.col[0] = x;
        loc.pos.eci.s.col[1] = y;
        loc.pos.eci.s.col[2] = z;
        loc.pos.eci.pass++;
        Convert::pos_eci(loc);
        avector abc(RADOF(a), RADOF(b), RADOF(c));
        loc.att.icrf.utc = loc.pos.eci.utc;
        loc.att.icrf.s = q_euler2quaternion(abc);
        loc.att.icrf.pass++;
        Convert::att_icrf(loc);

        loc.pos.geoc.utc += deltat;
        loc.pos.geoc.pass++;
        Convert::pos_geoc(loc);
        loc.att.geoc.utc += deltat;
        loc.att.geoc.pass++;
        att_geoc(loc);

        abc = a_quaternion2euler(loc.att.icrf.s);
        calstruc date = mjd2cal(loc.pos.eci.utc);

        printf("%d/%d/%d %d:%02d:%02d\t%.0f\t%f\t%f\t%f\t%f\t%f\t%f\n", date.month, date.dom, date.year, date.hour, date.minute, date.second, 86400.*(loc.pos.eci.utc - initialutc), loc.pos.eci.s.col[0], loc.pos.eci.s.col[1], loc.pos.eci.s.col[2], DEGOF(abc.h), DEGOF(abc.e), DEGOF(abc.b));
    };

    fclose(fp);

}
