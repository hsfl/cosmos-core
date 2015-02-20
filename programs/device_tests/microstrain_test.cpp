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
    double sep, tsep, tsep2, lastmjd, mjd;
    uint32_t count;
    avector euler;

    iretn = microstrain_connect((char *)"dev_wheel");

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
    mjd = lastmjd = currentmjd(0.);
    tsep = tsep2 = 0.;
    count = 0;
    while (count < 10000000L)
    {
        iretn = microstrain_accel_rate_magfield_orientation(handle, &vaccel, &vrate, &mag, &matrix);
        target = rv_mmult(matrix,rv_one());
        sep = DEGOF(sep_rv(base,target));
        tsep += sep;
        tsep2 += sep*sep;
        if ((int)(86400.*(currentmjd(0.)-lastmjd)) == 10)
        {
            lastmjd = currentmjd(0.);
            iretn = microstrain_euler_angles(handle, &euler);
            printf("%9.3f\t%.5g\t%.5g\t%.5g\t%.5g\t%.5g \t%.5g\t%d\n",86400.*(lastmjd-mjd),DEGOF(euler.b),DEGOF(euler.e),DEGOF(euler.h),sep,tsep/count,sqrt((tsep2-tsep*tsep/count)/(count-1)),count);
            tsep = tsep2 = 0.;
            count = 0;
        }
        //	printf("[%-+10.5g %-+10.5g %-+10.5g] [%-+10.5g]\r",target.col[0],target.col[1],target.col[2],sep);
        //	printf("[%-+10.5g %-+10.5g %-+10.5g] [%-+12.5g %-+12.5g %-+12.5g] [%-+10.5g %-+10.5g %-+10.5g]\r",vaccel.col[0],vaccel.col[1],vaccel.col[2],vrate.col[0],vrate.col[1],vrate.col[2],mag.col[0],mag.col[1],mag.col[2]);
        //	printf("[%-+10.5g %-+10.5g %-+10.5g] [%-+10.5g %-+10.5g %-+10.5g] [%-+10.5g %-+10.5g %-+10.5g]\r",matrix.row[0].col[0],matrix.row[0].col[1],matrix.row[0].col[2],matrix.row[1].col[0],matrix.row[1].col[1],matrix.row[1].col[2],matrix.row[2].col[0],matrix.row[2].col[1],matrix.row[2].col[2]);
        fflush(stdout);
        //	sleep(1);
        count++;
    }
    iretn = microstrain_temperature(handle,&t1,&t2,&t3,&t4);
    printf("\nTemperatures: %f %f %f %f\n",t1,t2,t3,t4);
    iretn = microstrain_disconnect(handle);
}
