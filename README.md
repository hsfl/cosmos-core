# COSMOS core 1.0.2 (beta)

**Please note that the software and its documentation are work in progress. If you find problems or no documentation for something you're looking for please let us know.**

The COSMOS core is the basic element of the COSMOS project with the essential libraries and programs. The api documentation includes the descriptions of the basic support functions necessary to make COSMOS core work, as well as the most useful agent and support programs. The cosmos core is broadly divided into \ref programs and \ref libraries. 
 
If you haven't downloaded the COSMOS software please go to the [COSMOS 101 tutorial](http://bitbucket.org/cosmos/tutorial). Also for the latest information on COSMOS refer to our website:
[http://www.cosmos-project.org/](http://www.cosmos-project.org/). The cosmos-core api documentation is available on [http://cosmos-project.org/docs/core/current/](http://cosmos-project.org/docs/core/current/)  with instructions on how to compile the code in different architectures, the api structure, some tutorials and the description for most classes and functions.

Quick-start:

1) Compile cosmos-core and run two agents that talk to each other. These instructions assume that you are in the **cosmos-source/core** folder. 

* Open Qt Creator and open the project file 'cosmos-core.pro' 
* Configure your project (ex: on Windows it will be something like `Desktop Qt 5.5.1 MinGW 32 bit')
* Optional step to compile the code faster. Please read note 1 if interested
* Build the project by pressing the "Build" button (hammer icon on left side of Qt Creator), or press 'ctrl+b'
* Select agent_001. Click on the "Project" button (computer icon on left side of Qt Creator) and select agent_001
* Run agent_001. Click on the "Run" button (big green icon on left side of Qt Creator) or press 'ctrl+r'
* Select agent_002. 
* Run agent_002. 

The two COSMOS agents should be talking to each other at this point. 
![agent_001_agent_002.PNG](https://bitbucket.org/repo/EpA5jo/images/402568663-agent_001_agent_002.PNG)

2) Install cosmos-core

make sure you have the latest version of Cmake from [http://www.cmake.org/](http://www.cmake.org/) (currently it is 3.3.1)

* Start Qt Creator
* Open the core CMakeLists.txt file (File -> Open File or Project -> Open project : cosmos-source/core/CMakeLists.txt)
* Select a build directory (or use the default)
* Select Generator : ex: MinGW Generator (Desktop Qt 5.5.1 MinGW 32 bit)
* Hit "Run CMake"
* Optional step to compile the code faster. Please read note 1 if interested
* on Targets select "install"
* Build the project : ctrl + b and wait some minutes to finish
* Check your "cosmos" folder (ex: C:\cosmos) to see the files that were installed

For more detailed instructions to get started with COSMOS
please read the [core/docs/README.md](https://bitbucket.org/cosmos/core/src/master/docs/README.md) inside the 
documentation folder.

## Note 1 ##
This is only valid for MinGW and GCC compilers (msvc uses Jom to handle multiple cores). To compile the code faster using all the cores on your machine go to Qt Creator -> Projects (icon on left side bar) -> Build Steps -> Make : Details (expand the icon) on arguments add "-j4" or whatever number of cores that your computer supports. In some cases you may have to add a space in between "-j 4". In some cases it is also possible to just add '-j' and the compiler will automatically use as many processes it can to compile. This approach works well on Windows but in linux it seems to freeze the computer. Use it with caution. If you really need super compilation times then install MSVC 2013 or above. See the results and make your decision.

Compilation tests from cosmos-core.pro using a Win7 with Qt 5.5.1 MinGW 32 bit, AMD FX(tm)-8120 Eight Core Processor 3.11 GHz, 16 GB Ram, 64 bit OS

Kit                             | Build Step                 | Cores used  | Compile time  | 
------------------------------: | :------------------------: | ----------: | :------------ | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe (default) | 1           | 3m 30s        | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe -j4       | 4           | 1m 16s        | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe -j8       | 8           | 1m 4s         | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe -j9       | 8           | 1m 4s         | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe -j        | 8           | 58 s          | 
Desktop Qt 5.5.1 MinGW 32bit    | jom.exe (custom step)      | 8           | 1m 16s        | 
Desktop Qt 5.5.1 MSVC2013 64bit | jom.exe (default)          | 8           | 30s           |