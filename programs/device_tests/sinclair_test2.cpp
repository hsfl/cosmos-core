// #include <unistd.h>
#include "sinclair_lib.h"
#include "timelib.h"

char device[50] = "/dev/wheel";

int main(int argc, char *argv[])
{
int32_t iretn, i;
sinclair_state handle;
//char buf[256];
double mjd, cmjd, speed;

if (argc == 2) strcpy(device,argv[1]);
iretn = sinclair_rw_connect(device,0x11,0x3e, &handle);

if (iretn < 0)
	{
	printf("Error: sinclair_rw_connect() %d\n",iretn);
	exit (1);
	}

iretn = nsp_ping(&handle);
printf("%s\n",(char *)handle.mbuf.data);
iretn = sinclair_mode_accel(&handle,50.);
for (i=1; i<4; i++)
	{
	mjd = currentmjd(0.);
	iretn = sinclair_mode_accel(&handle,i*50.);
	do
		{
		speed = sinclair_get_speed(&handle);
		printf("%.5f\r",speed);
	fflush(stdout);
		} while (speed < 400.);
	cmjd = currentmjd(0.);
	iretn = sinclair_mode_speed(&handle,0.);
	printf("\n%.15g %f\n",86400.*(cmjd-mjd),400./(86400.*(cmjd-mjd)));
	fflush(stdout);

	mjd = currentmjd(0.);
	iretn = sinclair_mode_accel(&handle,-i*50.);
	do
		{
		speed = sinclair_get_speed(&handle);
		printf("%.5f\r",speed);
	fflush(stdout);
		} while (speed > 0.);
	cmjd = currentmjd(0.);
	printf("\n%.15g %f\n",86400.*(cmjd-mjd),400./(86400.*(cmjd-mjd)));
	fflush(stdout);
	iretn = sinclair_mode_speed(&handle,0.);
	}
iretn = sinclair_mode_speed(&handle,0.);
iretn = sinclair_disconnect(&handle);
}
