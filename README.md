Welcome to COSMOS 1.0-alpha
============================

COSMOS is the Comprehensive Open-architecture Solution for Mission
Operations System, a software framework with source code for 
space mission operations, simulation and flight software.

For the latest information on COSMOS refer to our website:
http://www.cosmos-project.org/

Quick-start:

1) install the latest Qt version for Windows from 
http://www.qt.io/download-open-source/

select Mingw as your main compiler, you may also choose
to select MSVC as an alternative compiler but there may 
be some compatibility issues with COSMOS

2) Select a DEVELOPMENT location. This will be where all your
source code and compilation resides, and can be anywhere that
you have write access to. It will end up as a tree looking something
like this:

DEVELOPMENT
* 	| core
* 	| nodes
* * 		| cubesat1
* * 		.
* * 		.
* * 		.
* 	| projects
* 	| resources
* 	| tools
* * 		| cat
* * 		| ceo
* * 		| dmt
* * 		| libraries
* * 		| most
* 
3) Download COSMOS-core source code, nodes and resources

These should be placed in DEVELOPMENT\core, DEVELOPMENT\nodes and
DEVELOPMENT\resources. The URL's are:

* https://<yourusername>@bitbucket.org/cosmos/core.git
* https://<yourusername>@bitbucket.org/cosmos/nodes.git
* https://<yourusername>@bitbucket.org/cosmos/resources.git

-- using a git client such as Tortoise Git for win or Atlassian SourceTree.
if you are planning on contributing to the project using a 
client may be the easiest alternative

-- using the command line

```
#!bash

$ cd DEVELOPMENT
$ git clone https://<yourusername>@bitbucket.org/cosmos/core.git
$ git clone https://<yourusername>@bitbucket.org/cosmos/nodes.git
$ git clone https://<yourusername>@bitbucket.org/cosmos/resources.git

```

4) Select an (optional)INSTALL location. This will be where operational
files, such as executable binaries, compiled libraries, etc. end up. If you
do not choose one, the following default locations of will be used:
* - (Windows) c:\cosmos
* - (Linux) /usr/local/cosmos
* - (MacOS) /Applications/cosmos
If you do not have write permission (eg: /usr/local/linux),  or you choose
your own location, then you will need to set the COSMOS environment variable
to this location. (Note: It is permissible to set the COSMOS environment variable
to DEVELOPMENT.)

5.) Copy the contents of DEVELOPMENT\resources to INSTALL\resources. Copy any
nodes you wish to work with from DEVELOPMENT\nodes to INSTALL\nodes.

6.) Compile and test the code. This can be done either with Qt Creator, or with
Cmake.

For Qt Creator, Open two instances of Qt Creator and load two agent example projects: 
* - core/examples/agents/agent_001.pro 
* - core/examples/agents/agent_002.pro 
Compile and execute the two programs. Start agent_002 first. 
The two COSMOS agents will start talking to each other.

For Cmake, make sure you have the latest version of Cmake (for Windows this
can be found at:

http://www.cmake.org/

--to compile from the unix command line:

```
#!bash

$cd DEVELOPMENT/core/build
$mkdir {linux|macos} (depending on your OS)
$./do_cmake {linux|macos}
$cd {linux|macos}
$make install
```


--to compile from Qt Creator
*Start Qt Creator
*Open DEVELOPMENT/core/CMakeLists.txt
*Choose a build directory of DEVELOPMENT/core/build/{windows|linux|macos} (depending on your OS)
*Run CMake
*Build install

For more detailed instructions to get started with COSMOS
please read the GET-STARTED-WITH-COSMOS.txt inside the 
documentation folder.

By default we recommend that COSMOS is installed if the following folders
* for Windows: C:\cosmos
* for MacOS: /Applications/cosmos
* for Linux/Unix: /usr/local/cosmos