#include "devicedisk.h"

DeviceDisk::DeviceDisk()
{

}

uint64_t DeviceDisk::getSize()
{
#if defined COSMOS_LINUX_OS
    return getSize("/");
#endif

#if defined COSMOS_WIN_OS
    return getSize("C:");
#endif
}

// get the disk size in bytes
uint64_t DeviceDisk::getSize(std::string path)
{
    uint64_t disk_size = 0;
    uint64_t disk_free = 0;

#if defined COSMOS_LINUX_OS
    struct statvfs buf;

    statvfs(path.c_str(),&buf);

    // disk size in bytes
    // must add the casting otherwise there will be problems
    disk_size = (uint64_t)buf.f_frsize * (uint64_t)buf.f_blocks;

#endif


#if defined COSMOS_WIN_OS
    uint64_t freeSpace;

    GetDiskFreeSpaceEx( path.c_str(),
                        (PULARGE_INTEGER)&freeSpace,
                        (PULARGE_INTEGER)&disk_size,
                        (PULARGE_INTEGER)&disk_free);

#endif

    return disk_size;
}


double DeviceDisk::getSizeGB(std::string path)
{
    // convert from Byte to GB
    return (double)getSize(path)/GB;
}

double DeviceDisk::getSizeGB()
{
    // convert from Byte to GB
    return (double)getSize()/GB;
}

uint64_t DeviceDisk::getUsed()
{
#if defined COSMOS_LINUX_OS
    return getUsed("/");
#endif

#if defined COSMOS_WIN_OS
    return getUsed("C:");
#endif
}

uint64_t DeviceDisk::getUsed(std::string path)
{
    uint64_t disk_size = 0;
    uint64_t disk_used = 0;
    uint64_t disk_free = 0;

#if defined COSMOS_LINUX_OS
    struct statvfs buf;

    statvfs(path.c_str(),&buf);

    // disk size in bytes
    // must add the casting otherwise there will be problems
    disk_size = (uint64_t)buf.f_frsize * (uint64_t)buf.f_blocks;
//    disk_size = getSize(path);

    // free disk in bytes
     disk_free = (uint64_t)buf.f_frsize * (uint64_t)buf.f_bfree;
//    disk_free = getFree(path);

#endif


#if defined COSMOS_WIN_OS
    uint64_t freeSpace;

    GetDiskFreeSpaceEx( path.c_str(),
                        (PULARGE_INTEGER)&freeSpace,
                        (PULARGE_INTEGER)&disk_size,
                        (PULARGE_INTEGER)&disk_free);

#endif

    // total disk used in bytes
    disk_used = disk_size - disk_free;
    return (disk_used);
}


double DeviceDisk::getUsedGB(std::string path)
{
    // convert from Byte to GB
    return (double)getUsed(path)/GB;
}

double DeviceDisk::getUsedGB()
{
    // convert from Byte to GB
    return (double)getUsed()/GB;
}

// get the free disk in bytes
uint64_t DeviceDisk::getFree()
{
#if defined COSMOS_LINUX_OS
    return getFree("/");
#endif

#if defined COSMOS_WIN_OS
    return getFree("C:");
#endif
}

uint64_t DeviceDisk::getFree(std::string path)
{
    uint64_t disk_free;

#if defined COSMOS_LINUX_OS
    struct statvfs buf;

    statvfs(path.c_str(),&buf);

    disk_free = (uint64_t)buf.f_frsize * (uint64_t)buf.f_bfree;
#endif

#if defined COSMOS_WIN_OS
    uint64_t freeSpace, totalSpace, totalFreeSpace;

    GetDiskFreeSpaceEx( path.c_str(),
                        (PULARGE_INTEGER)&freeSpace,
                        (PULARGE_INTEGER)&totalSpace,
                        (PULARGE_INTEGER)&totalFreeSpace);

    disk_free = totalFreeSpace;
#endif

    return disk_free;
}


double DeviceDisk::getFreeGB(std::string path)
{
    // convert from Byte to GB
    return (double)getFree(path)/GB;
}

double DeviceDisk::getFreeGB()
{
    // convert from Byte to GB
    return (double)getFree()/GB;
}
