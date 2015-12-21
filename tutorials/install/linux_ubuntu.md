Install COSMOS on Linux Ubuntu {#install-linux-ubuntu}
==============================

Before anything else go to the [COSMOS 101 tutorial](https://bitbucket.org/cosmos/tutorial/) to make sure you have the basics covered. The COSMOS 101 tutorial should allow you to build the cosmos core in a standard way on linux as well. Only use the following instructions to install COSMOS on a Linux distribution using the command line or some other specific ways.

To compile cosmos-core from the command line (we recommend using Ubuntu 14.04 x64):

Go to the build folder cosmos-source/core
```
$ cd core/build
$ mkdir {linux|macos|arm|...} (depending on your target OS)
$ ./do_cmake {linux|macos|arm|...}
$ cd {linux|macos|arm|...}
$ sudo make -j8 install (-j8 will allocate 8 cores for the compiler, use the # of cores in your machine)
```

this will install COSMOS core into /usr/local/cosmos


# Cross-compiling on for ARM
If you want co cross compile cosmos-core and deploy it in another platform (like an embedded arm processor) please open the file [linux_arm.md](https://bitbucket.org/cosmos/core/src/master/tutorials/install/linux_arm.md)


# Virtual Box installation
If you want to use cosmos on a virtual box then follow these instructions. 
This Virtual Box image intents to make the COSMOS software development and
usage easy for anyone that wants to use it. You can build COSMOS
desktop applications or embedded applications (such as for the
gumstix) using this environment.

Before you run the VB image here are some setting that will make
your life more enjoyable while using the COSMOS VB image.

These settings are for VB 4.3.12

Open the settings menu
* In System/Motherboard, Increase the Base Memory (as much as you can)
* In System/Processor, increase the number of processors (as much as you can)
* In Display/Video, increase the Video Memory (as much as you can)
* In Display/Video, Enable 3D acceleration
* In Network, change the adapter to Bridged Adapter (this allows your guest OS to run on the same network)
* In Serial Ports, enable the serial port if using some hardware

also make sure you install the latest Guest Additions CD by going to the Virtual Box
Devices Menu and click on " Insert Guest Additions CD Image ..." when your guest OS is running

Programs Installed in the VB image
----------------------------------
- COSMOS source software + cosmosroot
- Ubuntu 12.04 x64 updated as of April 25, 2014 (more stable, no
graphics problems)
- Subversion client 1.8 (svn)
- git client
- cmake
- Qt SDK (Qt Creator, Qt Libraries)
- Qt 4.8
- Qt Creator 2.4.1
- Cross compiler tools for ARM architecture
- Google Earth  

COSMOS 101
----------
1. open terminal

2. change to the cosmos folder
$ cd ~/cosmos

3. update the subverion just in case
$ svn up

4. change to your project folder 
$ cd projects/hiakasat

5. make your make script executable
$ chmod +x make_hiakasat.sh 

6. edit your script to make sure it does the right steps
$ vi make_hiakasat.sh (or use your favorite editor)

6. run your make script and from here you have the power!
$ ./make_hiakasat.sh



2.) Cross compile tools for Ubuntu, g++-4.8-arm-linux-gnueabihf. Make sure you have version 4.8 at least. Also make sure you are using Ubuntu 14 or later, if you insist in using Ubuntu 12 please add the following line to your /etc/apt/sources.list
>>deb http://us.archive.ubuntu.com/ubuntu saucy main universe 
then do 
$sudo apt-get update
$sudo apt-get install g++-4.8-arm-linux-gnueabihf


	- "apt-get install g++-arm-linux-gnueabi gcc-arm-linux-gnueabi g++-arm-linux-gnueabihf gcc-arm-linux-gnueabihf uboot-mkimage"

3.) Not yet installed: Install tools for kernel compilation
	- "git clone git://www.sakoman.com/git/linux.git linux"
	- "cd linux"
	- "git checkout omap-3.2"
	- copy appropriate config to arch/arm/configs/omap3_deconfig
	- "mkdir ../kernel-3.2.1+"
	- "make O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- omap3_defconfig"
	- "make O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig"
	- "make O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- uImage -j4"
	- "make O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j4"
	- Make sure /dev/sdb2 mounted as /media/flight1/rootfs
	- Make sure /dev/sdb1 mounted as /media/flight1/boot
	- "sudo make INSTALL_MOD_PATH=/media/flight1/rootfs O=../kernel-3.2.1+ ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules_install"
	- "cp ../kernel-3.2.1+/.config /media/flight1/boot/config-3.2.1+"
	- "cp ../kernel-3.2.1+/arch/arm/boot/uImage /media/flight1/boot/uImage.3.2.1+"
	- "cp ../kernel-3.2.1+/arch/arm/boot/uImage /media/flight1/boot"




install gdb
-----------
installing GDB for cross debuggin in qt (python support) on Ubuntu 12.04


Configure
---------
/home/cosmos/Downloads/gdb-linaro-7.7.1-2014.06-1/gdb/configure --target=arm-linux-gnueabihf --with-python=/usr/bin/python

./configure --prefix /usr/local/gdb-python2 --with-python
./configure --target=arm-linux-gnueabihf --with-python=/usr/bin/python

#error1
configure: error: no termcap library found

solution
sudo apt-get install ncurses-dev
or
sudo apt-get install libncurses5-dev

#error2
configure: error: no usable python found at /usr/bin/python

solution
sudo apt-get install python-dev 
or
sudo apt-get install python2.7-dev

install cmake 2.8.12
--------------------
just in case you don't have cmake already installed
$ wget http://www.cmake.org/files/v2.8/cmake-2.8.12.2.tar.gz

create build directory inside 
$ mkdir build

run cmake
$cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
$ make
$ sudo make install  
$ sudo ldconfig


install git
-----------
just in case you don't have git already installed
$ sudo apt-get install git-core git-gui git-doc

download cosmos/software and cosmos/cosmosroot with svn


install qt 4.8
--------------
the hard way
http://qt-project.org/doc/qt-4.8/install-x11.html

the easy way
$ sudo apt-get install qtcreator

install qt 5.3
--------------
go to http://qt-project.org/downloads and download 
Qt Online Installer for Linux 64-bit (22 MB) 

you'll get this file
qt-opensource-linux-x64-1.6.0-4-online.run

execute the install script
$ chmod +x qt-opensource-linux-x64-1.6.0-4-online.run
$ ./qt-opensource-linux-x64-1.6.0-4-online.run

