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

SOURCES += agent_002.cpp

TARGET = agent_002
target.path = /home/cosmos/test
INSTALLS += target
