/**
 * @file devicecpu.h
 * @brief devicecip include file.
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

#ifndef DEVICECPU_H
#define DEVICECPU_H

/**
 * @ingroup
 * @defgroup DeviceCpu Generic CPU device library.
 * CPU Device Library.
 * 
 * Device level support CPU.
 */

#include "support/configCosmos.h"
#include "support/stringlib.h"
#include "support/timelib.h"
#include "support/datalib.h"

// for current process CPU utilization
#if defined (COSMOS_LINUX_OS)
#include <sys/times.h>
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

using std::ifstream;
using std::cout;
using std::endl;

/**
 * @brief 
 * 
 * @todo Document this.
 */
class DeviceCpu
{
public:
    DeviceCpu();

    /** @todo Document this. */
    double percentUseForCurrentProcess;
    /** @todo Document this. */
    int numProcessors;
    /** @todo Document this. */
    string processName;
    /** @todo Document this. */
    string hostName;
    /** @todo Document this. */
    double memoryUsed;
    /** @todo Document this. */
    double memoryTotal;
    /** @todo Document this. */
    double memoryFree;
    /** @todo Document this. */
    double memoryBuffers;
    /** @todo Document this. */
    double virtualMemoryUsed;
    /** @todo Document this. */
    double virtualMemoryTotal;
    /** @todo Document this. */
    double virtualMemoryFree;
    /** @todo Document this. */
    double load;

    // API functions (generic names)
    double getLoad();
    uint16_t getCpuCount();
    uint32_t getUptime();
    uint16_t getBootCount();
    double getVirtualMemoryTotal();
    double getVirtualMemoryUsed();
    /** @todo Not actually implemented. */
    double getVirtualMemoryFree();
    double getVirtualMemoryUsedPercent();
    double getPercentUseForCurrentProcess();
    string getHostName();

    /** @todo move to math_cpu.cpp? */
    double BytesToKiB(double bytes);
    double BytesToMiB(double bytes);
    double BytesToMB(double bytes);

#if defined(COSMOS_LINUX_OS)
    /** @todo Document this. */
    DeviceCpuLinux cpuLinux;
#endif

#if defined(COSMOS_MAC_OS)
    /** @todo Document this. */
    DeviceCpuMac cpuMac;
#endif

#if defined(COSMOS_WIN_OS)
    /** @todo Document this. */
    DeviceCpuWindows cpuWin;
#endif

    double getVirtualMemoryUsedKiB();
    double getVirtualMemoryUsedMiB();
    double getVirtualMemoryUsedMB();
    double getMemoryUsed();
    double getMemoryTotal();
    double getMemoryUsedKiB();
    double getMemoryUsedMiB();
    double getMemoryUsedMB();
    double getMemoryTotalKiB();
    double getMemoryTotalMiB();
    double getMemoryTotalMB();
    pid_t getPidOf(string processName);
    pid_t getMemoryUsedOf(string processName);
    float getPercentCpuOf(string processName);
    float getPercentMemoryOf(string processName);

    void stress();
};

#ifdef COSMOS_WIN_OS
/**
 * @brief 
 * 
 * @todo Document this.
 */
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
/**
 * @brief 
 * 
 * @todo Document this.
 */
class DeviceCpuLinux
{
    /** @todo Document this. */
    double lastCPUtime;
    /** @todo Document this. */
    double tic, toc;
    /** @todo Document this. */
    float percentCpu;

public:
    DeviceCpuLinux();

    // variables
    /** @todo Document this. */
    float load1minAverage;
    /** @todo Document this. */
    int numProcessors = 0;
    /** @todo Document this. */
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
    /** @todo Remove commented-out code? */
    //double GetLinuxCPULoad(), GetLinuxUsedDisk(), GetLinuxVirtualMem();
    /** @todo Remove commented-out code? */
    //double GetLinuxTotalDisk(), GetLinuxTotalVirtualMem();
    /** @todo Remove commented-out code? */
    //double CalculateLinuxCPULoad (float *out);
    double getMemoryUsed();
    double getMemoryTotal();
    pid_t getPidOf(string processName);
    double getMemoryUsedOf(string processName);
    float getPercentCpuOf(string processName);
    float getPercentMemoryOf(string processName);
    int32_t getCpuCount();
    int32_t getBootCount();
    int32_t getUptime();

    /**
     * @brief 
     * 
     * @todo Document this.
     */
    class procPidStat {
        /**
         * @brief 
         * 
         * @todo Document this.
         */
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
        /** @todo Remove commented-out code? */
        //int pid, comm, state, ppid, pgrp, session, tty_nr;
        /** @todo Remove commented-out code? */
        //int tpgid, flags, minflt, cminflt, majflt, cmajflt;
        /** @todo Document this. */
        float utime, stime; //, cutime, cstime, priority, nice;
        /** @todo Remove commented-out code? */
        //string O, itrealvalue, starttime;
        /** @todo Document this. */
        bool fileExists = false;

        procPidStat(string processName);
    };

    /**
     * @brief Document this.
     * 
     */
    class procStat {
    public:
        // stat vars
        /** @todo Document this. */
        string cpu, user, nice, system, idle, iowait, irq, softirq, steal,guest, guest_nice;
        /** @todo Document this. */
        int time_total;
        procStat();
    };
    
};
#endif

#if defined(COSMOS_MAC_OS)
/**
 * @brief 
 * 
 * @todo Document this.
 */
class DeviceCpuMac
{
    /** @todo Document this. */
    double lastCPUtime;
    /** @todo Document this. */
    double tic, toc;
    /** @todo Document this. */
    float percentCpu;

public:
    DeviceCpuMac();

    // variables
    /** @todo Document this. */
    float load1minAverage;
    /** @todo Document this. */
    int numProcessors = 0;
    /** @todo Document this. */
    string processName;

    // functions
    double getLoad1minAverage();
    /** @todo This function is not defined. */
    float getPercentUseForCurrentProcess();
    /** @todo This function is not defined. */
    float getPercentUseForCurrentProcessOverLifetime();
    /** @todo This function is not defined. */
    void initCpuUtilization();
    /** @todo This function is not defined. */
    double getVirtualMemoryUsed();
    /** @todo This function is not defined. */
    double getVirtualMemoryTotal();
    /** @todo This function is not defined. */
    double getVirtualMemoryFree();
    /** @todo This function is not defined. */
    string getCurrentProcessName();
    string getHostName();
    /** @todo Remove commented-out code? */
    //double GetLinuxCPULoad(), GetLinuxUsedDisk(), GetLinuxVirtualMem();
    /** @todo Remove commented-out code? */
    //double GetLinuxTotalDisk(), GetLinuxTotalVirtualMem();
    /** @todo Remove commented-out code? */
    //double CalculateLinuxCPULoad (float *out);
    /** @todo This function is not defined. */
    double getMemoryUsed();
    /** @todo This function is not defined. */
    double getMemoryTotal();
    /** @todo This function is not defined. */
    pid_t getPidOf(string processName);
    /** @todo This function is not defined. */
    double getMemoryUsedOf(string processName);
    /** @todo This function is not defined. */
    float getPercentCpuOf(string processName);
    /** @todo This function is not defined. */
    float getPercentMemoryOf(string processName);

    /**
     * @brief 
     * 
     * @todo Document this.
     */
    class procPidStat {
        /**
         * @brief 
         * 
         * @todo Document this.
         */
        struct stat {
            // stat vars
            /** @todo Document this. */
            string pid, comm, state, ppid, pgrp, session, tty_nr;
            /** @todo Document this. */
            string tpgid, flags, minflt, cminflt, majflt, cmajflt;
            /** @todo Document this. */
            string utime, stime, cutime, cstime, priority, nice;
            /** @todo Document this. */
            string O, itrealvalue, starttime;
            /** @todo Document this. */
            unsigned long vsize;
            /** @todo Document this. */
            long rss;
        };
    public:
        /** @todo Remove commented-out code? */
        //int pid, comm, state, ppid, pgrp, session, tty_nr;
        /** @todo Remove commented-out code? */
        //int tpgid, flags, minflt, cminflt, majflt, cmajflt;
        /** @todo Document this. */
        float utime, stime; //, cutime, cstime, priority, nice;
        /** @todo Remove commented-out code? */
        //string O, itrealvalue, starttime;
        /** @todo Document this. */
        bool fileExists = false;

        /** @todo This function is not defined. */
        procPidStat(string processName);
    };

    class procStat {
    public:
        // stat vars
        /** @todo Document this. */
        string cpu, user, nice, system, idle, iowait, irq, softirq, steal,guest, guest_nice;
        /** @todo Document this. */
        int time_total;

        /** @todo This function is not defined. */
        procStat();
    };
};
#endif

#endif // DEVICECPU_H
