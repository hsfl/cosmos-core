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

// TODO: validate the firmware version for this lib (1.1)

#include "vn100_lib.h"


//! Connect to VN100
/*! Connect to a Vectornav VN100 Inertial Measurement Unit, connected
 * to the specified RS232 device, in preparation for querying it for
 * IMU measurements.
 * \param dev Name of RS232 character device.
 * \param handle Pointer to ::vn100_handle.
 * \return Zero, or negative error number.
*/
int32_t vn100_connect(std::string dev, vn100_handle *handle)
{
	int32_t iretn;

	cssl_start();
    handle->serial = cssl_open(dev.c_str(),VN100_BAUD, VN100_BITS,VN100_PARITY,VN100_STOPBITS);

    if (handle->serial == NULL)
	{
		return (CSSL_ERROR_OPEN);
	}
    // Settimeout nd set flow control is already in cssl_open,
    //	if ((iretn=cssl_settimeout(handle->serial, 0, .1)) < 0)
    //	{
    //		return (iretn);
    //	}

    //    if ((iretn=cssl_setflowcontrol(handle->serial, 0, 0)) < 0)
    //	{
    //		return (iretn);
    //	}

    // stop data stream, we are going to request the data instead
	if ((iretn=vn100_asynchoff(handle)) < 0)
	{
		return (iretn);
	}

	if ((iretn=vn100_magcal_off(handle)) < 0)
	{
		return (iretn);
	}

    return 0;

}

//! Disconnect from VN100.
/*! Disconnect from previously connected VN100, closing associated
 * serial device.
 * \param handle Pointer to ::vn100_handle.
 * \return Zero, or negative error number.
*/
int32_t vn100_disconnect(vn100_handle *handle)
{
    if (handle->serial == NULL) return (CSSL_ERROR_NOTSTARTED);

    cssl_close(handle->serial);
    return 0;
}

//! Get Processed VN100 Measurements.
/*! Query previously opened VN100 for current readings on Magnetic
 * Field, Angular Rate, and Acceleration. Upon a succesful query, the
 * results will be stored in the ::imustruc of the handle.
 * \param handle Pointer to ::vn100_handle.
 * \return Zero, or negative error number.
*/
int32_t vn100_measurements(vn100_handle *handle)
{
	int32_t iretn;

	strcpy(handle->buf,(char *)"VNRRG,252");
	if ((iretn=cssl_putnmea(handle->serial, (uint8_t *)handle->buf, strlen(handle->buf))) < 0)
		return (iretn);

	if ((iretn=cssl_getnmea(handle->serial, (uint8_t *)handle->buf, 150)) < 0)
		return (iretn);
	
	if (strncmp((char *)handle->buf, "VNRRG,252", 9))
	{
		return VN100_ERROR_NACK;
	}

	iretn = sscanf((char *)handle->buf, "VNRRG,252,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%f",
		&handle->imu.mag.col[0],
		&handle->imu.mag.col[1],
		&handle->imu.mag.col[2],
		&handle->imu.accel.col[0],
		&handle->imu.accel.col[1],
		&handle->imu.accel.col[2],
		&handle->imu.omega.col[0],
		&handle->imu.omega.col[1],
		&handle->imu.omega.col[2],
		&handle->imu.gen.temp);

	if (iretn != 10)
	{
		printf("Oops: %d\n",iretn);
		return VN100_ERROR_SCAN;
	}
	
	return(0);
}

//! Get VN100 Raw Voltage.
/*! Query previously opened VN100 for current readings on Magnetic
 * Field, Angular Rate, and Acceleration, providing the result as 
 * raw voltages. Upon a succesful query, the
 * results will be stored in the ::imustruc of the handle.
 * \param handle Pointer to ::vn100_handle.
 * \return Zero, or negative error number.
*/
int32_t vn100_voltages(vn100_handle *handle)
{
	int32_t iretn;

	strcpy(handle->buf,(char *)"VNRRG,251");
	if ((iretn=cssl_putnmea(handle->serial, (uint8_t *)handle->buf, strlen(handle->buf))) < 0)
		return (iretn);

	if ((iretn=cssl_getnmea(handle->serial, (uint8_t *)handle->buf, 150)) < 0)
		return (iretn);
	
	if (strncmp((char *)handle->buf, "VNRRG,251", 9))
	{
		return VN100_ERROR_NACK;
	}

	sscanf((char *)handle->buf, "VNRRG,251,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%f",
		&handle->imu.mag.col[0],
		&handle->imu.mag.col[1],
		&handle->imu.mag.col[2],
		&handle->imu.accel.col[0],
		&handle->imu.accel.col[1],
		&handle->imu.accel.col[2],
		&handle->imu.omega.col[0],
		&handle->imu.omega.col[1],
		&handle->imu.omega.col[2],
		&handle->imu.gen.temp);
	
	return(0);
}

//! Get magnetometer status
/*! Read the VN100 Magnetometer Calibration Status and return values
 * for the most recently collected magnetic measurement.  Upon a succesful query, the
 * results will be stored in the ::imustruc of the handle.
 * \param handle Pointer to ::vn100_handle.
 * \return Zero, or negative error number.
*/
int32_t vn100_magcal_get(vn100_handle *handle)
{
	int32_t iretn;

	strcpy(handle->buf,(char *)"VNRRG,45");
	if ((iretn=cssl_putnmea(handle->serial, (uint8_t *)handle->buf, strlen(handle->buf))) < 0)
		return (iretn);

	if ((iretn=cssl_getnmea(handle->serial, (uint8_t *)handle->buf, 150)) < 0)
		return (iretn);
	
	if (strncmp((char *)handle->buf, "VNRRG,45", 8))
	{
		return VN100_ERROR_NACK;
	}

	sscanf((char *)handle->buf, "VNRRG,45,%*d,%*d,%*f,%lf,%lf,%lf",
		&handle->imu.mag.col[0],
		&handle->imu.mag.col[1],
		&handle->imu.mag.col[2]);
	
	return(0);
}

int32_t vn100_magcal_off(vn100_handle *handle)
{
	int32_t iretn;

	if ((iretn=cssl_putnmea(handle->serial, (uint8_t *)"VNWRG,44,0,0,1", 14)) < 0)
	{
		return (iretn);
	}

	if ((iretn=cssl_getnmea(handle->serial, (uint8_t *)handle->buf, 150)) < 0)
		return (iretn);

	if (strncmp((char *)handle->buf, "VNWRG,44", 8))
	{
		return VN100_ERROR_NACK;
	}

	return 0;
}

int32_t vn100_magcal_on(vn100_handle *handle)
{
	int32_t iretn;

	if ((iretn=cssl_putnmea(handle->serial, (uint8_t *)"VNWRG,44,0,1,1", 14)) < 0)
	{
		return (iretn);
	}

	if ((iretn=cssl_getnmea(handle->serial, (uint8_t *)handle->buf, 150)) < 0)
		return (iretn);

	if (strncmp((char *)handle->buf, "VNWRG,44", 8))
	{
		return VN100_ERROR_NACK;
	}

	return 0;
}

// TODO: explain what is this function doing?
int32_t vn100_asynchoff(vn100_handle *handle)
{
	int32_t iretn;

    // references: VN-100 User Manual (UM001) pg. 47

    // write register command: VNWRG
    // register access ID: 6 (asynchronous data output type)
    // ex. $VNRRG,06,0*69
	if ((iretn=cssl_putnmea(handle->serial, (uint8_t *)"VNWRG,6,0", 9)) < 0)
	{
		return (iretn);
	}

	if ((iretn=cssl_getnmea(handle->serial, (uint8_t *)handle->buf, 150)) < 0)
		return (iretn);

	if (strncmp((char *)handle->buf, "VNWRG,06", 8))
	{
		printf("vn100_asynchoff: %s\n",(char *)handle->buf);
		return VN100_ERROR_NACK;
	}

	return 0;
}

int32_t vn100_get_mag(vn100_handle *handle, double error, double delay)
{
	rvector mn, st, last[5];
	uint32_t count=0;
	int32_t iretn;

	for (uint16_t i=0; i<4; ++i)
	{
		if ((iretn=vn100_measurements(handle)) < 0)
		{
			return (iretn);
		}
		last[i] = handle->imu.mag;
	}

	double mjd=currentmjd(0.);
	delay /= 86400.;
	do
	{
		if ((iretn=vn100_measurements(handle)) < 0)
		{
			return (iretn);
		}
		last[4] = handle->imu.mag;
		mn = rv_zero();
		st = rv_zero();
		for (uint16_t j=0; j<5; ++j)
		{
			mn = rv_add(mn, last[j]);
			st = rv_add(st, rv_mult(last[j], last[j]));
			if (j)
			{
				last[j-1] = last[j];
			}
		}
		st = rv_sqrt(rv_smult(.25, (rv_sub(st, rv_smult(.2, rv_mult(mn, mn))))));
		mn = rv_smult(.2, mn);
		++count;
	} while (((currentmjd(0.)-mjd) < delay) & (st.col[0] > error || st.col[1] > error || st.col[2] > error));
	handle->imu.mag = mn;
	return (count);
}
