# Install COSMOS on Windows {#install-windows}

Before anything else go to the [COSMOS 101 tutorial](https://bitbucket.org/cosmos/tutorial/) to make sure you have the basics covered. If you are just using a default installation using Qt with mingw then you should be good to get started to compile the cosmos software.

The following instructions are just for specific installs like using MSVC and to cross compile on windows. The COSMOS software has been tested with Qt5.4 and Mingw 4.9.

## MSVC install \(optional\)

Note: you don't have to use MSVC to compile the COSMOS sofware, in fact we recommend that you use MinGW - the default compiler for Qt on windows.

First install Microsoft Visual Studio Express 2013 \(v12.0\), we recommend  
Express because it is free and 2013 because is \(somewhat\) compliant with C++11.   
Do not use an earlier version of MSVC. The 2015 version will be released soon and will have better support for C++11.   
From this link you can download MSVC 2013 express   
and the Debugging Tools for Windows:   
[https://msdn.microsoft.com/en-us/windows/hardware/hh852365](https://msdn.microsoft.com/en-us/windows/hardware/hh852365)

If MSVC was correctly installed Qt Creator will automatically detect MSVC   
and CDB. Make sure you have the msvc2013 64-bit OpenGL version of Qt for 5.4   
\(you may check/install this with the MaintenanceTool.exe inside the Qt   
installation folder \(typically C:\Qt\). Finally just make sure you have a   
Qt kit setup for MSVC.

## Cross-compiling on Windows for ARM

please open the file [install-arm.txt](https://bitbucket.org/cosmos/core/src/master/tutorials/install/linux_arm.md)

