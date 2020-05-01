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

#include "support/configCosmos.h"
#include "agent/agentclass.h"
#include "support/timeutils.h"
#include "support/jsonlib.h"
#include "support/elapsedtime.h"
#include "support/stringlib.h"
#include "support/print_utils.h"

#include "device/cpu/devicecpu.h"
#include "device/disk/devicedisk.h"

#include <stdio.h>
#include <iostream>

#if defined (COSMOS_WIN_OS)
#include "windows.h"
#include <tchar.h>
#endif

// TODO: remove from here?
//#if defined (COSMOS_LINUX_OS)
//#include <stdlib.h>
//#include <sys/statvfs.h>
//#include <sys/types.h>
//#include <sys/sysinfo.h>
//#include <unistd.h>
//#endif

//using std::cout;
//using std::endl;

// flag to turn on/off print
bool printStatus = true;

int agent_cpu(), create_node();

//int32_t request_soh(char *request, char* response, Agent *);
int32_t request_bootCount(char *request, char* response, Agent *);

// disk
int32_t request_diskSize(char *request, char *response, Agent *);
int32_t request_diskUsed(char *request, char *response, Agent *);
int32_t request_diskFree(char *request, char *response, Agent *);
int32_t request_diskFreePercent (char*request, char *response, Agent *);

// cpu
int32_t request_cpuProcess(char*request, char *response, Agent *);
int32_t request_load(char *request, char *response, Agent *);

// memory
int32_t request_mem(char *request, char *response, Agent *);
int32_t request_mem_kib(char *request, char *response, Agent *);
int32_t request_mem_percent (char*request, char *response, Agent *);
int32_t request_mem_total(char *request, char *response, Agent *);
int32_t request_mem_total_kib(char *request, char *response, Agent *);

int32_t request_printStatus(char *request, char *response, Agent *);


static std::string sohstring = "{\"device_cpu_utc_000\","
                               "\"device_cpu_maxgib_000\","
                               "\"device_cpu_gib_000\","
                               "\"memory_utilization_000\","
                               "\"cpu_utilization_000\","
                               "\"device_cpu_maxload_000\","
                               "\"device_cpu_load_000\"";

// cosmos classes
static ElapsedTime et;
static DeviceDisk deviceDisk;
static DeviceCpu deviceCpu;
//static DeviceCpu cpu;
static uint16_t cpu_cidx;

static Agent *agent;

int main(int argc, char *argv[])
{
    int32_t iretn;

    if (argc == 2)
    {
        agent = new Agent(argv[1], "cpu", 5.);
    }
    else
    {
        agent = new Agent("", "cpu", 5.);
    }

    if ((iretn = agent->wait()) < 0)
    {
        fprintf(agent->get_debug_fd(), "%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    }
    else
    {
        fprintf(agent->get_debug_fd(), "%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
    }

    iretn = json_createpiece(agent->cinfo, "main_cpu", DeviceType::CPU);
    if (iretn < 0)
    {
        fprintf(agent->get_debug_fd(), "Failed to add CPU %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(1);
    }
    cpu_cidx = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;

    static const double GiB = 1024. * 1024. * 1024.;

    agent->cinfo->device[cpu_cidx].cpu.load = static_cast <float>(deviceCpu.getLoad());
    agent->cinfo->device[cpu_cidx].cpu.gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/GiB);
    agent->cinfo->device[cpu_cidx].cpu.maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/GiB);
    agent->cinfo->device[cpu_cidx].cpu.maxload = deviceCpu.getCount();
    deviceCpu.numProcessors = agent->cinfo->device[cpu_cidx].cpu.maxload;
    deviceCpu.getPercentUseForCurrentProcess();

    vector <DeviceDisk::info> dinfo = deviceDisk.getInfo();
    for (uint16_t i=0; i<dinfo.size(); ++i)
    {
        char name[10];
        sprintf(name, "disk_%02u", i);
        iretn = json_createpiece(agent->cinfo, name, DeviceType::DISK);
        if (iretn < 0)
        {
            fprintf(agent->get_debug_fd(), "Failed to add DISK %s : %s\n", dinfo[i].mount.c_str(), cosmos_error_string(iretn).c_str());
            agent->shutdown();
            exit(1);
        }
        uint16_t cidx = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
        strncpy(agent->cinfo->device[cidx].disk.path, dinfo[i].mount.c_str(), COSMOS_MAX_NAME);
//        iretn = json_createport(agent->cinfo, dinfo[i].mount, PORT_TYPE_DRIVE);
//        uint16_t portidx = static_cast <uint16_t>(iretn);
//        json_mapportentry(portidx, agent->cinfo);
//        agent->cinfo->device[cidx].all.portidx = portidx;
    }

    json_dump_node(agent->cinfo);

    // TODO: determine number of disks automatically
    PrintUtils print;
    print.scalar("Number of Disks: ",agent->cinfo->devspec.disk_cnt);
    print.endline();
    print.scalar("Number of Cores: ",agent->cinfo->device[cpu_cidx].cpu.maxload);
    print.endline();

    fprintf(agent->get_debug_fd(), "CPU Agent initialized\n");

//    agent->add_request("soh",request_soh,"","current state of health message");
    agent->add_request("diskSize",request_diskSize,"","disk size in GB");
    agent->add_request("diskUsed",request_diskUsed,"","disk used in GB");
    agent->add_request("diskFree",request_diskFree,"","disk free in GB");
    agent->add_request("diskFreePercent",request_diskFreePercent,"","disk free in %");
    agent->add_request("mem",request_mem,"","current virtual memory used in Bytes");
    agent->add_request("mem_kib",request_mem_kib,"","current virtual memory used in KiB");
    agent->add_request("mem_percent",request_mem_percent,"","memory percentage");
    agent->add_request("mem_total",request_mem_total,"","total memory in Bytes");
    agent->add_request("mem_total_kib",request_mem_total_kib,"","total memory in KiB");
    agent->add_request("load",request_load,"","current CPU load (0-1 is good, >1 is overloaded)");
    agent->add_request("cpuProc",request_cpuProcess,"","the %CPU usage for this process");
    agent->add_request("printStatus",request_printStatus,"","print the status data");
    agent->add_request("bootCount",request_bootCount,"","reboot count");

    char tempstring[200];

    for (uint16_t i=0; i<agent->cinfo->devspec.disk_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_disk_utc_%03d\",\"device_disk_path_%03d\"", i, i);
        sohstring += tempstring;
        sprintf(tempstring, ",\"device_disk_gib_%03d\",\"device_disk_maxgib_%03d\",\"disk_utilization_%03d\"", i, i, i);
        sohstring += tempstring;
    }

    sohstring += "}";
    agent->set_sohstring(sohstring);

    et.start();

    agent->cinfo->agent[0].aprd = 1.;
    agent->start_active_loop();

    // Start performing the body of the agent
    while(agent->running())
    {
        agent->cinfo->device[cpu_cidx].all.utc = currentmjd();
        if (agent->debug_level)
        {
            fprintf(agent->get_debug_fd(), "%16.10f ", agent->cinfo->device[cpu_cidx].all.utc);
        }

        // get cpu info
        if (agent->cinfo->devspec.cpu_cnt)
        {
            agent->cinfo->device[cpu_cidx].cpu.load = deviceCpu.getLoad();
            agent->cinfo->device[cpu_cidx].cpu.gib = deviceCpu.getVirtualMemoryUsed()/GiB;
            agent->cinfo->device[cpu_cidx].cpu.maxgib = deviceCpu.getVirtualMemoryTotal()/GiB;
            deviceCpu.getPercentUseForCurrentProcess();
        }
        if (agent->debug_level)
        {
            fprintf(agent->get_debug_fd(), "Load %6.2f %6.2f ", agent->cinfo->device[cpu_cidx].cpu.load, agent->cinfo->device[cpu_cidx].cpu.maxload);
            fprintf(agent->get_debug_fd(), "Memory %6.2f %6.2f ", agent->cinfo->device[cpu_cidx].cpu.gib, agent->cinfo->device[cpu_cidx].cpu.maxgib);
        }

        // get disk info
        for (size_t i=0; i<agent->cinfo->devspec.disk_cnt; ++i)
        {
            agent->cinfo->device[agent->cinfo->devspec.disk[i]].disk.utc = currentmjd();

            string node_path = agent->cinfo->device[agent->cinfo->devspec.disk[i]].disk.path;

            agent->cinfo->device[agent->cinfo->devspec.disk[i]].disk.gib = deviceDisk.getUsedGiB(node_path);
            agent->cinfo->device[agent->cinfo->devspec.disk[i]].disk.maxgib = deviceDisk.getSizeGiB(node_path);
            if (agent->debug_level)
            {
                fprintf(agent->get_debug_fd(), "%s %6.2f %6.2f ", node_path.c_str(), agent->cinfo->device[agent->cinfo->devspec.disk[i]].disk.gib, agent->cinfo->device[agent->cinfo->devspec.disk[i]].disk.maxgib);
            }
        }

        if (agent->debug_level)
        {
            fprintf(agent->get_debug_fd(), "\n");
        }

        agent->finish_active_loop();

        // if printStatus is true then print in a loop
//        if (printStatus) {
//            PrintUtils print;

//            print.delimiter_flag = true;
//            print.scalar("Load",deviceCpu.load ,1,"",4,4);
//            print.scalar("DiskSize[GiB]",deviceDisk.SizeGiB ,1,"",4,4);

//            cout << "DiskSize[GiB]," << deviceDisk.SizeGiB << ", ";
//            cout << "DiskUsed[GiB]," << deviceDisk.UsedGiB << ", ";
//            cout << "DiskFree[GiB]," << deviceDisk.FreeGiB << ", ";
//            cout << "CPU Proc[%]," << deviceCpu.percentUseForCurrentProcess << endl;
//        }
    }

    agent->shutdown();

    // Start our own thread
//    agent_cpu();

    return 0;

}

//int agent_cpu()
//{

//    return 0;
//}


//int32_t request_soh(char *, char* response, Agent *)
//{
//    std::string rjstring;
//    //	strcpy(response,json_of_list(rjstring,sohstring,agent->cinfo));
//    strcpy(response,json_of_table(rjstring, agent->sohtable, agent->cinfo));

//    return 0;
//}



// ----------------------------------------------
// disk
int32_t request_diskSize(char *, char* response, Agent *)
{
    return (sprintf(response, "%f", deviceDisk.SizeGiB));
}

int32_t request_diskUsed(char *, char* response, Agent *)
{
    return (sprintf(response, "%f", deviceDisk.UsedGiB));
}

int32_t request_diskFree(char *, char* response, Agent *)
{
    // TODO: implement diskFree
    //return (sprintf(response, "%.1f", agent->cinfo->device[cpu_cidx].cpugib));

    // in the mean time use this
    return (sprintf(response, "%f", deviceDisk.FreeGiB));

}

int32_t request_diskFreePercent (char *, char *response, Agent *)
{
    return (sprintf(response, "%f", deviceDisk.FreePercent));
}



// ----------------------------------------------
// cpu
int32_t request_load (char *, char* response, Agent *)
{
    return (sprintf(response, "%.2f", deviceCpu.load));
}

int32_t request_cpuProcess(char *, char *response, Agent *){

    return (sprintf(response, "%f", deviceCpu.percentUseForCurrentProcess));
}

// ----------------------------------------------
// memory in Bytes
int32_t request_mem(char *, char* response, Agent *)
{
    return (sprintf(response, "%.0f", deviceCpu.virtualMemoryUsed));
}

// used memory in KiB
int32_t request_mem_kib(char *, char* response, Agent *)
{
    return (sprintf(response, "%.0f", deviceCpu.virtualMemoryUsed/1024.));
}


int32_t request_mem_percent (char *, char *response, Agent *)
{

    return (sprintf(response, "%.0f", deviceCpu.getVirtualMemoryUsedPercent()));
}

// total memory in Bytes
int32_t request_mem_total(char *, char* response, Agent *)
{
    return (sprintf(response, "%.0f", deviceCpu.getVirtualMemoryTotal()));
}

// total memory in Bytes
int32_t request_mem_total_kib(char *, char* response, Agent *)
{
    return (sprintf(response, "%.0f", deviceCpu.getVirtualMemoryTotal()/1024));
}



// ----------------------------------------------
// boot count
int32_t request_bootCount(char *, char* response, Agent *)
{

    std::ifstream ifs ("/hiakasat/nodes/hiakasat/boot.count");
    std::string counts;

    if (ifs.is_open()) {
        std::getline(ifs,counts);
        ifs.close();
    }
    else {
        cout << "Error opening file";
    }

    return (sprintf(response, "%s", counts.c_str()));
}

// ----------------------------------------------
// debug info
int32_t request_printStatus(char *request, char *, Agent *)
{

    sscanf(request,"%*s %d",&printStatus);
    cout << "printStatus is " << printStatus <<  endl;

    return 0;
}
