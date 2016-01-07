# COSMOS agent receive example

# define the COSMOS core folder
COSMOS_SOURCE_CORE = $$PWD/../../..

TEMPLATE = app
CONFIG += console c++11
CONFIG -= qt app_bundle


include( $$COSMOS_SOURCE_CORE/common.pri )

SOURCES += agent_receive.cpp
