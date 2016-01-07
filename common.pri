# Global COSMOS qmake include file
# Include this file if you want to use COSMOS within Qt Creator
# if you are compiling with a GCC compiler we suggest that you add '-j'
# to the make arguments to speed up the compilation time

# The reason to keep the variable COSMOS_SOURCE_CORE instead of just COSMOS_SOURCE/core
# is that if for any reason the core folder is not named "core" (ex: when downloading the master
# from bitbucket and unzipping it out of place) then the user can still compile the code within core

#--------------------------------------------------------------------
#add COSMOS support to the path
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries # generic path to allow for math/types.h etc.
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/math
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/support
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/thirdparty
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/thirdparty/jpeg

#message("Building the libraries")
LIBRARIESFOLDER = $$OUT_PWD/../../../libraries
#message($$LIBRARIESFOLDER)
LIBS += -L$$LIBRARIESFOLDER -lCosmosSupport -lCosmosMath -lCosmosDeviceCpu -lzlib -ldirent

#--------------------------------------------------------------------
# Mac config
#--------------------------------------------------------------------
macx { #mac
    #message( "Building on MAC OS X" )
}


#--------------------------------------------------------------------
# Unix config
#--------------------------------------------------------------------
unix:!macx {
    #message( "Building on Unix" )
    # add libraries
    LIBS += -pthread #-ljpeg
}


#--------------------------------------------------------------------
# Windows config
#--------------------------------------------------------------------
win32 {
    #message( "Building on Win32" )

    # add libraries for MinGW
    *-g++* {
        #message("Compiler: MinGW")
        LIBS += -lpthread -lwsock32 -lwinmm -lws2_32 -liphlpapi
        QMAKE_CXXFLAGS += -Wall -pedantic -std=c++11 -pthread
    }

    *-msvc* {
        #message("Compiler: MSVC")
        LIBS += -lwsock32 -lwinmm -lws2_32 -liphlpapi

        QMAKE_CXXFLAGS += -W4 -D_CRT_NONSTDC_NO_DEPRECATE

    }

}

#message("")
