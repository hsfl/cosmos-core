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


#include "support/configCosmos.h"
#include "support/stringlib.h"

// for current process CPU utilization
#if defined (COSMOS_LINUX_OS)
#include <sys/times.h>
#include <sys/vtimes.h>
#include <sys/sysinfo.h>

#include <sys/time.h>
#include <sys/resource.h>
#endif

#if defined (COSMOS_WIN_OS)
#include "windows.h"
#include <tchar.h>
#endif

#include <fstream>   // std::ifstream
#include <algorithm> // std::unique

using std::string;
using std::ifstream;
using std::cout;
using std::endl;

#ifdef COSMOS_WIN_OS
class DeviceCpuWindows
{

public:
    DeviceCpuWindows();

    double getLoad();
    double CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
    unsigned long long FileTimeToInt64(const FILETIME & ft);
    string getHostName();
    double getVirtualMemoryUsed();
    double getVirtualMemoryTotal();
    pid_t getPidOf(string processName);
};

#endif


#if defined(COSMOS_LINUX_OS)
class DeviceCpuLinux
{

    double lastCPUtime;
    double tic, toc;
    float percentCpu;

public:
    DeviceCpuLinux();

    // variables
    float load1minAverage;
    int numProcessors = 0;
    string processName;

    // functions
    double getLoad1minAverage();
    float getPercentUseForCurrentProcess();
    float getPercentUseForCurrentProcessOverLifetime();
    void initCpuUtilization();
    double getVirtualMemoryUsed();
    double getVirtualMemoryTotal();
    double getVirtualMemoryFree();
    string getCurrentProcessName();
    string getHostName();
    //double GetLinuxCPULoad(), GetLinuxUsedDisk(), GetLinuxVirtualMem();
    //double GetLinuxTotalDisk(), GetLinuxTotalVirtualMem();
    //double CalculateLinuxCPULoad (float *out);
    double getMemoryUsed();
    double getMemoryTotal();
    pid_t getPidOf(string processName);
    double getMemoryUsedOf(string processName);
    float getPercentCpuOf(string processName);

    class procPidStat {

        struct stat {
            // stat vars
            string pid, comm, state, ppid, pgrp, session, tty_nr;
            string tpgid, flags, minflt, cminflt, majflt, cmajflt;
            string utime, stime, cutime, cstime, priority, nice;
            string O, itrealvalue, starttime;
            unsigned long vsize;
            long rss;
        };



    public:
        //int pid, comm, state, ppid, pgrp, session, tty_nr;
        //int tpgid, flags, minflt, cminflt, majflt, cmajflt;
        float utime, stime; //, cutime, cstime, priority, nice;
        //string O, itrealvalue, starttime;

        bool fileExists = false;

        procPidStat(string processName);
    };

    class procStat {

    public:
        // stat vars
        string cpu, user, nice, system, idle, iowait, irq, softirq, steal,guest, guest_nice;
        int time_total;
        procStat();
    };
    float getPercentMemoryOf(string processName);
};

#endif


#if defined(COSMOS_MAC_OS)
class DeviceCpuMac
{
    double lastCPUtime;
    double tic, toc;
    float percentCpu;

public:
    DeviceCpuMac();

    // variables
    float load1minAverage;
    int numProcessors = 0;
    string processName;

    // functions
    double getLoad1minAverage();
    float getPercentUseForCurrentProcess();
    float getPercentUseForCurrentProcessOverLifetime();
    void initCpuUtilization();
    double getVirtualMemoryUsed();
    double getVirtualMemoryTotal();
    double getVirtualMemoryFree();
    string getCurrentProcessName();
    string getHostName();
    //double GetLinuxCPULoad(), GetLinuxUsedDisk(), GetLinuxVirtualMem();
    //double GetLinuxTotalDisk(), GetLinuxTotalVirtualMem();
    //double CalculateLinuxCPULoad (float *out);
    double getMemoryUsed();
    double getMemoryTotal();
    pid_t getPidOf(string processName);
    double getMemoryUsedOf(string processName);
    float getPercentCpuOf(string processName);

    class procPidStat {

        struct stat {
            // stat vars
            string pid, comm, state, ppid, pgrp, session, tty_nr;
            string tpgid, flags, minflt, cminflt, majflt, cmajflt;
            string utime, stime, cutime, cstime, priority, nice;
            string O, itrealvalue, starttime;
            unsigned long vsize;
            long rss;
        };



    public:
        //int pid, comm, state, ppid, pgrp, session, tty_nr;
        //int tpgid, flags, minflt, cminflt, majflt, cmajflt;
        float utime, stime; //, cutime, cstime, priority, nice;
        //string O, itrealvalue, starttime;

        bool fileExists = false;

        procPidStat(string processName);
    };

    class procStat {

    public:
        // stat vars
        string cpu, user, nice, system, idle, iowait, irq, softirq, steal,guest, guest_nice;
        int time_total;
        procStat();
    };
    float getPercentMemoryOf(string processName);
};

#endif



class DeviceCpu
{
public:
    DeviceCpu();

    double percentUseForCurrentProcess;
    int numProcessors;
    string processName;
    string hostName;
    double memoryUsed;
    double memoryTotal;
    double memoryFree;
    double memoryBuffers;
    double virtualMemoryUsed;
    double virtualMemoryTotal;
    double virtualMemoryFree;
    double load;

    void stress();

    // API functions (generic names)
    double getLoad();
    double getVirtualMemoryTotal();
    double getVirtualMemoryUsed();
    double getVirtualMemoryFree();
    double getVirtualMemoryUsedPercent();
    double getPercentUseForCurrentProcess();
    string getHostName();

    // move to math_cpu.cpp?
    double BytesToKiB(double bytes);
    double BytesToMiB(double bytes);
    double BytesToMB(double bytes);

#if defined(COSMOS_LINUX_OS)
    DeviceCpuLinux cpuLinux;
#endif

#if defined(COSMOS_MAC_OS)
    DeviceCpuMac cpuMac;
#endif

#if defined(COSMOS_WIN_OS)
    DeviceCpuWindows cpuWin;
#endif

    double getVirtualMemoryUsedKiB();
    double getVirtualMemoryUsedMiB();

    double getVirtualMemoryUsedMB();
    double getMemoryUsed();
    double getMemoryTotal();
    double getMemoryUsedKiB();
    double getMemoryUsedMiB();
    double getMemoryTotalKiB();
    double getMemoryTotalMiB();
    double getMemoryUsedMB();
    double getMemoryTotalMB();
    pid_t getPidOf(string processName);
    pid_t getMemoryUsedOf(string processName);
    float getPercentCpuOf(string processName);
    float getPercentMemoryOf(string processName);
};


#endif // DEVICECPU_H
