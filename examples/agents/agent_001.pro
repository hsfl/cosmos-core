# DEFINE THE COSMOS SOFTWARE HOME FOLDER
COSMOS                  = $$PWD/../../..
PROJECT                 = $$PWD

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++0x -pthread

include( $$COSMOS/core/qt/cosmos.pri )

#HEADERS +=
SOURCES += $$COSMOS_CORE/programs/testbed/request_examples/agent_001.cpp
#SOURCES += $$COSMOS_CORE/programs/agents/agent_soh.cpp
