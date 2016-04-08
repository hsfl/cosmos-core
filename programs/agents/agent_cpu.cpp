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
#include "agent/agentlib.h"
#include "timeutils.hpp"
#include "jsonlib.h"
#include "elapsedtime.h"
#include "stringlib.h"

#include "device/cpu/devicecpu.h"
#include "device/disk/devicedisk.h"

#include <stdio.h>
#include <iostream>

#if defined (COSMOS_WIN_OS)
#include "windows.h"
#include <tchar.h>
#endif

// TODO: remove from here?
#if defined (COSMSO_LINUX_OS)
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
int32_t request_bootCount(char *request, char* response, void *);

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


std::string agentname  = "cpu";
std::string nodename;
char sohstring[] = "{\"device_cpu_utc_000\",\"device_cpu_disk_000\",\"device_cpu_maxdisk_000\",\"device_cpu_maxmem_000\",\"device_cpu_mem_000\",\"device_cpu_load_000\",\"}";

// cosmos classes
ElapsedTime et;
DeviceDisk disk;
DeviceCpu deviceCpu;
DeviceCpu cpu;

Agent agent;

int main(int argc, char *argv[])
{

    std::cout << "Starting agent cpu" << std::endl;

    switch (argc)
    {
    case 1:
    {
        nodename = "cpu_" + deviceCpu.getHostName();
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
    agent.addRequest("bootCount",request_bootCount,"","count the number of reboots");

    agent_set_sohstring(agent.cdata, sohstring);

    // Start our own thread
    myagent();

    return 0;

}




int myagent()
{

    ElapsedTime et;

    et.start();

    // Start performing the body of the agent
    while(agent.isRunning())
    {

        COSMOS_SLEEP(agent.cdata[0].agent[0].aprd);

        agent.cdata[0].devspec.cpu[0]->gen.utc = currentmjd();

        if (agent.cdata[0].devspec.cpu_cnt)
        {
            // cpu
            agent.cdata[0].devspec.cpu[0]->load   = deviceCpu.getLoad();
            agent.cdata[0].devspec.cpu[0]->mem    = deviceCpu.getVirtualMemoryUsed();
            agent.cdata[0].devspec.cpu[0]->maxmem = deviceCpu.getVirtualMemoryTotal();
            deviceCpu.getPercentUseForCurrentProcess();
        }

        // TODO: add disk to node.ini
        // disk
        // reset the values
        //disk.Used = 0;
        //disk.Free = 0;
        //disk.Size = 0;

        if (agent.cdata[0].devspec.disk_cnt)
        {
            for (size_t i=0; i<agent.cdata[0].devspec.disk_cnt; ++i)
            {
                disk.getAll(agent.cdata[0].port[agent.cdata[0].devspec.disk[i]->gen.portidx].name);
            }
        } else {
            // get the default disk info
            disk.getAll();
        }

        if (printStatus) {
            cout << "Load," << deviceCpu.load << ", ";
            cout << "DiskSize[GiB]," << disk.SizeGiB << ", ";
            cout << "DiskUsed[GiB]," << disk.UsedGiB << ", ";
            cout << "DiskFree[GiB]," << disk.FreeGiB << ", ";
            cout << "CPU Proc[%]," << deviceCpu.percentUseForCurrentProcess << endl;

        }

        // for testing purposes only
        // cpu.stress();
        // cpu.stress();

    }

    agent.shutdown();

    return 0;
}


int32_t request_soh(char *request, char* response, void *)
{
    std::string rjstring;
    //	strcpy(response,json_of_list(rjstring,sohstring,agent.cdata));
    strcpy(response,json_of_table(rjstring, agent.cdata[0].agent[0].sohtable, agent.cdata));

    return 0;
}



// ----------------------------------------------
// disk
int32_t request_diskSize(char *request, char* response, void *)
{
    return (sprintf(response, "%f", disk.SizeGiB));
}

int32_t request_diskUsed(char *request, char* response, void *)
{
    return (sprintf(response, "%f", disk.UsedGiB));
}

int32_t request_diskFree(char *request, char* response, void *)
{
    // TODO: implement diskFree
    //return (sprintf(response, "%.1f", agent.cdata[0].devspec.cpu[0]->disk));

    // in the mean time use this
    return (sprintf(response, "%f", disk.FreeGiB));

}

int32_t request_diskFreePercent (char *request, char *response, void *)
{
    return (sprintf(response, "%f", disk.FreePercent));
}



// ----------------------------------------------
// cpu
int32_t request_load (char *request, char* response, void *)
{
    return (sprintf(response, "%.2f", deviceCpu.load));
}

int32_t request_cpuProcess(char *request, char *response, void */*cdata*/){

    return (sprintf(response, "%f", deviceCpu.percentUseForCurrentProcess));
}

// ----------------------------------------------
// memory
int32_t request_mem(char *request, char* response, void *)
{
    return (sprintf(response, "%f", deviceCpu.virtualMemoryUsed));
}

int32_t request_mempercent (char *request, char *response, void *)
{

    return (sprintf(response, "%f", deviceCpu.getVirtualMemoryUsedPercent()));
}


// ----------------------------------------------
// boot count
int32_t request_bootCount(char *request, char* response, void *)
{

    std::ifstream ifs ("/hiakasat/nodes/hiakasat/boot.count");
    std::string counts;

    if (ifs.is_open()) {
        getline (ifs,counts);
        ifs.close();
    }
    else {
        std::cout << "Error opening file";
    }

    return (sprintf(response, "%s", counts.c_str()));
}

// ----------------------------------------------
// debug info
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
