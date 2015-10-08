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

#include "ic9100_lib.h"

int32_t ic9100_connect(string device, ic9100_handle &handle)
{
	cssl_start();
	handle.serial = cssl_open(device.c_str(), IC9100_BAUD, IC9100_BITS, IC9100_PARITY, IC9100_STOPBITS);

	if (handle.serial == NULL)
	{
		return (CSSL_ERROR_OPEN);
	}
	return 0;
}

int32_t ic9100_disconnect(ic9100_handle &handle)
{
	if (handle.serial == NULL) return (CSSL_ERROR_NOTSTARTED);

	cssl_close(handle.serial);
	return 0;
}

int32_t ic9100_write(ic9100_handle &handle, string message)
{
	int32_t iretn = 0;

	iretn = cssl_putchar(handle.serial, 0xfe);
	if (iretn < 0)
	{
		return iretn;
	}
	iretn = cssl_putchar(handle.serial, 0xfe);
	if (iretn < 0)
	{
		return iretn;
	}
	iretn = cssl_putchar(handle.serial, 0x7c);
	if (iretn < 0)
	{
		return iretn;
	}
	iretn = cssl_putchar(handle.serial, 0xc0);
	if (iretn < 0)
	{
		return iretn;
	}
	iretn = cssl_putdata(handle.serial, (uint8_t *)message.data(), message.length());
	if (iretn < 0)
	{
		return iretn;
	}
	iretn = cssl_putchar(handle.serial, 0xfd);
	if (iretn < 0)
	{
		return iretn;
	}

	uint8_t buffer[100];
	iretn = cssl_getdata(handle.serial, buffer, 100);
	if (iretn < 0)
	{
		return iretn;
	}

	if (buffer[0] != 0xfe || buffer[1] != 0xfe || buffer[2] != 0xe0 || buffer[3] != 0x7c)
	{
		return IC9100_ERROR_WRITE;
	}

	if (buffer[4] != 0xfb)
	{
		return IC9100_ERROR_NG;
	}

	return 0;
}

int32_t ic9100_read(ic9100_handle &handle, string &message)
{
	int32_t iretn = 0;

	uint8_t buffer[100];
	iretn = cssl_getdata(handle.serial, buffer, 100);
	if (iretn < 0)
	{
		return iretn;
	}

	if (buffer[0] != 0xfe || buffer[1] != 0xfe || buffer[2] != 0xe0 || buffer[3] != 0x7c)
	{
		return IC9100_ERROR_WRITE;
	}

	if (iretn > 7)
	{
		message.resize(iretn-7);
		memcpy((void *)message.data(), &buffer[6], message.size());
	}

	return iretn-7;
}

int32_t ic9100_set_channel(ic9100_handle &handle, uint8_t channel)
{
	int32_t iretn = 0;
	string command ("\x7\x0");

	switch (channel)
	{
	case IC9100_CHANNEL_A:
	case IC9100_CHANNEL_B:
		{
			command[1] = channel;
		}
		break;
	default:
		return IC9100_ERROR_OUTOFRANGE;
	}

	iretn = ic9100_write(handle, command);
	return iretn;
}

int32_t ic9100_set_frequency(ic9100_handle &handle, uint8_t channel, double frequency)
{
	int32_t iretn = 0;
	iretn = ic9100_set_channel(handle, channel);
	if (iretn < 0)
	{
		return iretn;
	}

	string command ("\x5\x0\x0\x0\x0\x0\x0");

	if (frequency >= 1e10 || frequency < 0)
	{
		return IC9100_ERROR_OUTOFRANGE;
	}

	frequency = trunc(frequency);
	for (size_t i=0; i<5; ++i)
	{
		command[2+i] = 0;
		for (size_t j=0; j<2; ++j)
		{
			uint8_t digit = fmod(frequency, 10.);
			switch (j)
			{
			case 0:
				command[2+i] += digit;
				break;
			case 1:
				command[2+i] += digit << 4;
				break;
			}
			frequency = trunc(frequency / 10.);
		}
	}

	iretn = ic9100_write(handle, command);
	return iretn;
}

int32_t ic9100_set_mode(ic9100_handle &handle, uint8_t channel, uint8_t mode)
{
	int32_t iretn = 0;
	iretn = ic9100_set_channel(handle, channel);
	if (iretn < 0)
	{
		return iretn;
	}

	switch (mode)
	{
	case IC9100_MODE_AM:
	case IC9100_MODE_CW:
	case IC9100_MODE_CWR:
	case IC9100_MODE_DV:
	case IC9100_MODE_FM:
	case IC9100_MODE_LSB:
	case IC9100_MODE_RTTY:
	case IC9100_MODE_RTTYR:
	case IC9100_MODE_USB:
		break;
	default:
		return IC9100_ERROR_OUTOFRANGE;
		break;
	}

	string command ("\x6\x0\x0\x0");
	command[2] = mode;

	iretn = ic9100_write(handle, command);
	return iretn;
}

int32_t ic9100_set_rfgain(ic9100_handle &handle, uint8_t channel, uint8_t rfgain)
{
	int32_t iretn = 0;

	iretn = ic9100_set_channel(handle, channel);
	if (iretn < 0)
	{
		return iretn;
	}

	string command ("\x14\x0\x2\x0\x0");

	for (size_t i=0; i<2; ++i)
	{
		command[3-i] = 0;
		for (size_t j=0; j<2; ++j)
		{
			uint8_t digit = rfgain % 10;
			switch (j)
			{
			case 0:
				command[3-i] += digit;
				break;
			case 1:
				command[3-i] += digit << 4;
				break;
			}
			rfgain /= 10;
		}
	}

	iretn = ic9100_write(handle, command);
	return iretn;
}

int32_t ic9100_set_squelch(ic9100_handle &handle, uint8_t channel, uint8_t squelch)
{
	int32_t iretn = 0;

	iretn = ic9100_set_channel(handle, channel);
	if (iretn < 0)
	{
		return iretn;
	}

	string command ("\x14\x0\x3\x0\x0");

	for (size_t i=0; i<2; ++i)
	{
		command[3-i] = 0;
		for (size_t j=0; j<2; ++j)
		{
			uint8_t digit = squelch % 10;
			switch (j)
			{
			case 0:
				command[3-i] += digit;
				break;
			case 1:
				command[3-i] += digit << 4;
				break;
			}
			squelch /= 10;
		}
	}

	iretn = ic9100_write(handle, command);
	return iretn;
}

int32_t ic9100_get_frequency(ic9100_handle &handle, uint8_t channel, double &frequency)
{
	int32_t iretn = 0;

	iretn = ic9100_set_channel(handle, channel);
	if (iretn < 0)
	{
		return iretn;
	}

	string command ("\x3\x0");

	iretn = ic9100_write(handle, command);
	if (iretn < 0)
	{
		return iretn;
	}

	string response;
	iretn = ic9100_read(handle, response);
	if (iretn < 0)
	{
		return iretn;
	}

	if (iretn != 5)
	{
		return IC9100_ERROR_OUTOFRANGE;
	}

	frequency = 0.;
	for (size_t i=0; i<5; ++i)
	{
		frequency += 10. * (response[4-i] >> 4) + (response[4-i] % 16);
		frequency *= 100.;
	}

	return iretn;
}

int32_t ic9100_get_mode(ic9100_handle &handle, uint8_t channel, uint8_t &mode)
{
	int32_t iretn = 0;

	iretn = ic9100_set_channel(handle, channel);
	if (iretn < 0)
	{
		return iretn;
	}

	string command ("\x4\x0");

	iretn = ic9100_write(handle, command);
	if (iretn < 0)
	{
		return iretn;
	}

	string response;
	iretn = ic9100_read(handle, response);
	if (iretn < 0)
	{
		return iretn;
	}

	if (iretn != 2)
	{
		return IC9100_ERROR_OUTOFRANGE;
	}

	mode = response[0];

	return iretn;
}

int32_t ic9100_get_rfgain(ic9100_handle &handle, uint8_t channel, uint8_t &rfgain)
{

	return 0;
}

int32_t ic9100_get_squelch(ic9100_handle &handle, uint8_t channel, uint8_t &squelch)
{

	return 0;
}

