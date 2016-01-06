TEMPLATE = lib
CONFIG += staticlib

TARGET = CosmosSupport

HEADERS = *.h
SOURCES = *.cpp

INCLUDEPATH += ../
INCLUDEPATH += ../thirdparty/ # for zlib

macx {
    message( "Building on MAC OS X" )
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
}

DESTDIR = ../
