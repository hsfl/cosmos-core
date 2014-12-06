#-------------------------------------------------
#
# Project created by QtCreator 2013-08-11T15:58:32
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = a35control
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    a35control.cpp \
    acq_a35.cpp \
    ../gige_lib.cpp \
    ../../support/mathlib.cpp \
    ../../support/timelib.cpp \
    ../../support/agentlib.cpp \
    ../../support/memlib.cpp \
    ../../support/datalib.cpp \
    ../../support/convertlib.cpp \
    ../../support/jsonlib.cpp \
    ../../support/sliplib.cpp \
    ../../support/nodelib.cpp \
    ../../support/jpleph.cpp \
    ../../support/demlib.cpp \
    ../../support/geomag.cpp \
    ../../support/stringlib.cpp

HEADERS += \
    a35control.h \
    acq_a35.h \
    ../gige_lib.h

INCLUDEPATH += ../../support
