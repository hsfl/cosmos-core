TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += alias.cpp

# -------------------------------------
# User defined folders
COSMOS_SOURCE   = $$PWD/../../../../

MODULES += agentlib jsonlib

include( $$COSMOS_SOURCE/core/cosmos-core.pri )


