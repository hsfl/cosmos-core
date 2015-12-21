# COSMOS core 1.0.1 (beta)

**Please note that the documenation is work in progress. If there is some part of the documentation that is missing or not clear please let us know.**

If you haven't downloaded the COSMOS software please go to the [COSMOS 101 tutorial](http://bitbucket.org/cosmos/tutorial). Also for the latest information on COSMOS refer to our website:
[http://www.cosmos-project.org/](http://www.cosmos-project.org/). The cosmos-core api documentation is available on [http://cosmos-project.org/docs/core/1.0.1/](http://cosmos-project.org/docs/core/1.0.1/). There you will find more detailed instructions on how to compile the code in different architectures, some tutorials and the description for most classes and functions.

Quick-start:

1) Compile and test the first two agent examples. These instructions assume that you are in the **cosmos-source** folder. 

* Open Qt Creator and load agent_002.pro from core/tutorials/agents/agent_002/
* build the agent by pressing the "Build" button (hammer icon on left side of Qt Creator), or press ctrl+b
* run the agent by pressing the "Run" button (green play icon on left side of Qt Creator), or press ctrl+r
* now load agent_001.pro from core/tutorials/agents/agent_001/
* build and run agent

The two COSMOS agents should be talking to each other at this point

2) Install cosmos-core

make sure you have the latest version of Cmake from [http://www.cmake.org/](http://www.cmake.org/) (currently it is 3.3.1)

* Start Qt Creator
* Open the core CMakeLists.txt file (File -> Open File or Project -> Open project : cosmos-source/core/CMakeLists.txt)
* Select a build directory (or use the default)
* Select Generator : ex: MinGW Generator (Desktop Qt 5.5.1 MinGW 32 bit)
* Hit "Run CMake"
* Go to Projects -> Build Steps -> Details (expand the icon)
* on additional arguments add "-j4" or whatever number of cores that your computer supports
* on Targets select "install"
* Build the project : ctrl + b and wait some minutes to finish
* Check your "cosmos" folder (ex: C:\cosmos) to see the files that were installed

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