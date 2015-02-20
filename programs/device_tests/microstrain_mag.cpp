// #include <unistd.h>
#include "microstrain_lib.h"
#include "timelib.h"

int main(int argc, char *argv[])
{
	int32_t iretn, handle;
	rvector mag, magm, mags;
	uint32_t count;

	iretn = microstrain_connect(argv[1]);
	COSMOS_USLEEP(100000);

	if (iretn < 0)
		{
		printf("Error: microstrain_connect() %d\n",iretn);
		exit (1);
		}

	handle = iretn;

	count = 1;
	iretn = microstrain_magfield(handle, &magm);
	COSMOS_USLEEP(100000);
	mags = rv_mult(magm, magm);
	for (uint16_t i=0; i<19; ++i)
	{
		iretn = microstrain_magfield(handle, &mag);
		COSMOS_USLEEP(100000);
		magm = rv_add(magm, mag);
		mags = rv_add(mags, rv_mult(mag, mag));
		++count;
	}

	mags = rv_sqrt(rv_smult(1./19., rv_sub(mags, rv_smult(1./20., rv_mult(magm, magm)))));
	magm = rv_smult(1./20., magm);

	printf("%+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f %+-10.5f\n",magm.col[0], magm.col[1], magm.col[2], mags.col[0], mags.col[1], mags.col[2], length_rv(magm));
	iretn = microstrain_disconnect(handle);
}
