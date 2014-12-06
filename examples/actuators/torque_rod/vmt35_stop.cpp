#include <unistd.h>
#include "vmt35_lib.h"
#include "microstrain_lib.h"

int main(int argc, char *argv[])
{
int iretn, handle;
int16_t icurrent;
uint16_t ivoltage;
float current;
rvector magfield, mtrvec;

iretn = vmt35_connect((char *)"dev_vmt35");

if (iretn < 0)
	{
	printf("Error: vmt35_connect() %d\n",iretn);
	exit (iretn);
	}

vmt35_enable();
vmt35_set_amps(0,0.);
vmt35_set_amps(1,0.);
vmt35_set_amps(2,0.);
vmt35_disable();

}
