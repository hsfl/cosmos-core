/*! brief CPU monitoring agent
*/

//! \ingroup agents
//! \defgroup agent_cpu CPU monitoring agent
//! This program accepts requests to return the machines:
//! - memory
//! - disk space
//! - load

#include "configCosmos.h"
#include "agentlib.h"
#include "timeutils.hpp"
#include "jsonlib.h"
#include "elapsedtime.hpp"
#include <stdio.h>
#include <iostream>

#if defined (COSMOS_WIN_OS)
#include "windows.h"
#else
#include <stdlib.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#endif

using namespace std;

int myagent();

int32_t request_soh(char *request, char* response, void *cdata);
int32_t request_disk(char *request, char *response, void *cdata);
int32_t request_load(char *request, char *response, void *cdata);
int32_t request_mem(char *request, char *response, void *cdata);

#ifdef COSMOS_WIN_OS
float GetWindowsCPULoad(), GetWindowsUsedDisk(), GetWindowsVirtualMem();
static float CalculateWindowsCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
static unsigned long long FileTimeToInt64(const FILETIME & ft) {return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);}
#else
float GetLinuxCPULoad(), GetLinuxUsedDisk(), GetLinuxVirtualMem();
static float CalculateLinuxCPULoad (float *out);
#endif

string agentname  = "agent_cpu";
string nodename   = "computer";

int waitsec = 5; // wait to find other agents of your 'type/name', seconds
int loopmsec = 1; // period of heartbeat
char buf4[512];

char sohstring[] = "{\"device_cpu_disk_000\",\"device_cpu_mem_000\",\"device_cpu_load_000\",\"}";

#define MAXBUFFERSIZE 256 // comm buffer for agents

cosmosstruc *cdata;  // to access the cosmos data
ElapsedTime et;

int main(int argc, char *argv[])
{
	cout << "Starting agent cpu" << endl;

	int iretn;
	et.start();
	// Establish the command channel and heartbeat
	if (!(cdata = agent_setup_server(AGENT_TYPE_UDP,
	nodename.c_str(),
	agentname.c_str(),
	1.0,
	0,
	AGENTMAXBUFFER)))
	{
		cout << agentname << ": agent_setup_server failed (returned <"<<AGENT_ERROR_JSON_CREATE<<">)"<<endl;
		exit (AGENT_ERROR_JSON_CREATE);
	} else {
		cout<<"Starting " << agentname << " ... OK" << endl;
	}

	// Add additional requests
	if ((iretn=agent_add_request(cdata, (char *)"soh",request_soh)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"disk",request_disk)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"mem",request_mem)))
		exit (iretn);
	if ((iretn=agent_add_request(cdata, (char *)"load",request_load)))
		exit (iretn);

	agent_set_sohstring(cdata, sohstring);

	// Start our own thread
	iretn = myagent();

	return 0;
}

int myagent()
{
	cout << agentname << " ...online " << endl;

	cdata[0].agent[0].aprd = 2;

	// Start performing the body of the agent
	while(agent_running(cdata))
	{

		COSMOS_SLEEP(cdata[0].agent[0].aprd);

#ifdef COSMOS_WIN_OS
		cdata[0].devspec.cpu[0]->load = GetWindowsCPULoad();
		cdata[0].devspec.cpu[0]->disk = GetWindowsUsedDisk();
		cdata[0].devspec.cpu[0]->mem = GetWindowsVirtualMem();
#else
		cdata[0].devspec.cpu[0]->load = GetLinuxCPULoad();
		cdata[0].devspec.cpu[0]->disk = GetLinuxUsedDisk();
		cdata[0].devspec.cpu[0]->mem = GetLinuxVirtualMem();
#endif
		cout << et.lap() << " " << cdata[0].devspec.cpu[0]->load << endl;
		et.start();
	}
	return 0;
}

#ifdef COSMOS_WIN_OS

float GetWindowsCPULoad()
{
	FILETIME idleTime, kernelTime, userTime;
	return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateWindowsCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime)+FileTimeToInt64(userTime)) : -1.0f;
}

float GetWindowsUsedDisk()
{
	int64_t freeSpace, totalSpace, totalFreeSpace;

	GetDiskFreeSpaceEx( "C:",
	(PULARGE_INTEGER)&freeSpace,
	(PULARGE_INTEGER)&totalSpace,
	(PULARGE_INTEGER)&totalFreeSpace);

	return (totalSpace - totalFreeSpace) * 0.001; // convert byte to kilobyte
}

float GetWindowsVirtualMem()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
	return (virtualMemUsed) * 0.001; // convert byte to kilobyte
}

static float CalculateWindowsCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
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

#else
float GetLinuxCPULoad()
{
	float avg;
	if (!CalculateLinuxCPULoad(&avg))
	{
		printf("Could not get load average\n");
		return 0;
	}

	return avg;
}

float GetLinuxUsedDisk()
{
	struct statvfs buf;
	int64_t disk_size, used, free;
	char *path = "/";
	unsigned int function = 1;

	statvfs(path,&buf);

	disk_size = buf.f_frsize * buf.f_blocks;
	free = buf.f_frsize * buf.f_bfree;
	used = disk_size - free;

	return (used) * 0.000976563; // convert byte to kibibyte
}

float GetLinuxVirtualMem() // unsure if working??
{
	struct sysinfo memInfo;
	sysinfo (&memInfo);

	long long virtualMemUsed = memInfo.totalram - memInfo.freeram;

	virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
	virtualMemUsed *= memInfo.mem_unit;

	return (virtualMemUsed) * 0.000976563; // convert byte to kibibyte
}

static float CalculateLinuxCPULoad(float *out)
{
	FILE *f;
	float avg;
	int	n;

	if (out == NULL)
		return false;
	*out = 0.0;

	if ((f = fopen("/proc/loadavg", "r")) == NULL)
		return false;

	n = fscanf(f, "%f", &avg);
	fclose(f);

	if (n != 1)
		return false;

	*out = avg;
	return true;
}

#endif
int32_t request_soh(char *request, char* response, void *)
{
	string rjstring;
	//	strcpy(response,json_of_list(rjstring,sohstring,cdata));
	strcpy(response,json_of_table(rjstring, cdata[0].agent[0].sohtable, cdata));

	return 0;
}

// raspberry pi agent, cross compile,
// temperature agent, soh string temperature
int32_t request_mem(char *request, char* response, void *)
{
	string mem = to_string(cdata[0].devspec.cpu[0]->mem);
	return (sprintf(response, &mem[0u]));
}

int32_t request_disk(char *request, char* response, void *)
{
	string disk = to_string(cdata[0].devspec.cpu[0]->disk);
	return (sprintf(response, &disk[0u]));
}

int32_t request_load (char *request, char* response, void *)
{
	string load = to_string(cdata[0].devspec.cpu[0]->load);
	return (sprintf(response, &load[0u]));
}
