# COSMOS agent post example
# Tested on windows with MinGW and MSVC

# define the COSMOS source folder
COSMOS_SOURCE = $$PWD/../../..

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

MODULES += agentlib
#MODULES += elapsedtime
#MODULES += timeutils
#MODULES += socketlib
#MODULES += jsonlib
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
#MODULES += math-mathlib
include( $$COSMOS_SOURCE/core/cosmos-core.pri )

SOURCES += agent_post.cpp

TARGET = agent_post
