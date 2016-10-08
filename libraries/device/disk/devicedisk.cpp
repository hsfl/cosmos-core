#include "device/disk/devicedisk.h"

DeviceDisk::DeviceDisk()
{

}

// get the disk size percentage
double DeviceDisk::getAll()
{
#if defined COSMOS_LINUX_OS
    return getAll("/");
#endif

#if defined COSMOS_WIN_OS
    return getAll("C:");
#endif
}

double DeviceDisk::getAll(std::string path)
{
    Size = 0;
    Free = 0;
    Used = 0;
    FreePercent = 0;

#if defined COSMOS_LINUX_OS
    struct statvfs buf;

    statvfs(path.c_str(),&buf);

    // disk size in bytes
    // must add the casting otherwise there will be problems
    Size = (uint64_t)buf.f_frsize * (uint64_t)buf.f_blocks;
    Free = (uint64_t)buf.f_frsize * (uint64_t)buf.f_bfree;

#endif


#if defined COSMOS_WIN_OS
    uint64_t freeSpace;

    GetDiskFreeSpaceExA( (LPCSTR) path.c_str(),
                        (PULARGE_INTEGER)&freeSpace,
                        (PULARGE_INTEGER)&Size,
                        (PULARGE_INTEGER)&Free);

#endif

    Used = Size - Free;
    FreePercent = (double)Free / Used;

    // convert to GiB
    SizeGiB = Size/GiB;
    FreeGiB = Free/GiB;
    UsedGiB = Used/GiB;

    return FreePercent;
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

// get all disk information in bytes
uint64_t DeviceDisk::getSize(std::string path)
{
    getAll(path);
    return Size;
}


double DeviceDisk::getSizeGiB(std::string path)
{
    // convert from Byte to GiB
    return (double)getSize(path)/GiB;
}

double DeviceDisk::getSizeGiB()
{
    // convert from Byte to GiB
    return (double)getSize()/GiB;
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
    getAll(path);
    return (Used);
}


double DeviceDisk::getUsedGiB(std::string path)
{
    // convert from Byte to GiB
    return (double)getUsed(path)/GiB;
}

double DeviceDisk::getUsedGiB()
{
    // convert from Byte to GiB
    return (double)getUsed()/GiB;
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
    getAll(path);

    return Free;
}


double DeviceDisk::getFreeGiB(std::string path)
{
    // convert from Byte to GiB
    return (double)getFree(path)/GiB;
}

double DeviceDisk::getFreeGiB()
{
    // convert from Byte to GiB
    return (double)getFree()/GiB;
}
