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

/*! \file cosmos-errno.cpp
        \brief COSMOS Errno support library source file
*/

#include "support/cosmos-errno.h"

//! \ingroup error
//! \defgroup error_functions Errno support functions
//! @{

//! Errno string
//! Return the string equivalent of a COSMOS Error Number. If the error number is greater than
//! -256, then use strerror.
//! \param cosmos_errno COSMOS Error Number
//! \return String of error number, or empty string.
string cosmos_error_string(int32_t cosmos_errno)
{
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
        switch (cosmos_errno)
        {
        case GS232B_ERROR_OPEN:
            error_string = "GS232B Open";
            break;
        case GS232B_ERROR_CLOSED:
            error_string = "GS232B Device Closed";
            break;
        case GS232B_ERROR_OUTOFRANGE:
            error_string = "GS232B Value Out of Range";
            break;
        case GS232B_ERROR_SEND:
            error_string = "GS232B Send";
            break;
        case MICROSTRAIN_ERROR_OPEN:
            error_string = "MICROSTRAIN_ERROR_OPEN";
            break;
        case MICROSTRAIN_ERROR_CLOSED:
            error_string = "";
            break;
        case MICROSTRAIN_ERROR_OUTOFRANGE:
            error_string = "MICROSTRAIN_ERROR_CLOSED";
            break;
        case MICROSTRAIN_ERROR_FAILED:
            error_string = "MICROSTRAIN_ERROR_FAILED";
            break;
        case MICROSTRAIN_ERROR_TOOMANY:
            error_string = "MICROSTRAIN_ERROR_TOOMANY";
            break;
        case MICROSTRAIN_ERROR_FUNCTION:
            error_string = "MICROSTRAIN_ERROR_FUNCTION";
            break;
        case MICROSTRAIN_ERROR_SEND:
            error_string = "MICROSTRAIN_ERROR_SEND";
            break;
        case SINCLAIR_ERROR_OPEN:
            error_string = "SINCLAIR_ERROR_OPEN";
            break;
        case SINCLAIR_ERROR_CLOSED:
            error_string = "SINCLAIR_ERROR_CLOSED";
            break;
        case SINCLAIR_ERROR_OUTOFRANGE:
            error_string = "SINCLAIR_ERROR_OUTOFRANGE";
            break;
        case SINCLAIR_ERROR_FAILED:
            error_string = "SINCLAIR_ERROR_FAILED";
            break;
        case SINCLAIR_ERROR_TOOMANY:
            error_string = "SINCLAIR_ERROR_TOOMANY";
            break;
        case NSP_ERROR_CRC:
            error_string = "NSP_ERROR_CRC";
            break;
        case NSP_ERROR_SIZE:
            error_string = "NSP_ERROR_SIZE";
            break;
        case NSP_ERROR_NACK :
            error_string = "NSP_ERROR_NACK";
            break;
        case TS2000_ERROR_OPEN:
            error_string = "TS2000_ERROR_OPEN";
            break;
        case TS2000_ERROR_CLOSED:
            error_string = "TS2000_ERROR_CLOSED";
            break;
        case TS2000_ERROR_OUTOFRANGE:
            error_string = "TS2000_ERROR_OUTOFRANGE";
            break;
        case VMT35_ERROR_RODNUM:
            error_string = "VMT35_ERROR_RODNUM";
            break;
        case VMT35_ERROR_BYTE:
            error_string = "VMT35_ERROR_BYTE";
            break;
        case VMT35_ERROR_OUTOFRANGE:
            error_string = "VMT35_ERROR_OUTOFRANGE";
            break;
        case VMT35_ERROR_OPENED:
            error_string = "VMT35_ERROR_OPENED";
            break;
        case VMT35_ERROR_CLOSED:
            error_string = "VMT35_ERROR_CLOSED";
            break;
        case VMT35_ERROR_NACK:
            error_string = "VMT35_ERROR_NACK";
            break;
        case VMT35_ERROR_CRC:
            error_string = "VMT35_ERROR_CRC";
            break;
        case VMT35_ERROR_DEVNAME:
            error_string = "VMT35_ERROR_DEVNAME";
            break;
        case JSON_ERROR_NAME_LENGTH:
            error_string = "JSON_ERROR_NAME_LENGTH";
            break;
        case JSON_ERROR_DATA_LENGTH:
            error_string = "JSON_ERROR_DATA_LENGTH";
            break;
        case JSON_ERROR_INDEX_SIZE:
            error_string = "JSON_ERROR_INDEX_SIZE";
            break;
        case JSON_ERROR_NOJMAP:
            error_string = "JSON_ERROR_NOJMAP";
            break;
        case JSON_ERROR_NOENTRY:
            error_string = "JSON_ERROR_NOENTRY";
            break;
        case JSON_ERROR_EOS:
            error_string = "JSON_ERROR_EOS";
            break;
        case JSON_ERROR_SCAN:
            error_string = "JSON_ERROR_SCAN";
            break;
        case JSON_ERROR_JSTRING:
            error_string = "JSON_ERROR_JSTRING";
            break;
        case JSON_ERROR_NAN:
            error_string = "JSON_ERROR_NAN";
            break;
        case SLIP_ERROR_CRC:
            error_string = "SLIP_ERROR_CRC";
            break;
        case SLIP_ERROR_PACKING:
            error_string = "SLIP_ERROR_PACKING";
            break;
        case SLIP_ERROR_BUFFER:
            error_string = "SLIP_ERROR_BUFFER";
            break;
        case AGENT_ERROR_JSON_CREATE:
            error_string = "AGENT_ERROR_JSON_CREATE";
            break;
        case AGENT_ERROR_REQ_COUNT:
            error_string = "AGENT_ERROR_REQ_COUNT";
            break;
        case AGENT_ERROR_MEMORY:
            error_string = "AGENT_ERROR_MEMORY";
            break;
        case AGENT_ERROR_SOCKET:
            error_string = "AGENT_ERROR_SOCKET";
            break;
        case AGENT_ERROR_CHANNEL:
            error_string = "AGENT_ERROR_CHANNEL";
            break;
        case AGENT_ERROR_BUFLEN:
            error_string = "AGENT_ERROR_BUFLEN";
            break;
        case AGENT_ERROR_NULL:
            error_string = "AGENT_ERROR_NULL";
            break;
        case AGENT_ERROR_DISCOVERY:
            error_string = "AGENT_ERROR_DISCOVERY";
            break;
        case AGENT_ERROR_REQUEST:
            error_string = "AGENT_ERROR_REQUEST";
            break;
        case MATH_ERROR_GJ_UNDEFINED:
            error_string = "MATH_ERROR_GJ_UNDEFINED";
            break;
        case MATH_ERROR_GJ_OUTOFRANGE:
            error_string = "MATH_ERROR_GJ_OUTOFRANGE";
            break;
        case AGENT_ERROR_SERVER_RUNNING:
            error_string = "Agent Server was running in another instance";
            break;
        case AGENT_ERROR_LCM_CREATE:
            error_string = "AGENT_ERROR_LCM_CREATE";
            break;
        case AGENT_ERROR_LCM_SUBSCRIBE:
            error_string = "AGENT_ERROR_LCM_SUBSCRIBE";
            break;
        case AGENT_ERROR_LCM_TIMEOUT:
            error_string = "AGENT_ERROR_LCM_TIMEOUT";
            break;
        case RW_ERROR_OPEN:
            error_string = "RW_ERROR_OPEN";
            break;
        case RW_ERROR_CLOSED:
            error_string = "RW_ERROR_CLOSED";
            break;
        case RW_ERROR_OUTOFRANGE:
            error_string = "RW_ERROR_OUTOFRANGE";
            break;
        case RW_ERROR_FAILED:
            error_string = "RW_ERROR_FAILED";
            break;
        case RW_ERROR_TOOMANY:
            error_string = "RW_ERROR_TOOMANY";
            break;
        case RW_ERROR_SEND:
            error_string = "RW_ERROR_SEND";
            break;
        case STK_ERROR_LOWINDEX:
            error_string = "STK_ERROR_LOWINDEX";
            break;
        case STK_ERROR_HIGHINDEX:
            error_string = "STK_ERROR_HIGHINDEX";
            break;
        case STK_ERROR_OUTOFRANGE:
            error_string = "STK_ERROR_OUTOFRANGE";
            break;
        case STK_ERROR_NOTFOUND:
            error_string = "STK_ERROR_NOTFOUND";
            break;
        case DEM_ERROR_SEMINIT:
            error_string = "DEM_ERROR_SEMINIT";
            break;
        case DEM_ERROR_OPEN:
            error_string = "DEM_ERROR_OPEN";
            break;
        case DEM_ERROR_INSUFFICIENT_MEMORY:
            error_string = "DEM_ERROR_INSUFFICIENT_MEMORY";
            break;
        case DEM_ERROR_NOTFOUND:
            error_string = "DEM_ERROR_NOTFOUND";
            break;
        case TLE_ERROR_OUTOFRANGE:
            error_string = "TLE_ERROR_OUTOFRANGE";
            break;
        case ORBIT_ERROR_NOTSUPPORTED:
            error_string = "ORBIT_ERROR_NOTSUPPORTED";
            break;
        case TLE_ERROR_NOTFOUND:
            error_string = "TLE_ERROR_NOTFOUND";
            break;
        case NODE_ERROR_NODE:
            error_string = "NODE_ERROR_NODE";
            break;
        case NODE_ERROR_GROUNDSTATION:
            error_string = "NODE_ERROR_GROUNDSTATION";
            break;
        case NODE_ERROR_ROOTDIR:
            error_string = "NODE_ERROR_ROOTDIR";
            break;
        case MOST_USER_CANCELLED:
            error_string = "MOST_USER_CANCELLED";
            break;
        case DATA_ERROR_ARCHIVE:
            error_string = "DATA_ERROR_ARCHIVE";
            break;
        case DATA_ERROR_SIZE_MISMATCH:
            error_string = "DATA_ERROR_SIZE_MISMATCH";
            break;
        case DATA_ERROR_CRC:
            error_string = "DATA_ERROR_CRC";
            break;
        case DATA_ERROR_FORMAT:
            error_string = "DATA_ERROR_FORMAT";
            break;
        case DATA_ERROR_RESOURCES_FOLDER:
            error_string = "DATA_ERROR_RESOURCES_FOLDER";
            break;
        case DATA_ERROR_NODES_FOLDER:
            error_string = "DATA_ERROR_NODES_FOLDER";
            break;
        case DATA_ERROR_ROOT_FOLDER:
            error_string = "DATA_ERROR_ROOT_FOLDER";
            break;
        case GIGE_ERROR_NACK:
            error_string = "GIGE_ERROR_NACK";
            break;
        case CSSL_ERROR_BASE:
            error_string = "CSSL_ERROR_BASE";
            break;
        case SERIAL_ERROR_CHECKSUM:
            error_string = "SERIAL_ERROR_CHECKSUM";
            break;
        case SERIAL_ERROR_READ:
            error_string = "SERIAL_ERROR_READ";
            break;
        case SERIAL_ERROR_TIMEOUT:
            error_string = "SERIAL_ERROR_TIMEOUT";
            break;
        case SERIAL_ERROR_EOT:
            error_string = "SERIAL_ERROR_EOT";
            break;
        case SERIAL_ERROR_SLIPIN:
            error_string = "SERIAL_ERROR_SLIPIN";
            break;
        case SERIAL_ERROR_SLIPOUT:
            error_string = "SERIAL_ERROR_SLIPOUT";
            break;
        case SERIAL_ERROR_BUFFER_SIZE_EXCEEDED:
            error_string = "SERIAL_ERROR_BUFFER_SIZE_EXCEEDED";
            break;
        case SERIAL_ERROR_BAD_ESCAPE:
            error_string = "SERIAL_ERROR_BAD_ESCAPE";
            break;
        case SERIAL_ERROR_OPEN:
            error_string = "SERIAL_ERROR_OPEN";
            break;
        case IC9100_ERROR_OPEN:
            error_string = "IC9100_ERROR_OPEN";
            break;
        case IC9100_ERROR_CLOSED:
            error_string = "IC9100_ERROR_CLOSED";
            break;
        case IC9100_ERROR_OUTOFRANGE:
            error_string = "IC9100_ERROR_OUTOFRANGE";
            break;
        case IC9100_ERROR_WRITE:
            error_string = "IC9100_ERROR_WRITE";
            break;
        case IC9100_ERROR_NG:
            error_string = "IC9100_ERROR_NG";
            break;
        case IC9100_ERROR_READ:
            error_string = "IC9100_ERROR_READ";
            break;
        case IC9100_ERROR_ADDR:
            error_string = "IC9100_ERROR_ADDR";
            break;
        case ASTRODEV_ERROR_HEADER_CS:
            error_string = "ASTRODEV_ERROR_HEADER_CS";
            break;
        case ASTRODEV_ERROR_SYNC0:
            error_string = "ASTRODEV_ERROR_SYNC0";
            break;
        case ASTRODEV_ERROR_NACK:
            error_string = "ASTRODEV_ERROR_NACK";
            break;
        case ASTRODEV_ERROR_PAYLOAD_CS:
            error_string = "ASTRODEV_ERROR_PAYLOAD_CS";
            break;
        case ASTRODEV_ERROR_SYNC1:
            error_string = "ASTRODEV_ERROR_SYNC1";
            break;
        case ASTRODEV_ERROR_HEADER:
            error_string = "ASTRODEV_ERROR_HEADER";
            break;
        case ASTRODEV_ERROR_PAYLOAD:
            error_string = "ASTRODEV_ERROR_PAYLOAD";
            break;
        case GPS_ERROR_OPEN:
            error_string = "GPS_ERROR_OPEN";
            break;
        case GPS_ERROR_CLOSED:
            error_string = "GPS_ERROR_CLOSED";
            break;
        case OEMV_ERROR_OPEN:
            error_string = "OEMV_ERROR_OPEN";
            break;
        case OEMV_ERROR_CLOSED:
            error_string = "OEMV_ERROR_CLOSED";
            break;
        case OEMV_ERROR_CRC:
            error_string = "OEMV_ERROR_CRC";
            break;
        case OEMV_ERROR_RESPONSE:
            error_string = "OEMV_ERROR_RESPONSE";
            break;
        case PIC_ERROR_OPEN:
            error_string = "PIC_ERROR_OPEN";
            break;
        case PIC_ERROR_CLOSED:
            error_string = "PIC_ERROR_CLOSED";
            break;
        case PIC_ERROR_PORT:
            error_string = "PIC_ERROR_PORT";
            break;
        case PIC_ERROR_NACK:
            error_string = "PIC_ERROR_NACK";
            break;
        case PIC_ERROR_SIZE:
            error_string = "PIC_ERROR_SIZE";
            break;
        case PIC_ERROR_ERR:
            error_string = "PIC_ERROR_ERR";
            break;
        case PIC_ERROR_TYPE:
            error_string = "PIC_ERROR_TYPE";
            break;
        case SUCHI_ERROR_CLOSED:
            error_string = "SUCHI_ERROR_CLOSED";
            break;
        case VN100_ERROR_OPEN:
            error_string = "VN100_ERROR_OPEN";
            break;
        case VN100_ERROR_NACK:
            error_string = "VN100_ERROR_NACK";
            break;
        case VN100_ERROR_SCAN:
            error_string = "VN100_ERROR_SCAN";
            break;
        case ISC_ERROR_CLOSED:
            error_string = "ISC_ERROR_CLOSED";
            break;
        case TRANSFER_ERROR_MATCH:
            error_string = "TRANSFER_ERROR_MATCH";
            break;
        case TRANSFER_ERROR_QUEUEFULL:
            error_string = "TRANSFER_ERROR_QUEUEFULL";
            break;
        case TRANSFER_ERROR_INDEX:
            error_string = "TRANSFER_ERROR_INDEX";
            break;
        case TRANSFER_ERROR_NODE:
            error_string = "TRANSFER_ERROR_NODE";
            break;
        case SOCKET_ERROR_CS:
            error_string = "SOCKET_ERROR_CS";
            break;
        case SOCKET_ERROR_PROTOCOL:
            error_string = "SOCKET_ERROR_PROTOCOL";
            break;
        case SOCKET_READ_TIMEOUT:
            error_string = "SOCKET_READ_TIMEOUT";
            break;
        case SOCKET_ERROR_OPEN :
            error_string = "SOCKET_ERROR_OPEN";
            break;
        case SOCKET_ERROR_OPEN_RECEIVE:
            error_string = "SOCKET_ERROR_OPEN_RECEIVE";
            break;
        case SOCKET_ERROR_OPEN_TRANSMIT:
            error_string = "SOCKET_ERROR_OPEN_TRANSMIT";
            break;
        case SOCKET_ERROR_OPEN_CONNECT:
            error_string = "SOCKET_ERROR_OPEN_CONNECT";
            break;
        case SOCKET_ERROR_CLOSE:
            error_string = "SOCKET_ERROR_CLOSE";
            break;
        case TRIMBLE_ERROR_CS:
            error_string = "TRIMBLE_ERROR_CS";
            break;
        case JPLEPHEM_ERROR_NOTFOUND:
            error_string = "JPLEPHEM_ERROR_NOTFOUND";
            break;
        case JPLEPHEM_ERROR_INSUFFICIENT_MEMORY:
            error_string = "JPLEPHEM_ERROR_INSUFFICIENT_MEMORY";
            break;
        case JPLEPHEM_ERROR_OUTOFRANGE:
            error_string = "JPLEPHEM_ERROR_OUTOFRANGE";
            break;
        case JPLEPHEM_ERROR_NUTATIONS:
            error_string = "JPLEPHEM_ERROR_NUTATIONS";
            break;
        case JPLEPHEM_ERROR_LIBRATIONS:
            error_string = "JPLEPHEM_ERROR_LIBRATIONS";
            break;
        case GEOMAG_ERROR_NOTFOUND:
            error_string = "GEOMAG_ERROR_NOTFOUND";
            break;
        case GEOMAG_ERROR_OUTOFRANGE:
            error_string = "GEOMAG_ERROR_OUTOFRANGE";
            break;
        case PRKX2SU_ERROR_OPEN:
            error_string = "PRKX2SU_ERROR_OPEN";
            break;
        case PRKX2SU_ERROR_CLOSED:
            error_string = "PRKX2SU_ERROR_CLOSED";
            break;
        case PRKX2SU_ERROR_OUTOFRANGE:
            error_string = "PRKX2SU_ERROR_OUTOFRANGE";
            break;
        case PRKX2SU_ERROR_SEND:
            error_string = "PRKX2SU_ERROR_SEND";
            break;
        case CONVERT_ERROR_UTC:
            error_string = "CONVERT_ERROR_UTC";
            break;
        case CUBEADCS_SERIALCOMM_RESPONSE:
            error_string = "CUBEADCS SERIALCOMM: ADCS REPLY DID NOT MATCH EXPECTED REPLY";
            break;
        case CUBEADCS_INVALID_RESPONSE:
            error_string = "Invalid Protocol Response";
            break;
        case CUBEADCS_NOT_CONNECTED:
            error_string = "Not Actively Connected";
            break;
        case CUBEADCS_SERIAL_PROTOCOL:
            error_string = "Error in Serial Protocol";
            break;
        case CLYDE_EPS_NOT_CONNECTED:
            error_string = "Clyde EPS Not Connected";
            break;
        case CLYDE_BATTERY_NOT_CONNECTED:
            error_string = "Clyde Battery Not Connected";
            break;
        case CLYDE_GENERAL_BAD_ADDRESS:
            error_string = "Clyde Bad Address";
            break;
        case MEADE_MOUNT_ERROR_BAD_RESPONSE:
            error_string = "Meade Mount Bad Address";
            break;
        case MEADE_MOUNT_ERROR_NOT_CONNECTED:
            error_string = "Meade Mount Not Connected";
            break;
        case MEADE_MOUNT_ERROR_VALUE_INVALID:
            error_string = "Meade Mount Value Invalid";
            break;
        case MEADE_MOUNT_ERROR_OUTSIDE_LIMITS:
            error_string = "Meade Mount Request Outside Limits";
            break;
        case SCPI_ERROR_BAD_RESPONSE:
            error_string = "SCPI Bad Address";
            break;
        case SCPI_ERROR_NOT_CONNECTED:
            error_string = "SCPI Not Connected";
            break;
        case SCPI_ERROR_VALUE_INVALID:
            error_string = "SCPI Value Invalid";
            break;
        case SCPI_ERROR_OUTSIDE_LIMITS:
            error_string = "SCPI Request Outside Valid Limits";
            break;
        case SCPI_ERROR_OUTSIDE_RANGE:
            error_string = "SCPI Request Outside Supported Range";
            break;
        case GENERAL_ERROR_BAD_SIZE:
            error_string = "Bad Size";
            break;
        case GENERAL_ERROR_BUSY:
            error_string = "Busy";
            break;
        case GENERAL_ERROR_CRC:
            error_string = "CRC";
            break;
        case GENERAL_ERROR_EMPTY:
            error_string = "Empty";
            break;
        case GENERAL_ERROR_INPUT:
            error_string = "Input";
            break;
        case GENERAL_ERROR_MEMORY:
            error_string = "Memory";
            break;
        case GENERAL_ERROR_NO_ANSWER:
            error_string = "No Answer";
            break;
        case GENERAL_ERROR_NO_CARRIER:
            error_string = "No Carrier";
            break;
        case GENERAL_ERROR_NO_DIALTONE:
            error_string = "No Dial Tone";
            break;
        case GENERAL_ERROR_NO_SIGNAL:
            error_string = "No Signal";
            break;
        case GENERAL_ERROR_NOTSTARTED:
            error_string = "Not Started";
            break;
        case GENERAL_ERROR_NULLPOINTER:
            error_string = "Null Pointer";
            break;
        case GENERAL_ERROR_OOPS:
            error_string = "Oops";
            break;
        case GENERAL_ERROR_OPEN:
            error_string = "Open";
            break;
        case GENERAL_ERROR_OUTOFRANGE:
            error_string = "Out Of Range";
            break;
        case GENERAL_ERROR_OUTPUT:
            error_string = "Output";
            break;
        case GENERAL_ERROR_OVERSIZE:
            error_string = "Over Size";
            break;
        case GENERAL_ERROR_TIMEOUT:
            error_string = "Time Out";
            break;
        case GENERAL_ERROR_UNDEFINED:
            error_string = "Undefined";
            break;
        case GENERAL_ERROR_UNDERSIZE:
            error_string = "Under Size";
            break;
        case GENERAL_ERROR_UNHANDLED_ERR:
            error_string = "Unhandled Error";
            break;
        case GENERAL_ERROR_UNIMPLEMENTED:
            error_string = "Unimplemented";
            break;
        case GENERAL_ERROR_ZEROSIZE:
            error_string = "Zero Size";
            break;
        default:
            char tval[30];
            sprintf(tval,"Undefined Value %d", cosmos_errno);
            error_string = tval;
            break;
        }
    }
    return error_string;
}
//! @}
