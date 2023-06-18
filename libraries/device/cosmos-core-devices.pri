# Global COSMOS qmake include file
# Include this file if you want to use COSMOS within Qt Creator
# if you are compiling with a GCC compiler we suggest that you add '-j'
# to the make arguments to speed up the compilation time


#message(" ")
message("cosmos-devices.pri >>")
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
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/astrodev/astrodev_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/astrodev/astrodev_lib.h)
    MODULES         +=  cssl_lib
}

contains(MODULES, serial){
    message( "- device/serial" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/serial.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/serial.h)
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

contains(MODULES, cssl_lib){
    message( "- device/cssl_lib" )
    INCLUDEPATH     += $$COSMOS_SOURCE/core/libraries/device
    SOURCES         += $$files($$COSMOS_SOURCE/core/libraries/device/cssl_lib.cpp)
    HEADERS         += $$files($$COSMOS_SOURCE/core/libraries/device/cssl_lib.h)
}

HEADERS += \
    $$PWD/cpu/devicecpu.h

SOURCES += \
    $$PWD/cpu/devicecpu.cpp
