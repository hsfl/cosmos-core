/**
 * * Contains definitions and functions for communicating with
 * * a MixW TNC in KISS mode (where TNC auto-generates FCS!).
 * * @file kisstnc_lib.h
 * * @brief KISSTNC controller includes
 * */

#ifndef _KISSTNCLIB_H
#define _KISSTNCLIB_H 1


#include "configCosmos.h"

#include <stdio.h>
#include <errno.h>

#include "cssl_lib.h"
#include "cosmos-errno.h"

#define KISSTNC_MTU 254
#define KISSTNC_PAYLOAD_SIZE KISSTNC_MTU

//#define KISSTNC_PAYLOAD_SIZE 255
#define KISSTNC_FULL_SIZE (KISSTNC_PAYLOAD_SIZE+19)
#define AX25_FLAG 0x7E
#define AX25_NOLAYER3 0xF0

#define KISSTNC_ID 1
#define KISSTNC_BAUD 19200
#define KISSTNC_BITS 8
#define KISSTNC_PARITY 0
#define KISSTNC_STOPBITS 1

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
            uint8_t payload[KISSTNC_PAYLOAD_SIZE+2];
		} part;
        uint8_t full[KISSTNC_FULL_SIZE];
	};
	uint16_t size;
} kisstnc_frame;

typedef struct
{
	cssl_t *serial;
    kisstnc_frame frame;
	uint8_t flag;
} kisstnc_handle;

int32_t kisstnc_connect(char *dev, kisstnc_handle *handle, uint8_t flag);
int32_t kisstnc_disconnect(kisstnc_handle *handle);
int32_t kisstnc_recvframe(kisstnc_handle *handle);
int32_t kisstnc_sendframe(kisstnc_handle *handle);
int32_t kisstnc_loadframe(kisstnc_handle *handle, uint8_t *data, uint16_t size);
int32_t kisstnc_loadframe(kisstnc_handle *handle, vector<uint8_t> data);
int32_t kisstnc_unloadframe(kisstnc_handle *handle, uint8_t *data, uint16_t size);
int32_t kisstnc_unloadframe(kisstnc_handle *handle, vector<uint8_t> &data);
uint16_t kisstnc_calc_fcs(uint8_t *data, uint16_t length);
int32_t kisstnc_exitkiss(kisstnc_handle *handle);
int32_t kisstnc_enterkiss(kisstnc_handle *handle);


#endif
