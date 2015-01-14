# DEFINE THE COSMOS SOFTWARE HOME FOLDER
COSMOS                  = $$PWD/../../..
PROJECT                 = $$PWD

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

#QMAKE_CXXFLAGS += -pthread

MODULES = ""
MODULES += SUPPORT
include( $$COSMOS/core/qt/cosmos.pri )

#MODULES += POCO
#include( $$COSMOS/thirdparty/thirdparty.pri )

SOURCES += agent_001.cpp

