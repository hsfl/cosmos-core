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

/*! \file sinclair_lib.h
	\brief sinclair_lib include file
	A library providing control routines for Sinclair Interplanetary devices.
*/
//! \ingroup devices
//! \defgroup sinclair Sinclair Interplanetary device library
//! Sinclair Interplanetary Products device library

#include "configCosmos.h"
#if defined(COSMOS_LINUX_OS) || defined(COSMOS_CYGWIN_OS) || defined(COSMOS_MAC_OS) || defined(COSMOS_WIN_OS)

#ifndef _SINCLAIR_H
#define _SINCLAIR_H 1

#include "configCosmos.h"

#include "math/mathlib.h"
#include "cssl_lib.h"
#include "cosmos-errno.h"

#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <cstring>

//! \ingroup sinclair
//! \defgroup sinclair_constants Sinclair reaction wheel constants
//! @{

#define SINCLAIR_MAXCOUNT 5

#define SINCLAIR_ID 10
#define SINCLAIRRW_BAUD 57600
#define SINCLAIRSTT_BAUD 115200
#define SINCLAIR_BITS 8
#define SINCLAIR_PARITY 0
#define SINCLAIR_STOPBITS 1

#define SINCLAIR_MODE_IDLE 0x00
#define SINCLAIR_MODE_DAC 0x01
#define SINCLAIR_MODE_CURRENT 0x02
#define SINCLAIR_MODE_POWER 0x03
#define SINCLAIR_MODE_SPEED 0x05
#define SINCLAIR_MODE_ACCEL 0x14
#define SINCLAIR_MODE_MOMENTUM 0x15
#define SINCLAIR_MODE_TORQUE 0x16

#define SINCLAIR_PARAM_VOLTAGE 0x01
#define SINCLAIR_PARAM_CURRENT 0x02
#define SINCLAIR_PARAM_TEMPERATURE 0x03
#define SINCLAIR_PARAM_DAC 0x04
#define SINCLAIR_PARAM_SPEED 0x05
#define SINCLAIR_PARAM_SPEED_P 0x06
#define SINCLAIR_PARAM_SPEED_I 0x07
#define SINCLAIR_PARAM_SPEED_D 0x08
#define SINCLAIR_PARAM_SPEED_INTEGRATOR 0x09
#define SINCLAIR_PARAM_ACCEL 0x12
#define SINCLAIR_PARAM_INERTIA 0x1a

// **************************************************************************
// SLIP Byte Definitions - Modified 20140610JC, reduces conflicts with other SLIP libraries
// **************************************************************************
//! SLIP Buffer End character
#ifndef SLIP_FEND
#define SLIP_FEND 0xC0
#endif
//! SLIP Buffer Escape character
#ifndef SLIP_FESC
#define SLIP_FESC 0xDB
#endif
//! SLIP Buffer Escaped End character
#ifndef SLIP_TFEND
#define SLIP_TFEND 0xDC
#endif
//! SLIP Buffer Escaped Escape character
#ifndef SLIP_TFESC
#define SLIP_TFESC 0xDD
#endif

#define SLIP_MAXBUF 500

//! Sinclair Star Tracker Operational Return Code
#define STT_RETURN_CODE_IMAGE1_QUALITY 0x01
#define STT_RETURN_CODE_IMAGE2_QUALITY 0x02
#define STT_RETURN_CODE_IMAGE1_SUCCESS 0x04
#define STT_RETURN_CODE_IMAGE2_SUCCESS 0x08
#define STT_RETURN_CODE_FULL_PROCESSING 0x10
#define STT_RETURN_CODE_DETECTOR_IMAGE 0x20
#define STT_RETURN_CODE_CONSISTENT_SOLUTION 0x40

// Operational Results
#define STT_RETURN_CODE (0x4)
#define STT_ATT_OFFSET (0x8)
#define STT_VEL_OFFSET (0x28)
#define STT_EPOCH_TIME (0x40)
#define STT_HARDWARE_TELEMETRY_OFFSET (0x48)

// Defines for Hardware Telemetery Structure
#define STT_DETTEMP_OFFSET (0xC)
#define STT_PROCTEMP_OFFSET (0xE)
#define STT_STATUS_BITFIELD (0x10)
#define STT_VDDCORE_SETPOINT_OFFSET (0x14)
#define STT_MPU_SETPOINT_OFFSET (0x15)
#define STT_DARKOFFSETS_OFFSET (0x18)

// Star Tracker Go Codes
#define STT_GO_ON 0x01
#define STT_GO_LOAD_NAND 0x02
#define STT_GO_NOT_OFF 0x04
#define STT_GO_COMMAND 0x08
#define STT_GO_TEST 0x10
#define STT_GO_NO_RESET 0x20

// Star Tracker Combination Bitfield
//! Sequence Number
#define STT_COMBO_SEQ_NUM 0x0001
#define STT_COMBO_RET_COD 0x0002
#define STT_COMBO_ATT_QUA 0x0004
#define STT_COMBO_ANG_VEL 0x0008
#define STT_COMBO_EPO_TIM 0x0010
#define STT_COMBO_HAR_TEL 0x0020
#define STT_COMBO_STA_TEL 0x0040
#define STT_COMBO_IMA_TEL 0x0080
#define STT_COMBO_ERS_TEL 0x0100
#define STT_COMBO_CEN_TEL 0x0200
#define STT_COMBO_MAT_TEL 0x0400
#define STT_COMBO_TES_RES 0x0800

//! Sinclair NSP Ping command byte
#define NSP_COMMAND_PING 0x00
//! Sinclair NSP Init command byte
#define NSP_COMMAND_INIT 0x01
//! Sinclair NSP Peek command byte
#define NSP_COMMAND_PEEK 0x02
//! Sinclair NSP Poke command byte
#define NSP_COMMAND_POKE 0x03
//! Sinclair NSP Telemetry command byte
#define NSP_COMMAND_TELEMETRY 0x04
//! Sinclair NSP Calculate CRC command byte
#define NSP_COMMAND_CRC 0x06
//! Sinclair NSP Application-Telemetry command byte
#define NSP_COMMAND_APP_TELEMETRY 0x07
//! Sinclair NSP Application-Command command byte
#define NSP_COMMAND_APP_COMMAND 0x08

//! Sinclair NSP Star Tracker READ EDAC command byte
#define NSP_COMMAND_READ_EDAC 0x09
//! Sinclair NSP Star Tracker WRITE EDAC command byte
#define NSP_COMMAND_WRITE_EDAC 0x0A
//! Sinclair NSP Star Tracker "GO" command byte
#define NSP_COMMAND_GO 0x0B
//! Sinclair NSP Star Tracker READ RESULT command byte
#define NSP_COMMAND_READ_RESULT 0x0D
//! Sinclair NSP Star Tracker IMAGE command byte
#define NSP_COMMAND_READ_IMAGE 0x10
//! Sinclair NSP Star Tracker COMBINATION command byte
#define NSP_COMMAND_COMBINATION 0x12


//! Sinclair NSP message control A (ACK) bit
#define NSP_MCB_A 0x20
//! Sinclair NSP message control B bit
#define NSP_MCB_B 0x40
//! Sinclair NSP message control Poll bit
#define NSP_MCB_PF 0x80

#define NSP_MAX_MESSAGE 2048

//! @}

//! \ingroup sinclair
//! \defgroup sinclair_typedefs Sinclair reaction wheel typedefs
//! @{

//! NSP Message
/*! Structure representing an NSP message.
*/
typedef struct
{
	//! Destination Address
	uint8_t dst;
	//! Source Address
	uint8_t src;
	//! Message Control Field
	uint8_t mcf;
	//! Data
	uint8_t data[NSP_MAX_MESSAGE+1];
	//! Size of data
	uint16_t size;
	//! CRC
	uint16_t crc;
#ifdef COSMOS_WIN_BUILD_MSVC
} nsp_message;
#else
} __attribute__((__may_alias__)) nsp_message;
#endif

//! reaction wheel state
/*! Internal structure storing the state and device information for a connected reaction wheel.
*/
typedef struct
{
	//! Serial device
	//	char *dev;
	string dev;
	cssl_t *serial;
	nsp_message mbuf;
	double lmjd;
} sinclair_state;

//! Star Tracker status
/*! Structure for storing the state of the star tracker as read from
EDAC memory.
*/

typedef struct
{
	uint8_t junk1[12];
	int16_t detector_temperature;
	uint16_t functional_temperature;
	uint32_t status;
	uint8_t vdd_core;
	uint8_t vdd_mpu;
	uint16_t junk2;
	uint16_t dark_offsets[16];
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_hardware_telemetry;
#else
} __attribute__((__may_alias__)) sinclair_stt_hardware_telemetry;
#endif

typedef struct
{
	uint16_t image1_good_rows;
	uint16_t image2_good_rows;
	float image1_mean[8];
	float image2_mean[8];
	float image1_variation[8];
	float image2_variation[8];
	float temporal_variation[8];
	uint32_t junk1;
	uint32_t junk2;
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_statistics_telemetry;
#else
} __attribute__((__may_alias__)) sinclair_stt_statistics_telemetry;
#endif

typedef struct
{
	int32_t x;
	int32_t y;
	int32_t intensity;
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_peak_entry;
#else
} __attribute__((__may_alias__)) sinclair_stt_peak_entry;
#endif

typedef struct
{
	uint32_t initialization_flag;
	int32_t return_code;
	int32_t lit_pixel_count;
	uint32_t peak_count;
	double exposure_error;
	double capture_time;
	sinclair_stt_peak_entry peaks[30];
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_image_telemetry;
#else
} __attribute__((__may_alias__)) sinclair_stt_image_telemetry;
#endif

typedef struct
{
	uint32_t initialization_flag;
	int32_t return_code;
	int32_t star_mapping[10];
	double fit_residual;
	double angular_velocity_covariance[6];
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_ers_telemetry;
#else
} __attribute__((__may_alias__)) sinclair_stt_ers_telemetry;
#endif

typedef struct
{
	uint32_t initialization_flag;
	int32_t return_code;
	int32_t good_start_count;
	uint32_t junk1;
	double best_centroids[10][2];
	double best_star_vectors[10][3];
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_centroid_telemetry;
#else
} __attribute__((__may_alias__)) sinclair_stt_centroid_telemetry;
#endif

typedef struct
{
	uint32_t initialization_flag;
	int32_t return_code;
	uint32_t triangle_test_count;
	uint32_t junk1;
	double matching_error;
	double confidence_index;
	int32_t total_star_tests;
	int32_t total_set_tests;
	int32_t conset_size;
	uint32_t junk2;
	double conset_rms_error;
	int32_t matched_starts[10];
	quaternion attitude;
	double angular_velocity_covariance[6];
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_matching_telemetry;
#else
} __attribute__((__may_alias__)) sinclair_stt_matching_telemetry;
#endif

typedef struct
{
	float current_sense;
	float bus_voltage;
	float vdd_core;
	float vdd_mpu;
	float vdd_io;
	float supervisor_temperature;
	float vdd_supervisor;
	float vdd_detector;
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_analog_frame;
#else
} __attribute__((__may_alias__)) sinclair_stt_analog_frame;
#endif

typedef struct
{
	uint32_t sequence_number;
	uint32_t return_code;
	quaternion attitude;
	rvector omega;
	double epoch;
	sinclair_stt_hardware_telemetry htelem;
	sinclair_stt_statistics_telemetry stelem;
	uint32_t junk1;
	sinclair_stt_image_telemetry itelem[2];
	sinclair_stt_ers_telemetry ers;
	sinclair_stt_centroid_telemetry ctelem[2];
	sinclair_stt_matching_telemetry mtelem[2];
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_result_operational;
#else
} __attribute__((__may_alias__)) sinclair_stt_result_operational;
#endif

typedef struct
{
	sinclair_stt_analog_frame step1;
	sinclair_stt_analog_frame step2;
	sinclair_stt_analog_frame step3;
	sinclair_stt_analog_frame step4;
	sinclair_stt_analog_frame step5;
	sinclair_stt_analog_frame step6;
	sinclair_stt_analog_frame step7;
	sinclair_stt_analog_frame step9;
	sinclair_stt_analog_frame step10;
	sinclair_stt_analog_frame step11;
	int32_t first_test;
	int32_t second_test;
	sinclair_stt_analog_frame step12;
	sinclair_stt_hardware_telemetry htelem;
	sinclair_stt_analog_frame step13;
	sinclair_stt_statistics_telemetry stelem;
	sinclair_stt_analog_frame step14;
#ifdef COSMOS_WIN_BUILD_MSVC
} sinclair_stt_result_selftest;
#else
} __attribute__((__may_alias__)) sinclair_stt_result_selftest;
#endif

typedef union
{
	sinclair_stt_result_operational op;
	sinclair_stt_result_selftest test;
} sinclair_stt_result;
//! @}

//! \ingroup sinclair
//! \defgroup sinclair_functions Sinclair reaction wheel handling functions
//! @{

int32_t sinclair_stt_connect(const char *dev, uint8_t src, uint8_t dst, sinclair_state *handle);
int32_t sinclair_rw_connect(const char *dev, uint8_t src, uint8_t dst, sinclair_state *handle);
int sinclair_disconnect(sinclair_state *handle);
int32_t sinclair_readslip(sinclair_state *handle, uint8_t *buf, int size);
int sinclair_writeslip(sinclair_state *handle, uint8_t *buf, int size);
int sinclair_mode_idle(sinclair_state *handle);
int sinclair_mode_dac(sinclair_state *handle, float value);
int sinclair_mode_current(sinclair_state *handle, float amps);
int sinclair_mode_power(sinclair_state *handle, float watts);
int sinclair_mode_speed(sinclair_state *handle, float speed);
int sinclair_mode_accel(sinclair_state *handle, float accel);
int sinclair_mode_momentum(sinclair_state *handle, float momentum);
int sinclair_mode_torque(sinclair_state *handle, float torque);
float sinclair_get_voltage(sinclair_state *handle);
float sinclair_get_speed(sinclair_state *handle);
float sinclair_get_accel(sinclair_state *handle);
float sinclair_get_inertia(sinclair_state *handle);
int nsp_send_message(sinclair_state *handle);
uint16_t nsp_calc_crc(uint8_t *buf, int size);
int nsp_set_dst(sinclair_state *handle, uint8_t address);
int nsp_set_src(sinclair_state *handle, uint8_t address);
int nsp_set_mcf(sinclair_state *handle, uint8_t mcf);
int nsp_set_crc(sinclair_state *handle, uint16_t crc);
int nsp_set_size(sinclair_state *handle, uint16_t size);
int nsp_get_dst(sinclair_state *handle);
int nsp_get_src(sinclair_state *handle);
int nsp_get_mcf(sinclair_state *handle);
int nsp_get_crc(sinclair_state *handle);
int nsp_get_size(sinclair_state *handle);
uint8_t *nsp_get_data(sinclair_state *handle);
int nsp_ping(sinclair_state *handle);
int nsp_init(sinclair_state *handle, uint32_t address);
int nsp_app_command_mode(sinclair_state *handle, uint8_t mode, float value);
int nsp_app_command_parameter(sinclair_state *handle, uint8_t parameter, float value);
int nsp_app_telemetry_mode(sinclair_state *handle, uint8_t *mode, float *value);
int nsp_app_telemetry_parameter(sinclair_state *handle, uint8_t *parameter, float *value);

int nsp_stt_go(sinclair_state *handle, char *buf, uint8_t command);
int nsp_stt_read_edac_result_length(sinclair_state *handle, char *buf);
int32_t nsp_stt_read_edac_asynchronous(sinclair_state *handle, sinclair_stt_analog_frame *data);
int nsp_stt_read_result(sinclair_state *handle, char *buf, uint16_t addr_2_read, uint16_t num_2_read);
int nsp_stt_combination(sinclair_state *handle, sinclair_stt_result_operational *result, uint8_t go_command, uint32_t bitfield);
int32_t sinclair_stt_combo(sinclair_state *handle, sinclair_stt_result_operational *result);
int nsp_stt_combinationDiag(sinclair_state *handle, char *buf_out);

//! @}

#endif

#endif // define(COSMOS_MAC_OS) || define(COSMOS_LINUX_OS)
