TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11 # QT5 way of adding c++11 support

# define the COSMOS source folder
COSMOS_SOURCE_CORE = $$PWD/../../..

include( $$COSMOS_SOURCE_CORE/cosmos-core.pri )

SOURCES         += propagator_simple.cpp
