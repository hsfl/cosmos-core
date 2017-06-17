# COSMOS core 1.0.2 (beta)

**Please note that the software and its documentation are work in
progress. If you find problems or no documentation for something
you're looking for please let us know.**

The COSMOS/core is the basic element of the COSMOS project with the
essential libraries and programs. The api documentation includes the
descriptions of the basic support functions necessary to make 
COSMOS/core work, as well as the most useful agent and support programs. The
cosmos core is broadly divided into *programs* and *libraries*. 
 
If you haven't yet downloaded or setup the COSMOS software please go to the [COSMOS/core wiki](https://bitbucket.org/cosmos/core/wiki/Home). For the
latest information on COSMOS refer to the website:
[http://www.cosmos-project.org/](http://www.cosmos-project.org/). 
The COSMOS/core api documentation is available on
[http://cosmos-project.org/docs/core/current/](http://cosmos-project.org/docs/core/current/) 
with instructions on the api structure and the description
for most classes and functions. 

Note: remember to update the code
regularly (git pull). 

To get started with COSMOS you will run two COSMOS agents that will talk to each
other. 

**1) Using Qt Creator (cmake)**

1. Start Qt Creator. Note: Make sure you are using the latest version of Qt Creator (use the maintenance tool to upgrade if needed).
2. Open the CMakeLists.txt file (File -> Open File or Project ->
Open project : cosmos/source/core/CMakeLists.txt)
3. Select a build directory (or use the default)
4. Optional step to compile the code faster. Please read note 1 if
you're interested. (For Linux check note 2)
5. Build the project : ctrl + b
6. Check your "cosmos" folder (ex: C:\cosmos) to see the files that
were installed
7. Select agent_001. Click on the "Project" button (computer icon on
left side of Qt Creator) and select agent_001
8. Run agent_001. Click on the "Run" button (big green icon on left
side of Qt Creator) or press 'ctrl+r'
9. Select agent_002. 
10. Run agent_002. If running agent_002 stops the run of agent_001 see Note 2. 

The two COSMOS agents should be talking to each other at this point. 
![agent_001_agent_002.PNG](https://bitbucket.org/repo/EpA5jo/images/402568663-agent_001_agent_002.PNG)

**using qmake or qbs**

Alternatively you can use the cosmos-core.pro file (qmake) or
cosmos-core.qbs (QBS) but these are just experimental build options
for the moment.

1. Open Qt Creator and open the project file 'cosmos-core.pro' 
2. Configure your project (ex: on Windows it will be something like
`Desktop Qt 5.5.1 MinGW 32 bit')
3. Optional step to compile the code faster. Please read note 1 if
interested. (For Linux check note 2)
4. Build the project by pressing the "Build" button (hammer icon on
left side of Qt Creator), or press 'ctrl+b'

For more detailed instructions to get started with COSMOS
please read the
[core/docs/README.md](https://bitbucket.org/cosmos/core/src/master/docs/README.md)
inside the 
documentation folder.


**2) Using the terminal (cmake) **

Open a terminal and type

```
#!shell

cd cosmos/source/core/build
cmake ..
make
```

you can make the code compile faster by using multiple cores.
This example shows how to use 8 cores:
```
#!shell

make -j8
```


## Note 1 ##
This is only valid for MinGW and GCC compilers (msvc uses Jom to
handle multiple cores). 

To compile the code faster using all the cores
on your machine go to 

Qt Creator -> Projects (icon on left side bar)
-> Build Steps -> Make : Details (expand the icon) 

on arguments add
"-j4" or whatever number of cores that your computer supports. 

In some
cases you may have to add a space in between "-j 4". In some cases it
is also possible to just add '-j' and the compiler will automatically
use as many processes it can to compile. This approach works well on
Windows but in Linux it seems to freeze the computer because it starts
more threads than cores. Use it with caution. If you really need super
compilation times then install MSVC 2013 or above. See the results and
make your decision.

Compilation tests from cosmos-core.pro using a Win7 with Qt 5.5.1
MinGW 32 bit, AMD FX(tm)-8120 Eight Core Processor 3.11 GHz, 16 GB
Ram, 64 bit OS

Kit                             | Build Step                 | Cores used  | Compile time  | 
------------------------------: | :------------------------: | ----------: | :------------ | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe (default) | 1           | 3m 30s        | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe -j4       | 4           | 1m 16s        | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe -j8       | 8           | 1m 4s         | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe -j9       | 8           | 1m 4s         | 
Desktop Qt 5.5.1 MinGW 32bit    | mingw32-make.exe -j        | 8           | 58 s          | 
Desktop Qt 5.5.1 MinGW 32bit    | jom.exe (custom step)      | 8           | 1m 16s        | 
Desktop Qt 5.5.1 MSVC2013 64bit | jom.exe (default)          | 8           | 30s           |

## Note 2 ##
Qt creator on Linux and Windows has an option to close the programs automatically
when running another program. You will need to disable this behavior
to run the two agents at the same time. Go to Tools > Options > Build
and Run > General.  Change 'Stop applications before building:' to
None.

##Troubleshooting
**Cmake was not found**: This means that Qt was not able to find Cmake. To fix it (assuming that Cmake is installed), in your Qt window go to Tools -> Options -> Build and Run -> Cmake.  If nothing is listed under Auto-detected then click Add, give it a name, and under path put C:\Program Files\CMake\bin\cmake.exe (if you installed Cmake into a different folder or you are using a different OS then change the path appropriately).

**Path not found**: This means that the path the built programs wasn't made. To fix, open a terminal and enter the following commands for your respective OS.

**Linux**:
```
#!shell

cd
vi .profile
```
this should open up a text file. Using the arrow keys, go to the bottom of the file and add a new line (hit "**Esc**" to switch to typing mode):

PATH="HOME/cosmos/bin:$PATH"

Then hit "**Esc**" (to exit typing mode) **: x** "**Enter**" (to exit). Once you are outside of the file, type in
```
#!shell

source .profile
```
to check that the path was updated, use
```
#!shell

echo $PATH
```

**Windows**

**path=%PATH%;C:\cosmos\bin**

**path=%PATH%;C:\Qt\Tools\mingw530_32\bin**

These two commands will link to the bin in your cosmos folder and the compiler that you use in Qt respectively.
If your cosmos or Qt folder is in a different location, or if you are using a different compiler in Qt, change the entered path appropriately.