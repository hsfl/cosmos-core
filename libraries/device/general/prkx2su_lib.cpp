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
static Serial *prkx2su_serial[2];

static prkx2su_state ant_state;

int32_t prkx2su_init(std::string dev)
{
    int32_t iretn;
    string device;

    device = dev + "_az";
    prkx2su_serial[PRKX2SU_AXIS_AZ] = new Serial(device, PRKX2SU_BAUD, PRKX2SU_BITS, PRKX2SU_PARITY, PRKX2SU_STOPBITS);
    device = dev + "_el";
    prkx2su_serial[PRKX2SU_AXIS_EL] = new Serial(device, PRKX2SU_BAUD, PRKX2SU_BITS, PRKX2SU_PARITY, PRKX2SU_STOPBITS);

    return 0;
}

/**
* Connects to am MII prkx2su antenna controller, which in turn
* drives a Yaesu G-5500 antenna controller.
* @param dev pointer to a character string with the serial port it is
* connected to.
* @see cssl_start
* @see cssl_open
* @see cssl_setflowcontrol
*/
int32_t prkx2su_connect()
{
	int32_t iretn;
//	cssl_start();

    if (prkx2su_serial[PRKX2SU_AXIS_AZ]->get_open())
	{
        iretn = prkx2su_disconnect();
        if (iretn < 0)
        {
            return iretn;
        }
    }

    iretn = prkx2su_serial[PRKX2SU_AXIS_AZ]->open_device();
    if (iretn < 0)
	{
        return iretn;
	}

    iretn = prkx2su_serial[PRKX2SU_AXIS_AZ]->set_timeout(.5);
	if (iretn < 0)
	{
        prkx2su_disconnect();
		return iretn;
	}

    iretn = prkx2su_send(PRKX2SU_AXIS_AZ, "", true);
	if (iretn < 0)
	{
        prkx2su_disconnect();
		return iretn;
	}

    if (prkx2su_serial[PRKX2SU_AXIS_EL]->get_open())
	{
        return SERIAL_ERROR_OPEN;
    }

    iretn = prkx2su_serial[PRKX2SU_AXIS_EL]->open_device();
    if (iretn < 0)
    {
        prkx2su_disconnect();
        return iretn;
    }

    iretn = prkx2su_serial[PRKX2SU_AXIS_EL]->set_timeout(.5);
    if (iretn < 0)
	{
		prkx2su_disconnect();
		return iretn;
	}

    iretn = prkx2su_send(PRKX2SU_AXIS_EL, "", true);
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
    int32_t iretn = 0;
    if (prkx2su_serial[PRKX2SU_AXIS_AZ]->get_open())
	{
        iretn = prkx2su_serial[PRKX2SU_AXIS_AZ]->close_device();
    }

	if (prkx2su_serial[PRKX2SU_AXIS_EL] != nullptr)
	{
        iretn = prkx2su_serial[PRKX2SU_AXIS_EL]->close_device();
	}

    return iretn;
}

/**
* Routine to use in blocking mode. Reads the serial port until a New
* Line is received, then returns entire buffer.
* @param axis Axis to read.
* @param buf Pointer to a char buffer
* @param buflen 32 bit signed integer indicating the maximum size of the buffer
* @return 32 bit signed integer containing the number of bytes read.
*/
int32_t prkx2su_getdata(uint8_t axis, string buf, uint16_t buflen)
{
    int32_t j;

    buf.clear();
    while((j=prkx2su_serial[axis]->get_char()) >= 0)
	{
        buf.push_back(static_cast<char>(j));
        if (j == ';' || static_cast<uint16_t>(buf.size()) == buflen)
		{
			break;
		}
	}

    if (j < 0)
    {
        return j;
    }
    else
    {
        return static_cast<int32_t>(buf.size());
    }
}

/**
* Poll controller for Status and Heading. Turns
* calibration mode on in prkx2su.
* @param axis 32 bit signed integer , 0 = Azimuth, 1 = Elevation
*/
int32_t prkx2su_status(uint8_t axis)
{
    int32_t iretn;
    iretn = prkx2su_send(axis, "BIn;", true);
    if (iretn < 0)
    {
        return iretn;
    }
    string buf;
    iretn = prkx2su_getdata(axis, buf, 2000);
    if (iretn < 0)
    {
        return iretn;
    }
    switch (axis)
    {
    case PRKX2SU_AXIS_AZ:
        sscanf(buf.c_str(), "%c%c%f", &ant_state.azid, &ant_state.azstatus, &ant_state.currentaz);
        ant_state.currentaz = RADOF(ant_state.currentaz);
        break;
    case PRKX2SU_AXIS_EL:
        sscanf(buf.c_str(), "%c%c%f", &ant_state.elid, &ant_state.elstatus, &ant_state.currentel);
        ant_state.currentel = RADOF(ant_state.currentel);
        break;
    }
    return iretn;
}

int32_t prkx2su_get_limits(uint8_t axis)
{
    string buf;
    int32_t iretn;
    iretn = prkx2su_send(axis, "RH0;", true);
    if (iretn < 0)
    {
        return iretn;
    }
    iretn = prkx2su_getdata(axis, buf, 200);
    if (iretn < 0)
    {
        return iretn;
    }
    switch (axis)
    {
    case PRKX2SU_AXIS_AZ:
        sscanf(buf.c_str(), "H%f", &ant_state.minaz);
        ant_state.minaz = RADOF(ant_state.minaz);
        break;
    case PRKX2SU_AXIS_EL:
        sscanf(buf.c_str(), "H%f", &ant_state.minel);
        ant_state.minel = RADOF(ant_state.minel);
        break;
    }
    iretn = prkx2su_send(axis, "RI0;", true);
    if (iretn < 0)
    {
        return iretn;
    }
    iretn = prkx2su_getdata(axis, buf, 200);
    if (iretn < 0)
    {
        return iretn;
    }
    switch (axis)
    {
    case PRKX2SU_AXIS_AZ:
        sscanf(buf.c_str(), "H%f", &ant_state.maxaz);
        ant_state.maxaz = RADOF(ant_state.maxaz);
        break;
    case PRKX2SU_AXIS_EL:
        sscanf(buf.c_str(), "H%f", &ant_state.maxel);
        ant_state.maxel = RADOF(ant_state.maxel);
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
    iretn = prkx2su_send(axis, ";", true);
	return iretn;
}

int32_t prkx2su_ramp(uint8_t axis, uint8_t speed)
{
    int32_t iretn;
    char out[50];

    if (speed < 1 || speed > 10)
        return (PRKX2SU_ERROR_OUTOFRANGE);
    sprintf(out,"WNn%03hhu;", speed);
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

int32_t prkx2su_minimum_speed(uint8_t axis, uint8_t speed)
{
    int32_t iretn;
    char out[50];

    if (speed < 1 || speed > 10)
        return (PRKX2SU_ERROR_OUTOFRANGE);
    sprintf(out,"WFn%03hhu;", speed);
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

int32_t prkx2su_maximum_speed(uint8_t axis, uint8_t speed)
{
    int32_t iretn;
    char out[50];

    if (speed < 1 || speed > 10)
        return (PRKX2SU_ERROR_OUTOFRANGE);
    sprintf(out,"WGn%03hhu;", speed);
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

    iretn = prkx2su_serial[axis]->put_string("R10;");
	if (iretn < 0)
	{
		return iretn;
	}

    string buf;
	iretn = prkx2su_getdata(axis, buf, 100);
	if (iretn < 0)
	{
		return iretn;
	}
    if (buf[0] != '1' || buf[1] != 0x1 || buf[buf.size()-1] != ';')
	{
		return PRKX2SU_ERROR_SEND;
	}

	return 0;
}

int32_t prkx2su_send(uint8_t axis, string buf, bool force)
{
	int32_t iretn = 0;
    string lastbuf;

	iretn = prkx2su_test(axis);
	if (iretn < 0)
	{
		return iretn;
	}

    if (lastbuf != buf || force)
	{
        prkx2su_serial[axis]->put_string(buf);
        lastbuf = buf;
	}

	return iretn;
}

int32_t prkx2su_set_sensitivity(float sensitivity)
{
	ant_state.sensitivity = sensitivity;
	return 0;
}
