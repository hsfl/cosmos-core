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

#include "oemv_lib.h"
#include "convertlib.h"

char device[50]="/dev/ttyUSB5";
oemv_handle handle;

int main(int argc, char *argv[])
{
	int32_t iretn;

	/*
unsigned char buffer1[] = {0xAA, 0x44, 0x12, 0x1C , 0x01, 0x00, 0x82, 0x20 , 0x06, 0x00, 0x00, 0x00 , 0xFF, 0xB4, 0xEE, 0x04 , 0x60, 0x5A, 0x05, 0x13 , 0x00, 0x00, 0x4C, 0x00 , 0xFF, 0xFF, 0x5A, 0x80, 0x01, 0x00, 0x00, 0x00 , 0x4F, 0x4B};

uint8_t buffer2[] = {0xAA, 0x44, 0x12, 0x1C , 0x01, 0x00, 0x02, 0x40 , 0x20, 0x00, 0x00, 0x00 , 0x1D, 0x1D, 0x00, 0x00 , 0x29, 0x16, 0x00, 0x00 , 0x00, 0x00, 0x4C, 0x00, 0x55, 0x52, 0x5A, 0x80, 0x20, 0x00, 0x00, 0x00, 0x2A, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

char buffer3[] = "LOGA,COM1,0,66.0,UNKNOWN,0,15.917,004c0000,5255,32858;COM1,BESTPOSA,ONCE,0.000000,0.000000,NOHOLD";
char buffer3[] = "LOGA,COM1,0,0.,UNKNOWN,0,0.,0,0,0;COM1,BESTPOSA,ONCE,0.000000,0.000000,NOHOLD";

	uint32_t crc;

	crc = oemv_calc_crc32((uint8_t *)buffer1, 34);

	cout << hex << crc <<endl;

	crc = oemv_calc_crc32((uint8_t *)buffer2, 60);

	cout << hex << crc <<endl;

	crc = oemv_calc_crc32((uint8_t *)buffer3, strlen(buffer3));

	cout << hex << crc <<endl;
	*/

	if (argc == 2) strcpy(device,argv[1]);

	if ((iretn=oemv_connect(device, &handle)) < 0)
	{
		printf("Failed to connect to OEMV on %s, error %d\n",device,iretn);
		exit (iretn);
	}

	//	iretn = oemv_reset(&handle);
	//		iretn = oemv_saveconfig(&handle);

	if (argc == 6)
	{
		uint32_t week = atoi(argv[2]);
		double seconds = atof(argv[3]);

		double utc = gps2utc(week2gps(week, seconds));

		locstruc loc;
		loc.pos.geod.utc = utc;

		double lat = RADOF(atof(argv[4]));
		double lon = RADOF(atof(argv[5]));
		loc.pos.geod.s.h = 500000.;
		loc.pos.geod.s.lat = lat;
		loc.pos.geod.s.lon = lon;
		((uvector *)&loc.pos.geod.v)->r = rv_zero();
		++loc.pos.geod.pass;
		pos_geod(&loc);
		printf("Position: %f [%f %f %f]\n", utc, loc.pos.geoc.s.col[0], loc.pos.geoc.s.col[1], loc.pos.geoc.s.col[2]);

		iretn = oemv_setapproxpos(&handle, loc.pos.geod.s);
		iretn = oemv_setapproxtime(&handle, utc);
	}

	/*
	iretn = oemv_rxstatus(&handle);
	if (iretn < 0)
	{
		printf("Failed oemv_rxstatus: %d\n", iretn);
	}

	*/
	iretn = oemv_trackstat(&handle);
	if (iretn < 0)
	{
		printf("Failed oemv_trackstat: %d\n", iretn);
	}


	for (uint16_t i=0; i<5; ++i)
	{
		double utc=0.;
		iretn = oemv_time(&handle);
		if (iretn < 0)
		{
			printf("Failed oemv_time: %d\n",iretn);
		}
		else
		{
			utc = cal2mjd(handle.message.time.utc_year, handle.message.time.utc_month, handle.message.time.utc_day+handle.message.time.utc_hour/24.+handle.message.time.utc_minute/1440.+handle.message.time.utc_ms/86400000.);
			printf("{%u %u} %f %f [ %u/%u/%u %u:%u:%f ] %f ", handle.message.time.clock_status, handle.message.time.utc_status, handle.message.time.offset, handle.message.time.utc_offset, handle.message.time.utc_year, handle.message.time.utc_month, handle.message.time.utc_day, handle.message.time.utc_hour, handle.message.time.utc_minute, handle.message.time.utc_ms/1000., utc);
		}

		iretn = oemv_bestxyz(&handle);
		if (iretn < 0)
		{
			printf("Failed oemv_bestxyz: %d\n",iretn);
		}
		else
		{
			locstruc loc;
			loc_clear(&loc);
			loc.pos.geoc.utc = utc;
			loc.pos.geoc.s.col[0] = handle.message.bestxyz.position_x;
			loc.pos.geoc.s.col[1] = handle.message.bestxyz.position_y;
			loc.pos.geoc.s.col[2] = handle.message.bestxyz.position_z;
			loc.pos.geoc.v.col[0] = handle.message.bestxyz.velocity_x;
			loc.pos.geoc.v.col[1] = handle.message.bestxyz.velocity_y;
			loc.pos.geoc.v.col[2] = handle.message.bestxyz.velocity_z;
			++loc.pos.geoc.pass;
			pos_geoc(&loc);
			printf("{%u} %f [%u %f] %s [%f %f %f]\n", handle.message.header.time_status, gps2utc(week2gps(handle.message.header.gps_week, handle.message.header.gps_second)), handle.message.header.gps_week, handle.message.header.gps_second, (char *)handle.data, loc.pos.geod.s.h, DEGOF(loc.pos.geod.s.lat), DEGOF(loc.pos.geod.s.lon));
			printf("{%u %u} %f [%f] %f [%f] %f [%f] ", handle.message.bestxyz.position_status, handle.message.bestxyz.position_type, handle.message.bestxyz.position_x, handle.message.bestxyz.position_x_sd, handle.message.bestxyz.position_y, handle.message.bestxyz.position_y_sd, handle.message.bestxyz.position_z, handle.message.bestxyz.position_z_sd);
			printf("{%u %u} %f [%f] %f [%f] %f [%f]\n", handle.message.bestxyz.velocity_status, handle.message.bestxyz.velocity_type, handle.message.bestxyz.velocity_x, handle.message.bestxyz.velocity_x_sd, handle.message.bestxyz.velocity_y, handle.message.bestxyz.velocity_y_sd, handle.message.bestxyz.velocity_z, handle.message.bestxyz.velocity_z_sd);
		}

	}

	iretn = oemv_disconnect(&handle);
}
