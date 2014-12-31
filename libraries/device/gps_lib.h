/*! \file gps_lib.h
 * 	\brief gps_lib include file.
*/

/*!	\defgroup gps_lib Generic GPS device library.
 * @{
 * Generic GPS Device Library.
 *
 * @}
*/

#ifndef _GPSLIB_H
#define _GPSLIB_H

#include "configCosmos.h"
#include "cssl_lib.h"
#include "jsonlib.h"

#define GPS_BAUD 9600
#define GPS_BITS 8
#define GPS_PARITY 0
#define GPS_STOPBITS 1

typedef struct
{
	cssl_t *serial;
	gpsstruc gps;
	char buf[150];
} gps_handle;

int32_t gps_connect(char *dev, gps_handle *handle);
int32_t gps_disconnect(gps_handle *handle);
int32_t gps_measurements(gps_handle *handle);
#endif
