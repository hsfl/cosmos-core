#ifndef DEVICEDISK_H
#define DEVICEDISK_H

#include "configCosmos.h"

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


static const double GB = 1024. * 1024. * 1024.;

class DeviceDisk
{
public:
    DeviceDisk();
    uint64_t getSize(std::string path);
    uint64_t getUsed(std::string path);
    uint64_t getFree(std::string path);
    uint64_t getSize();
    uint64_t getUsed();
    uint64_t getFree();

    double getFreeGB(std::string path);
    double getUsedGB(std::string path);
    double getSizeGB(std::string path);
    double getFreeGB();
    double getUsedGB();
    double getSizeGB();
};

#endif // DEVICEDISK_H
