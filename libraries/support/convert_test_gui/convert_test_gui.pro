#-------------------------------------------------
#
# Project created by QtCreator 2012-01-24T17:40:40
#
#-------------------------------------------------

QT       += core gui

INCLUDEPATH = ..

TARGET = convert_test_gui
TEMPLATE = app

SUPPORT = ..
################################################################################
# COSMOS Framework
################################################################################
COSMOS_SOURCES += \
    $$SUPPORT/timelib.cpp \
    $$SUPPORT/stringlib.cpp \
    $$SUPPORT/sliplib.cpp \
    $$SUPPORT/orbitlib.cpp \
    $$SUPPORT/nrlmsise-00_data.cpp \
    $$SUPPORT/nrlmsise-00.cpp \
    $$SUPPORT/mathlib.cpp \
    $$SUPPORT/jsonlib.cpp \
    $$SUPPORT/jpleph.cpp \
    $$SUPPORT/geomag.cpp \
    $$SUPPORT/convertlib.cpp \
    #$$SUPPORT/agentlib.cpp \
    $$SUPPORT/memlib.cpp

COSMOS_HEADERS += \
    $$SUPPORT/timelib.h \
    $$SUPPORT/stringlib.h \
    $$SUPPORT/sliplib.h \
    $$SUPPORT/orbitlib.h \
    $$SUPPORT/nrlmsise-00.h \
    $$SUPPORT/mathlib.h \
    $$SUPPORT/jsonlib.h \
    $$SUPPORT/jpleph.h \
    $$SUPPORT/geomag.h \
    $$SUPPORT/cosmos-errno.h \
    $$SUPPORT/cosmos-defs.h \
    $$SUPPORT/convertlib.h \
    #$$SUPPORT/agentlib.h \
    #$$SUPPORT/agentdef.h \
    $$SUPPORT/memlib.h

SOURCES += main.cpp \
    mainwindow.cpp \
    filepathdialog.cpp \
    testcontainer.cpp \
    rearth_test.cpp \
    $$COSMOS_SOURCES \
    eci2kep_test.cpp

HEADERS  += \
    mainwindow.h \
    filepathdialog.h \
    testcontainer.h \
    rearth_test.h \
    $$COSMOS_HEADERS \
    eci2kep_test.h

FORMS    += mainwindow.ui \
    filepathdialog.ui \
    testcontainer.ui \
    rearth_test.ui \
    eci2kep_test.ui
