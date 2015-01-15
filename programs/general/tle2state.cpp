#include "configCosmos.h"
#include "convertlib.h"
#include "jsonlib.h"


int main(int argc, char *argv[])
{
	cartpos eci;
	vector <tlestruc> tle;
	double utc = 0.;
	string tlename;

	switch (argc)
	{
	case 3:
		utc = atof(argv[2]);
	case 2:
		tlename = argv[1];
		break;
	default:
		printf("Usage: tle2eci tlename [mjd] \n");
		exit(1);
		break;
	}

	load_lines(argv[1], tle);

	if (utc == 0.)
	{
		utc = tle[0].utc;
	}

	tle2eci(utc, tle[0], &eci);
	eci.utc = utc;

	string jsp;
	json_out_ecipos(jsp, eci);

	printf("%s\n", jsp.c_str());
}
