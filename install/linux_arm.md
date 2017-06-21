# Install COSMOS on Linux-ARM (crosscompile) {#install-arm} 

These instructions are to setup a cross-compiler environment so that you can compile COSMOS for embedded devices with different architectures (ARM, PowerPC, etc.). 
We have tested these instructions on Windows 7 and also on Ubuntu 14.04 running on a Virtual Box.

We have tested cross-compiling for the Gumstix Overo, Gumstix Duovero and RaspberryPi architectures but these instructions should work well for other architectures. 
Let us know if you successfully compile COSMOS in other platforms.

It is assumed you have installed the latest version of Qt and have downloaded the COSMOS source (if you haven't go to [COSMOS 101 tutorial](http://bitbucket.org/cosmos/tutorial)).

There are three main ways to compile the COSMOS software code:

1. using Qt Creator and .pro files  (probably the easiest)
2. using Qt Creator and Cmake files (recommended)
3. using the command line and Cmake (not recommended for beginners)

In either case you will have to install the cross compiler tools for 
the ARM. We recommend using the Linaro Toochain from the Linaro project. 
The current version is 14.11 with GCC 4.9 with C++11 compatibility.

# Install Cross-compiler for Windows 

These instructions were tested on Windows 7.

Choose a cross-toolchain for your particular device. In this example we use Linaro toolchain for ARM with hard float: linaro-toolchain-binaries 4.9 (little-endian). 
The latest Linaro cross-compiler for the ARM (as of Feb 6 2015) 
can be directly downloaded from  [http://releases.linaro.org/14.09/components/toolchain/binaries/](http://releases.linaro.org/archive/14.09/components/toolchain/binaries/) then 
choose **gcc-linaro-arm-linux-gnueabihf-4.9-2014.09-20140911_win32.exe**

This will install the cross compiler in a path like *
C:\Program Files (x86)\Linaro\gcc-linaro-arm-linux-gnueabihf-4.9-2014.09*

Linaro is just one option, there are tons of other cross-compiler options ... 

Next, if you want to use .pro files to compile your project go to the section 
[Cross-compile using Qt Creator and .pro files](#markdown-header-cross-compile-using-qt-creator-and-pro-files)
or go to [Cross-compile using Qt Creator and Cmake files](#markdown-header-cross-compile-using-qt-creator-and-cmake-files) if you want to use CMAKE files.


# Install Cross-compiler for Linux (Ubuntu 14)

We recommend installing g++4.8 (g++-4.8-arm-linux-gnueabihf) or above. Also it is best if you are using Ubuntu 14 or later. To install the latest crosscompiler (g++-5.2 as of Dec 2015):
```
$ sudo apt-get install g++-arm-linux-gnueabihf
$ sudo apt-get install gperf bison flex texinfo
# check the version
$ arm-linux-gnueabihf-g++ --version
# check the insllation path
$ which arm-linux-gnueabihf-g++
```

optional installs
```
$ sudo apt-get install g++-arm-linux-gnueabi gcc-arm-linux-gnueabi g++-arm-linux-gnueabihf gcc-arm-linux-gnueabihf uboot-mkimage
```

If you insist in using Ubuntu 12 please add the following line to your /etc/apt/sources.list
```
$ deb http://us.archive.ubuntu.com/ubuntu saucy main universe 
 
$ sudo apt-get update
$ sudo apt-get install g++-4.8-arm-linux-gnueabihf
```


installing GDB for cross debugging in qt (python support) 

1. download gdb from http://www.gnu.org/software/gdb/download/
recommended version is gdb 7.8
http://ftp.gnu.org/gnu/gdb/gdb-7.8.tar.gz

2. Extract and move to the extracted directory

3. create a build folder 

```
$ mkdir -p build/gdb
$ cd build/gdb
```

4. Configure with python enabled
```
#!sh
../../configure --target=arm-linux-gnueabihf --with-python
```

5. make
```
$ make -j5 # if you have 4 cores
```

** error1 **

configure: error: no termcap library found

solution
```
#!sh
sudo apt-get install ncurses-dev
# or
sudo apt-get install libncurses5-dev
```

** error2 **

configure: error: no usable python found at /usr/bin/python

solution
```
#!sh
sudo apt-get install python-dev 
# or
sudo apt-get install python2.7-dev
```

# Cross-compile using Qt Creator and .pro files
Using .pro files may be more convenient than Cmake files. We think of 
Cmake more for command line compilation of code although it is 
also possible to use Cmake files to load a COSMOS project in Qt 
(see next section).

Make sure you use the latest Qt Creator, these instructions were 
compiled using Qt Creator version 3.3.0 (Dec 2014) on Windows 7 
but they should work similarly for other platforms.

- Open Qt Creator 3.3
- Open Tools->Options
- Click on "Build and Run" Option on the left menu
- Open the Tab "Debuggers"  [*check note1*](#markdown-header-note-1)
- Move to the next Tab "Compilers": Add a GCC compiler, 
in this case the ARM-LINUX-GNUEABIHF-G++ 4.9 cross compiler (or whatever else you have). 
On windows the compiler path will be something like: C:\Program Files (x86)\Linaro\gcc-linaro-arm-linux-gnueabihf-4.9-2014.09\bin\arm-linux-gnueabihf-g++.exe
- Move to the Tab "Kits": this is the important part!!! 
   - Create a new Kit a give it a name that describes well the the target that will run the code, ex. FlightComputer
   - On "Device Type" choose "Generic Linux Device" 
   - On Device click Manage. Now you can add your target platform (make sure you can connect to it over an ip address or hostname). Click "Add", select "Generic Linux Device", "Start Wizard", type the info to connect to your device and finally go through the test step to make sure it works. You should see a report window saying something like: 
"Connecting to host...
Checking kernel version...
Linux 3.2.1+ armv7l
Checking if specified ports are available...
All specified ports are available.
Device test finished successfully."
(note: make sure you have installed GDB server on your remote device, ex. apt-get install gdbserver"
Click apply. Click on the "Build and Run" option on the left menu, go back to "Kits" to continue configuring your Kit. Make sure your new device is selected.
   - On Compiler choose the GCC cross compiler you just configured 
   - On Debugger choose the GDB cross debugger you just configured
-- On Qt version choose the latest Qt version you have installed, ex Qt 5.4 Mingw 32bit (this is important because qmake must be present to process the .pro files, even if qmake is not compatible with the compiler, the qmakespec will take care of choosing the right compiler )
   - On Qt mkspec type: linux-arm-gnueabihf-g++, next you will  add the Qt mkspec file so that it knows about your cross compiler.

- Now, go to the folder where your selected Qt version is installed.
**Ex.: C:\Qt\5.4\mingw491_32\mkspecs** or **Ex.: C:\Qt\Qt5.3.1\5.3\mingw482_32\mkspecs**
By default there is a folder linux-arm-gnueabihf-g++ already available. That would be for the mkspec "linux-arm-gnueabi-g++" but we need one for "linux-arm-gnueabihf-g++" (with hf added to gnueabi...). Copy the folder "linux-arm-gnueabi-g++" and paste it to the same level, rename it to be "linux-arm-gnueabihf-g++". Go inside the folder and open the file qmake.conf. Replace all instances of "gnueabi" to "gnueabihf". Save and close the file.
- Next we have to add the path to the crosscompiler binaries to the PATH environment so that the mkspec file can find the right tools (you could instead just add the full path to the cross compiler tools in the qmake.conf file). On Windows7 go to the Start Menu, right click on "Computer", select "Properties", select "Advanced System Settings", select "Environment variables", 
  and on "System Variables" select the PATH variable and at the end of the line add the path to your cross compiler tools, 
  ex: ";C:\Program Files (x86)\Linaro\gcc-linaro-arm-linux-gnueabihf-4.9-2014.09\bin". 
- The final thing: you are going to need a make program in your PATH environment, the easiest thing to do is to copy "mingw32-make" into your cross compiler bin folder. Copy "mingw32-make" from "C:\Qt\Tools\mingw491_32\bin" (or wherever your have your qt mingw install) to "C:\Program Files (x86)\Linaro\gcc-linaro-arm-linux-gnueabihf-4.9-2014.09\bin" and rename the file to "make.exe".

Now, everything should be set to go and you can start compiling the COSMOS code and deploy it to your ARM platform.

Let's just create a simple project and test the cross compilation steps. 
Open Qt Creator, a create a new project, select "Non-Qt Project" and "Plain C++ Project", select the folder and name for this this project. 
Select the Kit you just configured with your cross compiler, ex: Linaro. Hit "Finish". Click "ctr+b"

Window -> Show Sidebar
Edit .pro file for your project
Add the following:

	target.path = /root/work 
	INSTALLS += target

Where /root/work is your target computer's folder that will contain your deployed executables

Now run your program, and it should deploy the executable to the target machine, and run it in "Application Output".
 - To add command line arguments, click on the left side "Projects" -> Run (From your kit at the top, Run vs Build) -> Modify Run: Arguments

##### Note 1
If you downloaded the Linaro toolchain for windows it may not have GDB with python enabled, but you can still try using the Linaro GDB. Select the gdb debugger from the Linaro installation path, 
ex: C:\Program Files (x86)\Linaro\gcc-linaro-arm-linux-gnueabihf-4.9-2014.09\bin\arm-linux-gnueabihf-gdb
To compile gdb with python enabled check the section to build GDB with python

##### Note 2 
if you get a problem like this:

* 'test' is not recognized as an internal or external command *

this is because 'test' is not installed in your system (windows) .. or something like that. I think 'test' is a unix program that check for folders in unix. 
Either you install 'test' in your path ... OR!!! 

- don't use SUBDIRS in your .pro, I've found that if you load you prodject directly (without subdirs) it works ok!


Cross-compile using Qt Creator and Cmake files
-----------------------------------------------
On Qt Creator you can open the CMakeLists.txt as a project, this is very convenient
for developing large applications using cosmos. If you go this route use following 
arguments when running the cmake wizard in Qt Creator. 

-DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain_arm -DBUILD_ARM=ON //No longer  necessary 

On the generator list select:

The remote device you configured before, Unix Generator (FlightComputer-Overo)

Hit the button "Run Cmake". This will configure your project using cmake. Another option would be to use a *.pro file to configure this project. At this moment it isn't configured that way. The .pro file would be listed in hiakasat/qt/hiakasat.pro. Maybe this is prefererd, to use the .pro when working using Qt Creator and use cmake when compiling on the command line interface. But try to consolidate the build directories. This will help.

In Qt creator click on the Projects icon on the left. You should be now on the Build Settings
- click add->clone selected and rename the build configuration to what you are going to build. Ex: agent_adcs 
- check the build directory. It should look something like this
/home/cosmos/cosmos/projects/hawaiisat1/hiakasat-build
- on the build steps expand the details and select just the program you want to build. Ex. agent_adcs, add the make option to compile with multiple cores, -j8
your make step should look like this
make agent_adcs -j8

Now move to the Run Settings
- Add a deployment method, rename to what you want to deply. Ex. agent_adcs (Remote)
- delete step "check for free disk space"
- delete step "upload files via SFTP"
- add deploy step "custom process step"
on the command, browse for the pushbin script in the scripts folder
on the arguments, add the program that you are going to push into the remote computer. Ex. programs/agents/agent_adcs
Command: /home/cosmos/cosmos/projects/hawaiisat1/hiakasat/scripts/pushbin

On the Run configuration
- Click the "Add" button and selec the program you want to run in the remote host. Ex: agent_adcs (on Remote Device)
- check the box "use this command instead" and type /fligh_software/bin/agent_adcs
- add the arguments your program needs
- select working directory to be /flight_software/bin

Now build it and run it to see if it works. You should be able to see the progress in the "compile output"



# linux kernel compilation
```
$ git clone git://www.sakoman.com/git/linux.git linux"
$ cd linux"
$ git checkout omap-3.2"
# copy appropriate config to arch/arm/configs/omap3_deconfig
$ mkdir ../kernel-3.2.1+"
$ make O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- omap3_defconfig"
$ make O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig"
$ make O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- uImage -j4"
$ make O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j4"
# Make sure /dev/sdb2 mounted as /media/flight1/rootfs
# Make sure /dev/sdb1 mounted as /media/flight1/boot
$ sudo make INSTALL_MOD_PATH=/media/flight1/rootfs O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules_install"
$ cp ../kernel-3.2.1+/.config /media/flight1/boot/config-3.2.1+"
$ cp ../kernel-3.2.1+/arch/arm/boot/uImage /media/flight1/boot/uImage.3.2.1+"
$ cp ../kernel-3.2.1+/arch/arm/boot/uImage /media/flight1/boot"
```

