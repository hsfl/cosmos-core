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

/*! \file sinclair_lib.c
	\brief Support routines for Sinclair reaction wheel
*/

#include "configCosmos.h"
#if defined(COSMOS_WIN_OS) || defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

// #include <unistd.h>
#include "sinclair_lib.h"
#include "timelib.h"

//!	\ingroup sinclair
//! \defgroup sinclair_statics Sinclair reaction wheel statics
//! @{

//! Reaction wheel state indicator
vector<sinclair_state> nsi_state;

//! @}

//! \addtogroup sinclair_functions
//! @{

//! Connect to reaction wheel
/*! Establishes connection to sinclair reaction wheel on specific serial port.
	\param dev device name of serial port
	\param src Client source address (can be anything)
	\param dst Reaction wheel destination address (0x3e)
	\param handle Pointer to ::sinclair_state.
	\return Zero, otherwise negative error.
	\see cssl_start
	\see cssl_open
*/
int32_t sinclair_rw_connect(const char *dev, uint8_t src, uint8_t dst, sinclair_state *handle)
{
	int32_t iretn;

	cssl_start();
	handle->serial = cssl_open(dev,SINCLAIRRW_BAUD,SINCLAIR_BITS,SINCLAIR_PARITY,SINCLAIR_STOPBITS);

	if (handle->serial != NULL)
	{
		cssl_settimeout(handle->serial, 0, .1);
		handle->dev.assign(dev);
		handle->mbuf.src = src;
		handle->mbuf.dst = dst;
		if ((iretn = nsp_init(handle,0x00001000)) >=0)
		{
			return 0;
		}
		else
		{
			return (iretn);
		}
	}

	return (SINCLAIR_ERROR_FAILED);
}

//! Disconnect reaction wheel
/*! Close specified reaction wheels serial port and recover resources.
	\param handle integer handle returned from previous sinclair_rw_connect
	\return 0, otherwise error
	\see cssl_close
	\see sinclair_rw_connect
*/
int32_t sinclair_disconnect(sinclair_state *handle)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	cssl_close(handle->serial);
	return 0;
}

//! Read SLIP packet
/*! Read SLIP data from requested device. Convert and place in to buffer.
	\param handle handle of requested device
	\param buf character buffer to store returned data
	\param size size of character buffer, including terminal 0
	\return bytes actually read
	\see cssl_getdata
*/

int32_t sinclair_readslip(sinclair_state *handle, uint8_t *buf, int size)
{
	uint16_t i;
	int16_t ch;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	do
	{
		ch = cssl_getchar(handle->serial);
		if (ch < 0) return (ch);
	} while (ch != SLIP_FEND);

	i = 0;
	do
	{
		ch = cssl_getchar(handle->serial);
		if (ch < 0) return (ch);
		if (i < size)
		{
			switch (ch)
			{
			case SLIP_FESC:
				ch = cssl_getchar(handle->serial);
				if (ch < 0) return (ch);
				switch (ch)
				{
				case SLIP_TFEND:
					buf[i] = SLIP_FEND;
					break;
				case SLIP_TFESC:
					buf[i] = SLIP_FESC;
					break;
				}
				i++;
				break;
			case SLIP_FEND:
				break;
			default:
				buf[i] = ch;
				i++;
				break;
			}
		}
	} while (ch != SLIP_FEND);

	return (i);
}

//! Write SLIP packet
/*! Convert and write SLIP data to requested device.
	\param handle handle of requested device
	\param buf buffer of characters to write
	\param size size of character buffer, including terminal 0
	\return bytes actually written
	\see cssl_getdata
*/

int32_t sinclair_writeslip(sinclair_state *handle, uint8_t *buf, int size)
{
	int i, j;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	i = 0;
	cssl_drain(handle->serial);
	cssl_putchar(handle->serial,SLIP_FEND);
	for (j=0; j<size; j++)
	{
		switch (buf[j])
		{
		case SLIP_FEND:
			cssl_putchar(handle->serial,SLIP_FESC);
			cssl_putchar(handle->serial,SLIP_TFEND);
			i+=2;
			break;
		case SLIP_FESC:
			cssl_putchar(handle->serial,SLIP_FESC);
			cssl_putchar(handle->serial,SLIP_TFESC);
			i+=2;
			break;
		default:
			cssl_putchar(handle->serial,buf[j]);
			i++;
			break;
		}
	}
	cssl_putchar(handle->serial,SLIP_FEND);
	return (i);
}

//! NSP command and reply
/*! Send an NSP command with the requested data and flags and retrieve any reply.
	\param handle Handle of requested wheel
	\return bytes of data returned, otherwise negative error number
	\see sinclair_writeslip
	\see sinclair_readslip
*/

int nsp_send_message(sinclair_state *handle)
{
	int i, iretn;
	uint16_t crc, crc2;
	uint8_t xbuf[NSP_MAX_MESSAGE];

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	xbuf[0] = handle->mbuf.dst;
	xbuf[1] = handle->mbuf.src;
	xbuf[2] = handle->mbuf.mcf;
	for (i=0; i<handle->mbuf.size; i++)
	{
		xbuf[i+3] = handle->mbuf.data[i];
	}

	handle->mbuf.crc = slip_calc_crc(xbuf,handle->mbuf.size+3);
	uint16to((uint16_t)handle->mbuf.crc,(uint8_t *)&xbuf[handle->mbuf.size+3],(uint8_t)ORDER_LITTLEENDIAN);
	iretn = sinclair_writeslip(handle,xbuf,handle->mbuf.size+5);

	if (iretn < 0)
		return (iretn);

	handle->mbuf.size = 0;
	if (handle->mbuf.mcf & NSP_MCB_PF) // Did we expect an answer?
	{
		do
		{
			iretn = sinclair_readslip(handle,xbuf,NSP_MAX_MESSAGE);
			if (iretn >= 5) // Did we get at least the minimum number of return bytes?
			{
				crc = uint16from(&xbuf[iretn-2],ORDER_LITTLEENDIAN);
				crc2 = slip_calc_crc(xbuf,iretn-2);
				if (crc == crc2) // CRCs match?
				{
					handle->mbuf.src = (uint8_t)xbuf[0];
					handle->mbuf.dst = (uint8_t)xbuf[1];
					handle->mbuf.mcf = (uint8_t)xbuf[2];

					if ((handle->mbuf.mcf&0x1f) == NSP_COMMAND_COMBINATION || (handle->mbuf.mcf&0x1f) == NSP_COMMAND_READ_RESULT)
					{
						uint16_t place;
						place = uint16from(&xbuf[3],ORDER_LITTLEENDIAN);
                        //handle->mbuf.size = 0;
						memcpy((void *)&handle->mbuf.data[place], (void *)&xbuf[5], iretn-7);
						handle->mbuf.size = place + iretn-7;
					}
					else
					{
						memcpy((void *)&handle->mbuf.data[handle->mbuf.size], (void *)&xbuf[3], iretn-5);
						handle->mbuf.size += iretn-5;
					}

					handle->mbuf.crc = crc;

					if (!(handle->mbuf.mcf & NSP_MCB_A))
					{
						// NACK
						return (NSP_ERROR_NACK);
					}
				}
				else
				{
					return (NSP_ERROR_CRC);
				}
			}
			else
			{
				if (iretn < 0)
				{
					return (iretn);
				}
				else
				{
					return (NSP_ERROR_SIZE);
				}
			}
		} while (!(handle->mbuf.mcf & NSP_MCB_PF));
	}

	handle->lmjd = currentmjd(0.);

	return (handle->mbuf.size);
}

//! Calculate NSP CRC
/*! Calculate 16-bit CCITT CRC for the indicated buffer and number of bytes, with an
 * initial shift register value of 0xffff.
	\param buf bytes to calculate on
	\param size number of bytes
	\return calculated CRC
*/

uint16_t nsp_calc_crc(uint8_t *buf, int size)
{
	uint16_t crc = 0xffff;
	uint8_t ch;
	int i, j;

	for (i=0; i<size; i++)
	{
		ch = buf[i];
		for (j=0; j<8; j++)
		{
			crc = (crc >> 1)^(((ch^crc)&0x01)?0x8408:0);
			ch >>= 1;
		}
	}
	return (crc);
}

//! Set NSP Destination Address
/*! Set the Destination Address value for the requested reaction wheel.
	\param handle The handle for the requested device
	\param address The address to set
	\return 0 if successful, otherwise negative error value
*/

int nsp_set_dst(sinclair_state *handle, uint8_t address)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.dst = address;
	return 0;
}

//! Set NSP Source Address
/*! Set the Source Address value for the requested reaction wheel.
	\param handle The handle for the requested device
	\param address The address to set
	\return 0 if successful, otherwise negative error value
*/

int nsp_set_src(sinclair_state *handle, uint8_t address)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.src = address;
	return 0;
}

//! Set NSP Message Control Field
/*! Set the Message Control Field value for the requested reaction wheel.
	\param handle The handle for the requested device
	\param mcf Message Control Field. An Or'd combination of ::NSP_MCB_A, ::NSP_MCB_B
	plus one of ::NSP_COMMAND_PING, ::NSP_COMMAND_INIT, ::NSP_COMMAND_PEEK,
	::NSP_COMMAND_POKE, ::NSP_COMMAND_TELEMETRY, ::NSP_COMMAND_CRC,
	::NSP_COMMAND_APP_TELEMETRY, or ::NSP_COMMAND_APP_COMMAND
	and ::NSP_MCB_PF
	\return 0 if successful, otherwise negative error value
*/

int nsp_set_mcf(sinclair_state *handle, uint8_t mcf)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = mcf;
	return 0;
}

//! Set NSP CRC
/*! Set the  CRC value for the requested reaction wheel.
	\param handle The handle for the requested device
	\param crc CRC-16-CCITT, calculated with polynomial x^16+x^12+x^5+1 reversed to
	yield a divisor of 0x8408
	\return 0 if successful, otherwise negative error value
*/

int nsp_set_crc(sinclair_state *handle, uint16_t crc)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.crc = crc;
	return 0;
}

//! Set NSP size
/*! Set the  size value for the requested device.
	\param handle The handle for the requested device
	\param size The address to set
	\return 0 if successful, otherwise negative error value
*/

int nsp_set_size(sinclair_state *handle, uint16_t size)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.size = size;
	return 0;
}

//! Get NSP Destination Address
/*! Get the  Destination Address value for the requested reaction wheel.
	\param handle The handle for the requested device
	\return 0 if successful, otherwise negative error value
*/

int nsp_get_dst(sinclair_state *handle)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	return (handle->mbuf.dst);
}

//! Get NSP Source Address
/*! Get the  Source Address value for the requested reaction wheel.
	\param handle The handle for the requested device
	\return 0 if successful, otherwise negative error value
*/

int nsp_get_src(sinclair_state *handle)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	return (handle->mbuf.src);
}

//! Get NSP MCF
/*! Get the Message Control Field value for the requested reaction wheel.
	\param handle The handle for the requested device
	\return 0 if successful, otherwise negative error value
*/

int nsp_get_mcf(sinclair_state *handle)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	return (handle->mbuf.mcf);
}

//! Get NSP CRC
/*! Get the  CRC value for the requested reaction wheel.
	\param handle The handle for the requested device
	\return 0 if successful, otherwise negative error value
*/

int nsp_get_crc(sinclair_state *handle)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	return (handle->mbuf.crc);
}

//! Get NSP size
/*! Get the data size for the requested device.
	\param handle The handle for the requested device
	\return 0 if successful, otherwise negative error value
*/

int nsp_get_size(sinclair_state *handle)
{
	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	return (handle->mbuf.size);
}

//! GET NSP data field
/*! Get the pointer to the data field for the requested device.
	\param handle The handle for the requested device
	\return pointer to the beginning of the data are, otherwise NULL
*/
uint8_t *nsp_get_data(sinclair_state *handle)
{
	if (handle == NULL)
		return ((uint8_t *)NULL);
	return (handle->mbuf.data);
}

//! NSP PING command
/*! Send an NSP Module ping command and retrieve any returned data.
	\param handle Handle of requested wheel
	\param buf buffer to place return string
	\param size size of buffer
	\return Number of bytes in returned message, otherwise negative error.
*/

int nsp_ping(sinclair_state *handle)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = NSP_COMMAND_PING|NSP_MCB_PF;
	handle->mbuf.size = 0;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

//	memcpy((void *)buf,(void *)handle->mbuf.data,size);
	if (nsp_get_size(handle) < NSP_MAX_MESSAGE+1)
		handle->mbuf.data[nsp_get_size(handle)] = 0;
	return (nsp_get_size(handle));
}

//! NSP INIT command
/*! Send an NSP Module init command for the requested address.
	\param handle Handle of requested wheel
	\param address 32-bit address to init from
	\return 0 if successful
*/

int nsp_init(sinclair_state *handle, uint32_t address)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = NSP_COMMAND_INIT|NSP_MCB_PF;
	uint32to(address,handle->mbuf.data,ORDER_LITTLEENDIAN);
	handle->mbuf.size = 4;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

	COSMOS_USLEEP(10000);
	return 0;
}

//! NSP Application Command Mode command
/*! Send an NSP Module Application Command Mode command to the requested wheel.
	\param handle Handle of requested wheel
	\param mode NSP mode type
	\param value NSP mode value
	\return 0 if successful
*/

int nsp_app_command_mode(sinclair_state *handle, uint8_t mode, float value)
{
	//	uint8_t *data;
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = NSP_COMMAND_APP_COMMAND|NSP_MCB_PF;

	//	data = handle->mbuf.data;
	handle->mbuf.data[0] = 0;
	handle->mbuf.data[1] = mode;
	floatto(value,&handle->mbuf.data[2],ORDER_LITTLEENDIAN);

	handle->mbuf.size = 6;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

	return 0;
}

//! NSP Application Command Parameter command
/*! Send an NSP Module Application Command Parameter command to the requested wheel.
	\param handle Handle of requested wheel
	\param param NSP parameter number
	\param value NSP parameter value
	\return 0 if successful
*/

int nsp_app_command_param(sinclair_state *handle, uint8_t param, float value)
{
	//	uint8_t *data;
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = NSP_COMMAND_APP_COMMAND|NSP_MCB_PF;
	//	data = handle->mbuf.data;
	handle->mbuf.data[0] = param;
	floatto(value,&handle->mbuf.data[1],ORDER_LITTLEENDIAN);
	handle->mbuf.size = 5;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

	return 0;
}

//! NSP Application Telemetry Mode command
/*! Retrieve the current mode type and value from the requested wheel.
	\param handle Handle of requested wheel
	\param mode pointer to returned moded type
	\param value pointer to returned mode value
	\return 0 if successful
*/

int nsp_app_telemetry_mode(sinclair_state *handle, uint8_t *mode, float *value)
{
	//	uint8_t *data;
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = NSP_COMMAND_APP_TELEMETRY|NSP_MCB_PF;
	//	data = handle->mbuf.data;
	handle->mbuf.data[0] = 0;
	handle->mbuf.size = 1;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

	*mode = handle->mbuf.data[1];
	memcpy((void *)value,(void *)&handle->mbuf.data[2],4);
	return 0;
}

//! NSP Application Telemetry Parameter command
/*! Retrieve an NSP parameter from  the requested wheel.
	\param handle Handle of requested device
	\param param pointer to the requested parameter
	\param value pointer to the returned value
	\return 0 if successful
*/

int nsp_app_telemetry_param(sinclair_state *handle, uint8_t param, float *value)
{
	//	uint8_t *data;
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = NSP_COMMAND_APP_TELEMETRY|NSP_MCB_PF;
	//	data = handle->mbuf.data;
	handle->mbuf.data[0] = param;
	handle->mbuf.size = 1;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);
	*value = floatfrom(&handle->mbuf.data[1],ORDER_LITTLEENDIAN);

	return 0;
}

//! Sinclair Idle Mode
/*! Set Sinclair Reaction Wheel Mode to Idle.
	\param handle Handle for requested reaction wheel
	\return 0, otherwise negative error
	\see nsp_app_command_mode
*/

int32_t sinclair_mode_idle(sinclair_state *handle)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	iretn = nsp_app_command_mode(handle,SINCLAIR_MODE_IDLE,0.);
	return (iretn);
}

//! Sinclair DAC Mode
/*! Set Sinclair Reaction Wheel Mode to DAC.
	\param handle Handle for requested reaction wheel
	\param value Value to set DAC to, between -65535. and +65535.
	\return 0, otherwise negative error
	\see nsp_app_command_mode
*/

int32_t sinclair_mode_dac(sinclair_state *handle, float value)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	if (value < -65535.)
		value = -65535.;

	if (value > 65535.)
		value = 65535.;

	iretn = nsp_app_command_mode(handle,SINCLAIR_MODE_DAC,value);
	return (iretn);
}

//! Sinclair Current Mode
/*! Set Sinclair Reaction Wheel Mode to Current.
	\param handle Handle for requested reaction wheel
	\param amps Current value to attempt to track.
	\return 0, otherwise negative error
	\see nsp_app_command_mode
*/

int32_t sinclair_mode_current(sinclair_state *handle, float amps)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	iretn = nsp_app_command_mode(handle,SINCLAIR_MODE_CURRENT,amps);
	return (iretn);
}

//! Sinclair Power Mode
/*! Set Sinclair Reaction Wheel Mode to Power.
	\param handle Handle for requested reaction wheel
	\param watts Power value to attempt to track.
	\return 0, otherwise negative error
	\see nsp_app_command_mode
*/

int32_t sinclair_mode_power(sinclair_state *handle, float watts)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	iretn = nsp_app_command_mode(handle,SINCLAIR_MODE_POWER,watts);
	return (iretn);
}

//! Sinclair Speed Mode
/*! Set Sinclair Reaction Wheel Mode to Angular Speed.
	\param handle Handle for requested reaction wheel
	\param speed Angular velocity in radians/second to attempt to track.
	\return 0, otherwise negative error
	\see nsp_app_command_mode
*/

int32_t sinclair_mode_speed(sinclair_state *handle, float speed)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	iretn = nsp_app_command_mode(handle,SINCLAIR_MODE_SPEED,speed);
	return (iretn);
}

//! Sinclair Accel Mode
/*! Set Sinclair Reaction Wheel Mode to Angular Acceleration.
	\param handle Handle for requested reaction wheel
	\param accel Angular acceleration in radians/second/second to attempt to track.
	\return 0, otherwise negative error
	\see nsp_app_command_mode
*/

int32_t sinclair_mode_accel(sinclair_state *handle, float accel)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	iretn = nsp_app_command_mode(handle,SINCLAIR_MODE_ACCEL,accel);
	return (iretn);
}

//! Sinclair Momentum Mode
/*! Set Sinclair Reaction Wheel Mode to Angular Momentum.
	\param handle Handle for requested reaction wheel
	\param momentum Angular momentum in newton-meter-second to attempt to track.
	\return 0, otherwise negative error
	\see nsp_app_command_mode
*/

int32_t sinclair_mode_momentum(sinclair_state *handle, float momentum)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	iretn = nsp_app_command_mode(handle,SINCLAIR_MODE_MOMENTUM,momentum);
	return (iretn);
}

//! Sinclair Torque Mode
/*! Set Sinclair Reaction Wheel Mode to Angular Torque.
	\param handle Handle for requested reaction wheel
	\param torque Torque in newton-meter to attempt to track.
	\return 0, otherwise negative error
	\see nsp_app_command_mode
*/

int32_t sinclair_mode_torque(sinclair_state *handle, float torque)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	iretn = nsp_app_command_mode(handle,SINCLAIR_MODE_TORQUE,torque);
	return (iretn);
}

//! Get Sinclair Voltage
/*! Get the current voltage of a Sinclair Reaction Wheel.
	 \param handle Handle for requested reaction wheel
	 \return voltage Voltage of requested wheel
	 \see nsp_app_telemetry_param
*/

float sinclair_get_voltage(sinclair_state *handle)
{
	float voltage=0.;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);


	nsp_app_telemetry_param(handle,SINCLAIR_PARAM_VOLTAGE,&voltage);
	return (voltage);
}

//! Get Sinclair Speed
/*! Get the current angular velocity of a Sinclair Reaction Wheel.
	 \param handle Handle for requested reaction wheel
	 \return Angular velocity of requested wheel in radians/sec
	 \see nsp_app_telemetry_param
*/

float sinclair_get_speed(sinclair_state *handle)
{
	int32_t iretn;
	float speed;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);


	iretn = nsp_app_telemetry_param(handle,SINCLAIR_PARAM_SPEED,&speed);
	if (iretn < 0)
		return (NAN);
	else
		return (speed);
}

//! Get Sinclair Acceleration
/*! Get the current angular acceleration of a Sinclair Reaction Wheel.
	 \param handle Handle for requested reaction wheel
	 \return Angular acceleration of requested wheel in radians/sec/sec
	 \see nsp_app_telemetry_param
*/

float sinclair_get_accel(sinclair_state *handle)
{
	int32_t iretn;
	float accel;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);


	iretn = nsp_app_telemetry_param(handle,SINCLAIR_PARAM_ACCEL,&accel);
	if (iretn < 0)
		return (NAN);
	else
		return (accel);
}

//! Get Sinclair Inertial Moment
/*! Get the angular moment of a Sinclair Reaction Wheel.
	 \param handle Handle for requested reaction wheel
	 \return Angular moment of requested wheel in kg-m2
	 \see nsp_app_telemetry_param
*/

float sinclair_get_inertia(sinclair_state *handle)
{
	int32_t iretn;
	float inertia;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);


	iretn = nsp_app_telemetry_param(handle,SINCLAIR_PARAM_INERTIA,&inertia);
	if (iretn < 0)
		return (NAN);
	else
		return (inertia);
}

//! Star Tracker INIT command
/*! Send an NSP Module Stat Tracker init command for the requested address.
	\param handle Handle of requested wheel
	\param address 32-bit address to init from
	\return 0 if successful
*/

int32_t sinclair_stt_connect(const char *dev, uint8_t src, uint8_t dst, sinclair_state *handle)
{
	int32_t iretn;

	cssl_start();
	handle->serial = cssl_open(dev,SINCLAIRSTT_BAUD,SINCLAIR_BITS,SINCLAIR_PARITY,SINCLAIR_STOPBITS);

	if (handle->serial != NULL)
	{
		cssl_settimeout(handle->serial, 0, 1.);
		handle->dev.assign(dev);
		handle->mbuf.src = src;
		handle->mbuf.dst = dst;
/*
		if ((iretn=nsp_ping(handle)) >= 0)
		{
			return 0;
		}
*/
		if ((iretn=nsp_init(handle,0x00002000)) >=0)
		{
			if ((iretn=nsp_ping(handle)) >= 0)
			{
				return 0;
			}
			else
			{
				return (iretn);
			}
		}
		else
		{
			if ((iretn=nsp_ping(handle)) >= 0)
			{
				return 0;
			}
			else
			{
				return (iretn);
			}
		}
	}

	return (SINCLAIR_ERROR_FAILED);
}

//! Star Tracker GO command
/*! Send an NSP Start Tracker "GO" command to start a start tracker sequence.
	\param handle Handle of Star Tracker
	\param buf buffer to place return string
	\param size size of data buffer
	\return 0 if successful
*/
int nsp_stt_go(sinclair_state *handle, char *buf_out, uint8_t command)
{
	//	uint8_t *command_data;
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = NSP_COMMAND_GO|NSP_MCB_PF;
	//	command_data = handle->mbuf.data;
	handle->mbuf.data[0] = command;
	handle->mbuf.size = 1;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

	memcpy((void *)buf_out,(void *)handle->mbuf.data,1);
	if (nsp_get_size(handle) <= 1)
		buf_out[nsp_get_size(handle)] = 0;

	return 0;
}

//! Sinclair Star Tracker Read EDAC Asynchronous
/*! Read the Asynchronous data portion of the Sinclair Star Tracker indicated.
 * \param handle Pointer to Star Tracker State handle.
 * \param data Pointer to ::sinclair_stt_analog_frame containing asynchronous result.
 * \return 0 if successful, otherwise negative error.
 */
int32_t nsp_stt_read_edac_asynchronous(sinclair_state *handle, sinclair_stt_analog_frame *data)
{
	//	uint8_t *command_data;
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	// Set command to be sent
	handle->mbuf.mcf = NSP_COMMAND_READ_EDAC|NSP_MCB_PF;
	//	command_data = handle->mbuf.data;

	// Starting at 0x04 (Asynchronous current sense telemetry)
	uint16to(0x04, handle->mbuf.data, ORDER_LITTLEENDIAN);
	// Going for 32 bytes (Size of an analog frame)
	handle->mbuf.data[2] = sizeof(sinclair_stt_analog_frame);

	// Set number of bytes in command
	handle->mbuf.size = 3;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

	//	command_data = handle->mbuf.data;
	sinclair_stt_analog_frame *frame = (sinclair_stt_analog_frame *)&handle->mbuf.data[2];
	*data = *frame;

	return 0;
}


//! NSP Star Tracker READ EDAC command
/*! Read NSP Star Tracker EDAC memory.
	\param handle Handle of Star Tracker
	\param buf buffer to place return string
	\param size size of data buffer
	\return 0 if successful
*/
int nsp_stt_read_edac_result_length(sinclair_state *handle, char *buf_out)
{
	//	uint8_t *command_data;
	uint16_t result_length_address = 0x4C;
	int32_t iretn;
	int size=6;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = NSP_COMMAND_READ_EDAC|NSP_MCB_PF;
	//	command_data = handle->mbuf.data;
	uint16to(result_length_address, handle->mbuf.data, ORDER_LITTLEENDIAN);
	handle->mbuf.data[2] = 4;    // Number of bytes to get
	handle->mbuf.size = 3;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

	memcpy((void *)buf_out,(void *)handle->mbuf.data,size);
	if (nsp_get_size(handle) <= size)
		buf_out[nsp_get_size(handle)] = 0;
	return 0;
}

//! NSP Star Tracker READ RESULT command
/*! Read NSP Start Tracker Result structure.
	\param handle Handle of Star Tracker
	\param buf buffer to place return string
	\param size size of data buffer
	\return 0 if successful
*/
int nsp_stt_read_result(sinclair_state *handle, char *buf_out, uint16_t addr_2_read, uint16_t num_2_read)
{
	//	uint8_t *command_in;
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

	handle->mbuf.mcf = NSP_COMMAND_READ_RESULT|NSP_MCB_PF;
	//	command_in = handle->mbuf.data;
	uint16to(addr_2_read, &handle->mbuf.data[0], ORDER_LITTLEENDIAN);
	uint16to(num_2_read, &handle->mbuf.data[2], ORDER_LITTLEENDIAN);
	handle->mbuf.size = 4;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

	memcpy((void *)buf_out,(void *)(handle->mbuf.data+2),num_2_read);
	if (nsp_get_size(handle) <= num_2_read+2)
		buf_out[nsp_get_size(handle)] = 0;
	return (nsp_get_size(handle)-2);
}

//! NSP Star Tracker COMBINATION command
/*! Send NSP Star Tracker COMBINATION command.
	\param handle Handle of Star Tracker
	\param buf buffer to place return string
	\param size size of data buffer
	\return 0 if successful
*/
int nsp_stt_combination(sinclair_state *handle, sinclair_stt_result_operational *result, uint8_t go_command, uint32_t bitfield)
{
	int32_t iretn;

	if (handle == NULL)
		return (SINCLAIR_ERROR_CLOSED);

    // Prime prep combo command
	handle->mbuf.mcf = NSP_COMMAND_COMBINATION|NSP_MCB_PF;

	// Put command into command buffer
	handle->mbuf.data[0] = go_command;

	// Put combination bitfield into command buffer
	uint32to(bitfield, &handle->mbuf.data[1], ORDER_LITTLEENDIAN);
	handle->mbuf.size = 4;

	if ((iretn=nsp_send_message(handle)) < 0)
		return (iretn);

	*result = *(sinclair_stt_result_operational *)handle->mbuf.data;
//	memcpy((void *)buf_out,(void *)(handle->mbuf.data),handle->mbuf.size);
	return (handle->mbuf.size);
}



int32_t sinclair_stt_combo(sinclair_state *handle, sinclair_stt_result_operational *result)
{
//	int i;
//	char buf[4096];
//	uint8_t command[4];

	// Send COMBINATION command 0x00001E0B
//	command[0] = 0x0B;
//	command[1] = 0x7E;
//	command[2] = 0x00;
//	command[3] = 0x00;

	// Get the result of the Combination Command
    if(handle->serial->fd >= 0)
	{     // If there is a valid star tracker handle
        nsp_stt_combination(handle, result
                            // old, segmentation fault problem
                            //,STT_GO_ON|STT_GO_LOAD_NAND|STT_GO_NOT_OFF|STT_GO_COMMAND
                            //,STT_COMBO_RET_COD|STT_COMBO_ATT_QUA|STT_COMBO_ANG_VEL|STT_COMBO_EPO_TIM|STT_COMBO_HAR_TEL|STT_COMBO_STA_TEL
                            // new, fixed segmentation fault by removing "STT_GO_NOT_OFF"
                            // and didnt' hurt to add STT_COMBO_SEQ_NUM
                            ,STT_GO_ON|STT_GO_LOAD_NAND|STT_GO_COMMAND
                            ,STT_COMBO_SEQ_NUM|STT_COMBO_RET_COD|STT_COMBO_ATT_QUA|STT_COMBO_ANG_VEL|STT_COMBO_EPO_TIM|STT_COMBO_HAR_TEL|STT_COMBO_STA_TEL

                            );

//		*result = *(sinclair_stt_result_operational *)handle->mbuf.data;
	}
	else
	{
		return (CSSL_ERROR_OPEN);
	}

	return 0;
}

//! @}

#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)
