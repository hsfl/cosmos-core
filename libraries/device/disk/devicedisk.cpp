#include "devicedisk.h"

DeviceDisk::DeviceDisk()
{

}


// get the disk size in bytes
uint64_t DeviceDisk::getSize()
{
    struct statvfs buf;
    uint64_t disk_size;

    statvfs("/",&buf);

    disk_size = (uint64_t)buf.f_frsize * (uint64_t)buf.f_blocks;

    return disk_size;
}


float DeviceDisk::getSizeGB()
{
    // convert from Byte to GB
    return (float)getSize()/GB;
}


uint64_t DeviceDisk::getUsed()
{
    // compare with
    // $ fdisk -l | grep Disk
    // $ df -h

    struct statvfs buf;
    uint64_t disk_size = 0;
    uint64_t disk_used = 0;
    uint64_t disk_free = 0;

    //char *path = (char*) "/";

    statvfs("/",&buf);

    // disk size in bytes
    // must add the casting otherwise there will be problems
    //disk_size = (int64_t)buf.f_frsize * (int64_t)buf.f_blocks;
    disk_size = getSize();

    // free disk in bytes
    // free = (int64_t)buf.f_frsize * (int64_t)buf.f_bfree;
    disk_free = getFree();

    // total disk used in bytes
    disk_used = disk_size - disk_free;

    //    return (used) * 0.000976563; // convert byte to kilobytes
    return (disk_used);
}


float DeviceDisk::getUsedGB()
{
    // convert from Byte to GB
    return (float)getUsed()/GB;
}

// get the free disk in bytes
uint64_t DeviceDisk::getFree()
{
    struct statvfs buf;
    uint64_t diskFree;

    statvfs("/",&buf);

    diskFree = (uint64_t)buf.f_frsize * (uint64_t)buf.f_bfree;

    return diskFree;
}


float DeviceDisk::getFreeGB()
{
    // convert from Byte to GB
    return (float)getFree()/GB;
}
