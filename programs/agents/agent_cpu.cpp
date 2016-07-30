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
#include "agent/agent.h"
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

int32_t request_soh(char *request, char* response, cosmosAgent *agent);
int32_t request_bootCount(char *request, char* response, cosmosAgent *);

// disk
int32_t request_diskSize(char *request, char *response, cosmosAgent *agent);
int32_t request_diskUsed(char *request, char *response, cosmosAgent *agent);
int32_t request_diskFree(char *request, char *response, cosmosAgent *agent);
int32_t request_diskFreePercent (char*request, char *response, cosmosAgent *agent);

// cpu
int32_t request_cpuProcess(char*request, char *response, cosmosAgent *agent);
int32_t request_load(char *request, char *response, cosmosAgent *agent);
int32_t request_mem(char *request, char *response, cosmosAgent *agent);
int32_t request_mempercent (char*request, char *response, cosmosAgent *agent);
int32_t request_printStatus(char *request, char *response, cosmosAgent *agent);


std::string agentname  = "cpu";
std::string nodename;
std::string sohstring = "{\"device_cpu_utc_000\","
                        "\"device_cpu_maxgib_000\","
                        "\"device_cpu_gib_000\","
                        "\"device_cpu_maxload_000\","
                        "\"device_cpu_load_000\"";

// cosmos classes
ElapsedTime et;
DeviceDisk disk;
DeviceCpu deviceCpu;
DeviceCpu cpu;

cosmosAgent *agent;

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

//    agent->version = "1.0";

    // Add additional requests

//    agent = new cosmosAgent(NetworkType::UDP, std::ref(nodename), std::ref(agentname), 5.);
    agent = new cosmosAgent(NetworkType::UDP, nodename, agentname, 5.);

    agent->add_request("soh",request_soh);
    agent->add_request("diskSize",request_diskSize,"","get disk size in GB");
    agent->add_request("diskUsed",request_diskUsed,"","get disk used in GB");
    agent->add_request("diskFree",request_diskFree,"","get disk free in GB");
    agent->add_request("diskFreePercent",request_diskFreePercent,"","get disk free in %");
    agent->add_request("mem",request_mem);
    agent->add_request("load",request_load);
    agent->add_request("mem_percent",request_mempercent);
    agent->add_request("cpuProc",request_cpuProcess,"","get the % CPU for this process");
    agent->add_request("printStatus",request_printStatus,"","print the status data");
    agent->add_request("bootCount",request_bootCount,"","count the number of reboots");

    char tempstring[200];

    for (uint16_t i=0; i<agent->cdata[0].devspec.disk_cnt; ++i)
    {
        sprintf(tempstring, ",\"device_disk_utc_%03d\",\"device_disk_temp_%03d\"", i, i);
        sohstring += tempstring;
        sprintf(tempstring, ",\"device_disk_gib_%03d\",\"device_disk_maxgib_%03d\"",i,i);
        sohstring += tempstring;
    }

    sohstring += "}";
    agent->set_sohstring(sohstring);

    // Start our own thread
    myagent();

    return 0;

}




int myagent()
{

    ElapsedTime et;
    static const double GiB = 1024. * 1024. * 1024.;

    et.start();

    // Start performing the body of the agent
    while(agent->running())
    {

        COSMOS_SLEEP(agent->cdata[0].agent[0].aprd);

        agent->cdata[0].devspec.cpu[0]->gen.utc = currentmjd();

        if (agent->cdata[0].devspec.cpu_cnt)
        {
            // cpu
            agent->cdata[0].devspec.cpu[0]->load   = deviceCpu.getLoad();
            agent->cdata[0].devspec.cpu[0]->gib    = deviceCpu.getVirtualMemoryUsed()/GiB;
            agent->cdata[0].devspec.cpu[0]->maxgib = deviceCpu.getVirtualMemoryTotal()/GiB;
            deviceCpu.getPercentUseForCurrentProcess();
        }

        for (size_t i=0; i<agent->cdata[0].devspec.disk_cnt; ++i)
        {
            agent->cdata[0].devspec.disk[i]->gen.utc = currentmjd();
            agent->cdata[0].devspec.disk[i]->gib = disk.getUsedGiB(agent->cdata[0].port[agent->cdata[0].devspec.disk[i]->gen.portidx].name);
            agent->cdata[0].devspec.disk[i]->maxgib = disk.getSizeGiB(agent->cdata[0].port[agent->cdata[0].devspec.disk[i]->gen.portidx].name);
        }

        if (printStatus) {
            cout << "Load," << deviceCpu.load << ", ";
            cout << "DiskSize[GiB]," << disk.SizeGiB << ", ";
            cout << "DiskUsed[GiB]," << disk.UsedGiB << ", ";
            cout << "DiskFree[GiB]," << disk.FreeGiB << ", ";
            cout << "CPU Proc[%]," << deviceCpu.percentUseForCurrentProcess << endl;

        }

    }

    agent->shutdown();

    return 0;
}


int32_t request_soh(char *, char* response, cosmosAgent *)
{
    std::string rjstring;
    //	strcpy(response,json_of_list(rjstring,sohstring,agent->cdata));
    strcpy(response,json_of_table(rjstring, agent->cdata[0].agent[0].sohtable, agent->cdata));

    return 0;
}



// ----------------------------------------------
// disk
int32_t request_diskSize(char *, char* response, cosmosAgent *)
{
    return (sprintf(response, "%f", disk.SizeGiB));
}

int32_t request_diskUsed(char *, char* response, cosmosAgent *)
{
    return (sprintf(response, "%f", disk.UsedGiB));
}

int32_t request_diskFree(char *, char* response, cosmosAgent *)
{
    // TODO: implement diskFree
    //return (sprintf(response, "%.1f", agent->cdata[0].devspec.cpu[0]->gib));

    // in the mean time use this
    return (sprintf(response, "%f", disk.FreeGiB));

}

int32_t request_diskFreePercent (char *, char *response, cosmosAgent *)
{
    return (sprintf(response, "%f", disk.FreePercent));
}



// ----------------------------------------------
// cpu
int32_t request_load (char *, char* response, cosmosAgent *)
{
    return (sprintf(response, "%.2f", deviceCpu.load));
}

int32_t request_cpuProcess(char *, char *response, cosmosAgent *){

    return (sprintf(response, "%f", deviceCpu.percentUseForCurrentProcess));
}

// ----------------------------------------------
// memory
int32_t request_mem(char *, char* response, cosmosAgent *)
{
    return (sprintf(response, "%f", deviceCpu.virtualMemoryUsed));
}

int32_t request_mempercent (char *, char *response, cosmosAgent *)
{

    return (sprintf(response, "%f", deviceCpu.getVirtualMemoryUsedPercent()));
}


// ----------------------------------------------
// boot count
int32_t request_bootCount(char *, char* response, cosmosAgent *)
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
int32_t request_printStatus(char *request, char *, cosmosAgent *)
{

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
        agent->cdata = json_create();
        strcpy(agent->cdata->node.name, nodename.c_str());
        agent->cdata->node.type = NODE_TYPE_COMPUTER;

        agent->cdata->node.piece_cnt = 11;
        agent->cdata->piece.resize(agent->cdata->node.piece_cnt);
        agent->cdata->node.device_cnt = agent->cdata->node.piece_cnt;
        agent->cdata->device.resize(agent->cdata->node.device_cnt);
        agent->cdata->devspec.cpu_cnt = 1;
        agent->cdata->devspec.disk_cnt = 10;
        agent->cdata->node.port_cnt = 10;
        agent->cdata->port.resize(agent->cdata->node.port_cnt);
        //        json_addbaseentry(agent->cdata);

        for (size_t i=0; i<agent->cdata->node.piece_cnt; ++i)
        {
            agent->cdata->piece[i].cidx = i;
            switch (i)
            {
            case 0:
                strcpy(agent->cdata->piece[i].name, "Main CPU");
                break;
            default:
                sprintf(agent->cdata->piece[i].name, "Drive %d", i);
//                strcpy(agent->cdata->piece[i].name, "Main Drive");
                break;
            }

            agent->cdata->piece[i].type = PIECE_TYPE_DIMENSIONLESS;
            agent->cdata->piece[i].emi = .8;
            agent->cdata->piece[i].abs = .88;
            agent->cdata->piece[i].hcap = 800;
            agent->cdata->piece[i].hcon = 237;
            agent->cdata->piece[i].pnt_cnt = 1;
            for (uint16_t j=0; j<3; ++j)
            {
                agent->cdata->piece[i].points[0].col[j] = 0.;
            }
            json_addpieceentry(i,agent->cdata);

            agent->cdata->device[i].all.gen.pidx = i;
            agent->cdata->device[i].all.gen.cidx = i;
            switch (i)
            {
            case 0:
                agent->cdata->device[i].all.gen.type = DEVICE_TYPE_CPU;
                agent->cdata->device[i].all.gen.didx = 0;
                agent->cdata->device[i].all.gen.portidx = PORT_TYPE_NONE;
                break;
            default:
                agent->cdata->device[i].all.gen.type = DEVICE_TYPE_DISK;
                agent->cdata->device[i].all.gen.didx = i-1;
                agent->cdata->device[i].all.gen.portidx = agent->cdata->device[i].all.gen.didx;
                agent->cdata->port[agent->cdata->device[i].all.gen.didx].type = PORT_TYPE_DRIVE;
#ifdef COSMOS_WIN_OS
                strcpy(agent->cdata->port[agent->cdata->device[i].all.gen.didx].name, "c:/");
#else
                strcpy(agent->cdata->port[agent->cdata->device[i].all.gen.didx].name, "/");
#endif
                json_addentry("port_name",agent->cdata->device[i].all.gen.didx,65535u,(ptrdiff_t)offsetof(portstruc,name)+(agent->cdata->device[i].all.gen.didx)*sizeof(portstruc),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_GROUP_PORT,agent->cdata);
                json_addentry("port_type",agent->cdata->device[i].all.gen.didx,65535u,(ptrdiff_t)offsetof(portstruc,type)+(agent->cdata->device[i].all.gen.didx)*sizeof(portstruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_GROUP_PORT,agent->cdata);
                break;
            }
            json_addcompentry(i, agent->cdata);
            json_adddeviceentry(i, agent->cdata);
        }

        int32_t iretn = json_dump_node(agent->cdata);
        json_destroy(agent->cdata);
        return iretn;
    }
    else
    {
        return 0;
    }
}
