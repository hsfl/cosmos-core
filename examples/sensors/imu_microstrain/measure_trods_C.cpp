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

#include <iostream>
#include <fstream>
#include <time.h>
#include "microstrain_lib.h"
#include "timelib.h"
#include <sys/time.h>
#include "mathlib.h"

#include <stdio.h>
#include <unistd.h>
#include "vmt35_lib.h"

using namespace std;

int main()
{
    int iretn, handle, iretn_mag;
    avector euler;
    //rvector ;
    rvector magfield, magref, magdiff, stablemagfield,magCF,rod_F;
    float magX,magY,magZ;
    float magdiffX,magdiffY,magdiffZ;
    float stablemagfieldX,stablemagfieldY,stablemagfieldZ;
    float roll, pitch, yaw;
    rvector accel,rate;
    //struct timeval first, last, delta;
    //struct timezone x;
    //float timeDiff;
    //float times[100];
    //float sum;
    //float avg;
    //float stddev;
    int i = 0;

    ofstream imu_logfile;
    ofstream imu_lastdatapoint;



    //myfile.open ("example.txt");
    imu_logfile.open ("imu_log.dat", fstream::in | fstream::out | fstream::app);
    //imu_lastdatapoint.open ("imu_log.dat", fstream::in | fstream::out | fstream::app);



    iretn = microstrain_connect("/dev/ttyOS3"); //"/dev/ttyUSB0"

    if (iretn < 0)
    {
        printf("Error: microstrain_connect() %d\n",iretn);
        exit (1);
    }

    handle = iretn;

    iretn_mag = microstrain_magfield(handle, &magref);

    while (1) { //i < 1e2

        imu_lastdatapoint.open ("imu_lastdatapoint.dat",ios::trunc);

        iretn = microstrain_euler_angles(handle, &euler);
        roll = DEGOF(euler.b);
        pitch = DEGOF(euler.e);
        yaw  = DEGOF(euler.h);
        magX = magfield.col[0];
        magY = magfield.col[1];
        magZ = magfield.col[2];

		rod_F.col[0]=1;
		rod_F.col[1]=1;
		rod_F.col[2]=0;
		
		
		
		magCF=rv_cross(magfield,rod_F);
		
		
        iretn_mag = microstrain_magfield(handle, &magfield);
        iretn_mag = microstrain_stab_accel_rate_magfield(handle,&accel,&rate,&stablemagfield);


        stablemagfieldX = stablemagfield.col[0];
        stablemagfieldY = stablemagfield.col[1];
        stablemagfieldZ = stablemagfield.col[2];

        magdiff = rv_sub(stablemagfield,magfield);

        magdiffX = magdiff.col[0];
        magdiffY = magdiff.col[1];
        magdiffZ = magdiff.col[2];
		
        //
        printf("%d: %' '3.2f %' '3.2f %' '3.2f | ",i, roll,pitch,yaw);
        printf("%' '3.5f %' '3.5f %' '3.5f | ", magX,magY,magZ);
        printf("%' '3.2f %' '3.2f %' '3.2f | ", magdiffX,magdiffY,magdiffZ,length_rv(magfield));
        printf("%' '3.2f %' '3.2f %' '3.2f |%f  ||", stablemagfieldX,stablemagfieldY,stablemagfieldZ,length_rv(stablemagfield));
        
		printf("%' '3.2f %' '3.2f %' '3.2f | %f \n", magCF.col[0],magCF.col[0],magCF.col[0],length_rv(magCF));

        //imu_logfile << i << "," << roll << "," << pitch << "," << yaw << "\n";
        imu_lastdatapoint << i << "," << roll << "," << pitch << "," << yaw << "\n";

        imu_lastdatapoint.close();

        i++;
		/*
		vmt35_set_amps(0,current);
		vmt35_set_amps(1,current);
		vmt35_set_amps(3,0);
		*/
    }


    imu_logfile.close();

    iretn = microstrain_disconnect(handle);
    void cssl_stop();

    return 0;
}

