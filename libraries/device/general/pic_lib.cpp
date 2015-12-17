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

#include "pic_lib.h"

//! Connect to PIC.
/*! Connect to a PIC speaking NMEA protocol, connected to the
 * specified RS232 device, in preparation for querying it for position
 * measurements.
 * \param dev Name of RS232 character device.
 * \param handle Pointer to ::pic_handle.
 * \return Zero, or negative error number.
*/
int32_t pic_connect(char *dev, pic_handle *handle)
{
cssl_start();
if (handle->serial != NULL)
	return(PIC_ERROR_OPEN);

handle->serial = cssl_open(dev,PIC_BAUD,PIC_BITS,PIC_PARITY,PIC_STOPBITS);
cssl_setflowcontrol(handle->serial,1,0);
if (handle->serial == NULL)
	return (PIC_ERROR_PORT);

cssl_settimeout(handle->serial,1,.1);

handle->buffer.seq = 0;

return 0;
}

//! Disconnect from PIC.
/*! Disconnect from previously connected PIC, closing associated
 * serial device.
 * \param handle Pointer to ::pic_handle.
 * \return Zero, or negative error number.
*/
int32_t pic_disconnect(pic_handle *handle)
{
if (handle->serial == NULL)
	return (PIC_ERROR_CLOSED);

cssl_close(handle->serial);
return 0;
}

int32_t pic_getframe(pic_handle *handle)
{
	int32_t iretn;
	uint16_t size;

	iretn=cssl_getslip(handle->serial, handle->buffer.raw, sizeof(handle->buffer.raw));
	if (iretn < 0)
		return (iretn);

	size = handle->buffer.packet.header.size+PIC_HEADER_SIZE;

	union
	{
		uint16_t crc;
		uint8_t crcbuf[2];
	} tbuf;
	tbuf.crcbuf[0] = handle->buffer.raw[size];
	tbuf.crcbuf[1] = handle->buffer.raw[size+1];
	if (tbuf.crc != slip_calc_crc(handle->buffer.raw, size)	)
		return (SLIP_ERROR_CRC);

	return (iretn);
}

int32_t pic_putframe(pic_handle *handle)
{
	int32_t iretn;
	uint16_t size;

	size = (uint16_t)(handle->buffer.packet.header.size+PIC_HEADER_SIZE);

	union
	{
		uint16_t crc;
		uint8_t crcbuf[2];
	} tbuf;
	tbuf.crc = slip_calc_crc(handle->buffer.raw, size);
	handle->buffer.raw[size] = tbuf.crcbuf[0];
	handle->buffer.raw[size+1] = tbuf.crcbuf[1];

	iretn=cssl_putslip(handle->serial, handle->buffer.raw, size+2);

	return (iretn);
}

int32_t suchi_heater(pic_handle *handle, uint8_t number, uint8_t state)
{
	if (state)
		handle->buffer.packet.header.cmd = SUCHI_CMD_SET_HEATER_ON;
	else
		handle->buffer.packet.header.cmd = SUCHI_CMD_SET_HEATER_OFF;
	handle->buffer.packet.header.flags = 0;
	handle->buffer.packet.header.seq =  handle->buffer.seq++;
	handle->buffer.packet.header.size = sizeof(pic_cmd_args1);

	handle->buffer.packet.args_1_16.arg = number;

	pic_putframe(handle);

	return 0;
}

int32_t suchi_shutter(pic_handle *handle, uint8_t number, uint8_t state)
{
	if (state)
		handle->buffer.packet.header.cmd = SUCHI_CMD_SET_SHUTTER_OPEN;
	else
		handle->buffer.packet.header.cmd = SUCHI_CMD_SET_SHUTTER_CLOSED;
	handle->buffer.packet.header.flags = 0;
	handle->buffer.packet.header.seq =  handle->buffer.seq++;
	handle->buffer.packet.header.size = sizeof(pic_cmd_args1);

	handle->buffer.packet.args_1_16.arg = number;

	pic_putframe(handle);

	return 0;
}

int32_t suchi_camera(pic_handle *handle, uint8_t state)
{
	if (state)
		handle->buffer.packet.header.cmd = SUCHI_CMD_SET_CAMERA_ON;
	else
		handle->buffer.packet.header.cmd = SUCHI_CMD_SET_CAMERA_OFF;
	handle->buffer.packet.header.flags = 0;
	handle->buffer.packet.header.seq =  handle->buffer.seq++;
	handle->buffer.packet.header.size = sizeof(pic_cmd_args0);

	pic_putframe(handle);

	return 0;
}

int32_t suchi_report(pic_handle *handle)
{
	handle->buffer.packet.header.cmd = SUCHI_CMD_GET_REPORT;
	handle->buffer.packet.header.flags = 0;
	handle->buffer.packet.header.seq =  handle->buffer.seq++;
	handle->buffer.packet.header.size = sizeof(pic_cmd_args0);

	pic_putframe(handle);

	return 0;
}

