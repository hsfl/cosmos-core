// #include <unistd.h>
#include "sinclair_lib.h"

int main(int argc, char *argv[])
{
int32_t iretn, i, j;
sinclair_state handle;
//char buf[256];
char device[15];

printf("About to connect");

for (uint16_t i=1; i<4; i++)
{
	sprintf(device,"/dev/ttyUSB%u",i);
	if ((iretn = sinclair_rw_connect(device,0x11,0x3e, &handle)) >= 0)
	{
		if ((iretn = nsp_ping(&handle)) >= 0) break;
	}
}

if (iretn < 0)
	{
	printf("Error: sinclair_rw_connect() %d\n",iretn);
	exit (1);
	}

printf("Connected");

printf("Ping: iretn=%d buf=%s\n",iretn,(char *)handle.mbuf.data);


for (i=1; i<4; i++)
	{
	iretn = sinclair_mode_speed(&handle,i*50.);
	for (j=0; j<3; j++)
		{
		printf("%.5f\r",sinclair_get_speed(&handle));
		fflush(stdout);
		sleep(1);
		}
	}
for (i=3; i>0; i--)
	{
	iretn = sinclair_mode_speed(&handle,i*50.);
	for (j=0; j<3; j++)
		{
		printf( "%.5f\r",sinclair_get_speed(&handle) );
		fflush(stdout);
		sleep(1);
		}
	}
iretn = sinclair_mode_speed(&handle,0.);
iretn = sinclair_disconnect(&handle);
}
