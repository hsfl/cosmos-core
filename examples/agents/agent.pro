# This will compile the agent program, the essence of COSMOS
# to compile faster add -j8 on Projetcs->Build Steps-> Make arguments (this uses multiple cores if you have them)

# DEFINE THE COSMOS SOFTWARE HOME FOLDER
COSMOS                  = $$PWD/../../..
PROJECT                 = $$PWD

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

QMAKE_CXXFLAGS += -std=c++0x -pthread

BUILD_COSMOS_CORE_LIB_SUPPORT = "ON"
BUILD_COSMOS_CORE_LIB_DEVICE  = "OFF"
BUILD_COSMOS_CORE_LIB_THIRDPARTY_ZLIB = "ON"

include( $$COSMOS/core/qt/cosmos.pri )

#HEADERS +=
SOURCES += $$COSMOS_CORE/programs/agents/agent.cpp

TARGET = agent
#TARGET_EXT = .exe

target1.file = $$OUT_PWD/agent.exe
target1.path = C:/acstb/cosmos/bin
INSTALLS    += target1
