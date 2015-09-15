# Global COSMOS qmake include file
# Include this file if you want to use COSMOS within Qt Creator
# if you are compiling with a GCC compiler we suggest that you add '-j'
# to the make arguments to speed up the compilation time


#message(" ")
message("cosmos-core.pri >>")
#--------------------------------------------------------------------
# Windows config
#--------------------------------------------------------------------
win32 {
    #message( "Building on Win32" )

    #QMAKE_CXXFLAGS += -std=c++0x -pthread

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
        INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/thirdparty/dirent
        SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/thirdparty/dirent/*.c)
        HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/thirdparty/dirent/*.h)
    }

}

#contains(QMAKE_CC, cl){
#    # Visual Studio
#    message("Compiler: Visual Studio")
#    #CONFIG += precompile_header

#    win32{
#    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/thirdparty/dirent
#    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/thirdparty/dirent/*.c)
#    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/thirdparty/dirent/*.h)
#    }
#    #SOURCES += $$COSMOS_SOURCE_CORE/programs/agents/agent_soh.cpp

#    #LIBS += -lpthread -lwsock32 -lwinmm -lws2_32 -liphlpapi
#    LIBS += -lws2_32 -lwsock32 -liphlpapi

#}



################################################################################
# Mac config
################################################################################
macx { #mac
    #message( "Building on MAC OS X" )
}


################################################################################
# Unix config
################################################################################
unix:!macx{
    #message( "Building on Unix" )
    # add libraries
    LIBS += -pthread #-ljpeg
}

#message("")
#message("COSMOS Modules >>")

#--------------------------------------------------------------------
#add COSMOS support to the path
INCLUDEPATH += $$COSMOS_SOURCE/core/libraries # generic path to allow for math/types.h etc.
INCLUDEPATH += $$COSMOS_SOURCE/core/libraries/math
INCLUDEPATH += $$COSMOS_SOURCE/core/libraries/support


# Add all COSMOS support libraries
contains(MODULES, SUPPORT){
    message( "Add library: SUPPORT" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/support
    #SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/support/*.cpp)
    #HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/support/*.h)
    MODULES += ZLIB
}


# -----------------------------------------------
# Tier 1 libraries,
# these libraries depend on Tier 0 libraries

contains(MODULES, agentlib){
    message( "- support/agentlib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/agentlib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/agentlib.h
    MODULES += socketlib   # agentlib depends on socketlib
    MODULES += sliplib     # and sliplib
    MODULES += elapsedtime # and elapsedtime
    MODULES += jsonlib
    MODULES += timelib
    MODULES += stringlib
    MODULES += datalib

}

contains(MODULES, elapsedtime){
    message( "- support/elapsedtime" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/elapsedtime.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/elapsedtime.hpp
}

contains(MODULES, socketlib){
    message( "- support/socketlib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/socketlib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/socketlib.h
    MODULES += mathlib
}

contains(MODULES, jsondef){
    message( "- support/jsondef" )
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/jsondef.h
}

contains(MODULES, jsonlib){
    message( "- support/jsonlib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/jsonlib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/jsonlib.h
    MODULES += convertlib
}

contains(MODULES, timelib){
    message( "- support/timelib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/timelib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/timelib.h
    MODULES += ephemlib
    MODULES += datalib
}

contains(MODULES, datalib){
    message( "- support/datalib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/datalib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/datalib.h
    MODULES += zlib
    MODULES += datadef
    MODULES += jsondef
    MODULES += jsonlib
    MODULES += timelib
}

contains(MODULES, convertlib){
    message( "- support/convertlib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/convertlib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/convertlib.h
    MODULES += geomag
}

contains(MODULES, convertdef){
    message( "- support/convertdef" )
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/convertdef.h
}

contains(MODULES, ephemlib){
    message( "- support/ephemlib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/ephemlib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/ephemlib.h
    MODULES += jpleph
}

contains(MODULES, geomag){
    message( "- support/geomag" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/geomag.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/geomag.h
}

contains(MODULES, physicslib){
    message( "- support/physicslib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/physicslib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/physicslib.h
    MODULES += nrlmsise
}

contains(MODULES, demlib){
    message( "- support/demlib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/demlib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/demlib.h
}

# -----------------------------------------------
# Tier 0 libraries
# The following libraries do not depend on other libraries
# These are self contained, they are the building blocks for
# more complext libraries such as agentlib
contains(MODULES, gp_cosmostimeutils){
    message( "- support/gp_cosmostimeutils" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/gp_cosmostimeutils.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/gp_cosmostimeutils.h
}

contains(MODULES, mathlib){
    message( "- math/mathlib" )
    #TODO: remove this later to force the use of #include "math/mathfile"
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/math
    SOURCES += $$COSMOS_SOURCE/core/libraries/math/mathlib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/math/mathlib.h
    MODULES += math-vector
    MODULES += math-matrix
    MODULES += math-rotation
    MODULES += math-quaternion
}

contains(MODULES, math-lsfit){
    message( "- math/lsfit" )
    HEADERS += $$COSMOS_SOURCE/core/libraries/math/lsfit.h
    SOURCES += $$COSMOS_SOURCE/core/libraries/math/lsfit.cpp
}

contains(MODULES, math-matrix){
    message( "- math/matrix" )
    HEADERS += $$COSMOS_SOURCE/core/libraries/math/matrix.h
    SOURCES += $$COSMOS_SOURCE/core/libraries/math/matrix.cpp
    MODULES += math-vector
}

contains(MODULES, math-vector){
    message( "- math/vector" )
    HEADERS += $$COSMOS_SOURCE/core/libraries/math/vector.h
    SOURCES += $$COSMOS_SOURCE/core/libraries/math/vector.cpp
}

contains(MODULES, math-rotation){
    message( "- math/rotation" )
    HEADERS += $$COSMOS_SOURCE/core/libraries/math/rotation.h
    SOURCES += $$COSMOS_SOURCE/core/libraries/math/rotation.cpp
}

contains(MODULES, math-quaternion){
    message( "- math/quaternion" )
    HEADERS += $$COSMOS_SOURCE/core/libraries/math/quaternion.h
    SOURCES += $$COSMOS_SOURCE/core/libraries/math/quaternion.cpp
}


contains(MODULES, stringlib){
    message( "- support/stringlib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/stringlib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/stringlib.h
}

contains(MODULES, jpleph){
    message( "- support/jpleph" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/jpleph.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/jpleph.h
}

contains(MODULES, sliplib){
    message( "- support/sliplib" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/sliplib.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/sliplib.h
}

contains(MODULES, timeutils){
    message( "- support/timeutils" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/timeutils.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/timeutils.h
}

contains(MODULES, nrlmsise){
    message( "- support/nrlmsise" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/nrlmsise-00.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/nrlmsise-00.h
}

contains(MODULES, print_utils){
    message( "- support/print_utils" )
    SOURCES += $$COSMOS_SOURCE/core/libraries/support/print_utils.cpp
    HEADERS += $$COSMOS_SOURCE/core/libraries/support/print_utils.h
}


#--------------------------------------------------------------------
# Add COSMOS device
contains(MODULES, device){
    message( "- device" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/*.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/*.h)
}

contains(MODULES, astrodev_lib){
    message( "- device/astrodev_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/astrodev_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/astrodev_lib.h)
    MODULES         +=  cssl_lib
}


contains(MODULES, microstrain_lib){
    message( "- device/microstrain_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/microstrain_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/microstrain_lib.h)
}

contains(MODULES, serial){
    message( "- device/serial" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/serial.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/serial.h)
}

contains(MODULES, oemv_lib){
    message( "- device/oemv_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/oemv_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/oemv_lib.h)
}

contains(MODULES, vn100_lib){
    message( "- device/vn100_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/vn100_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/vn100_lib.h)

    # dependencies
    MODULES += cssl_lib
}

contains(MODULES, vmt35_lib){
    message( "- device/vmt35_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/vmt35_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/vmt35_lib.h)
}

contains(MODULES, gige_lib){
    message( "- device/gige_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/gige_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/gige_lib.h)
}

contains(MODULES, mixwtnc_lib){
    message( "- device/mixwtnc_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/mixwtnc_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/mixwtnc_lib.h)
}

contains(MODULES, sinclair_lib){
    message( "- device/sinclair_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/sinclair_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/sinclair_lib.h)
}

contains(MODULES, sinclair_lib){
    message( "Add library: sinclair_lib" )
    INCLUDEPATH     += $$COSMOS/core/libraries/device
    SOURCES         += $$files($$COSMOS/core/libraries/device/sinclair_lib.cpp)
    HEADERS         += $$files($$COSMOS/core/libraries/device/sinclair_lib.h)
}

contains(MODULES, cssl_lib){
    message( "- device/cssl_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/cssl_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/cssl_lib.h)
}

#--------------------------------------------------------------------
# Add COSMOS core thirdparty libraries
INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/thirdparty
#INCLUDEPATH     += $$COSMOS_SOURCE/thirdparty

# Add Zlib
contains(MODULES, zlib){
    message( "- thirdparty/zlib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib
    #SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/thirdparty/zlib/*.c)
    #HEADERS         += $$files( $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/*.h)
    #SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/*.c
    #HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/*.h
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/adler32.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/compress.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/crc32.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/deflate.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/gzclose.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/gzlib.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/gzread.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/gzwrite.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/infback.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/inffast.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/inflate.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/inftrees.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/trees.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/uncompr.c
    SOURCES         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/zutil.c

    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/crc32.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/deflate.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/gzguts.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/inffast.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/inffixed.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/inflate.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/inftrees.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/trees.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/zconf.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/zlib.h
    HEADERS         += $$COSMOS_SOURCE/core/libraries/thirdparty/zlib/zutil.h

}

# Add JPEG
contains(MODULES, jpeg){
    message( "- thirdparty/jpeg" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/thirdparty/jpeg
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/thirdparty/jpeg/*.c)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/thirdparty/jpeg/*.h)
}

message("")
