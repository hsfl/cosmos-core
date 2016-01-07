TEMPLATE = lib
CONFIG += staticlib

DESTDIR = ../../

# include dirent for MSVC
*-msvc* {
    INCLUDEPATH  += .
    HEADERS = *.h
    SOURCES = *.c
}
