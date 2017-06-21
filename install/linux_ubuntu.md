# Install COSMOS on Linux Ubuntu {#install-linux-ubuntu}

Before anything else go to the [COSMOS 101 tutorial](https://bitbucket.org/cosmos/tutorial/) to make sure you have the basics covered. The COSMOS 101 tutorial should allow you to build the cosmos core in a standard way on linux as well. Only use the following instructions to install COSMOS on a Linux distribution using the command line or some other specific ways.

To compile cosmos-core from the command line \(we recommend using Ubuntu 14.04 x64\):

Go to the build folder cosmos-source/core

```
$ cd core/build
$ mkdir {linux|macos|arm|...} (depending on your target OS)
$ ./do_cmake {linux|macos|arm|...}
$ cd {linux|macos|arm|...}
$ sudo make -j8 install (-j8 will allocate 8 cores for the compiler, use the # of cores in your machine)
```

this will install COSMOS core into /usr/local/cosmos

# Cross-compiling for ARM

If you want co cross compile cosmos-core and deploy it in another platform \(like an embedded arm processor\) please open the file [linux\_arm.md](https://bitbucket.org/cosmos/core/src/master/tutorials/install/linux_arm.md)

# install cmake

we recommend cmake 2.8.12 or above. Install the easy way:

```
$ sudo apt-get install cmake
# check cmake version
$ cmake --version
```

The hard way:

```
$ wget http://www.cmake.org/files/v2.8/cmake-2.8.12.2.tar.gz
# create build directory inside 
$ mkdir build
# run cmake to build cmake
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
$ make
$ sudo make install  
$ sudo ldconfig
```

# install git

---

just in case you don't have git already installed

```
# install git
$ sudo apt-get install git
# check git version
$ git --version
```

optional \(but recommended\)

```
# install git gui tool
$ sudo apt-get install git-gui
# check git gui
$ git gui
```

# install qt 5.3

go to [http://www.qt.io/download-open-source/](http://www.qt.io/download-open-source/) and download the Qt Installer for Linux 64-bit \(~30 MB\). \(ex: [http://download.qt.io/official\_releases/online\_installers/qt-unified-linux-x64-online.run](http://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run)\)

you'll get a file like this qt-unified-linux-x64-2.0.2-2-online

execute the install script

```
$ chmod +x qt-unified-linux-x64-2.0.2-2-online
$ ./qt-unified-linux-x64-2.0.2-2-online
```



