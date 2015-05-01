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

#include "configCosmos.h"
#include "vmt35_lib.h"
#include "vn100_lib.h"
#include "agentlib.h"

cosmosstruc *cdata;

int main(int argc, char *argv[])
{
	int32_t iretn;
	vn100_handle vn100handle;
	vmt35_handle vmt35handle;

	switch (argc)
	{
	case 2:
		if (!(cdata=agent_setup_client(SOCKET_TYPE_UDP, argv[1], 1000)))
		{
			printf("Couldn't establish client\n");
			exit (-1);
		}

		if ((iretn=vmt35_connect(cdata[0].port[cdata[0].devspec.tcu[0]->gen.portidx].name, &vmt35handle)) < 0)
		{
			printf("Couldn't connect to VMT35\n");
			exit (1);
		}


		if ((iretn=vn100_connect(cdata[0].port[cdata[0].devspec.imu[0]->gen.portidx].name, &vn100handle)) != 0)
		{
			printf("Couldn't connect to VN100\n");
			exit (1);
		}

		break;
	case 3:
		if ((iretn=vmt35_connect(argv[2], &vmt35handle)) < 0)
		{
			printf("Couldn't connect to VMT35\n");
			exit (1);
		}

		if ((iretn=vn100_connect(argv[1], &vn100handle)) != 0)
		{
			printf("Couldn't connect to VN100\n");
			exit (1);
		}
		break;
	}

	vmt35_set_percent_voltage(&vmt35handle, 0, 100);
	vmt35_set_percent_voltage(&vmt35handle, 1, 100);
	vmt35_set_percent_voltage(&vmt35handle, 2, 100);
	vmt35_enable(&vmt35handle);

//	for (double amp=-.1; amp<=.1; amp+=.001)
	for (int i=0; i<20; ++i)
	{
		if ((iretn=vn100_measurements(&vn100handle)) == 0)
		{
			printf("Magx: %8f Magy: %8f Magz: %8f\n",vn100handle.imu.mag.col[0],vn100handle.imu.mag.col[1],vn100handle.imu.mag.col[2]);
		}

//		double amp;
//		amp = cdata[0].devspec.mtr[i]->poly[0] + cdata[0].devspec.mtr[i]->rmom * (cdata[0].devspec.mtr[i]->poly[1] + cdata[0].devspec.mtr[i]->rmom * cdata[0].devspec.mtr[i]->poly[2]);
		iretn = vmt35_set_amps(&vmt35handle, 0, .09);
		printf("%d\n",iretn);
		iretn = vmt35_set_amps(&vmt35handle, 1, .09);
		printf("%d\n",iretn);
		iretn = vmt35_set_amps(&vmt35handle, 2, .09);
		printf("%d\n",iretn);

		COSMOS_SLEEP(5);

		if ((iretn=vn100_measurements(&vn100handle)) == 0)
		{
			printf("Magx: %8f Magy: %8f Magz: %8f\n",vn100handle.imu.mag.col[0],vn100handle.imu.mag.col[1],vn100handle.imu.mag.col[2]);
		}

		iretn = vmt35_set_amps(&vmt35handle, 0, -.09);
		printf("%d\n",iretn);
		iretn = vmt35_set_amps(&vmt35handle, 1, -.09);
		printf("%d\n",iretn);
		iretn = vmt35_set_amps(&vmt35handle, 2, -.09);
		printf("%d\n",iretn);

		COSMOS_SLEEP(5);

	} ;

	vmt35_set_amps(&vmt35handle, 0, 0.);
	vmt35_set_amps(&vmt35handle, 1, 0.);
	vmt35_set_amps(&vmt35handle, 2, 0.);
	vmt35_set_percent_voltage(&vmt35handle, 0, 0);
	vmt35_set_percent_voltage(&vmt35handle, 1, 0);
	vmt35_set_percent_voltage(&vmt35handle, 2, 0);
	vmt35_disable(&vmt35handle);
	vmt35_disconnect(&vmt35handle);

}
