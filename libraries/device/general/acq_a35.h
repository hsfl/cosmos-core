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

//////////////////////////////////////////////////////////////////////
//
//	acq_a35.h : header file
//
//////////////////////////////////////////////////////////////////////

#include "agent/agent.h"
#include "gige_lib.h"

//! \ingroup devices
//! \defgroup acq_a35 A35 camera support

//! \ingroup acq_a35
//! \defgroup acq_a35_functions A35 library function declarations
//! @{
bool InitCamera(uint16_t width, uint16_t height, gige_handle *handle);
bool GetCameraFrame(gige_handle* handle, uint16_t *ImageFrameBuffer, uint32_t ImageSize);
void WriteRegister(gige_handle* handle,  int16_t command, int16_t param1, int16_t param2, int16_t param3);
void StartCamera(gige_handle *handle);
int ShutdownCamera(gige_handle *handle);

bool ResetCamera(gige_handle *handle);
bool AutoFFC(gige_handle *handle);
bool ManualFFC(gige_handle *handle);
bool DoFFC(gige_handle *handle);
bool GetTemperature(gige_handle *handle);
bool SetFramesToSkip(gige_handle* handle, int FramesToSkip);
bool SetPacketDelay(gige_handle* handle, int PacketDelay);
//! @}
