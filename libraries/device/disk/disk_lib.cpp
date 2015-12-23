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

#include "disk_lib.h"

#if defined(COSMOS_LINUX_OS)
double disk_use(std::string path)
{
    struct statvfs buf;
    int64_t disk_size, used, free;

    statvfs(path.c_str(),&buf);

    disk_size = buf.f_frsize * buf.f_blocks;
    free = buf.f_frsize * buf.f_bfree;
    used = disk_size - free;

    return (used) * 0.000976563; // convert byte to kibibyte
}

double disk_usetotal(std::string path) // NOT TESTED
{
    struct statvfs buf;
    int64_t disk_size;
//    int64_t used, free;

    statvfs(path.c_str(),&buf);

    disk_size = buf.f_frsize * buf.f_blocks;
//	free = buf.f_frsize * buf.f_bfree;
//	used = disk_size - free;

    return (disk_size) * 0.000976563; // convert byte to kibibyte
}
#endif

#if defined (COSMOS_WIN_OS)
double disk_use(std::string path)
{
    int64_t freeSpace, totalSpace, totalFreeSpace;

    GetDiskFreeSpaceEx( path.c_str(),
                        (PULARGE_INTEGER)&freeSpace,
                        (PULARGE_INTEGER)&totalSpace,
                        (PULARGE_INTEGER)&totalFreeSpace);

    return (totalSpace - totalFreeSpace) * 0.001; // convert byte to kilobyte
}

double disk_usetotal(std::string path)
{
    int64_t freeSpace, totalSpace, totalFreeSpace;

    GetDiskFreeSpaceEx( path.c_str(),
                        (PULARGE_INTEGER)&freeSpace,
                        (PULARGE_INTEGER)&totalSpace,
                        (PULARGE_INTEGER)&totalFreeSpace);

    return (totalSpace) * 0.001; // convert byte to kilobyte
}

#endif
