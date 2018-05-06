# COSMOS Agent
# Tested on windows with MinGW and MSVC

# define the COSMOS source folder
COSMOS_SOURCE_CORE = $$PWD/../../..

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

include( $$COSMOS_SOURCE_CORE/common.pri )

#MODULES += agentlib
#include( $$COSMOS_SOURCE_CORE/cosmos-core.pri )

SOURCES += agent_001.cpp

#TARGET = agent_001

## if you want to install this file in the default COSMOS folder
# uncomment the following lines
#release: target.files = $$OUT_PWD/release/agent_001.exe
#debug: target.files = $$OUT_PWD/debug/agent_001.exe

#win32:target.path = "C:/COSMOS/core/bin"
#linux:target.path = /home/cosmos/bin

#INSTALLS    += target
