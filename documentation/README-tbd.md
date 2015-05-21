Overview
--------
  License
  Installing
  COSMOS Resources
  Contributions
  Acknowledgements





System Requirements
------------------- 

* MacOSX 10.4 (Tiger) or higher
* Intel or PowerPC processor
* 512 MB RAM
* Up to 2 GB available hard disk space
* 1024 x 768 graphic device with 256 colors (higher resolution 
recommended)
* QtStk (Qt current for this build: 4.8.6)

Please make sure you have enough free memory in the temporary directory 
on your system, and please ensure that read, write and run access 
rights have been granted. Close all other programs before starting the 
installation process.






QT 4.8.x INSTALLATION (X11 Platforms)
________________________________________

1. Download the latest Qt version. Don't worry about the version for now ... we're just getting setup. We'll install Qt 4.8.6. in a few steps further.
	http://download.qt-project.org/official_releases/online_installers/qt-opensource-linux-x64-1.5.0-2-online.run
2. Change permissions on the file you just downloaded and run it
	$ sudo chmod +x qt-opensource-linux-x64-1.5.0-2-online.run
	$ ./qt-opensource-linux-x64-1.5.0-2-online.run
3. Download the Qt libraries 4.8.x for Linux/X11
	http://download.qt-project.org/official_releases/qt/4.8
4. Install the Qt for X11 required development packages:
	$ sudo apt-get install libfontconfig1-dev libfreetype6-dev libx11-dev libxcursor-dev libxext-dev libxfixes-dev libxft-dev libxi-dev libxrandr-dev libxrender-dev libglu1-mesa-dev
5. Extract and install the Qt 4.8.x libraries
	$ tar -zxvf /tmp/qt-everywhere-opensource-src-4.8.x.tar.gz
	$ cd  /tmp/qt-everywhere-opensource-src-4.8.x
	$ ./configure
		-When prompted, type 'o' to install the open source edition
		-Type 'yes' to accept the license
		-If there is a pervious configuration, recongifure by running 'make confclean' and 'configure'
	$ make
	$ make install
		-If you receive permission errors, try "sudo make install"
6.  Extend the the environment variables in order to use Qt
	-In .profile (if your shell is bash, ksh, zsh, or sh) add the following lines:
		$ PATH=/usr/local/Trolltech/Qt-4.8.x/bin:$PATH
		$ export PATH
	-If your shell is csh or tcsh, add the following line in .login
		$ setenv PATH /usr/local/Trolltech/Qt-4.8.5/bin:$PATH
7. Configure the Build & Run settings in QtCreator
	-Open QtCreator (it comes with Qt)
	-Go to Tools -> Options -> Build & Run
	-Under Qt Versions, click the 'Add' button and browse to /usr/local/TrollTech/Qt-4.8.x/bin/qmake
	-Under Kits, click the 'Add' button and change the Qt version to Qt 4.8.x (Qt-4.8.x)
	-Confirm that the Compiler parameter is set to either GCC 32bit or 64bit depending on your system
	-Click the 'Make Default' button and press 'Apply'
8. Open .pro file in QtCreator
	-Open the details for the Desktop Build
	-Check shadow build
	-Check either the debug or release (or both) configurations
9. Add any additional build steps if needed
	-Click on the Projects icon (on the left)
	-Build Steps -> Add Build Step -> Make
	-Enter make arguments
10. Hit Run!

INSTALLATION (WIN32)
----------------------------------------------------------------------

1) Download the latest Qt STK Suite 
2) Download the source code for most: 
	https://wwww.hsfl.hawaii.edu/svn/cosmos/most 
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


INSTALLATION (MAC OS X)
----------------------------------------------------------------------

1) Download the latest Qt STK Suite 
2) Download the source code for most: 
	https://wwww.hsfl.hawaii.edu/svn/cosmos/most 
3) Open the MOST.pro file in QT Creator
4) Use shadow build and select to build with the latest Qt version 
4) Switch to project mode (^5)
4) On the Build Settings
	General:
	- Confirm the Qt version
	- Make sure that Shadow Build is checked

	Build Steps
	- Add Build step
	- Select Make
	- Make arguments: 'install'
5) On the Run Settings
	- Arguments: ../MOST/buses/hawaiisat-1
6) Hit Run!

Old Note (not valid anymore): this installation does not need a 
make install in the project arguments because the 
files are installed automatically in the bundle


INSTALLATION (LINUX)
----------------------------------------------------------------------

NOTE: if you are using this source code from a secondary drive formated in NTFS you must enable execution rights oin that drive. Ex.: Ubuntu 12.04, install Storage Device Manager and set defaults on the options.

1) Download the latest Qt STK Suite 
2) Download the source code for most: https://wwww.hsfl.hawaii.edu/svn/cosmos/most 
3) Open the MOST.pro file in QT Creator
4) On the Target Setup dialog
	- open the details for the Desktop Build
	- Check the Shadow Build
	- Check either the debug or release (or both) configurations
5) IMPORTANT! 
	- Click on the Projects icon (on the left)
	- Build Steps -> Add Build Step -> Make
	- Make Arguments: add 'install'
	- This installs the necessary files to run MOST from the shadow build directory
5) Hit Run!




Directory Structure
-------------------


Running Example Applications
----------------------------


Building COSMOS from Source
---------------------------


Developing COSMOS Applications
------------------------------


FAQs
----
Is COSMOS Really Free for Any User?



Getting Involved
----------------------------------------------------------------------



Reporting Bugs & Issues
----------------------------------------------------------------------


Using SVN
----------------------------------------------------------------------
$ svn up  - to update the local repository
$ svn cleanup 
$ svn commit -m "add: filex." - Send changes from your working copy 
			      - to the repository.



User Support
----------------------------------------------------------------------


Major Releases
----------------------------------------------------------------------

Last update February 2011
- February 10, 2012 / MOST v0.5.1 
- October 14, 2011 / MOST v0.5.0  


Want to Know More?
----------------------------------------------------------------------

