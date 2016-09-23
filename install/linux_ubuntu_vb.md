# Linux Ubuntu on Virtual Box {#install-linux-ubuntu-vb}

For generic Ubuntu configurations/installation read the linux_ubuntu.md file.
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

Typical Programs Installed in the VB image
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




