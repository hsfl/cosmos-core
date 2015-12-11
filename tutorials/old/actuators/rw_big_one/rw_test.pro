# TEMPLATE FOR QT CONSOLE PROJECTS

# DEFINE COSMOS SOFTWARE HOME FOLDER
COSMOSHOME = ../../..			# change this line !!!
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

SOURCES += rw_test.cpp    \		# change this line !!!
    $$DEVICE/*.cpp	    \
    $$SUPPORT/*.cpp	    \

HEADERS +=		    \		# change this line !!!
    $$DEVICE/*.h	    \
    $$SUPPORT/*.h	    \
