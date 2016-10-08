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

#ifndef _GIGE_H
#define _GIGE_H 1

#include "support/configCosmos.h"
#include "support/socketlib.h"

/*! \file gige_lib.cpp
*	\brief gige_lib include file.
*
*	\defgroup gige GigE Vision support.
* @{
* GigE Vision.
*
* This library provides the functions necessary to any GigE Vision conforming camera.
* @}
*/

//! \ingroup gige
//!	\defgroup gige_cmd_constants GigE Vision Command constants
//!	@{

#define GIGE_CMD_DISCOVERY		0x0002
#define GIGE_CMD_FORCEIP		0x0004
#define GIGE_CMD_PACKETRESEND	0x0040
#define GIGE_CMD_READREG		0x0080
#define GIGE_CMD_WRITEREG		0x0082
#define GIGE_CMD_READMEM		0x0084
#define GIGE_CMD_WRITEMEM		0x0086
#define GIGE_CMD_EVENT			0x00c0

//! @}

//! \ingroup gige
//!	\defgroup gige_ack_constants GigE Vision Acknowledge constants
//!	@{

#define GIGE_ACK_DISCOVERY		0x0003
#define GIGE_ACK_FORCEIP		0x0005
#define GIGE_ACK_READREG		0x0081
#define GIGE_ACK_WRITEREG		0x0083
#define GIGE_ACK_READMEM		0x0085
#define GIGE_ACK_WRITEMEM		0x0087
#define GIGE_ACK_PENDING		0x0089

//! @}

//! \ingroup gige
//!	\defgroup gige_reg_constants GigE Vision Register constants
//!	@{

#define GIGE_REG_VERSION 0x0000
#define GIGE_REG_DEVICE_MODE 0x0004
#define GIGE_REG_DEVICE_MAC_HIGH 0x0008
#define GIGE_REG_DEVICE_MAC_LOW 0x000c
#define GIGE_REG_NET_CAPABILITY 0x0010
#define GIGE_REG_NET_CONFIG 0x0014
#define GIGE_REG_CURRENT_IP_ADDRESS 0x0024
#define GIGE_REG_CURRENT_IP_NETMASK 0x0034
#define GIGE_REG_CURRENT_IP_GATEWAY 0x0044
#define GIGE_REG_MANUFACTURER_NAME 0x0048
#define GIGE_REG_MODEL_NAME 0x0068
#define GIGE_REG_DEVICE_VERSION 0x0088
#define GIGE_REG_MANUFACTURER_INFO 0x00a8
#define GIGE_REG_SERIAL_NUMBER 0x00d8
#define GIGE_REG_USER_DEFINED_NAME 0x00e8
#define GIGE_REG_FIRST_URL 0x0200
#define GIGE_REG_SECOND_URL 0x0400
#define GIGE_REG_NET_COUNT 0x0600
#define GIGE_REG_PERSISTENT_IP_ADDRESS 0x064c
#define GIGE_REG_PERSISTENT_IP_NETMASK 0x065c
#define GIGE_REG_PERSISTENT_IP_GATEWAY 0x066c
#define GIGE_REG_PERSISTENT_IP_LINK_SPEED 0x0670
#define GIGE_REG_MESSAGE_CHANNEL_COUNT 0x0900
#define GIGE_REG_STREAM_CHANNEL_COUNT 0x0904
#define GIGE_REG_ACTION_CHANNEL_COUNT 0x0908
#define GIGE_REG_ACTION_DEVICE_KEY 0x090c
#define GIGE_REG_ACTIVE_LINK_COUNT 0x0910
#define GIGE_REG_GVSP_CAPABILITY 0x092c
#define GIGE_REG_MESSAGE_CHANNEL_CAPABILITY 0x0930
#define GIGE_REG_GVCP_CAPABILITY 0x0934
#define GIGE_REG_GVCP_HEARTBEAT_TIMEOUT 0x0938
#define GIGE_REG_TICK_FREQUENCY_HIGH 0x093c
#define GIGE_REG_TICK_FREQUENCY_LOW 0x0940
#define GIGE_REG_TIMESTAMP_CONTROL 0x0944
#define GIGE_REG_TIMESTAMP_VALUE_HIGH 0x0948
#define GIGE_REG_TIMESTAMP_VALUE_LOW 0x094c
#define GIGE_REG_DISCOVERY_ACK_DELAY 0x0950
#define GIGE_REG_GVCP_CONFIGURATION 0x0954
#define GIGE_REG_PENDING_TIMEOUT 0x0958
#define GIGE_REG_CONTROL_SWITCHOVER_KEY 0x095c
#define GIGE_REG_GVSP_CONFIGURATION 0x0960
#define GIGE_REG_PHYSICAL_LINK_CAPABILITY 0x0964
#define GIGE_REG_PHYSICAL_LINK_CONFIGURATION 0x0968
#define GIGE_REG_IEEE_1588_STATUS 0x096c
#define GIGE_REG_QUEUE_SIZE 0x0970
#define GIGE_REG_CONTROL_CHANNEL_PRIVILEGE 0x0a00
#define GIGE_REG_CCP 0x0a00
#define GIGE_REG_PRIMARY_APPLICATION_PORT 0x0a04
#define GIGE_REG_PRIMARY_APPLICATION_IP_ADDRESS 0x0a14
#define GIGE_REG_MESSAGE_CHANNEL_PORT 0x0b00
#define GIGE_REG_MCP 0x0b00
#define GIGE_REG_MESSAGE_CHANNEL_DESTINATION 0x0b10
#define GIGE_REG_MESSAGE_CHANNEL_TIMEOUT 0x0b14
#define GIGE_REG_MCDA 0x0b14
#define GIGE_REG_MESSAGE_CHANNEL_RETRY_COUNT 0x0b18
#define GIGE_REG_MESSAGE_CHANNEL_SOURCE_PORT 0x0b1c
#define GIGE_REG_STREAM_CHANNEL_PORT 0x0d00
#define GIGE_REG_SCP 0x0d00
#define GIGE_REG_STREAM_CHANNEL_PACKET_SIZE 0x0d04
#define GIGE_REG_SCPS 0x0d04
#define GIGE_REG_STREAM_CHANNEL_PACKET_DELAY 0x0d08
#define GIGE_REG_SCPD 0x0d08
#define GIGE_REG_STREAM_CHANNEL_DESTINATION_ADDRESS 0x0d18
#define GIGE_REG_SCDA 0x0d18
#define GIGE_REG_STREAM_CHANNEL_SOURCE_PORT 0x0d1c
#define GIGE_REG_STREAM_CHANNEL_CAPABILITY 0x0d20
#define GIGE_REG_STREAM_CHANNEL_CONFIGURATION 0x0d24
#define GIGE_REG_STREAM_CHANNEL_ZONE 0x0d28
#define GIGE_REG_STREAM_CHANNEL_ZONE_DIRECTION 0x0d2c
#define GIGE_REG_MANIFEST_TABLE 0x900
#define GIGE_REG_ACTION_GROUP_KEY 0x9800
#define GIGE_REG_ACTION_GROUP_MASK 0x9804
#define GIGE_REG_MANUFACTURER_SPECIFIC 0xa000

#define GIGE_MAX_MANUFACTURER_NAME 32
#define GIGE_MAX_MODEL_NAME 32
#define GIGE_MAX_DEVICE_VERSION 32
#define GIGE_MAX_MANUFACTURER_INFO 48
#define GIGE_MAX_SERIAL_NUMBER 16
#define GIGE_MAX_USER_DEFINED_NAME 16
#define GIGE_MAX_FIRST_URL 512
#define GIGE_MAX_SECOND_URL 512 
#define GIGE_MAX_PACKET 16384
#define GIGE_MIN_PACKET 576

//! @}

//! \ingroup gige
//!	\defgroup gige_prosilica_constants GigE Vision Prosilica constants
//!	@{

//! Prosilica specific registers
#define PROSILICA_DeviceID						0x00d8
#define PROSILICA_SensorWidth					0x11024
#define PROSILICA_SensorHeight					0x11028
#define PROSILICA_BinningXMax					0x11104
#define PROSILICA_BinningXValue					0x11108
#define PROSILICA_BinningYMax					0x11124
#define PROSILICA_BinningYValue					0x11128
#define PROSILICA_StreamBytesPerSec				0x11144
#define PROSILICA_MaxWidth						0x12110
#define PROSILICA_MaxHeight						0x12114
#define PROSILICA_PixelFormat					0x12120
#define PROSILICA_PixelFormat_Mono8				17301505
#define PROSILICA_PixelFormat_Mono10			17825795
#define PROSILICA_PixelFormat_Mono12			17825797
#define PROSILICA_PixelFormat_BayerGR8			17301512
#define PROSILICA_PixelFormat_BayerRG8			17301513
#define PROSILICA_PixelFormat_BayerBG8			17301515
#define PROSILICA_PixelFormat_BayerBG10			17825807
#define PROSILICA_PixelFormat_BayerGR12			17825808
#define PROSILICA_PixelFormat_BayerRG12			17825809
#define PROSILICA_Width							0x12124
#define PROSILICA_Height						0x12128
#define PROSILICA_RegionX						0x1212c
#define PROSILICA_RegionY						0x12130
#define PROSILICA_PayloadSize					0x12200
#define PROSILICA_AcquisitionCommand			0x130f4
#define PROSILICA_AcquisitionCommand_Stop		0
#define PROSILICA_AcquisitionCommand_Start		1
#define PROSILICA_AcquisitionCommand_Abort		2
#define PROSILICA_AcquisitionMode				0x13104
#define PROSILICA_AcquisitionMode_Continouous	1
#define PROSILICA_AcquisitionMode_SingleFrame	2
#define PROSILICA_AcquisitionMode_MultiFrame	3
#define PROSILICA_AcquisitionMode_Recorder		4
#define PROSILICA_AcquisitionFrameCount			0x13108
#define PROSILICA_TriggerMode					0x13410
#define PROSILICA_TriggerSource					0x13410
#define PROSILICA_TriggerEvent					0x13418
#define PROSILICA_TriggerDelay					0x1341C
#define PROSILICA_ExposureMode					0x14104
//! Do not automatically adjust exposure
#define PROSILICA_ExposureMode_AutoOff	1
//! Use one frame to calculate exposure
#define PROSILICA_ExposureMode_AutoOnce	3
//! Adjust exposure on each frame
#define PROSILICA_ExposureMode_Auto	2
#define PROSILICA_ExposureValueMin				0x14108
#define PROSILICA_ExposureValueMax				0x1410C
#define PROSILICA_ExposureValue					0x14110
#define PROSILICA_GainMode						0x14144
#define PROSILICA_GainMode_AutoOff	1
#define PROSILICA_GainMode_AutoOnce	3
#define PROSILICA_GainMode_Auto	2
#define PROSILICA_GainValueMin					0x14148
#define PROSILICA_GainValueMax					0x1414C
#define PROSILICA_GainValue						0x14150
#define PROSILICA_WhitebalMode					0x14184
#define PROSILICA_WhitebalValueMin				0x14188
#define PROSILICA_WhitebalValueMax				0x1418C
#define PROSILICA_IrisMode						0x14204
#define PROSILICA_IrisVideoLevelMin				0x1420C
#define PROSILICA_IrisVideoLevelMax				0x14210
#define PROSILICA_IrisVideoLevel				0x14214

//!	@}

//! \ingroup gige
//! \defgroup gige_typedefs GigE Vision type definitions
//! @{
typedef union
{
	uint32_t address;
	uint32_t data;
	typedef struct
	{
		uint16_t major;
		uint16_t minor;
	} version;
} gige_register;

typedef struct
{
	uint16_t flag;
	uint16_t command;
	uint16_t length;
	uint16_t req_id;
	uint32_t address;
	uint32_t data;
} gige_command;

typedef struct
{
	uint16_t status;
	uint16_t acknowledge;
	uint16_t length;
	uint16_t ack_id;
	uint32_t data;
} gige_acknowledge;

typedef struct
{
	uint16_t status;
	uint16_t acknowledge;
	uint16_t length;
	uint16_t ack_id;
	uint32_t address;
	uint8_t data[600-12];
} gige_acknowledge_mem;

typedef struct
{
	uint16_t status;
	uint16_t acknowledge;
	uint16_t length;
	uint16_t ack_id;
	uint16_t spec_major;
	uint16_t spec_minor;
	uint32_t device_mode;
	uint16_t res1;
	uint16_t mac_high;
	uint32_t mac_low;
	uint32_t ip_config_options;
	uint32_t ip_config_current;
	uint32_t res2[3];
	uint32_t address;
	uint32_t res3[3];
	uint32_t subnet;
	uint32_t res4[3];
	uint32_t gateway;
	char manufacturer[32];
	char model[32];
	char device[32];
	char manufacturer_info[48];
	char serial_number[16];
	char user_defined_name[16];
} gige_acknowledge_ack;

typedef struct
{
	uint16_t status;
	uint16_t block_id;
	union
	{
		uint8_t format;
		uint32_t packet_id;
	};
	uint64_t block_id64;
	uint32_t packet_id32;
	uint16_t payload_specific;
	uint16_t payload_type;
} gige_data_leader_packet;


typedef uint32_t gige_device_mode;

typedef struct
{
	uint8_t byte[4];
} gige_address;

typedef uint32_t gige_link_speed;

typedef uint32_t gige_count;

typedef struct
{
	//! Command channel
	socket_channel command;
	//! Stream channel
	socket_channel stream;
	//! Camera Control Channel input buffer
	union
	{
		uint8_t cbyte[600];
		uint32_t cword[150];
		gige_command creg;
		gige_acknowledge cack;
		gige_acknowledge_mem cack_mem;
		gige_acknowledge_ack cack_ack;
	};
	//! Camera Stream Channel input buffer
	uint8_t sbuf[600];
	//! Request ID
	uint16_t req_id;
	//! Flow rate
	uint32_t streambps;
	//! Best packet size
	uint16_t bestsize;
	//! Detector Width
	uint16_t maxwidth;
	//! Detector Height
	uint16_t maxheight;
	//! Requested Width
	uint16_t width;
	//! Requested Height
	uint16_t height;
} gige_handle;

//! @}

//! \ingroup gige
//! \defgroup gige_functions GigE Vision functions
//! @{

std::vector<gige_acknowledge_ack> gige_discover();
gige_handle *gige_open(char address[18],uint8_t privilege, uint32_t heartbeat_msec, uint32_t socket_usec, uint32_t streambps);
int gige_writereg(gige_handle *handle, uint32_t address, uint32_t data);
uint32_t gige_readreg(gige_handle *handle, uint32_t address);
uint32_t gige_readmem(gige_handle *handle, uint32_t address, uint32_t nbytes);
void gige_close(gige_handle *handle);
uint32_t gige_address_to_value(char *address);
char *gige_value_to_address(uint32_t value);
int prosilica_config(gige_handle *handle, uint32_t format, uint32_t xbin, uint32_t ybin, uint32_t xsize, uint32_t ysize, uint32_t xoffset, uint32_t yoffset);
int prosilica_image(gige_handle *handle, uint16_t emode, uint32_t exposure, uint32_t gain, uint8_t *buffer, uint16_t bsize);
int a35_image(gige_handle *handle, uint32_t frames, uint8_t *buffer, uint16_t bsize);
int a35_config(gige_handle *handle, uint32_t xsize, uint32_t ysize, uint32_t video_rate);
//! @}



// ***************************************************
// *********************  A35  ***********************
// ***************************************************

uint32_t gige_readreg2(gige_handle *handle, uint32_t address);
uint32_t gige_request(gige_handle *handle, uint32_t address);

//! @}


//! \ingroup gige
//!	\defgroup gige_a35_constants GigE Vision A35 constants
//!	@{

//! A35 specific registers


#define A35_NETWORK                 0x0014
#define A35_SETA35IP                0x064C
#define A35_SETA35MASK              0x065C
#define A35_SETA35GATEWAY           0x066C
#define A35_PACKETDELAY             0x0D08
#define A35_SETCOMPUTERIP           0x0D18
#define A35_FRAMESTOSKIP            0xB81C
#define A35_WIDTH					0xD300
#define A35_HEIGHT                  0xD304
#define A35_PIXELFORMAT             0xD308
#define A35_ACQUISITIONSTART        0xD314
#define A35_ACQUISITIONSTOP         0xD318
#define A35_TESTIMAGESELECTOR       0xD33C
#define A35_DEVICERESET             0xD340
#define A35_CAMERAHEADRESET         0xE81C1
#define A35_TEMPERATURE             0xE83C
#define A35_TEMPERATUREFPA          0xE840
#define A35_COMMAND_DOFFC           0xE938  // (send 0 to do FFC)
#define A35_FFCMODE                 0xE940  // (Flat Field Correction)
#define A35_SENSORVIDEOSTANDARD     0xE9A4
#define A35_DIGITALOUTPUT           0xE9C8
#define A35_XPMODE                  0xE9D4
#define A35_LVDSMODE                0xE9D8
#define A35_CMOSBITDEPTH            0xE9DC
#define A35_LVDSBITDEPTH            0xE9E0
#define A35_IMAGEADJUST				0xE9EC
#define A35_IPENGINETESTPATTERN     0x80000000

//! A35 Defined Values
#define A35_NETWORK_OFF             4
#define A35_NETWORK_MANUAL          5
#define A35_NETWORK_DHCP            6
#define A35_PIXELFORMAT_8BIT        0x01080001
#define A35_PIXELFORMAT_14BIT       0x01100025
#define A35_DIGITALOUTPUT_8BIT      2
#define A35_DIGITALOUTPUT_14BIT     3
#define A35_SENSORVIDEOSTANDARD_30HZ 0
#define A35_SENSORVIDEOSTANDARD_60HZ 4
#define A35_FFCMODE_MANUAL          0
#define A35_FFCMODE_AUTO            1
#define A35_FFCMODE_EXTERNAL             2
#define A35_XPMODE_CMOS             2
#define A35_SKIP0                   0x00004000
#define A35_SKIP1                   0x00014000
#define A35_SKIP2                   0x00024000
#define A35_SKIP3                   0x00034000
#define A35_SKIP4                   0x00044000
#define A35_IMAGEADJUST_PLATEAUHISTOGRAM	0
#define A35_IMAGEADJUST_ONCEBRIGHT			1
#define A35_IMAGEADJUST_AUTOBRIGHT			2
#define A35_IMAGEADJUST_MANUAL				3
#define A35_IMAGEADJUST_LINEAR				4

//#define NETWORKMASK                 0xFFFFFF00  //255.255.255.0
//!	@}

#endif
