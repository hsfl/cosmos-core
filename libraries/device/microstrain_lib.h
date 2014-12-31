/*! \file microstrain_lib.h
	\brief microstrain_lib include file
	A library providing control routines for microstrain IMUs.
*/
//! \ingroup devices
//! \defgroup microstrain MicroStrain IMU handling library
//! Microstrain IMU device library

#ifndef _MICROSTRAIN_H
#define _MICROSTRAIN_H 1

#include "configCosmos.h"
#if defined(COSMOS_WIN_OS) || defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS)

#include "mathlib.h"
#include "cssl_lib.h"
#include "cosmos-errno.h"

//#include <sys/types.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

//! \ingroup microstrain
//! \defgroup microstrain_constants MicroStrain IMU constants
//! @{

#define IMUMAXCOUNT 4

#define MICROSTRAIN_ID 12
#define MICROSTRAIN_BAUD 115200
#define MICROSTRAIN_BITS 8
#define MICROSTRAIN_PARITY 0
#define MICROSTRAIN_STOPBITS 1

#define MICROSTRAIN_RAW_ACC_RAT 0xc1
#define MICROSTRAIN_ACC_RAT 0xc2
#define MICROSTRAIN_DELTA_ANG_VEL 0xc3
#define MICROSTRAIN_SET_CONTINUOUS 0xc4
#define MICROSTRAIN_ORI 0xc5
#define MICROSTRAIN_ORI_UPDATE 0xc6
#define MICROSTRAIN_MAG 0xc7
#define MICROSTRAIN_ACC_RAT_ORI 0xc8
#define MICROSTRAIN_WRITE_ACC_BIAS 0xc9
#define MICROSTRAIN_WRITE_GYR_BIAS 0xca
#define MICROSTRAIN_ACC_RAT_MAG 0xcb
#define MICROSTRAIN_ACC_RAT_MAG_ORI 0xcc
#define MICROSTRAIN_INIT_GYRO_BIAS 0xcd
#define MICROSTRAIN_EULER_ANG 0xce
#define MICROSTRAIN_EULER_ANG_RAT 0xcf
#define MICROSTRAIN_WRITE_NVM 0xd0
#define MICROSTRAIN_TEMP 0xd1
#define MICROSTRAIN_STAB_ACC_RAT_MAG 0xd2
#define MICROSTRAIN_DELTA_ANG_VEL_MAG 0xd3
#define MICROSTRAIN_WRITE_EEPROM 0xe4
#define MICROSTRAIN_READ_EEPROM 0xe5
#define MICROSTRAIN_FIRMWARE 0xe9
#define MICROSTRAIN_DEVICEID 0xea
#define MICROSTRAIN_STOP_CONTINUOUS 0xfa

#define MICROSTRAIN_RAW_ACC_RAT_BOUT 1
#define MICROSTRAIN_ACC_RAT_BOUT 1
#define MICROSTRAIN_DELTA_ANG_VEL_BOUT 1
#define MICROSTRAIN_SET_CONTINUOUS_BOUT 4
#define MICROSTRAIN_ORI_BOUT 1
#define MICROSTRAIN_ORI_UPDATE_BOUT 1
#define MICROSTRAIN_MAG_BOUT 1
#define MICROSTRAIN_ACC_RAT_ORI_BOUT 1
#define MICROSTRAIN_WRITE_ACC_BIAS_BOUT 15
#define MICROSTRAIN_WRITE_GYR_BIAS_BOUT 15
#define MICROSTRAIN_ACC_RAT_MAG_BOUT 1
#define MICROSTRAIN_ACC_RAT_MAG_ORI_BOUT 1
#define MICROSTRAIN_INIT_GYRO_BIAS_BOUT 5
#define MICROSTRAIN_EULER_ANG_BOUT 1
#define MICROSTRAIN_EULER_ANG_RAT_BOUT 1
#define MICROSTRAIN_WRITE_NVM_BOUT 5
#define MICROSTRAIN_TEMP_BOUT 1
#define MICROSTRAIN_STAB_ACC_RAT_MAG_BOUT 1
#define MICROSTRAIN_DELTA_ANG_VEL_MAG_BOUT 1
#define MICROSTRAIN_WRITE_EEPROM_BOUT 8
#define MICROSTRAIN_READ_EEPROM_BOUT 4
#define MICROSTRAIN_FIRMWARE_BOUT 1
#define MICROSTRAIN_DEVICEID_BOUT 2
#define MICROSTRAIN_STOP_CONTINUOUS_BOUT 1

#define MICROSTRAIN_RAW_ACC_RAT_BIN 31
#define MICROSTRAIN_ACC_RAT_BIN 31
#define MICROSTRAIN_DELTA_ANG_VEL_BIN 31
#define MICROSTRAIN_SET_CONTINUOUS_BIN 8
#define MICROSTRAIN_ORI_BIN 43
#define MICROSTRAIN_ORI_UPDATE_BIN 43
#define MICROSTRAIN_MAG_BIN 19
#define MICROSTRAIN_ACC_RAT_ORI_BIN 67
#define MICROSTRAIN_WRITE_ACC_BIAS_BIN 19
#define MICROSTRAIN_WRITE_GYR_BIAS_BIN 19
#define MICROSTRAIN_ACC_RAT_MAG_BIN 43
#define MICROSTRAIN_ACC_RAT_MAG_ORI_BIN 79
#define MICROSTRAIN_INIT_GYRO_BIAS_BIN 19
#define MICROSTRAIN_EULER_ANG_BIN 19
#define MICROSTRAIN_EULER_ANG_RAT_BIN 31
#define MICROSTRAIN_WRITE_NVM_BIN 9
#define MICROSTRAIN_TEMP_BIN 15
#define MICROSTRAIN_STAB_ACC_RAT_MAG_BIN 43
#define MICROSTRAIN_DELTA_ANG_VEL_MAG_BIN 43
#define MICROSTRAIN_WRITE_EEPROM_BIN 5
#define MICROSTRAIN_READ_EEPROM_BIN 5
#define MICROSTRAIN_FIRMWARE_BIN 7
#define MICROSTRAIN_DEVICEID_BIN 20
#define MICROSTRAIN_STOP_CONTINUOUS_BIN 0

#define MICROSTRAIN_MODEL_NUMBER 0x00
#define MICROSTRAIN_SERIAL_NUMBER 0x01
#define MICROSTRAIN_MODEL_NAME 0x02
#define MICROSTRAIN_DEVICE_OPTIONS 0x03

#define MICROSTRAIN_MAX_MESSAGE 100
#define MICROSTRAIN_G 9.80665

//! @}

//! \ingroup microstrain
//! \defgroup microstrain_typedefs MicroStrain IMU typedefs
//! @{

//! IMU state
/*! Internal structure storing the state and device information for a connected IMU.
*/
typedef struct
	{
	//! Serial device
	char *dev;
	//! cssl handle
	cssl_t *handle;
	//! Data
	uint8_t data[MICROSTRAIN_MAX_MESSAGE];
	//! Size of data
	uint16_t size;
	//! checksum
	uint16_t checksum;
	//! timer
	uint32_t timer;
	} microstrain_state;

//! @}

//! \ingroup microstrain
//! \defgroup microstrain_functions MicroStrain IMU handling functions
//! @{

int microstrain_connect(const char *dev);
int microstrain_disconnect(int handle);
int microstrain_send_message(int handle, uint16_t rsize, uint16_t rtime);
uint16_t microstrain_calc_checksum(uint8_t *buf, uint16_t size);
int microstrain_set_size(int handle, uint16_t size);
int32_t microstrain_get_timer(int handle);
uint16_t microstrain_get_checksum(int handle);
int microstrain_get_size(int handle);
uint8_t *microstrain_get_data(int handle);
int microstrain_raw_accel_rate(int handle, rvector *accel, rvector *rate);
int microstrain_accel_rate(int handle, rvector *accel, rvector *rate);
int microstrain_delta_angle_geocv(int handle, rvector *deltaa, rvector *deltav);
int microstrain_orientation(int handle, rmatrix *matrix);
int microstrain_orientation_update(int handle, rmatrix *matrix);
int microstrain_magfield(int handle, rvector *field);
//int microstrain_gyro(int handle, rvector *gyro);
int microstrain_accel_rate_magfield_orientation(int handle, rvector *accel, rvector *rate, rvector *field, rmatrix *matrix);
int microstrain_accel_rate_magfield(int handle, rvector *accel, rvector *rate, rvector *field);
int microstrain_stab_accel_rate_magfield(int handle, rvector *accel, rvector *rate, rvector *field);
int microstrain_capture_gyro_bias(int handle, uint16_t msec, rvector *bias);
int microstrain_euler_angles(int handle, avector *euler);
int microstrain_temperature(int handle, double *tempa, double *tempgx, double *tempgy, double *tempgz);
int microstrain_device_identifier(int handle, uint8_t selector, char *buf);

//! @}

#ifdef __cplusplus
}
#endif

#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)

#endif // _MICROSTRAIN_H
