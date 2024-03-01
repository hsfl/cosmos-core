/**
 * @file gige_lib.h
 * @brief gige_lib include file.
 * 
 * Copyright (C) 2024 by Interstel Technologies, Inc. and Hawaii Space Flight
 * Laboratory.
 * 
 * This file is part of the COSMOS/core that is the central module for COSMOS.
 * For more information on COSMOS go to <http://cosmos-project.com>
 * 
 * The COSMOS/core software is licenced under the GNU Lesser General Public
 * License (LGPL) version 3 licence.
 * 
 * You should have received a copy of the GNU Lesser General Public License. If
 * not, go to <http://www.gnu.org/licenses/>
 * 
 * COSMOS/core is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 * 
 * COSMOS/core is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * Refer to the "licences" folder for further information on the conditions and
 * terms to use this software.
 */

#ifndef _GIGE_H
#define _GIGE_H 1

#include <mutex>
#include <thread>
#include "support/configCosmos.h"
#include "support/socketlib.h"
#include "math/bytelib.h"
#include "math/vector.h"
using namespace Cosmos::Math::Vectors;

namespace Cosmos {
    namespace Devices {
        /**
         * @brief defgroup gige GigE Vision support.
         * @{
         * GigE Vision.
         * 
         * This library provides the functions necessary to any GigE Vision 
         * conforming camera.
         * @}
         */

        /**
         * @ingroup gige
         * @defgroup gige_cmd_constants GigE Vision Command constants
         * @{
         */

/** @todo Document this. */
#define GIGE_CMD_DISCOVERY		0x0002
/** @todo Document this. */
#define GIGE_CMD_FORCEIP		0x0004
/** @todo Document this. */
#define GIGE_CMD_PACKETRESEND	0x0040
/** @todo Document this. */
#define GIGE_CMD_READREG		0x0080
/** @todo Document this. */
#define GIGE_CMD_WRITEREG		0x0082
/** @todo Document this. */
#define GIGE_CMD_READMEM		0x0084
/** @todo Document this. */
#define GIGE_CMD_WRITEMEM		0x0086
/** @todo Document this. */
#define GIGE_CMD_EVENT			0x00c0

        /**
         * @}
         * @ingroup gige
         * @defgroup gige_ack_constants GigE Vision Acknowledge constants
         * @{
         */

/** @todo Document this. */
#define GIGE_ACK_DISCOVERY		0x0003
/** @todo Document this. */
#define GIGE_ACK_FORCEIP		0x0005
/** @todo Document this. */
#define GIGE_ACK_READREG		0x0081
/** @todo Document this. */
#define GIGE_ACK_WRITEREG		0x0083
/** @todo Document this. */
#define GIGE_ACK_READMEM		0x0085
/** @todo Document this. */
#define GIGE_ACK_WRITEMEM		0x0087
/** @todo Document this. */
#define GIGE_ACK_PENDING		0x0089

        /**
         * @}
         * @ingroup gige
         * @defgroup gige_reg_constants GigE Vision Register constants
         * @{
         */

/** @todo Document this. */
#define GIGE_REG_VERSION 0x0000
/** @todo Document this. */
#define GIGE_REG_DEVICE_MODE 0x0004
/** @todo Document this. */
#define GIGE_REG_DEVICE_MAC_HIGH 0x0008
/** @todo Document this. */
#define GIGE_REG_DEVICE_MAC_LOW 0x000c
/** @todo Document this. */
#define GIGE_REG_NET_CAPABILITY 0x0010
/** @todo Document this. */
#define GIGE_REG_NET_CONFIG 0x0014
/** @todo Document this. */
#define GIGE_REG_CURRENT_IP_ADDRESS 0x0024
/** @todo Document this. */
#define GIGE_REG_CURRENT_IP_NETMASK 0x0034
/** @todo Document this. */
#define GIGE_REG_CURRENT_IP_GATEWAY 0x0044
/** @todo Document this. */
#define GIGE_REG_MANUFACTURER_NAME 0x0048
/** @todo Document this. */
#define GIGE_REG_MODEL_NAME 0x0068
/** @todo Document this. */
#define GIGE_REG_DEVICE_VERSION 0x0088
/** @todo Document this. */
#define GIGE_REG_MANUFACTURER_INFO 0x00a8
/** @todo Document this. */
#define GIGE_REG_SERIAL_NUMBER 0x00d8
/** @todo Document this. */
#define GIGE_REG_USER_DEFINED_NAME 0x00e8
/** @todo Document this. */
#define GIGE_REG_FIRST_URL 0x0200
/** @todo Document this. */
#define GIGE_REG_SECOND_URL 0x0400
/** @todo Document this. */
#define GIGE_REG_NET_COUNT 0x0600
/** @todo Document this. */
#define GIGE_REG_PERSISTENT_IP_ADDRESS 0x064c
/** @todo Document this. */
#define GIGE_REG_PERSISTENT_IP_NETMASK 0x065c
/** @todo Document this. */
#define GIGE_REG_PERSISTENT_IP_GATEWAY 0x066c
/** @todo Document this. */
#define GIGE_REG_PERSISTENT_IP_LINK_SPEED 0x0670
/** @todo Document this. */
#define GIGE_REG_MESSAGE_CHANNEL_COUNT 0x0900
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_COUNT 0x0904
/** @todo Document this. */
#define GIGE_REG_ACTION_CHANNEL_COUNT 0x0908
/** @todo Document this. */
#define GIGE_REG_ACTION_DEVICE_KEY 0x090c
/** @todo Document this. */
#define GIGE_REG_ACTIVE_LINK_COUNT 0x0910
/** @todo Document this. */
#define GIGE_REG_GVSP_CAPABILITY 0x092c
/** @todo Document this. */
#define GIGE_REG_MESSAGE_CHANNEL_CAPABILITY 0x0930
/** @todo Document this. */
#define GIGE_REG_GVCP_CAPABILITY 0x0934
/** @todo Document this. */
#define GIGE_REG_GVCP_HEARTBEAT_TIMEOUT 0x0938
/** @todo Document this. */
#define GIGE_REG_TICK_FREQUENCY_HIGH 0x093c
/** @todo Document this. */
#define GIGE_REG_TICK_FREQUENCY_LOW 0x0940
/** @todo Document this. */
#define GIGE_REG_TIMESTAMP_CONTROL 0x0944
/** @todo Document this. */
#define GIGE_REG_TIMESTAMP_VALUE_HIGH 0x0948
/** @todo Document this. */
#define GIGE_REG_TIMESTAMP_VALUE_LOW 0x094c
/** @todo Document this. */
#define GIGE_REG_DISCOVERY_ACK_DELAY 0x0950
/** @todo Document this. */
#define GIGE_REG_GVCP_CONFIGURATION 0x0954
/** @todo Document this. */
#define GIGE_REG_PENDING_TIMEOUT 0x0958
/** @todo Document this. */
#define GIGE_REG_CONTROL_SWITCHOVER_KEY 0x095c
/** @todo Document this. */
#define GIGE_REG_GVSP_CONFIGURATION 0x0960
/** @todo Document this. */
#define GIGE_REG_PHYSICAL_LINK_CAPABILITY 0x0964
/** @todo Document this. */
#define GIGE_REG_PHYSICAL_LINK_CONFIGURATION 0x0968
/** @todo Document this. */
#define GIGE_REG_IEEE_1588_STATUS 0x096c
/** @todo Document this. */
#define GIGE_REG_QUEUE_SIZE 0x0970
/** @todo Document this. */
#define GIGE_REG_CONTROL_CHANNEL_PRIVILEGE 0x0a00
/** @todo Document this. */
#define GIGE_REG_CCP 0x0a00
/** @todo Document this. */
#define GIGE_REG_PRIMARY_APPLICATION_PORT 0x0a04
/** @todo Document this. */
#define GIGE_REG_PRIMARY_APPLICATION_IP_ADDRESS 0x0a14
/** @todo Document this. */
#define GIGE_REG_MESSAGE_CHANNEL_PORT 0x0b00
/** @todo Document this. */
#define GIGE_REG_MCP 0x0b00
/** @todo Document this. */
#define GIGE_REG_MESSAGE_CHANNEL_DESTINATION 0x0b10
/** @todo Document this. */
#define GIGE_REG_MESSAGE_CHANNEL_TIMEOUT 0x0b14
/** @todo Document this. */
#define GIGE_REG_MCDA 0x0b14
/** @todo Document this. */
#define GIGE_REG_MESSAGE_CHANNEL_RETRY_COUNT 0x0b18
/** @todo Document this. */
#define GIGE_REG_MESSAGE_CHANNEL_SOURCE_PORT 0x0b1c
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_PORT 0x0d00
/** @todo Document this. */
#define GIGE_REG_SCP 0x0d00
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_PACKET_SIZE 0x0d04
/** @todo Document this. */
#define GIGE_REG_SCPS 0x0d04
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_PACKET_DELAY 0x0d08
/** @todo Document this. */
#define GIGE_REG_SCPD 0x0d08
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_DESTINATION_ADDRESS 0x0d18
/** @todo Document this. */
#define GIGE_REG_SCDA 0x0d18
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_SOURCE_PORT 0x0d1c
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_CAPABILITY 0x0d20
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_CONFIGURATION 0x0d24
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_ZONE 0x0d28
/** @todo Document this. */
#define GIGE_REG_STREAM_CHANNEL_ZONE_DIRECTION 0x0d2c
/** @todo Document this. */
#define GIGE_REG_MANIFEST_TABLE 0x900
/** @todo Document this. */
#define GIGE_REG_ACTION_GROUP_KEY 0x9800
/** @todo Document this. */
#define GIGE_REG_ACTION_GROUP_MASK 0x9804
/** @todo Document this. */
#define GIGE_REG_MANUFACTURER_SPECIFIC 0xa000

/** @todo Document this. */
#define GIGE_MAX_MANUFACTURER_NAME 32
/** @todo Document this. */
#define GIGE_MAX_MODEL_NAME 32
/** @todo Document this. */
#define GIGE_MAX_DEVICE_VERSION 32
/** @todo Document this. */
#define GIGE_MAX_MANUFACTURER_INFO 48
/** @todo Document this. */
#define GIGE_MAX_SERIAL_NUMBER 16
/** @todo Document this. */
#define GIGE_MAX_USER_DEFINED_NAME 16
/** @todo Document this. */
#define GIGE_MAX_FIRST_URL 512
/** @todo Document this. */
#define GIGE_MAX_SECOND_URL 512 
/** @todo Document this. */
#define GIGE_MAX_PACKET 16384
/** @todo Document this. */
#define GIGE_MIN_PACKET 576

        /**
         * @}
         * @ingroup gige
         * @defgroup gige_prosilica_constants GigE Vision Prosilica constants
         * @{
         */

        /** Prosilica specific registers */

/** @todo Document this. */
#define PROSILICA_DeviceID						0x00d8
/** @todo Document this. */
#define PROSILICA_SensorWidth					0x11024
/** @todo Document this. */
#define PROSILICA_SensorHeight					0x11028
/** @todo Document this. */
#define PROSILICA_BinningXMax					0x11104
/** @todo Document this. */
#define PROSILICA_BinningXValue					0x11108
/** @todo Document this. */
#define PROSILICA_BinningYMax					0x11124
/** @todo Document this. */
#define PROSILICA_BinningYValue					0x11128
/** @todo Document this. */
#define PROSILICA_StreamBytesPerSec				0x11144
/** @todo Document this. */
#define PROSILICA_MaxWidth						0x12110
/** @todo Document this. */
#define PROSILICA_MaxHeight						0x12114
/** @todo Document this. */
#define PROSILICA_PixelFormat					0x12120
/** @todo Document this. */
#define PROSILICA_PixelFormat_Mono8				17301505
/** @todo Document this. */
#define PROSILICA_PixelFormat_Mono10			17825795
/** @todo Document this. */
#define PROSILICA_PixelFormat_Mono12			17825797
/** @todo Document this. */
#define PROSILICA_PixelFormat_BayerGR8			17301512
/** @todo Document this. */
#define PROSILICA_PixelFormat_BayerRG8			17301513
/** @todo Document this. */
#define PROSILICA_PixelFormat_BayerBG8			17301515
/** @todo Document this. */
#define PROSILICA_PixelFormat_BayerBG10			17825807
/** @todo Document this. */
#define PROSILICA_PixelFormat_BayerGR12			17825808
/** @todo Document this. */
#define PROSILICA_PixelFormat_BayerRG12			17825809
/** @todo Document this. */
#define PROSILICA_Width							0x12124
/** @todo Document this. */
#define PROSILICA_Height						0x12128
/** @todo Document this. */
#define PROSILICA_RegionX						0x1212c
/** @todo Document this. */
#define PROSILICA_RegionY						0x12130
/** @todo Document this. */
#define PROSILICA_PayloadSize					0x12200
/** @todo Document this. */
#define PROSILICA_AcquisitionCommand			0x130f4
/** @todo Document this. */
#define PROSILICA_AcquisitionCommand_Stop		0
/** @todo Document this. */
#define PROSILICA_AcquisitionCommand_Start		1
/** @todo Document this. */
#define PROSILICA_AcquisitionCommand_Abort		2
/** @todo Document this. */
#define PROSILICA_AcquisitionMode				0x13104
/** @todo Document this. */
#define PROSILICA_AcquisitionMode_Continouous	1
/** @todo Document this. */
#define PROSILICA_AcquisitionMode_SingleFrame	2
/** @todo Document this. */
#define PROSILICA_AcquisitionMode_MultiFrame	3
/** @todo Document this. */
#define PROSILICA_AcquisitionMode_Recorder		4
/** @todo Document this. */
#define PROSILICA_AcquisitionFrameCount			0x13108
/** @todo Document this. */
#define PROSILICA_TriggerMode					0x13410
/** @todo Document this. */
#define PROSILICA_TriggerSource					0x13410
/** @todo Document this. */
#define PROSILICA_TriggerEvent					0x13418
/** @todo Document this. */
#define PROSILICA_TriggerDelay					0x1341C
/** @todo Document this. */
#define PROSILICA_ExposureMode					0x14104
        /** Do not automatically adjust exposure */
#define PROSILICA_ExposureMode_AutoOff	1
        /** Use one frame to calculate exposure */
#define PROSILICA_ExposureMode_AutoOnce	3
        /** Adjust exposure on each frame */
#define PROSILICA_ExposureMode_Auto	2
/** @todo Document this. */
#define PROSILICA_ExposureValueMin				0x14108
/** @todo Document this. */
#define PROSILICA_ExposureValueMax				0x1410C
/** @todo Document this. */
#define PROSILICA_ExposureValue					0x14110
/** @todo Document this. */
#define PROSILICA_GainMode						0x14144
/** @todo Document this. */
#define PROSILICA_GainMode_AutoOff	1
/** @todo Document this. */
#define PROSILICA_GainMode_AutoOnce	3
/** @todo Document this. */
#define PROSILICA_GainMode_Auto	2
/** @todo Document this. */
#define PROSILICA_GainValueMin					0x14148
/** @todo Document this. */
#define PROSILICA_GainValueMax					0x1414C
/** @todo Document this. */
#define PROSILICA_GainValue						0x14150
/** @todo Document this. */
#define PROSILICA_WhitebalMode					0x14184
/** @todo Document this. */
#define PROSILICA_WhitebalValueMin				0x14188
/** @todo Document this. */
#define PROSILICA_WhitebalValueMax				0x1418C
/** @todo Document this. */
#define PROSILICA_IrisMode						0x14204
/** @todo Document this. */
#define PROSILICA_IrisVideoLevelMin				0x1420C
/** @todo Document this. */
#define PROSILICA_IrisVideoLevelMax				0x14210
/** @todo Document this. */
#define PROSILICA_IrisVideoLevel				0x14214

        /**
         * @}
         * @ingroup gige
         * @defgroup gige_typedefs GigE Vision type definitions
         * @{
         */

        /** @todo Document this. */
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

        /** @todo Document this. */
        typedef struct
        {
            uint16_t flag;
            uint16_t command;
            uint16_t length;
            uint16_t req_id;
            uint32_t address;
            uint32_t data;
        } gige_command;

        /** @todo Document this. */
        typedef struct
        {
            uint16_t status;
            uint16_t acknowledge;
            uint16_t length;
            uint16_t ack_id;
            uint32_t data;
        } gige_acknowledge;

        /** @todo Document this. */
        typedef struct
        {
            uint16_t status;
            uint16_t acknowledge;
            uint16_t length;
            uint16_t ack_id;
            uint32_t address;
            uint8_t data[600-12];
        } gige_acknowledge_mem;

        /** @todo Document this. */
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

        /** @todo Document this. */
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

        /** @todo Document this. */
        typedef uint32_t gige_device_mode;

        /** @todo Document this. */
        typedef struct
        {
            uint8_t byte[4]={0};
        } gige_address;

        /** @todo Document this. */
        typedef uint32_t gige_link_speed;
        /** @todo Document this. */
        typedef uint32_t gige_count;

        /** @todo Document this. */
        struct gige_handle
        {
            //! Command channel
            socket_channel command;
            //! Stream channel
            socket_channel stream;
            //! Camera Control Channel input buffer
            union
            {
                uint8_t cbyte[600]={0};
                uint32_t cword[150];
                gige_command creg;
                gige_acknowledge cack;
                gige_acknowledge_mem cack_mem;
                gige_acknowledge_ack cack_ack;
            };
            //! Camera Stream Channel input buffer
            uint8_t sbuf[600]={0};
            //! Request ID
            uint16_t req_id;
            //! Flow rate
            uint32_t streambps;
            //! Best packet size
            uint16_t bestsize;
            //! Detector Width
            size_t maxwidth;
            //! Detector Height
            size_t maxheight;
            //! Requested Width
            size_t width;
            //! Requested Height
            size_t height;
            //! Bin Width
            size_t binwidth;
            //! Bin Height
            size_t binheight;
            vector<uint8_t> bufferin;
            vector<uint16_t> bufferout;
            thread ptthread;
            deque<vector<uint8_t>> ptqueue;
            bool ptrun;
            mutex ptmutex;
            uint8_t privilege;
            uint32_t heartbeat_msec;
        };

        /** @todo Document this. */
        struct gige_data
        {
            double average;
            Vector max;
            Vector min;
            vector<vector<double>> mean;
            vector<vector<double>> std;
        };

        /**
         * @}
         * @ingroup gige
         * @defgroup gige_functions GigE Vision functions
         * @{
         */

        vector<gige_acknowledge_ack> gige_discover();
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
        void pt1000_loop();
        int32_t pt1000_start_image(gige_handle *handle);
        int32_t pt1000_stop_image(gige_handle *handle);
        int32_t pt1000_drain(gige_handle *handle, double timeout=1.);
        int32_t pt1000_image(gige_handle *handle, uint32_t frames, gige_data &data);
        int32_t pt1000_image_dark(gige_handle *handle, uint32_t frames, gige_data &data, gige_data &dark);
        int32_t pt1000_image_flat(gige_handle *handle, uint32_t frames, gige_data &data, gige_data &flat);
        int32_t pt1000_image(gige_handle *handle, uint32_t frames, uint8_t *buffer, uint16_t bsize);
        int32_t pt1000_config(gige_handle *handle, uint32_t xsize, uint32_t ysize, uint32_t xbin=1, uint32_t ybin=1);
        
        /**
         * @}
         */

        // ***************************************************
        // *********************  A35  ***********************
        // ***************************************************

        uint32_t gige_readreg2(gige_handle *handle, uint32_t address);
        uint32_t gige_request(gige_handle *handle, uint32_t address);

        /**
         * @}
         * @ingroup gige
         * @defgroup gige_a35_constants GigE Vision A35 constants
         * @{
         */

        /** A35 specific registers */

/** @todo Document this. */ 
#define A35_NETWORK                 0x0014
/** @todo Document this. */ 
#define A35_SETA35IP                0x064C
/** @todo Document this. */ 
#define A35_SETA35MASK              0x065C
/** @todo Document this. */ 
#define A35_SETA35GATEWAY           0x066C
/** @todo Document this. */ 
#define A35_PACKETDELAY             0x0D08
/** @todo Document this. */ 
#define A35_SETCOMPUTERIP           0x0D18
/** @todo Document this. */ 
#define A35_FRAMESTOSKIP            0xB81C
/** @todo Document this. */ 
#define A35_WIDTH					0xD300
/** @todo Document this. */ 
#define A35_HEIGHT                  0xD304
/** @todo Document this. */ 
#define A35_PIXELFORMAT             0xD308
/** @todo Document this. */ 
#define A35_ACQUISITIONSTART        0xD314
/** @todo Document this. */ 
#define A35_ACQUISITIONSTOP         0xD318
/** @todo Document this. */ 
#define A35_TESTIMAGESELECTOR       0xD33C
/** @todo Document this. */ 
#define A35_DEVICERESET             0xD340
/** @todo Document this. */ 
#define A35_CAMERAHEADRESET         0xE81C1
/** @todo Document this. */ 
#define A35_TEMPERATURE             0xE83C
/** @todo Document this. */ 
#define A35_TEMPERATUREFPA          0xE840
/** (send 0 to do FFC) */ 
#define A35_COMMAND_DOFFC           0xE938  
/** (Flat Field Correction) */ 
#define A35_FFCMODE                 0xE940  
/** @todo Document this. */ 
#define A35_SENSORVIDEOSTANDARD     0xE9A4
/** @todo Document this. */ 
#define A35_DIGITALOUTPUT           0xE9C8
/** @todo Document this. */ 
#define A35_XPMODE                  0xE9D4
/** @todo Document this. */ 
#define A35_LVDSMODE                0xE9D8
/** @todo Document this. */ 
#define A35_CMOSBITDEPTH            0xE9DC
/** @todo Document this. */ 
#define A35_LVDSBITDEPTH            0xE9E0
/** @todo Document this. */ 
#define A35_IMAGEADJUST				0xE9EC
/** @todo Document this. */ 
#define A35_IPENGINETESTPATTERN     0x80000000

        /** A35 Defined Values */

/** @todo Document this. */
#define A35_NETWORK_OFF             4
/** @todo Document this. */
#define A35_NETWORK_MANUAL          5
/** @todo Document this. */
#define A35_NETWORK_DHCP            6
/** @todo Document this. */
#define A35_PIXELFORMAT_8BIT        0x01080001
/** @todo Document this. */
#define A35_PIXELFORMAT_14BIT       0x01100025
/** @todo Document this. */
#define A35_DIGITALOUTPUT_8BIT      2
/** @todo Document this. */
#define A35_DIGITALOUTPUT_14BIT     3
/** @todo Document this. */
#define A35_SENSORVIDEOSTANDARD_30HZ 0
/** @todo Document this. */
#define A35_SENSORVIDEOSTANDARD_60HZ 4
/** @todo Document this. */
#define A35_FFCMODE_MANUAL          0
/** @todo Document this. */
#define A35_FFCMODE_AUTO            1
/** @todo Document this. */
#define A35_FFCMODE_EXTERNAL             2
/** @todo Document this. */
#define A35_XPMODE_CMOS             2
/** @todo Document this. */
#define A35_SKIP0                   0x00004000
/** @todo Document this. */
#define A35_SKIP1                   0x00014000
/** @todo Document this. */
#define A35_SKIP2                   0x00024000
/** @todo Document this. */
#define A35_SKIP3                   0x00034000
/** @todo Document this. */
#define A35_SKIP4                   0x00044000
/** @todo Document this. */
#define A35_IMAGEADJUST_PLATEAUHISTOGRAM	0
/** @todo Document this. */
#define A35_IMAGEADJUST_ONCEBRIGHT			1
/** @todo Document this. */
#define A35_IMAGEADJUST_AUTOBRIGHT			2
/** @todo Document this. */
#define A35_IMAGEADJUST_MANUAL				3
/** @todo Document this. */
#define A35_IMAGEADJUST_LINEAR				4

        /** @todo Remove commented-out code? */
        //#define NETWORKMASK                 0xFFFFFF00  //255.255.255.0
        
        /**
         * @}
         * @ingroup gige
         * @defgroup gige_pt1000_constants GigE Vision PT1000-CL4 constants
         * @{
         */

        /**
         * @brief 
         * 
         * 
         */
        enum PT1000
            {
            DeviceScanTypeReg = 0xD32C,
            DeviceResetReg = 0xD340,
            SensorWidthReg = 0xA020,
            SensorHeightReg = 0xA024,
            WidthReg = 0xD300,
            HeightReg = 0xD304,
            OffsetXReg = 0xD31C,
            OffsetYReg = 0xD320,
            PixelFormatReg = 0xD308,
            TestImageSelectorReg = 0xD33C,
            AcquisitionModeReg = 0xD310,
            AcquisitionStartReg = 0xD314,
            AcquisitionStopReg = 0xD318,
            AcquisitionFrameCount = 0xD334
            };

        /**
         * @brief 
         * 
         * @todo Document this.
         */
        enum PT1000AcquisitionMode
            {
            Continuous = 0,
            SingleFrame,
            MultiFrame,
            ContinuousRecording,
            ContinuousReadout,
            SingleFrameRecording,
            SingleFrameReadout
            };

        /**
         * @brief 
         * 
         * @todo Document this.
         */
        enum PT1000Format
            {
            Mono8 = 17301505,
            Mono8Signed = 17301506,
            Mono10 = 17825795,
            Mono10Packed = 17563652,
            Mono12 = 17825797,
            Mono12Packed = 17563654,
            Mono14 = 17825829,
            Mono16 = 17825799,
            BayerGR8 = 17301512,
            BayerRG8 = 17301513,
            BayerGB8 = 17301514,
            BayerBG8 = 17301515,
            BayerGR10 = 17825804,
            BayerRG10 = 17825805,
            BayerGB10 = 17825806,
            BayerBG10 = 17825807,
            BayerGR12 = 17825808,
            BayerRG12 = 17825809,
            BayerGB12 = 17825810,
            BayerBG12 = 17825811,
            BayerGR10Packed = 17563686,
            BayerRG10Packed = 17563687,
            BayerGB10Packed = 17563688,
            BayerGR12Packed = 17563690,
            BayerRG12Packed = 17563691,
            BayerGB12Packed = 17563692,
            BayerBG12Packed = 17563693,
            BayerGR16 = 17825838,
            BayerRG16 = 17825839,
            BayerGB16 = 17825840,
            BayerBG16 = 17825841,
            RGB8Packed = 35127316,
            BGR8Packed = 35127317,
            YUV411Packed = 34340894,
            YUV422Packed = 34603039,
            YUV444Packed = 35127328
            };
    }
}

using namespace Cosmos::Devices;

//!	@}


#endif
