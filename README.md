# COSMOS core 1.0.2 (beta)

**Please note that this software and its documentation are a work in progress. If you run into problems or find that there is missing documentation for something you're looking for, please let us know.**

* [Get started with the COSMOS Documentation page](https://hsfl.github.io/cosmos-docs/)
* [COSMOS Core Doxygen](https://hsfl.github.io/cosmos-core)

COSMOS/core consists of the basic elements of the COSMOS project and includes essential libraries and programs that make up COSMOS. The API documentation includes descriptions of the basic support functions necessary to make COSMOS/core work, as well as the most useful agent and support programs. COSMOS/core is broadly divided into \ref programs and \ref libraries. 
 
For the latest information on COSMOS, refer to the website:

[http://www.cosmos-project.org/](http://www.cosmos-project.org/)

The COSMOS/core API documentation is available at:

[http://cosmos-project.org/docs/core/current/](http://cosmos-project.org/docs/core/current/)

The documentation includes instructions on how to compile the code in different architectures, the API structure, tutorials, and descriptions of most classes and functions. 

*Note*: Remember to update the code regularly (`git pull`). 

## Quick-start ##

This section will walk through building/installing COSMOS/core and running two agents that talk to each other. These instructions assume that you are in the root directory of this repository, `cosmos-core`. Make sure you have all the tools required to build COSMOS, including CMake and Git.

### OPTION 1: Command Line Interface

Open a terminal and enter the commands:

```
cd build
cmake ..
make install
```
You can make the code compile faster by using multiple cores. For example, to compile with 8 cores, use:
```
make -j8 install
```

Once the build has finished, executable files will be saved in `$HOME/cosmos/bin`. You can add these executables to your path by running the command (on Linux):
```
echo "export PATH=$PATH:$HOME/cosmos/bin" >> ~/.bashrc
```

Restart the terminal for this change to take effect.

Now, you can run the agents. Enter `agent_001` in your current terminal window, then open another terminal window and enter `agent_002`.

### OPTION 2: Qt Creator

1) Make sure you are using the latest version of Qt and start Qt Creator
2) Open the CMakeLists.txt file (File -> Open File or Project ->
Open project : `cosmos-core/CMakeLists.txt`)
3) Select a build directory (or use the default)
4) Optional step to compile the code faster. Please read [Note 1](#note-1) if
you're interested. (For Linux check [Note 2](#note-2))
5) Build the project : `CTRL + B` 
6) Check your `cosmos` folder (ex: `C:/cosmos`) to see the files that
were installed 
7) Select `agent_001`. Click on the `Project` button (the computer icon on
left side of Qt Creator) and select `agent_001`.
8) Run `agent_001`. Click on the `Run` button (the big green icon on left
side of Qt Creator) or press `CTRL + R`.
9) Select `agent_002`. 
10) Run `agent_002`. If running `agent_002` stops the run of `agent_001` see [Note 2](#note-2). 

### After completing either of the options above, the two COSMOS agents should be talking to each other: 
![agent_001_agent_002.PNG](https://bitbucket.org/repo/EpA5jo/images/402568663-agent_001_agent_002.PNG)

Alternatively, you can use the `cosmos-core.pro` file (qmake) or `cosmos-core.qbs` (QBS), but at the moment these are just experimental build options.

* Open Qt Creator and open the project file `cosmos-core.pro` 
* Configure your project (ex: on Windows it will be something like
'Desktop Qt 5.5.1 MinGW 32 bit')
* Optional step to compile the code faster. Please read [Note 1](#note-1) if
interested. (For Linux check [Note 2](#note-2))
* Build the project by pressing the `Build` button (the hammer icon on
left side of Qt Creator), or press `CTRL + B`

[//]: # (BROKEN LINK For more detailed instructions to get started with COSMOS please read the [core/docs/README.md]&#40;https://bitbucket.org/cosmos-project/core/src/master/docs/&#41; inside the documentation folder.)

### Note 1 ###
This is only valid for MinGW and GCC compilers (msvc uses Jom to handle multiple cores). To compile the code faster using all the cores on your machine go to Qt Creator -> Projects (icon on left side bar) -> Build Steps -> Make : Details (expand the icon) on arguments add "-j4" or whatever number of cores that your computer supports. In some cases you may have to add a space in between "-j 4". In some cases it is also possible to just add '-j' and the compiler will automatically use as many processes it can to compile. This approach works well on Windows but in Linux it seems to freeze the computer because it starts more threads than cores. Use it with caution. If you really need super compilation times then install MSVC 2013 or above. See the results and make your decision.

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

### Note 2 ###
Qt creator on Linux and Windows has an option to close the programs automatically when running another program. You will need to disable this behavior to run the two agents at the same time. Go to Tools > Options > Build and Run > General.  Change `Stop applications before building` to `None`.
