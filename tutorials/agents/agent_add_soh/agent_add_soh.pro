# COSMOS Agent
# Tested on windows with MinGW and MSVC

# DEFINE THE COSMOS SOFTWARE HOME FOLDER
COSMOS_SOURCE = $$PWD/../../../..

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
MODULES += math-mathlib
include( $$COSMOS_SOURCE/core/cosmos-core.pri )

SOURCES += agent_add_soh.cpp
