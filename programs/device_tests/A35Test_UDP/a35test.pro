QT       += core
QT       -= gui

TARGET = a35test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    a35test.cpp



################################################################################
# Libraries and thirdparty code
################################################################################
WIN_LIBRARIES_DIR = ../../../MOST/lib/win32-x86


################################################################################
# Windows config
################################################################################
win32 {
    message( "Building on Win32" )

    # add libraries
    LIBS += $$WIN_LIBRARIES_DIR/pthreadVC2.lib
    LIBS += $$WIN_LIBRARIES_DIR/libwsock32.a  #c:/QtSDK/mingw/lib/libwsock32.a
    LIBS += $$WIN_LIBRARIES_DIR/libws2_32.a
}

