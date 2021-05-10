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

//int agent_cpu(), create_node();
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

int32_t get_sensors(map<string, float> &temps);

static string sohstring;

// cosmos classes
static ElapsedTime et;
static DeviceDisk deviceDisk;
static DeviceCpu deviceCpu;

//static DeviceCpu cpu;
static uint16_t cpu_cidx;
static uint16_t cpu_didx;
static uint16_t didx;

static Agent *agent;

int main(int argc, char *argv[])
{
    int32_t iretn;
    map<string, float> temps;

//	cout<<"size of devicestruc = "<<sizeof(devicestruc)<<endl;
    printf("Memory: %s\n", json_memory_usage().c_str());

    if (argc == 2) {
		agent = new Agent(argv[1], "cpu", 15.);
	} else {
        char hostname[60];
        gethostname(hostname, sizeof (hostname));
        agent = new Agent(hostname, "cpu", 15.);
	}
    printf("Mmeory: %s\n", json_memory_usage().c_str());
    agent->set_debug_level(1);

    if ((iretn = agent->wait()) < 0) {
        agent->debug_error.Printf("%16.10f %s Failed to start Agent %s on Node %s Dated %s : %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str(), cosmos_error_string(iretn).c_str());
        exit(iretn);
    } else {
        agent->debug_error.Printf("%16.10f %s Started Agent %s on Node %s Dated %s\n",currentmjd(), mjd2iso8601(currentmjd()).c_str(), agent->getAgent().c_str(), agent->getNode().c_str(), utc2iso8601(data_ctime(argv[0])).c_str());
    }

    iretn = json_createpiece(agent->cinfo, agent->nodeName + "_cpu", DeviceType::CPU);

    if (iretn < 0)
    {
        agent->debug_error.Printf("Failed to add CPU %s\n", cosmos_error_string(iretn).c_str());
        agent->shutdown();
        exit(1);
    }
    cpu_cidx = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
    cpu_didx = agent->cinfo->device[cpu_cidx]->didx;

    sohstring = "{\"node_downtime\"";
    sohstring += ",\"device_cpu_utc_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_temp_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_maxgib_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_gib_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_maxload_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_load_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_uptime_00" + std::to_string(cpu_didx) + "\"";
    sohstring += ",\"device_cpu_boot_count_00" + std::to_string(cpu_didx) + "\"";

//    static const double GiB = 1024. * 1024. * 1024.;

    agent->cinfo->devspec.cpu[cpu_didx].load = static_cast <float>(deviceCpu.getLoad());
    agent->cinfo->devspec.cpu[cpu_didx].gib = static_cast <float>(deviceCpu.getVirtualMemoryUsed()/1073741824.);
    agent->cinfo->devspec.cpu[cpu_didx].maxgib = static_cast <float>(deviceCpu.getVirtualMemoryTotal()/1073741824.);
    agent->cinfo->devspec.cpu[cpu_didx].maxload = deviceCpu.getCpuCount();
    deviceCpu.numProcessors = agent->cinfo->devspec.cpu[cpu_didx].maxload;
    deviceCpu.getPercentUseForCurrentProcess();

    vector <DeviceDisk::info> dinfo = deviceDisk.getInfo();
    for (uint16_t i=0; i<dinfo.size(); ++i)
    {
        char name[10];
        sprintf(name, "disk_%02u", i);
        iretn = json_createpiece(agent->cinfo, name, DeviceType::DISK);
        if (iretn < 0)
        {
            agent->debug_error.Printf("Failed to add DISK %s : %s\n", dinfo[i].mount.c_str(), cosmos_error_string(iretn).c_str());
            agent->shutdown();
            exit(1);
        }
        uint16_t cidx = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
        uint16_t didx = agent->cinfo->device[cidx]->didx;
        if (dinfo[i].mount.size() > COSMOS_MAX_NAME)
        {
            dinfo[i].mount.resize(COSMOS_MAX_NAME);
        }
        agent->cinfo->devspec.disk[didx].path = dinfo[i].mount.c_str();
        sohstring += ",\"device_disk_utc_00" + std::to_string(didx) + "\"";
        sohstring += ",\"device_disk_temp_00" + std::to_string(didx) + "\"";
        sohstring += ",\"device_disk_path_00" + std::to_string(didx) + "\"";
        sohstring += ",\"device_disk_gib_00" + std::to_string(didx) + "\"";
        sohstring += ",\"device_disk_maxgib_00" + std::to_string(didx) + "\"";
        sohstring += ",\"device_disk_utilization_00" + std::to_string(didx) + "\"";
        sohstring += ",\"device_disk_temp_00" + std::to_string(didx) + "\"";
    }

    iretn = get_sensors(temps);
    size_t tcount = 0;
    for (auto const &entry : temps)
    {
        string name = "telem_" + to_unsigned(tcount, 2, true);
        iretn = json_createpiece(agent->cinfo, name, DeviceType::TELEM);
        uint16_t cidx = agent->cinfo->pieces[static_cast <uint16_t>(iretn)].cidx;
        uint16_t didx = agent->cinfo->device[cidx]->didx;
        agent->cinfo->device[cidx]->utc = currentmjd();
        agent->cinfo->devspec.telem[didx].name = entry.first;
        agent->cinfo->devspec.telem[didx].type = TelemTypeValue["vfloat"];
        agent->cinfo->devspec.telem[didx].vfloat = entry.second;
        sohstring += ",\"device_telem_utc_" + to_unsigned(didx, 3, true) + "\"";
        sohstring += ",\"device_telem_name_" + to_unsigned(didx, 3, true) + "\"";
        sohstring += ",\"device_telem_type_" + to_unsigned(didx, 3, true) + "\"";
        sohstring += ",\"device_telem_vfloat_" + to_unsigned(didx, 3, true) + "\"";
        ++tcount;
    }

    sohstring += "}";
    agent->set_sohstring(sohstring);
    json_updatecosmosstruc(agent->cinfo);
    printf("Mmeory: %s\n", json_memory_usage().c_str());

    printf("SOH String: %s\n", sohstring.c_str());

//    json_dump_node(agent->cinfo);

    // TODO: determine number of disks automatically
    agent->debug_error.Printf("Disks: %u Cores: %f\n", agent->cinfo->devspec.disk_cnt, agent->cinfo->devspec.cpu[cpu_didx].maxload);
//    PrintUtils print;
//    print.scalar("Number of Disks: ",agent->cinfo->devspec.disk_cnt);
//    print.endline();
//    print.scalar("Number of Cores: ",agent->cinfo->devspec.cpu[didx].maxload);
//    print.endline();

    agent->debug_error.Printf("CPU Agent initialized\n");

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
    agent->set_debug_level(0);
    while(agent->running())
    {
        agent->cinfo->device[cpu_cidx]->utc = currentmjd();
        if (agent->get_debug_level())
        {
            agent->debug_error.Printf("%16.10f ", agent->cinfo->device[cpu_cidx]->utc);
        }
        agent->cinfo->node.downtime = get_last_offset();

        // get cpu info
        if (agent->cinfo->devspec.cpu_cnt)
        {
            agent->cinfo->devspec.cpu[didx].uptime = deviceCpu.getUptime();
            agent->cinfo->devspec.cpu[didx].boot_count = deviceCpu.getBootCount();
            agent->cinfo->devspec.cpu[didx].load = deviceCpu.getLoad();
            agent->cinfo->devspec.cpu[didx].gib = deviceCpu.getVirtualMemoryUsed()/1073741824.;
            agent->cinfo->devspec.cpu[didx].maxgib = deviceCpu.getVirtualMemoryTotal()/1073741824.;
            deviceCpu.getPercentUseForCurrentProcess();
        }
        if (agent->get_debug_level())
        {
            agent->debug_error.Printf("Load %6.2f %6.2f ", agent->cinfo->devspec.cpu[didx].load, agent->cinfo->devspec.cpu[didx].maxload);
            agent->debug_error.Printf("Memory %6.2f %6.2f ", agent->cinfo->devspec.cpu[didx].gib, agent->cinfo->devspec.cpu[didx].maxgib);
        }

        // get disk info
        for (size_t i=0; i<agent->cinfo->devspec.disk_cnt; ++i)
        {
            agent->cinfo->devspec.disk[i].utc = currentmjd();

            string node_path = agent->cinfo->devspec.disk[i].path;

            agent->cinfo->devspec.disk[i].gib = deviceDisk.getUsedGiB(node_path);
            agent->cinfo->devspec.disk[i].maxgib = deviceDisk.getSizeGiB(node_path);
            if (agent->get_debug_level())
            {
                agent->debug_error.Printf("%s %6.2f %6.2f ", node_path.c_str(), agent->cinfo->devspec.disk[i].gib, agent->cinfo->devspec.disk[i].maxgib);
            }
        }

        // get temperature
        string response;
        iretn = data_execute("get_cpu_temperature", response);
        if (iretn > 0)
        {
            float ctemp = stof(response);
            cputemp.update(currentmjd(), ctemp);
            agent->cinfo->devspec.cpu[cpu_didx].temp = cputemp.eval(currentmjd());
        }

        iretn = get_sensors(temps);
        if (iretn > 0)
        {
            size_t tcount = 0;
            for (auto const &entry : temps)
            {
                agent->cinfo->devspec.telem[tcount].utc = currentmjd();
                agent->cinfo->devspec.telem[tcount].name = entry.first;
                agent->cinfo->devspec.telem[tcount].vfloat = entry.second;
                ++tcount;
            }
        }

        if (agent->get_debug_level())
        {
            agent->debug_error.Printf("\n");
        }

        agent->finish_active_loop();
    }

    agent->shutdown();

    // Start our own thread
//    agent_cpu();

    return 0;

}

int32_t get_sensors(map<string, float> &temps)
{
    // get sensors
    string response;
    int32_t iretn;
    iretn = data_execute("sensors -u", response);
    if (iretn > 0)
    {
        temps.clear();
        vector<string> input = string_split(response, "\n");
        string device = "";
        for (string line : input)
        {
            if (line.find(":") == string::npos)
            {
                device = line.substr(0, line.find('\n'));
            }
            else if (line.find("ERROR") != string::npos)
            {
                continue;
            }
            else if (line.find("_input") != string::npos)
            {
                char name[30];
                float value;
                sscanf(line.c_str(), "%s %f", name, &value);
                string longname = name;
                longname = longname.substr(0, longname.find('_'));
                temps[device+":"+longname] = value;
            }
            else if (line.find("power") != string::npos && line.find("_average") != string::npos)
            {
                char name[30];
                float value;
                sscanf(line.c_str(), "%s %f", name, &value);
                string longname = name;
                longname = longname.substr(0, longname.find('_'));
                temps[device+":"+longname] = value;
            }
        }
        return temps.size();
    }
    else {
        return iretn;
    }
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
    //return (response =  "%.1f", agent->cinfo->devspec.cpu[didx]gib));

    // in the mean time use this
    return ((response = std::to_string(deviceDisk.FreeGiB)).length());

}

int32_t request_diskFreePercent (string &, string &response, Agent *)
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

    std::ifstream ifs ("/var/log/boot_count.log");
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
	int tempint;
    sscanf(request.c_str(),"%*s %d",&tempint);
	printStatus = tempint;
    cout << "printStatus is " << printStatus <<  endl;

    return 0;
}
