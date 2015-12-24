TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += playground.cpp

# -------------------------------------
# User defined folders
COSMOS_SOURCE   = $$PWD/../../../

MODULES += mathlib stringlib
include( $$COSMOS_SOURCE/core/cosmos-core.pri )
include( $$COSMOS_SOURCE/thirdparty/cosmos-thirdparty.pri )


# -------------------------------------
# Deployment to remote device

TARGET = playground
target.path = /root/
INSTALLS += target
