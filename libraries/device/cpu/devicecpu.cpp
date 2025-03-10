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

double DeviceCpu::getLoad()
{

#if defined(COSMOS_LINUX_OS)
    load = cpuLinux.getLoad1minAverage();
#endif

#if defined(COSMOS_MAC_OS)
    load = cpuMac.getLoad1minAverage();
#endif

#if defined(COSMOS_WIN_OS)
    load = cpuWin.getLoad() * numProcessors;
#endif

    return load;
}

uint16_t DeviceCpu::getCpuCount()
{
    uint16_t count = 1;
#if defined(COSMOS_LINUX_OS)
    count = cpuLinux.getCpuCount();
#endif
    return count;
}

uint16_t DeviceCpu::getBootCount()
{
    uint16_t count = 1;
#if defined(COSMOS_LINUX_OS)
    count = cpuLinux.getBootCount();
#endif
    return count;
}

uint32_t DeviceCpu::getUptime()
{
    uint32_t count = 1;
#if defined(COSMOS_LINUX_OS)
    count = cpuLinux.getUptime();
#endif
    return count;
}

// in bytes
double DeviceCpu::getMemoryTotal(){

#if defined(COSMOS_LINUX_OS)
    memoryTotal = cpuLinux.getMemoryTotal();
#endif

#if defined(COSMOS_WIN_OS)
    memoryTotal = cpuWin.getVirtualMemoryTotal();
#endif
    return memoryTotal;
}

// in bytes
double DeviceCpu::getMemoryUsed(){

#if defined(COSMOS_LINUX_OS)
    memoryUsed = cpuLinux.getMemoryUsed();
#endif

#if defined(COSMOS_WIN_OS)
    memoryUsed = cpuWin.getVirtualMemoryUsed();
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

#if defined(COSMOS_MAC_OS)
    hostName = cpuMac.getHostName();
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
    return cpuWin.getPidOf(processName);
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

    return -1;
}

float DeviceCpu::getPercentCpuOf(string processName)
{
#if defined(COSMOS_LINUX_OS)
    return cpuLinux.getPercentCpuOf(processName);
#endif

#if defined(COSMOS_WIN_OS)
    //    return cpuWin.getPercentCpuOf(processName);
#endif
    return 0.;
}

float DeviceCpu::getPercentMemoryOf(string processName)
{
#if defined(COSMOS_LINUX_OS)
    return cpuLinux.getPercentMemoryOf(processName);
#endif

#if defined(COSMOS_WIN_OS)
    //    return cpuWin.getPercentMemoryOf(processName);
#endif

    return 0.;
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

    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
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
    static FILE *f =  nullptr;
    float load = 0.0;
    int	n;

    if (f == nullptr)
    {
        f = fopen("/proc/loadavg", "r");
        if (f == nullptr)
        {
            return -1;
        }
    }
    fseek(f, 0, SEEK_SET);
    n = fscanf(f, "%f", &load);
//    fclose(f);

    if (n != 1)
        return -1;

    // update internal variable
    load1minAverage = load;
    return load;
}





// this function is to be called before getPercentUseForCurrentProcess
// not really working properly, using 'ps' for now
void DeviceCpuLinux::initCpuUtilization()
{

    static FILE* file = nullptr;
    //    struct tms timeSample;
    char line[128];


    // times is not working properly for some reason
    //lastCPU = times(&timeSample);
    //lastSysCPU = timeSample.tms_stime;
    //lastUserCPU = timeSample.tms_utime;

    //    tic = getTimeInSec();
    //    lastCPUtime  = get_cpu_time();
    //std::clock_t lastCPU_ = std::clock();
    //lastCPUtime = getTimeInSec();


    if (file == nullptr)
    {
        file = fopen("/proc/cpuinfo", "r");
        if (file == nullptr)
        {
            return;
        }
    }
    numProcessors = 0;

    bool foundProcessorTag = false;
    while(fgets(line, 128, file) != NULL){
        if (strncmp(line, "processor", 9) == 0) {
            numProcessors++;
            foundProcessorTag = true;
        }
    }
//    fclose(file);

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
    string tmp = "pidof " + processName;
    string tdata;
    int32_t iretn = 0;

    iretn = data_execute(tmp, tdata);
    if (iretn < 0)
    {
        return iretn;
    }
    pid_t pid = strtoul(tdata.c_str(), nullptr, 10);

    return pid;
}

// in bytes
double DeviceCpuLinux::getMemoryUsedOf(string processName)
{
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

    if ((fd = open(status_child.c_str(), O_RDONLY)) < 0)
        return -1;

    size_t ret = read(fd, buf, 4095);
    if (ret == 0)
    {
        return 0;
    }
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

    secondsleep(1.0);

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
double getTimeInSec() {
    timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec / 1000000.0;;
}

double get_cpu_time(){
    return (double)clock() / CLOCKS_PER_SEC;
}

DeviceCpuMac::DeviceCpuMac()
{
    load1minAverage = 0.0;
    //initCpuUtilization();

    tic = getTimeInSec();
    lastCPUtime  = get_cpu_time();
}


double cpu_load()
{
    double avg;

    return avg;

}


double DeviceCpuMac::getLoad1minAverage()
{
    double loadavg[3];

    // same as running $sysctl vm.loadavg
    getloadavg(loadavg,3);

    // update internal variable
    load1minAverage = loadavg[0];
    return load1minAverage;
}

string DeviceCpuMac::getHostName() // NOT TESTED
{
    char hostname[128];
    gethostname(hostname, sizeof hostname);
    return (string)hostname;
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

int32_t DeviceCpuWindows::getPidOf(string processName)
{
    return 0;
}

#endif

#if defined(COSMOS_LINUX_OS)
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

int32_t DeviceCpuLinux::getCpuCount()
{
    uint16_t tcount = 0;
//    uint16_t tindex;
//    string tdata;
//    int32_t iretn = 0;

//    iretn = data_execute("lscpu -p=cpu", tdata);
//    if (iretn > 0)
//    {
//        vector<string> lines = string_split(tdata, "\n");
//        for (string line : lines)
//        {
//            if (sscanf(line.c_str(), "%hu\n", &tindex) == 1)
//            {
//                ++tcount;
//            }
//        }
//    }
//    else {
//        iretn = data_execute("grep processor /proc/cpuinfo | wc -l", tdata);
//        if (iretn > 0)
//        {
//            tcount = stoi(tdata);
//        }
//    }

    tcount = get_nprocs();

    return tcount;
}

int32_t DeviceCpuLinux::getBootCount()
{
    uint16_t bootcount = 0;
    string tdata;
    int32_t iretn = 0;
    uint16_t tindex;

    iretn = data_execute("boot_count_get", tdata);
    if (iretn > 0)
    {
        if (sscanf(tdata.c_str(), "%hu\n", &tindex) == 1)
        {
            bootcount = tindex;
        }
    }
    //    pclose( fp );
    return bootcount;
}

int32_t DeviceCpuLinux::getUptime()
{
    uint32_t uptime = 0.;
//    string tdata;
//    int32_t iretn = 0;

//    iretn = data_execute("uptime -s", tdata);
//    if (iretn > 0)
//    {
//        calstruc cal;
//        if (sscanf(tdata.c_str(), "%d-%d-%d %d:%d:%d\n", &cal.year, &cal.month, &cal.dom, &cal.hour, &cal.minute, &cal.second) == 6)
//        {
//            uptime = 86400. * (currentmjd() - cal2mjd(cal));
//        }
//    }

    struct sysinfo meminfo;
    sysinfo(&meminfo);
    uptime = meminfo.uptime;

    return uptime;
}
#endif
