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
	return (slip_decode(sbuf, ssize, rbuf, rsize));
}

int32_t slip_unpack(vector<uint8_t> &sbuf, vector<uint8_t> &rbuf)
{
    return (slip_decode(sbuf, rbuf));
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
                if (j > ssize-3)
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

int32_t slip_decode(vector<uint8_t> &sbuf, vector<uint8_t> &rbuf)
{
    size_t j, ch;
    rbuf.clear();

    j = 0;
    do
    {
        if (j > sbuf.size()-1)
        {
            return (SLIP_ERROR_PACKING);
        }
        ch = sbuf[j++];
    } while (ch != SLIP_FEND);

    do
    {
        if (j > sbuf.size()-1)
        {
            return (SLIP_ERROR_PACKING);
        }
        ch = sbuf[j++];
        switch (ch)
        {
        case SLIP_FESC:
            if (j > sbuf.size()-1)
                return (SLIP_ERROR_PACKING);
            ch = sbuf[j++];
            switch (ch)
            {
            case SLIP_TFEND:
                rbuf.push_back(SLIP_FEND);
                break;
            case SLIP_TFESC:
                rbuf.push_back(SLIP_FESC);
                break;
            }
            break;
        case SLIP_FEND:
            break;
        default:
            rbuf.push_back(ch);
            break;
        }
    } while (ch != SLIP_FEND);

    return (rbuf.size());
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
	if (ssize < 2)
		return (SLIP_ERROR_BUFFER);
	sbuf[i++] = SLIP_FEND;

	for (j=0; j<rsize; ++j)
	{
		switch (rbuf[j])
		{
		case SLIP_FEND:
			if (i >= ssize-2)
				return (SLIP_ERROR_BUFFER);
			sbuf[i++] = SLIP_FESC;
			sbuf[i++] = SLIP_TFEND;
			break;
		case SLIP_FESC:
			if (i >= ssize-2)
				return (SLIP_ERROR_BUFFER);
			sbuf[i++] = SLIP_FESC;
			sbuf[i++] = SLIP_TFESC;
			break;
		default:
			if (i >= ssize-1)
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

int32_t slip_encode(vector<uint8_t> &rbuf, vector<uint8_t> &sbuf)
{
    sbuf.clear();
    sbuf.push_back(SLIP_FEND);

    for (size_t j=0; j<rbuf.size(); ++j)
    {
        switch (rbuf[j])
        {
        case SLIP_FEND:
            sbuf.push_back(SLIP_FESC);
            sbuf.push_back(SLIP_TFEND);
            break;
        case SLIP_FESC:
            sbuf.push_back(SLIP_FESC);
            sbuf.push_back(SLIP_TFESC);
            break;
        default:
            sbuf.push_back(rbuf[j]);
            break;
        }
    }
    sbuf.push_back(SLIP_FEND);
    return (sbuf.size());
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
	return (i);
}

int32_t slip_pack(vector<uint8_t> &rbuf, vector<uint8_t> &sbuf)
{
    int32_t iretn = 0;

    iretn = slip_encode(rbuf, sbuf);
    if (iretn < 0)
    {
        return iretn;
    }

    return sbuf.size();
}

int32_t slip_extract(FILE *fp, vector<uint8_t> &rbuf)
{
    int32_t ch;
//    int32_t j = 0;
    rbuf.clear();

    do
    {
//        if (j > sbuf.size()-3)
//        {
//            return (SLIP_ERROR_PACKING);
//        }
        ch = fgetc(fp);
        if (ch == EOF)
        {
            return GENERAL_ERROR_BAD_FD;
        }
    } while (ch != SLIP_FEND);

    do
    {
//        if (j > sbuf.size()-3)
//        {
//            return (SLIP_ERROR_PACKING);
//        }
        ch = fgetc(fp);
        if (ch == EOF)
        {
            return GENERAL_ERROR_BAD_FD;
        }
        switch (ch)
        {
        case SLIP_FESC:
//            if (j > sbuf.size()-3)
//                return (SLIP_ERROR_PACKING);
            ch = fgetc(fp);
            switch (ch)
            {
            case SLIP_TFEND:
                rbuf.push_back(SLIP_FEND);
                break;
            case SLIP_TFESC:
                rbuf.push_back(SLIP_FESC);
                break;
            }
            break;
        case SLIP_FEND:
            break;
        default:
            rbuf.push_back(ch);
            break;
        }
    } while (ch != SLIP_FEND);

    return (rbuf.size());
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

uint16_t slip_calc_crc(vector<uint8_t> &buf)
{
    uint16_t crc = 0xffff;
    uint8_t ch;

    for (size_t i=0; i<buf.size()-2; ++i)
    {
        ch = buf[i];
        for (size_t j=0; j<8; ++j)
        {
            crc = (crc >> 1)^(((ch^crc)&0x01)?0x8408:0);
            ch >>= 1;
        }
    }
    return (crc);
}

//! @}
