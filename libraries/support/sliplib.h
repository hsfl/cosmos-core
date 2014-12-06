#ifndef _SLIPLIB_H
#define _SLIPLIB_H

/*!	\file sliplib.h
	\brief COSMOS SLIP support include file
*/

//! \ingroup support
//! \defgroup sliplib COSMOS SLIP encoding library
//! A library providing support functions for the COSMOS specific SLIP protocol.


#include "configCosmos.h"

#include "cosmos-errno.h"
#include <cstring>

//! \ingroup sliplib
//! \defgroup sliplib_constants COSMOS SLIP support constants
//! @{

// **************************************************************************
// SLIP Byte Definitions - Added 20140610 - To prevent errors with other SLIP Libs
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

//! CRC-16-CCITT Normal
#define CRC16CCITT 0x1021
//! CRC-16-CCITT Reversed
#define CRC16CCITTR 0x8408
//! CRC-16-CCITT Reversed Reciprocal
#define CRC16CCITTRR 0x8810

//! @}

//! \ingroup sliplib
//! \defgroup sliplib_functions COSMOS SLIP support functions
//! @{

int32_t slip_check_crc(uint8_t *sbuf, uint16_t ssize);
int32_t slip_unpack(uint8_t *sbuf, uint16_t ssize, uint8_t *rbuf, uint16_t rsize);
int32_t slip_decode(uint8_t *sbuf, uint16_t ssize, uint8_t *rbuf, uint16_t rsize);
int32_t slip_pack(uint8_t *rbuf, uint16_t rsize, uint8_t *sbuf, uint16_t ssize);
int32_t slip_encode(uint8_t *rbuf, uint16_t rsize, uint8_t *sbuf, uint16_t ssize);
uint16_t slip_calc_crc(uint8_t *buf, uint16_t size);
uint16_t slip_get_crc(uint8_t *buf, uint16_t size);
uint16_t slip_set_crc(uint8_t *buf, uint16_t size);

//! @}

#endif
