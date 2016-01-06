# COSMOS Agent
# Tested on windows with MinGW and MSVC

# define the COSMOS source folder
COSMOS_SOURCE_CORE = $$PWD/../../..

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

#MODULES += agentlib
#include( $$COSMOS_SOURCE_CORE/cosmos-core.pri )

INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/support
INCLUDEPATH += $$COSMOS_SOURCE_CORE/libraries/thirdparty

SOURCES += agent_002.cpp

LIBRARIESFOLDER = $$OUT_PWD/../../../libraries
LIBS += -L$$LIBRARIESFOLDER -lCosmosSupport
LIBS += -L$$LIBRARIESFOLDER -lCosmosMath
LIBS += -L$$LIBRARIESFOLDER -lCosmosDeviceCpu
LIBS += -L$$LIBRARIESFOLDER -lzlib

#TARGET = agent_002
#target.path = /home/cosmos/test
#INSTALLS += target
