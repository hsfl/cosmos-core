TEMPLATE = subdirs
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle
CONFIG += c++11

SUBDIRS += thirdparty/zlib
SUBDIRS += math
SUBDIRS += support
SUBDIRS += device/cpu


