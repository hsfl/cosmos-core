# Software profiler {#tutorial-software-profiler}

## Linux
To profile your software in linux you can use you can use 'gprof'. Here are some instructions to use it

1) Compile with correct switches 
-pg

CFLAGS = -pg 

go to examples/profiler
\$ make testprofiler

2) run to completion, exit normally 
this will create file gmon.out

3) gprof testprofiler
it reads gmon.out and prints a report

## Mac OS

To profile on the Mac use ``Instruments" budled with Xcode
or install http://valgrind.org

Here is a list of profiling tools recomended by Qt: http://qt-project.org/wiki/Profiling-and-Memory-Checking-Tools
