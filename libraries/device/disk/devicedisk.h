#ifndef DEVICEDISK_H
#define DEVICEDISK_H

#include "support/configCosmos.h"
#include "support/jsondef.h"
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


//static const double GiB = 1024. * 1024. * 1024.;

class DeviceDisk
{
public:
    uint64_t Size=0;
    uint64_t Used=0;
    uint64_t Free=0;
    double SizeGiB=0.;
    double UsedGiB=0.;
    double FreeGiB=0.;
    double FreePercent=0.;

    struct info
    {
        string mount="";
        uint64_t size=0;
        uint64_t used=0;
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
