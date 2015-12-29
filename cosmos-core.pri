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
        INCLUDEPATH     += $$COSMOS_SOURCE_CORE/libraries/thirdparty/dirent
        SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/dirent/dirent.c
        HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/dirent/dirent.h
    }

}

#contains(QMAKE_CC, cl) {
#    # Visual Studio
#    message("Compiler: Visual Studio")
#    #CONFIG += precompile_header

#    win32{
#    INCLUDEPATH     += $$COSMOS_SOURCE_CORE/libraries/thirdparty/dirent
#    SOURCES         += $$files($$COSMOS_SOURCE_CORE/libraries/thirdparty/dirent/*.c)
#    HEADERS         += $$files($$COSMOS_SOURCE_CORE/libraries/thirdparty/dirent/*.h)
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
unix:!macx {
    #message( "Building on Unix" )
    # add libraries
    LIBS += -pthread #-ljpeg
}

#message("")
#message("COSMOS Modules >>")

#--------------------------------------------------------------------
#add COSMOS support to the path
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries # generic path to allow for math/types.h etc.
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/math
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/support


# Add all COSMOS support libraries
contains(MODULES, SUPPORT) {
    message( "Add library: SUPPORT" )
    INCLUDEPATH     += $$COSMOS_SOURCE_CORE/libraries/support
    #SOURCES         += $$files($$COSMOS_SOURCE_CORE/libraries/support/*.cpp)
    #HEADERS         += $$files($$COSMOS_SOURCE_CORE/libraries/support/*.h)
    MODULES += ZLIB
}


# -----------------------------------------------
# Tier 1 libraries,
# these libraries depend on Tier 0 libraries

contains(MODULES, agentlib){
    message( "- support/agentlib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/agentlib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/agentlib.h
    MODULES += socketlib   # agentlib depends on socketlib
    MODULES += sliplib     # and sliplib
    MODULES += elapsedtime # and elapsedtime
    MODULES += jsonlib
    MODULES += timelib
    MODULES += timeutils
    MODULES += stringlib
    MODULES += datalib

}

contains(MODULES, elapsedtime){
    message( "- support/elapsedtime" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/elapsedtime.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/elapsedtime.hpp
}

contains(MODULES, socketlib){
    message( "- support/socketlib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/socketlib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/socketlib.h
    MODULES += mathlib
}

contains(MODULES, jsondef){
    message( "- support/jsondef" )
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/jsondef.h
}

contains(MODULES, jsonlib){
    message( "- support/jsonlib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/jsonlib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/jsonlib.h
    MODULES += convertlib
}

contains(MODULES, timelib){
    message( "- support/timelib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/timelib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/timelib.h
    MODULES += ephemlib
    MODULES += datalib
}

contains(MODULES, datalib){
    message( "- support/datalib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/datalib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/datalib.h
    MODULES += zlib
    MODULES += datadef
    MODULES += jsondef
    MODULES += jsonlib
    MODULES += timelib
}

contains(MODULES, convertlib){
    message( "- support/convertlib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/convertlib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/convertlib.h
    MODULES += geomag
}

contains(MODULES, convertdef){
    message( "- support/convertdef" )
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/convertdef.h
}

contains(MODULES, ephemlib){
    message( "- support/ephemlib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/ephemlib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/ephemlib.h
    MODULES += jpleph
}

contains(MODULES, geomag){
    message( "- support/geomag" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/geomag.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/geomag.h
}

contains(MODULES, physicslib){
    message( "- support/physicslib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/physicslib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/physicslib.h
    MODULES += nrlmsise
}

contains(MODULES, demlib){
    message( "- support/demlib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/demlib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/demlib.h
}

# -----------------------------------------------
# Tier 0 libraries
# -----------------------------------------------
# The following libraries do not depend on other libraries
# These are self contained, they are the building blocks for
# more complext libraries such as agentlib
contains(MODULES, gp_cosmostimeutils){
    message( "- support/gp_cosmostimeutils" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/gp_cosmostimeutils.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/gp_cosmostimeutils.h
}


# -----------------------------------------------
# Tier 0 libraries for Math
contains(MODULES, mathlib){
    message( "- math/mathlib" )
    #TODO: remove this later to force the use of #include "math/mathfile"
    INCLUDEPATH     += $$COSMOS_SOURCE_CORE/libraries/math
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/math/mathlib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/math/mathlib.h
    MODULES += math-vector
    MODULES += math-matrix
    MODULES += math-rotation
    MODULES += math-quaternion
}

contains(MODULES, math-lsfit){
    message( "- math/lsfit" )
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/math/lsfit.h
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/math/lsfit.cpp
}

contains(MODULES, math-matrix){
    message( "- math/matrix" )
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/math/matrix.h
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/math/matrix.cpp
    MODULES += math-vector
}

contains(MODULES, math-vector){
    message( "- math/vector" )
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/math/vector.h
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/math/vector.cpp
}

contains(MODULES, math-rotation){
    message( "- math/rotation" )
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/math/rotation.h
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/math/rotation.cpp
}

contains(MODULES, math-quaternion){
    message( "- math/quaternion" )
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/math/quaternion.h
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/math/quaternion.cpp
}

# -----------------------------------------------
# Tier 0 libraries for physics
message( "" )

contains(MODULES, physics-keplerianorbit){
#    message( "- physics/keplerianorbit" )
#    HEADERS += $$COSMOS_SOURCE_CORE/libraries/physics/keplerianorbit.h
#    SOURCES += $$COSMOS_SOURCE_CORE/libraries/physics/keplerianorbit.cpp
#    MODULES += physics-constants
}

contains(MODULES, physics-constants){
    message( "- physics/constants" )
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/physics/constants.h
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/physics/constants.cpp
}


# -----------------------------------------------
# Tier 0 libraries for physics
message( "- support/" )
contains(MODULES, stringlib){
    message( "- support/stringlib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/stringlib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/stringlib.h
}

contains(MODULES, jpleph){
    message( "- support/jpleph" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/jpleph.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/jpleph.h
}

contains(MODULES, sliplib){
    message( "- support/sliplib" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/sliplib.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/sliplib.h
}

contains(MODULES, timeutils){
    message( "- support/timeutils" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/timeutils.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/timeutils.h
}

contains(MODULES, nrlmsise){
    message( "- support/nrlmsise" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/nrlmsise-00.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/nrlmsise-00.h
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/nrlmsise-00_data.cpp
}

contains(MODULES, print_utils){
    message( "- support/print_utils" )
    SOURCES += $$COSMOS_SOURCE_CORE/libraries/support/print_utils.cpp
    HEADERS += $$COSMOS_SOURCE_CORE/libraries/support/print_utils.h
}



#--------------------------------------------------------------------
# Add COSMOS core devices libraries
include( $$COSMOS_SOURCE_CORE/libraries/device/cosmos-core-devices.pri )



#--------------------------------------------------------------------
# Add COSMOS core thirdparty libraries
INCLUDEPATH     += $$COSMOS_SOURCE_CORE/libraries/thirdparty
#INCLUDEPATH     += $$COSMOS_SOURCE/thirdparty

# Add Zlib
contains(MODULES, zlib){
    message( "- thirdparty/zlib" )
    INCLUDEPATH     += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib

    # note: sometimes including all the files with * creates problems
    #SOURCES         += $$files($$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/*.c)
    #HEADERS         += $$files( $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/*.h)
    #SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/*.c
    #HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/*.h

    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/adler32.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/compress.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/crc32.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/deflate.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/gzclose.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/gzlib.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/gzread.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/gzwrite.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/infback.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/inffast.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/inflate.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/inftrees.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/trees.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/uncompr.c
    SOURCES         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/zutil.c

    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/crc32.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/deflate.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/gzguts.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/inffast.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/inffixed.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/inflate.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/inftrees.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/trees.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/zconf.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/zlib.h
    HEADERS         += $$COSMOS_SOURCE_CORE/libraries/thirdparty/zlib/zutil.h

}

# Add JPEG
contains(MODULES, jpeg){
    message( "- thirdparty/jpeg" )
    INCLUDEPATH     += $$COSMOS_SOURCE_CORE/libraries/thirdparty/jpeg
    SOURCES         += $$files($$COSMOS_SOURCE_CORE/libraries/thirdparty/jpeg/*.c)
    HEADERS         += $$files($$COSMOS_SOURCE_CORE/libraries/thirdparty/jpeg/*.h)
}

# Add Eigen
contains(MODULES, thirdparty-Eigen){
    message( "- thirdparty/Eigen" )

    # We just need to inlcude the parent folder for Eigen
    # then in the code we alwas reference it using
    # #include <Eigen/Eigen>
    #INCLUDEPATH     += $$COSMOS_SOURCE_CORE/libraries/thirdparty
}


message("")
