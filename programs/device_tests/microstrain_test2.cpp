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

// #include <unistd.h>
#include "microstrain_lib.h"
#include "timelib.h"

int main(int argc, char *argv[])
{
    int32_t iretn, handle, i;
    char buf[256];
    rvector vaccel, vrate, mag;
    rvector base, target;
    rvector bias;
    rmatrix matrix;
    double t1, t2, t3, t4;
    double sep, lastmjd;
    uint32_t count;
    avector euler;

    iretn = microstrain_connect("derv_imu");

    if (iretn < 0)
    {
        printf("Error: microstrain_connect() %d\n",iretn);
        exit (1);
    }

    handle = iretn;
    for (i=0; i<4; i++)
    {
        iretn = microstrain_device_identifier(handle,(uint8_t)i,buf);
        printf("%s\n",buf);
    }

    microstrain_capture_gyro_bias(handle,10000,&bias);
    printf("[ %8g %8g %8g ]\n",bias.col[0],bias.col[1],bias.col[2]);

    iretn = microstrain_euler_angles(handle, &euler);
    printf(" %f %f %f\n",DEGOF(euler.b),DEGOF(euler.e),DEGOF(euler.h));
    iretn = microstrain_accel_rate_magfield_orientation(handle, &vaccel, &vrate, &mag, &matrix);
    base = rv_mmult(matrix,rv_one());
    lastmjd = currentmjd(0.);
    count = 0;
    while (count < 10000000L)
    {
        iretn = microstrain_accel_rate_magfield_orientation(handle, &vaccel, &vrate, &mag, &matrix);
        target = rv_mmult(matrix,rv_one());
        sep = DEGOF(sep_rv(base,target));
        iretn = microstrain_euler_angles(handle, &euler);
        printf("%10.3f %+-10.6g %+-10.6g %+-10.6g %+-10.6g\r",86400.*(lastmjd-currentmjd(0.)),DEGOF(euler.b),DEGOF(euler.e),DEGOF(euler.h),sep);
        lastmjd = currentmjd(0.);
        fflush(stdout);
        //	COSMOS_SLEEP(1);
    }
    iretn = microstrain_temperature(handle,&t1,&t2,&t3,&t4);
    printf("\nTemperatures: %f %f %f %f\n",t1,t2,t3,t4);
    iretn = microstrain_disconnect(handle);
}
