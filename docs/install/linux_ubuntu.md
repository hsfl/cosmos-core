Install COSMOS on Linux Ubuntu 12.04 x64 {#install-linux-ubuntu}
-----------------------------------------

Follow these instructions to install COSMOS on a Virtual Box image.

This Virtual Box image intents to make the COSMOS software development and
usage easy for anyone that wants to use it. You can build COSMOS
desktop applications or embedded applications (such as for the
gumstix) using this environment.

Before you run the VB image here are some setting that will make
your life more enjoyable while using the COSMOS VB image.

These settings are for VB 4.3.12

> Open the settings menu
> In System/Motherboard, Increase the Base Memory (as much as you can)
> In System/Processor, increase the number of processors (as much as you can)
> In Display/Video, increase the Video Memory (as much as you can)
> In Display/Video, Enable 3D acceleration
> In Network, change the adapter to Bridged Adapter (this allows your guest OS to run on the same network)
> In Serial Ports, enable the serial port if using some hardware

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



Things that got installed
-------------------------

1.) COSMOS software, of course!

Here are the steps we did for you ...

create cosmos folder
$ mkdir /home/cosmos/cosmos

set empty repository
$ svn co https://www.hsfl.hawaii.edu/svn/cosmos/trunk/ . --depth empty

download cosmos core
$ svn up --set-depth infinity core

configure the projects folder and download the hiakasat project
$ svn up --set-depth empty projects
$ svn up --set-depth infinity projects/hiakasat

configure the tools folder and dowload it
$ svn up --set-depth empty tools
$ svn up --set-depth infinity tools

configure the cosmosroot folder and dowload the resources and nodes/hiakasat
$ svn up --set-depth empty cosmosroot
$ svn up --set-depth infinity cosmosroot/resources
$ svn up --set-depth empty cosmosroot/nodes
$ svn up --set-depth infinity cosmosroot/nodes/hiakasat

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
$ sudo apt-get install git-core git-gui git-doc

download cosmos/software and cosmos/cosmosroot with svn

install svn 1.8
---------------
$ sudo sh -c 'echo "# WANdisco Open Source Repo" >> /etc/apt/sources.list.d/WANdisco.list'
$ sudo sh -c 'echo "deb http://opensource.wandisco.com/ubuntu precise svn18" >> /etc/apt/sources.list.d/WANdisco.list'
$ wget -q http://opensource.wandisco.com/wandisco-debian.gpg -O- | sudo apt-key add -
$ sudo apt-get update

To confirm that the new subversion package has been detected, run:
$ apt-cache show subversion | grep '^Version:'

If the 1.8 package is listed, you should then be able to install as normal:
$ sudo apt-get install subversion


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

