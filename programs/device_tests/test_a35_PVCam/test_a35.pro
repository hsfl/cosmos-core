#-------------------------------------------------
#
# Project created by QtCreator 2013-01-19T15:43:13
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = test_a35
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    a35acq.cpp \
    main.cpp

HEADERS += \
    a35acq.h

INCLUDEPATH += /home/mark/include

QMAKE_RPATHDIR += /home/mark/lib:/home/mark/lib/genicam/bin/Linux64_x64

LIBS += -L/home/mark/lib	\
                        -lPvBase             	\
                        -lPvDevice          	\
                        -lPvBuffer          	\
                        -lPvGUIUtils         	\
                        -lPvGUI              	\
                        -lPvPersistence      	\
                        -lPvGenICam          	\
                        -lPvStreamRaw        	\
                        -lPvStream           	\
                        -lPvTransmitterRaw   	\
                        -lPvVirtualDevice
