#ifndef DEVICEDISK_H
#define DEVICEDISK_H

#include "support/configCosmos.h"
#include "support/jsondef.h"

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

    struct info
    {
        string mount;
        uint64_t size;
        uint64_t used;
        uint64_t free;
    };

    DeviceDisk();
    vector <info> getInfo();
    double getAll(string path);
    uint64_t getSize(string path);
    uint64_t getUsed(string path);
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
