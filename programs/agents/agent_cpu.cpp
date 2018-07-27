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

// memory
int32_t request_mem(char *request, char *response, Agent *);
int32_t request_mem_kib(char *request, char *response, Agent *);
int32_t request_mem_percent (char*request, char *response, Agent *);
int32_t request_mem_total(char *request, char *response, Agent *);
int32_t request_mem_total_kib(char *request, char *response, Agent *);

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
    int32_t iretn;

//    cout << "Starting agent cpu" << endl;

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

    if ((iretn=create_node()))
    {
        cout << "Unable to make node " << endl;
        exit(1);
    }

//    agent->version = "1.0";

    // Add additional requests

    agent = new Agent(nodename, agentname, 5.);
    if (agent->cinfo == nullptr || !agent->running())
    {
        cout << "Unable to start agent" << endl;
        exit(1);
    }

    agent->add_request("soh",request_soh,"","current state of health message");
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
        sprintf(tempstring, ",\"device_disk_utc_%03d\",\"device_disk_temp_%03d\"", i, i);
        sohstring += tempstring;
        sprintf(tempstring, ",\"device_disk_gib_%03d\",\"device_disk_maxgib_%03d\"",i,i);
        sohstring += tempstring;
    }

    sohstring += "}";
    agent->set_sohstring(sohstring);

    // Start our own thread
    agent_cpu();

    return 0;

}




int agent_cpu()
{

    ElapsedTime et;
    static const double GiB = 1024. * 1024. * 1024.;
    deviceCpu.numProcessors = agent->cinfo->devspec.cpu[0]->maxload;

    // TODO: determine number of disks automatically
    PrintUtils print;
    print.scalar("Number of Disks: ",agent->cinfo->devspec.disk_cnt);
    print.endline();

    et.start();

    // Start performing the body of the agent
    while(agent->running())
    {

        COSMOS_SLEEP(agent->cinfo->agent[0].aprd);

        agent->cinfo->devspec.cpu[0]->utc = currentmjd();

        // get cpu info
        if (agent->cinfo->devspec.cpu_cnt)
        {
            agent->cinfo->devspec.cpu[0]->load = deviceCpu.getLoad();
            agent->cinfo->devspec.cpu[0]->gib = deviceCpu.getVirtualMemoryUsed()/GiB;
            agent->cinfo->devspec.cpu[0]->maxgib = deviceCpu.getVirtualMemoryTotal()/GiB;
            deviceCpu.getPercentUseForCurrentProcess();
        }

        // get disk info
        for (size_t i=0; i<agent->cinfo->devspec.disk_cnt; ++i)
        {
            agent->cinfo->devspec.disk[i]->utc = currentmjd();

            std::string node_path = agent->cinfo->port[agent->cinfo->devspec.disk[i]->portidx].name;

            agent->cinfo->devspec.disk[i]->gib = disk.getUsedGiB(node_path);
            agent->cinfo->devspec.disk[i]->maxgib = disk.getSizeGiB(node_path);
        }

        // if printStatus is true then print in a loop
        if (printStatus) {
            PrintUtils print;
            print.delimiter_flag = true;
            print.scalar("Load",deviceCpu.load ,1,"",4,4);
            print.scalar("DiskSize[GiB]",disk.SizeGiB ,1,"",4,4);

//            cout << "DiskSize[GiB]," << disk.SizeGiB << ", ";
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
    strcpy(response,json_of_table(rjstring, agent->sohtable, agent->cinfo));

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
    //return (sprintf(response, "%.1f", agent->cinfo->devspec.cpu[0]->gib));

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



int create_node () // only use when unsure what the node is
{
    cosmosstruc *cinfo;
    //	std::string node_directory;

    // Ensure node is present
    //cout << "Node name is " << nodename << endl;
    // If could not find node directory then make one on the fly
    if (get_nodedir(nodename).empty())
    {
        cout << endl << "Couldn't find Node directory, making directory now...";
        if (get_nodedir(nodename, true).empty())
        {
            cout << "Couldn't create Node directory." << endl;
            return 1;
        }
        cinfo = json_create();
        json_mapbaseentries(cinfo);
        strcpy(cinfo->node.name, nodename.c_str());
        cinfo->name = nodename;
        cinfo->node.type = NODE_TYPE_COMPUTER;

        json_addpiece(cinfo, "main_cpu", PIECE_TYPE_BOX, 0);
        json_mappieceentry(cinfo->pieces.size()-1, cinfo);
        json_togglepieceentry(cinfo->pieces.size()-1, cinfo, true);

        json_addpiece(cinfo, "main_drive", PIECE_TYPE_BOX, 1);
        json_mappieceentry(cinfo->pieces.size()-1, cinfo);
        json_togglepieceentry(cinfo->pieces.size()-1, cinfo, true);

        cinfo->node.device_cnt = cinfo->node.piece_cnt;
        cinfo->device.resize(cinfo->node.device_cnt);
        cinfo->devspec.cpu_cnt = 1;
        cinfo->devspec.disk_cnt = 1;
        cinfo->node.port_cnt = 1;
        cinfo->port.resize(cinfo->node.port_cnt);

//        cinfo->node.piece_cnt = 2;
//        cinfo->pieces.resize(cinfo->node.piece_cnt);
        for (size_t i=0; i<cinfo->node.piece_cnt; ++i)
        {
//            cinfo->pieces[i].cidx = i;
//            switch (i)
//            {
//            case 0:
//                strcpy(cinfo->pieces[i].name, "Main CPU");
//                break;
//            default:
//                sprintf(cinfo->pieces[i].name, "Drive %lu", i);
//                break;
//            }

//            cinfo->pieces[i].type = PIECE_TYPE_DIMENSIONLESS;
//            cinfo->pieces[i].emi = .8;
//            cinfo->pieces[i].abs = .88;
//            cinfo->pieces[i].hcap = 800;
//            cinfo->pieces[i].hcon = 237;
//            cinfo->pieces[i].density = 1000;
//            cinfo->pieces[i].pnt_cnt = 1;
//            for (uint16_t j=0; j<3; ++j)
//            {
//                cinfo->pieces[i].points[0].col[j] = 0.;
//            }

//            json_mappieceentry(i, cinfo);
//            json_togglepieceentry(i, cinfo, true);
//            cinfo->pieces[i].enabled = true;

            cinfo->device[i].all.pidx = i;
            cinfo->device[i].all.cidx = i;
            switch (i)
            {
            case 0:
                cinfo->device[i].all.type = (uint16_t)DeviceType::CPU;
                cinfo->device[i].all.didx = 0;
                cinfo->device[i].all.portidx = PORT_TYPE_NONE;
                cinfo->device[i].cpu.maxload = 1.;
                cinfo->device[i].cpu.maxgib = 1.;
                json_mapdeviceentry(cinfo->device[i], cinfo);
                break;
            default:
                cinfo->device[i].disk.maxgib = 1000.;
                cinfo->device[i].all.type = (uint16_t)DeviceType::DISK;
                cinfo->device[i].all.didx = i-1;
                cinfo->device[i].all.portidx = cinfo->device[i].all.didx;
                cinfo->port[cinfo->device[i].all.didx].type = PORT_TYPE_DRIVE;
                json_mapdeviceentry(cinfo->device[i], cinfo);
                json_toggledeviceentry(i-1, (uint16_t)DeviceType::DISK, cinfo, true);
#ifdef COSMOS_WIN_OS
                strcpy(cinfo->port[cinfo->device[i].all.didx].name, "c:/");
#else
                strcpy(cinfo->port[cinfo->device[i].all.didx].name, "/");
#endif
                json_mapportentry(cinfo->device[i].all.portidx, cinfo);
                json_toggleportentry(cinfo->device[i].all.portidx, cinfo, true);
                break;
            }
            json_mapcompentry(i, cinfo);
            json_togglecompentry(i, cinfo, true);
            cinfo->device[i].all.enabled = true;
        }

        int32_t iretn = json_dump_node(cinfo);
        json_destroy(cinfo);

        cout << " done!" << endl;
        return iretn;
    }
    else
    {
        return 0;
    }
}
