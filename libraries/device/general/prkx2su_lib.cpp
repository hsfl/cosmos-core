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

#include "support/configCosmos.h"

#include "device/general/prkx2su_lib.h"
#include <cstring>
#include <cmath>

/**
* Internal descriptor for cssl serial control of prkx2su.
* @brief prkx2su serial handle
*/
static cssl_t *prkx2su_serial[2] = {nullptr, nullptr};

static prkx2su_state ant_state;

/**
* Connects to am MII prkx2su antenna controller, which in turn
* drives a Yaesu G-5500 antenna controller.
* @param dev pointer to a character string with the serial port it is
* connected to.
* @see cssl_start
* @see cssl_open
* @see cssl_setflowcontrol
*/
int32_t prkx2su_connect(std::string dev)
{
	int32_t iretn;
	cssl_start();

	if (prkx2su_serial[PRKX2SU_AXIS_AZ] != nullptr)
	{
		prkx2su_disconnect();
	}

	std::string device = dev + "_az";
	prkx2su_serial[PRKX2SU_AXIS_AZ] = cssl_open(device.c_str(), PRKX2SU_BAUD, PRKX2SU_BITS, PRKX2SU_PARITY, PRKX2SU_STOPBITS);
	if (prkx2su_serial[PRKX2SU_AXIS_AZ] == nullptr)
	{
		return CSSL_ERROR_OPEN;
	}

	iretn = cssl_settimeout(prkx2su_serial[PRKX2SU_AXIS_AZ], 0, .1);
	if (iretn < 0)
	{
		prkx2su_disconnect();
		return iretn;
	}

	iretn = prkx2su_send(PRKX2SU_AXIS_AZ, (char *)"", true);
	if (iretn < 0)
	{
		prkx2su_disconnect();
		return iretn;
	}

	if (prkx2su_serial[PRKX2SU_AXIS_EL] != nullptr)
	{
		prkx2su_disconnect();
		prkx2su_serial[PRKX2SU_AXIS_EL] = nullptr;
	}

	device = dev + "_el";
	prkx2su_serial[PRKX2SU_AXIS_EL] = cssl_open(device.c_str(), PRKX2SU_BAUD, PRKX2SU_BITS, PRKX2SU_PARITY, PRKX2SU_STOPBITS);
	if (prkx2su_serial[PRKX2SU_AXIS_EL] == nullptr)
	{
		prkx2su_disconnect();
		return CSSL_ERROR_OPEN;
	}

	iretn = cssl_settimeout(prkx2su_serial[PRKX2SU_AXIS_EL], 0, .1);
	if (iretn < 0)
	{
		prkx2su_disconnect();
		return iretn;
	}

	iretn = prkx2su_send(PRKX2SU_AXIS_EL, (char *)"", true);
	if (iretn < 0)
	{
		prkx2su_disconnect();
		return iretn;
	}

	return 0;
}

/**
* Close currently open prkx2su.
*/
int32_t prkx2su_disconnect()
{
	if (prkx2su_serial[PRKX2SU_AXIS_AZ] != nullptr)
	{
		cssl_close(prkx2su_serial[PRKX2SU_AXIS_AZ]);
		prkx2su_serial[PRKX2SU_AXIS_AZ] = nullptr;
	}

	if (prkx2su_serial[PRKX2SU_AXIS_EL] != nullptr)
	{
		cssl_close(prkx2su_serial[PRKX2SU_AXIS_EL]);
		prkx2su_serial[PRKX2SU_AXIS_EL] = nullptr;
	}

	return 0;
}

/**
* Routine to use in blocking mode. Reads the serial port until a New
* Line is received, then returns entire buffer.
* @param axis Axis to read.
* @param buf Pointer to a char buffer
* @param buflen 32 bit signed integer indicating the maximum size of the buffer
* @return 32 bit signed integer containing the number of bytes read.
*/
int32_t prkx2su_getdata(uint8_t axis, char *buf, int32_t buflen)
{
	int32_t i,j;

	i = 0;
	while ((j=cssl_getchar(prkx2su_serial[axis])) >= 0)
	{
		buf[i++] = j;
        if (j == ';' || i == buflen)
		{
			break;
		}
	}
//	while ((j=cssl_getdata(prkx2su_serial[axis], (uint8_t *)&buf[i],buflen-i)))
//	{
//		if (j < 0)
//		{
//			return j;
//		}
//		else
//		{
//			i += j;
//			if (buf[i-1] == ';')
//				break;
//		}
//	}
	if (j >= 0)
	{
		buf[i] = 0;
		return (i);
	}
	else
	{
		return j;
	}
}

/**
* Poll controller for Status and Heading. Turns
* calibration mode on in prkx2su.
* @param axis 32 bit signed integer , 0 = Azimuth, 1 = Elevation
*/
int32_t prkx2su_status(int8_t axis)
{
	int32_t iretn;
	iretn = prkx2su_send(axis, (char *)"BIn;", true);
	if (iretn < 0)
	{
		return iretn;
	}
	char buf[20];
	iretn = prkx2su_getdata(axis, buf, 20);
	if (iretn < 0)
	{
		return iretn;
	}
	switch (axis)
	{
	case PRKX2SU_AXIS_AZ:
		float az;
		sscanf(buf, "%c%c%f", &ant_state.azid, &ant_state.azstatus, &az);
		ant_state.currentaz = RADOF(az);
		break;
	case PRKX2SU_AXIS_EL:
		float el;
		sscanf(buf, "%c%c%f", &ant_state.elid, &ant_state.elstatus, &el);
		ant_state.currentel = RADOF(el);
		break;
	}
	return iretn;
}

/**
* Routine to stop current action. Whatever the current command is, it will
* cancelled before completeion.
* @return 0 or negative error.
*/
int32_t prkx2su_stop(uint8_t axis)
{
	int32_t iretn;
	iretn = prkx2su_send(axis, (char *)";", true);
	return iretn;
}

int32_t prkx2su_maximum_speed(uint8_t axis, uint8_t speed)
{
	int32_t iretn;
	char out[50];

	if (speed < 1 || speed > 10)
		return (PRKX2SU_ERROR_OUTOFRANGE);
	sprintf(out,"WGn%03hu;",speed);
	iretn = prkx2su_send(axis, out, true);
	if (iretn < 0)
	{
		return iretn;
	}
	else
	{
		return (speed);
	}
}

int32_t prkx2su_goto(float az, float el)
{
	int32_t iretn;
	char out[50];

	if (az < 0 || az > RADOF(360))
	{
		az = fixangle(az);
	}

	if (el < 0)
	{
		el = 0.;
	}
	else if (el > DPI2)
	{
		el = DPI2;
	}

	float daz = az - ant_state.targetaz;
	float del = el - ant_state.targetel;
	float sep = sqrt(daz*daz+del*del);

	if (sep > ant_state.sensitivity)
	{
		ant_state.targetaz = az;
		ant_state.targetel = el;
		az = DEGOF(az);
		el = DEGOF(el);
		sprintf(out, "APn%03d.%1d\r;", (int16_t)az, (int16_t)(10 * (az - (int16_t)az)));
		iretn = prkx2su_send(PRKX2SU_AXIS_AZ, out, true);
		if (iretn >= 0)
		{
			sprintf(out, "APn%03d.%1d\r;", (int16_t)el, (int16_t)(10 * (el - (int16_t)el)));
			iretn = prkx2su_send(PRKX2SU_AXIS_EL, out, true);
			if (iretn >= 0)
			{
				iretn = prkx2su_get_az_el(ant_state.currentaz, ant_state.currentel);
			}
		}
	}

	return 0;
}

float prkx2su_get_az()
{
	return (ant_state.currentaz);
}

float prkx2su_get_el()
{
	return (ant_state.currentel);
}

int32_t prkx2su_get_az_el(float &az, float &el)
{
	int32_t iretn = 0;

	iretn = prkx2su_status(PRKX2SU_AXIS_AZ);
	if (iretn >= 0)
	{
		iretn = prkx2su_status(PRKX2SU_AXIS_EL);
		if (iretn >= 0)
		{
			az = ant_state.currentaz;
			el = ant_state.currentel;
		}
	}
	return iretn;
}

int32_t prkx2su_write_calibration(uint8_t axis, float value)
{
	int32_t iretn;
	char out[50];

	if (value < 0.)
	{
		value = 0.;
	}
	switch (axis)
	{
	case PRKX2SU_AXIS_AZ:
		if (value > D2PI)
		{
			value = D2PI;
		}
		break;
	case PRKX2SU_AXIS_EL:
		if (value > DPI)
		{
			value = DPI;
		}
	}

	value = DEGOF(value);
	sprintf(out, "Awn%03d.%1d;", (int16_t)value, (int16_t)(10 * (value - (int16_t)value)));
	iretn = prkx2su_send(axis, out, true);
	return iretn;
}

float prkx2su_get_az_offset()
{
	return (ant_state.az_offset);
}

float prkx2su_get_el_offset()
{
	return (ant_state.el_offset);
}

void prkx2su_get_state(prkx2su_state &state)
{
	state = ant_state;
}

int32_t prkx2su_test(uint8_t axis)
{
	int32_t iretn;

	iretn = cssl_putstring(prkx2su_serial[axis], (char *)"R10;");
	if (iretn < 0)
	{
		return iretn;
	}

	char buf[100];
	iretn = prkx2su_getdata(axis, buf, 100);
	if (iretn < 0)
	{
		return iretn;
	}
	if (buf[0] != '1' || buf[1] != 0x1 || buf[strlen(buf)-1] != ';')
	{
		return PRKX2SU_ERROR_SEND;
	}

	return 0;
}

int32_t prkx2su_send(uint8_t axis, char *buf, bool force)
{
	int32_t iretn = 0;
	static char lastbuf[256];

	iretn = prkx2su_test(axis);
	if (iretn < 0)
	{
		return iretn;
	}

	if (strcmp(lastbuf,buf) || force)
	{
		iretn = cssl_putstring(prkx2su_serial[axis], buf);
		strncpy(lastbuf,buf,256);
	}

	return iretn;
}

int32_t prkx2su_set_sensitivity(float sensitivity)
{
	ant_state.sensitivity = sensitivity;
	return 0;
}
