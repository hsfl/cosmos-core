TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11 # QT5 way of adding c++11 support

# User defined folders
COSMOS                  = $$PWD/../../..
COSMOS_PROJECTS         = $$COSMOS/projects
COSMOS_PROJECTS_COMMON  = $$COSMOS_PROJECTS/common
PROJECT                 = $$PWD

# User config
# decide wheter or not to build certain parts of COSMOS,
# you must know what you are doing, by default just let everything "ON"
PROJECT_BUILD_COSMOS_CORE_LIB_SUPPORT           = "ON"
PROJECT_BUILD_COSMOS_CORE_LIB_DEVICE            = "OFF"
PROJECT_BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB   = "ON"

#
include( $$COSMOS/core/qt/cosmos.pri )
include( $$PROJECT/qt/project.pri )

# main program
SOURCES         += $$PROJECT/propagator_simple.cpp

#HEADERS         += $$PROJECT/libraries/controllib.h
#SOURCES         += $$PROJECT/libraries/controllib.cpp


message($$PROJECT_COMMON)
TARGET = propagator_simple
#TARGET_EXT = .exe

#target.path = .
#INSTALLS += target
