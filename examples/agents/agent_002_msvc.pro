# Build agent for MSVC

# DEFINE THE COSMOS SOFTWARE HOME FOLDER
COSMOS                  = $$PWD/../../..
PROJECT                 = $$PWD

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

#QMAKE_CXXFLAGS += -pthread

#include( $$COSMOS/core/qt/cosmos_msvc.pri )

SOURCES += agent_002_msvc.cpp

#ElapsedTime
INCLUDEPATH     += $$COSMOS/core/libraries/support
SOURCES         += $$COSMOS/core/libraries/support/elapsedtime.cpp
HEADERS         += $$COSMOS/core/libraries/support/elapsedtime.hpp

SOURCES         += $$COSMOS/core/libraries/support/timeutils.cpp
HEADERS         += $$COSMOS/core/libraries/support/timeutils.h

#agentlib
INCLUDEPATH     += $$COSMOS/core/libraries/support

SOURCES         += $$COSMOS/core/libraries/support/agentlib.cpp
HEADERS         += $$COSMOS/core/libraries/support/agentlib.h

SOURCES         += $$COSMOS/core/libraries/support/jsonlib.cpp
HEADERS         += $$COSMOS/core/libraries/support/jsonlib.h

SOURCES         += $$COSMOS/core/libraries/support/mathlib.cpp
HEADERS         += $$COSMOS/core/libraries/support/mathlib.h

SOURCES         += $$COSMOS/core/libraries/support/timelib.cpp
HEADERS         += $$COSMOS/core/libraries/support/timelib.h

SOURCES         += $$COSMOS/core/libraries/support/datalib.cpp
HEADERS         += $$COSMOS/core/libraries/support/datalib.h

SOURCES         += $$COSMOS/core/libraries/support/convertlib.cpp
HEADERS         += $$COSMOS/core/libraries/support/convertlib.h

SOURCES         += $$COSMOS/core/libraries/support/convertdef.cpp
HEADERS         += $$COSMOS/core/libraries/support/convertdef.h

SOURCES         += $$COSMOS/core/libraries/support/stringlib.cpp
HEADERS         += $$COSMOS/core/libraries/support/stringlib.h

SOURCES         += $$COSMOS/core/libraries/support/jpleph.cpp
HEADERS         += $$COSMOS/core/libraries/support/jpleph.h

SOURCES         += $$COSMOS/core/libraries/support/geomag.cpp
HEADERS         += $$COSMOS/core/libraries/support/geomag.h

SOURCES         += $$COSMOS/core/libraries/support/sliplib.cpp
HEADERS         += $$COSMOS/core/libraries/support/sliplib.h

INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty
SOURCES         += $$files($$COSMOS/core/libraries/thirdparty/zlib/*.c)
HEADERS         += $$files($$COSMOS/core/libraries/thirdparty/zlib/*.h)

INCLUDEPATH     += $$COSMOS/core/libraries/thirdparty/dirent
SOURCES         += $$files($$COSMOS/core/libraries/thirdparty/dirent/*.c)
HEADERS         += $$files($$COSMOS/core/libraries/thirdparty/dirent/*.h)


#LIBS += -lpthread -lwsock32 -lwinmm -lws2_32 -liphlpapi
LIBS += -lws2_32 -liphlpapi
