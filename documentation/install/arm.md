Cross-compile COSMOS for the ARM plarform
=========================================

These instructions are to setup a cross-compiler environment so that you can compile COSMOS for embedded devices with different architectures (ARM, PowerPC, etc.). 
We have tested these instructions on Windows 7 and also on Ubuntu 14.04 running on a Virtual Box.

We have tested cross-compiling for the Gumstix Overo, Gumstix Duovero and RaspberryPi architectures but these instructions should work well for other architectures. 
Let us know if you successfully compile COSMOS in other platforms.

It is assumed you have installed the latest version of Qt and have downloaded the COSMOS source.

There are three ways to compile the code for the COSMOS tools and 
the flight software:
1. using Qt Creator and .pro files (probably the easiest)
2. using Qt Creator and Cmake files
3. using the command line and Cmake  (not recommended for beginners)

In either case you will have to install the cross compiler tools for 
the ARM. We recommend using the Linaro Toochain from the Linaro project. 
The current version is 14.11 with GCC 4.9 with C++11 compatibility.

Install Cross-compiler for Windows
----------------------------------
(Tested on Windows 7)

Choose a cross-toolchain for your particular device. In this example we use Linaro toolchain for ARM with hard float: linaro-toolchain-binaries 4.9 (little-endian). 
The latest Linaro cross-compiler for the ARM (as of Feb 6 2015) 
can be directly downloaded from  [http://releases.linaro.org/14.09/components/toolchain/binaries/](http://releases.linaro.org/14.09/components/toolchain/binaries/) then 
choose **gcc-linaro-arm-linux-gnueabihf-4.9-2014.09-20140911_win32.exe**

This will install the cross compiler in a path like *
C:\Program Files (x86)\Linaro\gcc-linaro-arm-linux-gnueabihf-4.9-2014.09*

Linaro is just one option, there are tons of other cross-compiler options ... 

Next, if you want to use .pro files to compile your project go to the section 
[Cross-compile using Qt Creator and .pro files](#pro-files) [Cross-compile using Qt Creator and .pro files][]
or go to [Cross-compile using Qt Creator and Cmake files](#cmake-files) if you want to use CMAKE files.

Install Cross-compiler for Linux (Ubuntu)
-----------------------------------------

install crosscompiler:

- "apt-get install g++-arm-linux-gnueabi gcc-arm-linux-gnueabi 
    g++-arm-linux-gnueabihf gcc-arm-linux-gnueabihf uboot-mkimage"

installing GDB for cross debugging in qt (python support) on Ubuntu 12.04

Configure
```
#!sh
./gdb/configure --target=arm-linux-gnueabihf --with-python=/usr/bin/python
./configure --prefix /usr/local/gdb-python2 --with-python
./configure --target=arm-linux-gnueabihf --with-python=/usr/bin/python
```

* error1
configure: error: no termcap library found

solution
sudo apt-get install ncurses-dev
or
sudo apt-get install libncurses5-dev

* error2
configure: error: no usable python found at /usr/bin/python

solution
sudo apt-get install python-dev 
or
sudo apt-get install python2.7-dev


# Cross-compile using Qt Creator and .pro files #
---------------------------------------------
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
- Open the Tab "Debuggers"  [*check note1*](#note1)
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

<a name="note1"></a>
##### Note 1
If you downloaded the Linaro toolchain for windows it may not have GDB with python enabled, but you can still try using the Linaro GDB. Select the gdb debugger from the Linaro installation path, 
ex: C:\Program Files (x86)\Linaro\gcc-linaro-arm-linux-gnueabihf-4.9-2014.09\bin\arm-linux-gnueabihf-gdb
To compile gdb with python enabled check the section to build GDB with python

 <a name="cmake-files"></a>
Cross-compile using Qt Creator and Cmake files
-------------------------------------------------
On Qt Creator you can open the CMakeLists.txt as a project, this is very convenient
for developing large applications using cosmos. If you go this route use following 
arguments when running the cmake wizard in Qt Creator. 

-DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain_arm -DBUILD_ARM=ON

On the generator list select:
Unix Generator (FlightComputer-Overo), you must configure your remote device before 

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



