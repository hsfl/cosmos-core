# COSMOS Agent
# Tested on windows with MinGW and MSVC

# DEFINE THE COSMOS_SOURCE
COSMOS_SOURCE_CORE = $$PWD/../../../

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

#message("----------------------------------------")


include( $$COSMOS_SOURCE_CORE/common.pri )

SOURCES += agent_generic_device_neighbour.cpp

