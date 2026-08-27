QT       += core gui widgets
CONFIG   += c++17
TEMPLATE  = app
TARGET    = node-editor

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    nodedata.cpp

HEADERS += \
    mainwindow.h \
    nodedata.h

# Default install alongside other COSMOS tools
target.path = $$PREFIX/bin
INSTALLS   += target
