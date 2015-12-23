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


#ifndef CPU_LIB_H
#define CPU_LIB_H

/*! \file cpu_lib.h
* \brief cpu_lib include file.
*/

//! \ingroup devices
//!	\defgroup cpu_lib Generic CPU device library.
//! CPU Device Library.
//!
//! Device level support CPU.

#include "configCosmos.h"

#ifndef COSMOS_MAC_OS
#if defined (COSMOS_WIN_OS)
#include "windows.h"
#include <tchar.h>
#else
#include <stdlib.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#endif
#endif

double cpu_load();
double cpu_vmemtotal();
double cpu_vmem();

#ifdef COSMOS_WIN_OS
double CalculateWindowsCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
unsigned long long FileTimeToInt64(const FILETIME & ft);
std::string getWindowsDeviceName();
#else
double GetLinuxCPULoad(), GetLinuxUsedDisk(), GetLinuxVirtualMem();
double GetLinuxTotalDisk(), GetLinuxTotalVirtualMem();
static double CalculateLinuxCPULoad (float *out);
#endif


#endif // CPU_LIB_H

