TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

TARGET = CosmosSupport

HEADERS = *.h
SOURCES = *.cpp

INCLUDEPATH += ../
INCLUDEPATH += ../thirdparty/ # for zlib


DESTDIR = ../

win32 {
    #message( "Building on Win32" )

    # add libraries for MinGW
    *-g++* {
        #message("Compiler: MinGW")
        LIBS += -lpthread -lwsock32 -lwinmm -lws2_32 -liphlpapi
        QMAKE_CXXFLAGS += -Wall -pedantic -std=c++11 -pthread
    }

    *-msvc* {
        #message("Compiler: MSVC")
        LIBS += -lwsock32 -lwinmm -lws2_32 -liphlpapi

        QMAKE_CXXFLAGS += -W4 -D_CRT_NONSTDC_NO_DEPRECATE

        # include dirent for MSVC
    }

}

macx {
    #message( "Building on MAC OS X" )
    QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
}

unix {
    #message( "Building on Linux" )
    QMAKE_CXXFLAGS += -std=c++11  # -stdlib=libc++
}
