#-------------------------------------------------
#
# Project created by QtCreator 2013-01-24T18:16:42
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = stt_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    stt_test.cpp \
    ../../support/memlib.cpp \
    ../../support/mathlib.cpp \
    ../sinclair_lib.cpp \
    ../cssl_lib.cpp

HEADERS += \
    ../../support/mathlib.h \
    ../../support/memlib.h \
    ../sinclair_lib.h \
    ../cssl_lib.h

INCLUDEPATH += ../../support \
    ../
