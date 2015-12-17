TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle

SUPPORT = ../../support
DEVICE = ../../device

INCLUDEPATH += $$SUPPORT
INCLUDEPATH += $$DEVICE

#LIBS += $$SUPPORT/dll/pthreadVC2.lib
#LIBS += $$SUPPORT/dll/libwsock32.a
#LIBS += $$SUPPORT/dll/libwinmm.a
#LIBS += $$SUPPORT/dll/libws2_32.a

SOURCES += razor_imu.cpp \
    $$files($$SUPPORT/*.cpp) \
    $$files($$DEVICE/*.cpp) \
    ../rw_lib.cpp

HEADERS += \
    $$files($$SUPPORT/*.h) \
    $$files($$DEVICE/*.h) \
    ../rw_lib.h


