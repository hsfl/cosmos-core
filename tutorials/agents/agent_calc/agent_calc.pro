# DEFINE THE COSMOS SOFTWARE HOME FOLDER
COSMOS                  = $$PWD/../../..
PROJECT                 = $$PWD

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

include( $$COSMOS/core/qt/cosmos.pri )

#HEADERS +=
SOURCES += $$COSMOS_CORE/programs/agents/agent_calc.cpp
