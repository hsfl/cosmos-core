Install COSMOS on Mac
=========================

The COSMOS software has been tested with Qt5.4 on MacOX 10.10 
with Xcode clang 64bit

We assume (and strongly suggest) that you have the COSMOS folder in 
/Applications/COSMOS/

Getting Started
--------------
1) install the latest Qt version for Mac from 
http://www.qt.io/download-open-source/

You should have XCode installed already and clang should 
be your main compiler by default, if another compiler
is selected it may have compatibility issues with COSMOS

2) Download COSMOS-core source code

-- using a git client such as SourceTree (recommended) or 
GitHub for Mac. Using a git client is recommended if you are 
planning on contributing to the project 

-- using the command line
$ git clone https://<yourusername>@bitbucket.org/cosmos/core.git

3) setup enviromnet variables

To make your life much, much, much easier you should add a couple of
COSMOS environment variables: COSMOSNODES, COSMOSRESOURCES
so that when you run the COSMOS programs they will know where the
nodes and resources folders are.

To do this create a file 'environment.plist' in 
/Users/<user>/Library/LaunchAgents

This file will setup the environment variables automatically when 
you login into your account. No need to start the terminal or 
manually setting the variables.

Put the following xml code into the file:

<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>Label</key>
  <string>my.startup</string>
  <key>ProgramArguments</key>
  <array>
    <string>sh</string>
    <string>-c</string>
    <string>
    launchctl setenv COSMOSNODES "/Applications/COSMOS/nodes/"
    launchctl setenv COSMOSRESOURCES "/Applications/COSMOS/resources/"
    </string>

  </array>
  <key>RunAtLoad</key>
  <true/>
</dict>
</plist>




Compiling MOST
--------------
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