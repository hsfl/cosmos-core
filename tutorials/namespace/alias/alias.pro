TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += alias.cpp

# -------------------------------------
# User defined folders
COSMOS_SOURCE_CORE = $$PWD/../../../

 jsonlib

include( $$COSMOS_SOURCE_CORE/common.pri )


