COSMOS core 1.0-alpha
=====================

Note: If you haven't downloaded the COSMOS software please go to the [COSMOS 101 tutorial](https://bitbucket.org/cosmos/tutorial). Also for the latest information on COSMOS refer to our website:
[http://www.cosmos-project.org/](http://www.cosmos-project.org/)

Quick-start:

Compile and test the agent examples. This can be done either with Qt Creator, or with
Cmake. These instructions assume that you are in the COSMOS-DEVELOPMENT folder.

**For Qt Creator with qmake (.pro files)**

Open two instances of Qt Creator and load the two agent example projects: 

* core/examples/agents/agent_001.pro 
* core/examples/agents/agent_002.pro

Compile and execute the two programs. Start agent_002 first. 
The two COSMOS agents will start talking to each other.

**For Cmake**

make sure you have the latest version of Cmake from [http://www.cmake.org/](http://www.cmake.org/) (currently it is 3.3.1)

To compile from the unix command line (we recommend using Ubuntu 14):

```
$ cd core/build
$ mkdir {linux|macos|arm|...} (depending on your target OS)
$ ./do_cmake {linux|macos|arm|...}
$ cd {linux|macos|arm|...}
$ sudo make -j8 install (-j8 will allocate 8 cores for the compiler, use the # of cores in your machine)
```

this will install COSMOS core intro /usr/local/cosmos

**For Qt Creator using Cmake**

* Start Qt Creator
* Open core/CMakeLists.txt
* Choose a build directory of core/build/{windows|linux|macos} (depending on your OS)
* Run CMake
* Build install

For more detailed instructions to get started with COSMOS
please read the [core/documentation/README.md](https://bitbucket.org/cosmos/core/src/master/documentation/README.md) inside the 
documentation folder.

Troubleshooting
---------------
If you get an error 