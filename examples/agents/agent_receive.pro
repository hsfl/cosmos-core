# COSMOS agent receive example

# define the COSMOS source folder
COSMOS_SOURCE = $$PWD/../../..

TEMPLATE = app
CONFIG += console c++11
CONFIG -= qt app_bundle

MODULES += agentlib
include( $$COSMOS_SOURCE/core/cosmos-core.pri )

SOURCES += agent_receive.cpp
