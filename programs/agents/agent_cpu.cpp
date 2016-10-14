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

//using namespace std;
using std::cout;
using std::endl;

// flag to turn on/off print
bool printStatus;

int myagent(), create_node();

int32_t request_soh(char *request, char* response, Agent *);
int32_t request_bootCount(char *request, char* response, Agent *);

// disk
int32_t request_diskSize(char *request, char *response, Agent *);
int32_t request_diskUsed(char *request, char *response, Agent *);
int32_t request_diskFree(char *request, char *response, Agent *);
int32_t request_diskFreePercent (char*request, char *response, Agent *);

// cpu
int32_t request_cpuProcess(char*request, char *response, Agent *);
int32_t request_load(char *request, char *response, Agent *);
int32_t request_mem(char *request, char *response, Agent *);
int32_t request_mempercent (char*request, char *response, Agent *);
int32_t request_printStatus(char *request, char *response, Agent *);


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

Agent *agent;

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

    agent = new Agent(nodename, agentname, 5.);

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

    for (uint16_t i=0; i<agent->cinfo->pdata.devspec.disk_cnt; ++i)
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

        COSMOS_SLEEP(agent->cinfo->pdata.agent[0].aprd);

        agent->cinfo->pdata.devspec.cpu[0]->gen.utc = currentmjd();

        if (agent->cinfo->pdata.devspec.cpu_cnt)
        {
            // cpu
            agent->cinfo->pdata.devspec.cpu[0]->load   = deviceCpu.getLoad();
            agent->cinfo->pdata.devspec.cpu[0]->gib    = deviceCpu.getVirtualMemoryUsed()/GiB;
            agent->cinfo->pdata.devspec.cpu[0]->maxgib = deviceCpu.getVirtualMemoryTotal()/GiB;
            deviceCpu.getPercentUseForCurrentProcess();
        }

        for (size_t i=0; i<agent->cinfo->pdata.devspec.disk_cnt; ++i)
        {
            agent->cinfo->pdata.devspec.disk[i]->gen.utc = currentmjd();
            agent->cinfo->pdata.devspec.disk[i]->gib = disk.getUsedGiB(agent->cinfo->pdata.port[agent->cinfo->pdata.devspec.disk[i]->gen.portidx].name);
            agent->cinfo->pdata.devspec.disk[i]->maxgib = disk.getSizeGiB(agent->cinfo->pdata.port[agent->cinfo->pdata.devspec.disk[i]->gen.portidx].name);
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


int32_t request_soh(char *, char* response, Agent *)
{
    std::string rjstring;
    //	strcpy(response,json_of_list(rjstring,sohstring,agent->cinfo));
    strcpy(response,json_of_table(rjstring, agent->cinfo->pdata.agent[0].sohtable, agent->cinfo->meta, agent->cinfo->pdata));

    return 0;
}



// ----------------------------------------------
// disk
int32_t request_diskSize(char *, char* response, Agent *)
{
    return (sprintf(response, "%f", disk.SizeGiB));
}

int32_t request_diskUsed(char *, char* response, Agent *)
{
    return (sprintf(response, "%f", disk.UsedGiB));
}

int32_t request_diskFree(char *, char* response, Agent *)
{
    // TODO: implement diskFree
    //return (sprintf(response, "%.1f", agent->cinfo->pdata.devspec.cpu[0]->gib));

    // in the mean time use this
    return (sprintf(response, "%f", disk.FreeGiB));

}

int32_t request_diskFreePercent (char *, char *response, Agent *)
{
    return (sprintf(response, "%f", disk.FreePercent));
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
// memory
int32_t request_mem(char *, char* response, Agent *)
{
    return (sprintf(response, "%f", deviceCpu.virtualMemoryUsed));
}

int32_t request_mempercent (char *, char *response, Agent *)
{

    return (sprintf(response, "%f", deviceCpu.getVirtualMemoryUsedPercent()));
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
        std::cout << "Error opening file";
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



int create_node () // only use when unsure what the node is
{
    cosmosstruc *cinfo;
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
        cinfo = json_create();
        strcpy(cinfo->pdata.node.name, nodename.c_str());
        cinfo->pdata.node.type = NODE_TYPE_COMPUTER;

        cinfo->pdata.node.piece_cnt = 11;
        cinfo->pdata.piece.resize(cinfo->pdata.node.piece_cnt);
        cinfo->pdata.node.device_cnt = cinfo->pdata.node.piece_cnt;
        cinfo->pdata.device.resize(cinfo->pdata.node.device_cnt);
        cinfo->pdata.devspec.cpu_cnt = 1;
        cinfo->pdata.devspec.disk_cnt = 10;
        cinfo->pdata.node.port_cnt = 10;
        cinfo->pdata.port.resize(cinfo->pdata.node.port_cnt);
        //        json_addbaseentry(cinfo);

        for (size_t i=0; i<cinfo->pdata.node.piece_cnt; ++i)
        {
            cinfo->pdata.piece[i].cidx = i;
            switch (i)
            {
            case 0:
                strcpy(cinfo->pdata.piece[i].name, "Main CPU");
                break;
            default:
                sprintf(cinfo->pdata.piece[i].name, "Drive %d", i);
//                strcpy(cinfo->pdata.piece[i].name, "Main Drive");
                break;
            }

            cinfo->pdata.piece[i].type = PIECE_TYPE_DIMENSIONLESS;
            cinfo->pdata.piece[i].emi = .8;
            cinfo->pdata.piece[i].abs = .88;
            cinfo->pdata.piece[i].hcap = 800;
            cinfo->pdata.piece[i].hcon = 237;
            cinfo->pdata.piece[i].pnt_cnt = 1;
            for (uint16_t j=0; j<3; ++j)
            {
                cinfo->pdata.piece[i].points[0].col[j] = 0.;
            }
            json_addpieceentry(i, cinfo->meta);

            cinfo->pdata.device[i].all.gen.pidx = i;
            cinfo->pdata.device[i].all.gen.cidx = i;
            switch (i)
            {
            case 0:
                cinfo->pdata.device[i].all.gen.type = DEVICE_TYPE_CPU;
                cinfo->pdata.device[i].all.gen.didx = 0;
                cinfo->pdata.device[i].all.gen.portidx = PORT_TYPE_NONE;
                break;
            default:
                cinfo->pdata.device[i].all.gen.type = DEVICE_TYPE_DISK;
                cinfo->pdata.device[i].all.gen.didx = i-1;
                cinfo->pdata.device[i].all.gen.portidx = cinfo->pdata.device[i].all.gen.didx;
                cinfo->pdata.port[cinfo->pdata.device[i].all.gen.didx].type = PORT_TYPE_DRIVE;
#ifdef COSMOS_WIN_OS
                strcpy(cinfo->pdata.port[cinfo->pdata.device[i].all.gen.didx].name, "c:/");
#else
                strcpy(cinfo->pdata.port[cinfo->pdata.device[i].all.gen.didx].name, "/");
#endif
                json_addentry("port_name",cinfo->pdata.device[i].all.gen.didx,65535u,(ptrdiff_t)offsetof(portstruc,name)+(cinfo->pdata.device[i].all.gen.didx)*sizeof(portstruc),COSMOS_MAX_NAME, (uint16_t)JSON_TYPE_NAME,JSON_STRUCT_PORT,cinfo->meta);
                json_addentry("port_type",cinfo->pdata.device[i].all.gen.didx,65535u,(ptrdiff_t)offsetof(portstruc,type)+(cinfo->pdata.device[i].all.gen.didx)*sizeof(portstruc), COSMOS_SIZEOF(uint16_t), (uint16_t)JSON_TYPE_UINT16,JSON_STRUCT_PORT,cinfo->meta);
                break;
            }
            json_addcompentry(i, cinfo->meta, cinfo->pdata);
            json_adddeviceentry(i, cinfo->meta, cinfo->pdata);
        }

        int32_t iretn = json_dump_node(cinfo->meta, cinfo->pdata);
        json_destroy(cinfo);
        return iretn;
    }
    else
    {
        return 0;
    }
}
