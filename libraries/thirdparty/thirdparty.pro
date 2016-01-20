TEMPLATE = subdirs
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

SUBDIRS += zlib

# include dirent for MSVC
*-msvc* {
    SUBDIRS += dirent
}

