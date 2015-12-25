/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/
#include "devicecpu.h"

DeviceCpu::DeviceCpu()
{
    load1minAverage = 0.0;
}

// ----------------------------------------------
// Linux
#if defined(COSMOS_LINUX_OS)

// simple function to collect the results from an exectuted command
// used to get the information from 'ps'
std::string exec(std::string command) {

    const char* cmd = command.c_str();
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


double DeviceCpu::getLoad1minAverage()
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

    // update internal variable
    load1minAverage = load;
    return load;
}

// this function is to be called before getPercentUseForCurrentProcess
// not really working properly, using 'ps' for now
void DeviceCpu::initCpuUtilization(){

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


std::string DeviceCpu::getCurrentProcessName(){

    std::ifstream ifs ("/proc/self/status");
    std::string line;

    if (ifs.is_open()) {
        while ( getline (ifs,line) )
        {
//            std::cout << line << '\n';

            std::size_t pos = line.find("Name");
            if (pos!=std::string::npos) {
               //std::cout << "-------------------" << std::endl;
                StringParser sp(line,'\t');
                processName = sp.getFieldNumber(2);
                break;
            }

        }
        ifs.close();
    }
    else {
        std::cout << "Error opening file";
    }

    //processName = getenv("_");
    return processName;
}

float DeviceCpu::getPercentUseForCurrentProcess()
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

    using std::string;

    processName = getCurrentProcessName();
    //string command = "ps -C "+ processName +" -o %cpu,%mem";
    string procInfo = exec("ps -C "+ processName +" -o %cpu,%mem");

    // using 'ps' command
    // get the second line given by the process
    std::size_t pos = procInfo.find("\n");
    if(pos!=string::npos) {
        procInfo = procInfo.substr(pos);
    }

    // go through every instance of '\n' and remove it
    while ( (pos = procInfo.find("\n") ) !=string::npos) {
        procInfo.erase(pos,1);
    }


    if (procInfo.size() > 0) {
        StringParser sp(procInfo);
        percent = sp.getFieldNumberAsDouble(1);
    } else {
        percent = -1.0;
    }

    //    cout << procInfo << endl;

    return percent;

}


double DeviceCpu::getVirtualMemory()
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long virtualMemUsed = memInfo.totalram - memInfo.freeram;

    virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
    virtualMemUsed *= memInfo.mem_unit;

    return (virtualMemUsed) * 0.000976563; // convert byte to kibibyte
}

double DeviceCpu::getVirtualMemoryTotal() // NOT TESTED
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long totalVirtualMem = memInfo.totalram;

    totalVirtualMem += memInfo.totalswap;
    totalVirtualMem *= memInfo.mem_unit;

    return (totalVirtualMem) * 0.000976563; // convert byte to kibibyte
}


#endif // defined(COSMOS_LINUX_OS)



// test function to sress cpu
void DeviceCpu::stress(){
    for (int i = 0; i< 40000; i++) {
        double temp = sqrt(i)*i/log(i);
    }
}









#if defined (COSMOS_MAC_OS)
double cpu_load()
{
    double avg;

    return avg;

}
#endif

#if defined (COSMOS_WIN_OS)
double cpu_load()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateWindowsCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime)+FileTimeToInt64(userTime)) : -1.0f;
}

double cpu_vmem()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    return (virtualMemUsed) * 0.001; // convert byte to kilobyte
}

double cpu_vmemtotal()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
    return (totalVirtualMem) * 0.001; // convert byte to kilobyte
}

double CalculateWindowsCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
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

//std::string getWindowsDeviceName()
//{
//    TCHAR nameBuf[MAX_COMPUTERNAME_LENGTH + 2];
//    DWORD nameBufSize;

//    nameBufSize = sizeof nameBuf - 1;
//    if (GetComputerName(nameBuf, &nameBufSize) == TRUE) {
//        _tprintf(_T("Device name is %s\n"), nameBuf);
//    }

//    //TODO: fix this
//    //return  std::string(nameBuf);
//    return  "";
//}

unsigned long long FileTimeToInt64(const FILETIME & ft)
{
    return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);
}

#endif
