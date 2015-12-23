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

// for current process CPU utilization
#include <sys/times.h>
#include <sys/vtimes.h>

// for executing processs
#include <string>
#include <iostream>
#include <cstdio>
#include <memory>

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

// cpu
float GetLinuxCPULoad();
float CalculateLinuxCPULoad();
float GetLinuxCPUCurrentUseByProcess();

// memory
double GetLinuxVirtualMem();
double GetLinuxTotalVirtualMem();

// disk in bytes
uint64_t GetLinuxDiskSize();
uint64_t GetLinuxDiskUsed();
uint64_t GetLinuxDiskFree();

// disk in GB

static const int GB = 1024 * 1024 * 1024;

float GetLinuxDiskSizeGB();
float GetLinuxDiskUsedGB();
float GetLinuxDiskFreeGB();

#endif

std::string agentname  = "cpu_monitor";
std::string nodename;

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat
char buf4[512];

char sohstring[] = "{\"device_cpu_utc_000\",\"device_cpu_disk_000\",\"device_cpu_maxdisk_000\",\"device_cpu_maxmem_000\",\"device_cpu_mem_000\",\"device_cpu_load_000\",\"}";

#define MAXBUFFERSIZE 256 // comm buffer for agents

cosmosstruc *cdata;  // to access the cosmos data
ElapsedTime et;
#endif //COSMOS_MAC_OS


static clock_t lastCPU, lastSysCPU, lastUserCPU;
static int numProcessors;


void initCpuUtilization(){
    FILE* file;
    struct tms timeSample;
    char line[128];


    lastCPU = times(&timeSample);
    lastSysCPU = timeSample.tms_stime;
    lastUserCPU = timeSample.tms_utime;


    file = fopen("/proc/cpuinfo", "r");
    numProcessors = 0;

    bool foundProcessorTag = false;
    while(fgets(line, 128, file) != NULL){
        if (strncmp(line, "processor", 9) == 0) {
            numProcessors++;
            foundProcessorTag = true;
        }
    }
    fclose(file);

    // if it didn't find the processor tag inside /proc/cpuinfo
    // then assume that there is at least one cpu
    if (foundProcessorTag == false) {
        numProcessors = 1.0;
    }
}



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

    // Establish the command channel and heartbeat
    if (!(cdata = agent_setup_server(NetworkType::UDP,
                                     nodename.c_str(),
                                     agentname.c_str(),
                                     5.0,
                                     0,
                                     AGENTMAXBUFFER)))
    {
        std::cout << agentname << ": agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<std::endl;
        exit (AGENT_ERROR_JSON_CREATE);
    } else {
        std::cout<<"Starting " << agentname << " ... OK" << std::endl;
    }

    // Add additional requests

    if ((iretn=agent_add_request(cdata, (char *)"soh",request_soh)))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"diskSize",request_diskSize,"","get disk size in GB")))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"diskUsed",request_diskUsed,"","get disk used in GB")))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"diskFree",request_diskFree,"","get disk free in GB")))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"diskFreePercent",request_diskFreePercent,"","get disk free in %")))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"mem",request_mem)))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"load",request_load)))
        exit (iretn);
    if ((iretn=agent_add_request(cdata, (char *)"mem_percent",request_mempercent)))
        exit (iretn);

    if ((iretn=agent_add_request(cdata, (char *)"cpuProc",request_cpuProcess,"","get the % CPU for this process")))
        exit (iretn);

    if ((iretn=agent_add_request(cdata, (char *)"printStatus",request_printStatus,"","print the status data")))
        exit (iretn);

    agent_set_sohstring(cdata, sohstring);


    initCpuUtilization();

    // Start our own thread
    iretn = myagent();

    return 0;
#endif //COSMOS_MAC_OS
}

void stressCPU(){
    double temp;
    for (int i = 0; i< 40000; i++) {
        temp = sqrt(i)*i/log(i);
    }
}

#ifndef COSMOS_MAC_OS
int myagent()
{
    std::cout << agentname << " ...online " << std::endl;
    ElapsedTime et;
    et.start();

    // Start performing the body of the agent
    while(agent_running(cdata))
    {

        COSMOS_SLEEP(1);

        cdata[0].devspec.cpu[0]->gen.utc = currentmjd();

        // why ?
        //		if (et.split() == 5.)
        //		{
        //			et.reset();
#ifdef COSMOS_WIN_OS
        cdata[0].devspec.cpu[0]->load = GetWindowsCPULoad();
        cdata[0].devspec.cpu[0]->disk = GetWindowsUsedDisk();
        cdata[0].devspec.cpu[0]->mem = GetWindowsVirtualMem();
        cdata[0].devspec.cpu[0]->maxdisk = GetWindowsTotalDisk();
        cdata[0].devspec.cpu[0]->maxmem = GetWindowsTotalVirtualMem();
#else
        cdata[0].devspec.cpu[0]->load    = GetLinuxCPULoad();
        // disk
        cdata[0].devspec.cpu[0]->disk    = GetLinuxDiskUsedGB(); // TODO: rename disk to diskUsed
        cdata[0].devspec.cpu[0]->maxdisk = GetLinuxDiskSizeGB(); // TODO: rename disk to diskSize
        //diskFree = GetLinuxDiskFree();
        //cdata[0].devspec.cpu[0]->diskFree    = GetLinuxDiskFree(); // TODO: rename disk to diskUsed

        cdata[0].devspec.cpu[0]->mem     = GetLinuxVirtualMem();
        cdata[0].devspec.cpu[0]->maxmem  = GetLinuxTotalVirtualMem();
#endif
        //		}
        //		std::cout << et.lap() << "           " << mempercent << std::endl;
        //		et.start();

        if (printStatus) {
            cout << "Load," << cdata[0].devspec.cpu[0]->load << ", ";
            cout << "DiskSize[GB]," << cdata[0].devspec.cpu[0]->maxdisk << ", ";
            cout << "DiskUsed[GB]," << cdata[0].devspec.cpu[0]->disk << ", ";
            cout << "DiskFree[GB]," << GetLinuxDiskFreeGB() << ", ";
            cout << "CPU Proc[%]," << GetLinuxCPUCurrentUseByProcess() << endl;

        }

        stressCPU();

    }

    agent_shutdown_server(cdata);
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
float GetLinuxCPULoad()
{
    float load = CalculateLinuxCPULoad();
    if (load == -1)
    {
        printf("Could not get load average\n");
        return -1;
    }

    return load;
}


std::string exec(const char* cmd) {
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

float GetLinuxCPUCurrentUseByProcess()
{
    float percent;

    // $ ps -C agent_cpu -o %cpu,%mem
    // $ top -b -n 3 | grep agent_cpu

    // the following core does not give
    // same results as top ... so let's use 'ps' to get this information
    //    struct tms timeSample;
    //    clock_t now;

    //    now = times(&timeSample);
    //    if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
    //            timeSample.tms_utime < lastUserCPU){
    //        //Overflow detection. Just skip this value.
    //        percent = -1.0;
    //    }
    //    else{
    //        percent = (timeSample.tms_stime - lastSysCPU) +
    //                (timeSample.tms_utime - lastUserCPU);
    //        percent /= (now - lastCPU);
    //        percent /= numProcessors;
    //        percent *= 100.0;
    //    }
    //    lastCPU = now;
    //    lastSysCPU = timeSample.tms_stime;
    //    lastUserCPU = timeSample.tms_utime;


    // using 'ps' command
    using std::string;
    string procInfo = exec("ps -C agent_cpu -o %cpu,%mem");
    std::size_t findNewLine = procInfo.find("\n");
    procInfo = procInfo.substr(findNewLine);

    // remove '\n'
    procInfo.replace(procInfo.find("\n"),1,"");
    procInfo.replace(procInfo.find("\n"),1,"");

    StringParser sp(procInfo);
    percent = sp.getFieldNumberAsDouble(1);

    //    cout << procInfo << endl;

    return percent;

}

uint64_t GetLinuxDiskUsed()
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
    disk_size = GetLinuxDiskSize();

    // free disk in bytes
    // free = (int64_t)buf.f_frsize * (int64_t)buf.f_bfree;
    disk_free = GetLinuxDiskFree();

    // total disk used in bytes
    disk_used = disk_size - disk_free;

    //    return (used) * 0.000976563; // convert byte to kilobytes
    return (disk_used);
}

// get the disk size in bytes
uint64_t GetLinuxDiskSize()
{
    struct statvfs buf;
    uint64_t disk_size;

    statvfs("/",&buf);

    disk_size = (uint64_t)buf.f_frsize * (uint64_t)buf.f_blocks;

    return disk_size;
}

// get the free disk in bytes
uint64_t GetLinuxDiskFree()
{
    struct statvfs buf;
    uint64_t diskFree;

    statvfs("/",&buf);

    diskFree = (uint64_t)buf.f_frsize * (uint64_t)buf.f_bfree;

    return diskFree;
}


float GetLinuxDiskSizeGB()
{
    // convert from Byte to GB
    return (float)GetLinuxDiskSize()/GB;
}

float GetLinuxDiskUsedGB()
{
    // convert from Byte to GB
    return (float)GetLinuxDiskUsed()/GB;
}


float GetLinuxDiskFreeGB()
{
    // convert from Byte to GB
    return (float)GetLinuxDiskFree()/GB;
}



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

float CalculateLinuxCPULoad()
{
    FILE *f;
    float load = 0.0;
    int	n;

    if ((f = fopen("/proc/loadavg", "r")) == NULL)
        return -1;

    n = fscanf(f, "%f", &load);
    fclose(f);

    if (n != 1)
        return -1;

    return load;
}

#endif
int32_t request_soh(char *request, char* response, void *)
{
    std::string rjstring;
    //	strcpy(response,json_of_list(rjstring,sohstring,cdata));
    strcpy(response,json_of_table(rjstring, cdata[0].agent[0].sohtable, cdata));

    return 0;
}

int32_t request_mem(char *request, char* response, void *)
{
    return (sprintf(response, "%f", cdata[0].devspec.cpu[0]->mem));
}

// ----------------------------------------------
// disk
int32_t request_diskSize(char *request, char* response, void *)
{
    return (sprintf(response, "%f", cdata[0].devspec.cpu[0]->maxdisk));
}

int32_t request_diskUsed(char *request, char* response, void *)
{
    return (sprintf(response, "%f", cdata[0].devspec.cpu[0]->disk));
}

int32_t request_diskFree(char *request, char* response, void *)
{
    // TODO: implement diskFree
    //return (sprintf(response, "%.1f", cdata[0].devspec.cpu[0]->disk));

    // in the mean time use this
    return (sprintf(response, "%f", GetLinuxDiskFreeGB()));

}


int32_t request_load (char *request, char* response, void *)
{
    return (sprintf(response, "%.2f", cdata[0].devspec.cpu[0]->load));
}

int32_t request_diskFreePercent (char *request, char *response, void *)
{
#ifdef COSMOS_WIN_OS
    float diskpercent = GetWindowsUsedDisk() / GetWindowsTotalDisk();
#else
    float diskpercent = GetLinuxDiskUsedGB() / GetLinuxDiskSizeGB();
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

    return (sprintf(response, "%f", GetLinuxCPUCurrentUseByProcess()));
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
        cdata = json_create();
        strcpy(cdata->node.name, nodename.c_str());
        cdata->node.type = NODE_TYPE_COMPUTER;

        cdata->node.piece_cnt = 1;
        cdata->piece.resize(1);
        cdata->piece[0].cidx = 0;
        strcpy(cdata->piece[0].name, "Main CPU");
        cdata->piece[0].type = PIECE_TYPE_DIMENSIONLESS;
        cdata->piece[0].emi = .8;
        cdata->piece[0].abs = .88;
        cdata->piece[0].hcap = 800;
        cdata->piece[0].hcon = 237;
        cdata->piece[0].pnt_cnt = 1;
        for (uint16_t i=0; i<3; ++i)
        {
            cdata->piece[0].points[0].col[i] = 0.;
        }
        json_addpieceentry(0,cdata);

        cdata->node.device_cnt = 1;
        cdata->device.resize(1);
        cdata->devspec.cpu_cnt = 1;
        cdata->device[0].all.gen.pidx = 0;
        cdata->device[0].all.gen.cidx = 0;
        cdata->device[0].all.gen.didx = 0;
        cdata->device[0].all.gen.type = DEVICE_TYPE_CPU;
        cdata[0].device[0].all.gen.portidx = PORT_TYPE_NONE;
        json_addcompentry(0, cdata);
        json_adddeviceentry(0, cdata);

        int32_t iretn = json_dump_node(cdata);
        json_destroy(cdata);
        return iretn;
    }
    else
    {
        return 0;
    }
    //	if (get_nodedir(nodename).empty())
    //	{
    //		std::cout << "Couldn't find Node directory, making directory now..." << std::endl;
    //#ifdef COSMOS_WIN_OS
    //		node_directory = "M:\\work\\nodes\\" + nodename + "\\";
    //		CreateDirectory (node_directory.c_str(), NULL); // create folder
    //#else
    //		DIR* dir = opendir("/home/glennga/work/nodes/"); // check if on gumstix
    //		if (dir)
    //			node_directory = "/home/glennga/work/nodes/" + nodename + "/";
    //		else
    //			node_directory = "/flight_software/cosmosroot/nodes/" + nodename + "/";
    //		char *nodetemp = &node_directory[0u];
    //		mkdir (nodetemp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    //#endif
    //		std::ofstream ofile;
    //		ofile.open (node_directory + "devices_general.ini");

    //		ofile << "{\"comp_type_000\":5}" <<
    //		"{\"comp_model_000\":0}" <<
    //		"{\"comp_didx_000\":0}" <<
    //		"{\"comp_pidx_000\":0}" <<
    //		"{\"comp_bidx_000\":0}" <<
    //		"{\"comp_portidx_000\":0}" <<
    //		"{\"comp_nvolt_000\":0}" <<
    //		"{\"comp_namp_000\":0}" << std::endl;
    //		ofile.close();

    //		ofile.open (node_directory + "devices_specific.ini");
    //		ofile << "{\"device_cpu_maxdisk_000\":0}" <<
    //		"{\"device_cpu_maxmem_000\":0}" <<
    //		"{\"device_cpu_maxload_000\":0}" << std::endl;
    //		ofile.close();

    //		ofile.open (node_directory + "events.dict");
    //		ofile << "{\"event_name\":\"UMB_IN\"}{\"event_type\":4614}{\"event_flag\":249878}{\"event_condition\":\"(\"node_loc_pos_earthsep\"<0.)\"}{\"event_data\":\"node_loc_pos_earthsep\"}" <<
    //		"{\"event_name\":\"UMB_OUT\"}{\"event_type\":4614}{\"event_flag\":258070}{\"event_condition\":\"(\"node_loc_pos_earthsep\">0.)\"}{\"event_data\":\"node_loc_pos_pos_earthsep\"}" <<
    //		"{\"event_name\":\"N60D\"}{\"event_type\":4354}{\"event_flag\":229433}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\"<1.0472)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"N30D\"}{\"event_type\":4354}{\"event_flag\":229433}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\"<0.5236)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"S60D\"}{\"event_type\":4354}{\"event_flag\":229433}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\"<-1.0472)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"S30D\"}{\"event_type\":4354}{\"event_flag\":229433}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\"<-0.5236)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"N60A\"}{\"event_type\":4353}{\"event_flag\":229433}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\">1.0472)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"N30A\"}{\"event_type\":4353}{\"event_flag\":229433}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\">0.5236)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"S60A\"}{\"event_type\":4353}{\"event_flag\":229433}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\">-1.0472)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"S30A\"}{\"event_type\":4353}{\"event_flag\":229433}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\">-0.5236)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"EQD\"}{\"event_type\":4354}{\"event_flag\":229417}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\"<0.0)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"EQA\"}{\"event_type\":4353}{\"event_flag\":229417}{\"event_condition\":\"(\"node_loc_pos_geod_s_lat\">0.0)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"MaxN\"}{\"event_type\":4368}{\"event_flag\":229417}{\"event_condition\":\"(\"node_loc_pos_geod_v_lat\"<0.)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"MaxS\"}{\"event_type\":4384}{\"event_flag\":229417}{\"event_condition\":\"(\"node_loc_pos_geod_v_lat\">0.)\"}{\"event_data\":\"node_loc_pos_geod_s_lat\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_000}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_000\"=5)&(\"target_elto_000\">\"target_min_000\"))\"}{\"event_data\":\"target_elto_000\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_000}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_000\"=5)&(\"target_elto_000\"<\"target_min_000\"))\"}{\"event_data\":\"target_elto_000\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_000}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_000\"=5)&(\"target_close_000\"<0))\"}{\"event_data\":\"target_range_000\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_000}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_000\"=1)&(\"target_elto_000\">\"target_min_000\"))\"}{\"event_data\":\"target_elto_000\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_000}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_000\"=1)&(\"target_elto_000\">(\"target_min_000\"+.0873)))\"}{\"event_data\":\"target_elto_000\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_000}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_000\"=1)&(\"target_elto_000\">(\"target_min_000\"+.1745)))\"}{\"event_data\":\"target_elto_000\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_000}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_000\"=1)&(\"target_elto_000\"<\"target_min_000\"))\"}{\"event_data\":\"target_elto_000\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_000}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_000\"=1)&(\"target_elto_000\"<(\"target_min_000\"+.0873)))\"}{\"event_data\":\"target_elto_000\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_000}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_000\"=1)&(\"target_elto_000\"<(\"target_min_000\"+.1745)))\"}{\"event_data\":\"target_elto_000\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_001}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_001\"=5)&(\"target_elto_001\">\"target_min_001\"))\"}{\"event_data\":\"target_elto_001\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_001}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_001\"=5)&(\"target_elto_001\"<\"target_min_001\"))\"}{\"event_data\":\"target_elto_001\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_001}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_001\"=5)&(\"target_close_001\"<0))\"}{\"event_data\":\"target_range_001\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_001}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_001\"=1)&(\"target_elto_001\">\"target_min_001\"))\"}{\"event_data\":\"target_elto_001\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_001}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_001\"=1)&(\"target_elto_001\">(\"target_min_001\"+.0873)))\"}{\"event_data\":\"target_elto_001\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_001}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_001\"=1)&(\"target_elto_001\">(\"target_min_001\"+.1745)))\"}{\"event_data\":\"target_elto_001\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_001}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_001\"=1)&(\"target_elto_001\"<\"target_min_001\"))\"}{\"event_data\":\"target_elto_001\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_001}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_001\"=1)&(\"target_elto_001\"<(\"target_min_001\"+.0873)))\"}{\"event_data\":\"target_elto_001\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_001}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_001\"=1)&(\"target_elto_001\"<(\"target_min_001\"+.1745)))\"}{\"event_data\":\"target_elto_001\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_002}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_002\"=5)&(\"target_elto_002\">\"target_min_002\"))\"}{\"event_data\":\"target_elto_002\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_002}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_002\"=5)&(\"target_elto_002\"<\"target_min_002\"))\"}{\"event_data\":\"target_elto_002\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_002}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_002\"=5)&(\"target_close_002\"<0))\"}{\"event_data\":\"target_range_002\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_002}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_002\"=1)&(\"target_elto_002\">\"target_min_002\"))\"}{\"event_data\":\"target_elto_002\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_002}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_002\"=1)&(\"target_elto_002\">(\"target_min_002\"+.0873)))\"}{\"event_data\":\"target_elto_002\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_002}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_002\"=1)&(\"target_elto_002\">(\"target_min_002\"+.1745)))\"}{\"event_data\":\"target_elto_002\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_002}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_002\"=1)&(\"target_elto_002\"<\"target_min_002\"))\"}{\"event_data\":\"target_elto_002\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_002}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_002\"=1)&(\"target_elto_002\"<(\"target_min_002\"+.0873)))\"}{\"event_data\":\"target_elto_002\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_002}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_002\"=1)&(\"target_elto_002\"<(\"target_min_002\"+.1745)))\"}{\"event_data\":\"target_elto_002\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_003}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_003\"=5)&(\"target_elto_003\">\"target_min_003\"))\"}{\"event_data\":\"target_elto_003\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_003}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_003\"=5)&(\"target_elto_003\"<\"target_min_003\"))\"}{\"event_data\":\"target_elto_003\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_003}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_003\"=5)&(\"target_close_003\"<0))\"}{\"event_data\":\"target_range_003\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_003}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_003\"=1)&(\"target_elto_003\">\"target_min_003\"))\"}{\"event_data\":\"target_elto_003\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_003}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_003\"=1)&(\"target_elto_003\">(\"target_min_003\"+.0873)))\"}{\"event_data\":\"target_elto_003\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_003}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_003\"=1)&(\"target_elto_003\">(\"target_min_003\"+.1745)))\"}{\"event_data\":\"target_elto_003\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_003}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_003\"=1)&(\"target_elto_003\"<\"target_min_003\"))\"}{\"event_data\":\"target_elto_003\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_003}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_003\"=1)&(\"target_elto_003\"<(\"target_min_003\"+.0873)))\"}{\"event_data\":\"target_elto_003\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_003}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_003\"=1)&(\"target_elto_003\"<(\"target_min_003\"+.1745)))\"}{\"event_data\":\"target_elto_003\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_004}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_004\"=5)&(\"target_elto_004\">\"target_min_004\"))\"}{\"event_data\":\"target_elto_004\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_004}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_004\"=5)&(\"target_elto_004\"<\"target_min_004\"))\"}{\"event_data\":\"target_elto_004\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_004}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_004\"=5)&(\"target_close_004\"<0))\"}{\"event_data\":\"target_range_004\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_004}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_004\"=1)&(\"target_elto_004\">\"target_min_004\"))\"}{\"event_data\":\"target_elto_004\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_004}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_004\"=1)&(\"target_elto_004\">(\"target_min_004\"+.0873)))\"}{\"event_data\":\"target_elto_004\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_004}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_004\"=1)&(\"target_elto_004\">(\"target_min_004\"+.1745)))\"}{\"event_data\":\"target_elto_004\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_004}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_004\"=1)&(\"target_elto_004\"<\"target_min_004\"))\"}{\"event_data\":\"target_elto_004\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_004}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_004\"=1)&(\"target_elto_004\"<(\"target_min_004\"+.0873)))\"}{\"event_data\":\"target_elto_004\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_004}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_004\"=1)&(\"target_elto_004\"<(\"target_min_004\"+.1745)))\"}{\"event_data\":\"target_elto_004\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_005}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_005\"=5)&(\"target_elto_005\">\"target_min_005\"))\"}{\"event_data\":\"target_elto_005\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_005}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_005\"=5)&(\"target_elto_005\"<\"target_min_005\"))\"}{\"event_data\":\"target_elto_005\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_005}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_005\"=5)&(\"target_close_005\"<0))\"}{\"event_data\":\"target_range_005\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_005}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_005\"=1)&(\"target_elto_005\">\"target_min_005\"))\"}{\"event_data\":\"target_elto_005\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_005}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_005\"=1)&(\"target_elto_005\">(\"target_min_005\"+.0873)))\"}{\"event_data\":\"target_elto_005\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_005}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_005\"=1)&(\"target_elto_005\">(\"target_min_005\"+.1745)))\"}{\"event_data\":\"target_elto_005\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_005}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_005\"=1)&(\"target_elto_005\"<\"target_min_005\"))\"}{\"event_data\":\"target_elto_005\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_005}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_005\"=1)&(\"target_elto_005\"<(\"target_min_005\"+.0873)))\"}{\"event_data\":\"target_elto_005\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_005}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_005\"=1)&(\"target_elto_005\"<(\"target_min_005\"+.1745)))\"}{\"event_data\":\"target_elto_005\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_006}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_006\"=5)&(\"target_elto_006\">\"target_min_006\"))\"}{\"event_data\":\"target_elto_006\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_006}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_006\"=5)&(\"target_elto_006\"<\"target_min_006\"))\"}{\"event_data\":\"target_elto_006\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_006}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_006\"=5)&(\"target_close_006\"<0))\"}{\"event_data\":\"target_range_006\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_006}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_006\"=1)&(\"target_elto_006\">\"target_min_006\"))\"}{\"event_data\":\"target_elto_006\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_006}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_006\"=1)&(\"target_elto_006\">(\"target_min_006\"+.0873)))\"}{\"event_data\":\"target_elto_006\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_006}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_006\"=1)&(\"target_elto_006\">(\"target_min_006\"+.1745)))\"}{\"event_data\":\"target_elto_006\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_006}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_006\"=1)&(\"target_elto_006\"<\"target_min_006\"))\"}{\"event_data\":\"target_elto_006\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_006}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_006\"=1)&(\"target_elto_006\"<(\"target_min_006\"+.0873)))\"}{\"event_data\":\"target_elto_006\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_006}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_006\"=1)&(\"target_elto_006\"<(\"target_min_006\"+.1745)))\"}{\"event_data\":\"target_elto_006\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_007}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_007\"=5)&(\"target_elto_007\">\"target_min_007\"))\"}{\"event_data\":\"target_elto_007\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_007}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_007\"=5)&(\"target_elto_007\"<\"target_min_007\"))\"}{\"event_data\":\"target_elto_007\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_007}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_007\"=5)&(\"target_close_007\"<0))\"}{\"event_data\":\"target_range_007\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_007}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_007\"=1)&(\"target_elto_007\">\"target_min_007\"))\"}{\"event_data\":\"target_elto_007\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_007}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_007\"=1)&(\"target_elto_007\">(\"target_min_007\"+.0873)))\"}{\"event_data\":\"target_elto_007\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_007}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_007\"=1)&(\"target_elto_007\">(\"target_min_007\"+.1745)))\"}{\"event_data\":\"target_elto_007\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_007}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_007\"=1)&(\"target_elto_007\"<\"target_min_007\"))\"}{\"event_data\":\"target_elto_007\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_007}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_007\"=1)&(\"target_elto_007\"<(\"target_min_007\"+.0873)))\"}{\"event_data\":\"target_elto_007\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_007}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_007\"=1)&(\"target_elto_007\"<(\"target_min_007\"+.1745)))\"}{\"event_data\":\"target_elto_007\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_008}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_008\"=5)&(\"target_elto_008\">\"target_min_008\"))\"}{\"event_data\":\"target_elto_008\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_008}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_008\"=5)&(\"target_elto_008\"<\"target_min_008\"))\"}{\"event_data\":\"target_elto_008\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_008}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_008\"=5)&(\"target_close_008\"<0))\"}{\"event_data\":\"target_range_008\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_008}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_008\"=1)&(\"target_elto_008\">\"target_min_008\"))\"}{\"event_data\":\"target_elto_008\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_008}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_008\"=1)&(\"target_elto_008\">(\"target_min_008\"+.0873)))\"}{\"event_data\":\"target_elto_008\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_008}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_008\"=1)&(\"target_elto_008\">(\"target_min_008\"+.1745)))\"}{\"event_data\":\"target_elto_008\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_008}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_008\"=1)&(\"target_elto_008\"<\"target_min_008\"))\"}{\"event_data\":\"target_elto_008\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_008}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_008\"=1)&(\"target_elto_008\"<(\"target_min_008\"+.0873)))\"}{\"event_data\":\"target_elto_008\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_008}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_008\"=1)&(\"target_elto_008\"<(\"target_min_008\"+.1745)))\"}{\"event_data\":\"target_elto_008\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_009}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_009\"=5)&(\"target_elto_009\">\"target_min_009\"))\"}{\"event_data\":\"target_elto_009\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_009}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_009\"=5)&(\"target_elto_009\"<\"target_min_009\"))\"}{\"event_data\":\"target_elto_009\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_009}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_009\"=5)&(\"target_close_009\"<0))\"}{\"event_data\":\"target_range_009\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_009}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_009\"=1)&(\"target_elto_009\">\"target_min_009\"))\"}{\"event_data\":\"target_elto_009\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_009}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_009\"=1)&(\"target_elto_009\">(\"target_min_009\"+.0873)))\"}{\"event_data\":\"target_elto_009\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_009}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_009\"=1)&(\"target_elto_009\">(\"target_min_009\"+.1745)))\"}{\"event_data\":\"target_elto_009\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_009}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_009\"=1)&(\"target_elto_009\"<\"target_min_009\"))\"}{\"event_data\":\"target_elto_009\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_009}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_009\"=1)&(\"target_elto_009\"<(\"target_min_009\"+.0873)))\"}{\"event_data\":\"target_elto_009\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_009}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_009\"=1)&(\"target_elto_009\"<(\"target_min_009\"+.1745)))\"}{\"event_data\":\"target_elto_009\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_010}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_010\"=5)&(\"target_elto_010\">\"target_min_010\"))\"}{\"event_data\":\"target_elto_010\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_010}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_010\"=5)&(\"target_elto_010\"<\"target_min_010\"))\"}{\"event_data\":\"target_elto_010\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_010}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_010\"=5)&(\"target_close_010\"<0))\"}{\"event_data\":\"target_range_010\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_010}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_010\"=1)&(\"target_elto_010\">\"target_min_010\"))\"}{\"event_data\":\"target_elto_010\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_010}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_010\"=1)&(\"target_elto_010\">(\"target_min_010\"+.0873)))\"}{\"event_data\":\"target_elto_010\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_010}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_010\"=1)&(\"target_elto_010\">(\"target_min_010\"+.1745)))\"}{\"event_data\":\"target_elto_010\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_010}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_010\"=1)&(\"target_elto_010\"<\"target_min_010\"))\"}{\"event_data\":\"target_elto_010\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_010}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_010\"=1)&(\"target_elto_010\"<(\"target_min_010\"+.0873)))\"}{\"event_data\":\"target_elto_010\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_010}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_010\"=1)&(\"target_elto_010\"<(\"target_min_010\"+.1745)))\"}{\"event_data\":\"target_elto_010\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_011}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_011\"=5)&(\"target_elto_011\">\"target_min_011\"))\"}{\"event_data\":\"target_elto_011\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_011}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_011\"=5)&(\"target_elto_011\"<\"target_min_011\"))\"}{\"event_data\":\"target_elto_011\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_011}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_011\"=5)&(\"target_close_011\"<0))\"}{\"event_data\":\"target_range_011\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_011}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_011\"=1)&(\"target_elto_011\">\"target_min_011\"))\"}{\"event_data\":\"target_elto_011\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_011}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_011\"=1)&(\"target_elto_011\">(\"target_min_011\"+.0873)))\"}{\"event_data\":\"target_elto_011\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_011}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_011\"=1)&(\"target_elto_011\">(\"target_min_011\"+.1745)))\"}{\"event_data\":\"target_elto_011\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_011}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_011\"=1)&(\"target_elto_011\"<\"target_min_011\"))\"}{\"event_data\":\"target_elto_011\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_011}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_011\"=1)&(\"target_elto_011\"<(\"target_min_011\"+.0873)))\"}{\"event_data\":\"target_elto_011\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_011}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_011\"=1)&(\"target_elto_011\"<(\"target_min_011\"+.1745)))\"}{\"event_data\":\"target_elto_011\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_012}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_012\"=5)&(\"target_elto_012\">\"target_min_012\"))\"}{\"event_data\":\"target_elto_012\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_012}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_012\"=5)&(\"target_elto_012\"<\"target_min_012\"))\"}{\"event_data\":\"target_elto_012\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_012}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_012\"=5)&(\"target_close_012\"<0))\"}{\"event_data\":\"target_range_012\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_012}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_012\"=1)&(\"target_elto_012\">\"target_min_012\"))\"}{\"event_data\":\"target_elto_012\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_012}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_012\"=1)&(\"target_elto_012\">(\"target_min_012\"+.0873)))\"}{\"event_data\":\"target_elto_012\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_012}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_012\"=1)&(\"target_elto_012\">(\"target_min_012\"+.1745)))\"}{\"event_data\":\"target_elto_012\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_012}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_012\"=1)&(\"target_elto_012\"<\"target_min_012\"))\"}{\"event_data\":\"target_elto_012\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_012}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_012\"=1)&(\"target_elto_012\"<(\"target_min_012\"+.0873)))\"}{\"event_data\":\"target_elto_012\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_012}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_012\"=1)&(\"target_elto_012\"<(\"target_min_012\"+.1745)))\"}{\"event_data\":\"target_elto_012\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_013}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_013\"=5)&(\"target_elto_013\">\"target_min_013\"))\"}{\"event_data\":\"target_elto_013\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_013}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_013\"=5)&(\"target_elto_013\"<\"target_min_013\"))\"}{\"event_data\":\"target_elto_013\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_013}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_013\"=5)&(\"target_close_013\"<0))\"}{\"event_data\":\"target_range_013\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_013}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_013\"=1)&(\"target_elto_013\">\"target_min_013\"))\"}{\"event_data\":\"target_elto_013\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_013}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_013\"=1)&(\"target_elto_013\">(\"target_min_013\"+.0873)))\"}{\"event_data\":\"target_elto_013\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_013}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_013\"=1)&(\"target_elto_013\">(\"target_min_013\"+.1745)))\"}{\"event_data\":\"target_elto_013\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_013}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_013\"=1)&(\"target_elto_013\"<\"target_min_013\"))\"}{\"event_data\":\"target_elto_013\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_013}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_013\"=1)&(\"target_elto_013\"<(\"target_min_013\"+.0873)))\"}{\"event_data\":\"target_elto_013\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_013}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_013\"=1)&(\"target_elto_013\"<(\"target_min_013\"+.1745)))\"}{\"event_data\":\"target_elto_013\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_014}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_014\"=5)&(\"target_elto_014\">\"target_min_014\"))\"}{\"event_data\":\"target_elto_014\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_014}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_014\"=5)&(\"target_elto_014\"<\"target_min_014\"))\"}{\"event_data\":\"target_elto_014\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_014}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_014\"=5)&(\"target_close_014\"<0))\"}{\"event_data\":\"target_range_014\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_014}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_014\"=1)&(\"target_elto_014\">\"target_min_014\"))\"}{\"event_data\":\"target_elto_014\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_014}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_014\"=1)&(\"target_elto_014\">(\"target_min_014\"+.0873)))\"}{\"event_data\":\"target_elto_014\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_014}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_014\"=1)&(\"target_elto_014\">(\"target_min_014\"+.1745)))\"}{\"event_data\":\"target_elto_014\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_014}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_014\"=1)&(\"target_elto_014\"<\"target_min_014\"))\"}{\"event_data\":\"target_elto_014\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_014}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_014\"=1)&(\"target_elto_014\"<(\"target_min_014\"+.0873)))\"}{\"event_data\":\"target_elto_014\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_014}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_014\"=1)&(\"target_elto_014\"<(\"target_min_014\"+.1745)))\"}{\"event_data\":\"target_elto_014\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_015}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_015\"=5)&(\"target_elto_015\">\"target_min_015\"))\"}{\"event_data\":\"target_elto_015\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_015}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_015\"=5)&(\"target_elto_015\"<\"target_min_015\"))\"}{\"event_data\":\"target_elto_015\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_015}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_015\"=5)&(\"target_close_015\"<0))\"}{\"event_data\":\"target_range_015\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_015}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_015\"=1)&(\"target_elto_015\">\"target_min_015\"))\"}{\"event_data\":\"target_elto_015\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_015}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_015\"=1)&(\"target_elto_015\">(\"target_min_015\"+.0873)))\"}{\"event_data\":\"target_elto_015\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_015}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_015\"=1)&(\"target_elto_015\">(\"target_min_015\"+.1745)))\"}{\"event_data\":\"target_elto_015\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_015}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_015\"=1)&(\"target_elto_015\"<\"target_min_015\"))\"}{\"event_data\":\"target_elto_015\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_015}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_015\"=1)&(\"target_elto_015\"<(\"target_min_015\"+.0873)))\"}{\"event_data\":\"target_elto_015\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_015}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_015\"=1)&(\"target_elto_015\"<(\"target_min_015\"+.1745)))\"}{\"event_data\":\"target_elto_015\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_016}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_016\"=5)&(\"target_elto_016\">\"target_min_016\"))\"}{\"event_data\":\"target_elto_016\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_016}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_016\"=5)&(\"target_elto_016\"<\"target_min_016\"))\"}{\"event_data\":\"target_elto_016\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_016}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_016\"=5)&(\"target_close_016\"<0))\"}{\"event_data\":\"target_range_016\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_016}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_016\"=1)&(\"target_elto_016\">\"target_min_016\"))\"}{\"event_data\":\"target_elto_016\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_016}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_016\"=1)&(\"target_elto_016\">(\"target_min_016\"+.0873)))\"}{\"event_data\":\"target_elto_016\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_016}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_016\"=1)&(\"target_elto_016\">(\"target_min_016\"+.1745)))\"}{\"event_data\":\"target_elto_016\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_016}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_016\"=1)&(\"target_elto_016\"<\"target_min_016\"))\"}{\"event_data\":\"target_elto_016\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_016}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_016\"=1)&(\"target_elto_016\"<(\"target_min_016\"+.0873)))\"}{\"event_data\":\"target_elto_016\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_016}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_016\"=1)&(\"target_elto_016\"<(\"target_min_016\"+.1745)))\"}{\"event_data\":\"target_elto_016\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_017}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_017\"=5)&(\"target_elto_017\">\"target_min_017\"))\"}{\"event_data\":\"target_elto_017\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_017}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_017\"=5)&(\"target_elto_017\"<\"target_min_017\"))\"}{\"event_data\":\"target_elto_017\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_017}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_017\"=5)&(\"target_close_017\"<0))\"}{\"event_data\":\"target_range_017\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_017}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_017\"=1)&(\"target_elto_017\">\"target_min_017\"))\"}{\"event_data\":\"target_elto_017\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_017}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_017\"=1)&(\"target_elto_017\">(\"target_min_017\"+.0873)))\"}{\"event_data\":\"target_elto_017\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_017}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_017\"=1)&(\"target_elto_017\">(\"target_min_017\"+.1745)))\"}{\"event_data\":\"target_elto_017\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_017}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_017\"=1)&(\"target_elto_017\"<\"target_min_017\"))\"}{\"event_data\":\"target_elto_017\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_017}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_017\"=1)&(\"target_elto_017\"<(\"target_min_017\"+.0873)))\"}{\"event_data\":\"target_elto_017\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_017}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_017\"=1)&(\"target_elto_017\"<(\"target_min_017\"+.1745)))\"}{\"event_data\":\"target_elto_017\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_018}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_018\"=5)&(\"target_elto_018\">\"target_min_018\"))\"}{\"event_data\":\"target_elto_018\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_018}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_018\"=5)&(\"target_elto_018\"<\"target_min_018\"))\"}{\"event_data\":\"target_elto_018\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_018}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_018\"=5)&(\"target_close_018\"<0))\"}{\"event_data\":\"target_range_018\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_018}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_018\"=1)&(\"target_elto_018\">\"target_min_018\"))\"}{\"event_data\":\"target_elto_018\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_018}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_018\"=1)&(\"target_elto_018\">(\"target_min_018\"+.0873)))\"}{\"event_data\":\"target_elto_018\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_018}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_018\"=1)&(\"target_elto_018\">(\"target_min_018\"+.1745)))\"}{\"event_data\":\"target_elto_018\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_018}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_018\"=1)&(\"target_elto_018\"<\"target_min_018\"))\"}{\"event_data\":\"target_elto_018\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_018}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_018\"=1)&(\"target_elto_018\"<(\"target_min_018\"+.0873)))\"}{\"event_data\":\"target_elto_018\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_018}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_018\"=1)&(\"target_elto_018\"<(\"target_min_018\"+.1745)))\"}{\"event_data\":\"target_elto_018\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_019}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_019\"=5)&(\"target_elto_019\">\"target_min_019\"))\"}{\"event_data\":\"target_elto_019\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_019}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_019\"=5)&(\"target_elto_019\"<\"target_min_019\"))\"}{\"event_data\":\"target_elto_019\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_019}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_019\"=5)&(\"target_close_019\"<0))\"}{\"event_data\":\"target_range_019\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_019}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_019\"=1)&(\"target_elto_019\">\"target_min_019\"))\"}{\"event_data\":\"target_elto_019\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_019}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_019\"=1)&(\"target_elto_019\">(\"target_min_019\"+.0873)))\"}{\"event_data\":\"target_elto_019\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_019}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_019\"=1)&(\"target_elto_019\">(\"target_min_019\"+.1745)))\"}{\"event_data\":\"target_elto_019\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_019}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_019\"=1)&(\"target_elto_019\"<\"target_min_019\"))\"}{\"event_data\":\"target_elto_019\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_019}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_019\"=1)&(\"target_elto_019\"<(\"target_min_019\"+.0873)))\"}{\"event_data\":\"target_elto_019\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_019}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_019\"=1)&(\"target_elto_019\"<(\"target_min_019\"+.1745)))\"}{\"event_data\":\"target_elto_019\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_020}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_020\"=5)&(\"target_elto_020\">\"target_min_020\"))\"}{\"event_data\":\"target_elto_020\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_020}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_020\"=5)&(\"target_elto_020\"<\"target_min_020\"))\"}{\"event_data\":\"target_elto_020\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_020}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_020\"=5)&(\"target_close_020\"<0))\"}{\"event_data\":\"target_range_020\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_020}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_020\"=1)&(\"target_elto_020\">\"target_min_020\"))\"}{\"event_data\":\"target_elto_020\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_020}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_020\"=1)&(\"target_elto_020\">(\"target_min_020\"+.0873)))\"}{\"event_data\":\"target_elto_020\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_020}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_020\"=1)&(\"target_elto_020\">(\"target_min_020\"+.1745)))\"}{\"event_data\":\"target_elto_020\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_020}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_020\"=1)&(\"target_elto_020\"<\"target_min_020\"))\"}{\"event_data\":\"target_elto_020\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_020}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_020\"=1)&(\"target_elto_020\"<(\"target_min_020\"+.0873)))\"}{\"event_data\":\"target_elto_020\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_020}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_020\"=1)&(\"target_elto_020\"<(\"target_min_020\"+.1745)))\"}{\"event_data\":\"target_elto_020\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_021}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_021\"=5)&(\"target_elto_021\">\"target_min_021\"))\"}{\"event_data\":\"target_elto_021\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_021}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_021\"=5)&(\"target_elto_021\"<\"target_min_021\"))\"}{\"event_data\":\"target_elto_021\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_021}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_021\"=5)&(\"target_close_021\"<0))\"}{\"event_data\":\"target_range_021\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_021}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_021\"=1)&(\"target_elto_021\">\"target_min_021\"))\"}{\"event_data\":\"target_elto_021\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_021}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_021\"=1)&(\"target_elto_021\">(\"target_min_021\"+.0873)))\"}{\"event_data\":\"target_elto_021\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_021}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_021\"=1)&(\"target_elto_021\">(\"target_min_021\"+.1745)))\"}{\"event_data\":\"target_elto_021\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_021}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_021\"=1)&(\"target_elto_021\"<\"target_min_021\"))\"}{\"event_data\":\"target_elto_021\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_021}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_021\"=1)&(\"target_elto_021\"<(\"target_min_021\"+.0873)))\"}{\"event_data\":\"target_elto_021\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_021}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_021\"=1)&(\"target_elto_021\"<(\"target_min_021\"+.1745)))\"}{\"event_data\":\"target_elto_021\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_022}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_022\"=5)&(\"target_elto_022\">\"target_min_022\"))\"}{\"event_data\":\"target_elto_022\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_022}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_022\"=5)&(\"target_elto_022\"<\"target_min_022\"))\"}{\"event_data\":\"target_elto_022\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_022}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_022\"=5)&(\"target_close_022\"<0))\"}{\"event_data\":\"target_range_022\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_022}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_022\"=1)&(\"target_elto_022\">\"target_min_022\"))\"}{\"event_data\":\"target_elto_022\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_022}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_022\"=1)&(\"target_elto_022\">(\"target_min_022\"+.0873)))\"}{\"event_data\":\"target_elto_022\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_022}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_022\"=1)&(\"target_elto_022\">(\"target_min_022\"+.1745)))\"}{\"event_data\":\"target_elto_022\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_022}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_022\"=1)&(\"target_elto_022\"<\"target_min_022\"))\"}{\"event_data\":\"target_elto_022\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_022}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_022\"=1)&(\"target_elto_022\"<(\"target_min_022\"+.0873)))\"}{\"event_data\":\"target_elto_022\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_022}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_022\"=1)&(\"target_elto_022\"<(\"target_min_022\"+.1745)))\"}{\"event_data\":\"target_elto_022\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_023}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_023\"=5)&(\"target_elto_023\">\"target_min_023\"))\"}{\"event_data\":\"target_elto_023\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_023}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_023\"=5)&(\"target_elto_023\"<\"target_min_023\"))\"}{\"event_data\":\"target_elto_023\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_023}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_023\"=5)&(\"target_close_023\"<0))\"}{\"event_data\":\"target_range_023\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_023}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_023\"=1)&(\"target_elto_023\">\"target_min_023\"))\"}{\"event_data\":\"target_elto_023\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_023}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_023\"=1)&(\"target_elto_023\">(\"target_min_023\"+.0873)))\"}{\"event_data\":\"target_elto_023\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_023}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_023\"=1)&(\"target_elto_023\">(\"target_min_023\"+.1745)))\"}{\"event_data\":\"target_elto_023\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_023}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_023\"=1)&(\"target_elto_023\"<\"target_min_023\"))\"}{\"event_data\":\"target_elto_023\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_023}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_023\"=1)&(\"target_elto_023\"<(\"target_min_023\"+.0873)))\"}{\"event_data\":\"target_elto_023\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_023}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_023\"=1)&(\"target_elto_023\"<(\"target_min_023\"+.1745)))\"}{\"event_data\":\"target_elto_023\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_024}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_024\"=5)&(\"target_elto_024\">\"target_min_024\"))\"}{\"event_data\":\"target_elto_024\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_024}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_024\"=5)&(\"target_elto_024\"<\"target_min_024\"))\"}{\"event_data\":\"target_elto_024\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_024}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_024\"=5)&(\"target_close_024\"<0))\"}{\"event_data\":\"target_range_024\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_024}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_024\"=1)&(\"target_elto_024\">\"target_min_024\"))\"}{\"event_data\":\"target_elto_024\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_024}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_024\"=1)&(\"target_elto_024\">(\"target_min_024\"+.0873)))\"}{\"event_data\":\"target_elto_024\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_024}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_024\"=1)&(\"target_elto_024\">(\"target_min_024\"+.1745)))\"}{\"event_data\":\"target_elto_024\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_024}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_024\"=1)&(\"target_elto_024\"<\"target_min_024\"))\"}{\"event_data\":\"target_elto_024\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_024}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_024\"=1)&(\"target_elto_024\"<(\"target_min_024\"+.0873)))\"}{\"event_data\":\"target_elto_024\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_024}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_024\"=1)&(\"target_elto_024\"<(\"target_min_024\"+.1745)))\"}{\"event_data\":\"target_elto_024\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_025}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_025\"=5)&(\"target_elto_025\">\"target_min_025\"))\"}{\"event_data\":\"target_elto_025\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_025}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_025\"=5)&(\"target_elto_025\"<\"target_min_025\"))\"}{\"event_data\":\"target_elto_025\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_025}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_025\"=5)&(\"target_close_025\"<0))\"}{\"event_data\":\"target_range_025\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_025}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_025\"=1)&(\"target_elto_025\">\"target_min_025\"))\"}{\"event_data\":\"target_elto_025\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_025}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_025\"=1)&(\"target_elto_025\">(\"target_min_025\"+.0873)))\"}{\"event_data\":\"target_elto_025\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_025}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_025\"=1)&(\"target_elto_025\">(\"target_min_025\"+.1745)))\"}{\"event_data\":\"target_elto_025\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_025}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_025\"=1)&(\"target_elto_025\"<\"target_min_025\"))\"}{\"event_data\":\"target_elto_025\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_025}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_025\"=1)&(\"target_elto_025\"<(\"target_min_025\"+.0873)))\"}{\"event_data\":\"target_elto_025\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_025}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_025\"=1)&(\"target_elto_025\"<(\"target_min_025\"+.1745)))\"}{\"event_data\":\"target_elto_025\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_026}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_026\"=5)&(\"target_elto_026\">\"target_min_026\"))\"}{\"event_data\":\"target_elto_026\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_026}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_026\"=5)&(\"target_elto_026\"<\"target_min_026\"))\"}{\"event_data\":\"target_elto_026\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_026}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_026\"=5)&(\"target_close_026\"<0))\"}{\"event_data\":\"target_range_026\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_026}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_026\"=1)&(\"target_elto_026\">\"target_min_026\"))\"}{\"event_data\":\"target_elto_026\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_026}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_026\"=1)&(\"target_elto_026\">(\"target_min_026\"+.0873)))\"}{\"event_data\":\"target_elto_026\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_026}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_026\"=1)&(\"target_elto_026\">(\"target_min_026\"+.1745)))\"}{\"event_data\":\"target_elto_026\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_026}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_026\"=1)&(\"target_elto_026\"<\"target_min_026\"))\"}{\"event_data\":\"target_elto_026\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_026}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_026\"=1)&(\"target_elto_026\"<(\"target_min_026\"+.0873)))\"}{\"event_data\":\"target_elto_026\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_026}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_026\"=1)&(\"target_elto_026\"<(\"target_min_026\"+.1745)))\"}{\"event_data\":\"target_elto_026\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_027}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_027\"=5)&(\"target_elto_027\">\"target_min_027\"))\"}{\"event_data\":\"target_elto_027\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_027}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_027\"=5)&(\"target_elto_027\"<\"target_min_027\"))\"}{\"event_data\":\"target_elto_027\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_027}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_027\"=5)&(\"target_close_027\"<0))\"}{\"event_data\":\"target_range_027\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_027}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_027\"=1)&(\"target_elto_027\">\"target_min_027\"))\"}{\"event_data\":\"target_elto_027\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_027}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_027\"=1)&(\"target_elto_027\">(\"target_min_027\"+.0873)))\"}{\"event_data\":\"target_elto_027\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_027}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_027\"=1)&(\"target_elto_027\">(\"target_min_027\"+.1745)))\"}{\"event_data\":\"target_elto_027\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_027}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_027\"=1)&(\"target_elto_027\"<\"target_min_027\"))\"}{\"event_data\":\"target_elto_027\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_027}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_027\"=1)&(\"target_elto_027\"<(\"target_min_027\"+.0873)))\"}{\"event_data\":\"target_elto_027\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_027}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_027\"=1)&(\"target_elto_027\"<(\"target_min_027\"+.1745)))\"}{\"event_data\":\"target_elto_027\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_028}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_028\"=5)&(\"target_elto_028\">\"target_min_028\"))\"}{\"event_data\":\"target_elto_028\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_028}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_028\"=5)&(\"target_elto_028\"<\"target_min_028\"))\"}{\"event_data\":\"target_elto_028\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_028}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_028\"=5)&(\"target_close_028\"<0))\"}{\"event_data\":\"target_range_028\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_028}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_028\"=1)&(\"target_elto_028\">\"target_min_028\"))\"}{\"event_data\":\"target_elto_028\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_028}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_028\"=1)&(\"target_elto_028\">(\"target_min_028\"+.0873)))\"}{\"event_data\":\"target_elto_028\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_028}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_028\"=1)&(\"target_elto_028\">(\"target_min_028\"+.1745)))\"}{\"event_data\":\"target_elto_028\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_028}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_028\"=1)&(\"target_elto_028\"<\"target_min_028\"))\"}{\"event_data\":\"target_elto_028\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_028}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_028\"=1)&(\"target_elto_028\"<(\"target_min_028\"+.0873)))\"}{\"event_data\":\"target_elto_028\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_028}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_028\"=1)&(\"target_elto_028\"<(\"target_min_028\"+.1745)))\"}{\"event_data\":\"target_elto_028\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_029}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_029\"=5)&(\"target_elto_029\">\"target_min_029\"))\"}{\"event_data\":\"target_elto_029\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_029}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_029\"=5)&(\"target_elto_029\"<\"target_min_029\"))\"}{\"event_data\":\"target_elto_029\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_029}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_029\"=5)&(\"target_close_029\"<0))\"}{\"event_data\":\"target_range_029\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_029}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_029\"=1)&(\"target_elto_029\">\"target_min_029\"))\"}{\"event_data\":\"target_elto_029\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_029}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_029\"=1)&(\"target_elto_029\">(\"target_min_029\"+.0873)))\"}{\"event_data\":\"target_elto_029\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_029}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_029\"=1)&(\"target_elto_029\">(\"target_min_029\"+.1745)))\"}{\"event_data\":\"target_elto_029\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_029}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_029\"=1)&(\"target_elto_029\"<\"target_min_029\"))\"}{\"event_data\":\"target_elto_029\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_029}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_029\"=1)&(\"target_elto_029\"<(\"target_min_029\"+.0873)))\"}{\"event_data\":\"target_elto_029\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_029}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_029\"=1)&(\"target_elto_029\"<(\"target_min_029\"+.1745)))\"}{\"event_data\":\"target_elto_029\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_030}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_030\"=5)&(\"target_elto_030\">\"target_min_030\"))\"}{\"event_data\":\"target_elto_030\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_030}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_030\"=5)&(\"target_elto_030\"<\"target_min_030\"))\"}{\"event_data\":\"target_elto_030\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_030}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_030\"=5)&(\"target_close_030\"<0))\"}{\"event_data\":\"target_range_030\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_030}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_030\"=1)&(\"target_elto_030\">\"target_min_030\"))\"}{\"event_data\":\"target_elto_030\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_030}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_030\"=1)&(\"target_elto_030\">(\"target_min_030\"+.0873)))\"}{\"event_data\":\"target_elto_030\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_030}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_030\"=1)&(\"target_elto_030\">(\"target_min_030\"+.1745)))\"}{\"event_data\":\"target_elto_030\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_030}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_030\"=1)&(\"target_elto_030\"<\"target_min_030\"))\"}{\"event_data\":\"target_elto_030\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_030}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_030\"=1)&(\"target_elto_030\"<(\"target_min_030\"+.0873)))\"}{\"event_data\":\"target_elto_030\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_030}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_030\"=1)&(\"target_elto_030\"<(\"target_min_030\"+.1745)))\"}{\"event_data\":\"target_elto_030\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_031}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_031\"=5)&(\"target_elto_031\">\"target_min_031\"))\"}{\"event_data\":\"target_elto_031\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_031}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_031\"=5)&(\"target_elto_031\"<\"target_min_031\"))\"}{\"event_data\":\"target_elto_031\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_031}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_031\"=5)&(\"target_close_031\"<0))\"}{\"event_data\":\"target_range_031\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_031}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_031\"=1)&(\"target_elto_031\">\"target_min_031\"))\"}{\"event_data\":\"target_elto_031\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_031}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_031\"=1)&(\"target_elto_031\">(\"target_min_031\"+.0873)))\"}{\"event_data\":\"target_elto_031\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_031}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_031\"=1)&(\"target_elto_031\">(\"target_min_031\"+.1745)))\"}{\"event_data\":\"target_elto_031\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_031}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_031\"=1)&(\"target_elto_031\"<\"target_min_031\"))\"}{\"event_data\":\"target_elto_031\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_031}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_031\"=1)&(\"target_elto_031\"<(\"target_min_031\"+.0873)))\"}{\"event_data\":\"target_elto_031\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_031}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_031\"=1)&(\"target_elto_031\"<(\"target_min_031\"+.1745)))\"}{\"event_data\":\"target_elto_031\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_032}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_032\"=5)&(\"target_elto_032\">\"target_min_032\"))\"}{\"event_data\":\"target_elto_032\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_032}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_032\"=5)&(\"target_elto_032\"<\"target_min_032\"))\"}{\"event_data\":\"target_elto_032\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_032}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_032\"=5)&(\"target_close_032\"<0))\"}{\"event_data\":\"target_range_032\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_032}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_032\"=1)&(\"target_elto_032\">\"target_min_032\"))\"}{\"event_data\":\"target_elto_032\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_032}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_032\"=1)&(\"target_elto_032\">(\"target_min_032\"+.0873)))\"}{\"event_data\":\"target_elto_032\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_032}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_032\"=1)&(\"target_elto_032\">(\"target_min_032\"+.1745)))\"}{\"event_data\":\"target_elto_032\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_032}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_032\"=1)&(\"target_elto_032\"<\"target_min_032\"))\"}{\"event_data\":\"target_elto_032\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_032}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_032\"=1)&(\"target_elto_032\"<(\"target_min_032\"+.0873)))\"}{\"event_data\":\"target_elto_032\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_032}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_032\"=1)&(\"target_elto_032\"<(\"target_min_032\"+.1745)))\"}{\"event_data\":\"target_elto_032\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_033}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_033\"=5)&(\"target_elto_033\">\"target_min_033\"))\"}{\"event_data\":\"target_elto_033\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_033}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_033\"=5)&(\"target_elto_033\"<\"target_min_033\"))\"}{\"event_data\":\"target_elto_033\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_033}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_033\"=5)&(\"target_close_033\"<0))\"}{\"event_data\":\"target_range_033\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_033}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_033\"=1)&(\"target_elto_033\">\"target_min_033\"))\"}{\"event_data\":\"target_elto_033\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_033}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_033\"=1)&(\"target_elto_033\">(\"target_min_033\"+.0873)))\"}{\"event_data\":\"target_elto_033\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_033}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_033\"=1)&(\"target_elto_033\">(\"target_min_033\"+.1745)))\"}{\"event_data\":\"target_elto_033\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_033}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_033\"=1)&(\"target_elto_033\"<\"target_min_033\"))\"}{\"event_data\":\"target_elto_033\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_033}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_033\"=1)&(\"target_elto_033\"<(\"target_min_033\"+.0873)))\"}{\"event_data\":\"target_elto_033\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_033}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_033\"=1)&(\"target_elto_033\"<(\"target_min_033\"+.1745)))\"}{\"event_data\":\"target_elto_033\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_034}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_034\"=5)&(\"target_elto_034\">\"target_min_034\"))\"}{\"event_data\":\"target_elto_034\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_034}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_034\"=5)&(\"target_elto_034\"<\"target_min_034\"))\"}{\"event_data\":\"target_elto_034\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_034}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_034\"=5)&(\"target_close_034\"<0))\"}{\"event_data\":\"target_range_034\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_034}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_034\"=1)&(\"target_elto_034\">\"target_min_034\"))\"}{\"event_data\":\"target_elto_034\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_034}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_034\"=1)&(\"target_elto_034\">(\"target_min_034\"+.0873)))\"}{\"event_data\":\"target_elto_034\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_034}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_034\"=1)&(\"target_elto_034\">(\"target_min_034\"+.1745)))\"}{\"event_data\":\"target_elto_034\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_034}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_034\"=1)&(\"target_elto_034\"<\"target_min_034\"))\"}{\"event_data\":\"target_elto_034\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_034}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_034\"=1)&(\"target_elto_034\"<(\"target_min_034\"+.0873)))\"}{\"event_data\":\"target_elto_034\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_034}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_034\"=1)&(\"target_elto_034\"<(\"target_min_034\"+.1745)))\"}{\"event_data\":\"target_elto_034\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_035}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_035\"=5)&(\"target_elto_035\">\"target_min_035\"))\"}{\"event_data\":\"target_elto_035\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_035}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_035\"=5)&(\"target_elto_035\"<\"target_min_035\"))\"}{\"event_data\":\"target_elto_035\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_035}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_035\"=5)&(\"target_close_035\"<0))\"}{\"event_data\":\"target_range_035\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_035}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_035\"=1)&(\"target_elto_035\">\"target_min_035\"))\"}{\"event_data\":\"target_elto_035\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_035}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_035\"=1)&(\"target_elto_035\">(\"target_min_035\"+.0873)))\"}{\"event_data\":\"target_elto_035\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_035}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_035\"=1)&(\"target_elto_035\">(\"target_min_035\"+.1745)))\"}{\"event_data\":\"target_elto_035\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_035}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_035\"=1)&(\"target_elto_035\"<\"target_min_035\"))\"}{\"event_data\":\"target_elto_035\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_035}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_035\"=1)&(\"target_elto_035\"<(\"target_min_035\"+.0873)))\"}{\"event_data\":\"target_elto_035\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_035}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_035\"=1)&(\"target_elto_035\"<(\"target_min_035\"+.1745)))\"}{\"event_data\":\"target_elto_035\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_036}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_036\"=5)&(\"target_elto_036\">\"target_min_036\"))\"}{\"event_data\":\"target_elto_036\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_036}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_036\"=5)&(\"target_elto_036\"<\"target_min_036\"))\"}{\"event_data\":\"target_elto_036\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_036}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_036\"=5)&(\"target_close_036\"<0))\"}{\"event_data\":\"target_range_036\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_036}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_036\"=1)&(\"target_elto_036\">\"target_min_036\"))\"}{\"event_data\":\"target_elto_036\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_036}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_036\"=1)&(\"target_elto_036\">(\"target_min_036\"+.0873)))\"}{\"event_data\":\"target_elto_036\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_036}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_036\"=1)&(\"target_elto_036\">(\"target_min_036\"+.1745)))\"}{\"event_data\":\"target_elto_036\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_036}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_036\"=1)&(\"target_elto_036\"<\"target_min_036\"))\"}{\"event_data\":\"target_elto_036\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_036}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_036\"=1)&(\"target_elto_036\"<(\"target_min_036\"+.0873)))\"}{\"event_data\":\"target_elto_036\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_036}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_036\"=1)&(\"target_elto_036\"<(\"target_min_036\"+.1745)))\"}{\"event_data\":\"target_elto_036\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_037}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_037\"=5)&(\"target_elto_037\">\"target_min_037\"))\"}{\"event_data\":\"target_elto_037\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_037}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_037\"=5)&(\"target_elto_037\"<\"target_min_037\"))\"}{\"event_data\":\"target_elto_037\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_037}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_037\"=5)&(\"target_close_037\"<0))\"}{\"event_data\":\"target_range_037\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_037}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_037\"=1)&(\"target_elto_037\">\"target_min_037\"))\"}{\"event_data\":\"target_elto_037\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_037}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_037\"=1)&(\"target_elto_037\">(\"target_min_037\"+.0873)))\"}{\"event_data\":\"target_elto_037\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_037}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_037\"=1)&(\"target_elto_037\">(\"target_min_037\"+.1745)))\"}{\"event_data\":\"target_elto_037\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_037}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_037\"=1)&(\"target_elto_037\"<\"target_min_037\"))\"}{\"event_data\":\"target_elto_037\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_037}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_037\"=1)&(\"target_elto_037\"<(\"target_min_037\"+.0873)))\"}{\"event_data\":\"target_elto_037\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_037}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_037\"=1)&(\"target_elto_037\"<(\"target_min_037\"+.1745)))\"}{\"event_data\":\"target_elto_037\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_038}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_038\"=5)&(\"target_elto_038\">\"target_min_038\"))\"}{\"event_data\":\"target_elto_038\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_038}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_038\"=5)&(\"target_elto_038\"<\"target_min_038\"))\"}{\"event_data\":\"target_elto_038\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_038}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_038\"=5)&(\"target_close_038\"<0))\"}{\"event_data\":\"target_range_038\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_038}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_038\"=1)&(\"target_elto_038\">\"target_min_038\"))\"}{\"event_data\":\"target_elto_038\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_038}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_038\"=1)&(\"target_elto_038\">(\"target_min_038\"+.0873)))\"}{\"event_data\":\"target_elto_038\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_038}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_038\"=1)&(\"target_elto_038\">(\"target_min_038\"+.1745)))\"}{\"event_data\":\"target_elto_038\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_038}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_038\"=1)&(\"target_elto_038\"<\"target_min_038\"))\"}{\"event_data\":\"target_elto_038\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_038}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_038\"=1)&(\"target_elto_038\"<(\"target_min_038\"+.0873)))\"}{\"event_data\":\"target_elto_038\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_038}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_038\"=1)&(\"target_elto_038\"<(\"target_min_038\"+.1745)))\"}{\"event_data\":\"target_elto_038\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_039}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_039\"=5)&(\"target_elto_039\">\"target_min_039\"))\"}{\"event_data\":\"target_elto_039\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_039}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_039\"=5)&(\"target_elto_039\"<\"target_min_039\"))\"}{\"event_data\":\"target_elto_039\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_039}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_039\"=5)&(\"target_close_039\"<0))\"}{\"event_data\":\"target_range_039\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_039}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_039\"=1)&(\"target_elto_039\">\"target_min_039\"))\"}{\"event_data\":\"target_elto_039\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_039}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_039\"=1)&(\"target_elto_039\">(\"target_min_039\"+.0873)))\"}{\"event_data\":\"target_elto_039\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_039}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_039\"=1)&(\"target_elto_039\">(\"target_min_039\"+.1745)))\"}{\"event_data\":\"target_elto_039\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_039}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_039\"=1)&(\"target_elto_039\"<\"target_min_039\"))\"}{\"event_data\":\"target_elto_039\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_039}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_039\"=1)&(\"target_elto_039\"<(\"target_min_039\"+.0873)))\"}{\"event_data\":\"target_elto_039\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_039}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_039\"=1)&(\"target_elto_039\"<(\"target_min_039\"+.1745)))\"}{\"event_data\":\"target_elto_039\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_040}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_040\"=5)&(\"target_elto_040\">\"target_min_040\"))\"}{\"event_data\":\"target_elto_040\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_040}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_040\"=5)&(\"target_elto_040\"<\"target_min_040\"))\"}{\"event_data\":\"target_elto_040\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_040}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_040\"=5)&(\"target_close_040\"<0))\"}{\"event_data\":\"target_range_040\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_040}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_040\"=1)&(\"target_elto_040\">\"target_min_040\"))\"}{\"event_data\":\"target_elto_040\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_040}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_040\"=1)&(\"target_elto_040\">(\"target_min_040\"+.0873)))\"}{\"event_data\":\"target_elto_040\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_040}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_040\"=1)&(\"target_elto_040\">(\"target_min_040\"+.1745)))\"}{\"event_data\":\"target_elto_040\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_040}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_040\"=1)&(\"target_elto_040\"<\"target_min_040\"))\"}{\"event_data\":\"target_elto_040\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_040}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_040\"=1)&(\"target_elto_040\"<(\"target_min_040\"+.0873)))\"}{\"event_data\":\"target_elto_040\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_040}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_040\"=1)&(\"target_elto_040\"<(\"target_min_040\"+.1745)))\"}{\"event_data\":\"target_elto_040\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_041}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_041\"=5)&(\"target_elto_041\">\"target_min_041\"))\"}{\"event_data\":\"target_elto_041\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_041}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_041\"=5)&(\"target_elto_041\"<\"target_min_041\"))\"}{\"event_data\":\"target_elto_041\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_041}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_041\"=5)&(\"target_close_041\"<0))\"}{\"event_data\":\"target_range_041\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_041}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_041\"=1)&(\"target_elto_041\">\"target_min_041\"))\"}{\"event_data\":\"target_elto_041\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_041}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_041\"=1)&(\"target_elto_041\">(\"target_min_041\"+.0873)))\"}{\"event_data\":\"target_elto_041\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_041}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_041\"=1)&(\"target_elto_041\">(\"target_min_041\"+.1745)))\"}{\"event_data\":\"target_elto_041\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_041}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_041\"=1)&(\"target_elto_041\"<\"target_min_041\"))\"}{\"event_data\":\"target_elto_041\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_041}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_041\"=1)&(\"target_elto_041\"<(\"target_min_041\"+.0873)))\"}{\"event_data\":\"target_elto_041\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_041}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_041\"=1)&(\"target_elto_041\"<(\"target_min_041\"+.1745)))\"}{\"event_data\":\"target_elto_041\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_042}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_042\"=5)&(\"target_elto_042\">\"target_min_042\"))\"}{\"event_data\":\"target_elto_042\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_042}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_042\"=5)&(\"target_elto_042\"<\"target_min_042\"))\"}{\"event_data\":\"target_elto_042\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_042}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_042\"=5)&(\"target_close_042\"<0))\"}{\"event_data\":\"target_range_042\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_042}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_042\"=1)&(\"target_elto_042\">\"target_min_042\"))\"}{\"event_data\":\"target_elto_042\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_042}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_042\"=1)&(\"target_elto_042\">(\"target_min_042\"+.0873)))\"}{\"event_data\":\"target_elto_042\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_042}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_042\"=1)&(\"target_elto_042\">(\"target_min_042\"+.1745)))\"}{\"event_data\":\"target_elto_042\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_042}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_042\"=1)&(\"target_elto_042\"<\"target_min_042\"))\"}{\"event_data\":\"target_elto_042\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_042}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_042\"=1)&(\"target_elto_042\"<(\"target_min_042\"+.0873)))\"}{\"event_data\":\"target_elto_042\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_042}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_042\"=1)&(\"target_elto_042\"<(\"target_min_042\"+.1745)))\"}{\"event_data\":\"target_elto_042\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_043}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_043\"=5)&(\"target_elto_043\">\"target_min_043\"))\"}{\"event_data\":\"target_elto_043\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_043}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_043\"=5)&(\"target_elto_043\"<\"target_min_043\"))\"}{\"event_data\":\"target_elto_043\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_043}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_043\"=5)&(\"target_close_043\"<0))\"}{\"event_data\":\"target_range_043\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_043}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_043\"=1)&(\"target_elto_043\">\"target_min_043\"))\"}{\"event_data\":\"target_elto_043\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_043}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_043\"=1)&(\"target_elto_043\">(\"target_min_043\"+.0873)))\"}{\"event_data\":\"target_elto_043\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_043}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_043\"=1)&(\"target_elto_043\">(\"target_min_043\"+.1745)))\"}{\"event_data\":\"target_elto_043\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_043}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_043\"=1)&(\"target_elto_043\"<\"target_min_043\"))\"}{\"event_data\":\"target_elto_043\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_043}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_043\"=1)&(\"target_elto_043\"<(\"target_min_043\"+.0873)))\"}{\"event_data\":\"target_elto_043\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_043}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_043\"=1)&(\"target_elto_043\"<(\"target_min_043\"+.1745)))\"}{\"event_data\":\"target_elto_043\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_044}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_044\"=5)&(\"target_elto_044\">\"target_min_044\"))\"}{\"event_data\":\"target_elto_044\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_044}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_044\"=5)&(\"target_elto_044\"<\"target_min_044\"))\"}{\"event_data\":\"target_elto_044\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_044}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_044\"=5)&(\"target_close_044\"<0))\"}{\"event_data\":\"target_range_044\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_044}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_044\"=1)&(\"target_elto_044\">\"target_min_044\"))\"}{\"event_data\":\"target_elto_044\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_044}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_044\"=1)&(\"target_elto_044\">(\"target_min_044\"+.0873)))\"}{\"event_data\":\"target_elto_044\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_044}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_044\"=1)&(\"target_elto_044\">(\"target_min_044\"+.1745)))\"}{\"event_data\":\"target_elto_044\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_044}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_044\"=1)&(\"target_elto_044\"<\"target_min_044\"))\"}{\"event_data\":\"target_elto_044\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_044}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_044\"=1)&(\"target_elto_044\"<(\"target_min_044\"+.0873)))\"}{\"event_data\":\"target_elto_044\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_044}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_044\"=1)&(\"target_elto_044\"<(\"target_min_044\"+.1745)))\"}{\"event_data\":\"target_elto_044\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_045}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_045\"=5)&(\"target_elto_045\">\"target_min_045\"))\"}{\"event_data\":\"target_elto_045\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_045}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_045\"=5)&(\"target_elto_045\"<\"target_min_045\"))\"}{\"event_data\":\"target_elto_045\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_045}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_045\"=5)&(\"target_close_045\"<0))\"}{\"event_data\":\"target_range_045\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_045}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_045\"=1)&(\"target_elto_045\">\"target_min_045\"))\"}{\"event_data\":\"target_elto_045\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_045}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_045\"=1)&(\"target_elto_045\">(\"target_min_045\"+.0873)))\"}{\"event_data\":\"target_elto_045\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_045}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_045\"=1)&(\"target_elto_045\">(\"target_min_045\"+.1745)))\"}{\"event_data\":\"target_elto_045\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_045}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_045\"=1)&(\"target_elto_045\"<\"target_min_045\"))\"}{\"event_data\":\"target_elto_045\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_045}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_045\"=1)&(\"target_elto_045\"<(\"target_min_045\"+.0873)))\"}{\"event_data\":\"target_elto_045\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_045}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_045\"=1)&(\"target_elto_045\"<(\"target_min_045\"+.1745)))\"}{\"event_data\":\"target_elto_045\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_046}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_046\"=5)&(\"target_elto_046\">\"target_min_046\"))\"}{\"event_data\":\"target_elto_046\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_046}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_046\"=5)&(\"target_elto_046\"<\"target_min_046\"))\"}{\"event_data\":\"target_elto_046\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_046}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_046\"=5)&(\"target_close_046\"<0))\"}{\"event_data\":\"target_range_046\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_046}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_046\"=1)&(\"target_elto_046\">\"target_min_046\"))\"}{\"event_data\":\"target_elto_046\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_046}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_046\"=1)&(\"target_elto_046\">(\"target_min_046\"+.0873)))\"}{\"event_data\":\"target_elto_046\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_046}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_046\"=1)&(\"target_elto_046\">(\"target_min_046\"+.1745)))\"}{\"event_data\":\"target_elto_046\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_046}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_046\"=1)&(\"target_elto_046\"<\"target_min_046\"))\"}{\"event_data\":\"target_elto_046\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_046}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_046\"=1)&(\"target_elto_046\"<(\"target_min_046\"+.0873)))\"}{\"event_data\":\"target_elto_046\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_046}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_046\"=1)&(\"target_elto_046\"<(\"target_min_046\"+.1745)))\"}{\"event_data\":\"target_elto_046\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_047}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_047\"=5)&(\"target_elto_047\">\"target_min_047\"))\"}{\"event_data\":\"target_elto_047\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_047}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_047\"=5)&(\"target_elto_047\"<\"target_min_047\"))\"}{\"event_data\":\"target_elto_047\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_047}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_047\"=5)&(\"target_close_047\"<0))\"}{\"event_data\":\"target_range_047\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_047}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_047\"=1)&(\"target_elto_047\">\"target_min_047\"))\"}{\"event_data\":\"target_elto_047\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_047}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_047\"=1)&(\"target_elto_047\">(\"target_min_047\"+.0873)))\"}{\"event_data\":\"target_elto_047\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_047}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_047\"=1)&(\"target_elto_047\">(\"target_min_047\"+.1745)))\"}{\"event_data\":\"target_elto_047\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_047}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_047\"=1)&(\"target_elto_047\"<\"target_min_047\"))\"}{\"event_data\":\"target_elto_047\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_047}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_047\"=1)&(\"target_elto_047\"<(\"target_min_047\"+.0873)))\"}{\"event_data\":\"target_elto_047\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_047}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_047\"=1)&(\"target_elto_047\"<(\"target_min_047\"+.1745)))\"}{\"event_data\":\"target_elto_047\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_048}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_048\"=5)&(\"target_elto_048\">\"target_min_048\"))\"}{\"event_data\":\"target_elto_048\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_048}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_048\"=5)&(\"target_elto_048\"<\"target_min_048\"))\"}{\"event_data\":\"target_elto_048\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_048}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_048\"=5)&(\"target_close_048\"<0))\"}{\"event_data\":\"target_range_048\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_048}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_048\"=1)&(\"target_elto_048\">\"target_min_048\"))\"}{\"event_data\":\"target_elto_048\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_048}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_048\"=1)&(\"target_elto_048\">(\"target_min_048\"+.0873)))\"}{\"event_data\":\"target_elto_048\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_048}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_048\"=1)&(\"target_elto_048\">(\"target_min_048\"+.1745)))\"}{\"event_data\":\"target_elto_048\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_048}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_048\"=1)&(\"target_elto_048\"<\"target_min_048\"))\"}{\"event_data\":\"target_elto_048\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_048}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_048\"=1)&(\"target_elto_048\"<(\"target_min_048\"+.0873)))\"}{\"event_data\":\"target_elto_048\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_048}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_048\"=1)&(\"target_elto_048\"<(\"target_min_048\"+.1745)))\"}{\"event_data\":\"target_elto_048\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_049}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_049\"=5)&(\"target_elto_049\">\"target_min_049\"))\"}{\"event_data\":\"target_elto_049\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_049}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_049\"=5)&(\"target_elto_049\"<\"target_min_049\"))\"}{\"event_data\":\"target_elto_049\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_049}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_049\"=5)&(\"target_close_049\"<0))\"}{\"event_data\":\"target_range_049\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_049}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_049\"=1)&(\"target_elto_049\">\"target_min_049\"))\"}{\"event_data\":\"target_elto_049\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_049}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_049\"=1)&(\"target_elto_049\">(\"target_min_049\"+.0873)))\"}{\"event_data\":\"target_elto_049\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_049}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_049\"=1)&(\"target_elto_049\">(\"target_min_049\"+.1745)))\"}{\"event_data\":\"target_elto_049\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_049}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_049\"=1)&(\"target_elto_049\"<\"target_min_049\"))\"}{\"event_data\":\"target_elto_049\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_049}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_049\"=1)&(\"target_elto_049\"<(\"target_min_049\"+.0873)))\"}{\"event_data\":\"target_elto_049\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_049}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_049\"=1)&(\"target_elto_049\"<(\"target_min_049\"+.1745)))\"}{\"event_data\":\"target_elto_049\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_050}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_050\"=5)&(\"target_elto_050\">\"target_min_050\"))\"}{\"event_data\":\"target_elto_050\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_050}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_050\"=5)&(\"target_elto_050\"<\"target_min_050\"))\"}{\"event_data\":\"target_elto_050\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_050}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_050\"=5)&(\"target_close_050\"<0))\"}{\"event_data\":\"target_range_050\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_050}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_050\"=1)&(\"target_elto_050\">\"target_min_050\"))\"}{\"event_data\":\"target_elto_050\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_050}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_050\"=1)&(\"target_elto_050\">(\"target_min_050\"+.0873)))\"}{\"event_data\":\"target_elto_050\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_050}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_050\"=1)&(\"target_elto_050\">(\"target_min_050\"+.1745)))\"}{\"event_data\":\"target_elto_050\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_050}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_050\"=1)&(\"target_elto_050\"<\"target_min_050\"))\"}{\"event_data\":\"target_elto_050\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_050}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_050\"=1)&(\"target_elto_050\"<(\"target_min_050\"+.0873)))\"}{\"event_data\":\"target_elto_050\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_050}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_050\"=1)&(\"target_elto_050\"<(\"target_min_050\"+.1745)))\"}{\"event_data\":\"target_elto_050\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_051}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_051\"=5)&(\"target_elto_051\">\"target_min_051\"))\"}{\"event_data\":\"target_elto_051\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_051}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_051\"=5)&(\"target_elto_051\"<\"target_min_051\"))\"}{\"event_data\":\"target_elto_051\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_051}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_051\"=5)&(\"target_close_051\"<0))\"}{\"event_data\":\"target_range_051\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_051}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_051\"=1)&(\"target_elto_051\">\"target_min_051\"))\"}{\"event_data\":\"target_elto_051\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_051}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_051\"=1)&(\"target_elto_051\">(\"target_min_051\"+.0873)))\"}{\"event_data\":\"target_elto_051\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_051}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_051\"=1)&(\"target_elto_051\">(\"target_min_051\"+.1745)))\"}{\"event_data\":\"target_elto_051\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_051}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_051\"=1)&(\"target_elto_051\"<\"target_min_051\"))\"}{\"event_data\":\"target_elto_051\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_051}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_051\"=1)&(\"target_elto_051\"<(\"target_min_051\"+.0873)))\"}{\"event_data\":\"target_elto_051\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_051}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_051\"=1)&(\"target_elto_051\"<(\"target_min_051\"+.1745)))\"}{\"event_data\":\"target_elto_051\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_052}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_052\"=5)&(\"target_elto_052\">\"target_min_052\"))\"}{\"event_data\":\"target_elto_052\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_052}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_052\"=5)&(\"target_elto_052\"<\"target_min_052\"))\"}{\"event_data\":\"target_elto_052\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_052}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_052\"=5)&(\"target_close_052\"<0))\"}{\"event_data\":\"target_range_052\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_052}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_052\"=1)&(\"target_elto_052\">\"target_min_052\"))\"}{\"event_data\":\"target_elto_052\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_052}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_052\"=1)&(\"target_elto_052\">(\"target_min_052\"+.0873)))\"}{\"event_data\":\"target_elto_052\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_052}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_052\"=1)&(\"target_elto_052\">(\"target_min_052\"+.1745)))\"}{\"event_data\":\"target_elto_052\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_052}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_052\"=1)&(\"target_elto_052\"<\"target_min_052\"))\"}{\"event_data\":\"target_elto_052\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_052}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_052\"=1)&(\"target_elto_052\"<(\"target_min_052\"+.0873)))\"}{\"event_data\":\"target_elto_052\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_052}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_052\"=1)&(\"target_elto_052\"<(\"target_min_052\"+.1745)))\"}{\"event_data\":\"target_elto_052\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_053}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_053\"=5)&(\"target_elto_053\">\"target_min_053\"))\"}{\"event_data\":\"target_elto_053\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_053}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_053\"=5)&(\"target_elto_053\"<\"target_min_053\"))\"}{\"event_data\":\"target_elto_053\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_053}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_053\"=5)&(\"target_close_053\"<0))\"}{\"event_data\":\"target_range_053\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_053}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_053\"=1)&(\"target_elto_053\">\"target_min_053\"))\"}{\"event_data\":\"target_elto_053\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_053}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_053\"=1)&(\"target_elto_053\">(\"target_min_053\"+.0873)))\"}{\"event_data\":\"target_elto_053\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_053}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_053\"=1)&(\"target_elto_053\">(\"target_min_053\"+.1745)))\"}{\"event_data\":\"target_elto_053\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_053}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_053\"=1)&(\"target_elto_053\"<\"target_min_053\"))\"}{\"event_data\":\"target_elto_053\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_053}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_053\"=1)&(\"target_elto_053\"<(\"target_min_053\"+.0873)))\"}{\"event_data\":\"target_elto_053\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_053}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_053\"=1)&(\"target_elto_053\"<(\"target_min_053\"+.1745)))\"}{\"event_data\":\"target_elto_053\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_054}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_054\"=5)&(\"target_elto_054\">\"target_min_054\"))\"}{\"event_data\":\"target_elto_054\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_054}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_054\"=5)&(\"target_elto_054\"<\"target_min_054\"))\"}{\"event_data\":\"target_elto_054\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_054}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_054\"=5)&(\"target_close_054\"<0))\"}{\"event_data\":\"target_range_054\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_054}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_054\"=1)&(\"target_elto_054\">\"target_min_054\"))\"}{\"event_data\":\"target_elto_054\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_054}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_054\"=1)&(\"target_elto_054\">(\"target_min_054\"+.0873)))\"}{\"event_data\":\"target_elto_054\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_054}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_054\"=1)&(\"target_elto_054\">(\"target_min_054\"+.1745)))\"}{\"event_data\":\"target_elto_054\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_054}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_054\"=1)&(\"target_elto_054\"<\"target_min_054\"))\"}{\"event_data\":\"target_elto_054\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_054}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_054\"=1)&(\"target_elto_054\"<(\"target_min_054\"+.0873)))\"}{\"event_data\":\"target_elto_054\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_054}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_054\"=1)&(\"target_elto_054\"<(\"target_min_054\"+.1745)))\"}{\"event_data\":\"target_elto_054\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_055}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_055\"=5)&(\"target_elto_055\">\"target_min_055\"))\"}{\"event_data\":\"target_elto_055\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_055}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_055\"=5)&(\"target_elto_055\"<\"target_min_055\"))\"}{\"event_data\":\"target_elto_055\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_055}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_055\"=5)&(\"target_close_055\"<0))\"}{\"event_data\":\"target_range_055\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_055}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_055\"=1)&(\"target_elto_055\">\"target_min_055\"))\"}{\"event_data\":\"target_elto_055\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_055}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_055\"=1)&(\"target_elto_055\">(\"target_min_055\"+.0873)))\"}{\"event_data\":\"target_elto_055\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_055}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_055\"=1)&(\"target_elto_055\">(\"target_min_055\"+.1745)))\"}{\"event_data\":\"target_elto_055\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_055}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_055\"=1)&(\"target_elto_055\"<\"target_min_055\"))\"}{\"event_data\":\"target_elto_055\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_055}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_055\"=1)&(\"target_elto_055\"<(\"target_min_055\"+.0873)))\"}{\"event_data\":\"target_elto_055\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_055}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_055\"=1)&(\"target_elto_055\"<(\"target_min_055\"+.1745)))\"}{\"event_data\":\"target_elto_055\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_056}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_056\"=5)&(\"target_elto_056\">\"target_min_056\"))\"}{\"event_data\":\"target_elto_056\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_056}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_056\"=5)&(\"target_elto_056\"<\"target_min_056\"))\"}{\"event_data\":\"target_elto_056\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_056}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_056\"=5)&(\"target_close_056\"<0))\"}{\"event_data\":\"target_range_056\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_056}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_056\"=1)&(\"target_elto_056\">\"target_min_056\"))\"}{\"event_data\":\"target_elto_056\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_056}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_056\"=1)&(\"target_elto_056\">(\"target_min_056\"+.0873)))\"}{\"event_data\":\"target_elto_056\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_056}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_056\"=1)&(\"target_elto_056\">(\"target_min_056\"+.1745)))\"}{\"event_data\":\"target_elto_056\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_056}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_056\"=1)&(\"target_elto_056\"<\"target_min_056\"))\"}{\"event_data\":\"target_elto_056\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_056}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_056\"=1)&(\"target_elto_056\"<(\"target_min_056\"+.0873)))\"}{\"event_data\":\"target_elto_056\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_056}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_056\"=1)&(\"target_elto_056\"<(\"target_min_056\"+.1745)))\"}{\"event_data\":\"target_elto_056\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_057}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_057\"=5)&(\"target_elto_057\">\"target_min_057\"))\"}{\"event_data\":\"target_elto_057\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_057}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_057\"=5)&(\"target_elto_057\"<\"target_min_057\"))\"}{\"event_data\":\"target_elto_057\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_057}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_057\"=5)&(\"target_close_057\"<0))\"}{\"event_data\":\"target_range_057\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_057}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_057\"=1)&(\"target_elto_057\">\"target_min_057\"))\"}{\"event_data\":\"target_elto_057\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_057}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_057\"=1)&(\"target_elto_057\">(\"target_min_057\"+.0873)))\"}{\"event_data\":\"target_elto_057\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_057}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_057\"=1)&(\"target_elto_057\">(\"target_min_057\"+.1745)))\"}{\"event_data\":\"target_elto_057\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_057}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_057\"=1)&(\"target_elto_057\"<\"target_min_057\"))\"}{\"event_data\":\"target_elto_057\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_057}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_057\"=1)&(\"target_elto_057\"<(\"target_min_057\"+.0873)))\"}{\"event_data\":\"target_elto_057\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_057}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_057\"=1)&(\"target_elto_057\"<(\"target_min_057\"+.1745)))\"}{\"event_data\":\"target_elto_057\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_058}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_058\"=5)&(\"target_elto_058\">\"target_min_058\"))\"}{\"event_data\":\"target_elto_058\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_058}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_058\"=5)&(\"target_elto_058\"<\"target_min_058\"))\"}{\"event_data\":\"target_elto_058\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_058}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_058\"=5)&(\"target_close_058\"<0))\"}{\"event_data\":\"target_range_058\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_058}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_058\"=1)&(\"target_elto_058\">\"target_min_058\"))\"}{\"event_data\":\"target_elto_058\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_058}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_058\"=1)&(\"target_elto_058\">(\"target_min_058\"+.0873)))\"}{\"event_data\":\"target_elto_058\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_058}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_058\"=1)&(\"target_elto_058\">(\"target_min_058\"+.1745)))\"}{\"event_data\":\"target_elto_058\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_058}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_058\"=1)&(\"target_elto_058\"<\"target_min_058\"))\"}{\"event_data\":\"target_elto_058\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_058}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_058\"=1)&(\"target_elto_058\"<(\"target_min_058\"+.0873)))\"}{\"event_data\":\"target_elto_058\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_058}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_058\"=1)&(\"target_elto_058\"<(\"target_min_058\"+.1745)))\"}{\"event_data\":\"target_elto_058\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_059}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_059\"=5)&(\"target_elto_059\">\"target_min_059\"))\"}{\"event_data\":\"target_elto_059\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_059}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_059\"=5)&(\"target_elto_059\"<\"target_min_059\"))\"}{\"event_data\":\"target_elto_059\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_059}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_059\"=5)&(\"target_close_059\"<0))\"}{\"event_data\":\"target_range_059\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_059}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_059\"=1)&(\"target_elto_059\">\"target_min_059\"))\"}{\"event_data\":\"target_elto_059\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_059}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_059\"=1)&(\"target_elto_059\">(\"target_min_059\"+.0873)))\"}{\"event_data\":\"target_elto_059\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_059}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_059\"=1)&(\"target_elto_059\">(\"target_min_059\"+.1745)))\"}{\"event_data\":\"target_elto_059\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_059}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_059\"=1)&(\"target_elto_059\"<\"target_min_059\"))\"}{\"event_data\":\"target_elto_059\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_059}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_059\"=1)&(\"target_elto_059\"<(\"target_min_059\"+.0873)))\"}{\"event_data\":\"target_elto_059\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_059}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_059\"=1)&(\"target_elto_059\"<(\"target_min_059\"+.1745)))\"}{\"event_data\":\"target_elto_059\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_060}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_060\"=5)&(\"target_elto_060\">\"target_min_060\"))\"}{\"event_data\":\"target_elto_060\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_060}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_060\"=5)&(\"target_elto_060\"<\"target_min_060\"))\"}{\"event_data\":\"target_elto_060\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_060}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_060\"=5)&(\"target_close_060\"<0))\"}{\"event_data\":\"target_range_060\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_060}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_060\"=1)&(\"target_elto_060\">\"target_min_060\"))\"}{\"event_data\":\"target_elto_060\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_060}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_060\"=1)&(\"target_elto_060\">(\"target_min_060\"+.0873)))\"}{\"event_data\":\"target_elto_060\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_060}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_060\"=1)&(\"target_elto_060\">(\"target_min_060\"+.1745)))\"}{\"event_data\":\"target_elto_060\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_060}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_060\"=1)&(\"target_elto_060\"<\"target_min_060\"))\"}{\"event_data\":\"target_elto_060\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_060}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_060\"=1)&(\"target_elto_060\"<(\"target_min_060\"+.0873)))\"}{\"event_data\":\"target_elto_060\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_060}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_060\"=1)&(\"target_elto_060\"<(\"target_min_060\"+.1745)))\"}{\"event_data\":\"target_elto_060\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_061}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_061\"=5)&(\"target_elto_061\">\"target_min_061\"))\"}{\"event_data\":\"target_elto_061\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_061}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_061\"=5)&(\"target_elto_061\"<\"target_min_061\"))\"}{\"event_data\":\"target_elto_061\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_061}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_061\"=5)&(\"target_close_061\"<0))\"}{\"event_data\":\"target_range_061\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_061}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_061\"=1)&(\"target_elto_061\">\"target_min_061\"))\"}{\"event_data\":\"target_elto_061\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_061}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_061\"=1)&(\"target_elto_061\">(\"target_min_061\"+.0873)))\"}{\"event_data\":\"target_elto_061\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_061}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_061\"=1)&(\"target_elto_061\">(\"target_min_061\"+.1745)))\"}{\"event_data\":\"target_elto_061\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_061}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_061\"=1)&(\"target_elto_061\"<\"target_min_061\"))\"}{\"event_data\":\"target_elto_061\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_061}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_061\"=1)&(\"target_elto_061\"<(\"target_min_061\"+.0873)))\"}{\"event_data\":\"target_elto_061\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_061}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_061\"=1)&(\"target_elto_061\"<(\"target_min_061\"+.1745)))\"}{\"event_data\":\"target_elto_061\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_062}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_062\"=5)&(\"target_elto_062\">\"target_min_062\"))\"}{\"event_data\":\"target_elto_062\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_062}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_062\"=5)&(\"target_elto_062\"<\"target_min_062\"))\"}{\"event_data\":\"target_elto_062\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_062}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_062\"=5)&(\"target_close_062\"<0))\"}{\"event_data\":\"target_range_062\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_062}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_062\"=1)&(\"target_elto_062\">\"target_min_062\"))\"}{\"event_data\":\"target_elto_062\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_062}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_062\"=1)&(\"target_elto_062\">(\"target_min_062\"+.0873)))\"}{\"event_data\":\"target_elto_062\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_062}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_062\"=1)&(\"target_elto_062\">(\"target_min_062\"+.1745)))\"}{\"event_data\":\"target_elto_062\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_062}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_062\"=1)&(\"target_elto_062\"<\"target_min_062\"))\"}{\"event_data\":\"target_elto_062\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_062}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_062\"=1)&(\"target_elto_062\"<(\"target_min_062\"+.0873)))\"}{\"event_data\":\"target_elto_062\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_062}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_062\"=1)&(\"target_elto_062\"<(\"target_min_062\"+.1745)))\"}{\"event_data\":\"target_elto_062\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_063}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_063\"=5)&(\"target_elto_063\">\"target_min_063\"))\"}{\"event_data\":\"target_elto_063\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_063}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_063\"=5)&(\"target_elto_063\"<\"target_min_063\"))\"}{\"event_data\":\"target_elto_063\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_063}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_063\"=5)&(\"target_close_063\"<0))\"}{\"event_data\":\"target_range_063\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_063}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_063\"=1)&(\"target_elto_063\">\"target_min_063\"))\"}{\"event_data\":\"target_elto_063\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_063}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_063\"=1)&(\"target_elto_063\">(\"target_min_063\"+.0873)))\"}{\"event_data\":\"target_elto_063\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_063}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_063\"=1)&(\"target_elto_063\">(\"target_min_063\"+.1745)))\"}{\"event_data\":\"target_elto_063\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_063}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_063\"=1)&(\"target_elto_063\"<\"target_min_063\"))\"}{\"event_data\":\"target_elto_063\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_063}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_063\"=1)&(\"target_elto_063\"<(\"target_min_063\"+.0873)))\"}{\"event_data\":\"target_elto_063\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_063}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_063\"=1)&(\"target_elto_063\"<(\"target_min_063\"+.1745)))\"}{\"event_data\":\"target_elto_063\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_064}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_064\"=5)&(\"target_elto_064\">\"target_min_064\"))\"}{\"event_data\":\"target_elto_064\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_064}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_064\"=5)&(\"target_elto_064\"<\"target_min_064\"))\"}{\"event_data\":\"target_elto_064\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_064}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_064\"=5)&(\"target_close_064\"<0))\"}{\"event_data\":\"target_range_064\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_064}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_064\"=1)&(\"target_elto_064\">\"target_min_064\"))\"}{\"event_data\":\"target_elto_064\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_064}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_064\"=1)&(\"target_elto_064\">(\"target_min_064\"+.0873)))\"}{\"event_data\":\"target_elto_064\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_064}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_064\"=1)&(\"target_elto_064\">(\"target_min_064\"+.1745)))\"}{\"event_data\":\"target_elto_064\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_064}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_064\"=1)&(\"target_elto_064\"<\"target_min_064\"))\"}{\"event_data\":\"target_elto_064\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_064}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_064\"=1)&(\"target_elto_064\"<(\"target_min_064\"+.0873)))\"}{\"event_data\":\"target_elto_064\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_064}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_064\"=1)&(\"target_elto_064\"<(\"target_min_064\"+.1745)))\"}{\"event_data\":\"target_elto_064\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_065}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_065\"=5)&(\"target_elto_065\">\"target_min_065\"))\"}{\"event_data\":\"target_elto_065\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_065}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_065\"=5)&(\"target_elto_065\"<\"target_min_065\"))\"}{\"event_data\":\"target_elto_065\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_065}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_065\"=5)&(\"target_close_065\"<0))\"}{\"event_data\":\"target_range_065\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_065}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_065\"=1)&(\"target_elto_065\">\"target_min_065\"))\"}{\"event_data\":\"target_elto_065\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_065}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_065\"=1)&(\"target_elto_065\">(\"target_min_065\"+.0873)))\"}{\"event_data\":\"target_elto_065\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_065}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_065\"=1)&(\"target_elto_065\">(\"target_min_065\"+.1745)))\"}{\"event_data\":\"target_elto_065\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_065}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_065\"=1)&(\"target_elto_065\"<\"target_min_065\"))\"}{\"event_data\":\"target_elto_065\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_065}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_065\"=1)&(\"target_elto_065\"<(\"target_min_065\"+.0873)))\"}{\"event_data\":\"target_elto_065\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_065}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_065\"=1)&(\"target_elto_065\"<(\"target_min_065\"+.1745)))\"}{\"event_data\":\"target_elto_065\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_066}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_066\"=5)&(\"target_elto_066\">\"target_min_066\"))\"}{\"event_data\":\"target_elto_066\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_066}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_066\"=5)&(\"target_elto_066\"<\"target_min_066\"))\"}{\"event_data\":\"target_elto_066\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_066}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_066\"=5)&(\"target_close_066\"<0))\"}{\"event_data\":\"target_range_066\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_066}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_066\"=1)&(\"target_elto_066\">\"target_min_066\"))\"}{\"event_data\":\"target_elto_066\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_066}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_066\"=1)&(\"target_elto_066\">(\"target_min_066\"+.0873)))\"}{\"event_data\":\"target_elto_066\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_066}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_066\"=1)&(\"target_elto_066\">(\"target_min_066\"+.1745)))\"}{\"event_data\":\"target_elto_066\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_066}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_066\"=1)&(\"target_elto_066\"<\"target_min_066\"))\"}{\"event_data\":\"target_elto_066\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_066}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_066\"=1)&(\"target_elto_066\"<(\"target_min_066\"+.0873)))\"}{\"event_data\":\"target_elto_066\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_066}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_066\"=1)&(\"target_elto_066\"<(\"target_min_066\"+.1745)))\"}{\"event_data\":\"target_elto_066\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_067}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_067\"=5)&(\"target_elto_067\">\"target_min_067\"))\"}{\"event_data\":\"target_elto_067\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_067}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_067\"=5)&(\"target_elto_067\"<\"target_min_067\"))\"}{\"event_data\":\"target_elto_067\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_067}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_067\"=5)&(\"target_close_067\"<0))\"}{\"event_data\":\"target_range_067\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_067}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_067\"=1)&(\"target_elto_067\">\"target_min_067\"))\"}{\"event_data\":\"target_elto_067\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_067}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_067\"=1)&(\"target_elto_067\">(\"target_min_067\"+.0873)))\"}{\"event_data\":\"target_elto_067\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_067}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_067\"=1)&(\"target_elto_067\">(\"target_min_067\"+.1745)))\"}{\"event_data\":\"target_elto_067\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_067}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_067\"=1)&(\"target_elto_067\"<\"target_min_067\"))\"}{\"event_data\":\"target_elto_067\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_067}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_067\"=1)&(\"target_elto_067\"<(\"target_min_067\"+.0873)))\"}{\"event_data\":\"target_elto_067\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_067}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_067\"=1)&(\"target_elto_067\"<(\"target_min_067\"+.1745)))\"}{\"event_data\":\"target_elto_067\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_068}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_068\"=5)&(\"target_elto_068\">\"target_min_068\"))\"}{\"event_data\":\"target_elto_068\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_068}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_068\"=5)&(\"target_elto_068\"<\"target_min_068\"))\"}{\"event_data\":\"target_elto_068\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_068}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_068\"=5)&(\"target_close_068\"<0))\"}{\"event_data\":\"target_range_068\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_068}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_068\"=1)&(\"target_elto_068\">\"target_min_068\"))\"}{\"event_data\":\"target_elto_068\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_068}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_068\"=1)&(\"target_elto_068\">(\"target_min_068\"+.0873)))\"}{\"event_data\":\"target_elto_068\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_068}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_068\"=1)&(\"target_elto_068\">(\"target_min_068\"+.1745)))\"}{\"event_data\":\"target_elto_068\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_068}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_068\"=1)&(\"target_elto_068\"<\"target_min_068\"))\"}{\"event_data\":\"target_elto_068\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_068}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_068\"=1)&(\"target_elto_068\"<(\"target_min_068\"+.0873)))\"}{\"event_data\":\"target_elto_068\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_068}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_068\"=1)&(\"target_elto_068\"<(\"target_min_068\"+.1745)))\"}{\"event_data\":\"target_elto_068\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_069}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_069\"=5)&(\"target_elto_069\">\"target_min_069\"))\"}{\"event_data\":\"target_elto_069\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_069}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_069\"=5)&(\"target_elto_069\"<\"target_min_069\"))\"}{\"event_data\":\"target_elto_069\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_069}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_069\"=5)&(\"target_close_069\"<0))\"}{\"event_data\":\"target_range_069\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_069}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_069\"=1)&(\"target_elto_069\">\"target_min_069\"))\"}{\"event_data\":\"target_elto_069\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_069}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_069\"=1)&(\"target_elto_069\">(\"target_min_069\"+.0873)))\"}{\"event_data\":\"target_elto_069\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_069}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_069\"=1)&(\"target_elto_069\">(\"target_min_069\"+.1745)))\"}{\"event_data\":\"target_elto_069\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_069}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_069\"=1)&(\"target_elto_069\"<\"target_min_069\"))\"}{\"event_data\":\"target_elto_069\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_069}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_069\"=1)&(\"target_elto_069\"<(\"target_min_069\"+.0873)))\"}{\"event_data\":\"target_elto_069\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_069}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_069\"=1)&(\"target_elto_069\"<(\"target_min_069\"+.1745)))\"}{\"event_data\":\"target_elto_069\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_070}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_070\"=5)&(\"target_elto_070\">\"target_min_070\"))\"}{\"event_data\":\"target_elto_070\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_070}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_070\"=5)&(\"target_elto_070\"<\"target_min_070\"))\"}{\"event_data\":\"target_elto_070\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_070}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_070\"=5)&(\"target_close_070\"<0))\"}{\"event_data\":\"target_range_070\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_070}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_070\"=1)&(\"target_elto_070\">\"target_min_070\"))\"}{\"event_data\":\"target_elto_070\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_070}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_070\"=1)&(\"target_elto_070\">(\"target_min_070\"+.0873)))\"}{\"event_data\":\"target_elto_070\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_070}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_070\"=1)&(\"target_elto_070\">(\"target_min_070\"+.1745)))\"}{\"event_data\":\"target_elto_070\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_070}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_070\"=1)&(\"target_elto_070\"<\"target_min_070\"))\"}{\"event_data\":\"target_elto_070\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_070}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_070\"=1)&(\"target_elto_070\"<(\"target_min_070\"+.0873)))\"}{\"event_data\":\"target_elto_070\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_070}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_070\"=1)&(\"target_elto_070\"<(\"target_min_070\"+.1745)))\"}{\"event_data\":\"target_elto_070\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_071}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_071\"=5)&(\"target_elto_071\">\"target_min_071\"))\"}{\"event_data\":\"target_elto_071\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_071}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_071\"=5)&(\"target_elto_071\"<\"target_min_071\"))\"}{\"event_data\":\"target_elto_071\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_071}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_071\"=5)&(\"target_close_071\"<0))\"}{\"event_data\":\"target_range_071\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_071}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_071\"=1)&(\"target_elto_071\">\"target_min_071\"))\"}{\"event_data\":\"target_elto_071\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_071}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_071\"=1)&(\"target_elto_071\">(\"target_min_071\"+.0873)))\"}{\"event_data\":\"target_elto_071\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_071}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_071\"=1)&(\"target_elto_071\">(\"target_min_071\"+.1745)))\"}{\"event_data\":\"target_elto_071\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_071}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_071\"=1)&(\"target_elto_071\"<\"target_min_071\"))\"}{\"event_data\":\"target_elto_071\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_071}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_071\"=1)&(\"target_elto_071\"<(\"target_min_071\"+.0873)))\"}{\"event_data\":\"target_elto_071\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_071}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_071\"=1)&(\"target_elto_071\"<(\"target_min_071\"+.1745)))\"}{\"event_data\":\"target_elto_071\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_072}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_072\"=5)&(\"target_elto_072\">\"target_min_072\"))\"}{\"event_data\":\"target_elto_072\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_072}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_072\"=5)&(\"target_elto_072\"<\"target_min_072\"))\"}{\"event_data\":\"target_elto_072\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_072}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_072\"=5)&(\"target_close_072\"<0))\"}{\"event_data\":\"target_range_072\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_072}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_072\"=1)&(\"target_elto_072\">\"target_min_072\"))\"}{\"event_data\":\"target_elto_072\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_072}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_072\"=1)&(\"target_elto_072\">(\"target_min_072\"+.0873)))\"}{\"event_data\":\"target_elto_072\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_072}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_072\"=1)&(\"target_elto_072\">(\"target_min_072\"+.1745)))\"}{\"event_data\":\"target_elto_072\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_072}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_072\"=1)&(\"target_elto_072\"<\"target_min_072\"))\"}{\"event_data\":\"target_elto_072\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_072}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_072\"=1)&(\"target_elto_072\"<(\"target_min_072\"+.0873)))\"}{\"event_data\":\"target_elto_072\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_072}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_072\"=1)&(\"target_elto_072\"<(\"target_min_072\"+.1745)))\"}{\"event_data\":\"target_elto_072\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_073}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_073\"=5)&(\"target_elto_073\">\"target_min_073\"))\"}{\"event_data\":\"target_elto_073\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_073}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_073\"=5)&(\"target_elto_073\"<\"target_min_073\"))\"}{\"event_data\":\"target_elto_073\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_073}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_073\"=5)&(\"target_close_073\"<0))\"}{\"event_data\":\"target_range_073\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_073}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_073\"=1)&(\"target_elto_073\">\"target_min_073\"))\"}{\"event_data\":\"target_elto_073\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_073}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_073\"=1)&(\"target_elto_073\">(\"target_min_073\"+.0873)))\"}{\"event_data\":\"target_elto_073\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_073}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_073\"=1)&(\"target_elto_073\">(\"target_min_073\"+.1745)))\"}{\"event_data\":\"target_elto_073\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_073}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_073\"=1)&(\"target_elto_073\"<\"target_min_073\"))\"}{\"event_data\":\"target_elto_073\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_073}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_073\"=1)&(\"target_elto_073\"<(\"target_min_073\"+.0873)))\"}{\"event_data\":\"target_elto_073\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_073}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_073\"=1)&(\"target_elto_073\"<(\"target_min_073\"+.1745)))\"}{\"event_data\":\"target_elto_073\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_074}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_074\"=5)&(\"target_elto_074\">\"target_min_074\"))\"}{\"event_data\":\"target_elto_074\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_074}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_074\"=5)&(\"target_elto_074\"<\"target_min_074\"))\"}{\"event_data\":\"target_elto_074\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_074}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_074\"=5)&(\"target_close_074\"<0))\"}{\"event_data\":\"target_range_074\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_074}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_074\"=1)&(\"target_elto_074\">\"target_min_074\"))\"}{\"event_data\":\"target_elto_074\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_074}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_074\"=1)&(\"target_elto_074\">(\"target_min_074\"+.0873)))\"}{\"event_data\":\"target_elto_074\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_074}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_074\"=1)&(\"target_elto_074\">(\"target_min_074\"+.1745)))\"}{\"event_data\":\"target_elto_074\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_074}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_074\"=1)&(\"target_elto_074\"<\"target_min_074\"))\"}{\"event_data\":\"target_elto_074\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_074}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_074\"=1)&(\"target_elto_074\"<(\"target_min_074\"+.0873)))\"}{\"event_data\":\"target_elto_074\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_074}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_074\"=1)&(\"target_elto_074\"<(\"target_min_074\"+.1745)))\"}{\"event_data\":\"target_elto_074\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_075}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_075\"=5)&(\"target_elto_075\">\"target_min_075\"))\"}{\"event_data\":\"target_elto_075\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_075}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_075\"=5)&(\"target_elto_075\"<\"target_min_075\"))\"}{\"event_data\":\"target_elto_075\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_075}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_075\"=5)&(\"target_close_075\"<0))\"}{\"event_data\":\"target_range_075\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_075}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_075\"=1)&(\"target_elto_075\">\"target_min_075\"))\"}{\"event_data\":\"target_elto_075\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_075}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_075\"=1)&(\"target_elto_075\">(\"target_min_075\"+.0873)))\"}{\"event_data\":\"target_elto_075\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_075}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_075\"=1)&(\"target_elto_075\">(\"target_min_075\"+.1745)))\"}{\"event_data\":\"target_elto_075\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_075}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_075\"=1)&(\"target_elto_075\"<\"target_min_075\"))\"}{\"event_data\":\"target_elto_075\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_075}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_075\"=1)&(\"target_elto_075\"<(\"target_min_075\"+.0873)))\"}{\"event_data\":\"target_elto_075\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_075}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_075\"=1)&(\"target_elto_075\"<(\"target_min_075\"+.1745)))\"}{\"event_data\":\"target_elto_075\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_076}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_076\"=5)&(\"target_elto_076\">\"target_min_076\"))\"}{\"event_data\":\"target_elto_076\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_076}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_076\"=5)&(\"target_elto_076\"<\"target_min_076\"))\"}{\"event_data\":\"target_elto_076\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_076}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_076\"=5)&(\"target_close_076\"<0))\"}{\"event_data\":\"target_range_076\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_076}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_076\"=1)&(\"target_elto_076\">\"target_min_076\"))\"}{\"event_data\":\"target_elto_076\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_076}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_076\"=1)&(\"target_elto_076\">(\"target_min_076\"+.0873)))\"}{\"event_data\":\"target_elto_076\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_076}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_076\"=1)&(\"target_elto_076\">(\"target_min_076\"+.1745)))\"}{\"event_data\":\"target_elto_076\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_076}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_076\"=1)&(\"target_elto_076\"<\"target_min_076\"))\"}{\"event_data\":\"target_elto_076\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_076}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_076\"=1)&(\"target_elto_076\"<(\"target_min_076\"+.0873)))\"}{\"event_data\":\"target_elto_076\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_076}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_076\"=1)&(\"target_elto_076\"<(\"target_min_076\"+.1745)))\"}{\"event_data\":\"target_elto_076\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_077}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_077\"=5)&(\"target_elto_077\">\"target_min_077\"))\"}{\"event_data\":\"target_elto_077\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_077}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_077\"=5)&(\"target_elto_077\"<\"target_min_077\"))\"}{\"event_data\":\"target_elto_077\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_077}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_077\"=5)&(\"target_close_077\"<0))\"}{\"event_data\":\"target_range_077\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_077}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_077\"=1)&(\"target_elto_077\">\"target_min_077\"))\"}{\"event_data\":\"target_elto_077\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_077}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_077\"=1)&(\"target_elto_077\">(\"target_min_077\"+.0873)))\"}{\"event_data\":\"target_elto_077\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_077}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_077\"=1)&(\"target_elto_077\">(\"target_min_077\"+.1745)))\"}{\"event_data\":\"target_elto_077\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_077}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_077\"=1)&(\"target_elto_077\"<\"target_min_077\"))\"}{\"event_data\":\"target_elto_077\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_077}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_077\"=1)&(\"target_elto_077\"<(\"target_min_077\"+.0873)))\"}{\"event_data\":\"target_elto_077\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_077}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_077\"=1)&(\"target_elto_077\"<(\"target_min_077\"+.1745)))\"}{\"event_data\":\"target_elto_077\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_078}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_078\"=5)&(\"target_elto_078\">\"target_min_078\"))\"}{\"event_data\":\"target_elto_078\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_078}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_078\"=5)&(\"target_elto_078\"<\"target_min_078\"))\"}{\"event_data\":\"target_elto_078\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_078}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_078\"=5)&(\"target_close_078\"<0))\"}{\"event_data\":\"target_range_078\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_078}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_078\"=1)&(\"target_elto_078\">\"target_min_078\"))\"}{\"event_data\":\"target_elto_078\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_078}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_078\"=1)&(\"target_elto_078\">(\"target_min_078\"+.0873)))\"}{\"event_data\":\"target_elto_078\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_078}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_078\"=1)&(\"target_elto_078\">(\"target_min_078\"+.1745)))\"}{\"event_data\":\"target_elto_078\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_078}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_078\"=1)&(\"target_elto_078\"<\"target_min_078\"))\"}{\"event_data\":\"target_elto_078\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_078}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_078\"=1)&(\"target_elto_078\"<(\"target_min_078\"+.0873)))\"}{\"event_data\":\"target_elto_078\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_078}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_078\"=1)&(\"target_elto_078\"<(\"target_min_078\"+.1745)))\"}{\"event_data\":\"target_elto_078\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_079}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_079\"=5)&(\"target_elto_079\">\"target_min_079\"))\"}{\"event_data\":\"target_elto_079\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_079}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_079\"=5)&(\"target_elto_079\"<\"target_min_079\"))\"}{\"event_data\":\"target_elto_079\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_079}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_079\"=5)&(\"target_close_079\"<0))\"}{\"event_data\":\"target_range_079\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_079}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_079\"=1)&(\"target_elto_079\">\"target_min_079\"))\"}{\"event_data\":\"target_elto_079\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_079}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_079\"=1)&(\"target_elto_079\">(\"target_min_079\"+.0873)))\"}{\"event_data\":\"target_elto_079\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_079}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_079\"=1)&(\"target_elto_079\">(\"target_min_079\"+.1745)))\"}{\"event_data\":\"target_elto_079\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_079}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_079\"=1)&(\"target_elto_079\"<\"target_min_079\"))\"}{\"event_data\":\"target_elto_079\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_079}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_079\"=1)&(\"target_elto_079\"<(\"target_min_079\"+.0873)))\"}{\"event_data\":\"target_elto_079\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_079}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_079\"=1)&(\"target_elto_079\"<(\"target_min_079\"+.1745)))\"}{\"event_data\":\"target_elto_079\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_080}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_080\"=5)&(\"target_elto_080\">\"target_min_080\"))\"}{\"event_data\":\"target_elto_080\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_080}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_080\"=5)&(\"target_elto_080\"<\"target_min_080\"))\"}{\"event_data\":\"target_elto_080\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_080}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_080\"=5)&(\"target_close_080\"<0))\"}{\"event_data\":\"target_range_080\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_080}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_080\"=1)&(\"target_elto_080\">\"target_min_080\"))\"}{\"event_data\":\"target_elto_080\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_080}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_080\"=1)&(\"target_elto_080\">(\"target_min_080\"+.0873)))\"}{\"event_data\":\"target_elto_080\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_080}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_080\"=1)&(\"target_elto_080\">(\"target_min_080\"+.1745)))\"}{\"event_data\":\"target_elto_080\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_080}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_080\"=1)&(\"target_elto_080\"<\"target_min_080\"))\"}{\"event_data\":\"target_elto_080\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_080}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_080\"=1)&(\"target_elto_080\"<(\"target_min_080\"+.0873)))\"}{\"event_data\":\"target_elto_080\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_080}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_080\"=1)&(\"target_elto_080\"<(\"target_min_080\"+.1745)))\"}{\"event_data\":\"target_elto_080\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_081}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_081\"=5)&(\"target_elto_081\">\"target_min_081\"))\"}{\"event_data\":\"target_elto_081\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_081}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_081\"=5)&(\"target_elto_081\"<\"target_min_081\"))\"}{\"event_data\":\"target_elto_081\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_081}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_081\"=5)&(\"target_close_081\"<0))\"}{\"event_data\":\"target_range_081\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_081}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_081\"=1)&(\"target_elto_081\">\"target_min_081\"))\"}{\"event_data\":\"target_elto_081\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_081}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_081\"=1)&(\"target_elto_081\">(\"target_min_081\"+.0873)))\"}{\"event_data\":\"target_elto_081\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_081}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_081\"=1)&(\"target_elto_081\">(\"target_min_081\"+.1745)))\"}{\"event_data\":\"target_elto_081\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_081}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_081\"=1)&(\"target_elto_081\"<\"target_min_081\"))\"}{\"event_data\":\"target_elto_081\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_081}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_081\"=1)&(\"target_elto_081\"<(\"target_min_081\"+.0873)))\"}{\"event_data\":\"target_elto_081\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_081}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_081\"=1)&(\"target_elto_081\"<(\"target_min_081\"+.1745)))\"}{\"event_data\":\"target_elto_081\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_082}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_082\"=5)&(\"target_elto_082\">\"target_min_082\"))\"}{\"event_data\":\"target_elto_082\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_082}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_082\"=5)&(\"target_elto_082\"<\"target_min_082\"))\"}{\"event_data\":\"target_elto_082\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_082}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_082\"=5)&(\"target_close_082\"<0))\"}{\"event_data\":\"target_range_082\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_082}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_082\"=1)&(\"target_elto_082\">\"target_min_082\"))\"}{\"event_data\":\"target_elto_082\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_082}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_082\"=1)&(\"target_elto_082\">(\"target_min_082\"+.0873)))\"}{\"event_data\":\"target_elto_082\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_082}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_082\"=1)&(\"target_elto_082\">(\"target_min_082\"+.1745)))\"}{\"event_data\":\"target_elto_082\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_082}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_082\"=1)&(\"target_elto_082\"<\"target_min_082\"))\"}{\"event_data\":\"target_elto_082\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_082}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_082\"=1)&(\"target_elto_082\"<(\"target_min_082\"+.0873)))\"}{\"event_data\":\"target_elto_082\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_082}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_082\"=1)&(\"target_elto_082\"<(\"target_min_082\"+.1745)))\"}{\"event_data\":\"target_elto_082\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_083}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_083\"=5)&(\"target_elto_083\">\"target_min_083\"))\"}{\"event_data\":\"target_elto_083\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_083}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_083\"=5)&(\"target_elto_083\"<\"target_min_083\"))\"}{\"event_data\":\"target_elto_083\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_083}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_083\"=5)&(\"target_close_083\"<0))\"}{\"event_data\":\"target_range_083\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_083}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_083\"=1)&(\"target_elto_083\">\"target_min_083\"))\"}{\"event_data\":\"target_elto_083\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_083}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_083\"=1)&(\"target_elto_083\">(\"target_min_083\"+.0873)))\"}{\"event_data\":\"target_elto_083\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_083}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_083\"=1)&(\"target_elto_083\">(\"target_min_083\"+.1745)))\"}{\"event_data\":\"target_elto_083\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_083}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_083\"=1)&(\"target_elto_083\"<\"target_min_083\"))\"}{\"event_data\":\"target_elto_083\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_083}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_083\"=1)&(\"target_elto_083\"<(\"target_min_083\"+.0873)))\"}{\"event_data\":\"target_elto_083\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_083}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_083\"=1)&(\"target_elto_083\"<(\"target_min_083\"+.1745)))\"}{\"event_data\":\"target_elto_083\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_084}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_084\"=5)&(\"target_elto_084\">\"target_min_084\"))\"}{\"event_data\":\"target_elto_084\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_084}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_084\"=5)&(\"target_elto_084\"<\"target_min_084\"))\"}{\"event_data\":\"target_elto_084\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_084}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_084\"=5)&(\"target_close_084\"<0))\"}{\"event_data\":\"target_range_084\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_084}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_084\"=1)&(\"target_elto_084\">\"target_min_084\"))\"}{\"event_data\":\"target_elto_084\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_084}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_084\"=1)&(\"target_elto_084\">(\"target_min_084\"+.0873)))\"}{\"event_data\":\"target_elto_084\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_084}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_084\"=1)&(\"target_elto_084\">(\"target_min_084\"+.1745)))\"}{\"event_data\":\"target_elto_084\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_084}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_084\"=1)&(\"target_elto_084\"<\"target_min_084\"))\"}{\"event_data\":\"target_elto_084\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_084}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_084\"=1)&(\"target_elto_084\"<(\"target_min_084\"+.0873)))\"}{\"event_data\":\"target_elto_084\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_084}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_084\"=1)&(\"target_elto_084\"<(\"target_min_084\"+.1745)))\"}{\"event_data\":\"target_elto_084\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_085}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_085\"=5)&(\"target_elto_085\">\"target_min_085\"))\"}{\"event_data\":\"target_elto_085\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_085}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_085\"=5)&(\"target_elto_085\"<\"target_min_085\"))\"}{\"event_data\":\"target_elto_085\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_085}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_085\"=5)&(\"target_close_085\"<0))\"}{\"event_data\":\"target_range_085\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_085}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_085\"=1)&(\"target_elto_085\">\"target_min_085\"))\"}{\"event_data\":\"target_elto_085\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_085}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_085\"=1)&(\"target_elto_085\">(\"target_min_085\"+.0873)))\"}{\"event_data\":\"target_elto_085\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_085}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_085\"=1)&(\"target_elto_085\">(\"target_min_085\"+.1745)))\"}{\"event_data\":\"target_elto_085\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_085}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_085\"=1)&(\"target_elto_085\"<\"target_min_085\"))\"}{\"event_data\":\"target_elto_085\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_085}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_085\"=1)&(\"target_elto_085\"<(\"target_min_085\"+.0873)))\"}{\"event_data\":\"target_elto_085\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_085}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_085\"=1)&(\"target_elto_085\"<(\"target_min_085\"+.1745)))\"}{\"event_data\":\"target_elto_085\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_086}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_086\"=5)&(\"target_elto_086\">\"target_min_086\"))\"}{\"event_data\":\"target_elto_086\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_086}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_086\"=5)&(\"target_elto_086\"<\"target_min_086\"))\"}{\"event_data\":\"target_elto_086\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_086}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_086\"=5)&(\"target_close_086\"<0))\"}{\"event_data\":\"target_range_086\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_086}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_086\"=1)&(\"target_elto_086\">\"target_min_086\"))\"}{\"event_data\":\"target_elto_086\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_086}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_086\"=1)&(\"target_elto_086\">(\"target_min_086\"+.0873)))\"}{\"event_data\":\"target_elto_086\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_086}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_086\"=1)&(\"target_elto_086\">(\"target_min_086\"+.1745)))\"}{\"event_data\":\"target_elto_086\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_086}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_086\"=1)&(\"target_elto_086\"<\"target_min_086\"))\"}{\"event_data\":\"target_elto_086\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_086}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_086\"=1)&(\"target_elto_086\"<(\"target_min_086\"+.0873)))\"}{\"event_data\":\"target_elto_086\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_086}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_086\"=1)&(\"target_elto_086\"<(\"target_min_086\"+.1745)))\"}{\"event_data\":\"target_elto_086\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_087}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_087\"=5)&(\"target_elto_087\">\"target_min_087\"))\"}{\"event_data\":\"target_elto_087\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_087}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_087\"=5)&(\"target_elto_087\"<\"target_min_087\"))\"}{\"event_data\":\"target_elto_087\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_087}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_087\"=5)&(\"target_close_087\"<0))\"}{\"event_data\":\"target_range_087\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_087}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_087\"=1)&(\"target_elto_087\">\"target_min_087\"))\"}{\"event_data\":\"target_elto_087\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_087}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_087\"=1)&(\"target_elto_087\">(\"target_min_087\"+.0873)))\"}{\"event_data\":\"target_elto_087\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_087}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_087\"=1)&(\"target_elto_087\">(\"target_min_087\"+.1745)))\"}{\"event_data\":\"target_elto_087\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_087}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_087\"=1)&(\"target_elto_087\"<\"target_min_087\"))\"}{\"event_data\":\"target_elto_087\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_087}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_087\"=1)&(\"target_elto_087\"<(\"target_min_087\"+.0873)))\"}{\"event_data\":\"target_elto_087\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_087}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_087\"=1)&(\"target_elto_087\"<(\"target_min_087\"+.1745)))\"}{\"event_data\":\"target_elto_087\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_088}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_088\"=5)&(\"target_elto_088\">\"target_min_088\"))\"}{\"event_data\":\"target_elto_088\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_088}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_088\"=5)&(\"target_elto_088\"<\"target_min_088\"))\"}{\"event_data\":\"target_elto_088\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_088}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_088\"=5)&(\"target_close_088\"<0))\"}{\"event_data\":\"target_range_088\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_088}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_088\"=1)&(\"target_elto_088\">\"target_min_088\"))\"}{\"event_data\":\"target_elto_088\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_088}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_088\"=1)&(\"target_elto_088\">(\"target_min_088\"+.0873)))\"}{\"event_data\":\"target_elto_088\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_088}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_088\"=1)&(\"target_elto_088\">(\"target_min_088\"+.1745)))\"}{\"event_data\":\"target_elto_088\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_088}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_088\"=1)&(\"target_elto_088\"<\"target_min_088\"))\"}{\"event_data\":\"target_elto_088\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_088}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_088\"=1)&(\"target_elto_088\"<(\"target_min_088\"+.0873)))\"}{\"event_data\":\"target_elto_088\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_088}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_088\"=1)&(\"target_elto_088\"<(\"target_min_088\"+.1745)))\"}{\"event_data\":\"target_elto_088\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_089}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_089\"=5)&(\"target_elto_089\">\"target_min_089\"))\"}{\"event_data\":\"target_elto_089\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_089}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_089\"=5)&(\"target_elto_089\"<\"target_min_089\"))\"}{\"event_data\":\"target_elto_089\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_089}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_089\"=5)&(\"target_close_089\"<0))\"}{\"event_data\":\"target_range_089\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_089}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_089\"=1)&(\"target_elto_089\">\"target_min_089\"))\"}{\"event_data\":\"target_elto_089\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_089}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_089\"=1)&(\"target_elto_089\">(\"target_min_089\"+.0873)))\"}{\"event_data\":\"target_elto_089\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_089}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_089\"=1)&(\"target_elto_089\">(\"target_min_089\"+.1745)))\"}{\"event_data\":\"target_elto_089\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_089}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_089\"=1)&(\"target_elto_089\"<\"target_min_089\"))\"}{\"event_data\":\"target_elto_089\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_089}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_089\"=1)&(\"target_elto_089\"<(\"target_min_089\"+.0873)))\"}{\"event_data\":\"target_elto_089\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_089}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_089\"=1)&(\"target_elto_089\"<(\"target_min_089\"+.1745)))\"}{\"event_data\":\"target_elto_089\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_090}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_090\"=5)&(\"target_elto_090\">\"target_min_090\"))\"}{\"event_data\":\"target_elto_090\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_090}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_090\"=5)&(\"target_elto_090\"<\"target_min_090\"))\"}{\"event_data\":\"target_elto_090\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_090}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_090\"=5)&(\"target_close_090\"<0))\"}{\"event_data\":\"target_range_090\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_090}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_090\"=1)&(\"target_elto_090\">\"target_min_090\"))\"}{\"event_data\":\"target_elto_090\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_090}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_090\"=1)&(\"target_elto_090\">(\"target_min_090\"+.0873)))\"}{\"event_data\":\"target_elto_090\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_090}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_090\"=1)&(\"target_elto_090\">(\"target_min_090\"+.1745)))\"}{\"event_data\":\"target_elto_090\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_090}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_090\"=1)&(\"target_elto_090\"<\"target_min_090\"))\"}{\"event_data\":\"target_elto_090\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_090}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_090\"=1)&(\"target_elto_090\"<(\"target_min_090\"+.0873)))\"}{\"event_data\":\"target_elto_090\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_090}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_090\"=1)&(\"target_elto_090\"<(\"target_min_090\"+.1745)))\"}{\"event_data\":\"target_elto_090\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_091}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_091\"=5)&(\"target_elto_091\">\"target_min_091\"))\"}{\"event_data\":\"target_elto_091\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_091}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_091\"=5)&(\"target_elto_091\"<\"target_min_091\"))\"}{\"event_data\":\"target_elto_091\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_091}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_091\"=5)&(\"target_close_091\"<0))\"}{\"event_data\":\"target_range_091\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_091}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_091\"=1)&(\"target_elto_091\">\"target_min_091\"))\"}{\"event_data\":\"target_elto_091\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_091}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_091\"=1)&(\"target_elto_091\">(\"target_min_091\"+.0873)))\"}{\"event_data\":\"target_elto_091\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_091}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_091\"=1)&(\"target_elto_091\">(\"target_min_091\"+.1745)))\"}{\"event_data\":\"target_elto_091\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_091}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_091\"=1)&(\"target_elto_091\"<\"target_min_091\"))\"}{\"event_data\":\"target_elto_091\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_091}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_091\"=1)&(\"target_elto_091\"<(\"target_min_091\"+.0873)))\"}{\"event_data\":\"target_elto_091\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_091}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_091\"=1)&(\"target_elto_091\"<(\"target_min_091\"+.1745)))\"}{\"event_data\":\"target_elto_091\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_092}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_092\"=5)&(\"target_elto_092\">\"target_min_092\"))\"}{\"event_data\":\"target_elto_092\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_092}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_092\"=5)&(\"target_elto_092\"<\"target_min_092\"))\"}{\"event_data\":\"target_elto_092\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_092}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_092\"=5)&(\"target_close_092\"<0))\"}{\"event_data\":\"target_range_092\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_092}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_092\"=1)&(\"target_elto_092\">\"target_min_092\"))\"}{\"event_data\":\"target_elto_092\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_092}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_092\"=1)&(\"target_elto_092\">(\"target_min_092\"+.0873)))\"}{\"event_data\":\"target_elto_092\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_092}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_092\"=1)&(\"target_elto_092\">(\"target_min_092\"+.1745)))\"}{\"event_data\":\"target_elto_092\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_092}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_092\"=1)&(\"target_elto_092\"<\"target_min_092\"))\"}{\"event_data\":\"target_elto_092\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_092}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_092\"=1)&(\"target_elto_092\"<(\"target_min_092\"+.0873)))\"}{\"event_data\":\"target_elto_092\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_092}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_092\"=1)&(\"target_elto_092\"<(\"target_min_092\"+.1745)))\"}{\"event_data\":\"target_elto_092\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_093}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_093\"=5)&(\"target_elto_093\">\"target_min_093\"))\"}{\"event_data\":\"target_elto_093\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_093}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_093\"=5)&(\"target_elto_093\"<\"target_min_093\"))\"}{\"event_data\":\"target_elto_093\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_093}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_093\"=5)&(\"target_close_093\"<0))\"}{\"event_data\":\"target_range_093\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_093}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_093\"=1)&(\"target_elto_093\">\"target_min_093\"))\"}{\"event_data\":\"target_elto_093\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_093}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_093\"=1)&(\"target_elto_093\">(\"target_min_093\"+.0873)))\"}{\"event_data\":\"target_elto_093\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_093}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_093\"=1)&(\"target_elto_093\">(\"target_min_093\"+.1745)))\"}{\"event_data\":\"target_elto_093\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_093}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_093\"=1)&(\"target_elto_093\"<\"target_min_093\"))\"}{\"event_data\":\"target_elto_093\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_093}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_093\"=1)&(\"target_elto_093\"<(\"target_min_093\"+.0873)))\"}{\"event_data\":\"target_elto_093\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_093}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_093\"=1)&(\"target_elto_093\"<(\"target_min_093\"+.1745)))\"}{\"event_data\":\"target_elto_093\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_094}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_094\"=5)&(\"target_elto_094\">\"target_min_094\"))\"}{\"event_data\":\"target_elto_094\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_094}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_094\"=5)&(\"target_elto_094\"<\"target_min_094\"))\"}{\"event_data\":\"target_elto_094\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_094}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_094\"=5)&(\"target_close_094\"<0))\"}{\"event_data\":\"target_range_094\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_094}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_094\"=1)&(\"target_elto_094\">\"target_min_094\"))\"}{\"event_data\":\"target_elto_094\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_094}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_094\"=1)&(\"target_elto_094\">(\"target_min_094\"+.0873)))\"}{\"event_data\":\"target_elto_094\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_094}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_094\"=1)&(\"target_elto_094\">(\"target_min_094\"+.1745)))\"}{\"event_data\":\"target_elto_094\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_094}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_094\"=1)&(\"target_elto_094\"<\"target_min_094\"))\"}{\"event_data\":\"target_elto_094\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_094}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_094\"=1)&(\"target_elto_094\"<(\"target_min_094\"+.0873)))\"}{\"event_data\":\"target_elto_094\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_094}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_094\"=1)&(\"target_elto_094\"<(\"target_min_094\"+.1745)))\"}{\"event_data\":\"target_elto_094\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_095}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_095\"=5)&(\"target_elto_095\">\"target_min_095\"))\"}{\"event_data\":\"target_elto_095\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_095}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_095\"=5)&(\"target_elto_095\"<\"target_min_095\"))\"}{\"event_data\":\"target_elto_095\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_095}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_095\"=5)&(\"target_close_095\"<0))\"}{\"event_data\":\"target_range_095\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_095}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_095\"=1)&(\"target_elto_095\">\"target_min_095\"))\"}{\"event_data\":\"target_elto_095\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_095}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_095\"=1)&(\"target_elto_095\">(\"target_min_095\"+.0873)))\"}{\"event_data\":\"target_elto_095\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_095}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_095\"=1)&(\"target_elto_095\">(\"target_min_095\"+.1745)))\"}{\"event_data\":\"target_elto_095\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_095}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_095\"=1)&(\"target_elto_095\"<\"target_min_095\"))\"}{\"event_data\":\"target_elto_095\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_095}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_095\"=1)&(\"target_elto_095\"<(\"target_min_095\"+.0873)))\"}{\"event_data\":\"target_elto_095\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_095}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_095\"=1)&(\"target_elto_095\"<(\"target_min_095\"+.1745)))\"}{\"event_data\":\"target_elto_095\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_096}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_096\"=5)&(\"target_elto_096\">\"target_min_096\"))\"}{\"event_data\":\"target_elto_096\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_096}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_096\"=5)&(\"target_elto_096\"<\"target_min_096\"))\"}{\"event_data\":\"target_elto_096\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_096}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_096\"=5)&(\"target_close_096\"<0))\"}{\"event_data\":\"target_range_096\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_096}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_096\"=1)&(\"target_elto_096\">\"target_min_096\"))\"}{\"event_data\":\"target_elto_096\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_096}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_096\"=1)&(\"target_elto_096\">(\"target_min_096\"+.0873)))\"}{\"event_data\":\"target_elto_096\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_096}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_096\"=1)&(\"target_elto_096\">(\"target_min_096\"+.1745)))\"}{\"event_data\":\"target_elto_096\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_096}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_096\"=1)&(\"target_elto_096\"<\"target_min_096\"))\"}{\"event_data\":\"target_elto_096\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_096}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_096\"=1)&(\"target_elto_096\"<(\"target_min_096\"+.0873)))\"}{\"event_data\":\"target_elto_096\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_096}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_096\"=1)&(\"target_elto_096\"<(\"target_min_096\"+.1745)))\"}{\"event_data\":\"target_elto_096\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_097}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_097\"=5)&(\"target_elto_097\">\"target_min_097\"))\"}{\"event_data\":\"target_elto_097\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_097}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_097\"=5)&(\"target_elto_097\"<\"target_min_097\"))\"}{\"event_data\":\"target_elto_097\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_097}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_097\"=5)&(\"target_close_097\"<0))\"}{\"event_data\":\"target_range_097\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_097}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_097\"=1)&(\"target_elto_097\">\"target_min_097\"))\"}{\"event_data\":\"target_elto_097\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_097}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_097\"=1)&(\"target_elto_097\">(\"target_min_097\"+.0873)))\"}{\"event_data\":\"target_elto_097\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_097}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_097\"=1)&(\"target_elto_097\">(\"target_min_097\"+.1745)))\"}{\"event_data\":\"target_elto_097\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_097}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_097\"=1)&(\"target_elto_097\"<\"target_min_097\"))\"}{\"event_data\":\"target_elto_097\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_097}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_097\"=1)&(\"target_elto_097\"<(\"target_min_097\"+.0873)))\"}{\"event_data\":\"target_elto_097\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_097}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_097\"=1)&(\"target_elto_097\"<(\"target_min_097\"+.1745)))\"}{\"event_data\":\"target_elto_097\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_098}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_098\"=5)&(\"target_elto_098\">\"target_min_098\"))\"}{\"event_data\":\"target_elto_098\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_098}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_098\"=5)&(\"target_elto_098\"<\"target_min_098\"))\"}{\"event_data\":\"target_elto_098\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_098}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_098\"=5)&(\"target_close_098\"<0))\"}{\"event_data\":\"target_range_098\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_098}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_098\"=1)&(\"target_elto_098\">\"target_min_098\"))\"}{\"event_data\":\"target_elto_098\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_098}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_098\"=1)&(\"target_elto_098\">(\"target_min_098\"+.0873)))\"}{\"event_data\":\"target_elto_098\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_098}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_098\"=1)&(\"target_elto_098\">(\"target_min_098\"+.1745)))\"}{\"event_data\":\"target_elto_098\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_098}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_098\"=1)&(\"target_elto_098\"<\"target_min_098\"))\"}{\"event_data\":\"target_elto_098\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_098}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_098\"=1)&(\"target_elto_098\"<(\"target_min_098\"+.0873)))\"}{\"event_data\":\"target_elto_098\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_098}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_098\"=1)&(\"target_elto_098\"<(\"target_min_098\"+.1745)))\"}{\"event_data\":\"target_elto_098\"}" <<
    //		"{\"event_name\":\"TIV_${target_name_099}\"}{\"event_type\":6144}{\"event_flag\":251410}{\"event_condition\":\"((\"target_type_099\"=5)&(\"target_elto_099\">\"target_min_099\"))\"}{\"event_data\":\"target_elto_099\"}" <<
    //		"{\"event_name\":\"TOV_${target_name_099}\"}{\"event_type\":6144}{\"event_flag\":259602}{\"event_condition\":\"((\"target_type_099\"=5)&(\"target_elto_099\"<\"target_min_099\"))\"}{\"event_data\":\"target_elto_099\"}" <<
    //		"{\"event_name\":\"TMR_${target_name_099}\"}{\"event_type\":6144}{\"event_flag\":233522}{\"event_condition\":\"((\"target_type_099\"=5)&(\"target_close_099\"<0))\"}{\"event_data\":\"target_range_099\"}" <<
    //		"{\"event_name\":\"AOS_${target_name_099}\"}{\"event_type\":5120}{\"event_flag\":251411}{\"event_condition\":\"((\"target_type_099\"=1)&(\"target_elto_099\">\"target_min_099\"))\"}{\"event_data\":\"target_elto_099\"}" <<
    //		"{\"event_name\":\"AOS+5_${target_name_099}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_099\"=1)&(\"target_elto_099\">(\"target_min_099\"+.0873)))\"}{\"event_data\":\"target_elto_099\"}" <<
    //		"{\"event_name\":\"AOS+10_${target_name_099}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_099\"=1)&(\"target_elto_099\">(\"target_min_099\"+.1745)))\"}{\"event_data\":\"target_elto_099\"}" <<
    //		"{\"event_name\":\"LOS_${target_name_099}\"}{\"event_type\":5120}{\"event_flag\":259603}{\"event_condition\":\"((\"target_type_099\"=1)&(\"target_elto_099\"<\"target_min_099\"))\"}{\"event_data\":\"target_elto_099\"}" <<
    //		"{\"event_name\":\"LOS+5_${target_name_099}\"}{\"event_type\":5121}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_099\"=1)&(\"target_elto_099\"<(\"target_min_099\"+.0873)))\"}{\"event_data\":\"target_elto_099\"}" <<
    //		"{\"event_name\":\"LOS+10_${target_name_099}\"}{\"event_type\":5122}{\"event_flag\":235027}{\"event_condition\":\"((\"target_type_099\"=1)&(\"target_elto_099\"<(\"target_min_099\"+.1745)))\"}{\"event_data\":\"target_elto_099\"}" << std::endl;
    //		ofile.close();

    //		ofile.open (node_directory + "node.ini");
    //		ofile << "{\"node_type\":0}" <<
    //		"{\"piece_cnt\":1}" <<
    //		"{\"comp_cnt\":1}" <<
    //		"{\"port_cnt\":0}" << std::endl;
    //		ofile.close();

    //		ofile.open (node_directory + "node_utcstart.ini");
    //		ofile << "57119.1133001281" << std::endl;
    //		ofile.close();

    //		ofile.open (node_directory + "pieces.ini");
    //		ofile << "{\"piece_name_000\":\"CPU\"}" <<
    //		"{\"piece_type_000\":0}" <<
    //		"{\"piece_cidx_000\":0}" <<
    //		"{\"piece_mass_000\":0}" <<
    //		"{\"piece_emi_000\":0.80000001}" <<
    //		"{\"piece_abs_000\":0.88}" <<
    //		"{\"piece_hcap_000\":800}" <<
    //		"{\"piece_hcon_000\":237}" <<
    //		"{\"piece_dim_000\":0}" <<
    //		"{\"piece_pnt_cnt_000\":1}" <<
    //		"{\"piece_pnt_000_000\":[0,0,0]}" << std::endl;
    //		ofile.close();

    //		ofile.open (node_directory + "ports.ini");
    //		ofile << "" << std::endl;
    //		ofile.close();
    //	}

    //	if (get_nodedir(nodename).empty())
    //		return 1;
    //	else
    //		return 0;
}
#endif //COSMOS_MAC_OS
