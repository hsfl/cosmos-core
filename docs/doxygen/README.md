COSMOS core 1.0.1.0 (beta)
=========================

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
You may get an error when executing do_cmake like the following
```
bash: ./do_cmake: /bin/bash^M: bad interpreter: No such file or directory
```
this is because the do_cmake file was created on windows and has a different line feeds and carriage returns that unix expects so can't read the file you. We have to fix the line endings so that it can run in linux. One easy way to do this is to open the file on vi and run:

```
:set fileformat=unix ( to change the line endings)
:x (to save and exit)
```

update: this error should be fixed if the .gitattributes is in the repository