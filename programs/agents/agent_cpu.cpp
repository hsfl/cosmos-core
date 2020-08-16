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
int32_t get_last_offset();

//int32_t request_soh(string &request, string &response, Agent *);
int32_t request_bootCount(string &request, string &response, Agent *);

// disk
int32_t request_diskSize(string &request, string &response, Agent *);
int32_t request_diskUsed(string &request, string &response, Agent *);
int32_t request_diskFree(string &request, string &response, Agent *);
int32_t request_diskFreePercent (string &request, string &response, Agent *);

// cpu
int32_t request_cpuProcess(string &request, string &response, Agent *);
int32_t request_load(string &request, string &response, Agent *);

// memory
int32_t request_mem(string &request, string &response, Agent *);
int32_t request_mem_kib(string &request, string &response, Agent *);
int32_t request_mem_percent (string &request, string &response, Agent *);
int32_t request_mem_total(string &request, string &response, Agent *);
int32_t request_mem_total_kib(string &request, string &response, Agent *);

int32_t request_printStatus(string &request, string &response, Agent *);


static string sohstring;

// cosmos classes
static ElapsedTime et;
static DeviceDisk deviceDisk;
static DeviceCpu deviceCpu;
//static DeviceCpu cpu;
static uint16_t cpu_cidx;
static uint16_t cpu_didx;

static Agent *agent;

int main(int argc, char *argv[])
{
    int32_t iretn;

    if (argc == 2)
    {
        agent = new Agent(argv[1], "cpu", 15.);
    }
    else
    {
        agent = new Agent("", "cpu", 15.);
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

    iretn = json_createpiece(agent->cinfo, agent->nodeName + "_cpu", DeviceType::CPU);
    if (iretn < 0)
    {
        fprintf(agent->get_debug_fd(), "Failed to add CPU %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(1);
    }
    cpu_cidx = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
    cpu_didx = agent->cinfo->device[cpu_cidx].all.didx;

    sohstring = "{\"node_downtime\"";
    sohstring += ",\"device_cpu_utc_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_temp_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_maxgib_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_gib_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_maxload_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_load_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_uptime_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_boot_count_00" + std::to_string(cpu_didx) + "\"";

    static const double GiB = 1024. * 1024. * 1024.;

    agent->cinfo->device[cpu_cidx].cpu.load = static_cast <float>(deviceCpu.getLoad());
    agent->cinfo->device[cpu_cidx].cpu.gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/GiB);
    agent->cinfo->device[cpu_cidx].cpu.maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/GiB);
    agent->cinfo->device[cpu_cidx].cpu.maxload = deviceCpu.getCpuCount();
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
        uint16_t didx = agent->cinfo->device[cidx].all.didx;
        strncpy(agent->cinfo->device[cidx].disk.path, dinfo[i].mount.c_str(), COSMOS_MAX_NAME);
        sohstring += ",\"device_disk_utc_00" + std::to_string(cpu_didx) + "\"";
        sohstring += ",\"device_disk_temp_00" + std::to_string(cpu_didx) + "\"";
        sohstring += ",\"device_disk_path_00" + std::to_string(cpu_didx) + "\"";
        sohstring += ",\"device_disk_gib_00" + std::to_string(cpu_didx) + "\"";
        sohstring += ",\"device_disk_maxgib_00" + std::to_string(cpu_didx) + "\"";
        sohstring += ",\"device_disk_utilization_00" + std::to_string(cpu_didx) + "\"";
        sohstring += ",\"device_disk_temp_00" + std::to_string(cpu_didx) + "\"";
    }
    sohstring += "}";
    agent->set_sohstring(sohstring);
    printf("SOH String: %s\n", sohstring.c_str());

//    json_dump_node(agent->cinfo);

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


    et.start();

    agent->cinfo->agent[0].aprd = agent->cinfo->agent[0].beat.bprd;
    agent->start_active_loop();

    // Initialize temperature Fit
    LsFit cputemp(4, 1);
    // Start performing the body of the agent
    agent->debug_level = 0;
    while(agent->running())
    {
        agent->cinfo->device[cpu_cidx].all.utc = currentmjd();
        if (agent->debug_level)
        {
            fprintf(agent->get_debug_fd(), "%16.10f ", agent->cinfo->device[cpu_cidx].all.utc);
        }
        agent->cinfo->node.downtime = get_last_offset();

        // get cpu info
        if (agent->cinfo->devspec.cpu_cnt)
        {
            agent->cinfo->device[cpu_cidx].cpu.uptime = deviceCpu.getUptime();
            agent->cinfo->device[cpu_cidx].cpu.boot_count = deviceCpu.getBootCount();
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

        // get temperature
        FILE *cmd_pipe = popen("/cosmos/scripts/get_cpu_temperature", "r");

        if (cmd_pipe != nullptr)
        {
            float ctemp;
            fscanf(cmd_pipe, "%f", &ctemp);
            pclose( cmd_pipe );

            cputemp.update(currentmjd(), ctemp);
            agent->cinfo->device[cpu_cidx].cpu.temp = cputemp.eval(currentmjd());
        }

        if (agent->debug_level)
        {
            fprintf(agent->get_debug_fd(), "\n");
        }

        agent->finish_active_loop();

    }

    agent->shutdown();

    // Start our own thread
//    agent_cpu();

    return 0;

}

int32_t get_last_offset()
{
    int32_t offset = 0;
    FILE *fp = fopen(("/cosmos/nodes/" + agent->nodeName + "/last_offset").c_str(), "r");
    if (fp != nullptr)
    {
        fscanf(fp, "%d", &offset);
        fclose(fp);
    }
    return offset;
}

//int agent_cpu()
//{

//    return 0;
//}


//int32_t request_soh(string &, string &response, Agent *)
//{
//    string rjstring;
//    //	response = (json_of_list(rjstring,sohstring,agent->cinfo));
//    response = (json_of_table(rjstring, agent->sohtable, agent->cinfo));

//    return 0;
//}



// ----------------------------------------------
// disk
int32_t request_diskSize(string &, string &response, Agent *)
{
    return ((response = std::to_string(deviceDisk.SizeGiB)).length());
}

int32_t request_diskUsed(string &, string &response, Agent *)
{
    return ((response = std::to_string(deviceDisk.UsedGiB)).length());
}

int32_t request_diskFree(string &, string &response, Agent *)
{
    // TODO: implement diskFree
    //return (response =  "%.1f", agent->cinfo->device[cpu_cidx].cpugib));

    // in the mean time use this
    return ((response = std::to_string(deviceDisk.FreeGiB)).length());

}

int32_t request_diskFreePercent (std::string &, std::string &response, Agent *)
{
    return ((response = std::to_string(deviceDisk.FreePercent)).length());
}



// ----------------------------------------------
// cpu
int32_t request_load (string &, string &response, Agent *)
{
    return ((response = std::to_string(deviceCpu.load)).length());
}

int32_t request_cpuProcess(string &, string &response, Agent *){

    return ((response = std::to_string(deviceCpu.percentUseForCurrentProcess)).length());
}

// ----------------------------------------------
// memory in Bytes
int32_t request_mem(string &, string &response, Agent *)
{
    return ((response = std::to_string(deviceCpu.virtualMemoryUsed)).length());
}

// used memory in KiB
int32_t request_mem_kib(string &, string &response, Agent *)
{
    return ((response = std::to_string(deviceCpu.virtualMemoryUsed/1024.)).length());
}


int32_t request_mem_percent (string &, string &response, Agent *)
{

    return ((response = std::to_string(deviceCpu.getVirtualMemoryUsedPercent())).length());
}

// total memory in Bytes
int32_t request_mem_total(string &, string &response, Agent *)
{
    return ((response = std::to_string(deviceCpu.getVirtualMemoryTotal())).length());
}

// total memory in Bytes
int32_t request_mem_total_kib(string &, string &response, Agent *)
{
    return ((response = std::to_string(deviceCpu.getVirtualMemoryTotal()/1024)).length());
}



// ----------------------------------------------
// boot count
int32_t request_bootCount(string &, string &response, Agent *)
{

    std::ifstream ifs ("/hiakasat/nodes/hiakasat/boot.count");
    string counts;

    if (ifs.is_open()) {
        std::getline(ifs,counts);
        ifs.close();
    }
    else {
        cout << "Error opening file";
    }

    return ((response = counts).length());
}

// ----------------------------------------------
// debug info
int32_t request_printStatus(string &request, string &, Agent *)
{

    sscanf(request.c_str(),"%*s %d",&printStatus);
    cout << "printStatus is " << printStatus <<  endl;

    return 0;
}
