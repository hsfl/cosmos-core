/**
 * * Contains definitions and functions for communicating with
 * * a Kantronix KPC-9612P TNC in KISS mode.
 * * @file kpc9612p_lib.h
 * * @brief KPC-9612P controller includes
 * */

#ifndef _KPC9612PLIB_H
#define _KPC9612PLIB_H 1


#include "configCosmos.h"

#include <stdio.h>
#include <errno.h>

#include "cssl_lib.h"
#include "cosmos-errno.h"

#define KPC9612P_MTU 254
#define KPC9612P_PAYLOAD_SIZE KPC9612P_MTU

//#define KPC9612P_PAYLOAD_SIZE 255
#define KPC9612P_FULL_SIZE (KPC9612P_PAYLOAD_SIZE+19)
#define AX25_FLAG 0x7E
#define AX25_NOLAYER3 0xF0

#define KPC9612P_ID 1
#define KPC9612P_BAUD 19200
#define KPC9612P_BITS 8
#define KPC9612P_PARITY 0
#define KPC9612P_STOPBITS 1

typedef struct
{
	union
	{
		struct
		{
			union
			{
				struct
				{
					uint8_t flag;
					char destination_address[7];
					char source_address[7];
					uint8_t control;
					uint8_t protocol_id;
				} header;
				uint8_t preamble[17];
			};
			uint8_t payload[KPC9612P_PAYLOAD_SIZE+2];
		} part;
		uint8_t full[KPC9612P_FULL_SIZE];
	};
	uint16_t size;
} kpc9612p_frame;

typedef struct
{
	cssl_t *serial;
	kpc9612p_frame frame;
	uint8_t flag;
} kpc9612p_handle;

int32_t kpc9612p_connect(char *dev, kpc9612p_handle *handle, uint8_t flag);
int32_t kpc9612p_disconnect(kpc9612p_handle *handle);
int32_t kpc9612p_recvframe(kpc9612p_handle *handle);
int32_t kpc9612p_sendframe(kpc9612p_handle *handle);
int32_t kpc9612p_loadframe(kpc9612p_handle *handle, uint8_t *data, uint16_t size);
int32_t kpc9612p_loadframe(kpc9612p_handle *handle, vector<uint8_t> data);
int32_t kpc9612p_unloadframe(kpc9612p_handle *handle, uint8_t *data, uint16_t size);
int32_t kpc9612p_unloadframe(kpc9612p_handle *handle, vector<uint8_t> &data);
uint16_t kpc9612p_calc_fcs(uint8_t *data, uint16_t length);
int32_t kpc9612p_exitkiss(kpc9612p_handle *handle);
int32_t kpc9612p_enterkiss(kpc9612p_handle *handle);


#endif
