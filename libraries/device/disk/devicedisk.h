#ifndef DEVICEDISK_H
#define DEVICEDISK_H

#include <cstdint> // uint64_t
#include <sys/statvfs.h>

static const int GB = 1024 * 1024 * 1024;

class DeviceDisk
{
public:
    DeviceDisk();
    uint64_t getSize();
    uint64_t getUsed();
    uint64_t getFree();

    float getFreeGB();
    float getUsedGB();
    float getSizeGB();
};

#endif // DEVICEDISK_H
