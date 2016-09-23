# Install COSMOS on Linux Centos {#install-linux-centos}

Before anything else go to the [COSMOS 101 tutorial](https://bitbucket.org/cosmos/tutorial/) to understand the basic developer setup. 

To compile COSMOS on a Linux distribution we recommend starting by using Ubuntu 14.04 x64 because that will facilitate your initial setup and will get you running COSMOS almost without any specific installations like you have to do on CentOS. But if you really want to run COSMOS on CentOS, it's very possible with just a little bit more work. These instructions were tested on CentOS 7.

# Install Dev. Tools

Let's first start by installing the basic development tools you will need (gcc, git, etc.).

```
# yum group install "Development Tools"
```

# Install Cmake 
Now let's install cmake. Cmake 2.8.12 or above is required. Go to the cmake website (https://cmake.org/download/) and download the latest release for linux (ex: cmake-3.6.2-Linux-x86_64.sh).

Enter the folder the script file was downloaded to make it executable, run the script and copy cmake to /usr/local:
```
# cd ~/Downloads/
# chmod +x cmake-3.6.2-Linux-x86_64.sh
# ./cmake-3.6.2-Linux-x86_64.sh
# cd cmake-3.6.2-Linux-x86_64/
# rsync -avu * /usr/local
```

# Install Qt 
go to http://www.qt.io/download-open-source/ and download the Qt Installer for Linux 64-bit (~30 MB). (ex: http://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run)

you'll get a file like this qt-unified-linux-x64-2.0.3-1-online.run

execute the install script
```
# chmod +x qt-unified-linux-x64-2.0.3-1-online.run
# ./qt-unified-linux-x64-2.0.3-1-online.run
```

this will install Qt in ~/Qt. To open Qt Creator go to /home/miguel/Qt/Tools/QtCreator/bin or go to the "Applications -> Programming" menu. Once Qt Creator is open go to "Tools -> Options -> Build and Run -> CMake" and add the newly installed Cmake "/usr/local/bin/cmake". Go to "Kits" and select the autodetected kit and make sure the CMake Tool is properly selected.

# Compile COSMOS 
Make sure to download the COSMOS repositories to the "cosmos-source" folder (check the [COSMOS 101 tutorial](https://bitbucket.org/cosmos/tutorial/)). Open Qt Creator, "File -> Open File or Project" and load the "CMakeLists.txt" file on the COSMOS/core folder. If all goes well you should see the "cosmos-core" project open on the projects browser, on the left. Click on the "Projects" icon and on the "Build Steps" add the argument "-j 4" (or whatever number of cores you have available on your machine). Finally click on the "Hammer" icon (at the very bottom left of Qt Creator) to build the code, or type "ctr+b".
 









