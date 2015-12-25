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
#ifndef DEVICECPU_H
#define DEVICECPU_H

/*! \file devicecpu.h
* \brief devicecpu include file.
*/

//! \ingroup devices
//!	\defgroup DeviceCpu Generic CPU device library.
//! CPU Device Library.
//!
//! Device level support CPU.


#include "configCosmos.h"
#include "stringlib.h"

// for current process CPU utilization
#if defined (COSMOS_LINUX_OS)
#include <sys/times.h>
#include <sys/vtimes.h>
#include <sys/sysinfo.h>
//#include <stdlib.h>
//#include <sys/statvfs.h>
//#include <sys/types.h>
//#include <unistd.h>
#endif

#if defined (COSMOS_WIN_OS)
#include "windows.h"
#include <tchar.h>
#endif

#include <fstream>      // std::ifstream


#ifdef COSMOS_WIN_OS
class DeviceCpuWindows
{

public:
    DeviceCpuWindows();

    double getLoad();
    double CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
    unsigned long long FileTimeToInt64(const FILETIME & ft);
    std::string getDeviceName();
    double getVirtualMemory();
    double getVirtualMemoryTotal();
};

#endif


class DeviceCpu
{
public:
    DeviceCpu();



    void stress();

    // API functions (generic names)
    double getLoad();
    double getVirtualMemory();
    double getVirtualMemoryTotal();
    double getPercentUseForCurrentProcess();
};



#ifdef COSMOS_LINUX_OS
class DeviceCpuLinux
{
    clock_t lastCPU, lastSysCPU, lastUserCPU;
public:
    DeviceCpuLinux();

    // variables
    float load1minAverage;
    int numProcessors;
    std::string processName;

    // functions
    double getLoad1minAverage();
    float getPercentUseForCurrentProcess();
    void initCpuUtilization();
    double getVirtualMemory();
    double getVirtualMemoryTotal();
    std::string getCurrentProcessName();
    //double GetLinuxCPULoad(), GetLinuxUsedDisk(), GetLinuxVirtualMem();
    //double GetLinuxTotalDisk(), GetLinuxTotalVirtualMem();
    //double CalculateLinuxCPULoad (float *out);
};

#endif






#endif // DEVICECPU_H
