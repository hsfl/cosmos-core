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

/*! \file cosmos-errclass.cpp
        \brief COSMOS Errno support library source file
*/

#include "support/cosmos-errclass.h"
#include "support/timelib.h"
#include "support/stringlib.h"
#include "support/datalib.h"
#include <stdarg.h>

namespace Cosmos {
    namespace Support {
        Error::Error()
        {
            ErrorStrings[ErrorNumbers::COSMOS_GS232B_ERROR_OPEN] = "GS232B Open";
            ErrorStrings[ErrorNumbers::COSMOS_GS232B_ERROR_CLOSED] = "GS232B Device Closed";
            ErrorStrings[ErrorNumbers::COSMOS_GS232B_ERROR_OUTOFRANGE] = "GS232B Value Out of Range";
            ErrorStrings[ErrorNumbers::COSMOS_GS232B_ERROR_SEND] = "GS232B Send";
            ErrorStrings[ErrorNumbers::COSMOS_MICROSTRAIN_ERROR_OPEN] = "MICROSTRAIN_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_MICROSTRAIN_ERROR_CLOSED] = "";
            ErrorStrings[ErrorNumbers::COSMOS_MICROSTRAIN_ERROR_OUTOFRANGE] = "MICROSTRAIN_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_MICROSTRAIN_ERROR_FAILED] = "MICROSTRAIN_ERROR_FAILED";
            ErrorStrings[ErrorNumbers::COSMOS_MICROSTRAIN_ERROR_TOOMANY] = "MICROSTRAIN_ERROR_TOOMANY";
            ErrorStrings[ErrorNumbers::COSMOS_MICROSTRAIN_ERROR_FUNCTION] = "MICROSTRAIN_ERROR_FUNCTION";
            ErrorStrings[ErrorNumbers::COSMOS_MICROSTRAIN_ERROR_SEND] = "MICROSTRAIN_ERROR_SEND";
            ErrorStrings[ErrorNumbers::COSMOS_SINCLAIR_ERROR_OPEN] = "SINCLAIR_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_SINCLAIR_ERROR_CLOSED] = "SINCLAIR_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_SINCLAIR_ERROR_OUTOFRANGE] = "SINCLAIR_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_SINCLAIR_ERROR_FAILED] = "SINCLAIR_ERROR_FAILED";
            ErrorStrings[ErrorNumbers::COSMOS_SINCLAIR_ERROR_TOOMANY] = "SINCLAIR_ERROR_TOOMANY";
            ErrorStrings[ErrorNumbers::COSMOS_NSP_ERROR_CRC] = "NSP_ERROR_CRC";
            ErrorStrings[ErrorNumbers::COSMOS_NSP_ERROR_SIZE] = "NSP_ERROR_SIZE";
            ErrorStrings[ErrorNumbers::COSMOS_NSP_ERROR_NACK ] = "NSP_ERROR_NACK";
            ErrorStrings[ErrorNumbers::COSMOS_TS2000_ERROR_OPEN] = "TS2000_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_TS2000_ERROR_CLOSED] = "TS2000_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_TS2000_ERROR_OUTOFRANGE] = "TS2000_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_VMT35_ERROR_RODNUM] = "VMT35_ERROR_RODNUM";
            ErrorStrings[ErrorNumbers::COSMOS_VMT35_ERROR_BYTE] = "VMT35_ERROR_BYTE";
            ErrorStrings[ErrorNumbers::COSMOS_VMT35_ERROR_OUTOFRANGE] = "VMT35_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_VMT35_ERROR_OPENED] = "VMT35_ERROR_OPENED";
            ErrorStrings[ErrorNumbers::COSMOS_VMT35_ERROR_CLOSED] = "VMT35_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_VMT35_ERROR_NACK] = "VMT35_ERROR_NACK";
            ErrorStrings[ErrorNumbers::COSMOS_VMT35_ERROR_CRC] = "VMT35_ERROR_CRC";
            ErrorStrings[ErrorNumbers::COSMOS_VMT35_ERROR_DEVNAME] = "VMT35_ERROR_DEVNAME";
            ErrorStrings[ErrorNumbers::COSMOS_JSON_ERROR_NAME_LENGTH] = "JSON_ERROR_NAME_LENGTH";
            ErrorStrings[ErrorNumbers::COSMOS_JSON_ERROR_DATA_LENGTH] = "JSON_ERROR_DATA_LENGTH";
            ErrorStrings[ErrorNumbers::COSMOS_JSON_ERROR_INDEX_SIZE] = "JSON_ERROR_INDEX_SIZE";
            ErrorStrings[ErrorNumbers::COSMOS_JSON_ERROR_NOJMAP] = "JSON_ERROR_NOJMAP";
            ErrorStrings[ErrorNumbers::COSMOS_JSON_ERROR_NOENTRY] = "JSON_ERROR_NOENTRY";
            ErrorStrings[ErrorNumbers::COSMOS_JSON_ERROR_EOS] = "JSON_ERROR_EOS";
            ErrorStrings[ErrorNumbers::COSMOS_JSON_ERROR_SCAN] = "JSON_ERROR_SCAN";
            ErrorStrings[ErrorNumbers::COSMOS_JSON_ERROR_JSTRING] = "JSON_ERROR_JSTRING";
            ErrorStrings[ErrorNumbers::COSMOS_JSON_ERROR_NAN] = "JSON_ERROR_NAN";
            ErrorStrings[ErrorNumbers::COSMOS_SLIP_ERROR_CRC] = "SLIP_ERROR_CRC";
            ErrorStrings[ErrorNumbers::COSMOS_SLIP_ERROR_PACKING] = "SLIP_ERROR_PACKING";
            ErrorStrings[ErrorNumbers::COSMOS_SLIP_ERROR_BUFFER] = "SLIP_ERROR_BUFFER";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_JSON_CREATE] = "AGENT_ERROR_JSON_CREATE";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_REQ_COUNT] = "AGENT_ERROR_REQ_COUNT";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_MEMORY] = "AGENT_ERROR_MEMORY";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_SOCKET] = "AGENT_ERROR_SOCKET";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_CHANNEL] = "AGENT_ERROR_CHANNEL";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_BUFLEN] = "AGENT_ERROR_BUFLEN";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_NULL] = "AGENT_ERROR_NULL";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_DISCOVERY] = "AGENT_ERROR_DISCOVERY";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_REQUEST] = "AGENT_ERROR_REQUEST";
            ErrorStrings[ErrorNumbers::COSMOS_MATH_ERROR_GJ_UNDEFINED] = "MATH_ERROR_GJ_UNDEFINED";
            ErrorStrings[ErrorNumbers::COSMOS_MATH_ERROR_GJ_OUTOFRANGE] = "MATH_ERROR_GJ_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_SERVER_RUNNING] = "Agent Server was running in another instance";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_LCM_CREATE] = "AGENT_ERROR_LCM_CREATE";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_LCM_SUBSCRIBE] = "AGENT_ERROR_LCM_SUBSCRIBE";
            ErrorStrings[ErrorNumbers::COSMOS_AGENT_ERROR_LCM_TIMEOUT] = "AGENT_ERROR_LCM_TIMEOUT";
            ErrorStrings[ErrorNumbers::COSMOS_RW_ERROR_OPEN] = "RW_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_RW_ERROR_CLOSED] = "RW_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_RW_ERROR_OUTOFRANGE] = "RW_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_RW_ERROR_FAILED] = "RW_ERROR_FAILED";
            ErrorStrings[ErrorNumbers::COSMOS_RW_ERROR_TOOMANY] = "RW_ERROR_TOOMANY";
            ErrorStrings[ErrorNumbers::COSMOS_RW_ERROR_SEND] = "RW_ERROR_SEND";
            ErrorStrings[ErrorNumbers::COSMOS_STK_ERROR_LOWINDEX] = "STK_ERROR_LOWINDEX";
            ErrorStrings[ErrorNumbers::COSMOS_STK_ERROR_HIGHINDEX] = "STK_ERROR_HIGHINDEX";
            ErrorStrings[ErrorNumbers::COSMOS_STK_ERROR_OUTOFRANGE] = "STK_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_STK_ERROR_NOTFOUND] = "STK_ERROR_NOTFOUND";
            ErrorStrings[ErrorNumbers::COSMOS_DEM_ERROR_SEMINIT] = "DEM_ERROR_SEMINIT";
            ErrorStrings[ErrorNumbers::COSMOS_DEM_ERROR_OPEN] = "DEM_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_DEM_ERROR_INSUFFICIENT_MEMORY] = "DEM_ERROR_INSUFFICIENT_MEMORY";
            ErrorStrings[ErrorNumbers::COSMOS_DEM_ERROR_NOTFOUND] = "DEM_ERROR_NOTFOUND";
            ErrorStrings[ErrorNumbers::COSMOS_TLE_ERROR_OUTOFRANGE] = "TLE_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_ORBIT_ERROR_NOTSUPPORTED] = "ORBIT_ERROR_NOTSUPPORTED";
            ErrorStrings[ErrorNumbers::COSMOS_TLE_ERROR_NOTFOUND] = "TLE_ERROR_NOTFOUND";
            ErrorStrings[ErrorNumbers::COSMOS_NODE_ERROR_NODE] = "NODE_ERROR_NODE";
            ErrorStrings[ErrorNumbers::COSMOS_NODE_ERROR_GROUNDSTATION] = "NODE_ERROR_GROUNDSTATION";
            ErrorStrings[ErrorNumbers::COSMOS_NODE_ERROR_ROOTDIR] = "NODE_ERROR_ROOTDIR";
            ErrorStrings[ErrorNumbers::COSMOS_MOST_USER_CANCELLED] = "MOST_USER_CANCELLED";
            ErrorStrings[ErrorNumbers::COSMOS_DATA_ERROR_ARCHIVE] = "DATA_ERROR_ARCHIVE";
            ErrorStrings[ErrorNumbers::COSMOS_DATA_ERROR_SIZE_MISMATCH] = "DATA_ERROR_SIZE_MISMATCH";
            ErrorStrings[ErrorNumbers::COSMOS_DATA_ERROR_CRC] = "DATA_ERROR_CRC";
            ErrorStrings[ErrorNumbers::COSMOS_DATA_ERROR_FORMAT] = "DATA_ERROR_FORMAT";
            ErrorStrings[ErrorNumbers::COSMOS_DATA_ERROR_RESOURCES_FOLDER] = "DATA_ERROR_RESOURCES_FOLDER";
            ErrorStrings[ErrorNumbers::COSMOS_DATA_ERROR_NODES_FOLDER] = "DATA_ERROR_NODES_FOLDER";
            ErrorStrings[ErrorNumbers::COSMOS_DATA_ERROR_ROOT_FOLDER] = "DATA_ERROR_ROOT_FOLDER";
            ErrorStrings[ErrorNumbers::COSMOS_GIGE_ERROR_NACK] = "GIGE_ERROR_NACK";
            ErrorStrings[ErrorNumbers::COSMOS_CSSL_ERROR_BASE] = "CSSL_ERROR_BASE";
            ErrorStrings[ErrorNumbers::COSMOS_SERIAL_ERROR_CHECKSUM] = "SERIAL_ERROR_CHECKSUM";
            ErrorStrings[ErrorNumbers::COSMOS_SERIAL_ERROR_READ] = "SERIAL_ERROR_READ";
            ErrorStrings[ErrorNumbers::COSMOS_SERIAL_ERROR_TIMEOUT] = "SERIAL_ERROR_TIMEOUT";
            ErrorStrings[ErrorNumbers::COSMOS_SERIAL_ERROR_EOT] = "SERIAL_ERROR_EOT";
            ErrorStrings[ErrorNumbers::COSMOS_SERIAL_ERROR_SLIPIN] = "SERIAL_ERROR_SLIPIN";
            ErrorStrings[ErrorNumbers::COSMOS_SERIAL_ERROR_SLIPOUT] = "SERIAL_ERROR_SLIPOUT";
            ErrorStrings[ErrorNumbers::COSMOS_SERIAL_ERROR_BUFFER_SIZE_EXCEEDED] = "SERIAL_ERROR_BUFFER_SIZE_EXCEEDED";
            ErrorStrings[ErrorNumbers::COSMOS_SERIAL_ERROR_BAD_ESCAPE] = "SERIAL_ERROR_BAD_ESCAPE";
            ErrorStrings[ErrorNumbers::COSMOS_SERIAL_ERROR_OPEN] = "SERIAL_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_IC9100_ERROR_OPEN] = "IC9100_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_IC9100_ERROR_CLOSED] = "IC9100_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_IC9100_ERROR_OUTOFRANGE] = "IC9100_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_IC9100_ERROR_WRITE] = "IC9100_ERROR_WRITE";
            ErrorStrings[ErrorNumbers::COSMOS_IC9100_ERROR_NG] = "IC9100_ERROR_NG";
            ErrorStrings[ErrorNumbers::COSMOS_IC9100_ERROR_READ] = "IC9100_ERROR_READ";
            ErrorStrings[ErrorNumbers::COSMOS_IC9100_ERROR_ADDR] = "IC9100_ERROR_ADDR";
            ErrorStrings[ErrorNumbers::COSMOS_ASTRODEV_ERROR_HEADER_CS] = "ASTRODEV_ERROR_HEADER_CS";
            ErrorStrings[ErrorNumbers::COSMOS_ASTRODEV_ERROR_SYNC0] = "ASTRODEV_ERROR_SYNC0";
            ErrorStrings[ErrorNumbers::COSMOS_ASTRODEV_ERROR_NACK] = "ASTRODEV_ERROR_NACK";
            ErrorStrings[ErrorNumbers::COSMOS_ASTRODEV_ERROR_PAYLOAD_CS] = "ASTRODEV_ERROR_PAYLOAD_CS";
            ErrorStrings[ErrorNumbers::COSMOS_ASTRODEV_ERROR_SYNC1] = "ASTRODEV_ERROR_SYNC1";
            ErrorStrings[ErrorNumbers::COSMOS_ASTRODEV_ERROR_HEADER] = "ASTRODEV_ERROR_HEADER";
            ErrorStrings[ErrorNumbers::COSMOS_ASTRODEV_ERROR_PAYLOAD] = "ASTRODEV_ERROR_PAYLOAD";
            ErrorStrings[ErrorNumbers::COSMOS_GPS_ERROR_OPEN] = "GPS_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_GPS_ERROR_CLOSED] = "GPS_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_OEMV_ERROR_OPEN] = "OEMV_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_OEMV_ERROR_CLOSED] = "OEMV_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_OEMV_ERROR_CRC] = "OEMV_ERROR_CRC";
            ErrorStrings[ErrorNumbers::COSMOS_OEMV_ERROR_RESPONSE] = "OEMV_ERROR_RESPONSE";
            ErrorStrings[ErrorNumbers::COSMOS_PIC_ERROR_OPEN] = "PIC_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_PIC_ERROR_CLOSED] = "PIC_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_PIC_ERROR_PORT] = "PIC_ERROR_PORT";
            ErrorStrings[ErrorNumbers::COSMOS_PIC_ERROR_NACK] = "PIC_ERROR_NACK";
            ErrorStrings[ErrorNumbers::COSMOS_PIC_ERROR_SIZE] = "PIC_ERROR_SIZE";
            ErrorStrings[ErrorNumbers::COSMOS_PIC_ERROR_ERR] = "PIC_ERROR_ERR";
            ErrorStrings[ErrorNumbers::COSMOS_PIC_ERROR_TYPE] = "PIC_ERROR_TYPE";
            ErrorStrings[ErrorNumbers::COSMOS_SUCHI_ERROR_CLOSED] = "SUCHI_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_VN100_ERROR_OPEN] = "VN100_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_VN100_ERROR_NACK] = "VN100_ERROR_NACK";
            ErrorStrings[ErrorNumbers::COSMOS_VN100_ERROR_SCAN] = "VN100_ERROR_SCAN";
            ErrorStrings[ErrorNumbers::COSMOS_ISC_ERROR_CLOSED] = "ISC_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_TRANSFER_ERROR_MATCH] = "TRANSFER_ERROR_MATCH";
            ErrorStrings[ErrorNumbers::COSMOS_TRANSFER_ERROR_QUEUEFULL] = "TRANSFER_ERROR_QUEUEFULL";
            ErrorStrings[ErrorNumbers::COSMOS_TRANSFER_ERROR_INDEX] = "TRANSFER_ERROR_INDEX";
            ErrorStrings[ErrorNumbers::COSMOS_TRANSFER_ERROR_NODE] = "TRANSFER_ERROR_NODE";
            ErrorStrings[ErrorNumbers::COSMOS_TRANSFER_ERROR_FILENAME] = "TRANSFER_ERROR_FILENAME";
            ErrorStrings[ErrorNumbers::COSMOS_TRANSFER_ERROR_DUPLICATE] = "TRANSFER_ERROR_DUPLICATE";
            ErrorStrings[ErrorNumbers::COSMOS_TRANSFER_ERROR_FILESIZE] = "Bad File Size";
            ErrorStrings[ErrorNumbers::COSMOS_TRANSFER_ERROR_FILEZERO] = "Zero File Size";
            ErrorStrings[ErrorNumbers::COSMOS_TRANSFER_ERROR_VERSION] = "Transfer protocol version mismatch";
            ErrorStrings[ErrorNumbers::COSMOS_SOCKET_ERROR_CS] = "SOCKET_ERROR_CS";
            ErrorStrings[ErrorNumbers::COSMOS_SOCKET_ERROR_PROTOCOL] = "SOCKET_ERROR_PROTOCOL";
            ErrorStrings[ErrorNumbers::COSMOS_SOCKET_READ_TIMEOUT] = "SOCKET_READ_TIMEOUT";
            ErrorStrings[ErrorNumbers::COSMOS_SOCKET_ERROR_OPEN ] = "SOCKET_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_SOCKET_ERROR_OPEN_RECEIVE] = "SOCKET_ERROR_OPEN_RECEIVE";
            ErrorStrings[ErrorNumbers::COSMOS_SOCKET_ERROR_OPEN_TRANSMIT] = "SOCKET_ERROR_OPEN_TRANSMIT";
            ErrorStrings[ErrorNumbers::COSMOS_SOCKET_ERROR_OPEN_CONNECT] = "SOCKET_ERROR_OPEN_CONNECT";
            ErrorStrings[ErrorNumbers::COSMOS_SOCKET_ERROR_CLOSE] = "SOCKET_ERROR_CLOSE";
            ErrorStrings[ErrorNumbers::COSMOS_TRIMBLE_ERROR_CS] = "TRIMBLE_ERROR_CS";
            ErrorStrings[ErrorNumbers::COSMOS_JPLEPHEM_ERROR_NOTFOUND] = "JPLEPHEM_ERROR_NOTFOUND";
            ErrorStrings[ErrorNumbers::COSMOS_JPLEPHEM_ERROR_INSUFFICIENT_MEMORY] = "JPLEPHEM_ERROR_INSUFFICIENT_MEMORY";
            ErrorStrings[ErrorNumbers::COSMOS_JPLEPHEM_ERROR_OUTOFRANGE] = "JPLEPHEM_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_JPLEPHEM_ERROR_NUTATIONS] = "JPLEPHEM_ERROR_NUTATIONS";
            ErrorStrings[ErrorNumbers::COSMOS_JPLEPHEM_ERROR_LIBRATIONS] = "JPLEPHEM_ERROR_LIBRATIONS";
            ErrorStrings[ErrorNumbers::COSMOS_GEOMAG_ERROR_NOTFOUND] = "GEOMAG_ERROR_NOTFOUND";
            ErrorStrings[ErrorNumbers::COSMOS_GEOMAG_ERROR_OUTOFRANGE] = "GEOMAG_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_PRKX2SU_ERROR_OPEN] = "PRKX2SU_ERROR_OPEN";
            ErrorStrings[ErrorNumbers::COSMOS_PRKX2SU_ERROR_CLOSED] = "PRKX2SU_ERROR_CLOSED";
            ErrorStrings[ErrorNumbers::COSMOS_PRKX2SU_ERROR_OUTOFRANGE] = "PRKX2SU_ERROR_OUTOFRANGE";
            ErrorStrings[ErrorNumbers::COSMOS_PRKX2SU_ERROR_SEND] = "PRKX2SU_ERROR_SEND";
            ErrorStrings[ErrorNumbers::COSMOS_CONVERT_ERROR_UTC] = "CONVERT_ERROR_UTC";
            ErrorStrings[ErrorNumbers::COSMOS_CUBEADCS_SERIALCOMM_RESPONSE] = "CUBEADCS SERIALCOMM: ADCS REPLY DID NOT MATCH EXPECTED REPLY";
            ErrorStrings[ErrorNumbers::COSMOS_CUBEADCS_INVALID_RESPONSE] = "Invalid Protocol Response";
            ErrorStrings[ErrorNumbers::COSMOS_CUBEADCS_NOT_CONNECTED] = "Not Actively Connected";
            ErrorStrings[ErrorNumbers::COSMOS_CUBEADCS_SERIAL_PROTOCOL] = "Error in Serial Protocol";
            ErrorStrings[ErrorNumbers::COSMOS_CLYDE_EPS_NOT_CONNECTED] = "Clyde EPS Not Connected";
            ErrorStrings[ErrorNumbers::COSMOS_CLYDE_BATTERY_NOT_CONNECTED] = "Clyde Battery Not Connected";
            ErrorStrings[ErrorNumbers::COSMOS_CLYDE_GENERAL_BAD_ADDRESS] = "Clyde Bad Address";
            ErrorStrings[ErrorNumbers::COSMOS_MEADE_MOUNT_ERROR_BAD_RESPONSE] = "Meade Mount Bad Address";
            ErrorStrings[ErrorNumbers::COSMOS_MEADE_MOUNT_ERROR_NOT_CONNECTED] = "Meade Mount Not Connected";
            ErrorStrings[ErrorNumbers::COSMOS_MEADE_MOUNT_ERROR_VALUE_INVALID] = "Meade Mount Value Invalid";
            ErrorStrings[ErrorNumbers::COSMOS_MEADE_MOUNT_ERROR_OUTSIDE_LIMITS] = "Meade Mount Request Outside Limits";
            ErrorStrings[ErrorNumbers::COSMOS_SCPI_ERROR_BAD_RESPONSE] = "SCPI Bad Address";
            ErrorStrings[ErrorNumbers::COSMOS_SCPI_ERROR_NOT_CONNECTED] = "SCPI Not Connected";
            ErrorStrings[ErrorNumbers::COSMOS_SCPI_ERROR_VALUE_INVALID] = "SCPI Value Invalid";
            ErrorStrings[ErrorNumbers::COSMOS_SCPI_ERROR_OUTSIDE_LIMITS] = "SCPI Request Outside Valid Limits";
            ErrorStrings[ErrorNumbers::COSMOS_SCPI_ERROR_OUTSIDE_RANGE] = "SCPI Request Outside Supported Range";
            ErrorStrings[ErrorNumbers::COSMOS_PACKET_TYPE_MISMATCH] = "Packet Type ID not handled";
            ErrorStrings[ErrorNumbers::COSMOS_DECRYPTION_ERROR] = "Decryption failed";
            ErrorStrings[ErrorNumbers::COSMOS_ENCRYPTION_ERROR] = "Encryption failed";
            ErrorStrings[ErrorNumbers::COSMOS_RADIO_FILLER_PACKET] = "Radio filler";
            ErrorStrings[ErrorNumbers::COSMOS_RADIO_SYNC_WORD_ERROR] = "Sync word not found";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_OK] = "No Problem";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_NO_SIGNAL] = "No Signal";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_NOTSTARTED] = "Not Started";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_NULLPOINTER] = "Null Pointer";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_OOPS] = "Oops";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_MEMORY] = "Memory";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_OPEN] = "Open";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_CRC] = "CRC";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_OVERSIZE] = "Over Size";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_ZEROSIZE] = "Zero Size";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_UNIMPLEMENTED] = "Unimplemented";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_EMPTY] = "Empty";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_UNHANDLED_ERR] = "Unhandled Error";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_UNDERSIZE] = "Under Size";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_BAD_SIZE] = "Bad Size";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_TIMEOUT] = "Time Out";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_INPUT] = "Input";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_OUTPUT] = "Output";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_UNDEFINED] = "Undefined";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_OUTOFRANGE] = "Out Of Range";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_NOTREADY] = "Not Ready";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_BUSY] = "Busy";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_NO_CARRIER] = "No Carrier";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_NO_DIALTONE] = "No Dial Tone";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_NO_ANSWER] = "No Answer";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_DUPLICATE] = "Duplicate";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_MISMATCH] = "Mismatch";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_ERROR] = "General Error";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_ARGS] = "Wrong Arguments";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_TOO_LOW] = "Value Too Low";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_POSITION] = "Improper Position";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_ATTITUDE] = "Improper Attitude";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_NAME] = "Bad File or Device Name";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_BAD_FD] = "Bad or No File Descriptor";
            ErrorStrings[ErrorNumbers::COSMOS_GENERAL_ERROR_BAD_ACK] = "Bad Acknowledgement";

            ErrorStrings[ErrorNumbers::COSMOS_ISIS_CMD_REJECTED] = "COSMOS_ISIS_CMD_REJECTED";
            ErrorStrings[ErrorNumbers::COSMOS_ISIS_INVALID_COMMAND_CODE] = "COSMOS_ISIS_INVALID_COMMAND_CODE";
            ErrorStrings[ErrorNumbers::COSMOS_ISIS_PARAM_MISSING] = "COSMOS_ISIS_PARAM_MISSING";
            ErrorStrings[ErrorNumbers::COSMOS_ISIS_PARAM_INVALID] = "COSMOS_ISIS_PARAM_INVALID";
            ErrorStrings[ErrorNumbers::COSMOS_ISIS_UNAVAILABLE_IN_CURRENT_MODE] = "COSMOS_ISIS_UNAVAILABLE_IN_CURRENT_MODE";
            ErrorStrings[ErrorNumbers::COSMOS_ISIS_INVALID_SYSTYPE_OR_IVID_OR_BID] = "COSMOS_ISIS_INVALID_SYSTYPE_OR_IVID_OR_BID";
            ErrorStrings[ErrorNumbers::COSMOS_ISIS_INTERNAL_ERROR] = "COSMOS_ISIS_INTERNAL_ERROR";

        }

        string Error::ErrorString(int16_t number)
        {
            string error_string="";

            if (number > -256)
            {
                if (number >= 0)
                {
                    error_string = "Success";
                }
                else
                {
                    char *unix_error_string = strerror(-number);
                    if (unix_error_string != nullptr)
                    {
                        error_string = unix_error_string;
                    }
                }
            }
            else {
                ErrorMap::iterator mit;
                if ((mit=ErrorStrings.find(number)) != ErrorStrings.end())
                {
                    error_string = mit->second;
                }
            }
            return error_string;
        }

        //! \ingroup error
        //! \defgroup error_functions Errno support functions
        //! @{

        //! Errno string
        //! Return the string equivalent of a COSMOS Error Number. If the error number is greater than
        //! -256, then use strerror.
        //! \param cosmos_errno COSMOS Error Number
        //! \param message Additional message to prepend to string
        //! \return String of error number, or empty string.
        string cosmos_error_string(int32_t cosmos_errno, string message)
        {
            static Error Err;
            string error_string="";

            if (cosmos_errno > -256)
            {
                if (cosmos_errno >= 0)
                {
                    error_string = "Success";
                }
                else
                {
                    char *unix_error_string = strerror(-cosmos_errno);
                    if (unix_error_string != nullptr)
                    {
                        error_string = unix_error_string;
                    }
                }
            }
            else
            {
                error_string = Err.ErrorString(cosmos_errno);
            }
            if (message.empty())
            {
            return error_string;
            }
            else
            {
                return message + ": " + error_string;
            }
        }
    }
}


//! @}
