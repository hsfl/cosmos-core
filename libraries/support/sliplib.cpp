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

/*!	\file sliplib.cpp
	\brief Support functions for COSMOS SLIP encoding
*/

#include "support/sliplib.h"
#include <stdio.h>

//! \addtogroup sliplib_functions
//! @{

//! Check SLIP packet CRC
/*! Get the CRC from the end of a SLIP packet and check it against the
 * calculated CRC.
	\param sbuf Pointer to buffer containing SLIP encoded data plus CRC.
	\param ssize Total size of buffer, including CRC.
	\return 0 if CRC checks out, otherwise SLIP_ERROR_CRC.
*/
int32_t slip_check_crc(uint8_t *sbuf, uint16_t ssize)
	{
	uint16_t crc, crc2;

	crc = slip_get_crc(sbuf,ssize-2);
	crc2 = slip_calc_crc(sbuf,ssize-2);
	if (crc != crc2)
		return (SLIP_ERROR_CRC);
	return 0;
	}

//! Unpack SLIP packet
/*! Convert SLIP data in one buffer to raw ASCII data in second buffer and return
 * length.
	\param sbuf character buffer to read SLIP data from
	\param ssize size of SLIP buffer
	\param rbuf character buffer to store raw data
	\param rsize size of raw buffer, including terminal 0
	\return bytes in raw buffer
*/

int32_t slip_unpack(uint8_t *sbuf, uint16_t ssize, uint8_t *rbuf, uint16_t rsize)
{
	if (slip_check_crc(sbuf, ssize)) return (SLIP_ERROR_CRC);

	return (slip_decode(sbuf, ssize-2, rbuf, rsize));
}

//! Decode SLIP packet
/*! Convert SLIP data in one buffer to raw ASCII data in second buffer and return
 * length.
	\param sbuf character buffer to read SLIP data from
	\param ssize size of SLIP buffer
	\param rbuf character buffer to store raw data
	\param rsize size of raw buffer, including terminal 0
	\return bytes in raw buffer
*/

int32_t slip_decode(uint8_t *sbuf, uint16_t ssize, uint8_t *rbuf, uint16_t rsize)
{
	uint16_t i, j, ch;

	i = j = 0;
	do
	{
		if (j > ssize-3)
			return (SLIP_ERROR_PACKING);
		ch = sbuf[j++];
		if (i < rsize)
		{
			switch (ch)
			{
			case SLIP_FESC:
				if (ssize > j+3)
					return (SLIP_ERROR_PACKING);
				ch = sbuf[j++];
				switch (ch)
				{
				case SLIP_TFEND:
					rbuf[i] = SLIP_FEND;
					break;
				case SLIP_TFESC:
					rbuf[i] = SLIP_FESC;
					break;
				}
				++i;
				break;
			case SLIP_FEND:
				break;
			default:
				rbuf[i] = ch;
				++i;
				break;
			}
		}
		else
			return (SLIP_ERROR_BUFFER);
	} while (ch != SLIP_FEND);

	return (i);
}

//! Encode data in to SLIP packet
/*! Convert raw ASCII in one buffer to SLIP encoded data in second
 * buffer.
	\param rbuf Buffer with raw data
	\param rsize Number of bytes in raw buffer
	\param sbuf Buffer to place SLIP data in
	\param ssize Maximum size of SLIP buffer
	\return bytes actually written
*/

int32_t slip_encode(uint8_t *rbuf, uint16_t rsize, uint8_t *sbuf, uint16_t ssize)
{
	uint16_t i, j;

	i = j = 0;
	if (ssize < 3)
		return (SLIP_ERROR_BUFFER);
	sbuf[i++] = SLIP_FEND;

	for (j=0; j<rsize; ++j)
	{
		switch (rbuf[j])
		{
		case SLIP_FEND:
			if (i > ssize-4)
				return (SLIP_ERROR_BUFFER);
			sbuf[i++] = SLIP_FESC;
			sbuf[i++] = SLIP_TFEND;
			break;
		case SLIP_FESC:
			if (i > ssize-4)
				return (SLIP_ERROR_BUFFER);
			sbuf[i++] = SLIP_FESC;
			sbuf[i++] = SLIP_TFESC;
			break;
		default:
			if (i > ssize-3)
				return (SLIP_ERROR_BUFFER);
			sbuf[i++] = rbuf[j];
			break;
		}
	}
	if (ssize < i+1)
		return (SLIP_ERROR_BUFFER);
	sbuf[i++] = SLIP_FEND;
	return (i);
}

//! Pack data in to SLIP packet
/*! Convert raw ASCII in one buffer to SLIP encoded data in second
 * buffer, appending a 16 bit CCITT CRC.
	\param rbuf Buffer with raw data
	\param rsize Number of bytes in raw buffer
	\param sbuf Buffer to place SLIP data in
	\param ssize Maximum size of SLIP buffer
	\return bytes actually written
*/

int32_t slip_pack(uint8_t *rbuf, uint16_t rsize, uint8_t *sbuf, uint16_t ssize)
{
	int32_t i;

	i = slip_encode(rbuf, rsize, sbuf, ssize);
	if (i < 0) return (SLIP_ERROR_BUFFER);
	if (ssize < i+3) return (SLIP_ERROR_BUFFER);
	slip_set_crc(sbuf,i);
	i+=2;
	return (i);
}

//! Calculate CRC-16-CCITT
/*! Calculate 16-bit CCITT CRC for the indicated buffer and number of bytes.
 * The initial shift register value is 0xffff, and the calculation
 * starts with the LSB, so the Polynomial is 0x8408.
	\param buf bytes to calculate on
	\param size number of bytes
	\return calculated CRC
*/
uint16_t slip_calc_crc(uint8_t *buf, uint16_t size)
{
	uint16_t crc = 0xffff;
	uint8_t ch;
	int i, j;

	for (i=0; i<size; ++i)
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

//! Get CRC from SLIP buffer
/*! Extract the CRC from the specified point in a SLIP buffer and return it as an
 * unsigned integer.
	\param buf SLIP buffer
	\param index Location in buffer of start of CRC
	\return CRC
*/
uint16_t slip_get_crc(uint8_t *buf, uint16_t index)
{
	uint16_t crc;

	memcpy(&crc,&buf[index],2);

	return (crc);
}

//! Set CRC for SLIP buffer
/*! Calculate the CRC for the specified length SLIP buffer and append it to the buffer.
	\param buf SLIP buffer
	\param index Location in buffer of start of CRC
	\return Calculated CRC
*/
uint16_t slip_set_crc(uint8_t *buf, uint16_t index)
{
	uint16_t crc;

	crc = slip_calc_crc(buf,index);
	memcpy(&buf[index],&crc,2);

	return (crc);
}
//! @}
