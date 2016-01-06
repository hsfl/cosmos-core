TEMPLATE = lib
CONFIG += staticlib
HEADERS = *.h
SOURCES = *.cpp

TARGET = CosmosDeviceCpu

INCLUDEPATH += ../../support

macx {
    message( "Building on MAC OS X" )
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
}


DESTDIR = ../../
