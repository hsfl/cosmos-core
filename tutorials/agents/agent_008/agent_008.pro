# COSMOS Agent
# Tested on windows with MinGW and MSVC

# define the COSMOS source folder
COSMOS_SOURCE_CORE = $$PWD/../../..

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11


include( $$COSMOS_SOURCE_CORE/common.pri )


SOURCES += agent_008.cpp


#TARGET = agent_008
#target.path = /home/cosmos/test
#INSTALLS += target
