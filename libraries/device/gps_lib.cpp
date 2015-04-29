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

#include "gps_lib.h"

//! Connect to GPS.
/*! Connect to a GPS speaking NMEA protocol, connected to the
 * specified RS232 device, in preparation for querying it for position
 * measurements.
 * \param dev Name of RS232 character device.
 * \param handle Pointer to ::gps_handle.
 * \return Zero, or negative error number.
*/
int32_t gps_connect(char *dev, gps_handle *handle)
{
cssl_start();
if (handle->serial != NULL)
	return(GPS_ERROR_OPEN);

handle->serial = cssl_open(dev,GPS_BAUD,GPS_BITS,GPS_PARITY,GPS_STOPBITS);
cssl_setflowcontrol(handle->serial,1,0);
if (handle->serial == NULL)
	return (-errno);
cssl_settimeout(handle->serial,1,.1);

return 0;
}

//! Disconnect from GPS.
/*! Disconnect from previously connected GPS, closing associated
 * serial device.
 * \param handle Pointer to ::gps_handle.
 * \return Zero, or negative error number.
*/
int32_t gps_disconnect(gps_handle *handle)
{
if (handle->serial == NULL)
	return (GPS_ERROR_CLOSED);

cssl_close(handle->serial);
return 0;
}

//! Read NMEA response.
/*! Read the serial line for an appopriate NMEA response. The leading
 * '$' and trailing '*' are removed and only the contents are returned
 * in the buffer.
 * \param buf Character buffer for storing response.
 * \param buflen Maximum size of biffer.
 * \return Number of characters stored in buffer, or negative error.
*/
int32_t gps_getnmea(gps_handle *handle, char *buf, int buflen)
{
int32_t i,j;

i = 0;
while ((j=cssl_getdata(handle->serial,(uint8_t *)&buf[i],buflen-i)) > 0)
	{
	i += j;
	if (buf[i-1] == ';')
		{
		buf[i] = 0;
		return (i);
		}
	}
buf[i] = 0;
return (-i);
}

int32_t gps_measurements(gps_handle *handle)
{
//	uint8_t buf[30];



return 0;
}

