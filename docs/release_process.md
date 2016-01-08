Merge 'dev' branch to 'master' procedure
----------------------------------------

Everytime we merge the cosmos-core 'dev' branch to the 'master' it is important to pass the following tests:

Windows 7, MinGW
* update code to latest 'dev' commit
* clean all builds (delete folders and configuration files)
* delete installation files on C:\cosmos\bin,  C:\cosmos\include,  C:\cosmos\lib
* compile and install using cmake with MinGW Generator (Desktop Qt 5.5.1 MinGW 32 bit)
* check if agent_001 and agent_002 run
* check if files were installed on C:\cosmos\bin,  C:\cosmos\include,  C:\cosmos\lib

Windows 7, MSVC
* update code to latest 'dev' commit
* clean all builds (delete folders and configuration files)
* delete installation files on C:\cosmos\bin,  C:\cosmos\include,  C:\cosmos\lib
* compile and install using cmake with Ninja Generator (Desktop Qt 5.5.1 MSVC2013 64 bit)
* check if agent_001 and agent_002 run
* check if files were installed on C:\cosmos\bin,  C:\cosmos\include,  C:\cosmos\lib


Ubuntu 14.04
* update code to latest 'dev' commit
* clean all builds (delete folders and configuration files)
* delete installation files on ~/cosmos/bin, include, lib
* compile and install using cmake with Unix Generator (Desktop Qt 5.5.1 GCC 64 bit)
* check if agent_001 and agent_002 run
* check if files were installed on ~/cosmos/bin,  include, lib

MacOS 10
* update code to latest 'dev' commit
* clean all builds (delete folders and configuration files)
* delete installation files on ~/cosmos
* compile using cmake with kit gcc

Final steps
* make sure that the README.md instructions are still valid
* Update the documentation files on the cosmos-project.org website using doxygen