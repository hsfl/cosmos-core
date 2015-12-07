Install COSMOS on Windows {#install-windows}
=========================

The COSMOS software has been tested with the following
Qt5.4
Mingw 4.9


Getting Started
--------------
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
	| core
	| nodes
		| cubesat1
		.
		.
		.
	| projects
	| resources
	| tools
		| cat
		| ceo
		| dmt
		| libraries
		| most

3) Download COSMOS-core source code, nodes and resources

These should be placed in DEVELOPMENT\core, DEVELOPMENT\nodes and
DEVELOPMENT\resources. The URL's are:

https://<yourusername>@bitbucket.org/cosmos/core.git
https://<yourusername>@bitbucket.org/cosmos/nodes.git
https://<yourusername>@bitbucket.org/cosmos/resources.git

-- using a git client such as Tortoise Git for win or Atlassian SourceTree.
if you are planning on contributing to the project using a 
client may be the easiest alternative

-- using the command line
$ cd DEVELOPMENT
$ git clone https://<yourusername>@bitbucket.org/cosmos/core.git
$ git clone https://<yourusername>@bitbucket.org/cosmos/nodes.git
$ git clone https://<yourusername>@bitbucket.org/cosmos/resources.git


MSVC install (optional)
-----------------------
If you really want to use MSVC then follow these instruction. 
Note: you don't have to use MSVC just to use COSMOS, in fact we
recommend that you use MinGW - the default compiler for Qt on windows.

First install Microsoft Visual Studio Express 2013 (v12.0), we recommend
Express because it is free and 2013 because is more compliant with C++11. 
Do not use an earlier version of MSVC. The 2015 version will be fully 
compliant with C++11. From this link you can download MSVC 2013 express 
and the Debugging Tools for Windows: 
https://msdn.microsoft.com/en-us/windows/hardware/hh852365

If MSVC was correctly installed Qt Creator will automatically detect MSVC 
and CDB. Make sure you have the msvc2013 64-bit OpenGL version of Qt for 5.4 
(you may check/install this with the MaintenanceTool.exe inside the Qt 
installation folder (typically C:\Qt). Finally just make sure you have a 
Qt kit setup for MSVC.

Cross-compiling on Windows for ARM
----------------------------------
please read the file: install-arm.txt

MOST
----
3) Open the MOST.pro file in QT Creator
4) On the Target Setup dialog
	- open the details for the Desktop Build
	- Check the Shadow Build
	- Check either the debug or release (or both) configurations
5) IMPORTANT! 
	- Click on the Projects icon (on the left)
	- Build Steps -> Add Build Step -> Make
	- Make Arguments: add 'install'
	- This installs the necessary files to run MOST from the shadow 
	build directory
5) Hit Run!