# Global COSMOS qmake include file
# Include this file if you want to use COSMOS within Qt Creator


# Use these config flags on your project .pro
# PROJECT_BUILD_COSMOS_CORE_LIB_SUPPORT
# PROJECT_BUILD_COSMOS_CORE_LIB_DEVICE
# PROJECT_BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB
# PROJECT_BUILD_COSMOS_CORE_LIB_THIRDPARTY_JPEG

# By default these are on, unless told by the user in the .pro file
# using the PROJECT_BUILD_ ... flags

BUILD_COSMOS_CORE_LIB_SUPPORT           = "ON"
BUILD_COSMOS_CORE_LIB_DEVICE            = "ON"
BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB   = "ON"
BUILD_COSMOS_CORE_LIB_THIRDPARTY_JPEG   = "ON"

# just turn OFF flag if user requires it in .pro file
equals(PROJECT_BUILD_COSMOS_CORE_LIB_SUPPORT, "OFF"){
    BUILD_COSMOS_CORE_LIB_SUPPORT          = "OFF"
}

equals(PROJECT_BUILD_COSMOS_CORE_LIB_DEVICE, "OFF"){
    BUILD_COSMOS_CORE_LIB_DEVICE           = "OFF"
}

equals(PROJECT_BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB, "OFF"){
    BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB  = "OFF"
}

equals(PROJECT_BUILD_COSMOS_CORE_LIB_THIRDPARTY_JPEG, "OFF"){
    BUILD_COSMOS_CORE_LIB_THIRDPARTY_JPEG  = "OFF"
}

message("---------------------------------------------------------------------")

# COSMOS CORE
COSMOS_CORE                        = $$COSMOS/core
COSMOS_CORE_LIB                    = $$COSMOS_CORE/libraries
COSMOS_CORE_LIB_SUPPORT            = $$COSMOS_CORE_LIB/support
COSMOS_CORE_LIB_DEVICE             = $$COSMOS_CORE_LIB/device
COSMOS_CORE_LIB_THIRDPARTY         = $$COSMOS_CORE_LIB/thirdparty
COSMOS_CORE_LIB_THIRDPARTY_ZLIB    = $$COSMOS_CORE_LIB_THIRDPARTY/zlib
COSMOS_CORE_LIB_THIRDPARTY_JPEG    = $$COSMOS_CORE_LIB_THIRDPARTY/jpeg

COSMOS_CORE_PROGRAMS               = $$COSMOS_CORE/programs
COSMOS_CORE_PROGRAMS_AGENTS        = $$COSMOS_CORE_PROGRAMS/agents

# COSMOS TOOLS
COSMOS_TOOLS                       = $$COSMOS/tools
COSMOS_TOOLS_QTSUPPORT             = $$TOOLS/libraries/qtsupport

#INCLUDEPATH += $$TOOLS_QTSUPPORT

# Add COSMOS support
equals(BUILD_COSMOS_CORE_LIB_SUPPORT, "ON"){
    message("building COSMOS_CORE_LIB_SUPPORT")
    INCLUDEPATH     += $$COSMOS_CORE_LIB_SUPPORT
    #SOURCES         += $$files($$COSMOS_CORE_LIB_SUPPORT/*.cpp)
    #HEADERS         += $$files($$COSMOS_CORE_LIB_SUPPORT/*.h)
}

## Add COSMOS device
#equals(BUILD_COSMOS_CORE_LIB_DEVICE, "ON"){
#    message("building COSMOS_CORE_LIB_DEVICE")
#    INCLUDEPATH     += $$COSMOS_CORE_LIB_DEVICE
#    SOURCES         += $$files($$COSMOS_CORE_LIB_DEVICE/*.cpp)
#    HEADERS         += $$files($$COSMOS_CORE_LIB_DEVICE/*.h)

##    SOURCES         -= $$COSMOS_CORE_LIB_DEVICE/kisstnc_lib.cpp
##    SOURCES         -= $$COSMOS_CORE_LIB_DEVICE/kpc9612p_lib.cpp
##    SOURCES         -= $$COSMOS_CORE_LIB_DEVICE/mixwtnc_lib.cpp
#}

## Add COSMOS programs/agents
##INCLUDEPATH     += $$COSMOS_CORE_PROGRAMS_AGENTS
##HEADERS         += $$COSMOS_CORE_PROGRAMS_AGENTS/*.h
##SOURCES         += $$COSMOS_CORE_PROGRAMS_AGENTS/*.cpp

## Add COSMOS thirdparty

#INCLUDEPATH     += $$COSMOS_CORE_LIB_THIRDPARTY

## Add Zlib
#equals(BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB, "ON"){
#    INCLUDEPATH     += $$COSMOS_CORE_LIB_THIRDPARTY_ZLIB
#    SOURCES         += $$files($$COSMOS_CORE_LIB_THIRDPARTY_ZLIB/*.c)
#    HEADERS         += $$files($$COSMOS_CORE_LIB_THIRDPARTY_ZLIB/*.h)
#}

## Add JPEG
#equals(BUILD_COSMOS_CORE_LIB_THIRDPARTY_JPEG, "ON"){
#    INCLUDEPATH     += $$COSMOS_CORE_LIB_THIRDPARTY_JPEG
#    SOURCES         += $$files($$COSMOS_CORE_LIB_THIRDPARTY_JPEG/*.c)
#    HEADERS         += $$files($$COSMOS_CORE_LIB_THIRDPARTY_JPEG/*.h)
#}

##contains( BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB, ON ){

##}

## Add QCustomPlot
#equals(BUILD_QCUSTOMPLOT, "ON"){
#    message("building QCUSTOMPLOT")
#    QCUSTOMPLOT     = $$COSMOS_TOOLS/libraries/thirdparty/qcustomplot
#    INCLUDEPATH     += $$QCUSTOMPLOT
#    SOURCES         += $$QCUSTOMPLOT/qcustomplot.cpp
#    HEADERS         += $$QCUSTOMPLOT/qcustomplot.h
#}


################################################################################
# Windows config
################################################################################
win32 {
    message( "Building on Win32" )

    # add libraries
    #LIBS += -lpthread -lwsock32 -lwinmm -lws2_32 -liphlpapi

}


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
