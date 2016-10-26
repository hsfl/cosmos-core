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
#include "device/cpu/devicecpu.h"

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

// in bytes
double DeviceCpu::getMemoryTotal(){

#if defined(COSMOS_LINUX_OS)
    memoryTotal = cpuLinux.getMemoryTotal();
#endif

#if defined(COSMOS_WIN_OS)
    memoryTotal = cpuWin.getMemoryTotal();
#endif
    return memoryTotal;
}

// in bytes
double DeviceCpu::getMemoryUsed(){

#if defined(COSMOS_LINUX_OS)
    memoryUsed = cpuLinux.getMemoryUsed();
#endif

#if defined(COSMOS_WIN_OS)
    memoryUsed = cpuWin.getMemoryUsed();
#endif
    return memoryUsed;
}

// in bytes
double DeviceCpu::getVirtualMemoryUsed(){

#if defined(COSMOS_LINUX_OS)
    virtualMemoryUsed = cpuLinux.getVirtualMemoryUsed();
#endif

#if defined(COSMOS_WIN_OS)
    virtualMemoryUsed = cpuWin.getVirtualMemoryUsed();
#endif
    return virtualMemoryUsed;

}

double DeviceCpu::getMemoryUsedKiB()
{
    return BytesToKiB(getMemoryUsed());
}

double DeviceCpu::getMemoryUsedMiB()
{
    return BytesToMiB(getMemoryUsed());
}

double DeviceCpu::getMemoryUsedMB()
{
    return BytesToMB(getMemoryUsed());
}

double DeviceCpu::getMemoryTotalKiB()
{
    return BytesToKiB(getMemoryTotal());
}

double DeviceCpu::getMemoryTotalMiB()
{
    return BytesToMiB(getMemoryTotal());
}

double DeviceCpu::getMemoryTotalMB()
{
    return BytesToMB(getMemoryTotal());
}

double DeviceCpu::getVirtualMemoryUsedKiB()
{
    return BytesToKiB(getVirtualMemoryUsed());
}

double DeviceCpu::getVirtualMemoryUsedMiB()
{
    return BytesToMiB(getVirtualMemoryUsed());
}

double DeviceCpu::getVirtualMemoryUsedMB()
{
    return BytesToMB(getVirtualMemoryUsed());
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

string DeviceCpu::getHostName()
{
#if defined(COSMOS_LINUX_OS)
    hostName = cpuLinux.getHostName();
#endif

#if defined(COSMOS_WIN_OS)
    hostName = cpuWin.getHostName();
#endif

    return hostName;
}

pid_t DeviceCpu::getPidOf(string processName)
{
#if defined(COSMOS_LINUX_OS)
    return cpuLinux.getPidOf(processName);
#endif

#if defined(COSMOS_WIN_OS)
    //    return cpuWin.getPidOf(processName);
#endif

}

pid_t DeviceCpu::getMemoryUsedOf(string processName)
{
#if defined(COSMOS_LINUX_OS)
    return cpuLinux.getMemoryUsedOf(processName);
#endif

#if defined(COSMOS_WIN_OS)
    //    return cpuWin.getMemoryUsedOf(processName);
#endif

}

float DeviceCpu::getPercentCpuOf(string processName)
{
#if defined(COSMOS_LINUX_OS)
    return cpuLinux.getPercentCpuOf(processName);
#endif

#if defined(COSMOS_WIN_OS)
    //    return cpuWin.getPercentCpuOf(processName);
#endif
}

float DeviceCpu::getPercentMemoryOf(string processName)
{
#if defined(COSMOS_LINUX_OS)
    return cpuLinux.getPercentMemoryOf(processName);
#endif

#if defined(COSMOS_WIN_OS)
    //    return cpuWin.getPercentMemoryOf(processName);
#endif

}

double DeviceCpu::BytesToKiB(double bytes)
{
    return bytes/1024.;
}

double DeviceCpu::BytesToMiB(double bytes)
{
    return bytes/1024./1024.;
}

double DeviceCpu::BytesToMB(double bytes)
{
    return bytes/1000./1000.;
}





// ----------------------------------------------
// Linux
// ----------------------------------------------
#if defined(COSMOS_LINUX_OS)

//static clock_t lastCPU, lastSysCPU, lastUserCPU, lastCPU_;


double get_cpu_time(){
    return (double)clock() / CLOCKS_PER_SEC;
}

double getTimeInSec() {
    timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec / 1000000.0;;
}


// simple function to collect the results from an exectuted command
// used to get the information from 'ps' or other commands
string exec(string command) {

    const char* cmd = command.c_str();
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    string result = "";
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


string DeviceCpuLinux::getCurrentProcessName(){

    std::ifstream ifs ("/proc/self/status");
    string line;

    if (ifs.is_open()) {
        while ( std::getline(ifs,line) )
        {
            std::size_t pos = line.find("Name");
            if (pos!=string::npos) {
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
    //    using string;

    //    processName = getCurrentProcessName();

    //    //percent = getPercentUseForCurrentProcessOverLifetime();

    //    string procInfo = exec("top -b -n 1 | grep " + processName);

    //    // typical response
    //    // $ 30501 root      20   0 24416 4000 2068 S  6.8  0.8   0:41.99 agent_imu


    //    string::iterator new_end = std::unique(procInfo.begin(), procInfo.end(),
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




float DeviceCpuLinux::getPercentUseForCurrentProcessOverLifetime()
{
    // NOTE: this is not the same as %CPU reported by top
    // see: http://unix.stackexchange.com/questions/58539/top-and-ps-not-showing-the-same-cpu-result
    // ps -C agent_cpu -o %cpu gives the cpu percentage over the lifetime of the process
    // can be usefull as a metric but for now let's just use the current process cpu
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


// memory used in Bytes
double DeviceCpuLinux::getMemoryTotal()
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long memTotal = memInfo.totalram;
    memTotal *= memInfo.mem_unit;

    return (memTotal) ;
}

// memory used in Bytes
double DeviceCpuLinux::getMemoryUsed()
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long memUsed = memInfo.totalram - memInfo.freeram;
    memUsed *= memInfo.mem_unit;

    return (memUsed) ;
}




// virtual memory used in Bytes
double DeviceCpuLinux::getVirtualMemoryUsed()
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long virtualMemUsed = memInfo.totalram - memInfo.freeram;

    virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
    virtualMemUsed *= memInfo.mem_unit;

    return (virtualMemUsed) ;
}


// total virtual memory in Bytes
double DeviceCpuLinux::getVirtualMemoryTotal()
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long totalVirtualMem = memInfo.totalram;

    totalVirtualMem += memInfo.totalswap;
    totalVirtualMem *= memInfo.mem_unit;

    return (totalVirtualMem);
}

// free virtual memory in Bytes
double DeviceCpuLinux::getVirtualMemoryFree()
{
    struct sysinfo memInfo;
    sysinfo (&memInfo);

    long long virtualMemFree = memInfo.freeram;

    //virtualMemFree += memInfo.totalswap - memInfo.freeswap;
    virtualMemFree *= memInfo.mem_unit;

    return (virtualMemFree) ;

}

string DeviceCpuLinux::getHostName() // NOT TESTED
{
    char hostname[128];
    gethostname(hostname, sizeof hostname);
    return (string)hostname;
}

pid_t DeviceCpuLinux::getPidOf(string processName)
{
    char buf[512] = {'\0'};
    string tmp = "pidof " + processName;
    FILE *cmd_pipe = popen(tmp.c_str(), "r");

    fgets(buf, 512, cmd_pipe);
    pid_t pid = strtoul(buf, NULL, 10);

    pclose( cmd_pipe );

    return pid;
}

// in bytes
double DeviceCpuLinux::getMemoryUsedOf(string processName)
{
    //    char buf[512];
    //    string tmp = "pmap -x " + std::to_string(getPidOf(processName)) + " | grep total";
    //    FILE *cmd_pipe = popen(tmp.c_str(), "r");

    //    fgets(buf, 512, cmd_pipe);
    //    pclose( cmd_pipe );

    //    tmp = string(buf);
    //    StringParser sp(tmp);

    //    return sp.getFieldNumberAsInteger(3);;

    //    ifstream stat_stream("/proc/" + std::to_string(getPidOf(processName)) + "/stat",ios_base::in);
    //    string pid, comm, state, ppid, pgrp, session, tty_nr;
    if (processName.empty()) {
        return 0;
    }

    string pid = std::to_string(getPidOf(processName));

    if (pid.empty() || pid == "0") {
        return 0;
    }

    string status_child = "/proc/" + pid + "/status" ;

    int fd, data, stack;
    char buf[4096]; //, status_child[50];
    char *vm;

    //sprintf(status_child, tmp.c_str(), pid);
    if ((fd = open(status_child.c_str(), O_RDONLY)) < 0)
        return -1;

    read(fd, buf, 4095);
    buf[4095] = '\0';
    close(fd);

    data = stack = 0;

    vm = strstr(buf, "VmData:");
    if (vm) {
        sscanf(vm, "%*s %d", &data);
    }
    vm = strstr(buf, "VmStk:");
    if (vm) {
        sscanf(vm, "%*s %d", &stack);
    }

    // convert kB to bytes
    return (data + stack)*1024;

}



float DeviceCpuLinux::getPercentCpuOf(string processName)
{
    procStat pStat1;
    procPidStat p1(processName);

    if (!p1.fileExists) {
        return 0;
    }

    COSMOS_SLEEP(1.0);

    procStat pStat2;
    procPidStat p2(processName);

    float user_util = 100 * (p2.utime - p1.utime) / (pStat2.time_total - pStat1.time_total);
    float sys_util = 100 * (p2.stime - p1.stime) / (pStat2.time_total - pStat1.time_total);

    float percent = user_util + sys_util;

    return percent;
}


float DeviceCpuLinux::getPercentMemoryOf(string processName)
{
    return 100*getMemoryUsedOf(processName)/getMemoryTotal();
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

string DeviceCpuWindows::getHostName()
{
    char nameBuf[MAX_COMPUTERNAME_LENGTH + 2];
    DWORD nameBufSize;

    nameBufSize = sizeof nameBuf - 1;
    if (GetComputerNameA(nameBuf, &nameBufSize) == TRUE) {
        _tprintf(_T("Device name is %s\n"), nameBuf);
    }

    //TODO: fix this
    //    return  string(nameBuf);
    string hostname = nameBuf;
    return  hostname;
}

unsigned long long DeviceCpuWindows::FileTimeToInt64(const FILETIME & ft)
{
    return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);
}

#endif

DeviceCpuLinux::procPidStat::procPidStat(string processName)
{
    DeviceCpu dev;
    stat s;
    // read /proc/<pid>/stat

    //    cout << processName << endl;

    fileExists = false;

    if (processName.empty()) {
        return;
    }

    string pid = std::to_string(dev.getPidOf(processName));

//    cout << pid << endl;

    if (pid.empty() || pid == "0") {
        return;
    }

    // TODO: improve logic
    fileExists = true;

    string proc = "/proc/" + pid + "/stat";
    ifstream stat_pid_stream(proc,std::ios_base::in);

//    cout << stat_pid_stream.good() << endl;

    if (!stat_pid_stream.good()) {
        stat_pid_stream.close();
        return;
    }

    stat_pid_stream >> s.pid >> s.comm >> s.state >> s.ppid >> s.pgrp >> s.session >> s.tty_nr
            >> s.tpgid >> s.flags >> s.minflt >> s.cminflt >> s.majflt >> s.cmajflt
            >> s.utime >> s.stime >> s.cutime >> s.cstime >> s.priority >> s.nice
            >> s.O >> s.itrealvalue >> s.starttime >> s.vsize >> s.rss; // don't care about the rest


    stat_pid_stream.close();

    utime = stoi(s.utime);
    stime = stoi(s.stime);

}

DeviceCpuLinux::procStat::procStat()
{
    // read /proc/stat
    ifstream stat_stream("/proc/stat",std::ios_base::in);

    stat_stream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    stat_stream.close();

    time_total = stoi(user) + stoi(nice) + stoi(system) + stoi(idle) + stoi(iowait) + stoi(irq) + stoi(softirq) + stoi(steal) + stoi(guest) + stoi(guest_nice);

}
