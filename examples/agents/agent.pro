# This will compile the agent program, the essence of COSMOS
# to compile faster add -j on Projetcs->Build Steps-> Make arguments
# (this uses multiple cores if you have them)

# DEFINE THE COSMOS SOFTWARE HOME FOLDER
COSMOS                  = $$PWD/../../..

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

QMAKE_CXXFLAGS += -pthread


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

SOURCES += $$COSMOS/core/programs/agents/agent.cpp

TARGET = agent
#TARGET_EXT = .exe

## if you want to install this file in the default COSMOS folder
# uncomment the following lines
release: target.files = $$OUT_PWD/release/agent.exe
debug: target.files = $$OUT_PWD/debug/agent.exe

win32:target.path = "C:/COSMOS/core/bin"
linux:target.path = /home/cosmos/bin

INSTALLS += target
