/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

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
