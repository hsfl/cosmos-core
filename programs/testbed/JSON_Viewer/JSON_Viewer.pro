#-------------------------------------------------
#
# Project created by QtCreator 2012-03-15T16:26:34
#
#-------------------------------------------------

QT       += core gui

TARGET = JSON_Viewer
TEMPLATE = app

CONFIG += uitools

SOURCES += main.cpp\
        mainwindow.cpp \
    dataview.cpp \
    cosmosuiwriter.cpp

HEADERS  += mainwindow.h \
    dataview.h \
    cosmosuiwriter.h \
    ../../support/orbitlib.h \
    ../../support/jsonlib.h \
    ../../support/satlib.h

FORMS    += mainwindow.ui \
    testform.ui
