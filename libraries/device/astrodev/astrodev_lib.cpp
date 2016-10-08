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

#include "device/astrodev/astrodev_lib.h"
#include "support/timelib.h"

#include <cstring>

int32_t astrodev_connect(char *dev, astrodev_handle *handle)
{
	int32_t iretn;
	cssl_start();
	handle->serial = cssl_open(dev,ASTRODEV_BAUD, ASTRODEV_BITS,ASTRODEV_PARITY,ASTRODEV_STOPBITS);
	if (handle->serial == NULL) return (CSSL_ERROR_OPEN);
	cssl_settimeout(handle->serial, 0, .1);
	cssl_setflowcontrol(handle->serial, 0, 0);
	
	if ((iretn=astrodev_ping(handle)) < 0)
	{
		return (iretn);
	}

	return 0;
}

int32_t astrodev_disconnect(astrodev_handle *handle)
{
	if (handle->serial == NULL) return (CSSL_ERROR_NOTSTARTED);

	cssl_close(handle->serial);
	return 0;
}

int32_t astrodev_recvframe(astrodev_handle *handle)
{
	int16_t ch;
	
	union
	{
		uint16_t cs;
		uint8_t csb[2];
	};
	uint16_t size;

	// Wait for first sync character
	do
	{
		ch = cssl_getchar(handle->serial);
		if (ch < 0)
			return (ASTRODEV_ERROR_SYNC0);
	} while (ch != ASTRODEV_SYNC0);
	handle->frame.preamble[0] = ch;

	// Second sync character must immediately follow
	ch = cssl_getchar(handle->serial);
	if (ch < 0 || ch != ASTRODEV_SYNC1)
		return (ASTRODEV_ERROR_SYNC1);
	handle->frame.preamble[1] = ch;

	// Read rest of header
	for (uint16_t i=2; i<8; ++i)
	{
		ch = cssl_getchar(handle->serial);
		if (ch < 0)
			return (ASTRODEV_ERROR_HEADER);
		handle->frame.preamble[i] = ch;
	}

	// Check header for accuracy
	cs = astrodev_calc_cs(&handle->frame.preamble[2], 4);
	if (cs != handle->frame.header.cs)
		return (ASTRODEV_ERROR_HEADER_CS);

	// Check for NOACK
	if (handle->frame.header.size == 65535)
		return (ASTRODEV_ERROR_NACK);

	// Read rest of frame
	switch (handle->frame.header.command)
	{
	// Simple ACK:NOACK
	case ASTRODEV_NOOP:
	case ASTRODEV_RESET:
	case ASTRODEV_TRANSMIT:
	case ASTRODEV_SETTCVCONFIG:
	case ASTRODEV_FLASH:
	case ASTRODEV_RFCONFIG:
	case ASTRODEV_BEACONDATA:
	case ASTRODEV_BEACONCONFIG:
	case ASTRODEV_DIOKEY:
	case ASTRODEV_FIRMWAREUPDATE:
	case ASTRODEV_FIRMWAREPACKET:
	case ASTRODEV_FASTSETPA:
		break;
	default:
		csb[0] = handle->frame.preamble[5];
		csb[1] = handle->frame.preamble[4];
		size = cs;
		for (uint16_t i=0; i<size+2; ++i)
		{
			if((ch = cssl_getchar(handle->serial)) < 0)
			{
				cssl_drain(handle->serial);
				return (ASTRODEV_ERROR_PAYLOAD);
			}
			handle->frame.payload[i] = ch;
		}

		// Check payload for accuracy
		cs = handle->frame.payload[size] | (handle->frame.payload[size+1] << 8L);
		if (cs != astrodev_calc_cs(&handle->frame.preamble[2], 6+size))
			return (ASTRODEV_ERROR_PAYLOAD_CS);
		handle->frame.header.size = size;
		break;
	}

	return (handle->frame.header.command);
}

int32_t astrodev_checkframe(astrodev_frame* frame)
{
	union
	{
		uint16_t cs;
		uint8_t csb[2];
	};
	uint16_t size;

	// Wait for first sync character
	if (frame->preamble[0] != ASTRODEV_SYNC0)
	{
		return (ASTRODEV_ERROR_SYNC0);
	}

	// Second sync character must immediately follow
	if (frame->preamble[1] != ASTRODEV_SYNC1)
	{
		return (ASTRODEV_ERROR_SYNC1);
	}

	// Check header for accuracy
	cs = astrodev_calc_cs(&frame->preamble[2], 4);
	if (cs != frame->header.cs)
		return (ASTRODEV_ERROR_HEADER_CS);

	// Check for NOACK
	if (frame->header.size == 65535)
		return (ASTRODEV_ERROR_NACK);

	// Read rest of frame
	switch (frame->header.command)
	{
	// Simple ACK:NOACK
	case ASTRODEV_NOOP:
	case ASTRODEV_RESET:
	case ASTRODEV_TRANSMIT:
	case ASTRODEV_SETTCVCONFIG:
	case ASTRODEV_FLASH:
	case ASTRODEV_RFCONFIG:
	case ASTRODEV_BEACONDATA:
	case ASTRODEV_BEACONCONFIG:
	case ASTRODEV_DIOKEY:
	case ASTRODEV_FIRMWAREUPDATE:
	case ASTRODEV_FIRMWAREPACKET:
	case ASTRODEV_FASTSETPA:
		break;
	default:
		csb[0] = frame->preamble[5];
		csb[1] = frame->preamble[4];
		size = cs;

		// Check payload for accuracy
		cs = frame->payload[size] | (frame->payload[size+1] << 8L);
		if (cs != astrodev_calc_cs(&frame->preamble[2], 6+size))
			return (ASTRODEV_ERROR_PAYLOAD_CS);
		frame->header.size = size;
		break;
	}

	return (frame->header.command);
}

int32_t astrodev_unloadframe(astrodev_handle *handle, uint8_t *data, uint16_t size)
{
	uint16_t tsize;

	tsize = (handle->frame.header.size <= size?handle->frame.header.size:size);
	memcpy(data, handle->frame.payload, tsize);
	return (tsize);
}

uint16_t astrodev_loadframe(astrodev_handle *handle, uint8_t *data, uint16_t size)
{
	uint16_t tsize;

	tsize = (size <= ASTRODEV_MTU?size:ASTRODEV_MTU);
	handle->frame.header.size = tsize;
	memcpy(handle->frame.payload, data, tsize);

	return (tsize);
}

int32_t astrodev_setupframe(astrodev_frame* frame)
{
	union
	{
		uint16_t cs;
		uint8_t csb[2];
	};

	frame->header.sync0 = ASTRODEV_SYNC0;
	frame->header.sync1 = ASTRODEV_SYNC1;
	frame->header.type = ASTRODEV_COMMAND;
	uint16_t size = frame->header.size;
	cs = size;
	frame->preamble[4] = csb[1];
	frame->preamble[5] = csb[0];
	frame->header.cs = astrodev_calc_cs(&frame->preamble[2], 4);
	cs = astrodev_calc_cs(&frame->preamble[2], 6+size);
	frame->payload[size] = csb[0];
	frame->payload[size+1] = csb[1];

	return 0;
}

int32_t astrodev_sendframe(astrodev_handle *handle)
{
	int32_t iretn;
	if ((iretn=astrodev_setupframe(&handle->frame)) < 0)
	{
		return iretn;
	}

//	double lmjd = currentmjd(0.);
	for (uint16_t i=0; i<8; i++)
	{
		cssl_putchar(handle->serial, handle->frame.preamble[i]);
//		lmjd = currentmjd(0.);
	}

	for (uint16_t i=0; i<handle->frame.header.size+2; i++)
	{
		cssl_putchar(handle->serial, handle->frame.payload[i]);
//		lmjd = currentmjd(0.);
	}

	return 0;
}

int32_t astrodev_ping(astrodev_handle *handle)
{
	int32_t iretn;

	handle->frame.header.command = ASTRODEV_NOOP;
	handle->frame.header.size = 0;
	if ((iretn = astrodev_sendframe(handle)) < 0)
		return (iretn);
	if ((iretn = astrodev_recvframe(handle)) < 0)
		return (iretn);

	return 0;
}

int32_t astrodev_gettcvconfig(astrodev_handle *handle)
{
	int32_t iretn;

	handle->frame.header.command = ASTRODEV_GETTCVCONFIG;
	handle->frame.header.size = 0;
	if ((iretn = astrodev_sendframe(handle)) < 0)
		return (iretn);
	if ((iretn = astrodev_recvframe(handle)) < 0)
		return (iretn);

	return 0;
}

int32_t astrodev_rfconfig(astrodev_handle *handle)
{
	int32_t iretn;

	handle->frame.header.command = ASTRODEV_RFCONFIG;
	handle->frame.header.size = 0;
	if ((iretn = astrodev_sendframe(handle)) < 0)
		return (iretn);
	if ((iretn = astrodev_recvframe(handle)) < 0)
		return (iretn);

	return 0;
}

int32_t astrodev_firmwarerev(astrodev_handle *handle)
{
	int32_t iretn;

	handle->frame.header.command = ASTRODEV_FIRMWAREREV;
	handle->frame.header.size = 0;
	if ((iretn = astrodev_sendframe(handle)) < 0)
		return (iretn);
	if ((iretn = astrodev_recvframe(handle)) < 0)
		return (iretn);

	return 0;
}

int32_t astrodev_telemetry(astrodev_handle *handle)
{
	int32_t iretn;

	handle->frame.header.command = ASTRODEV_TELEMETRY;
	handle->frame.header.size = 0;
	if ((iretn = astrodev_sendframe(handle)) < 0)
		return (iretn);
	if ((iretn = astrodev_recvframe(handle)) < 0)
		return (iretn);

	return 0;
}

int32_t astrodev_settcvconfig(astrodev_handle *handle)
{
	int32_t iretn;

	handle->frame.header.command = ASTRODEV_SETTCVCONFIG;
	handle->frame.header.size = sizeof(astrodev_tcv_config);
	if ((iretn = astrodev_sendframe(handle)) < 0)
		return (iretn);
	if ((iretn = astrodev_recvframe(handle)) < 0)
		return (iretn);

	return 0;
}

int32_t astrodev_receive(astrodev_handle *handle, uint8_t *data, uint16_t size)
{
	int32_t iretn;

	handle->frame.header.command = ASTRODEV_RECEIVE;
	handle->frame.header.size = 0;
	if ((iretn = astrodev_sendframe(handle)) < 0)
		return (iretn);
	if ((iretn = astrodev_recvframe(handle)) < 0)
		return (iretn);
	astrodev_unloadframe(handle, data, size);

	return 0;
}

int32_t astrodev_transmit(astrodev_handle *handle, uint8_t *data, uint16_t size)
{
	int32_t iretn;

	handle->frame.header.command = ASTRODEV_TRANSMIT;
	astrodev_loadframe(handle, data, size);
	if ((iretn = astrodev_sendframe(handle)) < 0)
		return (iretn);
	if ((iretn = astrodev_recvframe(handle)) < 0)
		return (iretn);

	return 0;
}

uint16_t astrodev_calc_cs(uint8_t *data, uint16_t size)
{
	uint8_t ck_a=0, ck_b=0;
	uint16_t cs;

	for (uint16_t i=0; i<size; ++i)
	{
		ck_a += data[i];
		ck_b += ck_a;
	}
	cs = ck_a | (ck_b << 8L);

	return (cs);
}
