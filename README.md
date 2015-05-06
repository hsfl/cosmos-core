Welcome to COSMOS 1.0-alpha
============================

COSMOS is the Comprehensive Open-architecture Solution for Mission
Operations System, a software framework with source code for 
space mission operations, simulation and flight software.

For the latest information on COSMOS refer to our website:
http://www.cosmos-project.org/

Quick-start:

1) install the latest Qt version (currently Qt5.4) from

* http://www.qt.io/download-open-source/

2) clone the following repositories

* https://<your-user-name>@bitbucket.org/cosmos/core.git
* https://<your-user-name>@bitbucket.org/cosmos/nodes.git
* https://<your-user-name>@bitbucket.org/cosmos/resources.git
* https://<your-user-name>@bitbucket.org/cosmos/thirdparty.git

using your favorite client (e.g. SourceTree) or the the command line:

```
#!sh
git clone https://<your-user-name>@bitbucket.org/cosmos/core.git
```

3) Open Qt creator and load two agent example projects: 
- core/examples/agents/agent_001.pro 
- core/examples/agents/agent_002.pro 
Compile and execute the two programs. Start agent_002 first. 
The two COSMOS agents will startt talking to each other.

For more detailed instructions to get started with COSMOS
please read the README.md inside the documentation folder.

By default we recommend that COSMOS is installed if the following folders
* for Windows: C:\COSMOS
* for MacOS: /Applications/COSMOS
* for Linux/Unix: /home/cosmos

Suggested structure for the COSMOS developer folder (the installation folder may have a different structure)
COSMOS
```
- bin
- lib
- include
- nodes
- resources
- projects (developer specific)
  |- my_project_1
    |- programs
    |- libraries
    |- build
  |- my_project_2
    |- programs
    |- libraries
    |- build
- documentation
- Tools
  |_ MOST
  |_ CEO
  |_ ...
- source (developer specific)
  |_ core
  |_ thirdparty
  |_ tools
  |_ documentation
```
the source folder is dedicated to developers, the remaining folders are for a regular users.