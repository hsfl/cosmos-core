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
#include "jsondef.h"

int32_t ic9100_connect(string device, uint8_t address, ic9100_handle &handle)
{
	int32_t iretn;

	cssl_start();
	handle.serial = cssl_open(device.c_str(), IC9100_BAUD, IC9100_BITS, IC9100_PARITY, IC9100_STOPBITS);

	if (handle.serial == NULL)
	{
		return (CSSL_ERROR_OPEN);
	}

	handle.address = address;
	iretn = ic9100_check_address(handle);

	return iretn;
}

int32_t ic9100_disconnect(ic9100_handle &handle)
{
	if (handle.serial == NULL) return (CSSL_ERROR_NOTSTARTED);

	cssl_close(handle.serial);
	return 0;
}

int32_t ic9100_write_header(ic9100_handle &handle)
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
	iretn = cssl_putchar(handle.serial, handle.address);
	if (iretn < 0)
	{
		return iretn;
	}
	iretn = cssl_putchar(handle.serial, 0xe0);
	if (iretn < 0)
	{
		return iretn;
	}

	return 0;
}

int32_t ic9100_write(ic9100_handle &handle, uint8_t command)
{
	int32_t iretn = 0;

	vector <uint8_t> data;
	iretn = ic9100_write(handle, command, data);
	return iretn;
}

int32_t ic9100_write(ic9100_handle &handle, uint8_t command, uint8_t subcommand)
{
	int32_t iretn = 0;

	vector <uint8_t> data;
	iretn = ic9100_write(handle, command, subcommand, data);
	return iretn;
}

int32_t ic9100_write(ic9100_handle &handle, uint8_t command, vector <uint8_t> message)
{
	int32_t iretn = 0;

	iretn = ic9100_write_header(handle);
	if (iretn < 0)
	{
		return iretn;
	}

	iretn = cssl_putchar(handle.serial, command);
	if (iretn < 0)
	{
		return iretn;
	}

	if (message.size())
	{
		iretn = cssl_putdata(handle.serial, (uint8_t *)message.data(), message.size());
		if (iretn < 0)
		{
			return iretn;
		}
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
	int32_t base = message.size() + 6;

	if (iretn < base)
	{
		return IC9100_ERROR_WRITE;
	}

	if (iretn == base)
	{
		return IC9100_ERROR_ADDR;
	}

	if (buffer[base] != 0xfe || buffer[base+1] != 0xfe || buffer[base+2] != 0xe0 || buffer[base+3] != handle.address || buffer[iretn-1] != 0xfd)
	{
		return IC9100_ERROR_WRITE;
	}

	if (buffer[base+4] == 0xfa)
	{
		return IC9100_ERROR_NG;
	}

	if (buffer[base+4] == 0xfb)
	{
		handle.response.resize(0);
		return 0;
	}
	else
	{
		base += 5;
		handle.response.resize(iretn-(base+1));
		memcpy((void *)handle.response.data(), &buffer[base], iretn-(base+1));
		return iretn-(base+1);
	}

}

int32_t ic9100_write(ic9100_handle &handle, uint8_t command, uint8_t subcommand, vector <uint8_t> message)
{
	int32_t iretn = 0;

	iretn = ic9100_write_header(handle);
	if (iretn < 0)
	{
		return iretn;
	}

	iretn = cssl_putchar(handle.serial, command);
	if (iretn < 0)
	{
		return iretn;
	}
	iretn = cssl_putchar(handle.serial, subcommand);
	if (iretn < 0)
	{
		return iretn;
	}
	if (message.size())
	{
		iretn = cssl_putdata(handle.serial, (uint8_t *)message.data(), message.size());
		if (iretn < 0)
		{
			return iretn;
		}
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

	int32_t base = message.size() + 7;
	if (iretn < base)
	{
		return IC9100_ERROR_WRITE;
	}

	if (iretn == base)
	{
		return IC9100_ERROR_ADDR;
	}

	if (buffer[base] != 0xfe || buffer[base+1] != 0xfe || buffer[base+2] != 0xe0 || buffer[base+3] != handle.address || buffer[iretn-1] != 0xfd)
	{
		return IC9100_ERROR_WRITE;
	}

	if (buffer[base+4] == 0xfa)
	{
		return IC9100_ERROR_NG;
	}

	if (buffer[base+4] == 0xfb)
	{
		handle.response.resize(0);
		return 0;
	}
	else
	{
		base += 6;
		handle.response.resize(iretn-(base+1));
		memcpy((void *)handle.response.data(), &buffer[base], iretn-(base+1));
		return iretn-(base+1);
	}
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

	if (buffer[0] != 0xfe || buffer[1] != 0xfe || buffer[2] != 0xe0 || buffer[3] != handle.address)
	{
		return IC9100_ERROR_READ;
	}

	if (iretn > 7)
	{
		message.resize(iretn-7);
		memcpy((void *)message.data(), &buffer[6], message.size());
	}

	return iretn-7;
}

int32_t ic9100_set_channel(ic9100_handle &handle, uint8_t channelnum)
{
	int32_t iretn = 0;

	switch (channelnum)
	{
	case IC9100_CHANNEL_A:
	case IC9100_CHANNEL_B:
		{
			iretn = ic9100_write(handle, 0x7, 0xd0+channelnum);
		}
		break;
	default:
		return IC9100_ERROR_OUTOFRANGE;
	}

	if (iretn < 0)
	{
		return iretn;
	}

	handle.channelnum = channelnum;

	return 0;
}

int32_t ic9100_set_frequency(ic9100_handle &handle, double frequency)
{
	int32_t iretn = 0;
	if (iretn < 0)
	{
		return iretn;
	}

	vector <uint8_t> data { 0x0,0x0,0x0,0x0,0x0 };

	if (frequency >= 1e10 || frequency < 0)
	{
		return IC9100_ERROR_OUTOFRANGE;
	}

	frequency = trunc(frequency);
	for (size_t i=0; i<5; ++i)
	{
		data[i] = 0;
		for (size_t j=0; j<2; ++j)
		{
			uint8_t digit = fmod(frequency, 10.);
			switch (j)
			{
			case 0:
				data[i] += digit;
				break;
			case 1:
				data[i] += digit << 4;
				break;
			}
			frequency = trunc(frequency / 10.);
		}
	}

	iretn = ic9100_write(handle, 0x5, data);
	if (iretn < 0)
	{
		return iretn;
	}

	if (frequency < 1.8e6)
	{
		handle.channel[handle.channelnum].freqband = 14;
	}
	else if (frequency < 2.0e6)
	{
		handle.channel[handle.channelnum].freqband = 1;
	}
	else if (frequency >= 3.4e6 && frequency < 4.1e6)
	{
		handle.channel[handle.channelnum].freqband = 2;
	}
	else if (frequency >= 6.9e6 && frequency < 7.5e6)
	{
		handle.channel[handle.channelnum].freqband = 3;
	}
	else if (frequency >= 9.9e6 && frequency < 10.5e6)
	{
		handle.channel[handle.channelnum].freqband = 4;
	}
	else if (frequency >= 13.9e6 && frequency < 14.5e6)
	{
		handle.channel[handle.channelnum].freqband = 5;
	}
	else if (frequency >= 17.9e6 && frequency < 18.5e6)
	{
		handle.channel[handle.channelnum].freqband = 6;
	}
	else if (frequency >= 20.9e6 && frequency < 21.5e6)
	{
		handle.channel[handle.channelnum].freqband = 7;
	}
	else if (frequency >= 24.4e6 && frequency < 25.1e6)
	{
		handle.channel[handle.channelnum].freqband = 8;
	}
	else if (frequency >= 28.0e6 && frequency < 30.0e6)
	{
		handle.channel[handle.channelnum].freqband = 9;
	}
	else if (frequency >= 50.0e6 && frequency <= 54.0e6)
	{
		handle.channel[handle.channelnum].freqband = 10;
	}
	else if (frequency >= 108.0e6 && frequency <= 174.0e6)
	{
		handle.channel[handle.channelnum].freqband = 11;
	}
	else if (frequency >= 420.0e6 && frequency <= 480.0e6)
	{
		handle.channel[handle.channelnum].freqband = 12;
	}
	else if (frequency >= 1240.0e6 && frequency <1320.0e6)
	{
		handle.channel[handle.channelnum].freqband = 13;
	}
	else
	{
		handle.channel[handle.channelnum].freqband = 14;
	}
	handle.channel[handle.channelnum].frequency = frequency;

	return 0;
}

int32_t ic9100_set_mode(ic9100_handle &handle, uint8_t mode)
{
	int32_t iretn = 0;
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

	vector <uint8_t> data { 0x0 };
	data[0] = mode;

	iretn = ic9100_write(handle, 0x6, data);
	if (iretn < 0)
	{
		return iretn;
	}

	switch (mode)
	{
	case IC9100_MODE_AM:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_AM;
		break;
	case IC9100_MODE_CW:
	case IC9100_MODE_CWR:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_CW;
		break;
	case IC9100_MODE_DV:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_DV;
		break;
	case IC9100_MODE_FM:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_FM;
		break;
	case IC9100_MODE_LSB:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_LSB;
		break;
	case IC9100_MODE_RTTY:
	case IC9100_MODE_RTTYR:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_RTTY;
		break;
	case IC9100_MODE_USB:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_USB;
		break;
	default:
		return IC9100_ERROR_OUTOFRANGE;
		break;
	}

	return 0;
}

int32_t ic9100_set_rfgain(ic9100_handle &handle, uint8_t rfgain)
{
	int32_t iretn = 0;

	if (iretn < 0)
	{
		return iretn;
	}

	vector <uint8_t> data { 0x0,0x0 };

	for (size_t i=0; i<2; ++i)
	{
		data[1-i] = 0;
		for (size_t j=0; j<2; ++j)
		{
			uint8_t digit = rfgain % 10;
			switch (j)
			{
			case 0:
				data[1-i] += digit;
				break;
			case 1:
				data[1-i] += digit << 4;
				break;
			}
			rfgain /= 10;
		}
	}

	iretn = ic9100_write(handle, 0x14, 0x2, data);
	return iretn;
}

int32_t ic9100_set_squelch(ic9100_handle &handle, uint8_t squelch)
{
	int32_t iretn = 0;

	if (iretn < 0)
	{
		return iretn;
	}

	vector <uint8_t> data { 0x0,0x0 };

	for (size_t i=0; i<2; ++i)
	{
		data[1-i] = 0;
		for (size_t j=0; j<2; ++j)
		{
			uint8_t digit = squelch % 10;
			switch (j)
			{
			case 0:
				data[1-i] += digit;
				break;
			case 1:
				data[1-i] += digit << 4;
				break;
			}
			squelch /= 10;
		}
	}

	iretn = ic9100_write(handle, 0x14, 0x3, data);
	return iretn;
}

int32_t ic9100_set_rfpower(ic9100_handle &handle, float power)
{
	uint8_t rfpower;
	int32_t iretn = 0;

	if (iretn < 0)
	{
		return iretn;
	}

	if (handle.channel[handle.channelnum].freqband < 11)
	{
		if (power < 2. || power > (handle.channel[handle.channelnum].mode==DEVICE_RADIO_MODE_AM?30.:100.))
		{
			return IC9100_ERROR_OUTOFRANGE;
		}
		rfpower = (power - 2.) / (handle.channel[handle.channelnum].mode==DEVICE_RADIO_MODE_AM?28.:98.);
	}
	else if (handle.channel[handle.channelnum].freqband < 12)
	{
		if (power < 2. || power > (100.))
		{
			return IC9100_ERROR_OUTOFRANGE;
		}
		rfpower = (power - 2.) / (98.);
	}
	else if (handle.channel[handle.channelnum].freqband < 13)
	{
		if (power < 2. || power > 75.)
		{
			return IC9100_ERROR_OUTOFRANGE;
		}
		rfpower = (power - 2.) / 73.;
	}
	else if (handle.channel[handle.channelnum].freqband < 14)
	{
		if (power < 2. || power > 10.)
		{
			return IC9100_ERROR_OUTOFRANGE;
		}
		rfpower = (power - 2.) / 8.;
	}
	vector <uint8_t> data { 0x0,0x0 };

	for (size_t i=0; i<2; ++i)
	{
		data[1-i] = 0;
		for (size_t j=0; j<2; ++j)
		{
			uint8_t digit = rfpower % 10;
			switch (j)
			{
			case 0:
				data[1-i] += digit;
				break;
			case 1:
				data[1-i] += digit << 4;
				break;
			}
			rfpower /= 10;
		}
	}

	iretn = ic9100_write(handle, 0x14, 0xa, data);
	if (iretn < 0)
	{
		return iretn;
	}

	handle.channel[handle.channelnum].power = power;
	return 0;
}

int32_t ic9100_check_address(ic9100_handle &handle)
{
	int32_t iretn = 0;

	iretn = ic9100_write(handle, 0x19, 0x0);
	if (iretn < 0)
	{
		return iretn;
	}

	if (handle.address != handle.response[0])
	{
		return IC9100_ERROR_OUTOFRANGE;
	}

	return 0;
}

int32_t ic9100_get_frequency(ic9100_handle &handle)
{
	int32_t iretn = 0;

	if (iretn < 0)
	{
		return iretn;
	}

	iretn = ic9100_write(handle, 0x3);
	if (iretn < 0)
	{
		return iretn;
	}

	if (iretn != 5)
	{
		return IC9100_ERROR_OUTOFRANGE;
	}

	double frequency = 0.;
	for (size_t i=0; i<5; ++i)
	{
		frequency *= 100.;
		frequency += 10. * (handle.response[4-i] >> 4) + (handle.response[4-i] % 16);
	}
	handle.channel[handle.channelnum].frequency = frequency;

	if (frequency < 1.8e6)
	{
		handle.channel[handle.channelnum].freqband = 14;
	}
	else if (frequency < 2.0e6)
	{
		handle.channel[handle.channelnum].freqband = 1;
	}
	else if (frequency >= 3.4e6 && frequency < 4.1e6)
	{
		handle.channel[handle.channelnum].freqband = 2;
	}
	else if (frequency >= 6.9e6 && frequency < 7.5e6)
	{
		handle.channel[handle.channelnum].freqband = 3;
	}
	else if (frequency >= 9.9e6 && frequency < 10.5e6)
	{
		handle.channel[handle.channelnum].freqband = 4;
	}
	else if (frequency >= 13.9e6 && frequency < 14.5e6)
	{
		handle.channel[handle.channelnum].freqband = 5;
	}
	else if (frequency >= 17.9e6 && frequency < 18.5e6)
	{
		handle.channel[handle.channelnum].freqband = 6;
	}
	else if (frequency >= 20.9e6 && frequency < 21.5e6)
	{
		handle.channel[handle.channelnum].freqband = 7;
	}
	else if (frequency >= 24.4e6 && frequency < 25.1e6)
	{
		handle.channel[handle.channelnum].freqband = 8;
	}
	else if (frequency >= 28.0e6 && frequency < 30.0e6)
	{
		handle.channel[handle.channelnum].freqband = 9;
	}
	else if (frequency >= 50.0e6 && frequency <= 54.0e6)
	{
		handle.channel[handle.channelnum].freqband = 10;
	}
	else if (frequency >= 108.0e6 && frequency <= 174.0e6)
	{
		handle.channel[handle.channelnum].freqband = 11;
	}
	else if (frequency >= 420.0e6 && frequency <= 480.0e6)
	{
		handle.channel[handle.channelnum].freqband = 12;
	}
	else if (frequency >= 1240.0e6 && frequency <1320.0e6)
	{
		handle.channel[handle.channelnum].freqband = 13;
	}
	else
	{
		handle.channel[handle.channelnum].freqband = 14;
	}
	return iretn;
}

int32_t ic9100_get_mode(ic9100_handle &handle)
{
	int32_t iretn = 0;

	if (iretn < 0)
	{
		return iretn;
	}

	iretn = ic9100_write(handle, 0x4);
	if (iretn < 0)
	{
		return iretn;
	}

	if (iretn != 2)
	{
		return IC9100_ERROR_OUTOFRANGE;
	}

	switch (handle.response[0])
	{
	case IC9100_MODE_AM:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_AM;
		break;
	case IC9100_MODE_FM:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_FM;
		break;
	case IC9100_MODE_LSB:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_LSB;
		break;
	case IC9100_MODE_USB:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_USB;
		break;
	case IC9100_MODE_CW:
	case IC9100_MODE_CWR:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_CW;
		break;
	case IC9100_MODE_RTTY:
	case IC9100_MODE_RTTYR:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_RTTY;
		break;
	case IC9100_MODE_DV:
		handle.channel[handle.channelnum].mode = DEVICE_RADIO_MODE_DV;
		break;
	}

	return iretn;
}

int32_t ic9100_get_rfgain(ic9100_handle &handle)
{
	int32_t iretn = 0;

	if (iretn < 0)
	{
		return iretn;
	}

	iretn = ic9100_write(handle, 0x14, 0x2);
	if (iretn != 2)
	{
		return IC9100_ERROR_OUTOFRANGE;
	}

	handle.channel[handle.channelnum].rfgain = 0;
	for (size_t i=0; i<2; ++i)
	{
		handle.channel[handle.channelnum].rfgain += 10. * (handle.response[i] >> 4) + (handle.response[i] % 16);
		handle.channel[handle.channelnum].rfgain *= 100.;
	}

	return iretn;
}

int32_t ic9100_get_squelch(ic9100_handle &handle)
{
	int32_t iretn = 0;

	if (iretn < 0)
	{
		return iretn;
	}

	iretn = ic9100_write(handle, 0x14, 0x3);
	if (iretn != 2)
	{
		return IC9100_ERROR_OUTOFRANGE;
	}
	handle.channel[handle.channelnum].squelch = 0;
	for (size_t i=0; i<2; ++i)
	{
		handle.channel[handle.channelnum].squelch += 10. * (handle.response[i] >> 4) + (handle.response[i] % 16);
		handle.channel[handle.channelnum].squelch *= 100.;
	}
	return 0;
}

int32_t ic9100_get_rfpower(ic9100_handle &handle)
{
	int32_t iretn = 0;
	float power;

	if (iretn < 0)
	{
		return iretn;
	}

	iretn = ic9100_write(handle, 0x14, 0xa);
	if (iretn != 2)
	{
		return IC9100_ERROR_OUTOFRANGE;
	}
	power = 0;
	for (size_t i=0; i<2; ++i)
	{
		power *= 100.;
		power += 10. * (handle.response[i] >> 4) + (handle.response[i] % 16);
	}

	if (handle.channel[handle.channelnum].freqband < 11)
	{
		power = 2. + power * (handle.channel[handle.channelnum].mode==DEVICE_RADIO_MODE_AM?28.:98.);
	}
	else if (handle.channel[handle.channelnum].freqband < 12)
	{
		power = 2. + power * 98.;
	}
	else if (handle.channel[handle.channelnum].freqband < 13)
	{
		power = 2. + power * 73.;
	}
	else if (handle.channel[handle.channelnum].freqband < 14)
	{
		power = 2. + power * 8.;
	}
	handle.channel[handle.channelnum].rfpower = power;
	return iretn;
}
