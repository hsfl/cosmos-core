TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

HEADERS = *.h
SOURCES = *.cpp

TARGET = CosmosMath

INCLUDEPATH += ../support

macx {
    #message( "Building on MAC OS X" )
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
}

DESTDIR = ../
