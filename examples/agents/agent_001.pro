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

SOURCES += agent_001.cpp

