/*! \file vn100_lib.h
 * 	\brief vn100_lib include file.
*/

//! \ingroup devices
//!	\defgroup vn100_lib Vectornav VN-100 Inertial Measurement Unit device library.
//! VN-100 Device Library.
//!
//! Device level support for the VN-100, connected through a serial interface. The unit is expected to be in
//! "command mode" and not sending data continuously. To achieve this, the Asynchronous Data Output Type Register
//! (system register 6), should be set to 0 (VNWRG,6,0).

#ifndef _VN100LIB_H
#define _VN100LIB_H

#include "configCosmos.h"
#include "cssl_lib.h"
#include "jsonlib.h"

#define VN100_BAUD 115200
#define VN100_BITS 8
#define VN100_PARITY 0
#define VN100_STOPBITS 1

typedef struct
{
	cssl_t *serial;
	imustruc imu;
	char buf[150];
} vn100_handle;

int32_t vn100_connect(char *dev, vn100_handle *handle);
int32_t vn100_disconnect(vn100_handle *handle);
int32_t vn100_measurements(vn100_handle *handle);
int32_t vn100_voltages(vn100_handle *handle);
int32_t vn100_asynchoff(vn100_handle *handle);
int32_t vn100_magcal_off(vn100_handle *handle);
int32_t vn100_magcal_on(vn100_handle *handle);
int32_t vn100_magcal_get(vn100_handle *handle);
int32_t vn100_get_mag(vn100_handle *handle, double error, double delay);
#endif
