TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

# -------------------------------------
# User defined folders
COSMOS_SOURCE   = $$PWD/../../../

MODULES += mathlib
include( $$COSMOS_SOURCE/core/cosmos-core.pri )
include( $$COSMOS_SOURCE/thirdparty/cosmos-thirdparty.pri )

