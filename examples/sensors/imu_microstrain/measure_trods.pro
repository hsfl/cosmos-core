# TEMPLATE FOR QT CONSOLE PROJECTS

# DEFINE COSMOS SOFTWARE HOME FOLDER
COSMOSHOME = ../../../
SUPPORT = $$COSMOSHOME/support
DEVICE = $$COSMOSHOME/device

TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle

OBJECTS_DIR = $$OUT_PWD/obj

# use this to change the qmake-genarated makefile name
# this will avoid overwriting the default makefile
QMAKE_MAKEFILE = makefileqt
MAKEFILE = makefileqt

# remove the objects directory, the makefileqt and the executable
QMAKE_CLEAN += -r $$OBJECTS_DIR makefileqt $$TARGET

INCLUDEPATH += $$SUPPORT
INCLUDEPATH += $$DEVICE

SOURCES += measure_trods.cpp \
    $$DEVICE/arduino_lib.cpp \
    $$DEVICE/microstrain_lib.cpp \
    $$DEVICE/cssl_lib.cpp \
    $$SUPPORT/mathlib.cpp \
    $$SUPPORT/memlib.cpp \

HEADERS += \
    $$DEVICE/arduino_lib.h \
    $$DEVICE/microstrain_lib.h \
    $$SUPPORT/configCosmos.h \
    $$SUPPORT/cosmos-errno.h \
    $$SUPPORT/mathlib.h \
    $$DEVICE/cssl_lib.h \
    $$SUPPORT/memlib.h
