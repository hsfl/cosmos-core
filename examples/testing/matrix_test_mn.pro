TEMPLATE = app
CONFIG += console
CONFIG -= qt
CONFIG -= app_bundle

# use this to change the qmake-genarated makefile name
# this will avoid overwriting the default makefile
QMAKE_MAKEFILE = makefileqt
MAKEFILE = makefileqt

#QMAKE_EXTRA_TARGETS += other
#PRE_TARGETDEPS += other
#other.commands = make -f makefileqt

SUPPORT = ../../support
#DEVICE = ../../device

INCLUDEPATH += $$SUPPORT
#INCLUDEPATH += $$DEVICE

SOURCES += \
    matrix_test_mn.cpp \
    $$SUPPORT/*.cpp

HEADERS += \
    $$SUPPORT/*.h

