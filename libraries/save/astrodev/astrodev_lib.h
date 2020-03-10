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

/**
* Contains definitions and functions for communicating with
* an Astrodev radio.
* @file astrodev_lib.h
* @brief ASTRODEV controller includes
*/

//! \ingroup devices
//! \defgroup astrodev Astrodev L1 radio support

#ifndef _ASTRODEVLIB_H
#define _ASTRODEVLIB_H 1

#include "support/configCosmos.h"

#include "device/general/cssl_lib.h"
#include "support/cosmos-errno.h"

//! \ingroup astrodev
//! \defgroup astrodev_defines Astrodev library definitions
//! @{
#define ASTRODEV_HEADER_SIZE 8
#define ASTRODEV_PAYLOAD_TRAILER_SIZE 2
#define ASTRODEV_OVERHEAD_SIZE_NO_PAYLOAD (ASTRODEV_HEADER_SIZE)
#define ASTRODEV_OVERHEAD_SIZE_WITH_PAYLOAD (ASTRODEV_HEADER_SIZE + ASTRODEV_PAYLOAD_TRAILER_SIZE)

#define ASTRODEV_SYNC0 'H'
#define ASTRODEV_SYNC1 'e'

#define ASTRODEV_COMMAND 0x10
#define ASTRODEV_RESPONSE 0x20

#define ASTRODEV_NOOP 0x01
#define ASTRODEV_RESET 0x02
#define ASTRODEV_TRANSMIT 0x03
#define ASTRODEV_RECEIVE 0x04
#define ASTRODEV_GETTCVCONFIG 0x05
#define ASTRODEV_SETTCVCONFIG 0x06
#define ASTRODEV_TELEMETRY 0x07
#define ASTRODEV_FLASH 0x08
#define ASTRODEV_RFCONFIG 0x09
#define ASTRODEV_BEACONDATA 0x10
#define ASTRODEV_BEACONCONFIG 0x11
#define ASTRODEV_FIRMWAREREV 0x12
#define ASTRODEV_DIOKEY 0x13
#define ASTRODEV_FIRMWAREUPDATE 0x14
#define ASTRODEV_FIRMWAREPACKET 0x15
#define ASTRODEV_FASTSETPA 0x20

#define ASTRODEV_16BYTE_OA_KEY "ABCDEFGHIJKLMNOP"

#define ASTRODEV_RF_BAUD(rate) (rate?4800<<rate:1200)
#define ASTRODEV_UART_BAUD(rate) (9600<<rate)

enum
	{
	ASTRODEV_BAUD_RATE_9600,
	ASTRODEV_BAUD_RATE_19200,
	ASTRODEV_BAUD_RATE_38400,
	ASTRODEV_BAUD_RATE_76800,
	ASTRODEV_BAUD_RATE_115200
	};

enum
	{
	ASTRODEV_RF_BAUD_RATE_1200,
	ASTRODEV_RF_BAUD_RATE_9600,
	ASTRODEV_RF_BAUD_RATE_19200,
	ASTRODEV_RF_BAUD_RATE_38400
	};

enum
	{
	ASTRODEV_MODULATION_GFSK,
	ASTRODEV_MODULATION_AFSK,
	ASTRODEV_MODULATION_BPSK
	};

// OA Commands
#define ASTRODEV_TELEMETRY_DUMP_COMMAND 0x30
#define ASTRODEV_PING_RETURN_COMMAND 0x31
#define ASTRODEV_CODE_UPLOAD_COMMAND 0x32
#define ASTRODEV_RADIO_RESET_COMMAND 0x33
#define ASTRODEV_PIN_TOGGLE_COMMAND 0x34

#define ASTRODEV_ID 1
#define ASTRODEV_BAUD 9600
#define ASTRODEV_BITS 8
#define ASTRODEV_PARITY 0
#define ASTRODEV_STOPBITS 1

#define ASTRODEV_MTU 254

//! @}

//! \ingroup astrodev
//! \defgroup astrodev_typedefs Astrodev library typedefs
//! @{
/**
* Internal structure containing an astrodev frame.
* @brief ASTRODEV state
*/

typedef struct
{
    // Nybble 1
    uint16_t ext_event_pin12_functions:2;   // 0: Off, Logic Low / 1: 2.5s Toggle / 2: TX Packet Toggle (12ms) / 3: Rx Packet Toggle (1.25ms)
    uint16_t config2_pin13_functions:2;     // 0: Off, Logic Low / 1: Tx/Rx Switch / 2: 2.5Hz WDT / 3: Rx Packet Toggle (1.25ms)

    // Nybble 2
    uint16_t config1_pin14_dio_enable:1;    // 0: CONFIG1 Off, Logic Low  /  1: CONFIG1 Digital IO Over the Air Key Enable
    uint16_t config1_pin14_dio_pattern:1;   // 0: CONFIG1 Pattern A, Latch High  /  1: CONFIG1 Pattern B, Toggle, 72ms High
    uint16_t rx_crc_enable:1;               // RX CRC Enable 1/Disable 0
    uint16_t tx_crc_enable:1;               // TBD // TX CRC Enable 1/Disable 0

    // Nybble 3
    uint16_t tlm_packet_logging_enable:1;   // Telemetry Packet Logging Enable 1/Disable 0
    uint16_t tlm_packet_logging_rate:2;     // Logging Rate 0 1/10 Hz, 1 1 Hz, 2 2 Hz,3 4 Hz
    uint16_t tlm_dump_enable:1;             // Telemetry Dump Enable 1/Disable 0

    // Nybble 4
    uint16_t oa_commands_enable:1;          // Enable OA Commands Enable 1/Disable 0
    uint16_t code_upload_enable:1;          // Code Upload Enable 1/Disable 0
    uint16_t radio_reset_enable:1;          // Radio Reset Enable 1/Disable 0
    uint16_t factory_defaults_restored:1;   // Flag: Factory settings restore complete
} astrodev_function_config1;

typedef struct
{
    // Nybble 1
    uint16_t rx_afc_enable:1;	// Receiver Automatic Frequency Control On/Off
    uint16_t test_mode_rx_cw:1;	// RX CW (CAUTION TEST MODE), Set to 0 for normal operation
    uint16_t test_mode_tx_cw:1;	// TX CW (CAUTION TEST MODE), Set to 0 for normal operation
    uint16_t test_mode_tbd:1;	// TBD (CAUTION TEST MODE), Set to 0 for normal operation

    // Nybble 2-4
    uint16_t zeros:4;		// Set to 0
} astrodev_function_config2;


typedef struct
{
	uint8_t interface_baud_rate;
	uint8_t power_amp_level;
	uint8_t rx_baud_rate;
	uint8_t tx_baud_rate;
	uint8_t rx_modulation;
	uint8_t tx_modulation;
	uint16_t rx_freq_low;
	uint16_t rx_freq_high;
	uint16_t tx_freq_low;
	uint16_t tx_freq_high;
	uint8_t ax25_source[6];
	uint8_t ax25_destination[6];
	uint16_t ax25_preamble_length;
	uint16_t ax25_postamble_length;
	uint16_t function_config;
	uint16_t function_config2;
	uint16_t cs;
#ifdef COSMOS_WIN_BUILD_MSVC
} astrodev_tcv_config;
#else
} __attribute__((__may_alias__)) astrodev_tcv_config;
#endif

typedef struct
{
	uint8_t interface_baud_rate;
	uint8_t power_amp_level;
	uint8_t rx_baud_rate;
	uint8_t tx_baud_rate;
	uint8_t rx_modulation;
	uint8_t tx_modulation;
	uint16_t rx_freq_low;
	uint16_t rx_freq_high;
	uint16_t tx_freq_low;
	uint16_t tx_freq_high;
	uint8_t ax25_source[6];
	uint8_t ax25_destination[6];
	uint16_t ax25_preamble_length;
	uint16_t ax25_postamble_length;
        union {
            astrodev_function_config1 function_config;   // Radio Configuration Discrete Behaviors
            uint16_t function_config_raw;
        };
        union {
            astrodev_function_config2 function_config2;  // Radio Configuration Discrete Behaviors #2
            uint16_t function_config2_raw;
        };
	uint16_t cs;
} astrodev_tcv_config_jc;

typedef struct
{
	uint8_t front_end_level;
	uint8_t power_amp_level;
	uint16_t tx_freq;
	uint16_t rx_freq;
	uint16_t cs;
#ifdef COSMOS_WIN_BUILD_MSVC
} astrodev_rf_config;
#else
} __attribute__((__may_alias__)) astrodev_rf_config;
#endif

typedef struct
{
	float rev;
	uint16_t cs;
} astrodev_firmware;

typedef struct
{
	uint16_t op_counter;
	int16_t msp430_temp;
	uint8_t time_rssi[4];
	uint32_t bytes_rx;
	uint32_t bytes_tx;
	uint16_t cs;
#ifdef COSMOS_WIN_BUILD_MSVC
} astrodev_telem;
#else
} __attribute__((__may_alias__)) astrodev_telem;
#endif

typedef struct
{
	uint16_t op_counter;    // uint_2 # operations
	int16_t msp430_temp;    // sint_2 Temperature in C
	uint8_t time_count[3];  // uint_1x3 Unknown
	uint8_t rssi;           // uint_1 rssi
	uint16_t bytes_rx_lo;      // uint_4 bytesRX
	uint16_t bytes_rx_hi;      // uint_4 bytesRX
	uint16_t bytes_tx_lo;      // uint_4 bytesTX
	uint16_t bytes_tx_hi;      // uint_4 bytesTX
	uint16_t cs;
} astrodev_telem_jc;


typedef struct
{
    uint8_t beacon_interval; // 0 is off, 2.5 sec delay per LSB
    uint16_t cs;
} astrodev_beacon_config;

typedef struct
{
	union
	{
		struct
		{
			uint8_t sync0;
			uint8_t sync1;
			uint8_t type;
			uint8_t command;
			uint16_t size;
			uint16_t cs;
		} header;
		uint8_t preamble[8];
	};
	union
	{
		uint8_t payload[ASTRODEV_MTU+20];
		astrodev_tcv_config tcv;
		astrodev_rf_config rf;
		astrodev_firmware firmware;
		astrodev_telem telemetry;
	};
#ifdef COSMOS_WIN_BUILD_MSVC
} astrodev_frame;
#else
} __attribute__((__may_alias__)) astrodev_frame;
#endif

typedef struct
{
	cssl_t *serial;
	astrodev_frame frame;
} astrodev_handle;

//! @}

//! \ingroup astrodev
//! \defgroup astrodev_functions Astrodev library function declarations
//! @{

int32_t astrodev_connect(string dev, astrodev_handle &handle);
int32_t astrodev_disconnect(astrodev_handle &handle);
int32_t astrodev_recvframe(astrodev_handle &handle);
int32_t astrodev_sendframe(astrodev_handle &handle);
int32_t astrodev_gettcvconfig(astrodev_handle &handle);
int32_t astrodev_settcvconfig(astrodev_handle &handle);
uint16_t astrodev_calc_cs(uint8_t *data, uint16_t size);
int32_t astrodev_setupframe(astrodev_frame *frame);
uint16_t astrodev_loadframe(astrodev_handle &handle, uint8_t *data, uint16_t size);
int32_t astrodev_unloadframe(astrodev_handle &handle, uint8_t *data, uint16_t size);
int32_t astrodev_checkframe(astrodev_frame* frame);
int32_t astrodev_ping(astrodev_handle &handle);
int32_t astrodev_rfconfig(astrodev_handle &handle);
int32_t astrodev_firmwarerev(astrodev_handle &handle);
int32_t astrodev_telemetry(astrodev_handle &handle);
int32_t astrodev_transmit(astrodev_handle &handle, uint8_t *data, uint16_t size);
int32_t astrodev_receive(astrodev_handle &handle, uint8_t *data, uint16_t size);

//! @}
#endif
