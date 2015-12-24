/*! brief CPU monitoring agent
*/

//! \ingroup agents
//! \defgroup agent_cpu CPU monitoring agent
//! This program accepts requests to return the machines:
//! - virtual memory
//! - virtual memory percentage
//! - disk space used
//! - disk space percentage
//! - load

#include "configCosmos.h"
#include "agentlib.h"
#include "timeutils.hpp"
#include "jsonlib.h"
#include "elapsedtime.hpp"
#include "stringlib.h"

#include "device/cpu/devicecpu.h"
#include "device/disk/devicedisk.h"

#include <stdio.h>
#include <iostream>

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

//using namespace std;
using std::cout;
using std::endl;

// flag to turn on/off print
bool printStatus;

int myagent(), create_node();

int32_t request_soh(char *request, char* response, void *cdata);

// disk
int32_t request_diskSize(char *request, char *response, void *cdata);
int32_t request_diskUsed(char *request, char *response, void *cdata);
int32_t request_diskFree(char *request, char *response, void *cdata);
int32_t request_diskFreePercent (char*request, char *response, void *cdata);

// cpu
int32_t request_cpuProcess(char*request, char *response, void *cdata);

int32_t request_load(char *request, char *response, void *cdata);
int32_t request_mem(char *request, char *response, void *cdata);
int32_t request_mempercent (char*request, char *response, void *cdata);

int32_t request_printStatus(char *request, char *response, void */*cdata*/);


#ifdef COSMOS_WIN_OS
double GetWindowsCPULoad(), GetWindowsUsedDisk(), GetWindowsVirtualMem();
double GetWindowsTotalDisk(), GetWindowsTotalVirtualMem();
static double CalculateWindowsCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
static unsigned long long FileTimeToInt64(const FILETIME & ft) {return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);}
std::string getWindowsDeviceName();
#else


// memory
double GetLinuxVirtualMem();
double GetLinuxTotalVirtualMem();

#endif

std::string agentname  = "cpu_monitor";
std::string nodename;

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat
char buf4[512];

char sohstring[] = "{\"device_cpu_utc_000\",\"device_cpu_disk_000\",\"device_cpu_maxdisk_000\",\"device_cpu_maxmem_000\",\"device_cpu_mem_000\",\"device_cpu_load_000\",\"}";

#define MAXBUFFERSIZE 256 // comm buffer for agents

//cosmosstruc *cdata;  // to access the cosmos data
ElapsedTime et;
#endif //COSMOS_MAC_OS



double diskUsed;
double diskSize;
double diskFree;
float cpuPercentUseCurrentProcess;

Agent agent;

int main(int argc, char *argv[])
{

#ifndef COSMOS_MAC_OS
    std::cout << "Starting agent cpu" << std::endl;
    int iretn;

    switch (argc)
    {
    case 1:
    {
#ifdef COSMOS_WIN_OS
        std::string devicename = getWindowsDeviceName();
        nodename = "cpu_" + devicename;
#else
        char devicename[128];
        gethostname(devicename, sizeof devicename);
        nodename = "cpu_" + (std::string)devicename;
#endif
    }
        break;
    case 2:
    {
        nodename = argv[1];
    }
        break;
    default:
    {
        printf("Usage: agent_cpu {node}\n");
    }
        break;
    }

    if (create_node())
    {
        std::cout << "Unable to make node " << std::endl;
        exit(1);
    }

    agent.version = "1.0";
    agent.setupServer(nodename, agentname);

    // Add additional requests

    agent.addRequest("soh",request_soh);
    agent.addRequest("diskSize",request_diskSize,"","get disk size in GB");
    agent.addRequest("diskUsed",request_diskUsed,"","get disk used in GB");
    agent.addRequest("diskFree",request_diskFree,"","get disk free in GB");
    agent.addRequest("diskFreePercent",request_diskFreePercent,"","get disk free in %");
    agent.addRequest("mem",request_mem);
    agent.addRequest("load",request_load);
    agent.addRequest("mem_percent",request_mempercent);
    agent.addRequest("cpuProc",request_cpuProcess,"","get the % CPU for this process");
    agent.addRequest("printStatus",request_printStatus,"","print the status data");

    agent_set_sohstring(agent.cdata, sohstring);

    // Start our own thread
    iretn = myagent();

    return 0;
#endif //COSMOS_MAC_OS
}



#ifndef COSMOS_MAC_OS
int myagent()
{
    std::cout << agentname << " ...online " << std::endl;
    ElapsedTime et;
    et.start();

    DeviceCpu cpu;
    DeviceDisk disk;



    // Start performing the body of the agent
    while(agent.isRunning())
    {

        COSMOS_SLEEP(1);

        agent.cdata[0].devspec.cpu[0]->gen.utc = currentmjd();

        // why ?
        //		if (et.split() == 5.)
        //		{
        //			et.reset();
#ifdef COSMOS_WIN_OS
        agent.cdata[0].devspec.cpu[0]->load = GetWindowsCPULoad();
        agent.cdata[0].devspec.cpu[0]->disk = GetWindowsUsedDisk();
        agent.cdata[0].devspec.cpu[0]->mem = GetWindowsVirtualMem();
        agent.cdata[0].devspec.cpu[0]->maxdisk = GetWindowsTotalDisk();
        agent.cdata[0].devspec.cpu[0]->maxmem = GetWindowsTotalVirtualMem();
#else
        // cpu
        agent.cdata[0].devspec.cpu[0]->load   = cpu.getLoad1minAverage();
        cpuPercentUseCurrentProcess     = cpu.getPercentUseForCurrentProcess();
        // disk

        diskUsed = disk.getUsedGB();
        diskSize = disk.getSizeGB();
        diskFree = disk.getFreeGB();

        // TODO: add disk to node.ini
        //agent.cdata[0].devspec.disk[0]->disk    = GetLinuxDiskUsedGB(); // TODO: rename disk to diskUsed
        //agent.cdata[0].devspec.disk[0]->maxdisk = GetLinuxDiskSizeGB(); // TODO: rename disk to diskSize
        //agent.cdata[0].devspec.disk[0]->diskFree    = GetLinuxDiskFree(); // TODO: rename disk to diskUsed

        agent.cdata[0].devspec.cpu[0]->mem     = GetLinuxVirtualMem();
        agent.cdata[0].devspec.cpu[0]->maxmem  = GetLinuxTotalVirtualMem();
#endif
        //		}
        //		std::cout << et.lap() << "           " << mempercent << std::endl;
        //		et.start();

        if (printStatus) {
            cout << "Load," << agent.cdata[0].devspec.cpu[0]->load << ", ";
            cout << "DiskSize[GiB]," << diskSize << ", ";
            cout << "DiskUsed[GiB]," << diskUsed << ", ";
            cout << "DiskFree[GiB]," << diskFree << ", ";
            cout << "CPU Proc[%]," << cpuPercentUseCurrentProcess << endl;

        }

        cpu.stress();
        cpu.stress();

    }

    agent.shutdown();

    return 0;
}

#ifdef COSMOS_WIN_OS

double GetWindowsCPULoad()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateWindowsCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime)+FileTimeToInt64(userTime)) : -1.0f;
}

double GetWindowsUsedDisk()
{
    int64_t freeSpace, totalSpace, totalFreeSpace;

    GetDiskFreeSpaceEx( "C:",
                        (PULARGE_INTEGER)&freeSpace,
                        (PULARGE_INTEGER)&totalSpace,
                        (PULARGE_INTEGER)&totalFreeSpace);

    return (totalSpace - totalFreeSpace) * 0.001; // convert byte to kilobyte
}

double GetWindowsTotalDisk()
{
    int64_t freeSpace, totalSpace, totalFreeSpace;

    GetDiskFreeSpaceEx( "C:",
                        (PULARGE_INTEGER)&freeSpace,
                        (PULARGE_INTEGER)&totalSpace,
                        (PULARGE_INTEGER)&totalFreeSpace);

    return (totalSpace) * 0.001; // convert byte to kilobyte
}

double GetWindowsTotalVirtualMem()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
    return (totalVirtualMem) * 0.001; // convert byte to kilobyte
}

double GetWindowsVirtualMem()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    return (virtualMemUsed) * 0.001; // convert byte to kilobyte
}

static double CalculateWindowsCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
    static unsigned long long _previousTotalTicks = 0;
    static unsigned long long _previousIdleTicks = 0;

    unsigned long long totalTicksSinceLastTime = totalTicks-_previousTotalTicks;
    unsigned long long idleTicksSinceLastTime  = idleTicks-_previousIdleTicks;

    float ret = 1.0f-((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime)/totalTicksSinceLastTime : 0);

    _previousTotalTicks = totalTicks;
    _previousIdleTicks  = idleTicks;
    return ret;
}
std::string getWindowsDeviceName()
{
    TCHAR nameBuf[MAX_COMPUTERNAME_LENGTH + 2];
    DWORD nameBufSize;

    nameBufSize = sizeof nameBuf - 1;
    if (GetComputerName(nameBuf, &nameBufSize) == TRUE) {
        _tprintf(_T("Device name is %s\n"), nameBuf);
    }

    return  std::string(nameBuf);
}

#else







double GetLinuxVirtualMem()
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long virtualMemUsed = memInfo.totalram - memInfo.freeram;

    virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
    virtualMemUsed *= memInfo.mem_unit;

    return (virtualMemUsed) * 0.000976563; // convert byte to kibibyte
}

double GetLinuxTotalVirtualMem() // NOT TESTED
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long totalVirtualMem = memInfo.totalram;

    totalVirtualMem += memInfo.totalswap;
    totalVirtualMem *= memInfo.mem_unit;

    return (totalVirtualMem) * 0.000976563; // convert byte to kibibyte
}


#endif
int32_t request_soh(char *request, char* response, void *)
{
    std::string rjstring;
    //	strcpy(response,json_of_list(rjstring,sohstring,agent.cdata));
    strcpy(response,json_of_table(rjstring, agent.cdata[0].agent[0].sohtable, agent.cdata));

    return 0;
}

int32_t request_mem(char *request, char* response, void *)
{
    return (sprintf(response, "%f", agent.cdata[0].devspec.cpu[0]->mem));
}

// ----------------------------------------------
// disk
int32_t request_diskSize(char *request, char* response, void *)
{
    return (sprintf(response, "%f", diskSize));
}

int32_t request_diskUsed(char *request, char* response, void *)
{
    return (sprintf(response, "%f", diskUsed));
}

int32_t request_diskFree(char *request, char* response, void *)
{
    // TODO: implement diskFree
    //return (sprintf(response, "%.1f", agent.cdata[0].devspec.cpu[0]->disk));

    // in the mean time use this
    return (sprintf(response, "%f", diskFree));

}


int32_t request_load (char *request, char* response, void *)
{
    return (sprintf(response, "%.2f", agent.cdata[0].devspec.cpu[0]->load));
}


int32_t request_diskFreePercent (char *request, char *response, void *)
{
#ifdef COSMOS_WIN_OS
    float diskpercent = GetWindowsUsedDisk() / GetWindowsTotalDisk();
#else
    float diskpercent = diskUsed / diskSize;
#endif
    return (sprintf(response, "%f", diskpercent));
}


int32_t request_mempercent (char *request, char *response, void *)
{
#ifdef COSMOS_WIN_OS
    float mempercent = GetWindowsVirtualMem() / GetWindowsTotalVirtualMem();
#else
    float mempercent = GetLinuxVirtualMem() / GetLinuxTotalVirtualMem();
#endif
    return (sprintf(response, "%f", mempercent));
}


int32_t request_cpuProcess(char *request, char *response, void */*cdata*/){

    return (sprintf(response, "%f", cpuPercentUseCurrentProcess));
}

int32_t request_printStatus(char *request, char *response, void */*cdata*/){

    sscanf(request,"%*s %d",&printStatus);
    cout << "printStatus is " << printStatus <<  endl;

    return 0;
}



int create_node () // only use when unsure what the node is
{
    //	std::string node_directory;

    // Ensure node is present
    std::cout << "Node name is " << nodename << std::endl;
    if (get_nodedir(nodename).empty())
    {
        std::cout << "Couldn't find Node directory, making directory now..." << std::endl;
        if (get_nodedir(nodename, true).empty())
        {
            std::cout << "Couldn't create Node directory." << std::endl;
            return 1;
        }
        agent.cdata = json_create();
        strcpy(agent.cdata->node.name, nodename.c_str());
        agent.cdata->node.type = NODE_TYPE_COMPUTER;

        agent.cdata->node.piece_cnt = 1;
        agent.cdata->piece.resize(1);
        agent.cdata->piece[0].cidx = 0;
        strcpy(agent.cdata->piece[0].name, "Main CPU");
        agent.cdata->piece[0].type = PIECE_TYPE_DIMENSIONLESS;
        agent.cdata->piece[0].emi = .8;
        agent.cdata->piece[0].abs = .88;
        agent.cdata->piece[0].hcap = 800;
        agent.cdata->piece[0].hcon = 237;
        agent.cdata->piece[0].pnt_cnt = 1;
        for (uint16_t i=0; i<3; ++i)
        {
            agent.cdata->piece[0].points[0].col[i] = 0.;
        }
        json_addpieceentry(0,agent.cdata);

        agent.cdata->node.device_cnt = 1;
        agent.cdata->device.resize(1);
        agent.cdata->devspec.cpu_cnt = 1;
        agent.cdata->device[0].all.gen.pidx = 0;
        agent.cdata->device[0].all.gen.cidx = 0;
        agent.cdata->device[0].all.gen.didx = 0;
        agent.cdata->device[0].all.gen.type = DEVICE_TYPE_CPU;
        agent.cdata[0].device[0].all.gen.portidx = PORT_TYPE_NONE;
        json_addcompentry(0, agent.cdata);
        json_adddeviceentry(0, agent.cdata);

        int32_t iretn = json_dump_node(agent.cdata);
        json_destroy(agent.cdata);
        return iretn;
    }
    else
    {
        return 0;
    }
}
#endif //COSMOS_MAC_OS
