# Global COSMOS qmake include file
# Include this file if you want to use COSMOS within Qt Creator


# Use these config flags on your project .pro
# These are the default
# By default these are on, unless told by the user in the .pro file
# using the PROJECT_BUILD_ ... flags

#BUILD_COSMOS_CORE_LIB_SUPPORT           = "ON"
#BUILD_COSMOS_CORE_LIB_DEVICE            = "ON"
#BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB   = "ON"
#BUILD_COSMOS_CORE_LIB_THIRDPARTY_JPEG   = "ON"

# just turn OFF flag if user requires it in .pro file
#equals(PROJECT_BUILD_COSMOS_CORE_LIB_SUPPORT, "OFF"){
#    BUILD_COSMOS_CORE_LIB_SUPPORT          = "OFF"
#}

#equals(PROJECT_BUILD_COSMOS_CORE_LIB_DEVICE, "OFF"){
#    BUILD_COSMOS_CORE_LIB_DEVICE           = "OFF"
#}

#equals(PROJECT_BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB, "OFF"){
#    BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB  = "OFF"
#}

#equals(PROJECT_BUILD_COSMOS_CORE_LIB_THIRDPARTY_JPEG, "OFF"){
#    BUILD_COSMOS_CORE_LIB_THIRDPARTY_JPEG  = "OFF"
#}

message("---------------------------------------------------------------------")

# Add COSMOS support
contains(MODULES, SUPPORT){
    message( "Add library: SUPPORT" )
    INCLUDEPATH     += $$COSMOS/core/libraries/support
    SOURCES         += $$files($$COSMOS/core/libraries/support/*.cpp)
    HEADERS         += $$files($$COSMOS/core/libraries/support/*.h)
}

# Add COSMOS device
contains(MODULES, DEVICE){
    message( "Add library: DEVICES" )
    INCLUDEPATH     += $$COSMOS/core/libraries/device
#    SOURCES         += $$files($$COSMOS/core/libraries/device/*.cpp)
#    HEADERS         += $$files($$COSMOS/core/libraries/device/*.h)
}

# Add COSMOS thirdparty
INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty
INCLUDEPATH     += $$COSMOS/thirdparty

# Add Zlib
contains(MODULES, ZLIB){
    INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty/zlib
    SOURCES         += $$files($$COSMOS/core/libraries/thirdparty/zlib/*.c)
    HEADERS         += $$files($$COSMOS/core/libraries/thirdparty/zlib/*.h)
}

# Add JPEG
contains(MODULES, JPEG){
    INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty/jpeg
    SOURCES         += $$files($$COSMOS/core/libraries/thirdparty/jpeg/*.c)
    HEADERS         += $$files($$COSMOS/core/libraries/thirdparty/jpeg/*.h)
}


################################################################################
# Windows config
################################################################################
win32 {
    message( "Building on Win32" )

    # add libraries
    LIBS += -lwsock32 -lwinmm -lws2_32 -liphlpapi #-lpthread

    msvc{
        message("MSVC compiler")

        # include dirent for MSVC
        INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty/dirent
        SOURCES         += $$files($$COSMOS/core/libraries/thirdparty/dirent/*.c)
        HEADERS         += $$files($$COSMOS/core/libraries/thirdparty/dirent/*.h)
    }

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
