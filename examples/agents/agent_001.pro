# COSMOS Agent
# Tested on windows with MinGW and MSVC

# DEFINE THE COSMOS SOFTWARE HOME FOLDER
COSMOS                  = $$PWD/../../..

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

MODULES += elapsedtime
MODULES += timeutils
MODULES += socketlib
MODULES += agentlib
MODULES += jsonlib
MODULES += mathlib
MODULES += timelib
MODULES += datalib
MODULES += convertlib
MODULES += convertdef
MODULES += stringlib
MODULES += jpleph
MODULES += ephemlib
MODULES += geomag
MODULES += sliplib
MODULES += zlib
include( $$COSMOS/core/qt/cosmos.pri )

SOURCES += agent_001.cpp

TARGET = agent_001

## if you want to install this file in the default COSMOS folder
# uncomment the following lines
#release: target.files = $$OUT_PWD/release/agent_001.exe
#debug: target.files = $$OUT_PWD/debug/agent_001.exe

#win32:target.path = "C:/COSMOS/core/bin"
linux:target.path = /home/cosmos/bin

INSTALLS    += target

