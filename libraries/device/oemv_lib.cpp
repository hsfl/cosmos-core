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



//! Time Status
//! check OEMV firmware documentation on page 31
map <string, uint16_t> time_status = {
    {"UNKNOWN",           0},
    {"APPROXIMATE",       1},
    {"COARSEADJUSTING",   2},
    {"COARSE",            3},
    {"COARSESTEERING",    4},
    {"FREEWHEELING",      5},
    {"FINEADJUSTING",     6},
    {"FINE",              7},
    {"FINESTEERING",      8}
};

//! Solution status
//! check OEMV firmware documentation on page 252, Table 51
map <string, uint16_t> solution_status = {
    {"SOL_COMPUTED",      0},
    {"INSUFFICIENT_OBS",  1},
    {"NO_CONVERGENCE",    2},
    {"SINGULARITY",       3},
    {"COV_TRACE",         4},
    {"TEST_DIST",         5},
    {"COLD_START",        6},
    {"V_H_LIMIT",         7},
    {"VARIANCE",          8},
    {"RESIDUALS",         9},
    {"DELTA_POS",         10},
    {"NEGATIVE_VAR",      11},
    {"RSERVED",           12},
    {"INTEGRITY_WARNING", 13},
    {"INS_1",             14},
    {"INS_2",             15},
    {"INS_3",             16},
    {"INS_4",             17},
    {"PENDING",           18},
    {"INVALID_FIX",       19},
    {"UNAUTHORIZED",      20},
    {"ANTENNA_WARNING",   21}
};

//! Position or Velocity types
//! check OEMV firmware documentation on page 251, Table 50
map <string, uint16_t> fix_type = {
    {"NONE",             0},
    {"FIXEDPOS",         1},
    {"FIXEDHEIGHT",      2},
    {"DOPPLER_VELOCITY", 8},
    {"SINGLE",           16},
    {"PSRDIFF",          17},
    {"WASS",             18},
    {"PROPAGATED",       19},
    {"OMNISTAR",         20},
    {"L1_FLOAT",         32},
    {"IONOFREE_FLOAT",   33},
    {"NARROW_FLOAT",     34},
    {"L1_INT",           48},
    {"WIDE_INT",         49},
    {"NARROW_INT",       50},
    {"RTK_DIRECT_INS",   51},
    {"INS",              52},
    {"INS_PSRSP",        53},
    {"INS_PSRDIFF",      54},
    {"INS_RTKFLOAT",     55},
    {"INS_RTKFIXED",     56},
    {"OMNISTAR_HP",      64},
    {"OMNISTAR_XP",      65},
    {"CDGPS",            66}
};


//! Clock Model status
//! check OEMV firmware documentation on page 268, table 54
map <string, uint16_t> clock_status = {
    {"VALID",      0},
    {"CONVERGING", 1},
    {"ITERATING",  2},
    {"INVALID",    3},
    {"ERROR",      4}
};

//! Clock UTC status
//! check OEMV firmware documentation on page 555
map <string, uint16_t> utc_status = {
    {"INVALID",      0},
    {"VALID",        1}
};




//! Connect to OEMV.
/*! Connect to a OEMV speaking NMEA protocol, connected to the
 * specified RS232 device, in preparation for querying it for position
 * measurements.
 * \param dev Name of RS232 character device.
 * \param handle Pointer to ::oemv_handle.
 * \return Zero, or negative error number.
*/
//int32_t oemv_connect(char *dev, oemv_handle *handle)
int32_t oemv_connect(string port, oemv_handle *handle)
{
	int32_t iretn;

	cssl_start();
    handle->serial = cssl_open(port.c_str(),OEMV_BAUD,OEMV_BITS,OEMV_PARITY,OEMV_STOPBITS);
	if (handle->serial == NULL)
		return (CSSL_ERROR_OPEN);
	if ((iretn=cssl_settimeout(handle->serial, 0, 5.)) < 0)
	{
		return (iretn);
	}
	if ((iretn=cssl_setflowcontrol(handle->serial, 0, 0)) < 0)
	{
		return (iretn);
	}

	if ((iretn=oemv_unlogall(handle)) < 0)
	{
		return (iretn);
	}

	return 0;
}

//! Disconnect from OEMV.
/*! Disconnect from previously connected OEMV, closing associated
 * serial device.
 * \param handle Pointer to ::oemv_handle.
 * \return Zero, or negative error number.
*/
int32_t oemv_disconnect(oemv_handle *handle)
{
	if (handle->serial == NULL)
		return (OEMV_ERROR_CLOSED);

	cssl_close(handle->serial);
	return 0;
}

//! Read OEMV Binary response.
/*! Read the serial line for an appopriate OEMV Binary response. The CRC is removed. Only the contents are
 * returned in the buffer.
 * \param handle Pointer to ::oemv_handle.
 * \return Number of characters stored in buffer, or negative error.
*/
int32_t oemv_getbinary(oemv_handle *handle)
{
	union
	{
		uint32_t crc1;
		uint8_t crcb[4];
	};
	uint32_t crc2;
	int32_t iretn;

	// Get 3 sync bytes
	if ((iretn=cssl_getchar(handle->serial)) < 0)
	{
		return (handle->message.header.sync1);
	}
	handle->message.header.sync1 = (uint8_t)iretn;
	printf("oemv_getbinary: sync %0x\n",handle->message.header.sync1);

	if ((iretn=cssl_getchar(handle->serial)) < 0)
	{
		return (handle->message.header.sync2);
	}
	handle->message.header.sync2 = (uint8_t)iretn;
	printf("oemv_getbinary: sync %0x\n",handle->message.header.sync2);

	if ((iretn=cssl_getchar(handle->serial)) < 0)
	{
		return (handle->message.header.sync3);
	}
	handle->message.header.sync3 = (uint8_t)iretn;
	printf("oemv_getbinary: sync %0x\n",handle->message.header.sync3);

	// Get header size
	if ((iretn=cssl_getchar(handle->serial)) < 0)
	{
		return (handle->message.header.header_size);
	}
	handle->message.header.header_size = (uint8_t)iretn;
	printf("oemv_getbinary: header size %d\n",handle->message.header.header_size);

	// Get header
	if ((iretn=cssl_getdata(handle->serial, (uint8_t *)&(handle->message.header.message_id), handle->message.header.header_size-4)) < 0)
		return (iretn);
	printf("oemv_getbinary: header %d\n",iretn);

	// Get data
	if ((iretn=cssl_getdata(handle->serial, (uint8_t *)&(handle->data), handle->message.header.message_size)) < 0)
		return (iretn);
	printf("oemv_getbinary: data size %d\n",iretn);

	for (uint16_t i=0; i<4; ++i)
	{
		if ((iretn=cssl_getchar(handle->serial)) < 0)
			return (iretn);
		crcb[i] = (uint8_t)iretn;
	}

	crc2 = oemv_calc_crc32((uint8_t *)&(handle->message), handle->message.header.header_size + handle->message.header.message_size);
	if (crc1 != crc2)
		return (OEMV_ERROR_CRC);

	printf("oemv_getbinary: %d bytes\n",handle->message.header.message_size);

	return (handle->message.header.message_size);
}

//! Send OEMV Binary message.
/*! Send message in provided ::oemv_handle in binary format.
 * \param handle Pointer to connected ::oemv_handle.
 * \return Zero or negative error.
 */
int32_t oemv_putbinary(oemv_handle *handle)
{
	int32_t iretn;

	// Send Sync characters
	if ((iretn=cssl_putchar(handle->serial, OEMV_SYNC1)) < 0)
	{
		return (iretn);
	}
	handle->message.header.sync1 = OEMV_SYNC1;

	if ((iretn=cssl_putchar(handle->serial, OEMV_SYNC2)) < 0)
	{
		return (iretn);
	}
	handle->message.header.sync2 = OEMV_SYNC1;

	if ((iretn=cssl_putchar(handle->serial, OEMV_SYNC3)) < 0)
	{
		return (iretn);
	}
	handle->message.header.sync3 = OEMV_SYNC1;

	// Send header
	printf("oemv_putbinary: header\n");
	if ((iretn=cssl_putdata(handle->serial, (uint8_t *)&(handle->message.header.message_id), handle->message.header.header_size)) < 0)
		return (iretn);

	// Send data
	printf("oemv_putbinary: data\n");
	if ((iretn=cssl_putdata(handle->serial, (uint8_t *)&(handle->data), handle->message.header.message_size)) < 0)
		return (iretn);

	// Calc CRC
	union
	{
		uint32_t crc;
		uint8_t crcb[4];
	};
	crc = oemv_calc_crc32((uint8_t *)&(handle->message), handle->message.header.header_size + handle->message.header.message_size);

	// Send CRC
	if ((iretn=cssl_putdata(handle->serial, crcb, 4)) < 0)
		return (iretn);

	printf("oemv_putbinary: CRC %0x\n",crc);
	return 0;
}

//! Read OEMV ASCII response.
/*! Read the serial line for an appopriate OEMV ASCII response. The leading
 * '#' and trailing '*' are removed, as well as the CRC. Only the contents are
 * returned in the buffer.
 * \param data Character buffer for storing response.
 * \param datalen Maximum size of buffer.
 * \return Number of characters stored in buffer, or negative error.
*/
int32_t oemv_getascii(oemv_handle *handle)
{
	uint16_t i,j;
	int32_t ch;
	uint32_t crc2;
	union
	{
		uint32_t crc1;
		uint8_t crcb[4];
	};

	// this loop is to ignore any incoming data
	// untill the lead code identifier for the log is received '#'
	// there is sometimes a strange character (-383) that is received ...
	// this happens also if the GPS is off.
	do
	{
		ch = cssl_getchar(handle->serial);
		//printf("%c",ch); //printf("(%d)",ch);
		if (ch < 0)
		{
			//continue;
			return (ch); // MN: this may be a problem in case the incoming byte is not ascii, but the following ones may be ok
		}
	} while (ch != '#'); // || ch != '$'

	// this loop collects the incoming log
	i = 0;
	do
	{
		ch = cssl_getchar(handle->serial);
		if (ch < 0)
			//continue;
			return (ch); // MN: this may be a problem in case the incoming byte is not ascii, but the following ones may be ok

		switch (ch)
		{
		case 10: // new line
			break;
		case 13: // carriage return
			break;
		case '*': // end of message '*'
			break;
		default: // save the incoming char
			handle->data[i++] = (uint8_t)ch;
			break;
		}
	} while (i<OEMV_MAX_DATA && ch != '*');

	handle->data[i] = 0;
	crc2 = oemv_calc_crc32(handle->data, i);

	// get the remaining bytes to compute the message crc
	crc1 = 0;
	char crcbuf[9];
	for (j=0; j<8; ++j)
	{
		ch = cssl_getchar(handle->serial);
		if (ch < 0)
		{
			return (ch);
		}
		crcbuf[j] = (char)ch;
	}
	crcbuf[8] = 0;
	sscanf(crcbuf, "%x", &crc1);
	if (crc1 != crc2)
		return (OEMV_ERROR_CRC);
	return (i);
}




//! Read OEMV ASCII response from GPGGA
/*! Read the serial line for an appopriate OEMV ASCII response. The leading
 * '#' and trailing '*' are removed, as well as the CRC. Only the contents are
 * returned in the buffer.
 * \param data Character buffer for storing response.
 * \param datalen Maximum size of buffer.
 * \return Number of characters stored in buffer, or negative error.
*/
int32_t oemv_getascii_gpgga(oemv_handle *handle)
{
	uint16_t i;
	int32_t ch;
	//    uint32_t crc2;
	union
	{
		uint32_t crc1;
		uint8_t crcb[4];
	};

	// this loop is to ignore any incoming data
	// untill the lead code identifier for the log is received '#'
	// there is sometimes a strange character (-383) that is received ...
	// this happens also if the GPS is off.
	do
	{
		ch = cssl_getchar(handle->serial);
		//printf("%c",ch); //printf("(%d)",ch);
		if (ch < 0)
		{
			//continue;
			return (ch); // MN: this may be a problem in case the incoming byte is not ascii, but the following ones may be ok
		}
	} while (ch != '$'); // || ch != '$'

	// this loop collects the incoming log
	i = 0;
	do
	{
		ch = cssl_getchar(handle->serial);
		if (ch < 0)
		{
			return (ch); // MN: this may be a problem in case the incoming byte is not ascii, but the following ones may be ok
		}

		switch (ch)
		{
		case 10: // new line
			break;
		case 13: // carriage return
			break;
		case '*': // end of message '*'
			break;
		default: // save the incoming char
			handle->data[i++] = (uint8_t)ch;
			break;
		}
	} while (i<OEMV_MAX_DATA && ch != '*');

	handle->data[i] = 0;
	/*
	// must change to 8bit-crc, check page 34 of firmware manual
	crc2 = oemv_calc_crc32(handle->data, i);


	// get the remaining bytes to compute the message crc
	crc1 = 0;
	char crcbuf[9];
	for (j=0; j<2; ++j)
	{
		ch = cssl_getchar(handle->serial);
		if (ch < 0)
			//continue;
			return (ch);
		crcbuf[j] = ch;
	}
	crcbuf[8] = 0;
	sscanf(crcbuf, "%x", &crc1);
	if (crc1 != crc2)
		return (OEMV_ERROR_CRC);
	*/
	return (i);
}






int32_t oemv_putascii(oemv_handle *handle)
{
	int32_t iretn;
	union
	{
		uint32_t crc;
		uint8_t crcb[4];
	};

	if ((iretn=cssl_putchar(handle->serial, '#')) < 0)
	{
		return (iretn);
	}
	for (uint16_t i=0; i<strlen((char *)handle->data); ++i)
	{
		if ((iretn=cssl_putchar(handle->serial, handle->data[i])) < 0)
		{
			return (iretn);
		}
	}

	if ((iretn=cssl_putchar(handle->serial, '*')) < 0)
	{
		return (iretn);
	}

	// Calculate and send CRC
	crc = oemv_calc_crc32(handle->data, strlen((char *)handle->data));
	for (uint16_t i=0; i<4; ++i)
	{
		char tbyte[3];
		sprintf(tbyte, "%02x", crcb[3-i]);
		tbyte[2] = 0;
		if ((iretn=cssl_putchar(handle->serial, tbyte[0])) < 0)
		{
			return (iretn);
		}
		if ((iretn=cssl_putchar(handle->serial, tbyte[1])) < 0)
		{
			return (iretn);
		}
	}

	if ((iretn=cssl_putchar(handle->serial, '\r')) < 0)
	{
		return (iretn);
	}

	return 0;
}

int32_t oemv_unlogall(oemv_handle *handle)
{
	int32_t iretn;

	sprintf((char *)handle->data, "UNLOGALL,COM1,0,0.,UNKNOWN,0,0.0,0,0;ALL_PORTS,TRUE");
	if ((iretn=oemv_talkascii(handle, false)) < 0)
	{
		return (iretn);
	}
	return 0;
}

int32_t oemv_reset(oemv_handle *handle)
{
	int32_t iretn;

	sprintf((char *)handle->data, "RESET,COM1,0,0.,UNKNOWN,0,0.0,0,0;2");
	if ((iretn=oemv_talkascii(handle, false)) < 0)
	{
		return (iretn);
	}
	return 0;
}

int32_t oemv_freset(oemv_handle *handle)
{
	int32_t iretn;

	sprintf((char *)handle->data, "FRESET,COM1,0,0.,UNKNOWN,0,0.0,0,0;2");
	if ((iretn=oemv_talkascii(handle, false)) < 0)
	{
		return (iretn);
	}
	return 0;
}

int32_t oemv_saveconfig(oemv_handle *handle)
{
	int32_t iretn;

	sprintf((char *)handle->data, "SAVECONFIG,COM1,0,0.,UNKNOWN,0,0.0,0,0;2");
	if ((iretn=oemv_talkascii(handle, false)) < 0)
	{
		return (iretn);
	}
	return 0;
}

int32_t oemv_setapproxpos(oemv_handle *handle, gvector pos)
{
	int32_t iretn;

	sprintf((char *)handle->data, "SETAPPROXPOS,COM1,0,0.,UNKNOWN,0,0.0,0,0;%f,%f,%f", DEGOF(pos.lat), DEGOF(pos.lon), pos.h);
	if ((iretn=oemv_talkascii(handle, false)) < 0)
	{
		return (iretn);
	}
	return 0;
}

int32_t oemv_log(oemv_handle *handle, const char* log)
{
	int32_t iretn;
	sprintf((char *)handle->data,
			"LOGA,COM1,0,0.,UNKNOWN,0,0.0,0,0;COM1,%s,ONCE,0.,0.,NOHOLD", log);

    // for NMEA messages starting with '$'
    if (strcmp(log, "GPGGAA") == 0 || strcmp(log, "GPGSVA") == 0)
    {


		if ((iretn=oemv_putascii(handle)) < 0)
		{
			return (iretn);
		}

		// Retrieve echoed command response
		if ((iretn=oemv_getascii(handle)) < 0)
		{
			return (iretn);
		}

		// Retrieve response
		if ((iretn=oemv_getascii_gpgga(handle)) < 0)
		{
			//printf("error 3\n");
			return (iretn);
		}
		//printf("gpgga test: %s\n", handle->data);


    }
    else // for the default messages starting with '#'
    {
		if ((iretn=oemv_talkascii(handle, true)) < 0)
		{
			//printf("error 2\n");
			return (iretn);
		}
		// Return should start with contents of log
		if (strncmp(log, (char *)handle->data, strlen(log)))
		{
			return(OEMV_ERROR_RESPONSE);
		}

		uint32_t length = iretn;

		string t_status;
		uint16_t nexti;
		uint16_t lasti=0;

        // Skip over four commas // ??? why ???
		for (uint16_t i=0; i<4; ++i)
		{
			for(nexti=lasti; nexti<length; ++nexti)
			{
				if (nexti >= length)
				{
					return(OEMV_ERROR_RESPONSE);
				}

				if (handle->data[nexti] == ',')
				{
					lasti = nexti + 1;
					break;
				}
			}

		}

		if (lasti >= length)
		{
			return(OEMV_ERROR_RESPONSE);
		}

        //  Read in clock_status // why here ??? should only be called in get_time or something
		for(nexti=lasti; nexti<length; ++nexti)
		{
			if (nexti >= length)
			{
				return(OEMV_ERROR_RESPONSE);
			}

			if (handle->data[nexti] == ',')
			{
				handle->data[nexti] = 0;
				t_status.assign((char *)&handle->data[lasti]);
				handle->message.header.time_status = time_status[t_status];
				lasti = nexti + 1;
				break;
			}
		}

		if (lasti >= length)
		{
			return(OEMV_ERROR_RESPONSE);
		}

        // Scan GPS Week and Seconds //
		sscanf((char *)&handle->data[lasti], "%hu,%f",
			   &handle->message.header.gps_week,
			   &handle->message.header.gps_second);
		// Adjust for transmission lag
		handle->message.header.gps_second += (float)(length * 10./OEMV_BAUD);
		if (handle->message.header.gps_second > 604600.)
		{
			handle->message.header.gps_second -= 604600.;
			handle->message.header.gps_week += 1;
		}

		// Skip over intro
		for (lasti=0; lasti<length; ++lasti)
		{
			if (handle->data[lasti] == ';')
			{
				break;
			}
		}

		if (++lasti >= length)
		{
			return(OEMV_ERROR_RESPONSE);
		}

		memmove(handle->data, &handle->data[lasti], length-lasti);

		return length-lasti;

	}

	return iretn;
}

int32_t oemv_trackstat(oemv_handle *handle)
{
	int32_t iretn;
	uint16_t satcnt;

	if ((iretn=oemv_log(handle, "TRACKSTATA")) < 0)
	{
		return iretn;
	}
	uint16_t length = iretn;

	uint16_t nexti;
	uint16_t lasti=0;

	// Skip over three commas
	for (uint16_t i=0; i<3; ++i)
	{
		for(nexti=lasti; nexti<length; ++nexti)
		{
			if (nexti >= length)
			{
				return(OEMV_ERROR_RESPONSE);
			}

			if (handle->data[nexti] == ',')
			{
				lasti = nexti + 1;
				break;
			}
		}

	}


	if (lasti >= length)
	{
		return(OEMV_ERROR_RESPONSE);
	}

	// Scan in number of satellites
	for(nexti=lasti; nexti<length; ++nexti)
	{
		if (nexti >= length)
		{
			return(OEMV_ERROR_RESPONSE);
		}

		if (handle->data[nexti] == ',')
		{
			handle->data[nexti] = 0;
			sscanf((char *)&handle->data[lasti], "%hu", &satcnt);
			lasti = nexti + 1;
			break;
		}
	}

	printf("%s\n", handle->data);

	if (lasti >= length)
	{
		return(OEMV_ERROR_RESPONSE);
	}

	for (uint16_t i=0; i<satcnt; ++i)
	{
		uint16_t basei = lasti;
		// Skip over ten commas
		for (uint16_t i=0; i<10; ++i)
		{
			for(nexti=lasti; nexti<length; ++nexti)
			{
				if (nexti >= length)
				{
					return(OEMV_ERROR_RESPONSE);
				}

				if (handle->data[nexti] == ',')
				{
					if (i == 9)
					{
						handle->data[nexti] = 0;
						printf("%s\n", (char *)&handle->data[basei]);
					}
					lasti = nexti + 1;
					break;
				}
			}

		}

	}

	return 0;
}

int32_t oemv_satvis(oemv_handle *handle)
{
	// SATVIS Satellite Visibility V123, pg 549
	// this provides an overview of all the satellites visible
	// probably too much information
	int32_t iretn;

	if ((iretn=oemv_log(handle, "SATVISA")) < 0)
	{
		return iretn;
	}

	//uint32_t length = iretn;
	//printf("%s\n", handle->data);

	return 0;
}

int32_t oemv_gpgga(oemv_handle *handle)
{
	// Time, position and fix-related data of the GPS receiver.
	// GPGGA GPS Fix Data and Undulation V123_NMEA. Pg. 313
	// we can get utc, lat, lon, alt, num satellites in use
	// to get number of satellites in view use: GPGSV pg. 328
	int32_t iretn;

	//printf("Requesting GPPGA\n");
	if ((iretn=oemv_log(handle, "GPGGAA")) < 0)
	{
		//printf("gpgga error \n");
		return iretn;
	}

	//    cout << "-------------" << endl;
	//    cout << "gpgga: << " << handle->data << endl;

	// collect the number of satellites
	char * pch;
	pch = strtok((char*)handle->data,",");

	string field = "";
	//char mm[7] = {'\0'};
	//char mm_lat[7] = {'\0'};

	int i = 1;
	//
	while (pch != NULL)
	{
		//printf ("%s\n",pch);
		pch = strtok (NULL, ",");
		if (pch == NULL){
			break;
		}
		field = string(pch);
		i++;

		if (i>10){
			// no more fields to collect, continue
			break;
		}

		if (i==3){
			// latitude DDmm.mm
			// split DD
			char DD[2] = {'\0'};
			strncpy(DD, pch, 2);

			float lat = 0;
			lat = atof(DD);

			// split mm.mm

			//strncpy(mm, pch+2, 7+2);
			if (field.size() != 9){
				break;
			}
#ifdef COSMOS_CYGWIN_OS
			handle->message.geo.lat  = lat + strtod(field.substr(2,field.size()).c_str(), NULL)/60.;
#else
			handle->message.geo.lat  = lat + stod(field.substr(2,field.size()))/60.;
#endif
		}

		if (i==4){
			// latitude direction
			if (pch[0] == 'S'){
				double temp_lat = -handle->message.geo.lat;
				handle->message.geo.lat = temp_lat;
			}
		}

		if (i==5){
			// longitude DDDmm.mm
			// split DDD
			char DDD[3] = {'\0'};
			strncpy(DDD, pch, 3);

			float lon = 0;
			lon = atof(DDD);

			// split mm.mm
			//string mmm = field.substr(3,9);
			//strncpy(mm, pch+3, 7+3);
			if (field.size() != 10){
				break;
			}
#ifdef COSMOS_CYGWIN_OS
			handle->message.geo.lon  = lon + strtod(field.substr(2,field.size()).c_str(), NULL)/60.;
#else
			handle->message.geo.lon  = lon + stod(field.substr(3,field.size()))/60.;
#endif
		}

		if (i==6){
			// latitude direction
			if (pch[0] == 'W'){
				double temp_lon = -handle->message.geo.lon;
				handle->message.geo.lon = temp_lon;
			}
		}

		if (i==7){
			//gps quality
		}

		if (i==8){
			// number of satellites used by GPS calculations
			handle->message.n_sats_used = atoi(pch);
		}

		if (i==10){
			// altitude
			handle->message.geo.h = atof(pch);
		}

	}

	return 0;
}

// to get number of satellites in view: GPGSV pg. 328
int32_t oemv_gpgsv(oemv_handle *handle)
{

	int32_t iretn;

	//printf("Requesting GPPGA\n");
	if ((iretn=oemv_log(handle, "GPGSVA")) < 0)
	{
		printf("error \n");
		return iretn;
	}

	//uint32_t length = iretn;
	//printf("gpgga: %s\n", handle->data);

	//    cout << "-------------" << endl;
	//    cout << "gpgsv: << " << handle->data << endl;

	// reset counter
	int i = 1;
	char * pch = strtok((char*)handle->data,",");
	while (pch != NULL)
	{
		//printf ("%s\n",pch);
		pch = strtok (NULL, ",");
		if (pch == NULL){
			break;
		}

		i++;

		if (i>4){
			// no more fields to collect, exit the loop
			break;
		}


		if (i==4){
			// number of satellites in view
			handle->message.n_sats_visible = atoi(pch);
		}


	}

	return 0;
}

// This log provides several time related pieces of information including receiver clock offset and UTC time and offset
// pg. 553
int32_t oemv_time(oemv_handle *handle)
{
	int32_t iretn;

	if ((iretn=oemv_log(handle, "TIMEA")) < 0)
	{
		return iretn;
	}

	uint32_t length = iretn;

    string c_status; // clock status string
//	uint16_t nexti;
//	uint16_t lasti=0;

    // convert handle->data to string
    // !!! this would be unecessary if omev_log would directly return a string
    string logdata;
    logdata.assign(handle->data, handle->data + length);
    //logdata = string( (char *)handle->data); // this should work because the handle->data is null terminated

    StringParser parser(logdata);
    // set the offset so we can call getFieldNumber with the tabulated field number in the OEMV documentation tables
    parser.offset = -2;

    //read in clock_status
    c_status = parser.getFieldNumber(2);

    if (c_status.empty())
    {
        return OEMV_ERROR_RESPONSE;
    }

//	// Find first comma and read in clock_status
//	for(nexti=lasti; nexti<length; ++nexti)
//	{
//		if (handle->data[nexti] == ',')
//		{
//			handle->data[nexti] = 0;
//			c_status.assign((char *)&handle->data[lasti]);
//			handle->message.time.clock_status = clock_status[c_status];
//			lasti = nexti + 1;
//			break;
//		}
//	}

    handle->message.time.clock_status = clock_status[c_status];
    handle->message.time.clock_status_str = c_status;

//	if (++lasti >= length)
//	{
//		return(OEMV_ERROR_RESPONSE);
//	}

//	// Scan variables in place
//	sscanf((char *)&handle->data[lasti],"%lf,%lf,%lf,%u,%hhu,%hhu,%hhu,%hhu,%u"
//		   ,&handle->message.time.offset
//		   ,&handle->message.time.offset_std
//		   ,&handle->message.time.utc_offset
//		   ,&handle->message.time.utc_year
//		   ,&handle->message.time.utc_month
//		   ,&handle->message.time.utc_day
//		   ,&handle->message.time.utc_hour
//		   ,&handle->message.time.utc_minute
//		   ,&handle->message.time.utc_ms
//		   );

    handle->message.time.offset      = parser.getFieldNumberAsDouble(3);
    handle->message.time.offset_std  = parser.getFieldNumberAsDouble(4);
    handle->message.time.utc_offset  = parser.getFieldNumberAsDouble(5);
    handle->message.time.utc_year    = parser.getFieldNumberAsDouble(6);
    handle->message.time.utc_month   = parser.getFieldNumberAsDouble(7);
    handle->message.time.utc_day     = parser.getFieldNumberAsDouble(8);
    handle->message.time.utc_hour    = parser.getFieldNumberAsDouble(9);
    handle->message.time.utc_minute  = parser.getFieldNumberAsDouble(10);
    handle->message.time.utc_ms      = parser.getFieldNumberAsDouble(11);


	// Adjust for transmission lag: 66 characters plus length of message
	double offset = (66 + length) * (10./OEMV_BAUD);
	handle->message.time.utc_ms += 1000. * offset + .5;

//	// Skip over intervening commas
//	for (uint16_t i=0; i<9; ++i)
//	{
//		for(nexti=lasti; nexti<length; ++nexti)
//		{
//			if (handle->data[nexti] == ',')
//			{
//				lasti = nexti;
//				break;
//			}
//		}

//		if (++lasti >= length)
//		{
//			return(OEMV_ERROR_RESPONSE);
//		}
//	}

//	// Scan in UTC Status
//	string u_status;
//	u_status.assign((char *)&handle->data[lasti]);
//	handle->message.time.utc_status = clock_status[u_status];
    handle->message.time.utc_status_str = parser.getFieldNumber(12);
    handle->message.time.utc_status = utc_status[handle->message.time.utc_status_str];

	return 0;
}

int32_t oemv_version(oemv_handle *handle)
{
	int32_t iretn;

	handle->message.header.message_id = OEMV_MESSAGE_ID_LOG;
	handle->message.header.header_size = 28;
	handle->message.header.message_size = sizeof(oemv_log_type);
	handle->message.header.sequence = 0;
	handle->message.log.message_id = OEMV_MESSAGE_ID_VERSION;

	if ((iretn=oemv_talkbinary(handle)) <= 0)
	{
		return (iretn);
	}

	return 0;
}

int32_t oemv_bestpos(oemv_handle *handle)
{
	int32_t iretn;

	handle->message.header.message_id = OEMV_MESSAGE_ID_LOG;
	handle->message.header.header_size = 28;
	handle->message.header.message_size = sizeof(oemv_log_type);
	handle->message.header.sequence = 0;
	handle->message.log.message_id = OEMV_MESSAGE_ID_BESTPOS;

	if ((iretn=oemv_talkbinary(handle)) <= 0)
	{
		return (iretn);
	}

	return 0;
}

int32_t oemv_bestvel(oemv_handle *handle)
{
	int32_t iretn;

	handle->message.header.message_id = OEMV_MESSAGE_ID_LOG;
	handle->message.header.header_size = 28;
	handle->message.header.message_size = sizeof(oemv_log_type);
	handle->message.header.sequence = 0;
	handle->message.log.message_id = OEMV_MESSAGE_ID_BESTVEL;

	if ((iretn=oemv_talkbinary(handle)) <= 0)
	{
		return (iretn);
	}

	return 0;
}

// request best available cartesian position and velocity
// ref pg 261
int32_t oemv_bestxyz(oemv_handle *handle)
{
	int32_t iretn;
    string p_status, p_type, v_status, v_type;

	if ((iretn=oemv_log(handle, "BESTXYZA")) < 0)
	{
		return iretn;
	}
	uint32_t length = iretn;


    // convert handle->data to string
    // !!! this would be unecessary if omev_log would directly return a string
    string logdata;
    logdata.assign(handle->data, handle->data + sizeof(handle->data));

    StringParser parser(logdata);
    p_status = parser.getFieldNumber(1);

    //cout << pos_status << endl;

    handle->message.bestxyz.position_status = solution_status[p_status];
    handle->message.bestxyz.position_status_str = p_status;

	// Find next comma and read in position_status

	uint16_t nexti=0;
	uint16_t lasti=0;
//	for(nexti=lasti; nexti<length; ++nexti)
//	{
//		if (handle->data[nexti] == ',')
//		{
//			handle->data[nexti] = 0;
//			p_status.assign((char *)&handle->data[lasti]);
//			handle->message.bestxyz.position_status = solution_status[p_status];
//			lasti = nexti;
//			break;
//		}
//	}

	if (++lasti >= length)
	{
		return(OEMV_ERROR_RESPONSE);
	}

	// Find next comma and read in position_type
	for(nexti=lasti; nexti<length; ++nexti)
	{
		if (handle->data[nexti] == ',')
		{
			handle->data[nexti] = 0;
			p_type.assign((char *)&handle->data[lasti]);
			handle->message.bestxyz.position_type = fix_type[p_type];
			lasti = nexti;
			break;
		}
	}

	if (++lasti >= length)
	{
		return(OEMV_ERROR_RESPONSE);
	}

	// Scan variables in place
	sscanf((char *)&handle->data[lasti],"%lf,%lf,%lf,%f,%f,%f"
		   ,&handle->message.bestxyz.position_x
		   ,&handle->message.bestxyz.position_y
		   ,&handle->message.bestxyz.position_z
		   ,&handle->message.bestxyz.position_x_sd
		   ,&handle->message.bestxyz.position_y_sd
		   ,&handle->message.bestxyz.position_z_sd
		   );

	// Skip over intervening commas
	for (uint16_t i=0; i<6; ++i)
	{
		for(nexti=lasti; nexti<length; ++nexti)
		{
			if (handle->data[nexti] == ',')
			{
				lasti = nexti;
				break;
			}
		}

		if (++lasti >= length)
		{
			return(OEMV_ERROR_RESPONSE);
		}
	}

	// Find next comma and read in velocity_status
	for(nexti=lasti; nexti<length; ++nexti)
	{
		fflush(stdout);
		if (handle->data[nexti] == ',')
		{
			handle->data[nexti] = 0;
			v_status.assign((char *)&handle->data[lasti]);
			handle->message.bestxyz.velocity_status = solution_status[v_status];
			lasti = nexti;
			break;
		}
	}

	if (++lasti >= length)
	{
		return(OEMV_ERROR_RESPONSE);
	}

	// Find next comma and read in velocity_type
	for(nexti=lasti; nexti<length; ++nexti)
	{
		if (handle->data[nexti] == ',')
		{
			handle->data[nexti] = 0;
			v_type.assign((char *)&handle->data[lasti]);
			handle->message.bestxyz.velocity_type = fix_type[v_type];
			lasti = nexti;
			break;
		}
	}

	if (++lasti >= length)
	{
		return(OEMV_ERROR_RESPONSE);
	}

	// Scan variables in place
	sscanf((char *)&handle->data[lasti],"%lf,%lf,%lf,%f,%f,%f"
		   ,&handle->message.bestxyz.velocity_x
		   ,&handle->message.bestxyz.velocity_y
		   ,&handle->message.bestxyz.velocity_z
		   ,&handle->message.bestxyz.velocity_x_sd
		   ,&handle->message.bestxyz.velocity_y_sd
		   ,&handle->message.bestxyz.velocity_z_sd
		   );


	return 0;
}


// RXSTATUS, Receiver status
// This log conveys various status parameters of the GPS receiver system. These include the Receiver
// Status and Error words which contain several flags specifying status and error conditions
// Ref: OEMV Family Firmware Version 3.500 Reference Manual Rev 6 pg 540
int32_t oemv_rxstatus(oemv_handle *handle)
{
	uint32_t length;
	int32_t iretn;

	sprintf((char *)handle->data, "LOGA,COM1,0,0.,UNKNOWN,0,0.0,0,0;COM1,RXSTATUSA,ONCE,0.,0.,NOHOLD");
	if ((iretn=oemv_talkascii(handle, true)) < 0)
	{
		return (iretn);
	}
	length = iretn;

	// Skip over intro
	uint16_t lasti=0;
	for (lasti=0; lasti<length; ++lasti)
	{
		if (handle->data[lasti] == ';')
		{
			break;
		}
	}

	if (++lasti >= length)
	{
		return(OEMV_ERROR_RESPONSE);
	}

	//	printf("%s\n\n", (char *)&handle->data[lasti]);

	// Scan variables in place
	sscanf((char *)&handle->data[lasti],"%u,%u"
		   ,&handle->message.rxstatus.error
		   ,&handle->message.rxstatus.count
		   );

	// Skip over intervening commas
	uint16_t nexti=0;
	for (uint16_t i=0; i<2; ++i)
	{
		for(nexti=lasti; nexti<length; ++nexti)
		{
			if (handle->data[nexti] == ',')
			{
				lasti = nexti;
				break;
			}
		}

		if (++lasti >= length)
		{
			return(OEMV_ERROR_RESPONSE);
		}
	}

	// Scan and skip in groups of 4 for next rxstatus.count
	for (uint16_t i=0; i<handle->message.rxstatus.count; ++i)
	{
		sscanf((char *)&handle->data[lasti],"%u,%u,%u,%u"
			   ,&handle->message.rxstatus.rx[i].word
			   ,&handle->message.rxstatus.rx[i].pri_mask
			   ,&handle->message.rxstatus.rx[i].set_mask
			   ,&handle->message.rxstatus.rx[i].clear_mask
			   );

		for (uint16_t i=0; i<4; ++i)
		{
			for(nexti=lasti; nexti<length; ++nexti)
			{
				if (handle->data[nexti] == ',')
				{
					lasti = nexti;
					break;
				}
			}

			if (++lasti >= length)
			{
				return(OEMV_ERROR_RESPONSE);
			}
		}
	}


	return 0;
}

//! Set Approximate Time
/*! Before acquisition, the GPS will not know its time or position.
 * This function will set an estimated time that will be used until
 * the actual time is acquired.
 * \param handle Pointer to ::oemv_handle of a connected OEMV GPS.
 * \param utc Coordinated Universal Time, expressed as Modified Julian Day.
 * \return Zero, or negative error number.
 */
int32_t oemv_setapproxtime(oemv_handle *handle, double utc)
{
	int32_t iretn;
	uint32_t gpsweek;
	double gpssecond;

	gps2week(utc2gps(utc), gpsweek, gpssecond);
	sprintf((char *)handle->data, "SETAPPROXTIME,COM1,0,0.,UNKNOWN,0,0.0,0,0;%d,%f",gpsweek,fabs(gpssecond));
	if ((iretn=oemv_talkascii(handle, false)) < 0)
	{
		return (iretn);
	}

	return 0;
}

uint32_t oemv_calc_crc32(uint8_t *data, uint16_t size)
{
	uint32_t crc1, crc2;
	uint32_t crc=0;

	for (uint16_t i=0; i<size; ++i)
	{
		crc1 = (crc >> 8) & 0x00ffffff;
		crc2 = (crc ^ data[i]) & 0xff;
		for (uint16_t j=0; j<8; ++j)
		{
			if (crc2 & 1)
				crc2 = (crc2 >> 1) ^ CRC32_POLYNOMIAL;
			else
				crc2 >>= 1;
		}
		crc = crc1 ^ crc2;
	}

	return (crc);
}


int32_t oemv_talkascii(oemv_handle *handle, bool data_flag)
{
	int32_t iretn;

	if ((iretn=oemv_putascii(handle)) < 0)
	{
		return (iretn);
	}

	// Retrieve echoed command response
	if ((iretn=oemv_getascii(handle)) < 0)
	{
		return (iretn);
	}
    // ??? what is the data flag doing here?
	if (!data_flag)
	{
		return (iretn);
	}

	// Retrieve response
	if ((iretn=oemv_getascii(handle)) < 0)
	{
		//printf("error 3\n");
        //cout << "error: " << iretn << endl;
		return (iretn);
	}

	return (iretn);
}

int32_t oemv_talkbinary(oemv_handle *handle)
{
	int32_t iretn;

	// Send binary message
	handle->message.log.port_address = OEMV_PORT_COM2;
	handle->message.log.message_type = OEMV_MESSAGE_TYPE_BINARY;
	handle->message.log.trigger = OEMV_MESSAGE_TRIGGER_ONCE;
	handle->message.log.period = 0.;
	handle->message.log.offset = 0.;
	handle->message.log.hold = 0;

	if ((iretn=oemv_putbinary(handle)) < 0)
	{
		return (iretn);
	}

	// Retrieve echoed command response
	if ((iretn=oemv_getbinary(handle)) <= 0)
	{
		return (iretn);
	}

	if (!handle->message.header.message_type && OEMV_MESSAGE_TYPE_RESPONSE)
	{
		return (OEMV_ERROR_RESPONSE);
	}

	if (handle->message.response.id != OEMV_RESPONSE_OK)
	{
		return (OEMV_ERROR_RESPONSE);
	}

	// Retrieve actual data
	if ((iretn=oemv_getbinary(handle)) <= 0)
	{
		return (iretn);
	}

	return 0;
}

int32_t oemv_getmessage(oemv_handle *handle)
{
	union
	{
		uint32_t crc1;
		uint8_t crcb[4];
	};
	uint32_t crc2, size;
	int32_t iretn;

	size = handle->message.header.header_size + handle->message.header.message_size;

	for (uint16_t i=0; i<size; ++i)
	{
		if ((iretn=cssl_getchar(handle->serial)) < 0)
			return (iretn);
		handle->data[i] = (uint8_t)iretn;
	}

	crc2 = oemv_calc_crc32(handle->data, size);
	for (uint16_t i=0; i<4; ++i)
	{
		if ((iretn=cssl_getchar(handle->serial)) < 0)
			return (iretn);
		crcb[i] = (uint8_t)iretn;
	}
	if (crc1 != crc2)
		return (OEMV_ERROR_CRC);

	return 0;
}
