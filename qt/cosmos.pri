# Global COSMOS qmake include file
# Include this file if you want to use COSMOS within Qt Creator
# if you are compiling with a GCC compiler we suggest that you add '-j'
# to the make arguments to speed up the compilation time



message("------------------------------------------------------------")
################################################################################
# Windows config
################################################################################
win32 {
    message( "Building on Win32" )

    #QMAKE_CXXFLAGS += -std=c++0x -pthread

    # add libraries
    LIBS += -lwsock32 -lwinmm -lws2_32 -liphlpapi #-lpthread

    msvc{
        message("Compiler: MSVC")

        # available modules for MSVC 2013
        #MODULES += elapsedtime
        #MODULES += timeutils
        #MODULES += socketlib
        #MODULES += agentlib
        #MODULES += jsonlib
        #MODULES += mathlib
        #MODULES += timelib
        #MODULES += datalib
        #MODULES += convertlib
        #MODULES += convertdef
        #MODULES += stringlib
        #MODULES += jpleph
        #MODULES += ephemlib
        #MODULES += geomag
        #MODULES += sliplib
        #MODULES += zlib


        # include dirent for MSVC
        INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty/dirent
        SOURCES         += $$files($$COSMOS/core/libraries/thirdparty/dirent/*.c)
        HEADERS         += $$files($$COSMOS/core/libraries/thirdparty/dirent/*.h)
    }

}

#contains(QMAKE_CC, cl){
#    # Visual Studio
#    message("Compiler: Visual Studio")
#    #CONFIG += precompile_header

#    win32{
#    INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty/dirent
#    SOURCES         += $$files($$COSMOS/core/libraries/thirdparty/dirent/*.c)
#    HEADERS         += $$files($$COSMOS/core/libraries/thirdparty/dirent/*.h)
#    }
#    #SOURCES += $$COSMOS_CORE/programs/agents/agent_soh.cpp

#    #LIBS += -lpthread -lwsock32 -lwinmm -lws2_32 -liphlpapi
#    LIBS += -lws2_32 -lwsock32 -liphlpapi

#}



################################################################################
# Mac config
################################################################################
macx { #mac
    message( "Building on MAC OS X" )
}


################################################################################
# Unix config
################################################################################
unix:!macx{
    message( "Building on Unix" )
    # add libraries
    LIBS += -pthread #-ljpeg
}

message("")
message("COSMOS Modules >>")

#--------------------------------------------------------------------
#add COSMOS support to the path
INCLUDEPATH     += $$COSMOS/core/libraries/support

# Add all COSMOS support libraries
contains(MODULES, SUPPORT){
    message( "Add library: SUPPORT" )
    INCLUDEPATH     += $$COSMOS/core/libraries/support
    #SOURCES         += $$files($$COSMOS/core/libraries/support/*.cpp)
    #HEADERS         += $$files($$COSMOS/core/libraries/support/*.h)
    MODULES += ZLIB
}

contains(MODULES, elapsedtime){
    message( "Add COSMOS module: elapsedtime" )
    SOURCES     += $$COSMOS/core/libraries/support/elapsedtime.cpp
    HEADERS     += $$COSMOS/core/libraries/support/elapsedtime.hpp
}

contains(MODULES, timeutils){
    message( "Add COSMOS module: timeutils" )
    SOURCES += $$COSMOS/core/libraries/support/timeutils.cpp
    HEADERS += $$COSMOS/core/libraries/support/timeutils.h
}

contains(MODULES, socketlib){
    message( "Add COSMOS module: socketlib" )
    SOURCES += $$COSMOS/core/libraries/support/socketlib.cpp
    HEADERS += $$COSMOS/core/libraries/support/socketlib.h
}

contains(MODULES, agentlib){
    message( "Add COSMOS module: agentlib" )
    SOURCES += $$COSMOS/core/libraries/support/agentlib.cpp
    HEADERS += $$COSMOS/core/libraries/support/agentlib.h
}

contains(MODULES, jsonlib){
    message( "Add COSMOS module: jsonlib" )
    SOURCES += $$COSMOS/core/libraries/support/jsonlib.cpp
    HEADERS += $$COSMOS/core/libraries/support/jsonlib.h
}

contains(MODULES, mathlib){
    message( "Add COSMOS module: mathlib" )
    SOURCES += $$COSMOS/core/libraries/support/mathlib.cpp
    HEADERS += $$COSMOS/core/libraries/support/mathlib.h
}

contains(MODULES, timelib){
    message( "Add COSMOS module: timelib" )
    SOURCES += $$COSMOS/core/libraries/support/timelib.cpp
    HEADERS += $$COSMOS/core/libraries/support/timelib.h
}

contains(MODULES, datalib){
    message( "Add COSMOS module: datalib" )
    SOURCES += $$COSMOS/core/libraries/support/datalib.cpp
    HEADERS += $$COSMOS/core/libraries/support/datalib.h
}

contains(MODULES, convertlib){
    message( "Add COSMOS module: convertlib" )
    SOURCES += $$COSMOS/core/libraries/support/convertlib.cpp
    HEADERS += $$COSMOS/core/libraries/support/convertlib.h
}

contains(MODULES, convertdef){
    message( "Add COSMOS module: convertdef" )
    HEADERS += $$COSMOS/core/libraries/support/convertdef.h
}

contains(MODULES, stringlib){
    message( "Add COSMOS module: stringlib" )
    SOURCES         += $$COSMOS/core/libraries/support/stringlib.cpp
    HEADERS         += $$COSMOS/core/libraries/support/stringlib.h
}

contains(MODULES, jpleph){
    message( "Add COSMOS module: jpleph" )
    SOURCES         += $$COSMOS/core/libraries/support/jpleph.cpp
    HEADERS         += $$COSMOS/core/libraries/support/jpleph.h
}

contains(MODULES, ephemlib){
    message( "Add COSMOS module: ephemlib" )
    SOURCES         += $$COSMOS/core/libraries/support/ephemlib.cpp
    HEADERS         += $$COSMOS/core/libraries/support/ephemlib.h
}

contains(MODULES, geomag){
    message( "Add COSMOS module: geomag" )
    SOURCES         += $$COSMOS/core/libraries/support/geomag.cpp
    HEADERS         += $$COSMOS/core/libraries/support/geomag.h
}

contains(MODULES, sliplib){
    message( "Add COSMOS module: sliplib" )
    SOURCES         += $$COSMOS/core/libraries/support/sliplib.cpp
    HEADERS         += $$COSMOS/core/libraries/support/sliplib.h
}




#--------------------------------------------------------------------
# Add COSMOS device
contains(MODULES, DEVICE){
    message( "Add library: DEVICES" )
    INCLUDEPATH     += $$COSMOS/core/libraries/device
    SOURCES         += $$files($$COSMOS/core/libraries/device/*.cpp)
    HEADERS         += $$files($$COSMOS/core/libraries/device/*.h)
}



#--------------------------------------------------------------------
# Add COSMOS thirdparty
INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty
INCLUDEPATH     += $$COSMOS/thirdparty

# Add Zlib
contains(MODULES, zlib){
    message( "Add thirdparty library: zlib" )
    INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty/zlib
    SOURCES         += $$files($$COSMOS/core/libraries/thirdparty/zlib/*.c)
    HEADERS         += $$files($$COSMOS/core/libraries/thirdparty/zlib/*.h)
}

# Add JPEG
contains(MODULES, jpeg){
    message( "Add thirdparty library: jpeg" )
    INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty/jpeg
    SOURCES         += $$files($$COSMOS/core/libraries/thirdparty/jpeg/*.c)
    HEADERS         += $$files($$COSMOS/core/libraries/thirdparty/jpeg/*.h)
}

