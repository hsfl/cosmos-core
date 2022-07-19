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

        int32_t Error::Type(uint16_t type)
        {
            return Set(type, pathName, interval, Extension);
        }

        int32_t Error::Type()
        {
            return type;
        }

        int32_t Error::Set(uint16_t itype, string ipathname, double iinterval, string iextension)
        {
            interval = iinterval;
            Extension = iextension;
            type = itype;
            switch (type)
            {
            case LOG_NONE:
                if (log_fd != nullptr && log_fd != stdout) {
                    fclose(log_fd);
                }
                log_fd = nullptr;
                pathName.clear();
                break;
            case LOG_STDOUT_FAST:
            case LOG_STDOUT_FFLUSH:
                if (log_fd != nullptr && log_fd != stdout) {
                    fclose(log_fd);
                }
                log_fd = stdout;
                pathName.clear();
                break;
            default:
                pathName = ipathname;
                break;
            }
            return type;
        }

        FILE* Error::Open()
        {
            switch (type)
            {
            case LOG_NONE:
                log_fd = nullptr;
                pathName.clear();
                break;
            case LOG_STDOUT_FAST:
            case LOG_STDOUT_FFLUSH:
                if (log_fd != stdout) {
                    if (log_fd != nullptr) {
                        fclose(log_fd);
                    }
                    log_fd = stdout;
                    pathName.clear();
                }
                break;
            default:
                double mjd = currentmjd();
                mjd -= fmod(mjd - floor(mjd), interval);
                if (fabs(mjd - oldmjd) > interval / 2.)
                {
                    string npathName;
                    if (Extension == "log")
                    {
                        npathName = pathName + data_name("", mjd, Extension + to_unsigned(type));
                    }
                    else {
                        npathName = pathName + data_name("", mjd, Extension);
                    }

                    if (log_fd != nullptr) {
                        if (npathName != pathName) {
                            FILE *fd = fopen(npathName.c_str(), "a");
                            if (fd != nullptr) {
                                if (log_fd != stdout) {
                                    fclose(log_fd);
                                }
                                log_fd = fd;
                                pathName = npathName;
                            }
                        }
                    }
                    else {
                        FILE *fd = fopen(npathName.c_str(), "a");
                        if (fd != nullptr) {
                            log_fd = fd;
                            pathName = npathName;
                        }
                    }
                    oldmjd = mjd;
                }
                break;
            }
            return log_fd;
        }

        int32_t Error::Close()
        {
            int32_t iretn;
            if (log_fd != nullptr && log_fd != stdout)
            {
                iretn = fclose(log_fd);
                if (iretn != 0) { return -errno; }
                log_fd = nullptr;
            }
            return 0;
        }

        int32_t Error::Printf(string output)
        {
            return Printf(output.c_str());
        }

        int32_t Error::Printf(const char* fmt, ...)
        {
            int32_t iretn = 0;

            Open();
            if (log_fd != nullptr)
            {
                va_list args;
                va_start(args, fmt);
                iretn = vfprintf(log_fd, fmt, args);
                va_end(args);
                switch (type)
                {
                case LOG_STDOUT_FFLUSH:
                case LOG_FILE_FFLUSH:
                    fflush(log_fd);
                    break;
                case LOG_FILE_CLOSE:
                    fclose(log_fd);
                    break;
                default:
                    break;
                }
            }

            return iretn;
        }


        static uint16_t debug_level = 0;
        static FILE* debug_fd = nullptr;
        static string debug_pathName;

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
                case TRANSFER_ERROR_FILENAME:
                    error_string = "TRANSFER_ERROR_FILENAME";
                    break;
                case TRANSFER_ERROR_DUPLICATE:
                    error_string = "TRANSFER_ERROR_DUPLICATE";
                    break;
                case TRANSFER_ERROR_FILESIZE:
                    error_string = "Bad File Size";
                    break;
                case TRANSFER_ERROR_FILEZERO:
                    error_string = "Zero File Size";
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
                case GENERAL_OK:
                    error_string = "No Problem";
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
                case GENERAL_ERROR_MEMORY:
                    error_string = "Memory";
                    break;
                case GENERAL_ERROR_OPEN:
                    error_string = "Open";
                    break;
                case GENERAL_ERROR_CRC:
                    error_string = "CRC";
                    break;
                case GENERAL_ERROR_OVERSIZE:
                    error_string = "Over Size";
                    break;
                case GENERAL_ERROR_ZEROSIZE:
                    error_string = "Zero Size";
                    break;
                case GENERAL_ERROR_UNIMPLEMENTED:
                    error_string = "Unimplemented";
                    break;
                case GENERAL_ERROR_EMPTY:
                    error_string = "Empty";
                    break;
                case GENERAL_ERROR_UNHANDLED_ERR:
                    error_string = "Unhandled Error";
                    break;
                case GENERAL_ERROR_UNDERSIZE:
                    error_string = "Under Size";
                    break;
                case GENERAL_ERROR_BAD_SIZE:
                    error_string = "Bad Size";
                    break;
                case GENERAL_ERROR_TIMEOUT:
                    error_string = "Time Out";
                    break;
                case GENERAL_ERROR_INPUT:
                    error_string = "Input";
                    break;
                case GENERAL_ERROR_OUTPUT:
                    error_string = "Output";
                    break;
                case GENERAL_ERROR_UNDEFINED:
                    error_string = "Undefined";
                    break;
                case GENERAL_ERROR_OUTOFRANGE:
                    error_string = "Out Of Range";
                    break;
                case GENERAL_ERROR_NOTREADY:
                    error_string = "Not Ready";
                    break;
                case GENERAL_ERROR_BUSY:
                    error_string = "Busy";
                    break;
                case GENERAL_ERROR_NO_CARRIER:
                    error_string = "No Carrier";
                    break;
                case GENERAL_ERROR_NO_DIALTONE:
                    error_string = "No Dial Tone";
                    break;
                case GENERAL_ERROR_NO_ANSWER:
                    error_string = "No Answer";
                    break;
                case GENERAL_ERROR_DUPLICATE:
                    error_string = "Duplicate";
                    break;
                case GENERAL_ERROR_MISMATCH:
                    error_string = "Mismatch";
                    break;
                case GENERAL_ERROR_ERROR:
                    error_string = "General Error";
                    break;
                case GENERAL_ERROR_ARGS:
                    error_string = "Wrong Arguments";
                    break;
                case GENERAL_ERROR_TOO_LOW:
                    error_string = "Value Too Low";
                    break;
                case GENERAL_ERROR_POSITION:
                    error_string = "Improper Position";
                    break;
                case GENERAL_ERROR_ATTITUDE:
                    error_string = "Improper Attitude";
                    break;
                case GENERAL_ERROR_NAME:
                    error_string = "Bad File or Device Name";
                    break;
                case GENERAL_ERROR_BAD_FD:
                    error_string = "Bad or No File Descriptor";
                    break;
                case GENERAL_ERROR_BAD_ACK:
                    error_string = "Bad Acknowledgement";
                    break;
                case GENERAL_ERROR_EOF:
                    error_string = "End of File";
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

        int32_t set_cosmos_error_level(uint16_t level, string path)
        {
            if (level == 0) {
                if (debug_fd != nullptr && debug_fd != stdout) {
                    fclose(debug_fd);
                }
                debug_fd = nullptr;
                debug_pathName.clear();
            }
            else if (level == 1) {
                if (debug_fd != stdout) {
                    if (debug_fd != nullptr) {
                        fclose(debug_fd);
                    }
                    debug_fd = stdout;
                }
                debug_pathName.clear();
            } else {
                debug_pathName = path;
            }
            debug_level = level;
            return level;
        }

        FILE* get_cosmos_error_fd(double mjd) {
//            static double oldmjd=0.;
            if (debug_level == 0) {
                debug_fd = nullptr;
                debug_pathName.clear();
            }
            else if (debug_level == 1) {
                if (debug_fd != stdout) {
                    if (debug_fd != nullptr) {
                        fclose(debug_fd);
                    }
                    debug_fd = stdout;
                    debug_pathName.clear();
                }
            } else {
                if (mjd == 0.) {
                    mjd = currentmjd();
//                    oldmjd = mjd;
                }
                mjd = mjd - fmod(mjd, 1./24.);
                string pathName = debug_pathName + data_name("", mjd, "log"+to_unsigned(debug_level));

                if (debug_fd != nullptr) {
                    if (pathName != debug_pathName) {
                        FILE *fd = fopen(pathName.c_str(), "a");
                        if (fd != nullptr) {
                            if (debug_fd != stdout) {
                                fclose(debug_fd);
                            }
                            debug_fd = fd;
                            debug_pathName = pathName;
                        }
//                        oldmjd = mjd;
                    }
                } else {
                    FILE *fd = fopen(pathName.c_str(), "a");
                    if (fd != nullptr) {
                        debug_fd = fd;
                        debug_pathName = pathName;
                    }
                }
            }
            return debug_fd;
        }

        int32_t close_cosmos_error_fd() {
            int32_t iretn;
            if (debug_fd != nullptr && debug_fd != stdout)
            {
                iretn = fclose(debug_fd);
                if (iretn != 0) { return -errno; }
                debug_fd = nullptr;
            }
            return 0;
        }
    }
}


//! @}
