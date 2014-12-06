#include "mathlib.h"

int main(int argc, char *argv[])
{
	rvector sourcea = {{1., 0., 0.}};
	rvector sourceb = {{0., 1., 0.}};
	rvector targeta;
	rvector targetb;

	switch (argc)
	{
	case 13:
		// sourcea, sourceb, targeta, targetb
		{
			for (uint16_t i=0; i<3; ++i)
			{
				sourcea.col[i] = atof(argv[i+1]);
				sourceb.col[i] = atof(argv[i+4]);
				targeta.col[i] = atof(argv[i+7]);
				targetb.col[i] = atof(argv[i+10]);
			}
		}
		break;
	case 7:
		// targetx, targety
		{
			for (uint16_t i=0; i<3; ++i)
			{
				targeta.col[i] = atof(argv[i+1]);
				targetb.col[i] = atof(argv[i+4]);
			}
		}
		break;
	default:
		{
			printf("Usage: calc_transform sourcea_x sourcea_y sourcea_z sourceb_x sourceb_y sourceb_z targeta_x targeta_y targeta_z targetb_x targetb_y targetb_z\n");
			printf("Usage: calc_transform targetx_x targetx_y targetx_z targety_x targety_y targety_z\n");
			exit(1);
		}
		break;
	}

	quaternion tq = q_transform_for(sourcea, sourceb, targeta, targetb);

	printf("%f [ %f, %f, %f ]\n", tq.w, tq.d.x, tq.d.y, tq.d.z);
}
