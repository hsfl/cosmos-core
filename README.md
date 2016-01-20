# COSMOS core 1.0.2 (beta)

**Please note that the software and its documentation are work in progress. If you find problems or no documentation for something you're looking for please let us know.**

The COSMOS core is the basic element of the COSMOS project with the essential libraries and programs. The api documentation includes the descriptions of the basic support functions necessary to make COSMOS core work, as well as the most useful agent and support programs. The cosmos core is broadly divided into \ref programs and \ref libraries. 
 
If you haven't downloaded the COSMOS software please go to the [COSMOS 101 tutorial](http://bitbucket.org/cosmos/tutorial). Also for the latest information on COSMOS refer to our website:
[http://www.cosmos-project.org/](http://www.cosmos-project.org/). The cosmos-core api documentation is available on [http://cosmos-project.org/docs/core/current/](http://cosmos-project.org/docs/core/current/)  with instructions on how to compile the code in different architectures, the api structure, some tutorials and the description for most classes and functions.

Quick-start:

1) Compile cosmos-core and run two agents that talk to each other. These instructions assume that you are in the **cosmos-source/core** folder. 

* Open Qt Creator and open the project file 'cosmos-core.pro' 
* Configure your project (ex: on Windows it will be something like `Desktop Qt 5.5.1 MinGW 32 bit')
* Go to Projects -> Build Steps -> Details (expand the icon) on additional arguments add "-j5" or whatever number of cores that your computer supports + 1 
* Build the project by pressing the "Build" button (hammer icon on left side of Qt Creator), or press 'ctrl+b'
* Select agent_001. Click on the "Project" button (computer icon on left side of Qt Creator) and select agent_001
* Run agent_001. Click on the "Run" button (big green icon on left side of Qt Creator) or press 'ctrl+r'
* Select agent_002. 
* Run agent_002. 

The two COSMOS agents should be talking to each other at this point

2) Install cosmos-core

make sure you have the latest version of Cmake from [http://www.cmake.org/](http://www.cmake.org/) (currently it is 3.3.1)

* Start Qt Creator
* Open the core CMakeLists.txt file (File -> Open File or Project -> Open project : cosmos-source/core/CMakeLists.txt)
* Select a build directory (or use the default)
* Select Generator : ex: MinGW Generator (Desktop Qt 5.5.1 MinGW 32 bit)
* Hit "Run CMake"
* Go to Projects -> Build Steps -> Details (expand the icon) on additional arguments add "-j5" or whatever number of cores that your computer supports + 1 
* on Targets select "install"
* Build the project : ctrl + b and wait some minutes to finish
* Check your "cosmos" folder (ex: C:\cosmos) to see the files that were installed

For more detailed instructions to get started with COSMOS
please read the [core/docs/README.md](https://bitbucket.org/cosmos/core/src/master/docs/README.md) inside the 
documentation folder.

