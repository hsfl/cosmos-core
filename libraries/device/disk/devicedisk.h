#ifndef DEVICEDISK_H
#define DEVICEDISK_H

#include "configCosmos.h"
#include "jsondef.h"

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


static const double GiB = 1024. * 1024. * 1024.;

class DeviceDisk
{
public:
    uint64_t Size;
    uint64_t Used;
    uint64_t Free;
    double SizeGiB;
    double UsedGiB;
    double FreeGiB;
    double FreePercent;

    DeviceDisk();
    double getAll(std::string path);
    uint64_t getSize(std::string path);
    uint64_t getUsed(std::string path);
    uint64_t getFree(std::string path);
    double getAll();
    uint64_t getSize();
    uint64_t getUsed();
    uint64_t getFree();

    double getFreeGiB(std::string path);
    double getUsedGiB(std::string path);
    double getSizeGiB(std::string path);
    double getFreeGiB();
    double getUsedGiB();
    double getSizeGiB();
};

#endif // DEVICEDISK_H
