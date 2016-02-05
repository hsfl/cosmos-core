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

}

double DeviceCpu::getLoad(){

#if defined(COSMOS_LINUX_OS)
    load = cpuLinux.getLoad1minAverage();
#endif

#if defined(COSMOS_WIN_OS)
    load = cpuWin.getLoad();
#endif

    return load;
}


double DeviceCpu::getVirtualMemoryUsed(){

#if defined(COSMOS_LINUX_OS)
    virtualMemoryUsed = cpuLinux.getVirtualMemoryUsed();
#endif

#if defined(COSMOS_WIN_OS)
    virtualMemoryUsed = cpuWin.getVirtualMemoryUsed();
#endif
    return virtualMemoryUsed;

}


double DeviceCpu::getVirtualMemoryTotal(){

#if defined(COSMOS_LINUX_OS)
    virtualMemoryTotal = cpuLinux.getVirtualMemoryTotal();
#endif

#if defined(COSMOS_WIN_OS)
    ;
    virtualMemoryTotal = cpuWin.getVirtualMemoryTotal();
#endif

    return virtualMemoryTotal;
}


double DeviceCpu::getVirtualMemoryUsedPercent(){

    double virtualMemoryUsedPercent = getVirtualMemoryUsed() / getVirtualMemoryTotal();

    return virtualMemoryUsedPercent;
}


double DeviceCpu::getPercentUseForCurrentProcess(){

#if defined(COSMOS_LINUX_OS)
    percentUseForCurrentProcess = cpuLinux.getPercentUseForCurrentProcess();
#endif

#if defined(COSMOS_WIN_OS)
    // TODO
    percentUseForCurrentProcess = 0;
#endif
    return percentUseForCurrentProcess;

}

std::string DeviceCpu::getHostName()
{
#if defined(COSMOS_LINUX_OS)
    hostName = cpuLinux.getHostName();
#endif

#if defined(COSMOS_WIN_OS)
    hostName = cpuWin.getHostName();
#endif

    return hostName;
}





// ----------------------------------------------
// Linux
// ----------------------------------------------
#if defined(COSMOS_LINUX_OS)

static clock_t lastCPU, lastSysCPU, lastUserCPU, lastCPU_;


double get_cpu_time(){
    return (double)clock() / CLOCKS_PER_SEC;
}

double getTimeInSec() {
    timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec / 1000000.0;;
}


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


DeviceCpuLinux::DeviceCpuLinux()
{
    load1minAverage = 0.0;
    //initCpuUtilization();

    tic = getTimeInSec();
    lastCPUtime  = get_cpu_time();
}

double DeviceCpuLinux::getLoad1minAverage()
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
void DeviceCpuLinux::initCpuUtilization(){

    FILE* file;
    struct tms timeSample;
    char line[128];


    // times is not working properly for some reason
    //lastCPU = times(&timeSample);
    //lastSysCPU = timeSample.tms_stime;
    //lastUserCPU = timeSample.tms_utime;

    //    tic = getTimeInSec();
    //    lastCPUtime  = get_cpu_time();
    //std::clock_t lastCPU_ = std::clock();
    //lastCPUtime = getTimeInSec();


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


std::string DeviceCpuLinux::getCurrentProcessName(){

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

float DeviceCpuLinux::getPercentUseForCurrentProcess()
{


    // $ ps -C agent_cpu -o %cpu,%mem --> cpu % for entire lifetime
    // $ top -b -n 1 | grep agent_cpu --> current cpu %

    // the following core does not give
    // same results as top ... so let's use 'ps' to get this information
    //    struct tms timeSample;
    //    clock_t now;

    //CLOCKS_PER_SEC;
    //rusage us;
    //getrusage(RUSAGE_SELF,us);

    //    double te_real = getTimeInSec() - lastCPUtime;
    //    double te_cpu  = (double)(clock() - lastCPU_)/CLOCKS_PER_SEC;


    // update every 2 seconds
    toc = getTimeInSec();
    double elapsedTime =  toc - tic;
    if (elapsedTime > 2){

        //double elapsedTime = getTimeInSec() - tic;
        double elapsedTimeCpu = get_cpu_time() - lastCPUtime;

        //cout << "It took me " << te << " clicks (" << (double)te_cpu << " seconds), real time ";
        //cout << (double)elapsedTime << " seconds" << endl;

        percentCpu = (float)elapsedTimeCpu/elapsedTime * 100;
        //std::cout << "%CPU " << percentCpu << std::endl;

        tic = getTimeInSec();
        lastCPUtime = get_cpu_time();
    }

    // because the function times is not working
    //    // ----
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

    // ------
    //    lastCPU_ = clock();
    //    lastCPUtime = getTimeInSec();

    //    percent = te_cpu/te_real * 100;


    //    // ------------------------------------------
    //    // using top or ps
    //    using std::string;

    //    processName = getCurrentProcessName();

    //    //percent = getPercentUseForCurrentProcessOverLifetime();

    //    string procInfo = exec("top -b -n 1 | grep " + processName);

    //    // typical response
    //    // $ 30501 root      20   0 24416 4000 2068 S  6.8  0.8   0:41.99 agent_imu


    //    std::string::iterator new_end = std::unique(procInfo.begin(), procInfo.end(),
    //                                                [](char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); } // lambda function
    //    );
    //    procInfo.erase(new_end, procInfo.end());

    //    // erase first space in case there is one
    //    if (procInfo.at(0) == ' ') {
    //        procInfo.erase(0,1);
    //    }

    //    if (procInfo.size() > 0) {
    //        StringParser sp(procInfo,' ');
    //        percent = sp.getFieldNumberAsDouble(9);
    //    } else {
    //        percent = -1.0;
    //    }

    return percentCpu;

}




float DeviceCpuLinux::getPercentUseForCurrentProcessOverLifetime(){
    // ps -C agent_cpu -o %cpu gives the cpu percentage over the lifetime of the process
    // can be usefull as a metric but for now let's just use the current process cpu
    using std::string;
    float percent;

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

    return percent;
}


double DeviceCpuLinux::getVirtualMemoryUsed()
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long virtualMemUsed = memInfo.totalram - memInfo.freeram;

    virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
    virtualMemUsed *= memInfo.mem_unit;

    // * 0.000976563 to convert byte to kibibyte
    // MN: keep the basic functions in bytes
    return (virtualMemUsed) ;
}

double DeviceCpuLinux::getVirtualMemoryTotal() // NOT TESTED
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long totalVirtualMem = memInfo.totalram;

    totalVirtualMem += memInfo.totalswap;
    totalVirtualMem *= memInfo.mem_unit;

    // * 0.000976563 to convert byte to kibibyte
    // MN: keep the basic functions in bytes
    return (totalVirtualMem);
}

std::string DeviceCpuLinux::getHostName() // NOT TESTED
{
    char hostname[128];
    gethostname(hostname, sizeof hostname);
    return (std::string)hostname;
}
#endif // defined(COSMOS_LINUX_OS)



// test function to sress cpu
void DeviceCpu::stress(){
    for (int i = 0; i< 40000; i++) {
        double temp = sqrt(i)*i/log(i);
        temp = sqrt(temp);
    }
}








// ----------------------------------------------
// MACOS
// ----------------------------------------------
#if defined (COSMOS_MAC_OS)
double cpu_load()
{
    double avg;

    return avg;

}
#endif


// ----------------------------------------------
// WINDOWS
// ----------------------------------------------
#if defined (COSMOS_WIN_OS)
DeviceCpuWindows::DeviceCpuWindows()
{

}

double DeviceCpuWindows::getLoad()
{
    FILETIME idleTime, kernelTime, userTime;
    return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime)+FileTimeToInt64(userTime)) : -1.0f;
}

double DeviceCpuWindows::CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
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

double DeviceCpuWindows::getVirtualMemoryTotal()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
    return (totalVirtualMem) * 0.001; // convert byte to kilobyte
}

double DeviceCpuWindows::getVirtualMemoryUsed()
{
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    DWORDLONG virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
    return (virtualMemUsed) * 0.001; // convert byte to kilobyte
}

std::string DeviceCpuWindows::getHostName()
{
    TCHAR nameBuf[MAX_COMPUTERNAME_LENGTH + 2];
    DWORD nameBufSize;

    nameBufSize = sizeof nameBuf - 1;
    if (GetComputerName(nameBuf, &nameBufSize) == TRUE) {
        _tprintf(_T("Device name is %s\n"), nameBuf);
    }

    //TODO: fix this
    //return  std::string(nameBuf);
    return  "";
}

unsigned long long DeviceCpuWindows::FileTimeToInt64(const FILETIME & ft)
{
    return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);
}

#endif
