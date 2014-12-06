/**
 * * Contains definitions and functions for communicating with
 * * a MixW TNC in KISS mode (where TNC auto-generates FCS!).
 * * @file mixwtnc_lib.h
 * * @brief MIXWTNC controller includes
 * */

#ifndef _MIXWTNCLIB_H
#define _MIXWTNCLIB_H 1


#include "configCosmos.h"

#include <stdio.h>
#include <errno.h>

#include "cssl_lib.h"
#include "cosmos-errno.h"

#define MIXWTNC_MTU 254
#define MIXWTNC_PAYLOAD_SIZE MIXWTNC_MTU

//#define MIXWTNC_PAYLOAD_SIZE 255
#define MIXWTNC_FULL_SIZE (MIXWTNC_PAYLOAD_SIZE+19)
#define AX25_FLAG 0x7E
#define AX25_NOLAYER3 0xF0

#define MIXWTNC_ID 1
#define MIXWTNC_BAUD 19200
#define MIXWTNC_BITS 8
#define MIXWTNC_PARITY 0
#define MIXWTNC_STOPBITS 1

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
            uint8_t payload[MIXWTNC_PAYLOAD_SIZE+2];
		} part;
        uint8_t full[MIXWTNC_FULL_SIZE];
	};
	uint16_t size;
} mixwtnc_frame;

typedef struct
{
	cssl_t *serial;
    mixwtnc_frame frame;
	uint8_t flag;
} mixwtnc_handle;

int32_t mixwtnc_connect(char *dev, mixwtnc_handle *handle, uint8_t flag);
int32_t mixwtnc_disconnect(mixwtnc_handle *handle);
int32_t mixwtnc_recvframe(mixwtnc_handle *handle);
int32_t mixwtnc_sendframe(mixwtnc_handle *handle);
int32_t mixwtnc_loadframe(mixwtnc_handle *handle, uint8_t *data, uint16_t size);
int32_t mixwtnc_loadframe(mixwtnc_handle *handle, vector<uint8_t> data);
int32_t mixwtnc_unloadframe(mixwtnc_handle *handle, uint8_t *data, uint16_t size);
int32_t mixwtnc_unloadframe(mixwtnc_handle *handle, vector<uint8_t>& data);
int32_t mixwtnc_unloadframe(mixwtnc_frame *frame, vector<uint8_t>& data);
uint16_t mixwtnc_calc_fcs(uint8_t *data, uint16_t length);
int32_t mixwtnc_exitkiss(mixwtnc_handle *handle);
int32_t mixwtnc_enterkiss(mixwtnc_handle *handle);


#endif
