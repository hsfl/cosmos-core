# COSMOS Agent
# Tested on windows with MinGW and MSVC

# DEFINE THE COSMOS SOFTWARE HOME FOLDER
COSMOS_SOURCE_CORE = $$PWD/../../..

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11


include( $$COSMOS_SOURCE_CORE/common.pri )

SOURCES += agent_add_soh.cpp
