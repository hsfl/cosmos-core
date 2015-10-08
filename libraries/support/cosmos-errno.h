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

#ifndef _COSMOS_ERROR_H
#define _COSMOS_ERROR_H 1

#include <errno.h>

/*!	\file cosmos-errno.h
	\brief COSMOS Error Codes
	\defgroup  error COSMOS Error codes
	@{
	Definitions of the error codes used throughout all COSMOS related software. These codes
	are guaranteed to lie outside the range of general error codes, and further divided in to
	groups of ten by area of applicability.
	@}
*/

//! \ingroup error
//! \defgroup error_defines COSMOS Error Code definitions
//! @{
// All COSMOS errors are less than -200 since the range of standard system errors
// is 1-200
#define GS232B_ERROR_OPEN -201
#define GS232B_ERROR_CLOSED -202
#define GS232B_ERROR_OUTOFRANGE -203

//! Error opening MicroStrain device
#define MICROSTRAIN_ERROR_OPEN -211
#define MICROSTRAIN_ERROR_CLOSED -212
#define MICROSTRAIN_ERROR_OUTOFRANGE -213
#define MICROSTRAIN_ERROR_FAILED -214
#define MICROSTRAIN_ERROR_TOOMANY -215
#define MICROSTRAIN_ERROR_FUNCTION -216
#define MICROSTRAIN_ERROR_SEND -217

#define SINCLAIR_ERROR_OPEN -221
#define SINCLAIR_ERROR_CLOSED -222
#define SINCLAIR_ERROR_OUTOFRANGE -223
#define SINCLAIR_ERROR_FAILED -224
#define SINCLAIR_ERROR_TOOMANY -225

#define NSP_ERROR_CRC -226
#define NSP_ERROR_SIZE -227
#define NSP_ERROR_NACK -228

#define TS2000_ERROR_OPEN -231
#define TS2000_ERROR_CLOSED -232
#define TS2000_ERROR_OUTOFRANGE -233

#define VMT35_ERROR_RODNUM -241
#define VMT35_ERROR_BYTE -242
#define VMT35_ERROR_OUTOFRANGE -243
#define VMT35_ERROR_OPENED -244
#define VMT35_ERROR_CLOSED -245
#define VMT35_ERROR_NACK -246
#define VMT35_ERROR_CRC -247
#define VMT35_ERROR_DEVNAME -248

#define JSON_ERROR_NAME_LENGTH -251
#define JSON_ERROR_DATA_LENGTH -252
#define JSON_ERROR_INDEX_SIZE -253
#define JSON_ERROR_NOJMAP -254
#define JSON_ERROR_NOENTRY -255
#define JSON_ERROR_EOS -256
#define JSON_ERROR_SCAN -257
#define JSON_ERROR_JSTRING -258
#define JSON_ERROR_NAN	-259

#define SLIP_ERROR_CRC -261
#define SLIP_ERROR_PACKING -262
#define SLIP_ERROR_BUFFER -263

#define AGENT_ERROR_JSON_CREATE -271
#define AGENT_ERROR_REQ_COUNT -272
#define AGENT_ERROR_MEMORY -273
#define AGENT_ERROR_SOCKET -274
#define AGENT_ERROR_CHANNEL -275
#define AGENT_ERROR_BUFLEN -276
#define AGENT_ERROR_NULL -277
#define AGENT_ERROR_DISCOVERY -278
#define AGENT_ERROR_REQUEST -279

#define MATH_ERROR_GJ_UNDEFINED -281
#define MATH_ERROR_GJ_OUTOFRANGE -282

#define AGENT_ERROR_LCM_CREATE -291
#define AGENT_ERROR_LCM_SUBSCRIBE -292
#define AGENT_ERROR_LCM_TIMEOUT -293

#define RW_ERROR_OPEN -301
#define RW_ERROR_CLOSED -302
#define RW_ERROR_OUTOFRANGE -303
#define RW_ERROR_FAILED -304
#define RW_ERROR_TOOMANY -305
#define RW_ERROR_SEND -306

#define STK_ERROR_LOWINDEX -311
#define STK_ERROR_HIGHINDEX -312
#define STK_ERROR_OUTOFRANGE -313
#define STK_ERROR_NOTFOUND -314

#define DEM_ERROR_SEMINIT -321
#define DEM_ERROR_OPEN -322
#define DEM_ERROR_INSUFFICIENT_MEMORY -323
#define DEM_ERROR_NOTFOUND -324

#define TLE_ERROR_OUTOFRANGE -331
#define ORBIT_ERROR_NOTSUPPORTED -332
#define TLE_ERROR_NOTFOUND -334

#define NODE_ERROR_NODE -341            // node.ini does not exist
#define NODE_ERROR_GROUNDSTATION -342
#define NODE_ERROR_ROOTDIR -343

#define MOST_USER_CANCELLED -351

#define DATA_ERROR_ARCHIVE -361
#define DATA_ERROR_SIZE_MISMATCH -362
#define DATA_ERROR_CRC -363
#define DATA_ERROR_FORMAT -364
#define DATA_ERROR_RESOURCES_FOLDER -365 // if agent can't find the cosmos/resources folder
#define DATA_ERROR_NODES_FOLDER -366

#define GIGE_ERROR_NACK -371

#define CSSL_ERROR_CHECKSUM -381
#define CSSL_ERROR_READ -382
#define CSSL_ERROR_TIMEOUT -383
#define CSSL_ERROR_EOT -384
#define CSSL_ERROR_SLIPIN -385 
#define CSSL_ERROR_SLIPOUT -386 
#define CSSL_ERROR_BUFFER_SIZE_EXCEEDED -387 
#define CSSL_ERROR_BAD_ESCAPE -388


#define IC9100_ERROR_OPEN -391
#define IC9100_ERROR_CLOSED -392
#define IC9100_ERROR_OUTOFRANGE -393
#define IC9100_ERROR_WRITE -394
#define IC9100_ERROR_NG -395
#define IC9100_ERROR_READ -396

#define ASTRODEV_ERROR_HEADER_CS -391
#define ASTRODEV_ERROR_SYNC0 -392
#define ASTRODEV_ERROR_NACK -393
#define ASTRODEV_ERROR_PAYLOAD_CS -394
#define ASTRODEV_ERROR_SYNC1 -395
#define ASTRODEV_ERROR_HEADER -396
#define ASTRODEV_ERROR_PAYLOAD -397

#define GPS_ERROR_OPEN -401
#define GPS_ERROR_CLOSED -402

#define OEMV_ERROR_OPEN -411
#define OEMV_ERROR_CLOSED -412
#define OEMV_ERROR_CRC -413
#define OEMV_ERROR_RESPONSE -414

#define PIC_ERROR_OPEN -421
#define PIC_ERROR_CLOSED -422
#define PIC_ERROR_PORT -423
#define PIC_ERROR_NACK -424
#define PIC_ERROR_SIZE -425
#define PIC_ERROR_ERR -426
#define PIC_ERROR_TYPE -427

#define CSSL_ERROR_BASE -439

#define SUCHI_ERROR_CLOSED -441

#define VN100_ERROR_OPEN -451
#define VN100_ERROR_NACK -452
#define VN100_ERROR_SCAN -453

#define ISC_ERROR_CLOSED -461

#define TRANSFER_ERROR_MATCH -471
#define TRANSFER_ERROR_QUEUEFULL -472
#define TRANSFER_ERROR_INDEX -473
#define TRANSFER_ERROR_NODE -474

#define SOCKET_ERROR_CS -481
#define SOCKET_ERROR_PROTOCOL -482
#define SOCKET_READ_TIMEOUT -483
#define SOCKET_ERROR_OPEN   -484
#define SOCKET_ERROR_OPEN_RECEIVE    -485
#define SOCKET_ERROR_OPEN_TRANSMIT  -486
#define SOCKET_ERROR_OPEN_CONNECT  -487
#define SOCKET_ERROR_CLOSE  -488

#define TRIMBLE_ERROR_CS -491

#define JPLEPHEM_ERROR_NOTFOUND -501
#define JPLEPHEM_ERROR_INSUFFICIENT_MEMORY -502
#define JPLEPHEM_ERROR_OUTOFRANGE -503
#define JPLEPHEM_ERROR_NUTATIONS -504
#define JPLEPHEM_ERROR_LIBRATIONS -505

#define GEOMAG_ERROR_NOTFOUND -511              // WMM File not found
#define GEOMAG_ERROR_OUTOFRANGE -512

#define GENERAL_OK                        0     /* everything is all right */
#define GENERAL_ERROR_NOSIGNAL         -2001     /* there's no free signal */
#define GENERAL_ERROR_NOTSTARTED       -2002     /* uninitialized error */
#define GENERAL_ERROR_NULLPOINTER      -2003     /* you gave a null pointer to the function */
#define GENERAL_ERROR_OOPS             -2004     /* internal error, something's erong */
#define GENERAL_ERROR_MEMORY           -2005     /* there's no memory for more of something */
#define GENERAL_ERROR_OPEN             -2006     /* file doesnt exist or you aren't good user */
#define GENERAL_ERROR_CRC              -2007     /* crc error */
#define GENERAL_ERROR_OVERSIZE         -2008     /* input was too large */
#define GENERAL_ERROR_ZEROSIZE         -2009     /* input cannot be zero size */
#define GENERAL_ERROR_UNIMPLEMENTED    -2010     /* command is not implemented */
#define GENERAL_ERROR_EMPTY            -2011     /* usually indicates that a buffer was empty */
#define GENERAL_ERROR_UNHANDLED_ERR    -2012     /* error that is not currently handled */
#define GENERAL_ERROR_UNDERSIZE        -2013     /* packet is smaller than expected size */
#define GENERAL_ERROR_BAD_SIZE         -2014     /* packet is an invalid size */
#define GENERAL_ERROR_TIMEOUT          -2015     /* a timer expired */
#define GENERAL_ERROR_INPUT            -2016     /* invalid input was provided */
#define GENERAL_ERROR_OUTPUT           -2017     /* output from a routine returned bad output, possibly related to bad input */


//! @}
#endif
