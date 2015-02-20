// #include <unistd.h>
#include "sinclair_lib.h"
#include "timelib.h"

int main(int argc, char *argv[])
{
int32_t iretn, i;
sinclair_state handle;
//char buf[256];
double speed, cmjd, mjd;

// Engineering wheel iretn = sinclair_rw_connect(argv[1],0x11,0x3e, &handle);
iretn = sinclair_rw_connect(argv[1],0x11,0x33, &handle);

if (iretn < 0)
	{
	printf("Error: sinclair_rw_connect() %d\n",iretn);
	exit (1);
	}

iretn = nsp_ping(&handle);
if (iretn < 0)
	{
	printf("Error: nsp_ping() %d\n",iretn);
	exit (1);
	}

printf("%s\n",(char *)handle.mbuf.data);
iretn = sinclair_mode_speed(&handle,50.);
for (i=1; i<2; i++)
	{
	mjd = currentmjd(0.);
	iretn = sinclair_mode_accel(&handle,i*50.);
        do
		{
		speed = sinclair_get_speed(&handle);
		printf("%.5f\r",speed);
		} while (speed < 400.);
	cmjd = currentmjd(0.);
	iretn = sinclair_mode_speed(&handle,0.);
	printf("\n%.15g %f\n",86400.*(cmjd-mjd),400./(86400.*(cmjd-mjd)));

	mjd = currentmjd(0.);
		iretn = sinclair_mode_speed(&handle,-50.);
	do
		{
		speed = sinclair_get_speed(&handle);
		printf("%.5f\r",speed);
		} while (speed > 0.);
	cmjd = currentmjd(0.);
	printf("\n%.15g %f\n",86400.*(cmjd-mjd),400./(86400.*(cmjd-mjd)));
	iretn = sinclair_mode_speed(&handle,0.);
	}
iretn = sinclair_mode_speed(&handle,0.);
iretn = sinclair_disconnect(&handle);
}
