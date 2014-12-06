#include <iostream>
#include <fstream>
#include <time.h>
#include "microstrain_lib.h"
#include "timelib.h"
#include <sys/time.h>
#include "mathlib.h"

using namespace std;

int main()
{
    int iretn, handle, iretn_mag;
    avector euler;
    //rvector ;
    rvector magfield, magref, magdiff, stablemagfield;
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


        iretn_mag = microstrain_magfield(handle, &magfield);
        iretn_mag = microstrain_stab_accel_rate_magfield(handle,&accel,&rate,&stablemagfield);


        stablemagfieldX = stablemagfield.col[0];
        stablemagfieldY = stablemagfield.col[1];
        stablemagfieldZ = stablemagfield.col[2];

        magdiff = rv_sub(magref,magfield);

        magdiffX = magdiff.col[0];
        magdiffY = magdiff.col[1];
        magdiffZ = magdiff.col[2];

        //
        printf("%d: %' '3.2f %' '3.2f %' '3.2f | ",i, roll,pitch,yaw);
        printf("%' '3.5f %' '3.5f %' '3.5f | ", magX,magY,magZ);
        //printf("%' '3.2f %' '3.2f %' '3.2f | ", magdiffX,magdiffY,magdiffZ,length_rv(magfield));
        printf("%' '3.2f %' '3.2f %' '3.2f | %f \n", stablemagfieldX,stablemagfieldY,stablemagfieldZ,length_rv(stablemagfield));


        //imu_logfile << i << "," << roll << "," << pitch << "," << yaw << "\n";
        imu_lastdatapoint << i << "," << roll << "," << pitch << "," << yaw << "\n";

        imu_lastdatapoint.close();

        i++;

    }


    imu_logfile.close();

    iretn = microstrain_disconnect(handle);
    void cssl_stop();

    return 0;
}

