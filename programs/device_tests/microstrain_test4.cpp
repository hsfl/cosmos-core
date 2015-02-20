// #include <unistd.h>
#include "microstrain_lib.h"
#include "timelib.h"

int main(int argc, char *argv[])
{
int32_t iretn, handle;
rvector vaccel, vrate, mag;
rvector vrate2, vrate1;
rvector iaccel, irate;
rvector position, velocity, z_one;
rmatrix imatrix, matrix;
double t1, t2, t3, t4;
uint32_t count;
avector euler;

iretn = microstrain_connect(argv[1]);

if (iretn < 0)
	{
	printf("Error: microstrain_connect() %d\n",iretn);
	exit (1);
	}

handle = iretn;

iretn = microstrain_euler_angles(handle, &euler);
printf(" %f %f %f\n",DEGOF(euler.b),DEGOF(euler.e),DEGOF(euler.h));
velocity = rv_zero();
iretn = microstrain_accel_rate_magfield_orientation(handle, &iaccel, &irate, &mag, &imatrix);
//imatrix = rm_transpose(imatrix);
irate = rv_mmult(imatrix,irate);
count = 0;
z_one = velocity = position = rv_zero();
z_one.col[2] = 1.;
while (count < 10000000L)
	{
	iretn = microstrain_accel_rate_magfield_orientation(handle, &vaccel, &vrate, &mag, &matrix);
	matrix = rm_mmult(imatrix,rm_transpose(matrix));
	position = rv_mmult(matrix,z_one);
	vrate1 = rv_sub(rv_mmult(imatrix,vrate),irate);
	vrate2 = rv_sub(rv_mmult(matrix,vrate),irate);
	printf("%+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f\r",vrate1.col[0],vrate2.col[0],vrate1.col[1],vrate2.col[1],vrate1.col[2],vrate2.col[2],position.col[0],position.col[1],position.col[2]);
	fflush(stdout);
//	sleep(1);
	}
iretn = microstrain_temperature(handle,&t1,&t2,&t3,&t4);
printf("\nTemperatures: %f %f %f %f\n",t1,t2,t3,t4);
iretn = microstrain_disconnect(handle);
}
