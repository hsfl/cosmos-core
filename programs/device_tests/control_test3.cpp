// #include <unistd.h>
#include "microstrain_lib.h"
#include "sinclair_lib.h"
#include "timelib.h"

int main(int argc, char *argv[])
{
int32_t iretn, mh, i, direction;
sinclair_state sh;
rvector vaccel, vrate, mag, irate;
rvector vrate2;
avector euler;
rmatrix matrix, imatrix;
double maxaccel, maxspeed;
double lastmjd, mjd, sspeed;

iretn = microstrain_connect((char *)"derv_imu");
if (iretn < 0)
	{
	printf("Error: microstrain_connect() %d\n",iretn);
	exit (1);
	}
mh = iretn;

iretn = sinclair_rw_connect((char *)"derv_wheel",0x11,0x3e, &sh);
if (iretn < 0)
	{
	printf("Error: sinclair_rw_connect() %d\n",iretn);
	iretn = microstrain_disconnect(mh);
	exit (1);
	}

//microstrain_capture_gyro_bias(mh,10000,&bias);

iretn = microstrain_accel_rate_magfield_orientation(mh, &vaccel, &irate, &mag, &imatrix);
imatrix = rm_transpose(imatrix);
irate = rv_mmult(imatrix,irate);

maxspeed = 300.;
maxaccel = 30.;
mjd = lastmjd = currentmjd(0.);
for (i=0; i<3; i++)
	{
	switch (i)
		{
	case 0:
	case 2:
		direction = 1;
		break;
	case 1:
		direction = -1;
		break;
		}
	iretn = sinclair_mode_accel(&sh,direction*maxaccel);
	do
		{
		iretn = microstrain_accel_rate_magfield_orientation(mh, &vaccel, &vrate, &mag, &matrix);
		matrix = rm_mmult(rm_transpose(matrix),imatrix);
		euler = a_quaternion2euler(q_dcm2quaternion_rm(matrix));
//		vrate1 = rv_sub(rv_mmult(imatrix,vrate),irate);
		vrate2 = rv_sub(rv_mmult(matrix,vrate),irate);
//		vrate = rv_sub(vrate,irate);
		sspeed = sinclair_get_speed(&sh);
		lastmjd = currentmjd(0.);
		printf("%10.5f %10.5f %5.1f %10.5f %10.5f\n",86400.*(lastmjd-mjd),euler.h,direction*maxaccel,vrate2.col[2],sspeed);
		fflush(stdout);
		} while (direction>0?sspeed<maxspeed:sspeed>-maxspeed);
	}

sinclair_mode_accel(&sh,0.);
sinclair_mode_speed(&sh,0.);
lastmjd = currentmjd(0.);
do
	{
	iretn = microstrain_accel_rate_magfield_orientation(mh, &vaccel, &vrate, &mag, &matrix);
	matrix = rm_mmult(rm_transpose(matrix),imatrix);
	euler = a_quaternion2euler(q_dcm2quaternion_rm(matrix));
//	vrate1 = rv_mmult(imatrix,rv_sub(vrate,irate));
	vrate2 = rv_sub(rv_mmult(matrix,vrate),irate);
//	vrate = rv_sub(vrate,irate);
	sspeed = sinclair_get_speed(&sh);
	printf("%10.5f %10.5f 0.0 %10.5f %10.5f\n",86400.*(currentmjd(0.)-mjd),euler.h,vrate2.col[2],sspeed);
	fflush(stdout);
	} while (86400.*(currentmjd(0.)-lastmjd) < 25.);
iretn = microstrain_disconnect(mh);
iretn = sinclair_disconnect(&sh);
}
