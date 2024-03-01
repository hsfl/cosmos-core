/**
 * @file devicedisk.h
 * @brief 
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

#ifndef DEVICEDISK_H
#define DEVICEDISK_H

#include "support/configCosmos.h"
/** @todo Remove commented-out code? */
//#include "support/jsonlib.h"
#include "support/datalib.h"
#include "support/stringlib.h"

#include <cstdint> // uint64_t
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

#if defined(COSMOS_MAC_OS)
    #include <sys/disk.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>
#endif

/** @todo Remove commented-out code? */
//static const double GiB = 1024. * 1024. * 1024.;

/**
 * @brief 
 * 
 * @todo Document this.
 */
class DeviceDisk
{
public:
    /** @todo Document this. */
    uint64_t Size=0;
    /** @todo Document this. */
    uint64_t Used=0;
    /** @todo Document this. */
    uint64_t Free=0;
    /** @todo Document this. */
    double SizeGiB=0.;
    /** @todo Document this. */
    double UsedGiB=0.;
    /** @todo Document this. */
    double FreeGiB=0.;
    /** @todo Document this. */
    double FreePercent=0.;

    /** @todo Document this. */
    struct info
    {
        /** @todo Document this. */
        string mount="";
        /** @todo Document this. */
        uint64_t size=0;
        /** @todo Document this. */
        uint64_t used=0;
        /** @todo Document this. */
        uint64_t free=0;
    };

    DeviceDisk();
    vector <info> getInfo();
    double getAll(const string path);
    uint64_t getSize(const string path);
    uint64_t getUsed(const string path);
    uint64_t getFree(string path);
    double getAll();
    uint64_t getSize();
    uint64_t getUsed();
    uint64_t getFree();

    double getFreeGiB(string path);
    double getUsedGiB(string path);
    double getSizeGiB(string path);
    double getFreeGiB();
    double getUsedGiB();
    double getSizeGiB();
};

#endif // DEVICEDISK_H
